
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

#ifndef __INCLUDED_SCOPIRA_AGENT_CLUSTERAGENT_H__
#define __INCLUDED_SCOPIRA_AGENT_CLUSTERAGENT_H__

#include <queue>

#include <scopira/tool/netflow.h>
#include <scopira/tool/url.h>
#include <scopira/tool/uuid.h>
#include <scopira/tool/time.h>
#include <scopira/core/register.h>
#include <scopira/agent/localagent.h>

namespace scopira
{
  namespace agent
  {
    class node_spec;
    class cluster_agent;

    class uptime_task;//forward
  }
}

/**
 * Cluster node/network specific info about a node
 * in the cluster.
 * This supplements the info in a machine_spec.
 *
 * @author Aleksander Demko
 */ 
class scopira::agent::node_spec
{
  public:
    enum {
      all_jobs_c,
      my_jobs_c,
      no_jobs_c,
    };

    scopira::tool::uuid pm_uuid;
    bool pm_ismaster;           // is this a master? (default, no)
    scopira::tool::url pm_url;    // url, DEDUCED (to make it NATable?) (only really valid if pm_listening==true) (only really valid if pm_listening==true)
    bool pm_isdirectroute;   //can direct peer-links be made to this node? (if not, all messages will be routed via the master node) (default, yes)
    short pm_jobfilter;        // what kind of jobs will this thing accept (default all)
  public:
    /// zero initing
    node_spec(void);

    bool load(scopira::tool::itflow_i& in);
    void save(scopira::tool::otflow_i& out) const;

    void parse_options(const std::string &options);
};

scopira::tool::oflow_i& operator<<(scopira::tool::oflow_i& o, const scopira::agent::machine_spec &spec);

/**
 * A network-aware agent that likes to work in stable and powerful clusters.
 *
 * @author Aleksander Demko
 */ 
class scopira::agent::cluster_agent : public scopira::agent::local_agent
{
  public:
    /// default port used by the master
    enum { default_port_c = 5555 };

  private:
    typedef local_agent parent_type;

    friend class scopira::agent::uptime_task;

  public:
    /// ctor
    cluster_agent(void);
    /// dtor
    virtual ~cluster_agent();

    virtual void notify_stop(void);
    /// block the caller until this agent_i has terminated
    /// because of a agent end message
    //virtual void wait_stop(void);

    virtual void enqueue_network_quit(void);

    /// has this agent successfully booted up?
    /// if not, you shouldnt use it and probably just nuke it.
    virtual bool failed(void) const;
    virtual void set_agenterror_reactor(agenterror_reactor_i *r);

    virtual int find_services(scopira::tool::uuid &serviceid, scopira::basekit::narray<scopira::tool::uuid> &out);

    virtual void reg_context(scopira::tool::uuid &ctxid, taskmsg_reactor_i *reac);
    virtual void unreg_context(scopira::tool::uuid ctxid);

    virtual int universe_size(void);
    virtual scopira::tool::uuid get_agent_id(void);

    virtual scopira::tool::uuid launch_task(const std::type_info &t, scopira::tool::uuid where);
    virtual scopira::tool::uuid launch_group(int numps, const std::type_info &t);
    virtual void launch_slaves(scopira::tool::uuid masterid, int numtotalps, const std::type_info &t,
      scopira::basekit::narray<scopira::tool::uuid> &peers);
    virtual void kill_task(scopira::tool::uuid ps);
    virtual bool wait_task(scopira::tool::uuid ps, int msec);
    virtual bool is_alive_task(scopira::tool::uuid ps);
    //SCOPIRA_EXPORT virtual bool is_killed_task(scopira::tool::uuid ps) = 0;  // using the local agent one directly!

    /* using local_agent's version for all of these:
    virtual bool wait_msg(scopira::tool::uuid src, scopira::tool::uuid dest, int timeout);
    virtual void send_msg(scopira::tool::uuid src, scopira::tool::uuid dest, scopira::tool::bufferflow *buf);
    virtual void recv_msg(scopira::tool::uuid &src, scopira::tool::uuid dest, scopira::tool::count_ptr<scopira::tool::bufferflow> &buf);*/
    virtual void send_msg_bcast(scopira::tool::uuid src, scopira::tool::uuid destserviceid, scopira::tool::bufferflow *buf);

    // needed by agent_i::get_cluster_server_url
    const node_spec & spec(void) const { return dm_nodespec; }

  protected:
    virtual void la_send_msg(scopira::tool::uuid src, scopira::tool::uuid dest, scopira::tool::bufferflow *buf);
    virtual void la_dead_task(scopira::tool::uuid taskid);

  private:
    class link; //fwd
    class net_link; //fwd

    /**
     * The core network msg class
     * @author Aleksander Demko
     */ 
    class admin_msg : public virtual scopira::tool::object
    {
      public:
        enum {
          type_normal_c,
          type_instant_c,   // use with care, executed in caller-thread and/or receive thread
          type_tmp_c,       // a speciail control msg that can be used while the links are in tmp mode
        };
      public:
        virtual const char * get_debug_name(void) const = 0;
        virtual short get_type(void) const { return type_normal_c; }
        virtual void execute_agent(cluster_agent &e) = 0;
    };
    /**
     * Everything that goes over the network (mustbe serializable)
     * @author Aleksander Demko
     */
    class network_msg : public admin_msg
    {
      public:
        // internal class
        struct xtion_t {
          scopira::tool::uuid pm_src;
          int pm_id;
          xtion_t(void) : pm_id(0) { }
          xtion_t(scopira::tool::uuid src, int id)
            : pm_src(src), pm_id(id) { }
          bool load(scopira::tool::itflow_i& in) { return pm_src.load(in) && in.read_int(pm_id); }
          void save(scopira::tool::otflow_i& out) const { pm_src.save(out); out.write_int(pm_id); }
        };
      public:
        /// this is implemented by network_msg and redirects to execute_agent(e, lk)
        /// which decendants must implement)
        virtual bool load(scopira::tool::iobjflow_i& in);
        virtual void save(scopira::tool::oobjflow_i& out) const;

        virtual void execute_agent(cluster_agent &e);
        virtual void execute_agent(cluster_agent &e, link *lk) = 0;

        void set_net_link(net_link *l);

        void set_xtion(const xtion_t &x) { dm_xtion = x; }
        const xtion_t & get_xtion(void) const { return dm_xtion; }

      protected:
        network_msg(void);
      private:
        scopira::tool::count_ptr<net_link> dm_link;

      protected:  // on so reply msg can reach em
        // these are used by both network_msg (for sending) and reply_msg (for replies)
        xtion_t dm_xtion;
    };
    /**
     * All replies to xtioned msgs are based on this type.
     * Replies are also just data, and no execute method.
     * This is also a valid msg unto itself, and as it carries no specific data,
     * can simply be used as a "ping" type reply to any xtion msg.
     *
     * You should dispatch these with enqueue_reply_msg().
     *
     * @author Aleksander Demko
     */
    class reply_msg : public network_msg
    {
      public:
        /// errorcode is whatever you want, if anything
        reply_msg(int errorcode = 0);

        virtual bool load(scopira::tool::iobjflow_i& in);
        virtual void save(scopira::tool::oobjflow_i& out) const;

        /// queues myself to the xtion table
        virtual const char * get_debug_name(void) const { return "reply_msg"; }
        virtual short get_type(void) const { return type_instant_c; }

        virtual void execute_agent(cluster_agent &e, link *lk);

        int get_code(void) const { return dm_errorcode; }
      private:
        int dm_errorcode;
    };

    /// adminMsg - makes a connection
    class make_link_msg;
    class quit_msg;
    class remove_link_msg;
    // network_msg - announces remote's uuid
    class peer_hello_msg;
    class nottmp_hello_msg;
    class make_back_link_msg;
    class all_quit_msg;
    class resolve_agent_msg;
    class reply_resolve_agent_msg;
    class routed_msg;
    class master_launch_task_msg;
    class reply_master_launch_task_msg;
    class make_task_msg;
    class client_wait_msg;
    class client_wait_task;
    class client_kill_msg;
    class client_is_alive_msg;
    class send_data_msg;
    class bcast_data_msg;
    class find_services_msg;
    class reply_find_services_msg;
    class reg_context_msg;
    class dead_task_msg;
    class default_group_size_msg;
    static scopira::core::register_flow<cluster_agent::reply_msg> r81123;
    static scopira::core::register_flow<cluster_agent::peer_hello_msg> r09344;
    static scopira::core::register_flow<cluster_agent::nottmp_hello_msg> r55123;
    static scopira::core::register_flow<cluster_agent::make_back_link_msg> r54788;
    static scopira::core::register_flow<cluster_agent::all_quit_msg> r72218;
    static scopira::core::register_flow<cluster_agent::resolve_agent_msg> r11928;
    static scopira::core::register_flow<cluster_agent::reply_resolve_agent_msg> r62383;
    static scopira::core::register_flow<cluster_agent::routed_msg> r49045;
    static scopira::core::register_flow<cluster_agent::master_launch_task_msg> r88312;
    static scopira::core::register_flow<cluster_agent::reply_master_launch_task_msg> r00922;
    static scopira::core::register_flow<cluster_agent::make_task_msg> r79933;
    static scopira::core::register_flow<cluster_agent::client_wait_msg> r28334;
    static scopira::core::register_flow<cluster_agent::client_kill_msg> r45666;
    static scopira::core::register_flow<cluster_agent::client_is_alive_msg> r44512;
    static scopira::core::register_flow<cluster_agent::send_data_msg> r49075;
    static scopira::core::register_flow<cluster_agent::bcast_data_msg> r49076;
    static scopira::core::register_flow<cluster_agent::find_services_msg> r4834444;
    static scopira::core::register_flow<cluster_agent::reply_find_services_msg> r4834445;
    static scopira::core::register_flow<cluster_agent::reg_context_msg> r89063;
    static scopira::core::register_flow<cluster_agent::dead_task_msg> r85221;
    static scopira::core::register_flow<cluster_agent::default_group_size_msg> r85553;

    /**
     * The actually socket
     * @author Aleksander Demko
     */
    class net_link : public virtual scopira::tool::object
    {
      private:
        friend class network_msg;

        enum {
          blocksize_c = 1024*8,
          magic_c = 0xE23A34F7,
        };

        struct queue_area
        {
          link *pm_link;

          scopira::tool::count_ptr<network_msg> pm_sendmsg;   // queue of 1

          bool pm_alive;            // should this link die asap?

          queue_area(void) : pm_link(0), pm_alive(true) { }
        };

        scopira::tool::event_area<queue_area> dm_queue;

        scopira::tool::count_ptr<scopira::tool::netflow> dm_sock;

        /// reciever thread
        scopira::tool::thread dm_recv_thread;
        /// sender thread
        scopira::tool::thread dm_send_thread;

      public:
        /**
         * Creates and returns a new net_link object that will be connected
         * to the given remote.
         * This will block on initial DNS and connections, ofcourse.
         * Returns false on error.
         *
         * @author Aleksander Demko
         */ 
        static bool make_link(const scopira::tool::url &remote, scopira::tool::count_ptr<net_link> &nl);

        /// ctor (created by accept() or make_link())
        net_link(scopira::tool::netflow *fl);
        /// dtor
        virtual ~net_link();

        /// set the link that will get recv and send-ok events
        /// you can only set this (non-null) ONCE
        void set_link(link *l);

        /// gets the real network socket
        scopira::tool::netflow * get_sock(void) const { return dm_sock.get(); }

        /**
         * Enqueue the given msg to this link's send queue.
         * @author Aleksander Demko
         */ 
        void enqueue_msg(network_msg *msg);

      private:
        static void* recv_thread_func(void *herep);
        static void* send_thread_func(void *herep);
    };

    /**
     * a link (and possible the connection) to another agent
     * @author Aleksander Demko
     */ 
    class link : public virtual scopira::tool::object
    {
      private:
        typedef std::list<scopira::tool::count_ptr<network_msg> > msgqueue_t;
        // type_tmp_c type msgs are my friends
        friend class peer_hello_msg;
        friend class nottmp_hello_msg;

        cluster_agent *dm_agent;

        /// am i the link initiator?
        bool dm_initter;
        /// remote url... only used when im the initiator
        scopira::tool::url dm_peerurl;
        /// are we on a tmp uuid address?
        bool dm_tmpaddress;

        /// uuid of the peer, but might be "temporary" (not a real uuid)
        /// never zero, never changes
        scopira::tool::uuid dm_peeruuid;

        struct queue_area
        {
          public:
            msgqueue_t pm_sendq;    // pop off the front, append to the back
            bool pm_remotetmp;                // true if the REMOTE end IS tmp
            bool pm_remotemaster;       // true if the remote is a master (ie. this is the master link connection)
            scopira::tool::count_ptr<net_link> pm_link;

          public:
            queue_area(void) : pm_remotetmp(true), pm_remotemaster(false) { }
        };

        scopira::tool::event_area<queue_area> dm_queue;

      public:
        /// ctor
        link(cluster_agent *a, scopira::tool::uuid remoteuuid,
          bool tmpaddress, const scopira::tool::url &initterurl = scopira::tool::url("dummy://hostname/"));

        bool is_tmpaddress(void) const { return dm_tmpaddress; }

        scopira::tool::uuid get_uuid(void) const { assert(!dm_tmpaddress); return dm_peeruuid; }

        // does locking!
        bool has_link(void);
        /// called when a new net_link is to be accepted
        void accept_net_link(net_link *nl);

        /// locks, queues and wakes up the sender thread
        void enqueue_msg(network_msg *msg);

        /// debug thing
        void print_status(void);

        /// are we currenly fully connected?
        //bool is_connected(void);

        // handlers from net_link
        void on_sent(network_msg *msg, scopira::tool::count_ptr<network_msg> &newnetmsg);
        void on_recv(network_msg *msg);
        // called only when the connection is *dropped* (ie. not on natural exits)
        void on_close_net_link(void);
      private:
        void unleash_a_msg(scopira::tool::event_ptr<queue_area> &L, scopira::tool::count_ptr<network_msg> *putnewmsghere);
    };

  private:
    enum {
      failed_initting_c,
      failed_cantinit_c,
      failed_opening_c,
      failed_cantopen_c,
      failed_ok_c,
      failed_lostmaster_c,
    };
    typedef std::map<scopira::tool::uuid, scopira::tool::count_ptr<link> > peers_t;

    /**
     * Kernel Area
     * @author Aleksander Demko
     */ 
    struct peer_area
    {
      public:
        /// link to my master/server, null if I am the cluster master/server
        scopira::tool::count_ptr<link> pm_masterlink;
        /// the links to our peers
        peers_t pm_peers;
        /// alive? (only possible after a successful boot up)
        bool pm_alive;
      public:
        peer_area(void) : pm_alive(false) { }
    };

    typedef std::list<scopira::tool::count_ptr<admin_msg> > msgqueue_t;
    class cron_item {
      public:
        double pm_triggertime;
        scopira::tool::count_ptr<admin_msg> pm_msg;
      public:
        cron_item(void) : pm_triggertime(0) { }
        cron_item(double triggertime, admin_msg *msg) : pm_triggertime(triggertime), pm_msg(msg) { }
        /// needed for priority queue ("greater" means less pm_triggertime)
        bool operator<(const cron_item &rhs) const { return pm_triggertime > rhs.pm_triggertime; }
    };
    typedef std::priority_queue<cron_item> cronqueue_t;

    /**
     * The admin event queue
     *
     * @author Aleksander Demko
     */
    struct admin_area
    {
      public:
        msgqueue_t pm_main_queue;
        cronqueue_t pm_cron_queue;
        /// alive? (only possible after a successful boot up)
        bool pm_alive;
        /// the failed state
        char pm_failedstate;
        /// agenterror handler, if any
        agenterror_reactor_i *pm_agenterror;
      public:
        admin_area(void) : pm_alive(false), pm_failedstate(failed_initting_c), pm_agenterror(0) { }
    };

    /// info about a computational node
    /// lock the meta area before changing any of this!!
    class meta_node : public scopira::tool::object
    {
      public:
        // all read only
        machine_spec pm_machinespec;
        node_spec pm_nodespec;
        // except for this
        typedef std::vector<scopira::tool::uuid> taskslist_t;
        struct load_area
        {
          taskslist_t pm_tasks;
        };
        scopira::tool::event_area<load_area> pm_loadarea;
      public:
        meta_node(const machine_spec &mspec, const node_spec &nspec);
    };
    typedef std::map<scopira::tool::uuid, scopira::tool::count_ptr<meta_node> > nodemap_t;
    typedef std::map<scopira::tool::uuid, scopira::tool::uuid> taskmap_t;

    /**
     * Where we keep information about the workers/cluster/etc.
     *
     * Only used in the MASTER process.
     *
     * @author Aleksander Demko
     */
    struct meta_area
    {
      public:
        /// maps agentids to node specs
        nodemap_t pm_nodes;
        /// maps tasks to their agents
        taskmap_t pm_tasks;
    };

    typedef std::map<int, scopira::tool::count_ptr<network_msg> > netmsgmap_t;
    typedef std::map<int, scopira::tool::count_ptr<reply_msg> > repmsgmap_t;

    struct xtion_area
    {
      public:
        /// for type I do_xtion
        repmsgmap_t pm_replies;
        /// for type II do_xtion
        /// the ->second might be null if the caller wanted a async handler but DIDNT care about the reply
        netmsgmap_t pm_handlers;
        /// next id to alloc
        int pm_nextid;
        /// still alive?
        bool pm_alive;

        xtion_area(void) : pm_nextid(3000), pm_alive(true) { }
    };

    typedef std::map<scopira::tool::uuid, scopira::tool::count_ptr<reply_resolve_agent_msg> > resolvemap_t;

    // my "dns" cache
    struct resolve_area
    {
      public:
        resolvemap_t pm_resolves;
    };

    /// my UUID generator
    scopira::tool::uuid_generator dm_uuidmaker;
    /// the age of this agent (ie. constant running chrono since agent start up)
    scopira::tool::chrono dm_age;

    /// my info
    node_spec dm_nodespec;

    /// cluster password, if any
    std::string dm_password;
    /// the port we're listenin on, 0 if none
    int dm_listenport;
    /// the udp port we're listening to, 0 if none
    /// this is only set for servers
    int dm_udpport;

    /// kernel area
    scopira::tool::event_area<peer_area> dm_peerarea;
    /// admin area
    scopira::tool::event_area<admin_area> dm_adminarea;
    /// meta (info about the cluster) area
    scopira::tool::event_area<meta_area> dm_metarea;
    /// the xtion synx area
    scopira::tool::event_area<xtion_area> dm_xtionarea;
    /// the resolve/dns cache area
    scopira::tool::event_area<resolve_area> dm_resolvearea;

    /// listen socket
    scopira::tool::netflow dm_listensocket;
    /// udp socket (only used in servers) (0 if not used)
    scopira::tool::udpflow dm_udpsocket;
    /// listen thread
    scopira::tool::thread dm_listenthread;
    /// udp listening thread... only active if dm_udpsocket!=0
    scopira::tool::thread dm_udpthread;
    /// admin thread (this processes the admin event queue)
    scopira::tool::thread dm_adminthread;

  private:
    /**
     * Enqueue the given msg. type_instant_c msgs will be ran in this (the caller's) thread.
     * @author Aleksander Demko
     */ 
    void enqueue_msg(admin_msg *msg);
    /**
     * Enqueue the given msg to the given target agent.
     * If targetuuid.is_zero, then "master agent" will be used.
     * @author Aleksander Demko
     */ 
    void enqueue_msg(scopira::tool::uuid targetuuid, network_msg *msg);
    /**
     * Enqueus the given msg as a reply to the source msg.
     * @author Aleksander Demko
     */ 
    void enqueue_reply_msg(network_msg *theq, reply_msg *thereply)
      { enqueue_reply_msg(theq->get_xtion(), thereply); }
    /**
     * Enqueus the given msg as a reply to the source msg.
     * @author Aleksander Demko
     */ 
    void enqueue_reply_msg(const network_msg::xtion_t &x, reply_msg *thereply);
    /**
     * Same as enqueue_msg(msg), but will only run this even in w seconds.
     *
     * @author Aleksander Demko
     */
    void enqueue_cron_msg(double w, admin_msg *msg);

    /**
     * Do a transation. This is known as type-I; blocking-edition.
     * If targetuuid.is_zero, then the master will receive this msg.
     *
     * @author Aleksander Demko
     */
    void do_xtion(scopira::tool::uuid targetuuid,
        network_msg *msg, scopira::tool::count_ptr<reply_msg> &replye);
    /**
     * Do a transation. This is known as type-II; async-handler-edition.
     * If targetuuid.is_zero, then the master will receive this msg.
     * Hanlder MAY be null (for "dont care about reply").
     *
     * @author Aleksander Demko
     */
    void enqueue_xtion(scopira::tool::uuid targetuuid,
        network_msg *msg, network_msg *handler);

    /**
     * Do a look up.
     * This will first consult the internal cache for massive speed up potensial.
     * @author Aleksander Demko
     */ 
    void do_resolve_xtion(scopira::tool::uuid id, bool dotaskresolve,
        scopira::tool::count_ptr<reply_resolve_agent_msg> &out);

    /// debug routine
    void print_status(void);

  private:
    static void* listen_thread_func(void *herep);
    static void* udp_thread_func(void *herep);
    static void* admin_thread_func(void *herep);
};

#endif

