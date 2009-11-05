
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

#ifndef __INCLUDED__SCOPIRA_COREUI_RADIOBUTTONS_H__
#define __INCLUDED__SCOPIRA_COREUI_RADIOBUTTONS_H__

#include <map>
#include <string>

#include <gtk/gtk.h>

#include <scopira/coreui/widget.h>
#include <scopira/coreui/export.h>

namespace scopira
{
  namespace coreui
  {
    class radiobutton_reactor_i;
    class radiobutton;
  }
}

/**
 * listener to radiobutton events, when the radiobutton changes
 * @author Aleksander Demko
 */
class scopira::coreui::radiobutton_reactor_i
{
  public:
    SCOPIRAUI_EXPORT virtual ~radiobutton_reactor_i() { }
    SCOPIRAUI_EXPORT virtual void react_radiobutton(scopira::coreui::radiobutton *source, int selection) = 0;
};

/**
 * a group of radio buttons.
 *
 * Binding Map (intput, output)
 * 0 - int_i the current selection value
 *
 * Event Map
 *  Value Changed (output)
 *    0 - int_i the current selection value
 *
 * @author Aleksander Demko
 * @author Rodrigo Vivanco
 */
class scopira::coreui::radiobutton : public scopira::coreui::widget
{
  protected:
    typedef std::map<int, GtkWidget* > idmap_t;

    GtkWidget *dm_firstbutton;

    idmap_t dm_idrad;

    /// listener of events, if any
    scopira::coreui::radiobutton_reactor_i *dm_radiobutton_reactor;

  public:
    /// constrcutor
    SCOPIRAUI_EXPORT radiobutton(void);
    /// caled when its handler setting time
    void set_radiobutton_reactor(scopira::coreui::radiobutton_reactor_i *reac) { dm_radiobutton_reactor = reac; }

    /// adds a selection
    SCOPIRAUI_EXPORT void add_selection(int id, const std::string &desc);
    // which of the radio buttons to select
    // @author Rodrigo Vivanco
    SCOPIRAUI_EXPORT void set_selection(int id);

    // which of the radio buttons is selected
    // @author Rodrigo Vivanco
    SCOPIRAUI_EXPORT int get_selection(void) const;

  protected:
    SCOPIRAUI_EXPORT void init_gui(void);

  private:
    //called when toggle information is changed for buffered data
    static void h_on_toggle(GtkToggleButton *but, gpointer data);
};

#endif

