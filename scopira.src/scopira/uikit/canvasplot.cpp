
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

#include <scopira/uikit/canvasplot.h>

#include <scopira/tool/output.h>
#include <scopira/core/register.h>
#include <scopira/basekit/narray.h>
#include <scopira/basekit/math.h>

//BBtargets libscopiraui.so

using namespace scopira::basekit;
using namespace scopira::coreui;
using namespace scopira::uikit;

//
//
// canvas_plot_v
//
//

canvas_plot_v::canvas_plot_v(void)
{
  dm_dirty = true;
  dm_dirtyzoom = true;
  dm_zoom = 0;

  dm_totalw = 0;
  dm_totalh = 0;

  init_gui(200, 200);
}

void canvas_plot_v::handle_init(scopira::coreui::widget_canvas &v, coord &crd)
{
  dm_basex = 0;
  dm_basey = 0;
  dm_totalw = 0;
  dm_totalh = 0;

  crd.reset_hbounds();
  crd.reset_vbounds();

  set_zoom(0);
}

void canvas_plot_v::handle_resize(scopira::coreui::widget_canvas &v, coord &crd)
{
  dm_dirty = true;
}

void canvas_plot_v::handle_repaint(scopira::coreui::widget_canvas &v, coord &crd)
{
  if (dm_dirty) {
    dm_dirty = false;

    if (v.width() < 3*border_x_c || v.height() < 3*border_y_c) {
      // canvas too small
      widget_context pen(v);
      pen.set_foreground(0xFF0000); //red
      v.clear(pen);
      return;
    }

    dm_draww = v.width() - 2*border_x_c;
    dm_drawh = v.height() - 2*border_y_c;

    // repaint the back buffer
    if (dm_back.width() != dm_draww || dm_back.height() != dm_drawh)
      dm_back.resize(this, dm_draww, dm_drawh);

    pixmap_context pen(dm_back);
    pen.set_foreground(0xFFFFFF);    // white
    dm_back.clear(pen);

    if (dm_zoom == 0) {
      // special case
      dm_dirtyzoom = false;
      dm_basex = 0;
      dm_basey = 0;
      dm_totalw = dm_draww;
      dm_totalh = dm_drawh;

      crd.reset_hbounds();
      crd.reset_vbounds();
    }
    if (dm_dirtyzoom) {
      double ratiox, ratioy;

      dm_dirtyzoom = false;

      assert(dm_totalw>0);
      assert(dm_totalh>0);
      ratiox = (dm_basex+dm_draww/2.0) / dm_totalw;
      ratioy = (dm_basey+dm_drawh/2.0) / dm_totalh;

      double scaler = ::pow(10.0, 2.0 * dm_zoom / 1000);
      //OUTPUT << "scaler = " << scaler << '\n';

      dm_totalw = static_cast<int>(dm_draww * scaler);
      dm_totalh = static_cast<int>(dm_drawh * scaler);
      dm_basex = static_cast<int>(ratiox * dm_totalw - dm_draww/2);
      dm_basey = static_cast<int>(ratioy * dm_totalh - dm_drawh/2);

      // sanity checks
      if (dm_basex + dm_draww > dm_totalw)
        dm_basex -= dm_basex + dm_draww - dm_totalw;
      if (dm_basey + dm_drawh > dm_totalh)
        dm_basey -= dm_basey + dm_drawh - dm_totalh;
      if (dm_basex<0)
        dm_basex = 0;
      if (dm_basey<0)
        dm_basey = 0;

      crd.set_hbounds(0, static_cast<float>(dm_totalw), static_cast<float>(dm_basex), static_cast<float>(dm_draww/10), static_cast<float>(dm_draww), static_cast<float>(dm_draww));
      crd.set_vbounds(0, static_cast<float>(dm_totalh), static_cast<float>(dm_basey), static_cast<float>(dm_drawh/10), static_cast<float>(dm_drawh), static_cast<float>(dm_drawh));
    }

    // draw on it
//OUTPUT << "full redraw: " << dm_basex << "," << dm_basey << "  " << dm_totalw << "x" << dm_totalh << "  @[" << dm_draww << "x" << dm_drawh << "] " << '\n';
    handle_canvas_plot(dm_back, dm_basex, dm_basey, dm_totalw, dm_totalh);
  }

  // blit the back buffer to the screen
  widget_context pen(v);

  pen.set_foreground(0xFFFFFF); //white
  v.clear(pen);

  v.draw_canvas(pen, 0, 0, dm_back, border_x_c, border_y_c, dm_back.width(), dm_back.height());

  pen.set_foreground(0x000000); //black
  v.draw_rectangle(pen, false, border_x_c, border_y_c, v.width()-2*border_x_c, v.height()-2*border_y_c);
}

void canvas_plot_v::handle_scroll(scopira::coreui::widget_canvas &v, coord &crd)
{
  dm_basex = crd.hval();
  dm_basey = crd.vval();

  dm_dirty = true;

  request_redraw();
}

void canvas_plot_v::handle_zoom(int newfactor)
{
  dm_zoom = newfactor;
  dm_dirty = true;
  dm_dirtyzoom = true;

  request_redraw();
}

void canvas_plot_v::request_plot(void)
{
  dm_dirty = true;
  request_redraw();
}

//
//
// palette_plot_v
//
//

static scopira::core::register_view<palette_plot_v> r2344("scopira::uikit::palette_plot_v", "scopira::uikit::palette_m", "View palette");

palette_plot_v::palette_plot_v(void)
  : dm_model(this)
{
  set_view_title("Plot Palette");
}

void palette_plot_v::handle_canvas_plot(scopira::coreui::pixmap_canvas &v, int basex, int basey, int totalw, int totalh)
{
  if (dm_model.is_null())
    return;

  narray<int> &pal = *dm_model->pm_array;

  if (pal.empty())
    return;

  pixmap_context pen(v);
  int x, idx, lastidx;

  lastidx = -1;
  for (x=0; x<v.width(); ++x) {
    idx = (pal.size()-1) *(basex+x) / totalw;

    if (idx != lastidx) {
      lastidx = idx;
      pen.set_foreground(pal[idx]);
    }

    v.draw_line(pen, x, 0, x, v.height()-1);
  }
}

void palette_plot_v::bind_model(scopira::core::model_i *sus)
{
  dm_model = dynamic_cast<scopira::uikit::palette_m*>(sus);

}

void palette_plot_v::react_model_update(scopira::core::model_i *sus, scopira::core::view_i *src)
{
  request_plot();
}

//
//
// image_plot_v
//
//

static scopira::core::register_view<image_plot_v> r2346("scopira::uikit::image_plot_v", "scopira::uikit::narray_m<int,2>", "View image");

image_plot_v::image_plot_v(void)
  : dm_model(this)
{
  set_view_title("Image View");
}

void image_plot_v::handle_canvas_plot(scopira::coreui::pixmap_canvas &v, int basex, int basey, int totalw, int totalh)
{
  if (dm_model.is_null())
    return;

  narray<int,2> &img = *dm_model->pm_array;

  if (img.empty())
    return;

  pixmap_context pen(v);
  pen.set_foreground(0xFFFFFF);
  v.clear(pen);

  int srcx, srcy, srcw, srch, j;

  srcx = dm_img.width()*basex/totalw;
  srcy = dm_img.height()*basey/totalh;
  srcw = dm_img.width()*v.width()/totalw;
  srch = dm_img.height()*v.height()/totalh;

  // check for over shirnking
  j = srcx + srcw - dm_img.width();
  if (j>0)
    srcw -= j;
  j = srcy + srch - dm_img.height();
  if (j>0)
    srch -= j;

  v.draw_scaled_image(pen, srcx, srcy, srcw, srch,
    dm_img,
    0, 0, v.width(), v.height());
}

void image_plot_v::bind_model(scopira::core::model_i *sus)
{
  dm_model = dynamic_cast<scopira::uikit::narray_m<int,2> *>(sus);

  narray<int,2> &img = *dm_model->pm_array;

  // load up the new image
  dm_img.resize(img.width(), img.height());

  for (int y=0; y<img.height(); ++y)
    for (int x=0; x<img.width(); ++x)
      dm_img(x,y) = rgb_image::color(img(x, y));
}

void image_plot_v::react_model_update(scopira::core::model_i *sus, scopira::core::view_i *src)
{
  request_plot();
}

