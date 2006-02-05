
/*
 *  Copyright (c) 2002    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#ifndef __INCLUDED__SCOPIRA_TOOL_LINCONRANDOM_H__
#define __INCLUDED__SCOPIRA_TOOL_LINCONRANDOM_H__

#include <assert.h>

#include <scopira/tool/constmod.h>

namespace scopira
{
  namespace tool
  {
    template<class IntType, IntType a, IntType c, IntType m, IntType val>
      class lincon_gen;

    // validation values from the publications
    typedef lincon_gen<int, 16807, 0, 2147483647, 1043618065> minstd_rand0;
    typedef lincon_gen<int, 48271, 0, 2147483647, 399268537> minstd_rand;

    /// a typical random core
    typedef minstd_rand0 lincon_core;

    // eventually remove rand48, boost's clone of rand48 in here?
    // (it requires uint64 (unsigned long long))
  }
}

/**
 * compile-time configurable linear congruential generator
 *
 * taken from boost
 *
 * @author Aleksander Demko
 */ 
template<class IntType, IntType a, IntType c, IntType m, IntType val>
class scopira::tool::lincon_gen
{
  public:
    typedef IntType result_type;

    static const bool has_fixed_range = true;
    static const result_type min_value = ( c == 0 ? 1 : 0 );
    static const result_type max_value = m-1;

    /// x0 is the seed
    explicit lincon_gen(IntType x0 = 1)
      : _x(x0) { 
      assert(c || x0); // if c == 0 and x(0) == 0 then x(n) = 0 for all n
      // overflow check
      // disabled because it gives spurious "divide by zero" gcc warnings
      // assert(m == 0 || (a*(m-1)+c) % m == (c < a ? c-a+m : c-a)); 
    }

    /// resets the seed
    void seed(IntType x0) { assert(c || x0); _x = x0; }
    result_type min(void) const { return c == 0 ? 1 : 0; }
    result_type max(void) const { return m-1; }

    /// get next num
    IntType operator()(void) {
      _x = const_mod<IntType, m>::mult_add(a, _x, c);
      return _x;
    }

    bool validation(IntType x) const { return val == x; }
  private:
    IntType _x;
};

#endif

