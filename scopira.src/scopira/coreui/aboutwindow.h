
/*
 *  Copyright (c) 2005-2007    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#ifndef __INCLUDED_SCOPIRA_COREUI_ABOUTWINDOW_H__
#define __INCLUDED_SCOPIRA_COREUI_ABOUTWINDOW_H__

#include <scopira/coreui/window.h>
#include <scopira/coreui/layout.h>
#include <scopira/coreui/export.h>

namespace scopira
{
  namespace coreui
  {
    class aboutwindow;

    class abouttab_i;
  }
}

/**
 * An about dialog box
 *
 * @author Aleksander Demko
 */
class scopira::coreui::aboutwindow : public scopira::coreui::dialog
{
  private:
    scopira::tool::count_ptr<tab_layout> dm_tabber;

  public:
    /**
     * Constructor
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT aboutwindow(const std::string &windowtitle);


    /**
     * Adds an about scopira tab.
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT void add_tab_scopira(void);

    /**
     * Adds a new tab
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT void add_tab(widget *wid, const std::string &label);

    /**
     * Adds a stock layout tab.
     *
     * xpm_data may be null.
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT void add_tab(const char **xpm_data, const std::string &desc, const std::string &tablabel);

  protected:
    /**
     * Adds all the dynamically registered plug in about tabs.
     *
     * Change this to public access, rather than feed off add_tab_scopira()?
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT void add_tab_dll(void);

  private:
    void init_gui(void);
};

/**
 * A dynamic about tab.
 *
 * @author Aleksander Demko
 */
class scopira::coreui::abouttab_i : public virtual scopira::tool::object
{
  public:
    /// simple ctor, does nothing
    abouttab_i(void);
    /// called when its time to create the tab
    virtual void add_tab(scopira::coreui::aboutwindow *aw) = 0;
};

#endif

