
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

#include <scopira/uikit/plotter.h>

#include <math.h>

#include <scopira/coreui/label.h>
#include <scopira/basekit/vectormath.h>
#include <scopira/basekit/math.h>
#include <scopira/basekit/color.h>
#include <scopira/tool/output.h>

using namespace scopira::tool;
using namespace scopira::basekit;
using namespace scopira::coreui;
using namespace scopira::uikit;

//BBtargets libscopiraui.so

//
// plotter_options data object
//

const double scopira::uikit::plotter_options::nan_c = DBL_MAX;

plotter_options::plotter_options(size_t numplot, size_t plotlen)
  : dm_name("2D Plot"), dm_xlabel("X"), dm_ylabel("Y")
{
  dm_backcol = 0;
  dm_freey = true;
  dm_style = "line_plot";
  dm_ymin = dm_ymax = 0;
}

plotter_options::plotter_options(const plotter_options &src)
  : dm_name(src.dm_name), dm_xlabel(src.dm_xlabel), dm_ylabel(src.dm_ylabel)
{
  dm_backcol = src.dm_backcol;
  dm_ymin = src.dm_ymin;
  dm_ymax = src.dm_ymax;
  dm_freey = src.dm_freey;
  dm_style = "line_plot";
}

bool plotter_options::load(scopira::tool::iobjflow_i &in)
{
  return  in.read_string(dm_name) && in.read_string(dm_xlabel) && in.read_string(dm_ylabel)
    && in.read_double(dm_ymin) && in.read_double(dm_ymax) && in.read_bool(dm_freey);
}

void plotter_options::save(scopira::tool::oobjflow_i &out) const
{
  out.write_string(dm_name);
  out.write_string(dm_xlabel);
  out.write_string(dm_ylabel);
  out.write_double(dm_ymin);
  out.write_double(dm_ymax);
  out.write_bool(dm_freey);
}

void plotter_options::set_name(const std::string &name)
{
  dm_name = name;
}

void plotter_options::set_xlabel(const std::string &lab)
{
  dm_xlabel = lab;
}

void plotter_options::set_ylabel(const std::string &lab)
{
  dm_ylabel = lab;
}

void plotter_options::set_freey(void)
{
  dm_freey = true;
}

void plotter_options::set_yaxis_range(double ymin, double ymax)
{
  dm_freey = false;
  dm_ymin = ymin;
  dm_ymax = ymax;
}


///
/// plotter_properties_dialog
///

plotter_properties_dialog::plotter_properties_dialog(const std::string &title)
  : dialog(false), dm_colorwin(0)     // call protected constructor
{
  init_gui();
  set_title("Plotter Properties");
}

plotter_properties_dialog::~plotter_properties_dialog(void)
{
  if (dm_colorwin) {
    dm_colorwin->remove_destroy_reactor(this); 
    dm_colorwin->on_destroy();
  }  
}

void plotter_properties_dialog::init_gui(void)
{
  count_ptr<colorbutton> color_button;
  int i;
  
  dm_tab = new tab_layout;
  count_ptr<grid_layout> label_grid,range_grid,color_grid,lines_grid;
  
  dialog::init_gui();

  dm_tab = new tab_layout;
  
  label_grid = new grid_layout(2,5);
  dm_tab->add_widget(label_grid.get(), "Labels");
    
  dm_title = new entry;
  label_grid->add_widget(new label("Title:"),0,0,1,1,false,false);
  label_grid->add_widget(dm_title.get(),1,0,1,1,false,false);

  dm_footer = new entry;
  label_grid->add_widget(new label("Footer:"),0,1,1,1,false,false);
  label_grid->add_widget(dm_footer.get(),1,1,1,1,false,false);
  
  dm_xlabel = new entry;
  label_grid->add_widget(new label("X Label:"),0,2,1,1,false,false);
  label_grid->add_widget(dm_xlabel.get(),1,2,1,1,false,false);

  dm_ylabel = new entry;
  label_grid->add_widget(new label("Y Label:"),0,3,1,1,false,false);
  label_grid->add_widget(dm_ylabel.get(),1,3,1,1,false,false);

  dm_font_size = new spinbutton(0, 65536, 1, 0, true);;
  label_grid->add_widget(new label("Font Size:"),0,4,1,1,true,false);
  label_grid->add_widget(dm_font_size.get(),1,4,1,1,true,false);

  color_grid = new grid_layout(2,5);
  dm_tab->add_widget(color_grid.get(), "Colors");
  
  for (i=0; i< total_colors;i++) {
    color_button = new colorbutton();
    color_button->set_min_size(18,18);
    color_button->set_colorbutton_reactor(this);
    dm_colors.push_back(color_button.get());
  }
  color_grid->add_widget(new label("Backgound Color:"),0,0,1,1,false,false);
  color_grid->add_widget(dm_colors[back_col].get(),1,0,1,1,false,false);
  color_grid->add_widget(new label("Border Color:"),0,1,1,1,false,false);
  color_grid->add_widget(dm_colors[border_col].get(),1,1,1,1,false,false);
  color_grid->add_widget(new label("Title Color:"),0,2,1,1,false,false);
  color_grid->add_widget(dm_colors[title_col].get(),1,2,1,1,false,false);
  color_grid->add_widget(new label("Labels Color:"),0,3,1,1,false,false);
  color_grid->add_widget(dm_colors[label_col].get(),1,3,1,1,false,false);
  
  dm_auto_col = new checkbutton("Auto Colors");
  dm_auto_col->set_checkbutton_reactor(this);
  color_grid->add_widget(dm_auto_col.get(),0,4,2,1,false,false);

  range_grid = new grid_layout(2,5);
  dm_tab->add_widget(range_grid.get(), "Ranges");
   
  dm_xmin = new entry();
  range_grid->add_widget(new label("X Min:"),0,0,1,1,false,false);
  range_grid->add_widget(dm_xmin.get(),1,0,1,1,false,false);
  
  dm_xmax = new entry();
  range_grid->add_widget(new label("X Max:"),0,1,1,1,false,false);
  range_grid->add_widget(dm_xmax.get(),1,1,1,1,false,false);
  
  dm_ymin = new entry();
  range_grid->add_widget(new label("Y Min:"),0,2,1,1,false,false);
  range_grid->add_widget(dm_ymin.get(), 1,2,1,1,false,false);
  
  dm_ymax = new entry();
  range_grid->add_widget(new label("Y Max:"),0,3,1,1,false,false);
  range_grid->add_widget(dm_ymax.get(),1,3,1,1,false,false);
  
  dm_auto_size = new checkbutton("Auto Size");
  dm_auto_size->set_checkbutton_reactor(this);
  range_grid->add_widget(dm_auto_size.get(),0,4,1,1,true,false);

  lines_grid = new grid_layout(2,7);
  dm_tab->add_widget(lines_grid.get(), "Lines");
  
  lines_grid->add_widget(new label("Line 1:"),0,0,1,1,false,false);
  lines_grid->add_widget(dm_colors[plot_col1].get(), 1,0,1,1,false,false);
  lines_grid->add_widget(new label("Line 2:"),0,1,1,1,false,false);
  lines_grid->add_widget(dm_colors[plot_col2].get(), 1,1,1,1,false,false);
  lines_grid->add_widget(new label("Line 3:"),0,2,1,1,false,false);
  lines_grid->add_widget(dm_colors[plot_col3].get(), 1,2,1,1,false,false);
  lines_grid->add_widget(new label("Line 4:"),0,3,1,1,false,false);
  lines_grid->add_widget(dm_colors[plot_col4].get(), 1,3,1,1,false,false);
  lines_grid->add_widget(new label("Line 5:"),0,4,1,1,false,false);
  lines_grid->add_widget(dm_colors[plot_col5].get(), 1,4,1,1,false,false);
  lines_grid->add_widget(new label("Line 6:"),0,5,1,1,false,false);
  lines_grid->add_widget(dm_colors[plot_col6].get(), 1,5,1,1,false,false);
  
  dm_auto_line = new checkbutton("Auto Colors");
  dm_auto_line->set_checkbutton_reactor(this);
  lines_grid->add_widget(dm_auto_line.get(),0,6,1,1,false,false);
  
  gtk_box_pack_start(GTK_BOX(get_window_area()), dm_tab->get_widget(), TRUE,TRUE,0);

  init_cancel_button("Cancel");

  dm_applybutton = new button("Apply");
  gtk_box_pack_start(GTK_BOX(get_button_area()), dm_applybutton->get_widget(),TRUE,TRUE,10);

  dm_okbutton = init_ok_button("OK", false);
  set_attach_gtk(dm_okbutton);
}

void plotter_properties_dialog::ok_connect(GCallback callback, gpointer func_data)
{
  g_signal_connect(G_OBJECT(dm_okbutton), "clicked", callback, func_data);
}

void plotter_properties_dialog::apply_connect(GCallback callback, gpointer func_data)
{
  g_signal_connect(G_OBJECT(dm_applybutton->get_widget()), "clicked", callback, func_data);
}

void plotter_properties_dialog::react_destroy(scopira::tool::object *source)
{
  dm_colorwin =0; 
}

void plotter_properties_dialog::react_checkbutton(scopira::coreui::checkbutton *source, bool val)
{
  int i;
  
  // auto colours is checked
  if (source == dm_auto_col.get()) {
    if (val) {
      // auto colours turned on
      set_back_color(0xFFFFFF);
      set_border_color(0xFFFFFF);
      set_title_color(0x000000);
      set_label_color(0x000000);
      
      for(i=0; i<plot_col1; i++)
        dm_colors[i].get()->set_sensitive(false);
    } 
    else {
      // auto colours turned off
      for(i=0; i<plot_col1; i++)
        dm_colors[i].get()->set_sensitive(true);
    }
  } 
  // auto ranges is checked
  else if (source == dm_auto_size.get()) {
    if (val) {
      // auto ranges turned on
      set_xmin(dm_xmin_real);
      set_xmax(dm_xmax_real);
      set_ymin(dm_ymin_real);
      set_ymax(dm_ymax_real);
      dm_xmin->set_sensitive(false);
      dm_xmax->set_sensitive(false);
      dm_ymin->set_sensitive(false);
      dm_ymax->set_sensitive(false);
    } 
    else {
      // auto ranges turned off
      dm_xmin->set_sensitive(true);
      dm_xmax->set_sensitive(true);
      dm_ymin->set_sensitive(true);
      dm_ymax->set_sensitive(true);
    }
  } 
  // auto lines is checked
  else if (source == dm_auto_line.get()) {
    if (val) {
      // auto line turned on
      narray<int> colors(6);
      colors[0] = red_c;
      colors[1] = blue_c;
      colors[2] = green_c;
      colors[3] = yellow_c;
      colors[4] = cyan_c;
      colors[5] = purple_c;
      set_line_colors(colors);
      
      for(i=plot_col1; i<total_colors; i++)
        dm_colors[i].get()->set_sensitive(false);
    } 
    else {
      // auto lines turned off
      for(i=plot_col1; i<total_colors; i++)
        dm_colors[i].get()->set_sensitive(true);
    }
  }
}


void plotter_properties_dialog::react_colorbutton(scopira::uikit::colorbutton *source, int newcol)
{  
  assert(source);
  int i;

  for(i=0; i<total_colors; i++)
    if (dm_colors[i].get() == source)
      break;
  
  assert(i<total_colors);
  
  dm_selected_color = i;
  //open edit color window
  if (dm_colorwin)
    dm_colorwin->on_focus(true);
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


int plotter_properties_dialog::get_color(int c) const
{
  return dm_colors[c].get()->get_packed_color();
}

const std::string plotter_properties_dialog::get_title(void) const
{
  std::string title;
  dm_title->get_text(title);
  return title;
}

const std::string plotter_properties_dialog::get_footer(void) const
{
  std::string footer;
  dm_footer->get_text(footer);
  return footer;
}

const std::string plotter_properties_dialog::get_xlabel(void) const
{
  return dm_xlabel->get_text();
}

const std::string plotter_properties_dialog::get_ylabel(void) const
{
  return dm_ylabel->get_text();
}

int plotter_properties_dialog::get_font_size(void) const
{
  return dm_font_size->get_value_as_int();
}

int plotter_properties_dialog::get_back_color(void) const
{
  return dm_colors[back_col].get()->get_packed_color();
}

int plotter_properties_dialog::get_border_color(void) const
{
  return dm_colors[border_col].get()->get_packed_color();
}

int plotter_properties_dialog::get_title_color(void) const
{
  return dm_colors[title_col].get()->get_packed_color();
}

int plotter_properties_dialog::get_label_color(void) const
{
  return dm_colors[label_col].get()->get_packed_color();
}

double plotter_properties_dialog::get_xmin(void) const
{
  double x = scopira::tool::string_to_double(dm_xmin->get_text());
  if (x) return x;
  else return 0.0;
}

double plotter_properties_dialog::get_xmax(void) const
{
  double x = scopira::tool::string_to_double(dm_xmax->get_text());
  if (x) return x;
  else return 0.0;
}

double plotter_properties_dialog::get_ymin(void) const
{
  double y = scopira::tool::string_to_double(dm_ymin->get_text());
  if (y) return y;
  else return 0.0;
}

double plotter_properties_dialog::get_ymax(void) const
{
  double y = scopira::tool::string_to_double(dm_ymax->get_text());
  if (y) return y;
  else return 0.0;
}

narray<int> plotter_properties_dialog::get_line_colors(void) const
{
  narray<int> colors(6);
  colors[0] = dm_colors[plot_col1].get()->get_packed_color();
  colors[1] = dm_colors[plot_col2].get()->get_packed_color();
  colors[2] = dm_colors[plot_col3].get()->get_packed_color();
  colors[3] = dm_colors[plot_col4].get()->get_packed_color();
  colors[4] = dm_colors[plot_col5].get()->get_packed_color();
  colors[5] = dm_colors[plot_col6].get()->get_packed_color();
  return colors;
}


void plotter_properties_dialog::set_color(int c,int col)
{
  dm_colors[c].get()->set_packed_color(col);
}


void plotter_properties_dialog::set_title(const std::string &title)
{
  dm_title->set_text(title);
}

void plotter_properties_dialog::set_footer(const std::string &footer)
{
  dm_footer->set_text(footer);
}

void plotter_properties_dialog::set_xlabel(const std::string &label)
{
  dm_xlabel->set_text(label);
}

void plotter_properties_dialog::set_ylabel(const std::string &label)
{
  dm_ylabel->set_text(label);
}

void plotter_properties_dialog::set_font_size(int size)
{
  dm_font_size->set_value(size);
}

void plotter_properties_dialog::set_back_color(int col)
{
  dm_colors[back_col].get()->set_packed_color(col);
}

void plotter_properties_dialog::set_border_color(int col)
{
  dm_colors[border_col].get()->set_packed_color(col);
}

void plotter_properties_dialog::set_title_color(int col)
{
  dm_colors[title_col].get()->set_packed_color(col);
}

void plotter_properties_dialog::set_label_color(int col)
{
  dm_colors[label_col].get()->set_packed_color(col);
}

void plotter_properties_dialog::set_xmin(double x)
{
  dm_xmin->set_text(scopira::tool::double_to_string(x));  
}

void plotter_properties_dialog::set_xmax(double x)
{
  dm_xmax->set_text(scopira::tool::double_to_string(x));
}

void plotter_properties_dialog::set_ymin(double y)
{
  dm_ymin->set_text(scopira::tool::double_to_string(y));
}

void plotter_properties_dialog::set_ymax(double y)
{
  dm_ymax->set_text(scopira::tool::double_to_string(y));
}

void plotter_properties_dialog::set_line_colors(const narray<int> &colors)
{
  dm_colors[plot_col1].get()->set_packed_color(colors[0]);
  dm_colors[plot_col2].get()->set_packed_color(colors[1]);
  dm_colors[plot_col3].get()->set_packed_color(colors[2]);
  dm_colors[plot_col4].get()->set_packed_color(colors[3]);
  dm_colors[plot_col5].get()->set_packed_color(colors[4]);
  dm_colors[plot_col6].get()->set_packed_color(colors[5]);
}

void plotter_properties_dialog::set_default_range(double xmin, double xmax, double ymin, double ymax)
{
  dm_xmin_real = xmin;
  dm_xmax_real = xmax;
  dm_ymin_real = ymin;
  dm_ymax_real = ymax;
}

void plotter_properties_dialog::h_colorwindow_ok(GtkWidget *widget, plotter_properties_dialog *data)
{
  GdkColor new_val;
  
  plotter_properties_dialog &here = *data;
  colorwindow &cswin =  * dynamic_cast<colorwindow*>(widget::get_attach_gtk(widget));

  assert(here.is_alive_object());
  assert(cswin.is_alive_object());
                                 
  //new gtk color
  new_val = cswin.get_selected_color();  
  // update view  
  here.set_color(here.dm_selected_color,gdkcolor_to_packed(new_val));
   
  // kill the color selection dialog
  cswin.on_destroy();
}


//
// plotter_options_bar data object (just plotter_options but displays as bars instead of lines)
//

plotter_options_bar::plotter_options_bar(size_t numplots, size_t plotlen)
{
  dm_name = "2D Bar Plot";
  dm_xlabel = "X";
  dm_ylabel = "Y";
  dm_backcol = 0;
  dm_freey = true;
  dm_style = "bar_plot";
  dm_ymin = dm_ymax = 0;  
}

plotter_options_bar::plotter_options_bar(const plotter_options_bar &src)
{
  dm_name = src.dm_name;
  dm_xlabel = src.dm_xlabel;
  dm_ylabel = src.dm_ylabel;
  dm_backcol = src.dm_backcol;
  dm_ymin = src.dm_ymin;
  dm_freey = src.dm_freey;
  dm_style = "bar_plot";
}

///
/// plotter
///

plotter::plotter(bool scroll, bool zoom)
{
  dm_prop_window = 0;
  
  // initialize scroll coordinates and zoom
  dm_crd_hval = dm_crd_vval = 0;
  dm_zoom = 0;
  // add scrollbars and zoomer tab
  set_show_scroll(scroll);
  set_show_zoom(zoom);
  // data is ascending, data has changed
  dm_ascending = true;
  dm_dirty = true;
  dm_plot_style = line_plot_c;

  dm_show_plotname = true;  //RV default should be true
  set_options(new plotter_options); //RV, we need one to use x,y labels, etc...
  dm_pad_percent = 0; //RV, some goofball didn't add this line to this constructor... perhaps a "init_dm()" method is needed?
  dm_ymin = dm_ymax = 0;

  dm_xdigits = 0;
  dm_ydigits = 2;

  // draw the plot
  init_gui();
}

plotter::plotter(int)
{
  dm_prop_window=0;
  
  // initialize scroll coordinates and zoom
  dm_crd_hval = dm_crd_vval = 0;
  dm_zoom = 0;
  // data is ascending, data has changed
  dm_ascending = true;
  dm_dirty = true;
  dm_plot_style = line_plot_c;
  dm_show_plotname = true;  //RV default should be true
  dm_xdigits = 0;
  dm_ydigits = 2;
  dm_pad_percent = 0;
  dm_ymin = dm_ymax = 0;
}


void plotter::react_destroy(scopira::tool::object *source)
{
  dm_prop_window =0;  
}

void plotter::set_options(plotter_options *plot)
{
  dm_meta.set(plot);
  calc_y_range();
  dm_dirty = true;
}

void plotter::set_labels(narray_o<double,1> *labels)
{
  dm_labels.set(labels);
  dm_dirty = true;
}

void plotter::set_plot_data(narray_o<double,2> *data)
{
  dm_data.set(data);
  calc_y_range();
  dm_dirty = true;
}

void plotter::set_title(const std::string &title)
{
  dm_title = title;
}

void plotter::set_footer(const std::string &footer)
{
  dm_footer = footer;
}

void plotter::set_xlabel(const std::string &lab)
{
  if (!dm_meta.is_null()) dm_meta->set_xlabel(lab);
}

void plotter::set_ylabel(const std::string &lab)
{
  if (!dm_meta.is_null()) dm_meta->set_ylabel(lab);
}

void plotter::set_font_size(int size)
{
  dm_font.set_size(size);
}

void plotter::set_back_color(int col)
{
  dm_backcol = col;
}

void plotter::set_border_color(int col)
{
  dm_bordercol = col;
}

void plotter::set_title_color(int col)
{
  dm_titlecol = col;
}

void plotter::set_label_color(int col)
{
  dm_labelcol = col;
}

void plotter::set_plot_style(int style)
{
  dm_plot_style = style;
}

void plotter::set_xmin(double x)
{
  dm_xmin_clip = x;
  dm_dirty= true;
}

void plotter::set_xmax(double x)
{
  dm_xmax_clip = x;
  dm_dirty = true;
}

void plotter::reset_x_range( void )
{
  dm_xmin_clip = dm_xmin_orig;
  dm_xmax_clip = dm_xmax_orig;
  dm_dirty = true;
}

void plotter::set_ymin(double y)
{
  dm_ymin_clip = y;
  dm_dirty = true;
}

void plotter::set_ymax(double y)
{
  dm_ymax_clip = y;
  dm_dirty = true;
}

void plotter::reset_y_range( void )
{
  dm_ymin_clip = dm_ymin_orig;
  dm_ymax_clip = dm_ymax_orig;
  dm_dirty = true;
}

void plotter::set_pad_percent(double p)
{
  dm_pad_percent = p;
  dm_dirty = true;
}

void plotter::set_line_colors(narray<int> colors)
{
  if ( colors.size() > dm_plotcols.size() )
    dm_plotcols.resize( colors.size() );

  for ( int i=0; i<colors.size(); i++ )
    dm_plotcols[i] = colors[i];
}

void plotter::set_dialog_properties(void)
{
  // make sure we have a window
  if (dm_prop_window) {
    // set the title
    if (!dm_title.empty())
      dm_prop_window->set_title(dm_title);
    else if (!dm_meta.is_null())
        dm_prop_window->set_title(dm_meta->get_name());
    // set the footer
    if (!dm_footer.empty())
      dm_prop_window->set_footer(dm_footer);
    // set the labels
    if (!dm_meta.is_null()) {
      dm_prop_window->set_xlabel(dm_meta->get_xlabel());
      dm_prop_window->set_ylabel(dm_meta->get_ylabel());
    }
    // set the font size
    dm_prop_window->set_font_size(dm_font.get_size());
    // set the colours
    dm_prop_window->set_back_color(dm_backcol);
    dm_prop_window->set_border_color(dm_bordercol);
    dm_prop_window->set_title_color(dm_titlecol);
    dm_prop_window->set_label_color(dm_labelcol);
    // set the data ranges
    dm_prop_window->set_xmin(dm_xmin);
    dm_prop_window->set_xmax(dm_xmax);
    dm_prop_window->set_ymin(dm_ymin);
    dm_prop_window->set_ymax(dm_ymax);
    // set the line colors
    dm_prop_window->set_line_colors(dm_plotcols);
  }
}

void plotter::get_dialog_properties(void)
{
  std::string title, footer;
  
  // set the title
  title = dm_prop_window->get_title();
  set_title(title);

  // set the footer
  footer = dm_prop_window->get_footer();
  set_footer(footer);

  // make sure we have meta data
  if (dm_meta.is_null())
    set_options(new plotter_options);
  // set x label
  set_xlabel(dm_prop_window->get_xlabel());
  // set y label
  set_ylabel(dm_prop_window->get_ylabel());  
  // set font size
  set_font_size(dm_prop_window->get_font_size());
  // set background colour
  set_back_color(dm_prop_window->get_back_color());  
  // set border colour
  set_border_color(dm_prop_window->get_border_color());  
  //set title colour
  set_title_color(dm_prop_window->get_title_color());  
  // set label colour
  set_label_color(dm_prop_window->get_label_color());
  // get xmin to clip to
  set_xmin(dm_prop_window->get_xmin());
  // get xmax
  set_xmax(dm_prop_window->get_xmax());
  // get ymin
  set_ymin(dm_prop_window->get_ymin());
  // get ymax
  set_ymax(dm_prop_window->get_ymax());
  // get line colors
  set_line_colors(dm_prop_window->get_line_colors());
}

void plotter::add_stock_menu_items(scopira::coreui::menu_handler_base::popup_menu &menu)
{
  menu.add_separator();
  menu.add_item("Plot Options...", cmd_show_properties_c);
}

void plotter::popup_properties(void)
{
  if (dm_prop_window) {
    dm_prop_window->on_focus(true);
    return;
  }
  
  // create the window
  dm_prop_window = new plotter_properties_dialog("Plot Options...");
  dm_prop_window->ok_connect(GCallback(h_propwindow_ok), this);
  dm_prop_window->apply_connect(GCallback(h_propwindow_apply), this);
  dm_prop_window->add_destroy_reactor(this);
  // set default range values
  if (dm_first) {
    dm_xmin_orig = dm_xmin;
    dm_ymin_orig = dm_ymin;
    dm_xmax_orig = dm_xmax;
    dm_ymax_orig = dm_ymax;
    dm_first = false;
    dm_clip = true;
  }
  dm_prop_window->set_default_range(dm_xmin_orig, dm_xmax_orig, dm_ymin_orig, dm_ymax_orig);
  // set data for the window
  set_dialog_properties();
  // pop up the window
  dm_prop_window->show_all();
}

void plotter::convert_screento_xy(int screenx, int screeny, double &x, double &y,
  bool *xvalid, bool *yvalid)
{
  double relative_dist;

  if (xvalid)
    *xvalid = false;
  if (yvalid)
    *yvalid = false;

  // did we press inside the plot?
  if (screenx >= dm_base.x && screenx <= dm_base.x+dm_size.x) {
    if (xvalid)
      *xvalid = true;

    relative_dist = ((dm_crd_hval + screenx - dm_base.x)) / static_cast<double>(dm_back.width());

    if (dm_ascending)
      x = (relative_dist * (dm_xmax_tick - dm_xmin_tick)) + dm_xmin_tick;
    else
      x = dm_xmin_tick - (relative_dist * (dm_xmin_tick - dm_xmax_tick));
  }

  if (screeny >= dm_base.y || screeny <= dm_size.y+dm_base.y) {
    if (yvalid)
      *yvalid = true;

    relative_dist = (dm_back.height() - (dm_crd_vval + screeny - dm_base.y)) / static_cast<double>(dm_back.height());

    y = (relative_dist * (dm_ymax - dm_ymin)) + dm_ymin;
  }
}

void plotter::convert_xy_to_screen(double x, double y, int &screenx, int &screeny,
  bool *xvalid, bool *yvalid)
{
  double relative_dist;

  if (xvalid)
    *xvalid = false;
  if (yvalid)
    *yvalid = false;

  if (dm_data.is_null() || dm_data->empty())
    return;

  // do the X
  if (dm_ascending)
    relative_dist = (x - dm_xmin_tick) / (dm_xmax_tick - dm_xmin_tick);
  else
    relative_dist = (dm_xmin_tick - x) / (dm_xmin_tick - dm_xmax_tick);
  screenx = static_cast<int>((relative_dist *  dm_back.width()) - dm_crd_hval + dm_base.x);

  // now do the Y
  relative_dist = (y - dm_ymin_tick) / (dm_ymax_tick - dm_ymin_tick);
  screeny = static_cast<int>(dm_back.height() - relative_dist * dm_back.height() - dm_crd_vval + dm_base.y);

  // did we press inside the plot?
  if (xvalid && screenx>=dm_base.x && screenx<=(dm_base.x+dm_size.x))
    *xvalid = true;
  if (yvalid && screeny>=dm_base.y && screeny<=(dm_base.y+dm_size.y))
    *yvalid = true;
}

int plotter::find_label_index(double val) const
{
  if (!dm_labels.is_null() && !dm_labels->empty()) {
    int uu;
    if (dm_labels->get(0) < dm_labels->get(dm_labels->size()-1)) {
      // ascending labels
      for (uu=0; uu<dm_labels->size() && val>dm_labels->get(uu); ++uu)
        ; // no body
    } else {
      // descending labels
      for (uu=0; uu<dm_labels->size() && val<dm_labels->get(uu); ++uu)
        ;
    }
    return (uu>0) ? (uu-1) : 0;
  } else
    return static_cast<int>(val);
}

int plotter::calc_tick_marks(double min, double max, scopira::tool::basic_array<double> &tick_spaces)
{
  double s_dist;  //scaled distance between tick marks
  double scale; //actual tick mark distance is scale*sDist

  int i;
  int tot_ticks = 1;
  int des_num_ticks = 11; //kludge: seems to work reasonably well!
  int num_sp_factor = 4; //kludge: seems to work reasonably well!

  scopira::tool::basic_array<double> spacing_factor;

  spacing_factor.resize(4);
  spacing_factor[0]=1.0;
  spacing_factor[1]=2.0;
  spacing_factor[2]=5.0;
  spacing_factor[3]=10.0;

  tick_spaces.resize(3);
  tick_spaces[0] = min;
  tick_spaces[1] = max;
  tick_spaces[2] = 0.0;

  //compute scale and scaled distance
  s_dist = (max - min) / des_num_ticks;
  scale = pow(10.0, floor(log(s_dist) / log(10.0)));
  s_dist /= scale;

  if (s_dist < 1.0){
    s_dist *= 10.0;
    scale /= 10.0;
  }

  if (s_dist >= 10.0){
    s_dist /= 10.0;
    scale *= 10.0;
  }

  //determine which spacing factor to use
  for (i = 0; spacing_factor[i] < s_dist && i < num_sp_factor; i++);

  //compute "sensible" distance
  tick_spaces[2] = spacing_factor[i] * scale;

  //compute "sensible" tick mark positions
  tick_spaces[0] = ceil(min / tick_spaces[2]) * tick_spaces[2];

  for (tick_spaces[1] = tick_spaces[0]; tick_spaces[1] < max; tot_ticks++) {
      tick_spaces[1] = floor(tick_spaces[1] / tick_spaces[2] + 0.5) * tick_spaces[2];
      tick_spaces[1] += tick_spaces[2];
  }

  //adjust sensible min to avoid clipping min_val.
  if (tick_spaces[0] > min) {
    tick_spaces[0] -= tick_spaces[2];
    tot_ticks++;
  }

  return tot_ticks;
}

void plotter::calc_y_range(void)
{
  if (dm_data.is_null() || dm_data->begin()==dm_data->end())
    return;

  if ( !dm_meta.is_null() && !dm_meta->is_freey() ) 
  {
    dm_ymin = dm_meta->get_y_min();
    dm_ymax = dm_meta->get_y_max();
    
    dm_ymin_orig = dm_ymin;
    dm_ymax_orig = dm_ymax;
    
    return;
  }

  // we have to auto calc them
  min(dm_data.ref(), dm_ymin);
  max(dm_data.ref(), dm_ymax);

  // pad them, a little
  if (is_equal(dm_ymin, dm_ymax)) {
    dm_ymin -= 1;
    dm_ymax += 1;
  } 
  else if ( dm_plot_style != bar_plot_c ) 
  {
    if ( dm_pad_percent <= 0 ) return;

    double margin = (dm_ymax - dm_ymin)*dm_pad_percent;
    dm_ymin -= margin;
    dm_ymax += margin;
  }

  dm_ymin_orig = dm_ymin;
  dm_ymax_orig = dm_ymax;
}

void plotter::handle_init(widget_canvas &v, coord &crd)
{
  // set up contexts for drawing
  dm_frontpen.set_widget(this);
  dm_backpen.set_widget(this);
  dm_back.resize(this, v.width(), v.height());

  // set plot colours
  dm_titlecol = black_c;
  dm_labelcol = black_c;
  dm_backcol = white_c;
  dm_bordercol = white_c;
  dm_gridcol = 0xBDBDBD;
  dm_linecol = black_c;

  // set plot line colours
  dm_plotcols.resize(6);
  dm_plotcols[0] = red_c;
  dm_plotcols[1] = blue_c;
  dm_plotcols[2] = green_c;
  dm_plotcols[3] = yellow_c;
  dm_plotcols[4] = cyan_c;
  dm_plotcols[5] = purple_c;

  // initialize
  set_zoom(0);
  dm_mouse_down = false;
  dm_mouse_pan = false;
  dm_old_rect = false;
  dm_first = true;
  dm_clip = false;
}

void plotter::handle_resize(widget_canvas &v, coord &crd)
{
  dm_dirty = true;
  handle_repaint(v, crd);
}

void plotter::handle_repaint(widget_canvas &v, coord &crd)
{
  paint(v, crd);
}

void plotter::handle_scroll(widget_canvas &v, coord &crd)
{
  handle_repaint(v, crd);
}

void plotter::handle_zoom(int newfactor)
{
  dm_zoom = newfactor;
  request_resize();
}

void plotter::handle_press(widget_canvas &v, coord &crd, const mouse_event &mevt)
{
  double new_width, new_height;
  double center_x, center_y;
  int x_up, y_up, temp;
  int new_hval, new_vval;
  int check_zoom;
   double zoom_f;
   int i;
  
  // left-click - for box selection zooming
  
  // left-click down
   if (mevt.press && mevt.button == 0) {
     // compensate for white space around axes
    dm_down.x = mevt.x - dm_base.x;  
    dm_down.y = mevt.y - dm_base.y;
    
    // check for clicked outside display image
    if (dm_down.x < 0 || dm_down.x > dm_back.width())
      return;
    if (dm_down.y < 0 || dm_down.y > dm_back.height())
      return;
     
    // prepare for dragging
    dm_mouse_down = true;
    set_cursor(GDK_CROSSHAIR);
    
    return;
  }

  // left-click up
  if (!mevt.press && mevt.button == 0 && dm_mouse_down) {
    // find where cmouse was released
    x_up = mevt.x - dm_base.x;
    y_up = mevt.y - dm_base.y;
    dm_mouse_down = false;
    
    // prevent later division by 0
    if (dm_down.x == x_up || dm_down.y == y_up) {
      set_cursor(GDK_LEFT_PTR);
      return;
    }
    
    // make sure release point is greater than press point - if not then swap
    if (x_up < dm_down.x) {
      temp = x_up;
      x_up = dm_down.x;
      dm_down.x = temp;
    }
    if (y_up < dm_down.y)  {
      temp = y_up;
      y_up = dm_down.y;
      dm_down.y = temp;
    }
    
     //calc zoom factor by predicting full width
    new_width = x_up - dm_down.x;
    new_width = dm_back.width()*dm_back.width()/new_width;
    dm_zoom = static_cast<int>((new_width - dm_size.x)*1000/(dm_size.x*4.0));
    if (dm_zoom > 1000) dm_zoom = 1000;
    
    //calc zoom factor by predicting full height
    new_height = y_up - dm_down.y;
    new_height = dm_back.height()*dm_back.height()/new_height;
    check_zoom = static_cast<int>((new_height - dm_size.y)*1000/(dm_size.y*4.0));
    
    // set the best zoom factor so the screen will show the full selected area
    if (check_zoom < dm_zoom)
      dm_zoom = check_zoom;
    set_zoom(dm_zoom);

    // center of points
    center_x = (x_up + dm_down.x) / 2 + crd.hval();
    center_y = (y_up + dm_down.y) / 2 + crd.vval();
    
    //calc zoom
    zoom_f = 4.0*dm_zoom/1000;

    //new front img size
    dm_base.x = 45;
    dm_base.y = 25;
    // account for long numbers along y axis
    for (i=10; i<=100000 && i<=(abs(static_cast<int>(dm_ymax))); i=i*10)
      dm_base.x += 5;
    // account for negative numbers - give more space for a minus sign
    if (dm_ymax < 0 || dm_ymin < 0)
      dm_base.x += 5;

    // calculate size of plot to be drawn
    if ((v.width() > dm_base.x) && (v.height() >  dm_base.y)) {
      dm_size.x = v.width() - dm_base.x - (dm_base.x/2);
      dm_size.y = v.height() - (2*dm_base.y) - (dm_base.y/2);
    } else {
      dm_size.x = v.width();
      dm_size.y = v.height();
    }

    //new width and height of back img
    new_width = dm_size.x + dm_size.x*zoom_f;
    new_height = dm_size.y + dm_size.y*zoom_f;

    //new img center
    center_x *= new_width/dm_back.width();
    center_y *= new_height/dm_back.height();

    // resize back
    dm_back.resize(this,static_cast<int>(new_width), static_cast<int>(new_height));

    //set up new scroll bars
    new_hval = static_cast<int>(center_x - (dm_size.x/2));
    new_vval = static_cast<int>(center_y - (dm_size.y/2));

    if (new_hval < 0)
      new_hval = 0;
    else if (new_hval+dm_size.x > dm_back.width())
      new_hval = dm_back.width() - dm_size.x;
    if (new_vval < 0)
      new_vval = 0;
    else if (new_vval+dm_size.y > dm_back.height())
      new_vval = dm_back.height() - dm_size.y ;
  
    // set up scroll bars
    crd.set_hbounds(0, static_cast<float>(dm_back.width()), static_cast<float>(new_hval), static_cast<float>(dm_size.x / 4), static_cast<float>(dm_size.x / 2), static_cast<float>(dm_size.x));
    crd.set_vbounds(0, static_cast<float>(dm_back.height()), static_cast<float>(new_vval), static_cast<float>(dm_size.y / 4), static_cast<float>(dm_size.y / 2), static_cast<float>(dm_size.y));
  
    // repaint the zoomed area
     dm_dirty = false;
     dm_old_rect = false;
    paint(v, crd);
    set_cursor(GDK_LEFT_PTR);
    
    return;
  }
  
  // center-click - for panning
  
  // center-click down
  if (mevt.press && mevt.button == 1) {
    // ensure plot is zoomed in so there is somewhere to move
    if (dm_zoom <= 0)  
      return;
    
    // prepare for mouse panning
    dm_mouse_pan = true;
    dm_down.x = mevt.x;
    dm_down.y = mevt.y;
    set_cursor(GDK_HAND2);
    
    return;
  }
  
  // center-click up
  if (!mevt.press && mevt.button == 1) {
    // finish panning
    dm_mouse_pan = false;
    set_cursor(GDK_LEFT_PTR);
    
    return;
  }
  
  // right-click - for menu popup
  
  // right-click down
  if (mevt.press && mevt.button == 2) {
    // add the properties option and popup menu
    popup_menu dm_menu(this);
    add_stock_menu_items(dm_menu);
    dm_menu.popup();
    
    return;
  }
}

void plotter::handle_motion(widget_canvas &v, coord &crd, const mouse_event &mevt)
{
  int original_x, original_y;
  int new_hval, new_vval;
  int temp_x, temp_y;
  
  // create a new widget context to invert the pixels
  widget_context invert_context;
  invert_context.set_widget(this);
  GdkGC *invert_gc = invert_context.getContext();
  gdk_gc_set_function(invert_gc,GDK_INVERT);

  // left button - draw the selection box
  if (dm_mouse_down) {
    // keep track of initial mouse click point
    original_x = static_cast<int>(dm_down.x + dm_base.x);
    original_y = static_cast<int>(dm_down.y + dm_base.y);
    temp_x = static_cast<int>(mevt.x);
    temp_y = static_cast<int>(mevt.y);

    // make sure you are drawing the box left to right, up to down
    if (original_x > mevt.x) {
      temp_x = original_x;
      original_x = mevt.x;
    }
    if (original_y > mevt.y) {
      temp_y = original_y;
      original_y = mevt.y;
    }

    // draw over the old rectangle
    if (dm_old_rect)
      v.draw_rectangle(invert_context, false, dm_old_down.x, dm_old_down.y, dm_old_up.x-dm_old_down.x, dm_old_up.y-dm_old_down.y);

    // draw new rectangle
    v.draw_rectangle(invert_context, false, original_x, original_y, temp_x-original_x, temp_y-original_y);
      
    // save rectangle coords
    dm_old_down.x = original_x;
    dm_old_down.y = original_y;
    dm_old_up.x = temp_x;
    dm_old_up.y = temp_y;
    dm_old_rect = true;
  }
  
  // middle button, pan through image
  if (dm_mouse_pan) {
    // keep track of original click
    temp_x = static_cast<int>(mevt.x - dm_down.x);
    temp_y = static_cast<int>(mevt.y - dm_down.y);
    
    // calculate new coord values
    new_hval = crd.hval() - temp_x;
    new_vval = crd.vval() - temp_y;
    if (new_hval < 0)
      new_hval = 0;
    else if (new_hval+dm_size.x > dm_back.width())
      new_hval = dm_back.width() - dm_size.x;
    if (new_vval < 0)
      new_vval = 0;
    else if (new_vval+dm_size.y > dm_back.height())
      new_vval = dm_back.height() - dm_size.y ;
    
    // repaint at new position
    crd.set_hvalue(static_cast<float>(new_hval));
    crd.set_vvalue(static_cast<float>(new_vval));
    handle_repaint(v, crd);
    
    // keep track for next move
    dm_down.x = mevt.x;
    dm_down.y = mevt.y;
  }
}

void plotter::handle_menu(widget_canvas &v, coord &crd, intptr_t menuid)
{
  // only one option - show properties window
  if (menuid == cmd_show_properties_c) {
    popup_properties();
  }
}

void plotter::h_propwindow_ok(GtkWidget *widget, plotter *data)
{
  plotter &here = *data;
  assert(here.is_alive_object());
  
  plotter_properties_dialog &ppwin = * dynamic_cast<plotter_properties_dialog*>(widget::get_attach_gtk(widget));

  here.get_dialog_properties();
  here.update_gui();

  // kill the dialog
  ppwin.on_destroy();
}


void plotter::h_propwindow_apply(GtkWidget *widget, plotter *data)
{
  plotter &here = *data;
  assert(here.is_alive_object());

  here.get_dialog_properties();
  here.update_gui();
}

void plotter::prepare_axis(void)
{
  scopira::tool::basic_array<double> tick_dist;
  int count, i, j;
  double val, new_tick_y, new_tick_x, new_num_x;
  //min value
  double y_axis_min_val, x_axis_min_val;
  // max value
  double x_axis_max_val;
  //step value between ticks
  double x_tick_spacing_val, y_tick_spacing_val;
  //total number of ticks
  int y_num_ticks, x_num_ticks;

  //calculate y tick marks
  y_num_ticks  = calc_tick_marks(dm_ymin, dm_ymax, tick_dist);
  y_tick_spacing_val = tick_dist[2];
  y_axis_min_val = tick_dist[0];
  dm_ymin_tick = y_axis_min_val;
  dm_ymax_tick = dm_ymin_tick + y_tick_spacing_val * ( y_num_ticks - 1 );

  // skip label of each second thick mark
  count=2;
  y_num_ticks = (y_num_ticks * 2)-1;
  y_axis_ticks.tick_per_pix = dm_back.height() / (y_num_ticks - 1.00);

  // if not enought tick marks per display, make spacing smaller
  if (y_axis_ticks.tick_per_pix > (dm_size.y/8)) {
    y_axis_ticks.tick_per_pix /= 2;
    y_tick_spacing_val /= 2;
    y_num_ticks = (y_num_ticks * 2)-1;
  } else {
    if (y_axis_ticks.tick_per_pix < 15)
      count*=2;
    if (y_axis_ticks.tick_per_pix < 8)
      count*=2;
  }

  //calculate y ticks position
  y_axis_ticks.axis_ticks.resize(y_num_ticks);
  new_tick_y = dm_back.height();
  for (i=0; i<y_num_ticks; i++) {
    y_axis_ticks.axis_ticks[i].x_min_pos = dm_base.x-3;
    y_axis_ticks.axis_ticks[i].x_max_pos = dm_base.x+3;
    y_axis_ticks.axis_ticks[i].y_min_pos = static_cast<int>(::ceil(new_tick_y));
    y_axis_ticks.axis_ticks[i].y_max_pos = static_cast<int>(::ceil(new_tick_y));
    y_axis_ticks.axis_ticks[i].number_label.x_pos = -1;
    new_tick_y -= y_axis_ticks.tick_per_pix;
  }

  //calculate y labels position
  val = y_axis_min_val;
  for (i=0; i<y_num_ticks; i=i+count) {
    new_num_x = static_cast<double>(dm_base.x)-26;
    for (j=10; j<=10000000 && (abs(static_cast<int>(val)) >= j) ; j=j*10)
      new_num_x -= 6;
    if (val < 0)
      new_num_x -= 3;

    y_axis_ticks.axis_ticks[i].number_label.x_pos = static_cast<int>(new_num_x);
    y_axis_ticks.axis_ticks[i].number_label.y_pos = y_axis_ticks.axis_ticks[i].y_min_pos - 6;
    y_axis_ticks.axis_ticks[i].number_label.num = scopira::tool::double_to_string(val, get_ydigits());
    val += y_tick_spacing_val*(count/2);
  }

  //calculate x tick marks
  tick_dist.clear();
  x_num_ticks = calc_tick_marks(dm_xmin, dm_xmax, tick_dist);
  x_tick_spacing_val = tick_dist[2];
  x_axis_min_val = tick_dist[0];
  x_axis_max_val = tick_dist[1];

  count=2;
  x_num_ticks = (x_num_ticks * 2)-1;
  x_axis_ticks.tick_per_pix = dm_back.width() / (x_num_ticks - 1.00);

  if (x_axis_ticks.tick_per_pix > (dm_size.x/8)) {
    x_axis_ticks.tick_per_pix /= 2;
    x_tick_spacing_val /= 2;
    x_num_ticks = (x_num_ticks * 2)-1;
  } else {
    if (x_axis_ticks.tick_per_pix < 15)
      count*=2;
    if (x_axis_ticks.tick_per_pix < 8)
      count*=2;
  }

  //calculate x ticks position
  x_axis_ticks.axis_ticks.resize(x_num_ticks);
  new_tick_x = 0;
  for (i=0; i<x_num_ticks; i++) {
    x_axis_ticks.axis_ticks[i].y_min_pos = dm_base.y+dm_size.y-3;
    x_axis_ticks.axis_ticks[i].y_max_pos = dm_base.y+dm_size.y+3;
    x_axis_ticks.axis_ticks[i].x_min_pos = static_cast<int>(ceil(new_tick_x));
    x_axis_ticks.axis_ticks[i].x_max_pos = static_cast<int>(ceil(new_tick_x));
    x_axis_ticks.axis_ticks[i].number_label.x_pos = -1;
    new_tick_x += x_axis_ticks.tick_per_pix;
  }

  //calculate x labels position
  if (dm_ascending) {
    dm_xmin_tick = x_axis_min_val;
    val = x_axis_min_val;
    for (i=0; i<x_num_ticks; i=i+count) {
      new_num_x = x_axis_ticks.axis_ticks[i].x_min_pos - 8;
      for (j=10; j<=100000 && (abs(static_cast<int>(val)) >= j); j=j*10)
        new_num_x-=2;
      if (val < 0)
        new_num_x -= 2;
      x_axis_ticks.axis_ticks[i].number_label.x_pos = static_cast<int>(new_num_x);
      x_axis_ticks.axis_ticks[i].number_label.y_pos = x_axis_ticks.axis_ticks[i].y_max_pos+2;
      x_axis_ticks.axis_ticks[i].number_label.num = scopira::tool::double_to_string(val, get_xdigits());
      dm_xmax_tick = val;
      val += x_tick_spacing_val*(count/2);
    }
  } else {
    dm_xmin_tick = x_axis_max_val;
    val = x_axis_max_val;
    for (i=0; i<x_num_ticks; i=i+count) {
      new_num_x = x_axis_ticks.axis_ticks[i].x_min_pos - 8;
      for (j=10; j<=100000 && (abs(static_cast<int>(val)) >= j); j=j*10)
        new_num_x-=2;
      if (val < 0)
        new_num_x -= 2;
      x_axis_ticks.axis_ticks[i].number_label.x_pos = static_cast<int>(new_num_x);
      x_axis_ticks.axis_ticks[i].number_label.y_pos = x_axis_ticks.axis_ticks[i].y_max_pos+2;
      x_axis_ticks.axis_ticks[i].number_label.num = scopira::tool::double_to_string(val, get_xdigits());
      dm_xmax_tick = val;
        val -= x_tick_spacing_val*(count/2);
    }
  }
}

void plotter::draw_axis(widget_canvas &v)
{
  double dist, new_tick_pos, new_num_pos;
  int start, end, i;

  //draw y ticks and number labels
  dist = (dm_back.height() - dm_crd_vval - dm_size.y)/y_axis_ticks.tick_per_pix;
  start = static_cast<int>(ceil(dist));
  dist = dm_crd_vval/y_axis_ticks.tick_per_pix;
  end = y_axis_ticks.axis_ticks.size() - static_cast<int>(ceil(dist)) -1;

  for (i=start; i<=end; i++) {
    //draw number label
    if (y_axis_ticks.axis_ticks[i].number_label.x_pos != -1) {
      new_num_pos = y_axis_ticks.axis_ticks[i].number_label.y_pos+ dm_base.y- dm_crd_vval;
      v.draw_text(dm_frontpen,dm_font,
          y_axis_ticks.axis_ticks[i].number_label.x_pos,
          static_cast<int>(ceil(new_num_pos)),
          y_axis_ticks.axis_ticks[i].number_label.num);
    }

    new_tick_pos = y_axis_ticks.axis_ticks[i].y_min_pos + dm_base.y - dm_crd_vval;

    //draw tick
    v.draw_line(dm_frontpen,y_axis_ticks.axis_ticks[i].x_min_pos,
        static_cast<int>(ceil(new_tick_pos)),
        y_axis_ticks.axis_ticks[i].x_max_pos,
        static_cast<int>(ceil(new_tick_pos)));

    //draw grid
    if ((static_cast<int>(ceil(new_tick_pos)) > dm_base.y+1) &&
        (static_cast<int>(ceil(new_tick_pos)) < dm_base.y+dm_size.y-1)) {
      dm_frontpen.set_foreground(dm_gridcol);
      dm_frontpen.set_line_attributes(1, GDK_LINE_ON_OFF_DASH, GDK_CAP_ROUND, GDK_JOIN_MITER);

      v.draw_line(dm_frontpen,y_axis_ticks.axis_ticks[i].x_max_pos, static_cast<int>(ceil(new_tick_pos)),
          dm_base.x+dm_size.x,static_cast<int>(ceil(new_tick_pos)));

      dm_frontpen.set_foreground(dm_linecol);
      dm_frontpen.set_line_attributes(1, GDK_LINE_SOLID, GDK_CAP_ROUND, GDK_JOIN_MITER);
    }
  }

  //draw x ticks and number labels
  dist = dm_crd_hval/x_axis_ticks.tick_per_pix;
  start =   static_cast<int>(ceil(dist));
  dist = (dm_back.width() - dm_crd_hval - dm_size.x)/x_axis_ticks.tick_per_pix;
  end = x_axis_ticks.axis_ticks.size() - static_cast<int>(ceil(dist)) -1;

  for (i=start; i<=end; i++) {
    if (x_axis_ticks.axis_ticks[i].number_label.x_pos != -1) {
      new_num_pos = x_axis_ticks.axis_ticks[i].number_label.x_pos + dm_base.x - dm_crd_hval;
      v.draw_text(dm_frontpen,dm_font,
        static_cast<int>(ceil(new_num_pos)),
        x_axis_ticks.axis_ticks[i].number_label.y_pos,
        x_axis_ticks.axis_ticks[i].number_label.num);
   }

   new_tick_pos = x_axis_ticks.axis_ticks[i].x_min_pos + dm_base.x - dm_crd_hval;

   //draw tick
   v.draw_line(dm_frontpen,static_cast<int>(ceil(new_tick_pos)),
    x_axis_ticks.axis_ticks[i].y_min_pos,
    static_cast<int>(ceil(new_tick_pos)),
    x_axis_ticks.axis_ticks[i].y_max_pos);

    //draw grid
    if ((static_cast<int>(ceil(new_tick_pos)) > dm_base.x+1) &&
       (static_cast<int>(ceil(new_tick_pos)) < dm_base.x+dm_size.x-1)) {
         dm_frontpen.set_foreground(dm_gridcol);
         dm_frontpen.set_line_attributes(1, GDK_LINE_ON_OFF_DASH, GDK_CAP_ROUND, GDK_JOIN_MITER);
         v.draw_line(dm_frontpen,static_cast<int>(ceil(new_tick_pos)),
          dm_base.y,static_cast<int>(ceil(new_tick_pos)),
          x_axis_ticks.axis_ticks[i].y_min_pos);

         dm_frontpen.set_line_attributes(1, GDK_LINE_SOLID, GDK_CAP_ROUND, GDK_JOIN_MITER);
         dm_frontpen.set_foreground(dm_linecol);
    }
  }

  dm_frontpen.set_line_attributes(1, GDK_LINE_SOLID, GDK_CAP_ROUND, GDK_JOIN_MITER);
}

const char * plotter::prepare_lines(void)
{
  double tol = 0.000000001;

  // make sure data is valid
  if (dm_data.is_null() || dm_data->empty())
    return "No data to plot";
  if (dm_data->width() < 2 || dm_data->height() < 1)
    return "Data matrix must be atleast 2 by 1";
  if (!dm_labels.is_null() && !dm_labels->empty() && dm_labels->size() != dm_data->width())
    return "Labels does not match data matrix width";
  if (dm_ymin >= dm_ymax)
    return "The Y range of the data is too small or zero";

  // iterate over all the data vectors
  dm_linear = dm_labels.is_null() || dm_labels->empty();
  if (dm_linear) {
    if (dm_data->width() <= 1)
      return "Data matrix is only one-wide";
    dm_xmin = 0;
    dm_xmax = dm_data->width()-1;

    dm_xmin_orig = dm_xmin;
    dm_xmax_orig = dm_xmax;
  }
  else {
    // descending order
    if (dm_labels.ref()[0] > dm_labels.ref()[dm_labels->size()-1]) {
      dm_ascending = false;
      dm_xmin = dm_labels.ref()[dm_labels->size()-1]  ;
      dm_xmax = dm_labels.ref()[0];
    }
    else if (dm_labels.ref()[dm_labels->size()-1] > dm_labels.ref()[0]) {
      //ascending order
      dm_xmin = dm_labels.ref()[0];
      dm_xmax = dm_labels.ref()[dm_labels->size()-1];
    }
    else
      return "Labels is flat";

    dm_xmin_orig = dm_xmin;
    dm_xmax_orig = dm_xmax;
  }

  // check and see if we've clipped the data
  if (dm_clip) {
    // do want to display outside the data range, if user sets viewing area outside original data range use it
    dm_xmin = dm_xmin_clip;
    dm_xmax = dm_xmax_clip;
    dm_ymin = dm_ymin_clip;
    dm_ymax = dm_ymax_clip;
  }

  // make sure ymin != ymax
  if (dm_ymax - dm_ymin < tol) {
    dm_ymin -= 1;
    dm_ymax += 1;
  }

  return 0;
}

void plotter::draw_lines(widget_canvas &v)
{
  size_t yrow, i, j;
  point cur, prev;
  nslice<double> row;
  int num_clrs = dm_plotcols.size();
//OUTPUT<<"plotter::draw_lines ymin: "<<dm_ymin<<" ymax: "<<dm_ymax<<"\n";
  assert(dm_ymin < dm_ymax);

  // clear
  dm_backpen.set_foreground(dm_backcol);
  dm_back.clear(dm_backpen);

  if (dm_ascending) {
    for (yrow=0; yrow<dm_data->height(); ++yrow) {
      row = dm_data->xslice(0, yrow, dm_data->width());
      dm_backpen.set_foreground(dm_plotcols[yrow % num_clrs]);

      j=find_label_index(dm_xmin);

      prev.x = static_cast<int>(((dm_linear?j:dm_labels.ref()[j]) - dm_xmin_tick) / (dm_xmax_tick - dm_xmin_tick) * dm_back.width());
      prev.y = dm_back.height() - static_cast<int>((row[j]-dm_ymin_tick) / (dm_ymax_tick-dm_ymin_tick) * dm_back.height());

      for (i=j+1; i<row.size(); ++i) {
        cur.x = static_cast<int>(((dm_linear?i:dm_labels.ref()[i]) - dm_xmin_tick) / (dm_xmax_tick - dm_xmin_tick) * dm_back.width());
        cur.y = dm_back.height() - static_cast<int>((row[i]-dm_ymin_tick) / (dm_ymax_tick-dm_ymin_tick) * dm_back.height());

        dm_back.draw_line(dm_backpen, prev, cur);
        prev = cur;
      }
    }
  } else {
    for (yrow=0; yrow<dm_data->height(); ++yrow) {
      row = dm_data->xslice(0, yrow, dm_data->width());
      dm_backpen.set_foreground(dm_plotcols[yrow % num_clrs]);

      j=find_label_index(dm_xmax);

      prev.x = static_cast<int>((dm_linear?0:dm_xmin_tick - dm_labels.ref()[j]) / (dm_xmin_tick - dm_xmax_tick) * dm_back.width());
      prev.y = dm_back.height() - static_cast<int>((row[j]-dm_ymin_tick) / (dm_ymax_tick-dm_ymin_tick) * dm_back.height());

      for (i=j+1; i<row.size(); ++i) {
        cur.x = static_cast<int>((dm_linear?i:dm_xmin_tick - dm_labels.ref()[i]) / (dm_xmin_tick - dm_xmax_tick) * dm_back.width());
        cur.y = dm_back.height() - static_cast<int>((row[i]-dm_ymin_tick) / (dm_ymax_tick-dm_ymin_tick) * dm_back.height());
        dm_back.draw_line(dm_backpen, prev, cur);
        prev = cur;
      }
    }
  }
}

void plotter::draw_bars(widget_canvas &v)
{
  size_t yrow, i;
  point cur;
  nslice<double> row;
  int dec, bar_width;
  double tmp_width;
  int num_clrs = dm_plotcols.size();

  assert(dm_ymin < dm_ymax);

  // clear
  dm_backpen.set_foreground(dm_backcol);
  dm_back.clear(dm_backpen);

  yrow = 0;  // TODO add functionality to have more than 1 row at a time (side-by-side or overlayed)

  // base bar-plot width on display range, not on data range, that way you can "zoom" in and not get a bat width of 1 on large plot-vecs
  // Rodrigo Vivanco
  double display_x_range = dm_xmax_tick - dm_xmin_tick;             // RV
  tmp_width = (dm_back.width() - display_x_range)/display_x_range;  // RV

  // the bar width must be an int of at least 1
  bar_width = std::max(static_cast<int>(tmp_width), 1);

  // center bar (horizontally) around the point
  dec = bar_width/2;

  if (dm_ascending) {
    row = dm_data->xslice(0, yrow, dm_data->width());
    dm_backpen.set_foreground(dm_plotcols[yrow % num_clrs]);

    for (i=find_label_index(dm_xmin); i<row.size(); ++i) {
      cur.x = static_cast<int>(((dm_linear?i:dm_labels.ref()[i]) - dm_xmin_tick) / (dm_xmax_tick - dm_xmin_tick) * dm_back.width());
      cur.y = dm_back.height() - static_cast<int>((row[i]-dm_ymin_tick) / (dm_ymax_tick-dm_ymin_tick) * dm_back.height());

      dm_back.draw_rectangle(dm_backpen, true, cur.x-dec, cur.y, bar_width, dm_back.height() - cur.y);
    }
  } else {
    row = dm_data->xslice(0, yrow, dm_data->width());
    dm_backpen.set_foreground(dm_plotcols[yrow % num_clrs]);

    for (i=find_label_index(dm_xmax); i<row.size(); ++i) {
      cur.x = static_cast<int>((dm_linear?i:dm_xmin_tick - dm_labels.ref()[i]) / (dm_xmin_tick - dm_xmax_tick) * dm_back.width());
      cur.y = dm_back.height() - static_cast<int>((row[i]-dm_ymin_tick) / (dm_ymax_tick-dm_ymin_tick) * dm_back.height());
      dm_back.draw_rectangle(dm_backpen, true, cur.x-dec, cur.y, bar_width, dm_back.height() - cur.y);
    }
  }
}

void plotter::prepare(widget_canvas &v, coord &crd)
{
  int i;
  int new_hval, new_vval;
  double zoom_f, center_x, center_y, new_width, new_height;

  dm_dirty = false;

  //old img center
  center_x = (dm_size.x/2) + crd.hval();
  center_y = (dm_size.y/2) + crd.vval();

  //calc zoom
  zoom_f = 4.0*dm_zoom/1000;

  //new front img size
  dm_base.x = 45;
  dm_base.y = 25;
  // account for long numbers along y axis
  for (i=10; i<=100000 && i<=(abs(static_cast<int>(dm_ymax))); i=i*10)
    dm_base.x += 5;
  // account for negative numbers - give more space for a minus sign
  if (dm_ymax < 0 || dm_ymin < 0)
    dm_base.x += 5;

  // calculate size of plot to be drawn
  if ((v.width() > dm_base.x) && (v.height() >  dm_base.y)) {
    dm_size.x = v.width() - dm_base.x - (dm_base.x/2);
    dm_size.y = v.height() - (2*dm_base.y) - (dm_base.y/2);
  } else {
    dm_size.x = v.width();
    dm_size.y = v.height();
  }

  //new width and height of back img
  new_width = dm_size.x + dm_size.x*zoom_f;
  new_height = dm_size.y + dm_size.y*zoom_f;

  //new img center
  center_x *= new_width/dm_back.width();
  center_y *= new_height/dm_back.height();

  // resize back
  dm_back.resize(this,static_cast<int>(new_width), static_cast<int>(new_height));

  // calculate values for new scroll bars
  new_hval = static_cast<int>(center_x - (dm_size.x/2));
  new_vval = static_cast<int>(center_y - (dm_size.y/2));

  // make sure scroll coordinates are valid
  if (new_hval < 0)
    new_hval = 0;
  else if (new_hval+dm_size.x > dm_back.width())
    new_hval = dm_back.width() - dm_size.x;
  if (new_vval < 0)
    new_vval = 0;
  else if (new_vval+dm_size.y > dm_back.height())
    new_vval = dm_back.height() - dm_size.y ;

  // set up scroll bars
  crd.set_hbounds(0, static_cast<float>(dm_back.width()), static_cast<float>(new_hval), static_cast<float>(dm_size.x / 4), static_cast<float>(dm_size.x / 2), static_cast<float>(dm_size.x));
  crd.set_vbounds(0, static_cast<float>(dm_back.height()), static_cast<float>(new_vval), static_cast<float>(dm_size.y / 4), static_cast<float>(dm_size.y / 2), static_cast<float>(dm_size.y));

  // prepare the data
  dm_error = prepare_lines();

  // prepare the axis
  prepare_axis();
}

void plotter::paint(widget_canvas &v, coord &crd)
{
  // if there was any changes, prepare the canvas
  if (dm_dirty)
    prepare(v, crd);

  // check for errors
  if (dm_error) {
    v.draw_text(dm_frontpen, dm_font, 5, 5, dm_error);
    return;
  }

  if (dm_plot_style == bar_plot_c)
    draw_bars(v);  // draw a bar plot
  else
    draw_lines(v); // draw a line plot

  // draw in the border (ie. the background for the entire plot)
  dm_frontpen.set_foreground(dm_bordercol);
  v.clear(dm_frontpen);
  dm_frontpen.set_foreground(dm_linecol);

  // draw plot background overtop of border background
  dm_frontpen.set_foreground(dm_backcol);
  v.draw_rectangle(dm_frontpen, true, dm_base, dm_size);
  
  // get new coordinates
  dm_crd_hval = crd.hval();
  dm_crd_vval = crd.vval();

  // draw plot
  dm_frontpen.set_foreground(dm_linecol);
  v.draw_canvas(dm_frontpen,dm_crd_hval, dm_crd_vval, dm_back, dm_base.x, dm_base.y, dm_size.x, dm_size.y);
  v.draw_rectangle(dm_frontpen, false, dm_base, dm_size);

  if (dm_show_plotname) {
    // draw title and footer, if applicable
    dm_frontpen.set_foreground(dm_titlecol);
    if (!dm_title.empty())
      v.draw_text(dm_frontpen, dm_font, 5, 5, dm_title);
    else  if (!dm_meta.is_null())
      v.draw_text(dm_frontpen, dm_font, 5, 5, dm_meta->get_name());
    if (!dm_footer.empty())
      v.draw_text(dm_frontpen, dm_font, 5, v.height() - 15, dm_footer);
  }

  // draw labels
  dm_frontpen.set_foreground(dm_labelcol);
  if (!dm_meta.is_null()) {
    v.draw_text(dm_frontpen, dm_font, 5, v.height()/2, dm_meta->get_ylabel());
    v.draw_text(dm_frontpen, dm_font, v.width()/2, v.height() - 15, dm_meta->get_xlabel());
  }
  dm_frontpen.set_foreground(dm_linecol);

  // draw axes
  draw_axis(v);
}

