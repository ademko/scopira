
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

#include <scopira/coreui/canvas.h>

#include <scopira/tool/flow.h>
#include <scopira/tool/util.h>
#include <scopira/tool/output.h>

#include <gtk/gtk.h>

//BBlibs scopira
//BBtargets libscopiraui.so

using namespace scopira;
using namespace scopira::tool;
using namespace scopira::coreui;

//
// menu_handler_base::popup_menu
//

menu_handler_base::popup_menu::popup_menu(menu_handler_base *base)
{
  dm_builder.set_attach_widget(base);
  base->set_popup_menu(dm_builder.get_gtk_widget());
}

void menu_handler_base::popup_menu::add_item(const std::string &label, intptr_t id)
{
  dm_builder.add_item(label);
  dm_builder.set_callback(GCallback(h_menu), int_to_void(id));
}

void menu_handler_base::popup_menu::add_separator(void)
{
  dm_builder.add_separator();
}

void menu_handler_base::popup_menu::push_menu(const std::string &label)
{
  dm_builder.push_menu(label);
}

void menu_handler_base::popup_menu::pop_menu(void)
{
  dm_builder.pop_menu();
}

void menu_handler_base::popup_menu::popup(void)
{
  dm_builder.popup();
}

//
// menu_handler_base
//

menu_handler_base::menu_handler_base(void)
{
  dm_current_popup_menu = 0;
}

menu_handler_base::~menu_handler_base()
{
  set_popup_menu(0);
}

void menu_handler_base::init_gui(int w, int h)
{
  widget::init_gui(make_drawing_area_impl());
  if (w>0 && h>0)
    gtk_widget_set_usize(dm_widget, w, h);
}

widget * menu_handler_base::make_drawing_area(int w, int h)
{
  GtkWidget *ret = make_drawing_area_impl();
  if (w>0 && h>0)
    gtk_widget_set_usize(ret, w, h);
  return new widget(ret);
}

void menu_handler_base::set_popup_menu(GtkWidget *pop)
{
  if (pop)
    gtk_widget_ref(pop);
  if (dm_current_popup_menu)
    gtk_widget_unref(dm_current_popup_menu);

  dm_current_popup_menu = pop;
}

void menu_handler_base::h_menu(GtkWidget *widget, gpointer dat)
{
  menu_handler_base *base = dynamic_cast<menu_handler_base*>(widget::get_attach_gtk(widget));
  intptr_t id = ptr_to_int(dat);

  assert(base->is_alive_object());
  base->on_menu(id);
}

//
// canvas_base
//

canvas_base::canvas_base(void)
  : dm_drawarea(0), dm_doneinit(false)
{
}

void canvas_base::on_menu(intptr_t id)
{
  widget_canvas c(dm_drawarea);

  handle_menu(c, id);
}

void canvas_base::request_resize(void)
{
  if (!dm_drawarea->window)
    return;

  gtk_widget_queue_resize(dm_drawarea);
}

void canvas_base::request_redraw(void)
{
  if (!dm_drawarea->window)
    return;

  gtk_widget_queue_draw(dm_drawarea);
}

void canvas_base::request_resize(int width, int height)
{
  if (!dm_drawarea->window)
    return;

  gtk_widget_set_size_request(dm_drawarea, width, height);
}

GtkWidget * canvas_base::make_drawing_area_impl(void)
{
  // can call this only once
  assert(!dm_drawarea);

  // create drawing area
  dm_drawarea = gtk_drawing_area_new();

  gtk_widget_set_events(dm_drawarea, GDK_EXPOSURE_MASK | GDK_LEAVE_NOTIFY_MASK | GDK_BUTTON_PRESS_MASK
    | GDK_BUTTON_RELEASE_MASK | GDK_POINTER_MOTION_MASK);

  gtk_widget_set_size_request(dm_drawarea, 10, 10);

  g_signal_connect(G_OBJECT(dm_drawarea), "realize", G_CALLBACK(h_realize), this);
  g_signal_connect(G_OBJECT(dm_drawarea), "configure_event", G_CALLBACK(h_configure), this);
  g_signal_connect(G_OBJECT(dm_drawarea), "expose_event", G_CALLBACK(h_expose), this);
  g_signal_connect(G_OBJECT(dm_drawarea), "button_press_event", G_CALLBACK(h_button), this);
  g_signal_connect(G_OBJECT(dm_drawarea), "button_release_event", G_CALLBACK(h_button), this);
  g_signal_connect(G_OBJECT(dm_drawarea), "motion_notify_event", G_CALLBACK(h_motion), this);  // RV

  return dm_drawarea;
}

void canvas_base::handle_menu(widget_canvas &v, intptr_t menuid)
{
}

void canvas_base::handle_init(widget_canvas &v)
{
}

void canvas_base::handle_resize(widget_canvas &v)
{
}

void canvas_base::handle_repaint(widget_canvas &v)
{
}

void canvas_base::handle_press(widget_canvas &v, const scopira::coreui::mouse_event &mevt)
{
}

void canvas_base::handle_motion(widget_canvas &v, const scopira::coreui::mouse_event &mevt)
{
}


void canvas_base::h_realize(GtkWidget* widget, gpointer data)
{
  canvas_base &here = *reinterpret_cast<canvas_base*>(data); // get pointer to instance

  assert(data);
  assert(here.is_alive_object());

  {
    widget_canvas c(widget);

    here.handle_init(c);
    here.dm_doneinit = true;
    here.handle_resize(c);
  }
}

gint canvas_base::h_expose(GtkWidget* widget, GdkEventExpose* event, gpointer data)
{
  canvas_base &here = *reinterpret_cast<canvas_base*>(data); // get pointer to instance

  assert(data);
  assert(here.is_alive_object());

  /// prepare a canvas object and handle it
  {
    widget_canvas c(widget);

    here.handle_repaint(c);
  }

  return TRUE;
}

gint canvas_base::h_configure(GtkWidget* widget, GdkEventConfigure* event, gpointer data)
{
  canvas_base &here = *reinterpret_cast<canvas_base*>(data); // get pointer to instance

  assert(data);
  assert(here.is_alive_object());

  if (!here.dm_doneinit)
    return TRUE;

  /// prepare a canvas object and handle it
  {
    widget_canvas c(widget);

    here.handle_resize(c);
  }

  return TRUE;
}


gint canvas_base::h_button(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
  canvas_base &here = *reinterpret_cast<canvas_base*>(data); // get pointer to instance

  assert(data);
  assert(here.is_alive_object());

  /// prepare a canvas object and handle it
  {
    widget_canvas c(here.dm_drawarea);
    scopira::coreui::mouse_event ev;

    ev.x = static_cast<int>(event->x);
    ev.y = static_cast<int>(event->y);
    ev.press = event->type == GDK_BUTTON_PRESS || event->type == GDK_2BUTTON_PRESS
      || event->type == GDK_3BUTTON_PRESS;
    ev.double_press = event->type == GDK_2BUTTON_PRESS;
    ev.button = event->button-1;
    
    // shift ,ctr and alt musk
    ev.modifier = 0;
    if (event->state & GDK_SHIFT_MASK)
      ev.modifier |= mouse_event::shift_mod_c;
    else if (event->state & GDK_CONTROL_MASK)
      ev.modifier |= mouse_event::control_mod_c;

    here.handle_press(c, ev);
  }

  return TRUE;
}


// @author Rodrigo Vivanco
gint canvas_base::h_motion(GtkWidget *widget, GdkEventMotion *event, gpointer data)
{
  canvas_base &here = *reinterpret_cast<canvas_base*>(data); // get pointer to instance

  assert(data);
  assert(here.is_alive_object());

  /// prepare a canvas object and handle it
  {
    widget_canvas c(here.dm_drawarea);
    scopira::coreui::mouse_event ev;  // uses coreui mouse event to hold (x,y) and modifier keys

    ev.x = static_cast<int>(event->x);
    ev.y = static_cast<int>(event->y);

    ev.modifier = 0;
    if (event->state & GDK_SHIFT_MASK)
      ev.modifier |= mouse_event::shift_mod_c;
    if (event->state & GDK_CONTROL_MASK)
      ev.modifier |= mouse_event::control_mod_c;

    here.handle_motion(c, ev);
  }

  return TRUE;
}


//
// zoomed_canvas_base
//

void zoomed_canvas_base::handle_zoom(int newfactor)
{
}

GtkWidget * zoomed_canvas_base::make_drawing_area_impl(void)
{
  GtkWidget *ret, *da, *zoomer;

  da = canvas_base::make_drawing_area_impl();
  g_signal_connect(G_OBJECT(dm_zoomer.get_adjustment()), "value_changed", G_CALLBACK(h_value_change), this);
  zoomer = GTK_WIDGET(dm_zoomer.get_widget());

  ret = gtk_vbox_new(FALSE, 2);
  gtk_box_pack_start(GTK_BOX(ret), da, TRUE, TRUE, 2);
  gtk_widget_show(da);
  gtk_box_pack_start(GTK_BOX(ret), zoomer, FALSE, TRUE, 2);
  gtk_widget_show(zoomer);

  return ret;
}

void zoomed_canvas_base::h_value_change(GtkAdjustment *adj, gpointer data)
{
  zoomed_canvas_base &here = *reinterpret_cast<zoomed_canvas_base*>(data); // get pointer to instance

  assert(data);
  assert(here.is_alive_object());

  // (val+10) / 20 * 1000
  here.handle_zoom(static_cast<int>(50 * (adj->value + 10)));
}

//
// scrolled_canvas_base::coord
//

scrolled_canvas_base::coord::coord(scrolled_canvas_base &can)
  : dm_can(can), dm_dirty(false)
{
}

scrolled_canvas_base::coord::~coord()
{
  if (dm_dirty) {
    gtk_adjustment_changed(dm_can.dm_hadj);
    gtk_adjustment_changed(dm_can.dm_vadj);
  }
}

void scrolled_canvas_base::coord::set_hbounds(float lower, float upper, float value,
  float step_inc, float page_inc, float page_size)
{
  dm_can.dm_hadj->lower = lower;
  dm_can.dm_hadj->upper = upper;
  dm_can.dm_hadj->value = value;
  dm_can.dm_hadj->step_increment = step_inc;
  dm_can.dm_hadj->page_increment = page_inc;
  dm_can.dm_hadj->page_size = page_size;
  dm_dirty = true;
}

void scrolled_canvas_base::coord::set_vbounds(float lower, float upper, float value,
  float step_inc, float page_inc, float page_size)
{
  dm_can.dm_vadj->lower = lower;
  dm_can.dm_vadj->upper = upper;
  dm_can.dm_vadj->value = value;
  dm_can.dm_vadj->step_increment = step_inc;
  dm_can.dm_vadj->page_increment = page_inc;
  dm_can.dm_vadj->page_size = page_size;
  dm_dirty = true;
}

void scrolled_canvas_base::coord::set_hvalue(float val)
{
  dm_can.dm_hadj->value = val;
  dm_dirty = true;
}

void scrolled_canvas_base::coord::set_vvalue(float val)
{
  dm_can.dm_vadj->value = val;
  dm_dirty = true;
}

float scrolled_canvas_base::coord::hvalue(void) const
{
  return static_cast<float>(dm_can.dm_hadj->value);
}

float scrolled_canvas_base::coord::vvalue(void) const
{
  return static_cast<float>(dm_can.dm_vadj->value);
}

//
// scrolled_canvas_base
//

scrolled_canvas_base::scrolled_canvas_base(void)
  : dm_doneinit(false)
{
  dm_show_scroll = true;
}

scrolled_canvas_base::~scrolled_canvas_base(void)
{
  // just in case we never included the scroll bars on the canvas
  if (!dm_show_scroll) {
    if (dm_hbar)
      gtk_widget_unref(GTK_WIDGET(dm_hbar));
    if (dm_vbar)
      gtk_widget_unref(GTK_WIDGET(dm_vbar));
  }
}

void scrolled_canvas_base::on_menu(intptr_t id)
{
  widget_canvas c(dm_drawarea);
  coord crd(*this);

  handle_menu(c, crd, id);
}

void scrolled_canvas_base::handle_menu(widget_canvas &v, coord &crd, intptr_t menuid)
{
}

void scrolled_canvas_base::handle_init(widget_canvas &v, coord &crd)
{
}

void scrolled_canvas_base::handle_resize(widget_canvas &v, coord &crd)
{
}


void scrolled_canvas_base::handle_press(widget_canvas &v, coord &crd, const scopira::coreui::mouse_event &mevt)
{
}

void scrolled_canvas_base::handle_motion(widget_canvas &v, coord &crd, const scopira::coreui::mouse_event &mevt)
{
}


void scrolled_canvas_base::handle_scroll(widget_canvas &v, coord &crd)
{
}

void scrolled_canvas_base::request_resize(void)
{
  if (!dm_drawarea->window)
    return;

  gtk_widget_queue_resize(dm_drawarea);
}

void scrolled_canvas_base::request_redraw(void)
{
  if (!dm_drawarea->window)
    return;

  gtk_widget_queue_draw(dm_drawarea);
}

void scrolled_canvas_base::set_show_scroll(bool scroll)
{
  dm_show_scroll = scroll;
}

void scrolled_canvas_base::request_resize(int width, int height)
{
  if (!dm_drawarea->window)
    return;

  gtk_widget_set_size_request(dm_drawarea, width, height);
}


GtkWidget * scrolled_canvas_base::make_drawing_area_impl(void)
{
  GtkWidget *retw;

  retw = gtk_table_new(2, 2, FALSE);
  //retw = gtk_scrolled_window_new(ha, va);
  //retw = gtk_scrolled_window_new(0, 0);

  // create drawing area
  dm_drawarea = gtk_drawing_area_new();
  gtk_table_attach(GTK_TABLE(retw), dm_drawarea, 0, 1, 0, 1,
    static_cast<GtkAttachOptions>(GTK_EXPAND|GTK_FILL),
    static_cast<GtkAttachOptions>(GTK_EXPAND|GTK_FILL), 4, 4);
  gtk_widget_show(dm_drawarea);
  gtk_widget_set_size_request(dm_drawarea, 300, 200);

  // create adjustments
  dm_hadj = GTK_ADJUSTMENT(gtk_adjustment_new(0, 0, 0, 0, 0, 0));
  dm_vadj = GTK_ADJUSTMENT(gtk_adjustment_new(0, 0, 0, 0, 0, 0));
  g_signal_connect(G_OBJECT(dm_hadj), "value_changed", G_CALLBACK(h_value_change), this);
  g_signal_connect(G_OBJECT(dm_vadj), "value_changed", G_CALLBACK(h_value_change), this);

  // create scrollbars
  dm_hbar = GTK_SCROLLBAR(gtk_hscrollbar_new(dm_hadj));
  dm_vbar = GTK_SCROLLBAR(gtk_vscrollbar_new(dm_vadj));
  // do we want the scrollbars to appear on the canvas?
  if (dm_show_scroll) {
    gtk_table_attach(GTK_TABLE(retw), GTK_WIDGET(dm_vbar), 1, 2, 0, 1,
      static_cast<GtkAttachOptions>(0), static_cast<GtkAttachOptions>(GTK_FILL), 2, 2);
    gtk_table_attach(GTK_TABLE(retw), GTK_WIDGET(dm_hbar), 0, 1, 1, 2,
      static_cast<GtkAttachOptions>(GTK_FILL), static_cast<GtkAttachOptions>(0), 2, 2);
    gtk_widget_show(GTK_WIDGET(dm_vbar));
    gtk_widget_show(GTK_WIDGET(dm_hbar));
  } else {
    // if they do not appear, we need to add a ref count to them
    assert(dm_hbar);
    gtk_widget_ref(GTK_WIDGET(dm_hbar));
    assert(dm_vbar);
    gtk_widget_ref(GTK_WIDGET(dm_vbar));
  }

  // set up events
  gtk_widget_set_events(dm_drawarea, GDK_EXPOSURE_MASK | GDK_LEAVE_NOTIFY_MASK | GDK_BUTTON_PRESS_MASK
    | GDK_BUTTON_RELEASE_MASK | GDK_POINTER_MOTION_MASK );

  g_signal_connect(G_OBJECT(dm_drawarea), "realize", G_CALLBACK(h_realize), this);
  g_signal_connect(G_OBJECT(dm_drawarea), "configure_event", G_CALLBACK(h_configure), this);
  g_signal_connect(G_OBJECT(dm_drawarea), "expose_event", G_CALLBACK(h_expose), this);
  g_signal_connect(G_OBJECT(dm_drawarea), "button_press_event", G_CALLBACK(h_button), this);
  g_signal_connect(G_OBJECT(dm_drawarea), "button_release_event", G_CALLBACK(h_button), this);
  g_signal_connect(G_OBJECT(dm_drawarea), "motion_notify_event", G_CALLBACK(h_motion), this);  // RV

  return retw;
}

void scrolled_canvas_base::h_realize(GtkWidget* widget, gpointer data)
{
  scrolled_canvas_base &here = *reinterpret_cast<scrolled_canvas_base*>(data); // get pointer to instance

  assert(data);
  assert(here.is_alive_object());

  {
    widget_canvas c(widget);
    coord crd(here);

    here.handle_init(c, crd);
    here.dm_doneinit = true;
    here.handle_resize(c, crd);
  }
}

gint scrolled_canvas_base::h_expose(GtkWidget* widget, GdkEventExpose* event, gpointer data)
{
  scrolled_canvas_base &here = *reinterpret_cast<scrolled_canvas_base*>(data); // get pointer to instance

  assert(data);
  assert(here.is_alive_object());

  /// prepare a canvas object and handle it
  {
    widget_canvas c(widget);
    coord crd(here);

    here.handle_repaint(c, crd);
  }

  return TRUE;
}

gint scrolled_canvas_base::h_configure(GtkWidget* widget, GdkEventConfigure* event, gpointer data)
{
  scrolled_canvas_base &here = *reinterpret_cast<scrolled_canvas_base*>(data); // get pointer to instance

  assert(data);
  assert(here.is_alive_object());

  if (!here.dm_doneinit)
    return TRUE;

  /// prepare a canvas object and handle it
  {
    widget_canvas c(widget);
    coord crd(here);

    here.handle_resize(c, crd);
  }

  return TRUE;
}

void scrolled_canvas_base::h_value_change(GtkAdjustment *adj, gpointer data)
{
  scrolled_canvas_base &here = *reinterpret_cast<scrolled_canvas_base*>(data); // get pointer to instance

  assert(data);
  assert(here.is_alive_object());

  /// prepare a canvas object and handle it
  {
    widget_canvas c(here.dm_drawarea);
    coord crd(here);

    here.handle_scroll(c, crd);
  }
}

gint scrolled_canvas_base::h_button(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
  scrolled_canvas_base &here = *reinterpret_cast<scrolled_canvas_base*>(data); // get pointer to instance

  assert(data);
  assert(here.is_alive_object());

  /// prepare a canvas object and handle it
  {
    widget_canvas c(here.dm_drawarea);
    coord crd(here);
    scopira::coreui::mouse_event ev;

    ev.x = static_cast<int>(event->x);
    ev.y = static_cast<int>(event->y);
    ev.press = event->type == GDK_BUTTON_PRESS || event->type == GDK_2BUTTON_PRESS
      || event->type == GDK_3BUTTON_PRESS;
    ev.double_press = event->type == GDK_2BUTTON_PRESS;
    ev.button = event->button-1;

    ev.modifier = 0;
    if (event->state & GDK_SHIFT_MASK)
      ev.modifier |= mouse_event::shift_mod_c;
    if (event->state & GDK_CONTROL_MASK)
      ev.modifier |= mouse_event::control_mod_c;

    here.handle_press(c, crd, ev);
  }

  return TRUE;
}


// @author Rodrigo Vivanco
gint scrolled_canvas_base::h_motion(GtkWidget *widget, GdkEventMotion *event, gpointer data)
{
  scrolled_canvas_base &here = *reinterpret_cast<scrolled_canvas_base*>(data); // get pointer to instance

  assert(data);
  assert(here.is_alive_object());

  /// prepare a canvas object and handle it
  {
    widget_canvas c(here.dm_drawarea);
    coord crd(here);
    scopira::coreui::mouse_event ev;  // uses coreui mouse event to hold (x,y) and modifier keys

    ev.x = static_cast<int>(event->x);
    ev.y = static_cast<int>(event->y);

    ev.modifier = 0;
    if (event->state & GDK_SHIFT_MASK)
      ev.modifier |= mouse_event::shift_mod_c;
    if (event->state & GDK_CONTROL_MASK)
      ev.modifier |= mouse_event::control_mod_c;


    here.handle_motion(c, crd, ev);
  }

  return TRUE;
}

///
/// zoomed_scrolled_canvas_base
///

zoomed_scrolled_canvas_base::zoomed_scrolled_canvas_base(void)
{
  dm_doneinit = false;
  dm_show_scroll = true;
  dm_show_zoom = true;
}

void zoomed_scrolled_canvas_base::set_show_zoom(bool zoom)
{
   dm_show_zoom = zoom;
}

void zoomed_scrolled_canvas_base::handle_zoom(int newfactor)
{
}

GtkWidget * zoomed_scrolled_canvas_base::make_drawing_area_impl(void)
{
  GtkWidget *da, *ret, *zoomer;

  // make the scrolled part
  ret = gtk_vbox_new(FALSE, 2);
  da = scrolled_canvas_base::make_drawing_area_impl();
  gtk_box_pack_start(GTK_BOX(ret), da, TRUE, TRUE, 2);
  gtk_widget_show(da);

  // make the zoomer
  zoomer = GTK_WIDGET(dm_zoomer.get_widget());
  g_signal_connect(G_OBJECT(dm_zoomer.get_adjustment()), "value_changed", G_CALLBACK(h_value_change), this);

  // do we want the zoomer to appear on the canvas?
  if (dm_show_zoom) {
    gtk_box_pack_start(GTK_BOX(ret), zoomer, FALSE, TRUE, 2);
    gtk_widget_show(zoomer);
  }

  return ret;
}

void zoomed_scrolled_canvas_base::h_value_change(GtkAdjustment *adj, gpointer data)
{
  zoomed_scrolled_canvas_base &here = *reinterpret_cast<zoomed_scrolled_canvas_base*>(data); // get pointer to instance

  assert(data);
  assert(here.is_alive_object());

  // (val+10) / 20 * 1000
  here.handle_zoom(static_cast<int>(50 * (adj->value + 10)));
}

