
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

#include <scopira/coreui/viewcontainer.h>

#include <scopira/core/model.h>
#include <scopira/coreui/layout.h>

//BBtargets libscopiraui.so

using namespace scopira::tool;
using namespace scopira::core;
using namespace scopira::coreui;

//
//
// viewwindow
//
//

viewwindow::viewwindow(scopira::core::model_i *m, scopira::core::view_i *v)
  : window("View"), dm_model(m), dm_view(v)
{
  assert(m);
  assert(v);

  // finally, make this our core widget
  assert(dynamic_cast<scopira::coreui::widget*>(v));
  assert(dynamic_cast<scopira::coreui::widget*>(v)->is_alive_object());

  dm_viewwidget = dynamic_cast<scopira::coreui::widget*>(v);
  assert(dm_viewwidget.get());
  assert(dm_viewwidget->get_widget() && "[Did you forget to call your own init_gui?]");

  //dm_viewwidget->set_min_size(300, 200);

  window::init_gui(dm_viewwidget->get_widget());

  // make me the event hanlder for this view
  v->set_view_action_reactor(this);

  bind_model_view(m, v);
}

viewwindow::~viewwindow()
{
  dm_view->set_view_action_reactor(this);
}

void viewwindow::react_view_action(scopira::core::view_i *source, int actionID)
{
  if (actionID == button_constants::action_close_c)
    on_destroy();
}

void viewwindow::react_view_set_title(scopira::core::view_i *source, const std::string &newtitle)
{
  set_title(newtitle);
}

//
//
// viewtabs
//
//

viewtabs::viewtabs(void)
{
  // similar to tab_layout, except with more obscure buttons and stuff
  dm_children.reserve(8);
  dm_widget = gtk_notebook_new();
  gtk_notebook_set_scrollable(GTK_NOTEBOOK(dm_widget), TRUE);
  init_gui();
}

viewtabs::~viewtabs()
{
  for (children_t::iterator ii=dm_children.begin(); ii != dm_children.end(); ++ii)
    dynamic_cast<view_i*>(ii->pm_widget.get())->set_view_action_reactor(0);
}

void viewtabs::react_view_action(scopira::core::view_i *source, int actionID)
{
  widget *wsource;

  if (!source)
    return;
  if (actionID != button_constants::action_close_c)
    return;

  // close action
  wsource = dynamic_cast<widget*>(source);
  assert(wsource);

  close_tab(wsource->get_widget());
}

void viewtabs::react_view_set_title(scopira::core::view_i *source, const std::string &newtitle)
{
  widget *wsource;

  if (!source)
    return;
  wsource = dynamic_cast<widget*>(source);
  assert(wsource);

  // find the page entry in our children list, then operate on the associated gtk label
  for (children_t::iterator ii=dm_children.begin(); ii != dm_children.end(); ++ii)
    if (ii->pm_widget.get() == wsource) {
      gtk_label_set_text(GTK_LABEL(ii->pm_label), newtitle.c_str());
      break;
    }
}

void viewtabs::react_view_insert(scopira::core::model_i *m, scopira::core::view_i *v)
{
  count_ptr<widget> w;

  assert(m->is_alive_object());
  assert(v->is_alive_object());

  w = dynamic_cast<widget*>(v);
  assert(w.get());
  assert(w->is_alive_object());

  // add it to the map, layout and bind em
  // use raw gtk stuff
  child_t newchild;
  GtkWidget *box, *detachbut, *closebut;

  detachbut = gtk_button_new();
  gtk_container_add(GTK_CONTAINER(detachbut), gtk_image_new_from_stock(GTK_STOCK_ZOOM_100, GTK_ICON_SIZE_SMALL_TOOLBAR));
  g_signal_connect(G_OBJECT(detachbut), "activate", G_CALLBACK(h_on_detach_tab_button), this);
  g_signal_connect(G_OBJECT(detachbut), "released", G_CALLBACK(h_on_detach_tab_button), this);
  g_object_set_data(G_OBJECT(detachbut), "target_widget", w->get_widget());

  closebut = gtk_button_new();
  gtk_container_add(GTK_CONTAINER(closebut), gtk_image_new_from_stock(GTK_STOCK_CLOSE, GTK_ICON_SIZE_SMALL_TOOLBAR));
  g_signal_connect(G_OBJECT(closebut), "activate", G_CALLBACK(h_on_close_tab_button), this);
  g_signal_connect(G_OBJECT(closebut), "released", G_CALLBACK(h_on_close_tab_button), this);
  g_object_set_data(G_OBJECT(closebut), "target_widget", w->get_widget());

  newchild.pm_widget = w;
  newchild.pm_model = m;
  newchild.pm_view = v;
  newchild.pm_label = gtk_label_new("New");   // careful not to drop this reference
  dm_children.push_back(newchild);

  box = gtk_hbox_new(false, 2);
  gtk_box_pack_start(GTK_BOX(box), newchild.pm_label, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(box), detachbut, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(box), closebut, FALSE, FALSE, 0);
  gtk_widget_show_all(box);

  gtk_notebook_append_page(GTK_NOTEBOOK(dm_widget), w->get_widget(), box);

  v->set_view_action_reactor(this);
  bind_model_view(m, v);
}

void viewtabs::close_tab(GtkWidget *target)
{
  gint idx;

  assert(target);

  // find the page
  idx = gtk_notebook_page_num(GTK_NOTEBOOK(dm_widget), target);
  assert(idx != -1);

  // remove it from the gtk tabber
  gtk_notebook_remove_page(GTK_NOTEBOOK(dm_widget), idx);

  // now, remove it from our data set
  for (children_t::iterator ii=dm_children.begin(); ii != dm_children.end(); ++ii)
    if (ii->pm_widget->get_widget() == target) {
      dm_children.erase(ii);
      break;
    }
}

void viewtabs::h_on_close_tab_button(GtkButton *but, gpointer data)
{
  viewtabs * here = reinterpret_cast<viewtabs*>(data);
  GtkWidget *target = reinterpret_cast<GtkWidget*>(g_object_get_data(G_OBJECT(but), "target_widget"));

  assert(but);
  assert(here);
  assert(here->is_alive_object());
  assert(target);

  // redirect
  here->close_tab(target);
}

void viewtabs::h_on_detach_tab_button(GtkButton *but, gpointer data)
{
  viewtabs * here = reinterpret_cast<viewtabs*>(data);
  GtkWidget *target = reinterpret_cast<GtkWidget*>(g_object_get_data(G_OBJECT(but), "target_widget"));
  child_t cd;
  viewwindow *vw;

  assert(but);
  assert(here);
  assert(here->is_alive_object());
  assert(target);

  // take this widget now and break it away from the tab layout, oy
  for (children_t::iterator ii=here->dm_children.begin(); ii != here->dm_children.end(); ++ii)
    if (ii->pm_widget->get_widget() == target) {
      cd = *ii;
      break;
    }

  assert(cd.pm_widget.get());   // make sure we found something

  // ok, first thing, remove it from the tabber.
  // because we're holding counts in "cd", we'll be ok
  here->close_tab(target);

  // now, open a new viewwindow on the stuff in cd. this might rebind
  // the model and view. so what
  vw = new viewwindow(cd.pm_model.get(), cd.pm_view.get());
  vw->show_all();
}

//
//
// viewcontainer
//
//

viewcontainer::viewcontainer(void)
  : box_layout(false, true), dm_thewidget(0)
{
}

viewcontainer::~viewcontainer()
{
  if (dm_thewidget)
    dynamic_cast<view_i*>(dm_thewidget)->set_view_action_reactor(0);
}

void viewcontainer::react_view_action(scopira::core::view_i *source, int actionID)
{
  if (actionID != button_constants::action_close_c)
    return;

  assert(dm_thewidget);

  source->set_view_action_reactor(0);

  remove_widget(dm_thewidget);

  dm_thewidget = 0;
}

void viewcontainer::react_view_set_title(scopira::core::view_i *source, const std::string &newtitle)
{
  // ignore these, since we don't display the title string
}

void viewcontainer::react_view_insert(scopira::core::model_i *m, scopira::core::view_i *v)
{
  if (dm_thewidget) {
    // remove the old one first
    dynamic_cast<view_i*>(dm_thewidget)->set_view_action_reactor(0);
    remove_widget(dm_thewidget);
    dm_thewidget = 0;
  }

  count_ptr<widget> w;

  assert(m->is_alive_object());
  assert(v->is_alive_object());

  w = dynamic_cast<widget*>(v);
  assert(w.get());

  // add it to the map, layout and bind em
  add_widget(w.get(), true, true);
  dm_thewidget = w.get();

  v->set_view_action_reactor(this);
  bind_model_view(m, v);
}

