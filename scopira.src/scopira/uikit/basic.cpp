
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

#include <scopira/uikit/basic.h>

#include <scopira/tool/archiveflow.h>
#include <scopira/core/register.h>
#include <scopira/core/project.h>
#include <scopira/core/loop.h>
#include <scopira/core/objecttype.h>
#include <scopira/coreui/label.h>
#include <scopira/coreui/layout.h>

//BBtargets libscopiraui.so

using namespace scopira::tool;
using namespace scopira::core;
using namespace scopira::coreui;
using namespace scopira::uikit;

//
//
// register
//
//

static scopira::core::register_view<rename_v> r1("scopira::uikit::rename_v", model_type_c, "(rename)", scopira::core::windowonly_uimode_c);
static scopira::core::register_view<remove_v> r2("scopira::uikit::remove_v", model_type_c, "(remove from project)",
  scopira::core::windowonly_uimode_c | scopira::core::vg_needs_project_c);
static scopira::core::register_view<copy_v> r3("scopira::uikit::copy_v", model_type_c, "(copy)",
  scopira::core::no_uimode_c | scopira::core::vg_needs_project_c | scopira::core::vg_needs_copyfunc_c);
static scopira::core::register_view<save_to_file_v> r4("scopira::uikit::save_to_file_v", model_type_c, "(save object to file)", scopira::core::windowonly_uimode_c);
//static scopira::core::register_view<insert_from_file_v> r5("scopira::uikit::insert_from_file_v", model_type_c, "(insert object from file)", scopira::core::windowonly_uimode_c | scopira::core::vg_needs_project_c);
static scopira::core::register_view<insert_from_file_v> r5("scopira::uikit::insert_from_file_v", project_type_c, "Basic/Insert object from file", scopira::core::windowonly_uimode_c);

//
//
// rename_v
//
//

rename_v::rename_v(void)
  : dm_model(this)
{
  init_gui();
}

void rename_v::bind_model(scopira::core::model_i *sus)
{
  dm_model = sus;

  set_view_title("Rename: " + dm_model->get_title());
}

void rename_v::react_model_update(scopira::core::model_i *sus, scopira::core::view_i *src)
{
  if (dm_model.get())
    dm_entry->set_text(dm_model->get_title());
}

void rename_v::react_button(scopira::coreui::button *source, int actionid)
{
  if (actionid == action_apply_c) {
    // do the rename
    dm_model->set_title(dm_entry->get_text());
    dm_model->notify_views(this);
  }

  viewwidget::react_button(source, actionid);
}

void rename_v::init_gui(void)
{
  count_ptr<box_layout> b = new box_layout(true, false);

  dm_entry = new entry;

  b->add_widget(new label("New name:"), false, false);
  b->add_widget(dm_entry.get(), true, true);

  viewwidget::init_gui(b.get(), button_ok_c|button_cancel_c);
}

//
//
// remove_v
//
//

remove_v::remove_v(void)
  : dm_model(this)
{
  viewwidget::init_gui(new label("Are you sure you want to remove\nthis model from the project?"), button_ok_c|button_cancel_c);
}

void remove_v::bind_model(scopira::core::model_i *sus)
{
  dm_model = sus;

  set_view_title("Remove: " + dm_model->get_title());
}

void remove_v::react_button(scopira::coreui::button *source, int actionid)
{
  if (actionid == action_apply_c) {
    project_i *prj;
    model_i *parent;

    prj = dm_model->get_project();

    if (!prj)
      return;

    parent = prj->get_parent(dm_model.get());   // parent might be null, and thats ok
    
    // do the remove
    prj->remove_model(parent, dm_model.get());
    prj->notify_views(this);
  }

  viewwidget::react_button(source, actionid);
}

//
//
// copy_v
//
//

void copy_v::bind_model(scopira::core::model_i *sus)
{
  // perhaps add more nicer error checking throughout this method
  assert(sus);
  assert(sus->get_project());   // perhaps quit gracefully if this fails?  // or make a new flag so this never happens? ("things that have projects")

  count_ptr<model_i> newcp;
  std::string name;
  objecttype *ot;

  assert(objflowloader::instance()->has_typeinfo(typeid(*sus)));
  name = objflowloader::instance()->get_name(typeid(*sus));

  ot = basic_loop::instance()->get_root_objecttype()->find(name);
  assert(ot);
  assert(ot->has_copyfunc() && "[This type doesnt have a registered copyctor]");

  // finally, perform the copy
  newcp = ot->load_copy<model_i>(sus);
  assert(newcp.get());

  newcp->set_title(sus->get_title() + " (copy)");

  // finally, insert it into the project
  sus->get_project()->add_model(sus->get_project()->get_parent(sus), newcp.get());

  // perhaps in the future, do a full recursive copy?
  // (but then should we do run time checking for copyable_model_is?) eithervia:
  //  - dynamic_cast
  //  - objflowloader, then check the tree?
  sus->get_project()->notify_views(this);
}

//
//
// save_to_file_v
//
//

save_to_file_v::save_to_file_v(void)
  : dm_model(this)
{
  dm_filename = new fileentry;

  dm_filename->set_filename("data.sobj");

  viewwidget::init_gui(dm_filename.get(), button_ok_c|button_cancel_c);
}

void save_to_file_v::bind_model(scopira::core::model_i *sus)
{
  dm_model = sus;

  set_view_title("Save: " + dm_model->get_title());
}

void save_to_file_v::react_button(scopira::coreui::button *source, int actionid)
{
  if (actionid == action_apply_c) {
    assert(dm_model.get());

    archiveoflow out;

    out.open(dm_filename->get_filename(), "SCOPIRA_OBJECT_100");

    if (out.failed())
      return;

    out.write_object(dm_model.get());
  }

  viewwidget::react_button(source, actionid);
}

//
//
// insert_from_file_v
//
//

insert_from_file_v::insert_from_file_v(void)
  : dm_model(this)
{
  dm_filename = new fileentry;

  dm_filename->set_filename("data.sobj");

  viewwidget::init_gui(dm_filename.get(), button_ok_c|button_cancel_c);
  set_view_title("Insert Object from a File");
}

void insert_from_file_v::bind_model(scopira::core::model_i *sus)
{
  //dm_model = sus;
  dm_model = dynamic_cast<scopira::core::project_i*>(sus);
}

void insert_from_file_v::react_button(scopira::coreui::button *source, int actionid)
{
  if (actionid == action_apply_c) {
    assert(dm_model.get());

    archiveiflow inf;
    std::string tag;

    tag = inf.open(dm_filename->get_filename(), "SCOPIRA_OBJECT_100");

    if (tag != "SCOPIRA_OBJECT_100")
      return;

    /*count_ptr<project_i> prj;

    prj = dm_model->get_project();
    assert(prj.get());*/

    count_ptr<model_i> m;
    if (!inf.read_object_type(m))
      return;

    /*prj->add_model(dm_model.get(), m.get());
    prj->notify_views(this);*/
    dm_model->add_model(0, m.get());
    dm_model->notify_views(this);
  }

  viewwidget::react_button(source, actionid);
}

