
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

#ifndef _INCLUDED__PAKIT_UI_PLOT_H_
#define _INCLUDED__PAKIT_UI_PLOT_H_

#include <scopira/uikit/plotter.h>
#include <scopira/coreui/label.h>
#include <scopira/coreui/checkbutton.h>

#include <pakit/ui/models.h>

namespace pakit
{
  class plot_spectra_v;	
}

/**
 * 2D Plotter of spectra
 *
 * @author Shantha Ramachandran
 */
class pakit::plot_spectra_v : public scopira::uikit::plotter,
  public virtual scopira::coreui::spinbutton_reactor_i,
  public virtual scopira::core::view_i
{
  protected:
    typedef plotter parent_type;

    scopira::core::model_ptr<pakit::patterns_m> dm_model;

    scopira::tool::count_ptr<scopira::coreui::spinbutton> dm_spinner;    // displays the value of the plot
    scopira::tool::count_ptr<scopira::coreui::label> dm_title;  // for info purposes
    scopira::tool::count_ptr<scopira::coreui::label> dm_class;  // for info purposes

    int dm_slice;

    // min and max over all spectra
    double dm_overall_ymin;
    double dm_overall_ymax;
    
    scopira::tool::count_ptr<scopira::coreui::widget> dm_basewidget;
  public:
    plot_spectra_v(void);
    
    virtual void react_spinbutton(scopira::coreui::spinbutton *source, int intval, double doubleval);

    virtual void bind_model(scopira::core::model_i *sus);
    virtual void react_model_update(scopira::core::model_i *sus, scopira::core::view_i *src);
    
  protected:
    void init_gui(void);

    /// called after change the data
    void update_gui(void);
};
  
#endif

