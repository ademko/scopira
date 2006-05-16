
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

//BBlibs scopira
//BBtargets scopiraagent.exe

#include <scopira/tool/url.h>
#include <scopira/tool/thread.h>
#include <scopira/tool/util.h>
#include <scopira/core/loop.h>
#include <scopira/agent/loop.h>
#include <scopira/agent/context.h>

class exit_agenterror_reactor : public scopira::agent::agenterror_reactor_i
{
  public:
    virtual void react_agenterror(scopira::agent::agent_i *a)
    { OUTPUT << "FATAL AGENT NETWORK DOWN - scopiraagent.exe TERMINATING\n"; exit(1); }
};

// global
static exit_agenterror_reactor g_exit_reactor;

int main(int argc, char **argv)
{
  scopira::core::basic_loop bl(argc, argv);

  if (!(bl.has_config("network") || bl.has_config("cluster")) && !bl.has_config("task")) {
    OUTPUT << "You need to provide either:\n"
      " task=TASK\n"
      "\n"
      " cluster=server\n"
      " cluster=serveronly\n"
      " cluster=auto\n"
      " cluster=autoclient\n"
      " cluster=SOMEURL\n"
      " cluster=SOMEURL task=TASK\n"
      "         SOMEURL is like scopira://hostname:port/options\n"
      "\n"
      /*" network=auto\n"
      " network=SOMEURL\n"
      " network=SOMEURL task=TASK\n"
      "\n"*/
      " (you also usually have a few lib= params)\n";
    return 1;
  }

  scopira::agent::agent_loop cl(argc, argv);

  if (cl.failed()) {
    OUTPUT << "Failed to start agent\n";
    return 2;
  }
  
  scopira::agent::agent_i::instance()->set_agenterror_reactor(&g_exit_reactor);  // set termination hanlder on fatal exits

  if (bl.has_config("task")) {
    // have a task, run it
    scopira::tool::count_ptr<scopira::tool::object> obj;
    scopira::agent::agent_task_i *t;
    int ret = 1;

    if (!scopira::tool::objflowloader::instance()->has_typeinfo(bl.get_config("task"))) {
      OUTPUT << "Unregistered task type: " << bl.get_config("task") << '\n';
      return 10;
    }

    obj = scopira::tool::objflowloader::instance()->load_object(
        scopira::tool::objflowloader::instance()->get_typeinfo(bl.get_config("task")));
    assert(obj.get());

    t = dynamic_cast<scopira::agent::agent_task_i*>(obj.get());

    if (!t) {
      OUTPUT << "Not a valid agent_task_i: " << bl.get_config("task") << '\n';
      return 11;
    }

    OUTPUT << "Running task=" << bl.get_config("task") << '\n';
    scopira::agent::task_context ctx;

    // finally, run the sucker
    // do this proper?
    while (ret != 0)
      ret = t->run(ctx);
  } else {
    // i dont have a task? lets just wait around then
    cl.set_wait_for_quit();
  }

  // should we be sending a quit signal?
  if (bl.get_config("allquit") == "1") {
    scopira::agent::agent_i::instance()->enqueue_network_quit();
    cl.set_wait_for_quit();
  }

  return 0;
}

