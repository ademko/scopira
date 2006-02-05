
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

#ifndef __INCLUDED_SCOPIRA_UIKIT_IMAGE_H__
#define __INCLUDED_SCOPIRA_UIKIT_IMAGE_H__

#include <math.h>

#include <limits>

#include <scopira/tool/platform.h>
#include <scopira/core/view.h>
#include <scopira/coreui/canvas.h>
#include <scopira/basekit/narray.h>
#include <scopira/basekit/color.h>

namespace scopira
{
  namespace uikit
  {
    template <bool B> class range_trait { };
    template <> class range_trait<true> { public: typedef int64_t range_type; };
    template <> class range_trait<false> { public: typedef double range_type; };

    template <class T> class value_image_base;
  }
}

/**
 * A base template class for building slice views. You decend from this class
 * and follow the same rules as the init_gui stuff in canvas.
 *
 * Note that this base class doesn't do anything view_i related.
 * Descendants may, though.
 *
 * T is a base type (int, double, etc).
 *
 * @author Aleksander Demko
 */ 
template <class T>
class scopira::uikit::value_image_base : public scopira::coreui::zoomed_scrolled_canvas_base
{
  public:
    typedef T data_type;
    typedef scopira::basekit::nslice<T, 2> slice_type;

  protected:
    /// ctor
    value_image_base(void);

    // init_gui rules are the same as for zoomed_scrolled_canvas_base

    virtual void handle_init(scopira::coreui::widget_canvas &v, coord &crd);
    virtual void handle_resize(scopira::coreui::widget_canvas &v, coord &crd) { dm_dirty = true; }
    virtual void handle_repaint(scopira::coreui::widget_canvas &v, coord &crd);
    virtual void handle_scroll(scopira::coreui::widget_canvas &v, coord &crd);
    virtual void handle_zoom(int newfactor);

    /// call this when you want to change the display slice
    void set_slice(slice_type s) {
      dm_datamatrix = s;
      dm_dirty = true;
    }
    
    void set_range(T _min, T _max) {
      dm_min = _min;
      dm_max = _max;
      dm_dirty = true;
    }

    void set_min_color(int packedcol) {
      dm_min_col = packedcol;
      dm_dirty = true;
    }
    void set_max_color(int packedcol) {
      dm_max_col = packedcol;
      dm_dirty = true;
    }
    void set_palette(scopira::basekit::narray_o<int> *pal) {
      dm_pal = pal;
      dm_dirty = true;
    }

    void set_dirty(void) { dm_dirty = true; }

    /// converts the given screen x y to real x y
    /// @return true if both points where within the plot.
    /// @author Aleksander Demko
    void convert_screento_xy(int screenx, int screeny, double &x, double &y,
      bool *xvalid = 0, bool *yvalid = 0);
    /// converts real xy to a on screen x, y
    /// @param xvalid (optional output).. will be true if the x conversion was valid
    /// @param yvalid (optional output).. will be true if the y conversion was valid
    /// @author Aleksander Demko
    void convert_xy_to_screen(double x, double y, int &screenx, int &screeny,
      bool *xvalid = 0, bool *yvalid = 0);

  protected:
    scopira::coreui::point dm_basept, dm_sizept, dm_datasizept;

  private:
    bool dm_dirty, dm_zoomdirty;
    double dm_dataperscreen;
    scopira::coreui::font dm_font;
    scopira::coreui::rgb_image dm_dataimage;

    scopira::basekit::nslice<T, 2> dm_datamatrix;
    data_type dm_min, dm_max;
    int dm_min_col, dm_max_col;
    int dm_last_hvalue, dm_last_vvalue;

    scopira::tool::count_ptr<scopira::basekit::narray_o<int> > dm_pal;
};

//
// implementation
//

template <class T>
scopira::uikit::value_image_base<T>::value_image_base(void)
  : dm_dirty(true)
{
  dm_zoomdirty = false;
  dm_dataperscreen = 1;
  dm_min = 0;
  dm_max = 0;
  dm_min_col = 0x000000;
  dm_max_col = 0xFFFFFF;
  dm_last_hvalue = 0;
  dm_last_vvalue = 0;
}

template <class T>
void scopira::uikit::value_image_base<T>::handle_init(scopira::coreui::widget_canvas &v, coord &crd)
{
}

template <class T>
void scopira::uikit::value_image_base<T>::handle_repaint(scopira::coreui::widget_canvas &v, coord &crd)
{
  // need to return-setup the draw buffer?
  if (dm_dirty) {
    dm_dirty = false;
    int col, idxlen, idx;
    typename range_trait<std::numeric_limits<T>::is_integer >::range_type len;

    if (dm_pal.is_null()) {
      dm_pal = new scopira::basekit::narray_o<int>(256);
      make_spectra_palette(dm_pal->all_slice());
    }

    if (dm_datamatrix.is_null())
      dm_dataimage.resize(0, 0);
    else {
      dm_dataimage.resize(dm_datamatrix.width(), dm_datamatrix.height());

      if (dm_max<=dm_min)
        dm_max = dm_min + 1;
      len = dm_max - dm_min + 1;
      assert(len>0);

      idxlen = dm_pal->size();   // in the future, we'll use real palettes

      for (int y=0; y<dm_datamatrix.height(); ++y)
        for (int x=0; x<dm_datamatrix.width(); ++x) {
          idx = static_cast<int>((dm_datamatrix(x,y) - dm_min) * idxlen / len);
          if (idx<0)
            col = dm_min_col;
          else if (idx>=idxlen)
            col = dm_max_col;
          else
            col = (*dm_pal)[idx];
          dm_dataimage(x, y).set_by_packed(col);
        }
    }

    dm_zoomdirty = true;
  }//if dm_dirty
  
  if (v.width() < 30) {
    dm_basept.x = 0;
    dm_sizept.x = v.width();
  } else {
    dm_basept.x = 10;
    dm_sizept.x = v.width() - 2*dm_basept.x;
  }
  if (v.height() < 30) {
    dm_basept.y = 0;
    dm_sizept.y = v.height();
  } else {
    dm_basept.y = 10;
    dm_sizept.y = v.height() - 2*dm_basept.y;
  }

  dm_datasizept = dm_sizept; //safe default, i guess

  // see if we need to adjust the scroll bars
  if (dm_zoomdirty && !dm_datamatrix.is_null()) {
    dm_zoomdirty = false;

    crd.set_hbounds(0, dm_datamatrix.width(),
        crd.hvalue(), dm_sizept.x * dm_dataperscreen / 10,
        dm_sizept.x * dm_dataperscreen, dm_sizept.x * dm_dataperscreen);
    crd.set_vbounds(0, dm_datamatrix.height(),
        crd.vvalue(), dm_sizept.y * dm_dataperscreen / 10,
        dm_sizept.y * dm_dataperscreen, dm_sizept.y * dm_dataperscreen);
  }

  // do the actual drawing
  scopira::coreui::widget_context c(v);
  int basew, baseh;

  c.set_foreground(0xFFFFFF);   // white clear()
  v.clear(c);

  dm_last_hvalue = static_cast<int>(crd.hvalue());
  dm_last_vvalue = static_cast<int>(crd.vvalue());

  if (dm_datamatrix.is_null()) {
    v.draw_text(c, dm_font, dm_basept.x, dm_basept.y, "(empty data)");
    return;
  }

  assert(dm_sizept.x>0);
  assert(dm_sizept.y>0);
  assert(dm_dataperscreen>0);
  basew = static_cast<int>(dm_sizept.x * dm_dataperscreen);
  baseh = static_cast<int>(dm_sizept.y * dm_dataperscreen);
  if (dm_last_hvalue > dm_datamatrix.width()) {
    dm_last_hvalue = 0;
    crd.set_hvalue(0);
  }
  if (dm_last_vvalue > dm_datamatrix.height()) {
    dm_last_vvalue = 0;
    crd.set_vvalue(0);
  }
  if (dm_last_hvalue + basew > dm_datamatrix.width())
    basew = dm_datamatrix.width() - dm_last_hvalue;
  if (dm_last_vvalue + baseh > dm_datamatrix.height())
    baseh = dm_datamatrix.height() - dm_last_vvalue;

  dm_datasizept.x = static_cast<int>(basew / dm_dataperscreen);
  dm_datasizept.y = static_cast<int>(baseh / dm_dataperscreen);

  // do the final blit
  v.draw_scaled_image(c, dm_last_hvalue, dm_last_vvalue, basew, baseh, dm_dataimage,
      dm_basept.x, dm_basept.y, dm_datasizept.x, dm_datasizept.y);
}

template <class T>
void scopira::uikit::value_image_base<T>::handle_scroll(scopira::coreui::widget_canvas &v, coord &crd)
{
  handle_repaint(v, crd);
}

template <class T>
void scopira::uikit::value_image_base<T>::handle_zoom(int newfactor)
{
  // calculate new data pix per screen pix
  dm_dataperscreen = ::pow(10.0, static_cast<double>(1000 - newfactor)/500 - 1);
  dm_zoomdirty = true;

  request_redraw();
}

template <class T>
void scopira::uikit::value_image_base<T>::convert_screento_xy(int screenx, int screeny, double &x, double &y,
      bool *xvalid, bool *yvalid)
{
  if (xvalid)
    *xvalid = false;
  if (yvalid)
    *yvalid = false;

  if (screenx >= dm_basept.x && screenx < dm_basept.x+dm_datasizept.x) {
    if (xvalid)
      *xvalid = true;
    x = dm_last_hvalue + (screenx-dm_basept.x) * dm_dataperscreen;
  }
  if (screeny >= dm_basept.y && screeny < dm_basept.y+dm_datasizept.y) {
    if (yvalid)
      *yvalid = true;
    y = dm_last_vvalue + (screeny-dm_basept.y) * dm_dataperscreen;
  }
}

template <class T>
void scopira::uikit::value_image_base<T>::convert_xy_to_screen(double x, double y, int &screenx, int &screeny,
      bool *xvalid, bool *yvalid)
{
  if (xvalid)
    *xvalid = false;
  if (yvalid)
    *yvalid = false;

  if (x>= dm_last_hvalue && x<=dm_last_hvalue + dm_datasizept.x*dm_dataperscreen) {
    if (xvalid)
      *xvalid = true;
    screenx = dm_basept.x + static_cast<int>((x-dm_last_hvalue) / dm_dataperscreen);
  }
  if (y>= dm_last_vvalue && y<=dm_last_vvalue + dm_datasizept.y*dm_dataperscreen) {
    if (yvalid)
      *yvalid = true;
    screeny = dm_basept.y + static_cast<int>((y-dm_last_vvalue) / dm_dataperscreen);
  }
}

#endif

