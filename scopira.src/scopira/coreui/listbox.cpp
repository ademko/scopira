
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

#include <scopira/coreui/listbox.h>

#include <assert.h>

#include <gtk/gtkentry.h>

#include <scopira/tool/util.h>
#include <scopira/tool/output.h>

//BBlibs scopira
//BBtargets libscopiraui.so

using namespace scopira;
using namespace scopira::coreui;

/**
 *******************************
 * CLASS: listbox
 * @author Rodrigo Vivanco
 *******************************
 */
listbox::listbox()
  :dm_reactor(0)
{
  init_gui();
}

void listbox::add_entry( const std::string &entry )
{
  dm_entry_list.push_back( entry );
  update_table_store();
}

void listbox::init_gui( void )
{
  GtkWidget *table_view;
  GtkTreeSelection  *selection;
  GtkTreeViewColumn *col;
  
  // fields are: label text
  dm_table_store = gtk_list_store_new( table_num_cols, G_TYPE_STRING );

  table_view = gtk_tree_view_new_with_model( GTK_TREE_MODEL(dm_table_store) );
  gtk_tree_view_set_rules_hint( GTK_TREE_VIEW(table_view), true );

  selection = gtk_tree_view_get_selection( GTK_TREE_VIEW(table_view) );
  gtk_tree_selection_set_mode( selection, GTK_SELECTION_MULTIPLE );
  g_signal_connect( G_OBJECT(selection), "changed", G_CALLBACK(h_on_table_selection_change), this );

  // add row columns to model
  col = gtk_tree_view_column_new_with_attributes("Name", gtk_cell_renderer_text_new(),
      "text", table_label_str_col, NULL);
  gtk_tree_view_append_column( GTK_TREE_VIEW(table_view), col );
  
  
  dm_widget = gtk_scrolled_window_new(0, 0);  // parent owns the dm_widget member
  gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW(dm_widget), GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
  gtk_scrolled_window_add_with_viewport( GTK_SCROLLED_WINDOW( dm_widget ), table_view );
  
  widget::init_gui();
}

void listbox::update_table_store( void )
{
  GtkTreeIter tree_ii;
  
  std::vector< std::string >::iterator ii, ii_end;
  std::string label_str;

  gtk_list_store_clear( dm_table_store );

  ii_end = dm_entry_list.end(); 
  for ( ii = dm_entry_list.begin(); ii != ii_end; ++ii )
  {
    label_str = (*ii);
    gtk_list_store_append( dm_table_store, &tree_ii ); 
    gtk_list_store_set( dm_table_store, &tree_ii, table_label_str_col, label_str.c_str(), -1 );
  }
}

void listbox::h_on_table_selection_change( GtkTreeSelection *selection, listbox *here )
{
  here->handle_table_selection( selection );
}

void listbox::handle_table_selection( GtkTreeSelection *selection )
{
  GtkTreeIter  iter;
  gboolean     valid;
  int num_sel, sel_idx, i;
  
  // set iter to top of table, get number of selected entries
  valid = gtk_tree_model_get_iter_first( GTK_TREE_MODEL(dm_table_store), &iter );
  num_sel = 0;
  while ( valid )
  {
    if ( gtk_tree_selection_iter_is_selected( selection, &iter) )
      num_sel++;

    valid = gtk_tree_model_iter_next( GTK_TREE_MODEL(dm_table_store), &iter );
  }

  // set iter to top of table, set entries index number in selection vector
  
  valid = gtk_tree_model_get_iter_first( GTK_TREE_MODEL(dm_table_store), &iter );
  sel_idx = 0;
  dm_selection.resize(num_sel);
  i = 0;
  while ( valid )
  {
    if ( gtk_tree_selection_iter_is_selected( selection, &iter) )
      dm_selection[i++] = sel_idx;

    sel_idx++;
    valid = gtk_tree_model_iter_next( GTK_TREE_MODEL(dm_table_store), &iter );
  }

}


