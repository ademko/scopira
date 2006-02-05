
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

#include <scopira/lab/window.h>

#include <scopira/tool/thread.h>
#include <scopira/coreui/layout.h>

#include <scopira/tool/output.h>

//BBtargets libscopiraui.so

using namespace scopira::tool;
using namespace scopira::coreui;
using namespace scopira::lab;

//
// lab_window::framer
//

lab_window::framer::framer(lab_window *parent)
  : scopira::coreui::window("", false), dm_parent(parent)
{
};

void lab_window::framer::react_button(scopira::coreui::button *source, int actionid)
{
  assert(is_alive_object());
  assert(dm_parent->is_alive_object());

  if (!dm_parent->on_button(actionid))
    return;
  dm_parent->push_action(actionid);

  if (actionid == action_close_c)
    do_hide();
}

void lab_window::framer::init_gui(scopira::coreui::widget *basewidget, int butflags)
{
  assert(basewidget);
  assert(basewidget->get_widget());

  if (butflags != button_none_c) {
    button_layout *blay = new button_layout(basewidget);
    dm_base = blay;

    blay->add_stock_buttons(butflags, this);

    // call it the parent class to setup the window
    assert(blay->get_widget());
    window::init_gui(blay->get_widget());
  } else {
    // no buttons, just use the widget directly
    dm_base = basewidget;
    window::init_gui(basewidget->get_widget());
  }
}

void lab_window::framer::on_close(void)
{
  dm_parent->push_action(action_close_c);

  window::on_close();
}

void lab_window::framer::on_destroy(void)
{
  {
    locker_ptr<open_area> L(dm_parent->dm_open);

    L->isopen = false;
  }

  window::on_destroy();

  // might end up as a dtor call!
  dm_parent->unreg_window();
}

void lab_window::framer::do_hide(void)
{
  {
    locker_ptr<open_area> L(dm_parent->dm_open);

    L->isopen = false;
  }

  // do the actual hide
  // do this before the unreg, because the unreg may result in a dtor call
  //hide();
  window::on_destroy();

  // might end up as a dtor call!
  dm_parent->unreg_window();
}

//
//
// lab_window
//
//

lab_window::lab_window(void)
  : dm_framer(new framer(this))
{
  dm_open.pm_data.isopen = false;

  event_ptr<action_area> L(dm_action);
  L->action_queue.reserve(32);
}

void lab_window::set_title(const std::string &newtitle)
{
  lab_lock LL;

  dm_framer->set_title(newtitle);
}

void lab_window::show_window(void)
{
  {
    locker_ptr<open_area> L(dm_open);

    if (L->isopen)
      return;

    L->isopen = true;
  }

  reg_window();

  lab_lock LL;
  dm_framer->show_all();
}

void lab_window::hide_window(void)
{
  if (!locker_ptr<open_area>(dm_open)->isopen)
    return;

  lab_lock LL;
  dm_framer->do_hide();
}

bool lab_window::wait_action(int timeout)
{
  event_ptr<action_area> L(dm_action);

  // is there already something in the queue? if so, we don't even need to wait
  if (!L->action_queue.empty())
    return true;

  // lets wait then
  if (timeout>0)
    L.wait(timeout);
  else
    L.wait();

  return !L->action_queue.empty();
}

bool lab_window::is_visible(void) const
{
  return locker_ptr<open_area>(dm_open)->isopen;
}

int lab_window::pop_action(void) const
{
  event_ptr<action_area> L(dm_action);

  if (L->action_queue.empty())
    return action_none_c;

  int ret = L->action_queue.front();
  L->action_queue.pop_front();

  return ret;
}

void lab_window::init_gui(scopira::coreui::widget *basewidget, int butflags)
{
  assert(lab_lock::is_locked());
  dm_framer->init_gui(basewidget, butflags);
}

void lab_window::push_action(int actionID)
{
  {
    event_ptr<action_area> L(dm_action);

    L->action_queue.push_back(actionID);

    // notify any listeners
    L.notify();
  }
  lab_window_i::push_action();
}

bool lab_window::on_button(int actionID)
{
  return true;
}

//
//
// error_window
//
//

error_window::error_window(void)
{
  dm_inerror = false;
}

bool error_window::failed(void) const
{
  lab_lock LL;

  return dm_inerror;
}

void error_window::init_gui(scopira::coreui::widget *w, bool okbutton)
{
  dm_vbox = new box_layout(false, false);

  assert(w);
  dm_vbox->add_widget(w, true, true);

  lab_window::init_gui(dm_vbox.get(), okbutton? (button_ok_c|button_cancel_c) : (button_apply_c|button_close_c));
}

void error_window::on_apply(void)
{
  // default does nothing
}

void error_window::clear_error_label(void)
{
  dm_inerror = false;

  if (dm_errorlabel.get() && dm_errorlabel->is_shown()) {
    dm_errorlabel->set_label("");
    dm_errorlabel->hide();
  }
}

void error_window::set_error_label(const std::string &msg)
{
  dm_inerror = true;

  if (dm_errorlabel.get()) {
    dm_errorlabel->set_label(msg);
    dm_errorlabel->show_all();
  } else {
    dm_errorlabel = new scopira::coreui::label(msg);
    assert(dm_errorlabel.get());
    dm_errorlabel->show_all();

    dm_vbox->add_widget(dm_errorlabel.get(), false, false);
  }
}

bool error_window::on_button(int actionID)
{
  assert(lab_lock::is_locked());
  if (actionID == action_apply_c) {
    on_apply();

    // if we have an error, HALT processing
    return !dm_inerror;
  }

  return true;
}

