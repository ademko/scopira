
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

#ifndef __INCLUDED_SCOPIRA_COREUI_VIEWMENU_H__
#define __INCLUDED_SCOPIRA_COREUI_VIEWMENU_H__

#include <scopira/core/model.h>
#include <scopira/coreui/button.h>
#include <scopira/coreui/menu.h>
#include <scopira/coreui/viewwidget.h>
#include <scopira/coreui/export.h>

namespace scopira
{
  namespace core
  {
    class viewtype;//fwd
  }
  namespace coreui
  {
    class view_filter_i;
    class view_filter_options;

    class noparen_filter;

    /**
     * Core builder of view menus based on models.
     * 
     * This will build a view menu into mb using m. m may be null, if you'd like.
     *
     * You must take care to make sure that m is a valid pointer through out the lifetime
     * of your menu. That is, m is not reference counted or kept by the function, but will
     * build handlers to this pointer.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRAUI_EXPORT void build_view_menu(scopira::core::model_i *m, scopira::coreui::menu_builder &mb);

    /**
     * Core builder of view menus based on models.
     * 
     * This will build a view menu into mb using m. m may be null, if you'd like.
     *
     * You must take care to make sure that m is a valid pointer through out the lifetime
     * of your menu. That is, m is not reference counted or kept by the function, but will
     * build handlers to this pointer.
     *
     * This version accepts additional parameters. The first is a set of options
     * that may be used to filter the generation of the menu. The 2nd, is a pointer
     * (which may be null) to the actuallt reactor that will be called when a choice is made
     * for view creation.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRAUI_EXPORT void build_view_menu(scopira::core::model_i *m, scopira::coreui::menu_builder &mb,
        const view_filter_options &filter, scopira::core::view_container_reactor_i *reac);

    /**
     * You can use this to launch the given view-type for the given model instance.
     *
     * @param m the model instance to operate on. Cannot be null.
     * @param vt the viewtype to use. Cannot be null. The other launch_view variant may be easier to use.
     * @param reac the receiver of the view. May be null (which performances the default "new window"
     * instantiation.
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT void launch_view(scopira::core::model_i *m, scopira::core::viewtype *vt,
      scopira::core::view_container_reactor_i *reac = 0);

    /**
     * You can use this to launch the given view-type for the given model instance.
     *
     * @param m the model instance to operate on. Cannot be null.
     * @param vt The full name of the view class that is to be used. This is the same
     * as the name used to register the view.
     * @param reac the receiver of the view. May be null (which performances the default "new window"
     * instantiation.
     * @return true on success (that is, the vt was valid)
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT bool launch_view(scopira::core::model_i *m, const std::string &vt,
      scopira::core::view_container_reactor_i *reac = 0);

    class view_button;
  }
}

/**
 * This is the interface for an object that can do view menu building
 * filtering.
 *
 * This probably could have been a simply function type, but is left as
 * an object interface for future growth.
 * @author Aleksander Demko
 */ 
class scopira::coreui::view_filter_i
{
  public:
    enum {
      menu_display_c,   // display the view in the menu, as normal
      menu_disable_c,   // show the view in the menu, but disabled and inaccessible
      menu_hide_c,      // hide theis viewtype from the menu completly
    };
  public:
    SCOPIRAUI_EXPORT virtual ~view_filter_i() { }
    /**
     * This is called for every model and viewtype pair.
     *
     * You must return one of the above enums.
     * @author Aleksander Demko
     */ 
    SCOPIRAUI_EXPORT virtual int filter_model_view(scopira::core::model_i *m, scopira::core::viewtype *vt) = 0;
};

/**
 * This tiny class is used to specify various filtering options to the view menu generator.
 *
 * You access it as a structure (ie. access it's member variables directly).
 *
 * @author Aleksander Demko
 */ 
class scopira::coreui::view_filter_options
{
  public:
    /// max depth to drill down. the default (-1), means drill down indefinatly.
    /// 0 means dont drill down at all. 1 means drill down at most one leve, etc
    int pm_max_depth;

    /// an optional user supplied filter. none (null), by default
    view_filter_i *pm_filter;

    /// a bitmask of vg_*_c constants to show. by default, this is vg_show_c,
    int pm_show_vgmask;

  public:
    /**
     * Constructor.
     *
     * Sets all the elements to their natural defaults.
     * @author Aleksander Demko
     */ 
    SCOPIRAUI_EXPORT view_filter_options(void);
};

/**
 * A filter that allows hides all the viewtypes that start with (
 * in their menu names.
 * @author Aleksander Demko
 */ 
class scopira::coreui::noparen_filter : public scopira::coreui::view_filter_i
{
  public:
    SCOPIRAUI_EXPORT virtual int filter_model_view(scopira::core::model_i *m, scopira::core::viewtype *vt);
};

/**
 * A button, that when clicked, launches a view menu
 *
 * @author Aleksander Demko
 */ 
class scopira::coreui::view_button : public scopira::coreui::button,
  protected virtual scopira::coreui::button_reactor_i
{
  public:
    /**
     * A button that spawns a view. You must set the model to be operated
     * on with bind_model (and everytime your model changes).
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT view_button(void);
    /**
     * A button that spawns a view. You must set the model to be operated
     * on with bind_model (and everytime your model changes).
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT view_button(const std::string &title);
    /**
     * A button that spawns a view. Pass your model_ptr too.
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT view_button(const std::string &title, const scopira::core::model_ptr_base &mptr);

    /**
     * Changes the model this button will operate.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRAUI_EXPORT void bind_model(scopira::core::model_i *sus);

    /**
     * Rather than binding to a model instant, bind to a container (pointer)
     * of models and just watch that.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRAUI_EXPORT void bind_model_ptr(scopira::core::model_i * const *mp);

    /**
     * Rather than binding to a model instant, bind to a container (pointer)
     * of models and just watch that.
     *
     * @author Aleksander Demko
     */ 
    void bind_model_ptr(const scopira::core::model_ptr_base &mptr)
      { bind_model_ptr(&mptr.get_model_ptr()); }

  protected:
    /// internal method
    SCOPIRAUI_EXPORT virtual void react_button(scopira::coreui::button *source, int actionid);

  private:
    scopira::tool::count_ptr<scopira::core::model_i> dm_model;
    scopira::core::model_i * const *dm_model_ptr;
};

#endif

