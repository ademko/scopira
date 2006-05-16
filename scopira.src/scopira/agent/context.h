
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
  }
}

/**
 * A object that sepecifies a network broadcast.
 * Basically, this will send your message to ALL
 * tasks in the system that have the given service
 * (ie. return has_service() true for the given service).
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
     * dest is your destination, or zero of all in your process group.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT send_msg(scopira::agent::task_context &ctx, scopira::tool::uuid dest);
    /**
     * Prepare and send msg transaction. Group edition.
     *
     * ctx is your context.
     * dest is your destination, by index number. -1 for broadcast.
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

    bool m_sendingbcast;
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
     *  A zero UUID implies that any source would be fine
     *  (in which case, use get_source() to find the actual source.
     *
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT recv_msg(scopira::agent::task_context &ctx, scopira::tool::uuid src);
    /**
     * Prepare and receive a msg in a transaction. Group edition.
     *
     * ctx the context.
     * src the source index youd lke to receive from, or -1 for "any"
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
    /// internal ctor, dont use
    SCOPIRA_EXPORT task_context(int myindex, const scopira::basekit::narray<scopira::tool::uuid> &peers);
    /// dtor
    SCOPIRA_EXPORT ~task_context();

    /**
     * Is the agent in a failed state?
     *
     * @author Aleksander Demko
     */
    bool failed(void) const { return agent_i::instance()->failed(); }

    /**
     * If enable true, then this decalres that this task can receive
     * broadcast messages targeted with the given serviceid.
     *
     * If enable is false, then any previous declaration (if any)
     * will be cleared.
     *
     * @author Aleksander Demko
     */
    void enable_service(scopira::tool::uuid serviceid, bool enable = true);

    /**
     * Spawn the given one task into the agent system.
     * The UUID of the task is returned, useful
     * for non-blocking spawns.
     *
     * The UUID is your "handle" to the running task. You should "forgot" all your
     * pointers to the given task object, as it will be absorbed into the agent
     * system (and possibly moved between machines, etc).
     *
     * If wait is true, then this will do a wait_task too (is this still useful?)
     *
     * @author Aleksander Demko
     */ 
    template <class M>
      scopira::tool::uuid launch_task(void)
        { return agent_i::instance()->launch_task(typeid(M)); }
    /// another variant
    SCOPIRA_EXPORT scopira::tool::uuid launch_task(const std::string &classname);
    
    /// the core variant
    //scopira::tool::uuid launch_task(const std::type_info &t)
      //{ return agent_i::instance()->launch_task(t); }

    /**
     * Spawn the given master process as a group.
     *
     * The given process will be the master process. numps-1 additional processes will be spawned.
     * If numps is 0, then it numps will be the number of free processes in the system.
     *
     * If numps<=0, then allnodes-numps tasks will be allocated in total.
     *
     * The t must be registered for serialization.
     *
     * If wait is true, then this will do a wait_task too (is this still useful?)
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
     * If numps<=0, then allnodes-numps tasks will be allocated in total.
     *
     * This cannot be called for processes that are already in process groups.
     *
     * @author Aleksander Demko
     */ 
    template <class M>
      void launch_slaves(int numps)    // doesnt block
        { launch_slaves_impl(numps, typeid(M)); }
    SCOPIRA_EXPORT void launch_slaves(int numps, const std::string &classname);

    // TODO
    // REPLACE style launch?
    // wait-on-msg style launch?
    // group launch w/ sync auto load %

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
     * Waits for a msg from the given src. Make src zero for "anyone".
     * Returns true if one came within the time out. Make timeout 0 for inifnity.
     * The actual source can be retrived with get_source().
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT bool wait_msg(scopira::tool::uuid src, int timeout = 0);
    /**
     * Waits for a msg from the given peer in my group. Make src -1 for "anyone".
     * Returns true if one came within the time out. Make timeout 0 for inifnity.
     * The actual source can be retrived with get_source().
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT bool wait_msg(int src = -1, int timeout = 0);
    /**
     * Waits for a msg that satifies the given query.
     * Returns true if one came within the time out. Make timeout 0 for inifnity.
     * The actual source can be retrived with get_source().
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT bool wait_msg(const scopira::agent::uuid_query &Q, int timeout = 0);

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
    int get_index_size(void) const { return dm_peers.size(); }
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

