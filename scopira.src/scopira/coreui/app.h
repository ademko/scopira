
/*
 *  Copyright (c) 2001    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#ifndef __INCLUDED__SCOPIRA_COREUI_APP__
#define __INCLUDED__SCOPIRA_COREUI_APP__

#include <string>

#include <scopira/coreui/export.h>

namespace scopira
{
  /**
   * coreui contains a common set of graphics code that is usefull
   * for gtk+ based scopira front ends and proponents
   *
   * @author Aleksander Demko
   */ 
  namespace coreui
  {
    class app_i;
    // forward
    class window;
  }
}

#define APPSTATUS(msg) scopira::coreui::app_i::instance()->set_status((msg))
#define APPERROR(msg) scopira::coreui::app_i::instance()->set_status((msg), true)

/**
 * A singleton application instance.
 *
 * An "application" is a manager of windows, provider of status bars
 * and other central UI app stuff.
 *
 * @author Aleksander Demko
 */
class scopira::coreui::app_i
{
  protected:
    /// the static, sole instance of mainwindow_i, if any
    SCOPIRAUI_EXPORT static app_i *dm_app;

  public:
    SCOPIRAUI_EXPORT virtual ~app_i() { }
    /// get appwindow instance
    static app_i * instance(void) { return dm_app; }

    /// add a new window to the list of windows
    SCOPIRAUI_EXPORT virtual void add_window(window *win) = 0;
    /// remove the given window from the list of windows
    SCOPIRAUI_EXPORT virtual void remove_window(window *win) = 0;
    /// update the status line
    SCOPIRAUI_EXPORT virtual void set_status(const std::string &msg, bool is_error = false) = 0;
    /// are we in debug mode?
    SCOPIRAUI_EXPORT virtual bool is_debug_mode(void) const = 0;

  protected:
    SCOPIRAUI_EXPORT app_i(void) { }
    
    SCOPIRAUI_EXPORT static void set_app(app_i *a);
};

#endif

