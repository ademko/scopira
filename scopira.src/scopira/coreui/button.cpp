
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

#include <scopira/coreui/button.h>

#include <assert.h>

#include <gtk/gtklabel.h>
#include <gtk/gtkbutton.h>

#include <scopira/coreui/image.h>
#include <scopira/coreui/layout.h>
#include <scopira/coreui/label.h>

//BBlibs scopira
//BBtargets libscopiraui.so

using namespace scopira::coreui;

button::button(const std::string &label, int actionid, int actionid2)
{
  dm_actionid = actionid;
  dm_actionid2 = actionid2;

  dm_button_reactor = 0;

  dm_widget = gtk_button_new_with_label(label.c_str());
  init_gui();
}

button::button(scopira::coreui::widget *w, int actionid, int actionid2)
  : dm_sublabel(w)
{
  assert(w);

  dm_actionid = actionid;
  dm_actionid2 = actionid2;

  dm_button_reactor = 0;

  dm_widget = gtk_button_new();
  gtk_container_add(GTK_CONTAINER(dm_widget), w->get_widget());
  init_gui();
}

button::button(stock_t dummy, const char *stock_id, int actionid, int actionid2)
{
  assert(dummy == stock_c);

  assert(stock_id);

  dm_actionid = actionid;
  dm_actionid2 = actionid2;

  dm_button_reactor = 0;

  dm_widget = gtk_button_new_from_stock(stock_id);
  init_gui();
}

widget * button::new_image_label(scopira::coreui::imagewidget *iw, const std::string &string)
{
  box_layout *ret;

  ret = new box_layout(true, false);

  assert(iw);
  ret->add_widget(iw, false, false);
  ret->add_widget(new label(string), false, false, 4);

  return ret;
}

void button::init_gui(void)
{
  g_signal_connect(G_OBJECT(dm_widget), "activate", G_CALLBACK(h_on_activate), this);
  g_signal_connect(G_OBJECT(dm_widget), "released", G_CALLBACK(h_on_activate), this);

  // call ancestor method
  widget::init_gui();
}

void button::set_label(const std::string &label)
{
  gtk_button_set_label(GTK_BUTTON(dm_widget), label.c_str());
}

void button::h_on_activate(GtkButton *but, gpointer data)
{
  button &here = *reinterpret_cast<button*>(data);

  assert(here.is_alive_object());

  if (here.dm_button_reactor) {
    here.dm_flushed_actions = false;
    here.dm_button_reactor->react_button(&here, here.dm_actionid);
    if (here.dm_actionid2 != 0 && !here.dm_flushed_actions)
      here.dm_button_reactor->react_button(&here, here.dm_actionid2);
  }
}

