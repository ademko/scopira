
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

#include <scopira/tool/prop.h>

#include <assert.h>
#include <scopira/tool/iterator_imp.h>
#include <scopira/tool/objflowloader.h>

//BBlibs
//BBtargets libscopira.so

using namespace scopira::tool;

static scopira::tool::register_flow<property> r1("scopira::tool::property");
static scopira::tool::register_flow<property_node> r2("scopira::tool::property_node");

/// default construtors
property::property(void)
{
}

property::property(const std::string &_val)
  : dm_val(_val)
{
}

property::property(object *o)
  : dm_obj(o)
{
}

/// destructor
property::~property()
{
}
 
oflow_i& property::print(oflow_i& o) const
{
  return o << '\"' << dm_val << '\"';
}


bool property::load(iobjflow_i& in)
{
  return in.read_string(dm_val) && in.read_object_type(dm_obj);
}

void property::save(oobjflow_i& out) const
{
  out.write_string(dm_val);
  out.write_object_type(dm_obj);
}

property::iterator property::get(const std::string& name)
{
  return iterator( new tool::null_iterator_imp_g< iterator::data_type > );
}

property::key_iterator property::get_key_iterator(void)
{
  return key_iterator( new tool::null_iterator_imp_g< key_iterator::data_type > );
}

void property::set_value(const std::string& val)
{
  dm_val = val;
}


/// default constructor
property_node::property_node(void)
  : property("record")
{
}

/// destructor
property_node::~property_node()
{
}
 
oflow_i & property_node::print(oflow_i &o) const
{
  propmap_t::const_iterator ii;
  sproplist_t::const_iterator jj, endjj;

  o << '{';
  for (ii=dm_propmap.begin(); ii != dm_propmap.end(); ++ii) {
    o << (*ii).first << '=';
    endjj = (*dm_propmap.find( (*ii).first )).second.end();
    for (jj = (*dm_propmap.find( (*ii).first )).second.begin(); jj != endjj; ++jj)
      o << *((*jj).get());
    o << ';';
  }
  o << '}';
  //property::print(o);
  //dm_prop.print(o);

  return o;
}

bool property_node::load(iobjflow_i &in)
{
  int i, sz, j, subsz;
  std::string nam;
  object *o;
  sproplist_t *ls;

  dm_propmap.clear();

  if (!property::load(in))
    return false;

  if (!in.read_int(sz))
    return false;
  assert(sz>=0);

  for (i=0; i<sz && in.read_string(nam); ++i) {
    if (!in.read_int(subsz))
      return false;
    assert(subsz>=0);
    ls = & dm_propmap[nam];
    for (j=0; j<subsz && in.read_object(o); ++j) {
      assert(dynamic_cast<property*>(o));
      ls->push_back( dynamic_cast<property*>(o) );
    }
  }
  return true;
}

void property_node::save(oobjflow_i &out) const
{
  property::save(out);
  propmap_t::const_iterator ii, endii;
  sproplist_t::const_iterator jj, endjj;

  out.write_int( static_cast<int>(dm_propmap.size()) );   // rather than write_size_t... dont want to break any files

  endii = dm_propmap.end();
  for (ii=dm_propmap.begin(); ii != endii; ++ii) {
    out.write_string( (*ii).first );
    out.write_int( static_cast<int>((*ii).second.size()) );   // rather than write_size_t... dont want to break any files
    endjj = (*ii).second.end();
    for (jj=(*ii).second.begin(); jj!=endjj; ++jj)
      out.write_object_type(*jj);
  }
}


void property_node::add_end(const std::string& name, property* prop)
{
  dm_propmap[name].push_back(prop);
}
 
namespace { class prop_def {
  public: 
    property * operator()(property_node::sproplist_t::value_type &v) {
      return v.get();
    }
}; }//namespace

 
property::iterator property_node::get(const std::string& name)
{
  propmap_t::iterator ii = dm_propmap.find(name);

  if (ii == dm_propmap.end())
    return iterator( new tool::null_iterator_imp_g< iterator::data_type > );
  else
    return iterator(
      new tool::stl_iterator_imp_g< sproplist_t::iterator, iterator::data_type,
        prop_def > ((*ii).second.begin(), (*ii).second.end()) );
}

 
property* property_node::get_first(const std::string& name)
{
  propmap_t::iterator ii = dm_propmap.find(name);

  if (ii == dm_propmap.end() || (*ii).second.empty())
    return 0;

  return (*(*ii).second.begin()).get();
}

size_t property_node::get_size(const std::string &name) const
{
  propmap_t::const_iterator ii = dm_propmap.find(name);

  if (ii == dm_propmap.end() || (*ii).second.empty())
    return 0;

  return (*ii).second.size();
}

bool property_node::contains(const std::string& name) const
{
  propmap_t::const_iterator ii = dm_propmap.find(name);
  if (ii == dm_propmap.end())
    return 0;
  else
    //return dm_propmap[name].size();
    return !((*ii).second.empty());     // maintains const - oh the wizardry
}

void property_node::remove(const std::string& name)
{
  dm_propmap.erase(name);
}
 
namespace { class key_def {
  public: 
    const std::string * operator()(const property_node::propmap_t::value_type &v) {
      return &v.first;
    }
}; }//namespace

property_node::key_iterator property_node::get_key_iterator(void)
{
  return key_iterator(
    new tool::stl_iterator_imp_g< propmap_t::iterator, key_iterator::data_type, key_def>
      (dm_propmap.begin(), dm_propmap.end()) );
}

const std::string * property_node::get_string_value(const std::string &name) const
{
  propmap_t::const_iterator ii = dm_propmap.find(name);

  if (ii == dm_propmap.end() || (*ii).second.empty())
    return 0;

  return & (*(*ii).second.begin())->get_value_as_string();
}

object * property_node::get_object_value(const std::string &name) const
{
  propmap_t::const_iterator ii = dm_propmap.find(name);

  if (ii == dm_propmap.end() || (*ii).second.empty())
    return 0;

  return (*(*ii).second.begin())->get_value_as_object();
}

