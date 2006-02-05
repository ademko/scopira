
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

#include <scopira/coreui/radiobutton.h>

#include <scopira/tool/util.h>

//BBlibs scopira
//BBtargets libscopiraui.so

using namespace scopira::tool;
using namespace scopira::coreui;

radiobutton::radiobutton(void)
{
  dm_radiobutton_reactor = 0;
  init_gui();
}

void radiobutton::add_selection(int id, const std::string &desc)
{
  GtkWidget *rad;

  // make the button
  if (dm_firstbutton)
    rad = gtk_radio_button_new_with_label(
      gtk_radio_button_get_group(GTK_RADIO_BUTTON(dm_firstbutton)), desc.c_str());
  else {
    rad = gtk_radio_button_new_with_label(0, desc.c_str());
    dm_firstbutton = rad;
  }

  // record it
  dm_idrad[id] = rad;

  g_signal_connect(G_OBJECT(rad), "toggled", G_CALLBACK(h_on_toggle), this);

  // add it to the vbox
  gtk_box_pack_start(GTK_BOX(dm_widget), rad, FALSE, FALSE, 0);
  gtk_widget_show(rad);
}

// which of the radio buttons to select
// @author Rodrigo Vivanco
void radiobutton::set_selection( int id_num )
{
  // copy/paste from set_data()
  idmap_t::iterator ii; 
  
  ii = dm_idrad.find( id_num );
  if (ii != dm_idrad.end()) {
    assert((*ii).second);

    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON((*ii).second), true);
  } // doesnt contain, just ignore?

}

// which of the radio buttons is selected
// @author Rodrigo Vivanco
int radiobutton::get_selection(void) const
{
  // copy/paste from get_data() with some changes to variable names... like what the heck does "io" mean in this context ?!?!
  int idx;
  idmap_t::const_iterator ii;

  // go find the pressed button
  for (ii = dm_idrad.begin(); ii != dm_idrad.end(); ++ii) 
  {
    idx = (*ii).first;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON((*ii).second))) {
      // found it
      return idx;
    }
  }

  // didnt find it???
  assert(false);
  return -1;
}


void radiobutton::init_gui(void)
{
  dm_firstbutton = 0;

  // make container
  dm_widget = gtk_vbox_new(TRUE, 0);

  // call ancestor method
  widget::init_gui();
}

void radiobutton::h_on_toggle(GtkToggleButton *but, gpointer data)
{
  radiobutton *here = reinterpret_cast<radiobutton*>(data);

  if (!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(but)))
    return; // i only want to listen to button ON events

  assert(here->is_alive_object());

  if (here->dm_radiobutton_reactor)
    here->dm_radiobutton_reactor->react_radiobutton(here, here->get_selection());
}

