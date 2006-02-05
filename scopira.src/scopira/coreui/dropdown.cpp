
/**
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

#include <scopira/coreui/dropdown.h>
#include <scopira/tool/output.h>

//BBlibs scopira
//BBtargets libscopiraui.so

using namespace scopira::tool;
using namespace scopira::coreui;

dropdown::dropdown(void)
{
  dm_dropdown_reactor = 0;
  init_gui();
}

// returns current entry value, method to be used with new non-kernel-module proponent-proponent interactions
// @author Rodrigo Vivanco
int dropdown::get_selection(void) const
{
  idmap_t::const_iterator ii; 
  int idx;
  
  #if GTK_MAJOR_VERSION <= 2 && GTK_MINOR_VERSION < 4
    GtkLabel *label;
    GtkListItem *item;
    char *current_label;
    
    idx = 0;
    for (ii=dm_idrad.begin(); ii != dm_idrad.end(); ++ii) {
      idx = ii->first;
      item = GTK_LIST_ITEM(dm_idrad.find(idx)->second);
      label = GTK_LABEL(GTK_BIN(item)->child);
      gtk_label_get(GTK_LABEL(label), &current_label);
      if (strcmp(gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(dm_widget)->entry)),current_label) == 0) 
        break;  
    }
  #endif
  #if GTK_MAJOR_VERSION >= 2 && GTK_MINOR_VERSION >= 4
    int selected;
    idx = selected = -1;
    // current selected item
    selected = gtk_combo_box_get_active(GTK_COMBO_BOX(dm_widget));
  
    // find its id and return it
    for (ii = dm_idrad.begin(); ii != dm_idrad.end(); ++ii) {
      idx = ii->first;
      if (ii->second == selected)
        break;
    }
  #endif
  
  return idx;
}

// if given idx within bounds, sets that entry as current selection
// @author Rodrigo Vivanco
// @author Marina Mandelzweig
void dropdown::set_selection(int sel_idx)
{
  if ( sel_idx<0 || sel_idx>dm_idrad.size() )
    return; // out of bounds,  do nothing
  
  #if GTK_MAJOR_VERSION <= 2 && GTK_MINOR_VERSION < 4
    // most of this is copy/past from set_data()
    GtkWidget *rec;
    GtkLabel *label;
    char *current;

    rec = dm_idrad[sel_idx];
    assert(rec);
  
    label = GTK_LABEL(GTK_BIN(rec)->child);
    gtk_label_get(GTK_LABEL(label), &current);
    
    GTK_LIST(GTK_COMBO(dm_widget)->list)->last_focus_child = GTK_WIDGET(rec);
    gtk_list_select_child(GTK_LIST(GTK_COMBO(dm_widget)->list),GTK_WIDGET(rec));
  #endif
  #if GTK_MAJOR_VERSION >= 2 && GTK_MINOR_VERSION >= 4
    idmap_t::iterator ii; 
    GtkTreeModel *model;
    int count;

    ii = dm_idrad.find(sel_idx);
    if (ii != dm_idrad.end()) {
      assert(ii->second >= 0);
      model = gtk_combo_box_get_model(GTK_COMBO_BOX(dm_widget));
      count = gtk_tree_model_iter_n_children(model, NULL);
      assert(ii->second <= count);
      gtk_combo_box_set_active(GTK_COMBO_BOX(dm_widget), ii->second);
    }
  #endif
}                                                                      


void dropdown::init_gui(void)
{
  #if GTK_MAJOR_VERSION <= 2 && GTK_MINOR_VERSION < 4
    // make a combo box container
    dm_widget = gtk_combo_new();
    gtk_combo_set_value_in_list(GTK_COMBO(dm_widget), TRUE, FALSE);
    //do not allow it to be edited
    gtk_entry_set_editable(GTK_ENTRY(GTK_COMBO(dm_widget)->entry), FALSE);
    dm_signalid = g_signal_connect(GTK_OBJECT(GTK_COMBO(dm_widget)->entry), "changed", G_CALLBACK(h_on_change), this);
  #endif
  #if GTK_MAJOR_VERSION >= 2 && GTK_MINOR_VERSION >= 4
    // make a combo box container
    dm_widget = gtk_combo_box_new_text();
    dm_signalid = g_signal_connect(GTK_COMBO_BOX(dm_widget), "changed", G_CALLBACK(h_on_change), this);
  #endif

  // call ancestor method
  widget::init_gui();
}

void dropdown::clear_selections(void)
{
  #if GTK_MAJOR_VERSION <= 2 && GTK_MINOR_VERSION < 4
    //block signal while removing selections
    gtk_signal_handler_block(GTK_OBJECT(GTK_COMBO(dm_widget)->entry), dm_signalid);
    // remove al litems
    while (!dm_idrad.empty()) {
      gtk_container_remove(GTK_CONTAINER(GTK_COMBO(dm_widget)->list),  dm_idrad.begin()->second);
      dm_idrad.erase(dm_idrad.begin());
    }
    // unblock signal handler
    gtk_signal_handler_unblock(GTK_OBJECT(GTK_COMBO(dm_widget)->entry), dm_signalid);
  #endif
  #if GTK_MAJOR_VERSION >= 2 && GTK_MINOR_VERSION >= 4
    //block signal while removing selections
    gtk_signal_handler_block(GTK_OBJECT(dm_widget), dm_signalid);
    //unselect all
    gtk_combo_box_set_active(GTK_COMBO_BOX(dm_widget), -1);
    // remove al litems
    while (!dm_idrad.empty()) {
      gtk_combo_box_remove_text(GTK_COMBO_BOX(dm_widget), 0);
      dm_idrad.erase(dm_idrad.begin());
    }
    // unblock signal handler
    gtk_signal_handler_unblock(GTK_OBJECT(dm_widget), dm_signalid);
  #endif
}

void dropdown::add_selection(int id, const std::string &desc)
{
  assert(is_alive_object());
  assert(!desc.empty());
  
  #if GTK_MAJOR_VERSION <= 2 && GTK_MINOR_VERSION < 4
    GtkWidget *item;
    
    assert(GTK_COMBO(dm_widget));
    assert(GTK_COMBO(dm_widget)->list);  
      
    //block signal
    gtk_signal_handler_block(GTK_OBJECT(GTK_COMBO(dm_widget)->entry), dm_signalid);

    // create the new item
    item = gtk_list_item_new_with_label(desc.c_str());
    //add item to the combo box
    gtk_combo_set_item_string(GTK_COMBO(dm_widget), GTK_ITEM(item), desc.c_str());
    gtk_container_add(GTK_CONTAINER(GTK_COMBO(dm_widget)->list), item);
    // record it
    dm_idrad[id] = item;
    //show it
    gtk_widget_show(item);
    
    // unblock signal handler
    gtk_signal_handler_unblock(GTK_OBJECT(GTK_COMBO(dm_widget)->entry), dm_signalid);
  #endif
  #if GTK_MAJOR_VERSION >= 2 && GTK_MINOR_VERSION >= 4
    GtkTreeModel *model;
    int count;
    
    assert(GTK_COMBO_BOX(dm_widget));
    
    //block signal while adding a selection
    gtk_signal_handler_block(GTK_OBJECT(dm_widget), dm_signalid);
    
    // record it
    model = gtk_combo_box_get_model(GTK_COMBO_BOX(dm_widget));
    count = gtk_tree_model_iter_n_children(model, NULL);
    dm_idrad[id] = count;
  
    gtk_combo_box_append_text(GTK_COMBO_BOX(dm_widget), desc.c_str());
    // set the first item to be selected
    gtk_combo_box_set_active(GTK_COMBO_BOX(dm_widget), 0);
    
    // unblock signal handler
    gtk_signal_handler_unblock(GTK_OBJECT(dm_widget), dm_signalid);
  #endif
}

void dropdown::h_on_change(GtkEditable *entry, gpointer data)
{  
  dropdown *here = reinterpret_cast<dropdown*>(data);
    
  assert(here->is_alive_object());
  
  #if GTK_MAJOR_VERSION <= 2 && GTK_MINOR_VERSION < 4
    // when a dropdown option is selected, the handler is called twice: first as an empty string which is invalid
    // and then the real selected item (which is not an empty string, so ignore the empty string entry)
    if (gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(here->dm_widget)->entry))[0] != 0) {
      if (here->dm_dropdown_reactor)
        here->dm_dropdown_reactor->react_dropdown(here, here->get_selection());
    }
  #endif
  #if GTK_MAJOR_VERSION >= 2 && GTK_MINOR_VERSION >= 4
  if (here->dm_dropdown_reactor)
    here->dm_dropdown_reactor->react_dropdown(here, here->get_selection());
  #endif
}

