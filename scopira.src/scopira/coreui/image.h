
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

#ifndef __INCLUDED__SCOPIRA_COREUI_IMAGE_H__
#define __INCLUDED__SCOPIRA_COREUI_IMAGE_H__

#include <scopira/coreui/widget.h>
#include <scopira/coreui/export.h>

namespace scopira
{
  namespace coreui
  {
    class imagewidget;

    // in the future:
    //  have a "image" that repsents a loaded (from file, xpm or stock) item
    //    that can be "drawn" onto canvas things
    //  an "image_store" that can cache/store/share a collection of images
    //    perhaps pegged to ui_loop
    //  rather than provide a imagewidget ctor for files, perhaps wait untill
    //    the store system is ready (so they can do shared reference counting, etc)
    //  BUT YEAH, need a reason to be doing random image-from-file into widget loading
  }
}

/**
 * one image
 *
 * @author Aleksander Demko
 */ 
class scopira::coreui::imagewidget : public scopira::coreui::widget
{
  protected:
    int dm_w, dm_h;
    
  public:
    /**
     * Constructrs an icon from XPM data.
     * hostwidget will be used for certain colour map defaults, if you dont provide
     * one, the root window will be used.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRAUI_EXPORT imagewidget(const char **xmp_data, widget *hostwidget = 0);

    /**
     * Creates an image from the stock set.
     *
     * See:
     * http://developer.gnome.org/doc/API/2.0/gtk/GtkImage.html#gtk-image-new-from-stock
     * For a list of stock names:
     * http://developer.gnome.org/doc/API/2.0/gtk/gtk-Stock-Items.html
     * For a list of things for sz:
     * http://developer.gnome.org/doc/API/2.0/gtk/gtk-Themeable-Stock-Images.html#GtkIconSize
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT imagewidget(const char *stockname, GtkIconSize sz = GTK_ICON_SIZE_BUTTON);

    /// returns image's the width
    int get_width(void) const { return dm_w; }
    /// returns image's the height
    int get_height(void) const { return dm_h; }
};

#endif

