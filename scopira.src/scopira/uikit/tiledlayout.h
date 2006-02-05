
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

#ifndef __INCLUDED__SCOPIRA_UIKIT_TILEDLAYOUT_H__
#define __INCLUDED__SCOPIRA_UIKIT_TILEDLAYOUT_H__

#include <vector>
#include <list>

#include <scopira/basekit/narray.h>
#include <scopira/coreui/button.h>
#include <scopira/coreui/label.h>
#include <scopira/coreui/layout.h>
#include <scopira/coreui/widget.h>
#include <scopira/coreui/spinbutton.h>
#include <scopira/coreui/slider.h>
#include <scopira/coreui/export.h>
#include <scopira/uikit/vcrbutton.h>

namespace scopira
{
  namespace uikit
  {
    class tiled_layout;
    class zoomed_tiled_layout;
    
    class select_tiled_layout;
    class border_select_tiled_layout;

    enum { 
      select_mode = 55,
      edit_mode,
      selection_one,
      selection_multiple
    };
  }
}

/**
* A tiled display of other proponent widgets
*
* @author Marina Mandelzweig
*/
class scopira::uikit::tiled_layout : public virtual scopira::coreui::widget,
  public virtual scopira::coreui::spinbutton_reactor_i,
  public virtual scopira::uikit::vcrbutton_reactor_i
{
  protected:    
    /// list of display tiles   
    std::vector< scopira::tool::count_ptr<widget> > dm_tiles;
    
    scopira::tool::count_ptr<scopira::coreui::box_layout> dm_top_box;    
    
    /// number of fields
    scopira::tool::count_ptr<scopira::coreui::spinbutton> dm_spinner;
    /// page scrooling
    scopira::tool::count_ptr<scopira::uikit::vcrbutton> dm_vcr;
    
    /// holds the tiled display table
    scopira::tool::count_ptr<scopira::coreui::box_layout> dm_palette_box,dm_tiled_view_box;
    /// table widget to hold tiled display
    scopira::tool::count_ptr<scopira::coreui::grid_layout> dm_table;

    // holds bottom edit gui
    scopira::tool::count_ptr<scopira::coreui::box_layout> dm_spin_vcr_hbox;
    /// tiled display title
    std::string dm_title;
    /// title frame
    scopira::tool::count_ptr<scopira::coreui::widget> dm_frame;
        
    /// edit or selection mode
    int dm_mode;  
    /// selection mode
    int dm_select_mode;
    /// number of widgets
    int dm_numwidgets; 
    
    /// columns, rows
    int dm_page_rows, dm_page_columns;
    int dm_start;
    
    /// tiled widgets width and height
    int dm_page_width, dm_page_height;
    int dm_tile_width, dm_tile_height;
    /// space between tiles
    int dm_hspace, dm_vspace;
    ///  width and height of the full tiled table
    int dm_back_width, dm_back_height;
    int dm_min_back_width, dm_min_back_height;
    int palette_changed_id;
    bool dm_dirty;    

  public:
		/// constrcutor
    SCOPIRAUI_EXPORT tiled_layout(void);
    
    /// does an update_gui() by default
    SCOPIRAUI_EXPORT virtual void react_spinbutton(scopira::coreui::spinbutton *source, int intval, double doubleval);
    /// does an update_gui() by default
    SCOPIRAUI_EXPORT virtual void react_vcrbutton(scopira::uikit::vcrbutton *source, int val);

    /// sets number of tiles
    SCOPIRAUI_EXPORT void set_size(int n);
    /// gets the number of tiles
    SCOPIRAUI_EXPORT int get_size(void) const;
    
    /// sets min display size  
    SCOPIRAUI_EXPORT void set_min_display_size(int w, int h);
    /// gets the display  width
    SCOPIRAUI_EXPORT int get_display_width(void);
    /// gets the display height
    SCOPIRAUI_EXPORT int get_display_height(void);

    /// sets mode (edit/select)
    SCOPIRAUI_EXPORT void set_mode(const int m);
    /// gets (edit/select)
    SCOPIRAUI_EXPORT int get_mode(void) const { return dm_mode; }
    
    // widget resized
    SCOPIRAUI_EXPORT void handle_resize(void);
    
    /// get the index of widget w in dm_tiles
    SCOPIRAUI_EXPORT int get_tile_index(scopira::coreui::widget *w);
    /// get the tiles star index within the dm_palette_data
    int get_start_index(void) { return dm_start;}
    
 protected:
    SCOPIRAUI_EXPORT void init_gui(void);
    
    //set view title
    SCOPIRAUI_EXPORT void set_title(const std::string title);
    //get title
    const std::string & get_title(void) { return dm_title; }  

    SCOPIRAUI_EXPORT void update_title_view(const std::string title);
    
    // tiled view gui
    SCOPIRAUI_EXPORT void init_tiled_gui(void);
    // create the palette box to hold the tiled view
    SCOPIRAUI_EXPORT void make_palette_box(void);
    // includes spinner and vcrtoolbar
    SCOPIRAUI_EXPORT void init_spin_vcr_gui(void);
    //  ipdate spinner
    SCOPIRAUI_EXPORT void update_edit_gui(void);
    
    /// prepare data
		SCOPIRAUI_EXPORT bool prepare(void);
    /// check if have data to display
    SCOPIRAUI_EXPORT bool check_empty(void);

    /// show the empty label
    SCOPIRAUI_EXPORT void show_empty(const std::string &message);

    SCOPIRAUI_EXPORT virtual void update_gui(void);
    /// resize the tiled table, by default does nothing
    SCOPIRAUI_EXPORT void resize_tiles_info(void);
    
    // calculate width, height and number of columns and rows taht fit the display
    SCOPIRAUI_EXPORT void calc_dimensions(void);
    // update vcr values    
    SCOPIRAUI_EXPORT void update_vcr(void);

    /// create the tileds table
    SCOPIRAUI_EXPORT virtual void create_table(int start_row, int start_column, int last_row, int last_column)=0;
    
    /// adds a cell to the table
    SCOPIRAUI_EXPORT void add_cell(scopira::tool::count_ptr<widget> child, int left_attach, int top_attach);    
    ///clear the current table view
    SCOPIRAUI_EXPORT void clear_view();
    
  private:
    /// called on resize container holding tiled table
    static void h_size_allocate(GtkWidget *widget, GtkAllocation *allocation, gpointer data);
};

/**
* A tiled display of other proponent widgets with selection management
*
* @author Marina Mandelzweig
*/
class scopira::uikit::select_tiled_layout : public scopira::uikit::tiled_layout
{
  protected:
    
    /// list of selected tiles index numbers    
    typedef std::list<int> select_t;
    select_t dm_selected;
  
  public:    
    // sets the selection
    SCOPIRAUI_EXPORT void set_selection( const scopira::basekit::intvec_o &selection);
    /// gets the selection vector
    SCOPIRAUI_EXPORT scopira::basekit::intvec_o get_selection(void) const;
    
    /// clears the selection list
    SCOPIRAUI_EXPORT void clear_selection(void);
    
    // returns true if child is in selection list
    SCOPIRAUI_EXPORT bool is_selected(int child);
    /// Set one tile to be selected/un-selected and update the selection list
    SCOPIRAUI_EXPORT void set_tile_selection(int child); 
            
 protected:
    /// adds a cell to the table
    SCOPIRAUI_EXPORT void add_cell(scopira::tool::count_ptr<scopira::coreui::widget> child, int left_attach, int top_attach);
};

/**
* A tiled display of other proponent widgets with selection management, 
* Indicates selection by drawing a border around a selected widget 

*
* @author Marina Mandelzweig
*/
class scopira::uikit::border_select_tiled_layout : public scopira::uikit::select_tiled_layout
{
  protected:
    /// selected border color, default red 
    GdkColor dm_default_color, dm_selected_color;
    
  public:
    //constructor
    SCOPIRAUI_EXPORT border_select_tiled_layout(void);
    
    ///set selected border color
    SCOPIRAUI_EXPORT void set_selected_color(std::string color);
    
    /// clears the selection list
    SCOPIRAUI_EXPORT void clear_selection(void);
    /// set one tile to be selected/un-selected and update the selection list
    SCOPIRAUI_EXPORT void set_tile_selection(int child); 

  protected:
    SCOPIRAUI_EXPORT void init_gui(void);
    /// adds a cell to the table
    SCOPIRAUI_EXPORT void add_cell(scopira::tool::count_ptr<scopira::coreui::widget> child, int left_attach, int top_attach);

    /// draws a border around selected tiles in a dm_selected_color, otherwise border is white
    SCOPIRAUI_EXPORT void draw_selected(int tile);
};


#endif

