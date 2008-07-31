
/*
 *  Copyright (c) 2002-2006    National Research Council
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
#include <scopira/tool/platform.h>

namespace scopira
{
  namespace tool
  {
    template<class IntType, IntType a, IntType c, IntType m, IntType val>
      class lincon_gen;

    /// a typical generator type (lincon_gen + some template paramters)
    /// cycle length: 2^31-2
    typedef lincon_gen<int, 16807, 0, 2147483647, 1043618065> minstd_rand0;
    /// cycle length: 2^31-2
    /// a typical generator type (lincon_gen + some template paramters)
    typedef lincon_gen<int, 48271, 0, 2147483647, 399268537> minstd_rand;

    /// a typical random core
    typedef minstd_rand0 lincon_core;

    class rand48;
  }
}

/**
 * Compile-time configurable linear congruential generator
 *
 * Taken from boost.
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

/**
 * An emulatation of the C-like rand48 function.
 * cycle length: 2^48-1
 *
 * Taken from boost.
 *
 * @author Aleksander Demko
 */ 
class scopira::tool::rand48 
{
  public:
    typedef int32_t result_type;
    static const bool has_fixed_range = true;
    static const int32_t min_value = 0;
    //static const int32_t max_value = integer_traits<int32_t>::const_max;

    int32_t min(void) const { return 0; }
    int32_t max(void) const { return std::numeric_limits<int32_t>::max(); }
    
    explicit rand48(int32_t x0 = 1) : lcf(cnv(x0)) { }
    explicit rand48(uint64_t x0) : lcf(x0) { }
    //template<class It> rand48(It& first, It last) : lcf(first, last) { }

    // compiler-generated copy ctor and assignment operator are fine

    void seed(int32_t x0 = 1) { lcf.seed(cnv(x0)); }
    void seed(uint64_t x0) { lcf.seed(x0); }
    //template<class It> void seed(It& first, It last) { lcf.seed(first,last); }

    int32_t operator()(void) { return static_cast<int32_t>(lcf() >> 17); }

  private:
    scopira::tool::lincon_gen<uint64_t, uint64_t(0xDEECE66DUL) | (uint64_t(0x5) << 32),
      0xB, uint64_t(1)<<48, 0> lcf;

  private:
    static uint64_t cnv(int32_t x) { return (static_cast<uint64_t>(x) << 16) | 0x330e;  }
};

#endif

