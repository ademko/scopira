
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

#ifndef __INCLUDED_PAKIT_UI_LDA_H__
#define __INCLUDED_PAKIT_UI_LDA_H__

#include <scopira/basekit/vectormath.h>
#include <scopira/coreui/viewwidget.h>
#include <scopira/coreui/entry.h>
#include <scopira/coreui/slider.h>

#include <pakit/ui/models.h>

namespace pakit
{
  class lda_v;
}

/**
 * LDA Viewer.
 *
 * @author Aleksander Demko
 * @author Brion Dolenko
 * @author Marina Mandelzweig
 */
class pakit::lda_v : public scopira::coreui::viewwidget,
  public virtual scopira::coreui::entry_reactor_i,
  public virtual scopira::coreui::slider_reactor_i
  
{
  private:
    scopira::core::model_ptr<pakit::patterns_m> dm_model;
    scopira::tool::count_ptr<pakit::probabilities_m> dm_probs;
    scopira::tool::count_ptr<scopira::uikit::narray_m<double,2> > dm_coefs;
    
    double dm_lda_line_rotation;
    
    scopira::tool::count_ptr<scopira::coreui::entry> dm_class_groupings_entry;
    scopira::tool::count_ptr<scopira::coreui::slider> dm_lda_position_slider;
    scopira::tool::count_ptr<scopira::coreui::slider> dm_lda_rotation_slider;
    scopira::tool::count_ptr<scopira::coreui::entry> dm_lda_rotation_entry;

  public:
    /// ctor
    lda_v(void);

    virtual void bind_model(scopira::core::model_i *sus);

    virtual void react_button(scopira::coreui::button *source, int actionid);
    virtual void react_entry(scopira::coreui::entry *source, const char *msg);
    virtual void react_slider(scopira::coreui::slider *source, double doubleval, int intval);
    
  private:    
    void init_gui(void);
    /// set lda rotation variable
    void set_lda_rotation_val(double doubleVal);
};

#endif

