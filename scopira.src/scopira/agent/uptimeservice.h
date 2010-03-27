
/*
 *  Copyright (c) 2007-2010    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#ifndef __INCLUDED_SCOPIRA_AGENT_UPTIMESERVICE_H__
#define __INCLUDED_SCOPIRA_AGENT_UPTIMESERVICE_H__

#include <map>

#include <scopira/tool/uuid.h>
#include <scopira/agent/context.h>

namespace scopira
{
  namespace agent
  {
    class uptime_service;
  }
}

/**
 * This is a service pack for accessing the uptime service.
 * Each agent has uptime task service, which, when queried can
 * return many statistics and other information about that agent,
 * it's host machine and it's tasks.
 *
 * These uptime server tasks are created one-per-agent when this
 * class is created and destroyed. Therefore, this is not a light operation.
 *
 * A "service pack" is a C++ API object that easies access to a service task.
 * It handles all the protocol communication with that service.
 *
 * @author Aleksander Demko
 */ 
class scopira::agent::uptime_service
{
  public:
    /// constructor
    uptime_service(scopira::agent::task_context &ctx);
    /// destructor
    ~uptime_service();

    /**
     * All the values for get_value are cached internally.
     * Call this will update the cached values from the agents.
     * This is called initially by the uptime_service constructor, too.
     *
     * @author Aleksander Demko
     */ 
    void update_uptime(void);

    /// returns the number of agents being monitored
    int get_num_agents(void) { return static_cast<int>(dm_servers.size()); }

    /**
     * Get the value for the given key from the given agentid.
     * agentid should be [0..get_num_agents).
     *
     * This will return BAD_AGENT or BAD_KEY for various errors.
     *
     * Key, for now, can be:
     *  keylist
     *  hostname
     *  age
     *  agentuuid
     *  load
     *  numcpu
     *  tasks
     *  ostype
     *
     * @author Aleksander Demko
     */ 
    const std::string & get_value(int agentnum, const std::string &key);

  private:
    scopira::agent::task_context &dm_ctx;

    scopira::basekit::narray<scopira::tool::uuid> dm_servers;

    typedef std::map<std::string, std::string> agent_values;
    typedef std::vector<agent_values> agent_list;

    agent_list dm_values;
};

#endif

