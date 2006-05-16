
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

#include <scopira/agent/clusteragent.h>

#include <algorithm>

#include <scopira/tool/objflowloader.h>
#include <scopira/tool/util.h>
#include <scopira/tool/polyflow.h>
#include <scopira/tool/time.h>
#include <scopira/core/loop.h>
#include <scopira/core/register.h>

//BBtargets libscopira.so

using namespace scopira::tool;
using namespace scopira::agent;
using scopira::core::basic_loop;
using scopira::basekit::narray;

//#define CLUSTER_OUTPUT

//
// internal objects:
//

static bool find_udp_server(scopira::tool::url &autourl);

class cluster_agent::make_link_msg : public cluster_agent::admin_msg
{
  private:
    scopira::tool::uuid dm_targetuuid;
    scopira::tool::url dm_targeturl;
    bool dm_makingmaster;
  public:
    static void enqueue_new_master_link_msg(cluster_agent &e, const scopira::tool::url &masterurl);
    static void enqueue_new_reconnect_link_msg(cluster_agent &e, const scopira::tool::url &rurl, scopira::tool::uuid id);
    static void enqueue_new_peer_link_msg(cluster_agent &e, scopira::tool::uuid id, network_msg *msg);

    virtual const char * get_debug_name(void) const { return "make_link_msg"; }
    virtual void execute_agent(cluster_agent &e);

  private:
    // use zero uuid for temp
    make_link_msg(scopira::tool::uuid u, const scopira::tool::url &l, bool makingmaster);
};

class cluster_agent::quit_msg : public cluster_agent::admin_msg
{
  private:
    short dm_counter;
    bool dm_agenterror;
  public:
    quit_msg(bool agenterror);
    virtual const char * get_debug_name(void) const { return "quit_msg"; }
    virtual void execute_agent(cluster_agent &e);
};

class cluster_agent::remove_link_msg : public cluster_agent::admin_msg
{
  private:
    uuid dm_rmuuid;
  public:
    remove_link_msg(scopira::tool::uuid rmuuid);
    virtual const char * get_debug_name(void) const { return "remove_link_msg"; }
    virtual void execute_agent(cluster_agent &e);
};

class cluster_agent::peer_hello_msg : public cluster_agent::network_msg
{
  private:
    typedef network_msg parent_type;

    machine_spec dm_sendermspec;
    node_spec dm_sendernspec;

    int dm_senderlistenport;
    bool dm_senderistmp;
  public:
    peer_hello_msg(void);
    peer_hello_msg(const machine_spec &mspec, const node_spec &nspec, int senderlistenport, bool senderistmp);
    virtual bool load(scopira::tool::iobjflow_i& in);
    virtual void save(scopira::tool::oobjflow_i& out) const;
    virtual const char * get_debug_name(void) const { return "peer_hello_msg"; }
    virtual short get_type(void) const { return type_tmp_c; }
    virtual void execute_agent(cluster_agent &e, cluster_agent::link *lk);
};

class cluster_agent::nottmp_hello_msg : public cluster_agent::network_msg
{
  public:
    nottmp_hello_msg(void);
    virtual const char * get_debug_name(void) const { return "nottmp_hello_msg"; }
    virtual short get_type(void) const { return type_tmp_c; }
    virtual void execute_agent(cluster_agent &e, cluster_agent::link *lk);
};

class cluster_agent::make_back_link_msg : public cluster_agent::network_msg
{
  private:
    scopira::tool::uuid dm_makelinkto, dm_makelinkfrom;
  public:
    make_back_link_msg(void);
    make_back_link_msg(scopira::tool::uuid makelinkto, scopira::tool::uuid makelinkfrom);
    virtual bool load(scopira::tool::iobjflow_i& in);
    virtual void save(scopira::tool::oobjflow_i& out) const;
    virtual const char * get_debug_name(void) const { return "make_back_link_msg"; }
    virtual void execute_agent(cluster_agent &e, cluster_agent::link *lk);
};

class cluster_agent::all_quit_msg : public cluster_agent::network_msg
{
  private:
    bool dm_initter;    // no transported
  public:
    all_quit_msg(void);
    all_quit_msg(bool initter);
    virtual const char * get_debug_name(void) const { return "all_quit_msg"; }
    virtual void execute_agent(cluster_agent &e, cluster_agent::link *lk);
};

class cluster_agent::resolve_agent_msg : public cluster_agent::network_msg
{
  public:
    scopira::tool::uuid pm_idtofind;
    bool pm_dotask;   // true if pm_idtofind is a TASKid, false if it's an AGENTid
  public:
    resolve_agent_msg(void);
    resolve_agent_msg(scopira::tool::uuid id, bool dotaskresolve);
    virtual bool load(scopira::tool::iobjflow_i& in);
    virtual void save(scopira::tool::oobjflow_i& out) const;
    virtual const char * get_debug_name(void) const { return "resolve_agent_msg"; }
    virtual short get_type(void) const { return type_instant_c; }
    virtual void execute_agent(cluster_agent &e, cluster_agent::link *lk);
};

class cluster_agent::reply_resolve_agent_msg : public cluster_agent::reply_msg
{
  public:
    scopira::tool::uuid pm_agentuuid;
    scopira::tool::url pm_agenturl;
    bool pm_isdirectroute;
  public:
    reply_resolve_agent_msg(void);
    reply_resolve_agent_msg(int errorcode, scopira::tool::uuid agentuuid,
        const scopira::tool::url &repurl, bool isdirectroute);
    virtual bool load(scopira::tool::iobjflow_i& in);
    virtual void save(scopira::tool::oobjflow_i& out) const;
    virtual const char * get_debug_name(void) const { return "reply_resolve_agent_msg"; }
};

class cluster_agent::routed_msg : public cluster_agent::network_msg
{
  private:
    scopira::tool::uuid dm_finalagent;
    scopira::tool::count_ptr<cluster_agent::network_msg> dm_finalmsg;
  public:
    routed_msg(void);
    routed_msg(scopira::tool::uuid finalagent, cluster_agent::network_msg *finalmsg);
    virtual bool load(scopira::tool::iobjflow_i& in);
    virtual void save(scopira::tool::oobjflow_i& out) const;
    virtual const char * get_debug_name(void) const { return "routed_msg"; }
    virtual void execute_agent(cluster_agent &e, cluster_agent::link *lk);
};

class cluster_agent::master_launch_task_msg : public cluster_agent::network_msg
{
  public:
    uuid dm_srcagent;
    std::string dm_typestring;
    int dm_numps;   /// number of processes to make
    uuid dm_existingmaster;
  public:
    master_launch_task_msg(void);
    master_launch_task_msg(scopira::tool::uuid srcagent, const std::string &typestring, int numps, const uuid &existingmaster = uuid());
    virtual bool load(scopira::tool::iobjflow_i& in);
    virtual void save(scopira::tool::oobjflow_i& out) const;
    virtual const char * get_debug_name(void) const { return "master_launch_task_msg"; }
    virtual void execute_agent(cluster_agent &e, cluster_agent::link *lk);
};

class cluster_agent::reply_master_launch_task_msg : public cluster_agent::reply_msg
{
  public:
    narray<uuid> pm_taskids;
  public:
    reply_master_launch_task_msg(void);
    reply_master_launch_task_msg(const narray<uuid> &ids);
    virtual bool load(scopira::tool::iobjflow_i& in);
    virtual void save(scopira::tool::oobjflow_i& out) const;
    virtual const char * get_debug_name(void) const { return "reply_master_launch_task_msg"; }
};

class cluster_agent::make_task_msg : public cluster_agent::network_msg
{
  private:
    // does this stuff need to be public for non-reply msgs?
    int dm_myindex;
    narray<uuid> dm_taskids;
    std::string dm_typestring;
    short dm_phase;
  public:
    make_task_msg(void);
    make_task_msg(int myindex, const narray<uuid> &ids, const std::string &typestring, short phase);
    virtual bool load(scopira::tool::iobjflow_i& in);
    virtual void save(scopira::tool::oobjflow_i& out) const;
    virtual const char * get_debug_name(void) const { return "make_task_msg"; }
    virtual short get_type(void) const { return type_instant_c; }   // oh oh
    virtual void execute_agent(cluster_agent &e, cluster_agent::link *lk);
};

class cluster_agent::client_wait_msg : public cluster_agent::network_msg
{
  private:
    scopira::tool::uuid dm_taskid;
    int dm_msec;
  public:
    client_wait_msg(void);
    client_wait_msg(scopira::tool::uuid taskid, int msec);
    virtual bool load(scopira::tool::iobjflow_i& in);
    virtual void save(scopira::tool::oobjflow_i& out) const;
    virtual const char * get_debug_name(void) const { return "client_wait_msg"; }
    virtual void execute_agent(cluster_agent &e, cluster_agent::link *lk);
};

class cluster_agent::client_wait_task : public agent_task_i
{
  private:
    cluster_agent *dm_agent;
    network_msg::xtion_t dm_xtion;
    scopira::tool::uuid dm_taskid;
    int dm_msec;
  public:
    client_wait_task(cluster_agent *ag, const network_msg::xtion_t &x, uuid &taskid, int msec);
    virtual int run(scopira::agent::task_context &e);
};

class cluster_agent::client_kill_msg : public cluster_agent::network_msg
{
  private:
    scopira::tool::uuid dm_taskid;
  public:
    client_kill_msg(void);
    client_kill_msg(scopira::tool::uuid taskid);
    virtual bool load(scopira::tool::iobjflow_i& in);
    virtual void save(scopira::tool::oobjflow_i& out) const;
    virtual const char * get_debug_name(void) const { return "client_kill_msg"; }
    virtual void execute_agent(cluster_agent &e, cluster_agent::link *lk);
};

class cluster_agent::client_is_alive_msg : public cluster_agent::network_msg
{
  private:
    scopira::tool::uuid dm_taskid;
  public:
    client_is_alive_msg(void);
    client_is_alive_msg(scopira::tool::uuid taskid);
    virtual bool load(scopira::tool::iobjflow_i& in);
    virtual void save(scopira::tool::oobjflow_i& out) const;
    virtual const char * get_debug_name(void) const { return "client_is_alive_msg"; }
    virtual void execute_agent(cluster_agent &e, cluster_agent::link *lk);
};

class cluster_agent::send_data_msg : public cluster_agent::network_msg
{
  private:
    scopira::tool::uuid dm_src, dm_dest;
    scopira::tool::count_ptr<scopira::tool::bufferflow> dm_buf;
  public:
    send_data_msg(void);
    send_data_msg(scopira::tool::uuid src, scopira::tool::uuid dest, scopira::tool::bufferflow *buf);
    virtual bool load(scopira::tool::iobjflow_i& in);
    virtual void save(scopira::tool::oobjflow_i& out) const;
    virtual const char * get_debug_name(void) const { return "send_data_msg"; }
    virtual void execute_agent(cluster_agent &e, cluster_agent::link *lk);
};

class cluster_agent::bcast_data_msg : public cluster_agent::network_msg
{
  private:
    scopira::tool::uuid dm_src, dm_destservice;
    scopira::tool::count_ptr<scopira::tool::bufferflow> dm_buf;
  public:
    bcast_data_msg(void);
    bcast_data_msg(scopira::tool::uuid src, scopira::tool::uuid destserviceid, scopira::tool::bufferflow *buf);
    virtual bool load(scopira::tool::iobjflow_i& in);
    virtual void save(scopira::tool::oobjflow_i& out) const;
    virtual const char * get_debug_name(void) const { return "bcast_data_msg"; }
    virtual void execute_agent(cluster_agent &e, cluster_agent::link *lk);
};

class cluster_agent::reg_context_msg : public cluster_agent::network_msg
{
  private:
    uuid dm_ctxid;
    bool dm_regit;
    uuid dm_agentid;
  public:
    reg_context_msg(void);
    reg_context_msg(scopira::tool::uuid ctxid, bool regit,
        scopira::tool::uuid agentid);
    virtual bool load(scopira::tool::iobjflow_i& in);
    virtual void save(scopira::tool::oobjflow_i& out) const;
    virtual const char * get_debug_name(void) const { return "reg_context_msg"; }
    virtual void execute_agent(cluster_agent &e, cluster_agent::link *lk);
};

class cluster_agent::dead_task_msg : public cluster_agent::network_msg
{
  private:
    uuid dm_taskid;
  public:
    dead_task_msg(void);
    dead_task_msg(scopira::tool::uuid taskid);
    virtual bool load(scopira::tool::iobjflow_i& in);
    virtual void save(scopira::tool::oobjflow_i& out) const;
    virtual const char * get_debug_name(void) const { return "dead_task_msg"; }
    virtual void execute_agent(cluster_agent &e, cluster_agent::link *lk);
};

//
// node_spec
//

node_spec::node_spec(void)
{
  pm_ismaster = false;
  pm_isdirectroute = true;
  pm_jobfilter = all_jobs_c;
}

bool node_spec::load(scopira::tool::itflow_i& in)
{
  return
    pm_uuid.load(in) &&
    in.read_bool(pm_ismaster) &&
    pm_url.load(in) &&
    in.read_bool(pm_isdirectroute) &&
    in.read_short(pm_jobfilter);
}

void node_spec::save(scopira::tool::otflow_i& out) const
{
  pm_uuid.save(out);
  out.write_bool(pm_ismaster);
  pm_url.save(out);
  out.write_bool(pm_isdirectroute);
  out.write_short(pm_jobfilter);
}

void node_spec::parse_options(const std::string &options)
{
  if (options.find("direct") != std::string::npos)
    pm_isdirectroute = true;
  if (options.find("tunnel") != std::string::npos || options.find("client") != std::string::npos)
    pm_isdirectroute = false;
  if (options.find("alljobs") != std::string::npos)
    pm_jobfilter = all_jobs_c;
  if (options.find("myjobs") != std::string::npos)
    pm_jobfilter = my_jobs_c;
  if (options.find("nojobs") != std::string::npos || options.find("client") != std::string::npos)
    pm_jobfilter = no_jobs_c;
}

scopira::tool::oflow_i& operator<<(scopira::tool::oflow_i& o, const scopira::agent::node_spec &spec)
{
  o << "[uuid=" << spec.pm_uuid << " ismaster=" << spec.pm_ismaster <<
    " url=" << spec.pm_url << " isidirectroute=" << spec.pm_isdirectroute << " jobfilter=" << spec.pm_jobfilter << ']';
  return o;
}

//
//
// cluster_agent
//
//

cluster_agent::cluster_agent(void)
  : dm_listenport(0), dm_udpport(0),
    dm_listenthread(listen_thread_func, this),
    dm_udpthread(udp_thread_func, this),
    dm_adminthread(admin_thread_func, this)
{
  int i;
  url serverurl;

  // pick an ID for myself
  dm_uuidmaker.next(dm_nodespec.pm_uuid);
  // stat the timer
  dm_age.start();

  // param stuff

  dm_nodespec.pm_ismaster = true;

  // determinate my agent type
  if (basic_loop::instance()->get_config("cluster") == "server" || 
      basic_loop::instance()->get_config("cluster") == "serveronly") {
    if (basic_loop::instance()->get_config("cluster") == "serveronly")
      dm_nodespec.pm_jobfilter = node_spec::my_jobs_c;      // this server dont work, yo
  } else {
    // some type of url

    // alias processing
    if (basic_loop::instance()->get_config("cluster") == "autoserver")
      serverurl.set_url("scopira://255.255.255.255/autoserver"); // automatic + server... be a server if nothing can be found via udp
    else if (basic_loop::instance()->get_config("cluster") == "auto")
      serverurl.set_url("scopira://255.255.255.255/broadcast");
    else if (basic_loop::instance()->get_config("cluster") == "autoclient")
      serverurl.set_url("scopira://255.255.255.255/broadcast,client");
    else if (!serverurl.set_url(basic_loop::instance()->get_config("cluster"))) {
      // Not a server, and the user gave me a url
      OUTPUT << "Failed to parse URL, cluster=" << basic_loop::instance()->get_config("cluster") << '\n';
      dm_adminarea.pm_data.pm_failedstate = failed_cantinit_c;
      return;
    }

    // assume non-master
    dm_nodespec.pm_ismaster = false;

    // check if we need to do an autoserver thing
    if (serverurl.get_filename().find("autoserver") != std::string::npos) {
      if (find_udp_server(serverurl))
        OUTPUT << "Server found.\n serverurl=" << serverurl << '\n';
      else {
        // am master after all
        dm_nodespec.pm_ismaster = true;
        OUTPUT << "Server not found - assuming server role\n";
      }
    } else if (serverurl.get_filename().find("broadcast") != std::string::npos) {
      // check if the url was with the scopirafind:// protocol, and if so, use udp to find a
      // real server
      if (find_udp_server(serverurl))
        OUTPUT << "Server found.\n serverurl=" << serverurl << '\n';
      else {
        OUTPUT << "Failed to find a server via: " << serverurl << '\n';
        dm_adminarea.pm_data.pm_failedstate = failed_cantinit_c;
        return;
      }
    }

    // not a master server

    if (serverurl.get_proto() != "scopira") {
      OUTPUT << "Protocol must be scopira://\n";
      dm_adminarea.pm_data.pm_failedstate = failed_cantinit_c;
      return;
    }

    dm_nodespec.parse_options(serverurl.get_filename());
  }

  // if it doesnt exist, this will return "", which is ok.
  dm_password = basic_loop::instance()->get_config("clusterpassword");

  if (dm_nodespec.pm_isdirectroute) {
    // am i doing any listening?

    // parse the listening port
    if (basic_loop::instance()->has_config("clusterport")) {
      if (!string_to_int(basic_loop::instance()->get_config("clusterport"), dm_listenport)) {
        OUTPUT << "Failed to parse a valid port from clusterport=" << basic_loop::instance()->get_config("clusterport") << '\n';
        dm_adminarea.pm_data.pm_failedstate = failed_cantinit_c;
        return;
      }
    } else {
      dm_listenport = default_port_c;
      if (!dm_nodespec.pm_ismaster)
        dm_listenport += 10;
    }

    // start the port listener thread
    for (i=0; i<100; ++i) {
      dm_listensocket.open(0, dm_listenport+i);
      if (!dm_listensocket.failed())
        break;    // success!
    }
    if (dm_listensocket.failed()) {
      OUTPUT << "Failed to open listen port from " << dm_listenport << " on\n";
      dm_adminarea.pm_data.pm_failedstate = failed_cantinit_c;
      return;
    }
    dm_listenport = dm_listenport + i;    // record the actual port

    // try to open the udp/broadcast listen stuff
    if (dm_nodespec.pm_ismaster && dm_listenport != 0) {
      dm_udpsocket.open(dm_listenport);

      if (dm_udpsocket.failed())
        dm_udpport = 0;
      else
        dm_udpport = dm_listenport;
    }
  }

  dm_nodespec.pm_url = url("scopira://" + get_hostname() + ":" + int_to_string(dm_listenport));

  // we're all ok now, lets start up
  if (dm_nodespec.pm_ismaster) {
    // add myself into the table, as I'm a server
    event_ptr<meta_area> L(dm_metarea);

    L->pm_nodes[dm_nodespec.pm_uuid] = new meta_node(la_get_spec(), dm_nodespec);
  }

  // at this point, we're live (seconds try, doesnt matter)
  dm_adminarea.pm_data.pm_failedstate = failed_opening_c;    // masters are always ok
  dm_peerarea.pm_data.pm_alive = true;
  dm_adminarea.pm_data.pm_alive = true;

  // start worker threads ??? (already done in parent class)

  // start listen thread
  if (dm_nodespec.pm_isdirectroute) {
    dm_listenthread.start();
    if (dm_udpport != 0)
      dm_udpthread.start();
  }
  dm_adminthread.start();

  OUTPUT << "Cluster Agent started.\n"
    " uuid=" << dm_nodespec.pm_uuid << "\n";
  if (dm_listenport == 0)
    ;
  else if (dm_nodespec.pm_ismaster)
    OUTPUT << " serverurl=" << dm_nodespec.pm_url << '\n';
  else
    OUTPUT << " nodeurl=" << dm_nodespec.pm_url << '\n';
  if (dm_udpport != 0)
    OUTPUT << " udpport=" << dm_udpport << '\n';
  OUTPUT << '\n';

  if (dm_nodespec.pm_ismaster)
    dm_adminarea.pm_data.pm_failedstate = failed_ok_c;    // masters are always ok
  else
    make_link_msg::enqueue_new_master_link_msg(*this, serverurl);   // link to master
}

cluster_agent::~cluster_agent()
{
  int state = event_ptr<admin_area>(dm_adminarea)->pm_failedstate;

  if (state != failed_initting_c && state != failed_cantinit_c) {
    notify_stop();

    // we must have been alive, so lets wait for the threads
    if (dm_nodespec.pm_isdirectroute)
      dm_listenthread.wait_stop();
    // wait for ALL the links (including master) to die
    dm_adminthread.wait_stop();
    if (dm_udpport != 0)
      dm_udpthread.wait_stop();
  }

  // lets explictly clear the peer are now, as its filled with threads
  {
    event_ptr<peer_area> L(dm_peerarea);

    L->pm_peers.clear();
    L->pm_masterlink = 0;
  }
}

void cluster_agent::notify_stop(void)
{
  {
    event_ptr<peer_area> L(dm_peerarea);

    L->pm_alive = false;
    L.notify_all();
  }
  {
    event_ptr<admin_area> L(dm_adminarea);

    L->pm_alive = false;
    L.notify_all();
  }
  {
    event_ptr<xtion_area> L(dm_xtionarea);

    L->pm_alive = false;
    L.notify_all();
  }

  parent_type::notify_stop();
}

void cluster_agent::enqueue_network_quit(void)
{
  enqueue_msg(new all_quit_msg(true));
}

bool cluster_agent::failed(void) const
{
  event_ptr<admin_area> L(dm_adminarea);
  chrono c;

  c.start();

  // 30 sec timeout
  while (L->pm_failedstate == failed_opening_c && L->pm_alive && c.get_running_time() < 30)
    L.wait();

  // we're failed if we're not alive or not ok
  return L->pm_failedstate != failed_ok_c;
}

void cluster_agent::set_agenterror_reactor(agenterror_reactor_i *r)
{
  event_ptr<admin_area> L(dm_adminarea);

  L->pm_agenterror = r;
}

void cluster_agent::reg_context(scopira::tool::uuid &ctxid, taskmsg_reactor_i *reac)
{
  local_agent::reg_context(ctxid, reac);

  assert(!ctxid.is_zero());

  count_ptr<reply_msg> replye;

  /// ctxid should have the new id now, lets use it
  do_xtion(uuid(), new reg_context_msg(ctxid, true, dm_nodespec.pm_uuid), replye);
}

void cluster_agent::unreg_context(scopira::tool::uuid ctxid)
{
  assert(!ctxid.is_zero());
  local_agent::unreg_context(ctxid);

  count_ptr<reply_msg> replye;

  do_xtion(uuid(), new reg_context_msg(ctxid, false, dm_nodespec.pm_uuid), replye);
}

scopira::tool::uuid cluster_agent::launch_task(const std::type_info &t)
{
  uuid psid;
  std::string typestring;

  assert(objflowloader::instance()->has_typeinfo(t) && "[Given type to launch_task is not registered]");
  typestring = objflowloader::instance()->get_name(t);

  count_ptr<master_launch_task_msg> q = new master_launch_task_msg(dm_nodespec.pm_uuid, typestring, 1);
  count_ptr<reply_msg> replye;
  reply_master_launch_task_msg *rep;

  // send it off
  do_xtion(uuid(), q.get(), replye);

  assert(replye.get());
  assert(replye->get_code() == 0);
  rep = dynamic_cast<reply_master_launch_task_msg*>(replye.get());
  assert(rep);

  psid = rep->pm_taskids[0];

  return psid;
}

scopira::tool::uuid cluster_agent::launch_group(int numps, const std::type_info &t)
{
  uuid psid;
  std::string typestring;

  assert(objflowloader::instance()->has_typeinfo(t) && "[Given type to launch_task is not registered]");
  typestring = objflowloader::instance()->get_name(t);

  count_ptr<master_launch_task_msg> q = new master_launch_task_msg(dm_nodespec.pm_uuid, typestring, numps);
  count_ptr<reply_msg> replye;
  reply_master_launch_task_msg *rep;

  // send it off
  do_xtion(uuid(), q.get(), replye);

  assert(replye.get());
  assert(replye->get_code() == 0);
  rep = dynamic_cast<reply_master_launch_task_msg*>(replye.get());
  assert(rep);

  psid = rep->pm_taskids[0];

  return psid;
}

void cluster_agent::launch_slaves(scopira::tool::uuid masterid, int numtotalps, const std::type_info &t,
      narray<uuid> &peers)
{
  std::string typestring;

  assert(!masterid.is_zero());

  assert(objflowloader::instance()->has_typeinfo(t) && "[Given type to launch_task is not registered]");
  typestring = objflowloader::instance()->get_name(t);

  count_ptr<master_launch_task_msg> q = new master_launch_task_msg(dm_nodespec.pm_uuid, typestring, numtotalps, masterid);
  count_ptr<reply_msg> replye;
  reply_master_launch_task_msg *rep;

  // send it off
  do_xtion(uuid(), q.get(), replye);

  assert(replye.get());
  assert(replye->get_code() == 0);
  rep = dynamic_cast<reply_master_launch_task_msg*>(replye.get());
  assert(rep);

  // return all the IDs
  peers = rep->pm_taskids;

  la_update_slave_master(masterid, peers);
}

void cluster_agent::kill_task(scopira::tool::uuid ps)
{
  assert(!ps.is_zero());
  count_ptr<reply_resolve_agent_msg> reso;

  do_resolve_xtion(ps, true, reso);

  if (reso.is_null())
    return;

  // send that subnode the wait even
  count_ptr<network_msg> netm;
  count_ptr<reply_msg> replye;

  netm = new client_kill_msg(ps);
  // just to make this sync, i guess?
  do_xtion(reso->pm_agentuuid, netm.get(), replye);
}

bool cluster_agent::wait_task(scopira::tool::uuid ps, int msec)
{
  assert(!ps.is_zero());
  count_ptr<reply_resolve_agent_msg> reso;

  // resolve where that task is
  do_resolve_xtion(ps, true, reso);

  if (reso.is_null())
    return true;

  // send that subnode the wait even
  count_ptr<network_msg> netm;
  count_ptr<reply_msg> replye;

  netm = new client_wait_msg(ps, msec);
  do_xtion(reso->pm_agentuuid, netm.get(), replye);

  // return true only if we have an event and its ok
  return replye.get() && (replye->get_code() == 0);
}

bool cluster_agent::is_alive_task(scopira::tool::uuid ps)
{
  assert(!ps.is_zero());
  count_ptr<reply_resolve_agent_msg> reso;

  // resolve where that task is
  do_resolve_xtion(ps, true, reso);

  if (reso.is_null())
    return false;

  // send that subnode the wait even
  count_ptr<network_msg> netm;
  count_ptr<reply_msg> replye;

  netm = new client_is_alive_msg(ps);
  do_xtion(reso->pm_agentuuid, netm.get(), replye);

  // return true only if we have an event and its ok
  return replye.get() && replye->get_code() == 0;
}

void cluster_agent::send_msg_bcast(scopira::tool::uuid src, scopira::tool::uuid destserviceid, scopira::tool::bufferflow *buf)
{
  // compose and send the bcast msg even to the master
  assert(!destserviceid.is_zero());

  // send the bcast msg to the master (so he can do the real distrbution)
  count_ptr<network_msg> netm;

  netm = new bcast_data_msg(src, destserviceid, buf);
  enqueue_msg(uuid(), netm.get());
}

void cluster_agent::la_send_msg(scopira::tool::uuid src, scopira::tool::uuid dest, scopira::tool::bufferflow *buf)
{
  assert(!dest.is_zero());
  count_ptr<reply_resolve_agent_msg> reso;

  // resolve where that task is
  do_resolve_xtion(dest, true, reso);

  if (reso.is_null())
    return;

  // send that subnode the wait even
  count_ptr<network_msg> netm;

  netm = new send_data_msg(src, dest, buf);
  enqueue_msg(reso->pm_agentuuid, netm.get());
}

void cluster_agent::la_dead_task(scopira::tool::uuid taskid)
{
  // signal the master of this death
  enqueue_msg(uuid(), new dead_task_msg(taskid));
}

void cluster_agent::enqueue_msg(admin_msg *msg)
{
  assert(msg->is_alive_object());

#ifdef CLUSTER_OUTPUT
  OUTPUT << "\033[31mcluster_agent::enqueue_msg " << msg->get_debug_name() << "\033[0m\n";
#endif

  if (msg->get_type() == admin_msg::type_instant_c) {
    count_ptr<admin_msg> ref(msg);    // cuz caller always assume im gonna ref count it
#ifdef CLUSTER_OUTPUT
    OUTPUT << "<\033[31m\033[1minstant_run\033[0m running=" << ref->get_debug_name() << " @=" << reinterpret_cast<long>(ref.get()) << ">\n";
#endif
    msg->execute_agent(*this);
#ifdef CLUSTER_OUTPUT
    OUTPUT << "</instant_run>\n";
#endif
    return;
  }

  // not instant? enqueue for the admin thread
  {
    event_ptr<admin_area> L(dm_adminarea);
    L->pm_main_queue.push_back(msg);
  }
  dm_adminarea.pm_condition.notify_all();
}

void cluster_agent::enqueue_msg(scopira::tool::uuid targetuuid, network_msg *msg)
{
  count_ptr<link> lk;

  assert(msg->is_alive_object());
#ifdef CLUSTER_OUTPUT
  OUTPUT << "\033[31mcluster_agent::enqueue_msg-target " << targetuuid << "; " << msg->get_debug_name() << "\033[0m\n";
#endif

  if ( (targetuuid == dm_nodespec.pm_uuid) || (targetuuid.is_zero() && dm_nodespec.pm_ismaster) ) {
    // that me, dont use the links
    enqueue_msg(msg);
    return;
  }

  {
    event_ptr<peer_area> L(dm_peerarea);

    if (targetuuid.is_zero()) {
      while ((L->pm_masterlink.is_null() || L->pm_masterlink->is_tmpaddress()) && L->pm_alive)
        L.wait();
      if (!L->pm_alive)
        return;
      lk = L->pm_masterlink;
      assert(lk.get());
    } else {
      peers_t::iterator ii = L->pm_peers.find(targetuuid);

      if (ii != L->pm_peers.end())
        lk = ii->second;
    }
  }

  if (lk.get()) {
    lk->enqueue_msg(msg);
    return;
  }

  // if lk is null here, then we're doing a direct-to-peer
  // msg for a node we DONT know about yet (a complex case)
  make_link_msg::enqueue_new_peer_link_msg(*this, targetuuid, msg);
}

void cluster_agent::enqueue_reply_msg(const network_msg::xtion_t &x, reply_msg *thereply)
{
  thereply->set_xtion(x);
  assert(!x.pm_src.is_zero());
  enqueue_msg(x.pm_src, thereply);
}

void cluster_agent::enqueue_cron_msg(double w, admin_msg *msg)
{
  assert(msg->is_alive_object());

#ifdef CLUSTER_OUTPUT
  OUTPUT << "\033[31mcluster_agent::enqueue_cron_msg time=" << w
    << " name=" << msg->get_debug_name() << "\033[0m\n";
#endif

  {
    event_ptr<admin_area> L(dm_adminarea);
    // put this timed even on our priority queue
    L->pm_cron_queue.push(cron_item(dm_age.get_running_time() + w, msg));
  }
  dm_adminarea.pm_condition.notify_all();
}

void cluster_agent::do_xtion(scopira::tool::uuid targetuuid,
  network_msg *msg, scopira::tool::count_ptr<reply_msg> &replye)
{
  int xid;

  replye = 0;

  // get the next xid
  {
    event_ptr<xtion_area> L(dm_xtionarea);

    xid = L->pm_nextid;
    ++L->pm_nextid;

    // inset my reply marker with an empty pointer
    L->pm_replies[xid] = 0;
  }

  // mark this object
  msg->set_xtion(network_msg::xtion_t(dm_nodespec.pm_uuid, xid));

#ifdef CLUSTER_OUTPUT
  OUTPUT << "<\033[32mxtion\033[0m: " << xid << " name=" << msg->get_debug_name() << ">\n";
#endif
  // send it off
  enqueue_msg(targetuuid, msg);

  // wait for the reply
  {
    event_ptr<xtion_area> L(dm_xtionarea);
    repmsgmap_t::iterator ii;

    while (L->pm_alive && replye.is_null())
      if ( ( (ii = L->pm_replies.find(xid)) != L->pm_replies.end() ) && ii->second.get() ) {
        replye = ii->second;
        assert(replye.get());
        L->pm_replies.erase(ii);
      } else
        L.wait();
  }//locking area
#ifdef CLUSTER_OUTPUT
  OUTPUT << "</\033[32mxtion\033[0m: " << xid << ">\n";
#endif
}

void cluster_agent::enqueue_xtion(scopira::tool::uuid targetuuid,
  network_msg *msg, network_msg *handler)
{
  int xid;

  assert(msg->is_alive_object());

  // insert the handler into the pool
  {
    event_ptr<xtion_area> L(dm_xtionarea);

    xid = L->pm_nextid;
    ++L->pm_nextid;

    L->pm_handlers[xid] = handler;
  }

  // mark this object
  msg->set_xtion(network_msg::xtion_t(dm_nodespec.pm_uuid, xid));

  // send it off
  enqueue_msg(targetuuid, msg);
}

void cluster_agent::do_resolve_xtion(scopira::tool::uuid id, bool dotaskresolve,
        scopira::tool::count_ptr<reply_resolve_agent_msg> &out)
{
  // lets first check the cache
  {
    event_ptr<resolve_area> L(dm_resolvearea);
    resolvemap_t::iterator ii;

    ii = L->pm_resolves.find(id);

    if (ii != L->pm_resolves.end()) {
      // found it, just return it, then
      out = ii->second.get();
      assert(out.get());
      return;
    }
  }

  count_ptr<network_msg> netm = new resolve_agent_msg(id, dotaskresolve);
  count_ptr<reply_msg> replye;

  out = 0;

  // resolve where that task is
  do_xtion(uuid(), netm.get(), replye);

  if (replye->get_code() != 0)
    return;   // not found

  out = dynamic_cast<reply_resolve_agent_msg*>(replye.get());
  assert(out.get());

  assert(out->get_code() == 0);      // perhaps replace with an if
  assert(!out->pm_agentuuid.is_zero());

  // add it to the cache for next time
  {
    event_ptr<resolve_area> L(dm_resolvearea);

    L->pm_resolves[id] = out;
  }
}

void cluster_agent::print_status(void)
{
  assert(is_alive_object());
  typedef std::vector<count_ptr<link> > links_t;
  count_ptr<link> amaster;
  links_t acopy;

  {
    event_ptr<peer_area> L(dm_peerarea);
    acopy.resize(L->pm_peers.size());
    int x = 0;
    for (peers_t::iterator ii=L->pm_peers.begin(); ii != L->pm_peers.end(); ++ii, ++x)
      acopy[x] = ii->second;
    amaster = L->pm_masterlink;
  }

  OUTPUT << "<link_list>\n";
  for (links_t::iterator ii=acopy.begin(); ii != acopy.end(); ++ii) {
    if (*ii == amaster)
      OUTPUT << " master_link: ";
    (*ii)->print_status();
  }
  OUTPUT << "</link_list>\n";
  OUTPUT << "<node_list>\n";
  {
    event_ptr<meta_area> L(dm_metarea);

    for (nodemap_t::iterator ii = L->pm_nodes.begin(); ii != L->pm_nodes.end(); ++ii)
      OUTPUT << " mspec=" << ii->second->pm_machinespec
        << " nspec=" << ii->second->pm_nodespec
        << " numload=" << event_ptr<meta_node::load_area>(ii->second->pm_loadarea)->pm_tasks.size()
        << '\n';
  }
  OUTPUT << "</node_list>\n";
}

void* cluster_agent::listen_thread_func(void *herep)
{
  cluster_agent *here = reinterpret_cast<cluster_agent*>(herep);

  assert(here->is_alive_object());

  while (true) {
    {
      event_ptr<peer_area> L(here->dm_peerarea);
      if (!L->pm_alive)
        return 0;
    }

    if (here->dm_listensocket.read_ready(1000) && !here->dm_listensocket.failed()) {
      {
        event_ptr<peer_area> L(here->dm_peerarea);
        if (!L->pm_alive)
          return 0;
      }

      count_ptr<netflow> nf = new netflow;
      count_ptr<link> lk;

      if (!here->dm_listensocket.accept(*nf)) {
        assert(false);    // should this ever happen? if so, lets investigate further
        continue;
      }

      {
        uuid tmpid;
        here->dm_uuidmaker.next(tmpid);
        lk = new link(here, tmpid, true);

        // queue it up
        event_ptr<peer_area> L(here->dm_peerarea);

        L->pm_peers[tmpid] = lk;
      }

      lk->accept_net_link(new net_link(nf.get()));
    }

  }//while true

  return 0;
}

//
// UDP stuff
//

namespace {//anonymous namespace
template <int LEN> class fixed_string
{
  public:
    char chars[LEN];

    /// sets from string, chars will be null terminated
    void set(const std::string &s) { strncpy(chars, s.c_str(), LEN); chars[LEN-1] = 0; }
    /// gets from string, no matter what chars is
    operator std::string(void) { chars[LEN-1] = 0; return std::string(chars); }
};
struct udpmsg_t {
  enum { magic_c = 0x52E3EF83 };
  uint32_t pm_magic;
  uint16_t pm_type;

  /// CRC in the future.
  union {
    /// type-1 "where are you, master"
    /// type-2 "I am the server you seek"
    struct type2_t {
      fixed_string<70> serverurl;
    } type2;
  } pm_payload;

  // CRC checking here too...
  bool is_valid(void) const { return pm_magic == magic_c; }
  // CRC checking here too...
  void package(int ty) { pm_type = ty; pm_magic = magic_c; }

  /// as bytes
  byte_t * as_bytes(void) { return reinterpret_cast<byte_t*>(this); }
  const byte_t * as_bytes(void) const { return reinterpret_cast<const byte_t*>(this); }
  size_t size(void) const { return sizeof(udpmsg_t); }
};
}//anonymous namespace

// this will bcast and try to find a server via UDP
// autourl will be changed to the actual url, on success
// true on success
static bool find_udp_server(scopira::tool::url &autourl)
{
  nethostrec hrec;
  netaddr bcastaddr;
  int bcastport;
  chrono clocky;    // monitor for time out
  udpflow socky;

  if (!hostname_to_hostrec(autourl.get_hostname(), hrec))
    return false;

  bcastaddr = hrec.get_addr();

  bcastport = autourl.get_port();
  if (bcastport == 0)
    bcastport = cluster_agent::default_port_c;

  socky.open(0);
  clocky.start();

  // send the bcast
  udpmsg_t bcastmsg;
  bcastmsg.package(1);
  socky.write(bcastaddr, bcastport, bcastmsg.as_bytes(), bcastmsg.size());

  while (clocky.get_running_time() < 1) {    // 1 second time out... let this be specified via the URL/other config mechanism?
    if (!socky.read_ready(250))
      continue;

    // we have something
    udpmsg_t msg;
    netaddr srcaddr;
    int srcport;
    if (msg.size() != socky.read(srcaddr, srcport, msg.as_bytes(), msg.size()))
      continue;

    if (!msg.is_valid() || msg.pm_type != 2)
      continue;

    // ok, we have the msg
    url goturl;
    if (goturl.set_url(msg.pm_payload.type2.serverurl)) {
      autourl = url(autourl.get_proto(), goturl.get_hostname(), goturl.get_port(), autourl.get_filename());
      return true;
    }
  }

  return false;
}

void* cluster_agent::udp_thread_func(void *herep)
{
  cluster_agent *here = reinterpret_cast<cluster_agent*>(herep);

  assert(here->is_alive_object());

  while (true) {
    if (!here->dm_udpsocket.read_ready(2000)) {
      // check if I should quit
      if (!event_ptr<peer_area>(here->dm_peerarea)->pm_alive)
        return 0;   // time to quit

      continue;
    }

    // process the incoming msg
    udpmsg_t msg;
    netaddr srcaddr;
    int srcport;
    if (msg.size() != here->dm_udpsocket.read(srcaddr, srcport, msg.as_bytes(), msg.size()))
      continue;

    if (!msg.is_valid())
      continue;
//OUTPUT << " ...received msg from " << srcaddr << ":" << srcport << " type=" << msg.pm_type << '\n';

    switch (msg.pm_type) {
      case 1: { //master query?
          udpmsg_t replymsg;

          replymsg.pm_payload.type2.serverurl.set( here->dm_nodespec.pm_url.get_url() );

//OUTPUT << "Notifying node that I am the server\n";
          replymsg.package(2);
          here->dm_udpsocket.write(srcaddr, srcport, replymsg.as_bytes(), replymsg.size());
      }
    }//switch
  }//while true

  return 0;
}

void* cluster_agent::admin_thread_func(void *herep)
{
  cluster_agent *here = reinterpret_cast<cluster_agent*>(herep);
  size_t queuesize = 0;
  bool doslow = false;

  assert(here->is_alive_object());

  while (true) {
    scopira::tool::count_ptr<admin_msg> ev;

    {
      event_ptr<admin_area> L(here->dm_adminarea);

      doslow = false;
      while (L->pm_alive && L->pm_main_queue.empty() &&
        !(doslow = (!L->pm_cron_queue.empty() && L->pm_cron_queue.top().pm_triggertime < here->dm_age.get_running_time()))) {
        L.wait(2000);
#ifdef CLUSTER_OUTPUT
        OUTPUT << '.';
#endif
      }

      if (!L->pm_alive)
        return 0;

      // grab the next msg then, and process it
      if (doslow) {
        assert(!L->pm_cron_queue.empty());
        ev = L->pm_cron_queue.top().pm_msg;
        L->pm_cron_queue.pop();

        queuesize = L->pm_cron_queue.size();
      } else {
        assert(!L->pm_main_queue.empty());
        ev = L->pm_main_queue.front();
        L->pm_main_queue.pop_front();

        queuesize = L->pm_main_queue.size();
      }
    }

    // process the msg
#ifdef CLUSTER_OUTPUT
    OUTPUT << "<\033[31m\033[1madmin_thread_func\033[0m " << (doslow?"CRONq":"mainq") << " queueleft="
      << queuesize << " running=" << ev->get_debug_name() << " @="
      << reinterpret_cast<long>(ev.get()) << ">\n";
#endif
    ev->execute_agent(*here);
#ifdef CLUSTER_OUTPUT
    OUTPUT << "</admin_thread_func>\n";
#endif
  }//while true

  return 0;
}

//
//
// msgs
//
//

//
// network_msg
//

cluster_agent::network_msg::network_msg(void)
{
}

bool cluster_agent::network_msg::load(scopira::tool::iobjflow_i& in)
{
  dm_link = 0;

  return dm_xtion.load(in);
}

void cluster_agent::network_msg::save(scopira::tool::oobjflow_i& out) const
{
  dm_xtion.save(out);
}

void cluster_agent::network_msg::execute_agent(cluster_agent &e)
{
  count_ptr<link> lk;

  if (dm_link.get()) {
    event_ptr<net_link::queue_area> L(dm_link->dm_queue);

    lk = L->pm_link;
  }

  execute_agent(e, lk.get());
}

void cluster_agent::network_msg::set_net_link(net_link *l)
{
  assert(dm_link.is_null());
  assert(l);
  assert(l->is_alive_object());
  dm_link = l;
}

//
// reply_msg
//

scopira::core::register_flow<cluster_agent::reply_msg> cluster_agent::r81123("scopira::agent::cluster_agent::reply_msg");

cluster_agent::reply_msg::reply_msg(int errorcode)
  : dm_errorcode(errorcode)
{
}

bool cluster_agent::reply_msg::load(scopira::tool::iobjflow_i& in)
{
  return network_msg::load(in) && in.read_int(dm_errorcode);
}

void cluster_agent::reply_msg::save(scopira::tool::oobjflow_i& out) const
{
  network_msg::save(out);
  out.write_int(dm_errorcode);
}

void cluster_agent::reply_msg::execute_agent(cluster_agent &e, link *lk)
{
  assert(!dm_xtion.pm_src.is_zero());

  event_ptr<xtion_area> L(e.dm_xtionarea);
  netmsgmap_t::iterator ii;
  repmsgmap_t::iterator jj;

  ii = L->pm_handlers.find(dm_xtion.pm_id);

#ifdef CLUSTER_OUTPUT
  OUTPUT << "<\033[32mreply_msg\033[0m to=" << dm_xtion.pm_id << ">\n";
#endif
  assert(!dm_xtion.pm_src.is_zero());
  assert(dm_xtion.pm_src == e.dm_nodespec.pm_uuid);

  if (ii != L->pm_handlers.end()) {
    count_ptr<network_msg> handly = ii->second;
    L->pm_handlers.erase(ii);
    // we found a handler, woo
    if (handly.get()) {
      L->pm_replies[dm_xtion.pm_id] = this;
      e.enqueue_msg(handly.get());
    }
    return;
  }

  jj = L->pm_replies.find(dm_xtion.pm_id);
  if (jj != L->pm_replies.end()) {
    // if found soeone to reply too
    jj->second = this;
    L.notify_all();
    return;
  }

#ifdef CLUSTER_OUTPUT
  OUTPUT << "Dropped reply; id=" << dm_xtion.pm_id << " name=" << get_debug_name() << '\n';
#endif
  assert(false);
}


//
// make_link_msg
//

void cluster_agent::make_link_msg::enqueue_new_master_link_msg(cluster_agent &e,
    const scopira::tool::url &masterurl)
{
  uuid tmpid;

  e.dm_uuidmaker.next(tmpid);

  {
    event_ptr<cluster_agent::peer_area> L(e.dm_peerarea);
    L->pm_peers[tmpid] = new link(&e, tmpid, true, masterurl);
  }

  e.enqueue_msg(new make_link_msg(tmpid, masterurl, true));
}

void cluster_agent::make_link_msg::enqueue_new_reconnect_link_msg(cluster_agent &e,
    const scopira::tool::url &rurl, scopira::tool::uuid id)
{
  e.enqueue_msg(new make_link_msg(id, rurl, false));
}

void cluster_agent::make_link_msg::enqueue_new_peer_link_msg(cluster_agent &e,
    scopira::tool::uuid id, network_msg *msg)
{
  count_ptr<reply_resolve_agent_msg> rep;
  count_ptr<link> lk;
  bool alreadyactive = false;
  bool iinitiate;

  assert(!id.is_zero());

  iinitiate = e.dm_nodespec.pm_uuid < id;

  // resolve the remotes URL
  e.do_resolve_xtion(id, false, rep);

  if (rep.is_null())
    return;

  // if im tunneled OR
  // if target is tunneled and im not master, then route it through master
  if (!e.dm_nodespec.pm_isdirectroute ||
      (!rep->pm_isdirectroute && !e.dm_nodespec.pm_ismaster)) {
    count_ptr<routed_msg> r;

    assert(msg);
    r = new routed_msg(id, msg);
    // send the routed msg to the master
    e.enqueue_msg(uuid(), r.get());
    return;
  }

  // add the link, if needed
  {
    event_ptr<cluster_agent::peer_area> L(e.dm_peerarea);
    peers_t::iterator ii;

    ii = L->pm_peers.find(id);
    if (ii == L->pm_peers.end()) {
      if (iinitiate)
        lk = new link(&e, id, false, rep->pm_agenturl);
      else
        lk = new link(&e, id, false);
      L->pm_peers[id] = lk;
    } else {
      lk = ii->second;
      alreadyactive = true;
    }
  }

  // enqueue the msg to the socket (weither its connected or not)
  if (msg)
    lk->enqueue_msg(msg);

  // if another thread is already doing this, then we might aswell bail right now
  if (alreadyactive)
    return;

  if (e.dm_nodespec.pm_uuid < id)
    e.enqueue_msg(new make_link_msg(rep->pm_agentuuid, rep->pm_agenturl, false));
  else
    e.enqueue_msg(uuid(), new make_back_link_msg(e.dm_nodespec.pm_uuid, id));
}

cluster_agent::make_link_msg::make_link_msg(scopira::tool::uuid u,
    const scopira::tool::url &l, bool makingmaster)
  : dm_targetuuid(u), dm_targeturl(l), dm_makingmaster(makingmaster)
{
}

void cluster_agent::make_link_msg::execute_agent(cluster_agent &e)
{
  // first, lets see if the link already has a link
  // (this prevents double queueing)
  count_ptr<link> lk;

  {
    event_ptr<cluster_agent::peer_area> L(e.dm_peerarea);

    cluster_agent::peers_t::iterator ii = L->pm_peers.find(dm_targetuuid);
    assert(ii != L->pm_peers.end());

    lk = ii->second;
  }

  if (lk->has_link())
    return;

  count_ptr<net_link> nf;

  // do proper error managment here
  if (!net_link::make_link(dm_targeturl, nf)) {
    if (dm_makingmaster) {
      // i dont retry master links, but i must set the failed state

      event_ptr<admin_area> L(e.dm_adminarea);

      L->pm_failedstate = failed_cantopen_c;
      L.notify_all();
    } else
      e.enqueue_cron_msg(5, this);  // this is questionable; keep trying peer connections, because maybe a peer's listener is just busy?
    return;
  }

  lk->accept_net_link(nf.get());
}

//
// quit_msg
//

cluster_agent::quit_msg::quit_msg(bool agenterror)
  : dm_counter(2), dm_agenterror(agenterror)
{
}

void cluster_agent::quit_msg::execute_agent(cluster_agent &e)
{
  --dm_counter;

  if (dm_counter == 0 || dm_agenterror) {
    if (dm_agenterror) {
      // agent error, set failed and signal handler (if any)
      agenterror_reactor_i *rec;

      {
        event_ptr<admin_area> L(e.dm_adminarea);

        L->pm_failedstate = failed_lostmaster_c;
        rec = L->pm_agenterror;
      }
      e.dm_adminarea.pm_condition.notify_all();

      if (rec)
        rec->react_agenterror(&e);
    }

    e.notify_stop();
  } else
    e.enqueue_cron_msg(2, this);
}

//
// remove_link_msg
//

cluster_agent::remove_link_msg::remove_link_msg(scopira::tool::uuid rmuuid)
  : dm_rmuuid(rmuuid)
{
}

void cluster_agent::remove_link_msg::execute_agent(cluster_agent &e)
{
#ifdef CLUSTER_OUTPUT
  e.print_status();
#endif

  // first, if i'm a master, nuke this link (and its associated tasks, if any)
  if (e.dm_nodespec.pm_ismaster) {
    event_ptr<meta_area> L(e.dm_metarea);

    nodemap_t::iterator ii = L->pm_nodes.find(dm_rmuuid);
    if (ii != L->pm_nodes.end()) {
      meta_node::taskslist_t killtasks;

      killtasks = event_ptr<meta_node::load_area>(ii->second->pm_loadarea)->pm_tasks;

      L->pm_nodes.erase(ii);

      // nuke the tasks
      for (int x=0; x<killtasks.size(); ++x) {
        taskmap_t::iterator jj = L->pm_tasks.find(killtasks[x]);
        if (jj != L->pm_tasks.end())
          L->pm_tasks.erase(jj);
      }
    }
  }//if #1

  // second, flush my dns cache to to be sure (perhaps draconian, whatever)
  {
    event_ptr<resolve_area> L(e.dm_resolvearea);

    L->pm_resolves.clear();
  }

  // third, and finally, nuke the link from our map
  {
    event_ptr<peer_area> L(e.dm_peerarea);
    peers_t::iterator ii = L->pm_peers.find(dm_rmuuid);

    if (ii != L->pm_peers.end())
      L->pm_peers.erase(ii);
  }

#ifdef CLUSTER_OUTPUT
  e.print_status();
#endif
}

//
// peer_hello_msg
//

scopira::core::register_flow<cluster_agent::peer_hello_msg> cluster_agent::r09344("scopira::agent::cluster_agent::peer_hello_msg");

cluster_agent::peer_hello_msg::peer_hello_msg(void)
{
}

cluster_agent::peer_hello_msg::peer_hello_msg(const machine_spec &mspec, const node_spec &nspec, int senderlistenport, bool senderistmp)
  : dm_sendermspec(mspec), dm_sendernspec(nspec), dm_senderlistenport(senderlistenport), dm_senderistmp(senderistmp)
{
}

bool cluster_agent::peer_hello_msg::load(scopira::tool::iobjflow_i& in)
{
  return parent_type::load(in)
    && dm_sendermspec.load(in)
    && dm_sendernspec.load(in)
    && in.read_int(dm_senderlistenport)
    && in.read_bool(dm_senderistmp);
}

void cluster_agent::peer_hello_msg::save(scopira::tool::oobjflow_i& out) const
{
  parent_type::save(out);
  dm_sendermspec.save(out);
  dm_sendernspec.save(out);
  out.write_int(dm_senderlistenport);
  out.write_bool(dm_senderistmp);
}

void cluster_agent::peer_hello_msg::execute_agent(cluster_agent &e, cluster_agent::link *lk)
{
  count_ptr<net_link> netlk;
  count_ptr<link> srclk, destlk;
  bool mademaster = false;

  assert(!dm_sendernspec.pm_uuid.is_zero());
  assert(is_alive_object());
  assert(lk->is_alive_object());

#ifdef CLUSTER_OUTPUT
  OUTPUT << "PEER-HELLO nspec=" << dm_sendernspec << '\n';
#endif

  if (dm_sendernspec.pm_uuid == lk->dm_peeruuid) {
    //e.print_status();
    return;
  }

#ifdef CLUSTER_OUTPUT
  OUTPUT << "PEER-HELLO OPERATING\n";
#endif

  srclk = lk;

  // lock the source, and get its netlink
  {
    event_ptr<link::queue_area> L(lk->dm_queue);

    L->pm_remotetmp = dm_senderistmp;
    netlk = L->pm_link;
  }

  // find the target
  {
    event_ptr<peer_area> L(e.dm_peerarea);
    peers_t::iterator ii = L->pm_peers.find(dm_sendernspec.pm_uuid);

    if (ii == L->pm_peers.end()) {
      // not found, need to make a new one
      destlk = new link(&e, dm_sendernspec.pm_uuid, false, lk->dm_peerurl);
      L->pm_peers[dm_sendernspec.pm_uuid] = destlk;
    } else
      destlk = ii->second;

    // mightaswell remove the source from the peer map here (but only if it was a temp)
    // if this is a reconnect-rename operation, leave the old one there "just in case"
    if (srclk->dm_tmpaddress) {
      ii = L->pm_peers.find(srclk->dm_peeruuid);
      assert(ii != L->pm_peers.end());    // might have already been erased if we had a double send, etc
      L->pm_peers.erase(ii);
    }

    // check if we have a new master
    if (dm_sendernspec.pm_ismaster && !L->pm_masterlink.get()) {
      L->pm_masterlink = destlk;
      mademaster = true;
      L.notify_all();   // for enqueue_msg
    }
  }

  // finally, do the trade
  assert(srclk.get() != destlk.get());
  srclk->accept_net_link(0);
  // before we deliver the net_link to the new link,
  // move over the "remote tmp bit" and "current object queue"
  {
    link::msgqueue_t outqueue;
    bool remotetmp;

    // flush the queus from the old, tmp link
    {
      event_ptr<link::queue_area> L(srclk->dm_queue);

      remotetmp = L->pm_remotetmp;

      outqueue.resize(L->pm_sendq.size());
      std::copy(L->pm_sendq.begin(), L->pm_sendq.end(), outqueue.begin());
      L->pm_sendq.clear();
    }

    {
      event_ptr<link::queue_area> L(destlk->dm_queue);

      L->pm_remotetmp = remotetmp;

#ifdef CLUSTER_OUTPUT
      OUTPUT << "PEER-HELLO MOVING from-out=" << outqueue.size() << " to-out=" << L->pm_sendq.size() << '\n';
#endif

      for (link::msgqueue_t::iterator ii=outqueue.begin(); ii != outqueue.end(); ++ii)
        L->pm_sendq.push_back(*ii);
    }
  }
  // finally, do the move of the link
  destlk->accept_net_link(netlk.get());
  // send out an even through this new link notifyin out peer that we're not tmp now
  destlk->enqueue_msg(new nottmp_hello_msg);

  // add the new spec (as a meta_node) to the agentmaster, if needed
  if (e.dm_nodespec.pm_ismaster) {
    event_ptr<meta_area> L(e.dm_metarea);
    nodemap_t::iterator ii = L->pm_nodes.find(dm_sendernspec.pm_uuid);

    if (ii == L->pm_nodes.end()) {
      url senderurl("scopira://" + netlk->get_sock()->get_addr().as_string() + ":" + int_to_string(dm_senderlistenport) + "/");
      L->pm_nodes[dm_sendernspec.pm_uuid] = new meta_node(dm_sendermspec, dm_sendernspec);
    }
  }

  // if I'm a client, update the status of my new have-master
  if (mademaster) {
    {
      event_ptr<admin_area> L(e.dm_adminarea);

      L->pm_failedstate = failed_ok_c;
      L.notify_all();
    }
    {
      event_ptr<link::queue_area> L(destlk->dm_queue);

      L->pm_remotemaster = true;
    }
  }

#ifdef CLUSTER_OUTPUT
  e.print_status();
#endif
}

//
// nottmp_hello_msg
//

scopira::core::register_flow<cluster_agent::nottmp_hello_msg> cluster_agent::r55123("scopira::agent::cluster_agent::nottmp_hello_msg");

cluster_agent::nottmp_hello_msg::nottmp_hello_msg(void)
{
}

void cluster_agent::nottmp_hello_msg::execute_agent(cluster_agent &e, cluster_agent::link *lk)
{
  assert(lk);
  assert(lk->is_alive_object());

  {
    event_ptr<link::queue_area> L(lk->dm_queue);

    L->pm_remotetmp = false;

    // release any sending
    lk->unleash_a_msg(L, 0);
  }
}

//
// make_back_link_msg
//

scopira::core::register_flow<cluster_agent::make_back_link_msg> cluster_agent::r54788("scopira::agent::cluster_agent::make_back_link_msg");

cluster_agent::make_back_link_msg::make_back_link_msg(void)
{
}

cluster_agent::make_back_link_msg::make_back_link_msg(scopira::tool::uuid makelinkto,
    scopira::tool::uuid makelinkfrom)
  : dm_makelinkto(makelinkto), dm_makelinkfrom(makelinkfrom)
{
}

bool cluster_agent::make_back_link_msg::load(scopira::tool::iobjflow_i& in)
{
  return network_msg::load(in) && dm_makelinkto.load(in) && dm_makelinkfrom.load(in);
}

void cluster_agent::make_back_link_msg::save(scopira::tool::oobjflow_i& out) const
{
  network_msg::save(out);
  dm_makelinkto.save(out);
  dm_makelinkfrom.save(out);
}

void cluster_agent::make_back_link_msg::execute_agent(cluster_agent &e, cluster_agent::link *lk)
{
  if (e.dm_nodespec.pm_uuid == dm_makelinkfrom)
    // I'm at the destination, lets make a link here
    // a little bit of recution here :)
    make_link_msg::enqueue_new_peer_link_msg(e, dm_makelinkto, 0);
  else
    // I'm not there, therefore I must be at the master... lets move on
    e.enqueue_msg(dm_makelinkfrom, this);
}

//
// all_quit_msg
//

scopira::core::register_flow<cluster_agent::all_quit_msg> cluster_agent::r72218("scopira::agent::cluster_agent::all_quit_msg");

cluster_agent::all_quit_msg::all_quit_msg(void)
  : dm_initter(false)
{
}

cluster_agent::all_quit_msg::all_quit_msg(bool initter)
  : dm_initter(initter)
{
}

void cluster_agent::all_quit_msg::execute_agent(cluster_agent &e, cluster_agent::link *lk)
{
  if (e.dm_nodespec.pm_ismaster) {
    // we're the server, so distribute to everyone else
    event_ptr<peer_area> L(e.dm_peerarea);

    for (peers_t::iterator ii = L->pm_peers.begin(); ii != L->pm_peers.end(); ++ii)
      ii->second->enqueue_msg(this);
  } else if (dm_initter) {
    // non-servers (ie. clients) only distribyte to the server
    // if they are the ones that triggered the quite (from the user via enqueue_network_quit)
    bool ok = false;

    {
      event_ptr<peer_area> L(e.dm_peerarea);

      if (L->pm_masterlink.get()) {
        L->pm_masterlink->enqueue_msg(this);
        ok = true;
      }
    }

    if (!ok) {
      e.enqueue_cron_msg(3, this);
      return;
    }
  }
  // finally, queue the local quit event
  e.enqueue_msg(new quit_msg(false));
}

//
// cluster_agent::resolve_agent_msg
//

scopira::core::register_flow<cluster_agent::resolve_agent_msg> cluster_agent::r11928("scopira::agent::cluster_agent::resolve_agent_msg");

cluster_agent::resolve_agent_msg::resolve_agent_msg(void)
  : pm_dotask(false)
{
}

cluster_agent::resolve_agent_msg::resolve_agent_msg(scopira::tool::uuid id, bool dotaskresolve)
  : pm_idtofind(id), pm_dotask(dotaskresolve)
{
}

bool cluster_agent::resolve_agent_msg::load(scopira::tool::iobjflow_i& in)
{
  return network_msg::load(in) && pm_idtofind.load(in) && in.read_bool(pm_dotask);
}

void cluster_agent::resolve_agent_msg::save(scopira::tool::oobjflow_i& out) const
{
  network_msg::save(out);
  pm_idtofind.save(out);
  out.write_bool(pm_dotask);
}

void cluster_agent::resolve_agent_msg::execute_agent(cluster_agent &e, cluster_agent::link *lk)
{
  uuid repid;
  url repurl;
  bool isdirectroute = false;
  int ok = 1;

  assert(!pm_idtofind.is_zero());

  {
    event_ptr<meta_area> L(e.dm_metarea);

    // do we need to resolve a taskid?
    if (pm_dotask) {
      taskmap_t::iterator jj = L->pm_tasks.find(pm_idtofind);

      if (jj == L->pm_tasks.end()) {
        // cant find it, error
        // dont do anything, leave repid as zero
      } else
        repid = jj->second; // all good
    } else
      repid = pm_idtofind;

    // if we have something to hunt, hunt it
    if (!repid.is_zero()) {
      nodemap_t::iterator ii = L->pm_nodes.find(repid);

      if (ii != L->pm_nodes.end()) {
        repurl = ii->second->pm_nodespec.pm_url;
        isdirectroute = ii->second->pm_nodespec.pm_isdirectroute;
        ok = 0;   // remember, 0 errorcode == success
      }
    }
  }

  count_ptr<reply_resolve_agent_msg> rep = new reply_resolve_agent_msg(ok, repid, repurl, isdirectroute);
  e.enqueue_reply_msg(this, rep.get());
}

//
// reply_resolve_agent_msg
//

scopira::core::register_flow<cluster_agent::reply_resolve_agent_msg> cluster_agent::r62383("scopira::agent::cluster_agent::reply_resolve_agent_msg");

cluster_agent::reply_resolve_agent_msg::reply_resolve_agent_msg(void)
{
}

cluster_agent::reply_resolve_agent_msg::reply_resolve_agent_msg(int errorcode,
    scopira::tool::uuid agentuuid, const scopira::tool::url &repurl,
    bool isdirectroute)
  : reply_msg(errorcode), pm_agentuuid(agentuuid), pm_agenturl(repurl),
    pm_isdirectroute(isdirectroute)
{
}

bool cluster_agent::reply_resolve_agent_msg::load(scopira::tool::iobjflow_i& in)
{
  return reply_msg::load(in) && pm_agentuuid.load(in) &&
    pm_agenturl.load(in) && in.read_bool(pm_isdirectroute);
}

void cluster_agent::reply_resolve_agent_msg::save(scopira::tool::oobjflow_i& out) const
{
  reply_msg::save(out);
  pm_agentuuid.save(out);
  pm_agenturl.save(out);
  out.write_bool(pm_isdirectroute);
}

//
// routed_msg
//

scopira::core::register_flow<cluster_agent::routed_msg> cluster_agent::r49045("scopira::agent::cluster_agent::routed_msg");

cluster_agent::routed_msg::routed_msg(void)
{
}

cluster_agent::routed_msg::routed_msg(scopira::tool::uuid finalagent, cluster_agent::network_msg *finalmsg)
  : dm_finalagent(finalagent), dm_finalmsg(finalmsg)
{
  assert(finalmsg);
}

bool cluster_agent::routed_msg::load(scopira::tool::iobjflow_i& in)
{
  return network_msg::load(in) && dm_finalagent.load(in) && in.read_object_type(dm_finalmsg);
}

void cluster_agent::routed_msg::save(scopira::tool::oobjflow_i& out) const
{
  network_msg::save(out);
  dm_finalagent.save(out);
  out.write_object_type(dm_finalmsg);
}

void cluster_agent::routed_msg::execute_agent(cluster_agent &e, cluster_agent::link *lk)
{
  assert(e.dm_nodespec.pm_ismaster);

  assert(!dm_finalagent.is_zero());
  assert(dm_finalmsg.get());
  e.enqueue_msg(dm_finalagent, dm_finalmsg.get());
}

//
// master_launch_task_msg
//

scopira::core::register_flow<cluster_agent::master_launch_task_msg> cluster_agent::r88312("scopira::agent::cluster_agent::master_launch_task_msg");

cluster_agent::master_launch_task_msg::master_launch_task_msg(void)
  : dm_numps(0)
{
}

cluster_agent::master_launch_task_msg::master_launch_task_msg(scopira::tool::uuid srcagent,
    const std::string &typestring, int numps, const uuid &existingmaster)
  : dm_srcagent(srcagent), dm_typestring(typestring), dm_numps(numps), dm_existingmaster(existingmaster)
{
}

bool cluster_agent::master_launch_task_msg::load(scopira::tool::iobjflow_i& in)
{
  return network_msg::load(in) && dm_srcagent.load(in) && in.read_string(dm_typestring)
    && in.read_int(dm_numps) && dm_existingmaster.load(in);
}

void cluster_agent::master_launch_task_msg::save(scopira::tool::oobjflow_i& out) const
{
  network_msg::save(out);
  dm_srcagent.save(out);
  out.write_string(dm_typestring);
  out.write_int(dm_numps);
  dm_existingmaster.save(out);
}

void cluster_agent::master_launch_task_msg::execute_agent(cluster_agent &e, cluster_agent::link *lk)
{
  narray<uuid> ids, allocagent;
  int x, numps;
  bool hasmaster = !dm_existingmaster.is_zero();

  if (dm_numps <= 0) {
    // auto calc numps
    event_ptr<meta_area> L(e.dm_metarea);
    numps = -dm_numps;

    for (nodemap_t::iterator ii=L->pm_nodes.begin(); ii != L->pm_nodes.end(); ++ii) {
      event_ptr<meta_node::load_area> LA(ii->second->pm_loadarea);
      int freecpu = static_cast<int>(ii->second->pm_machinespec.pm_numcpu) - LA->pm_tasks.size();

      if (freecpu > 0)
        numps += freecpu;
    }
  } else
    numps = dm_numps;

  assert(numps>0);

  ids.resize(numps);
  allocagent.resize(numps);

  if (hasmaster)
    ids[0] = dm_existingmaster;

  // create the ids
  for (x= (hasmaster?1:0); x<ids.size(); ++x)
    e.la_get_generator().next(ids[x]);

  {
    event_ptr<meta_area> L(e.dm_metarea);

    // find the next agent to allocate
    for (x= (hasmaster?1:0); x<numps; ++x) {
      // find the most idle of nodes
      int bestnumbfree = -9999;
      nodemap_t::iterator bestsofar = L->pm_nodes.end(), srcnode = L->pm_nodes.end(), ii;

      // scan all the agents
      for (ii=L->pm_nodes.begin(); ii != L->pm_nodes.end(); ++ii) {
        event_ptr<meta_node::load_area> LA(ii->second->pm_loadarea);
        int freecpu = static_cast<int>(ii->second->pm_machinespec.pm_numcpu) - LA->pm_tasks.size();

        // remember the self, incase we need it later
        if (ii->second->pm_nodespec.pm_uuid != dm_srcagent)
          srcnode = ii;
        // skip this node if he doesnt want ANY jobs
        if (ii->second->pm_nodespec.pm_jobfilter == node_spec::no_jobs_c)
          continue;
        // skip this node if he only wants jobs from himself
        if (ii->second->pm_nodespec.pm_jobfilter == node_spec::my_jobs_c &&
            ii->second->pm_nodespec.pm_uuid != dm_srcagent)
          continue;
        // is it better than the one we have (and are we allowed to allocate jobs to it?)
        if (freecpu > bestnumbfree) {
          bestsofar = ii;
          bestnumbfree = static_cast<long>(ii->second->pm_machinespec.pm_numcpu) - LA->pm_tasks.size();
        }
      }

      assert(!L->pm_nodes.empty());
      // we should always be able to alloce a job to someone
      if (bestsofar == L->pm_nodes.end())
        bestsofar = srcnode;    // if noone will take this job, then it HAS to go to the job request. a must.
      assert(bestsofar != L->pm_nodes.end());
      // ok, allocate it to the least loaded one
      allocagent[x] = bestsofar->first;
      assert(x<ids.size());
      // update the dm_tasks table
      L->pm_tasks[ids[x]] = bestsofar->first;

      event_ptr<meta_node::load_area> LA(bestsofar->second->pm_loadarea);
      LA->pm_tasks.push_back(ids[x]);
    }
  }//event_ptr

#ifdef CLUSTER_OUTPUT
  for (x= (hasmaster?1:0); x<numps; ++x)
    OUTPUT << "Allocating task index=" << x << " to node=" << allocagent[x] << " type=" << dm_typestring << '\n';
#endif

  // lets send all those sub agent creation requests (phase 1)
  for (x= (hasmaster?1:0); x<numps; ++x) {
    assert(!dm_typestring.empty());
    count_ptr<make_task_msg> m = new make_task_msg(x, ids, dm_typestring, 1);
    count_ptr<reply_msg> replye;

    e.do_xtion(allocagent[x], m.get(), replye);   // send it

    assert(replye->get_code() == 0);
  }
  // lets send all those sub agent creation requests (phase 1)
  for (x= (hasmaster?1:0); x<numps; ++x) {
    assert(!dm_typestring.empty());
    count_ptr<make_task_msg> m = new make_task_msg(x, ids, dm_typestring, 2);
    count_ptr<reply_msg> replye;

    e.do_xtion(allocagent[x], m.get(), replye);   // send it

    assert(replye->get_code() == 0);
  }

  // finally, send the reply to the caller
  count_ptr<reply_master_launch_task_msg> rep = new reply_master_launch_task_msg(ids);
  e.enqueue_reply_msg(this, rep.get());
}

//
// reply_master_launch_task_msg
//

scopira::core::register_flow<cluster_agent::reply_master_launch_task_msg> cluster_agent::r00922("scopira::agent::cluster_agent::reply_master_launch_task_msg");

cluster_agent::reply_master_launch_task_msg::reply_master_launch_task_msg(void)
{
}

cluster_agent::reply_master_launch_task_msg::reply_master_launch_task_msg(const narray<uuid> &ids)
  : pm_taskids(ids)
{
}

bool cluster_agent::reply_master_launch_task_msg::load(scopira::tool::iobjflow_i& in)
{
  size_t sz;
  if (!reply_msg::load(in) || !in.read_size_t(sz))
    return false;
  pm_taskids.resize(sz);
  for (size_t x=0; x<sz; ++x)
    if (!pm_taskids[x].load(in))
      return false;
  return true;
}

void cluster_agent::reply_master_launch_task_msg::save(scopira::tool::oobjflow_i& out) const
{
  reply_msg::save(out);
  out.write_size_t(pm_taskids.size());
  for (size_t x=0; x<pm_taskids.size(); ++x)
    pm_taskids[x].save(out);
}

//
// make_task_msg
//

scopira::core::register_flow<cluster_agent::make_task_msg> cluster_agent::r79933("scopira::agent::cluster_agent::make_task_msg");

cluster_agent::make_task_msg::make_task_msg(void)
  : dm_myindex(-1), dm_phase(-1)
{
}

cluster_agent::make_task_msg::make_task_msg(int myindex, const narray<uuid> &ids, const std::string &typestring, short phase)
  : dm_myindex(myindex), dm_taskids(ids), dm_typestring(typestring), dm_phase(phase)
{
  assert(!dm_typestring.empty());
}

bool cluster_agent::make_task_msg::load(scopira::tool::iobjflow_i& in)
{
  size_t sz;
  if (! (network_msg::load(in) && in.read_int(dm_myindex) && in.read_short(dm_phase) && in.read_size_t(sz)) )
    return false;

  assert(sz < 100000);    // sanity check, maybe remove in the future
  dm_taskids.resize(sz);
  for (size_t x=0; x<sz; ++x)
    if (!dm_taskids[x].load(in))
      return false;
    
  //return in.read_string(dm_typestring);
  bool r = in.read_string(dm_typestring);
  assert(r && !dm_typestring.empty());
  return r;
}

void cluster_agent::make_task_msg::save(scopira::tool::oobjflow_i& out) const
{
  network_msg::save(out);
  out.write_int(dm_myindex);
  out.write_short(dm_phase);
  out.write_size_t(dm_taskids.size());
  for (size_t x=0; x<dm_taskids.size(); ++x)
    dm_taskids[x].save(out);
  out.write_string(dm_typestring);
}

void cluster_agent::make_task_msg::execute_agent(cluster_agent &e, cluster_agent::link *lk)
{
  bool ok;

  assert(!dm_typestring.empty());
  // I dont need lk

  assert(dm_phase == 1 || dm_phase == 2);
  ok = e.la_launch_task(dm_myindex, dm_taskids, dm_typestring, dm_phase);

  // send the reply
  e.enqueue_reply_msg(this, new reply_msg(ok?0:1));
}

//
// client_wait_msg
//

scopira::core::register_flow<cluster_agent::client_wait_msg> cluster_agent::r28334("scopira::agent::cluster_agent::client_wait_msg");

cluster_agent::client_wait_msg::client_wait_msg(void)
{
}

cluster_agent::client_wait_msg::client_wait_msg(scopira::tool::uuid taskid, int msec)
  : dm_taskid(taskid), dm_msec(msec)
{
}

bool cluster_agent::client_wait_msg::load(scopira::tool::iobjflow_i& in)
{
  return network_msg::load(in) && dm_taskid.load(in) && in.read_int(dm_msec);
}

void cluster_agent::client_wait_msg::save(scopira::tool::oobjflow_i& out) const
{
  network_msg::save(out);
  dm_taskid.save(out);
  out.write_int(dm_msec);
}

void cluster_agent::client_wait_msg::execute_agent(cluster_agent &e, cluster_agent::link *lk)
{
  assert(!dm_taskid.is_zero());
  e.la_launch_proxy(new client_wait_task(&e, get_xtion(), dm_taskid, dm_msec));
}

//
// client_wait_task
//

cluster_agent::client_wait_task::client_wait_task(cluster_agent *ag, const network_msg::xtion_t &x,
    uuid &taskid, int msec)
  : dm_agent(ag), dm_xtion(x), dm_taskid(taskid), dm_msec(msec)
{
  assert(ag->is_alive_object());
}

int cluster_agent::client_wait_task::run(scopira::agent::task_context &e)
{
  bool reply;

  assert(!dm_taskid.is_zero());

  // wait for the task
  reply = dm_agent->local_agent::wait_task(dm_taskid, dm_msec);

  // return the reply
  dm_agent->enqueue_reply_msg(dm_xtion, new reply_msg(reply?0:1));

  return run_done_c;
}

//
// client_kill_msg
//

scopira::core::register_flow<cluster_agent::client_kill_msg> cluster_agent::r45666("scopira::agent::cluster_agent::client_kill_msg");

cluster_agent::client_kill_msg::client_kill_msg(void)
{
}

cluster_agent::client_kill_msg::client_kill_msg(scopira::tool::uuid taskid)
  : dm_taskid(taskid)
{
}

bool cluster_agent::client_kill_msg::load(scopira::tool::iobjflow_i& in)
{
  return network_msg::load(in) && dm_taskid.load(in);
}

void cluster_agent::client_kill_msg::save(scopira::tool::oobjflow_i& out) const
{
  network_msg::save(out);
  dm_taskid.save(out);
}

void cluster_agent::client_kill_msg::execute_agent(cluster_agent &e, cluster_agent::link *lk)
{
  assert(!dm_taskid.is_zero());

  e.local_agent::kill_task(dm_taskid);

  e.enqueue_reply_msg(this, new reply_msg(0));
}

//
// client_is_alive_msg
//

scopira::core::register_flow<cluster_agent::client_is_alive_msg> cluster_agent::r44512("scopira::agent::cluster_agent::client_is_alive_msg");

cluster_agent::client_is_alive_msg::client_is_alive_msg(void)
{
}

cluster_agent::client_is_alive_msg::client_is_alive_msg(scopira::tool::uuid taskid)
  : dm_taskid(taskid)
{
}

bool cluster_agent::client_is_alive_msg::load(scopira::tool::iobjflow_i& in)
{
  return network_msg::load(in) && dm_taskid.load(in);
}

void cluster_agent::client_is_alive_msg::save(scopira::tool::oobjflow_i& out) const
{
  network_msg::save(out);
  dm_taskid.save(out);
}

void cluster_agent::client_is_alive_msg::execute_agent(cluster_agent &e, cluster_agent::link *lk)
{
  assert(!dm_taskid.is_zero());

  e.enqueue_reply_msg(this, new reply_msg(e.local_agent::is_alive_task(dm_taskid)?0:1));
}

//
// send_data_msg
//

scopira::core::register_flow<cluster_agent::send_data_msg> cluster_agent::r49075("scopira::agent::cluster_agent::send_data_msg");

cluster_agent::send_data_msg::send_data_msg(void)
  : dm_buf(0)
{
}

cluster_agent::send_data_msg::send_data_msg(scopira::tool::uuid src, scopira::tool::uuid dest, scopira::tool::bufferflow *buf)
  : dm_src(src), dm_dest(dest), dm_buf(buf)
{
}

bool cluster_agent::send_data_msg::load(scopira::tool::iobjflow_i& in)
{
  size_t sz;
  bool b;

  b = network_msg::load(in) && dm_src.load(in) && dm_dest.load(in) && in.read_size_t(sz);

  if (!b)
    return false;

  dm_buf = new bufferflow;
  dm_buf->reset_resize(sz);

  return in.read(dm_buf->c_array(), sz) == sz;
}

void cluster_agent::send_data_msg::save(scopira::tool::oobjflow_i& out) const
{
  network_msg::save(out);
  dm_src.save(out);
  dm_dest.save(out);

  assert(dm_buf.get());
  out.write_size_t(dm_buf->size());
  out.write(dm_buf->c_array(), dm_buf->size());
}

void cluster_agent::send_data_msg::execute_agent(cluster_agent &e, cluster_agent::link *lk)
{
  // potensially recursive, but should never be so in practice
  e.send_msg(dm_src, dm_dest, dm_buf.get());
}

//
// bcast_data_msg
//

scopira::core::register_flow<cluster_agent::bcast_data_msg> cluster_agent::r49076("scopira::agent::cluster_agent::bcast_data_msg");

cluster_agent::bcast_data_msg::bcast_data_msg(void)
  : dm_buf(0)
{
}

cluster_agent::bcast_data_msg::bcast_data_msg(scopira::tool::uuid src, scopira::tool::uuid destserviceid, scopira::tool::bufferflow *buf)
  : dm_src(src), dm_destservice(destserviceid), dm_buf(buf)
{
}

bool cluster_agent::bcast_data_msg::load(scopira::tool::iobjflow_i& in)
{
  size_t sz;
  bool b;

  b = network_msg::load(in) && dm_src.load(in) && dm_destservice.load(in) && in.read_size_t(sz);

  if (!b)
    return false;

  dm_buf = new bufferflow;
  dm_buf->reset_resize(sz);

  return in.read(dm_buf->c_array(), sz) == sz;
}

void cluster_agent::bcast_data_msg::save(scopira::tool::oobjflow_i& out) const
{
  network_msg::save(out);
  dm_src.save(out);
  dm_destservice.save(out);

  assert(dm_buf.get());
  out.write_size_t(dm_buf->size());
  out.write(dm_buf->c_array(), dm_buf->size());
}

void cluster_agent::bcast_data_msg::execute_agent(cluster_agent &e, cluster_agent::link *lk)
{
  // ok, send a bcast_data_2_msg to ALL the nodes (if I'm the master
  if (e.dm_nodespec.pm_ismaster) {
    // we're the server, so distribute to everyone else
    event_ptr<peer_area> L(e.dm_peerarea);

    for (peers_t::iterator ii = L->pm_peers.begin(); ii != L->pm_peers.end(); ++ii)
      ii->second->enqueue_msg(this);
  }
  
  // aswell as process the bcast even for the local processes
  // (but make sure to call the local_agent version, which does the actually deployment
  e.local_agent::send_msg_bcast(dm_src, dm_destservice, dm_buf.get());
}

//
// reg_context_msg
//

scopira::core::register_flow<cluster_agent::reg_context_msg> cluster_agent::r89063("scopira::agent::cluster_agent::reg_context_msg");

cluster_agent::reg_context_msg::reg_context_msg(void)
  : dm_regit(false)
{
}

cluster_agent::reg_context_msg::reg_context_msg(scopira::tool::uuid ctxid, bool regit,
        scopira::tool::uuid agentid)
  : dm_ctxid(ctxid), dm_regit(regit), dm_agentid(agentid)
{
}

bool cluster_agent::reg_context_msg::load(scopira::tool::iobjflow_i& in)
{
  return network_msg::load(in) && dm_ctxid.load(in) && in.read_bool(dm_regit)
    && dm_agentid.load(in);
}

void cluster_agent::reg_context_msg::save(scopira::tool::oobjflow_i& out) const
{
  network_msg::save(out);
  dm_ctxid.save(out);
  out.write_bool(dm_regit);
  dm_agentid.save(out);
}

void cluster_agent::reg_context_msg::execute_agent(cluster_agent &e, cluster_agent::link *lk)
{
  assert(!dm_ctxid.is_zero());
  assert(!dm_agentid.is_zero());

  {
    event_ptr<meta_area> L(e.dm_metarea);

    if (dm_regit)
      L->pm_tasks[dm_ctxid] = dm_agentid;
    else {
      taskmap_t::iterator ii = L->pm_tasks.find(dm_ctxid);

      if (ii != L->pm_tasks.end())
        L->pm_tasks.erase(ii);
    }
  }

  e.enqueue_reply_msg(this, new reply_msg(0));
}

//
// dead_task_msg
//

scopira::core::register_flow<cluster_agent::dead_task_msg> cluster_agent::r85221("scopira::agent::cluster_agent::dead_task_msg");

cluster_agent::dead_task_msg::dead_task_msg(void)
{
}

cluster_agent::dead_task_msg::dead_task_msg(scopira::tool::uuid taskid)
  : dm_taskid(taskid)
{
}

bool cluster_agent::dead_task_msg::load(scopira::tool::iobjflow_i& in)
{
  return network_msg::load(in) && dm_taskid.load(in);
}

void cluster_agent::dead_task_msg::save(scopira::tool::oobjflow_i& out) const
{
  network_msg::save(out);
  dm_taskid.save(out);
}

void cluster_agent::dead_task_msg::execute_agent(cluster_agent &e, cluster_agent::link *lk)
{
  assert(!dm_taskid.is_zero());
  assert(e.dm_nodespec.pm_ismaster);

  event_ptr<meta_area> L(e.dm_metarea);
  uuid owneragent;

  taskmap_t::iterator jj = L->pm_tasks.find(dm_taskid);
  if (jj == L->pm_tasks.end())
    return;

  owneragent = jj->second;
  L->pm_tasks.erase(jj);

  assert(!owneragent.is_zero());
  // now go remote this task from the given node's load list
  nodemap_t::iterator ii = L->pm_nodes.find(owneragent);

  if (ii != L->pm_nodes.end()) {
    event_ptr<meta_node::load_area> K(ii->second->pm_loadarea);

    for (int x=0; x<K->pm_tasks.size(); ++x)
      if (K->pm_tasks[x] == dm_taskid) {
        K->pm_tasks.erase(K->pm_tasks.begin() + x);
        break;
      }
  }
}

//
//
// cluster_agent::net_link
//
//

bool cluster_agent::net_link::make_link(const scopira::tool::url &remote,
    scopira::tool::count_ptr<net_link> &nl)
{
  nethostrec recs;

#ifdef CLUSTER_OUTPUT
  OUTPUT << "MAKE_LINK(" << remote << ")\n";
#endif

  // DNS
  if (!hostname_to_hostrec(remote.get_hostname(), recs))
    return false;

  // connect
  count_ptr<netflow> nt = new netflow(&recs.get_addr(), remote.get_port());

  if (nt->failed())
    return false;

  nl = new net_link(nt.get());

  return true;
}

cluster_agent::net_link::net_link(scopira::tool::netflow *fl)
  : dm_sock(fl), dm_recv_thread(recv_thread_func, this),
    dm_send_thread(send_thread_func, this)
{
  assert(fl->is_alive_object());
  dm_recv_thread.start();
  dm_send_thread.start();
}

cluster_agent::net_link::~net_link()
{
  {
    event_ptr<queue_area> L(dm_queue);

    L->pm_alive = false;

    L.notify_all();
  }

  dm_recv_thread.wait_stop();
  dm_send_thread.wait_stop();
}

void cluster_agent::net_link::set_link(link *l)
{
  event_ptr<queue_area> L(dm_queue);

  assert(!l || !L->pm_link);

  L->pm_link = l;
  L->pm_sendmsg = 0;

  L.notify_all();
}

void cluster_agent::net_link::enqueue_msg(network_msg *msg)
{
  event_ptr<queue_area> L(dm_queue);

  assert(msg->is_alive_object());
  if (L->pm_sendmsg.get())
    return;

  L->pm_sendmsg = msg;

  L.notify_all();
}

void* cluster_agent::net_link::recv_thread_func(void *herep)
{
  // TODO replace the sizeo() calls with binflow64?
  cluster_agent::net_link *here = reinterpret_cast<cluster_agent::net_link*>(herep);
  bool readready;
  bufferflow inbuf;
  uint32_t magic;
  uint32_t insz;
  size_t r;

  assert(here->is_alive_object());

  while (true) {
 //OUTPUT << "Read block";
    readready = here->dm_sock->read_ready(1000);

    {//if readready
      event_ptr<queue_area> L(here->dm_queue);
      if (!L->pm_alive)
        return 0; //full exit
    }

    // check for fail and bail
    if (here->dm_sock->failed())
      break;

    if (readready) {
      insz = 0;
      magic = 0;

      here->dm_sock->read_void(&magic, sizeof(magic));
      if (here->dm_sock->failed())
        break;
      if (magic != magic_c)
        continue;   // or drop the connection?
      r = here->dm_sock->read_void(&insz, sizeof(insz));
      if (here->dm_sock->failed())
        break;

      // ok, now receive the object
      assert(r == sizeof(insz));
#ifdef CLUSTER_OUTPUT
      OUTPUT << "receiving event size=" << insz << "\n";
#endif

      if (insz == 0)
        continue;

      scopira::tool::count_ptr<network_msg> ev;
      size_t sz_blocks;

      // read payload
      sz_blocks = (insz/blocksize_c+1) * blocksize_c;
      assert(sz_blocks>0);
      if (sz_blocks != inbuf.size())
        inbuf.reset_resize(sz_blocks);
      else
        inbuf.reset();
      assert(insz <= inbuf.size());

      // read the data block
      r = here->dm_sock->read(inbuf.c_array(), insz);
      if (r != insz) {
        OUTPUT << "run_receiver:2 mismatch on payload read (wanted=" << insz << ", got=" << r << ")\n";
        assert(false);
        continue;
      }

      // deserialize the event from this inbuf
      {
        bin64iflow bin1(false, &inbuf);
        isoiflow iso1(false, &bin1);

        if (!iso1.read_object_type(ev))
          ev = 0;
      }

      // finally, queue the msg
      count_ptr<link> lk;
      if (ev.get()) {
#ifdef CLUSTER_OUTPUT
        OUTPUT << "\033[35mqueueing received event\033[0m: xid=" << ev->get_xtion().pm_id << " name=" << ev->get_debug_name() << '\n';
#endif
        {
          event_ptr<queue_area> L(here->dm_queue);

          while (!L->pm_link && L->pm_alive)
            L.wait();

          if (!L->pm_alive)
            return 0; //full exit
          //OUTPUT << "on_recv:\n";
          assert(L->pm_link);
          assert(L->pm_link->is_alive_object());
          ev->set_net_link(here);
          lk = L->pm_link;
        }
        lk->on_recv(ev.get());
      }
    }
  }//while (true)

  // if we're exiting this way, its because we LOST the connection
  count_ptr<link> lk;
  {
    event_ptr<queue_area> L(here->dm_queue);
    assert(L->pm_link);
    lk = L->pm_link;
  }
  lk->on_close_net_link();

  return 0;
}

void* cluster_agent::net_link::send_thread_func(void *herep)
{
  net_link *here = reinterpret_cast<net_link*>(herep);
  bufferflow outbuf;
  uint32_t magic = magic_c;
  uint32_t sz;

  assert(here->is_alive_object());

  here->dm_sock->write_array("ScopiraAgent/0.9", 16);   //must be a multiple of 8 (sizeof long @ 64bits) (magic)

  while (true) {
    scopira::tool::count_ptr<network_msg> ev;

    {
      event_ptr<queue_area> L(here->dm_queue);

      // keep checking until something is in the queue for us
      while (L->pm_alive && L->pm_sendmsg.is_null())
        L.wait();

      if (!L->pm_alive)
        return 0;   // full exit

      // grab the event we are sending
      ev = L->pm_sendmsg;
    }

    // process this event... serialize and send it
    assert(!ev.is_null());

    // reset the buffer
    outbuf.reset();

    {//encode the payload
      bin64oflow bin1(false, &outbuf);
      isooflow iso1(false, &bin1);

      // send it! ho!
      iso1.write_object_type(ev);
    }//encode the payload

#ifdef CLUSTER_OUTPUT
    OUTPUT << "\033[34msending event\033[0m of size=" << outbuf.size() << " xid=" << ev->get_xtion().pm_id << " name=" << ev->get_debug_name() << ")\n";
#endif

    // transmit the size and then the payload
    sz = outbuf.size();   // 64 bit oh oh
    here->dm_sock->write_void(&magic, sizeof(magic));
    here->dm_sock->write_void(&sz, sizeof(sz));
    here->dm_sock->write(outbuf.c_array(), sz);

    // this is tricky, as to avoid deadlock with the network
    outbuf.reset_resize(0); // nuke the buffer, back to its default size
    count_ptr<link> dalink;
    count_ptr<network_msg> newmsg;
    {
      event_ptr<queue_area> L(here->dm_queue);

      dalink = L->pm_link;
      assert(dalink.get());     //this triggered on tryin to send msgs to tasks on agents that no longer exist
      // do this outside of the lock
    }
    //call this outside of the lock as to avoid dead lock
    dalink->on_sent(ev.get(), newmsg);
    {
      event_ptr<queue_area> L(here->dm_queue);
      //nuke the send msg and signal the master link (while IN this lock, yes)
      assert(ev.get() == L->pm_sendmsg.get());
      //finally, replace the working with the "new msg" (which might be null)
      L->pm_sendmsg = newmsg;
    }
  }//while(true)

  return 0;
}

//
// misc stuff
//

cluster_agent::meta_node::meta_node(const machine_spec &mspec, const node_spec &nspec)
  : pm_machinespec(mspec), pm_nodespec(nspec)
{
}

//
//
// cluster_agent::link
//
//

cluster_agent::link::link(cluster_agent *a, scopira::tool::uuid remoteuuid,
  bool tmpaddress, const scopira::tool::url &initter)
  : dm_agent(a), dm_initter(initter.get_proto() != "dummy"), dm_peerurl(initter),
  dm_tmpaddress(tmpaddress), dm_peeruuid(remoteuuid)
{
  assert(a->is_alive_object());
  assert(!remoteuuid.is_zero());

#ifdef CLUSTER_OUTPUT
  OUTPUT << "new_link:\n";
  print_status();
#endif
}

bool cluster_agent::link::has_link(void)
{
  return event_ptr<queue_area>(dm_queue)->pm_link.get() != 0;
}

void cluster_agent::link::accept_net_link(net_link *nl)
{
  bool isremotetmp;

  {
    event_ptr<queue_area> L(dm_queue);

    if (L->pm_link.get() != nl) {
      if (L->pm_link.get())
        L->pm_link->set_link(0);
      L->pm_link = nl;
      if (L->pm_link.get())
        L->pm_link->set_link(this);
    }

    // send them somthing
    unleash_a_msg(L, 0);

    isremotetmp = L->pm_remotetmp;
  }

  if (nl)
    enqueue_msg(new peer_hello_msg(dm_agent->la_get_spec(), dm_agent->dm_nodespec, dm_agent->dm_listenport, isremotetmp));
  if (nl && !dm_tmpaddress)
    enqueue_msg(new nottmp_hello_msg);
}

void cluster_agent::link::enqueue_msg(network_msg *msg)
{
  assert(msg->is_alive_object());

  event_ptr<queue_area> L(dm_queue);

  L->pm_sendq.push_back(msg);

  // queue the next one, if any
  unleash_a_msg(L, 0);
}

void cluster_agent::link::print_status(void)
{
  assert(is_alive_object());
  bool haslink = event_ptr<queue_area>(dm_queue)->pm_link.get() != 0;

  OUTPUT << "  link:" << dm_peeruuid
    << (haslink?" socket ":" x ")
    << (dm_tmpaddress?" temporary ":" x ")
    << (dm_initter?" initter ":" x ")
    << '\n';
}

void cluster_agent::link::on_sent(network_msg *msg, count_ptr<network_msg> &newnetmsg)
{
  assert(msg->is_alive_object());

  event_ptr<queue_area> L(dm_queue);
  bool foundit = false;

  // find and remove the sent msg
  for (msgqueue_t::iterator ii=L->pm_sendq.begin(); ii != L->pm_sendq.end(); ++ii)
    if (ii->get() == msg) {
      // found it
      L->pm_sendq.erase(ii);
      foundit = true;
      break;
    }
  assert(foundit);

  // queue the next one, if any
  unleash_a_msg(L, &newnetmsg);
}

void cluster_agent::link::on_recv(network_msg *msg)
{
  // relay to agent
  assert(dm_agent);
  dm_agent->enqueue_msg(msg);
}

void cluster_agent::link::on_close_net_link(void)
{
  accept_net_link(0);

  if (event_ptr<queue_area>(dm_queue)->pm_remotemaster)
    dm_agent->enqueue_msg(new quit_msg(true));
  else
    dm_agent->enqueue_msg(new remove_link_msg(dm_peeruuid));
}

void cluster_agent::link::unleash_a_msg(scopira::tool::event_ptr<queue_area> &L, count_ptr<network_msg> *putnewmsghere)
{
  if (L->pm_link.is_null() || L->pm_sendq.empty())
    return;

  count_ptr<network_msg> tosend;

  if (dm_tmpaddress || L->pm_remotetmp) {
    // find a special out of band msg
    for (msgqueue_t::iterator ii=L->pm_sendq.begin(); ii != L->pm_sendq.end(); ++ii)
      if ((*ii)->get_type() == network_msg::type_tmp_c) {
        tosend = (*ii).get();
        break;
      }
  } else
    tosend = L->pm_sendq.front().get(); // just send any

  if (tosend.get()) {
    if (putnewmsghere)
      *putnewmsghere = tosend;
    else
      L->pm_link->enqueue_msg(tosend.get());
  }
}

