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

#ifndef __INCLUDED_PAKIT_RDP_H__
#define __INCLUDED_PAKIT_RDP_H__

#include <scopira/basekit/narray.h>

#include <pakit/export.h>

namespace pakit
{
  /**
   * given the triangle:
   *
   *     l
   *   B/ \C
   *  j-----k
   *     A
   *
   * This gives you the X,Y coords (origin at point J) given the
   * 3 sides, A, B, C
   *
   * @author Aleksander Demko
   */ 
  void PAKIT_EXPORT rdp_calc_xy(double A, double B, double C, double &X, double &Y);

  /**
   * Calc a 3D point (X,Y,Z) for a given N1, N2, N3.
   * N1 should be (0,0,0), N2 == (D12,0,0) N3 = (X3,Y3,0)
   * (you can use rdp_calc_xy to help calc X3 Y3).
   * @param D12 distance between pt 1 and 2
   * @param D1p distance between pt 1 and the target
   * @param D2p distance between pt 2 and the target
   * @param D3p distance between pt 3 and the target
   * @param D13 distance between pt 1 and 3
   * @param X output X
   * @param Y output Y
   * @param Z output Z
   * @return true on success
   * @author Aleksander Demko
   */ 
  PAKIT_EXPORT bool rdp_calc_xyz(double D12, double D1p, double D2p, double D3p, double D13, 
    double X3, double Y3, double &X, double &Y, double &Z);

  PAKIT_EXPORT void rdp_calc_2d_space(const scopira::basekit::narray<double,2> &distance_matrix,
      int n1, int n2, scopira::basekit::narray<double,2> &outpts);

  PAKIT_EXPORT void rdp_calc_2d_ratios(const scopira::basekit::narray<double,2> &distance_matrix,
      int n1, int n2, scopira::basekit::narray<double,2> &outpts);

  /// returns true on success
  PAKIT_EXPORT bool rdp_calc_3d_space(const scopira::basekit::narray<double,2> &distance_matrix,
      int n1, int n2, int n3, scopira::basekit::narray<double,2> &outpts);
}

#endif

