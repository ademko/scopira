
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

#ifndef __INCLUDED_SCOPIRA_AGENT_CONTEXT_H__
#define __INCLUDED_SCOPIRA_AGENT_CONTEXT_H__

#include <typeinfo>

#include <scopira/tool/export.h>
#include <scopira/tool/uuid.h>
#include <scopira/tool/binflow.h>
#include <scopira/agent/task.h>
#include <scopira/agent/agent.h>

namespace scopira
{
  namespace agent
  {
    class service_broadcast;

    class send_msg;
    class recv_msg;

    class task_context;  // rename to task_context

    /**
     * This function will run the given task by calling
     * it's run() method. it will properly analysis its
     * return codes and possible call the run method repeadedly
     * until the task is fininished or killed.
     *
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT void run_task(scopira::agent::task_context &ctx, scopira::agent::agent_task_i &t);
  }
}

/**
 * A object that sepecifies a network broadcast.
 * Basically, this will send your message to ALL
 * tasks in the system that have the given service
 * (ie. return has_service() true for the given service).
 *
 * This is obsolete and will probably be removed. Do not use.
 *
 * @author Aleksander Demko
 */
class scopira::agent::service_broadcast
{
  public:
    /**
     * Basic constructor.
     *
     * In the future, perhaps add a int depth=0 param.
     *
     * @author Aleksander Demko
     */
    service_broadcast(const scopira::tool::uuid &serviceid) : m_serviceud(serviceid) { }

    /// returns the service id
    const scopira::tool::uuid & get_serviceid(void) const { return m_serviceud; }

  private:
    scopira::tool::uuid m_serviceud;
};

/**
 * A send msg.
 *
 * @author Aleksander Demko
 */
class scopira::agent::send_msg : public scopira::tool::bin64oflow
{
  public:
    /**
     * Prepare and send msg transaction.
     *
     * ctx is your context.
     * dest is your destination. The zero-uuid is NOT valid. However, if the need
     * arrises, some broadcast facilities may be implemented.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT send_msg(scopira::agent::task_context &ctx, scopira::tool::uuid dest);
    /**
     * Prepare and send msg transaction. Group edition.
     *
     * ctx is your context.
     * dest is your destination, by index number. -1 is NOT valid, However, if the need
     * arrises, some broadcast facilities may be implemented.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT send_msg(scopira::agent::task_context &ctx, int dest);

    /**
     * Prepare and send msg transaction. Broadcast edition.
     *
     * ctx is your context.
     * dest is your destination
     *
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT send_msg(scopira::agent::task_context &ctx, const service_broadcast &targets);

    /// dtor
    SCOPIRA_EXPORT virtual ~send_msg();

  private:
    // in the future, change the implementation to use direct-to-wire
    scopira::tool::uuid dm_src, dm_dest;
    scopira::tool::count_ptr<scopira::tool::bufferflow> dm_buf;

    bool dm_service_bcast;
};

/**
 * A recv msg.
 *
 * @author Aleksander Demko
 */
class scopira::agent::recv_msg : public scopira::tool::bin64iflow
{
  public:
    /**
     * Prepare and receive a msg in a transaction.
     *
     * ctx the context.
     * src the source UUID youd like to check for messages from.
     *  A zero UUID implies that any source (in the universe) would be fine
     *  (in which case, use get_source() to find the actual source.
     *
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT recv_msg(scopira::agent::task_context &ctx, scopira::tool::uuid src);
    /**
     * Prepare and receive a msg in a transaction. Group edition.
     *
     * ctx the context.
     * src the source index youd lke to receive from, or -1 for "any in your group"
     *
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT recv_msg(scopira::agent::task_context &ctx, int src);
    /**
     * Prepare and receive a msg in a transaction.
     *
     * ctx the context.
     * src is the query you'd like to match (ie. this is the most general version)
     *
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT recv_msg(scopira::agent::task_context &ctx, const uuid_query &Q);

    /// dtor
    SCOPIRA_EXPORT virtual ~recv_msg();

    /**
     * Gets the source UUID of the msg.
     *
     * @author Aleksander Demko
     */ 
    scopira::tool::uuid get_source(void) const { return dm_lastsrc; }

  private:
    scopira::tool::uuid dm_lastsrc, dm_dest;
    scopira::tool::count_ptr<scopira::tool::bufferiflow> dm_buf;
    scopira::tool::count_ptr<scopira::tool::bufferflow> dm_realbuf;
};

/**
 * The simplest run constext.
 *
 * @author Aleksander Demko
 */ 
class scopira::agent::task_context
{
  public:
    /**
     * Constructor, for users.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT task_context(void);
    /**
     * A constrcutor, for users, that also registers a on-received-msg reactor.
     * You would then do all your recv_msg stuff in that handler.
     *
     * This is useful for gui programming and other places where you'd rather not
     * pole or block for messages.
     *
     * Note that your reactor will be called in one of the agent's threads.
     * If you do any gui stuff, make sure to aquire any proper locks and such.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT task_context(taskmsg_reactor_i *reac);
    // internal ctor for scopira use, DO NOT USE
    SCOPIRA_EXPORT task_context(int myindex, const scopira::basekit::narray<scopira::tool::uuid> &peers);
    /// destructor
    SCOPIRA_EXPORT ~task_context();

    /**
     * Is the agent in a failed state?
     *
     * @author Aleksander Demko
     */
    bool failed(void) const { return agent_i::instance()->failed(); }

    /**
     * Search for service providers.
     * This function will find all the (usually) tasks that support the given serviceid.
     *
     * In the future, this function may also accept distance and arch-querty critieria
     * to further refine the search.
     *
     * It will return the number of tasks found, which may be 0. The output list will be put
     * into out. The resulting array may have some rough sorting, and if it does it'll
     * be from more prefered (by what metric? usually network distance) to least.
     *
     * @author Aleksander Demko
     */ 
    int find_services(scopira::tool::uuid &serviceid, scopira::basekit::narray<scopira::tool::uuid> &out);

    /**
     * Returns the recommended/maximum number of full CPU-bound tasks to launch
     * in a group/at once. Will always return >0.
     *
     * @author Aleksander Demko
     */
    int universe_size(void)
      { return agent_i::instance()->universe_size(); }

    /**
     * Gets the UUID of current I'm running under agent.
     *
     * @author Aleksander Demko
     */ 
    scopira::tool::uuid get_agent_id(void) { return agent_i::instance()->get_agent_id(); }

    /**
     * Spawn the given one task into the agent system.
     * The UUID of the task is returned, useful
     * for non-blocking spawns.
     *
     * The UUID is your "handle" to the running task. You should "forgot" all your
     * pointers to the given task object, as it will be absorbed into the agent
     * system (and possibly moved between machines, etc).
     *
     * The where parameter allows you to control where the new task is deployed. This can
     * be:
     *  - zero-uuid for the default placement system
     *  - a specific agent-uuid for placement on that agent
     *  - a specific task-uuid for specific "load balancing" service agent (not yet implemented)
     *  - a constant uuid for a specific operation (see where_*_c in ids.h)
     *
     * @author Aleksander Demko
     */ 
    template <class M>
      scopira::tool::uuid launch_task(scopira::tool::uuid where = scopira::tool::uuid())
        { return agent_i::instance()->launch_task(typeid(M), where); }
    /// another variant
    SCOPIRA_EXPORT scopira::tool::uuid launch_task(const std::string &classname, scopira::tool::uuid where = scopira::tool::uuid());
    
    /// the core variant
    //scopira::tool::uuid launch_task(const std::type_info &t)
      //{ return agent_i::instance()->launch_task(t); }

    /**
     * Spawn the given master process as a group.
     *
     * The given process will be the master process. numps-1 additional processes will be spawned.
     * If numps is 0, then it numps will be the number of free processes in the system.
     *
     * numps must be greater than 0. Use universe_size() to get a good idea of a size
     * of group to launch.
     *
     * The t must be registered for serialization.
     *
     * @author Aleksander Demko
     */ 
    template <class M>
      scopira::tool::uuid launch_group(int numps)
        { return agent_i::instance()->launch_group(numps, typeid(M)); }
    /// another variant
    SCOPIRA_EXPORT scopira::tool::uuid launch_group(int numps, const std::string &classname);

    /// the core variant
    //scopira::tool::uuid launch_group(int numps, const std::type_info &t)
      //{ return agent_i::instance()->launch_group(numps, t); }

    /**
     * Spawns a group of slave processes making the calling task/process the "master".
     * This is especially useful for master threads/ui threads/userland threads.
     *
     * numps must be greater than 0. Use universe_size() to get a good idea of a size
     * of group to launch.
     *
     * This cannot be called for processes that are already in process groups.
     *
     * @author Aleksander Demko
     */ 
    template <class M>
      void launch_slaves(int numps)    // doesnt block
        { launch_slaves_impl(numps, typeid(M)); }
    SCOPIRA_EXPORT void launch_slaves(int numps, const std::string &classname);

    /**
     * Kills a given process.
     * If the process doesnt exit, nothing happens.
     * If it does, a kill signal is queued for the process. The process
     * will then be terminated at its next checkpoint.
     *
     * @author Aleksander Demko
     */
    void kill_task(scopira::tool::uuid ps)
      { agent_i::instance()->kill_task(ps); }

    /**
     * Wait for the given process to die. Returns true if
     * the process died/is dead/doesnt exist. False if the process
     * still lives after the given time out.
     * (1000 msec=second).
     * A timeout of 0 msec means to wait indefinatly.
     *
     * @author Aleksander Demko
     */ 
    bool wait_task(scopira::tool::uuid ps, int msec = 0)
      { return agent_i::instance()->wait_task(ps, msec); }

    /**
     * Check if the given process exists.
     *
     * @author Aleksander Demko
     */
    bool is_alive_task(scopira::tool::uuid ps)
      { return agent_i::instance()->is_alive_task(ps); }

    /**
     * Has this task been "killed"? A task may "kill" another task with the
     * kill_task method. When a task is killed, is_kill will return true
     * for that task. Also, when the victum task returns from its run
     * method, it will be destroyed. Note, this will return true when a task
     * has been scheduled to die, but has not yet completed its run method.
     * is_alive_task() on the otherhand, will only return fall (dead) when the
     * specified task no longer exists.
     *
     * @author Aleksander Demko
     */ 
    bool is_killed_task(void)
      { return agent_i::instance()->is_killed_task(dm_peers[dm_myindex]); }

    /**
     * Waits for a msg from the given src. Make src zero for "anyone in the universe".
     * Returns true if one came within the time out. Make timeout 0 for inifnity.
     * The actual source can be retrived with get_source().
     *
     * Use has_msg to do instantaneous polling.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT bool wait_msg(scopira::tool::uuid src, int timeout = 0);
    /**
     * Waits for a msg from the given peer in my group. Make src -1 for "anyone in the GROUP"
     * (and just the group).
     * Returns true if one came within the time out. Make timeout 0 for inifnity.
     * The actual source can be retrived with get_source().
     *
     * Use has_msg to do instantaneous polling.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT bool wait_msg(int src = -1, int timeout = 0);
    /**
     * Waits for a msg that satisfies the given query.
     * Returns true if one came within the time out. Make timeout 0 for inifnity.
     * The actual source can be retrived with get_source().
     *
     * Use has_msg to do instantaneous polling.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT bool wait_msg(const scopira::agent::uuid_query &Q, int timeout = 0);

    /**
     * Checks (polls) if there is a msg from the given peer in the event queue.
     * A zero src is "from anyone".
     * The actual source can be retrived with get_source().
     *
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT bool has_msg(scopira::tool::uuid src);
    /**
     * Checks if there a message from the given group peer.
     * -1 is anyone in the GROUP.
     * The actual source can be retrived with get_source().
     *
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT bool has_msg(int src = -1);
    /**
     * Checks if there a message that satisfies the given query.
     * The actual source can be retrived with get_source().
     *
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT bool has_msg(const scopira::agent::uuid_query &Q);

    /**
     * Gets the source UUID of the last wait_msg trigger.
     * This is not to be confused with the one in recv_msg::get_source: they're
     * unrelated
     *
     * @author Aleksander Demko
     */ 
    scopira::tool::uuid get_source(void) const { return dm_lastsrc; }

    /**
     * Gets my index within a group.
     *
     * @author Aleksander Demko
     */
    int get_index(void) const { assert(dm_myindex>=0); return dm_myindex; }
    /**
     * Gets the index of the given process UUID in my group.
     * Returns -1 for not found.
     *
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT int get_index(scopira::tool::uuid id) const;
    /**
     * Gets the size of my group. This will always be atleast 1.
     * @author Aleksander Demko
     */
    int get_group_size(void) const { return dm_peers.size(); }
    /**
     * Gets my UUID
     * @author Aleksander Demko
     */ 
    scopira::tool::uuid get_id(void) const { return dm_peers[dm_myindex]; }
    /**
     * Gets the UUID of the given peer by index.
     * the index must be valid.
     * @author Aleksander Demko
     */ 
    scopira::tool::uuid get_id(int idx) const { return dm_peers[idx]; }
    /**
     * Your whole peer group.
     *
     * @author Aleksander Demko
     */
    const scopira::basekit::narray<scopira::tool::uuid> & get_group(void) const { return dm_peers; }

    /**
     * A barrier for all the processes in this group (similar to MPI_Barrier)
     *
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT void barrier_group(void);

    /**
     * Waits until all the others in my group die (ie. wait_task on each of them)
     *
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT void wait_group(void);

  private:
    SCOPIRA_EXPORT void launch_slaves_impl(int numtotalps, const std::type_info &nfo);

  private:
    /// this is only non-null for user created contexts, since they have to maintain
    /// a ref counted pointer to the agent
    scopira::tool::count_ptr<agent_i> dm_server_link;

    // for wait_msg*
    scopira::tool::uuid dm_lastsrc;

    // group stuff... [0] is the master... the group size is always atleast one
    // [dm_myindex] is me
    scopira::basekit::narray<scopira::tool::uuid> dm_peers;
    int dm_myindex;

    // dont even try it (disabled)
    task_context(const task_context &) { }

  friend class scopira::agent::send_msg;
  friend class scopira::agent::recv_msg;
};

#endif

