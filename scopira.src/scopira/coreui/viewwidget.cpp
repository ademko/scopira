
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

#include <scopira/coreui/viewwidget.h>

#include <scopira/coreui/layout.h>
#include <scopira/coreui/viewmenu.h>

//BBtargets libscopiraui.so

using namespace scopira::coreui;


viewwidget::viewwidget(void)
{
}

void viewwidget::react_button(scopira::coreui::button *source, int actionid)
{
  react_view_action(actionid);
}

button_layout * viewwidget::init_gui(widget *basewidget, int buttonmask)
{
  button_layout * ret;

  assert(dm_viewwidget.is_null());
  assert(basewidget->is_alive_object());

  ret = new button_layout(basewidget);
  dm_viewwidget = ret;
  dm_widget = ret->get_widget();

  ret->add_stock_buttons(buttonmask, this);

  widget::init_gui();

  return ret;
}

void viewwidget::add_viewbutton(const std::string &title, scopira::core::model_i * const * daptr)
{
  button_layout * ret;

  assert(dm_viewwidget.get());
  ret = dynamic_cast<button_layout*>(dm_viewwidget.get());
  assert(ret);

  assert(daptr);

  view_button *vb = new view_button(title);
  vb->bind_model_ptr(daptr);
  ret->add_button(vb);
}

void viewwidget::init_gui(widget *basewidget)
{
  assert(dm_viewwidget.is_null());
  assert(basewidget->is_alive_object());

  // simple pass through and hold
  dm_viewwidget = basewidget;
  dm_widget = basewidget->get_widget();

  widget::init_gui();
}

