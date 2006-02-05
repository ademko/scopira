
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

#ifndef __INCLUDED__SCOPIRA_COREUI_SPINBUTTON_H__
#define __INCLUDED__SCOPIRA_COREUI_SPINBUTTON_H__

#include <gtk/gtk.h>
#include <scopira/coreui/widget.h>
#include <scopira/coreui/export.h>

namespace scopira
{
  namespace coreui
  {
    class spinbutton_reactor_i;
    class spinbutton;
  }
}

/**
 * listener to spinbutton events
 * @author Aleksander Demko
 */ 
class scopira::coreui::spinbutton_reactor_i
{
  public:
    SCOPIRAUI_EXPORT virtual ~spinbutton_reactor_i() { }
    /**
     * handler of spin button events
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT virtual void react_spinbutton(scopira::coreui::spinbutton *source, int intval, double doubleval) = 0;
};

/**
 * a spin button
 * @author Aleksander Demko
 */
class scopira::coreui::spinbutton : public scopira::coreui::widget
{
  protected:
    double dm_value, dm_min, dm_max, dm_step;
    short dm_numdigits;
    bool dm_wrap;

    /// listener of events, if any
    spinbutton_reactor_i *dm_spinbutton_reactor;

  public:
    /**
     * Create a spinbutton suitable for returning real that are in [0..1].
     *
     * @author Aleksander Demko
     */ 
    SCOPIRAUI_EXPORT spinbutton(void);
    /**
     * Constructor.
     *
     * min and max are the range, where step will the natural increment that happens
     * when pushing the buttons. numdigits is the number of digits displayed after the
     * decimal (ie. 0 == integers). wrap signals that the range should wrap around
     * as the user clicks the buttons.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRAUI_EXPORT spinbutton(double min, double max, double step, short numdigits = 0, bool wrap = false);
  
    /// sets the params
    SCOPIRAUI_EXPORT void set_range(double min, double max);

    /// gets the current value, as int
    SCOPIRAUI_EXPORT int get_value_as_int(void) const;
    /// gets the current value, as double
    SCOPIRAUI_EXPORT double get_value(void) const;
    /// sets the current value, as double
    SCOPIRAUI_EXPORT void set_value(double v);
    
    /// sets the spinbutton reactor
    void set_spinbutton_reactor(spinbutton_reactor_i *reac) { dm_spinbutton_reactor = reac; }

  protected:
    SCOPIRAUI_EXPORT void init_gui(void);

  private:
    /// called when value is chnaged/button is clicked
    static void h_on_change(GtkWidget *widget, gpointer data);
};

#endif

