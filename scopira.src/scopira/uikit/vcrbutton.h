
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

#ifndef __INCLUDED__SCOPIRA_UIKIT_VCRBUTTON_H__
#define __INCLUDED__SCOPIRA_UIKIT_VCRBUTTON_H__

#include <string>

#include <scopira/coreui/widget.h>
#include <scopira/coreui/export.h>

namespace scopira
{
  namespace uikit
  {
    class vcrbutton_reactor_i;
    class vcrbutton;
  }
}

/**
 * listern of vcr events
 * @author Aleksander Demko
 */
class scopira::uikit::vcrbutton_reactor_i
{
  public:
    SCOPIRAUI_EXPORT virtual ~vcrbutton_reactor_i() { }
    /**
     * Handler of vcr change events
     * @param val the current value of the vcr control
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT virtual void react_vcrbutton(scopira::uikit::vcrbutton *source, int val) = 0;
};

/**
 * a VCR like toolbar
 *
 * @author Marina Mandelzweig
 */
class scopira::uikit::vcrbutton : public scopira::coreui::widget
{
  protected:
    /// static menu commands
    enum {
      cmd_go_first = 0,
      cmd_go_back,
      cmd_go_next,
      cmd_go_last
    };
    
    /// current, max  and min values
    int dm_value, dm_min, dm_max;
    /// the VCR like buttons
    GtkWidget *dm_first_button, *dm_next_button, *dm_back_button, *dm_last_button;
    GtkWidget *dm_label;
    GtkWidget *dm_entry;
    GtkWidget *dm_entry_box;
    
    /// does it have the first and last buttons
    bool dm_have_first, dm_have_last;
    /// should it wrap result
    bool dm_wrap;
    /// should we display the max value in the entry box (no, by default)
    bool dm_show_max;
    /// listener of events, if any
    scopira::uikit::vcrbutton_reactor_i *dm_vcr_reactor;
     
  public:
    /// constrcutor
    SCOPIRAUI_EXPORT vcrbutton(void);
    
    /// caled when its handler setting time
    SCOPIRAUI_EXPORT void set_vcrbutton_reactor(scopira::uikit::vcrbutton_reactor_i *reac); 
    /// update the VCR buttons
    SCOPIRAUI_EXPORT void update_buttons(bool disable_all);
    
    SCOPIRAUI_EXPORT void set_min(int v);
    SCOPIRAUI_EXPORT void set_max(int v);
    SCOPIRAUI_EXPORT void set_label(const std::string &label);
    SCOPIRAUI_EXPORT void set_first(bool hasfirst);
    SCOPIRAUI_EXPORT void set_last(bool haslast);
    SCOPIRAUI_EXPORT void set_wrap(bool wrapit);
    SCOPIRAUI_EXPORT void set_showmax(bool smax);
    SCOPIRAUI_EXPORT void set_value(int v);
    SCOPIRAUI_EXPORT void set_entry(void);

    //get current value
    int get_value(void) const { return dm_value; }
    
  protected:
    /// update button and adjustment signals
    SCOPIRAUI_EXPORT void update_signals(void);
    
    SCOPIRAUI_EXPORT void init_gui(void);
    
    // update the info in the entry box
    SCOPIRAUI_EXPORT void update_entry(bool disable_all);
    
  private:
    /// called when any of the VCR buttons are clicked
    static void h_on_change(GtkWidget *widget, int cmd);
};

#endif

