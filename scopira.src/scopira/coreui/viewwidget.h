
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

#ifndef __INCLUDED_SCOPIRA_COREUI_VIEWWIDGET_H__
#define __INCLUDED_SCOPIRA_COREUI_VIEWWIDGET_H__

#include <scopira/core/model.h>
#include <scopira/core/view.h>
#include <scopira/coreui/widget.h>
#include <scopira/coreui/button.h>
#include <scopira/coreui/layout.h>
#include <scopira/coreui/export.h>

namespace scopira
{
  namespace coreui
  {
    class viewwidget;

    class button_layout;  //fwd
  }
}

/**
 * A useful widget-ish base class for widgets that want to do common view stuff.
 *
 * Widgets that are also views don't HAVE to inherit from this class, but it's handy.
 *
 * Basically, in your contructor, you build your gui and pass that whole panel to
 * viewwidget::init_gui().
 * You'll want to override react_button to listen to any button clicks. Add any other
 * reactor event types as needed.
 *
 * @author Aleksander Demko
 */
class scopira::coreui::viewwidget : public virtual scopira::coreui::widget,
  public virtual scopira::core::view_i,
  public virtual scopira::coreui::button_reactor_i,
  public scopira::coreui::button_constants
{
  protected:
    scopira::tool::count_ptr<widget> dm_viewwidget;

    /**
     * Descendants should override this.
     * You should also call this version after yours.
     * Currently, this redirects out the actionID through the react_view_action call
     * in view_i.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRAUI_EXPORT virtual void react_button(scopira::coreui::button *source, int actionid);

  protected:
    /// ctor
    SCOPIRAUI_EXPORT viewwidget(void);

    /**
     * Descendants must call this. This version is
     * button layout aware.
     *
     * The button layout class is returned, incase you want to add more of your
     * own buttons too.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRAUI_EXPORT button_layout * init_gui(widget *basewidget, int buttonmask);

    /**
     * Adds an additional view-launching button for the given model
     * pointer.
     *
     * This must be called after calling init_gui.
     *
     * @author Aleksander Demko
     */ 
    void add_viewbutton(const std::string &title, const scopira::core::model_ptr_base &mptr)
      { add_viewbutton(title, &mptr.get_model_ptr()); }

    /**
     * Adds an additional view-launching button for the given model
     * pointer.
     *
     * This must be called after calling init_gui.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRAUI_EXPORT void add_viewbutton(const std::string &title, scopira::core::model_i * const * daptr);

    /**
     * This version does not do any button layout logic for you.
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT void init_gui(widget *basewidget);
};

#endif

