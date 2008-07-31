
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
    /**
     * returns a "random" number, usefull for initing (this
     * is usually time(), so its not random at all.
     * it might have some pid stuff thrown in
     *
     * @author Aleksander Demko
     */ 
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
      double next_double(GEN &g, double mx) { return mx * (g() - g.min()) / (g.max() - g.min()); }
    /**
     * Returns an double from [0..1).
     *
     * @author Aleksander Demko
     */ 
    template <class GEN>
      double next_double(GEN &g) { return (g() - g.min()) / (g.max() - g.min()); }
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

/**
  \page scopiratoolrandompage Random numbers

  \section introsec Introduction

  Scopira includes a collection of random number generation classes that allow for complete control
  over random number generation and distribution. A light wrapper API is also provided for the basic OS
  provided random number generation, including any "real" random number generation services that may be provided.

  However, you should favour the Scopira provided classes for serveral reasons:
    - full source code available
    - portable to all OSes (ie. identical results on all platforms)
    - full control over the seeding process (ie. reproducable runs)

  \section classessec The random classes

  Broken down by header file:

    - scopira/tool/random.h - All random number generators in this header file use global state. You should not use them in your kernels.
      - scopira::tool::sysdev_gen - this is a "true" random number generator, using the facilities of the operating system (/dev/random under Linux).
      - scopira::tool::sysrand_gen - this uses the standard C rand() function.
      - scopira::tool::sysrandom_gen - this uses the standard C random() function.
      - scopira::tool::time_seed() - this is a simply function that will return you a nice starting seed, based on the system clock. Use it to seed your generator, if the user doesn't want an explicit seed. 
    - tool/linconrandom.h - This header contains a compile-time configurable (via template) linear congruential generator, and typedef's two instances with popular properties. They are:
      - scopira::tool::minstd_rand0
      - scopira::tool::minstd_rand 
      You should use any one of those as your random number generators in your kernels.
    - tool/distrandom.h - This header defines distributions. A distribution takes a uniform random number generator and present it as a new distribution.
      - scopira::tool::uni01_dist - real numbers (by default, double), [0..1)
      - scopira::tool::unireal_dist - real numbers (by default, double), [min, max) (specified via the constructor)
      - scopira::tool::unismallint_dist - integer numbers (by default, int), [min, max] (specified via the constructor) 

  \section usagesec Class usage

  All the random number generators and distributions follow a general compile type form - borrowed from boost (boost, boost's random). This makes them fast.

  To use these generators, you maintain an instance of one of the seed-able, local-state generators (currently, either minstd_rand or minstd_rand0). Call this your random generator core.

  Around your random core, you can have one or more distributions. Every time you ask them for a number, they ask the core then massage it to fit their distributions.

  All random generators and distributions provide you with various methods. min() and max() return just that. operator() returns you the next number in the sequence.

  \section examplesec Example

  Here is an example:

  \code
  #include <scopira/tool/random.h>
  #include <scopira/tool/linconrandom.h>

  void foo(void) {
    scopira::tool::lincon_core rcore;          // the typical code

    rcore.seed(time_seed());                   // seed it with a "random" value

    scopira::tool::lincon_01 dist(rcore);      // wrap a standard 0..1 distributor around the core
                                               // note that you can have multi distribution objects around the same
                                               // and that you can also simply make distribution objects as needed
                                               // (distribution objects have no state themselves)

    for (int x=0; x<5; ++x)
      OUTPUT << ' ' << dist();                 // the () operator invokes the distribution to get the next number
  }
  \endcode

*/

#endif

