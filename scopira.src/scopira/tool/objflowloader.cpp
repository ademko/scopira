
/*
 *  Copyright (c) 2002-2004    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#include <scopira/tool/objflowloader.h>

#include <assert.h>

#include <map>

#include <scopira/tool/output.h>

//BBtargets libscopira.so

using namespace std;
using namespace scopira::tool;

objflowloader* objflowloader::dm_loader;

class objflowloader::impl_t : public scopira::tool::object
{
  public:

  /**
   * we need a tiny wrapper around type_info for assignment
   * and comparison
   *
   * @author Aleksander Demko
   */
  class type_info_ptr
  {
    public:
      type_info_ptr(void)
        : dm_ptr(0) { }
      type_info_ptr(const type_info_ptr& o)
        : dm_ptr(o.dm_ptr) { }
      type_info_ptr(const std::type_info* tip)
        : dm_ptr(tip) { }

      type_info_ptr& operator=(const type_info_ptr& o)
        { dm_ptr = o.dm_ptr; return *this;}

      bool operator<(const type_info_ptr& rhs) const;

      const type_info* get(void) const
        { return dm_ptr; }

    protected:
      const std::type_info* dm_ptr;
  };

  class iter;

  struct payload_t
  {
    std::string pm_name;
    loadfunc_t pm_loader;
    count_ptr<object> pm_note;

    payload_t(void) : pm_loader(0) { }
    payload_t(const std::string &name, loadfunc_t loader)
      : pm_name(name), pm_loader(loader) { }
  };

  typedef std::map<type_info_ptr, payload_t> typemap_t;

  typedef std::map<std::string, type_info_ptr> namemap_t;

  typemap_t dm_typemap;
  namemap_t dm_namemap;

  /// ctor
  impl_t(void);
};

objflowloader::impl_t::impl_t(void)
  : object(true)
{
}

bool objflowloader::impl_t::type_info_ptr::operator<(const type_info_ptr& rhs) const
{
  if (!dm_ptr)
    return true;
  if (!rhs.dm_ptr)
    return false;   // better than failing

#ifdef _MSC_VER
  return dm_ptr->before(*(rhs.dm_ptr)) != 0;
#else
  return dm_ptr->before(*(rhs.dm_ptr));
#endif
}

namespace scopira
{
  namespace tool
  {
    count_ptr<objflowloader> g_auto_objfloat_ptr;
  }
}

objflowloader* objflowloader::instance(void)
{
  if (!dm_loader)
    g_auto_objfloat_ptr = new objflowloader;

  return dm_loader;
}

objflowloader::objflowloader(void)
  : object(true)
{
  //fprintf(stderr, "objflowloader-CTOR\n");
  if (dm_loader == 0) {
    dm_loader = this;
    dm_imp = new impl_t;
  } else
    dm_imp = dm_loader->dm_imp;   // save the existing implementation
                                  // an alternative is to not allow public-ctor and force all craetion via instance?
}
    
objflowloader::~objflowloader()
{
  if (dm_loader == this)
    dm_loader = 0;
  //fprintf(stderr, "objflowloader-dtor\n");
}

/// adds a type to the registry
void objflowloader::add_type(const std::type_info& id, const string& name, loadfunc_t loader)
{
  assert(dm_imp->dm_typemap.count(&id) == 0 && "[This type is already registered]");
  assert(dm_imp->dm_namemap.count(name) == 0 && "[this type string is already registered]");

  dm_imp->dm_typemap[&id] = impl_t::payload_t(name, loader);
  dm_imp->dm_namemap[name] = &id;
}

void objflowloader::add_type(const std::type_info& id, const char * name, loadfunc_t loader)
{
  add_type(id, std::string(name), loader);
}
    
/// gets the name, based on typeid
const string& objflowloader::get_name(const std::type_info &id) const
{
  return dm_imp->dm_typemap[&id].pm_name;
}
    
bool objflowloader::has_typeinfo(const std::type_info &id) const
{
  return dm_imp->dm_typemap.find(&id) != dm_imp->dm_typemap.end();
}

bool objflowloader::has_typeinfo(const std::string &name) const
{
  return dm_imp->dm_namemap.find(name) != dm_imp->dm_namemap.end();
}

/// gets the typeid, based on the name
const std::type_info& objflowloader::get_typeinfo(const string& name) const
{
  return *(*dm_imp->dm_namemap.find(name)).second.get();
}

/// invokes a loader
object* objflowloader::load_object(const std::type_info& id) const
{
  if (dm_imp->dm_typemap.count(&id) == 0)
    return 0;
  else
    return (dm_imp->dm_typemap[&id].pm_loader)();      //invoke the func ptr
}

void objflowloader::set_note(const std::type_info &id, scopira::tool::object *o)
{
  assert(dm_imp->dm_typemap.count(&id) == 1);
  dm_imp->dm_typemap.find(&id)->second.pm_note = o;
}

scopira::tool::object * objflowloader::get_note(const std::type_info &id) const
{
  assert(dm_imp->dm_typemap.count(&id) == 1);
  return dm_imp->dm_typemap.find(&id)->second.pm_note.get();
}

class objflowloader::impl_t::iter : public scopira::tool::iterator_imp_g<object*>
{
  protected:
    typemap_t::iterator dm_next, dm_end;
  public:
    iter(typemap_t::iterator beg, typemap_t::iterator en)
      : dm_next(beg), dm_end(en) { find_next(); }
    virtual object* current(void) { return dm_next->second.pm_note.get(); }
    virtual bool valid(void) const { return dm_next != dm_end; }
    virtual void next(void) { assert(dm_next != dm_end); ++dm_next; find_next(); }
  private:
    void find_next(void);
};

void objflowloader::impl_t::iter::find_next(void)
{
  while (dm_next != dm_end && dm_next->second.pm_note.is_null())
    ++dm_next;
}

objflowloader::note_iterator objflowloader::get_note_iterator(void)
{
  return new impl_t::iter(dm_imp->dm_typemap.begin(), dm_imp->dm_typemap.end());
}

void scopira::tool::flow_reg_imp(const std::type_info& id, const char * name, scopira::tool::objflowloader::loadfunc_t loader)
{
  scopira::tool::objflowloader::instance()->add_type(id, name, loader);
}

