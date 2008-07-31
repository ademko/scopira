
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

#ifndef __INCLUDED_SCOPIRA_UIKIT_PLOTTER_H__
#define __INCLUDED_SCOPIRA_UIKIT_PLOTTER_H__

#include <scopira/basekit/narray.h>
#include <scopira/coreui/export.h>
#include <scopira/coreui/window.h>
#include <scopira/coreui/checkbutton.h>
#include <scopira/coreui/radiobutton.h>
#include <scopira/coreui/spinbutton.h>
#include <scopira/coreui/entry.h>
#include <scopira/coreui/checkbutton.h>
#include <scopira/coreui/button.h>
#include <scopira/coreui/canvas.h>
#include <scopira/coreui/layout.h>
#include <scopira/uikit/color.h>

namespace scopira
{
  namespace uikit
  {
    class plotter_options;
    class plotter_options_bar;

    class plotter_properties_reactor_i;  // may want to know when plotter properties changed by user (RV)
    class plotter_properties_dialog;

    class plotter;
  }
}

enum scaling_mode_t { scale_auto_c, scale_to_max_c, scale_user_c };

/**
 * all the state needed for the 2d plot
 *
 * @author Aleksander Demko
 */ 
class scopira::uikit::plotter_options : public virtual scopira::tool::object
{
  public:
    static const double nan_c;

  protected:
    std::string dm_name, dm_xlabel, dm_ylabel, dm_style;
    
    int dm_backcol;       // in packed colour format (0xRRGGBB)
    int dm_scaling_mode;
    
  public:
    /// ctor
    SCOPIRAUI_EXPORT explicit plotter_options(size_t numplot = 0, size_t plotlen = 0);
    /// copy ctor with new size
    SCOPIRAUI_EXPORT plotter_options(const plotter_options &src);
    //plotter_options(const plotter_options_bar &src);

    /// serialization loader
    SCOPIRAUI_EXPORT virtual bool load(scopira::tool::iobjflow_i &in);
    /// serialization saver
    SCOPIRAUI_EXPORT virtual void save(scopira::tool::oobjflow_i &out) const;

    /// name setter
    SCOPIRAUI_EXPORT void set_name(const std::string &name);
    /// name getter
    const std::string & get_name(void) const { return dm_name; }
    /// xlabel setter
    SCOPIRAUI_EXPORT void set_xlabel(const std::string &lab);
    /// xlabel get_name
    const std::string & get_xlabel(void) const { return dm_xlabel; }
    /// ylabel setter
    SCOPIRAUI_EXPORT void set_ylabel(const std::string &lab);
    /// ylabel getter
    const std::string & get_ylabel(void) const { return dm_ylabel; }
    /// scaling mode setter
    SCOPIRAUI_EXPORT void set_scaling_mode(int val);
    /// get scaling mode
    int get_scaling_mode(void) const { return dm_scaling_mode; }
    /// get the style
    const std::string & get_style(void) const { return dm_style; }
};

/**
  * for displaying 2d bar plots
  * @author Joel Dyck
  */
class scopira::uikit::plotter_options_bar : public scopira::uikit::plotter_options
{
  public:
    SCOPIRAUI_EXPORT explicit plotter_options_bar(size_t numplot = 0, size_t plotlen = 0);
    SCOPIRAUI_EXPORT plotter_options_bar(const plotter_options_bar &src);
};


/**
 * To inform clients when plotter properties changed by user.
 *
 * @author Rodrigo Vivanco
 */
class scopira::uikit::plotter_properties_reactor_i : public virtual scopira::tool::object
{
  public:
    virtual void border_colour_changed( scopira::uikit::plotter *plotter_obj, int colour ) {};
};

/**
 * plotter_properties_dialog : a properties dialog class
 *
 * Opened from within a plot, allows users to set plot properties
 *
 * @author Shantha Ramachadran
 * @author Marina Mandelzweig
 */
class scopira::uikit::plotter_properties_dialog : public scopira::coreui::dialog,
  public virtual scopira::coreui::checkbutton_reactor_i,
  public virtual scopira::uikit::colorbutton_reactor_i,
  public virtual scopira::coreui::radiobutton_reactor_i,
  public virtual scopira::tool::destroy_reactor_i
{
  protected:
    enum colors_info {
       back_col,
       border_col,
       title_col,
       label_col,
       plot_col1,
       plot_col2,
       plot_col3,
       plot_col4,
       plot_col5,
       plot_col6,
       total_colors
    };
    /// entry boxes for plot labels
    scopira::tool::count_ptr<scopira::coreui::entry> dm_title, dm_footer, dm_xlabel, dm_ylabel;
    
    /// color widgets for plot colours and lines
    std::vector< scopira::tool::count_ptr<scopira::uikit::colorbutton> > dm_colors;
    
    /// entry boxes for plot ranges
    scopira::tool::count_ptr<scopira::coreui::entry> dm_xmin, dm_xmax, dm_ymin, dm_ymax;
    /// spinbutton for font size
    scopira::tool::count_ptr<scopira::coreui::spinbutton> dm_font_size;
    /// checkboxes for turning properties on and off
    scopira::tool::count_ptr<scopira::coreui::checkbutton> dm_auto_col, /* dm_auto_size, */ dm_auto_line;
    /// radio buttons for scaling options
    scopira::tool::count_ptr<scopira::coreui::radiobutton> dm_scaling_buttons;
    //int dm_scale_how;
  
    /// tab layout for all widgets
    scopira::tool::count_ptr<scopira::coreui::tab_layout> dm_tab;
    /// button to apply changes
    scopira::tool::count_ptr<scopira::coreui::button> dm_applybutton;
    /// ok button
    GtkWidget * dm_okbutton;
  
    /// true range values
    double dm_xmin_real, dm_xmax_real, dm_ymin_real, dm_ymax_real;
    
    /// current color selection win, if any
    scopira::uikit::colorwindow *dm_colorwin;
    int dm_selected_color;

  public:
    /// default constructor
    SCOPIRAUI_EXPORT plotter_properties_dialog(const std::string &title);
    SCOPIRAUI_EXPORT virtual ~plotter_properties_dialog(void);

    /// set up event handlers for ok and apply buttons
    SCOPIRAUI_EXPORT void ok_connect(GtkSignalFunc func, gpointer func_data);
    SCOPIRAUI_EXPORT void apply_connect(GtkSignalFunc func, gpointer func_data);

    /// called when the given object is in its destruction phase
    SCOPIRAUI_EXPORT virtual void react_destroy(scopira::tool::object *source);
    /// called when checkbutton is clicked
    SCOPIRAUI_EXPORT virtual void react_checkbutton(scopira::coreui::checkbutton *source, bool val);
    SCOPIRAUI_EXPORT virtual void react_colorbutton(scopira::uikit::colorbutton *source, int newcol);
    SCOPIRAUI_EXPORT virtual void react_radiobutton(scopira::coreui::radiobutton *source, int actionid);

    /// functions to get the property values from the dialog
    SCOPIRAUI_EXPORT int get_color(int c) const;
    SCOPIRAUI_EXPORT const std::string get_title(void) const;
    SCOPIRAUI_EXPORT const std::string get_footer(void) const;
    SCOPIRAUI_EXPORT const std::string get_xlabel(void) const;
    SCOPIRAUI_EXPORT const std::string get_ylabel(void) const;
    SCOPIRAUI_EXPORT int get_font_size(void) const;
    SCOPIRAUI_EXPORT int get_back_color(void) const;
    SCOPIRAUI_EXPORT int get_border_color(void) const;
    SCOPIRAUI_EXPORT int get_title_color(void) const;
    SCOPIRAUI_EXPORT int get_label_color(void) const;
    SCOPIRAUI_EXPORT double get_xmin(void) const;
    SCOPIRAUI_EXPORT double get_xmax(void) const;
    SCOPIRAUI_EXPORT double get_ymin(void) const;
    SCOPIRAUI_EXPORT double get_ymax(void) const;
    SCOPIRAUI_EXPORT scopira::basekit::narray<int> get_line_colors(void) const;
    SCOPIRAUI_EXPORT int get_scaling_method(void) const;

    /// functions to set the property values within the dialog
    SCOPIRAUI_EXPORT void set_color(int c,int col);
    SCOPIRAUI_EXPORT void set_title(const std::string &title);
    SCOPIRAUI_EXPORT void set_footer(const std::string &footer);
    SCOPIRAUI_EXPORT void set_xlabel(const std::string &label);
    SCOPIRAUI_EXPORT void set_ylabel(const std::string &label);
    SCOPIRAUI_EXPORT void set_font_size(int size);
    SCOPIRAUI_EXPORT void set_back_color(int col);
    SCOPIRAUI_EXPORT void set_border_color(int col);
    SCOPIRAUI_EXPORT void set_title_color(int col);
    SCOPIRAUI_EXPORT void set_label_color(int col);
    SCOPIRAUI_EXPORT void set_xmin(double x);
    SCOPIRAUI_EXPORT void set_xmax(double x);
    SCOPIRAUI_EXPORT void set_ymin(double y);
    SCOPIRAUI_EXPORT void set_ymax(double y);
    SCOPIRAUI_EXPORT void set_line_colors(const scopira::basekit::narray<int> &colors);
    SCOPIRAUI_EXPORT void set_scaling_method(int method);

    /// set the real range values of the data (ie values before being clipped)
    SCOPIRAUI_EXPORT void set_default_range(double xmin, double xmax, double ymin, double ymax);

  protected:
    SCOPIRAUI_EXPORT void init_gui(void);
  
  private:
    /// called when the user has chosen a new shape in the shape window
    static void h_colorwindow_ok(GtkWidget *widget, scopira::uikit::plotter_properties_dialog *data);
};


/**
 * plotter : an all purpose plotter
 *
 * Plots data along an x and y axis, allowing for zooming and scrolling.
 * Specify using set_show_scroll(bool) and set_show_zoom(bool) whether or not to display
 * scrollbars and zoomer - default is true, true.
 * Scrolling and zooming is still possible when scrollbars and/or zoomer is not displayed
 * by using mouse box selection (zoom) and mouse panning (scroll).
 *
 * See notes for handle_press and handle_motion when overriding this class.
 *
 * @author Shantha Ramachandran
 * @author Marina Mandelzweig
 */
class scopira::uikit::plotter : public scopira::coreui::zoomed_scrolled_canvas_base,
  public virtual scopira::tool::destroy_reactor_i
{
  public:
        enum plot_style { line_plot_c, bar_plot_c };
        
  protected:
    /// data
    scopira::tool::count_ptr<scopira::uikit::plotter_options> dm_meta; /// meta data
    scopira::tool::count_ptr<scopira::basekit::narray_o<double,1> > dm_labels; /// labels
    scopira::tool::count_ptr<scopira::basekit::narray_o<double,2> > dm_data; /// plotlen X plotvec
    std::string dm_title; /// title of plot
    std::string dm_footer;  /// footer on plot

    /// axes / tick marks
    struct tick_label {  /// number label for a tick
      //pix positions for a number
      int x_pos;      // if this is -1, then that means "no label here"
      int y_pos;
      //number label
      std::string num;
    };

    struct one_tick {  /// a tick mark
      //pix positions for a tick
      int x_min_pos;
      int x_max_pos;
      int y_min_pos;
      int y_max_pos;
      //number label
      tick_label number_label;
    };

    struct ticks {  // /all the ticks for one axis
      //step pix between ticks
      double tick_per_pix;
      //axis ticks
      scopira::tool::basic_array<one_tick> axis_ticks;
    };

    ticks x_axis_ticks, y_axis_ticks; /// x axis ticks, y axis ticks
    double dm_xmin_tick, dm_xmax_tick; /// min and max tick marks for x-axis
    double dm_ymin_tick, dm_ymax_tick; /// min and max tick marks for y-axis

    /// data ranges
    double dm_ymin, dm_ymax; /// data y min and max
    double dm_xmin, dm_xmax;  /// data x min and max
    double dm_ymin_orig, dm_ymax_orig; /// original y min / max - keep track when clipping
    double dm_xmin_orig, dm_xmax_orig;  /// original x  min / max - keep trakc when clipping
    double dm_xmin_clip, dm_xmax_clip; /// clipped x min / max
    double dm_ymin_clip, dm_ymax_clip;  /// clipped y min / max
    double dm_pad_percent; ///percentage of the y-range to pad the top and bottom of the plot

    /// drawing tools
    scopira::coreui::widget_context dm_backpen;  /// for drawing plot lines
    scopira::coreui::widget_context dm_frontpen;  /// for drawing plot
    scopira::coreui::font dm_font;  /// default font
    scopira::coreui::pixmap_canvas dm_back;  /// canvas where plot is drawn
  
    /// colours
    int dm_titlecol, dm_labelcol, dm_backcol, dm_bordercol, dm_gridcol, dm_linecol; /// plot colours
    scopira::basekit::narray<int> dm_plotcols; /// colours of the plotted lines
  
    /// sizing and position
    scopira::coreui::point dm_base, dm_size;  /// base and size of the inner plot
    int dm_crd_hval, dm_crd_vval; /// current horizontal / vertical scroll cord position
    int dm_zoom;  /// zoom factor
    bool dm_ascending;  /// is the x-scale ascending or descending?
    bool dm_linear;  /// if there are no labels, the plot is linear
    short dm_ydigits;   // number of digits to round too
    short dm_xdigits;   // number of digits to round too

    /// mouse events
    bool dm_mouse_down, dm_mouse_pan;  /// is mouse pressed / panning
    bool dm_old_rect; /// keep track of mouse selection box
    scopira::coreui::point dm_down;  /// where mouse was pressed
    scopira::coreui::point dm_old_down, dm_old_up;  /// where mouse was pressed / released
    
    /// status
    bool dm_dirty;  /// was there a change that requires an update
    const char *dm_error;  /// drawing error
    bool dm_show_plotname;  /// should we show the plot name
    bool dm_first; /// first time plot is drawn
    bool dm_clip;  /// can we clip the ranges

    /// style
    short dm_plot_style;  /// the style of plot

    /// properties window
    enum menu_list {  /// menu items
      cmd_show_properties_c = 1000,
    };

    /// scaling method
    int dm_scaling_method;
    
    scopira::uikit::plotter_properties_dialog *dm_prop_window;  /// properties window
  
    scopira::uikit::plotter_properties_reactor_i *dm_reactor;
  public:
    /// ctor
    SCOPIRAUI_EXPORT plotter(bool scroll=true, bool zoom=true);
    
    SCOPIRAUI_EXPORT virtual void react_destroy(scopira::tool::object *source);
    
    /// set meta info
    SCOPIRAUI_EXPORT void set_options(scopira::uikit::plotter_options *plot);
    /// set input data
    SCOPIRAUI_EXPORT void set_plot_data(scopira::basekit::narray_o<double,2> *matrix);
    /// set labels for x axis
    SCOPIRAUI_EXPORT void set_labels(scopira::basekit::narray_o<double,1> *labels);

    /// set plot title
    SCOPIRAUI_EXPORT void set_title(const std::string &title);
    /// set plot footer
    SCOPIRAUI_EXPORT void set_footer(const std::string &footer);
    /// set x label
    SCOPIRAUI_EXPORT void set_xlabel(const std::string &lab);
    /// set y label
    SCOPIRAUI_EXPORT void set_ylabel(const std::string &lab);
    /// set font size
    SCOPIRAUI_EXPORT void set_font_size(int size);
    /// set border colour
    SCOPIRAUI_EXPORT void set_border_color(int col);
    /// set plot background colour
    SCOPIRAUI_EXPORT void set_back_color(int col);
    /// set title colour
    SCOPIRAUI_EXPORT void set_title_color(int col);
    /// set labels colour
    SCOPIRAUI_EXPORT void set_label_color(int col);
    // sets plotting style (line plot, bar plot)
    SCOPIRAUI_EXPORT void set_plot_style( int style );

    // when true, will use the min/max values for axis
    void set_clip_range( bool flg ) {
      dm_clip = flg;
    }
    
    /// set xmin - clips the range viewed
    SCOPIRAUI_EXPORT void set_xmin(double x);
    /// set xmax - clips the range viewed
    SCOPIRAUI_EXPORT void set_xmax(double x);

    SCOPIRAUI_EXPORT void reset_x_range( void );
    
    /// set ymin - clips the y range viewed
    SCOPIRAUI_EXPORT void set_ymin(double y);
    /// set ymax - clips the y range viewed
    SCOPIRAUI_EXPORT void set_ymax(double y);
    
    SCOPIRAUI_EXPORT void reset_y_range( void );
    
    /// sets the percentage of the y-range to be used to pad the top and bottom
    SCOPIRAUI_EXPORT void set_pad_percent(double p);
    
    /// set the line colours
    SCOPIRAUI_EXPORT void set_line_colors(scopira::basekit::narray<int> colors);

    /// set the scaling method
    SCOPIRAUI_EXPORT void set_scaling_method(int method);

    /// returns the number digits (after the decimal) for the y axis
    short get_ydigits(void) const { return dm_ydigits; }
    /// returns the number digits (after the decimal) for the x axis
    short get_xdigits(void) const { return dm_xdigits; }
    /// sets the number digits (after the decimal) for the y axis
    void set_ydigits(short numdig) { dm_ydigits = numdig; }
    /// sets the number digits (after the decimal) for the x axis
    void set_xdigits(short numdig) { dm_xdigits = numdig; }
    
    /// set the fields of the properties window
    SCOPIRAUI_EXPORT void set_dialog_properties(void);
    /// get the data from the fields of the properties window
    SCOPIRAUI_EXPORT void get_dialog_properties(void);

    /// subclasses call this to add "Show Properties" to your menu
    SCOPIRAUI_EXPORT void add_stock_menu_items(scopira::coreui::menu_handler_base::popup_menu &menu);
    /// subclasses call this to pop up the properties window
    SCOPIRAUI_EXPORT void popup_properties(void);

     /// converts the given screen x y to real x y
     /// @return true if both points where within the plot.
     /// @author Marina Mandelzweig
    SCOPIRAUI_EXPORT void convert_screento_xy(int screenx, int screeny, double &x, double &y,
      bool *xvalid = 0, bool *yvalid = 0);
     /// converts real xy to a on screen x, y
     /// @param xvalid (optional output).. will be true if the x conversion was valid
     /// @param yvalid (optional output).. will be true if the y conversion was valid
     /// @author Marina Mandelzweig
    SCOPIRAUI_EXPORT void convert_xy_to_screen(double x, double y, int &screenx, int &screeny,
      bool *xvalid = 0, bool *yvalid = 0);
    /// finds the label for a given index
    SCOPIRAUI_EXPORT int find_label_index(double val) const;

    /// called before any repaints and such - should do colour and context alocing here
    SCOPIRAUI_EXPORT virtual void handle_init(scopira::coreui::widget_canvas &v, coord &crd);
    /// handle resize
    SCOPIRAUI_EXPORT virtual void handle_resize(scopira::coreui::widget_canvas &v, coord &crd);
    /// handle repaint
    SCOPIRAUI_EXPORT virtual void handle_repaint(scopira::coreui::widget_canvas &v, coord &crd);
    /// handle scroll
    SCOPIRAUI_EXPORT virtual void handle_scroll(scopira::coreui::widget_canvas &v, coord &crd);
    /// called when zoom changes, factor is [0..1000], initally, it's 500
    SCOPIRAUI_EXPORT virtual void handle_zoom(int newfactor);
    /// handle mouse press, 0 is selection, 1 is panning
    /// if overridden, this version must be called to get buttons 0,1 functionality
    SCOPIRAUI_EXPORT virtual void handle_press(scopira::coreui::widget_canvas &v, coord &crd, const mouse_event &mevt);
    /// handle mouse motion
    /// if overridden, this version must be called to get button 1 functionality (panning)
    SCOPIRAUI_EXPORT virtual void handle_motion(scopira::coreui::widget_canvas &v, coord &crd, const scopira::coreui::mouse_event &mevt);
    /// handle popup menu
    SCOPIRAUI_EXPORT virtual void handle_menu(scopira::coreui::widget_canvas &v, coord &crd, intptr_t menuid);

    /// Update view. you must call this after set_meta/label/plot_data
    void update_gui(void) { request_redraw(); }

    void set_reactor( scopira::uikit::plotter_properties_reactor_i *reactor ){
      dm_reactor = reactor;
    }

  protected:
    /**
     * Protected ctor. Descendants should this one, as this ones doesn't do init_gui
     * (and leaves that choice up to the descendant).
     *
     * The extra variable is just a dummy variable to differenciate it from the public
     * ctor - you should pass 0 to it.
     *
     * The same rules apply as for all canvas widgets... You either call
     * xzoomed_scrolled_canvas_base::init_gui, or you call make_drawing_area+widget::init_gui
     *
     * @author Aleksander Demko
     */ 
    SCOPIRAUI_EXPORT plotter(int);

    /// calculate min and max ticks, and spacing between them
    SCOPIRAUI_EXPORT int calc_tick_marks(double min, double max, scopira::tool::basic_array<double> &tick_dist);
    /// calculate ymin and ymax
    SCOPIRAUI_EXPORT void calc_y_range(void);

    /// prepare and draw the axes
    SCOPIRAUI_EXPORT void prepare_axis(void);
    SCOPIRAUI_EXPORT void draw_axis(scopira::coreui::widget_canvas &v);

    /// prepare and draw the plot lines or bars
    SCOPIRAUI_EXPORT const char * prepare_lines(void);
    SCOPIRAUI_EXPORT void draw_lines(scopira::coreui::widget_canvas &v);
    SCOPIRAUI_EXPORT void draw_bars(scopira::coreui::widget_canvas &v);

    // prepare and draw the plot onto the canvas
    SCOPIRAUI_EXPORT void prepare(scopira::coreui::widget_canvas &v, coord &crd);
    SCOPIRAUI_EXPORT void paint(scopira::coreui::widget_canvas &v, coord &crd);

    /// called when OK button on properties window is pressed
    SCOPIRAUI_EXPORT static void h_propwindow_ok(GtkWidget *widget, scopira::uikit::plotter *data);
    /// called when Apply button on properties window is pressed
    SCOPIRAUI_EXPORT static void h_propwindow_apply(GtkWidget *widget, scopira::uikit::plotter *data);
};

#endif

