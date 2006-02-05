
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

#include <scopira/core/model.h>

#include <algorithm>

#include <scopira/tool/flow.h>
#include <scopira/core/view.h>
#include <scopira/core/project.h>
#include <scopira/core/loop.h>
#include <scopira/core/register.h>

//BBtargets libscopira.so

using namespace scopira::tool;
using namespace scopira::core;

//
//
// model_i
//
//

model_i::model_i(void)
  : dm_title("Untitled"), dm_owner(0)
{
}

model_i::model_i(const std::string &title)
  : dm_title(title), dm_owner(0)
{
}

model_i::model_i(const model_i &src)
  : dm_title(src.dm_title), dm_owner(0)
{
}

model_i::~model_i()
{
  for (archive_cache_t::iterator ii = dm_archive_cache.begin(); ii != dm_archive_cache.end(); ++ii)
    basic_loop::instance()->save_object(ii->first, ii->second.get());
}

bool model_i::load(iobjflow_i& in)
{
  dm_archive_cache.clear();
  //FIXME add version stuff
  return in.read_string(dm_title);
}

void model_i::save(oobjflow_i& out) const
{
  //FIXME add version stuff
  out.write_string(dm_title);
}

void model_i::set_title(const std::string &newtitle)
{
  dm_title = newtitle;

  rename_list::iterator ii, endii;

  endii = dm_rename_reactors.end();
  for (ii=dm_rename_reactors.begin(); ii != endii; ++ii) {
    assert((*ii)->is_alive_object());
    (*ii)->react_rename(this, dm_title);
  }
}

void model_i::add_view(view_i *ins) const
{
  assert(ins->is_alive_object());
  dm_views.push_back(ins);
}

void model_i::remove_view(view_i *ins) const
{
  view_list::iterator ii;

  assert(ins->is_alive_object());

  ii = std::find(dm_views.begin(), dm_views.end(), ins);
  assert(ii != dm_views.end());
  dm_views.erase(ii);
}

void model_i::notify_views(view_i *src)
{
  view_list::iterator ii, endii;

  endii = dm_views.end();
  for (ii=dm_views.begin(); ii != endii; ++ii)
    if ((*ii) != src) {
      assert((*ii)->is_alive_object());
      (*ii)->react_model_update(this, src);
    }
}

void model_i::add_rename_reactor(rename_reactor_i *r) const
{
  assert(r->is_alive_object());
  dm_rename_reactors.push_back(r);
}

void model_i::remove_rename_reactor(rename_reactor_i *r) const
{
  rename_list::iterator ii;

  assert(r->is_alive_object());

  ii = std::find(dm_rename_reactors.begin(), dm_rename_reactors.end(), r);
  assert(ii != dm_rename_reactors.end());
  dm_rename_reactors.erase(ii);
}

void model_i::set_project(project_i *newowner)
{
  if (newowner != dm_owner)
    dm_owner = newowner;
}

void model_i::set_tagged_model(const std::string &name, model_i *what)
{
  assert(!name.empty());
  assert(what);

  what->set_title(name);

  if (dm_owner) {
    dm_owner->add_model(this, what); // we have a project, lets just save it there
    dm_owner->notify_views(0);
  } else {
    basic_loop::instance()->save_object(name, what); // just comit it to basic_loop storage
    dm_archive_cache[name] = what;
  }
}

bool model_i::get_tagged_model_impl(const std::string &name,
    scopira::tool::count_ptr<scopira::core::model_i> &out)
{
  assert(!name.empty());

  if (dm_owner)
    return dm_owner->get_tagged_model_from_project(this, name, out);
  else {
    count_ptr<object> o;

    if (!basic_loop::instance()->load_object(name, o))
      return false;
    if (o.is_null()) {
      // valid, but null
      out = 0;
      return true;
    }
    out = dynamic_cast<model_i*>(o.get());
    if (out.get())
      dm_archive_cache[name] = out.get();
    return out.get() != 0;  // success if we the cast went through
  }
}

//
//
// register
//
//

static scopira::core::register_object<model_i> f1(model_type_c, 0, 1);

