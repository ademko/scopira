
/*
 *  Copyright (c) 2005-2008    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#include <scopira/agent/context.h>

#include <scopira/tool/output.h>
#include <scopira/tool/objflowloader.h>
#include <scopira/tool/time.h>

//BBtargets libscopira.so

using namespace scopira::tool;
using scopira::basekit::narray;
using namespace scopira::agent;

//
//
// send_msg
//
//

send_msg::send_msg(scopira::agent::task_context &ctx, scopira::tool::uuid dest)
  : bin64oflow(true, 0), dm_src(ctx.dm_peers[ctx.dm_myindex]), dm_dest(dest)
{
  assert(!dest.is_zero() && "[send_msg cannt take a zero-uuid]\n");
  // broadcast not supported
  // perhaps do a list-based or query based broadcast system?
  // similar to the service broadcast, or remove this all together and stick
  // with the simple list based system

  dm_buf = new scopira::tool::bufferflow;
  dm_buf->reserve(1024*32);

  open(dm_buf.get());

  dm_service_bcast = false;
}

send_msg::send_msg(scopira::agent::task_context &ctx, int dest)
  : bin64oflow(true, 0), dm_src(ctx.dm_peers[ctx.dm_myindex]), dm_dest(dest == -1 ? scopira::tool::uuid() : ctx.dm_peers[dest])
{
  assert(dest>=0 && dest<ctx.dm_peers.size() && "[bad dest index sent to send_msg]\n");
  // broadcast not supported
  // perhaps do a list-based or query based broadcast system?
  // similar to the service broadcast, or remove this all together and stick
  // with the simple list based system

  dm_buf = new scopira::tool::bufferflow;
  dm_buf->reserve(1024*32);

  open(dm_buf.get());

  dm_service_bcast = false;
}

send_msg::send_msg(scopira::agent::task_context &ctx, const service_broadcast &targets)
  : bin64oflow(true, 0), dm_src(ctx.dm_peers[ctx.dm_myindex]), dm_dest(targets.get_serviceid())
{
  dm_buf = new scopira::tool::bufferflow;
  dm_buf->reserve(1024*32);

  open(dm_buf.get());

  dm_service_bcast = true;
}

send_msg::~send_msg()
{
  // ok, now the send thing
  if (dm_service_bcast)
    agent_i::instance()->send_msg_bcast(dm_src, dm_dest, dm_buf.get());
  else
    agent_i::instance()->send_msg(dm_src, dm_dest, dm_buf.get());
}

//
//
// recv_msg
//
//

recv_msg::recv_msg(scopira::agent::task_context &ctx, scopira::tool::uuid src)
  : bin64iflow(true, 0), dm_dest(ctx.dm_peers[ctx.dm_myindex])
{
  agent_i::instance()->recv_msg(uuid_query(src), dm_lastsrc, dm_dest, dm_realbuf);

  assert(dm_realbuf.get());

  dm_buf = new scopira::tool::bufferiflow(dm_realbuf->c_array(), dm_realbuf->size());

  open(dm_buf.get());
}

recv_msg::recv_msg(scopira::agent::task_context &ctx, int src)
  : bin64iflow(true, 0), dm_dest(ctx.dm_peers[ctx.dm_myindex])
{
  uuid_query q;

  if (src == -1)
    q.match_group(ctx.dm_peers);
  else
    q.match_one(ctx.dm_peers[src]);

  agent_i::instance()->recv_msg(q,
      dm_lastsrc, dm_dest, dm_realbuf);

  assert(dm_realbuf.get());

  // c_array() crashes if size is 0, so we need a check 
  if (dm_realbuf->size() > 0)
    dm_buf = new scopira::tool::bufferiflow(dm_realbuf->c_array(), dm_realbuf->size());
  else
    dm_buf = new scopira::tool::bufferiflow(0, 0);

  open(dm_buf.get());
}

recv_msg::recv_msg(scopira::agent::task_context &ctx, const uuid_query &Q)
  : bin64iflow(true, 0), dm_dest(ctx.dm_peers[ctx.dm_myindex])
{
  agent_i::instance()->recv_msg(Q, dm_lastsrc, dm_dest, dm_realbuf);

  assert(dm_realbuf.get());

  dm_buf = new scopira::tool::bufferiflow(dm_realbuf->c_array(), dm_realbuf->size());

  open(dm_buf.get());
}


recv_msg::~recv_msg()
{
  // nothin for now.
}

//
//
// task_context
//
//

task_context::task_context(void)
{
  dm_server_link = agent_i::instance();

  if (dm_server_link.is_null())
    dm_server_link = agent_i::new_agent();

  assert(dm_server_link.get());

  dm_myindex = 0;
  dm_peers.resize(1);

  // only userland created contexts are registered, this is because
  // ones in the agent are already bound to a task
  dm_server_link->reg_context(dm_peers[dm_myindex], 0);
  assert(!dm_peers[dm_myindex].is_zero());
}

task_context::task_context(taskmsg_reactor_i *reac)
{
  dm_server_link = agent_i::instance();

  if (dm_server_link.is_null())
    dm_server_link = agent_i::new_agent();

  assert(dm_server_link.get());

  dm_myindex = 0;
  dm_peers.resize(1);

  // only userland created contexts are registered, this is because
  // ones in the agent are already bound to a task
  assert(reac);
  dm_server_link->reg_context(dm_peers[dm_myindex], reac);
  assert(!dm_peers[dm_myindex].is_zero());
}

task_context::task_context(int myindex, const narray<uuid> &peers)
  : dm_peers(peers), dm_myindex(myindex)
{
  assert(!dm_peers[dm_myindex].is_zero());
}

task_context::~task_context()
{
  assert(dm_myindex<dm_peers.size());
  assert(!dm_peers.empty());
  assert(!dm_peers[dm_myindex].is_zero());
  if (dm_server_link.get())
    dm_server_link->unreg_context(dm_peers[dm_myindex]);
}

int task_context::find_services(scopira::tool::uuid &serviceid, scopira::basekit::narray<scopira::tool::uuid> &out)
{
  return agent_i::instance()->find_services(serviceid, out);
}

scopira::tool::uuid task_context::launch_task(const std::string &classname, scopira::tool::uuid where)
{
  assert(objflowloader::instance()->has_typeinfo(classname));
  return agent_i::instance()->launch_task(objflowloader::instance()->get_typeinfo(classname), where);
}

scopira::tool::uuid task_context::launch_group(int numps, const std::string &classname)
{
  assert(objflowloader::instance()->has_typeinfo(classname));
  return agent_i::instance()->launch_group(numps, objflowloader::instance()->get_typeinfo(classname));
}

void task_context::launch_slaves(int numps, const std::string &classname)
{
  assert(objflowloader::instance()->has_typeinfo(classname) && "[Given launch_slaves() class string is not registered]");
  launch_slaves_impl(numps, objflowloader::instance()->get_typeinfo(classname));
}

bool task_context::wait_msg(scopira::tool::uuid src, int timeout)
{
  return agent_i::instance()->wait_msg(uuid_query(src), dm_lastsrc, dm_peers[dm_myindex], timeout);
}

bool task_context::wait_msg(int src, int timeout)
{
  uuid_query q;

  if (src == -1)
    q.match_group(dm_peers);
  else
    q.match_one(dm_peers[src]);

  return agent_i::instance()->wait_msg(q, dm_lastsrc, dm_peers[dm_myindex], timeout);
}

bool task_context::wait_msg(const uuid_query &Q, int timeout)
{
  return agent_i::instance()->wait_msg(Q, dm_lastsrc, dm_peers[dm_myindex], timeout);
}

bool task_context::has_msg(scopira::tool::uuid src)
{
  // -7734 is the "has_msg" code
  return agent_i::instance()->wait_msg(uuid_query(src), dm_lastsrc, dm_peers[dm_myindex], -7734);
}

bool task_context::has_msg(int src)
{
  uuid_query q;

  if (src == -1)
    q.match_group(dm_peers);
  else
    q.match_one(dm_peers[src]);

  // -7734 is the "has_msg" code
  return agent_i::instance()->wait_msg(q, dm_lastsrc, dm_peers[dm_myindex], -7734);
}

bool task_context::has_msg(const uuid_query &Q)
{
  // -7734 is the "has_msg" code
  return agent_i::instance()->wait_msg(Q, dm_lastsrc, dm_peers[dm_myindex], -7734);
}

void task_context::launch_slaves_impl(int numtotalps, const std::type_info &nfo)
{
  assert(dm_peers.size() == 1);

  agent_i::instance()->launch_slaves(dm_peers[dm_myindex], numtotalps, nfo, dm_peers);

  assert(dm_peers.size() > 0);

  assert(dm_myindex == 0);
  assert(!dm_peers.empty());
  assert(!dm_peers[dm_myindex].is_zero());
}

int task_context::get_index(scopira::tool::uuid id) const
{
  for (int x=0; x<dm_peers.size(); ++x)
    if (dm_peers[x] == id)
      return x;

  return -1;
}

void task_context::barrier_group(void)
{
  // this stuff should be out of band or something in the future
  if (get_index() == 0) {
    // wait for each slaves ok msg
    for (int x=1; x<get_group_size(); ++x) {
      recv_msg r(*this, x);
      int dummy;
      r.read_int(dummy);
    }
    for (int x=1; x<get_group_size(); ++x) {
      send_msg s(*this, x);
      s.write_int(11);
    }
  } else {
    // send my ok msg
    {
      send_msg s(*this, 0);
      s.write_int(10);
    }
    // now wait for the trigger
    {
      recv_msg r(*this, 0);
      int x;
      r.read_int(x);
    }
    // go!
  }
}

void task_context::wait_group(void)
{
  for (int x=1; x<get_group_size(); ++x)
    if (x != get_index())
      wait_task(dm_peers[x]);
}

//
//
// run_task
//
//


/**
 * This function will run the given task by calling
 * it's run() method. it will properly analysis its
 * return codes and possible call the run method repeadedly
 * until the task is fininished or killed.
 *
 * @author Aleksander Demko
 */
void scopira::agent::run_task(scopira::agent::task_context &ctx, scopira::agent::agent_task_i &t)
{
  int retcode;
  chrono timer;
  double nexttime = 0;
  bool ontime;

  while (true) {
    retcode = t.run(ctx);

    // check if they want to quit
    if (retcode == 0)
      return;

    // check if they want an immediate rerun
    if ((retcode & 0xFF) == agent_task_i::run_again_0_c)
      continue;

    ontime = true;
    switch (retcode & 0xFF) { // lower 8 bits are for run next
      case agent_task_i::run_again_1_c: nexttime = 1; break;
      case agent_task_i::run_again_10_c: nexttime = 10; break;
      case agent_task_i::run_again_100_c: nexttime = 100; break;
      default: ontime = false; break;
    }

    // ok, guess we're doing more complex waiting
    if (ontime) {
      timer.stop();
      timer.reset();
      timer.start();
    }
    while (true) {
      if (ctx.wait_msg(uuid(), 1000) && retcode & agent_task_i::run_onmsg_c)
        break; // a msg came that the task was waiting for. break this inner loop and start the next run
      if (ontime && timer.get_running_time() >= nexttime)
        break;  // time out happened
    }//while (inner/waiting)
  }//while (outter)
}

