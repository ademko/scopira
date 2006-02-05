
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

#ifndef __INCLUDED_SCOPIRA_AGENT_TASK_H__
#define __INCLUDED_SCOPIRA_AGENT_TASK_H__

#include <scopira/tool/export.h>
#include <scopira/tool/object.h>

namespace scopira
{
  namespace agent
  {
    /**
     * Parse and return the numtask= config variable, from basic_loop.
     * You should only really call this in your id=0 or master threads.
     *
     * Returns 0 on failure or if the parameter was not provided.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT int get_config_numtask(void);

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
      run_done_c = 0,     // all done
      run_again_c,        // run again, whenever
      run_again_immovable_c,// run again, but do NOT move the process (it still needs to be registered, but doesnt nee save/load)
      run_sleep_c,       // run when a msg arrives
      //run_sync_c,     // run all group memebers at the same time
      // what about checkpoint-able processes? different launch type and resttricted communications?
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

