
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
     * This may be called by any thread to enqueue jobs for the "sidekick thread".
     * If there is no sidekick thread active (either via sidekick_thread or sidekick_loop),
     * then the sidekick will be executed outright in the calling thread.
     *
     * reac is the notify reactor. It may be null.
     *
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT void enqueue_sidekick(sidekick_i *t, sidekick_reactor_i *reac = 0);

    /**
     * This will return true if there are any jobs or tasks currently being
     * exectued in the sidekick thread.
     *
     * When there is no sidekick thread, this will always return false
     *
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT bool is_sidekick_running(void);
  }
}

/**
 * This is a "sidekick" task. Decendants can implement the run()
 * method and then be given to the sidekick thread for processing.
 *
 * See \ref scopiracoresidekick
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
    /// additional driver method
    SCOPIRA_EXPORT virtual bool is_sidekick_running(void) = 0;

    /// helper to get around friend functionenss
    sidekick_reactor_i * & get_reactor(sidekick_i &sk) { return sk.dm_treact; }

  protected:
    /// static instance variable
    SCOPIRA_EXPORT static sidekick_queue_i *dm_instance;

    friend void scopira::core::enqueue_sidekick(sidekick_i *, sidekick_reactor_i *);
    friend bool scopira::core::is_sidekick_running(void);
};

// impls are in sidekickthread

/**
 \page scopiracoresidekick Sidekicks

 \section introsec Introduction

 The sidekick ("mini") framework provides developers an easy to use method
 of performing intensive work in a dedicated worker thread, with the intent of
 letting the GUI thread continue to maintain the user interface
 and any user interactions.

 \section overviewsec Overview

 A sidekick-enabled must be broken up into the following componets:

   - The algorithm core must be wrapped in a scopira::core::sidekick_i derived object
   - The GUI thread should peridically monitor the sidekick
   - If there is any common information shared between the GUI and sidekick threads, it must be protected

 You often implement this using one of two techniques:

   - the sidekick is "embeded" into your GUI window or view (easier)
   - the sidekick is implemented as a seperate, dedicated class (more modular)

 Both techniques are presented below.

 \section embedsec Embedding the sidekick into the GUI class

 \subsection embsidekickmakesec Making your sidekick method

 First, make your GUI class (usually the window or widget that will manage the running
 process visually) a decendant of scopira::core::sidekick_i.
 Implement the run() method of this interface. The run method should have the form:

 \verbatim
    void run(void);
 \endverbatim

 In this method, you will perform the the work that is to be done in the sidekick thread.
 TODO
 As the run() method is executed concurently with the GUI thread, great care must be taken
 to not allow of variable access conflicts. In particular:
   - Try to use only local variables for intermediate results within the run() method
   - You may share startup/initialization shutdown/result variables without fear of conflict, as the worker thread does not exist (or has just finished) when they are accessed by the GUI thread
   - all other "shared" variables must be protected on a mutex (more on this below)
 
 \subsection embedchecksec Monitoring the sidekick thread

 The GUI thread can monitor the sidekick method's progress.

 The simplest case involves simply checking the scopira::core::is_sidekick_running()
 function during a user initiated event.

 To continuous and automically check the status without the user's intervetion, setup
 a scopira::coreui::uitimer object. This can be used for progress bars, continous plots, text out, etc, for example.

 \subsection embedsharedareasec Complex information sharing

 The GUI thread and sidekick thread may want to share information during the course of algorithm execution.
 This is particularly useful for sending intermidate and progress reports to the GUI thread, which can then report
 back to the user. The user may, via the GUI thread, decide to adjust the running parameters or cancel the job
 outright.

 Any and all shared variables between the GUI and sidekick threads must be protected with a thread scopira::tool::mutex
 class. This is best done using a scopira::tool::shared_area concept, that encapsulates the data to be shared
 between two threads and protects and controls access via a mutex:

 \verbatim
   struct progress_area {
     int workdone, totalworktodo;
   };

   scopira::tool::shared_area<progress_area> dm_progressarea;
 \endverbatim

 Then, any code segments (both for the GUI thread or run() method thread)
 should be done via scopira::tool::locker_ptr RIIA idiom object.
 For example:

 \verbatim
 ... other code ...
 {
   scopira::tool::locker_ptr<progress_area> L(dm_progressarea);

   // can now access the shared variables, knowing that the other thread cannot access it at the same time
   return L->workdone / L->totalworktodo;
 }
 ... other code ...
 \endverbatim

 You could forgo the shared_area and locker_ptr classes and just use member variables and one
 mutex class directly, this is not recommended and is more error-prone.

 \section seperatesec Using a seperate, dedicated sidekick task

 \subsection dedwrapsec Making a sidekick task

 The algorithm must be wrapped in a class that is a decendant of scopira::core::sidekick_i

 The GUI thread should then (often, as a result of user-interaction):

   - create an instance of the scopira::core::sidekick_i worker object
   - call any initialization methods (include the constructor)
   - pass the object off to scopira::core::enqueue_sidekick()
   - The sidekick thread will then run the object's run() method
   - You can also directly inspect the sidekick's data (without fear of thread conflicts)
     after it has completed running (executing it's run() method with the sidekick thread)
 
 \subsection dedchecksec Monitoring the sidekick

 The GUI thread can monitor the sidekick method's progress.

 The simplest case involves simply checking the scopira::core::is_sidekick_running()
 function during a user initiated event.

 To continuous and automically check the status without the user's intervetion, setup
 a scopira::coreui::uitimer object. This can be used for progress bars, continous plots, text out, etc, for example.

 \subsection dedsharedareasec Complex information sharing

 The GUI thread can also call methods within the running sidekick task to get
 more specific information about it's progress. However, as these inspection calls
 are done in the GUI thread, they may conflict with the sidekick thread who may be updating the same
 information that is being inspected.

 To prevent this from happening, the common area should be in a scopira::tool::mutex-protected
 area. This can be done with the scopira::tool::shared_area

 For example:

 \verbatim
   struct progress_area {
     int workdone, totalworktodo;
   };

   scopira::tool::shared_area<progress_area> dm_progressarea;
 \endverbatim

 Then, any code segments should be done via scopira::tool::locker_ptr RIIA idiom object.
 Your access method (which the GUI thread should call), could look like:

 \verbatim
 double ui_get_progress(void)
 {
   scopira::tool::locker_ptr<progress_area> L(dm_progressarea);

   return L->workdone / L->totalworktodo;
 }
 \endverbatim

 Your actual worker code would like this (this would be in the run() method):

 \verbatim
    ... algorithm code ...
    {
      scopira::tool::locker_ptr<progress_area> L(dm_progressarea);

      L->workdone += 1;   // did one more task
    }
    ... algorithm code ...
 \endverbatim

 You could forgo the shared_area and locker_ptr classes and just use member variables and one
 mutex class directly, this is not recommended and is more error-prone.

*/

#endif

