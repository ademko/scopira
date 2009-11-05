
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

#include <stdlib.h>

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

  if (!(bl.has_config("clusterquit") || bl.has_config("cluster")) && !bl.has_config("task")) {
    OUTPUT <<
      "Load one or more plug-in libraries with:\n"
      " lib=path/to/libplugin.so\n"
      "\n"
      "To use the auto cluster boot facility:\n"
      " clusterboot=macchine,list   (auto run a cluster with the given machines as the slave list)\n"
      " clusterboot_ssh=sshclient   (use the given sshclient. \"ssh\" is the default)\n"
      " clusterboot_exe=agentclient (use the given sshagent client. the current executable is the default)\n"
      "\n"
      "To join or setup a network-based agent system, provide one of:\n"
      " cluster=server              (be a coordination/routing server)\n"
      " cluster=serveronly          (be a server, but don't host jobs)\n"
      " cluster=autoclient          (auto find an existing network, and only run my own jobs)\n"
      " cluster=autoworker          (auto find an existing network)\n"
      " cluster=autoserver          (auto find an existing network, and be a server if none found)\n"
      " cluster=SOMEURL             (SOMEURL is of the form scopira://hostname:port/options,options)\n"
      "Options when using cluster=\n"
      " clusterport=port#           (use the given TCP port, when not specified elsewhere)\n"
      " clusterquit=1               (shutdown the cluster)\n"
      //" clusterpassword=password    (use the given password, not yet used)\n"
      "\n"
      "Options for the SOMEURL are:\n"
      " broadcast                   (search for the server, via the wild card hostname given)\n"
      " client                      (same as tunnel,nojobs)\n"
      " direct                      (no need for via-master routing -- default)\n"
      " tunnel                      (needs via-master routing)\n"
      " alljobs                     (runs all jobs -- default)\n"
      " myjobs                      (only run jobs I myself instantiate)\n"
      " nojobs                      (refuse all jobs)\n"
      "\n"
      "To initiate a job/task on the network, provide:\n"
      " task=TASK                   (the C++ classname of the task to run)\n"
      " numcpu=X                    (run with X CPUs, or 0 for ALL (the default))\n"
      "\n";
    return 1;
  }

  // a niceity
  if (bl.has_config("clusterquit"))
    bl.set_config_default("cluster", "autoclient");

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

    // finally, run the task using the driver routine
    run_task(ctx, *t);
  }
  else if (bl.get_config("clusterquit") == "1") {
    scopira::agent::agent_i::instance()->enqueue_network_quit();
    cl.set_wait_for_quit();
  } else {
    // i dont have a task? lets just wait around then
    cl.set_wait_for_quit();
  }

  return 0;
}

