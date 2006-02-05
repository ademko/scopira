
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

#ifndef __INCLUDED_SCOPIRA_TOOL_PROP_H__
#define __INCLUDED_SCOPIRA_TOOL_PROP_H__

#include <list>
#include <map>
 
#include <scopira/tool/object.h>
#include <scopira/tool/flow.h>
#include <scopira/tool/iterator.h>

namespace scopira
{
  namespace tool
  {
    class property;
    class property_node;
  }
}

/**
 * a string property value with type string
 *
 * @author Aleksander Demko
 */
class scopira::tool::property : public virtual scopira::tool::object
{
  public:
    typedef scopira::tool::iterator_g<scopira::tool::property*> iterator;
    typedef scopira::tool::iterator_g<const std::string *> key_iterator;

  protected:
    std::string dm_val;
    count_ptr<object> dm_obj;

  public:
    /// default constructor
    property(void);
    /// initing ctor
    property(const std::string &_val);
    /// initing ctor
    property(object *o);
    /// destructor
    virtual ~property();

    /// pretty ascii printer
    virtual oflow_i & print(oflow_i& o) const;
    /// serialization loader
    virtual bool load(iobjflow_i& in);
    /// serialization saver
    virtual void save(oobjflow_i& out) const;

    /// is this a record prop? (default, false)
    virtual bool is_node(void) const { return false; }

    /// node stuff
    /// appends to a property. creates a new property if need be
    virtual void add_end(const std::string& name, property* prop) { }
    /// gets a list of properties. if name doesnt exist, a valid, empty iterator will be returned
    virtual iterator get(const std::string& name);
    /// gets the first entry. this will crash if the entry doesnt exist!
    virtual property * get_first(const std::string& name) { return 0; }
    // gets the size of a list, same cost as list<>::size
    virtual size_t get_size(const std::string &name) const { return 0; }
    /// does the given key exist, at all
    virtual bool contains(const std::string& name) const { return false; }
    /// removes all the entries in the given prop
    virtual void remove(const std::string& name) { }
    /// returns an iterator of keys
    virtual key_iterator get_key_iterator(void);
    /// convinience routine, will always work, might return null
    virtual const std::string * get_string_value(const std::string &name) const { return 0; }
    /// convinience routine, will always work, might return null
    virtual object * get_object_value(const std::string &name) const { return 0; }

    /// sets the value string
    void set_value(const std::string& val);
    /// gets the value string
    const std::string& get_value_as_string(void) const { return dm_val; }
    /// sets the value, as an object*, will be ref counted
    void set_value(object *obj);
    /// gets the value, as a object
    object * get_value_as_object(void) const { return dm_obj.get(); }
};

/*
 * a string-prop_string list "record" of properties
 *
 * @author Aleksander Demko
 */
class scopira::tool::property_node : public scopira::tool::property
{
  public:
    typedef std::list < scopira::tool::count_ptr< scopira::tool::property > > sproplist_t; typedef std::map < std::string, sproplist_t > propmap_t;

  protected:
    propmap_t dm_propmap;

  public:
    /// default constructor
    property_node(void);
    /// destructor
    virtual ~property_node();

    /// pretty ascii printer
    virtual oflow_i & print(oflow_i &o) const;
    /// serialization loader
    virtual bool load(iobjflow_i &in);
    /// serialization saver
    virtual void save(oobjflow_i &out) const;

    /// is this a record prop?
    virtual bool is_node(void) const { return true; }

    /**
     * appends to a property. creates a new property if need be
     *
     * @author Aleksander Demko
     */
    virtual void add_end(const std::string& name, property* prop);
    /**
     * gets a list of properties. if name doesnt exist, a valid, empty
     * iterator will be returned
     *
     * @author Aleksander Demko
     */
    virtual iterator get(const std::string& name);
    /**
     * gets the first entry. returns null if doesnt exit
     *
     * @author Aleksander Demko
     */
    virtual property * get_first(const std::string& name);
    /**
     * gets the size of a list, same cost as list<>::size
     *
     * @author Aleksander Demko
     */
    virtual size_t get_size(const std::string &name) const;
    /**
     * does the given key exist, at all
     *
     * @author Aleksander Demko
     */
    virtual bool contains(const std::string& name) const;
    /**
     * removes all the entries in the given prop
     *
     * @author Aleksander Demko
     */
    virtual void remove(const std::string& name);

    /**
     * returns an iterator of keys
     *
     * @return the iterator
     * @author Aleksander Demko
     */
    virtual key_iterator get_key_iterator(void);

    /**
     * convinience routine, will always work, might return null
     *
     * @author Aleksander Demko
     */
    virtual const std::string * get_string_value(const std::string &name) const;
    /**
     * convinience routine, will always work, might return null
     *
     * @author Aleksander Demko
     */
    virtual object * get_object_value(const std::string &name) const;
};

#endif

