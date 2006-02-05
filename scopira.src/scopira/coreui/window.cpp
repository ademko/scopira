
/*
 *  Copyright (c) 2001-2005    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#include <scopira/coreui/window.h>

#include <assert.h>

#include <algorithm>

#include <gtk/gtk.h>

#include <scopira/tool/reactor.h>
#include <scopira/coreui/app.h>

//BBlibs scopira
//BBtargets libscopiraui.so

using namespace scopira::coreui;

//
//
// window
//
//

window::window(const std::string &title, bool modal)
  : dm_title(title), dm_modal(modal), dm_closable(true)
{
}

window::~window()
{
  dvec_t::iterator ii;

  assert(is_alive_object());

  for (ii = dm_destroyreactors.begin(); ii != dm_destroyreactors.end(); ii++)
    (*ii)->react_destroy(this);

  if (dm_widget)
    on_destroy();
}

void window::set_default_size(int w, int h)
{
  gtk_window_set_default_size(GTK_WINDOW(dm_widget), w, h);
}

void window::set_title(const std::string &t)
{
  dm_title = t;
  gtk_window_set_title(GTK_WINDOW(dm_widget), t.c_str());
}

/// adds a new destroy reactor
void window::add_destroy_reactor(scopira::tool::destroy_reactor_i *r)
{
  dm_destroyreactors.push_back(r);
}

/// removes the given destroy reactor
void window::remove_destroy_reactor(scopira::tool::destroy_reactor_i *r)
{
  dvec_t::iterator ii = std::find(dm_destroyreactors.begin(), dm_destroyreactors.end(), r);
  assert(ii != dm_destroyreactors.end());
  dm_destroyreactors.erase(ii);
}

/// adds a new close reactor
void window::add_close_reactor(close_reactor_i *r)
{
  dm_closereactors.push_back(r);
}

/// removes the given close reactor
void window::remove_close_reactor(close_reactor_i *r)
{
  cvec_t::iterator ii = std::find(dm_closereactors.begin(), dm_closereactors.end(), r);
  assert(ii != dm_closereactors.end());
  dm_closereactors.erase(ii);
}

// Called when received change focus
void window::on_focus(bool focus)
{
  widget::on_focus(focus);
}

// Called on winodw closing
void window::on_close(void)
{
  cvec_t::iterator ii;

  //set window to destroy
  set_closable(true);

  assert(is_alive_object());
  for (ii = dm_closereactors.begin(); ii != dm_closereactors.end(); ii++) {
    (*ii)->react_close(this);
    assert(is_alive_object());
  }
}


// Called when received a destroy event. default action is to sub_ref self
// overiding methods should NOT call this version. this is only called
// when an event is originating from the gtk side, not if the destructor
// is deleteing the gtk object
void window::on_destroy(void)
{
  if (dm_widget) {
    GtkWidget *w = dm_widget;
    assert(w);
    assert(GTK_OBJECT(w));
    dm_widget = 0;

    gtk_object_destroy(GTK_OBJECT(w));
    gtk_widget_unref(w);
  }

  // remember, if there is no appwin, sub ref self?
  if (app_i::instance())
    app_i::instance()->remove_window(this);
}

// Init the gui. decendant classes must overide this method, set
// dm_widget, then call this version at their end
void window::init_gui(void)
{
  assert(dm_widget);

  if (dm_modal)
    gtk_window_set_modal(GTK_WINDOW(dm_widget), true);
  gtk_window_set_position(GTK_WINDOW(dm_widget), GTK_WIN_POS_CENTER_ON_PARENT);

  //destroy handler
  g_signal_connect(G_OBJECT(dm_widget), "destroy", G_CALLBACK(h_destroy), this);
  //delete_event handler
  g_signal_connect(G_OBJECT(dm_widget), "delete_event", G_CALLBACK(h_delete_event), this);

  //add self to the list of windows in the app window
  if (app_i::instance())
    app_i::instance()->add_window(this);

  gtk_window_set_title(GTK_WINDOW(dm_widget), dm_title.c_str());

  widget::init_gui();
}

void window::init_gui(GtkWidget *widget, int flags)
{
  assert(widget);
  assert(dm_widget == 0);

  dm_widget = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  gtk_container_add(GTK_CONTAINER(dm_widget), widget);
  window::init_gui();
}

// Static handler, distroy a gtk widget
void window::h_destroy(GtkWidget *widget, gpointer data)
{
  assert(data);
  window &here = *reinterpret_cast<window*>(data);

  if (!here.dm_widget)
    return;   // we've done this before, bail now

  // else, call the virtual handler
  here.dm_widget = 0;   // set to 0 to prevent an infinite loop
  here.on_destroy();
}

// Static handler, delete_event
gint window::h_delete_event(GtkWidget *widget, GdkEvent *event, gpointer data)
{
  window &here = *reinterpret_cast<window*>(data);
  assert(data);

  assert(here.is_alive_object());
  //perform on close operation
  here.on_close();
  //return  true/false for the window to be destroyed
  assert(here.is_alive_object());
  return !here.is_closable();
}

//
//
// dialog
//
//

dialog::dialog(const std::string &title, bool  modal)
  : window(title, modal)
{
  init_gui();
}

///base building constructor
dialog::dialog(bool modal)
  : window("", modal)
{
}

// Destuctor
dialog::~dialog()
{
}

// Create dialog label
GtkWidget * dialog::init_label(const std::string &text)
{
  GtkWidget *label;

  // create the label
  label = gtk_label_new(text.c_str());
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dm_widget)->vbox),label, TRUE, TRUE, 0);

  gtk_widget_show(label);

  return label;
}


// Create an ok button
GtkWidget * dialog::init_ok_button(const std::string &label, bool defhandler)
{
  GtkWidget *but;

  if (label.empty() || label == "OK")
    but = gtk_button_new_from_stock(GTK_STOCK_OK);
  else
    but = gtk_button_new_with_label(label.c_str());
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dm_widget)->action_area), but, TRUE, TRUE, 10);
  if (defhandler) {
    g_signal_connect(G_OBJECT(but), "clicked", G_CALLBACK(h_on_ok), this);
    GTK_WIDGET_SET_FLAGS(but, GTK_CAN_DEFAULT);
    gtk_widget_grab_default(but);
  }

  gtk_widget_show(but);

  return but;
}


// Create a cancel button
GtkWidget * dialog::init_cancel_button(const std::string &label, bool defhandler)
{
  GtkWidget *but;

  if (label.empty() || label == "Cancel")
    but = gtk_button_new_from_stock(GTK_STOCK_CANCEL);
  else if (label == "Close")
    but = gtk_button_new_from_stock(GTK_STOCK_CLOSE);
  else
    but = gtk_button_new_with_label(label.c_str());
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dm_widget)->action_area), but, TRUE, TRUE, 10);
  if (defhandler)
    g_signal_connect(G_OBJECT(but), "clicked", G_CALLBACK(h_on_cancel), this);

  gtk_widget_show(but);

  return but;
}


//Create a text entry box
GtkWidget * dialog::init_text_entry(const std::string &label, const std::string &def, bool defhandler)
{
  GtkWidget *lab, *entry;

  // add label
  lab = gtk_label_new("Enter a new name for the module:");

  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dm_widget)->vbox), lab, FALSE, FALSE, 0);

  gtk_widget_show(lab);

  // make the text entery box
  entry = gtk_entry_new();
  gtk_entry_set_text(GTK_ENTRY(entry), def.c_str());

  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dm_widget)->vbox), entry, FALSE, TRUE, 0);

  gtk_widget_show(entry);

  return entry;
}

/// gets the vbox area
GtkWidget * dialog::get_window_area(void)
{
  return GTK_DIALOG(dm_widget)->vbox;
}

/// gets the action area
GtkWidget * dialog::get_button_area(void)
{
  return GTK_DIALOG(dm_widget)->action_area;
}

void dialog::init_window_area(GtkWidget *w)
{
  assert(w);
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dm_widget)->vbox), w, TRUE, TRUE, 10);
}

// OK button handler
void dialog::on_ok(void)
{
  //distroy dialog
  on_destroy();
}

// Cancel button handler
void dialog::on_cancel(void)
{
  //distroy dialog
  on_destroy();
}

// Initialize dialog widget
void dialog::init_gui(void)
{
  //dialog widget
  dm_widget = gtk_dialog_new();

  gtk_window_set_position(GTK_WINDOW(dm_widget),GTK_WIN_POS_MOUSE);
  gtk_window_set_title(GTK_WINDOW(dm_widget),get_title().c_str() );

  window::init_gui();
}

// Static ok button handler
void dialog::h_on_ok(GtkWidget *widget, gpointer data)
{
  assert(data);
  dialog &here = *reinterpret_cast<dialog*>(data);

  assert( here.is_alive_object() );
  here.on_ok();
}

// Static cancel button handler
void dialog::h_on_cancel(GtkWidget *widget, gpointer data)
{
  assert(data);
  dialog &here = *reinterpret_cast<dialog*>(data);

  assert(here.is_alive_object());
  here.on_cancel();
}

