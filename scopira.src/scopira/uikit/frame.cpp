
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

#include <scopira/uikit/frame.h>

#include <gtk/gtklabel.h>
#include <gtk/gtkframe.h>

#include <scopira/coreui/layout.h>
#include <scopira/coreui/label.h>

//BBtargets libscopiraui.so

using namespace scopira::coreui;
using namespace scopira::uikit;
using namespace std;


frame::frame(void)
{
  dm_widget = gtk_frame_new(0);
  init_gui();
}

frame::frame(const string &label)
{
  dm_widget = gtk_frame_new(label.c_str());
  init_gui();
}


void frame::set_label(const string &label)
{
  gtk_frame_set_label(GTK_FRAME(dm_widget), label.c_str());
}

void frame::add_widget(widget *w)
{
  assert(w);
  
  dm_child = w;
  gtk_container_add(GTK_CONTAINER(dm_widget), w->get_widget());
}


