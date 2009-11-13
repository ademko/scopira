
/*
 *  Copyright (c) 2009    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#ifndef __INCLUDED_SCOPIRA_AGENT_TIMESLICE_H__
#define __INCLUDED_SCOPIRA_AGENT_TIMESLICE_H__

#include <scopira/tool/time.h>

namespace scopira
{
  namespace agent
  {
    class timeslice;
    class global_timeslice;
    class subtimeslice;
    class simple_timeslice;
  }
}

/**
 * A RAII timing object to aid in code performance profiling.
 * This will emit a report upon destruction.
 *
 * @author Aleksander Demko
 */
class scopira::agent::timeslice
{
  public:
    /**
     * Basic constructor. Will report time upon destruction.
     *
     * @param prefix the output prefix to print when printing the report
     * If this is null, no report will be printed (useful to disable a timeslice)
     * @author Aleksander Demko
     */
    timeslice(const char * prefix);

    /// destructor
    ~timeslice();

  private:
    static const int num_timers = 5;
    double dm_numerators[num_timers];
    const char *dm_prefix;
    
    scopira::tool::chrono dm_chrono;

    friend class scopira::agent::subtimeslice;
};

/**
 * A global instance of a timeslice. You can only ever have one of these active
 * at any time (the 2nd constructor of subtimeslice is able to utilize this).
 *
 * @author Aleksander Demko
 */ 
class scopira::agent::global_timeslice : public scopira::agent::timeslice
{
  public:
    /// constructor
    global_timeslice(const char * prefix = "global_timeslice");
    /// destructor
    ~global_timeslice();

    /// returns the global static instance
    static global_timeslice * instance(void) { return dm_instance; }

  private:
    static global_timeslice * dm_instance;
};

/**
 * This is able to add to one of the internal sub-reports of a timeslice.
 * There are timeslice::num_timers, and you refer to one of them by passing
 * a index.
 *
 * @author Aleksander Demko
 */ 
class scopira::agent::subtimeslice
{
  public:
    /**
     * A constructor that uses the given timeslice instance.
     *
     * @author Aleksander Demko
     */ 
    subtimeslice(timeslice &parent, int index);
    /**
     * A constructor that uses the global_timeslice instance
     * (useful if passing a timeslice would be messy, code-wise)
     *
     * @author Aleksander Demko
     */ 
    subtimeslice(int index);
    /// destructor
    ~subtimeslice();
  private:
    timeslice *dm_parent;
    int dm_index;
    double dm_starttime;
};

/**
 * This is a simply RAII that emits how long it was alive for during destruction.
 *
 * @author Aleksander Demko
 */ 
class scopira::agent::simple_timeslice
{
  public:
    /// constructor, starts the time
    simple_timeslice(void);
    /// destructor, stops the emits the time elapsed since construction
    ~simple_timeslice();

  private:
    scopira::tool::chrono dm_chrono;
};

#endif

