
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

#include <scopira/coreui/slider.h>

#include <gtk/gtk.h>

#include <scopira/tool/util.h>

using namespace scopira::tool;
using namespace scopira::coreui;

//BBtargets libscopiraui.so

slider::slider(bool vertical)
  : dm_value(0.0), dm_min(0.0), dm_max(1000000.0), dm_step_increment(1.0), 
  dm_page_increment(2.0), dm_page_size(0.0), dm_int(false), dm_vertical(vertical)
{
 	dm_adjustment = 0;
 	dm_slider_reactor = 0;
 	init_gui();
}

// constructor
slider::slider(double min, double max, double step, double page, double page_size, bool vertical)
  : dm_value(0.0), dm_min(min), dm_max(max), dm_step_increment(step), 
  dm_page_increment(page), dm_page_size(page_size), dm_int(false), dm_vertical(vertical)
{ 
 	dm_adjustment = 0;
 	dm_slider_reactor = 0;
 	init_gui();  
}
 
// get position of slider as int
int slider::get_value_as_int(void) const
{
 	return static_cast<int>(dm_adjustment->value);
}

// get position of slider as double
double slider::get_value(void) const
{
  return dm_adjustment->value;
}

void slider::set_value(double v)
{
  gtk_adjustment_set_value(dm_adjustment, v);
}

void slider::set_update_policy(int policy)
{
  if (policy==update_discontinuous)
    gtk_range_set_update_policy(GTK_RANGE(dm_slider), GTK_UPDATE_DISCONTINUOUS);
  else if (policy==update_delayed)
    gtk_range_set_update_policy(GTK_RANGE(dm_slider), GTK_UPDATE_DELAYED);
  else if (policy==update_continuous)
    gtk_range_set_update_policy(GTK_RANGE(dm_slider), GTK_UPDATE_CONTINUOUS);
}

int slider::get_update_policy(void) const
{
  return gtk_range_get_update_policy(GTK_RANGE(dm_slider));
}

// show min and max
void slider::show_min_max(void)
{
	gtk_label_set_text(GTK_LABEL(dm_min_label),double_to_string(dm_min).c_str());	
	gtk_label_set_text(GTK_LABEL(dm_max_label),double_to_string(dm_max).c_str());
}

// set number of decimal places for granularity
void slider::set_digits(int digits)
{
  gtk_scale_set_digits( GTK_SCALE(dm_slider), digits );
}

// create the adjustment and slider
void slider::init_gui(void) 
{     
	// create box
	dm_widget = dm_vertical ? gtk_vbox_new(FALSE,2) : gtk_hbox_new(FALSE,2);
 
  //create min label
  dm_min_label = gtk_label_new("");
  gtk_box_pack_start(GTK_BOX(dm_widget),dm_min_label,FALSE,FALSE,0);
  //gtk_widget_show(dm_min_label);
  
  // create adjustment
  dm_adjustment = GTK_ADJUSTMENT(gtk_adjustment_new(dm_value, dm_min, dm_max, dm_step_increment,
  	dm_page_increment, dm_page_size));
  //make slider
  dm_slider = dm_vertical ? gtk_vscale_new(dm_adjustment) : gtk_hscale_new(dm_adjustment);

  gtk_box_pack_start(GTK_BOX(dm_widget),dm_slider,TRUE,TRUE,0);
  //gtk_widget_show(dm_slider);

  // apply the adjustment stuff
  // i dont think this even works...
  //dm_adjustment->lower = dm_min;
  //dm_adjustment->upper = dm_max;
  //dm_adjustment->step_increment = dm_step_increment;
  //dm_adjustment->page_increment = dm_page_increment;
  //dm_adjustment->page_size = dm_page_size;
  
  // create max label
  dm_max_label = gtk_label_new("");
  gtk_box_pack_start(GTK_BOX(dm_widget),dm_max_label,FALSE,FALSE,0);
  //gtk_widget_show(dm_max_label);
  
  g_signal_connect(G_OBJECT(dm_adjustment), "value_changed", G_CALLBACK(h_on_change), this);

  widget::init_gui();
  //gtk_widget_show(dm_widget);
} 

// called when value of slider is changed
void slider::h_on_change(GtkWidget *widget, gpointer data)
{
  slider *here = reinterpret_cast<slider*>(data);
  assert(here->is_alive_object());
  
  if (here->dm_slider_reactor)
    here->dm_slider_reactor->react_slider(here, here->dm_adjustment->value, static_cast<int>(here->dm_adjustment->value));
}

