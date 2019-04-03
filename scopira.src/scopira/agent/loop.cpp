
/*
 *  Copyright (c) 2005-2007    National Research Council
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

#include <scopira/tool/array.h>
#include <scopira/tool/file.h>
#include <scopira/tool/thread.h>
#include <scopira/core/loop.h>
#include <scopira/agent/agent.h>

#ifndef PLATFORM_win32
#include <sys/wait.h>
#include <unistd.h>
#endif

//BBtargets libscopira.so

using namespace scopira::tool;
using namespace scopira::core;
using namespace scopira::agent;

//
//
// agent_loop::clusterboot
//
//

#ifdef PLATFORM_win32
// a stub version for windows that doesnt do anything but should compile
// becase: clusterboot stuff isnt implemented for Windows

class agent_loop::clusterboot : public virtual scopira::tool::object
{
  public:
    clusterboot(void) { }

    static clusterboot * instance(void) { return 0; }
};

#else
// proper, UNIXy version that actually does something

class agent_loop::clusterboot : public virtual scopira::tool::object
{
  public:
    /// dtor
    virtual ~clusterboot();

    /**
     * this will return the existing clusterboot instance, and create one if needed
     * if a clusterbooter is not needed, then this will return 0.
     *
     * If it creates a clusterbooter, then it maybe also create an agent.
     * This agent may be retrieved via the instance() method on that class, ofcourse.
     *
     * @author Aleksander Demko
     */ 
    static clusterboot * instance(void);

  private:
    /// ctor
    clusterboot(void);

    void init(void);
    void finish_up(void);

  private:
    static clusterboot * dm_instance;

    scopira::tool::count_ptr<scopira::agent::agent_i> dm_worker;

    struct remote_host {
      std::string hostname;
      pid_t pid;
    };

    typedef std::vector<remote_host> hosts_t;
    hosts_t dm_hosts;
};

agent_loop::clusterboot * agent_loop::clusterboot::dm_instance;

agent_loop::clusterboot::clusterboot(void)
{
  assert(dm_instance == 0);
  dm_instance = this;

  init();
}

agent_loop::clusterboot::~clusterboot()
{
  finish_up();

  assert(dm_instance == this);
  dm_instance = 0;
}

agent_loop::clusterboot * agent_loop::clusterboot::instance(void)
{
  if (dm_instance)
    return dm_instance;

  if (scopira::core::basic_loop::instance()->has_config("clusterboot"))
    return dm_instance = new clusterboot;
  else
    return 0;
}

// returns pid of the child
static pid_t fork_and_run(const std::vector<std::string> &cmd)
{
  assert(cmd.size()>0);

  // fork myself
  pid_t pid = ::fork();

  if (pid > 0)
    return pid;  // im the parent, im done

  // now i must be the child, do the exec.

  int retcode;
  basic_array<char *> args;

  args.resize(cmd.size()+1);

  for (int x=0; x<cmd.size(); ++x)
    args[x] = const_cast<char*>(cmd[x].c_str());    // dangerous?
  args[args.size()-1] = 0;     // null terminate

  // use the p variant to search for "ssh"
//for (int j=0; j+1<args.size(); ++j) OUTPUT << "child's args[" << j << "] = \"" << args[j] << "\"\n";
  retcode = ::execvp(args[0], args.c_array());

  // one should never return from an exec call
  assert(false && "[problem spawning the sub slave ssh processes]\n");
  return retcode;
}

void agent_loop::clusterboot::init(void)
{
  std::vector<std::string> hostnames;
  basic_loop *looper = basic_loop::instance();
  std::string serverurl;

  assert(looper);

  string_tokenize(looper->get_config("clusterboot"), hostnames, ", \n");
//for (int j=0; j<hostnames.size(); ++j)
//  OUTPUT << "hostnames[" << j << "] = \"" << hostnames[j] << "\"\n";

  assert(hostnames.size()>0);

  // start the local agent, which will be the cluster master server
  agent_i *age = agent_i::instance();
  assert(!age);

  looper->set_config("cluster", "server");
  age = agent_i::new_agent();
  assert(age);

  // wait a little bit after the server starts up
  thread::sleep(1000);

  // get the url of the server we just made
  agent_i::get_cluster_server_url(serverurl);

  // setup the run parameters for the slave nodes
  std::vector<std::string> params;
  std::string key;

  looper->set_config_default("clusterboot_ssh", "ssh");
  looper->set_config_default("clusterboot_exe", looper->get_config("param"));

  params.push_back(looper->get_config("clusterboot_ssh"));
  params.push_back("MACHINE");  // this will be the machine name
  params.push_back(looper->get_config("clusterboot_exe"));

  // add a link to myself
  params.push_back("cluster=" + serverurl);
  // add the lib params
  for (key="lib"; looper->has_config(key); key.push_back('+'))
    params.push_back("lib=" + looper->get_config(key));

  // now setup all the slave nodes
  dm_hosts.resize(hostnames.size());
  for (int x=0; x<hostnames.size(); ++x) {
    dm_hosts[x].hostname = hostnames[x];
    params[1] = hostnames[x];
    dm_hosts[x].pid = fork_and_run(params);
    // wait a sec to let them start up
    thread::sleep(1000);
  }
}

void agent_loop::clusterboot::finish_up(void)
{
  for (hosts_t::iterator ii=dm_hosts.begin(); ii!=dm_hosts.end(); ++ii)
    ::waitpid(ii->pid, 0, 0);
}

#endif

//
//
// agent_loop
//
//

agent_loop::agent_loop(void)
  : dm_daemon_mode(false)
{
  dm_clusterboot = clusterboot::instance();

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
  dm_clusterboot = clusterboot::instance();

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
  else
    dm_worker->notify_stop();
}

void agent_loop::set_wait_for_quit(void)
{
  dm_daemon_mode = true;
}

