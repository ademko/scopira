
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

#ifndef __INCLUDED__SCOPIRA_COREUI_DROPDOWN_H__
#define __INCLUDED__SCOPIRA_COREUI_DROPDOWN_H__

#include <map>
#include <string>

#include <gtk/gtk.h>

#include <scopira/coreui/widget.h>
#include <scopira/coreui/export.h>

namespace scopira
{
  namespace coreui
  {
    class dropdown_reactor_i;
    class dropdown;
  }
}

/**
 * listener to dropdown events, when the dropdown changes
 * @author Aleksander Demko
 */
class scopira::coreui::dropdown_reactor_i
{
  public:
    SCOPIRAUI_EXPORT virtual ~dropdown_reactor_i() { }
    SCOPIRAUI_EXPORT virtual void react_dropdown(scopira::coreui::dropdown *source, int selection) = 0;
};

/**
 * A drop down box
 *
 * @author Aleksander Demko
 * @author Marina Mandelzweig
 */
class scopira::coreui::dropdown : public scopira::coreui::widget
{
  protected:
    #if GTK_MAJOR_VERSION <= 2 && GTK_MINOR_VERSION < 4
      typedef std::map<int, GtkWidget*> idmap_t;
      idmap_t dm_idrad;      // maps ids (intobj) to rows
    #endif  
    #if GTK_MAJOR_VERSION >= 2 && GTK_MINOR_VERSION >= 4
      typedef std::map<int, int> idmap_t; //isd, row
      idmap_t dm_idrad;      // maps ids (intobj) to rows 
    #endif

    /// listener of events, if any
    scopira::coreui::dropdown_reactor_i *dm_dropdown_reactor;
    
    int dm_signalid;
    
  public:
    /// construcutor
    SCOPIRAUI_EXPORT dropdown(void);
    
    /// caled when its handler setting time
    void set_dropdown_reactor(scopira::coreui::dropdown_reactor_i *reac) { dm_dropdown_reactor = reac; }

    /// clears all the current selections
    SCOPIRAUI_EXPORT void clear_selections(void);
    /// adds a selection
    SCOPIRAUI_EXPORT void add_selection(int id, const std::string &desc);

    // returns current entry value, method to be used with new non-kernel-module proponent-proponent interactions
    // @author Rodrigo Vivanco
    SCOPIRAUI_EXPORT int get_selection(void) const;

    // if given idx within bounds, sets that entry as current selection
    // @author Rodrigo Vivanco
    SCOPIRAUI_EXPORT void set_selection(int sel_idx);

  protected:
    SCOPIRAUI_EXPORT void init_gui(void);
    SCOPIRAUI_EXPORT void update_gui(void);
    
  private:
    //called value is chnaged/button is clicked
    static void h_on_change(GtkEditable *entry, gpointer data);
};

#endif

