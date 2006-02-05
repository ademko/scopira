
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

#include <scopira/coreui/label.h>

#include <gtk/gtk.h>

//BBtargets libscopiraui.so

using namespace scopira;

coreui::label::label(void)
{
  dm_widget = gtk_label_new("label");

  // call ancestor method
  coreui::widget::init_gui();
}

coreui::label::label(const std::string &label)
{
  dm_widget = gtk_label_new(label.c_str());

  // call ancestor method
  coreui::widget::init_gui();
}

void coreui::label::set_label(const std::string &val)  // RV
{
  gtk_label_set_text(GTK_LABEL(dm_widget), val.c_str());
}

