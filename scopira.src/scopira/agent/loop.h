
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

#ifndef __INCLUDED_SCOPIRA_AGENT_LOOP_H__
#define __INCLUDED_SCOPIRA_AGENT_LOOP_H__

#include <scopira/tool/export.h>
#include <scopira/tool/object.h>
#include <scopira/agent/agent.h>

namespace scopira
{
  /**
   * This namespace houses the API and implementations of Scopira Agents.
   *
   * @author Aleksander Demko
   */ 
  namespace agent
  {
    class agent_loop;
  }
}

/**
 * Constructs the Scopira/agent worker.
 *
 * You can do this AFTER you set up any other loop-types.
 *
 * Config params that this loop accepts:
 *  clusterurl=scopira://host:port/    (the Scopira agent URL)
 *  clustermaster=1    (Make me the master)
 *
 * @author Aleksander Demko
 */ 
class scopira::agent::agent_loop
{
  public:
    /**
     * Starts the agent server in a seperate thread.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT agent_loop(void);
    /**
     * Starts the agent server in a seperate thread.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT agent_loop(int &argc, char **&argv);
    /**
     * Kills/stops the server (see set_wait_for_quit()) for more information).
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT ~agent_loop();

    /**
     * Is the agent in a failed state?
     *
     * @author Aleksander Demko
     */
    bool failed(void) const { return dm_worker->failed(); }

    /**
     * Tells the agent loop to block its destruction until it has received a kill
     * command in the network.
     *
     * By default, the agent_loop is not in server mode.
     *
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT void set_wait_for_quit(void);

  private:
    class clusterboot;
    /// automatic boot system, if any
    scopira::tool::count_ptr<clusterboot> dm_clusterboot;
    /// in daemon mode (that is, exit only when given a network kill signal)
    volatile bool dm_daemon_mode;
    /// our link to the static worker
    scopira::tool::count_ptr<scopira::agent::agent_i> dm_worker;
};

/**
  \page scopiraagentssyspage Agents message passing

   - \subpage scopiraagentoverview
   - \subpage scopiraagenttutorial

*/

/**
  \page scopiraagentoverview Agents Overview

  \section introsec Introduction

  The Scopria Agent extension allows Scopira developers to write parallel procesing aware
  applications. It is a message based system and uses Scopira serialization-like primitives
  to package data for transport.

  Developers simply implement task objects. Agents will instantiate them
  at various nodes, and run them. Between run calls, Agents may serialize a tasks state
  and move it to a different node. Tasks communicate with each other using packet-based
  serialization objects.

  The complete system itself relies on code libraries and C++ objects. There are no seperate
  executables or deployment manament infrastructure utilities. The implementation is included
  in the developer's library itself, and thus is a natural part of the application.

  Finally, developers can always be assured that the agent facilities are available.
  The actual back-end agent implementation will be chosen at run time, everytime the application
  launches. If the user does not specify any particular agent style (like the beowulf cluster agent),
  the default always works Level-0 agent will be used. This tiny agent does nothing but impement
  the agent interface within the executable, using threads (allowing for parallelism within one machine). This allows developers to write (possibly graphical) applications that instantly are able to utilize agent networks, if available.

  \section frontsec Programmer API

  For the developer, the agent API provides a straightforward object-oriented API.
  The user simply makes decendants of the agent_task_i interface, and implement's it's
  run method. The interact with the parallel system through the well defined and simplified
  task_context interface object.

  All tasks and agents have a unique UUID (sometimes known as GUID) associated with them.
  This is a opaque, 128-bit integer that is unique within the universe. When a new UUID is needed,
  it is generated using some means that promotes this unqueness.

  Tasks then, through their contexts send and receive messages to other tasks using their UUIDs.
  The messages them selves are packages (and unpackages) using the typical Scopira-esque flow API.
  Serializable objects therefor are istantly usable.

  \section enginessec Engines

  Scopira Agents includes two engines or agent implementation.

  The "local_agent" implementation is always available, and chosen when no others are selected or
  available. It can never fail (as it is not networked in any way).
  It uses operating system threads to take advantage of parallelism. This allows it to use
  multiple processors in a single machine.

  This "cluster_agent" implementation allows one to run a network of agents of a cluster of machines
  connected by a network. This allows task developers to utilize the computation powers of off-the-shelf
  Beowulf clusters.

  This agent assumes that the machines and network are stable. Failures by nodes or the network
  during runs won't be handled gracefully. A Master admistrative node centralizes job decisions
  and routing. Worker nodes (and user clients) may attach anytime, and detach fromthe master when they
  are idle (not running any tasks).

  The detaching facility is particular interesting, as it allows client agents to connect
  to a master, submit their tasks to the network and then detach. They may then reattach
  to the network and query their running tasks for progress reports. This can bring cluster
  computing to a whole new group of graphical application users. You may also mix platforms
  (for example, Windows GUI applications connecting to Linux clusters).

*/

/**
  \page scopiraagenttutorial Agents Tutorial

  The Scopira Agents API provides an all-inclusive API for parallel
  and distributed computing. The API is object-oriented and scalable
  allow for ease of development. The implemenation is built into the Scopira
  library is is always availble (even if a network of machines is not).

  \section usertaskssec User tasks

  The first step of paralleizing your algorithm with Agents involves converting
  your algorithm to task objects. These are simply classes you make that
  descend from scopira::agent::agent_task_i and implement the run() method.
  The run method has the following interface:

  \verbatim
    class scopira::agent::agent_task_i
    {
      ...
        virtual int run(scopira::agent::task_context &ctx) = 0;
      ...
    }
  \endverbatim

  Through the scopira::agent::task_context interface, you tasks can comminicate with other
  task instances, effectivly cooperating to solve a larger problem in parallel.

  Note that you must register all your class types. Place something like the following
  in your .cpp file for every distint task type that you have:

  \verbatim
    #include <scopira/core/register.h>
    static scopira::core::register_flow<slave_conway_task> r1("slave_conway_task");
  \endverbatim

  \section decompsec Task decomposition

  The question of how to decompose and map a problem/algorithm musted be
  addressed in any parallel-implementation.

  \section case1sec Master-Slaves, no slave intercommunication

  In this case, their is a distinct master-slave relationship.
  The master process (always has groupid of 1), spawns one or more worker tasks
  to perform the work. The master task only does managment and administrative
  tasks - all algorithmic work is done in the slaves. Therefore, there will
  always be aleast one slave.

  Each slave asks the master for a unit of work. When it completes the
  work, it submits the results back to the master and asks for more.
  The master, in turn, simply doles out work units until there are no
  more, at which point it tells all the slaves to terminate.

  You typically have one task type for the master, and another for the slaves
  (they can be merged, but it's cleaner to seperate the tasks).

  The agent task will typically:
   - prepare the dataset, gather the run time parameters
   - spawn a number of tasks
   - wait for a message (indefinatly looping)
      - if its a slave task submitting results, file them and give them a new work unit
      - if its a slave task asking for its first work unit, give it to them
   - when there is no more work to do, tell (and wait for) all the slaves to terminate

*/

#endif

