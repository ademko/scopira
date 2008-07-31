
/*
 *  Copyright (c) 2005-2006    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#ifndef __INCLUDED_SCOPIRA_AGENT_TASK_H__
#define __INCLUDED_SCOPIRA_AGENT_TASK_H__

#include <scopira/tool/export.h>
#include <scopira/tool/object.h>
#include <scopira/tool/uuid.h>

namespace scopira
{
  namespace agent
  {
    class task_context; //fwd
    // users implement these
    class agent_task_i;
  }
}

/**
 * A simple task that once started, does no communication
 * with other tasks.
 *
 * Basically, the creator instantiates an instance, populates
 * any member variables, and then hands it off to the launch()
 * method.
 *
 * Because these tasks don't do any communication, they may also be checkedpointed
 * and recoverable. That is, they may be restarted at the last saved state if a
 * running process is lost on a dead machine.
 *
 * @author Aleksander Demko
 */ 
class scopira::agent::agent_task_i : public virtual scopira::tool::object
{
  public:
    enum {
      // the task is done, no need to run again
      run_done_c = 0,     // all done

      // pick one of these (whole first 8 bits (0xFF) are reserved for this stuff)
      run_again_0_c = 0x1,    // run again, as soon as possible
      run_again_1_c = 0x2,    // run again, ~1 sec
      run_again_10_c = 0x3,   // run again, ~10 sec
      run_again_100_c = 0x4,  // run again, ~100 sec

      // optional:
      run_canmove_c = 0x100,

      // optional:
      run_onmsg_c = 0x200,
    };

  public:
    /**
     * The run method for the task.
     *
     * The implementation should do some work and return.
     * It should return false if there is no more work to
     * do (ie. return true if the run method should be run
     * again).
     *
     * You should try to return once in awhile through the
     * executation of your task, as scopira can only move tasks
     * between run calls.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT virtual int run(scopira::agent::task_context &ctx) = 0;
};

#endif

