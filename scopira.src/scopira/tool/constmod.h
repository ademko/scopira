
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

#ifndef __INCLUDED_SCOPIRA_TOOL_CONSTMOD_H__
#define __INCLUDED_SCOPIRA_TOOL_CONSTMOD_H__

//
// taken from Boost, for random.h
//

#include <scopira/tool/limits.h>
#include <scopira/tool/platform.h>

namespace scopira {
namespace tool {

/*
 * Some random number generators require modular arithmetic.  Put
 * everything we need here.
 * IntType must be an integral type.
 */

template<class IntType, IntType m>
class const_mod
{
public:
  static IntType add(IntType x, IntType c)
  {
    if(c == 0)
      return x;
    else if(c <= traits::max() - m)    // i.e. m+c < max
      return add_small(x, c);
    else if(traits::is_signed)
      return add_signed(x, c);
    else {
      // difficult
      assert(!"const_mod::add with c too large");
      return 0;
    }
  }

  static IntType mult(IntType a, IntType x)
  {
    if(a == 1)
      return x;
    else if(m <= traits::max()/a)      // i.e. a*m <= max
      return mult_small(a, x);
    else if(traits::is_signed && (m%a < m/a))
      return mult_schrage(a, x);
    else {
      // difficult
      assert(!"const_mod::mult with a too large");
      return 0;
    }
  }

  static IntType mult_add(IntType a, IntType x, IntType c)
  { 
    if(m <= (traits::max()-c)/a)   // i.e. a*m+c <= max
      return (a*x+c) % m;
    else
      return add(mult(a, x), c);
  }

  static IntType invert(IntType x)
  { return x == 0 ? 0 : invert_euclidian(x); }

private:
  typedef std::numeric_limits<IntType> traits;

  const_mod();      // don't instantiate

  static IntType add_small(IntType x, IntType c)
  {
    x += c;
    if(x >= m)
      x -= m;
    return x;
  }

  static IntType add_signed(IntType x, IntType c)
  {
    x += (c-m);
    if(x < 0)
      x += m;
    return x;
  }

  static IntType mult_small(IntType a, IntType x)
  {
    return a*x % m;
  }

  static IntType mult_schrage(IntType a, IntType value)
  {
    const IntType q = m / a;
    const IntType r = m % a;

    assert(r < q);        // check that overflow cannot happen

    value = a*(value%q) - r*(value/q);
    while(value <= 0)
      value += m;
    return value;
  }

  // invert c in the finite field (mod m) (m must be prime)
  static IntType invert_euclidian(IntType c)
  {
    // we are interested in the gcd factor for c, because this is our inverse
    BOOST_STATIC_ASSERT(m > 0);
#ifndef BOOST_NO_LIMITS_COMPILE_TIME_CONSTANTS
    BOOST_STATIC_ASSERT(std::numeric_limits<IntType>::is_signed);
#endif
    assert(c > 0);
    IntType l1 = 0;
    IntType l2 = 1;
    IntType n = c;
    IntType p = m;
    for(;;) {
      IntType q = p / n;
      l1 -= q * l2;           // this requires a signed IntType!
      p -= q * n;
      if(p == 0)
        return (l2 < 1 ? l2 + m : l2);
      IntType q2 = n / p;
      l2 -= q2 * l1;
      n -= q2 * p;
      if(n == 0)
        return (l1 < 1 ? l1 + m : l1);
    }
  }
};

// The modulus is exactly the word size: rely on machine overflow handling.
// Due to a GCC bug, we cannot partially specialize in the presence of
// template value parameters.
template<>
class const_mod<unsigned int, 0>
{
  typedef unsigned int IntType;
public:
  static IntType add(IntType x, IntType c) { return x+c; }
  static IntType mult(IntType a, IntType x) { return a*x; }
  static IntType mult_add(IntType a, IntType x, IntType c) { return a*x+c; }

  // m is not prime, thus invert is not useful
private:                      // don't instantiate
  const_mod();
};

template<>
class const_mod<unsigned long, 0>
{
  typedef unsigned long IntType;
public:
  static IntType add(IntType x, IntType c) { return x+c; }
  static IntType mult(IntType a, IntType x) { return a*x; }
  static IntType mult_add(IntType a, IntType x, IntType c) { return a*x+c; }

  // m is not prime, thus invert is not useful
private:                      // don't instantiate
  const_mod();
};

template<>
class const_mod<uint64_t, uint64_t(1) << 48>
{
  typedef uint64_t IntType;
public:
  static IntType add(IntType x, IntType c) { return c == 0 ? x : mod(x+c); }
  static IntType mult(IntType a, IntType x) { return mod(a*x); }
  static IntType mult_add(IntType a, IntType x, IntType c)
    { return mod(a*x+c); }
  static IntType mod(IntType x) { return x &= ((uint64_t(1) << 48)-1); }

  // m is not prime, thus invert is not useful
private:                      // don't instantiate
  const_mod();
};

} // namespace random
} // namespace boost

#endif

