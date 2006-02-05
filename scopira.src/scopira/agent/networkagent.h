
/*
 *  Copyright (c) 2005    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#ifndef __INCLUDED_SCOPIRA_AGENT_NETWORKAGENT_H__
#define __INCLUDED_SCOPIRA_AGENT_NETWORKAGENT_H__

#include <scopira/tool/netflow.h>
#include <scopira/tool/url.h>
#include <scopira/tool/uuid.h>
#include <scopira/tool/time.h>
#include <scopira/core/register.h>
#include <scopira/agent/localagent.h>

namespace scopira
{
  namespace agent
  {
    class peer_spec;

    class network_agent;
  }
}

/**
 * This builds on machine_spec with peer-specific information.
 * @author Aleksander Demko
 */
class scopira::agent::peer_spec
{
  public:
    enum {
      all_jobs_c,
      my_jobs_c,
      no_jobs_c,
    };

    scopira::tool::uuid pm_uuid;

    scopira::tool::uuid pm_relay_uuid;
    scopira::tool::url pm_direct_url;   // might be the relay's url if relay!=zero
    int pm_pingdelay;
    int pm_hopcount;

  public:
    /// zero initing
    peer_spec(void);

    bool load(scopira::tool::itflow_i& in);
    void save(scopira::tool::otflow_i& out) const;

    void parse_options(const std::string &options);
};

/**
 * The fully distributred, peer to peer agent implementation.
 *
 * @author Aleksander Demko
 */ 
class scopira::agent::network_agent : public scopira::agent::local_agent
{
  private:
    typedef local_agent parent_type;
  public:
    /// ctor
    network_agent(void);
    /// dtor
    virtual ~network_agent();

    virtual void notify_stop(void);

    virtual bool failed(void) const;
    virtual void set_agenterror_reactor(agenterror_reactor_i *r);

  private:
    enum { default_port_c = 7777 };
    enum {
      failed_initting_c,
      failed_cantinit_c,
      failed_opening_c,
      failed_cantopen_c,
      failed_ok_c,
    };

    /**
     * The admin event queue
     *
     * @author Aleksander Demko
     */
    struct admin_area
    {
      public:
        //msgqueue_t pm_main_queue;
        //cronqueue_t pm_cron_queue;
        /// alive? (only possible after a successful boot up)
        bool pm_alive;
        /// the failed state
        char pm_failedstate;
        /// agenterror handler, if any
        agenterror_reactor_i *pm_agenterror;
      public:
        admin_area(void) : pm_alive(false), pm_failedstate(failed_initting_c), pm_agenterror(0) { }
    };

    /// my UUID generator
    scopira::tool::uuid_generator dm_uuidmaker;
    /// the age of this agent (ie. constant running chrono since agent start up)
    scopira::tool::chrono dm_age;

    /// my info
    peer_spec dm_peerspec;

    /// listen port
    int dm_networkport;

    /// network start address
    scopira::tool::url dm_networkstart;
    /// networkname
    std::string dm_networkname;

    /// listen (tcp) socket
    scopira::tool::netflow dm_listensocket;
    /// udp socket
    scopira::tool::udpflow dm_udpsocket;

    /// admin area
    scopira::tool::event_area<admin_area> dm_adminarea;

    /// listen thread
    scopira::tool::thread dm_listenthread;
    /// admin thread (this processes the admin event queue)
    scopira::tool::thread dm_udpthread;
    /// admin thread (this processes the admin event queue)
    scopira::tool::thread dm_adminthread;

  private:
    static void* listen_thread_func(void *herep);
    static void* udp_thread_func(void *herep);
    static void* admin_thread_func(void *herep);
};

#endif

