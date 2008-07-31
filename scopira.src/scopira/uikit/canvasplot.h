
/*
 *  Copyright (c) 2006    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#ifndef __INCLUDED_SCOPIRA_UIKIT_CANVASPLOT_H__
#define __INCLUDED_SCOPIRA_UIKIT_CANVASPLOT_H__

#include <scopira/core/view.h>
#include <scopira/core/model.h>
#include <scopira/coreui/canvas.h>
#include <scopira/coreui/export.h>

#include <scopira/uikit/models.h>

namespace scopira
{
  namespace uikit
  {
    class canvas_plot_v;

    class palette_plot_v;
    class image_plot_v;
  }
}

/**
 * A baseclass that is useful for building things that "plot" stuff in 2D.
 * This class provides scrolling and zooming calculations.
 *
 * This version lets sub classes draw on a virtual canvas of some size, but only shows some version.
 * Very handy for large data sets.
 *
 * Basically, descendants just provide a handle_canvas_plot().
 * Usually you'll also provide an empty/near basic ctor, implement view_i
 * and implement bind_model too.
 *
 * @author Aleksander Demko
 */ 
class scopira::uikit::canvas_plot_v : public scopira::coreui::zoomed_scrolled_canvas_base
{
  public:
    /**
     * Constructor.
     *
     * This will call init_gui, decedants shouldn't do that.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRAUI_EXPORT canvas_plot_v(void);

    SCOPIRAUI_EXPORT virtual void handle_init(scopira::coreui::widget_canvas &v, coord &crd);
    SCOPIRAUI_EXPORT virtual void handle_resize(scopira::coreui::widget_canvas &v, coord &crd);
    SCOPIRAUI_EXPORT virtual void handle_repaint(scopira::coreui::widget_canvas &v, coord &crd);
    SCOPIRAUI_EXPORT virtual void handle_scroll(scopira::coreui::widget_canvas &v, coord &crd);
    SCOPIRAUI_EXPORT virtual void handle_zoom(int newfactor);

    /**
     * Descendants implment this. This is called when it's time to draw
     * the plot.
     *
     * The total size of the canvas (and the size you should fill) is totalw by
     * totalh. You should always scale and draw your whole scene to fix this size.
     * However, all your rendering/drawing must be shifted by the basex/basey.
     * For example, instead of rendering a point at 5,5, render it at 5-basex, 5-basey.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRAUI_EXPORT virtual void handle_canvas_plot(scopira::coreui::pixmap_canvas &v, int basex, int basey, int totalw, int totalh) = 0;

  protected:
    /**
     * Descendants can call this if they have new data.
     * This basically sets the dirty flag (calling handle_canvas_plot)
     * to be called, and does a redraw.
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT void request_plot(void);

  private:
    enum {
      border_x_c = 20,
      border_y_c = 10,
    };
    scopira::coreui::pixmap_canvas dm_back;
    bool dm_dirty, dm_dirtyzoom;
    int dm_zoom; // 0..1000, initially, 500
    int dm_basex, dm_basey, dm_totalw, dm_totalh, dm_draww, dm_drawh;
};

/**
 * Plots a palette (basically, a viewer for palette objects).
 *
 * @author Aleksander Demko
 */ 
class scopira::uikit::palette_plot_v : public scopira::uikit::canvas_plot_v, public scopira::core::view_i
{
  public:
    /// ctor
    palette_plot_v(void);

    virtual void handle_canvas_plot(scopira::coreui::pixmap_canvas &v, int basex, int basey, int totalw, int totalh);

    virtual void bind_model(scopira::core::model_i *sus);
    virtual void react_model_update(scopira::core::model_i *sus, scopira::core::view_i *src);

  private:
    scopira::core::model_ptr<scopira::uikit::palette_m> dm_model;
};

/**
 * Views packed RGB images via the plotting class
 *
 * @author Aleksander Demko
 */ 
class scopira::uikit::image_plot_v : public scopira::uikit::canvas_plot_v, public scopira::core::view_i
{
  public:
    /// ctor
    image_plot_v(void);

    virtual void handle_canvas_plot(scopira::coreui::pixmap_canvas &v, int basex, int basey, int totalw, int totalh);

    virtual void bind_model(scopira::core::model_i *sus);
    virtual void react_model_update(scopira::core::model_i *sus, scopira::core::view_i *src);

  private:
    scopira::core::model_ptr<scopira::uikit::narray_m<int,2> > dm_model;

    scopira::coreui::rgb_image dm_img;
};

#endif

