
/*
 *  Copyright (c) 2003    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#ifndef __INCLUDED__SCOPIRA_COREUI_OUTPUT_H__
#define __INCLUDED__SCOPIRA_COREUI_OUTPUT_H__

#include <string>

#include <gtk/gtk.h>

#include <scopira/tool/flow.h>
#include <scopira/coreui/widget.h>
#include <scopira/coreui/export.h>

namespace scopira
{
  namespace coreui
  {
    class outputwidget;
  }
}

class scopira::coreui::outputwidget : public scopira::coreui::widget,
  private scopira::tool::oflow_i
{
  private:
    typedef scopira::coreui::widget parent_type;

  public:
    /// ctor
    SCOPIRAUI_EXPORT outputwidget(void);
    
    /// sets the diplsay buffer
    SCOPIRAUI_EXPORT void set_output(const std::string &buf);
    /// sets the diplsay buffer, -1 == null terminated
    SCOPIRAUI_EXPORT void set_output(const char *buf, int len = -1);
    /// appends to the existing display buffer
    SCOPIRAUI_EXPORT void append_output(const std::string &buf);
    /// different kind of append_output. -1 == null terminated
    SCOPIRAUI_EXPORT void append_output(const char *buf, int len = -1);
    /// moves the current to the end, you'll probably want to call this after a few outputs
    SCOPIRAUI_EXPORT void move_to_end(void);

    /// clears the output buffer
    void clear_output(void) { set_output(""); }
    /// returns this thing as a flow
    scopira::tool::oflow_i &flow_output(void) { return *this; }

  private:
    void init_gui(void);

    SCOPIRAUI_EXPORT virtual bool failed(void) const { return false; }
    SCOPIRAUI_EXPORT virtual size_t write(const byte_t *_buf, size_t _size);

  private:
    //the text widget is dm_widget
    GtkWidget  *dm_text_widget;

    GtkTextBuffer *dm_buffer; //the text buffer
    GtkTextMark *dm_mark;
};

#endif

