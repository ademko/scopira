
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

#ifndef __INCLUDED_SCOPIRA_COREUI_MESSAGEWINDOW_H__
#define __INCLUDED_SCOPIRA_COREUI_MESSAGEWINDOW_H__

#include <scopira/coreui/window.h>
#include <scopira/coreui/export.h>
#include <scopira/coreui/button.h>
#include <scopira/coreui/entry.h>

namespace scopira
{
  namespace coreui
  {
    class messagewindow;
    class entrywindow;
  }
}

/**
 * A window to present simple messages to the user
 *
 * @author Aleksander Demko
 * @author Marina Mandelzweig
 */
class scopira::coreui::messagewindow : public scopira::coreui::dialog
{
  public:
    enum {
      notype_c,
      info_c,
      error_c,
      question_c,   // in the future, I can add blocking calls thatll return which button they clicked (like in lab)
    };
  public:
    /**
     * Constructor
     * @author Aleksander Demko
     */
   SCOPIRAUI_EXPORT messagewindow(const std::string &title, const std::string &msg,
        int typ = notype_c, bool modal = true);

    /**
     * Create and spawn a message window.
     *
     * In the future, we can do blocking (like waitforreply in lab stuff)
     * after I look into syncing the gtk+ main loops a little more.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRAUI_EXPORT static void popup(const std::string &title, const std::string &msg,
        int typ = notype_c, bool modal = true);

    /**
     * Even simpler variant of popup()
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT static void popup_error(const std::string &msg);
};

/**
 * A windows with a simply entry box.
 *
 * @author Aleksander Demko
 */
class scopira::coreui::entrywindow : public scopira::coreui::window,
  public virtual scopira::coreui::button_reactor_i
{
  private:
    scopira::coreui::entry_reactor_i *dm_reactor;
  public:
    SCOPIRAUI_EXPORT entrywindow(const std::string &title, const std::string &msg,
      const std::string &defval = "");

    void set_entry_reactor(entry_reactor_i *react) { dm_reactor = react; }

    SCOPIRAUI_EXPORT virtual void react_button(scopira::coreui::button *source, int actionid);

  private:
    scopira::tool::count_ptr<scopira::coreui::widget> dm_thewidget;
    scopira::tool::count_ptr<scopira::coreui::entry> dm_entry;
};

#endif

