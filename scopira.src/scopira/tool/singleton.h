
/*
 *  Copyright (c) 2004    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#ifndef __INCLUDED_SCOPIRA_TOOL_SINGLETON_H__
#define __INCLUDED_SCOPIRA_TOOL_SINGLETON_H__

#include <assert.h>

namespace scopira
{
  namespace tool
  {
    template <class T> class singleton;
  }
}

/**
 * A singleton helper class.
 *
 * Not very useful (well, useless) for multiple inheritance singletons.
 *
 * This class is depreacted -- do not use.
 * It's more trouble than it's worth under very strict (read gcc3.4)
 * compilers.
 *
 * @author Aleksander Demko
 */
template <class T> class scopira::tool::singleton
{
  protected:
    typedef scopira::tool::singleton<T> singleton_type;

  public:
    /// return instance
    static T * instance(void) { return dm_instance; }

  protected:
    /// ctor
    singleton(T *here) {
      assert(!dm_instance);
      dm_instance = here;
    }
    /// dtor
    ~singleton(void) {
      assert(dm_instance);
      dm_instance = 0;
    }

  private:
    static T * dm_instance; /// the singleton instance
};

#endif

