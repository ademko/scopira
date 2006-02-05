
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

#ifndef __INCLUDED_SCOPIRA_COREUI_MENU_H__
#define __INCLUDED_SCOPIRA_COREUI_MENU_H__

#include <assert.h>

#include <string>

#include <scopira/tool/object.h>
#include <scopira/tool/array.h>
#include <scopira/coreui/widget.h>
#include <scopira/coreui/export.h>

#include <gtk/gtk.h>

/*
   Callbacks look like this:
     void h_stat_menu(GtkWidget *widget, gpointer blah)

   You probably want to use set_attach_gtk(item) too, so you may pass
   both a cmd id and attached object to the callback

   G_CALLBACK() should probably be wrapped around your function pointer.
*/

namespace scopira
{
  namespace coreui
  {
    class widget;  //fwd

    /**
     * Copies src to dest, but insets an extra underscore
     * after every underscore found.
     * @author Aleksander Demko
     */ 
    SCOPIRAUI_EXPORT void escape_underscores(const std::string &src, std::string &dest);
    /**
     * Returns an underscore scaped string
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT std::string escape_underscores(const std::string &src);

    /**
      * base class for menu builders
      * @author Aleksander Demko
      */ 
    class menu_builder
    {
      protected:
        scopira::tool::fixed_array<GtkWidget*, 30> dm_stack;
        int dm_cur;
        GtkWidget * dm_last;
        widget *dm_attach;
        void *dm_attach_ptr;

      public:
        /// ctor
        SCOPIRAUI_EXPORT menu_builder(GtkWidget *base);
        /// dtor
        SCOPIRAUI_EXPORT ~menu_builder();

        /// gets the raw widget, top level widget
        GtkWidget * get_gtk_widget(void) const { return dm_stack[0]; }

        /// gets the raw widget, top level widget
        widget * get_widget(void) const { return new widget(get_gtk_widget()); }

        /**
          * Sets the current attaching widget.
          * All future add_items will have this widget attached.
          *
          * @author Aleksander Demko
          */ 
        void set_attach_widget(widget *w) { dm_attach = w; }

        /**
         * Gets the arrached widget type for a particular GtkWidget.
         * Useful in menu handlers
         * @author Aleksander Demko
         */
        static widget * get_attach_widget(GtkWidget *w) { return widget::get_attach_gtk(w); }

        /**
         * Sets the generic void pointer to attach to all futured added items.
         *
         * @author Aleksander Demko
         */
        void set_attach_ptr(void *ptr) { dm_attach_ptr = ptr; }

        /**
          * Adds a new menu item. Right after this call, you may call
          * the various set_ methods.
          * Automatically does underscore escaping.
          * @author Aleksander Demko
          */
        SCOPIRAUI_EXPORT GtkWidget * add_item(const std::string &label);

        /**
         * Adds a "stock" menu item.
         *
         * See:
         * http://developer.gnome.org/doc/API/2.0/gtk/gtk-Stock-Items.html
         * for stockid things.
         *
         * @author Aleksander Demko
         */
        SCOPIRAUI_EXPORT GtkWidget *add_stock_item(const std::string &label, const char *stockid);
        
        /**
          * Adds a new check menu item
          * Automatically does underscore escaping.
          * @author Shantha Ramachandran
          */
        SCOPIRAUI_EXPORT GtkWidget * add_check_item(const std::string &label, bool checked);

        /**
          * sets the call back handler for the LAST item
          * default is none
          * @author Aleksander Demko
          */ 
        SCOPIRAUI_EXPORT void set_callback(GCallback callback, gpointer data = 0);

        /**
          * sets the sensitivity on the LAST item
          * default is true (enabled)
          * @author Aleksander Demko
          */ 
        SCOPIRAUI_EXPORT void set_enable(bool enab);

        /**
          * adds a seperator
          * @author Aleksander Demko
          */ 
        SCOPIRAUI_EXPORT GtkWidget * add_separator(void);

        /**
          * pushes a new sub menu item onto the stack. the new item is returned
          * Automatically does underscore escaping.
          * @author Aleksander Demko
          */
        SCOPIRAUI_EXPORT GtkWidget * push_menu(const std::string &label);

        /**
          * pops a menu off the stack
          * @author Aleksander Demko
          */ 
        SCOPIRAUI_EXPORT void pop_menu(void);

      protected:
        /// appends w to currunt menu on the stack
        SCOPIRAUI_EXPORT void append_and_show(GtkWidget *menu_item);
    };

    /**
      * menu bar menu builder
      * @author Aleksander Demko
      */ 
    class menu_bar_builder : public menu_builder
    {
      private:
        GtkWidget *dm_bar;
      public:
        /// ctor
        SCOPIRAUI_EXPORT menu_bar_builder(void);
        /// dtor
        SCOPIRAUI_EXPORT ~menu_bar_builder();
    };

    /**
     * a popup menu builder
     * @author Aleksander Demko
     */ 
    class menu_pop_builder : public menu_builder
    {
      private:
        GtkWidget *dm_pop;
      public:
        /// ctor
        SCOPIRAUI_EXPORT menu_pop_builder(void);
        /// dtor
        SCOPIRAUI_EXPORT ~menu_pop_builder();

        /**
         * pops the menu up.
         * mousebut may be the mouse button number, or -1 for "other", or 0 1 2 for mouse button
         * @author Aleksander Demko
         */ 
        SCOPIRAUI_EXPORT void popup(int mousebut = -1);
    };

    /**
     * A sub menu builder (useful when you have other menus or something)
     * @author Aleksander Demko
     */
    class menu_sub_builder : public menu_builder
    {
      private:
        GtkWidget *dm_sub, *dm_item;
      public:
        /// ctor
        SCOPIRAUI_EXPORT menu_sub_builder(const std::string &label);
        /// dtor
        SCOPIRAUI_EXPORT ~menu_sub_builder();

        /**
         * Gets the final menu object.
         * You must call this once, and only after you've built
         * your menu.
         * @author Aleksander Demko
         */
        SCOPIRAUI_EXPORT void commit_sub_menu(GtkWidget *menu);
    };
  }
}

#endif

