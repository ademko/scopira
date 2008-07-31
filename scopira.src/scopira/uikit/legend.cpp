
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

#include <scopira/uikit/legend.h>

#include <scopira/tool/stringflow.h>
#include <scopira/basekit/color.h>

//BBtargets libscopiraui.so

using namespace scopira::basekit;
using namespace scopira::coreui;
using namespace scopira::uikit;

/**
 * legend
 * The legend within the contour plot
 */

// constructor
legend::legend(void)
{
  init_gui(100, 40);

  dm_min = dm_max = 0;

  dm_num_dividers = 10;   // to keep with original hard-coded value
  dm_show_values = true;  // to keep with original default behaviour
}

// set the palette
void legend::set_palette(narray_o<int,1> *pal)
{
  dm_pal = pal;
}

// set the min and the max
void legend::set_range(double min, double max)
{
  dm_min = min;
  dm_max = max;
}

void legend::handle_repaint(scopira::coreui::widget_canvas &v)
{
  int marker, pal_marker;
  double num_marker;
//  int divider = 10;
  font afont;
  widget_context my_color(v);

  v.clear(v.background_context());

  // get the palette
  if (dm_pal == 0) {
    dm_pal = new narray_o<int,1>(1024);
    make_gray_palette(dm_pal->all_slice());
  }
  if (dm_pal->size() < dm_num_dividers)
    dm_num_dividers = dm_pal->size();
  pal_marker = dm_pal->size() / dm_num_dividers;

  // divide up the space
  marker = v.width() / dm_num_dividers;
  num_marker = (dm_max - dm_min) / dm_num_dividers;

  // draw the legend
  for (int i=0; i<dm_num_dividers; i++) {
    my_color.set_foreground(dm_pal->get(i*pal_marker));
    v.draw_rectangle(my_color, true, 5+i*marker, 0, marker, 20);
    if ( dm_show_values ) 
      v.draw_text(v.black_context(), afont, 5+i*marker, 25, scopira::tool::double_to_string(i*num_marker, 2));
  }
}

