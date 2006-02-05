
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

#include <scopira/agent/context.h>

#include <scopira/tool/output.h>
#include <scopira/tool/objflowloader.h>

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
  dm_buf = new scopira::tool::bufferflow;
  dm_buf->reserve(1024*32);

  open(dm_buf.get());
}

send_msg::send_msg(scopira::agent::task_context &ctx, int dest)
  : bin64oflow(true, 0), dm_src(ctx.dm_peers[ctx.dm_myindex]), dm_dest(dest == -1 ? scopira::tool::uuid() : ctx.dm_peers[dest])
{
  dm_buf = new scopira::tool::bufferflow;
  dm_buf->reserve(1024*32);

  open(dm_buf.get());
}

send_msg::~send_msg()
{
  // ok, now the send thing
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
  agent_i::instance()->recv_msg(uuid_query(src == -1 ? scopira::tool::uuid() : ctx.dm_peers[src]),
      dm_lastsrc, dm_dest, dm_realbuf);

  assert(dm_realbuf.get());

  dm_buf = new scopira::tool::bufferiflow(dm_realbuf->c_array(), dm_realbuf->size());

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

scopira::tool::uuid task_context::launch_task(const std::string &classname)
{
  assert(objflowloader::instance()->has_typeinfo(classname));
  return agent_i::instance()->launch_task(objflowloader::instance()->get_typeinfo(classname));
}

scopira::tool::uuid task_context::launch_group(int numps, const std::string &classname)
{
  assert(objflowloader::instance()->has_typeinfo(classname));
  return agent_i::instance()->launch_group(numps, objflowloader::instance()->get_typeinfo(classname));
}

void task_context::launch_slaves(int numps, const std::string &classname)
{
  assert(objflowloader::instance()->has_typeinfo(classname));
  launch_slaves_impl(numps, objflowloader::instance()->get_typeinfo(classname));
}

bool task_context::wait_msg(scopira::tool::uuid src, int timeout)
{
  return agent_i::instance()->wait_msg(uuid_query(src), dm_lastsrc, dm_peers[dm_myindex], timeout);
}

bool task_context::wait_msg(int src, int timeout)
{
  return agent_i::instance()->wait_msg(uuid_query(src == -1 ? scopira::tool::uuid() : dm_peers[src]), dm_lastsrc, dm_peers[dm_myindex], timeout);
}

bool task_context::wait_msg(const uuid_query &Q, int timeout)
{
  return agent_i::instance()->wait_msg(Q, dm_lastsrc, dm_peers[dm_myindex], timeout);
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

  assert(false && "[bad id fed to get_index()]\n");
  return -1;
}

void task_context::barrier_group(void)
{
  if (get_index() == 0) {
    // wait for each slaves ok msg
    for (int x=1; x<get_index_size(); ++x) {
      recv_msg r(*this, x);
      int dummy;
      r.read_int(dummy);
    }
    for (int x=1; x<get_index_size(); ++x) {
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
  for (int x=1; x<get_index_size(); ++x)
    if (x != get_index())
      wait_task(dm_peers[x]);
}

