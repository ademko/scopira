
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

#include <scopira/lab/sidekickloop.h>

//BBtargets libscopiraui.so

using namespace scopira::tool;
using namespace scopira::lab;

//
//
// sidekick_lab_loop
//
//

sidekick_lab_loop::sidekick_lab_loop(int &argc, char **&argv)
  : lab_loop(argc, argv)
{
  lab_lock L;
  set_quitonlastclose();
}

sidekick_lab_loop::sidekick_lab_loop(bool trydisplay, int &argc, char **&argv)
  : lab_loop(trydisplay, argc, argv)
{
  lab_lock L;
  set_quitonlastclose();
}

sidekick_lab_loop::~sidekick_lab_loop()
{
  // this is a specialized implemention of ~lab_loop that is designed to work
  // in concern with ~lab_loop. tread cautiously
  // insert wait/process loop here

  {
    event_ptr<open_area> L(dm_openwindows);

    L->gtkjumpnow = true;
    L.notify();

    while (!L->runninggtk)
      L.wait();   // wait until the gui is running before preceeding to wait for events
  }

  bool alive = true;
  count_ptr<scopira::core::sidekick_i> nt;

  while (alive) {
    {
      event_ptr<open_area> L(dm_openwindows);

      if (!L->runninggtk)
        alive = false;   // maybe just return? depends on if it makes sense to process sidekicks with no windows open
      else
        L.wait();

      // ~lab_loop will do the if(L->runninggtk) for us
    }

    //maybe we have a sidekick_i to process
    do {
      {
        locker_ptr<sidekick_area> L(dm_sidekickarea);

        if (!L->queue.empty()) {
          // we have a pending sidekick, snatch it
          nt = L->queue.front().get();
          L->queue.pop_front();
        } else
          nt = 0;
      }

      // do we have a sidekick to process?
      if (nt.get()) {
        // process the sidekick, outside of the area locks
        nt->run();
        // final notify, if any
        if (get_reactor(*nt))
          get_reactor(*nt)->react_sidekick_done(nt.get());
      }
    } while (nt.get());
    
    // nuke it
    nt = 0;
  }//while
}

void sidekick_lab_loop::enqueue_sidekick(scopira::core::sidekick_i *t)
{
  assert(lab_lock::is_locked());  // this should be called within the gui thread!
  {
    // ok, enqueue this sidekick and notify the listening worker thread
    locker_ptr<sidekick_area> L(dm_sidekickarea);
    // enqueue the object
    L->queue.push_back(t);
  }

  // notify (could do this outsode the the event_ptr), I guess
  dm_openwindows.pm_condition.notify();
}

