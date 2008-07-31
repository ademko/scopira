
/*
 *  Copyright (c) 2001-2006    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#include <scopira/coreui/filewindow.h>

#include <assert.h>

#include <gtk/gtk.h>
#include <scopira/tool/file.h>
#include <scopira/core/loop.h>

//BBlibs scopira
//BBtargets libscopiraui.so

using namespace scopira;
using namespace scopira::coreui;
using namespace scopira::tool;

//
//
// filewindow
//
//

filewindow::filewindow(const std::string &title, int type)
  : window(title), dm_reactor(0)     // call protoected constructor
{
  init_gui("", "", type);
}

filewindow::filewindow(const std::string &title, const std::string &filtername, const std::string &filterpattern, int type)
  : window(title), dm_reactor(0)     // call protoected constructor
{
  init_gui(filtername, filterpattern, type);
}

void filewindow::set_filename(const std::string &filename)
{
  gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(dm_widget), filename.c_str());
}

std::string filewindow::get_filename(void) const
{
  return gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dm_widget));
}

void filewindow::set_filename_reactor(filewindow_reactor_t r, void *data)
{
  dm_reactor = r;
  dm_reactor_payload = data;
}

void filewindow::init_gui(const std::string &patternname, const std::string &pattern, int type)
{
  GtkFileChooserAction act = static_cast<GtkFileChooserAction>(0);
  const char * butname = 0;

  switch (type) {
    case cmd_open_file_c: act = GTK_FILE_CHOOSER_ACTION_OPEN; butname = GTK_STOCK_OPEN; break;
    case cmd_save_file_c: act = GTK_FILE_CHOOSER_ACTION_SAVE; butname = GTK_STOCK_SAVE; break;
    case cmd_open_dir_c: act = GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER; butname = GTK_STOCK_OPEN; break;
    case cmd_save_dir_c: act = GTK_FILE_CHOOSER_ACTION_CREATE_FOLDER; butname = GTK_STOCK_SAVE; break;
    default: assert(false);
  }

  dm_widget = gtk_file_chooser_dialog_new(dm_title.c_str(),
    NULL,
    act,
    butname, GTK_RESPONSE_ACCEPT,
    GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
    NULL);

  if (!pattern.empty() && pattern != "*") {
    // add a pattern
    gtk_ptr<GtkFileFilter> f;

    f = gtk_file_filter_new();
    gtk_file_filter_set_name(f.get(), patternname.c_str());
    gtk_file_filter_add_pattern(f.get(), pattern.c_str());
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dm_widget), f.get());

    f = gtk_file_filter_new();
    gtk_file_filter_set_name(f.get(), "All files");
    gtk_file_filter_add_pattern(f.get(), "*");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dm_widget), f.get());
  }

  g_signal_connect(G_OBJECT(dm_widget), "response", G_CALLBACK (h_response), this);

  window::init_gui();
}

void filewindow::ok(void)
{
  if (dm_reactor)
    if (!dm_reactor(this, gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dm_widget)), dm_reactor_payload))
      return;

  on_destroy();
}

void filewindow::h_response(GtkWidget *widget, gint res, gpointer data)
{
  filewindow *here = reinterpret_cast<filewindow*>(data);
  assert(here->is_alive_object());

  switch (res) {
    case GTK_RESPONSE_ACCEPT:
      here->ok();
      break;
    case GTK_RESPONSE_CANCEL:
      here->on_destroy();
  }
}

//
//
// fileentry
//
//

fileentry::fileentry(void)
{
  dm_filewin = 0;
  dm_file_name_filter = "*";
  dm_file_action = cmd_open_file_c;

  init_gui();
}

fileentry::~fileentry()
{
  if (dm_filewin) {
    dm_filewin->on_destroy();
    if (dm_filewin) {
      assert(false);//try this code path out sometime
      dm_filewin->remove_destroy_reactor(this);
      delete dm_filewin;
    }
  }
}

void fileentry::react_destroy(scopira::tool::object *source)
{
  // it killed itself, note it
  dm_filewin = 0;
}

void fileentry::set_config_key(const std::string &configkey)
{
  dm_configkey = configkey;

  if (scopira::core::basic_loop::instance()->has_config(dm_configkey))
    set_filename(scopira::core::basic_loop::instance()->get_config(dm_configkey));
}

void fileentry::set_filename(const std::string &filename)
{
  gtk_entry_set_text(GTK_ENTRY(dm_entry), filename.c_str());
}

std::string fileentry::get_filename(void) const
{
  return gtk_entry_get_text(GTK_ENTRY(dm_entry));
}

void fileentry::init_gui(void)
{
  GtkWidget *but;

  dm_widget = gtk_hbox_new(FALSE, 0);

  // make my entry
  dm_entry = gtk_entry_new();
  g_signal_connect(G_OBJECT(dm_entry), "activate", G_CALLBACK(h_entry), this);
  gtk_box_pack_start(GTK_BOX(dm_widget), dm_entry, TRUE, TRUE, 0);
  gtk_widget_show(dm_entry);

  but = gtk_button_new_with_label("Browse...");
  g_signal_connect(G_OBJECT(but), "clicked", G_CALLBACK(h_browse), this);
  gtk_box_pack_start(GTK_BOX(dm_widget), but, FALSE, FALSE, 0);
  gtk_widget_show(but);

  // call ancestor method
  widget::init_gui();
}

void fileentry::h_browse(GtkWidget *widget, gpointer data)
{
  fileentry *here = reinterpret_cast<fileentry*>(data);
  filewindow *fwin;

  assert(here->is_alive_object());

  if (here->dm_filewin) {
    here->dm_filewin->on_focus(true);
    return;
  }

  // popup the file dialog
  fwin = new filewindow("Select A File", "File Filter", here->dm_file_name_filter, here->dm_file_action);
  fwin->set_filename(gtk_entry_get_text(GTK_ENTRY(here->dm_entry)));
  fwin->set_filename_reactor(&h_filewindow_ok, here);
  fwin->add_destroy_reactor(here);

  here->dm_filewin = fwin;

  fwin->show_all();
}

void fileentry::h_entry(GtkEntry *e, gpointer data)
{
  fileentry *here = reinterpret_cast<fileentry*>(data);

  assert(here->is_alive_object());

  if (!here->dm_configkey.empty())
    scopira::core::basic_loop::instance()->set_config_save(here->dm_configkey, gtk_entry_get_text(e)); // save it the string, which they changed via the text field
}

bool fileentry::h_filewindow_ok(scopira::coreui::filewindow *source, const char *filename, void *data)
{
  fileentry *here = reinterpret_cast<fileentry*>(data);

  assert(here->is_alive_object());

  // grab the new text
  gtk_entry_set_text(GTK_ENTRY(here->dm_entry), filename);

  if (!here->dm_configkey.empty())
    scopira::core::basic_loop::instance()->set_config_save(here->dm_configkey, filename); // save it, too

  return true;
}

