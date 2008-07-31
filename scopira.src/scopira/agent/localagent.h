
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

#ifndef __INCLUDED_SCOPIRA_AGENT_LOCALAGENT_H__
#define __INCLUDED_SCOPIRA_AGENT_LOCALAGENT_H__

#include <map>
#include <list>
#include <set>
#include <vector>

#include <scopira/tool/thread.h>
#include <scopira/tool/output.h>
#include <scopira/tool/time.h>
#include <scopira/agent/context.h>

namespace scopira
{
  namespace agent
  {
    class machine_spec;
    class local_agent;

    class uptime_task;//forward
  }
}

/**
 * A profile of the performance characterists of a particular machine.
 *
 * @author Aleksander Demko
 */ 
class scopira::agent::machine_spec
{
  public:
    short pm_numcpu;
    bool pm_intelorder;
    bool pm_64bit;
    int pm_speed;   // unkown units, TODO
    int pm_ram;     // in MEGS (gig=1024)
  public:
    /// zero initing
    machine_spec(void);

    bool load(scopira::tool::itflow_i& in);
    void save(scopira::tool::otflow_i& out) const;

    /// set all the value sby autodetecting
    void set_auto_detect(void);
};

scopira::tool::oflow_i& operator<<(scopira::tool::oflow_i& o, const scopira::agent::machine_spec &spec);

/**
 * A Local agent, capable of only local task executaion (on the same machine).
 * It is multithreaded, though.
 *
 * @author Aleksander Demko
 */ 
class scopira::agent::local_agent : public scopira::agent::agent_i
{
  private:
    friend class scopira::agent::uptime_task;

  public:
    /// ctor
    local_agent(void);
    /// dtor
    virtual ~local_agent();

    virtual void notify_stop(void);
    virtual void wait_stop(void);

    virtual void enqueue_network_quit(void) { }

    /// has this agent successfully booted up?
    /// if not, you shouldnt use it and probably just nuke it.
    virtual bool failed(void) const { return false; }
    virtual void set_agenterror_reactor(agenterror_reactor_i *r) { } //im never goin to call this, so ill just ignore this

    virtual void reg_context(scopira::tool::uuid &ctxid, taskmsg_reactor_i *reac);
    virtual void unreg_context(scopira::tool::uuid ctxid);

    virtual int find_services(scopira::tool::uuid &serviceid, scopira::basekit::narray<scopira::tool::uuid> &out);

    virtual int universe_size(void);
    virtual scopira::tool::uuid get_agent_id(void);

    virtual scopira::tool::uuid launch_task(const std::type_info &t, scopira::tool::uuid where);
    virtual scopira::tool::uuid launch_group(int numps, const std::type_info &t);
    virtual void launch_slaves(scopira::tool::uuid masterid, int numtotalps, const std::type_info &t,
      scopira::basekit::narray<scopira::tool::uuid> &peers);
    virtual void kill_task(scopira::tool::uuid ps);
    virtual bool wait_task(scopira::tool::uuid ps, int msec);
    virtual bool is_alive_task(scopira::tool::uuid ps);
    virtual bool is_killed_task(scopira::tool::uuid ps);

    virtual bool wait_msg(const uuid_query &srcq, scopira::tool::uuid &foundsrc, scopira::tool::uuid dest, int timeout);
    virtual void send_msg(scopira::tool::uuid src, scopira::tool::uuid dest, scopira::tool::bufferflow *buf);
    // this implementation does local deliver
    // decendants override this
    virtual void send_msg_bcast(scopira::tool::uuid src, scopira::tool::uuid destserviceid, scopira::tool::bufferflow *buf);
    virtual void recv_msg(const uuid_query &srcq, scopira::tool::uuid &foundsrc, scopira::tool::uuid dest, scopira::tool::count_ptr<scopira::tool::bufferflow> &buf);

  protected:
    // all these functions are for itneraction with inherited classes, if any
    // la_ means "local_agent" API

    /// yes, RW access
    machine_spec & la_get_spec(void) { return dm_spec; }
    /// get the generator
    scopira::tool::uuid_generator & la_get_generator(void) { return dm_uugen; }
    /// called when a new process is to be isserted
    /// returns true on success (or switch to codes?)
    bool la_launch_task(int myindex, const scopira::basekit::narray<scopira::tool::uuid> &taskids, const std::string &typestring, short phase);
    /// will be assigned new, random uuid and returned
    scopira::tool::uuid la_launch_proxy(agent_task_i *at);
    void la_update_slave_master(scopira::tool::uuid master, scopira::basekit::narray<scopira::tool::uuid> &peers);
    /// la calls this as a last resort (after trying local delivery)
    virtual void la_send_msg(scopira::tool::uuid src, scopira::tool::uuid dest, scopira::tool::bufferflow *buf) { }
    /// la calls this after a local task is terminated
    virtual void la_dead_task(scopira::tool::uuid taskid) { }
    /// the local_agent implementation simply does local deliverty

  private:
    static void* worker_func(void *data);

  private:
    /**
     * A msg
     * @author Aleksander Demko
     */ 
    struct msg_t
    {
      scopira::tool::uuid pm_src;
      scopira::tool::count_ptr<scopira::tool::bufferflow> pm_buf;

      public:
      msg_t(scopira::tool::uuid src, scopira::tool::bufferflow *buf)
        : pm_src(src), pm_buf(buf) { }
    };

    typedef std::list<msg_t> msglist_t;

    class process_t : public scopira::tool::object
    {
      public:
        // process "modes" (pm_mode)
        enum {
          ps_empty_c = 0,     // initial state, should never be in this state though
          ps_born_c,      // a process in phase 1 of a 2 phase creation sequence. this process is not yet runnable
          ps_ready_c,     // ready to run
          ps_running_c,   // currently being run by a worker thread
          ps_sleep_c,     // waiting for a MSG or for wake up time
          ps_done_c,      // process should die/is dieing

          // spcial ones, but ARE schedulable
          // NO SPECIALI maker, as per normal process?
          // ps_remote_c

          // "special" types (not schedulable):
          ps_context_c,   // this is a marker process for a context's msg queue
          //ps_proxy_c,    // this process is "remote" (not on this machine). all msgs sent to it should be done so via enqueue_remote_ps_msg
        };

        scopira::tool::uuid pm_id;
        bool pm_special;        // if true, this isnt a normal, process
        scopira::tool::count_ptr<scopira::agent::agent_task_i> pm_task;   // only non-null on local, normal processes

        // GROUP STUFF (TODO should this be in the state_area?)
        int pm_index;   // my index in the group
        scopira::basekit::narray<scopira::tool::uuid> pm_peers;    // this list should never be empty (ie. it should always contain atleast myself)

        // the services this task has. it is const after its initial creation
        std::set<scopira::tool::uuid> pm_services;

        // internal class
        struct state_area {
          short pm_mode;
          bool pm_killreq;      // this a kill requested been made of this task
          bool pm_canmove;      // can this process be migrated
          bool pm_onmsg;       // should this proces be awoken on a msg
          double pm_ontime;     // the agent-clock time (or any time after) this process should run. 0 means, not by time

          msglist_t pm_msgqueue;

          // a reactor, if any
          taskmsg_reactor_i *pm_reactor;

          // ctor
          state_area(void) : pm_mode(ps_empty_c), pm_killreq(false), pm_canmove(false),
            pm_onmsg(false), pm_ontime(0), pm_reactor(0) { }
        };

        scopira::tool::event_area<state_area> pm_state;

        process_t(scopira::tool::uuid id, short specialstate = ps_empty_c)
          : pm_id(id), pm_special(specialstate?ps_empty_c:false),
          pm_index(0) { pm_peers.resize(1); pm_peers[0]=id; pm_state.pm_data.pm_mode = specialstate; }
        process_t(int myindex, const scopira::basekit::narray<scopira::tool::uuid> &peers)
          : pm_id(peers[myindex]), pm_index(myindex), pm_peers(peers) { }

        // this will populate the pm_services vector from the global registry
        void load_services(const std::type_info &nfo);
    };

    typedef std::map<scopira::tool::uuid, scopira::tool::count_ptr<process_t> > psmap_t;
    typedef std::vector<scopira::tool::count_ptr<scopira::tool::thread> > threadlist_t;

    struct kernel_area {
      psmap_t pm_ps;    // the processes

      bool pm_alive;    // if this is false, Ive been sent the kill signal.

      threadlist_t pm_threads;

      scopira::tool::uuid pm_nextps; // the next ps to "look at", zero if do first

      kernel_area(void) : pm_alive(true) { } // ctor
    };

    scopira::tool::uuid_generator dm_uugen;

    scopira::tool::chrono dm_agentclock;

    machine_spec dm_spec;

    // never try to lock the kernel if youre holdin any process locks!
    scopira::tool::event_area<kernel_area> dm_kernel; // my "OS kernel"

  private:
    void get_ps(scopira::tool::uuid id, scopira::tool::count_ptr<process_t> &foundps) const;
    /// this does its own locking, do not call it within any locks
    /// will wake up the threads too
    void set_min_threads(void);
    /// sends a kill request to all the tasks here
    void kill_all_local_tasks(void);
};

#endif

