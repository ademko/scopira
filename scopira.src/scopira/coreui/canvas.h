
/*
 *  Copyright (c) 2002-2004    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#ifndef __INCLUDED__SCOPIRA_COREUI_XCANVAS_H__
#define __INCLUDED__SCOPIRA_COREUI_XCANVAS_H__

#include <vector>

#include <gtk/gtkscrollbar.h>

#include <scopira/tool/array.h>

#include <scopira/coreui/canvascommon.h>
#include <scopira/coreui/menu.h>
#include <scopira/tool/platform.h>
#include <scopira/coreui/export.h>

//
// Scopira Canvas 2D framework
// Simply a weak wrapper around GDK
//

namespace scopira
{
  namespace coreui
  {
    class menu_handler_base;

    class canvas_base;
    class zoomed_canvas_base;
    class scrolled_canvas_base;
    class zoomed_scrolled_canvas_base;
  }
}

/**
 * Base class for the various canvas things.
 *
 * Basically, decendants should read the rules on init_gui() and then perhaps
 * override various virtual methods.
 *
 * @author Aleksander Demko
 */
class scopira::coreui::menu_handler_base : public virtual scopira::coreui::widget
{
  public:
    /**
     * popup menu class. make it, then pop it!
     * @author Aleksander Demko
     */
    class popup_menu
    {
      private:
        menu_pop_builder dm_builder;
      public:
        /**
         * Instantiate a new popup menu.
         * @param base is usually "this," your canvas decendant.
         * @author Aleksander Demko
         */
        SCOPIRAUI_EXPORT popup_menu(menu_handler_base *base);

        /**
         * adds a menu item.
         * The item will trigger a handle_menu
         * with the given ID. If you'd like to use
         * a pointer instead of an id/enum, use the
         * ptr_to_int stuff in scopira/tool/util.h.
         * @author Aleksander Demko
         */
        SCOPIRAUI_EXPORT void add_item(const std::string &label, intptr_t id);
        /// adds a seperator
        SCOPIRAUI_EXPORT void add_separator(void);
        /// push a new menu
        SCOPIRAUI_EXPORT void push_menu(const std::string &label);
        /// pop the current menu
        SCOPIRAUI_EXPORT void pop_menu(void);
        /// display the final results
        SCOPIRAUI_EXPORT void popup(void);
    };

  protected:
    /// ctor
    SCOPIRAUI_EXPORT menu_handler_base(void);
    //// dtor
    SCOPIRAUI_EXPORT virtual ~menu_handler_base();
    /// this is a short range override. only the immediate bases shoudl touch this
    SCOPIRAUI_EXPORT virtual void on_menu(intptr_t id) = 0;

    /**
     * Descendants must call this, or use the alternate style.
     *
     * This version will create a drawing area, and then call widget::init_gui(onthatdrawingarea).
     *
     * Alternativly, you may create the drawing area manually (with make_drawing_area)
     * and then call widget::init_gui yourself (or via some other call, say a layout
     * container or viewwidget)
     *
     * @param w the min width of the canvas, 0 is the default
     * @param h the min height of the canvas, 0 is the default
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT void init_gui(int w = 0, int h = 0);

    /**
     * Creates and returns the drawing area.
     * If you use this, then do NOT use the init_gui(), but rather
     * call widget::init_gui.
     * Return widget should be reference counted.
     *
     * @param w the min width of the canvas, 0 is the default
     * @param h the min height of the canvas, 0 is the default
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT widget * make_drawing_area(int w = 0, int h = 0);

    /**
     * Internally driver for make_drawing_area. You shouldn't
     * override this.
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT virtual GtkWidget * make_drawing_area_impl(void) = 0;

  private:
    GtkWidget *dm_current_popup_menu;

    void set_popup_menu(GtkWidget *pop);

  private:
    static void h_menu(GtkWidget *widget, gpointer dat);
};

/**
 * a base class for canvase based proponents, without scrolling
 *
 * @author Aleksander Demko
 * @author Rodrigo Vivanco
 * @author Shantha Ramachandran
 */
class scopira::coreui::canvas_base : public scopira::coreui::menu_handler_base
{
  public:
    /**
     * request a resize event call
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT void request_resize(void);

    /**
     * request a redraw
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT void request_redraw(void);

    /**
     * resize the canvas
     *
     * @author Shantha Ramachandran
     */
    SCOPIRAUI_EXPORT void request_resize(int width, int height);

  protected:

    /// ctor
    SCOPIRAUI_EXPORT canvas_base(void);

    
    /// dont override this
    SCOPIRAUI_EXPORT virtual void on_menu(intptr_t id);

    /**
     * creates and returns the drawing area.
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT virtual GtkWidget * make_drawing_area_impl(void);

    // user overiden handlers

    /// called when a menu entry thingie was pressed
    SCOPIRAUI_EXPORT virtual void handle_menu(scopira::coreui::widget_canvas &v, intptr_t menuid);

    /// called before any repaints and such - should do colour and context alocing here
    /// default implmenetaion does nothing
    SCOPIRAUI_EXPORT virtual void handle_init(scopira::coreui::widget_canvas &v);

    // split generic "handle_press" event into "button_press" and "button_release" to reflect GTK events

    /// handle resize
    /// default implmenetaion does nothing
    SCOPIRAUI_EXPORT virtual void handle_resize(scopira::coreui::widget_canvas &v);

    /// repaint handler
    /// default implmenetaion does nothing
    SCOPIRAUI_EXPORT virtual void handle_repaint(scopira::coreui::widget_canvas &v);

    /// mouse click
    /// default implmenetaion does nothing
    SCOPIRAUI_EXPORT virtual void handle_press(scopira::coreui::widget_canvas &v, const scopira::coreui::mouse_event &mevt);

    // mouse move default implementaion does nothing
    // @author Rodrigo Vivanco
    SCOPIRAUI_EXPORT virtual void handle_motion(scopira::coreui::widget_canvas &v, const scopira::coreui::mouse_event &mevt);

  protected:
    /// Descendants may access this gtk drawing area instance directly, if needed
    GtkWidget *dm_drawarea;

  private:
    bool dm_doneinit;

    /// static handlers
    static void h_realize(GtkWidget* widget, gpointer data);
    static gint h_expose(GtkWidget* widget, GdkEventExpose* event, gpointer data);
    static gint h_configure(GtkWidget* widget, GdkEventConfigure* event, gpointer data);

    // mouse events
    // @author Rodrigo Vivanco
    static gint h_button(GtkWidget *widget, GdkEventButton *event, gpointer data);
    static gint h_motion(GtkWidget *widget, GdkEventMotion *event, gpointer data);
};

/**
 * xcanvas_base_p, with zooming
 *
 * @author Aleksander Demko
 */
class scopira::coreui::zoomed_canvas_base : public scopira::coreui::canvas_base
{
  protected:
    SCOPIRAUI_EXPORT zoomed_canvas_base(void) { }
    
    /// called when zoom changes, factor is [0..1000], initally, it's 500
    /// default implmenetaion does nothing
    SCOPIRAUI_EXPORT virtual void handle_zoom(int newfactor);

    /// creates and returns the drawing area
    SCOPIRAUI_EXPORT virtual GtkWidget * make_drawing_area_impl(void);

    /// sets the zoom factor
    void set_zoom(int newfactor) { dm_zoomer.set_factor(newfactor); }

  private:
    /// my zoomed widget, created by make_drawing_area_impl
    zoom_widget dm_zoomer;

    static void h_value_change(GtkAdjustment *adj, gpointer data);
};

/**
 * a base class for canvase based proponents, with scrolling
 *
 * @author Aleksander Demko
 * @author Rodrigo Vivanco
 */
class scopira::coreui::scrolled_canvas_base : public scopira::coreui::menu_handler_base
{
  public:
    typedef scopira::coreui::mouse_event mouse_event;

    /**
     * request a resize event call
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT void request_resize(void);

    /**
     * request a redraw
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT void request_redraw(void);

    /// sets whether to show the scrollbars - must be called before make_drawing_area_impl
    SCOPIRAUI_EXPORT void set_show_scroll(bool scroll);

    /**
     * resize the canvas drawable area to specified dimensions.
     *
     * @author Rodrigo Vivanco
     */
    SCOPIRAUI_EXPORT void request_resize(int width, int height);

  protected:
    /**
     * an object that encapsulates the virtual coordinates
     * the scroll bars provide
     *
     * @author Aleksander Demko
     */
    class coord {
      private:
        scrolled_canvas_base &dm_can;
        bool dm_dirty;

      public:
        SCOPIRAUI_EXPORT coord(scrolled_canvas_base &can);
        SCOPIRAUI_EXPORT ~coord();

        SCOPIRAUI_EXPORT void set_hbounds(float lower, float upper, float value, float step_inc,
          float page_inc, float page_size);
        SCOPIRAUI_EXPORT void set_vbounds(float lower, float upper, float value, float step_inc,
          float page_inc, float page_size);

        SCOPIRAUI_EXPORT void set_hvalue(float val);
        SCOPIRAUI_EXPORT void set_vvalue(float val);

        SCOPIRAUI_EXPORT float hvalue(void) const;
        SCOPIRAUI_EXPORT float vvalue(void) const;

        int hval(void) const { return static_cast<int>(hvalue()); }
        int vval(void) const { return static_cast<int>(vvalue()); }
    };

    /// ctor
    SCOPIRAUI_EXPORT scrolled_canvas_base(void);

    /// dtor
    SCOPIRAUI_EXPORT ~scrolled_canvas_base(void);

    /// dont override this
    SCOPIRAUI_EXPORT virtual void on_menu(intptr_t id);

    // user overiden handlers

    /// called when a menu entry thingie was pressed
    SCOPIRAUI_EXPORT virtual void handle_menu(scopira::coreui::widget_canvas &v, coord &crd, intptr_t menuid);
    /// called before any repaints and such - should do colour and context alocing here
    /// default implmenetaion does nothing
    SCOPIRAUI_EXPORT virtual void handle_init(scopira::coreui::widget_canvas &v, coord &crd);
    /// handle resize
    /// default implmenetaion does nothing
    SCOPIRAUI_EXPORT virtual void handle_resize(scopira::coreui::widget_canvas &v, coord &crd);
    /// repaint handler
    SCOPIRAUI_EXPORT virtual void handle_repaint(scopira::coreui::widget_canvas &v, coord &crd) = 0;

    /// mouse click
    /// default implmenetaion does nothing
    SCOPIRAUI_EXPORT virtual void handle_press(scopira::coreui::widget_canvas &v, coord &crd, const scopira::coreui::mouse_event &mevt);

    // mouse move default implementaion does nothing
    // @author Rodrigo Vivanco
    SCOPIRAUI_EXPORT virtual void handle_motion(scopira::coreui::widget_canvas &v, coord &crd, const scopira::coreui::mouse_event &mevt);

    /// handle scroll
    /// default implmenetaion does nothing
    virtual void handle_scroll(scopira::coreui::widget_canvas &v, coord &crd);

  protected:
    bool dm_doneinit;
    bool dm_show_scroll;  // whether or not to show the scrollbars

    /**
     * creates and returns the drawing area.
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT virtual GtkWidget * make_drawing_area_impl(void);

    GtkScrollbar * get_hbar(void) { return dm_hbar; }
    GtkScrollbar * get_vbar(void) { return dm_vbar; }

  protected:
    /// Descendants may access this gtk drawing area instance directly, if needed
    GtkWidget *dm_drawarea;

  private:
    // the following may also be promoted to protected stutus if need be (see me though) -ademko
    GtkAdjustment *dm_hadj, *dm_vadj;
    GtkScrollbar *dm_hbar, *dm_vbar;

  private:
    /// static handlers
    static void h_realize(GtkWidget* widget, gpointer data);
    static gint h_expose(GtkWidget* widget, GdkEventExpose* event, gpointer data);
    static gint h_configure(GtkWidget* widget, GdkEventConfigure* event, gpointer data);
    static void h_value_change(GtkAdjustment *adj, gpointer data);

    // mouse events
    // @author Rodrigo Vivanco
    static gint h_button(GtkWidget *widget, GdkEventButton *event, gpointer data);
    static gint h_motion(GtkWidget *widget, GdkEventMotion *event, gpointer data);
};

/**
  * mixed zooming and scrolling canvas base
  * @author Aleksander Demko
  */
class scopira::coreui::zoomed_scrolled_canvas_base : public scopira::coreui::scrolled_canvas_base
{
  public:
    /// sets whether to show the zoomer - must be called before make_drawing_area_impl
    void set_show_zoom(bool zoom);

  protected:
    /// ctor
    SCOPIRAUI_EXPORT zoomed_scrolled_canvas_base(void);
    
    /// called when zoom changes, factor is [0..1000], initally, it's 500
    /// default implmenetaion does nothing
    SCOPIRAUI_EXPORT virtual void handle_zoom(int newfactor);
    /// creates and returns the drawing area
    SCOPIRAUI_EXPORT virtual GtkWidget * make_drawing_area_impl(void);
    /// sets the zoom factor
    void set_zoom(int newfactor) { dm_zoomer.set_factor(newfactor); }
    ///get the zoom bar
    GtkWidget * get_hscale(void) { return dm_zoomer.get_hscale(); }

  private:
    bool dm_show_zoom;  // whether or not to show the zoomer

    /// my zoomed widget, created by make_drawing_area_impl
    zoom_widget dm_zoomer;
    static void h_value_change(GtkAdjustment *adj, gpointer data);
};

#endif

