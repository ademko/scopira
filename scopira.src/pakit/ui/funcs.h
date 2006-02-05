
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

#ifndef __INCLUDED_PAKIT_UI_FUNCS_H__
#define __INCLUDED_PAKIT_UI_FUNCS_H__

#include <scopira/core/view.h>
#include <scopira/coreui/filewindow.h>
#include <scopira/coreui/checkbutton.h>
#include <scopira/coreui/radiobutton.h>
#include <scopira/coreui/output.h>
#include <scopira/coreui/viewwidget.h>

#include <pakit/ui/models.h>
#include <pakit/ui/export.h>

namespace pakit
{
  //class load_ibdtext_v;
  class new_ibdtext_v;
  class save_ibdtext_v;
  class add_centroids_v;
  class general_report_v;
}

/**
 * Loads ibdtext files to patterns_record's
 *
 * @author Aleksander Demko
 */
class pakit::new_ibdtext_v : public scopira::coreui::viewwidget
{
  private:
    scopira::core::model_ptr<scopira::core::project_i> dm_model;
    scopira::tool::count_ptr<scopira::coreui::fileentry> dm_filename;
    scopira::tool::count_ptr<scopira::coreui::radiobutton> dm_type;
  public:
    /// ctor
    PAUIKIT_EXPORT new_ibdtext_v(void);

    PAUIKIT_EXPORT virtual void bind_model(scopira::core::model_i *sus);
    PAUIKIT_EXPORT virtual void react_button(scopira::coreui::button *source, int actionid);
};

/**
 * Saves a patterns_record to an ibd text file
 * @author Aleksander Demko
 */
class pakit::save_ibdtext_v : public scopira::coreui::viewwidget
{
  private:
    scopira::core::model_ptr<pakit::patterns_m> dm_model;
    scopira::tool::count_ptr<scopira::coreui::fileentry> dm_filename;
  public:
    /// ctor
    PAUIKIT_EXPORT save_ibdtext_v(void);

    PAUIKIT_EXPORT virtual void bind_model(scopira::core::model_i *sus);
    PAUIKIT_EXPORT virtual void react_button(scopira::coreui::button *source, int actionid);
};

/**
 * prepends centroids to the data set
 * @author Aleksander Demko
 */
class pakit::add_centroids_v : public scopira::coreui::viewwidget
{
  private:
    scopira::core::model_ptr<pakit::patterns_m> dm_model;
    scopira::tool::count_ptr<scopira::coreui::checkbutton> dm_super, dm_perclass;
  public:
    /// ctor
    PAUIKIT_EXPORT add_centroids_v(void);

    PAUIKIT_EXPORT virtual void bind_model(scopira::core::model_i *sus);
    PAUIKIT_EXPORT virtual void react_button(scopira::coreui::button *source, int actionid);
};

/**
 * a general report about classes, training, size, etc.
 *
 * @author Marina Mandelzweig
 * @author Brion Dolenko
 */
class pakit::general_report_v: public scopira::coreui::outputwidget,
  public virtual scopira::core::view_i
{
  private:
    struct one_class{
      int training;
      int testing;
      bool centroid_present;
      bool placeholder_only;
    };
    scopira::core::model_ptr<pakit::patterns_m> dm_model;
    void print(int n, const one_class &_class);

  public:
    /// ctor
    PAUIKIT_EXPORT general_report_v(void);

    PAUIKIT_EXPORT virtual void bind_model(scopira::core::model_i *sus);
    PAUIKIT_EXPORT virtual void react_model_update(scopira::core::model_i *sus, scopira::core::view_i *src);
};

#endif

