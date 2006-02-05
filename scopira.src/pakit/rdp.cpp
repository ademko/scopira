
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

#include <pakit/rdp.h>

#include <math.h>

#include <scopira/basekit/narray.h>
#include <scopira/basekit/math.h>

//BBlibs scopira
//BBtargets libpakit.so

using namespace scopira::tool;
using namespace scopira::basekit;
using namespace pakit;

const char * pakit::distance_func_type_string =
  "mfn:RBdouble_vec:RBdouble_vec:RDOUBLE";

void pakit::euclidean_distance(const nslice<double> &a,
  const nslice<double> &b, double &dist)
{
  double sum, dd;
  size_t x, l;

  assert(a.size() > 0);
  assert(b.size() > 0);
  assert(a.size() == b.size());
  l = a.size();

  sum = 0;
  for (x=0; x<l; x++) {
    dd = a[x] - b[x];
    sum += dd*dd;
  }
  
  dist = ::sqrt(sum);
}

void pakit::cityblock_distance(const nslice<double> &a,
  const nslice<double> &b, double &dist)
{
  double sum;
  size_t x, l;

  assert(a.size() > 0);
  assert(b.size() > 0);
  assert(a.size() == b.size());
  l = a.size();

  sum = 0;
  for (x=0; x<l; x++) {
    sum += ::fabs(a[x] - b[x]);
  }
  
  dist = sum;
}

void pakit::max_distance(const nslice<double> &a,
  const nslice<double> &b, double &dist)
{
  double curr_dist;
  size_t x, l;

  assert(a.size() > 0);
  assert(b.size() > 0);
  assert(a.size() == b.size());
  l = a.size();

  dist = 0.0;
  for (x=0; x<l; x++) {
    curr_dist = ::fabs(a[x] - b[x]);
    if ( curr_dist > dist ) {
      dist = curr_dist;
    }
  }
}

void pakit::calc_distance_matrix(const scopira::basekit::nslice<double,2> &patterns,
    scopira::basekit::narray<double,2> &outmatrix,
    distance_func_t func)
{
  size_t k, j, sz;
  double dd;

  sz = patterns.height();

  outmatrix.resize(sz, sz);

  for (k=1; k<sz; ++k)
    for (j=0; j<k; ++j) {
      func(patterns.xslice(0, k, patterns.width()),
          patterns.xslice(0, j, patterns.width()), dd);
      outmatrix(k, j) = dd;
      outmatrix(j, k) = dd;
    }

  // set the diagonal
  outmatrix.diagonal_slice().set_all(0);
}

void pakit::rdp_calc_xy(double A, double B, double C, double &X, double &Y)
{
  // brion magic :)
  X = (B*B + A*A - C*C) / (2*A);
  Y = ::sqrt(B*B - X*X);
}

bool pakit::rdp_calc_xyz(double D12, double D1p, double D2p, double D3p, double D13, 
  double X3, double Y3, double &X, double &Y, double &Z)
{
   // simple, 0-Y,Z cases
  if (is_equal(D12, D1p+D2p) || is_equal(D1p, D12+D2p)) {
    // 0-plane, middle or right
    X = D1p;
    Y = 0;
    Z = 0;
    return true;
  }
  else if (is_equal(D2p, D12+D1p)) {
    // 0-plane, left
    X = -D1p;
    Y = 0;
    Z = 0;
    return true;
  }
   
  X = (D12*D12 + D1p*D1p - D2p*D2p) / (2*D12);
  Y = (D1p*D1p - D3p*D3p + D13*D13 - 2*X3*X) / (2*Y3);
  
  //0-z case
  if (is_equal(D1p*D1p, (X*X + Y*Y))) {
    Z = 0;
    return true;
  }
  if ((D1p*D1p) < (X*X + Y*Y))   
    return false;
  
  Z = ::sqrt(D1p*D1p - X*X - Y*Y);
  
  return true;  
}    

void pakit::rdp_calc_2d_space(const scopira::basekit::narray<double,2> &distance_matrix,
      int n1, int n2, scopira::basekit::narray<double,2> &outpts)
{
  size_t sz = distance_matrix.width();
  size_t outsz = outpts.height();
  size_t y;
  double basedist, XX, YY;

  assert(distance_matrix.width() == distance_matrix.height());
  assert(n1>=0);
  assert(n1<sz);
  assert(n2>=0);
  assert(n2<sz);
  assert(n1 != n2);

  if ( outsz == sz + 1 ) {
    outpts.resize(2, outsz); // accounts for special case where outpts contains extra row
                             // for centroid of reference points
  }
  else {
    outpts.resize(2, sz);
  }

  basedist = distance_matrix(n1, n2);
  for (y=0; y<sz; ++y)
    if (y == n1) {
      outpts(0, y) = 0;
      outpts(1, y) = 0;
    } else if (y == n2) {
      outpts(0, y) = basedist;
      outpts(1, y) = 0;
    } else {
      rdp_calc_xy(basedist, distance_matrix(n1, y), distance_matrix(n2, y), XX, YY);
      outpts(0, y) = XX;
      outpts(1, y) = YY;
    }
}

bool pakit::rdp_calc_3d_space(const scopira::basekit::narray<double,2> &distance_matrix,
      int n1, int n2, int n3, scopira::basekit::narray<double,2> &outpts)
{
  size_t sz = distance_matrix.width();
  size_t y;
  double XX, YY, ZZ, X3, Y3;

  assert(distance_matrix.width() == distance_matrix.height());
  assert(n1>=0);
  assert(n1<sz);
  assert(n2>=0);
  assert(n2<sz);
  assert(n3>=0);
  assert(n3<sz);
  assert(n1 != n2);
  assert(n1 != n3);
  assert(n2 != n3);

  outpts.resize(3, sz);

  //calculate n3 values
  rdp_calc_xy(distance_matrix(n1, n2), distance_matrix(n1, n3), distance_matrix(n2, n3), X3, Y3);

  //for each point, calculate x,y,z
  for (y=0; y<sz; ++y) {
    if (y == n1) {  
      outpts.set(0, y, 0);
      outpts.set(1, y, 0);
      outpts.set(2, y, 0);
    } 
    else if (y == n2) {
      outpts.set(0, y, distance_matrix(n1, n2));
      outpts.set(1, y, 0);
      outpts.set(2, y, 0);
    } 
    else if (y == n3) {
      outpts.set(0, y, X3);
      outpts.set(1, y, Y3);
      outpts.set(2, y, 0);
    } else {
      if (! rdp_calc_xyz(distance_matrix(n1, n2), distance_matrix(n1, y), distance_matrix(n2, y), distance_matrix(n3, y), 
        distance_matrix(n1,n3), X3, Y3, XX, YY, ZZ)) {
          return false;
      }
      outpts.set(0, y, XX);
      outpts.set(1, y, YY);
      outpts.set(2, y, ZZ);
    }
  }

  return true;
}

