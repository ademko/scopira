
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

#ifndef __INCLUDED_SCOPIRA_AGENT_AGENT_H__
#define __INCLUDED_SCOPIRA_AGENT_AGENT_H__

#include <scopira/basekit/narray.h>
#include <scopira/tool/object.h>
#include <scopira/tool/uuid.h>
#include <scopira/tool/bufferflow.h>
#include <scopira/agent/task.h>
#include <scopira/tool/export.h>

namespace scopira
{
  namespace agent
  {
    class uuid_query;
    class agenterror_reactor_i;
    class taskmsg_reactor_i;
    class agent_i;
  }
}

/**
 * A potentially complex query for UUIDs.
 * This is used for specifying which IDs a task would like to listen * for.
 *
 * Basically, if you build this class with the (void) ctor, use the various
 * POSTFIX operations to build your query. Then engine will then evaluate your
 * query against all the tested nodes.
 *
 * TODO switch this to short-circut infix or prefix?
 *
 * @author Aleksander Demko
 */
class scopira::agent::uuid_query
{
  public:
    /**
     * Complex case constructor. You build your query then by
     * using the various operators.
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT uuid_query(void);
    /**
     * Simple expression contructor. simplecase can either be
     * the UUID you want to specifically wait for, or zero of ANY
     * one.
     * Yes, this is implicit.
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT explicit uuid_query(scopira::tool::uuid simplecase);

    /**
     * Evalute this expression against a uuid and see if it matches.
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT bool query(scopira::tool::uuid testid) const;

    //
    // The expression building operations.
    //

    SCOPIRA_EXPORT uuid_query & match_any(void);
    SCOPIRA_EXPORT uuid_query & match_one(scopira::tool::uuid id);
    SCOPIRA_EXPORT uuid_query & match_group(scopira::basekit::const_nslice<scopira::tool::uuid> ids);

    SCOPIRA_EXPORT uuid_query & op_and(void);
    SCOPIRA_EXPORT uuid_query & op_or(void);
    SCOPIRA_EXPORT uuid_query & op_xor(void);
    SCOPIRA_EXPORT uuid_query & op_not(void);

  private:
    scopira::tool::uuid dm_simpleuuid;
    // otherwise, this is the complex expression
    enum {
      match_any_c,    // this is basicaly a literal "true"
      match_one_c,   // + index
      match_group_c, // + index + size
      op_and_c,
      op_or_c,
      op_xor_c,
      op_not_c,
    };
    // the expression and its data
    std::vector<short> dm_exp;    // if this is empty, then use the simple case uuid
    std::vector<scopira::tool::uuid> dm_data;
};

/**
 * A handler of agent errors.
 * @author Aleksander Demko
 */ 
class scopira::agent::agenterror_reactor_i
{
  public:
    virtual ~agenterror_reactor_i() { }
    /**
     * This is called when the agent gets a fatal, abrupt network error or
     * something.
     *
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT virtual void react_agenterror(scopira::agent::agent_i *a) = 0;
};

/**
 * This is a handler for message arrive notices.
 *
 * @author Aleksander Demko
 */
class scopira::agent::taskmsg_reactor_i
{
  public:
    virtual ~taskmsg_reactor_i() { }

    /**
     * This is called when the given task (usually a local context)
     * has a message ready. Note that this is called from one of the agent's
     * threads. Make sure to do any gui locks if you're going to do gui stuff.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT virtual void react_taskmsg(scopira::agent::agent_i *a, scopira::tool::uuid srcid, scopira::tool::uuid destid) = 0;
};

/**
 * Basic agent interface
 * @author Aleksander Demko
 */ 
class scopira::agent::agent_i : public scopira::tool::object
{
  public:
    /// returns the static instance, might be null!
    /// ALL holds to this object must be via ref counting!
    static agent_i * instance(void) { return dm_instance; }

    /**
     * Instantiate a new agent... auto determines the internal
     * type n such.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT static agent_i * new_agent(void);

    /// dtor
    SCOPIRA_EXPORT virtual ~agent_i();

    SCOPIRA_EXPORT virtual void notify_stop(void) = 0;
    /// block the caller until this agent_i has terminated
    /// because of a agent end message
    SCOPIRA_EXPORT virtual void wait_stop(void) = 0;
    /// tell the whole cluster to quit
    /// will result in a local notify_stop
    SCOPIRA_EXPORT virtual void enqueue_network_quit(void) = 0;

    /// has this agent successfully booted up?
    /// if not, you shouldnt use it and probably just nuke it.
    SCOPIRA_EXPORT virtual bool failed(void) const = 0;

    /// sets the agenterror handler... there can only be one, eh
    SCOPIRA_EXPORT virtual void set_agenterror_reactor(agenterror_reactor_i *r) = 0;

    SCOPIRA_EXPORT virtual void reg_context(scopira::tool::uuid &ctxid, taskmsg_reactor_i *reac) = 0;
    SCOPIRA_EXPORT virtual void unreg_context(scopira::tool::uuid ctxid) = 0;

    SCOPIRA_EXPORT virtual scopira::tool::uuid launch_task(const std::type_info &t) = 0;
    SCOPIRA_EXPORT virtual scopira::tool::uuid launch_group(int numps, const std::type_info &t) = 0;
    SCOPIRA_EXPORT virtual void launch_slaves(scopira::tool::uuid masterid, int numtotalps, const std::type_info &t,
      scopira::basekit::narray<scopira::tool::uuid> &peers) = 0;
    SCOPIRA_EXPORT virtual void kill_task(scopira::tool::uuid ps) = 0;
    SCOPIRA_EXPORT virtual bool wait_task(scopira::tool::uuid ps, int msec) = 0;
    SCOPIRA_EXPORT virtual bool is_alive_task(scopira::tool::uuid ps) = 0;

    SCOPIRA_EXPORT virtual bool wait_msg(const uuid_query &srcq,scopira::tool::uuid &foundsrc,  scopira::tool::uuid dest, int timeout) = 0;
    SCOPIRA_EXPORT virtual void send_msg(scopira::tool::uuid src, scopira::tool::uuid dest, scopira::tool::bufferflow *buf) = 0;
    // src will be filled with the actual source, after
    SCOPIRA_EXPORT virtual void recv_msg(const uuid_query &srcq, scopira::tool::uuid &foundsrc, scopira::tool::uuid dest, scopira::tool::count_ptr<scopira::tool::bufferflow> &buf) = 0;

  protected:
    /// ctor
    SCOPIRA_EXPORT agent_i(void);

  private:
    SCOPIRA_EXPORT static agent_i *dm_instance;
};

#endif

