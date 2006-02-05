
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

#ifndef __INCLUDED_SCOPIRA_LAB_LOOP_H__
#define __INCLUDED_SCOPIRA_LAB_LOOP_H__

#include <set>
#include <map>

#include <gtk/gtk.h>

#include <scopira/tool/thread.h>
#include <scopira/coreui/loop.h>
#include <scopira/coreui/export.h>

namespace scopira
{
  /**
   * This subsystem provides the foundation and basic
   * classes for Scopira Lab.
   *
   * @author Aleksander Demko
   */ 
  namespace lab
  {
    class lab_lock;
    class lab_window_i;
    class lab_loop;
  }
}

/**
 * An internal class used by lab widgets.
 *
 * It aquires a lock on the gtk system so that your thread may do
 * gtk calls.
 *
 * @author Aleksander Demko
 */
class scopira::lab::lab_lock
{
  public:
    /// locking constructor
    SCOPIRAUI_EXPORT lab_lock(void);
    /// unlocking destructor
    SCOPIRAUI_EXPORT ~lab_lock();
    /**
     * This is a useful debugging aid or as a general checker.
     *
     * This can be used to check if youre currently in a lab_lock, very
     * useful for GUI code (that always must be called in a lab_lock).
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT static bool is_locked(void);
};

/**
 * A open window that is tracked by the shell engine.
 *
 * @author Aleksander Demko
 */
class scopira::lab::lab_window_i : public virtual scopira::tool::object
{
  private:
    friend class scopira::lab::lab_loop;
  public:
    /**
     * Sets the title of the window.
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT virtual void set_title(const std::string &newtitle) = 0;    // this signature here only for consistancy

    /**
     * Shows the window to the world.
     * Call this after preparing your window. This call does not
     * block (it'll return right away).
     *
     * Follow with a wait_action() call if you'd like to wait for the dialog
     * to reply.
     *
     * Descendants may override this and provide additional
     * functionality (make sure to call this version though).
     *
     * @author Aleksander Demko
     */ 
    SCOPIRAUI_EXPORT virtual void show_window(void) = 0;   // this signature here only for consistancy

    /**
     * Hides the window.
     * A window may be reshown.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRAUI_EXPORT virtual void hide_window(void) = 0;   // this signature here only for consistancy

    /**
     * Waits for the particular window to return an "event".
     * The most popular are close and apply events, but
     * particular windows and widgets many supply additional events.
     *
     * @param timeout an optional timeout to wait
     * @return returns true if there was an action, false if there was a timeout
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT virtual bool wait_action(int timeout = 0) = 0;    // this signature here only for consistancy

    /**
     * Is the window currently visible?
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT virtual bool is_visible(void) const = 0;    // this signature here only for consistancy

    /**
     * Gets the next action even and remove it from the internal queue.
     * This will return action_none_c if the queue is empty.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRAUI_EXPORT virtual int pop_action(void) const = 0;     // this signature here only for consistancy

  protected:
    /**
     * Does an actual window hide.
     *
     * This is called by a gui thread, within a lab_lock. When implementing
     * this, DO NOT setup your own lab_lock.
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT virtual void gui_hide(void) = 0;

    /**
     * Decedants should call this when they are shown.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRAUI_EXPORT void reg_window(void);
    /**
     * Decedants should call this when they are hidden.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRAUI_EXPORT void unreg_window(void);

    /**
     * Pushes a action notice to the shell.
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT void push_action(void);

    /// special function used by quit_window
    static void gui_hide_all(void);
};

/**
 * This sets up a *GUI* ScopiraLab environment.
 *
 * FOR LINUX ONLY. MS Windows' insistance on tieing event queues
 * per threads makes this near impossible to implement.
 *
 * You should instantiate one of these in your main() function.
 *
 * It will setup a background scopira engine for, which entails loading
 * various non-gui kits.
 *
 * This one also spawns a GUI thread in the background to help with the GUI system.
 *
 * @author Aleksander Demko
 */
class scopira::lab::lab_loop : public scopira::coreui::ui_loop
{
  private:
    friend class scopira::lab::lab_lock;
    friend class scopira::lab::lab_window_i;
  public:
    /// returns the static instance
    static lab_loop * lab_instance(void) { return dm_lab_instance; }
    /// returns the static instance (this version is a bonus)
    static lab_loop * instance(void) { return dm_lab_instance; }

    /**
     * A constructor that utilizes the command line
     * arguments in the main and also enables the GTK+ front end.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRAUI_EXPORT lab_loop(int &argc, char **&argv);
    /**
     * This version lets you specify if you'd like the shell to *try* to open a display.
     * If you pass false for trydisplay, the display will definatly not be connected.
     *
     * If you pass true for trydisplay, the display will only be connected if there is one.
     *
     * In either case, if there is no connected display, make sure to not make any
     * GUI calls at all. You may check for connected displays using the
     * has_display method (a static method!)
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT lab_loop(bool trydisplay, int &argc, char **&argv);

    /// destructor
    SCOPIRAUI_EXPORT ~lab_loop();

    /**
     * This can be used to start a gui if you didnt start it via the constructor.
     *
     * Returns false if the gui was not launched.
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT bool open_display(int &argc, char **&argv);

    /**
     * A master wait call. This version waits for ANY open lab_window_i to
     * produce a message.
     *
     * If you supply a time out, the wait will wait a maximum of that many msec
     * (1000=1 second). 0, the default, will block indefinatly.
     *
     * True is returned if an action somewhere was signalled, false is returned
     * if the timeout expired. You may extract the window of interest
     * with pop_action.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRAUI_EXPORT bool wait_action(int msec = 0);

    /**
     * Gets and removes the window that signalled the last action.
     * Null will be returned for none.
     *
     * Please note that the queue has a length of one.
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT lab_window_i * pop_action(void);

    /**
     * Are there any widgets currently visible?
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT bool is_visible(void) const;

    /**
     * Hides all open widgets.
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT void hide_all(void);

    /**
     * Is the shell connected to an active display.
     * If it's not, you should'nt do ANY GUI related calls, including
     * instantiating any window classes.
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT static bool has_display(void);

  protected://changed to protected for task_lab_loop
    /// singleton instance
    SCOPIRAUI_EXPORT static lab_loop *dm_lab_instance;

    /// coordinates the waiting of action messages
    struct open_area {
      typedef std::set<scopira::tool::count_ptr<lab_window_i> > windowlist_t;
      windowlist_t windowlist;
      bool runninggtk;    // is the gtk thread in gtk_main?
      bool gtkstopnow;    // tell the gtk thread to not go into gtk_main
      bool gtkjumpnow;    // tell the gtk thread to go into gtk_main now, and not wait for a window
    };
    // used by the lab_lock to signal the ui thread that it wants in
    struct attempt_area {
      bool attempt;
    };

    /// the action area for wait/pop/push
    struct action_area {
      lab_window_i *active_window;
    };

    /// i really dont trust the gtk_thread_enter stuff, use this too
    /// (and testing shows that gtk_thread_enter etc is insuffient too)
    scopira::tool::mutex dm_gtklock;
    /// local TLS for if a thread has the lock
    scopira::tool::thread::tlskey_t dm_gtklock_tls;

    /// number of open windows
    scopira::tool::event_area<open_area> dm_openwindows;
    /// a lab lock is attempting to get into gtk
    scopira::tool::shared_area<attempt_area> dm_attempt;
    /// the action area
    scopira::tool::event_area<action_area> dm_action;
    
    /// timer tag
    guint dm_timmertag;
    /// the gtk handling thread
    scopira::tool::thread dm_thread;
    /// do we actually have a gui? shared, but readonly, so its ok (i hope)
    bool dm_hasdisplay;

  private:
    /// called by the two ctors
    void ctor(void);
    /// thread runner
    static void* gui_thread_run(void *v);
    static gint h_timer_tick(gpointer data);
    /// called by friend lab_window_i
    void gui_hide_all(void);
  private:
    /// Don't try to call this people!
    /// the "run_gui()" part is already handled by a background thread
    /// (that's the whole point of lab_loop-stuff)
    void run_gui(bool quitonlastclose) { }
};

#endif

