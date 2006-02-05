
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

#include <scopira/lab/view.h>

#include <scopira/core/viewtype.h>
#include <scopira/core/register.h>
#include <scopira/coreui/label.h>

//BBtargets libscopiraui.so

using namespace scopira::tool;
using namespace scopira::core;
using namespace scopira::coreui;
using namespace scopira::lab;

view_window::view_window(scopira::core::model_i *m, const std::string &vtname)
  : dm_model(m)
{
  assert(m);

  lab_lock LK;

  // lots of stuff lifted from viewmenu.cpp //

  // find the type
  objecttype *view_meta, *subtype;
  viewtype *vt;

  view_meta = basic_loop::instance()->get_root_objecttype()->find(view_type_c);
  assert(view_meta);  // serious problem, view meta type is not reigstered

  subtype = view_meta->find(vtname);
  if (!subtype) {
    lab_window::init_gui(new label("View type note found: " + vtname), button_close_c);
    return;
  }

  vt = dynamic_cast<viewtype*>(subtype);
  assert(vt);

  count_ptr<object> loaded_oo;
  view_i *v;

  loaded_oo = objflowloader::instance()->load_object(objflowloader::instance()->get_typeinfo(vt->get_name()));
  assert(loaded_oo.get());
  v = dynamic_cast<view_i*>(loaded_oo.get());
  assert(v);

  if (vt->get_vgmask() & no_uimode_c) {
    bind_model_view(m, v);
    lab_window::init_gui(new label("Non-graphical view executed."), button_close_c);
    return;
  }

  // ok, lets run with it
  dm_view = v;
  // finally, make this our core widget
  assert(dynamic_cast<scopira::coreui::widget*>(v));

  // no additional buttons, the view has its own
  lab_window::init_gui(dynamic_cast<scopira::coreui::widget*>(v), button_none_c);
  lab_window::init_title("View");

  // order matters
  // make me the event hanlder for this view
  v->set_view_action_reactor(this);

  bind_model_view(m, v);
}

view_window::~view_window()
{
  dm_view->set_view_action_reactor(this);
}

void view_window::react_view_action(scopira::core::view_i *source, int actionID)
{
  // do the on_button stuff here too? why not
  if (!on_button(actionID))
    return;

  push_action(actionID);

  if (actionID == action_close_c)
    gui_hide();
}

void view_window::react_view_set_title(scopira::core::view_i *source, const std::string &newtitle)
{
  assert(is_alive_object());

  // this only makes sense when displayed
  //if (!is_visible())
    //return;

  assert(lab_lock::is_locked());

  init_title(newtitle);
}

