
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

#include <scopira/uikit/tiledlayout.h>

#include <algorithm>

#include <scopira/coreui/spinbutton.h>

#include <scopira/tool/output.h>

//BBtargets libscopiraui.so
using namespace std;
using namespace scopira::tool;
using namespace scopira::coreui;
using namespace scopira::uikit;
using namespace scopira::basekit;

//
//
// tiled_layout
//
//
tiled_layout::tiled_layout(void)
  : dm_mode(edit_mode), dm_select_mode(selection_one), dm_numwidgets(0), 
    dm_page_rows(0), dm_page_columns(0), dm_start(0), dm_page_width(470), 
    dm_page_height(130), dm_tile_width(50), dm_tile_height(50),dm_hspace(2), 
    dm_vspace(2), dm_back_width(0), dm_back_height(0),dm_min_back_width(470), 
    dm_min_back_height(130), palette_changed_id(-1),dm_dirty(false)
{ 
}

void tiled_layout::react_spinbutton(scopira::coreui::spinbutton *source, int intval, double doubleval)
{
  update_gui();
}

void tiled_layout::react_vcrbutton(scopira::uikit::vcrbutton *source, int val)
{ 
  update_gui();
}


void tiled_layout::set_size(int num)
{  
  dm_numwidgets = num;
  
  if (dm_mode == edit_mode  && !dm_spinner.is_null())
    dm_spinner->set_value(num);
}


int tiled_layout::get_size(void) const
{
  if (dm_mode == edit_mode && !dm_spinner.is_null()) 
    return dm_spinner->get_value_as_int();
  else
    return dm_numwidgets;
}

void tiled_layout::set_min_display_size(int w, int h)
{  
  dm_min_back_width = w;
  dm_min_back_height = h;
  
  if (dm_palette_box.get())
    dm_palette_box->set_min_size(dm_min_back_width, dm_min_back_height);
}


int tiled_layout::get_display_width(void)
{
  int width;
  
  width = 0;
  
  // get the width of the container holding the tiled widgets table
  if (dm_palette_box.get()) {
    width = dm_palette_box.get()->get_widget()->allocation.width;
    if (width <= 1)
      width = dm_palette_box.get()->get_widget()->requisition.width;
  }  
  if (width <= 1)
    width = dm_min_back_width;

  return width;
}

int tiled_layout::get_display_height(void)
{
  int height;
  
  height = 0;
  // get the height of the container holding the tiled widgets table
  if (dm_palette_box.get()) { 
    height = dm_palette_box.get()->get_widget()->allocation.height;
    if (height <= 1)
      height = dm_palette_box.get()->get_widget()->allocation.height;
  }
  if (height <= 1)
    height = dm_min_back_height;
    
  
  return height;
}

void tiled_layout::handle_resize()
{    
  // have new size
  if (get_display_width() != dm_page_width || get_display_height()!=dm_page_height) {  
    dm_dirty = true;
    update_gui();
  }
}
  

void tiled_layout::init_gui(void)
{	
  // main top layout
  dm_top_box = new box_layout(false,false, 2); //bool horiz, bool homogeneous, int spacing = 0
  
  init_tiled_gui();
 
  palette_changed_id =g_signal_connect(G_OBJECT(dm_palette_box.get()->get_widget()), "size_allocate", G_CALLBACK(h_size_allocate), this);
  init_spin_vcr_gui();
  
  widget::init_gui(dm_top_box.get());
}

void tiled_layout::init_tiled_gui(void)
{   
  GtkWidget *frame;
  
  frame = gtk_frame_new(NULL);
	gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_ETCHED_IN);
  
  dm_frame = new widget(frame);
  dm_top_box->add_widget(dm_frame.get(),true,true); //widget *wid, bool expand, bool fill, int padding = 0
  
  set_title(get_title());
  
  dm_tiled_view_box = new box_layout(true,false);//bool horiz, bool homogeneous, int spacing = 0
  gtk_container_add(GTK_CONTAINER(dm_frame.get()->get_widget()),dm_tiled_view_box.get()->get_widget());
  gtk_widget_show(dm_tiled_view_box.get()->get_widget()); 
 
  make_palette_box();
}


void tiled_layout::make_palette_box(void)
{
  // create vertical box for the tiled table
  dm_palette_box = new box_layout(false,false); //bool horiz, bool homogeneous, int spacing = 0
  dm_palette_box->set_min_size(dm_min_back_width, dm_min_back_height);
  dm_tiled_view_box->add_widget(dm_palette_box.get(),true,true);//widget *wid, bool expand, bool fill, int padding = 0

  //create tiled table
  dm_table = new grid_layout(dm_page_columns,dm_page_rows,false); //width(num columns),height (num rows),homo
  dm_table->set_border_size(0);
  dm_palette_box->add_widget(dm_table.get(),true,true);//widget *wid, bool expand, bool fill, int padding = 0
}


// Bottom spin vcr gui
void tiled_layout::init_spin_vcr_gui(void)
{ 
  GtkWidget *frame;
  count_ptr<widget> w;
  
  // spin vcr frame
  frame = gtk_frame_new(0);
  gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_ETCHED_IN);
  
  w = new widget(frame);
  dm_top_box->add_widget(w.get(),false,true,2); //widget *wid, bool expand, bool fill, int padding = 0
    
  // box for spin buttom and vcr
  dm_spin_vcr_hbox =  new box_layout(true,false);//bool horiz, bool homogeneous, int spacing = 0
  gtk_container_add(GTK_CONTAINER(w.get()->get_widget()),dm_spin_vcr_hbox.get()->get_widget());
  gtk_widget_show(dm_tiled_view_box.get()->get_widget()); 
  
  // vcr
  dm_vcr = new vcrbutton;
  dm_vcr->set_min(0);
  dm_vcr->set_max(0);
  dm_vcr->set_entry();
  dm_vcr->set_showmax(true);
  dm_vcr->set_vcrbutton_reactor(this);
  
  dm_spin_vcr_hbox->add_widget(dm_vcr.get(),false,false); //widget *wid, bool expand, bool fill, int padding = 0
  
  // spinner
  update_edit_gui();
}

void tiled_layout::update_edit_gui(void)
{ 
  // update bottom edit gui part
  if (dm_mode == edit_mode) {
    // if in edit mode, and no spinner, create it
    if (!dm_spinner.get()) {
      //edit area
      dm_spinner = new spinbutton(0, 10000, 4);
      dm_spinner->set_spinbutton_reactor(this);
      dm_spin_vcr_hbox->add_widget_end(dm_spinner.get(),false,false);
    }
  }
  else if (dm_spinner.get()) 
    // in select mode, and have a spinner, remove it
    dm_spin_vcr_hbox->remove_widget(dm_spinner.get());
}

// sets mode (edit/select)
void tiled_layout::set_mode(const int m) {
    dm_mode=m;
    update_edit_gui();
}


bool tiled_layout::prepare(void)
{ 
  if (dm_mode == edit_mode && !dm_spinner.get())
    return false;
  
  calc_dimensions();  
  update_vcr();
  
  dm_dirty = false;

  return true;
}


bool tiled_layout::check_empty(void)
{
  if (get_size() < 1) {
    show_empty("Input Size Too Small");
    return true;
  }
  else 
    set_title(get_title());
 
  
  return false;
}


void tiled_layout::show_empty(const string &message)
{
  // clear dm_tiles
  if (dm_tiles.size() > 0)
    clear_view(); 
  // clear vcr
  dm_vcr->set_min(0);
  dm_vcr->set_max(0);
  dm_vcr->set_value(0);
  
  update_title_view(message);
}


void tiled_layout::update_gui(void)
{ 
  //check if have data and prepare if nedded
  if (dm_dirty)
    if (check_empty() || !prepare())
      return;

  int page_num, min_row, max_row, min_col, max_col, 
  can_fit, created, should_create;
  
  // clear dm_tiles
  if (dm_tiles.size() > 0)
    clear_view();  

  page_num = dm_vcr->get_value();
  // vcr shows from 1 not zero
  if (page_num > 0)
    page_num--;
  
  dm_start = page_num*(dm_page_rows*dm_page_columns);
  
  //create all but last row, if last row is not full
  min_col = min_row = 0;
  max_col = dm_page_columns;
  max_row = min<int>((get_size()-dm_start)/dm_page_columns, dm_page_rows);
    
  create_table(min_row, min_col,max_row, max_col);
  
  //create last row, if last row is not full
  
  // how many can we fit in the display
  can_fit = dm_page_columns * dm_page_rows;
  // how many did we create in the display
  created = max_row * max_col;
  // do we have more data that has to be created
  should_create = get_size() - dm_start;
  
  if (can_fit > created && should_create > created) {
    min_col = 0;
    max_col = should_create - created;
  
    min_row = max_row;
    max_row = min_row+1;
        
    create_table(min_row, min_col,max_row, max_col);
  }  
}

void tiled_layout::resize_tiles_info(void)
{
}

void tiled_layout::calc_dimensions(void)
{  
  dm_page_width = get_display_width();
  dm_page_height = get_display_height();
  
  if (dm_tile_width+(2*dm_hspace) > dm_page_width)
    dm_page_columns = 1;
  else
    dm_page_columns = dm_page_width / (dm_tile_width+(2*dm_hspace));
  
  if (dm_tile_height+(2*dm_vspace) > dm_page_height)
    dm_page_rows = 1;
  else
    dm_page_rows = dm_page_height / (dm_tile_height+(2*dm_vspace));
  
  clear_view();
}


// update vcr
void tiled_layout::update_vcr(void)
{
  int max_rows, num_pages;
  
  max_rows = get_size() / dm_page_columns;
  if (get_size() % dm_page_columns > 0)
    max_rows++;  
    
  num_pages = max_rows / dm_page_rows;
  if (max_rows % dm_page_rows> 0)
    num_pages++; 
  
  dm_vcr->set_min(1);
  dm_vcr->set_max(num_pages);
  dm_vcr->set_value(1);
}
  

// Adds a cell to the table
void tiled_layout::add_cell(count_ptr<widget> child, int left_attach, int top_attach)
{  
  dm_tiles.push_back(child.get());
  
  dm_table->add_widget(child.get(),left_attach,top_attach,1,1,false,false,dm_hspace,dm_vspace);
  child->set_min_size(dm_tile_width, dm_tile_height);  
}    
                     

void tiled_layout::clear_view()
{
  // remove the table from its container parent (removed all child widgets i.e. tiles)
  dm_palette_box->remove_widget(dm_table.get());
  dm_tiles.erase(dm_tiles.begin(),dm_tiles.end());
  
  // make a new table with the right size
  dm_table = new grid_layout(dm_page_columns,dm_page_rows,false); //width(num columns),height (num rows),homo
  dm_table->set_border_size(0);
  dm_palette_box->add_widget(dm_table.get(),true,true);//widget *wid, bool expand, bool fill, int padding = 0  
}     


void tiled_layout::set_title(const string title)
{
  dm_title = title;
  update_title_view(title);
}
  

void tiled_layout::update_title_view(const string title)
{
  if (dm_frame.get()) {
    if (title != "")
      gtk_frame_set_label(GTK_FRAME(dm_frame.get()->get_widget()), title.c_str());
    else
      gtk_frame_set_label(GTK_FRAME(dm_frame.get()->get_widget()), NULL);      
  }
}

int tiled_layout::get_tile_index(widget *w)
{
  int index;
  vector< count_ptr<widget> >::iterator ii;
  
  // search dm_tiles for the tile index 
  for (index=0, ii=dm_tiles.begin(); ii != dm_tiles.end(); ++index,++ii) {
      if (ii->get() == w) 
        return index;
  }
  
  // not there
  return -1;
}

void tiled_layout::h_size_allocate(GtkWidget *widget, GtkAllocation *allocation, gpointer data)
{
  tiled_layout &here = *reinterpret_cast<tiled_layout*>(data); // get pointer to instance

  assert(data);
  assert(here.is_alive_object());
  
  here.handle_resize();
}


//
// select_tiled_layout
//

// sets the selection
void select_tiled_layout::set_selection( const intvec_o &selection)
{
  int i;
  
  for (i=0;i< selection.size(); i++) 
      dm_selected.push_back(selection[i]);
}



//return selection list in intvec_o
intvec_o select_tiled_layout::get_selection(void) const
{ 
  intvec_o selection;
  select_t::const_iterator jj;
  int i;
  
  selection.resize(dm_selected.size());
  for (i=0, jj = dm_selected.begin(); jj != dm_selected.end(); jj++, i++) 
      selection[i] = (*jj);
  
  return selection;
}
  
  
void select_tiled_layout::clear_selection(void)
{ 
  dm_selected.clear(); 
}


// Returns true if child is in selection list
bool select_tiled_layout::is_selected(int child)
{
  bool found;
  select_t::iterator ii;
   
  found = false;
  // search selection list for this tile
  ii = std::find(dm_selected.begin(), dm_selected.end(), child);
  if (ii != dm_selected.end())
    found = true;
  
  return found;
}

// Set one tile to be selected/un-selected and update the selection list
void select_tiled_layout::set_tile_selection(int child)
{ 
  // if selecting one at a time, clear selection list
  if (dm_select_mode == selection_one)
    clear_selection();

  // add the new tile to the selection list
  dm_selected.push_back(child);    
  
}


// Adds a cell to the table
void select_tiled_layout::add_cell(count_ptr<widget> child, int left_attach, int top_attach)
{
  GtkWidget *event_box;
  count_ptr<box_layout> box;
  count_ptr<widget> eb;
  
  event_box = gtk_event_box_new();
  eb = new widget(event_box);

  //add the event_box to the table
  dm_tiles.push_back(child.get());
  
  dm_table->add_widget(eb.get(),left_attach,top_attach,1,1,false,false,dm_hspace,dm_vspace);
  eb->set_min_size(dm_tile_width, dm_tile_height);
  
  // need a box in the event_box with a border to show the selected tile colored border
  box = new box_layout(true, false, 0);
  box.get()->set_border_size(1);  
  gtk_container_add(GTK_CONTAINER(eb.get()->get_widget()), box.get()->get_widget());

  
  // add the tile (widget) to the box
  box->add_widget(child.get(),true,true,0);
}    


//
// border_selection_tiled_widget
//
//
border_select_tiled_layout::border_select_tiled_layout(void)
{
  set_selected_color("red");
}

// Set selected border color
void border_select_tiled_layout::set_selected_color(string color)
{   
  gdk_color_parse(color.c_str(), &dm_selected_color);
}

void border_select_tiled_layout::set_tile_selection(int child)
{ 
  int i;  
  select_tiled_layout::set_tile_selection(child);
  
  //draw selection border
  for(i=0; i<dm_tiles.size(); i++)
    draw_selected(i);   
}

// Make all tiles un-selected
void border_select_tiled_layout::clear_selection(void)
{  
  int i;
   
  select_tiled_layout::clear_selection();
  
  //set the selection border
  for(i=0; i<dm_tiles.size(); i++)
    draw_selected(i);
}

void border_select_tiled_layout::init_gui(void)
{	
  select_tiled_layout::init_gui();
  dm_default_color = dm_top_box.get()->get_widget()->style->bg[GTK_STATE_NORMAL];
}

// Adds a cell to the table
void border_select_tiled_layout::add_cell(count_ptr<widget> child, int left_attach, int top_attach)
{ 
  select_tiled_layout::add_cell(child,left_attach,top_attach);
  draw_selected(dm_tiles.size()-1);
}


// Draws a border around selected tiles in a dm_selected_color, otherwise border is white
void border_select_tiled_layout::draw_selected(int child)
{
  GtkWidget *event_box;
  
  event_box = gtk_widget_get_parent(gtk_widget_get_parent(dm_tiles[child]->get_widget()));
  
  // drawing special border around selected tiles
  if (is_selected(child)) 
    //set background to dm_selected_color
    gtk_widget_modify_bg(event_box, GTK_STATE_NORMAL, &dm_selected_color);
  else
    //set background to default
    gtk_widget_modify_bg(event_box, GTK_STATE_NORMAL, &dm_default_color);
}
