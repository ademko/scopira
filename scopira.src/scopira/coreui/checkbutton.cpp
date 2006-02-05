

/*
 *  Copyright (c) 2002    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#include <scopira/coreui/checkbutton.h>

#include <assert.h>

#include <gtk/gtklabel.h>
#include <gtk/gtkcheckbutton.h>

#include <scopira/tool/reactor.h>

//BBlibs scopira
//BBtargets libscopiraui.so

using namespace scopira;
using namespace scopira::coreui;

checkbutton::checkbutton(void)
{
  dm_checkbutton_reactor = 0;
  init_gui(0);
}

checkbutton::checkbutton(const std::string &label, bool is_checked)
{
  dm_checkbutton_reactor = 0;
  init_gui(label.c_str());
  set_checked(is_checked);
}

void checkbutton::set_label(const std::string &label)
{
  gtk_label_set_text(GTK_LABEL(GTK_BIN(dm_widget)->child), label.c_str());
}

void checkbutton::set_checked(bool check)
{
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(dm_widget), check);
}

bool checkbutton::is_checked(void) const
{
  return gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(dm_widget)) != 0;
}

void checkbutton::init_gui(const char *label)
{
  // make my label
  dm_widget = gtk_check_button_new_with_label((label ? label : "label"));
  g_signal_connect(G_OBJECT(dm_widget), "toggled", G_CALLBACK(h_on_toggle), this);
  
  // call ancestor method
  widget::init_gui();
}

void checkbutton::h_on_toggle(GtkToggleButton *but, gpointer data)
{
  checkbutton &here = *reinterpret_cast<checkbutton*>(data);

  assert(here.is_alive_object());

  if (here.dm_checkbutton_reactor)
  	here.dm_checkbutton_reactor->react_checkbutton(&here, here.is_checked());
}

