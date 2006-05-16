
/*
 *  Copyright (c) 2005    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#ifndef __INCLUDED_SCOPIRA_MATLAB_LOOP_H
#define __INCLUDED_SCOPIRA_MATLAB_LOOP_H

#include <scopira/lab/loop.h>

namespace scopira
{
  namespace matlab
  {
    template <class loopT> class shared_loop;

    class matlab_lab_loop;
  }
}

/**
 * A utility class for keeping shared, static holds on a loop of some kind.
 * Usefull when you need loop, but don't have a central main-like function
 * to instantiate it in (common when making, for example, matlab functions).
 *
 * Basically, each library declares this once somewhere as a static.
 *
 * Not really used right now.
 *
 * @author Aleksander Demko
 */ 
template <class loopT> class scopira::matlab::shared_loop
{
  private:
    scopira::tool::count_ptr<loopT> dm_loop;
  public:
    /// ctor
    shared_loop(void) {
      int count = 0;
      char **c = 0;
      if (loopT::instance())
        dm_loop = loopT::instance();
      else
        dm_loop = new loopT(count, c);
      assert(dm_loop.get());
    }
};

/**
 * A handle object that is used to share a common lab_loop among many
 * libraries, intended for matlab MEX library/functions.
 *
 * Each MEX library should have one static instance of this. Make sure
 * to pass true to the constructor if that particular needs a gui.
 *
 * @author Aleksander Demko
 */ 
class scopira::matlab::matlab_lab_loop
{
  private:
    scopira::tool::count_ptr<scopira::lab::lab_loop> dm_loop;
  public:
    /// ctor
    matlab_lab_loop(bool needgui = false);
    /// dtor
    ~matlab_lab_loop();
};

#endif

