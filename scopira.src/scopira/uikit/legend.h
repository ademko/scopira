
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

#ifndef __INCLUDED_SCOPIRA_UIKIT_LEGEND_H__
#define __INCLUDED_SCOPIRA_UIKIT_LEGEND_H__

#include <scopira/basekit/narray.h>
#include <scopira/coreui/canvas.h>
#include <scopira/coreui/export.h>

namespace scopira
{
  namespace uikit
  {
    class legend;
  }
}

/**
 * Legend for colours within the contour plot
 *
 * @author Aleksander Demko
 * @author Shantha Ramachandran
 * @author Rodrigo Vivanco
 */
class scopira::uikit::legend : public scopira::coreui::canvas_base
{
  protected:
    scopira::tool::count_ptr<scopira::basekit::narray_o<int,1> > dm_pal;  // the palette
    double dm_min, dm_max;  // min and max values of the data
    bool  dm_show_values;
    int   dm_num_dividers;

  public:
    SCOPIRAUI_EXPORT legend(void);

    /// sets the palette
    /// if you dont supply
    SCOPIRAUI_EXPORT void set_palette(scopira::basekit::narray_o<int,1> *pal);
    // sets the min and max values of the legend
    // request a redraw after, eh
    SCOPIRAUI_EXPORT void set_range(double min, double max);

    void set_show_values( bool flg ) {
      dm_show_values = flg;
    }

    void set_num_dividers( int num ) {
      dm_num_dividers = num;
    }

    SCOPIRAUI_EXPORT virtual void handle_repaint(scopira::coreui::widget_canvas &v);
};

#endif

