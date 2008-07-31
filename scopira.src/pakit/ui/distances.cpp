
/*
 *  Copyright (c) 2004    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#include <pakit/ui/distances.h>

#include <scopira/core/register.h>
#include <scopira/basekit/vectormath.h>
#include <scopira/basekit/color.h>
#include <scopira/coreui/label.h>
#include <scopira/coreui/viewmenu.h>
#include <scopira/coreui/messagewindow.h>
#include <pakit/rdp.h>
#include <pakit/util.h>

//BBlibs scopiraui
//BBtargets libpauikit.so

using namespace scopira::tool;
using namespace scopira::coreui;
using namespace scopira::basekit;
using namespace scopira::uikit;
using namespace pakit;
using namespace std;

//
//
// register
//
//

static scopira::core::register_view<calc_distance_via_euclidean_v> r1(
    "pakit::calc_distance_via_euclidean_v", "pakit::patterns_m", "Calc Distance/Euclidean",
    scopira::core::no_uimode_c);
static scopira::core::register_view<calc_distance_via_cityblock_v> r2(
    "pakit::calc_distance_via_cityblock_v", "pakit::patterns_m", "Calc Distance/City Block",
    scopira::core::no_uimode_c);
static scopira::core::register_view<calc_distance_via_max_distance_v> r3(
    "pakit::calc_distance_via_max_distance_v", "pakit::patterns_m", "Calc Distance/Max Distance",
    scopira::core::no_uimode_c);
static scopira::core::register_view<calc_distance_via_ab_v> r4(
    "pakit::calc_distance_via_ab_v", "pakit::patterns_m", "Calc Distance/AB Distance", 
    scopira::core::windowonly_uimode_c);
static scopira::core::register_view<calc_distance_via_chernoff_v> r5(
    "pakit::calc_distance_via_chernoff_v", "pakit::patterns_m", "Calc Distance/Chernoff Distance", 
    scopira::core::windowonly_uimode_c);
static scopira::core::register_view<calc_distance_via_sym_kl_divergence_v> r6(
    "pakit::calc_distance_via_sym_kl_divergence_v", "pakit::patterns_m", "Calc Distance/Symmetric KL Divergence Distance", 
    scopira::core::windowonly_uimode_c);
static scopira::core::register_view<calc_distance_via_patrick_fisher_v> r7(
    "pakit::calc_distance_via_patrick_fisher_v", "pakit::patterns_m", "Calc Distance/Patrick-Fisher Distance", 
    scopira::core::windowonly_uimode_c);

//
// SIMPLE distances - no intervening covariance matrices etc.
//

/**
 * Base class holding commonality amongst the simple distance functions
 * Descendants need only override calc_distance_matrix_simple and register their view.
 *
 * This is a gui-less view base class.
 *
 * @author Aleksander Demko
 */

void calc_simple_distance_via_gui_base::bind_model(scopira::core::model_i *sus)
{
  if (!sus)
    return;

  patterns_m *pat;
  count_ptr<distances_m> output;
  std::string desc;

  pat = dynamic_cast<patterns_m*>(sus);
  assert(pat);

  output = new distances_m;
  output->pm_array = new narray_o<double,2>;

  // do the calculation via the virtual method
  desc = calc_distance_matrix_simple(pat->pm_data->all_slice(), output->pm_array.ref());

  output->pm_patterns_link = pat;
  output->set_title("Distance Matrix (" + desc + ")");
  output->notify_views(this);

  // add it to the catalog, if any
  if (pat->get_project()) {
    pat->get_project()->add_model(pat, output.get());
    pat->get_project()->notify_views(this);
  }
}

std::string calc_distance_via_euclidean_v::calc_distance_matrix_simple(const scopira::basekit::nslice<double,2> &patterns,
      scopira::basekit::narray<double,2> &outmatrix)
{
  pakit::calc_distance_matrix_simple(patterns, outmatrix, euclidean_distance);
  return "Euclidean";
}

std::string calc_distance_via_cityblock_v::calc_distance_matrix_simple(const scopira::basekit::nslice<double,2> &patterns,
      scopira::basekit::narray<double,2> &outmatrix)
{
  pakit::calc_distance_matrix_simple(patterns, outmatrix, cityblock_distance);
  return "City Block";
}

std::string calc_distance_via_max_distance_v::calc_distance_matrix_simple(const scopira::basekit::nslice<double,2> &patterns,
      scopira::basekit::narray<double,2> &outmatrix)
{
  pakit::calc_distance_matrix_simple(patterns, outmatrix, max_distance);
  return "Max Distance";
}

//
// COMPOUND distance functions require two classes of interest, usually for a
// covariance matrix calculation
//

/**
 * Base class holding commonality amongst the compound distance functions
 * @author Brion Dolenko
 */

calc_compound_distance_via_gui_base::calc_compound_distance_via_gui_base(void)
  : dm_model(this)
{
  dm_model = new patterns_m;
  dm_distance = new distances_m;
  dm_distance->pm_array = new narray_o<double,2>;
  dm_distance->pm_patterns_link = dm_model.get();
}

void calc_compound_distance_via_gui_base::bind_model(scopira::core::model_i *sus)
{
  if (!sus)
    return;

  dm_model = dynamic_cast<patterns_m*>(sus);
  dm_distance = new distances_m;
  dm_distance->pm_array = new narray_o<double,2>;
  dm_distance->pm_patterns_link = dm_model.get();
}

void calc_compound_distance_via_gui_base::init_gui(void)
{
  dm_g = new grid_layout(2, 3);

  dm_class1_entry = new entry;
  dm_g->add_widget(new label("Class 1 number:"), 0, 0);
  dm_g->add_widget(dm_class1_entry.get(), 1, 0);
  dm_class1_entry->set_text("1");

  dm_class2_entry = new entry;
  dm_g->add_widget(new label("Class 2 number:"), 0, 1);
  dm_g->add_widget(dm_class2_entry.get(), 1, 1);
  dm_class2_entry->set_text("2");
}

void calc_compound_distance_via_gui_base::react_button(scopira::coreui::button *source, int actionid)
{
  viewwidget::react_button(source, actionid);
}

void calc_compound_distance_via_gui_base::react_button(compound_distance_func_t func,
                                                       double class_weight)
{
  int cl1, cl2;
  string msg;
  bool success;

  cl1 = string_to_int(dm_class1_entry->get_text());
  cl2 = string_to_int(dm_class2_entry->get_text());

  success = func(dm_model.get(),dm_distance.get(), cl1,cl2, class_weight,msg);
  
  if (!success) {
    messagewindow::popup_error(msg);
    return;
  }
  
  dm_distance->set_title(msg);
  dm_distance->notify_views(this);

  // add it to the catalog, if any
  if (dm_model->get_project()) {
    dm_model->get_project()->add_model(dm_model.get(), dm_distance.get());
    dm_model->get_project()->notify_views(this);
  }
}

/*
 * Anderson-Bahadur distance GUI
 *
 * Uses a mixture of the covariance matrices of the two user-specified classes.
 * Mixture controlled through the "class_weight" parameter
 *
 * @author Brion Dolenko
 */

calc_distance_via_ab_v::calc_distance_via_ab_v(void)
{
  init_gui();
}

void calc_distance_via_ab_v::bind_model(scopira::core::model_i *sus)
{
  if (!sus)
    return;

  calc_compound_distance_via_gui_base::bind_model(sus);
}

/*
void calc_distance_via_ab_v::react_button(scopira::coreui::button *source, int actionid)
{
  int cl1, cl2;
  double class_weight; 
  string msg;
  bool success;

  if (actionid == action_apply_c) {
    cl1 = string_to_int(dm_class1_entry->get_text());
    cl2 = string_to_int(dm_class2_entry->get_text());
    class_weight = dm_cl_wt_slider->get_value();

    success = calc_abdistance(dm_model.get(),dm_distance.get(), cl1,cl2, class_weight,msg);
    
    if (!success) {
      messagewindow::popup_error(msg);
      return;
    }
    
    dm_distance->set_title(msg);
    dm_distance->notify_views(this);

    // add it to the catalog, if any
    if (dm_model->get_project()) {
      dm_model->get_project()->add_model(dm_model.get(), dm_distance.get());
      dm_model->get_project()->notify_views(this);
    }
  }

  viewwidget::react_button(source, actionid);
}
*/
void calc_distance_via_ab_v::react_button(scopira::coreui::button *source, int actionid)
{
  double class_weight;

  if (actionid == action_apply_c) {
    class_weight = dm_cl_wt_slider->get_value();
    calc_compound_distance_via_gui_base::react_button( calc_ab_distance, class_weight );
  }

  viewwidget::react_button(source, actionid);
}

void calc_distance_via_ab_v::init_gui(void)
{
  calc_compound_distance_via_gui_base::init_gui();

  dm_cl_wt_slider = new slider(0.0, 1.0, 0.01, 2, 0, false);
  dm_g->add_widget(new label("Class weight:"), 0, 2);
  dm_g->add_widget(dm_cl_wt_slider.get(), 1, 2);
  dm_cl_wt_slider->show_min_max();
  dm_cl_wt_slider->set_value(0.5);
  dm_cl_wt_slider->set_digits(2);

  viewwidget::init_gui(dm_g.get(), button_ok_c|button_cancel_c);
  set_view_title("AB Distance");
}

/*
 * Chernoff distance GUI class
 *
 * Uses a mixture of the covariance matrices of the two user-specified classes.
 * Also a log term involving determinants of the covariance matrices is added.
 * Mixture controlled through the "class_weight" parameter
 *
 * @author Brion Dolenko
 */

calc_distance_via_chernoff_v::calc_distance_via_chernoff_v(void)
{
  init_gui();
}

void calc_distance_via_chernoff_v::bind_model(scopira::core::model_i *sus)
{
  if (!sus)
    return;

  calc_compound_distance_via_gui_base::bind_model(sus);
}

/*
void calc_distance_via_chernoff_v::react_button(scopira::coreui::button *source, int actionid)
{
  int cl1, cl2;
  double class_weight; 
  string msg;
  bool success;

  if (actionid == action_apply_c) {
    cl1 = string_to_int(dm_class1_entry->get_text());
    cl2 = string_to_int(dm_class2_entry->get_text());
    class_weight = dm_cl_wt_slider->get_value();

    success = calc_chernoff_distance(dm_model.get(),dm_distance.get(), cl1,cl2, class_weight,msg);
    
    if (!success) {
      messagewindow::popup_error(msg);
      return;
    }
    
    dm_distance->set_title(msg);
    dm_distance->notify_views(this);

    // add it to the catalog, if any
    if (dm_model->get_project()) {
      dm_model->get_project()->add_model(dm_model.get(), dm_distance.get());
      dm_model->get_project()->notify_views(this);
    }
  }

  viewwidget::react_button(source, actionid);
}
*/
void calc_distance_via_chernoff_v::react_button(scopira::coreui::button *source, int actionid)
{
  double class_weight;

  if (actionid == action_apply_c) {
    class_weight = dm_cl_wt_slider->get_value();
    calc_compound_distance_via_gui_base::react_button( calc_chernoff_distance, class_weight );
  }

  viewwidget::react_button(source, actionid);
}

void calc_distance_via_chernoff_v::init_gui(void)
{
  calc_compound_distance_via_gui_base::init_gui();

  dm_cl_wt_slider = new slider(0.0, 1.0, 0.01, 2, 0, false);
  dm_g->add_widget(new label("Class weight:"), 0, 2);
  dm_g->add_widget(dm_cl_wt_slider.get(), 1, 2);
  dm_cl_wt_slider->show_min_max();
  dm_cl_wt_slider->set_value(0.5);
  dm_cl_wt_slider->set_digits(2);

  viewwidget::init_gui(dm_g.get(), button_ok_c|button_cancel_c);
  set_view_title("Chernoff Distance");
}

// Symmetric KL Divergence function
             

/*
 * sym_kl_divergence distance GUI class
 *
 * Uses a mixture of the covariance matrices of the two user-specified classes.
 * Also a trace term involving inverses of the covariance matrices is added.
 *
 * @author Brion Dolenko
 */

//
// calc_distance_via_sym_kl_divergence_v
//

calc_distance_via_sym_kl_divergence_v::calc_distance_via_sym_kl_divergence_v(void)
{
  init_gui();
}

void calc_distance_via_sym_kl_divergence_v::bind_model(scopira::core::model_i *sus)
{
  if (!sus)
    return;

  calc_compound_distance_via_gui_base::bind_model(sus);
}

/*
void calc_distance_via_sym_kl_divergence_v::react_button(scopira::coreui::button *source, int actionid)
{
  int cl1, cl2;
  string msg;
  bool success;

  if (actionid == action_apply_c) {
    cl1 = string_to_int(dm_class1_entry->get_text());
    cl2 = string_to_int(dm_class2_entry->get_text());

    success = calc_sym_kl_divergence_distance(dm_model.get(),dm_distance.get(), cl1,cl2, msg);
    
    if (!success) {
      messagewindow::popup_error(msg);
      return;
    }
    
    dm_distance->set_title(msg);
    dm_distance->notify_views(this);

    // add it to the catalog, if any
    if (dm_model->get_project()) {
      dm_model->get_project()->add_model(dm_model.get(), dm_distance.get());
      dm_model->get_project()->notify_views(this);
    }
  }

  viewwidget::react_button(source, actionid);
}
*/
void calc_distance_via_sym_kl_divergence_v::react_button(scopira::coreui::button *source, int actionid)
{
  if (actionid == action_apply_c) {
    calc_compound_distance_via_gui_base::react_button( calc_sym_kl_divergence_distance, 0.5 );
  }

  viewwidget::react_button(source, actionid);
}

void calc_distance_via_sym_kl_divergence_v::init_gui(void)
{
  calc_compound_distance_via_gui_base::init_gui();

  viewwidget::init_gui(dm_g.get(), button_ok_c|button_cancel_c);
  set_view_title("sym_kl_divergence Distance");
}

// Patrick-Fisher distance function
             
/*
 * Patrick-Fisher distance GUI class
 *
 * Takes the exponential of a covariance term, multiplies by a constant and
 * adds a different constant
 *
 * @author Brion Dolenko
 */

calc_distance_via_patrick_fisher_v::calc_distance_via_patrick_fisher_v(void)
{
  init_gui();
}

void calc_distance_via_patrick_fisher_v::bind_model(scopira::core::model_i *sus)
{
  if (!sus)
    return;

  calc_compound_distance_via_gui_base::bind_model(sus);
}

/*
void calc_distance_via_patrick_fisher_v::react_button(scopira::coreui::button *source, int actionid)
{
  int cl1, cl2;
  string msg;
  bool success;

  if (actionid == action_apply_c) {
    cl1 = string_to_int(dm_class1_entry->get_text());
    cl2 = string_to_int(dm_class2_entry->get_text());

    success = calc_patrick_fisher_distance(dm_model.get(),dm_distance.get(), cl1,cl2, msg);
    
    if (!success) {
      messagewindow::popup_error(msg);
      return;
    }
    
    dm_distance->set_title(msg);
    dm_distance->notify_views(this);

    // add it to the catalog, if any
    if (dm_model->get_project()) {
      dm_model->get_project()->add_model(dm_model.get(), dm_distance.get());
      dm_model->get_project()->notify_views(this);
    }
  }

  viewwidget::react_button(source, actionid);
}
*/

void calc_distance_via_patrick_fisher_v::react_button(scopira::coreui::button *source, int actionid)
{
  if (actionid == action_apply_c) {
    calc_compound_distance_via_gui_base::react_button( calc_patrick_fisher_distance, 0.5 );
  }

  viewwidget::react_button(source, actionid);
}

void calc_distance_via_patrick_fisher_v::init_gui(void)
{
  calc_compound_distance_via_gui_base::init_gui();

  viewwidget::init_gui(dm_g.get(), button_ok_c|button_cancel_c);
  set_view_title("Patrick-Fisher Distance");
}
