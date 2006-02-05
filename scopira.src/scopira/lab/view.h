
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

#ifndef __INCLUDED_SCOPIRA_LAB_VIEW_H__
#define __INCLUDED_SCOPIRA_LAB_VIEW_H__

#include <scopira/core/model.h>
#include <scopira/core/view.h>
#include <scopira/lab/window.h>
#include <scopira/coreui/export.h>

namespace scopira
{
  namespace lab
  {
    class view_window;
  }
}

/**
 * A lab window for displaying a view and its matching model.
 *
 * This will also allow the view to spawn other views, etc.
 *
 * @author Aleksander Demko
 */ 
class scopira::lab::view_window : public scopira::lab::lab_window,
  protected virtual scopira::core::view_action_reactor_i
{
  public:
    /**
     * Public ctor, for use in wrapping around an explicit model
     * and matching view.
     *
     * This ctor will also "bind" the model to the view, via bind_model_view.
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT view_window(scopira::core::model_i *m, const std::string &vtname);

    /// dtor
    SCOPIRAUI_EXPORT virtual ~view_window();

  protected:
    /// this is called by the view to send actionIDs... ill just redirect back
    SCOPIRAUI_EXPORT virtual void react_view_action(scopira::core::view_i *source, int actionID);
    SCOPIRAUI_EXPORT virtual void react_view_set_title(scopira::core::view_i *source, const std::string &newtitle);

  protected:
    scopira::tool::count_ptr<scopira::core::model_i> dm_model;
    scopira::tool::count_ptr<scopira::core::view_i> dm_view;
};

#endif

