
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

#include <scopira/core/project.h>

#include <scopira/tool/flow.h>

#include <scopira/core/register.h>

//BBtargets libscopira.so

using namespace scopira::tool;
using namespace scopira::core;

//
//
// model_i
//
//

project_i::project_i(void)
{
}

project_i::project_i(const std::string &title)
  : model_i(title)
{
}

//
//
// project_i
//
//


project_base::project_base(void)
{
}

project_base::project_base(const std::string &title)
  : project_i(title)
{
}

project_base::~project_base()
{
  clear_project();
}

bool project_base::load(scopira::tool::iobjflow_i& in)
{
  int x, sz;
  count_ptr<model_i> p, r;

  assert(is_alive_object());

  if (!model_i::load(in) || !in.read_int(x))
    return false;

  assert(x == 1);   // serialization version

  if (!in.read_int(sz))
    return false;

  clear_project();

  for (x=0; x<sz; ++x) {
    p = 0; // p may be null even after read
    r = 0;
    if (!in.read_object_type(p) || !in.read_object_type(r))
      return false;
    add_model(p.get(), r.get());
  }

  return true;
}

void project_base::save(scopira::tool::oobjflow_i& out) const
{
  model_set_t::const_iterator ii, endii;

  assert(is_alive_object());
  model_i::save(out);

  // write out version number (for future expansion for properties)
  out.write_int(1);

  out.write_int(static_cast<int>(dm_models.size()));

  endii = dm_models.end();
  for (ii=dm_models.begin(); ii != endii; ++ii) {
    assert(ii->second.get());
    out.write_object(ii->first.get());
    out.write_object(ii->second.get());
  }
}

void project_base::add_model(model_i *parent, model_i *rec)
{
  assert(rec);
  assert(rec->is_alive_object());
  assert(!contains_model(parent, rec));
  assert(!rec->get_project());

  rec->set_project(this);
  dm_models.insert(model_set_t::value_type(parent, rec));
  dm_child[rec] = parent;
}

void project_base::remove_model(model_i *parent, model_i *rec)
{
  assert(rec);
  assert(contains_model(parent, rec));
  assert(rec->get_project());
  assert(rec->get_project() == this);

  rec->set_project(0);

  iterator_pair pp = dm_models.equal_range(parent);
  for (; pp.first != pp.second; ++pp.first)
    if (pp.first->second.get() == rec)
      break;
  assert(pp.first != pp.second);

  assert(pp.first->second->is_alive_object());
  remove_by_range(pp.first, pp.first);
}

bool project_base::contains_model(model_i *parent, model_i *rec)
{
  iterator_pair pp = dm_models.equal_range(parent);
  for (; pp.first != pp.second; ++pp.first)
    if (pp.first->second.get() == rec)
      return true;
  return false;
}

bool project_base::contains_model(model_i *rec)
{
  return dm_child.count(rec) > 0;
}

model_i * project_base::get_parent(model_i *child)
{
  assert(dm_child.find(child) != dm_child.end());

  return dm_child[child];
}

model_i * project_base::find_model(model_i *parent, const std::string &title) const
{
  const_iterator_pair pp = dm_models.equal_range(parent);
  for (; pp.first != pp.second; ++pp.first)
    if (pp.first->second->get_title() == title)
      return pp.first->second.get();
  return 0;
}

class project_base::project_iterator : public scopira::tool::iterator_imp_g<model_i*>
{
  private:
    model_set_t::const_iterator dm_cur, dm_end;

  public:
    project_iterator(model_set_t::const_iterator b, model_set_t::const_iterator e);

    virtual model_i* current(void) { return dm_cur->second.get(); }
    /// is there a next one?
    virtual bool valid(void) const { return dm_cur != dm_end; }
    /// advance to the next item
    virtual void next(void) { assert(dm_cur != dm_end); ++dm_cur; }
};

project_base::project_iterator::project_iterator(model_set_t::const_iterator b, model_set_t::const_iterator e)
  : dm_cur(b), dm_end(e)
{
  // there used to be a find_next like call here
}

model_iterator project_base::get_model_iterator(model_i *parent) const
{
  return new project_iterator(dm_models.lower_bound(parent), dm_models.upper_bound(parent));
}

bool project_base::is_project_empty(void) const
{
  return dm_models.empty();
}

void project_base::clear_project(void)
{
  // do own the current sub models
  model_set_t::iterator ii, endii;

  endii = dm_models.end();
  for (ii = dm_models.begin(); ii != endii; ++ii) {
    assert(ii->second->get_project());
    assert(ii->second->get_project() == this);
    ii->second->set_project(0);
  }

  dm_models.clear();

  dm_child.clear();
}

void project_base::move_project(scopira::core::project_i *rhs)
{
  project_base *real_rhs;

  assert(rhs);

  real_rhs = dynamic_cast<project_base*>(rhs);
  assert(real_rhs);

  dm_models = real_rhs->dm_models;
  dm_child = real_rhs->dm_child;

  rhs->clear_project();

  model_set_t::iterator ii, endii;

  endii = dm_models.end();
  for (ii = dm_models.begin(); ii != endii; ++ii) {
    assert(ii->second->get_project() == 0);
    ii->second->set_project(this);
  }
}

bool project_base::get_tagged_model_from_project(model_i *parent, const std::string &name,
      scopira::tool::count_ptr<scopira::core::model_i> &out)
{
  iterator_pair pp;
  pp = dm_models.equal_range(parent);

  for (; pp.first != pp.second; ++pp.first)
    if (pp.first->second->get_title() == name) {
      // found it!
      out = pp.first->second.get();
      return true;
    }

  // recurse, if possible
  if (parent)
    return project_base::get_tagged_model_from_project(dm_child[parent], name, out);
  else
    return false; // end of recurtion
}

void project_base::remove_by_range(model_set_t::iterator _begin, model_set_t::iterator _last)
{
  model_set_t::iterator cur;
  iterator_pair pp;
  bool alive = true;

  // first pass, nuke all the children
  cur = _begin;
  while (alive) {
    pp = dm_models.equal_range(_begin->second.get());

    // recursive nuke
    if (pp.first != pp.second)
      remove_by_range(pp.first, --pp.second);

    // remote from all the relavent maps
    assert(cur->second->is_alive_object());
    assert(dm_child.find(cur->second.get()) != dm_child.end());
    dm_child.erase(dm_child.find(cur->second.get()));

    if (cur == _last)
      alive = false;    // we just did out last iteration
    else
      ++cur;
  }
  // 2nd pass, nuke itself
  dm_models.erase(_begin, ++_last);
}

//
//
// register
//
//

static scopira::core::register_model<scopira::core::project_i> r1(project_type_c, 0, scopira::core::interface_model_type() );
static scopira::core::register_model<scopira::core::project_base> r2("scopira::core::project_base", project_type_c);

