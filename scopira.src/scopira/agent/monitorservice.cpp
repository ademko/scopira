
/*
 *  Copyright (c) 2006-2007    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#include <scopira/agent/monitorservice.h>

#include <map>
#include <list>

#include <scopira/tool/output.h>
#include <scopira/tool/time.h>
#include <scopira/tool/thread.h>
#include <scopira/core/register.h>
#include <scopira/agent/ids.h>
#include <scopira/agent/register.h>

//BBtargets libscopira.so

using namespace scopira::tool;
using namespace scopira::agent;

/// the service type id for the monitor service
static scopira::tool::uuid service_type_id("b1602881-7830-4cc7-bd59-b25911a639bd");

namespace scopira
{
  namespace agent
  {
    class monitor_task;
  }
}

/// the monitor task implementation
class scopira::agent::monitor_task : public scopira::agent::agent_task_i
{
  public:
    monitor_task(void);
    virtual ~monitor_task();
    virtual int run(scopira::agent::task_context &ctx);

  private:
    struct action_t {
      bool killonwatch;  // in the future, allow kill/notify (how, msg, tag, etc)
      action_t(void) { }
      action_t(bool _killonwatch) : killonwatch(_killonwatch) { }
    };
    typedef std::map<scopira::tool::uuid, action_t> watchers_t;
    typedef std::map<scopira::tool::uuid, watchers_t > watchlist_t;

    watchlist_t dm_watches;

    chrono dm_tick;
};

static scopira::core::register_flow<scopira::agent::monitor_task> r1("scopira::agent::monitor_task");
static scopira::agent::register_service r1s("scopira::agent::monitor_task", service_type_id);

//
//
// monitor_service (the service pack)
//
//

monitor_service::monitor_service(scopira::agent::task_context &ctx)
  : dm_ctx(ctx)
{
//OUTPUT << "SERIVCE-ID: " << service_type_id << '\n';
  scopira::basekit::narray<uuid> ids;

  dm_ctx.find_services(service_type_id, ids);

  if (ids.empty())
    dm_server = dm_ctx.launch_task("scopira::agent::monitor_task", where_master_c);
  else
    dm_server = ids[0];

//OUTPUT << "Monitor server new=" << ids.empty() << " id=" << dm_server << '\n';
}

void monitor_service::add_watch(scopira::tool::uuid whotowatch, scopira::tool::uuid receipient, bool killonwatch)
{
  send_msg M(dm_ctx, dm_server);

  M.write_int(100); // code fore add a watch
  whotowatch.save(M);
  receipient.save(M);
  M.write_bool(killonwatch);
}

void monitor_service::remove_watch(scopira::tool::uuid whotowatch, scopira::tool::uuid receipient)
{
  send_msg M(dm_ctx, dm_server);

  M.write_int(101); // code fore remove a watch
  whotowatch.save(M);
  receipient.save(M);
}

//
//
// monitor_task
//
//

monitor_task::monitor_task(void)
{
//OUTPUT << "monitor_task::constructor\n";
  dm_tick.start();
}

monitor_task::~monitor_task()
{
//OUTPUT << "monitor_task::destructor...\n";
}

int monitor_task::run(scopira::agent::task_context &ctx)
{
//OUTPUT << "monitor_task(" << ctx.get_id() << ")::run start\n";

  // convert this to be multi-run() called based in the future?
  while (ctx.has_msg(uuid())) {
    // check if we should die now
    // got a msg, process it
    int cmd;
    uuid whotowatch, receipient;
    recv_msg M(ctx, uuid());

    M.read_int(cmd);
    switch (cmd) {
      case 100:
      case 101: {
                  // get the params
                  whotowatch.load(M);
                  receipient.load(M);

                  // finally, process the command
                  if (cmd == 100) {
                    bool killonwatch;
                    M.read_bool(killonwatch);
                    // add a watch
                    dm_watches[whotowatch][receipient] = action_t(killonwatch);
                  } else {
                    // remove a watch
                    watchlist_t::iterator ii = dm_watches.find(whotowatch);
                    watchers_t::iterator jj;

                    if (ii == dm_watches.end())
                      break;

                    jj = ii->second.find(receipient);

                    if (jj == ii->second.end())
                      break;

                    // finally remove it
                    ii->second.erase(jj);
                    if (ii->second.empty())
                      dm_watches.erase(ii);
                  }
                } break;
    }
  }//while has_msg
  
  if (dm_tick.get_running_time() > 10) {    //FIXME put this back to 60 eventually
   // timeout + periodic heart beat
    watchlist_t::iterator ii;
    std::list<uuid> deadlist;
    std::list<uuid>::iterator jj;

    // find all the dead tasks
    for (ii=dm_watches.begin(); ii!=dm_watches.end(); ++ii)
      if (!ctx.is_alive_task(ii->first))
        deadlist.push_back(ii->first);

    // now, notify all the listeners and remove them from the list
    for (jj=deadlist.begin(); jj!=deadlist.end(); ++jj) {
      watchers_t::iterator kk;
      // notify listeners
      ii = dm_watches.find(*jj);
      for (kk=ii->second.begin(); kk != ii->second.end(); ++kk) {
//OUTPUT << "monitor_task: killing/notifing " << kk->first << '\n';
        if (kk->second.killonwatch)
          ctx.kill_task(kk->first);
        else {
          send_msg M(ctx, kk->first);
          // send them a message, with the watched ID
          M.write_int(1100);
          jj->save(M);
        }
      }

      // remove it
      dm_watches.erase(ii);
    }

    // ok, reset the timer
    dm_tick.stop();
    dm_tick.reset();
    dm_tick.start();
  }//if get_running_time

//OUTPUT << "monitor_task::run end\n";
  return run_again_10_c | run_onmsg_c;   // FIXME switch this to _10_c from _1_c
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

class test_slave_task : public scopira::agent::agent_task_i
{
  public:
    virtual int run(scopira::agent::task_context &ctx);
};

static scopira::core::register_flow<test_task> r3("test_task");
static scopira::core::register_flow<test_slave_task> r3b("test_slave_task");

int test_task::run(scopira::agent::task_context &ctx)
{
OUTPUT << "MASTER BEGIN\n";
  monitor_service S(ctx);
  uuid subtask;

  subtask = ctx.launch_task("test_slave_task", where_master_c);

  S.add_watch(subtask, ctx.get_id());

OUTPUT << "master waiting\n";
  ctx.wait_msg(uuid());
  //thread::sleep(20000);
OUTPUT << "MASTER END\n";
  return 0;
}

int test_slave_task::run(scopira::agent::task_context &ctx)
{
OUTPUT << "SLAVE-BEGIN\n";
  thread::sleep(5000);
OUTPUT << "SLAVE-END\n";
  return 0;
}

*/

