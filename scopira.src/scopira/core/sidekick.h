
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

#ifndef __INCLUDED_SCOPIRA_CORE_SIDEKICK_H__
#define __INCLUDED_SCOPIRA_CORE_SIDEKICK_H__

#include <scopira/tool/export.h>
#include <scopira/tool/thread.h>
#include <scopira/tool/object.h>

namespace scopira
{
  namespace core
  {
    class sidekick_i;
    class sidekick_reactor_i;
    
    class sidekick_queue_i;

    /**
     * This may be called by any thread to enqueu jobs for the "sidekick thread".
     * If there is no sidekick thread active (either via sidekick_thread or sidekick_loop),
     * then the sidekick will be executed outright in the calling thread.
     *
     * reac is the notify reactor. It may be null.
     *
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT void enqueue_sidekick(sidekick_i *t, sidekick_reactor_i *reac = 0);
  }
}

/**
 * This is a "sidekick".
 *
 * @author Aleksander Demko
 */
class scopira::core::sidekick_i : public virtual scopira::tool::object,
                             public virtual scopira::tool::runnable_i
{
  public:
  protected:
    /// ctor
    sidekick_i(void) : dm_treact(0) { }

    /**
     * Descendants may call this to emit a notify event to any listening reactors
     *
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT void notify_reactors(void);

    /// descendants should provide one of these (inherited from runnable_i)
    //virtual void run(void);

  protected:
    // in the future, make a class sidekick_msg_window that allows
    // a window to be open the whole time during the run.
    // ... allow output too?
    // ... allow cancelability?
    // bah, make them use core?

  private:
    sidekick_reactor_i *dm_treact;

    friend class scopira::core::sidekick_queue_i;
    friend void scopira::core::enqueue_sidekick(sidekick_i *, sidekick_reactor_i *);
};

/**
 * Something that listens to sidekick-related events.
 *
 * @author Aleksander Demko
 */
class scopira::core::sidekick_reactor_i
{
  public:
    virtual ~sidekick_reactor_i() { }
    /**
     * This is called when the sidekick calls the notify function.
     *
     * Note that this will be called from the worker thread, possibly without
     * any gui locks!
     *
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT virtual void react_sidekick_notify(sidekick_i *t) = 0;
    /**
     * This is called upon completion of the sidekick.
     *
     * Note that this will be called from the worker thread, possibly without
     * any gui locks!
     *
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT virtual void react_sidekick_done(sidekick_i *t) = 0;
};

/**
 * A abtract interface for things that can process sidekicks.
 *
 * @author Aleksander Demko
 */
class scopira::core::sidekick_queue_i : public virtual scopira::tool::object
{
  protected:
    /// ctor (sets up instance)
    SCOPIRA_EXPORT sidekick_queue_i(void);
    /// ctor (sets up instance)
    SCOPIRA_EXPORT virtual ~sidekick_queue_i();

    /// actuall driver
    SCOPIRA_EXPORT virtual void enqueue_sidekick(sidekick_i *t) = 0;

    // helper to get around friend functionenss
    sidekick_reactor_i * & get_reactor(sidekick_i &sk) { return sk.dm_treact; }

  protected:
    SCOPIRA_EXPORT static sidekick_queue_i *dm_instance;

    friend void scopira::core::enqueue_sidekick(sidekick_i *, sidekick_reactor_i *);
};

// impls are in sidekickthread

#endif

