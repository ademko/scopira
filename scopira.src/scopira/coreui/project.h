
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

#ifndef __INCLUDED_SCOPIRA_COREUI_PROJECT_H__X_
#define __INCLUDED_SCOPIRA_COREUI_PROJECT_H__X_

#include <scopira/core/project.h>
#include <scopira/coreui/window.h>
#include <scopira/coreui/viewmenu.h>
#include <scopira/coreui/filewindow.h>
// stuff needed for tree (seperate this to another .h file?
#include <scopira/core/view.h>
#include <scopira/core/model.h>
// helpful includes
#include <scopira/coreui/menu.h>
// for the reactro header
#include <scopira/coreui/viewwidget.h>
#include <scopira/coreui/export.h>

namespace scopira
{
  namespace coreui
  {
    class project_window_base;
    class project_tree_reactor_i;
    class project_tree;
  }
}

/**
 * A base class for implementing a project-based project window.
 *
 * Descendants must call init_gui.
 *
 * Deliberaly not a view.
 *
 * @author Aleksander Demko
 */ 
class scopira::coreui::project_window_base : public scopira::coreui::window
{
  public:
    /**
     * You can call this to force load a particular project from disk.
     * Returns true on success.
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT bool load_project(const std::string &filename);

  protected:
    /// ctor
    SCOPIRAUI_EXPORT project_window_base(const std::string &title, scopira::core::project_i *prj);

    /**
     * If you use the default file menu stuff, you must implement this.
     * If prj is null, you should created a empty one yourself.
     *
     * The function should return null on failure (if the particular prj is not
     * of the proper type).
     * The default imeplemenation simply returns null.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRAUI_EXPORT virtual project_window_base * spawn(scopira::core::project_i *prj);

    /**
     * You may calls this to add a stock menu to a menu you're building
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT void init_stock_menu(menu_builder &mb);

    /**
     * Descendants ctor must eventual call this.
     *
     * You should set (each optional, except dm_main_area)
     * before calling this.
     *
     * Future versions will have a toolbar and status bar option too.
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT void init_gui(widget *basewidget, widget *menu = 0);

  protected:

  private:
    std::string dm_filename;

    scopira::tool::count_ptr<scopira::core::project_i> dm_prj;

    scopira::tool::count_ptr<scopira::coreui::widget>
      dm_menu, dm_status_bar, dm_basewidget, dm_wholewidget;

    short dm_lastcmd;   // used in filedialog stuff

  private:
    enum {
      cmd_new_c,
      cmd_open_c,
      cmd_save_c,
      cmd_save_as_c,
      cmd_close_c,
    };
    static void h_menu(GtkWidget *widget, gpointer ptr);
    static bool h_filename(scopira::coreui::filewindow *source, const char *filename, void *data);
};

/**
 * Reactor to project tree events.
 *
 * @author Aleksander Demko
 */
class scopira::coreui::project_tree_reactor_i
{
  public:
    virtual ~project_tree_reactor_i() { }
    /**
     * Called when a new node is selected.
     *
     * selected_model may sometimes be null.
     *
     * @author Aleksander Demko
     */
    SCOPIRAUI_EXPORT virtual void react_project_tree_select(scopira::coreui::project_tree *source, scopira::core::model_i *selected_model) = 0;
};

/**
 * A tree viewer widget for projects.
 *
 * @author Aleksander Demko
 */ 
class scopira::coreui::project_tree :
  public virtual scopira::coreui::widget,
  protected virtual scopira::core::view_i,
  protected virtual scopira::tool::rename_reactor_i
{
  public:
    /**
     * Sets the current reactor. Null is valid (and is the default).
     *
     * @author Aleksander Demko
     */
    void set_project_tree_reactor(project_tree_reactor_i *reac) { dm_preac = reac; }

  private:
    // a node_t contains all the package that need to be associated
    // with a gtk tree node to refer to a scopira model_i
    class node_t : public scopira::tool::object
    {
      private:
        // the model_i, decomposed into more primative interfaces
        scopira::core::view_i *dm_master_inspector;
        rename_reactor_i *dm_master_rename;
      public:
        bool is_root;
        GtkTreeIter iter, parent_iter;    // parent_iter is only valid if is_root == false
        const scopira::tool::count_ptr<scopira::core::model_i> here;   // pointer to the node
        const scopira::tool::count_ptr<scopira::core::model_i> parent;   // pointer to the PARENT node (may be null)
      public:
        /// initing ctor
        node_t(scopira::core::view_i *masterI, scopira::tool::rename_reactor_i *masterR,
            GtkTreeIter _iter, GtkTreeIter _parent_iter,
            scopira::core::model_i *_here, scopira::core::model_i *_parent);
        /// no-parent (root node) ctor
        node_t(scopira::core::view_i *masterI, scopira::tool::rename_reactor_i *masterR,
            GtkTreeIter _iter,
            scopira::core::model_i *_here, scopira::core::model_i *_parent);
        /// dtor
        ~node_t();
    };


    typedef std::map<core::model_i*, tool::count_ptr<node_t> > recmap_t;

    enum {
      field_title_c = 0,
      field_ptr_c,
      field_n_c,
    };
    enum {
      but_dupe_c,
      but_del_c,
    };

    // the top level catalog and manager
    tool::count_ptr<scopira::core::project_i> dm_top;

    // the core widget
    GtkTreeStore *dm_data_store;
    GtkWidget *dm_treeview;

    // the association between records and their additional display data needed
    recmap_t dm_mapper;
    tool::count_ptr<node_t> dm_current;   // null for nothing selected

    scopira::coreui::view_filter_options dm_vopt;
    scopira::core::view_container_reactor_i *dm_reac;

    project_tree_reactor_i * dm_preac;

  public:
    /**
     * Constructor.
     *
     * prj will be the project to use, and may not be null.
     * reac is the optional (may be null) view_container_reactor_i to use
     * for instatiations.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRAUI_EXPORT project_tree(scopira::core::project_i *prj);
    
    /**
     * Sets the view filter options to use from now on.
     * 
     * The pm_show_vgmask will automatically have the vg_project_c bit added it.
     * If this is not desirable, we could perhaps move this operation to the caller's responsibility.
     *
     * reac may be null.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRAUI_EXPORT void set_view_filter_options(const scopira::coreui::view_filter_options &opt, scopira::core::view_container_reactor_i *reac);

  protected:
    SCOPIRAUI_EXPORT virtual void react_rename(object *source, const std::string &newname);

    SCOPIRAUI_EXPORT virtual void bind_model(scopira::core::model_i *sus);
    SCOPIRAUI_EXPORT virtual void react_model_update(scopira::core::model_i *sus, scopira::core::view_i *src);

  protected:
    SCOPIRAUI_EXPORT void init_gui(void);

    /// called on data change
    SCOPIRAUI_EXPORT void update_gui(void);

  private:
    /// sync the gui display via dm_mapper to the structure
    /// rec may be null (for the root node)
    void sync_level(GtkTreeIter *iter, core::model_i *rec);

    static void h_on_data_row(GtkTreeView *treeview, GtkTreePath *arg1, GtkTreeViewColumn *arg2, project_tree *here);
    static gboolean h_on_click(GtkWidget *widget, GdkEventButton *event, gpointer user_data);

    static void h_on_tree_selection_change( GtkTreeSelection *selection, project_tree *here );
};

#endif


