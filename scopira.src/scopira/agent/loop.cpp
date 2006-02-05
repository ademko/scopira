
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

#include <scopira/agent/loop.h>

#include <scopira/agent/agent.h>
#include <scopira/core/loop.h>

//BBtargets libscopira.so

using namespace scopira::agent;

agent_loop::agent_loop(void)
  : dm_daemon_mode(false)
{
  // start the loop system in another thread
  dm_worker = agent_i::instance();

  if (dm_worker.is_null())
    dm_worker = agent_i::new_agent();

  assert(dm_worker.get());
  assert(agent_i::instance());
}

agent_loop::agent_loop(int &argc, char **&argv)
  : dm_daemon_mode(false)
{
  // start the loop system in another thread
  dm_worker = agent_i::instance();

  if (dm_worker.is_null())
    dm_worker = agent_i::new_agent();

  assert(dm_worker.get());
  assert(agent_i::instance());
}

agent_loop::~agent_loop()
{
  // kill the thread of (if dm_daemon_mode==true), wait
  // until kill msg
  if (dm_daemon_mode)
    dm_worker->wait_stop();
}

void agent_loop::set_wait_for_quit(void)
{
  dm_daemon_mode = true;
}

