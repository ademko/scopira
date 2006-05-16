
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

#include <scopira/coreui/output.h>

//BBlibs scopira
//BBtargets libscopiraui.so

using namespace scopira::coreui;

outputwidget::outputwidget(void)
{
  dm_mark = 0;

  init_gui();
}
    
void outputwidget::set_output(const std::string &buf)
{
  set_output(buf.c_str(), buf.size());
}

void outputwidget::set_output(const char *buf, int len)
{
  assert(buf);
  assert(len >= -1);
  gtk_text_buffer_set_text(dm_buffer, buf, len);
}

void outputwidget::append_output(const std::string &buf)
{
  append_output(buf.c_str(), buf.size());
}

void outputwidget::append_output(const char *buf, int len)
{
  GtkTextIter end;

  if (len == 0)
    return;

  assert(len>0 || len == -1);
  assert(buf);

  //get the end of the buffer
  gtk_text_buffer_get_end_iter(dm_buffer, &end);
  //insert the message at the end
  gtk_text_buffer_insert(dm_buffer, &end, buf, len);
  //move cursor and scroll to the end
}

void outputwidget::move_to_end(void)
{
  GtkTextIter end;
  
  //move to the end of the buffer
  gtk_text_buffer_get_end_iter(dm_buffer, &end);
  //place cursor at the end of the buffer
  gtk_text_buffer_place_cursor(dm_buffer, &end);
  //mark the end of the buffer
  if (!dm_mark) 
    dm_mark = gtk_text_buffer_create_mark(dm_buffer, 0, &end, TRUE);
  else
    gtk_text_buffer_move_mark(dm_buffer, dm_mark, &end);  
  
  //scroll to the mark
  gtk_text_view_scroll_to_mark(GTK_TEXT_VIEW(dm_text_widget), dm_mark, 0, TRUE, 0, 0);
}

void outputwidget::init_gui(void)
{
  // text widget
  dm_text_widget = gtk_text_view_new();

  // scrolled container widget
  dm_widget = gtk_scrolled_window_new( NULL, NULL );
  gtk_scrolled_window_add_with_viewport( GTK_SCROLLED_WINDOW( dm_widget ), dm_text_widget );
  
  //fixed width font
  PangoFontDescription *font;

  //text font
  font = pango_font_description_from_string("Monospace 10");
  gtk_widget_modify_font(dm_text_widget, font);
  pango_font_description_free(font);
  
  gtk_text_view_set_left_margin(GTK_TEXT_VIEW(dm_text_widget), 2);
  gtk_text_view_set_editable(GTK_TEXT_VIEW(dm_text_widget), false);
  gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(dm_text_widget), GTK_WRAP_NONE);  
  
  //create text buffer
  dm_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(dm_text_widget));
  gtk_text_buffer_set_text(dm_buffer, "", -1);
  
  parent_type::init_gui();
}

size_t outputwidget::write(const byte_t *_buf, size_t _size)
{
  append_output(reinterpret_cast<const char*>(_buf), _size);
  return _size;
}

