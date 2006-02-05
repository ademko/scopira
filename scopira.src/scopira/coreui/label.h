
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

#ifndef __INCLUDED__SCOPIRA_COREUI_LABEL__
#define __INCLUDED__SCOPIRA_COREUI_LABEL__

#include <string>

#include <scopira/coreui/widget.h>
#include <scopira/coreui/export.h>

namespace scopira
{
  namespace coreui
  {
    class label;
  }
}

/**
 * A static label/string.
 *
 * @author Aleksander Demko
 * @author Rodrigo Vivanco
 */
class scopira::coreui::label : public scopira::coreui::widget
{
  public:
    /**
     * Empty label constructor.
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT label(void);
    
    /**
     * Builds a label with the given string.
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT label(const std::string &label);

    /**
     * Sets the current label
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT void set_label(const std::string &val);
};

#endif

