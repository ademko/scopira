
/*
 *  Copyright (c) 2003    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#include <scopira/coreui/widget.h>

#include <assert.h>

#include <gtk/gtk.h>

using namespace scopira::coreui;

widget::widget(GtkWidget *w)
  : dm_widget(w), dm_isfocus(false)
{
  // in lue of a full init_gui call...
  assert(dm_widget);
  gtk_widget_ref(dm_widget);
}

widget::widget(void)
  : dm_widget(0), dm_isfocus(false)
{
}

widget::~widget()
{ 
  if (dm_widget)
    gtk_widget_unref(dm_widget);
}

void widget::set_attach_gtk(GtkWidget *w)
{
  assert(w);
  g_object_set_data(G_OBJECT(w), "sco_att_g", static_cast<widget*>(this));                                             
}

widget * widget::get_attach_gtk(GtkWidget *w)
{
  assert(w);
  return reinterpret_cast<widget*>(g_object_get_data(G_OBJECT(w), "sco_att_g"));
}

void widget::show_all(void)
{
  gtk_widget_show_all(dm_widget);
}

void widget::hide(void)
{
  gtk_widget_hide(dm_widget);
}

void widget::set_min_size(int w, int h)
{
 // gtk_widget_set_usize(dm_widget, w, h); //deprecated
 gtk_widget_set_size_request(dm_widget, w, h);
}

void widget::set_border_size(int px)
{
  assert(GTK_IS_CONTAINER(dm_widget));
  gtk_container_set_border_width(GTK_CONTAINER(dm_widget), px);
}

void widget::set_cursor(GdkCursorType type)
{
  if (dm_widget->window) {
    gdk_window_set_cursor(dm_widget->window, gdk_cursor_new(type));
    gdk_flush();    // i want this to show up IMMEDIATLY
  }
}

void widget::set_attach_ptr(GtkWidget *w, void *item)
{
  // null item is allowed.
  assert(w);
  g_object_set_data(G_OBJECT(w), "sco_att_p", item);
}

void * widget::get_attach_ptr(GtkWidget *w)
{
  assert(w);
  return reinterpret_cast<void*>(g_object_get_data(G_OBJECT(w), "sco_att_p"));
}

void widget::on_focus(bool focus)
{  
  if (dm_isfocus != focus) {
    dm_isfocus = focus;
    
    if (focus) {
      if (GDK_WINDOW(dm_widget->window)) {
        gdk_window_show(dm_widget->window);
        gdk_window_focus(dm_widget->window, gtk_get_current_event_time ());
      }
      else {
        gtk_widget_show(dm_widget);
      }
    }
  }
}

// Init the gui. decendant classes must overide this method, set
// dm_widget, then call this version at their end
void widget::init_gui()
{
  assert(dm_widget);

  gtk_widget_ref(dm_widget);
  
  // window focus in handler
  g_signal_connect_after(G_OBJECT(dm_widget),"focus_in_event",G_CALLBACK(h_focus_in), this);
  // window focus in handler
  g_signal_connect_after(G_OBJECT(dm_widget),"focus_out_event",G_CALLBACK(h_focus_out), this);      
}

void widget::init_gui(GtkWidget *basewidget)
{
  assert(basewidget);

  dm_widget = basewidget;

  widget::init_gui();
}

void widget::init_gui(widget *basewidget)
{
  assert(basewidget);

  dm_count_widget = basewidget;
  dm_widget = basewidget->get_widget();

  widget::init_gui();
}

// Static widget focus in handler
void widget::h_focus_in(GtkWidget *w, GdkEventFocus* event, gpointer data)
{
  widget &here = *reinterpret_cast<widget*>(data);
  assert(data);

  if (!here.dm_isfocus) {
    here.dm_isfocus = true;
    here.on_focus(true); //handle focus in
  }
}

// Static widget focus out handler
void widget::h_focus_out(GtkWidget *w, GdkEventFocus* event, gpointer data)
{
  widget &here = *reinterpret_cast<widget*>(data);
  assert(data);

  if (here.dm_isfocus) {
    here.dm_isfocus = false;
    here.on_focus(false); //handle focus out
  }
}

//BBlibs scopira gtk+-2.0 gthread-2.0
//BBtargets libscopiraui.so

