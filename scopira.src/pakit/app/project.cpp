
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

#include <pakit/app/project.h>

#include <gtk/gtk.h>

#include <scopira/coreui/aboutwindow.h>
#include <scopira/coreui/label.h>
#include <scopira/coreui/viewmenu.h>
#include <scopira/coreui/viewcontainer.h>

#include <pakit/ui/models.h>

//BBlibs scopiraui
//BBtargets scopirapa.exe

using namespace scopira::tool;
using namespace scopira::coreui;
using namespace pakit;

main_window::main_window(patterns_project_m *prj)
  : project_window_base("Scopira/PA", prj?prj:prj = new patterns_project_m)
{
  dm_prj = prj;
  init_gui();
}

project_window_base * main_window::spawn(scopira::core::project_i *prj)
{
  if (!prj)
    return new main_window;

  patterns_project_m *sp = dynamic_cast<patterns_project_m*>(prj);

  if (!sp)
    return 0;   // failed to cast

  return new main_window(sp);
}

static void h_about(GtkWidget *widget, gpointer blah)
{
  aboutwindow *aw = new aboutwindow("About Scopira/PA");

  aw->add_tab(0,
    "\nA General Framework for Pattern Analysis\n\n"
    "Version: " "0.9" " (" __DATE__ " " __TIME__ ")\n"
    , "PAKit");

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

