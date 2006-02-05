
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

#ifndef __INCLUDED_SCOPIRA_CORE_SIDEKICKTHREAD_H__
#define __INCLUDED_SCOPIRA_CORE_SIDEKICKTHREAD_H__

#include <list>

#include <scopira/core/sidekick.h>

namespace scopira
{
  namespace core
  {
    class sidekick_runner;
    class sidekick_thread;
  }
}

/**
 * This is a runner-sigleton that can process events queued
 * by equeue_sidekick, often in lue of sidekick_lab_loop. You can run
 * this with your threads directly, or just use the simple
 * sidekick_thread which is simply a thread+this object.
 *
 * @author Aleksander Demko
 */
class scopira::core::sidekick_runner : public virtual scopira::tool::runnable_i,
    public scopira::core::sidekick_queue_i
{
  public:
    /// ctor
    SCOPIRA_EXPORT sidekick_runner(void);

    /// the run implementation    
    SCOPIRA_EXPORT virtual void run(void);

    /// called by sidekick_thread's dtor
    SCOPIRA_EXPORT virtual void notify_stop(void);
    
  protected:
    SCOPIRA_EXPORT virtual void enqueue_sidekick(sidekick_i *t);
    
  private:
    typedef std::list<scopira::tool::count_ptr<sidekick_i> > sidekick_list;

    // the condition in dm_openwindows is used as the signal for this area
    struct sidekick_area {
      bool alive;
      sidekick_list queue;
    };

    scopira::tool::event_area<sidekick_area> dm_sidekickarea;
};

/**
 * This is simply a thread+sidekick_runner. It does the start()
 * in the ctor and a wait_stop in the dor. You can use this
 * in lue of a sidekick_loop (like, for Windows).
 *
 * @author Aleksander Demko
 */
class scopira::core::sidekick_thread
{
  public:
    /// ctor, starts the sidekick_runner thread in the background
    SCOPIRA_EXPORT sidekick_thread(void);
    /// dtor, blocks the thread until it stops
    SCOPIRA_EXPORT ~sidekick_thread();
    
  private:
    sidekick_runner dm_runner;
    scopira::tool::thread dm_thready;
};

#endif

