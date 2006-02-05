

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

#ifndef __INCLUDED_SCOPIRA_UIKIT_COLOR_H__
#define __INCLUDED_SCOPIRA_UIKIT_COLOR_H__

#include <scopira/tool/reactor.h>
#include <scopira/coreui/viewwidget.h>
#include <scopira/coreui/window.h>
#include <scopira/coreui/viewmenu.h>
#include <scopira/coreui/canvas.h>
#include <scopira/uikit/tiledlayout.h>
#include <scopira/uikit/models.h>

namespace scopira
{
  namespace uikit
  {
    class make_palette_base;
    
    class make_gray_palette_v;
    class make_red_palette_v;
    class make_green_palette_v;
    class make_blue_palette_v;
    class make_pick_palette_v;
    class make_heat_palette_v;
    class make_rainbow_palette_v;
    class make_bluered_palette_v;
    class make_bone_palette_v;
    class make_copper_palette_v;
    class make_spectra_palette_v;
    
    class colorwindow;
    
    class colorbutton_reactor_i;
    class colorbutton;

    class colorpalette;
    class colorpalette_v;
  }
}


/**
 * Helper base class for views that make a palette vector.
 * Descedants need only override make_palette and register their view.
 *
 * This is a gui-less view base class.
 *
 * @author Marina Mandelzweig
 */
class scopira::uikit::make_palette_base : public scopira::core::view_i
{
  public:
    virtual void bind_model(scopira::core::model_i *sus);

    // returns the name
    virtual std::string make_palette(const scopira::basekit::nslice<int> &palette) = 0;
};

/**
 * Make a gray palette
 * @author Marina Mandelzweig
 */ 
class scopira::uikit::make_gray_palette_v : public scopira::uikit::make_palette_base
{
  public:
    virtual std::string make_palette(const scopira::basekit::nslice<int> &palette);
};

/**
 * Make a red palette
 * @author Marina Mandelzweig
 */ 
class scopira::uikit::make_red_palette_v : public scopira::uikit::make_palette_base
{
  public:
    virtual std::string make_palette(const scopira::basekit::nslice<int> &palette);
};

/**
 * Make a green palette
 * @author Marina Mandelzweig
 */ 
class scopira::uikit::make_green_palette_v : public scopira::uikit::make_palette_base
{
  public:
    virtual std::string make_palette(const scopira::basekit::nslice<int> &palette);
};

/**
 * Make a blue palette
 * @author Marina Mandelzweig
 */ 
class scopira::uikit::make_blue_palette_v : public scopira::uikit::make_palette_base
{
  public:
    virtual std::string make_palette(const scopira::basekit::nslice<int> &palette);
};

/**
 * Make a pick palette
 * @author Marina Mandelzweig
 */ 
class scopira::uikit::make_pick_palette_v : public scopira::uikit::make_palette_base
{
  public:
    virtual std::string make_palette(const scopira::basekit::nslice<int> &palette);
};

/**
 * Make a heat palette
 * @author Marina Mandelzweig
 */ 
class scopira::uikit::make_heat_palette_v : public scopira::uikit::make_palette_base
{
  public:
    virtual std::string make_palette(const scopira::basekit::nslice<int> &palette);
};

/**
 * Make a rainbow palette
 * @author Marina Mandelzweig
 */ 
class scopira::uikit::make_rainbow_palette_v : public scopira::uikit::make_palette_base
{
  public:
    virtual std::string make_palette(const scopira::basekit::nslice<int> &palette);
};

/**
 * Make a bluered palette
 * @author Marina Mandelzweig
 */ 
class scopira::uikit::make_bluered_palette_v : public scopira::uikit::make_palette_base
{
  public:
    virtual std::string make_palette(const scopira::basekit::nslice<int> &palette);
};

/**
 * Make a bone palette
 * @author Marina Mandelzweig
 */ 
class scopira::uikit::make_bone_palette_v : public scopira::uikit::make_palette_base
{
  public:
    virtual std::string make_palette(const scopira::basekit::nslice<int> &palette);
};

/**
 * Make a copper palette
 * @author Marina Mandelzweig
 */ 
class scopira::uikit::make_copper_palette_v : public scopira::uikit::make_palette_base
{
  public:
    virtual std::string make_palette(const scopira::basekit::nslice<int> &palette);
};

/**
 * Make a spectra palette
 * @author Marina Mandelzweig
 */ 
class scopira::uikit::make_spectra_palette_v : public scopira::uikit::make_palette_base
{
  public:
    virtual std::string make_palette(const scopira::basekit::nslice<int> &palette);
};



/**
 * a window wrapper around gtkcolorselection
 *
 * Hmm, move this to a private class?
 *
 * @author Marina Mandelzweig
 */
class scopira::uikit::colorwindow : public scopira::coreui::window
{
	protected:
		GtkColorSelection *dm_colorsel;

  public:
    /// default constructor
    colorwindow(const std::string &title);

    /// sets the current selected color
    void set_selected_color(GdkColor color);
    /// gets the current selected color
    GdkColor get_selected_color(void) const;

    /// attach a new signal function to the ok button. you MUST call this
    /// at least once, and you should probably on_destroy the colorindow
    /// in it the widget will have been attache_gtk to the  colorindow
    void ok_connect(GtkSignalFunc func, gpointer func_data);

  protected:
    void init_gui();

  private:
    static void h_cancel(GtkWidget *widget, colorwindow *data);
};


/**
 * Listener of colour changing events
 * @author Aleksander Demko
 */
class scopira::uikit::colorbutton_reactor_i
{
  public:
    virtual ~colorbutton_reactor_i() { }
    /**
     * Called when the colour has changed
     * @param newcol new colour selected, in packed-colour format (0xRRGGBB)
     * @author Aleksander Demko
     */
    virtual void react_colorbutton(scopira::uikit::colorbutton *source, int newcol) = 0;
};


/**
* Color selection proponent. Edits one rgb color
* @param 0 (double-num_i, input) The rgb packed color
*
* @author Marina Mandelzweig
*/
class scopira::uikit::colorbutton : public scopira::coreui::canvas_base  
{
  protected:
    scopira::coreui::widget_context dm_pen;

    GdkColor dm_gdk_color; /// current gdk color

    /// current color int	 value
		int dm_color;
    bool dm_dirty; //do we need to recalculate data
    
    scopira::coreui::point dm_basep, dm_sizep;

    /// listener of events, if any
    scopira::uikit::colorbutton_reactor_i *dm_color_reactor;
    
  public:
    /// constrcutor
    colorbutton();


    /// caled when its handler setting time
    void set_colorbutton_reactor(scopira::uikit::colorbutton_reactor_i *reac) { dm_color_reactor = reac; }

    /// sets packed color
    void set_packed_color(const int c);
    /// gets packed color
    int get_packed_color(void) const { return dm_color; }

    /// sets gdk color
    void set_gdk_color(const GdkColor gdk_color) { dm_gdk_color = gdk_color; }
    /// gets gdk color
    GdkColor get_gdk_color(void) const { return dm_gdk_color; }
   
        
  protected:
      /// called before any repaints and such - should do colour and context alocing here
    virtual void handle_init(scopira::coreui::widget_canvas &v);
    /// handle resize
    virtual void handle_resize(scopira::coreui::widget_canvas &v);
    /// repaint handler
    virtual void handle_repaint(scopira::coreui::widget_canvas &v);
    /// draws it
    virtual void paint(scopira::coreui::widget_canvas &v);
    /// mouse click
    virtual void handle_press(scopira::coreui::widget_canvas &v, const scopira::coreui::mouse_event &mevt);    
};



/**
 * A colour pallette editor
 *
 * @author Marina Mandelzweig
 */ 
class scopira::uikit::colorpalette: public scopira::uikit::border_select_tiled_layout,
  public virtual scopira::uikit::colorbutton_reactor_i,
  public virtual scopira::tool::destroy_reactor_i
{
  protected:
    /// int data vector
    scopira::basekit::narray<int> dm_palette_data;
    /// current color selection win, if any
    scopira::uikit::colorwindow *dm_colorwin;

	public:
		/// constrcutor
    colorpalette(void);
     ~colorpalette(void);
    
    virtual void react_destroy(scopira::tool::object *source);

    virtual void react_spinbutton(scopira::coreui::spinbutton *source, int intval, double doubleval);
    virtual void react_vcrbutton(scopira::uikit::vcrbutton *source, int val);

    virtual void react_colorbutton(scopira::uikit::colorbutton *source, int newcol);

    /// resize the tiled table
    virtual void resize_palette(void);
    
    /// get the values associated with each tiled widgets
    scopira::basekit::narray<int,1> get_values(void) const {return dm_palette_data;}
    /// sets the data for the palette
    void set_values(scopira::basekit::narray<int,1> val) {dm_palette_data = val;}
      
    /// updates after a new palette data is set
    void update(void);
    
	protected:      
    /// create the tilred table
    virtual void create_table(int start_row, int start_column, int last_row, int last_column);

  private:
    /// called when the user has chosen a new shape in the shape window
    static void h_colorwindow_ok(GtkWidget *widget, colorpalette *data);
};



/**
 * A colour pallette editor viewer
 *
 * @author Marina Mandelzweig
 */ 
class scopira::uikit::colorpalette_v :  public scopira::coreui::viewwidget
{
  protected:
    scopira::tool::count_ptr<scopira::uikit::colorpalette> dm_palette;
    /// in case part of a record_i
    scopira::core::model_ptr<scopira::uikit::palette_m> dm_model;
    /// palettes button
    scopira::tool::count_ptr<scopira::coreui::view_button> dm_palettes_button;

	public:
		/// constrcutor
    colorpalette_v(void);
    
    virtual void bind_model(scopira::core::model_i *sus);
    virtual void react_model_update(scopira::core::model_i *sus, scopira::core::view_i *src);

    virtual void react_button(scopira::coreui::button *source, int actionid);
    
    /// update a widget/palette size
    virtual void on_change();

	protected:
    void init_gui(void);
};

#endif

