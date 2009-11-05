
/*
 *  Copyright (c) 2002    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#ifndef __INCLUDED__SCOPIRA_COREUI_ENTRY_H__
#define __INCLUDED__SCOPIRA_COREUI_ENTRY_H__

#include <string>
#include <list>

#include <gtk/gtk.h>

#include <scopira/coreui/widget.h>
#include <scopira/coreui/export.h>

namespace scopira
{
  namespace coreui
  {
    class entry_reactor_i;
    class entry;
  }
}

/**
 * listener to entry events, when the entry changes
 * @author Aleksander Demko
 */
class scopira::coreui::entry_reactor_i
{
  public:
    SCOPIRAUI_EXPORT virtual ~entry_reactor_i() { }
    SCOPIRAUI_EXPORT virtual void react_entry(scopira::coreui::entry *source, const char *msg) = 0;
};

/**
 * A simple text entry box.
 * @author Aleksander Demko
 */ 
class scopira::coreui::entry : public scopira::coreui::widget
{
  protected:
    // various set options
    bool dm_oint, dm_odouble, dm_onospace, dm_opassword;
    bool dm_hasmin, dm_hasmax, dm_hasminlen, dm_hasmaxlen;
    double dm_min, dm_max;
    int dm_minlen, dm_maxlen;
    bool dm_editable;

    entry_reactor_i *dm_reactor;

  public:
    /// constrcutor
    SCOPIRAUI_EXPORT entry(void);
    
    /// sets the reactor
    void set_entry_reactor(entry_reactor_i *react) { dm_reactor = react; }

    /// another getter
    SCOPIRAUI_EXPORT std::string get_text(void) const;
    /// gets the current data text
    SCOPIRAUI_EXPORT void get_text(std::string &out);
    /// sets the current data text
    SCOPIRAUI_EXPORT void set_text(const std::string &t);
    /// sets the current data text. faster than above. 0 == ""
    SCOPIRAUI_EXPORT void set_text(const char *t);

    /// highlights all the text
    SCOPIRAUI_EXPORT void select_all(void);
    /// highlights none of the text
    SCOPIRAUI_EXPORT void select_none(void);
		/// changes the size request of the entry to be about the right 
		/// size for n_chars characters.-1, reverts to the default entry size.
		SCOPIRAUI_EXPORT void set_max_length(int max);

  protected:
    SCOPIRAUI_EXPORT void init_gui(void);

  private:
    static void h_on_activate(GtkEntry *e, gpointer data);
};

#endif
