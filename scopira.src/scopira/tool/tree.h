
/*
 *  Copyright (c) 2002-2003    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#ifndef __INCLUDED_SCOPIRA_TOOL_TREE_H__
#define __INCLUDED_SCOPIRA_TOOL_TREE_H__

#include <map>
#include <string>

#include <scopira/tool/object.h>
#include <scopira/tool/iterator.h>
#include <scopira/tool/object.h>
#include <scopira/tool/export.h>

namespace scopira
{
  namespace tool
  {
    class treenode;
  }
}

/**
 * A recursive tree structure, useful for building menus and other light systems.
 *
 * Each node can have an associated (reference counted) data object and name.
 *
 * @author Aleksander Demko
 */
class scopira::tool::treenode : public scopira::tool::object
{
  public:
    typedef scopira::tool::iterator_g< const std::string& > key_iterator;

  private:
    typedef std::map<std::string, scopira::tool::count_ptr<treenode> > nodemap_t;

  public:
    /// ctor
    SCOPIRA_EXPORT treenode(scopira::tool::object *o = 0);
    /// dtor
    SCOPIRA_EXPORT virtual ~treenode();

    /// gets the object
    SCOPIRA_EXPORT scopira::tool::object * get_object(void) { return dm_data.get(); }

    /// sets a node, returns created node
    SCOPIRA_EXPORT void set(const std::string &name, treenode *nd);
    /// gets a node, might return null
    SCOPIRA_EXPORT treenode * get(const std::string &name);

    /**
     * Parses the path (with / deliminating sub nodes) and
     * inserts the node into the tree.
     *
     * nd is ref counted.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT void set_path(const std::string &path_name, treenode *nd);

    /**
     * Get an iterator of all the childeren.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT key_iterator get_key_iterator(void);

  protected:
    nodemap_t dm_nodes;
    scopira::tool::count_ptr<scopira::tool::object> dm_data;
};

#endif

