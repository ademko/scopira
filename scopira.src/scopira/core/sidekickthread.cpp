
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

#include <scopira/core/sidekickthread.h>

#include <scopira/core/loop.h>

//BBtargets libscopira.so

using namespace scopira::tool;
using namespace scopira::core;

//
// stuff for sidekick.h
//

//
// sidekick_i
//

void sidekick_i::notify_reactors(void)
{
  if (dm_treact)
    dm_treact->react_sidekick_notify(this);
}

//
// sidekick_queue_i
//

sidekick_queue_i * sidekick_queue_i::dm_instance;

sidekick_queue_i::sidekick_queue_i(void)
{
  assert(dm_instance == 0);
  dm_instance = this;
}

sidekick_queue_i::~sidekick_queue_i()
{
  assert(dm_instance == this);
  dm_instance = 0;
}


//
// main dispatch function: enqueue_sidekick
//

void scopira::core::enqueue_sidekick(sidekick_i *t, sidekick_reactor_i *reac)
{
  // bind the reactor to the sidekick (even for null, as that would clear any previous one)
  assert(t);
  t->dm_treact = reac;

  if (sidekick_queue_i::dm_instance) {
    sidekick_queue_i::dm_instance->enqueue_sidekick(t);
    return;
  }

  // nothing? do inline then
  // no sidekick, just process this right now
  count_ptr<sidekick_i> dummy = t;  
  {
    scopira::core::basic_loop::configstack_level L;   // what's this for again?
    t->run();
  }
  if (reac)
    reac->react_sidekick_done(t);
}

//
//
// sidekick_runner
//
//


sidekick_runner::sidekick_runner(void)
{
  dm_sidekickarea.pm_data.alive = true;
}

void sidekick_runner::run(void)
{
  count_ptr<sidekick_i> nt;
  
  while (true) {
    {
      event_ptr<sidekick_area> L(dm_sidekickarea);

      if (!L->alive)
        return; //we're done, exit now
        
      L.wait();
      
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
        
      nt = 0;
    }    
  }//while
}

void sidekick_runner::notify_stop(void)
{
  {
    // ok, enqueue this sidekick and notify the listening worker thread
    locker_ptr<sidekick_area> L(dm_sidekickarea);
    
    L->alive = false;
  }
  
  dm_sidekickarea.pm_condition.notify();
}

void sidekick_runner::enqueue_sidekick(sidekick_i *t)
{
  {
    // ok, enqueue this sidekick and notify the listening worker thread
    locker_ptr<sidekick_area> L(dm_sidekickarea);
    // enqueue the object
    L->queue.push_back(t);
  }
  
  dm_sidekickarea.pm_condition.notify();
}

//
//
// sidekick_thread
//
//

sidekick_thread::sidekick_thread(void)
  : dm_thready(&dm_runner)
{
  dm_thready.start();
}

sidekick_thread::~sidekick_thread()
{
  dm_runner.notify_stop();
  
  dm_thready.wait_stop();
}

