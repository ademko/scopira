
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

#ifndef __INCLUDED_SCOPIRA_COREUI_VIEWWINDOW_H__
#define __INCLUDED_SCOPIRA_COREUI_VIEWWINDOW_H__

#include <scopira/core/view.h>
#include <scopira/coreui/window.h>
#include <scopira/coreui/viewmenu.h>
#include <scopira/coreui/export.h>

namespace scopira
{
  namespace coreui
  {
    class viewwindow;
    class viewtabs;
    class viewcontainer;
  }
}

/**
 * A window that holds a model and view.
 * Created and instantited by viewmenu.h stuff.
 *
 * @author Aleksander Demko
 */ 
class scopira::coreui::viewwindow : public scopira::coreui::window,
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
    SCOPIRAUI_EXPORT viewwindow(scopira::core::model_i *m, scopira::core::view_i *v);

    /// dtor
    SCOPIRAUI_EXPORT virtual ~viewwindow();

  protected:
    /// this is called by the view to send actionIDs... ill just redirect back
    SCOPIRAUI_EXPORT virtual void react_view_action(scopira::core::view_i *source, int actionID);
    SCOPIRAUI_EXPORT virtual void react_view_set_title(scopira::core::view_i *source, const std::string &newtitle);

  protected:
    scopira::tool::count_ptr<scopira::core::model_i> dm_model;
    scopira::tool::count_ptr<scopira::core::view_i> dm_view;
    scopira::tool::count_ptr<scopira::coreui::widget> dm_viewwidget;
};

/**
 * This tab-layout like class is a 
 * view_container_reactor_i (receiver of new views) that puts the
 * new views as new tabs.
 *
 * @author Aleksander Demko
 */ 
class scopira::coreui::viewtabs : public scopira::coreui::widget,
  protected virtual scopira::core::view_action_reactor_i,
  public virtual scopira::core::view_container_reactor_i
{
  public:
    /// ctor
    SCOPIRAUI_EXPORT viewtabs(void);
    /// dtor
    SCOPIRAUI_EXPORT virtual ~viewtabs();

    SCOPIRAUI_EXPORT virtual void react_view_insert(scopira::core::model_i *m, scopira::core::view_i *v);

  protected:
    /// this is called by the view to send actionIDs... ill just redirect back
    SCOPIRAUI_EXPORT virtual void react_view_action(scopira::core::view_i *source, int actionID);
    SCOPIRAUI_EXPORT virtual void react_view_set_title(scopira::core::view_i *source, const std::string &newtitle);

  private:
    struct child_t {
      scopira::tool::count_ptr<widget> pm_widget;
      scopira::tool::count_ptr<scopira::core::model_i> pm_model;
      scopira::tool::count_ptr<scopira::core::view_i> pm_view;
      GtkWidget *pm_label;
    };
    typedef std::vector< child_t > children_t;
    children_t dm_children;

  private:
    void close_tab(GtkWidget *target);
    static void h_on_close_tab_button(GtkButton *but, gpointer data);
    static void h_on_detach_tab_button(GtkButton *but, gpointer data);
};

/**
 * This container holds a single view and implements the view_container_reactor_i
 * interface. Any new views *replace* the existing one, if any.
 *
 * @author Aleksander Demko
 */ 
class scopira::coreui::viewcontainer : public scopira::coreui::box_layout,
  protected virtual scopira::core::view_action_reactor_i,
  public virtual scopira::core::view_container_reactor_i
{
  public:
    /// ctor
    SCOPIRAUI_EXPORT viewcontainer(void);
    /// dtor
    SCOPIRAUI_EXPORT virtual ~viewcontainer();

    SCOPIRAUI_EXPORT virtual void react_view_insert(scopira::core::model_i *m, scopira::core::view_i *v);

  protected:
    widget *dm_thewidget;   // its ref counted by the box_layout, no need to do it here

  protected:
    /// this is called by the view to send actionIDs... ill just redirect back
    SCOPIRAUI_EXPORT virtual void react_view_action(scopira::core::view_i *source, int actionID);
    SCOPIRAUI_EXPORT virtual void react_view_set_title(scopira::core::view_i *source, const std::string &newtitle);
};

#endif

