
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

#include <scopira/coreui/project.h>

#include <scopira/tool/output.h>
#include <scopira/tool/platform.h>
#include <scopira/tool/util.h>
#include <scopira/tool/archiveflow.h>
#include <scopira/core/register.h>
#include <scopira/coreui/viewmenu.h>
#include <scopira/coreui/layout.h>

//BBtargets libscopiraui.so

using namespace scopira::tool;
using namespace scopira::core;
using namespace scopira::coreui;

//
//
// project_window_base
//
//

project_window_base::project_window_base(const std::string&title, scopira::core::project_i *prj)
  : window(title), dm_prj(prj)
{
}

bool project_window_base::load_project(const std::string &filename)
{
  // yeah, its hacky
  dm_lastcmd = cmd_open_c;
  h_filename(0, filename.c_str(), this);

  return true;    // make this proper in the future
}

project_window_base * project_window_base::spawn(scopira::core::project_i *prj)
{
  return 0;
}

void project_window_base::init_stock_menu(menu_builder &mb)
{
  mb.set_attach_widget(this);

  mb.push_menu("_Project");
  mb.add_stock_item("_New...", GTK_STOCK_NEW);
  mb.set_callback(G_CALLBACK(h_menu), int_to_void(cmd_new_c));
  mb.add_stock_item("_Open...", GTK_STOCK_OPEN);
  mb.set_callback(G_CALLBACK(h_menu), int_to_void(cmd_open_c));
  mb.add_stock_item("_Save", GTK_STOCK_SAVE);
  mb.set_callback(G_CALLBACK(h_menu), int_to_void(cmd_save_c));
  mb.add_stock_item("Save _As", GTK_STOCK_SAVE_AS);
  mb.set_callback(G_CALLBACK(h_menu), int_to_void(cmd_save_as_c));
  mb.add_separator();
  mb.add_stock_item("_Close", GTK_STOCK_CLOSE);
  mb.set_callback(G_CALLBACK(h_menu), int_to_void(cmd_close_c));
  mb.pop_menu();
}

void project_window_base::init_gui(widget *basewidget, widget *menu)
{
  box_layout *bl;

  dm_wholewidget = bl = new box_layout(false, false);

  if (menu) {
    dm_menu = menu;
    bl->add_widget(menu, false, false);
  }

  assert(basewidget);
  dm_basewidget = basewidget;
  bl->add_widget(basewidget, true, true);

  /*if (statusbar) {
    dm_status_bar = statusbar;
    bl->add_widget(statusbar, false, false);
  }*/

  dm_wholewidget->set_min_size(600, 500);

  window::init_gui(dm_wholewidget->get_widget());
}

void project_window_base::h_menu(GtkWidget *widget, gpointer ptr)
{
  project_window_base *here = dynamic_cast<project_window_base*>(menu_builder::get_attach_widget(widget));
  intptr_t cmd = ptr_to_int(ptr);

  assert(here);
  assert(here->is_alive_object());

  here->dm_lastcmd = static_cast<short>(cmd);

  switch (cmd) {
    case cmd_new_c:
      {
        project_window_base *newwin = here->spawn(0);
        assert(newwin && "[you need to implement spawn() for new to work]");
        newwin->show_all();
      }
      break;
    case cmd_open_c:
      {
        filewindow *fwin = new filewindow("Open Project", "*.prj",cmd_open_file);
        fwin->set_filename_reactor(h_filename, here);
        fwin->show_all();
      }
      break;
    case cmd_save_c:
      if (!here->dm_filename.empty()) {
        // just do the save
        h_filename(0, here->dm_prj->get_title().c_str(), here);
        break;
      }//else, fall through and do a save ass...
    case cmd_save_as_c:
      {
        filewindow *fwin = new filewindow("Save Project As", "*.prj",cmd_save_file);
        fwin->set_filename_reactor(h_filename, here);
        fwin->show_all();
      }
      break;
    case cmd_close_c:
      here->on_destroy();
      break;
  }
}

bool project_window_base::h_filename(scopira::coreui::filewindow *source, const char *filename, void *data)
{
  project_window_base *here = reinterpret_cast<project_window_base*>(data);

  assert(here && here->is_alive_object());

  switch (here->dm_lastcmd) {
    case cmd_open_c:
      {
        count_ptr<project_i> newprj;

        // lets save the current project
        archiveiflow innie;

        innie.open(filename, "SCOPROJ200");
        if (innie.failed())
          return false;

        if (!innie.read_object_type(newprj))
          return false;

        if (here->dm_prj->is_project_empty()) {
          // just use the existing window/project
          here->dm_prj->move_project(newprj.get());
          here->dm_filename = filename;
          here->dm_prj->notify_views(0);
          here->dm_prj->set_title(filename);
          here->set_title(filename);
        } else {
          // create a new window instance
          project_window_base *newwin = here->spawn(newprj.get());

          newwin->dm_filename = filename;

          if (newprj.is_null())
            return false;     // must be a failed data type then (or they didnt implement the spawn function)

          newwin->show_all();

          newwin->set_title(filename);
          newprj->set_title(filename);
        }
        return true;
      }
      break;
    case cmd_save_c:
    case cmd_save_as_c:
      {
        // lets save the current project
        archiveoflow out;

        out.open(filename, "SCOPROJ200");
        if (out.failed())
          return false;

        here->dm_prj->set_title(filename);

        out.write_object_type(here->dm_prj);
        here->dm_filename = filename;
        return true;
      }
      break;
  }

  return false;
}

//
//
// project_tree
//
//

/**
 * An internal node that is associated with each model_i shown in the tree view.
 * This thing contains some important stuff like the GtkTreeIter stuff needed
 * for tree view manipulation
 * @author Aleksander Demko
 */ 
project_tree::node_t::node_t(
  view_i *masterI, rename_reactor_i *masterR,
  GtkTreeIter _iter, GtkTreeIter _parent_iter,
  model_i *_here, model_i *_parent)
  : dm_master_inspector(masterI), dm_master_rename(masterR),
    iter(_iter), parent_iter(_parent_iter),
    here(_here), parent(_parent)
{
  is_root = false;

  assert(dm_master_inspector);
  assert(dm_master_rename);
  assert(here.get());

  here->add_view(dm_master_inspector);
  here->add_rename_reactor(dm_master_rename);
}

project_tree::node_t::node_t(
  view_i *masterI, rename_reactor_i *masterR,
  GtkTreeIter _iter,
  model_i *_here, model_i *_parent)
  : dm_master_inspector(masterI), dm_master_rename(masterR),
    iter(_iter),
    here(_here), parent(_parent)
{
  is_root = true;

  assert(dm_master_inspector);
  assert(dm_master_rename);
  assert(here.get());

  here->add_view(dm_master_inspector);
  here->add_rename_reactor(dm_master_rename);
}

project_tree::node_t::~node_t()
{
  here->remove_view(dm_master_inspector);
  here->remove_rename_reactor(dm_master_rename);
}

//
// back to project_tree
//

project_tree::project_tree(scopira::core::project_i *prj)
  : dm_reac(0), dm_preac(0)
{
  init_gui();
  update_gui();
  //update_buttons();

  assert(prj);

  // bind myself to the project
  // do an update too?
  bind_model(prj);
}

void project_tree::set_view_filter_options(const scopira::coreui::view_filter_options &opt, scopira::core::view_container_reactor_i *reac)
{
  dm_vopt = opt;
  dm_reac = reac;
}

void project_tree::react_rename(object *source, const std::string &newname)
{
  model_i *rec = dynamic_cast<model_i*>(source);
  node_t *n;

  if (!rec)
    return;

  assert(dm_mapper.count(rec)>0);
  n = dm_mapper[rec].get();

  // update the gui
  gtk_tree_store_set(dm_data_store, &n->iter, field_title_c, newname.c_str(), -1);
}

void project_tree::bind_model(scopira::core::model_i *sus)
{
  dm_top = dynamic_cast<project_i*>(sus);
  update_gui();
}

void project_tree::react_model_update(scopira::core::model_i *sus, scopira::core::view_i *src)
{
  // with this new catalogII system, i only really care about inspect_model() events
  // to the root project_i: dm_top

  if (dm_top.is_null() || sus != dm_top.get())
    return;

  assert(dm_mapper.count(dm_top.get())>0);
  assert(dm_mapper[dm_top.get()].get());
  sync_level(&dm_mapper[dm_top.get()]->iter, 0);

  // expand the view to show it
  gtk_tree_view_expand_all(GTK_TREE_VIEW(dm_treeview));
}

/*void project_tree::declare_models(declare_models_i &dsi)
{
  if (dm_top.is_null() || dm_current.is_null())
    return;

  if (dm_current->is_root)
    dsi.declare_model("project", dm_current->here.get(), declare_models_i::exact_match_c | declare_models_i::exclude_primary_c);
  else
    dsi.declare_model("selected data", dm_current->here.get());
}*/

void project_tree::init_gui(void)
{
  GtkWidget *tree, *scroller;
  GtkTreeViewColumn *col;

  dm_data_store = gtk_tree_store_new(field_n_c, G_TYPE_STRING, G_TYPE_POINTER);

  tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(dm_data_store));
  dm_treeview = tree;
  gtk_tree_view_expand_all(GTK_TREE_VIEW(tree));

  GtkTreeSelection *select;
  select = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree));
  gtk_tree_selection_set_mode(select, GTK_SELECTION_SINGLE);
  g_signal_connect(G_OBJECT(select), "changed", G_CALLBACK(h_on_tree_selection_change), this);

//  g_signal_connect(G_OBJECT(tree), "row-activated", G_CALLBACK(h_on_data_row), this);
  g_signal_connect(G_OBJECT(tree), "button-press-event", G_CALLBACK(h_on_click), this);
  col = gtk_tree_view_column_new_with_attributes("Data Title", gtk_cell_renderer_text_new(),
      "text", field_title_c, static_cast<void*>(0));
  gtk_tree_view_append_column(GTK_TREE_VIEW(tree), col);

  scroller = gtk_scrolled_window_new(0, 0);
  gtk_widget_set_size_request(scroller, 100, 300);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroller), GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
  gtk_container_add(GTK_CONTAINER(scroller), tree);

  widget::init_gui(scroller);
}

void project_tree::update_gui(void)
{
  dm_current = 0;
  gtk_tree_store_clear(dm_data_store);
  dm_mapper.clear();

  if (dm_top.get()) {
    //sub_data_insert(0, 0, dm_top.get());
    //add in the root node
    GtkTreeIter ii;
    core::model_iterator ri;

    gtk_tree_store_append(dm_data_store, &ii, 0);
    // we need the static cast cuz we need EXACTLY as model_i in the *C* based handler
    gtk_tree_store_set(dm_data_store, &ii, field_title_c, dm_top->get_title().c_str(), 
        field_ptr_c, static_cast<model_i*>(dm_top.get()), -1);
    dm_mapper[dm_top.get()] = new node_t(this, this, ii, dm_top.get(), 0);

    if (dm_current.is_null()) {
      dm_current = dm_mapper[dm_top.get()];
      //update_buttons();
    }

    sync_level(&ii, 0);

    // expand the view to show it
    gtk_tree_view_expand_all(GTK_TREE_VIEW(dm_treeview));
  }
}

void project_tree::sync_level(GtkTreeIter *iter, scopira::core::model_i *rec)
{
  bool nukeone, nuketotal;
  recmap_t::iterator ii, endii;

  // lets see if any new children where added
  for (model_iterator ri = dm_top->get_model_iterator(rec); ri.valid(); ++ri) {
    if (dm_mapper.find(*ri) == dm_mapper.end()) {
      // its new! lets add it
      GtkTreeIter new_iter;
      core::model_i *new_rec;

      new_rec = *ri;

      gtk_tree_store_append(dm_data_store, &new_iter, iter);
      gtk_tree_store_set(dm_data_store, &new_iter, field_title_c, new_rec->get_title().c_str(), 
          field_ptr_c, new_rec, -1);
      dm_mapper[new_rec] = new node_t(this, this, new_iter, new_iter, new_rec, rec);
    }

    // new and existing nodes need to be resynced
    sync_level(&dm_mapper[*ri]->iter, *ri);
  }//for

  nuketotal = false;

  // we know rec is valid, so lets just prune all the nodes from the gtktree
  // that are no longer valid.
  // this may leave dangling dm_mapper entres, but the last section
  // of this function will take care of that
  do {
    // now lets find all the children that were nuked
    nukeone = false;
  
    endii = dm_mapper.end();
    for (ii = dm_mapper.begin(); ii != endii; ++ii)
      if (!ii->second->is_root && ii->second->parent.get() == rec && !dm_top->contains_model(rec, ii->first)) {
        GtkTreeIter iter = ii->second->iter;
        // nuke it from the gui
        gtk_tree_store_remove(dm_data_store, &iter);

        if (ii->second == dm_current)
          dm_current = 0;

        // remove it from the mapper
        // this may leave danglers, but again, the last section will cover this
        assert(ii->first != dm_top.get());
        dm_mapper.erase(ii);

        nukeone = true;
        nuketotal = true;
        break;
      }

  } while (nukeone);

  // did we nuke anything? if so, lets search for any danling dm_mapper
  // enties.
  // lets only do this during the root pass, since we dont need to do this per level
  if (nuketotal && rec == 0)
    do {
      nukeone = false;

      endii = dm_mapper.end();
      for (ii = dm_mapper.begin(); ii != endii; ++ii)
        if (!ii->second->is_root && ii->second->is_root && !dm_top->contains_model(ii->first)) {
          if (ii->second == dm_current)
            dm_current = 0;

          // chaning the iterator, lets restart it!
          assert(ii->first != dm_top.get());
          dm_mapper.erase(ii);

          nukeone = true;
          break;
        }
    } while (nukeone);
}

void project_tree::h_on_data_row(GtkTreeView *treeview, GtkTreePath *arg1, GtkTreeViewColumn *arg2, project_tree *here)
{
  GtkTreeIter ii;
  model_i *r;

  assert(arg1);
  assert(here->is_alive_object());

  gtk_tree_model_get_iter(GTK_TREE_MODEL(here->dm_data_store), &ii, arg1);
  gtk_tree_model_get(GTK_TREE_MODEL(here->dm_data_store), &ii, field_ptr_c, &r, -1);

  assert(r->is_alive_object());
  assert(here->dm_mapper.count(r)>0);

  here->dm_current = here->dm_mapper[r];

  if (here->dm_preac)
    here->dm_preac->react_project_tree_select(here, here->dm_current.is_null() ? 0 : here->dm_current->here.get());
}

gboolean project_tree::h_on_click(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
  project_tree * here = reinterpret_cast<project_tree*>(user_data);

  assert(here->is_alive_object());

  if (event->button == 3) {
    menu_pop_builder mb;

    if (here->dm_current.get()) {
      view_filter_options opts = here->dm_vopt;

      if (here->dm_current->here.get() != here->dm_top.get())
        opts.pm_show_vgmask |= scopira::core::vg_needs_project_c | vg_needs_copyfunc_c;

      build_view_menu(here->dm_current->here.get(), mb, opts, here->dm_reac);
    } else {
      mb.add_item("(nothing selected)");
      mb.set_enable(false);
    }

    mb.popup();

    return TRUE;
  }

  return FALSE;
}

void project_tree::h_on_tree_selection_change( GtkTreeSelection *selection, project_tree *here )
{
  assert( here->is_alive_object() );

  GtkTreeIter   iter; 
  GtkTreeModel  *model = GTK_TREE_MODEL(here->dm_data_store);
  
  if (gtk_tree_selection_get_selected(selection, &model, &iter))
  {
    model_i *mdl;
    
    gtk_tree_model_get( model, &iter, field_ptr_c, &mdl, -1 );

    assert( mdl->is_alive_object() );
    assert( here->dm_mapper.count(mdl) > 0 );

    here->dm_current = here->dm_mapper[mdl];

    if ( here->dm_preac )
      here->dm_preac->react_project_tree_select(here, here->dm_current.is_null() ? 0 : here->dm_current->here.get());
  }
}
