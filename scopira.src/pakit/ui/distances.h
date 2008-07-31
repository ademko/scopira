
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

#ifndef __INCLUDED_PAKIT_UI_DISTANCES_H__
#define __INCLUDED_PAKIT_UI_DISTANCES_H__

#include <scopira/coreui/canvas.h>
#include <scopira/coreui/entry.h>
#include <scopira/coreui/label.h>
#include <scopira/coreui/slider.h>
#include <scopira/coreui/spinbutton.h>
#include <scopira/coreui/viewwidget.h>
#include <scopira/uikit/matrix.h>
#include <scopira/basekit/narray.h>
#include <scopira/basekit/stringvector.h>

#include <pakit/distances.h>
#include <pakit/ui/models.h>


namespace pakit
{
  class calc_simple_distance_via_gui_base;
  class calc_distance_via_euclidean_v;
  class calc_distance_via_cityblock_v;
  class calc_distance_via_max_distance_v;

  class calc_compound_distance_via_gui_base;
  class calc_distance_via_ab_v;
  class calc_distance_via_chernoff_v;
  class calc_distance_via_sym_kl_divergence_v;
  class calc_distance_via_patrick_fisher_v;
}

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
class pakit::calc_simple_distance_via_gui_base : public scopira::core::view_i
{
  public:
    virtual void bind_model(scopira::core::model_i *sus);

    // returns the name
    virtual std::string calc_distance_matrix_simple(const scopira::basekit::nslice<double,2> &patterns,
      scopira::basekit::narray<double,2> &outmatrix) = 0;
};

/**
 * @author Aleksander Demko
 */ 
class pakit::calc_distance_via_euclidean_v : public pakit::calc_simple_distance_via_gui_base
{
  public:
    virtual std::string calc_distance_matrix_simple(const scopira::basekit::nslice<double,2> &patterns,
      scopira::basekit::narray<double,2> &outmatrix);
};

/**
 * @author Aleksander Demko
 */ 
class pakit::calc_distance_via_cityblock_v : public pakit::calc_simple_distance_via_gui_base
{
  public:
    virtual std::string calc_distance_matrix_simple(const scopira::basekit::nslice<double,2> &patterns,
      scopira::basekit::narray<double,2> &outmatrix);
};

/**
 * @author Aleksander Demko
 */ 
class pakit::calc_distance_via_max_distance_v : public pakit::calc_simple_distance_via_gui_base
{
  public:
    virtual std::string calc_distance_matrix_simple(const scopira::basekit::nslice<double,2> &patterns,
      scopira::basekit::narray<double,2> &outmatrix);
};

//
// COMPOUND distance functions require two classes of interest, usually for a
// covariance matrix calculation
//

namespace pakit
{
    typedef bool (*compound_distance_func_t)(pakit::patterns_m *dm_model,
      pakit::distances_m *dm_distance, int cl1, int cl2, double class_weight,
      std::string &msg);

}

/**
 * Base class holding commonality amongst the compound distance functions
 * @author Brion Dolenko
 */

class pakit::calc_compound_distance_via_gui_base : public scopira::coreui::viewwidget
{
  protected:
    scopira::core::model_ptr<pakit::patterns_m> dm_model;
    scopira::tool::count_ptr<pakit::distances_m> dm_distance;

    scopira::tool::count_ptr<scopira::coreui::grid_layout> dm_g;
    scopira::tool::count_ptr<scopira::coreui::entry> dm_class1_entry, dm_class2_entry;

  public:
    /// ctor
    calc_compound_distance_via_gui_base(void);

    virtual void bind_model(scopira::core::model_i *sus);
    virtual void react_button(scopira::coreui::button *source, int actionid);
    void react_button(compound_distance_func_t func, double class_weight);

  protected:
    void init_gui(void);
};

/**
 * Anderson-Bahadur distance GUI
 * @author Brion Dolenko
 * @author Marina Mandelzweig
 */

class pakit::calc_distance_via_ab_v : public pakit::calc_compound_distance_via_gui_base
{
  protected:
    scopira::tool::count_ptr<scopira::coreui::slider> dm_cl_wt_slider;

  public:
    /// ctor
    calc_distance_via_ab_v(void);
    
    virtual void bind_model(scopira::core::model_i *sus);
    virtual void react_button(scopira::coreui::button *source, int actionid);

  protected:
    void init_gui(void);

};

/**
 * Chernoff distance GUI
 * @author Brion Dolenko
 */

class pakit::calc_distance_via_chernoff_v : public pakit::calc_compound_distance_via_gui_base
{
  protected:
    scopira::tool::count_ptr<scopira::coreui::slider> dm_cl_wt_slider;

  public:
    /// ctor
    calc_distance_via_chernoff_v(void);
    
    virtual void bind_model(scopira::core::model_i *sus);
    virtual void react_button(scopira::coreui::button *source, int actionid);

  protected:
    void init_gui(void);

};

/**
 * Symmetric KL Divergence distance GUI
 * @author Brion Dolenko
 */

class pakit::calc_distance_via_sym_kl_divergence_v : public pakit::calc_compound_distance_via_gui_base
{
  public:
    /// ctor
    calc_distance_via_sym_kl_divergence_v(void);
    
    virtual void bind_model(scopira::core::model_i *sus);
    virtual void react_button(scopira::coreui::button *source, int actionid);

  protected:
    void init_gui(void);
};

/**
 * Patrick-Fisher distance GUI
 * @author Brion Dolenko
 */

class pakit::calc_distance_via_patrick_fisher_v : public pakit::calc_compound_distance_via_gui_base
{
  public:
    /// ctor
    calc_distance_via_patrick_fisher_v(void);
    
    virtual void bind_model(scopira::core::model_i *sus);
    virtual void react_button(scopira::coreui::button *source, int actionid);

  protected:
    void init_gui(void);
};

#endif
