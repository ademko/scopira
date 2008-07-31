
/*
 *  Copyright (c) 2004    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#include <scopira/coreui/loop.h>

#include <gtk/gtk.h>

#include <scopira/tool/output.h>
#include <scopira/coreui/window.h>

//BBlibs scopira
//BBtargets libscopiraui.so

using namespace scopira::tool;
using namespace scopira::coreui;

void scopira::coreui::process_ui_events(void)
{
  while (gtk_events_pending())
    gtk_main_iteration();
}

//
//
// ui_loop
//
//

ui_loop::ui_loop(int &argc, char **&argv)
  : scopira::core::basic_loop(argc, argv)
{
  dm_quitonlastclose = false;

  set_app(this);

  init_gui(true, argc, argv);
}

ui_loop::ui_loop(bool startgui, int &argc, char **&argv)
  : scopira::core::basic_loop(argc, argv)
{
  dm_quitonlastclose = false;

  set_app(this);

  if (startgui)
    init_gui(true, argc, argv);
}

ui_loop::~ui_loop()
{
  gui_hide_all();

  set_app(0);
}

void ui_loop::run_gui(bool quitonlastclose)
{
  dm_quitonlastclose = quitonlastclose;

  gdk_threads_enter();
  gtk_main();
  gdk_threads_leave();
}

void ui_loop::add_window(window *win)
{
  count_ptr<window> cp(win);

  locker_ptr<window_area> L(dm_windowlist);

  assert(L->windowlist.count(cp) == 0);
  L->windowlist.insert(cp);
}

void ui_loop::remove_window(window *win)
{
  count_ptr<window> cp(win);
  bool emp = false;

  {
    locker_ptr<window_area> L(dm_windowlist);

    assert(L->windowlist.count(cp) == 1);
    L->windowlist.erase(cp);   // just watchin my temporaries here

    emp = L->windowlist.empty();
  }

  if (dm_quitonlastclose && emp)
    gtk_main_quit();
}

void ui_loop::set_status(const std::string &msg, bool is_error)
{
  OUTPUT << "set_status<" << msg << ">\n";
}

bool ui_loop::is_debug_mode(void) const
{
  return false;
}

bool ui_loop::init_gui(bool req, int &argc, char **&argv)
{
  // warm up the gui
  g_thread_init(0);
  // see: http://developer.gnome.org/doc/API/2.0/gdk/gdk-Threads.html
  gdk_threads_init();

  /// gtk init
  if (req) {
    gtk_init(&argc, &argv); // address of local temporaries, yeah...
    return true;
  } else
    return gtk_init_check(&argc, &argv) != 0; // address of local temporaries, yeah...
}

void ui_loop::gui_hide_all(void)
{
  count_ptr<window> win;

  do {
    // get the next window to close
    {
      locker_ptr<window_area> L(dm_windowlist);

      if (L->windowlist.empty())
        win = 0;
      else
        win = *L->windowlist.begin();
    }

    if (win.get())
      win->on_destroy(); // die already
  } while (win.get());
}

