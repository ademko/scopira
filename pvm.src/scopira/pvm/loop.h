
/*
 *  Copyright (c) 2006    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#ifndef __INCLUDED_SCOPIRA_PVM_LOOP_H__
#define __INCLUDED_SCOPIRA_PVM_LOOP_H__

#include <pvm3.h>

namespace scopira
{
  namespace pvm
  {
    class pvm_session;
    class pvm_loop;
  }
}

/**
 * A pvm_session is like a pvm_loop.
 * Basically, a GUI (or other non-pvm-only) application has a pvm_session
 * object instantiated for the time it needs a pvm task id, etc.
 *
 * You can only have one pvm_session active at any one time.
 * If you open a second, etc pvm_session, then it will return true
 * for failed().
 *
 * @author Aleksander Demko
 */
class scopira::pvm::pvm_session
{
  public:
    /// ctor
    pvm_session(void);
    /// dtor
    ~pvm_session();

    bool failed(void) const { return dm_failed; }

  private:
    bool dm_failed;

    static pvm_session * dm_instance;   // just to make sure there is only one session, ever
};

/**
 * Constructors the PVM stuff.
 *
 * You can do this after loop times.
 *
 * @author Aleksander Demko
 */
class scopira::pvm::pvm_loop
{
  public:
    /**
     * Joins the PVM system.
     *
     * @author Aleksander Demko
     */
    pvm_loop(void);

    /**
     * Upon destruction, this will exit the pvm system.
     *
     * @author Aleksander Demko
     */
    ~pvm_loop();

    /**
     * Returns the static/singleton instance. May return null.
     *
     * @author Aleksander Demko
     */
    static pvm_loop * instance(void) { return dm_instance; }

  private:
    static pvm_loop * dm_instance;
};

#endif

