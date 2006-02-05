
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

#ifndef __INCLUDED__SCOPIRA_TOOL_RANDOM_H__
#define __INCLUDED__SCOPIRA_TOOL_RANDOM_H__

#include <stdlib.h>
#include <string>

#include <scopira/tool/object.h>
#include <scopira/tool/limits.h>
#include <scopira/tool/export.h>

namespace scopira
{
  namespace tool
  {
    // returns a "random" number, usefull for initing (this
    // is usually time(), so its not random at all.
    // it might have some pid stuff thrown in
    SCOPIRA_EXPORT int time_seed(void);

    class sysdev_gen;
    class sysrand_gen;
    class sysrandom_gen;

    // alternates to distribution objects

    /**
     * Returns an double from [0..max).
     *
     * @author Aleksander Demko
     */ 
    template <class GEN>
      double next_double(GEN &g, double mx) { return mx * (g() - g.min()) / (g.mx() - g.min()); }
    /**
     * Returns an double from [0..1).
     *
     * @author Aleksander Demko
     */ 
    template <class GEN>
      double next_double(GEN &g) { return (g() - g.min()) / (g.mx() - g.min()); }
  }
}

/**
 * a class that connects to an os device to generate non-deterministic
 * random numbers
 *
 * @author Aleksander Demko
 */ 
class scopira::tool::sysdev_gen
{
  public:

    typedef unsigned int result_type;
    
    static const bool has_fixed_range = true;
    //static const result_type min_value = std::numeric_limits<result_type>::min();
    //static const result_type max_value = std::numeric_limits<result_type>::max();
    
    /**
     * Constructor.
     * By default, uses /dev/urandom under linux.
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT explicit sysdev_gen(const std::string& token = default_token);
    /// dtor
    SCOPIRA_EXPORT ~sysdev_gen();

    /// min val
    result_type min(void) const { return std::numeric_limits<result_type>::min(); }
    /// max val
    result_type max(void) const { return std::numeric_limits<result_type>::max(); }

    /**
     * Gets the next number in the sequence.
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT result_type operator()(void);
    /**
     * Gets the next number in the sequence.
     * Capped by mx (that is, <mx).
     * Not numerically sound (FIXME).
     * @author Aleksander Demko
     */ 
    result_type operator()(result_type mx) { return (*this)() % mx; }

  private:
    static const char * const default_token;

    class imp_t;
    imp_t * m_imp;
};

/**
 * a global-state random generato that uses the C libraries
 * srand and rand calls. the follow the boost form
 *
 * @author Aleksander Demko
 */ 
class scopira::tool::sysrand_gen
{
  public:
    typedef int result_type;
    
    static const bool has_fixed_range = true;
    static const int min_value = 0;
    static const int max_value = RAND_MAX;
    
    /// ctor
    SCOPIRA_EXPORT explicit sysrand_gen(unsigned int seed0 = 1);

    /// min val
    result_type min(void) const { return min_value; }
    /// max val
    result_type max(void) const { return max_value; }

    /// seed the generator
    SCOPIRA_EXPORT static void seed(unsigned int seed0);

    SCOPIRA_EXPORT static result_type next(void);

    /**
     * Gets the next number in the sequence.
     * @author Aleksander Demko
     */ 
    result_type operator()(void) { return next(); }
    /**
     * Gets the next number in the sequence.
     * Capped by mx (that is, <mx).
     * Not numerically sound (FIXME).
     * @author Aleksander Demko
     */ 
    result_type operator()(result_type mx) { return (*this)() % mx; }

    // not serializable
};

/**
 * a global-state random generato that uses the C libraries
 * srand and rand calls. the follow the boost form
 *
 * @author Aleksander Demko
 */ 
class scopira::tool::sysrandom_gen
{
  public:
    typedef long int result_type;

    static const bool has_fixed_range = true;
    static const int min_value = 0;
    static const int max_value = RAND_MAX;

    /// ctor
    SCOPIRA_EXPORT explicit sysrandom_gen(unsigned int seed0 = 1);

    /// min val
    result_type min(void) const { return min_value; }
    /// max val
    result_type max(void) const { return max_value; }

    /// seed the generator
    SCOPIRA_EXPORT static void seed(unsigned int seed0);

    SCOPIRA_EXPORT result_type next(void);

    /**
     * Gets the next number in the sequence.
     * @author Aleksander Demko
     */ 
    result_type operator()(void) { return next(); }
    /**
     * Gets the next number in the sequence.
     * Capped by mx (that is, <mx).
     * Not numerically sound (FIXME).
     * @author Aleksander Demko
     */ 
    result_type operator()(result_type mx) { return (*this)() % mx; }

    // not serializable
};

#endif

