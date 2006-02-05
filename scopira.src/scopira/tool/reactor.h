
/*
 *  Copyright (c) 2001    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#ifndef __INCLUDED__SCOPIRA_TOOL_REACTOR__
#define __INCLUDED__SCOPIRA_TOOL_REACTOR__

#include <string>

#include <scopira/tool/object.h>

namespace scopira
{
  namespace tool
  {
    class destroy_reactor_i;
    class update_reactor_i;
    class rename_reactor_i;
  }
}

/**
 * a reactor interface that receives destroy actions
 *
 * @author Aleksander Demko
 */
class scopira::tool::destroy_reactor_i : public virtual scopira::tool::object
{
  public:
    /**
     * called when the given object is in its destruction phase
     *
     * @author Aleksander Demko
     */
    virtual void react_destroy(object *source) = 0;
};

/**
 * a reactor interface that receives update actions
 *
 * @author Aleksander Demko
 */
class scopira::tool::update_reactor_i : public virtual scopira::tool::object
{
  public:
    /**
     * called when the given object has been updated
     *
     * @author Aleksander Demko
     */
    virtual void react_update(object *source) = 0;
};

/**
 * Called when an object is renamed
 * @author Aleksander Demko
 */ 
class scopira::tool::rename_reactor_i : public virtual scopira::tool::object
{
  public:
    /**
     * Called when the item is renamed.
     * @author Aleksander Demko
     */
    virtual void react_rename(object *source, const std::string &newname) = 0;
};

#endif

