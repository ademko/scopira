
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

#ifndef __INCLUDED_SCOPIRA_UIKIT_JOYSTICK_H__
#define __INCLUDED_SCOPIRA_UIKIT_JOYSTICK_H__

#include <scopira/coreui/widget.h>
#include <scopira/coreui/button.h>
#include <scopira/coreui/export.h>

namespace scopira
{
  namespace uikit
  {
    class joystick_reactor_i;
    class joystick;
  }
}

/**
 * Listener to joystick events.
 * Future additions: icons, subset usage of the 8-directions.
 * @author Aleksander Demko
 */
class scopira::uikit::joystick_reactor_i
{
  public:
    SCOPIRAUI_EXPORT virtual ~joystick_reactor_i() { }
    /**
     * Called when a joystick button was pressed
     * @param source source of this joystick event. This may be null, depending
     * on the producer
     * @param deltax change in X axis. One of -1, 0 or 1
     * @param deltay change in Y axis. One of -1, 0 or 1
     * @param code The "code" of this change. See the number pad on your keyboad
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT virtual void react_joystick(scopira::uikit::joystick *source,
        short deltax, short deltay, short code) = 0;
};

/**
 * A collection of 8 buttons that make up a joystick like control.
 * This is a candidate for baseuikit.
 * @author Aleksander Demko
 */
class scopira::uikit::joystick : public scopira::coreui::widget,
  public virtual scopira::coreui::button_reactor_i
{
  private:
    scopira::tool::count_ptr<scopira::coreui::widget> dm_basewidget;
  protected:
    joystick_reactor_i *dm_joystick_reactor;
  public:
    /// ctor
    SCOPIRAUI_EXPORT joystick(void);

    SCOPIRAUI_EXPORT virtual void react_button(scopira::coreui::button *source, int childid);

    /**
     * Sets the joystick even listener. May be null to clear the setting.
     * @author Aleksander Demko
     */ 
    void set_joystick_reactor(joystick_reactor_i *react) { dm_joystick_reactor = react; }
};

#endif
