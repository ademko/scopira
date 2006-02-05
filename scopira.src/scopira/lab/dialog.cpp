
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

#include <scopira/lab/dialog.h>

//BBtargets libscopiraui.so

using namespace scopira::tool;
using namespace scopira::coreui;
using namespace scopira::lab;

//
//
// msg_window
//
//

msg_window::msg_window(void)
{
  init_gui(button_close_c);
}

msg_window::msg_window(int buttons)
{
  init_gui(buttons);
}

void msg_window::set_msg(const std::string &label)
{
  lab_lock L;

  gtk_label_set_text(GTK_LABEL(dm_label), label.c_str());
}

void msg_window::popup(const std::string &label, bool waitforreply)
{
  count_ptr<msg_window> stat = new msg_window;

  stat->set_title("Information");
  stat->set_msg(label);
  stat->show_window();

  if (waitforreply)
    stat->wait_action();
}

int msg_window::popup(const std::string &label, int buttons)
{
  count_ptr<msg_window> stat = new msg_window(buttons);

  stat->set_title("Question");
  stat->set_msg(label);
  stat->show_window();

  stat->wait_action();

  return stat->pop_action();
}

void msg_window::init_gui(int buttons)
{
  lab_lock L;

  dm_label = gtk_label_new("(label)");

  lab_window::init_gui(new scopira::coreui::widget(dm_label), buttons);
}

//
//
// query_window
//
//

query_window::query_window(int buttons)
{
  init_gui(buttons);
}

void query_window::set_question(const std::string &label)
{
  lab_lock L;

  gtk_label_set_text(GTK_LABEL(dm_label), label.c_str());
}

void query_window::set_answer(const std::string &txt)
{
  lab_lock L;

  gtk_entry_set_text(GTK_ENTRY(dm_entry), txt.c_str());
}

std::string query_window::get_answer(void) const
{
  lab_lock L;

  return gtk_entry_get_text(GTK_ENTRY(dm_entry));
}

std::string query_window::popup(const std::string &question, const std::string &default_answer)
{
  count_ptr<query_window> win = new query_window;

  win->set_title("Query");
  win->set_question(question);
  win->set_answer(default_answer);
  win->show_window();

  win->wait_action();

  return win->get_answer();
}

void query_window::init_gui(int buttons)
{
  lab_lock L;
  GtkWidget *box;

  box = gtk_vbox_new(FALSE, 10);
  dm_label = gtk_label_new("Question?");
  dm_entry = gtk_entry_new();

  gtk_box_pack_start(GTK_BOX(box), dm_label, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(box), dm_entry, FALSE, FALSE, 0);

  lab_window::init_gui(box, buttons);
}

//
//
// filename_window
//
//

filename_window::filename_window(int buttons)
{
  init_gui(buttons);
}

void filename_window::set_question(const std::string &label)
{
  lab_lock L;

  dm_label->set_label(label);
}

void filename_window::set_filename(const std::string &filename)
{
  lab_lock L;

  dm_fileentry->set_filename(filename);
}

std::string filename_window::get_filename(void) const
{
  lab_lock L;

  return dm_fileentry->get_filename();
}

std::string filename_window::popup(const std::string &question, const std::string &default_filename)
{
  count_ptr<filename_window> win = new filename_window;

  win->set_question(question);
  win->set_filename(default_filename);
  win->show_window();

  win->wait_action();

  return win->get_filename();
}

void filename_window::init_gui(int buttons)
{
  lab_lock L;
  count_ptr<box_layout> box;

  box = new box_layout(true, false);
  dm_label = new label;
  dm_fileentry = new fileentry;

  box->add_widget(dm_label.get(), false, false);
  box->add_widget(dm_fileentry.get(), true, true);

  lab_window::init_gui(box.get(), buttons);
}

quit_window::quit_window(void)
  : msg_window(button_yes_c|button_no_c)
{
  dm_clickedyes = false;

  // at this point, the underlying msg_window is fully constructed
  set_msg("Close all open Scopira Lab windows?");
}

void quit_window::popup(void)
{
  count_ptr<quit_window> qwin = new quit_window;

  qwin->show_window();
}

bool quit_window::on_button(int actionID)
{
  if (actionID == action_yes_c)
    dm_clickedyes = true;

  if (actionID == action_close_c && dm_clickedyes) {
    gui_hide_all();
    // the above will do the kill for us, no need to let the default system do it
    return false;
  }

  return true;
}

