
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

#ifndef __INCLUDED_PAKIT_UI_MODELS_H__
#define __INCLUDED_PAKIT_UI_MODELS_H__

#include <scopira/core/model.h>
#include <scopira/core/project.h>
#include <scopira/uikit/models.h>
#include <pakit/ui/export.h>

#include <pakit/types.h>

namespace pakit
{
  class patterns_m;
  class patterns_project_m;
  class distances_m;
  class probabilities_m;
  class rdp_2d_m;
  class rdp_3d_m;
}

/**
 * A record that holds a "patterns" set (features, classes, training
 * and labels).
 * @author Aleksander Demko
 */ 
class pakit::patterns_m : public virtual scopira::core::model_i,
  public pakit::patterns_t
{
  public:
    std::string pm_title;

  public:
    /// ctor
    PAUIKIT_EXPORT patterns_m(void);
    /// (deep) copy ctor
    PAUIKIT_EXPORT patterns_m(const patterns_m &src);

    PAUIKIT_EXPORT virtual bool load(scopira::tool::iobjflow_i& in);
    PAUIKIT_EXPORT virtual void save(scopira::tool::oobjflow_i& out) const;

    /**
     * Sort the data members by class.
     * You may only call this RIGHT after changing all the pm_*
     * members, but before notifying all the other inspectors
     * via notify_inspectors().
     * @author Aleksander Demko
     */ 
    PAUIKIT_EXPORT void sort_by_class(void);
};

/**
 * DEPRECATED. FLAGGED FOR DELETION.
 * @author Aleksander Demko
 */ 
class pakit::patterns_project_m : public virtual scopira::core::project_base
{
  public:
    /// ctor
    PAUIKIT_EXPORT patterns_project_m(void);

    PAUIKIT_EXPORT virtual bool load(scopira::tool::iobjflow_i& in);
    PAUIKIT_EXPORT virtual void save(scopira::tool::oobjflow_i& out) const;
};

/**
 * A distance matrix container, for RDP.
 * @author Aleksander Demko
 */ 
class pakit::distances_m : public scopira::uikit::narray_m<double,2>
{
  private:
    typedef scopira::uikit::narray_m<double,2> parent_type;

  public:
    scopira::tool::count_ptr<pakit::patterns_m> pm_patterns_link;

  public:
    /// ctor
    PAUIKIT_EXPORT distances_m(void);
    /// copy ctor
    PAUIKIT_EXPORT distances_m(const distances_m &src);

    PAUIKIT_EXPORT virtual bool load(scopira::tool::iobjflow_i& in);
    PAUIKIT_EXPORT virtual void save(scopira::tool::oobjflow_i& out) const;
};

/**
 * Classifier probabilities.  A classifier may use different class and training vectors than
 * the parent patterns_m model
 * @author Brion Dolenko
 */ 
class pakit::probabilities_m : public scopira::uikit::narray_m<double,2>
{
  private:
    typedef scopira::uikit::narray_m<double,2> parent_type;

  public:
    scopira::tool::count_ptr<pakit::patterns_m> pm_patterns_link;
    scopira::tool::count_ptr<scopira::basekit::intvec_o> pm_classes;
    scopira::tool::count_ptr<scopira::basekit::boolvec_o> pm_training;

  public:
    /// ctor
    PAUIKIT_EXPORT probabilities_m(void);
    /// copy ctor
    PAUIKIT_EXPORT probabilities_m(const probabilities_m &src);

    PAUIKIT_EXPORT virtual bool load(scopira::tool::iobjflow_i& in);
    PAUIKIT_EXPORT virtual void save(scopira::tool::oobjflow_i& out) const;
};
/**
 * A 2D RDP projection
 * @author Aleksander Demko
 */
class pakit::rdp_2d_m : public scopira::uikit::narray_m<double,2>
{
  private:
    typedef scopira::uikit::narray_m<double,2> parent_type;

  public:
    scopira::tool::count_ptr<pakit::patterns_m> pm_patterns_link;
    scopira::tool::count_ptr<pakit::distances_m> pm_distances_link;
    // this is also checked if pm_distances_link is null
    scopira::tool::count_ptr<scopira::basekit::narray_o<double,2> > pm_distances;

    int pm_n1, pm_n2;       /// the two reference points, very important
    
    /// you must call calc_rdp() after any changes or n1/n2
    /// or initially

  public:
    /// ctor
    PAUIKIT_EXPORT rdp_2d_m(void);
    /// copy ctor
    PAUIKIT_EXPORT rdp_2d_m(const rdp_2d_m &src);

    PAUIKIT_EXPORT virtual bool load(scopira::tool::iobjflow_i& in);
    PAUIKIT_EXPORT virtual void save(scopira::tool::oobjflow_i& out) const;

    /**
     * True if this N1, N2 would be valid (similar syntax to calc_rdp)
     * @author Aleksander Demko
     */
    PAUIKIT_EXPORT bool is_valid_n(int n1, int n2, scopira::basekit::narray<double,2> *dmatrix = 0) const;

    /**
     * Sets n1, n2, and calc rdp.. also notifed all relevant listeners.
     * calc_rdp() will be called with a null dmatrix param.
     * Quite the hlper.
     * @author Aleksander Demko
     */
    PAUIKIT_EXPORT void set_n_calc_rdp(scopira::core::view_i *src, int n1, int n2);

    /**
     * Actually builds the pm_matrix rdp space points from pm_n1 and pm_n2
     * if pm_matrix is null, or n1 or n2 have changed, you'll need to
     * call this.
     *
     * You must supply the distance matrix to use. Null (the default),
     * says to use pm_distances (if not-null) or pm_distances_link's distance
     * matrix (useful if there is NO pm_distances_link) will be used.
     *
     * @author Aleksander Demko
     */
    PAUIKIT_EXPORT void calc_rdp(scopira::basekit::narray<double,2> *dmatrix = 0);
    PAUIKIT_EXPORT void calc_rdp_ratios(scopira::basekit::narray<double,2> *dmatrix = 0);

    /**
     * Calls set_title with some fixed string that uses N1, 2, etc
     * @author Aleksander Demko
     */
    PAUIKIT_EXPORT void set_title_auto(void);
};

/**
 * A #D RDP projection
 * @author Aleksander Demko
 */
class pakit::rdp_3d_m : public scopira::uikit::narray_m<double,2>
{
  private:
    typedef scopira::uikit::narray_m<double,2> parent_type;

  public:
    scopira::tool::count_ptr<pakit::patterns_m> pm_patterns_link;
    scopira::tool::count_ptr<pakit::distances_m> pm_distances_link;
    // this is also checked if pm_distances_link is null
    scopira::tool::count_ptr<scopira::basekit::narray_o<double,2> > pm_distances;

    int pm_n1, pm_n2, pm_n3;       /// the two reference points, very important
    
    /// you must call calc_rdp() after any changes or n1/n2
    /// or initially

  public:
    /// ctor
    PAUIKIT_EXPORT rdp_3d_m(void);
    /// copy ctor
    PAUIKIT_EXPORT rdp_3d_m(const rdp_3d_m &src);

    PAUIKIT_EXPORT virtual bool load(scopira::tool::iobjflow_i& in);
    PAUIKIT_EXPORT virtual void save(scopira::tool::oobjflow_i& out) const;

    /**
     * True if this N1, N2, N3 would be valid (similar syntax to calc_rdp)
     * @author Aleksander Demko
     */
    PAUIKIT_EXPORT bool is_valid_n(int n1, int n2, int n3, scopira::basekit::narray<double,2> *dmatrix = 0) const;

    /**
     * Sets n1, n2, n3 and calc rdp.. also notifed all relevant listeners.
     * calc_rdp() will be called with a null dmatrix param.
     * Quite the hlper.
     * @author Aleksander Demko
     */
    PAUIKIT_EXPORT void set_n_calc_rdp(scopira::core::view_i *src, int n1, int n2, int n3);

    /**
     * Actually builds the pm_matrix rdp space points from pm_n1 and pm_n2
     * if pm_matrix is null, or n1 or n2 have changed, you'll need to
     * call this.
     *
     * You must supply the distance matrix to use. Null (the default),
     * says to use pm_distances_link's distance matrix (useful if there
     * is NO pm_distances_link)
     *
     * returns true on success.
     *
     * @author Aleksander Demko
     */
    PAUIKIT_EXPORT bool calc_rdp(scopira::basekit::narray<double,2> *dmatrix = 0);

    /**
     * Calls set_title with some fixed string that uses N1, 2, etc
     * @author Aleksander Demko
     */
    PAUIKIT_EXPORT void set_title_auto(void);
};

#endif

