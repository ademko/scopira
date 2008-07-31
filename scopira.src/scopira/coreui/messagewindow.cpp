
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

#include <scopira/coreui/messagewindow.h>

#include <gtk/gtk.h>

#include <scopira/coreui/layout.h>
#include <scopira/coreui/button.h>
#include <scopira/coreui/label.h>

//BBlibs scopira
//BBtargets libscopiraui.so

using namespace scopira::tool;
using namespace scopira::coreui;

messagewindow::messagewindow(const std::string &title, const std::string &msg,
        int typ, bool modal)
  : dialog(modal)
{
  dialog::init_gui();
  set_title(title);
  init_ok_button("OK");

  if (typ == notype_c) {
    gtk_box_pack_start(GTK_BOX(get_window_area()), gtk_label_new(msg.c_str()), TRUE, TRUE, 4);
    return;
  }

  GtkWidget *hbox, *image;

  hbox = gtk_hbox_new(FALSE, 4);

  switch (typ) {
    case question_c: 
      image = gtk_image_new_from_stock(GTK_STOCK_DIALOG_QUESTION, GTK_ICON_SIZE_DIALOG);
      init_cancel_button( "No" );
      break;
    case error_c: 
      image = gtk_image_new_from_stock(GTK_STOCK_DIALOG_ERROR, GTK_ICON_SIZE_DIALOG);
      break;
    default: 
      image = gtk_image_new_from_stock(GTK_STOCK_DIALOG_INFO, GTK_ICON_SIZE_DIALOG);
  }

  assert(image);

  gtk_box_pack_start(GTK_BOX(hbox), image, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(hbox), gtk_label_new(msg.c_str()), TRUE, TRUE, 0);

  gtk_box_pack_start(GTK_BOX(get_window_area()), hbox, TRUE, TRUE, 4);
}

void messagewindow::popup(const std::string &title, const std::string &msg,
        int typ, bool modal)
{
  // it's assumed the central window list system will keep refs on this instance
  messagewindow *win = new messagewindow(title, msg, typ, modal);

  // show it
  win->show_all();
}

void messagewindow::popup_error(const std::string &msg)
{
  popup("Error", msg, error_c, true);
}

//
//
// entrywindow
//
//

entrywindow::entrywindow(const std::string &title, const std::string &msg,
      const std::string &defval)
  : window(title, false), dm_reactor(0)
{
  count_ptr<box_layout> bx;
  count_ptr<button_layout> bl;

  bx = new box_layout(false, false);

  dm_entry = new entry;
  dm_entry->set_text(defval);

  bx->add_widget(new label(msg), false, false);
  bx->add_widget(dm_entry.get(), true, true);

  bl = new button_layout(bx.get());
  bl->add_stock_buttons(button_constants::button_ok_c | button_constants::button_cancel_c, this);

  dm_thewidget = bl.get();
  window::init_gui(dm_thewidget->get_widget());
}

void entrywindow::react_button(scopira::coreui::button *source, int actionid)
{
  if (actionid == button_constants::action_apply_c && dm_reactor) {
    // reflect this msg
    std::string txt;
    dm_entry->get_text(txt);
    dm_reactor->react_entry(dm_entry.get(), txt.c_str());
  }

  if (actionid == button_constants::action_close_c)
    on_destroy();
}

