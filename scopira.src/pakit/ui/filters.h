
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

#ifndef __INCLUDED_PAKIT_UI_FILTERS_H__
#define __INCLUDED_PAKIT_UI_FILTERS_H__

#include <scopira/basekit/narray.h>
#include <scopira/coreui/radiobutton.h>
#include <scopira/coreui/checkbutton.h>
#include <scopira/coreui/filewindow.h>
#include <scopira/coreui/viewwidget.h>
#include <pakit/ui/models.h>

using namespace scopira::basekit;

namespace pakit
{
  class pattern_whiten_v;
  class pattern_areanorm_v;
  class pattern_firstderiv_v;
  class pattern_secondderiv_v;
  class pattern_rankorder_v;
}

/**
 * Areanorm - Normalize to unit area and zero mean
 *
 @ author Brion Dolenko
 */
class pakit::pattern_areanorm_v : public scopira::coreui::viewwidget
{
  private:
    scopira::core::model_ptr<pakit::patterns_m> dm_model;
    scopira::tool::count_ptr<scopira::coreui::checkbutton> dm_norm_scale;
    scopira::tool::count_ptr<scopira::coreui::checkbutton> dm_norm_offset;
  public:
    pattern_areanorm_v(void);
    virtual void bind_model(scopira::core::model_i *sus);
    virtual void react_button(scopira::coreui::button *source, int actionid);
};

/**
 * Firstderiv - Simple first derivative
 *
 @ author Brion Dolenko
 */
class pakit::pattern_firstderiv_v : public scopira::core::view_i
{
  public:
    virtual void bind_model(scopira::core::model_i *sus);
};

/**
 * Secondderiv - Simple second derivative
 *
 @ author Brion Dolenko
 */
class pakit::pattern_secondderiv_v : public scopira::core::view_i
{
  public:
    virtual void bind_model(scopira::core::model_i *sus);
};

/**
 * Rankorder - perform rank ordering on the data values
 *
 @ author Brion Dolenko
 */
class pakit::pattern_rankorder_v : public scopira::coreui::viewwidget
{
  private:
    scopira::core::model_ptr<pakit::patterns_m> dm_model;
    scopira::tool::count_ptr<scopira::coreui::radiobutton> dm_method;
  public:
    pattern_rankorder_v(void);
    virtual void bind_model(scopira::core::model_i *sus);
    void rankPointsWithin(patterns_t &pat);
    void rankPointsBetween(patterns_t &pat);
    virtual void react_button(scopira::coreui::button *source, int actionid);
};

/**
 * Whiten - normalize the patterns
 *
 @ author Brion Dolenko
 */
class pakit::pattern_whiten_v : public scopira::coreui::viewwidget
{
  private:
    scopira::core::model_ptr<pakit::patterns_m> dm_model;
    scopira::tool::count_ptr<scopira::coreui::radiobutton> dm_method;
    scopira::tool::count_ptr<scopira::coreui::fileentry> dm_parmsfile;
  public:
    pattern_whiten_v(void);
    virtual void bind_model(scopira::core::model_i *sus);
    void whitenMeanWithin(patterns_t &pat, narray_o<double> &subtractVec, narray_o<double> &divideVec);
    void whitenMean(patterns_t &pat, narray_o<double> &subtractVec, narray_o<double> &divideVec);
    void whitenMedian(patterns_t &pat, narray_o<double> &subtractVec, narray_o<double> &divideVec);
    bool whitenFromFile(patterns_t &pat);
    void outputParms(narray_o<double> &subtractVec, narray_o<double> &divideVec);
    virtual void react_button(scopira::coreui::button *source, int actionid);
};
#endif
