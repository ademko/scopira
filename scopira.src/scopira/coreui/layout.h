
/*
 *  Copyright (c) 2004    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#ifndef __INCLUDED__SCOPIRA_COREUI_LAYOUT_H__
#define __INCLUDED__SCOPIRA_COREUI_LAYOUT_H__

#include <gtk/gtk.h>

#include <list>
#include <vector>
#include <string>

#include <scopira/tool/array.h>

#include <scopira/coreui/widget.h>
#include <scopira/coreui/export.h>

// THIS FILE HAS BEEN FULLY DOCUMENTED

namespace scopira
{
  namespace coreui
  {
    class box_layout;
    class alignment_layout;
    class buttonbox_layout;
    class tab_layout;
    class split_layout;
    class grid_layout;

    class frame;

    class button_constants;
    class button_layout;

    class button_reactor_i; //fwd
  }
}

/*

Naming rules for reactors.

See button_p for an implementation:

Let C_p be the widget type. (for C="button", this is button_p)

The reactor interface will be:

class C_reactor_i {
  public:
    virtual void react_C(C_p *source, ...parms...) = 0;
}

The additional methods added to C_p should be:
  set_C_reactor(C_reactor_i *react);

rename set to add if the widget may have _multiple_ reactors.

*/

/**
 * A horizontal or vertical packing box.
 *
 * @author Aleksander Demko
 */
class scopira::coreui::box_layout : public scopira::coreui::widget
{
  protected:
    std::list<scopira::tool::count_ptr<widget> > dm_children;

  public:
    /**
     * Constructor.
     * @param horiz true for horizontal, false for vertical.
     * @param homogeneous All the widgets in this box will take up the same amount of space.
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT box_layout(bool horiz, bool homogeneous, int spacing = 0);

    /**
     * Sets the (additional) spacing added between each widget.
     *
     * @author Aleksander Demko
     */
    void set_spacing(int pix) { gtk_box_set_spacing(GTK_BOX(dm_widget), pix); }

    /**
     * Adds a widget to the box.
     * Might want to add more options here.
     *
     * @param wid the widget
     * @param expand should this child accept any extra space that may be allocated?
     * @param fill If true, any extra expanded space is given to the
     * widget itself, rather than just padding around the widget.
     * Only relevant if expand is true.
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT void add_widget(widget *wid, bool expand, bool fill, int padding = 0);

        /**
     * Adds a widget to the end of the box.
     * Might want to add more options here.
     *
     * @param wid the widget
     * @param expand should this child accept any extra space that may be allocated?
     * @param fill If true, any extra expanded space is given to the
     * widget itself, rather than just padding around the widget.
     * Only relevant if expand is true.
     *
     * @author Aleksander Demko
     * @author Marina Mandelzweig
     */
    SCOPIRAUI_EXPORT void add_widget_end(widget *wid, bool expand, bool fill, int padding = 0);

    /**
     * Removes a widget from the box layout.
     * The widget must exist in this box.
     *
     * @author Rodrigo Vivanco
     */
    SCOPIRAUI_EXPORT void remove_widget(widget *wid);

  protected:
    /// inheritance cotr, doesnt call widget::init_gui, but does set dm_widget
    SCOPIRAUI_EXPORT box_layout(GtkWidget *w);
};

/**
 * A layout container than holds one subwidget, but controls
 * its relativly and total placement (alighment)
 *
 * @author Aleksander Demko
 */ 
class scopira::coreui::alignment_layout : public scopira::coreui::widget
{
  private:
    scopira::tool::count_ptr<scopira::coreui::widget> dm_child;

  public:
    /**
     * Constructor.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRAUI_EXPORT alignment_layout(void);

    /**
     * All-in-one constructor.
     *
     * @param wid the widget to add
     * @param xalign the horizontal alignment (0 is left, 1 is right)
     * @param yalign the veritical alignment (0 is top, 1 is bottom)
     * @param xscale the horizontal scaling (0 is dont expant, 1 is expand to all space)
     * @param xscale the veritical scaling (0 is dont expant, 1 is expand to all space)
     * @author Aleksander Demko
     */ 
    SCOPIRAUI_EXPORT alignment_layout(widget *wid, float xalign = 0, float yalign = 0, float xscale = 0, float yscale = 0);

    /**
     * Adds the one subwidget to the alignment_layout
     *
     * @param wid the widget to add
     * @param xalign the horizontal alignment (0 is left, 1 is right)
     * @param yalign the veritical alignment (0 is top, 1 is bottom)
     * @param xscale the horizontal scaling (0 is dont expant, 1 is expand to all space)
     * @param xscale the veritical scaling (0 is dont expant, 1 is expand to all space)
     * @author Aleksander Demko
     */ 
    SCOPIRAUI_EXPORT void add_widget(widget *wid, float xalign = 0, float yalign = 0, float xscale = 0, float yscale = 0);
};

/**
 * A special box for buttons.
 *
 * @author Aleksander Demko
 */
class scopira::coreui::buttonbox_layout : public scopira::coreui::box_layout
{
  public:
    enum {
      style_default_c = GTK_BUTTONBOX_DEFAULT_STYLE,
      style_spread_c = GTK_BUTTONBOX_SPREAD,
      style_edge_c = GTK_BUTTONBOX_EDGE,
      style_start_c = GTK_BUTTONBOX_START,
      style_end_c = GTK_BUTTONBOX_END,
    };
  public:
    /**
     * Constructor.
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT buttonbox_layout(bool horiz, int layoutstyle = style_end_c);
};

/**
 * A layout of two children that are seperated by a user adjustable border.
 *
 * @author Aleksander Demko
 */
class scopira::coreui::split_layout : public scopira::coreui::widget
{
  protected:
    scopira::tool::count_ptr<widget> dm_0, dm_1;
  public:
    /**
     * Constructor.
     *
     * @param horiz true for horizontal, false for vertical.
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT split_layout(bool horiz);

    /**
     * Adds a widget to the box.
     *
     * @param wid the widget
     * @param position position. 0 for top (or left), 1 for bottom (or right)
     * @param expand should this child accept any extra space that may be allocated?
     * @param fill If true, any extra expanded space is given to the
     * widget itself, rather than just padding around the widget.
     * Only relevant if expand is true.
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT void add_widget(widget *wid, short position, bool expand, bool fill);
};

/**
 * A Tabbed-pane layout. Has multiple tabs, each containing a
 * widget.
 * @author Aleksander Demko
 */
class scopira::coreui::tab_layout : public scopira::coreui::widget
{
  protected:
    typedef std::pair<scopira::tool::count_ptr<widget>,
            scopira::tool::count_ptr<widget> > widgetpair_t;
    typedef std::vector<widgetpair_t > children_t;
    children_t dm_children;

  public:
    /// Constructor
    SCOPIRAUI_EXPORT tab_layout(void);

    /**
     * Adds a widget with the given tab title.
     * @param wid the widget to add
     * @param tablabel the label to assign this widget in the tab
     * @return the index of this tab, starts at 0 etc
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT int add_widget(widget *wid, const std::string &tablabel);

    /**
     * Adds a widget with the given tab title.
     * @param wid the widget to add
     * @param tablabel the label to assign this widget in the tab
     * @return the index of this tab, starts at 0 etc
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT int add_widget(widget *wid, widget *labelwidget);

    /**
     * Adds a widget with the given tab title.
     * @param wid the widget to add
     * @param tablabel the label to assign this widget in the tab
     * @return the index of this tab, starts at 0 etc
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT int add_widget_prepend(widget *wid, const std::string &tablabel);

    /**
     * Adds a widget with the given tab title.
     * @param wid the widget to add
     * @param tablabel the label to assign this widget in the tab
     * @return the index of this tab, starts at 0 etc
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT int add_widget_prepend(widget *wid, widget *labelwidget);

    /**
     * switch the view to the given tabbed pane
     * @param id the id from the
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT void switch_tab(int id);

    /**
     * Sets the tab label of a particular page
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT void set_tab_label(int id, const std::string &tablabel);

    /**
     * Finds a tab, baed on a widget.
     *
     * Returns -1 for not found.
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT int find_tab(widget *wid);

    /**
     * Removes a tab, by id.
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT void remove_tab(int idx);
};

/**
 * A layout widget that contains other widgets.
 * The contained widgets are layed out over a grid of cells.
 *
 * @author Aleksander Demko
 */
class scopira::coreui::grid_layout : public scopira::coreui::widget
{
  protected:
    scopira::tool::basic_matrix<scopira::tool::count_ptr<widget> > dm_children;

  public:
    /**
     * Constructor.
     * Constructs a grid layout of the given dimensions
     * @param width the width (in calls) of the grid
     * @param height the height (in calls) of the grid
     * @param homo true if all the cells should have the same size
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT grid_layout(int width, int heigth, bool homo = false);

    /**
     * Add a widget to this layout.
     *
     * @param wid the widget to add
     * @param x the x/column number (0-based)
     * @param y the y/row number (0-based)
     * @param w the width (in cells). default is 1
     * @param h the height (in cells). default is 1
     * @param expandH should the widget be exapanded to fill all available horizontal space?
     * @param expandV should the widget be exapanded to fill all available veritcal space?
     * @param paddingH amount of padding on the lef and right of  the child widget
     * @param paddingV amount of padding above and below the child widget
     * @return wid
     * @author Aleksander Demko
     * @author Marina Mandelzweig
     */
    SCOPIRAUI_EXPORT void add_widget(widget *wid, int x, int y, int w=1, int h=1, bool expandH=true, bool expandV=true, int paddingH=2, int paddingV=2);
};

/**
 * A decorative frame and an optional label.
 *
 * @author Aleksander Demko
 */
class scopira::coreui::frame : public virtual scopira::coreui::widget
{
  public:
    /// ctor
    SCOPIRAUI_EXPORT frame(void);

    /// ctor
    SCOPIRAUI_EXPORT frame(const std::string &label);

    /// api access routines
    SCOPIRAUI_EXPORT void set_label(const std::string &label);

    /// add a container inside
    SCOPIRAUI_EXPORT void add_widget(scopira::coreui::widget *w);

  private:
    /// contained widget
    scopira::tool::count_ptr<scopira::coreui::widget> dm_child;
};

/**
 * An empty class that defines all the button constants.
 * This is so that various classes, not just button_layout, may have
 * these as in-class constants.
 *
 * @author Aleksander Demko
 */
class scopira::coreui::button_constants
{
  public:
    /// stock actions
    enum {
      action_none_c = 0,
      action_close_c,
      action_apply_c,
      action_revert_c,
      action_yes_c,
      action_no_c,
      action_save_c,
      action_open_c,
      action_clear_c,
      action_refresh_c,

      action_user_c = 100,  // use action ids can start counting from 100
    };
    enum {
      button_none_c = 0,
      button_apply_c = 1,
      button_ok_c = 2,        // produces an apply_c then close_c event
      button_close_c = 4,
      button_cancel_c = 8,    // same as close, just different label
      button_revert_c = 16,   // produces action_revert_c
      button_yes_c = 32,      // produces yes then close
      button_no_c = 64,       // produces no then close
      button_save_c = 128,    // produces save then close
      button_open_c = 256,    // produces open then close
      button_clear_c = 512,   // produces action_clear_c
      button_refresh_c = 1024,// produces action_refresh_c
    };
};

/**
 * A layout that has a main area and a button area that you can add too.
 *
 * @author Aleksander Demko
 */
class scopira::coreui::button_layout : public scopira::coreui::widget,
  public scopira::coreui::button_constants
{
  public:
    /**
     * Constructor. You must provide the main widget immiediatly.
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT button_layout(widget *mainwidget);

    /**
     * Gets the action area, for you to add stuff too.
     * This will already have mainwidget added to it.
     *
     * @author Aleksander Demko
     */
    //box_layout * get_window_area(void) const { return dm_actionbox.get(); }

    /**
     * Gets the button area, so you can manually add buttons and stuff too.
     *
     * @author Aleksander Demko
     */
    //box_layout * get_button_area(void) const { return dm_butbox.get(); }

    /**
     * Use this instead of direct box manipulation for extra consistancy.
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT void add_button(widget *but);

    /**
     * Use this instead of direct box manipulation for extra consistancy.
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT void add_button_end(widget *but);

    /**
     * Adds a collection of stock buttons with the given listener.
     *
     * @param button_mask a bitmask of button_*_c defines. It may be 0, which makes this method do nothing.
     * @param reac the listener that will receive the button events, cannot be null.
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT void add_stock_buttons(int button_mask, scopira::coreui::button_reactor_i *reac);

  private:
    scopira::tool::count_ptr<box_layout> dm_mainbox, /*dm_actionbox,*/ dm_butbox;
};
#endif

