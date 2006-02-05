
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

#include <scopira/coreui/viewmenu.h>

#include <algorithm>

#include <scopira/tool/objflowloader.h>
#include <scopira/tool/tree.h>
#include <scopira/core/loop.h>
#include <scopira/core/register.h>
#include <scopira/core/view.h>
#include <scopira/core/viewtype.h>
#include <scopira/coreui/menu.h>
#include <scopira/coreui/viewcontainer.h>
#include <scopira/coreui/image.h>

//BBtargets libscopiraui.so

using namespace scopira::tool;
using namespace scopira::core;
using namespace scopira::coreui;

namespace {
  class vm_treenode : public scopira::tool::treenode
  {
    public:
      viewtype *pm_type;
      bool pm_showdisabled;

    public:
      vm_treenode(viewtype *t, bool showdisabled);
  };
}

vm_treenode::vm_treenode(viewtype *t, bool showdisabled)
  : pm_type(t), pm_showdisabled(showdisabled)
{
}

void scopira::coreui::launch_view(scopira::core::model_i *m, scopira::core::viewtype *vt,
      scopira::core::view_container_reactor_i *reac)
{
  count_ptr<object> loaded_oo;
  view_i *v;

  assert(m);
  assert(vt);

  loaded_oo = objflowloader::instance()->load_object(objflowloader::instance()->get_typeinfo(vt->get_name()));
  assert(loaded_oo.get());
  v = dynamic_cast<view_i*>(loaded_oo.get());
  assert(v);

  // ok, now spawn the viewer, send the viewer to the reactor or just run the non-gui viewer
  if (vt->get_vgmask() & no_uimode_c)
    bind_model_view(m, v);
  else {
    if (!reac || (vt->get_vgmask() & windowonly_uimode_c)) {
      viewwindow *vw = new viewwindow(m, v);
      vw->show_all();
    } else {
      assert(dynamic_cast<widget*>(v));
      dynamic_cast<widget*>(v)->show_all();
      reac->react_view_insert(m, v);
    }
  }
}

bool scopira::coreui::launch_view(scopira::core::model_i *m, const std::string &vt,
      scopira::core::view_container_reactor_i *reac)
{
  objecttype *view_meta, *subtype;
  viewtype *newvt;

  view_meta = basic_loop::instance()->get_root_objecttype()->find(view_type_c);
  assert(view_meta);  // serious problem, view meta type is not reigstered

  subtype = view_meta->find(vt);

  if (!subtype)
    return false;

  newvt = dynamic_cast<viewtype*>(subtype);
  assert(newvt);

  // call the actual spawner
  launch_view(m, newvt, reac);

  return true;
}

view_filter_options::view_filter_options(void)
{
  pm_max_depth = -1;
  pm_filter = 0;
  pm_show_vgmask = vg_show_c;
}

static void h_menu_callback(GtkWidget *wid, gpointer blah)
{
  // respond to a menu event
  assert(blah);
  object *oo;
  count_ptr<object> loaded_oo;
  viewtype *vt;
  model_i *mod;

  oo = reinterpret_cast<object*>(blah);
  assert(oo->is_alive_object());

  vt = dynamic_cast<viewtype*>(oo);
  assert(vt->is_alive_object());

  assert(wid);
  mod = reinterpret_cast<model_i*>(widget::get_attach_ptr(wid));
  assert(mod);
  assert(mod->is_alive_object());

  view_container_reactor_i *spawner = reinterpret_cast<view_container_reactor_i*>(g_object_get_data(G_OBJECT(wid), "sco_reac"));

  launch_view(mod, vt, spawner);
}

static bool menu_sort(const std::string * lhs, const std::string *rhs)
{
  bool mini_lhs, mini_rhs;

  mini_lhs = !(*lhs).empty() && (*lhs)[0] == '(';
  mini_rhs = !(*rhs).empty() && (*rhs)[0] == '(';

  if (mini_lhs == mini_rhs)
    return *lhs < *rhs;
  if (mini_rhs)
    return true;
  else
    return false;
}

static void recursive_menu_build(treenode *node, menu_builder &builder, view_container_reactor_i *reac)
{
  treenode *subnode;
  typedef std::vector<const std::string *> sorted_names_t;
  sorted_names_t sorted_names;

  assert(node);

  sorted_names.reserve(32);

  for (treenode::key_iterator ii = node->get_key_iterator(); ii.valid(); ++ii)
    sorted_names.push_back(&(*ii));
  std::sort(sorted_names.begin(), sorted_names.end(), menu_sort);

  for (sorted_names_t::iterator ii=sorted_names.begin(); ii != sorted_names.end(); ++ii) {
  //for (treenode::key_iterator ii = node->get_key_iterator(); ii.valid(); ++ii) {
    subnode = node->get(*(*ii));
    assert(subnode);
    vm_treenode *vmnode = dynamic_cast<vm_treenode*>(subnode);

    if (vmnode) {
      GtkWidget *w;

      // this is an actual entry, add the item
      w = builder.add_item(*(*ii));
      builder.set_callback(G_CALLBACK(h_menu_callback), static_cast<object*>(vmnode->pm_type));  // need to be ABSOLUTLY sure of the type here
      if (vmnode->pm_showdisabled)
        builder.set_enable(false);
      g_object_set_data(G_OBJECT(w), "sco_reac", reac);
    } else {
      // this is a sub menu, recurse
      builder.push_menu(*(*ii));
      recursive_menu_build(subnode, builder, reac);
      builder.pop_menu();
    }
  }//for
}

void scopira::coreui::build_view_menu(scopira::core::model_i *m, scopira::coreui::menu_builder &mb)
{
  build_view_menu(m, mb, view_filter_options(), 0);
}

void scopira::coreui::build_view_menu(scopira::core::model_i *m, scopira::coreui::menu_builder &mb,
        const view_filter_options &filter, view_container_reactor_i *reac)
{
  typedef std::vector<count_ptr<vm_treenode> > views_t;
  views_t views;
  objecttype *target_meta;
  objecttype *view_meta;

  if (!m) {
    mb.add_item("(no current data)");
    mb.set_enable(false);
    return;
  }

  views.reserve(128);

  // find this models type
  target_meta = basic_loop::instance()->get_root_objecttype()->find(
      objflowloader::instance()->get_name(typeid(*m)));
  assert(target_meta && "[This model is not registered in the object type tree]");

  // PASS 1 - find all the viewtypes that can operate on this model
  view_meta = basic_loop::instance()->get_root_objecttype()->find(view_type_c);
  assert(view_meta);  // serious problem, view meta type is not reigstered

  for (objecttype::objecttype_iterator ii=view_meta->get_child_iterator(); ii.valid(); ++ii) {
    int depth, classification;
    viewtype *vt = dynamic_cast<viewtype*>(*ii);

    depth = vt->get_modeltype()->base_depth(target_meta);

    if (
        depth != -1 &&
        (filter.pm_max_depth == -1 || depth <= filter.pm_max_depth) &&
        ( (filter.pm_show_vgmask & vt->get_vgmask()) == vt->get_vgmask()) &&    // this viewtype must have all the requested vgflags
        ( ((vt->get_vgmask() & vg_needs_copyfunc_c) == 0) || target_meta->has_copyfunc())    // if the vt needs a copyfunc, make sure the model has one
        ) {
      classification = filter.pm_filter ? filter.pm_filter->filter_model_view(m, vt) : view_filter_i::menu_display_c;

      if (classification != view_filter_i::menu_hide_c)
        views.push_back(new vm_treenode(vt, classification == view_filter_i::menu_disable_c));
    }
  }

  if (views.empty()) {
    // nothing? show a msg and die.
    mb.add_item("(no views found)");
    mb.set_enable(false);
    return;
  }

  // PASS 2 - build the menu tree from the list of valid views
  treenode root;

  for (views_t::iterator jj=views.begin(); jj != views.end(); ++jj)
    root.set_path((*jj)->pm_type->get_menuname(), jj->get());

  // PASS 3 - take that node tree, and build the actual menu
  mb.set_attach_ptr(static_cast<model_i*>(m));

  /// recurse!
  recursive_menu_build(&root, mb, reac);
}

int noparen_filter::filter_model_view(scopira::core::model_i *m, scopira::core::viewtype *vt)
{
  assert(vt);
  assert(!vt->get_menuname().empty());
  if (vt->get_menuname()[0] == '(')
    return menu_hide_c;
  else
    return menu_display_c;
}

//
//
// view_button
//
//

view_button::view_button(void)
  : button(new_image_label(new imagewidget(GTK_STOCK_PROPERTIES), "New View..."))
{
  dm_model_ptr = 0;
  set_button_reactor(this);
}

view_button::view_button(const std::string &title)
  : button(new_image_label(new imagewidget(GTK_STOCK_PROPERTIES), title))
{
  dm_model_ptr = 0;
  set_button_reactor(this);
}

view_button::view_button(const std::string &title, const scopira::core::model_ptr_base &mptr)
  : button(new_image_label(new imagewidget(GTK_STOCK_PROPERTIES), title))
{
  dm_model_ptr = 0;
  set_button_reactor(this);

  bind_model_ptr(mptr);
}

void view_button::bind_model(scopira::core::model_i *sus)
{
  dm_model = sus;
}

void view_button::bind_model_ptr(scopira::core::model_i * const *mp)
{
  dm_model_ptr = mp;
}

void view_button::react_button(scopira::coreui::button *source, int actionid)
{
  if (dm_model.is_null() && (!dm_model_ptr || !(*dm_model_ptr)))
    return;

  menu_pop_builder mb;

  build_view_menu(dm_model_ptr ? *dm_model_ptr : dm_model.get(), mb);
  mb.popup();
}

