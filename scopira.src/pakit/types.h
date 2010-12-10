
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

#ifndef __INCLUDED_PAKIT_TYPES_H__
#define __INCLUDED_PAKIT_TYPES_H__

#include <scopira/basekit/narray.h>
#include <scopira/basekit/stringvector.h>

#include <pakit/export.h>

/**
 * The pakit provides basic tools for pattern analysis and data classification
 *
 * @author Aleksander Demko
 */ 
namespace pakit
{
  class patterns_t;
}

/**
 * a patterns data set (new style)
 * @author Aleksander Demko
 */ 
class pakit::patterns_t
{
  public:
    scopira::tool::count_ptr<scopira::basekit::narray_o<double,2> > pm_data;
    scopira::tool::count_ptr<scopira::basekit::narray_o<int,1> > pm_classes;
    scopira::tool::count_ptr<scopira::basekit::narray_o<bool,1> > pm_training;
    scopira::tool::count_ptr<scopira::basekit::stringvector_o> pm_labels;

    std::string pm_comment;

  public:
    /**
     * Constructor.
     * @param init if true, all pointers are "new'ed" to something.
     * if false, theyre left to null
     * @author Aleksander Demko
     */
    PAKIT_EXPORT explicit patterns_t(bool init);
    /**
     * Copy constructor
     * @author Aleksander Demko
     */
    PAKIT_EXPORT patterns_t(const patterns_t &rhs, bool deepcopy);

    /**
     * Copy function.
     * If deepcopy is false, then this will point to the same internal arrays
     * as in rhs.
     *
     * @author Aleksander Demko
     */
    PAKIT_EXPORT void copy_patterns(const patterns_t &rhs, bool deepcopy);

    // non virtual

    PAKIT_EXPORT bool load(scopira::tool::iobjflow_i& in);
    PAKIT_EXPORT void save(scopira::tool::oobjflow_i& out) const;

    /**
     * Alias for copy_patterns(rhs, true)
     *
     * @author Aleksander Demko
     */ 
    PAKIT_EXPORT patterns_t operator = (const patterns_t &rhs) {
      copy_patterns(rhs, true);
      return *this;
    }
};

#endif

