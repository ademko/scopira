
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

#ifndef __INCLUDED__SCOPIRA_UIKIT_FRAME_H__
#define __INCLUDED__SCOPIRA_UIKIT_FRAME_H__

#include <gtk/gtk.h>

#include <list>
#include <vector>

#include <scopira/tool/array.h>
#include <scopira/coreui/widget.h>
#include <scopira/coreui/export.h>

namespace scopira
{
  namespace uikit
  {
    class frame;
  }
}

/**
 * decorative frame and optional label
 *
 * @author Marina Mandelzweig
 */ 
class scopira::uikit::frame : public scopira::coreui::widget
{
  protected:
    /// subwidget, not always used
    scopira::tool::count_ptr<scopira::coreui::widget> dm_child;
  
    
  public:
    /**
     * Constructor.
     * @author Marina Mandelzweig
     */ 
    SCOPIRAUI_EXPORT frame(void);
    
    /**
     * Constructor.
     * @param label the label
     * @author Marina Mandelzweig
     */ 
    SCOPIRAUI_EXPORT frame(const std::string &label);
        
    /// api access routines
    SCOPIRAUI_EXPORT void set_label(const std::string &label);
    
    /// add a container inside
    SCOPIRAUI_EXPORT void add_widget(scopira::coreui::widget *w);
};

#endif

