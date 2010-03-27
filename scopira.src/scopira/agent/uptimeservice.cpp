
/*
 *  Copyright (c) 2007-2010    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#include <scopira/agent/uptimeservice.h>

#include <scopira/tool/output.h>
#include <scopira/core/register.h>
#include <scopira/agent/ids.h>
#include <scopira/agent/register.h>

#include <scopira/tool/thread.h>

//BBtargets libscopira.so

using namespace scopira::tool;
using namespace scopira::agent;

/// the service type id for the uptime service
static scopira::tool::uuid service_type_id("ea50cfcb-24d0-47b9-8d02-98b4556058c9");

uptime_service::uptime_service(scopira::agent::task_context &ctx)
  : dm_ctx(ctx)
{
  scopira::basekit::narray<scopira::tool::uuid> agents;

  ctx.find_services(worker_agent_service_c, agents);

OUTPUT << "Launching: " << agents.size() << '\n';
  dm_servers.resize(agents.size());
  for (int x=0; x<agents.size(); ++x)
    dm_servers[x] = ctx.launch_task("scopira::agent::uptime_task", agents[x]);

  // and grab their agent locations
  update_uptime();

  assert(!dm_servers.empty());

OUTPUT << "SERVERS: " << dm_servers << '\n';
}

uptime_service::~uptime_service()
{
  for (int x=0; x<dm_servers.size(); ++x)
    dm_ctx.kill_task(dm_servers[x]);
}

void uptime_service::update_uptime(void)
{
OUTPUT << "update_uptime-start\n";
  std::string k, v;

  k.reserve(128);
  v.reserve(128);

  dm_values.clear();

  dm_values.resize(dm_servers.size());

  for (int x=0; x<dm_servers.size(); ++x) {
OUTPUT << "update_uptime::sent to " << dm_servers[x] << '\n';
assert(dm_ctx.is_alive_task(dm_servers[x]));
    {
      send_msg M(dm_ctx, dm_servers[x]);

      M.write_int(100); // request all keys and values
    }
OUTPUT << "update_uptime::sent to done\n";

    // process the reply, if any
    if (dm_ctx.wait_msg(dm_servers[x], 5000)) {
OUTPUT << "Recved!\n";
      recv_msg M(dm_ctx, dm_servers[x]);

      while (M.read_string(k) && M.read_string(v) && !k.empty())
        dm_values[x][k] = v;
    }//if recv_msg
  }//for each server
OUTPUT << "update_uptime-end\n";
}

static std::string bad_agent_c("BAD_AGENT");
static std::string bad_key_c("BAD_KEY");

const std::string & uptime_service::get_value(int agentnum, const std::string &key)
{
  if (agentnum<0 || agentnum>=dm_values.size())
    return bad_agent_c;

  agent_values &v = dm_values[agentnum];
  agent_values::iterator ii;

  ii = v.find(key);
  if (ii == v.end())
    return bad_key_c;

  return ii->second;
}

//
//
// uptime_task
//
//

#include <scopira/agent/localagent.h>
#include <scopira/agent/clusteragent.h>

namespace scopira
{
  namespace agent
  {
    class uptime_task;
  }
}

/**
 * This is a service task (managed by uptime_service) that needs intimate 
 * access to the cluster_agent and local_agent internals.
 *
 * @author Aleksander Demko
 */ 
class scopira::agent::uptime_task : public scopira::agent::agent_task_i
{
  private:
    local_agent *dm_la;
    cluster_agent *dm_ca;
  public:
    uptime_task(void);
    virtual int run(scopira::agent::task_context &ctx);
};

static scopira::core::register_flow<uptime_task> r519843336("scopira::agent::uptime_task");
static scopira::agent::register_service r519843336SS("scopira::agent::uptime_task", service_type_id);

uptime_task::uptime_task(void)
{
  dm_la = dynamic_cast<local_agent*>(agent_i::instance());
  dm_ca = dynamic_cast<cluster_agent*>(agent_i::instance());
  assert(dm_la);
}

int uptime_task::run(scopira::agent::task_context &ctx)
{
OUTPUT << "uptime_task::run as " << ctx.get_id() << '\n';
  if (!ctx.has_msg(uuid()))
    return run_onmsg_c;

  // i have a message, lets process it
OUTPUT << "uptime_task::have msg!\n";
  
  {
    // consume the msg
    recv_msg M(ctx, ctx.get_source());
  }

  // send them all my keys
  {
    scopira::agent::send_msg M(ctx, ctx.get_source());

    if (dm_ca) {
      M.write_string("keylist");
      M.write_string("hostname numcpu agentuuid ");

      M.write_string("hostname");
      M.write_string(dm_ca->dm_nodespec.pm_url.get_hostname());
    } else {
      M.write_string("keylist");
      M.write_string("numcpu agentuuid");
    }
    // common to both local and cluster
    M.write_string("numcpu");
    M.write_string(int_to_string(dm_la->la_get_spec().pm_numcpu));
    M.write_string("agentuuid");
    M.write_string(dm_la->get_agent_id().as_string());
  }

  return run_onmsg_c;
}

//
//
// ************** TESTING STUFF *******************
//
//


/*
class test_task : public scopira::agent::agent_task_i
{
  public:
    virtual int run(scopira::agent::task_context &ctx);
};

static scopira::core::register_flow<test_task> r3("test_task");

int test_task::run(scopira::agent::task_context &ctx)
{
  uptime_service us(ctx);

  int N = us.get_num_agents();

  OUTPUT << "Agents: " << N << '\n';
  
  for (int x=0; x<N; ++x) {
    OUTPUT << "Agent     : #" << x << '\n';
    OUTPUT << "numcpu    : " << us.get_value(x, "numcpu") << '\n';
    OUTPUT << "agent id  : " << us.get_value(x, "agentuuid") << '\n';
    OUTPUT << "hostname  : " << us.get_value(x, "hostname") << '\n';
  }

  return 0;
}
*/
