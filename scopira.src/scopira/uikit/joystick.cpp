
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

#include <scopira/uikit/joystick.h>

#include <scopira/coreui/layout.h>

//BBtargets libscopiraui.so

using namespace scopira::tool;
using namespace scopira::coreui;
using namespace scopira::uikit;

//
//
// joystick
//
//

joystick::joystick(void)
{
  count_ptr<grid_layout> grid = new grid_layout(3, 3, true);
  count_ptr<button> but;

  but = new button("NW", 7);
  but->set_button_reactor(this);
  grid->add_widget(but.get(), 0, 0);
  but = new button("N", 8);
  but->set_button_reactor(this);
  grid->add_widget(but.get(), 1, 0);
  but = new button("NE", 9);
  but->set_button_reactor(this);
  grid->add_widget(but.get(), 2, 0);

  but = new button("W", 4);
  but->set_button_reactor(this);
  grid->add_widget(but.get(), 0, 1);

  but = new button("E", 6);
  but->set_button_reactor(this);
  grid->add_widget(but.get(), 2, 1);

  but = new button("SW", 1);
  but->set_button_reactor(this);
  grid->add_widget(but.get(), 0, 2);
  but = new button("S", 2);
  but->set_button_reactor(this);
  grid->add_widget(but.get(), 1, 2);
  but = new button("SE", 3);
  but->set_button_reactor(this);
  grid->add_widget(but.get(), 2, 2);

  dm_basewidget = grid.get();
  widget::init_gui(dm_basewidget->get_widget());
}

void joystick::react_button(scopira::coreui::button *source, int childid)
{
  short dx, dy;

  // cound do some % or / magic, whatever...
  switch (childid) {
    case 1: dx = -1; dy = 1; break;
    case 2: dx = 0; dy = 1; break;
    case 3: dx = 1; dy = 1; break;
    case 4: dx = -1; dy = 0; break;
    case 6: dx = 1; dy = 0; break;
    case 7: dx = -1; dy = -1; break;
    case 8: dx = 0; dy = -1; break;
    case 9: dx = 1; dy = -1; break;
    default: dx = 0; dy = 0; childid = 5;
  }

  if (dm_joystick_reactor)
    dm_joystick_reactor->react_joystick(this, dx, dy, childid);
}

