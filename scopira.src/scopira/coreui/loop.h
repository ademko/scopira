
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

#ifndef __INCLUDED_SCOPIRA_COREUI_LOOP_H__
#define __INCLUDED_SCOPIRA_COREUI_LOOP_H__

#include <set>

#include <scopira/tool/thread.h>
#include <scopira/core/loop.h>
#include <scopira/coreui/app.h>
#include <scopira/coreui/export.h>

namespace scopira
{
  namespace coreui
  {
    class ui_loop;

    /**
     * Process some UI events.
     *
     * This should be called a GUI threads that want to flush/process all/any pending UI
     * events before proceeding.
     *
     * @author Aleksander Demko
     */ 
    void process_ui_events(void);
  }
}

/**
 * Builds upon scopira::core::shell by adding redamentary gtk things.
 * An engine is always started with this version.
 *
 * @author Aleksander Demko
 */ 
class scopira::coreui::ui_loop : public scopira::core::basic_loop, protected scopira::coreui::app_i
{
  public:
    /**
     * Constructor.
     *
     * Registeres baseuikit too. And does some multi threaded
     * initialization of gtk, if startgtk is true.
     *
     * If startgtk is false, you can start up gtk with init_gui.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRAUI_EXPORT ui_loop(int &argc, char **&argv);

    /**
     * Destructor.
     *
     * It will delete any open windows.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRAUI_EXPORT virtual ~ui_loop();      // virtual cuz im inheriting from app_i... perhaps switch that to containment?

    /**
     * Blocks and performa the gtk main loop.
     *
     * If quitonlastclose is true, the syste will trigger a gtk_main_quit when the last
     * window closes itself.
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT void run_gui(bool quitonlastclose = true);

  public:
    SCOPIRAUI_EXPORT virtual void add_window(window *win);
    SCOPIRAUI_EXPORT virtual void remove_window(window *win);
    SCOPIRAUI_EXPORT virtual void set_status(const std::string &msg, bool is_error = false);
    SCOPIRAUI_EXPORT virtual bool is_debug_mode(void) const;

  protected:
    /**
     * Constructor.
     *
     * Registeres baseuikit too. And does some multi threaded
     * initialization of gtk, if startgtk is true.
     *
     * If startgtk is false, you can start up gtk with init_gui.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRAUI_EXPORT ui_loop(bool startgui, int &argc, char **&argv);

    /**
     * Descedants may use this to initialize the gtk system.
     *
     * @param req if true, failure to initialize the gtk system results in program abortion
     * @return true if the gui system was initialized successdfully. Always true if req is true
     * (because if there was a failure, the whole program would be aborted)
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT bool init_gui(bool req, int &argc, char **&argv);

    /// flushes the window list
    SCOPIRAUI_EXPORT void gui_hide_all(void);

    /// just fot task_lab_loop
    SCOPIRAUI_EXPORT void set_quitonlastclose(void) { dm_quitonlastclose = true; }

  protected:
    struct window_area
    {
      typedef std::set<scopira::tool::count_ptr<window> > windowlist_t;

      windowlist_t windowlist;
    };

    /// the window list area
    scopira::tool::shared_area<window_area> dm_windowlist;

  private:
    /// should gtk_main_quit be called on last window close?
    volatile bool dm_quitonlastclose;
};

/**
  \page scopirauisyspage Scopira UI Reference

  This subsystem provides the standard GUI widget library and 2D drawing capabilities
  to Scopira applications.

  Namespaces that comprise this library are: scopira::coreui, scopira::uikit
  and scopira::lab

*/

/**
  \page scopiraappspage Scopira applications

  Scopira is predomenantly an library for building domain-specific
  applications. However two generic applications are provides.

  \section appsec scopira.exe

  scopira.exe is a basic, graphical model/view project application.
  It includes all the basic facilities for model managment and view launching.
  You can build on this application by making plug-ins that register models
  and/or views, and then loading those libraries with the lib= command line parameter.

  \section agentsec scopiraagent.exe

  scopiraagent.exe is a non-gui program for launch agent tasks.
  You typically load your plug-in of tasks via a lib= parameter, and tell
  scopiraagent.exe which task to run with the task= parameter.
  You should provide the full registration string of the tkas you'd like luanched
  (typically, you shadow the full C++ name when deciding on registration strings).

*/

#endif

