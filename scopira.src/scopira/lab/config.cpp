
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

#include <scopira/lab/config.h>

#include <scopira/tool/util.h>
#include <scopira/coreui/entry.h>
#include <scopira/coreui/spinbutton.h>
#include <scopira/coreui/filewindow.h>
#include <scopira/coreui/layout.h>
#include <scopira/coreui/label.h>
#include <scopira/coreui/checkbutton.h>
#include <scopira/coreui/dropdown.h>
#include <scopira/coreui/radiobutton.h>

//BBtargets libscopiraui.so

using namespace scopira::tool;
using namespace scopira::core;
using namespace scopira::coreui;
using namespace scopira::lab;

//
// config_window::position
//

config_window::position::position(int magic)
{
  pm_x = magic / 1000;
  pm_y = magic % 1000;
  pm_w = 1;
  pm_h = 1;
  pm_expand_h = true;
  pm_expand_w = true;
}

config_window::position::position(short x, short y)
{
  pm_x = x;
  pm_y = y;
  pm_w = 1;
  pm_h = 1;
  pm_expand_h = true;
  pm_expand_w = true;
}

config_window::position::position(short x, short y, short w, short h)
{
  pm_x = x;
  pm_y = y;
  pm_w = w;
  pm_h = h;
  pm_expand_h = true;
  pm_expand_w = true;
}

config_window::position::position(short x, short y, short w, short h, bool expandH, bool expandV)
{
  pm_x = x;
  pm_y = y;
  pm_w = w;
  pm_h = h;
  pm_expand_h = expandH;
  pm_expand_w = expandV;
}

//
//
// config_window
//
//

// internal class for ease of bridging widgets to config
class config_window::config_widget_i
{
  public:
    virtual ~config_widget_i() { }
    virtual void commit_config(bool todisktoo) = 0;
  protected:
    // utlity
    void set_config(const std::string &key, const std::string &value, bool todisktoo) {
      if (todisktoo)
        basic_loop::instance()->set_config_save(key, value);
      else
        basic_loop::instance()->set_config(key, value);
    }
};

config_window::config_window(void)
{
  lab_lock LL;

  dm_cw.reserve(32);

  dm_boxer = 0;
  init_gui(dm_tabber = new tab_layout, button_save_c|button_ok_c|button_cancel_c);
  init_title("Configuration Settings");
}

config_window::config_window(short w, short h)
{
  lab_lock LL;

  dm_cw.reserve(32);

  dm_tabber = 0;

  init_gui(dm_boxer = new grid_layout(w, h), button_save_c|button_ok_c|button_cancel_c);
  init_title("Configuration Settings");
}

void config_window::add_tab(const std::string &label, short w, short h)
{
  lab_lock LL;

  assert(dm_tabber);

  dm_tabber->add_widget(dm_boxer = new grid_layout(w, h), label);
}

void config_window::add_separator(position pos, bool horiz)
{
  lab_lock LL;

  assert(dm_boxer);
  dm_boxer->add_widget(new widget(horiz ? gtk_hseparator_new() : gtk_vseparator_new()),
      pos.pm_x, pos.pm_y, pos.pm_w, pos.pm_h, pos.pm_expand_h, pos.pm_expand_w);
}

void config_window::add_label(position pos, const std::string &label)
{
  lab_lock LL;

  assert(dm_boxer);
  dm_boxer->add_widget(new scopira::coreui::label(label),
      pos.pm_x, pos.pm_y, pos.pm_w, pos.pm_h, pos.pm_expand_h, pos.pm_expand_w);
}

// these can all be inlined as theyre only used once (and the virtual things are unrolled anyways)

namespace { class entry_cw : public scopira::coreui::entry, public config_window::config_widget_i
{
  public:
    entry_cw(const std::string &key)
      : dm_key(key)
      { set_text(basic_loop::instance()->get_config(key)); }
    virtual void commit_config(bool todisktoo)
      { set_config(dm_key, get_text(), todisktoo); }
  private:
    std::string dm_key;
}; }

namespace { class spinbutton_cw : public scopira::coreui::spinbutton, public config_window::config_widget_i
{
  public:
    spinbutton_cw(const std::string &key,
      double min, double max, double step, short numdigits)
      : scopira::coreui::spinbutton(min, max, step, numdigits), dm_key(key)
      { set_value(string_to_double(basic_loop::instance()->get_config(key))); }
    virtual void commit_config(bool todisktoo)
      { set_config(dm_key, double_to_string(get_value()), todisktoo); }
  private:
    std::string dm_key;
}; }

namespace { class filename_cw : public scopira::coreui::fileentry, public config_window::config_widget_i
{
  public:
    filename_cw(const std::string &key)
      : dm_key(key)
      { set_filename(basic_loop::instance()->get_config(key)); }
    virtual void commit_config(bool todisktoo)
      { set_config(dm_key, get_filename(), todisktoo); }
  private:
    std::string dm_key;
}; }

namespace { class check_cw : public scopira::coreui::checkbutton, public config_window::config_widget_i
{
  public:
    check_cw(const std::string &key, const std::string &label)
      : checkbutton(label,
        basic_loop::instance()->has_config(key) && basic_loop::instance()->get_config(key) != "0"), dm_key(key)
      { }
    virtual void commit_config(bool todisktoo)
      { set_config(dm_key, is_checked() ? "1" : "0", todisktoo); }
  private:
    std::string dm_key;
}; }

namespace { class drop_cw : public scopira::coreui::dropdown, public config_window::config_widget_i,
  public config_window::dropdown_add
{
  public:
    drop_cw(const std::string &key)
      : dm_key(key)
      { if (!string_to_int(basic_loop::instance()->get_config(key), dm_def))
          dm_def = 0; }
    virtual void commit_config(bool todisktoo)
      { set_config(dm_key, int_to_string(get_selection()), todisktoo); }
     virtual config_window::dropdown_add * add(int id, const std::string &label)
     {
       lab_lock L;

       add_selection(id, label);

       if (id == dm_def)
         set_selection(id);

       return this;
     }
  private:
    std::string dm_key;
    int dm_def;
}; }

namespace { class radiobutton_cw : public scopira::coreui::radiobutton, public config_window::config_widget_i,
  public config_window::radiobutton_add
{
  public:
    radiobutton_cw(const std::string &key)
      : dm_key(key)
      { if (!string_to_int(basic_loop::instance()->get_config(key), dm_def))
          dm_def = 0; }
    virtual void commit_config(bool todisktoo)
      { set_config(dm_key, int_to_string(get_selection()), todisktoo); }
     virtual config_window::radiobutton_add * add(int id, const std::string &label)
     {
       lab_lock L;

       add_selection(id, label);

       if (id == dm_def)
         set_selection(id);

       return this;
     }
  private:
    std::string dm_key;
    int dm_def;
}; }

void config_window::add_entry(position pos, const std::string &keyname)
{
  lab_lock LL;

  assert(dm_boxer);
  entry_cw *w = new entry_cw(keyname);

  dm_boxer->add_widget(w,
      pos.pm_x, pos.pm_y, pos.pm_w, pos.pm_h, pos.pm_expand_h, pos.pm_expand_w);
  dm_cw.push_back(w);
}

void config_window::add_spinbutton(position pos, const std::string &keyname,
      double min, double max, double step, short numdigits)
{
  lab_lock LL;

  assert(dm_boxer);
  spinbutton_cw *w = new spinbutton_cw(keyname, min, max, step, numdigits);

  dm_boxer->add_widget(w,
      pos.pm_x, pos.pm_y, pos.pm_w, pos.pm_h, pos.pm_expand_h, pos.pm_expand_w);
  dm_cw.push_back(w);
}

void config_window::add_filename(position pos, const std::string &keyname)
{
  lab_lock LL;

  assert(dm_boxer);
  filename_cw *w = new filename_cw(keyname);

  dm_boxer->add_widget(w,
      pos.pm_x, pos.pm_y, pos.pm_w, pos.pm_h, pos.pm_expand_h, pos.pm_expand_w);
  dm_cw.push_back(w);
}

void config_window::add_checkbutton(position pos, const std::string &keyname, const std::string &label)
{
  lab_lock LL;

  assert(dm_boxer);
  check_cw *w = new check_cw(keyname, label);

  dm_boxer->add_widget(w,
      pos.pm_x, pos.pm_y, pos.pm_w, pos.pm_h, pos.pm_expand_h, pos.pm_expand_w);
  dm_cw.push_back(w);
}

config_window::dropdown_add * config_window::add_dropdown(position pos, const std::string &keyname)
{
  lab_lock LL;

  assert(dm_boxer);
  drop_cw *w = new drop_cw(keyname);

  dm_boxer->add_widget(w,
      pos.pm_x, pos.pm_y, pos.pm_w, pos.pm_h, pos.pm_expand_h, pos.pm_expand_w);
  dm_cw.push_back(w);

  return w;
}

config_window::radiobutton_add * config_window::add_radiobutton(position pos, const std::string &keyname)
{
  lab_lock LL;

  assert(dm_boxer);
  radiobutton_cw *w = new radiobutton_cw(keyname);

  dm_boxer->add_widget(w,
      pos.pm_x, pos.pm_y, pos.pm_w, pos.pm_h, pos.pm_expand_h, pos.pm_expand_w);
  dm_cw.push_back(w);

  return w;
}

bool config_window::on_button(int actionID)
{
  if (actionID != action_save_c && actionID != action_apply_c)
    return true;

  bool todisk = actionID == action_save_c;

  for (cwlist_t::iterator ii = dm_cw.begin(); ii != dm_cw.end(); ++ii)
    (*ii)->commit_config(todisk);

  if (todisk)
    basic_loop::instance()->commit_config();

  return true;
}

