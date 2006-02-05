
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

#ifndef __INCLUDED_PRINT_CLASSIFICATION_STATISTICS_H__
#define __INCLUDED_PRINT_CLASSIFICATION_STATISTICS_H__

#include <scopira/basekit/narray.h>
#include <scopira/coreui/viewwidget.h>
#include <scopira/coreui/output.h>
#include <scopira/coreui/entry.h>
#include <scopira/coreui/filewindow.h>
#include <scopira/coreui/radiobutton.h>
#include <scopira/coreui/dropdown.h>

#include <pakit/ui/models.h>
#include <pakit/ui/export.h>

namespace pakit
{
  class printclassnstats_v;
}

/**
 * Take a set of classifier probabilities and output a set of statistics
 *
 * @author Brion Dolenko
 */
class pakit::printclassnstats_v : public scopira::coreui::viewwidget,
  public virtual scopira::core::view_i,
  public virtual scopira::coreui::dropdown_reactor_i,
  public virtual scopira::coreui::radiobutton_reactor_i,
  public virtual scopira::coreui::entry_reactor_i
{
  public:
    /// ctor
    PAUIKIT_EXPORT printclassnstats_v(void);

    PAUIKIT_EXPORT virtual void react_button(scopira::coreui::button *source, int actionid);
    PAUIKIT_EXPORT virtual void react_dropdown(scopira::coreui::dropdown *source, int selection);
    PAUIKIT_EXPORT virtual void react_radiobutton(scopira::coreui::radiobutton *source, int actionid);
    PAUIKIT_EXPORT virtual void react_entry(scopira::coreui::entry *source, const char *msg);

    PAUIKIT_EXPORT virtual void bind_model(scopira::core::model_i *sus);
    PAUIKIT_EXPORT virtual void react_model_update(scopira::core::model_i *sus, scopira::core::view_i *src);

  protected:
    PAUIKIT_EXPORT void update_view(void);
    PAUIKIT_EXPORT void init_gui(void);
    
  protected:
    scopira::core::model_ptr<pakit::probabilities_m> dm_model;
    scopira::tool::count_ptr<scopira::coreui::outputwidget> dm_output;
    scopira::tool::count_ptr<scopira::coreui::entry> dm_nvalidation_entry;
    scopira::tool::count_ptr<scopira::coreui::radiobutton> dm_set_buttons;
    scopira::tool::count_ptr<scopira::coreui::dropdown> dm_stats_dropdown;
    scopira::tool::count_ptr<scopira::coreui::fileentry> dm_filename;

    int dm_print_what;
    int dm_which_set;
    int dm_nvalidation;

  private:
    int validfilename( std::stringstream &fname );
    bool fuzzyClassification( double *memberVec, int nclasses );
    void sortProbArray ( scopira::basekit::doublematrix_o *probArray, scopira::basekit::intvec_o *desClass, 
                         int npats, int nclass, int sortProbs, double *sortedProbArray,
                         long *sortIndex );
};

#endif

