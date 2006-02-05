
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

#include <scopira/lab/loop.h>

#include <vector>

#include <scopira/tool/util.h>
#include <scopira/tool/file.h>
#include <scopira/tool/output.h>
#include <scopira/lab/window.h>

//BBtargets libscopiraui.so

using namespace scopira::tool;
using namespace scopira::lab;

//
//
// lab_lock
//
//

lab_lock::lab_lock(void)
{
  if (!lab_loop::lab_instance())  // so i work in non-lab_loop mode too
    return;

  if ( thread::get_tls(lab_loop::lab_instance()->dm_gtklock_tls) != 0)
    return;   // allow nested locks

  //OUTPUT << "[" << reinterpret_cast<long>(this) << "]" << "lab_lock...\n";

  {
    locker_ptr<lab_loop::attempt_area> L(lab_loop::lab_instance()->dm_attempt);
    L->attempt = true;
  }

  lab_loop::lab_instance()->dm_gtklock.lock();
  thread::set_tls(lab_loop::lab_instance()->dm_gtklock_tls, this);
  gdk_threads_enter();
  //OUTPUT << "[" << reinterpret_cast<long>(this) << "]" << " ...aquired\n";
}

lab_lock::~lab_lock()
{
  if (!lab_loop::lab_instance())  // so i work in non-lab_loop mode too
    return;

  if ( thread::get_tls(lab_loop::lab_instance()->dm_gtklock_tls) != this)
    return;   // allow nested locks

  //OUTPUT << "[" << reinterpret_cast<long>(this) << "]" << " ...released\n";
  gdk_threads_leave();
  thread::set_tls(lab_loop::lab_instance()->dm_gtklock_tls, 0);
  lab_loop::lab_instance()->dm_gtklock.unlock();

  {
    locker_ptr<lab_loop::attempt_area> L(lab_loop::lab_instance()->dm_attempt);
    L->attempt = false;
  }
}

bool lab_lock::is_locked(void)
{
  return thread::get_tls(lab_loop::lab_instance()->dm_gtklock_tls) != 0;
}

//
//
// lab_window_i
//
//

void lab_window_i::reg_window(void)
{
  event_ptr<lab_loop::open_area> L(lab_loop::lab_instance()->dm_openwindows);

  L->windowlist.insert(this);

  // notify any listeners for this (namely, gui thread waiting for first window)
  L.notify();
}

void lab_window_i::unreg_window(void)
{
  event_ptr<lab_loop::open_area> L(lab_loop::lab_instance()->dm_openwindows);

  // might result in a dtor call!
  L->windowlist.erase(this);

  // notify any listeners for this (namely, my dtor)
  L.notify();
}

void lab_window_i::push_action(void)
{
  event_ptr<lab_loop::action_area> L(lab_loop::lab_instance()->dm_action);

  if (!L->active_window)
    L->active_window = this;

  L.notify();
}

void lab_window_i::gui_hide_all(void)
{
  lab_loop::lab_instance()->gui_hide_all();
}

//
//
// lab_loop
//
//

//old gcc3.4isms... keeping it around just for the memories
//template <>
//scopira::lab::lab_loop * scopira::tool::singleton<lab_loop>::dm_lab_instance;
//template class singleton<lab_loop>;
// 3.2isms
//template <class T> lab_loop* singleton<lab_loop>::dm_lab_instance;
//template class singleton<lab_loop>;
lab_loop * lab_loop::dm_lab_instance;

lab_loop::lab_loop(int &argc, char **&argv)
  : ui_loop(false, argc, argv), dm_thread(gui_thread_run, this)
{
  dm_hasdisplay = true;
  init_gui(true, argc, argv);
  ctor();
}

lab_loop::lab_loop(bool trydisplay, int &argc, char **&argv)
  : ui_loop(false, argc, argv), dm_thread(gui_thread_run, this)
{
  dm_hasdisplay = false;
  if (trydisplay && !has_config("noui"))
    dm_hasdisplay = init_gui(false, argc, argv);
  ctor();
}

bool lab_loop::open_display(int &argc, char **&argv)
{
  assert(!dm_hasdisplay && "[open_display: gui already running]\n");

  if (!has_config("noui"))
    dm_hasdisplay = init_gui(false, argc, argv);

  return dm_hasdisplay;
}

lab_loop::~lab_loop()
{
  //OUTPUT << "~lab_loop() -- 1\n";
  // signal the thread to stop
  {
    event_ptr<open_area> L(dm_openwindows);

    L->gtkstopnow = true;

    L.notify();
  }

  //OUTPUT << "~lab_loop() -- 2\n";
  // wait for all the open windows to die
  {
    event_ptr<open_area> L(dm_openwindows);

    while (!L->windowlist.empty())
      L.wait();

    if (L->runninggtk) {
      lab_lock LL;

      gtk_main_quit();

      L->runninggtk = false;
      L.notify();
    }
  }

  //OUTPUT << "~lab_loop() -- 3\n";

  // wait for the GUI thread to shut down
  dm_thread.wait_stop();

  // kill TLS
  thread::close_tls(dm_gtklock_tls);

  //OUTPUT << "~lab_loop() -- 4\n";

#ifndef NDEBUG
  gboolean bb =
#endif
  g_source_remove(dm_timmertag);    // is this correct? does this leak?
  assert(bb);

  //OUTPUT << "~lab_loop() -- 5\n";

  assert(dm_lab_instance == this);
  dm_lab_instance = 0;
}

bool lab_loop::wait_action(int msec)
{
  event_ptr<action_area> L(dm_action);

  if (L->active_window)
    return true;

  L.wait(msec);

  return  L->active_window != 0;
}

lab_window_i * lab_loop::pop_action(void)
{
  locker_ptr<action_area> L(dm_action);
  lab_window_i *ret;

  ret = L->active_window;
  L->active_window = 0;

  return ret;
}

bool lab_loop::is_visible(void) const
{
  // perhaps should use the window list from ui_shell, or not...
  locker_ptr<open_area> L(dm_openwindows);

  return !L->windowlist.empty();
}

void lab_loop::hide_all(void)
{
  event_ptr<open_area> L(dm_openwindows);

  while (!L->windowlist.empty()) {
    {
      lab_lock LL;

      L->windowlist.begin()->get()->gui_hide();
    }
    // itll try to unreg itself, wait for its signal here
    L.wait();
  }
}

bool lab_loop::has_display(void)
{
  return lab_instance()->dm_hasdisplay;
}

void lab_loop::ctor(void)
{
  // allocate TLS
  thread::open_tls(dm_gtklock_tls);

  assert(dm_lab_instance == 0);
  dm_lab_instance = this;

  // init shared areas
  dm_openwindows.pm_data.runninggtk = false;
  dm_openwindows.pm_data.gtkstopnow = false;
  dm_openwindows.pm_data.gtkjumpnow = false;
  dm_attempt.pm_data.attempt = false;
  dm_action.pm_data.active_window = 0;

  // set up the ticker just so we dont stall on idle free guis
  {
    lab_lock LL;
    dm_timmertag = g_timeout_add(100, h_timer_tick, this);
  }

  // spawn the gtk GUI handling thread
  dm_thread.start();
}

// see here for gtk thread stuff
// http://developer.gnome.org/doc/API/2.0/gdk/gdk-Threads.html
// http://developer.gnome.org/doc/API/2.0/gtk/gtk-General.html#gtk-main
void* lab_loop::gui_thread_run(void *v)
{
  lab_loop *here = reinterpret_cast<lab_loop*>(v);

  // wait till atleast one window is open
  {
    event_ptr<open_area> L(here->dm_openwindows);

    while (L->windowlist.empty() && !L->gtkjumpnow) {
      // check if we were awoken by the main threads dtor
      // if so, just exist now
      if (L->gtkstopnow)
        return 0;

      L.wait();

      // check if we were awoken by the main threads dtor
      // if so, just exist now
      if (L->gtkstopnow)
        return 0;
    }

    L->runninggtk = true;

    L.notify();
  }

  // Don't use lab_lock here... lab_lock is for general users in the worker (main) threader
  lab_loop::lab_instance()->dm_gtklock.lock();
  thread::set_tls(here->dm_gtklock_tls, here);
  gdk_threads_enter();
  gtk_main();
  gdk_threads_leave();
  thread::set_tls(here->dm_gtklock_tls, 0);
  lab_loop::lab_instance()->dm_gtklock.unlock();

  {
    event_ptr<open_area> L(here->dm_openwindows);

    L->runninggtk = false;
    L.notify();
  }

  return 0;
}

gint lab_loop::h_timer_tick(gpointer data)
{
  lab_loop *here = reinterpret_cast<lab_loop*>(data);

  thread::set_tls(here->dm_gtklock_tls, 0);
  lab_loop::lab_instance()->dm_gtklock.unlock();

  assert(here);

  // this is executed outside of the gtklock

  // lets check of someone's lab_lock is trying for the lock. if so, lets sleep
  // a good half second
  // because, when we return, the mainline is going to grab the gtk lock back
  //OUTPUT << "TICK\n";
  if (locker_ptr<attempt_area>(here->dm_attempt)->attempt)
    thread::sleep(500);

  lab_loop::lab_instance()->dm_gtklock.lock();
  thread::set_tls(here->dm_gtklock_tls, here);

  return 1; // non-zero == keep this timer goin
}

void lab_loop::gui_hide_all(void)
{
  std::vector<count_ptr<lab_window_i> > hidelist;

  hidelist.reserve(16);

  {
    event_ptr<lab_loop::open_area> L(dm_openwindows);

    for (lab_loop::open_area::windowlist_t::iterator ii=L->windowlist.begin(); ii!=L->windowlist.end(); ++ii)
      hidelist.push_back(ii->get());
  }

  for (std::vector<count_ptr<lab_window_i> >::iterator ii=hidelist.begin(); ii!=hidelist.end(); ++ii)
    ii->get()->gui_hide();

  hidelist.clear();

  // we call this after our more highly evolved shell_windows have been cleaned up
  ui_loop::gui_hide_all();
}

