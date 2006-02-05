
/*
 *  Copyright (c) 2001    National Research Council
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

filewindow::filewindow(const std::string &title, const std::string &pattern, int type)
  : window(title), dm_reactor(0)     // call protoected constructor
{
  init_gui(pattern,type);
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

void filewindow::init_gui(const std::string &pattern, int type)
{
  if (type == cmd_open_file) 
    dm_widget =  gtk_file_chooser_dialog_new(dm_title.c_str(), 
      NULL,
      GTK_FILE_CHOOSER_ACTION_OPEN,
      GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
      NULL);
   else if (type == cmd_save_file)
     dm_widget =  gtk_file_chooser_dialog_new(dm_title.c_str(), 
      NULL,
      GTK_FILE_CHOOSER_ACTION_SAVE,
      GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
      NULL);
              
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

void fileentry::set_filename(const std::string &filename)
{
  std::string dirs, fullname;
  
  // get the directory components of a file name, 
  dirs = g_path_get_dirname(filename.c_str());
  
  //  if the file name has no directory components, dirs will contain  only "."  
  if (dirs == ".") {
    fullname = g_build_filename(g_get_current_dir(), filename.c_str(), NULL);
    gtk_entry_set_text(GTK_ENTRY(dm_entry), fullname.c_str());
  }
  else
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
  fwin = new filewindow("Select A File", "*",cmd_open_file);                                             
  fwin->set_filename(gtk_entry_get_text(GTK_ENTRY(here->dm_entry)));
  fwin->set_filename_reactor(&h_filewindow_ok, here);
  fwin->add_destroy_reactor(here);

  here->dm_filewin = fwin;

  fwin->show_all();
}

bool fileentry::h_filewindow_ok(scopira::coreui::filewindow *source, const char *filename, void *data)
{
  fileentry *here = reinterpret_cast<fileentry*>(data);

  // grab the new text
  gtk_entry_set_text(GTK_ENTRY(here->dm_entry), filename);

  return true;
}

