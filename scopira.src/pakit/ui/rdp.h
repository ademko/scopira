
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

#ifndef __INCLUDED_PAKIT_UI_RDP_H__
#define __INCLUDED_PAKIT_UI_RDP_H__

#include <scopira/coreui/canvas.h>
#include <scopira/coreui/entry.h>
#include <scopira/coreui/label.h>
#include <scopira/coreui/spinbutton.h>
#include <scopira/uikit/frame.h>
#include <scopira/uikit/matrix.h>

#include <pakit/ui/models.h>

namespace pakit
{
  class calc_distance_matrix_base;

  class calc_distance_via_euclidean_v;
  class calc_distance_via_cityblock_v;
  class calc_distance_via_max_distance_v;

  class rdp_space_v;
  class rdp_space_patterns_v;
  class rdp_scatterplot_space_patterns_v;

  class rdp_2d_pick_v;
  class rdp_3d_pick_v;

  class rdp_2d_plot_v;
}

/**
 * Helper base class for little views that calculate distance matrices.
 * Descedants need only override calc_distance and register their view.
 *
 * This is a gui-less view base class.
 *
 * @author Aleksander Demko
 */
class pakit::calc_distance_matrix_base : public scopira::core::view_i
{
  public:
    virtual void bind_model(scopira::core::model_i *sus);

    // returns the name
    virtual std::string calc_distance_matrix(const scopira::basekit::nslice<double,2> &patterns,
      scopira::basekit::narray<double,2> &outmatrix) = 0;
};

/**
 * @author Aleksander Demko
 */ 
class pakit::calc_distance_via_euclidean_v : public pakit::calc_distance_matrix_base
{
  public:
    virtual std::string calc_distance_matrix(const scopira::basekit::nslice<double,2> &patterns,
      scopira::basekit::narray<double,2> &outmatrix);
};

/**
 * @author Aleksander Demko
 */ 
class pakit::calc_distance_via_cityblock_v : public pakit::calc_distance_matrix_base
{
  public:
    virtual std::string calc_distance_matrix(const scopira::basekit::nslice<double,2> &patterns,
      scopira::basekit::narray<double,2> &outmatrix);
};

/**
 * @author Aleksander Demko
 */ 
class pakit::calc_distance_via_max_distance_v : public pakit::calc_distance_matrix_base
{
  public:
    virtual std::string calc_distance_matrix(const scopira::basekit::nslice<double,2> &patterns,
      scopira::basekit::narray<double,2> &outmatrix);
};

/**
 * produces initial RDP projections from a distance matrix
 * @author Aleksander Demko
 */
class pakit::rdp_space_v : public scopira::core::view_i
{
  public:
    virtual void bind_model(scopira::core::model_i *sus);
};

/**
 * Produces initial RDP projections from a distance matrix.
 *
 * This version outputs patterns_m rather than rdp_2d_m.
 *
 * 2D only (for now). Add a checkbox and a 3rd entry box in the future
 * if you need 3D.
 *
 * @author Aleksander Demko
 */
class pakit::rdp_space_patterns_v : public scopira::coreui::viewwidget
{
  protected:
    scopira::core::model_ptr<pakit::distances_m> dm_model;

    scopira::tool::count_ptr<scopira::coreui::spinbutton> dm_n1, dm_n2;

  public:
    rdp_space_patterns_v(void);

    virtual void bind_model(scopira::core::model_i *sus);
    virtual void react_button(scopira::coreui::button *source, int actionid);
};

/**
 * Produces RDP projections for pairs of reference pairs, as in the
 * RDP scatterplot, from a distance matrix.
 *
 * This version outputs patterns_m rather than rdp_2d_m.
 *
 * @author Brion Dolenko
 */
class pakit::rdp_scatterplot_space_patterns_v : public scopira::coreui::viewwidget
{
  protected:
    scopira::core::model_ptr<pakit::distances_m> dm_model;

    /// reference points selection container
    scopira::tool::count_ptr<scopira::coreui::grid_layout> dm_pair1_box, dm_pair2_box;
    scopira::tool::count_ptr<scopira::uikit::frame> dm_ref_frame;
    /// first pair reference points
    scopira::tool::count_ptr<scopira::coreui::entry> dm_pair1_n1_entry, dm_pair1_n2_entry;
    /// first pair reference points entry labels
    scopira::tool::count_ptr<scopira::coreui::label> dm_pair1_n1_label, dm_pair1_n2_label;
    /// second pair reference points
    scopira::tool::count_ptr<scopira::coreui::entry> dm_pair2_n1_entry, dm_pair2_n2_entry;
    /// second pair reference points entry labels
    scopira::tool::count_ptr<scopira::coreui::label> dm_pair2_n1_label, dm_pair2_n2_label;

  public:
    rdp_scatterplot_space_patterns_v(void);

    virtual void bind_model(scopira::core::model_i *sus);
    virtual void react_button(scopira::coreui::button *source, int actionid);
};

/**
 * n* changer/picker for rdp spaces
 * @author Aleksander Demko
 */
class pakit::rdp_2d_pick_v : public scopira::coreui::viewwidget
{
  protected:
    scopira::core::model_ptr<pakit::rdp_2d_m> dm_model;
    scopira::tool::count_ptr<scopira::coreui::spinbutton> dm_n1, dm_n2;

  public:
    /// ctor
    rdp_2d_pick_v(void);

    virtual void bind_model(scopira::core::model_i *sus);
    virtual void react_model_update(scopira::core::model_i *sus, scopira::core::view_i *src);
    virtual void react_button(scopira::coreui::button *source, int actionid);
};

/**
 * n* changer/picker for rdp spaces
 * @author Aleksander Demko
 */
class pakit::rdp_3d_pick_v : public scopira::coreui::viewwidget
{
  protected:
    scopira::core::model_ptr<pakit::rdp_3d_m> dm_model;
    scopira::tool::count_ptr<scopira::coreui::spinbutton> dm_n1, dm_n2, dm_n3;

  public:
    /// ctor
    rdp_3d_pick_v(void);

    virtual void bind_model(scopira::core::model_i *sus);
    virtual void react_model_update(scopira::core::model_i *sus, scopira::core::view_i *src);
    virtual void react_button(scopira::coreui::button *source, int actionid);
};

/**
 * Core 2D RDP separation plotter.
 * A basic, simple version. Features may be added through
 * subclassing and inspectors.
 * @author Aleksander Demko
 */
class pakit::rdp_2d_plot_v : public scopira::coreui::zoomed_canvas_base,
  public virtual scopira::core::view_i
{
  protected:
    enum color_info {
      // colour stuff
      textcol_c = 0x000000,
      backcol_c = 0xFFFFFF
    };

    struct pixpoint {
      int index, x, y;
    };

    /// current class vector (this might be from the link in the rdp_2d_record
    /// or from the inslots in "module mode"
    scopira::tool::count_ptr<scopira::basekit::narray_o<int,1> > dm_classes;
    /// current distance matrix (this might be from the link in the rdp_2d_record
    /// or from the inslots in "module mode"
    scopira::tool::count_ptr<scopira::basekit::narray_o<double,2> > dm_distancem;
    // maybe eventually training here too

    /// current data set
    scopira::core::model_ptr<pakit::rdp_2d_m> dm_data;
    /// current colours
    scopira::core::model_ptr<scopira::uikit::palette_m> dm_colors;
    /// current shapes palette
    scopira::core::model_ptr<scopira::uikit::shapes_m> dm_shapes;

    // corresponding screen points for each RDP space point
    std::string dm_title;
    typedef std::vector< pixpoint > coords_t;
    coords_t dm_coords;

    scopira::coreui::widget_context dm_pen;
    scopira::coreui::font dm_font;

    pixpoint dm_n_click;      // index -1 for none... first click of 2 (indexes into dm_coords)
    pixpoint dm_n_sel;        // index -1 for none... a SELECTED point (indexes into dm_coords)
    scopira::coreui::point dm_basep, dm_sizep;   // base image coord and size of plot display, in pixels
    scopira::coreui::point dm_A, dm_B;       // pixel points of N1 N2
    double dm_perpix;           // how many real RDP space units PER screen pixel
    int dm_numclass;    // number of classes

    const char * dm_gooddata; // NULL if good, notnull == error msg
    int dm_zoom;        /// current zoom factor

    /// count vec (numclass X bincount);
    scopira::basekit::narray<int,2> dm_countmatrix;
    /// histogram basep, uses same sizep though
    scopira::coreui::point dm_hbasep, dm_hsizep;
    /// bin size, in pixels
    short dm_binsize;
    /// max number of classes in any element in countvec
    int dm_maxbincount;

    scopira::tool::count_ptr<scopira::coreui::widget> dm_basewidget;

  public:
    /// ctor
    rdp_2d_plot_v(void);

    virtual void handle_menu(scopira::coreui::widget_canvas &v, intptr_t menuid);
    virtual void handle_init(scopira::coreui::widget_canvas &v);
    virtual void handle_resize(scopira::coreui::widget_canvas &v);
    virtual void handle_repaint(scopira::coreui::widget_canvas &v);
    virtual void handle_press(scopira::coreui::widget_canvas &v, const scopira::coreui::mouse_event &mevt);
    virtual void handle_zoom(int newfactor);

    virtual void bind_model(scopira::core::model_i *sus);
    virtual void react_model_update(scopira::core::model_i *sus, scopira::core::view_i *src);

  protected:
    /// descendants may overirde this... be sure to call this
    /// one first though
    virtual const char * prep_data(scopira::coreui::widget_canvas &v);
    /// descendants may overirde this... be sure to call this
    /// one first though
    virtual void paint(scopira::coreui::widget_canvas &v);
    /// draws one point
    void paint_point(scopira::coreui::widget_canvas &v, int idx, int x, int y);
    /// finds the best index into dm_coords that is closed to the given
    /// pixels... -1 on none
    int find_closest_xy(int x, int y);
    /// called after changes to dm_classes
    void calc_numclass(void);
};

#endif

