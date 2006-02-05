
/*
 *  Copyright (c) 2002-2003    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#ifndef __INCLUDED_SCOPIRA_BASEKIT_MATH_H__
#define __INCLUDED_SCOPIRA_BASEKIT_MATH_H__

#include <scopira/tool/platform.h>

#ifdef PLATFORM_win32
#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#include <float.h>
#endif

#include <assert.h>
#include <math.h>

#include <scopira/tool/platform.h>

// should these be in this file?
#ifndef TINYNUM
#define TINYNUM (1.0e-20)
#endif

#ifndef SMALLNUM
#define SMALLNUM (1.0e-5)
#endif

#define IMIN(a,b) ((a) < (b) ? (a) : (b))
#define FMAX(a,b) ((a) > (b) ? (a) : (b))
#define SQR(a) ((a) == 0.0 ? 0.0 : a*a)
#define SIGN(a,b) ((b) >= 0.0 ? fabs(a) : -fabs(a))
#define SWAP(a,b) temp=(a);(a)=(b);(b)=temp;

#ifndef M_E
#define M_E 2.71828182845904523536
#endif
#ifndef M_LOG2E
#define M_LOG2E 1.44269504088896340736
#endif
#ifndef M_LOG10E
#define M_LOG10E 0.434294481903251827651
#endif
#ifndef M_LN2
#define M_LN2 0.693147180559945309417
#endif
#ifndef M_LN10
#define M_LN10 2.30258509299404568402
#endif
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#ifndef M_PI_2
#define M_PI_2 1.57079632679489661923
#endif
#ifndef M_PI_4
#define M_PI_4 0.785398163397448309616
#endif
#ifndef M_1_PI
#define M_1_PI 0.318309886183790671538
#endif
#ifndef M_2_PI
#define M_2_PI 0.636619772367581343076
#endif
#ifndef M_2_SQRTPI
#define M_2_SQRTPI 1.12837916709551257390
#endif
#ifndef M_SQRT2
#define M_SQRT2 1.41421356237309504880
#endif
#ifndef M_SQRT1_2
#define M_SQRT1_2 0.707106781186547524401
#endif

namespace scopira
{
  namespace basekit
  {
    const double tol_c = 0.0000001;
    const double tinynum_c = 1.0e-20;
    const double smallnum_c = 1.0e-5;

    template <class T>
      inline T sqr(T v) { return v*v; }
    template <class T>
      inline bool is_equal(T v1, T v2) { T tmp(v1 - v2); return tmp<tol_c && tmp> (-tol_c); }
    template <class T>
      inline bool is_zero(T v) { return v<tol_c && v> (-tol_c); }
#ifdef PLATFORM_win32
    inline bool is_nan(double v) { return _isnan(v) != 0; }
    inline bool is_nan(float v) { return _isnan(v) != 0; }//incase is*() is a macro, which it often is
#else
    inline bool is_nan(double v) { return isnan(v) != 0; }
    inline bool is_nan(float v) { return isnan(v) != 0; }//incase is*() is a macro, which it often is
#endif
    /// see man isinf for the meanin of the return
#ifdef PLATFORM_win32
    inline int is_inf(double v) { return !_finite(v) && !_isnan(v); }
    inline int is_inf(float v) { return !_finite(v) && !_isnan(v); }
#else
#ifdef PLATFORM_irix
    // surely irix has an implmentation of these?
    inline int is_inf(double v) { assert(false); return 0; }
    inline int is_inf(float v) { assert(false); return 0; }
#else
    inline int is_inf(double v) { return isinf(v); }
    inline int is_inf(float v) { return isinf(v); }//incase is*() is a macro, which it often is
#endif
#endif
    template <class T>
      inline T sign(T v) { if (v>0) return 1; if (v<0) return -1; return 0; }
      
    //http://more.sourceforge.net/doc/html/math_8h-source.html
    /**
     *  
     * Compute the factorial of n.  If m != 0, compute factorial of (n-m).
     * 
     * @param source natural number n
     * @param source difference integer m, m < n.
     * @return n! if (m==0), (n-m)! otherwise.
     * 
     * @author Mark Alexiuk
     */  
    int factorial(int n, int m = 0);
    /**
     * Compute the binomial coefficient (n/(n-i, i)).
     *
     * @param source value n (total things)
     * @param source value i (choosen i at a time)
     * @return binomial coefficient (n/(n-i, i))
     * 
     * @author Mark Alexiuk
     */  
    int binomial(int n, int i);
  }
}

#endif

