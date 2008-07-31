
/*
 *  Copyright (c) 2006    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */


#ifndef __INCLUDED_SCOPIRA_COREUI_PROGRESSBAR_H__
#define __INCLUDED_SCOPIRA_COREUI_PROGRESSBAR_H__

#include <string>

#include <scopira/coreui/widget.h>
#include <scopira/coreui/export.h>

namespace scopira
{
  namespace coreui
  {
    class progressbar;
  }
}

/**
 * A progreess bar.
 *
 * @author Aleksander Demko
 */
class scopira::coreui::progressbar : public scopira::coreui::widget
{
  public:
    /**
     * Constructor. horiz is if you want a horizonal (the default)
     * rather than a vertical one.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRAUI_EXPORT progressbar(bool horiz = true);

    /**
     * Sets the progress bar progress, from 0..1
     *
     * @author Aleksander Demko
     */ 
    SCOPIRAUI_EXPORT void set_fraction(double d);
    /**
     * Set the text, if any, to superimpose over the progress bar.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRAUI_EXPORT void set_text(const std::string &txt);
    /**
     * "Pulse" the prorgess bar -- useful when you just want to show
     * activity but do not know the fraction/percent complete.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRAUI_EXPORT void pulse(void);
};

#endif

