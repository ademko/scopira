
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

#include <scopira/uikit/vcrbutton.h>

#include <gtk/gtk.h>

#include <scopira/tool/util.h>

//BBtargets libscopiraui.so

using namespace scopira::tool;
using namespace scopira::coreui;
using namespace scopira::uikit;

vcrbutton::vcrbutton(void)
  :dm_value(0), dm_min(0), dm_max(100), 
  dm_first_button(0), dm_next_button(0), dm_back_button(0), dm_last_button(0),
  dm_label(0),dm_entry(0), dm_have_first(true), dm_have_last(true), 
  dm_wrap(false), dm_show_max(false)
{
  dm_vcr_reactor = 0;
  init_gui();
}

void vcrbutton::update_entry(bool disable_all)
{
  std::string str;
  
  if (dm_entry) {
    if (disable_all || (dm_value == 0 && dm_max == 0)) {
      gtk_entry_set_text(GTK_ENTRY(dm_entry), "");
      gtk_widget_set_sensitive(dm_entry, FALSE);
    }
    else {
      gtk_widget_set_sensitive(dm_entry, TRUE);
      
      str = int_to_string(dm_value);
      if (dm_show_max)
        str+=" of "+ tool::int_to_string(dm_max);
      gtk_entry_set_text(GTK_ENTRY(dm_entry), str.c_str());
    }
  } 
}

void vcrbutton::init_gui(void)
{
  GtkWidget *hbox, *start_toolbar, *start_frame, *end_frame, *end_toolbar;
  
  // make container
  dm_widget = gtk_hbox_new(FALSE, 2);
  
  // make label
  dm_label = gtk_label_new("");
  gtk_box_pack_start(GTK_BOX(dm_widget), dm_label, FALSE, FALSE, 2);
  gtk_widget_show(dm_label);
  
  hbox =  gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start(GTK_BOX(dm_widget), hbox, FALSE, FALSE, 0);
  gtk_widget_show(dm_label);
  
  //make frame
  start_frame = gtk_frame_new(NULL);
	gtk_frame_set_shadow_type(GTK_FRAME(start_frame), GTK_SHADOW_ETCHED_IN);
  gtk_box_pack_start(GTK_BOX(hbox), start_frame, FALSE, TRUE, 0);
  gtk_widget_show(start_frame);
  
  //create start toolbar
  start_toolbar = gtk_toolbar_new();
  gtk_toolbar_set_style(GTK_TOOLBAR(start_toolbar),GTK_TOOLBAR_ICONS);
  gtk_toolbar_set_icon_size(GTK_TOOLBAR(start_toolbar),GTK_ICON_SIZE_SMALL_TOOLBAR);
  gtk_container_add(GTK_CONTAINER(start_frame),start_toolbar);
  gtk_widget_show(start_toolbar);
  
  if (dm_have_first) {
    //go to start button
    dm_first_button = gtk_toolbar_insert_stock(GTK_TOOLBAR(start_toolbar),GTK_STOCK_GOTO_FIRST,"Go First",
      NULL,NULL,NULL,-1);
    set_attach_gtk(dm_first_button);
    gtk_widget_show(dm_first_button);
  }

  //go back button
  dm_back_button = gtk_toolbar_insert_stock(GTK_TOOLBAR(start_toolbar),GTK_STOCK_GO_BACK,"Go Back",
    NULL,NULL,NULL,-1);
  set_attach_gtk(dm_back_button);
  gtk_widget_show(dm_back_button);
  
  // entry container
  dm_entry_box = gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start(GTK_BOX(hbox), dm_entry_box, TRUE, TRUE, 0);
  
  //make frame
  end_frame = gtk_frame_new(NULL);
	gtk_frame_set_shadow_type(GTK_FRAME(end_frame), GTK_SHADOW_ETCHED_IN);
  gtk_box_pack_start(GTK_BOX(hbox), end_frame, FALSE, TRUE, 0);
  gtk_widget_show(end_frame);
  
   //create end toolbar
  end_toolbar = gtk_toolbar_new();
  gtk_toolbar_set_style(GTK_TOOLBAR(end_toolbar),GTK_TOOLBAR_ICONS);
  gtk_toolbar_set_icon_size(GTK_TOOLBAR(end_toolbar),GTK_ICON_SIZE_SMALL_TOOLBAR);
  gtk_container_add(GTK_CONTAINER(end_frame),end_toolbar);
  gtk_widget_show(end_toolbar);
  
  //go next button
  dm_next_button = gtk_toolbar_insert_stock(GTK_TOOLBAR(end_toolbar),GTK_STOCK_GO_FORWARD,"Go Next",
    NULL,NULL,NULL,-1);
  set_attach_gtk(dm_next_button);
  gtk_widget_show(dm_next_button);
  
  if (dm_have_first) {
    //go to end button
    dm_last_button = gtk_toolbar_insert_stock(GTK_TOOLBAR(end_toolbar),GTK_STOCK_GOTO_LAST,"Go Last",
      NULL,NULL,NULL,-1);
    set_attach_gtk(dm_last_button);
    gtk_widget_show(dm_last_button);
  }
 
  // call ancestor method
  widget::init_gui();
  gtk_widget_show(dm_widget);
}


void vcrbutton::set_vcrbutton_reactor(scopira::uikit::vcrbutton_reactor_i *reac)
{ 
  dm_vcr_reactor = reac;
  update_signals();
}

void vcrbutton::update_signals(void)
{
  g_signal_connect(G_OBJECT(dm_back_button), "clicked", G_CALLBACK(h_on_change), GINT_TO_POINTER(cmd_go_back));
  g_signal_connect(G_OBJECT(dm_next_button), "clicked", G_CALLBACK(h_on_change), GINT_TO_POINTER(cmd_go_next));
  
  if (dm_have_first)
    g_signal_connect(G_OBJECT(dm_first_button), "clicked", G_CALLBACK(h_on_change), GINT_TO_POINTER(cmd_go_first));
  
   if (dm_have_last)
     g_signal_connect(G_OBJECT(dm_last_button), "clicked", G_CALLBACK(h_on_change), GINT_TO_POINTER(cmd_go_last));
}

void vcrbutton::update_buttons(bool disable_all)
{
  if (disable_all) {
    gtk_widget_set_sensitive(dm_next_button, FALSE);
    if (dm_last_button)
      gtk_widget_set_sensitive(dm_last_button, FALSE);
    gtk_widget_set_sensitive(dm_back_button, FALSE);
    if (dm_first_button)
      gtk_widget_set_sensitive(dm_first_button, FALSE);    
  }
  else {
    if (!dm_wrap && dm_value == dm_max) {
      gtk_widget_set_sensitive(dm_next_button, FALSE);
      if (dm_last_button)
        gtk_widget_set_sensitive(dm_last_button, FALSE);
    }      
    else {
      gtk_widget_set_sensitive(dm_next_button, TRUE);
      if (dm_last_button)
        gtk_widget_set_sensitive(dm_last_button, TRUE);
    }
  
    if (!dm_wrap && (dm_value == dm_min || dm_value == 0)) {
      gtk_widget_set_sensitive(dm_back_button, FALSE);
      if (dm_first_button)
        gtk_widget_set_sensitive(dm_first_button, FALSE);
    }
    else {
      gtk_widget_set_sensitive(dm_back_button, TRUE);
      if (dm_first_button)
        gtk_widget_set_sensitive(dm_first_button, TRUE);
    }
  }
}

void vcrbutton::set_min(int v)
{
  dm_min = v;
  update_buttons(false);
}

void vcrbutton::set_max(int v)
{
  dm_max = v;
  update_buttons(false);
}

void vcrbutton::set_label(const std::string &label)
{
  gtk_label_set_text(GTK_LABEL(dm_label), label.c_str());
  update_buttons(false);
}

void vcrbutton::set_first(bool hasfirst)
{
  dm_have_first = hasfirst;
  update_buttons(false);
}

void vcrbutton::set_last(bool haslast)
{
  dm_have_last = haslast;
  update_buttons(false);
}

void vcrbutton::set_wrap(bool wrapit)
{
  dm_wrap = wrapit;
  update_buttons(false);
}

void vcrbutton::set_showmax(bool smax)
{
  dm_show_max = smax;
  update_buttons(false);
}

void vcrbutton::set_value(int v)
{
  dm_value = v;

  update_entry(false);
  update_buttons(false);
}

void vcrbutton::set_entry(void)
{
  assert(dm_entry == 0);

  dm_entry = gtk_entry_new();

  gtk_widget_set_size_request(dm_entry, 70, -1);
  gtk_editable_set_editable(GTK_EDITABLE(dm_entry), FALSE);
  gtk_box_pack_start(GTK_BOX(dm_entry_box), dm_entry, FALSE, FALSE, 0);
  gtk_widget_show(dm_entry);

  update_buttons(false);
}

void vcrbutton::h_on_change(GtkWidget *widget, int cmd)
{
  vcrbutton &here =  *reinterpret_cast<vcrbutton*>(get_attach_gtk(widget));

  assert(here.is_alive_object());
   
  switch (cmd) {
    case cmd_go_first:
      here.dm_value = here.dm_min;
      break;
      
    case cmd_go_back:
      if (!here.dm_wrap || here.dm_value > here.dm_min)
        here.dm_value--;
      else
        here.dm_value = here.dm_max;
      break;
    case cmd_go_next:
      if (!here.dm_wrap || here.dm_value < here.dm_max)
        here.dm_value++;
      else
        here.dm_value = here.dm_min;      
      break;
    case cmd_go_last:
      here.dm_value = here.dm_max;
      break;
  }       

  here.update_buttons(false);
  here.update_entry(false);
    
  
  if (here.dm_vcr_reactor)
    here.dm_vcr_reactor->react_vcrbutton(&here, here.dm_value);
}

