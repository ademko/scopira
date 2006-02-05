
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

#include <scopira/coreui/aboutwindow.h>

#include <scopira/tool/platform.h>
#include <scopira/coreui/xpmdata.h>
#include <scopira/coreui/image.h>
#include <scopira/coreui/label.h>

//BBtargets libscopiraui.so

using namespace scopira::tool;
using namespace scopira::coreui;

aboutwindow::aboutwindow(const std::string &windowtitle)
  : dialog(true)
{
  dialog::init_gui();

  set_title(windowtitle);

  init_cancel_button("Close", true);

  dm_tabber = new tab_layout;

  init_window_area(dm_tabber->get_widget());
}

void aboutwindow::add_tab_scopira(void)
{
  add_tab(
    scopira_splash_xpm,
    "\nA C++ Library for Data Analysis, Visualization\nand Parallel Processing\n\n"
    "Version: " SCOPIRA_VERSION " (" __DATE__ " " __TIME__ ")\n"
    "Platform: " PLATFORM_DESC "\n"
#ifdef PLATFORM_64
    "Memory model: 64-bit\n"
#else
    "Memory model: 32-bit\n"
#endif
#ifndef NDEBUG
    "Build: Debug\n"
#else
    "Build: Optimized\n"
#endif
    ,
    "Scopira");
}

void aboutwindow::add_tab(widget *wid, const std::string &label)
{
  assert(wid);
  dm_tabber->add_widget(wid, label);
}

void aboutwindow::add_tab(const char **xpm_data, const std::string &desc, const std::string &tablabel)
{
  if (!xpm_data) {
    dm_tabber->add_widget(new label(desc), tablabel);
    return;
  }

  count_ptr<box_layout> boxer;

  assert(xpm_data);

  boxer = new box_layout(true, false, 10);

  boxer->add_widget(new imagewidget(xpm_data), false, false, 10);
  boxer->add_widget(new label(desc), false, false, 10);
  boxer->set_border_size(10);

  dm_tabber->add_widget(boxer.get(), tablabel);
}

