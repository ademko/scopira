
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

#ifndef __INCLUDED_PAKIT_UI_PATTERNSORT_H__
#define __INCLUDED_PAKIT_UI_PATTERNSORT_H__

#include <scopira/coreui/dropdown.h>
#include <scopira/coreui/viewwidget.h>

#include <pakit/ui/models.h>

namespace pakit
{
  class pattern_sort_v;
  class mysort;
}

/**
 * Lets you resort the patterns
 *
 * @author Aleksander Demko
 */
class pakit::pattern_sort_v : public scopira::coreui::viewwidget
{
  private:
    scopira::core::model_ptr<pakit::patterns_m> dm_model;
    scopira::tool::count_ptr<scopira::coreui::dropdown> dm_drop;
  public:
    pattern_sort_v(void);
    virtual void bind_model(scopira::core::model_i *sus);
    virtual void react_model_update(scopira::core::model_i *sus, scopira::core::view_i *src);
    virtual void react_button(scopira::coreui::button *source, int actionid);
  private:
    void update_gui(void);
};

/**
 * Helper class so can interface with scopira::tool::qsort
 */
class pakit::mysort
{
  private:
    int dm_row;
    patterns_t &dm_pat;
    scopira::basekit::narray<double> dm_temp;     // so we dont need to realloc
    std::string dm_string;
  public:
		PAUIKIT_EXPORT mysort(int row, patterns_t &pat);
    PAUIKIT_EXPORT int compare_element(int lidx, int ridx) const;
    PAUIKIT_EXPORT void swap_element(int lidx, int ridx);
};

#endif

