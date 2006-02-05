
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

#include <scopira/core/objecttype.h>
#include <scopira/core/viewtype.h>

#include <list>

#include <scopira/tool/iterator_imp.h>
#include <scopira/tool/output.h>

//BBtargets libscopira.so

using namespace scopira::tool;
using namespace scopira::core;

objecttype::objecttype(const std::string &name, copyfunc_t cf)
  : dm_name(name), dm_copyfunc(cf)
{
}

void objecttype::add_child(objecttype *d)
{
  assert(d);
  d->dm_parents.push_back(this);
  dm_childs.push_back(d);
}

objecttype::objecttype_iterator objecttype::get_parent_iterator(void)
{
  return objecttype_iterator(
    new stl_iterator_imp_g<parents_t::iterator,
      objecttype*, deref<objecttype*> >
    (
      dm_parents.begin(),
      dm_parents.end()
    ));
}

objecttype::objecttype_iterator objecttype::get_child_iterator(void)
{
  return objecttype_iterator(
    new stl_iterator_imp_g<childs_t::iterator,
      objecttype*, deref_objauto<objecttype*, deref<childs_t::value_type> > >
    (
      dm_childs.begin(),
      dm_childs.end()
    ));
}

objecttype * objecttype::find(const std::string & _name)
{
  if (dm_name == _name)
    return this;    // I'm it!

  objecttype *d;
  childs_t::iterator kids = dm_childs.begin();

  // iterate the call to all the children
  for (; kids != dm_childs.end(); ++kids) {
    d = (*kids)->find(_name);
    if (d)
      return d;
  }

  return 0;   // nothin found, return null
}

bool objecttype::is_base(const objecttype *d) const
{
  typedef std::list<const objecttype*> q_t;
  q_t paren;
  parents_t::const_iterator jj;

  assert(d);

  // quick pre-check
  if (this == d)
    return true;  // we're the same, bail now
  paren.push_back(d);
  do {
    // pop first one
    d = paren.front();
    assert(d);
    paren.pop_front();

    // check it
    if (this == d)
      return true;  // we're the same, bail now

    // take all the parents of d, put it on our queue
    for (jj=d->dm_parents.begin(); jj != d->dm_parents.end(); ++jj)
      paren.push_back(*jj);
  } while (!paren.empty());

  // didnt find it, we've failed
  return false;
}

int objecttype::base_depth(const objecttype *d) const
{
  childs_t::const_iterator ii, endii;
  int ret, cur;

  // quick pre-check
  if (this == d)
    return 0;

  ret = -1;

  // check all my children, and find the smallest, valid distance and add one
  endii = dm_childs.end();
  for (ii = dm_childs.begin(); ii != endii; ++ii) {
    cur = (*ii)->base_depth(d);
    if ((ret == -1) || (cur != -1 && cur<ret))
      ret = cur;
  }

  if (ret == -1)
    return -1;
  else
    return ret + 1;
}

void objecttype::print_tree(int indent)
{
  print_tree_this(indent);

  objecttype_iterator ii = get_child_iterator();

  indent += 2;

  for (; ii.valid(); ++ii)
    (*ii)->print_tree(indent);
}

void objecttype::print_tree_this(int indent)
{
  for (int x=0; x<indent; ++x)
    OUTPUT << ' ';
  OUTPUT << dm_name << '\n';
}

object * objecttype::load_copy_impl(const scopira::tool::object *srco) const
{
  assert(dm_copyfunc);
  assert(srco);
  return dm_copyfunc(srco);
}

viewtype::viewtype(const std::string &name, copyfunc_t cf, const std::string &menuname, int vgmask)
  : objecttype(name, cf), dm_menuname(menuname), dm_modeltype(0), dm_vgmask(vgmask)
{
}

void viewtype::set_modeltype(objecttype *mt)
{
  dm_modeltype = mt;
}

void viewtype::print_tree_this(int indent)
{
  for (int x=0; x<indent; ++x)
    OUTPUT << ' ';
  OUTPUT << get_name() << ' ' << get_vgmask() << "_view_on " << dm_modeltype->get_name() << '\n';
}

