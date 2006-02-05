
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

#ifndef __INCLUDED_SCOPIRA_COREUI_TIMER_H__
#define __INCLUDED_SCOPIRA_COREUI_TIMER_H__

#include <glib.h>

#include <scopira/coreui/export.h>

namespace scopira
{
  namespace coreui
  {
    class uitimer;
  }
}

/**
 * This represents a GTK+ timer. A timer is something that will
 * call a given handler function WITHIN THE GTK+/GUI thread.
 *
 * @author Aleksander Demko
 */ 
class scopira::coreui::uitimer
{
  public:
    /// a timer function, typically they look like:
    //return TRUE to be called again, FALSE if you want to no longer be called
    //gboolean yourfunc(gpointer data);
    typedef gboolean (*timerfunc_t)(gpointer data);

  public:
    /**
     * Constructor. Call the given function with the given data
     * at the given internal. interval is in msec, 1000 = 1 second.
     *
     * @author Aleksander Demko
     */ 
    uitimer(unsigned int interval, timerfunc_t func, void *funcdata)
      { dm_timmertag = ::g_timeout_add(interval, func, funcdata); }
    /// dtor
    ~uitimer()
      { ::g_source_remove(dm_timmertag); }

  private:
    guint dm_timmertag;
};

#endif

