
/*
 *  Copyright (c) 2001-2003    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */
 

#include <scopira/basekit/matrixmath.h>

#include <math.h>
#include <scopira/basekit/math.h>
#include <scopira/basekit/narray.h>
#include <scopira/basekit/vectormath.h>

using namespace scopira;
using namespace scopira::basekit;

namespace scopira
{
  namespace basekit
  {
    namespace matrix
    {
      // helper tempalte functions
      struct getint_t;
      struct getdouble_t;
      template <class T, class VECT> struct get_g;
      struct setint_t;
      struct setdouble_t;
      template <class T, class VECT> struct set_g;
      // interface based routines
      template <class T, class VECT, class GETF, class SETF > inline void copy_i_g(VECT &dest, const VECT &src);
      template <class T, class VECT, class GETF, class SETF > inline void sca_add_i_g(VECT &dest, const VECT &src, T val);
      template <class T, class VECT, class GETF, class SETF > inline void sca_sub_i_g(VECT &dest, const VECT &src, T val);
      template <class T, class VECT, class GETF, class SETF > inline void sca_mult_i_g(VECT &dest, const VECT &src, T val);
      template <class T, class VECT, class GETF, class SETF > inline void add_i_g(VECT &dest, const VECT &src, const VECT &delta);
      template <class T, class VECT, class GETF, class SETF > inline void sub_i_g(VECT &dest, const VECT &src, const VECT &delta);
      template <class T, class VECT, class GETF, class SETF >
        inline void mult_i_g(VECT &dest, const VECT &m1, const VECT &m2, bool t1, bool t2);
    }
  }
}

// *****************************************
// Small Helpers
// *****************************************
/*struct scopira::basekit::matrix::getint_t {
  inline int operator()(const nummatrix_i& v, size_t x, size_t y) { return v.get_int(x, y); }
};
struct scopira::basekit::matrix::getdouble_t {
  inline double operator()(const nummatrix_i& v, size_t x, size_t y) { return v.get_double(x, y); }
};
template <class T, class VECT> struct scopira::basekit::matrix::get_g {
  inline T operator()(const VECT& v, size_t x, size_t y) { return v.get(x, y); }
};
struct scopira::basekit::matrix::setint_t {
  inline void operator()(nummatrix_i& m, size_t x, size_t y, int v) { m.set_int(x, y, v); }
};
struct scopira::basekit::matrix::setdouble_t {
  inline void operator()(nummatrix_i& m, size_t x, size_t y, double v) { m.set_double(x, y, v); }
};
template <class T, class VECT> struct scopira::basekit::matrix::set_g {
  inline void operator()(VECT& m, size_t x, size_t y, T v) { return m.set(x, y, v); }
};*/

// *****************************************
// Interface based template algs
// *****************************************
template <class T, class VECT, class GETF, class SETF >
  void matrix::copy_i_g(VECT &dest, const VECT &src)
{
  size_t x, y, w, h;

  w = src.width();
  h = src.height();

  dest.resize(w, h);

  if ( (w<=0) || (h<=0) )
    return;

  for (y=0; y<h; y++)
    for (x=0; x<w; x++)
      SETF()(dest, x, y, GETF()(src, x, y));
}

template <class T, class VECT, class GETF, class SETF >
  void matrix::sca_add_i_g(VECT &dest, const VECT &src, T val)
{
  size_t x, y, w, h;

  w = src.width();
  h = src.height();
  dest.resize(w, h);
  for (y=0; y<h; y++)
    for (x=0; x<w; x++)
      SETF()(dest, x, y, GETF()(src, x, y) + val);
}

template <class T, class VECT, class GETF, class SETF >
  void matrix::sca_sub_i_g(VECT &dest, const VECT &src, T val)
{
  size_t x, y, w, h;

  w = src.width();
  h = src.height();
  dest.resize(w, h);
  for (y=0; y<h; y++)
    for (x=0; x<w; x++)
      SETF()(dest, x, y, GETF()(src, x, y) - val);
}

template <class T, class VECT, class GETF, class SETF >
  void matrix::sca_mult_i_g(VECT &dest, const VECT &src, T val)
{
  size_t x, y, w, h;

  w = src.width();
  h = src.height();
  dest.resize(w, h);
  for (y=0; y<h; y++)
    for (x=0; x<w; x++)
      SETF()(dest, x, y, GETF()(src, x, y) * val);
}

template <class T, class VECT, class GETF, class SETF >
  void matrix::add_i_g(VECT &dest, const VECT &src, const VECT &delta)
{
  size_t x, y, w, h;

  w = src.width();
  h = src.height();

  assert( w == delta.width() );
  assert( h == delta.height() );
  dest.resize(w, h);
  for (y=0; y<h; y++)
    for (x=0; x<w; x++)
      SETF()(dest, x, y, GETF()(src, x, y) + GETF()(delta, x, y) );
}

template <class T, class VECT, class GETF, class SETF >
  void matrix::sub_i_g(VECT &dest, const VECT &src, const VECT &delta)
{
  size_t x, y, w, h;

  w = src.width();
  h = src.height();

  assert( w == delta.width() );
  assert( h == delta.height() );
  dest.resize(w, h);
  for (y=0; y<h; y++)
    for (x=0; x<w; x++)
      SETF()(dest, x, y, GETF()(src, x, y) - GETF()(delta, x, y) );
}

template <class T, class VECT, class GETF, class SETF >
  void matrix::mult_i_g(VECT &dest, const VECT &m1, const VECT &m2, bool t1, bool t2)
{
  size_t w, h, x, y, i, len;
  size_t rx1, ry1, *x1, *y1;
  size_t rx2, ry2, *x2, *y2;
  T sum;

  // calc my dimensions and settings by the transpose flags
  if (t1) {
    h = m1.width();
    len = m1.height();
    x1 = &ry1;
    y1 = &rx1;
  } else {
    h = m1.height();
    len = m1.width();
    x1 = &rx1;
    y1 = &ry1;
  }
  if (t2) {
    w = m2.height();
    x2 = &ry2;
    y2 = &rx2;
  } else {
    w = m2.width();
    x2 = &rx2;
    y2 = &ry2;
  }

  assert( (t1 ? m1.height() : m1.width()) == (t2 ? m2.width() : m2.height()) );

  // resize myself
  dest.resize(w, h);

//cerr << "=== MATRIX-MULT:\n"
//  "    L: " << m1.width() << 'X' << m1.height() << (t1?"TRAN":"") <<
//  "\n    R: " << m2.width() << 'X' << m2.height() << (t2?"TRAN":"") <<
//  "\n    Resizing to : " << w << 'X' << h << " len=" << len << endl;

  // do the multiply
  for (y=0; y<h; y++)
    for (x=0; x<w; x++) {
      // start the accumulator
      sum = 0;
      *y1 = y;
      *x2 = x;
      for (i=0; i<len; i++) {
        *x1 = i;
        *y2 = i;
        sum += GETF()(m1, rx1, ry1) * GETF()(m2, rx2, ry2);
      }
      SETF()(dest, x, y, sum);
    }

//cerr << "=== A:\n" << m1 << endl
//     << "=== B:\n" << m2 << endl
//     << "=== ANSWER:\n" << *this << endl;
}


//BBlibs
//BBtargets libscopira.so

