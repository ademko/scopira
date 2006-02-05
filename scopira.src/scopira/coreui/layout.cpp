
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

#include <scopira/coreui/layout.h>

#include <gtk/gtk.h>

#include <string>

#include <scopira/coreui/button.h>


//BBlibs scopira
//BBtargets libscopiraui.so

using namespace scopira::tool;
using namespace scopira::coreui;

//
//
// box_layout
//
//

box_layout::box_layout(bool horiz, bool homogeneous, int spacing)
{
  if (horiz)
    dm_widget = gtk_hbox_new(homogeneous?TRUE:FALSE, spacing);
  else
    dm_widget = gtk_vbox_new(homogeneous?TRUE:FALSE, spacing);
  widget::init_gui();
}

box_layout::box_layout(GtkWidget *w)
{
  dm_widget = w;
}

void box_layout::add_widget(widget *wid, bool expand, bool fill, int padding)
{
  assert(wid->is_alive_object());
  dm_children.push_back(wid);
  gtk_box_pack_start(GTK_BOX(dm_widget), wid->get_widget(),
      expand?TRUE:FALSE, fill?TRUE:FALSE, padding);

  gtk_widget_show_all( dm_widget );
}

void box_layout::add_widget_end(widget *wid, bool expand, bool fill, int padding)
{
  assert(wid->is_alive_object());
  dm_children.push_back(wid);
  gtk_box_pack_end(GTK_BOX(dm_widget), wid->get_widget(),
      expand?TRUE:FALSE, fill?TRUE:FALSE, padding);

  gtk_widget_show_all( dm_widget );
}

void box_layout::remove_widget(widget *wid)
{
  assert(wid->is_alive_object());
  
  bool found = false;
  scopira::tool::count_ptr<widget>  w;
  
  std::list< scopira::tool::count_ptr<widget> >::iterator ii=dm_children.begin(), ii_end=dm_children.end();  
  for ( ; ii != ii_end; ++ii )
    if ( ii->get() == wid ) {
      found = true;
      break;
    }
  
  assert(found);
  
  gtk_container_remove(GTK_CONTAINER(dm_widget), wid->get_widget());
  dm_children.erase(ii);
}

//
//
// buttonbox_layout
//
//

buttonbox_layout::buttonbox_layout(bool horiz, int layoutstyle)
  : box_layout(horiz ? gtk_hbutton_box_new() : gtk_vbutton_box_new())
{
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dm_widget), static_cast<GtkButtonBoxStyle>(layoutstyle));
  gtk_box_set_spacing(GTK_BOX(dm_widget), 10);
  widget::init_gui();
}

//
//
// split_layout
//
//

split_layout::split_layout(bool horiz)
{
  dm_widget = horiz ? gtk_hpaned_new() : gtk_vpaned_new();
  assert(dm_widget);
  init_gui();
}

void split_layout::add_widget(widget *wid, short position, bool expand, bool fill)
{
  assert(wid->is_alive_object());
  if (position == 0) {
    dm_0 = wid;
    gtk_paned_pack1(GTK_PANED(dm_widget), wid->get_widget(), expand?TRUE:FALSE, fill?TRUE:FALSE);
  } else {
    dm_1 = wid;
    gtk_paned_pack2(GTK_PANED(dm_widget), wid->get_widget(), expand?TRUE:FALSE, fill?TRUE:FALSE);
  }
}

//
//
// tab_layout
//
//

tab_layout::tab_layout(void)
{
  dm_children.reserve(8);
  dm_widget = gtk_notebook_new();
  gtk_notebook_set_scrollable(GTK_NOTEBOOK(dm_widget), TRUE);
  init_gui();
}

int tab_layout::add_widget(widget *wid, const std::string &tablabel)
{
  assert(wid->is_alive_object());

  dm_children.push_back(wid);
  gtk_notebook_append_page(GTK_NOTEBOOK(dm_widget), wid->get_widget(), gtk_label_new(tablabel.c_str()));

  // not all gtk versions return int from gtk_notebook_append_page, so we have to do this here
  return find_tab(wid);
}

int tab_layout::add_widget_prepend(widget *wid, const std::string &tablabel)
{
  assert(wid->is_alive_object());

  dm_children.push_back(wid);
  gtk_notebook_prepend_page(GTK_NOTEBOOK(dm_widget), wid->get_widget(), gtk_label_new(tablabel.c_str()));

  // not all gtk versions return int from gtk_notebook_append_page, so we have to do this here
  return find_tab(wid);
}

void tab_layout::switch_tab(int id)
{
  gtk_notebook_set_current_page(GTK_NOTEBOOK(dm_widget), id);
}

void tab_layout::set_tab_label(int id, const std::string &tablabel)
{
  gtk_notebook_set_tab_label(GTK_NOTEBOOK(dm_widget), gtk_notebook_get_nth_page(GTK_NOTEBOOK(dm_widget), id),
    gtk_label_new(tablabel.c_str()));
}

int tab_layout::find_tab(widget *wid)
{
  assert(wid && wid->is_alive_object() && wid->get_widget());
  return gtk_notebook_page_num(GTK_NOTEBOOK(dm_widget), wid->get_widget());
}

void tab_layout::remove_tab(int idx)
{
  GtkWidget *w;

  w = gtk_notebook_get_nth_page(GTK_NOTEBOOK(dm_widget), idx);
  assert(w);

  // remove the page
  gtk_notebook_remove_page(GTK_NOTEBOOK(dm_widget), idx);
  // kill the object
  for (children_t::iterator ii=dm_children.begin(); ii != dm_children.end(); ++ii)
    if ((*ii)->get_widget() == w) {
      dm_children.erase(ii);
      return;
    }
}

//
//
// grid_layout
//
//

grid_layout::grid_layout(int width, int heigth, bool homo)
  : dm_children(width, heigth)
{
  dm_widget = gtk_table_new(heigth, width, homo?TRUE:FALSE);
  init_gui();
}

void grid_layout::add_widget(widget *wid, int x, int y, int w, int h, bool expandH, bool expandV, int paddingH, int paddingV)
{
  assert(wid->is_alive_object());
  assert( (x+w) <= dm_children.width() );
  assert( (y+h) <= dm_children.height() );

  dm_children(x,y) = wid;

  gtk_table_attach(GTK_TABLE(dm_widget), wid->get_widget(), x, x+w, y, y+h,
      static_cast<GtkAttachOptions>(expandH ? GTK_EXPAND|GTK_FILL : 0),
      static_cast<GtkAttachOptions>(expandV ? GTK_EXPAND|GTK_FILL : 0),
      paddingH, paddingV); 

   gtk_widget_show(wid->get_widget());
}


//
// button_layout
//
//


button_layout::button_layout(widget *mainwidget)
{
  assert(mainwidget);

  dm_mainbox = new box_layout(false, false);
  //count_ptr<box_layout> dm_actionbox = new box_layout(false, false);    // revert this to a member at sometime in the future, if we ever need window_area stuff exposed to the end user

  //dm_actionbox->add_widget(mainwidget, true, true);

  //dm_mainbox->add_widget(dm_actionbox.get(), true, true);
  dm_mainbox->add_widget(mainwidget, true, true, 6);

  dm_widget = dm_mainbox->get_widget();

  dm_mainbox->set_border_size(4);

  init_gui();
}

void button_layout::add_button(widget *but)
{
  if (dm_butbox.is_null()) {
    dm_butbox = new buttonbox_layout(true);
    dm_mainbox->add_widget(new widget(gtk_hseparator_new()), false, false, 6);
    dm_mainbox->add_widget(dm_butbox.get(), false, false);
  }

  dm_butbox->add_widget(but, false, false, 10);
}

void button_layout::add_stock_buttons(int button_mask, button_reactor_i *reac)
{
  struct spec {
    int flag;
    const char *label;
    const char *stock_icon;
    int action, action2;
  };
  spec specs[] = {
    {button_yes_c, "Yes", GTK_STOCK_YES, action_yes_c, action_close_c},
    {button_no_c, "No", GTK_STOCK_NO, action_none_c, action_close_c},
    {button_open_c, "Open", GTK_STOCK_OPEN, action_open_c, action_close_c},
    {button_save_c, "Save", GTK_STOCK_SAVE, action_save_c, action_close_c},
    {button_revert_c, "Revert", GTK_STOCK_UNDO, action_revert_c, 0},
    {button_refresh_c, "Refresh", GTK_STOCK_REFRESH, action_refresh_c, 0},
    {button_clear_c, "Clear", GTK_STOCK_CLEAR, action_clear_c, 0},
    {button_ok_c, "OK", GTK_STOCK_OK, action_apply_c, action_close_c},
    {button_apply_c, "Apply", GTK_STOCK_APPLY, action_apply_c, 0},
    {button_close_c, "Close", GTK_STOCK_CLOSE, action_close_c, 0},
    {button_cancel_c, "Cancel", GTK_STOCK_CANCEL, action_close_c, 0},
    {0, 0, 0, 0}};
  spec *cur;

  assert(reac);

  // setup any buttons
  for (cur = specs; cur->flag; ++cur)
    if (cur->flag & button_mask) {
      count_ptr<button> but;
      
      if (cur->stock_icon)
        but = new button(button::stock_c, cur->stock_icon, cur->action, cur->action2);
      else
        but = new button(cur->label, cur->action, cur->action2);

      but->set_button_reactor(reac);

      add_button(but.get());
    }
}

