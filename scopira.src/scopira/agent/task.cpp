
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
#include <scopira/agent/task.h>

#include <scopira/tool/util.h>
#include <scopira/core/register.h>
#include <scopira/core/loop.h>

//BBtargets libscopira.so

static scopira::core::register_object<scopira::agent::agent_task_i>
  r1("scopira::agent::agent_task_i", 0, 1);


int scopira::agent::get_config_numtask(void)
{
  int numtask = 0;
  if (scopira::core::basic_loop::instance()->has_config("numtask") &&
    !scopira::tool::string_to_int(scopira::core::basic_loop::instance()->get_config("numtask"), numtask))
      numtask = 0;
  return numtask;
}

