
/*
 *  Copyright (c) 2007    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#ifndef __INCLUDED_SCOPIRA_UIKIT_AGENTSMONITOR_H__
#define __INCLUDED_SCOPIRA_UIKIT_AGENTSMONITOR_H__

#include <scopira/coreui/viewwidget.h>
#include <scopira/agent/context.h>
#include <scopira/agent/uptimeservice.h>

namespace scopira
{
  namespace uikit
  {
    class agents_monitor_v;
  }
}

class scopira::uikit::agents_monitor_v : public scopira::coreui::viewwidget
{
  public:
    /// ctor
    agents_monitor_v(void);

  private:
    void update_stats(void);
    void update_stats_agent(scopira::agent::uptime_service &serv);

  private:
    scopira::agent::task_context dm_ctx;

    enum {
      acol_uuid = 0,
      acol_hostname,
      acol_numcpu,
      acol_tasks,
      acol_n,
    };

    GtkListStore *dm_astore, dm_tstore;
    GtkWidget *dm_atable, dm_ttable;
};

#endif

