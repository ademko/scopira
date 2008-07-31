
/*
 *  Copyright (c) 2006    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#include <scopira/coreui/progressbar.h>

#include <gtk/gtk.h>

//BBtargets libscopiraui.so
//BBlibs scopira

using namespace scopira::coreui;

progressbar::progressbar(bool horiz)
{
  dm_widget = gtk_progress_bar_new();

  gtk_progress_bar_set_orientation(GTK_PROGRESS_BAR(dm_widget), horiz ? GTK_PROGRESS_LEFT_TO_RIGHT : GTK_PROGRESS_BOTTOM_TO_TOP);

  init_gui();
}

void progressbar::set_fraction(double d)
{
  gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(dm_widget), d);
}

void progressbar::set_text(const std::string &txt)
{
  gtk_progress_bar_set_text(GTK_PROGRESS_BAR(dm_widget), txt.c_str());
}

void progressbar::pulse(void)
{
  gtk_progress_bar_pulse(GTK_PROGRESS_BAR(dm_widget));
}

