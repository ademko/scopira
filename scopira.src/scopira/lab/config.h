
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

#ifndef __INCLUDED_SCOPIRA_LAB_CONFIG_H__
#define __INCLUDED_SCOPIRA_LAB_CONFIG_H__

#include <scopira/coreui/layout.h>
#include <scopira/lab/window.h>
#include <scopira/coreui/export.h>

namespace scopira
{
  namespace lab
  {
    class config_window;
  }
}

/**
 * A window class for lab users to quickly build configuration screens
 * for interfacing with the "config" system (command line and stored options/flags).
 *
 * @author Aleksander Demko
 */
class scopira::lab::config_window : public scopira::lab::lab_window
{
  public:
    /**
     * A position object. Used to specify where a widget should go
     *
     * @author Aleksander Demko
     */
    class position
    {
      public:
        short pm_x, pm_y, pm_w, pm_h;
        bool pm_expand_h, pm_expand_w;
      public:
        /// ctor, where magic is the XY in the form of XYYY (for example, 9003 is x=9 y=3)
        /// w=1 and h=1 for this ctor
        /// yeah, its cute as it allows implicit creating
        SCOPIRAUI_EXPORT position(int magic);
        /// ctor
        SCOPIRAUI_EXPORT position(short x, short y);
        /// ctor
        SCOPIRAUI_EXPORT position(short x, short y, short w, short h);
        /// ctor
        SCOPIRAUI_EXPORT position(short x, short y, short w, short h, bool expandH, bool expandV);
    };

    /**
     * Constructor.
     *
     * This version is you'd like a *tab* set of widgets.
     *
     * You must call add_tab() for each tab page you'd like. You must
     * call add_tab atleast once.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRAUI_EXPORT config_window(void);

    /**
     * Constructor.
     *
     * This version is for if you want only one screen of widgets.
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT config_window(short w, short h);

    /**
     * Adds a new tab. Only value if you used the (void) ctor.
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT void add_tab(const std::string &label, short w, short h);

    /**
     * Adds a separator line. This is cosmetic.
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT void add_separator(position pos, bool horiz);

    /**
     * Adds a label. This is cosmetic.
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT void add_label(position pos, const std::string &label);
    /**
     * Adds a generic string entry box.
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT void add_entry(position pos, const std::string &keyname);

    /**
     * Adds a spin button.
     *
     * The default options specify paramters for editing a real number between 0 and 1
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT void add_spinbutton(position pos, const std::string &keyname,
      double min=0, double max=1, double step=0.05, short numdigits = 5);

    /**
     * Adds a file browing box.
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT void add_filename(position pos, const std::string &keyname);

    /**
     * Adds a checkbox with 1|0 toggle action
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT void add_checkbutton(position pos, const std::string &keyname, const std::string &label);

    /// used by add_dropdown
    class dropdown_add
    {
      public:
        virtual ~dropdown_add() { }
        SCOPIRAUI_EXPORT virtual dropdown_add * add(int id, const std::string &label) = 0;
    };

    /**
     * Adds a drop down box.
     *
     * You may then use the returned object to add some entries.
     *
     * @author Aleksander Demko
     */ 
    dropdown_add * add_dropdown(position pos, const std::string &keyname);

    typedef dropdown_add radiobutton_add;

    /**
     * Adds a drop down box.
     *
     * You may then use the returned object to add some entries.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRAUI_EXPORT radiobutton_add * add_radiobutton(position pos, const std::string &keyname);

    // future things
    // add ... option spec objects for placement and width (like existing file placement)
    // _int(min,max) _double(min,ax)
    // _separate(void)
    // _list("label=value:label=value)

    class config_widget_i;

  protected:
    SCOPIRAUI_EXPORT virtual bool on_button(int actionID);
  private:
    typedef std::vector<config_widget_i*> cwlist_t;
    cwlist_t dm_cw;

    scopira::coreui::tab_layout * dm_tabber;
    scopira::coreui::grid_layout * dm_boxer;
};

#endif

