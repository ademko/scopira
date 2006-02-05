
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

#ifndef __INCLUDED_SCOPIRA_CORE_VIEW_H__
#define __INCLUDED_SCOPIRA_CORE_VIEW_H__

#include <string>

#include <scopira/tool/object.h>
#include <scopira/tool/export.h>

namespace scopira
{
  namespace core
  {
    class view_action_reactor_i;
    class view_container_reactor_i;

    class view_i;

    class model_i; //fwd

    /**
     * Binds a view to a model.
     *
     * This should be called sometime after a view is created.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT void bind_model_view(scopira::core::model_i *m, scopira::core::view_i *v);
  }
}

/// internal handler interface
class scopira::core::view_action_reactor_i
{
  public:
    SCOPIRA_EXPORT virtual ~view_action_reactor_i() { }
    SCOPIRA_EXPORT virtual void react_view_action(scopira::core::view_i *source, int actionID) = 0;
    SCOPIRA_EXPORT virtual void react_view_set_title(scopira::core::view_i *source, const std::string &newtitle) = 0;
};

/**
 * An interface for classes that can receive new views from a view-spawning menu.
 * Yes, this is in core, not coreui.
 *
 * @author Aleksander Demko
 */ 
class scopira::core::view_container_reactor_i
{
  public:
    SCOPIRA_EXPORT virtual ~view_container_reactor_i() { }
    /**
     * Called when your spawn_reactor is to receive a new view.
     *
     * You should also call bind_model_view on the pair, after your setup, to
     * properly bind the model to the view.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT virtual void react_view_insert(scopira::core::model_i *m, scopira::core::view_i *v) = 0;
};

/**
 * A generic "view" that can be paired with a model.
 * This used to be an inspector.
 *
 * @author Aleksander Demko
 */ 
class scopira::core::view_i : public virtual scopira::tool::object
{
  private:
    view_action_reactor_i *dm_action_handler;
    std::string dm_pretitle;  //used if we get title change requests before we actually have a listener

  public:
    /**
     * This is called when a new model_i is bound/unbound.
     * This is also the only method called on non-gui views, so you should put
     * your action code in this method in that case.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT virtual void bind_model(scopira::core::model_i *sus);
    /// called when the model_i has "changed" by the given inspector (might be null)
    SCOPIRA_EXPORT virtual void react_model_update(scopira::core::model_i *sus, scopira::core::view_i *src);

    /// (internal) this is calle by infrastructure systems
    SCOPIRA_EXPORT void set_view_action_reactor(view_action_reactor_i *vh);

  protected:
    SCOPIRA_EXPORT view_i(void);

    /// descendants call this when they want to emit an action event
    SCOPIRA_EXPORT void react_view_action(int actionID);

    /// descedants call this when they want to change their displayed title
    SCOPIRA_EXPORT void set_view_title(const std::string &newtitle);
};

inline void scopira::core::bind_model_view(scopira::core::model_i *m, scopira::core::view_i *v)
{
  v->bind_model(m);
  v->react_model_update(m, 0);
}

#endif

