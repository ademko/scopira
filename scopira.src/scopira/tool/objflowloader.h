
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
 
#ifndef __INCLUDED_SCOPIRA_TOOL_OBJFLOWLOADER_H__
#define __INCLUDED_SCOPIRA_TOOL_OBJFLOWLOADER_H__

#include <typeinfo>
#include <string>

#include <scopira/tool/object.h>
#include <scopira/tool/iterator.h>
#include <scopira/tool/export.h>

namespace scopira
{
  namespace tool
  {
    class objflowloader;

    // forward decls
    class object;

    // utility template function for the static loader functions
    // the add_type methods often need
    template <class T> object* objloadfunc_g(void) { return new T(); }

    template <class T> class register_flow;
  }
}

/**
 * A flow object loader and type registry.
 * 
 * Basically, a registration of an object contains:
 *  it's C++ type_info
 *  a universally unique name (usually the fully qualified C++ name)
 *  the loader function (in some rare cases, this is null)
 *  an optional note object
 *
 * The internal maps names -> type_info -> the full payload.
 *
 * @author Aleksander Demko
 */
class scopira::tool::objflowloader : public scopira::tool::object
{
  public:
    typedef scopira::tool::iterator_g<object*> note_iterator;

  protected:
    class impl_t;
    scopira::tool::count_ptr<impl_t> dm_imp;
  
    static objflowloader* dm_loader;     // the static loader to use  

  public:
    /// a loader function type
    typedef object* (*loadfunc_t)(void);

  public:
    
    /**
     * Gets the current loader. This will never return null (that is, it'll
     * create one if need be).
     *
     * However, this method ends up creating it, it will put it in a reference counted
     * state.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT static objflowloader* instance(void);
  
    /**
     * constructor. the objflowloader constructed will automatically
     * register itself as the global objflowloader
     *
     * it will automaitcally call scopira::tool::register_flowtypes
     * on itself. you should explictly register any other classes
     *
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT objflowloader(void);
    
    /// destructor
    SCOPIRA_EXPORT ~objflowloader();
    
    /// adds a type to the registry. calling this one the same
    /// type is fine, as it simply does replacement
    SCOPIRA_EXPORT void add_type(const std::type_info& id, const std::string& name, loadfunc_t loader);

    /// alternate just for performance reasons
    SCOPIRA_EXPORT void add_type(const std::type_info& id, const char * name, loadfunc_t loader);
    
    /// gets the name, based on typeid
    SCOPIRA_EXPORT const std::string& get_name(const std::type_info &id) const;
    
    /// do i have this type?
    SCOPIRA_EXPORT bool has_typeinfo(const std::type_info &id) const;
    /// do I know about this type?
    SCOPIRA_EXPORT bool has_typeinfo(const std::string &name) const;
    /// gets the typeid, based on the unique name, the name must exist
    SCOPIRA_EXPORT const std::type_info& get_typeinfo(const std::string &name) const;

    /// invokes a loader, null on fail
    SCOPIRA_EXPORT object* load_object(const std::type_info& id) const;

    /**
     * sets the optional annotation object for the given name
     * if the object already has a annotation object, it will be replaced
     * the object will be ref counted.
     *
     * Notes should have used they object(bool = true) ctor (go look it up)
     * as notes may live outside the rec coutning debug system's lifetime.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT void set_note(const std::type_info &id, scopira::tool::object *o);
    /// gets the optional note object for the given name, might return null for none
    SCOPIRA_EXPORT scopira::tool::object * get_note(const std::type_info &id) const;

    /**
     * Get a iterator of all the notes in the system.
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT note_iterator get_note_iterator(void);
};

namespace scopira
{
  namespace tool
  {
    SCOPIRA_EXPORT void flow_reg_imp(const std::type_info& id, const char * name, scopira::tool::objflowloader::loadfunc_t loader);
  }
}

/**
 * A registration object for registering objects just for
 * serialization (or at the least, virtual ctor loading).
 * This is the most basic registration object.
 *
 * This version is a simplified version/carbon copy
 * of the one in core.
 *
 * @author Aleksander Demko
 */
template <class T>
class scopira::tool::register_flow
{
  public:
    /// ctor, yes, just plain char *
    register_flow(const char *fullname) {
      scopira::tool::flow_reg_imp(typeid(T), (fullname), scopira::tool::objloadfunc_g<T>);
    }
    /// version that has a NULL loader
    register_flow(const char *fullname, int dummy) {
      scopira::tool::flow_reg_imp(typeid(T), (fullname), 0);
    }
};

#endif


