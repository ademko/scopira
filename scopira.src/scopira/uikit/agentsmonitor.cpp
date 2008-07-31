
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

#include <scopira/uikit/agentsmonitor.h>

#include <scopira/tool/output.h>
#include <scopira/core/register.h>
#include <scopira/coreui/label.h>
#include <scopira/coreui/layout.h>

//BBtargets libscopiraui.so
//BBlibs scopira

using namespace scopira::tool;
using namespace scopira::coreui;
using namespace scopira::uikit;
using namespace scopira::agent;

//static scopira::core::register_view<agents_monitor_v> r100("scopira::uikit::agents_monitor_v",
//    "scopira::core::project_i", "(control panel)/Agent Monitor");


agents_monitor_v::agents_monitor_v(void)
{
  count_ptr<tab_layout> tabby;
  GtkCellRenderer *renderer;
  GtkTreeViewColumn *column;

  set_view_title("Agents Monitor");
  tabby = new tab_layout;

  dm_astore = gtk_list_store_new(acol_n, G_TYPE_STRING,
      G_TYPE_STRING, G_TYPE_INT, G_TYPE_INT);
  dm_atable = gtk_tree_view_new_with_model(GTK_TREE_MODEL(dm_astore));
  gtk_widget_show_all(dm_atable);

  renderer = gtk_cell_renderer_text_new();
  column = gtk_tree_view_column_new_with_attributes("ID", renderer, "text", acol_uuid, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(dm_atable), column);

  renderer = gtk_cell_renderer_text_new();
  column = gtk_tree_view_column_new_with_attributes("Hostname", renderer, "text", acol_hostname, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(dm_atable), column);

  renderer = gtk_cell_renderer_text_new();
  column = gtk_tree_view_column_new_with_attributes("nCPU", renderer, "text", acol_numcpu, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(dm_atable), column);

  renderer = gtk_cell_renderer_text_new();
  column = gtk_tree_view_column_new_with_attributes("nTasks", renderer, "text", acol_tasks, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(dm_atable), column);

  tabby->add_widget(new widget(dm_atable), "Agent Nodes");
  tabby->add_widget(new label("tasks here"), "Tasks");

  // add tables
  // begin monitoring via local context object
  
  init_gui(tabby.get(), button_close_c);

  update_stats();
}

void agents_monitor_v::update_stats(void)
{
  uptime_service serv(dm_ctx);

  update_stats_agent(serv);
}

namespace {
  struct agent_entry {
    uuid id;
    std::string hostname;
    int numcpu;
    int tasks;
  };
}

void agents_monitor_v::update_stats_agent(scopira::agent::uptime_service &serv)
{
  typedef std::map<uuid, agent_entry> agents_t;
  agents_t agents;
  agents_t::iterator aa;
  int x, mx;

  // ok, lets parse out the agent info
  mx = serv.get_num_agents();
  for (x=0; x<mx; ++x) {
    agent_entry e;

    // load the agent info
    if (!e.id.parse_string(serv.get_value(x, "agentuuid")))
      continue;
    e.hostname = serv.get_value(x, "hostname");
    e.numcpu = string_to_int(serv.get_value(x, "numcpu"));
    e.tasks = string_to_int(serv.get_value(x, "tasks"));

    // add the entry
    agents[e.id] = e;
  }

  // now, update the gui table with this info
  for (aa=agents.begin(); aa != agents.end(); ++aa) {
    GtkTreeIter ii;
    gtk_list_store_insert_before(dm_astore, &ii, 0);
    gtk_list_store_set(dm_astore, &ii,
      acol_uuid, aa->second.id.as_string().c_str(),
      acol_hostname, aa->second.hostname.c_str(),
      acol_numcpu, aa->second.numcpu,
      acol_tasks, aa->second.tasks,
      -1);
  }
}

