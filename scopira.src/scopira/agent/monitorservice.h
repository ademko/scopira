
/*
 *  Copyright (c) 2006-2007    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#ifndef __INCLUDED_SCOPIRA_AGENT_MONITORSERVICE_H__
#define __INCLUDED_SCOPIRA_AGENT_MONITORSERVICE_H__

#include <scopira/tool/uuid.h>
#include <scopira/agent/context.h>

namespace scopira
{
  namespace agent
  {
    class monitor_service;
  }
}

/**
 * This is a service pack for accessing the monitor service.
 * The monitor service lets you register your process for notification
 * of other tasks deaths, aswell as have your own task be killed when another
 * process dies.
 *
 * You may create this object as needed, it is lightweight.
 *
 * A "service pack" is a C++ API object that easies access to a service task.
 * It handles all the protocol communication with that service.
 *
 * @author Aleksander Demko
 */ 
class scopira::agent::monitor_service
{
  public:
    /// constructor
    monitor_service(scopira::agent::task_context &ctx);

    /**
     * Add a watch to be monitored. If the whotowatch task dies while this watch
     * is active, then receipient will be:
     *   killonwatch = true? receipient will be sent the kill signal/be killed
     *   killonwatch = false? receipient will be send the msg int(1100) uuid(whotowatch)
     *
     * @author Aleksander Demko
     */ 
    void add_watch(scopira::tool::uuid whotowatch, scopira::tool::uuid receipient, bool killonwatch=false);
    void remove_watch(scopira::tool::uuid whotowatch, scopira::tool::uuid receipient);

  private:
    scopira::agent::task_context &dm_ctx;
    scopira::tool::uuid dm_server;
};

#endif

