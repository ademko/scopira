

/*
 *  Copyright (c) 2001-2006    National Research Council
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
#include <scopira/tool/output.h>
#include <scopira/coreui/export.h>
#include <scopira/coreui/window.h>
#include <scopira/coreui/entry.h>

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
      cmd_open_file_c,
      cmd_save_file_c,
      cmd_open_dir_c,
      cmd_save_dir_c,
    };
  }
}

/**
 * A window for selecting files.
 *
 * @author Aleksander Demko
 */
class scopira::coreui::filewindow : public scopira::coreui::window
{
  public:
    /**
     * Simply constructor that lets you add a filter.
     * type is one of cmd_*_c constants
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT filewindow(const std::string &title, int type=cmd_open_file_c);
    /**
     * Simply constructor that uses no filters.
     * filtername is some kind of nice name of your filter (file type)
     * filterpattern is the shell glob/file pattern of your file type ("*.prj" for example)
     * type is one of cmd_*_c constants
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT filewindow(const std::string &title, const std::string &filtername, const std::string &filterpattern, int type);

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

    /// sets a event reactor
    SCOPIRAUI_EXPORT void set_filename_reactor(filewindow_reactor_t r, void *data = 0);

  protected:
    SCOPIRAUI_EXPORT void init_gui(const std::string &patternname, const std::string &pattern, int type);
    SCOPIRAUI_EXPORT void ok(void);

  private:
    filewindow_reactor_t dm_reactor;
    void *dm_reactor_payload;

  private:
    static void h_response(GtkWidget *widget, gint res, gpointer data);
};

/**
 * A combination entry box and button for choosing files or directories.
 * The button, when pressed, will open up the filewindow to graphically
 * select files.
 *
 * @author Aleksander Demko
 */
class scopira::coreui::fileentry : public scopira::coreui::widget,
    public virtual scopira::tool::destroy_reactor_i
{
  private:
    GtkWidget *dm_entry;      // the entry field
    scopira::coreui::filewindow *dm_filewin;    /// CANNOT be ref counted, since we listen for destroy events
    std::string dm_file_name_filter;
    int dm_file_action;

  public:
    /// ctor
    SCOPIRAUI_EXPORT fileentry(void);
    /// dtor
    SCOPIRAUI_EXPORT virtual ~fileentry();

    SCOPIRAUI_EXPORT virtual void react_destroy(scopira::tool::object *source);

    /**
     * This sets the config name to use. If set, the widget will utilize
     * scopira::core::basic_loop's config facilities to set and
     * remember file names.
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT void set_config_key(const std::string &configkey);

    /// sets the currently entered filename
    SCOPIRAUI_EXPORT void set_filename(const std::string &filename);

    /// sets the action, or type, of file selection this is (use one of the cmd_*_c constants)
    void set_action(int act) { dm_file_action = act; }
    /// sets the shell glob filter for the files ("*" for example)
    void set_filter( const std::string &str ) { dm_file_name_filter = str; }

    /// gets the currently entered filename
    SCOPIRAUI_EXPORT std::string get_filename(void) const;

  private:
    void init_gui(void);

  private:
    std::string dm_configkey;

  private:
    static void h_browse(GtkWidget *widget, gpointer data);
    static void h_entry(GtkEntry *e, gpointer data);
    static bool h_filewindow_ok(scopira::coreui::filewindow *source, const char *filename, void *data);
};

#endif

