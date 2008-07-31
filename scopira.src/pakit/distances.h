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

#ifndef __INCLUDED_PAKIT_DISTANCES_H__
#define __INCLUDED_PAKIT_DISTANCES_H__

#include <scopira/basekit/narray.h>
#include <scopira/basekit/stringvector.h>

#include <pakit/export.h>
#include <pakit/ui/models.h> // TODO should be moved out of ui subdirectory?

namespace pakit
{
  // SIMPLE distances - no intervening covariance matrices etc.

  typedef void (*simple_distance_func_t)(const scopira::basekit::nslice<double> &a,
    const scopira::basekit::nslice<double> &b, double &dist);

  // euclidean distance mfn
  PAKIT_EXPORT void euclidean_distance(const scopira::basekit::nslice<double> &a,
    const scopira::basekit::nslice<double> &b, double &dist);
  // cityblock distance mfn
  PAKIT_EXPORT void cityblock_distance(const scopira::basekit::nslice<double> &a,
    const scopira::basekit::nslice<double> &b, double &dist);
  // max distance mfn
  PAKIT_EXPORT void max_distance(const scopira::basekit::nslice<double> &a,
    const scopira::basekit::nslice<double> &b, double &dist);

  // This function is called first, and it calls the others!
  PAKIT_EXPORT void calc_distance_matrix_simple(const scopira::basekit::nslice<double,2> &patterns,
    scopira::basekit::narray<double,2> &outmatrix,
    simple_distance_func_t func = euclidean_distance);

  // COMPOUND distances - intervening covariance matrices, additive and
  // multiplicative terms

  // AB distance

  PAKIT_EXPORT bool calc_ab_distance(pakit::patterns_m *dm_model,
      pakit::distances_m *dm_distance, int cl1, int cl2, double class_weight,
      std::string &msg);
  PAKIT_EXPORT bool calc_distance_matrix_ab(scopira::basekit::narray<double,2> &inn, 
    const scopira::basekit::narray<int,1> &cls,
    const scopira::basekit::stringvector_o &labels,
    scopira::basekit::narray<double,2> &outmatrix,
    int cl1, int cl2, double class_weight, std::string &msg);

  // Chernoff distance

  PAKIT_EXPORT bool calc_chernoff_distance(pakit::patterns_m *dm_model,
      pakit::distances_m *dm_distance, int cl1, int cl2, double class_weight,
      std::string &msg);
  PAKIT_EXPORT bool calc_distance_matrix_chernoff(scopira::basekit::narray<double,2> &inn, 
    const scopira::basekit::narray<int,1> &cls,
    const scopira::basekit::stringvector_o &labels,
    scopira::basekit::narray<double,2> &outmatrix,
    int cl1, int cl2, double class_weight, std::string &msg);

  // Symmetric KL Divergence distance

  PAKIT_EXPORT bool calc_sym_kl_divergence_distance(pakit::patterns_m *dm_model,
      pakit::distances_m *dm_distance, int cl1, int cl2, 
      double class_weight, std::string &msg);
  PAKIT_EXPORT bool calc_distance_matrix_sym_kl_divergence(scopira::basekit::narray<double,2> &inn,
    const scopira::basekit::narray<int,1> &cls,
    const scopira::basekit::stringvector_o &labels,
    scopira::basekit::narray<double,2> &outmatrix,
    int cl1, int cl2, std::string &msg);

  // Patrick-Fisher distance

  PAKIT_EXPORT bool calc_patrick_fisher_distance(pakit::patterns_m *dm_model,
      pakit::distances_m *dm_distance, int cl1, int cl2, 
      double class_weight, std::string &msg);
  PAKIT_EXPORT bool calc_distance_matrix_patrick_fisher(scopira::basekit::narray<double,2> &inn, 
    const scopira::basekit::narray<int,1> &cls,
    const scopira::basekit::stringvector_o &labels,
    scopira::basekit::narray<double,2> &outmatrix,
    int cl1, int cl2, std::string &msg);

  // All compound distance functions call this one function at their core!

  PAKIT_EXPORT void calc_distance_matrix_compound(scopira::basekit::narray<double,2> &patterns,
    const scopira::basekit::narray<double,2> &matrix_inverse,
    const double multiplicative_term,
    const double additive_term,
    scopira::basekit::narray<double,2> &outmatrix);
}

#endif

