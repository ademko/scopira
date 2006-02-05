
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

#ifndef __INCLUDED_SCOPIRA_LAB_WINDOW_H__
#define __INCLUDED_SCOPIRA_LAB_WINDOW_H__

#include <scopira/tool/cacheflow.h>     // for circular_vector
#include <scopira/coreui/window.h>
#include <scopira/coreui/button.h>
#include <scopira/coreui/label.h>
#include <scopira/coreui/layout.h>
#include <scopira/lab/loop.h>

namespace scopira
{
  namespace lab
  {
    class lab_window;
    class error_window;
  }
}

/**
 * A lab widget base class than integrates with the lab_shell.
 *
 * Implementors should see the
 * protected constructor comments for implementation details.
 *
 * @author Aleksander Demko
 */
class scopira::lab::lab_window : public scopira::lab::lab_window_i,
  public scopira::coreui::button_constants
{
  public:
    /**
     * Sets the title of the window.
     *
     * @author Aleksander Demko
     */
    virtual void set_title(const std::string &newtitle);

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
    virtual void show_window(void);

    /**
     * Hides the window.
     * A window may be reshown.
     *
     * @author Aleksander Demko
     */ 
    virtual void hide_window(void);

    /**
     * Waits for the particular window to return an "event".
     * The most popular are close and apply events, but
     * particular windows and widgets many supply additional events.
     *
     * @param timeout an optional timeout to wait, in msec (1000 == 1 second), 0 for indefinate
     * @return returns true if there was an action, false if there was a timeout
     * @author Aleksander Demko
     */
    virtual bool wait_action(int timeout = 0);

    /**
     * Is the window currently visible?
     *
     * @author Aleksander Demko
     */
    virtual bool is_visible(void) const;

    /**
     * Gets the next action even and remove it from the internal queue.
     * This will return action_none_c if the queue is empty.
     *
     * @author Aleksander Demko
     */ 
    virtual int pop_action(void) const;

  protected:
    typedef scopira::lab::lab_lock lab_lock;

    /**
     * Contructor for implementators.
     *
     * Descedants should do the following:
     *  - in their ctor, they should do their own gui initing in a init_gui
     *    method
     *  - call lab_window::init_gui with your stuff. Make sure to call this
     *    method (and all your other gui initialization stuff) within
     *    a lab_lock!
     *  - all your gui accesing/chaning methods must do so within a lab_lock
     *
     * @author Aleksander Demko
     */ 
    lab_window(void);

    /**
     * Implementators must call this near the end of their UI construction phase.
     *
     * Notice: This function must be called inside of a lab_lock!
     *
     * @param basewidget the widget to put in the window. will be ref counted
     * @param butflags a flag set of button_ flags of buttons to have
     * @author Aleksander Demko
     */
    void init_gui(scopira::coreui::widget *basewidget, int butflags = button_close_c);

    /**
     * Convinience variant of init_gui.
     *
     * @author Aleksander Demko
     */
    void init_gui(GtkWidget *basewidget, int butflags = button_close_c)
      { init_gui(new scopira::coreui::widget(basewidget), butflags); }

    /**
     * Decendants may call this in their init_gui, but after their calls
     * to lab_window::init_gui and still within a lab_lock to change the title
     * of the window
     *
     * @author Aleksander Demko
     */
    void init_title(const std::string &newtitle)
      { dm_framer->set_title(newtitle); }

    /**
     * Call this if you want to signal an action that the script writing
     * can wait for. The default buttons (if used) send back the predefined
     * action_ constants already.
     *
     * @author Aleksander Demko
     */ 
    void push_action(int actionID);

    /**
     * This is called before doing the default button actions.
     * Descendants may overide this.
     * Return true to continue with default processing.
     * Return false to halt it.
     *
     * The default implementation does nothing but return true.
     *
     * @author Aleksander Demko
     */ 
    virtual bool on_button(int actionID);

    /// implementation
    virtual void gui_hide(void) { dm_framer->do_hide(); }

  private:
    /// internall class
    class framer : public scopira::coreui::window,
      public scopira::coreui::button_reactor_i
    {
      private:
        lab_window *dm_parent;
        scopira::tool::count_ptr<scopira::coreui::widget> dm_base;
      public:
        framer(lab_window *parent);
        virtual void react_button(scopira::coreui::button *source, int actionid);
        void init_gui(scopira::coreui::widget *basewidget, int butflags = button_close_c);
        virtual void on_close(void);
        virtual void on_destroy(void);
        void do_hide(void);
    };

    // monitors current window state... states go:
    struct open_area {
      bool isopen;        // is currently open
    };

    struct action_area {
      // this thing is tiny, replace with a fixed_array based one?
      scopira::tool::circular_vector<int> action_queue;
    };

    /// the actually window wrapping the widget
    scopira::tool::count_ptr<framer> dm_framer;
    /// the shared area between threads
    scopira::tool::shared_area<open_area> dm_open;
    /// Used to maintain the action event/wait queue
    scopira::tool::event_area<action_area> dm_action;
};

/**
 * A base/helper class that can be used to build window screens
 * that are comprised of other widgets.
 *
 * @author Aleksander Demko
 */ 
class scopira::lab::error_window : public scopira::lab::lab_window
{
  public:

    // Remember, any public methods that you expose to the Scopira Lab
    // user must internally use lab_locks before accessing anything in the
    // GUI

    /**
     * Does this window have it's in-error bit set?
     *
     * @author Aleksander Demko
     */
    bool failed(void) const;

  protected:
    /**
     * Constructor, used by descendants
     *
     * @author Aleksander Demko
     */ 
    error_window(void);

    /**
     * Descendants must call this method once sometime near the end
     * of their constructor.
     *
     * This is similar to container_base_p::init_gui().
     *
     * @param w the base widget of your container. ie the widget
     * that contains all your widgets. Cannot be null.
     * Will be count_ptr'ed.
     * @author Aleksander Demko
     */
    void init_gui(scopira::coreui::widget *w, bool okbutton);

    /**
     * Called when the user clicks apply or ok.
     *
     * The implementation does nothing.
     *
     * If the error flag is set, then the window will not be closed
     * (if this is an OK button).
     *
     * @author Aleksander Demko
     */ 
    virtual void on_apply(void);

    /**
     * Clears the error label and the error status.
     *
     * @author Aleksander Demko
     */ 
    void clear_error_label(void);
    /**
     * Sets the string of the current error label. A new error label will
     * be poped up if one doesn't exist yet.
     *
     * This also sets the error flag, which may be used in some contexts.
     *
     * @author Aleksander Demko
     */ 
    void set_error_label(const std::string &msg);

    /// implementation
    virtual bool on_button(int actionID);

  private:
    bool dm_inerror;    // GUI variable, make sure youre in a lab_lock
    scopira::tool::count_ptr<scopira::coreui::label> dm_errorlabel;
    scopira::tool::count_ptr<scopira::coreui::box_layout> dm_vbox;
};

#endif

