
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

#ifndef __INCLUDED__SCOPIRA_COREUI_SLIDER_H__
#define __INCLUDED__SCOPIRA_COREUI_SLIDER_H__

#include <scopira/coreui/widget.h>
#include <scopira/coreui/export.h>

namespace scopira
{
  namespace coreui
  {
    class slider_reactor_i;
    class slider;
  }
}

/**
 * listener to slider events, when the slider changes
 * @author Aleksander Demko
 */
class scopira::coreui::slider_reactor_i
{
  public:
    virtual ~slider_reactor_i() { }
    SCOPIRAUI_EXPORT virtual void react_slider(scopira::coreui::slider *source, double doubleval, int intval) = 0;
};

/**
 * Slider proponent
 * 
 * @author Shantha Ramachandran
 */
class scopira::coreui::slider : public scopira::coreui::widget
{
  public:
    // update policy types
    enum {
      update_continuous,
      update_discontinuous,
      update_delayed
    };

	protected:
    GtkAdjustment *dm_adjustment;
    double dm_value, dm_min, dm_max, dm_step_increment, dm_page_increment, dm_page_size;    
    int dm_digits, dm_draw_pos;
    bool dm_int, dm_vertical, dm_draw_label, dm_draw_min_max;
    GtkWidget *dm_min_label, *dm_max_label, *dm_slider;

    /// listener of events, if any
    scopira::coreui::slider_reactor_i *dm_slider_reactor;
    
  public:
    /// construcutor
    SCOPIRAUI_EXPORT slider(bool vertical);
    /// constrcutor
    SCOPIRAUI_EXPORT slider(double min, double max, double step, double page, double page_size, bool vertical);
    
    /// gets the current integer value
    SCOPIRAUI_EXPORT int get_value_as_int(void) const;
    /// gets the current double value
    SCOPIRAUI_EXPORT double get_value(void) const;
    /// sets the current value, as double
    SCOPIRAUI_EXPORT void set_value(double v);

    /// sets the update policy
    SCOPIRAUI_EXPORT void set_update_policy(int policy);
    /// gets the update policy
    SCOPIRAUI_EXPORT int get_update_policy(void) const;

    /// caled when its handler setting time
    void set_slider_reactor(scopira::coreui::slider_reactor_i *reac) { dm_slider_reactor = reac; }
    /// shows min and max values
    SCOPIRAUI_EXPORT void show_min_max(void);
    /// set number of decimal places for granularity
    SCOPIRAUI_EXPORT void set_digits(int digits);
    
  protected:
    SCOPIRAUI_EXPORT void init_gui(void);

  private:
    /// called when value is chnaged/button is clicked
    static void h_on_change(GtkWidget *widget, gpointer data);
};

#endif

