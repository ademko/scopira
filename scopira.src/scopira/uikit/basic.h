
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

#ifndef __INCLUDED_SCOPIRA_UIKIT_BASIC_H__
#define __INCLUDED_SCOPIRA_UIKIT_BASIC_H__

#include <scopira/coreui/entry.h>
#include <scopira/coreui/viewwidget.h>

namespace scopira
{
  namespace uikit
  {
    class rename_v;
    class remove_v;
    class copy_v;
  }
}

/**
 * Renames/retitles any model
 *
 * @author Aleksander Demko
 */ 
class scopira::uikit::rename_v : public scopira::coreui::viewwidget
{
  protected:
    scopira::core::model_ptr<scopira::core::model_i > dm_model;

  public:
    /// ctor
    rename_v(void);

    virtual void bind_model(scopira::core::model_i *sus);
    virtual void react_model_update(scopira::core::model_i *sus, scopira::core::view_i *src);

    virtual void react_button(scopira::coreui::button *source, int actionid);

  private:
    void init_gui(void);

  private:
    scopira::tool::count_ptr<scopira::coreui::entry> dm_entry;
};

/**
 * Removes the model from the view.
 * Note that his is part of the vg_project_c group (and thus must be deliveratly included
 * in your viewmenu display filter options)
 *
 * @author Aleksander Demko
 */ 
class scopira::uikit::remove_v : public scopira::coreui::viewwidget
{
  protected:
    scopira::core::model_ptr<scopira::core::model_i > dm_model;

  public:
    /// ctor
    remove_v(void);

    virtual void bind_model(scopira::core::model_i *sus);

    virtual void react_button(scopira::coreui::button *source, int actionid);
};

/**
 * Duplicates a copyable_model_i.
 *
 * @author Aleksander Demko
 */
class scopira::uikit::copy_v : public scopira::core::view_i
{
  public:
    virtual void bind_model(scopira::core::model_i *sus);
};

#endif

