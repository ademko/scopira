
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

#ifndef __INCLUDED__SCOPIRA_COREUI_WIDGET_H__
#define __INCLUDED__SCOPIRA_COREUI_WIDGET_H__

#include <glib.h>
#include <gtk/gtkwidget.h>        

#include <scopira/tool/object.h>
#include <scopira/coreui/export.h>

namespace scopira
{
  namespace coreui
  {
    template <class T> class gtk_ptr;

    class widget;
  }
}

/**
 * A reference counting auto pointer of object and its decendants.
 * This template class calls g_object_ref() and g_object_unref() on its targets,
 * which is suitable for not only C-based gtk widgets, but also for glib stuff.
 *
 * This class has the following key characteristics:
 * 1) save, null initialization 2) always maintain one (and only one)
 * count to its given pointer
 *
 * @author Aleksander Demko
 */
template <class T> class scopira::coreui::gtk_ptr
{
  public:
    typedef T data_type;

  protected:
    T *dm_ptr;

  public:
    /**
     * Constructor, null initializing
     *
     * @author Aleksander Demko
     */
    gtk_ptr(void)
      : dm_ptr(0) { }
    /**
     * Initializing constructor
     *
     * @author Aleksander Demko
     */
    gtk_ptr(T *o)
      : dm_ptr(o) { if (o) g_object_ref(G_OBJECT(o)); }
    /**
     * Copy constructor
     *
     * @author Aleksander Demko
     */
    gtk_ptr(const gtk_ptr<T> &o)
      : dm_ptr(o.dm_ptr) { if (dm_ptr) g_object_ref(G_OBJECT(dm_ptr)); }
    /**
     * Destructor
     * @author Aleksander Demko
     */
    ~gtk_ptr()
      { if (dm_ptr) g_object_unref(G_OBJECT(dm_ptr)); }

    /**
     * Sets the current pointer. Any old pointer will ofcourse
     * be unreferenced.
     *
     * @author Aleksander Demko
     */
    void set(T* o)
      { if (o) g_object_ref(G_OBJECT(o)); if (dm_ptr) g_object_unref(G_OBJECT(dm_ptr)); dm_ptr = o; }
    /**
     * Assignment.
     *
     * @author Aleksander Demko
     */
    gtk_ptr & operator = (T *o)
      { set(o); return *this; }

    /**
     * Assignment.
     *
     * @author Aleksander Demko
     */
    gtk_ptr & operator = (const gtk_ptr<T> &o)
      { set(o.dm_ptr); return *this; }
    /**
     * Comparison (equals) - compares internal pointer values.
     *
     * @author Aleksander Demko
     */
    bool operator == (const gtk_ptr<T> &rhs) const
      { return dm_ptr == rhs.dm_ptr; }
    /**
     * Comparison (not equals) - compares internal pointer values.
     *
     * @author Aleksander Demko
     */
    bool operator != (const gtk_ptr<T> &rhs) const
      { return dm_ptr != rhs.dm_ptr; }
    /**
     * Comparison (less than) - compares internal pointer values.
     *
     * @author Aleksander Demko
     */
    bool operator < (const gtk_ptr<T> &rhs) const
      { return dm_ptr < rhs.dm_ptr; }

    // do save and load like the above?

    /**
     * Gets the current object
     *
     * @author Aleksander Demko
     */
    T* get(void) const
      { return dm_ptr; }
    //T* get(void)
      //{ return dm_ptr; }

    /**
     * Gets the current object, as a reference
     *
     * @author Aleksander Demko
     */
    T& ref(void) const
      { assert("[NULL pointer dereferenced in gtk_ptr]" && dm_ptr); return *dm_ptr; }
    //T& ref(void)
      //{ assert(dm_ptr); return *dm_ptr; }

    /**
     * Convinient -> accesor to the object
     *
     * @author Aleksander Demko
     */
    T* operator ->(void) const
      { assert("[NULL pointer dereferenced in gtk_ptr]" && dm_ptr); return dm_ptr; }
    //T* operator ->(void)
      //{ assert(dm_ptr); return dm_ptr; }

    /**
      * Convinient * deferencer.
      *
      * @author Aleksander Demko
      */
    T & operator *(void) const
      { assert("[NULL pointer dereferenced in gtk_ptr]" && dm_ptr); return *dm_ptr; }

    /**
     * Is the pointer null?
     *
     * @author Aleksander Demko
     */
    bool is_null(void) const { return dm_ptr == 0; }
};

/**
 *  base widget shared by all GTK widgets
 *
 *  @author Aleksander Demko
 */
class scopira::coreui::widget : public virtual scopira::tool::object
{
  protected:
    /// widget
    GtkWidget *dm_widget;
    /// focus indicator
    bool dm_isfocus;

  public:
    /**
     * Constructor that simply wraps and protects the given GtkWidget.
     * 
     * This constructor is only for users that use widget directly.
     *
     * Descendants of widget should use the protected constructor.
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT widget(GtkWidget *w);
    /// destructor
    SCOPIRAUI_EXPORT virtual ~widget();

    /// gets the raw widget
    ///  (make this virtual so decendants can change return?? - no, to many dm_widget refs)
    GtkWidget * get_widget(void) const { return dm_widget; }

    /// get focus indicator
    bool is_focus(void) const { return dm_isfocus; }
    /// is it visible?
    bool is_shown(void) const { return GTK_WIDGET_VISIBLE(dm_widget); }

    /// common C++/GTK+ object shadoing pattern helper
    /// set's the attached widget of the given gtk widget to this
    SCOPIRAUI_EXPORT void set_attach_gtk(GtkWidget *w);
    /// get the attached pointer, if any
    SCOPIRAUI_EXPORT static widget * get_attach_gtk(GtkWidget *w);

    /// calls gtk_widget_show_all on the widget
    SCOPIRAUI_EXPORT void show_all(void);

    /// calls gtk_hide on the widget (but not _all)
    SCOPIRAUI_EXPORT void hide(void);

    /// sets the minimum size, in pixels, of this widget
    SCOPIRAUI_EXPORT void set_min_size(int w, int h);
    /// sets the border width
    /// only valid for countainer based widgets (yeah, so why is it in this class, I know)
    SCOPIRAUI_EXPORT void set_border_size(int px);

    /// sets current cursor for this widget
    SCOPIRAUI_EXPORT void set_cursor(GdkCursorType type = GDK_LEFT_PTR);

    /// just to be nice
    void set_cursor_watch(void) { set_cursor(GDK_WATCH); }

    // to do arbitrary, by name attaching, use the gtk_object_set_data API directly

    /// in the gtk widgets data-table
    /// attaching null is allowed
    SCOPIRAUI_EXPORT static void set_attach_ptr(GtkWidget *w, void *item);
    /// get the attached pointer, if any
    SCOPIRAUI_EXPORT static void * get_attach_ptr(GtkWidget *w);
    /// convineince
    void set_attach_ptr(void *item) { set_attach_ptr(dm_widget, item); }
    /// convineince
    void * get_attach_ptr(void) const { return get_attach_ptr(dm_widget); }
    
    // sets the sensitivity of a widget, f the user can interact with it
    void set_sensitive(bool sensitive) {gtk_widget_set_sensitive(dm_widget,sensitive);}

    /// called when received change focus
    SCOPIRAUI_EXPORT virtual void on_focus(bool focus);

  protected:
    /**
     * Constructor for descendants.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRAUI_EXPORT widget(void);

    /**
     * Inits the gui.
     * Descendants must call this function after setting dm_widget
     * in their "gui initialization".
     * A descendant will typically do their "gui initialization"
     * in their own init_gui() method (which their ctor calls)
     * or right in their ctor.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRAUI_EXPORT void init_gui(void);

    /**
     * This version is similar to the no-param one, except that it will
     * set dm_widget for you, given the widget you supply.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRAUI_EXPORT void init_gui(GtkWidget *basewidget);

    /**
     * This variant is similar to the previous one, but it will
     * also maintain a count_ptr to the given class.
     * You cannot retrive this instance later, however. If you need to do
     * this, hold your own count_ptr and use the init_gui(GtkWidget*) variant.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRAUI_EXPORT void init_gui(widget *basewidget);

  private:
    /// static widget focus handler
    static void h_focus_in(GtkWidget *w, GdkEventFocus* event,gpointer data);
    // Static widget focus out handler
    static void h_focus_out(GtkWidget *w, GdkEventFocus* event,gpointer data);

  private:
    // yes, private
    scopira::tool::count_ptr<widget> dm_count_widget;
};

#endif

