

/*
 *  Copyright (c) 2001    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#ifndef __INCLUDED__SCOPIRA_COREUI_FILEWINDOW__
#define __INCLUDED__SCOPIRA_COREUI_FILEWINDOW__

#include <scopira/tool/reactor.h>
#include <scopira/coreui/window.h>
#include <scopira/coreui/export.h>
#include <scopira/tool/output.h>

namespace scopira
{
  namespace coreui
  {
    class filewindow;
    class fileentry;

    /**
     * The reactor type.
     * It should return true on success..
     *
     * @author Aleksander Demko
     */ 
    typedef bool (*filewindow_reactor_t)(scopira::coreui::filewindow *source, const char *filename, void *data);
    
    enum {
      cmd_open_file,
      cmd_save_file,
    };
  }
}

/**
 * a window wrapper around gtkfileselection
 *
 * @author Aleksander Demko
 */
class scopira::coreui::filewindow : public scopira::coreui::window
{
  public:
    /// default constructor
    SCOPIRAUI_EXPORT filewindow(const std::string &title, const std::string &pattern, int type=cmd_open_file);
    
    /**
     * Gets the arrached widget type for a particular GtkWidget.
     * Useful in handlers, since the filewindow is always attached
     * to its GtkWidgets in the hanlders.
     *
     * @author Aleksander Demko
     */
    static filewindow * get_attach_filewindow(GtkWidget *w) { return dynamic_cast<filewindow*>(widget::get_attach_gtk(w)); }

    /// sets the full dir/file name
    SCOPIRAUI_EXPORT void set_filename(const std::string &filename);
    /// gets the full dir/file name
    SCOPIRAUI_EXPORT std::string get_filename(void) const;

    SCOPIRAUI_EXPORT void set_filename_reactor(filewindow_reactor_t r, void *data = 0);

  protected:
    SCOPIRAUI_EXPORT void init_gui(const std::string &pattern, int type);
    SCOPIRAUI_EXPORT void ok(void);
    
  private:
    filewindow_reactor_t dm_reactor;
    void *dm_reactor_payload;

  private:
    static void h_response(GtkWidget *widget, gint res, gpointer data);
};

/**
 * a entry/file window button combo
 *
 * @author Aleksander Demko
 */
class scopira::coreui::fileentry : public scopira::coreui::widget,
    public virtual scopira::tool::destroy_reactor_i
{
  private:
    GtkWidget *dm_entry;      // the entry field
    scopira::coreui::filewindow *dm_filewin;    /// CANNOT be ref counted, since we listen for destroy events

  public:
    /// ctor
    SCOPIRAUI_EXPORT fileentry(void);
    /// dtor
    SCOPIRAUI_EXPORT virtual ~fileentry();

    SCOPIRAUI_EXPORT virtual void react_destroy(scopira::tool::object *source);

    /// sets the filename
    SCOPIRAUI_EXPORT void set_filename(const std::string &filename);
    /// gets the filename
    SCOPIRAUI_EXPORT std::string get_filename(void) const;

  private:
    void init_gui(void);

  private:
    static void h_browse(GtkWidget *widget, gpointer data);
    static bool h_filewindow_ok(scopira::coreui::filewindow *source, const char *filename, void *data);
};

#endif

