
/*
 *  Copyright (c) 2001-2006    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#ifndef __INCLUDED__SCOPIRA_UIKIT_SEEDENTRY_H__
#define __INCLUDED__SCOPIRA_UIKIT_SEEDENTRY_H__

#include <scopira/coreui/export.h>
#include <scopira/coreui/widget.h>
#include <scopira/coreui/checkbutton.h>
#include <scopira/coreui/entry.h>

namespace scopira
{
  namespace uikit
  {
    class seedentry;
  }
}

/**
 * A widget that lets the user choose a random seed.
 *
 * A seed is an interger, where 0 has the special meaning of "random seed".
 *
 * By default, it's 0.
 *
 * @author Aleksander Demko
 */ 
class scopira::uikit::seedentry : public virtual scopira::coreui::widget,
  public virtual scopira::coreui::checkbutton_reactor_i
{
  public:
    /// ctor
    SCOPIRAUI_EXPORT seedentry(void);

    SCOPIRAUI_EXPORT virtual void react_checkbutton(scopira::coreui::checkbutton *source, bool checkval);

    /// sets the current value of the widget
    SCOPIRAUI_EXPORT void set_seed(int s);

    /// gets the current value of the widget
    SCOPIRAUI_EXPORT int get_seed(void) const;

  protected:
    scopira::tool::count_ptr<scopira::coreui::entry> dm_entry;
    scopira::tool::count_ptr<scopira::coreui::checkbutton> dm_check;
};

#endif

