
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

#ifndef __INCLUDED_SCOPIRA_LAB_SIDEKICKLOOP_H__
#define __INCLUDED_SCOPIRA_LAB_SIDEKICKLOOP_H__

#include <list>

#include <scopira/coreui/export.h>
#include <scopira/tool/thread.h>
#include <scopira/lab/loop.h>
#include <scopira/core/sidekick.h>

namespace scopira
{
  namespace lab
  {
    class sidekick_lab_loop;
  }
}

/**
 * A lab_loop variant that is able to handles sidekicks requests.
 *
 * FOR LINUX ONLY. MS Windows' insistance on tieing event queues
 * per threads makes this near impossible to implement.
 * Windows users can use the sidekick_thread helper though, just make sure
 * to do NO gui stuff at all. 
 *
 * QUESTION. Is this thing still even needed? Or will people just use
 * lab_loop + sidekick_thread? Or maybe not...
 *
 * That is, the worker thread while in ~sidekick_lab_loop() will process any
 * queued sidekicks from the gui thread (which queues them via append_sidekick()).
 *
 * @author Aleksander Demko
 */
class scopira::lab::sidekick_lab_loop : public scopira::lab::lab_loop,
    public scopira::core::sidekick_queue_i
{
  public:
    /// ctor, same as in lab_loop
    SCOPIRAUI_EXPORT sidekick_lab_loop(int &argc, char **&argv);
    /// ctor, same as in lab_loop
    SCOPIRAUI_EXPORT sidekick_lab_loop(bool trydisplay, int &argc, char **&argv);

    /**
     * Dtor. This dtor also does the bulk of the event processing for the sidekick_lab_loop
     * though.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRAUI_EXPORT virtual ~sidekick_lab_loop();

  protected:
    /**
     * Enqueues a trask to the sidekick_lab_loop. scopira::lab::enqueue_sidekick
     * calls this, so you probably should be using that version instead.
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT void enqueue_sidekick(scopira::core::sidekick_i *t);

  private:
    typedef std::list<scopira::tool::count_ptr<scopira::core::sidekick_i> > sidekick_list;

    // the condition in dm_openwindows is used as the signal for this area
    struct sidekick_area {
      sidekick_list queue;
    };

    scopira::tool::shared_area<sidekick_area> dm_sidekickarea;
};

#endif

