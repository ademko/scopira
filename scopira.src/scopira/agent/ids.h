
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

#ifndef __INCLUDED_SCOPIRA_AGENT_IDS_H__
#define __INCLUDED_SCOPIRA_AGENT_IDS_H__

#include <scopira/tool/uuid.h>
#include <scopira/tool/export.h>

namespace scopira
{
  namespace agent
  {
    /// a where specification to place the task at the master's agent
    SCOPIRA_EXPORT extern scopira::tool::uuid where_master_c;
    /// a where specification to place the task at the same agent as the launcher (like "localhost")
    SCOPIRA_EXPORT extern scopira::tool::uuid where_this_c;

    /// the "service" type for agents themselves
    SCOPIRA_EXPORT extern scopira::tool::uuid agent_service_c;
    /// the "service" type for agents themselves (but only the job accepting agents)
    SCOPIRA_EXPORT extern scopira::tool::uuid worker_agent_service_c;
  }
}

#endif


