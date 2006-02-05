
/*
 *  Copyright (c) 2002    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#include <scopira/tool/tree.h>

#include <vector>

#include <scopira/tool/iterator_imp.h>
#include <scopira/tool/util.h>

using namespace scopira;

tool::treenode::treenode(scopira::tool::object *o)
  : dm_data(o)
{
}

tool::treenode::~treenode()
{
  assert(is_alive_object());
}
  
void tool::treenode::set(const std::string &name, treenode *nd)
{
  assert(nd);
  dm_nodes[name] = nd;
}

tool::treenode * tool::treenode::get(const std::string &name)
{
  nodemap_t::iterator ii = dm_nodes.find(name);

  if (ii == dm_nodes.end())
    return 0;
  else
    return (*ii).second.get();
}

void tool::treenode::set_path(const std::string &path_name, treenode *nd)
{
  std::vector< std::string > toks;
  treenode *cur, *t;
  int i;

  assert(nd);

  assert(!path_name.empty());
  tool::string_tokenize(path_name, toks, "/");
  assert(toks.size() > 0);

  cur = this;
  for (i=0; (i+1) < toks.size(); ++i) {
    t = cur->get(toks[i]);
    if (!t)
      cur->set(toks[i], t = new treenode);
    cur = t;
  }

  cur->set(toks[toks.size() - 1], nd);
}

// will try using this local-scope object instead of a deref_* stack -
// it might be easier to make these little things that for the developer
// to use the stack
namespace {
class type_deref {
  public:
  const std::string& operator()(std::map<std::string, tool::count_ptr<tool::treenode> >::value_type &p) {
    return p.first;
  }
};//class
}//anonymous namespace

tool::treenode::key_iterator tool::treenode::get_key_iterator(void)
{
  return key_iterator(
    new tool::stl_iterator_imp_g<nodemap_t::iterator,
      const std::string&, type_deref >
    (
      dm_nodes.begin(),
      dm_nodes.end()
    ));
}


//BBlibs
//BBtargets libscopira.so

