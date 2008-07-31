
/*
 *  Copyright (c) 2006    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#ifndef __INCLUDED_SCOPIRA_MATLAB_TASK_H__
#define __INCLUDED_SCOPIRA_MATLAB_TASK_H__

#include <vector>
#include <map>

#include <scopira/tool/uuid.h>
#include <scopira/tool/output.h>
#include <scopira/core/loop.h>
#include <scopira/agent/task.h>
#include <scopira/agent/context.h>

#include <scopira/matlab/loop.h>

namespace scopira
{
  namespace matlab
  {
    extern scopira::tool::uuid job_service;

    class server_link;

    class job_server;
    class query_server;

    class job_client;

    class link_loop;
  }
}

/**
 * The daemon/server tasks that manages "jobs"
 *
 * @author Aleksander Demko
 */
class scopira::matlab::job_server : public scopira::agent::agent_task_i
{
  public:
    virtual int run(scopira::agent::task_context &ctx);

  private:
    class job_team {
      public:
        std::vector<scopira::tool::uuid> pm_peers;
    };

    typedef std::map<std::string, job_team> jobmap_t;

    jobmap_t dm_jobs;
};

/**
 * This task simply finds a job_server, and if found, querys its
 * status and other information.
 *
 * @author Aleksander Demko
 */ 
class scopira::matlab::query_server : public scopira::agent::agent_task_i
{
  public:
    virtual int run(scopira::agent::task_context &ctx);
};

/**
 * A nice object that encasulates the client api for talking to job_servers.
 *
 * You need one of these PER MTJS team.
 *
 * @author Aleksander Demko
 */ 
class scopira::matlab::job_client : public virtual scopira::tool::object
{
  private:
    /// the job server... zero for not connected/couldnt find
    scopira::tool::uuid dm_server;
    /// my team name... empty for not active... you should use one, but its optional
    std::string dm_team;
    /// the whole peer array
    std::vector<scopira::tool::uuid> dm_peers;
    /// my index in the peer array
    int dm_myindex;
    /// did i birth the server (if so, ill have to kill it)
    bool dm_madeserver;

    /// my agents context
    scopira::agent::task_context dm_ctx;

  public:
    /// ctor
    job_client(void);
    /// dtor
    virtual ~job_client(void);

    /**
     * Makes the given team of size team. You will be node 0.
     * This blocks until the team is made and everyone has joined.
     *
     * Returns true on success.
     *
     * @author Aleksander Demko
     */ 
    bool make_team(const std::string &teamname, int teamsz);
    /**
     * This joins the existing (previously make_team'ed) team.
     * This blocks until the team is made and everyone has joined.
     *
     * Returns true on success.
     *
     * @author Aleksander Demko
     */ 
    bool join_team(const std::string &teamname);

    /**
     * Returns your id (index) within the team.
     * It will always be from 0..team_size()-1.
     *
     * @author Aleksander Demko
     */ 
    int team_id(void) { return dm_myindex; }
    /**
     * Return the number of nodes in the team. This is always the same as
     * teamsz, as passed to make_team.
     *
     * @author Aleksander Demko
     */ 
    int team_size(void) { return dm_peers.size(); }

    /**
     * Gets the id of the given peer.
     * @author Aleksander Demko
     */
    const scopira::tool::uuid & team_id(int idx) { return dm_peers[idx]; }

    /**
     * Gets the internal context object
     * @author Aleksander Demko
     */
    scopira::agent::task_context & context(void) { return dm_ctx; }
};

/**
 * This is used by the various matlab MEX functions to share one
 * instance of the client link.
 *
 * @author Aleksander Demko
 */ 
class scopira::matlab::link_loop
{
  private:
    class singleton_job_client : public scopira::matlab::job_client
    {
      private:
        static singleton_job_client *dm_instance;
      public:
        /// ctor
        singleton_job_client(void);
        /// dtor
        virtual ~singleton_job_client();
        /// instance getter
        static singleton_job_client * instance(void) { return dm_instance; }
    };
  private:
    scopira::matlab::shared_loop<scopira::core::basic_loop> dm_basicloop;
    scopira::tool::count_ptr<singleton_job_client> dm_jorb;
  public:
    /// ctor
    link_loop(void) {
      if (singleton_job_client::instance())
        dm_jorb = singleton_job_client::instance();
      else
        dm_jorb = new singleton_job_client;
    }

    static job_client *client_instance(void) { return singleton_job_client::instance(); }
};

#endif

