
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

#ifndef __INCLUDED_SCOPIRA_CORE_OBJECTTYPE_H__
#define __INCLUDED_SCOPIRA_CORE_OBJECTTYPE_H__

#include <vector>

#include <scopira/tool/object.h>
#include <scopira/tool/objflowloader.h>
#include <scopira/tool/iterator.h>
#include <scopira/tool/export.h>
#include <scopira/core/register.h>

namespace scopira
{
  namespace core
  {
    class objecttype;
  }
}

/**
 * A representative of an object type.
 *
 * @author Aleksander Demko
 */
class scopira::core::objecttype : public scopira::tool::object
{
  private:
    typedef std::vector<objecttype*> parents_t;
    typedef std::vector<scopira::tool::count_ptr<objecttype> > childs_t;      // bad english, yeah :)

  public:
    typedef scopira::tool::iterator_g<objecttype*> objecttype_iterator;

  private:
    parents_t dm_parents;
    childs_t dm_childs;   // parents and children
    std::string dm_name;    // name of the type
    copyfunc_t dm_copyfunc;

  public:
    /// ctor
    SCOPIRA_EXPORT objecttype(const std::string &name, copyfunc_t cp);

    /**
     * Gets the name of this type.
     *
     * @author Aleksander Demko
     */
    const std::string &get_name(void) const { return dm_name; }

    /**
     * Does this object have a *registered* copy constructor?
     *
     * @author Aleksander Demko
     */ 
    bool has_copyfunc(void) const { return dm_copyfunc != 0; }

    /**
     * Adds a child data type to this
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT void add_child(objecttype *d);

    /**
     * For iteration over the parents
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT objecttype_iterator get_parent_iterator(void);
    /**
     * For iteration over the children
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT objecttype_iterator get_child_iterator(void);

    /**
     * Searches all the decendants for a type of the given name
     *
     * @param _name the name of the type
     * @return the found objecttype, null if not found
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT objecttype * find(const std::string & _name);

    /**
     * Is this type a basetype (ancestor) for the given type?
     *
     * @param the type to test against
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT bool is_base(const objecttype *d) const;

    /**
     * Calculates the distance (depth) that this type is to the given
     * type. If this class is NOT a base, -1 is returned.
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT int base_depth(const objecttype *d) const;

    /**
     * Used for debugging
     *
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT void print_tree(int indent = 0);

    template <class CT>
      CT * load_copy(const scopira::tool::object *src) const { return dynamic_cast<CT*>(load_copy_impl(src)); }

  protected:
    SCOPIRA_EXPORT virtual void print_tree_this(int indent);

  private:
    SCOPIRA_EXPORT object * load_copy_impl(const scopira::tool::object *srco) const;
};

#endif

