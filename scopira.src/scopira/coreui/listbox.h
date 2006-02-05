
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

#ifndef __INCLUDED__SCOPIRA_COREUI_LISTBOX_H__
#define __INCLUDED__SCOPIRA_COREUI_LISTBOX_H__

#include <vector>

#include <gtk/gtk.h>

#include <scopira/basekit/narray.h>
#include <scopira/coreui/widget.h>
#include <scopira/coreui/export.h>

namespace scopira
{
  namespace coreui
  {
    class listbox_reactor_i;
    class listbox;
  }
}

/**
 * Listens to box_list events.
 * 
 * @author Rodrigo Vivanco
 */
class scopira::coreui::listbox_reactor_i
{
  public:
    SCOPIRAUI_EXPORT virtual ~listbox_reactor_i() { }
    SCOPIRAUI_EXPORT virtual void react_selection_change( scopira::coreui::listbox *source, scopira::basekit::intvec_o selection ) = 0;
};

/**
 * Simple list box widget which allows multiple selection of text entries, 
 * based on GTK+ table (list) store.
 *
 * @author Rodrigo Vivanco
 */
class scopira::coreui::listbox : public scopira::coreui::widget
{
  protected:
    enum {
      table_label_str_col, // entry's label, col # '0' in table
      table_num_cols  // total number of fields in list-store
    };

    GtkListStore *dm_table_store;

    std::vector< std::string > dm_entry_list;
    scopira::basekit::narray<int> dm_selection;     
    
    // listener of events, if any
    scopira::coreui::listbox_reactor_i *dm_reactor;
    
  public:
    /// constrcutor
    SCOPIRAUI_EXPORT listbox(void);
    
    /**
     * Adds an entry
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT void add_entry(const std::string &entry);
    
    void set_reactor( scopira::coreui::listbox_reactor_i *reac ) { dm_reactor = reac; }

    // index numbers of current selection
    const scopira::basekit::narray<int> & get_selection(void) const { return dm_selection; }

    const std::vector< std::string >& get_entries( void ) const { return dm_entry_list; }

  protected:
    SCOPIRAUI_EXPORT void init_gui(void);

    SCOPIRAUI_EXPORT void update_table_store(void);

    SCOPIRAUI_EXPORT static void h_on_table_selection_change(GtkTreeSelection *selection, scopira::coreui::listbox *data);
    SCOPIRAUI_EXPORT void handle_table_selection( GtkTreeSelection *selection );
};

#endif

