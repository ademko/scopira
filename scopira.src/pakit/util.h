
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

#ifndef __INCLUDED_PAKIT_UTIL_H__
#define __INCLUDED_PAKIT_UTIL_H__

#include <string>

#include <scopira/basekit/narray.h>

#include <pakit/types.h>
#include <pakit/export.h>

namespace pakit
{
  class ibdtext_io_alg;
  class sort_data_alg;
}

/**
 * Functor that can do "IBD Text" file IO.
 * @author Aleksander Demko
 */
class pakit::ibdtext_io_alg
{
  public:
    enum {
      fixed_labels_c,
      variable_labels_c,
    };
  private:
    std::string dm_filename, dm_error;
    bool dm_swaptrain;
    bool dm_trainonly;
    int dm_type;
  public:
    /// ctor
    PAKIT_EXPORT ibdtext_io_alg(void);
    /// sets the type of file to load, variable_labels_c is the default
    /// only used in loads
    void set_type(int t) { dm_type = t; }
    /// sets the filename to load or save to
    void set_filename(const std::string &filename) { dm_filename = filename; }
    /// sets whether training and test sets should be swapped
    void set_swap(bool b) { dm_swaptrain = b; }
    /// sets whether only training data should be passed downstream
    void set_trainonly(bool b) { dm_trainonly = b; }
    /// gets the error string, might be empty
    const std::string &get_error(void) const { return dm_error; }

    /// loads a file, returns false on failure and fills the error string
    PAKIT_EXPORT bool load_file(std::string &title, pakit::patterns_t &outpat);
    /// saves a file, returns false on failure and fills the error string
    /// any of the (pointer) params, except data, may be null
    PAKIT_EXPORT bool save_file(const std::string &title, const pakit::patterns_t &inpat);
};

/**
 * Sorter of pattern data.
 * @author Aleksander Demko
 */
class pakit::sort_data_alg : public pakit::patterns_t
{
  public:
    /**
     * Constructor
     * @author Aleksander Demko
     */
    PAKIT_EXPORT sort_data_alg(void);

    /**
     * Is the dataset already sorted by class?
     * @author Aleksander Demko
     */ 
    PAKIT_EXPORT bool is_sorted_by_class(void) const;
    /**
     * Sort the data by class.
     * You might want to call is_sorted_by_class first
     * before calling this unesesarly.
     * The this data items will be changed (sort in place).
     * @author Aleksander Demko
     */ 
    PAKIT_EXPORT void sort_by_class(void);

    /**
     * Is this data set sorted by training?
     * @author Aleksander Demko
     */
    PAKIT_EXPORT bool is_sorted_by_training(void) const;

    /**
     * Sort by training (true first, then falses)
     * @author Aleksander Demko
     */
    PAKIT_EXPORT void sort_by_training(void);

    /**
     * Calculates various stats about the stats vector.
     * Any param may be null.
     * @param min the min class
     * @param max the max class
     * @param minimin the min class that is >= 1 (may not always be >= 1,
     * if no such items exist in the data set)
     * @return true of something was done (namely, false is returned
     * if the class vector is 0 length)
     * @author Aleksander Demko
     */ 
    PAKIT_EXPORT bool class_stats(int *min, int *max, int *minimin) const;

    /**
     * Calculates the centroids and adds them.
     * A new data set with the centroids prepended
     * will be produced.
     * The this data items will not be changed. Note
     * that the output and this data items cannot point
     * to the same objects in memory.
     * @param super compute a global centroid
     * @param perclass compute per class centroids
     * @param output the alg that contains the items to shove the output into
     * @author Aleksander Demko
     */
    PAKIT_EXPORT void add_centroids(bool super, bool perclass, pakit::patterns_t &output) const;

    /**
     * Calculates the centroid for a block of data.
     * @author Aleksander Demko
     * Only useful for computing global centroid; add_centroids computes
     * the per-class centroids now -BD
     */ 
    PAKIT_EXPORT static void calc_global_centroid(const scopira::basekit::nslice<double,2> &pts,const scopira::basekit::nslice<double> &centout);

  private:
    class class_sort_view;
};

#endif

