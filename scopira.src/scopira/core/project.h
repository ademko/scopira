
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

#ifndef __INCLUDED_SCOPIRA_CORE_MODELGROUP_H__
#define __INCLUDED_SCOPIRA_CORE_MODELGROUP_H__

#include <map>

#include <scopira/core/model.h>
#include <scopira/tool/export.h>

namespace scopira
{
  namespace core
  {
    class project_i;
    class project_base;
  }
}

/**
 * The project interface.
 * 
 * a project houses a collection of one or more models, and other information.
 *
 * @author Aleksander Demko
 */ 
class scopira::core::project_i : public virtual scopira::core::model_i
{
  public:
    typedef scopira::core::model_iterator model_iterator;

  public:
    /// I AM a project!
    virtual bool is_project(void) const { return true; }

    /**
     * Adds a model.
     * null parent == root node
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT virtual void add_model(model_i *parent, model_i *rec) = 0;
    /**
     * Removes the given model
     * null parent == root node
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT virtual void remove_model(model_i *parent, model_i *rec) = 0;
    /**
     * Does this group have the given model?
     * null parent == root node
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT virtual bool contains_model(model_i *parent, model_i *rec) = 0;
    /**
     * Does the group have this model anywhere, regarless of root.
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT virtual bool contains_model(model_i *rec) = 0;

    /**
     * Gets the parent model of a given model.
     * May be null (for root).
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT virtual model_i * get_parent(model_i *child) = 0;

    // future functions
    // something about properties
    // named-models? (or is that what find_model is for?)
    // rename get_parent to get_parent_model

    /**
     * Finds a model by name.
     * There may be many of them -- this finds the "first"
     * one.
     * Returns 0 if none was found.
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT virtual model_i * find_model(model_i *parent, const std::string &title) const = 0;

    /// gets a listing of all the sub models
    SCOPIRA_EXPORT virtual model_iterator get_model_iterator(model_i *parent) const = 0;

    /**
     * Is the project "empty?"
     *
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT virtual bool is_project_empty(void) const = 0;

    /**
     * Clears or resets the project to empty state.
     *
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT virtual void clear_project(void) = 0;

    /**
     * Move a project's contents into this one.
     *
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT virtual void move_project(scopira::core::project_i *rhs) = 0;

    /// used by model_i
    SCOPIRA_EXPORT virtual bool get_tagged_model_from_project(model_i *parent, const std::string &name,
      scopira::tool::count_ptr<scopira::core::model_i> &out) = 0;

  protected:
    /// Default initing ctor
    SCOPIRA_EXPORT project_i(void);
    /// title initing ctor
    SCOPIRA_EXPORT project_i(const std::string &title);
};

/**
 * A basic project_base implementation.
 *
 * Descendants can simply descend from this.
 *
 * @author Aleksander Demko
 */ 
class scopira::core::project_base : public virtual scopira::core::project_i
{
  protected:
    // this is a MULTI map of parents->children
    typedef std::multimap< tool::count_ptr<core::model_i>, tool::count_ptr<core::model_i> > model_set_t;
    typedef std::pair<model_set_t::const_iterator, model_set_t::const_iterator> const_iterator_pair;
    typedef std::pair<model_set_t::iterator, model_set_t::iterator> iterator_pair;

    // this is a map of children, parent (reverse of the above)
    typedef std::map<core::model_i*, core::model_i*> child_map;

    // in the future, there will be a child->optional properties set
    // map here

    model_set_t dm_models;
    child_map dm_child;

  public:
    /// Default initing ctor
    SCOPIRA_EXPORT project_base(void);
    /// title initing ctor
    SCOPIRA_EXPORT project_base(const std::string &title);

    /// dtor
    SCOPIRA_EXPORT virtual ~project_base();

    SCOPIRA_EXPORT virtual bool load(tool::iobjflow_i& in);
    SCOPIRA_EXPORT virtual void save(tool::oobjflow_i& out) const;

    /**
     * Adds a model.
     * null parent == root node
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT virtual void add_model(model_i *parent, model_i *rec);
    /**
     * Removes the given model
     * null parent == root node
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT virtual void remove_model(model_i *parent, model_i *rec);
    /**
     * Does this group have the given model?
     * null parent == root node
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT virtual bool contains_model(model_i *parent, model_i *rec);
    /**
     * Does the group have this model anywhere, regarless of root.
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT virtual bool contains_model(model_i *rec);

    /**
     * Gets the parent model of a given model.
     * May be null (for root).
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT virtual model_i * get_parent(model_i *child);

    /**
     * Finds a model by name.
     * There may be many of them -- this finds the "first"
     * one.
     * Returns 0 if none was found.
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT virtual model_i * find_model(model_i *parent, const std::string &title) const;

    /// gets a listing of all the sub models
    SCOPIRA_EXPORT virtual model_iterator get_model_iterator(model_i *parent) const;

    SCOPIRA_EXPORT virtual bool is_project_empty(void) const;
    SCOPIRA_EXPORT virtual void clear_project(void);
    SCOPIRA_EXPORT virtual void move_project(scopira::core::project_i *rhs);

    /// used by model_i
    SCOPIRA_EXPORT virtual bool get_tagged_model_from_project(model_i *parent, const std::string &name,
      scopira::tool::count_ptr<scopira::core::model_i> &out);

  private:
    /// removes all the nodes within the ranges
    /// including the node itself
    /// last is the LAST VALID element (not the usual end() one after syntax)
    void remove_by_range(model_set_t::iterator _begin, model_set_t::iterator _last);

    class project_iterator;
};

#endif

