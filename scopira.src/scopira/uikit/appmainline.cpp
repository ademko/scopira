
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

//BBlibs scopiraui
//BBtargets scopira.exe

#include <scopira/tool/platform.h>
#include <scopira/core/sidekickthread.h>
#include <scopira/coreui/loop.h>
#include <scopira/coreui/project.h>
#include <scopira/coreui/aboutwindow.h>
#include <scopira/coreui/viewcontainer.h>
#include <scopira/agent/loop.h>

using namespace scopira::tool;
using namespace scopira::coreui;

class main_window : public scopira::coreui::project_window_base
{
  public:
    /// ctor
    main_window(scopira::core::project_base *prj = 0);

    virtual project_window_base * spawn(scopira::core::project_i *prj);

  private:
    scopira::tool::count_ptr<scopira::core::project_base> dm_prj;

  private:
    void init_gui(void);
};

main_window::main_window(scopira::core::project_base *prj)
  : project_window_base("Scopira", prj?prj:prj = new scopira::core::project_base)
{
  dm_prj = prj;
  init_gui();
}

project_window_base * main_window::spawn(scopira::core::project_i *prj)
{
  if (!prj)
    return new main_window;

  scopira::core::project_base *sp = dynamic_cast<scopira::core::project_base*>(prj);

  if (!sp)
    return 0;   // failed to cast

  return new main_window(sp);
}

static void h_about(GtkWidget *widget, gpointer blah)
{
  aboutwindow *aw = new aboutwindow("About Scopira");

  aw->add_tab_scopira();

  aw->show_all();
}

void main_window::init_gui(void)
{
  // make the menu
  menu_bar_builder mm;

  // build the menu
  init_stock_menu(mm);

  mm.push_menu("_Insert");

  {
    view_filter_options filter;
    noparen_filter np;

    //filter.pm_max_depth = 0;
    filter.pm_filter = &np;
    build_view_menu(dm_prj.get(), mm, filter, 0);
  }
  mm.pop_menu();

  mm.push_menu("_Help");
  mm.add_stock_item("_About...", GTK_STOCK_HELP);
  mm.set_callback(G_CALLBACK(h_about));
  mm.pop_menu();

  // build the main area
  count_ptr<split_layout> hbox = new split_layout(true);
  count_ptr<viewtabs> vt = new viewtabs;
  count_ptr<project_tree> pt = new project_tree(dm_prj.get());
  
  pt->set_view_filter_options(view_filter_options(), vt.get());

  pt->set_min_size(240, 400);

  hbox->add_widget(pt.get(), 0, false, false);
  hbox->add_widget(vt.get(), 1, true, true);

  // finally, send all these objects to the main class
  project_window_base::init_gui(hbox.get(), mm.get_widget());
}

int main(int argc, char **argv)
{
  scopira::coreui::ui_loop looper(argc, argv);
  scopira::core::sidekick_thread thr;  //optional, ofcourse, but oh so powerful
  scopira::agent::agent_loop cl(argc, argv);  //optional, ofcourse, but oh so power II
  
  main_window *mainwin = new main_window;
  
  mainwin->show_all();

  if (looper.has_config("project"))
    mainwin->load_project(looper.get_config("project"));

  // do the gui stuff
  looper.run_gui();

  return 0;
}

