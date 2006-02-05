/*
 *  Copyright (c) 2003    National Research Council
 *
 *  All rights reserved.
 *
 *  This msaterial is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#include <scopira/coreui/menu.h>

#include <assert.h>

#include <scopira/tool/flow.h>
#include <scopira/coreui/widget.h>

//BBlibs scopira
//BBtargets libscopiraui.so

using namespace std;
using namespace scopira::coreui;

void scopira::coreui::escape_underscores(const std::string &src, std::string &dest)
{
  std::string::const_iterator ii, endii;

  dest.clear();

  endii = src.end();
  for (ii=src.begin(); ii != endii; ++ii) {
    if (*ii == '_')
      dest.push_back('_');
    dest.push_back(*ii);
  }
}

std::string scopira::coreui::escape_underscores(const std::string &src)
{
  std::string dest;
  std::string::const_iterator ii, endii;

  dest.reserve(src.size()+8);

  endii = src.end();
  for (ii=src.begin(); ii != endii; ++ii) {
    if (*ii == '_')
      dest.push_back('_');
    dest.push_back(*ii);
  }
  
  return dest;
}

//
//
// menu_builder
//
//

menu_builder::menu_builder(GtkWidget *base)
{
  dm_stack[0] = base;
  dm_cur = 0;
  dm_last = 0;
  dm_attach_ptr = 0;
}

menu_builder::~menu_builder()
{
  assert(dm_cur == 0);
}

GtkWidget * menu_builder::add_item(const std::string &label)
{
  dm_last = gtk_menu_item_new_with_mnemonic(label.c_str());
  append_and_show(dm_last);

  if (dm_attach)
    dm_attach->set_attach_gtk(dm_last);
  if (dm_attach_ptr)
    widget::set_attach_ptr(dm_last, dm_attach_ptr);

  return dm_last;
}

GtkWidget * menu_builder::add_stock_item(const std::string &label, const char *stockid)
{
  dm_last = gtk_image_menu_item_new_with_mnemonic(label.c_str());

  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(dm_last),
    gtk_image_new_from_stock(stockid, GTK_ICON_SIZE_MENU));

  append_and_show(dm_last);

  if (dm_attach)
    dm_attach->set_attach_gtk(dm_last);
  if (dm_attach_ptr)
    widget::set_attach_ptr(dm_last, dm_attach_ptr);

  return dm_last;
}

GtkWidget * menu_builder::add_check_item(const std::string &label, bool checked)
{
	dm_last = gtk_check_menu_item_new_with_mnemonic(label.c_str());
	append_and_show(dm_last);
	
	if (dm_attach)
		dm_attach->set_attach_gtk(dm_last);
  if (dm_attach_ptr)
    widget::set_attach_ptr(dm_last, dm_attach_ptr);
		
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(dm_last), checked);
	
	return dm_last;
}

void menu_builder::set_callback(GCallback callback, gpointer data)
{
  assert(dm_last);
  assert(callback);
  g_signal_connect(G_OBJECT(dm_last), "activate", callback, data);
}

void menu_builder::set_enable(bool enab)
{
  assert(dm_last);
  gtk_widget_set_sensitive(dm_last, enab?TRUE:FALSE);
}

GtkWidget * menu_builder::add_separator(void)
{
  GtkWidget *menu_item;

  dm_last = 0;

  menu_item = gtk_menu_item_new();
  gtk_container_add(GTK_CONTAINER(menu_item), gtk_hseparator_new());
  gtk_widget_set_sensitive(menu_item, FALSE);
  append_and_show(menu_item);

  return menu_item;
}

GtkWidget * menu_builder::push_menu(const std::string &label)
{
  GtkWidget *item, *menu;

  item = gtk_menu_item_new_with_mnemonic(label.c_str());
  menu = gtk_menu_new();
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(item), menu);

  append_and_show(item);

  dm_cur++;
  assert(dm_cur<dm_stack.size());
  dm_stack[dm_cur] = menu;

  dm_last = 0;

  return item;
}

void menu_builder::pop_menu(void)
{
  dm_last = 0;
  assert(dm_cur>0);
  dm_cur--;
}

void menu_builder::append_and_show(GtkWidget *menu_item)
{
  gtk_menu_shell_append(GTK_MENU_SHELL(dm_stack[dm_cur]), menu_item);
  gtk_widget_show_all(menu_item);
}

//
//
// menu_bar_builder
//
//

menu_bar_builder::menu_bar_builder(void)
  : menu_builder(dm_bar = gtk_menu_bar_new())
{
  gtk_widget_ref(dm_bar);
}

menu_bar_builder::~menu_bar_builder()
{
  gtk_widget_unref(dm_bar);
}

//
//
// menu_pop_builder
//
//

menu_pop_builder::menu_pop_builder(void)
  : menu_builder(0)
{
  GtkWidget *menu;

  dm_pop = gtk_tearoff_menu_item_new();
  menu = gtk_menu_new();
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(dm_pop), menu);

  dm_stack[0] = menu;
  
  gtk_widget_ref(dm_pop);
}

menu_pop_builder::~menu_pop_builder()
{
  gtk_widget_unref(dm_pop);
}

void menu_pop_builder::popup(int mousebut)
{
  gtk_menu_popup(GTK_MENU(dm_stack[0]), 0, 0, 0, 0, mousebut+1, gtk_get_current_event_time());
}

//
//
// menu_sub_builder
//
//

menu_sub_builder::menu_sub_builder(const std::string &label)
  : menu_builder(dm_sub = gtk_menu_new())
{
  dm_item = gtk_menu_item_new_with_mnemonic(label.c_str());
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(dm_item), dm_sub);

  gtk_widget_ref(dm_item);
}

menu_sub_builder::~menu_sub_builder()
{
  gtk_widget_unref(dm_item);
}

void menu_sub_builder::commit_sub_menu(GtkWidget *menu)
{
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), dm_item);
  gtk_widget_show_all(dm_item);
}

