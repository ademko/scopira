
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

#ifndef __INCLUDED_SCOPIRA_AGENT_LOOP_H__
#define __INCLUDED_SCOPIRA_AGENT_LOOP_H__

#include <scopira/tool/export.h>
#include <scopira/tool/object.h>
#include <scopira/agent/agent.h>

namespace scopira
{
  namespace agent
  {
    class agent_loop;
  }
}

/**
 * Constructs the Scopira/agent worker.
 *
 * You can do this AFTER you set up any other loop-types.
 *
 * Config params that this loop accepts:
 *  clusterurl=scopira://host:port/    (the Scopira agent URL)
 *  clustermaster=1    (Make me the master)
 *
 * @author Aleksander Demko
 */ 
class scopira::agent::agent_loop
{
  public:
    /**
     * Starts the agent server in a seperate thread.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT agent_loop(void);
    /**
     * Starts the agent server in a seperate thread.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT agent_loop(int &argc, char **&argv);
    /**
     * Kills/stops the server (see set_wait_for_quit()) for more information).
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT ~agent_loop();

    /**
     * Is the agent in a failed state?
     *
     * @author Aleksander Demko
     */
    bool failed(void) const { return dm_worker->failed(); }

    /**
     * Tells the agent loop to block its destruction until it has received a kill
     * command in the network.
     *
     * By default, the agent_loop is not in server mode.
     *
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT void set_wait_for_quit(void);

  private:
    /// in daemon mode (that is, exit only when given a network kill signal)
    volatile bool dm_daemon_mode;
    /// our link to the static worker
    scopira::tool::count_ptr<scopira::agent::agent_i> dm_worker;
};

#endif

