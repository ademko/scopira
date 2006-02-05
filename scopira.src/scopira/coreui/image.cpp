
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

#include <scopira/coreui/image.h>

#include <gtk/gtk.h>

//BBtargets libscopiraui.so

using namespace scopira::tool;
using namespace scopira::coreui;

imagewidget::imagewidget(const char **xmp_data, widget *hostwidget)
{
  GdkPixmap *pix;
  GdkBitmap *mask;
  GdkWindow *win;

  // prepare host window
  if (hostwidget)
    win = hostwidget->get_widget()->window;
  else
    win = gdk_get_default_root_window();
  assert(win);

  pix = 0;
  mask = 0;

  pix = gdk_pixmap_create_from_xpm_d(win, &mask, 0, const_cast<char**>(xmp_data));
  assert(pix);
  assert(mask);
  g_object_ref(pix);
  g_object_ref(mask);

  dm_widget = gtk_image_new_from_pixmap(pix, mask);
  assert(dm_widget);

  widget::init_gui();

  // clean up... perhaps in the future, make a nice count_ptr like template class
  g_object_unref(pix);
  g_object_unref(mask);
}

imagewidget::imagewidget(const char *stockname, GtkIconSize sz)
{
  dm_widget = gtk_image_new_from_stock(stockname, sz);
  assert(dm_widget);
  widget::init_gui();
}

