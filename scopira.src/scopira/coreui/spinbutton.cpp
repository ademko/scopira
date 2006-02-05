
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

#include <scopira/coreui/spinbutton.h>

//BBtargets libscopiraui.so

#include <scopira/tool/util.h>
#include <scopira/tool/reactor.h>

using namespace scopira::tool;
using namespace scopira::coreui;

spinbutton::spinbutton(void)
  : dm_value(0), dm_min(0), dm_max(1), dm_step(0.005), 
  dm_numdigits(3), dm_wrap(false)
{
  dm_spinbutton_reactor = 0;
  init_gui();
}

spinbutton::spinbutton(double min, double max, double step, short numdigits, bool wrap)
  : dm_value(0), dm_min(min), dm_max(max), dm_step(step), 
  dm_numdigits(numdigits), dm_wrap(wrap)
{
  dm_spinbutton_reactor = 0;
  init_gui();
}

void spinbutton::set_range(double min, double max)
{
  gtk_spin_button_set_range(GTK_SPIN_BUTTON(dm_widget), min, max);
}

int spinbutton::get_value_as_int(void) const
{
  return gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(dm_widget));
}

double spinbutton::get_value(void) const
{
  return gtk_spin_button_get_value(GTK_SPIN_BUTTON(dm_widget));
}

void spinbutton::set_value(double v)
{
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(dm_widget), v);
}

void spinbutton::init_gui(void)
{
  GtkObject *adj;

  // make adjustment  
  adj = gtk_adjustment_new(dm_value, dm_min, dm_max, dm_step, 10, 0);
  g_signal_connect(G_OBJECT(adj), "value_changed", G_CALLBACK(h_on_change), this);
  
  //make spinner
  dm_widget = gtk_spin_button_new(GTK_ADJUSTMENT(adj), dm_step, dm_numdigits);
  gtk_spin_button_set_update_policy(GTK_SPIN_BUTTON(dm_widget), GTK_UPDATE_IF_VALID);
  gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(dm_widget), TRUE);
  gtk_widget_show(dm_widget);
   
  gtk_spin_button_set_wrap(GTK_SPIN_BUTTON(dm_widget), dm_wrap);
  
  // call ancestor method
  widget::init_gui();
}

void spinbutton::h_on_change(GtkWidget *widget, gpointer data)
{
  spinbutton &here = *reinterpret_cast<spinbutton*>(data);
  assert(here.is_alive_object());
  
  if (here.dm_spinbutton_reactor)
    here.dm_spinbutton_reactor->react_spinbutton(&here, here.get_value_as_int(), here.get_value());
}

