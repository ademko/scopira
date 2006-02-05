

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

#ifndef __INCLUDED__SCOPIRA_COREUI_CHECKBUTTON_H__
#define __INCLUDED__SCOPIRA_COREUI_CHECKBUTTON_H__

#include <gtk/gtk.h>

#include <string>

#include <scopira/coreui/widget.h>
#include <scopira/coreui/export.h>

namespace scopira
{
  namespace coreui
  {
  	class checkbutton_reactor_i;
    class checkbutton;
  }
}

/**
 * listener to checkbutton events
 * @author Shantha Ramachandran
 */ 
class scopira::coreui::checkbutton_reactor_i
{
  public:
    SCOPIRAUI_EXPORT virtual ~checkbutton_reactor_i() { }
    /**
     * The checkbutton event handler.
     */
    SCOPIRAUI_EXPORT virtual void react_checkbutton(scopira::coreui::checkbutton *source, bool checkval) = 0;
};

/**
 * check button proponent
 *
 * @author Aleksander Demko
 */ 
class scopira::coreui::checkbutton : public scopira::coreui::widget
{
  protected:
    /// listener of events, if any
    checkbutton_reactor_i *dm_checkbutton_reactor;
    
  public:
    /// ctor
    SCOPIRAUI_EXPORT checkbutton(void);
    /// ctor
    SCOPIRAUI_EXPORT checkbutton(const std::string &label, bool is_checked = false);

    /// api access routines
    SCOPIRAUI_EXPORT void set_label(const std::string &label);
    /// set the state
    SCOPIRAUI_EXPORT void set_checked(bool check);
    /// is checked?
    SCOPIRAUI_EXPORT bool is_checked(void) const;
    
    void set_checkbutton_reactor(checkbutton_reactor_i *reac) { dm_checkbutton_reactor = reac; }

  protected:
    void init_gui(const char *label);

  private:
    //called when toggle information is changed for buffered data
    static void h_on_toggle(GtkToggleButton *but, gpointer data);
};

#endif

