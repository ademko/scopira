
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

#ifndef __INCLUDED__SCOPIRA_TOOL_DISTRANDOM_H__
#define __INCLUDED__SCOPIRA_TOOL_DISTRANDOM_H__

#include <scopira/tool/limits.h>
#include <scopira/tool/linconrandom.h>

namespace scopira
{
  namespace tool
  {
    template <class UniRanGen, class RealType = double>
      class uni01_dist;
    template <class UniRanGen, class RealType = double>
      class unireal_dist;
    template <class UniRanGen, class IntType = int>
      class unismallint_dist;
    template<class UniRanGen, class IntType = int>
      class uniint_dist;

    // "typical" distrbutions
    typedef uni01_dist<lincon_core> lincon_01;
    typedef unireal_dist<lincon_core> lincon_real;
    typedef unismallint_dist<lincon_core> lincon_smallint;  // how small is small? who knows
    typedef uniint_dist<lincon_core> lincon_int;
  }
}

// various random distributions

/**
 * random distribution of [0..1)
 *
 * Because it is so commonly used: uniform distribution on the real [0..1)
 * range.  This allows for specializations to avoid a costly FP division
 *
 * @author Aleksander Demko
 */ 
template <class UniRanGen, class RealType>
class scopira::tool::uni01_dist
{
  public:
    typedef UniRanGen base_type;
    typedef RealType result_type;
    static const bool has_fixed_range = true;

    /// ctor
    explicit uni01_dist(base_type &rng) : m_rng(rng) { }

    /// min
    result_type min() const { return 0.0; }
    /// max
    result_type max() const { return 1.0; }

    result_type operator()(void) {
      return static_cast<result_type>(m_rng() - m_rng.min()) /
        (static_cast<result_type>(m_rng.max()-m_rng.min()) +
         (std::numeric_limits<base_result>::is_integer ? 1.0 : 0.0));
    }

  private:
    typedef typename base_type::result_type base_result;
    base_type & m_rng;
};

/**
 * uniform random real distribution [min..max), internally uses uni01_dist
 *
 * @author Aleksander Demko
 */ 
template <class UniRanGen, class RealType>
class scopira::tool::unireal_dist
{
  public:
    typedef UniRanGen base_type;
    typedef RealType result_type;
    static const bool has_fixed_range = false;

    /// ctor
    unireal_dist(base_type &rng, RealType min, RealType max)
      : m_rng(rng), m_min(min), m_max(max) { }

    /// min
    result_type min(void) const { return m_min; }
    /// max
    result_type max(void) const { return m_max; }

    result_type operator()() { return m_rng() * (m_max - m_min) + m_min; }

  private:
    uni01_dist<base_type, result_type> m_rng;
    RealType m_min, m_max;
};

/**
 * uniform integer distribution on a small range [min, max]
 *
 * @author Aleksander Demko
 */ 
template <class UniRanGen, class IntType>
class scopira::tool::unismallint_dist
{
  public:
    typedef UniRanGen base_type;
    typedef IntType result_type;
    static const bool has_fixed_range = false;

  public:
    /// ctor
    unismallint_dist(base_type &rng, IntType min, IntType max);

    /// min
    result_type min(void) const { return m_min; }
    /// max
    result_type max(void) const { return m_max; }

    /// get next
    result_type operator()(void) {
      // we must not use the low bits here, because LCGs get very bad then
      return long((m_rng() - m_rng.min()) / m_factor) % long(m_range) + m_min;  // MDA  casts used to prevent bad types
    }

  private:
    typedef typename base_type::result_type base_result;
    base_type & m_rng;
    IntType m_min, m_max;
    base_result m_range;
    int m_factor;
};

/// ctor for unismallint_dist
template <class UniRanGen, class IntType>
  scopira::tool::unismallint_dist<UniRanGen, IntType>::unismallint_dist
  (base_type & rng, IntType min, IntType max) 
  : m_rng(rng), m_min(min), m_max(max),
    m_range(static_cast<base_result>(m_max-m_min)+1), m_factor(1)
{
  //assert(min < max);
  
  // LCGs get bad when only taking the low bits.
  // (probably put this logic into a partial template specialization)
  // Check how many low bits we can ignore before we get too much
  // quantization error.
  base_result r_base = m_rng.max() - m_rng.min();
  if (r_base == std::numeric_limits<base_result>::max()) {
    m_factor = 2;
    r_base /= 2;
  }
  r_base += 1;
  if(long(r_base) % long(m_range) == 0) {   // MDA invalid double operators to %, added casts
    // No quantization effects, good
    m_factor = int(double(r_base) / double(m_range));  
  } else {
    //const base_result r = 32 * m_range * m_range;
    //for(; r_base >= r; m_factor *= 2)
      //r_base /= 2;
    // carefully avoid overflow; pessimizing heree
    for ( ; r_base/m_range/32 >= m_range; m_factor *= 2)
      r_base /= 2;
  }
}

template<class UniRanGen, class IntType>
class scopira::tool::uniint_dist
{
  public:
    typedef UniRanGen base_type;
    typedef IntType input_type;
    typedef IntType result_type;
    static const bool has_fixed_range = false;

    explicit uniint_dist(UniRanGen &rng, IntType min, IntType max)
      : m_rng(rng), m_min(min), m_max(max) { m_range = m_max - m_min; }

    /// min
    result_type min(void) const { return m_min; }
    /// max
    result_type max(void) const { return m_max; }
  
    /// get next
    result_type operator()(void);

  private:
    typedef typename base_type::result_type base_result;
    base_type & m_rng;
    IntType m_min, m_max, m_range;
};

template<class UniRanGen, class IntType>
IntType scopira::tool::uniint_dist<UniRanGen, IntType>::operator ()(void)
{
  base_result bmin = (m_rng.min)();
  base_result brange = (m_rng.max)() - (m_rng.min)();

  if (m_range == 0) {
    return m_min;    
  } else if (brange == m_range) {
    // this will probably never happen in real life
    // basically nothing to do; just take care we don't overflow / underflow
    return static_cast<result_type>(m_rng() - bmin) + m_min;
  } else if (brange < m_range) {
    // use rejection method to handle things like 0..3 --> 0..4
    for(;;) {
      // concatenate several invocations of the base RNG
      // take extra care to avoid overflows
      result_type limit;
      if(m_range == (std::numeric_limits<result_type>::max)()) {
        limit = m_range/(result_type(brange)+1);
        if(m_range % result_type(brange)+1 == result_type(brange))
          ++limit;
      } else {
        limit = (m_range+1)/(result_type(brange)+1);
      }
      // We consider "result" as expressed to base (brange+1):
      // For every power of (brange+1), we determine a random factor
      result_type result = result_type(0);
      result_type mult = result_type(1);
      while(mult <= limit) {
        result += (m_rng() - bmin) * mult;
        mult *= result_type(brange)+result_type(1);
      }
      if(mult == limit)
        // m_range+1 is an integer power of brange+1: no rejections required
        return result;
      // m_range/mult < brange+1  -> no endless loop
      result += uniint_dist<UniRanGen, result_type>(m_rng, 0, m_range/mult)() * mult;
      if(result <= m_range)
        return result + m_min;
    }
  } else {                   // brange > range
    if (brange / m_range > 4 /* quantization_cutoff */ ) {
      // the new range is vastly smaller than the source range,
      // so quantization effects are not relevant
      return unismallint_dist<UniRanGen, result_type>(m_rng, m_min, m_max)();
    } else {
      // use rejection method to handle cases like 0..5 -> 0..4
      for(;;) {
        base_result result = m_rng() - bmin;
        // result and range are non-negative, and result is possibly larger
        // than range, so the cast is safe
        if(result <= static_cast<base_result>(m_range))
          return result + m_min;
      }
    }
  }
}

#endif

