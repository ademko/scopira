
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

#include <scopira/core/register.h>

#include <map>

#include <scopira/tool/array.h>
#include <scopira/tool/output.h>
#include <scopira/core/viewtype.h>

//BBtargets libscopira.so

namespace scopira
{
  namespace core
  {
    class object_note;
    class model_note;
    class view_note;
  }
}

const char * scopira::core::object_type_c = "scopira::tool::object";
const char * scopira::core::model_type_c = "scopira::core::model_i";
const char * scopira::core::project_type_c = "scopira::core::project_i";
const char * scopira::core::view_type_c = "scopira::core::view_i";

/**
 * An internal note object thats tied to object flow loader entries
 * @author Aleksander Demko
 */ 
class scopira::core::object_note : public scopira::tool::object
{
  private:
    std::string dm_name;
    scopira::tool::fixed_array<std::string, 3> dm_parents;
    copyfunc_t dm_copyfunc;
  public:
    // null is ok
    object_note(const std::string &name, const char *paren0);

    // null is ok
    void add_parent(const char *par);

    const std::string & get_name(void) const { return dm_name; }

    const std::string & get_parent(int idx) const { return dm_parents[idx]; }
    int get_parent_size(void) const { return dm_parents.size(); }

    void set_copyfunc(copyfunc_t cf) { dm_copyfunc = cf; }
    copyfunc_t get_copyfunc(void) const { return dm_copyfunc; }
};

class scopira::core::model_note : public scopira::core::object_note
{
  public:
    model_note(const std::string &name, bool linktomodeltype);
};

class scopira::core::view_note : public scopira::core::object_note
{
  private:
    std::string dm_modeltype, dm_menuname;
    int dm_vgmake;
  public:
    view_note(const std::string &name, const char *model_type,
      const char *menu_name, int vgmask);

    const std::string &get_model(void) const { return dm_modeltype; }
    const std::string &get_menuname(void) const { return dm_menuname; }

    int get_vgmask(void) const { return dm_vgmake; }
};

using namespace scopira::tool;
using namespace scopira::core;

object_note::object_note(const std::string &name, const char *parent0)
  : object(true), dm_name(name)
{
  if (parent0)
    dm_parents[0] = parent0;
  dm_copyfunc = 0;
}

void object_note::add_parent(const char *par)
{
  if (!par)
    return;

  for (int x=0; x<dm_parents.size(); ++x)
    if (dm_parents[x].empty()) {
      dm_parents[x] = par;
      return;
    }

  assert(false);    // to many parents already! this can be extended if need be.....
}

model_note::model_note(const std::string &name, bool linktomodeltype)
  : object_note(name, linktomodeltype ? model_type_c : 0)
{
}

view_note::view_note(const std::string &name, const char *model_type,
  const char *menu_name, int vgmask)
  : object_note(name, view_type_c), dm_modeltype(model_type), dm_menuname(menu_name),
    dm_vgmake(vgmask)
{
}

void scopira::core::flow_reg_imp(const std::type_info& id, const char * name, scopira::tool::objflowloader::loadfunc_t loader)
{
  scopira::tool::objflowloader::instance()->add_type(id, name, loader);
}

void scopira::core::link_object_parent_imp(const std::type_info &id, const char *parent_name)
{
  object *o;

  o = objflowloader::instance()->get_note(id);

  if (o) {
    object_note * on = dynamic_cast<object_note*>(o);

    assert(on);

    // add to thte existing note
    on->add_parent(parent_name);
  } else
    // create new note
    objflowloader::instance()->set_note(id, new object_note(objflowloader::instance()->get_name(id), parent_name));
}

void scopira::core::link_object_parent_imp(const std::type_info &id, const char **parents)
{
  assert(parents);
  assert(parents[0]);

  for (int idx=0; parents[idx]; ++idx)
    link_object_parent_imp(id, parents[idx]);
}

void scopira::core::link_object_copyfunc(const std::type_info &id, copyfunc_t cf)
{
  object *o;

  o = objflowloader::instance()->get_note(id);
  assert(o);

  object_note * on = dynamic_cast<object_note*>(o);
  assert(on);

  // add to thte existing note
  on->set_copyfunc(cf);
}

void scopira::core::link_model_imp(const std::type_info &id, bool linktomodeltype)
{
  // for now, we don't allow allow something to be both a model and a view
  // perhaps in the future, we could
  assert(!objflowloader::instance()->get_note(id));   // was already registered?
  objflowloader::instance()->set_note(id, new model_note(objflowloader::instance()->get_name(id), linktomodeltype));
}

void scopira::core::link_view_imp(const std::type_info &id, const char *model_type, const char *menu_name, int vgmask)
{
  // for now, we don't allow allow something to be both a model and a view
  // perhaps in the future, we could
  assert(!objflowloader::instance()->get_note(id));   // was already registered?
  objflowloader::instance()->set_note(id, new view_note(objflowloader::instance()->get_name(id),
    model_type, menu_name, vgmask));
}

void scopira::core::build_objecttype_tree(scopira::tool::count_ptr<scopira::core::objecttype> &out)
{
  typedef std::map<std::string, count_ptr<scopira::core::objecttype> > store_t;
  store_t store;
  iterator_g<object*> ii;
  object_note *on;
  objecttype *ot;
  view_note *vn;
  viewtype *vt;

  out = new objecttype(object_type_c, 0);

  // insert the root
  store[object_type_c] = out;

  // pass one, create all the objectypes
  for (ii=objflowloader::instance()->get_note_iterator(); ii.valid(); ++ii) {
    on = dynamic_cast<object_note*>(*ii);
    if (!on)
      continue;
    vn = dynamic_cast<view_note*>(*ii);
    if (vn)
      store[on->get_name()] = new viewtype(on->get_name(), on->get_copyfunc(), vn->get_menuname(), vn->get_vgmask());
    else
      store[on->get_name()] = new objecttype(on->get_name(), on->get_copyfunc());
  }

  // second pass, connect the parents
  for (ii=objflowloader::instance()->get_note_iterator(); ii.valid(); ++ii) {
    on = dynamic_cast<object_note*>(*ii);
    if (!on)
      continue;

    assert(store.find(on->get_name()) != store.end());
    ot = store.find(on->get_name())->second.get();
    assert(ot->is_alive_object());

    for (int x=0; x<on->get_parent_size(); ++x) {
      const std::string &p = on->get_parent(x);

      if (p.empty())
        continue;

      store_t::iterator fii = store.find(p);
      if (fii != store.end())
        fii->second->add_child(ot);
      else
        OUTPUT << "Parent \"" << p << "\" for type \"" << on->get_name() << "\" note found, skipping\n";
    }//for

    // check if its a view, and if so, bind its model type
    vn = dynamic_cast<view_note*>(*ii);
    if (vn) {
      vt = dynamic_cast<viewtype*>(ot);
      assert(vt);

      store_t::iterator fii = store.find(vn->get_model());
      if (fii != store.end())
        vt->set_modeltype(fii->second.get());
      else
        OUTPUT << "Cannot find model \"" << vn->get_model() << "\" for view type \"" << on->get_name() << "\", skipping\n";
    }//if vt
  }//bigger for

  // final pass - find all the non parented types and hook them up to scopira::tool::object
  store_t::iterator skipper = store.find(object_type_c);
  for (store_t::iterator jj=store.begin(); jj != store.end(); ++jj)
    if (jj != skipper && !jj->second->get_parent_iterator().valid())
      out->add_child(jj->second.get());

  // cleaup pass, remove all the object note objects to save memory
  // i turns out we cant do this, because the objects were (most definatly) created
  // before the objflow counter was created, yet we're deleting them after it
  // was created (so the counter thinks we're deleting objects that dont exist
  for (store_t::iterator jj=store.begin(); jj != store.end(); ++jj)
    if (jj != skipper)
      objflowloader::instance()->set_note(objflowloader::instance()->get_typeinfo(jj->first), 0);
}

