
/*
 *  Copyright (c) 2001-2005    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#ifndef __INCLUDED__SCOPIRA_COREUI_WINDOW__
#define __INCLUDED__SCOPIRA_COREUI_WINDOW__

#include <list>
#include <string>

#include <scopira/coreui/widget.h>
#include <scopira/coreui/export.h>

namespace scopira
{
  namespace tool
  {
    // forward
    class destroy_reactor_i;
  }
  namespace coreui
  {
    class close_reactor_i;

    class window;
    class dialog;
  }
}

/**
 * reactor to window-close attempts
 *
 * @author Aleksander Demko
 */
class scopira::coreui::close_reactor_i : public virtual scopira::tool::object
{
  public:
    /**
     * called when the user attempts to close the window via the
     * X. you may reject this via set_closable
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT virtual void react_close(scopira::tool::object *source) = 0;
};

/**
 * window widget. this is standard stuff that all non-appwindows
 * should have. it binds this c++ class with the given gtk window
 * (bound via the destroy signal that is).
 *
 * Decendants should do this:
 *   - ctor
 *     - must calls its own init_gui
 *   - init_ui
 *     - must set dm_widget (preferable to a gtk_window
 *     - must call window::init_gui at its end
 *
 * @author Aleksander Demko
 * @author Marina Mandelzweig
 */
class scopira::coreui::window : public scopira::coreui::widget
{
  protected:
    ///  title
    std::string dm_title;
    /// am modal?
    bool dm_modal;
    /// destroy reactors
    typedef std::list<tool::destroy_reactor_i*> dvec_t;
    typedef std::list<close_reactor_i*> cvec_t;

    dvec_t dm_destroyreactors;
    cvec_t dm_closereactors;

    /// should "delete_event" signal handler, emit the "destroy" signal.
    /// TRUE means not to distroy the window
    bool dm_closable;

  public:
    /// destuctor
    SCOPIRAUI_EXPORT virtual ~window(void);

    /**
     * Get the title of the window
     * @author Aleksander Demko
     */
    const std::string & get_title(void) const { return dm_title; }

    /**
     * Sets the default size of the window.
     * If the window's "natural" size is larger than this,
     * that will be used instead.
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT void set_default_size(int w, int h);

    /**
     * Set title variable and updates the gtk widget. IE. sets the title
     * of the window. dm_widget must exist at this point.
     * Alternativly, you can set dm_title somewhere in your ctor.
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT virtual void set_title(const std::string &t);

    /// adds a new destroy reactor
    SCOPIRAUI_EXPORT void add_destroy_reactor(scopira::tool::destroy_reactor_i *r);
    /// removes the given destroy reactor
    SCOPIRAUI_EXPORT void remove_destroy_reactor(scopira::tool::destroy_reactor_i *r);
    /// adds a new close reactor (attempt to close... good place to override delete requests)
    SCOPIRAUI_EXPORT void add_close_reactor(close_reactor_i *r);
    /// removes the given close reactor
    SCOPIRAUI_EXPORT void remove_close_reactor(close_reactor_i *r);

    /// called when received change focus
    SCOPIRAUI_EXPORT virtual void on_focus(bool focus);

    /**
     * this is called to query if the current object can be
     * destroyed (often because of clicking the close button
     * on the window)
     * default implementation returns false.
     *
     * @author Aleksander Demko
     */
    virtual bool is_closable(void) const { return dm_closable; }
    /// sets the closable flag... you usually call this
    /// in your close_reactor_i
    virtual void set_closable(bool f) { dm_closable = f; }

    /**
     * Called when the window has been requested to close, via the close
     * window X button.
     * This version first sets dm_closable then calls all of the close listeners.
     *
     * You version should call this version first.
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT virtual void on_close(void);

    /**
     * Called when received a destroy event. default action is to sub_ref self.
     * overiding methods should NOT call this version. this is only called
     * when an event is originating from the gtk side, not if the destructor
     * is deleteing the gtk object
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT virtual void on_destroy(void);

  protected:
    /// constructor
    SCOPIRAUI_EXPORT window(const std::string &title, bool modal = false);

    /**
     * Inits the GUI.
     *
     * This version assumes that you have set dm_widget to be a valid gtk_window or
     * descendant.
     *
     * In your constructor, you should
     * call your own init_gui-style method. That method should setup your
     * gui by settings dm_widget to something valid. It should then call this version.
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT void init_gui(void);

    /**
     * Inits the GUI.
     *
     * This version will build the dm_widget for you, give the flags provided.
     * There are currently no flags defined, but there will be in the future
     * (for stuff like title bar buttons, resizability, etc).
     *
     * @author Aleksander Demko
     */ 
    SCOPIRAUI_EXPORT void init_gui(GtkWidget *widget, int flags = 0);

  private:
    /// static handler, distroy a gtk widget
    static void h_destroy(GtkWidget *widget, gpointer data);
    /// static handler, delete_event
    static gint h_delete_event(GtkWidget *widget, GdkEvent *event, gpointer data);
};

/**
 * Dialog window. This can either be used by itself, or subclassed. If subclassed:
 *
 * Decendants should do this:
 *   - ctor
 *     - must call the protoected inherited ctor
 *     - should call their own init_gui
 *   - init_gui:
 *     - should call dialog::init_gui at the start
 *     - do NOT override dm_widget. use get_*_area to build your screen
 *     - you may call init_* to setup some stock buttons
 *
 * @author Aleksander Demko
 * @author Marina Mandelzweig
 */
class scopira::coreui::dialog : public scopira::coreui::window
{
  public:
    /**
     * Constructor.
     * This version is NOT FOR descendants!
     *
     * @param title title bar string
     * @param modal should the dialog be modal?
     * @param buttons should the frame have window buttons (close, etc)
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT dialog(const std::string &title, bool modal);
    /// destuctor
    SCOPIRAUI_EXPORT virtual ~dialog();

    /// Create an simple label, add it to the dialog, and return it
    SCOPIRAUI_EXPORT GtkWidget * init_label(const std::string &text);
    /// Create an ok button, add it to the dialog, attach handler and return it
    SCOPIRAUI_EXPORT GtkWidget * init_ok_button(const std::string &label, bool defhandler = true);
    /// create a cancel button, add it to the dialog, attach handler and return it
    SCOPIRAUI_EXPORT GtkWidget * init_cancel_button(const std::string &label, bool defhandler = true);
    /// create text entry bo, add it to the dialog, attach handler and return it
    SCOPIRAUI_EXPORT GtkWidget * init_text_entry(const std::string &label, const std::string &def, bool defhandler = true);

    /// gets the vbox area, a vbox
    SCOPIRAUI_EXPORT GtkWidget * get_window_area(void);
    /// gets the action area, an hbox
    SCOPIRAUI_EXPORT GtkWidget * get_button_area(void);

    /// adds the given widget to the window area... short hand for a quick box push back on get_window_area
    SCOPIRAUI_EXPORT void init_window_area(GtkWidget *w);

    /// ok button handler
    SCOPIRAUI_EXPORT virtual void on_ok(void);
    /// cancel button handler
    SCOPIRAUI_EXPORT virtual void on_cancel(void);

  protected:
    /**
     * Constructor. Decendants MUST use this one.
     *
     * @param modal should the dialog be modal?
     * @param buttons should the frame have window buttons (close, etc)
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT dialog(bool modal);
    /**
     * You need not prepare anything, but you must call this sometime.
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT void init_gui(void);

  private:
    /// static cancel button handler
    static void h_on_cancel(GtkWidget *widget, gpointer data);
    /// static ok button handler
    static void h_on_ok(GtkWidget *widget, gpointer data);
};

#endif

