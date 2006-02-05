
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

#include <scopira/uikit/color.h>

#include <scopira/core/register.h>
#include <scopira/coreui/canvas.h>
#include <scopira/coreui/viewmenu.h>
#include <scopira/basekit/color.h>
#include <scopira/uikit/models.h>

#include <scopira/tool/output.h>

using namespace std;
using namespace scopira::core;
using namespace scopira::tool;
using namespace scopira::basekit;
using namespace scopira::coreui;
using namespace scopira::uikit;

//BBtargets libscopiraui.so

//
//
// register
//
//

static register_view<make_gray_palette_v> p1(
  "scopira::uikit::make_gray_palette_v",
  "scopira::uikit::palette_m", 
  "Make Palette/Gray",
  no_uimode_c);
  
static register_view<make_red_palette_v> p2(
  "scopira::uikit::make_red_palette_v",
  "scopira::uikit::palette_m", 
  "Make Palette/Red",
  no_uimode_c);
  
static register_view<make_green_palette_v> p3(
  "scopira::uikit::make_green_palette_v",
  "scopira::uikit::palette_m", 
  "Make Palette/Green",
  no_uimode_c);
  
static register_view<make_blue_palette_v> p4(
  "scopira::uikit::make_blue_palette_v",
  "scopira::uikit::palette_m", 
  "Make Palette/Blue",
  no_uimode_c);
  
static register_view<make_pick_palette_v> p5(
  "scopira::uikit::make_pick_palette_v",
  "scopira::uikit::palette_m", 
  "Make Palette/Pick",
  no_uimode_c);
  
static register_view<make_heat_palette_v> p6(
  "scopira::uikit::make_heat_palette_v",
  "scopira::uikit::palette_m", 
  "Make Palette/Heat",
  no_uimode_c);
  
  
static register_view<make_rainbow_palette_v> p7(
  "scopira::uikit::make_rainbow_palette_v",
  "scopira::uikit::palette_m", 
  "Make Palette/Rainbow",
  no_uimode_c);
  
static register_view<make_bluered_palette_v> p8(
  "scopira::uikit::make_bluered_palette_v",
  "scopira::uikit::palette_m", 
  "Make Palette/Bluered",
  no_uimode_c);
  
static register_view<make_bone_palette_v> p9(
  "scopira::uikit::make_bone_palette_v",
  "scopira::uikit::palette_m", 
  "Make Palette/Bone",
  no_uimode_c);
  
static register_view<make_copper_palette_v> p10(
  "scopira::uikit::make_copper_palette_v",
  "scopira::uikit::palette_m", 
  "Make Palette/Copper",
  no_uimode_c);
  
  
static register_view<make_spectra_palette_v> p11(
  "scopira::uikit::make_spectra_palette_v",
  "scopira::uikit::palette_m", 
  "Make Palette/Spectra",
  no_uimode_c);

    
static register_view<colorpalette_v> r2(
  "scopira::uikit::colorpalette_v",
  "scopira::uikit::palette_m", 
  "Edit Palette");


//
//
// make_palette_base
//
//
void make_palette_base::bind_model(scopira::core::model_i *sus)
{
  if (!sus)
    return;

  palette_m *palette;
  std::string desc;

  palette = dynamic_cast<palette_m*>(sus);
  assert(palette);


  // make the palette via the virtual method
  desc = make_palette(palette->pm_array->all_slice()); 
  palette->notify_views(this);
}

                         
std::string make_gray_palette_v::make_palette(const scopira::basekit::nslice<int> &palette)
{
  make_gray_palette(palette);
  return "Gray";
}

std::string make_red_palette_v::make_palette(const scopira::basekit::nslice<int> &palette)
{
  make_red_palette(palette);
  return "Red";
}

std::string make_green_palette_v::make_palette(const scopira::basekit::nslice<int> &palette)
{
  make_green_palette(palette);
  return "Green";
}

std::string make_blue_palette_v::make_palette(const scopira::basekit::nslice<int> &palette)
{
  make_blue_palette(palette);
  return "Blue";
}

std::string make_pick_palette_v::make_palette(const scopira::basekit::nslice<int> &palette)
{
  make_pick_palette(palette);
  return "Pick";
}

std::string make_heat_palette_v::make_palette(const scopira::basekit::nslice<int> &palette)
{
  make_heat_palette(palette);
  return "Heat";
}

std::string make_rainbow_palette_v::make_palette(const scopira::basekit::nslice<int> &palette)
{
  make_rainbow_palette(palette);
  return "Rainbow";
}

std::string make_bluered_palette_v::make_palette(const scopira::basekit::nslice<int> &palette)
{
  make_bluered_palette(palette);
  return "Bluered";
}

std::string make_bone_palette_v::make_palette(const scopira::basekit::nslice<int> &palette)
{
  make_bone_palette(palette);
  return "Bone";
}

std::string make_copper_palette_v::make_palette(const scopira::basekit::nslice<int> &palette)
{
  make_copper_palette(palette);
  return "Copper";
}

std::string make_spectra_palette_v::make_palette(const scopira::basekit::nslice<int> &palette)
{
  make_spectra_palette(palette);
  return "Spectra";
}

//
//
// colorwindow
//
//

colorwindow::colorwindow(const std::string &title)
  : window(title)     // call protoected constructor
{
  init_gui();
}

void colorwindow::set_selected_color(GdkColor color)
{ 
	gtk_color_selection_set_previous_color(dm_colorsel, &color);
  gtk_color_selection_set_current_color(dm_colorsel, &color);
}

GdkColor colorwindow::get_selected_color(void) const
{
	GdkColor color;
	
	gtk_color_selection_get_current_color(dm_colorsel, &color);
	
	return color;
}

  
void colorwindow::init_gui()
{
	dm_widget = gtk_color_selection_dialog_new(dm_title.c_str());	
  
	dm_colorsel = GTK_COLOR_SELECTION(GTK_COLOR_SELECTION_DIALOG(dm_widget)->colorsel);
	gtk_color_selection_set_has_opacity_control(GTK_COLOR_SELECTION(dm_colorsel), FALSE);
	gtk_color_selection_set_has_palette(GTK_COLOR_SELECTION(dm_colorsel), TRUE);
	
  g_signal_connect(G_OBJECT(GTK_COLOR_SELECTION_DIALOG(dm_widget)->cancel_button), "clicked",G_CALLBACK(h_cancel), this);
  set_attach_gtk(GTK_WIDGET(GTK_COLOR_SELECTION_DIALOG(dm_widget)->ok_button));
	
  window::init_gui();
}

void colorwindow::ok_connect(GCallback callback, gpointer func_data)
{
  g_signal_connect(G_OBJECT(GTK_COLOR_SELECTION_DIALOG(dm_widget)->ok_button), "clicked",
    callback, func_data);
}

void colorwindow::h_cancel(GtkWidget *widget, colorwindow *data)
{
  assert(data->is_alive_object());
  data->on_destroy();
}


 

colorbutton::colorbutton()
  : dm_color(16777215), dm_dirty(true)
{
  init_gui();
  gtk_widget_show(dm_widget);
}

void colorbutton::handle_init(scopira::coreui::widget_canvas &v)
{
  dm_pen.set_widget(this);
  dm_dirty = true;
}

void colorbutton::handle_resize(scopira::coreui::widget_canvas &v)
{
  dm_dirty = true;
  paint(v);
}

void colorbutton::handle_repaint(scopira::coreui::widget_canvas &v)
{
  paint(v);
}

void colorbutton::handle_press(scopira::coreui::widget_canvas &v, const scopira::coreui::mouse_event &mevt)
{
  if (!mevt.press || mevt.double_press)
    return;
  
  if (dm_color_reactor) 
    dm_color_reactor->react_colorbutton(this, dm_color);    
}

void colorbutton::set_packed_color(const int c)
{
  dm_color = c;
  set_gdk_color(packed_to_gdkcolor(c));
  dm_dirty = true;
  request_redraw();
}

void colorbutton::paint(scopira::coreui::widget_canvas &v)
{
  // Set new bg color
  dm_pen.set_foreground(dm_color);
  v.clear(dm_pen);

  // draw border
  dm_pen.set_foreground(0XBCBCBC);
  v.draw_rectangle(dm_pen, false,0, 0, v.width()-1, v.height()-1);
}

//
//
// colorpalette
//
//

colorpalette::colorpalette(void)
{
  dm_colorwin=0;
  dm_palette_data.clear();

  dm_tile_width = dm_tile_height = 18;
  init_gui();   
}

colorpalette::~colorpalette(void)
{
  if (dm_colorwin) {
    dm_colorwin->remove_destroy_reactor(this); 
    dm_colorwin->on_destroy();
  }  
}


void colorpalette::react_spinbutton(scopira::coreui::spinbutton *source, int intval, double doubleval)
{  
  if (intval != dm_palette_data.size()) {
    resize_palette();
    // update
    update();
  }
}

void colorpalette::react_vcrbutton(scopira::uikit::vcrbutton *source, int val)
{
  clear_selection();
  update_gui();
}

void colorpalette::react_colorbutton(scopira::uikit::colorbutton *source, int newcol)
{   
  int index;
  assert(source);
  
   // set new tile selection
  index =get_tile_index(source);
  set_tile_selection(index);
    
  if (dm_colorwin) {
    //open edit color window    
    dm_colorwin->on_focus(true);
  }
   else {
     // popup the color selection dialog
     dm_colorwin = new colorwindow("Color Selection");
     dm_colorwin->ok_connect(GCallback(h_colorwindow_ok), this);
     dm_colorwin->add_destroy_reactor(this);
     // show the color selection dialog
     dm_colorwin->show_all();
   }
   dm_colorwin->set_selected_color(source->get_gdk_color());
}


void colorpalette::react_destroy(scopira::tool::object *source)
{  
  dm_colorwin=0;
}


// Updates after a new palette data is set 
void colorpalette::update(void)
{
  set_size(dm_palette_data.size());
  dm_dirty = true;
  update_gui();
}


void colorpalette::resize_palette(void)
{
  narray<int> oldpal;
  int i;

  oldpal.copy(dm_palette_data);

  // update the size of  dm_palette_data.
  dm_palette_data.resize(get_size());

  // copy in the old colours
  for (i=0; i<oldpal.size() && i<dm_palette_data.size(); ++i)
    dm_palette_data[i] = oldpal[i];
  
  // fill the rest with white
  fill_color_gradient(dm_palette_data.xslice(i, dm_palette_data.size()-i), white_c,white_c);  
}


void colorpalette::create_table(int start_row, int start_column, int last_row, int last_column)
{
  int i, r, c;
	count_ptr<colorbutton> c_edit;

  //create rows
  for (r=start_row; r<last_row; r++) {
    for (c=start_column; c<last_column; c++) {
      //create color edit
      c_edit = new colorbutton();
      i = dm_start+(r*dm_page_columns + c);
      //set color
      c_edit->set_packed_color(dm_palette_data[i]);
      c_edit->set_colorbutton_reactor(this);
      //add the cell
      add_cell(c_edit.get(), c,r);
    }
    start_column = 0;
  }
}

void colorpalette::h_colorwindow_ok(GtkWidget *widget, colorpalette *data)
{
  int start,index;
  GdkColor new_val;
  intvec_o selection;
  
  colorpalette &here = *data;
  colorwindow &cswin =  * dynamic_cast<colorwindow*>(widget::get_attach_gtk(widget));

  assert(here.is_alive_object());
  assert(cswin.is_alive_object());
                                 
  //new gtk color
  new_val = cswin.get_selected_color();
  // index in dm_tiles
  index = (here.get_selection())[0];
  // start index into palette data
  start = here.get_start_index();
  
  // update view and palette data 
  here.dm_palette_data[index+start] = gdkcolor_to_packed(new_val);
  here.update_gui();
   
  // kill the color selection dialog
  cswin.on_destroy();
}                                                                       


//
//
// colorpalette_v
//
//

colorpalette_v::colorpalette_v(void)
  :dm_model(this)
{
  set_view_title("Colors Palette");
  init_gui();   
}


void colorpalette_v::init_gui(void)
{	
  count_ptr<box_layout > top_box;
  count_ptr<buttonbox_layout > button_panel;
   
  top_box = new box_layout(false,false, 2); //bool horiz, bool homogeneous, int spacing = 0
  
  // palette
  dm_palette = new colorpalette();
  top_box->add_widget(dm_palette.get(),true,true);
  
  // buttons layout
  button_panel = new buttonbox_layout(true); // (horiz, layoutstyle = style_end_c)
  top_box->add_widget_end(button_panel.get(), false,false);

  // palettes button
  dm_palettes_button = new view_button("Palettes", dm_model);
  button_panel->add_widget(dm_palettes_button.get(), false, false);  

  
  viewwidget::init_gui(top_box.get(), button_ok_c|button_apply_c|button_cancel_c);
}


void colorpalette_v::bind_model(scopira::core::model_i *sus)
{
    if (!sus)
    return;
  
  dm_model = dynamic_cast<palette_m*>(sus);
}

void colorpalette_v::react_model_update(scopira::core::model_i *sus, scopira::core::view_i *src)
{
    if (dm_model.get()) {
    dm_palette->set_values(dm_model->pm_array.ref());
    // update
    dm_palette->update();
  }
}


void colorpalette_v::react_button(scopira::coreui::button *source, int actionid)
{
  if (actionid == action_apply_c) 
    // signal to model
    on_change();

  viewwidget::react_button(source, actionid);
}


void colorpalette_v::on_change(void)
{ 
  // if have a model, copy info and notify
  if (dm_model.get()) {
    // copy the data over and signal the inspectors
    dm_model->pm_array =  new narray_o<int,1> (dm_palette->get_values());
    dm_model->dm_action = 104;
    // notify
    dm_model->notify_views(this);
  }
}
