
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

#include <scopira/coreui/entry.h>

#include <assert.h>

#include <gtk/gtkentry.h>

#include <scopira/tool/util.h>
#include <scopira/tool/output.h>

//BBlibs scopira
//BBtargets libscopiraui.so

using std::string;
using scopira::coreui::widget;
using namespace scopira;

coreui::entry::entry(void)
{
  dm_widget = 0;
  
  dm_oint = false;
  dm_odouble = false;
  dm_onospace = false;
  dm_opassword = false;
  dm_hasmin = false;
  dm_hasmax = false;
  dm_hasminlen = false;
  dm_hasmaxlen = false;
  dm_editable = true;

  dm_reactor = 0;

  init_gui();
}

std::string coreui::entry::get_text(void) const
{
  return gtk_entry_get_text(GTK_ENTRY(dm_widget));
}

void coreui::entry::get_text(std::string &out)
{
  out = gtk_entry_get_text(GTK_ENTRY(dm_widget));
}

void coreui::entry::set_text(const std::string &t)
{
  gtk_entry_set_text(GTK_ENTRY(dm_widget), t.c_str());
}

void coreui::entry::set_text(const char *t)
{
  gtk_entry_set_text(GTK_ENTRY(dm_widget), t?t:"");
}

void coreui::entry::select_all(void)
{
  gtk_editable_select_region(GTK_EDITABLE(dm_widget), 0, -1);
}

void coreui::entry::set_max_length(int max)
{
	gtk_entry_set_width_chars(GTK_ENTRY(dm_widget),max);
}

void coreui::entry::select_none(void)
{
  gtk_editable_select_region(GTK_EDITABLE(dm_widget), 0, 0);
}

void coreui::entry::init_gui(void)
{
  // make my entry
  dm_widget = gtk_entry_new();

  // attach my listener to it, if need be
  g_signal_connect(G_OBJECT(dm_widget), "activate", G_CALLBACK(h_on_activate), this);

  // call ancestor method
  widget::init_gui();
}

void coreui::entry::h_on_activate(GtkEntry *e, gpointer data)
{
  entry *here = reinterpret_cast<entry*>(data);

  assert(here->is_alive_object());

  if (here->dm_reactor)
    here->dm_reactor->react_entry(here, gtk_entry_get_text(GTK_ENTRY(here->dm_widget)) );
}
