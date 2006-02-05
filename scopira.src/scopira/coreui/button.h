
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

#ifndef __INCLUDED__SCOPIRA_COREUI_BUTTON_H__
#define __INCLUDED__SCOPIRA_COREUI_BUTTON_H__

#include <gtk/gtk.h>

#include <string>

#include <scopira/coreui/widget.h>

namespace scopira
{
  namespace coreui
  {
    class button_reactor_i;
    class button;

    class imagewidget; //fwd
  }
}

/**
 * listener to button events
 * @author Aleksander Demko
 */ 
class scopira::coreui::button_reactor_i
{
  public:
    SCOPIRAUI_EXPORT virtual ~button_reactor_i() { }
    /**
     * The button event handler.
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT virtual void react_button(scopira::coreui::button *source, int actionid) = 0;
};

/**
 * button proponent, event only
 *
 * @author Aleksander Demko
 */ 
class scopira::coreui::button : public scopira::coreui::widget
{
  public:
    enum stock_t
    {
      stock_c = 1
    };

  protected:
    int dm_actionid, dm_actionid2;
    
    /// listener of events, if any
    button_reactor_i *dm_button_reactor;
    
    /// subwidget, not always used
    scopira::tool::count_ptr<scopira::coreui::widget> dm_sublabel;

    bool dm_flushed_actions;

  public:
    /**
     * Constructor.
     * @param label the label
     * @param actionid the id that will be sent back with events
     * @author Aleksander Demko
     */ 
    SCOPIRAUI_EXPORT button(const std::string &label, int actionid = 0, int actionid2 = 0);

    /**
     * Constructor, thats lets you provide any widget to be the "label".
     * Often, you'll use an iconwidget, label, or combination there of (via a box or some kind)
     * @param w the label
     * @param actionid the id that will be sent back with events
     * @author Aleksander Demko
     */ 
    SCOPIRAUI_EXPORT button(scopira::coreui::widget *w, int actionid = 0, int actionid2 = 0);

    /**
     * A constructor that lets you specify a stock gtk icon
     * http://developer.gnome.org/doc/API/2.0/gtk/gtk-Stock-Items.html
     * The first parameter must be button::stock_c
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT button(stock_t dummy, const char *stock_id, int actionid = 0, int actionid2 = 0);

    
    /**
     * This static function packs the given image and string together into one
     * new widget that is returned. You usually pass this return object to
     * the widget-accepting ctor of button
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT static scopira::coreui::widget * new_image_label(scopira::coreui::imagewidget *iw, const std::string &string);

    /// gets the current actionid number 
    int get_actionid(void) const { return dm_actionid; }
    /// api access routines
    SCOPIRAUI_EXPORT void set_label(const std::string &label);
    /// sets the reactor
    void set_button_reactor(button_reactor_i *react) { dm_button_reactor = react; }

    /**
     * Users can call this in their handlers to stop further action methods
     * from come down. Useful for example if you'd like to prevent the subsequent
     * action_close_c from coming after a "failed" handling of action_apply_c.
     *
     * You can also undo a previous flush_actions(true) by calling this with false.
     *
     * @author Aleksander Demko
     */
    void flush_actions(bool flushem = true) { dm_flushed_actions = flushem; }

  private:
    /**
     * only one may be non-null
     * @author Aleksander Demko
     */ 
    SCOPIRAUI_EXPORT void init_gui(void);

  private:
    //called when toggle information is changed for buffered data
    static void h_on_activate(GtkButton *but, gpointer data);
};

#endif
