
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

#include <scopira/agent/networkagent.h>

#include <string.h>

#include <scopira/tool/file.h>
#include <scopira/tool/random.h>
#include <scopira/core/loop.h>

#ifdef PLATFORM_win32
// disable depreacted warnings
#pragma warning(disable:4996)
#endif

//BBtargets libscopira.so

//#define NETWORK_OUTPUT

using namespace scopira::tool;
using namespace scopira::core;
using namespace scopira::agent;

//
//
// peer_spec
//
//

peer_spec::peer_spec(void)
  : pm_pingdelay(0), pm_hopcount(0)
{
}

bool peer_spec::load(scopira::tool::itflow_i& in)
{
  return pm_uuid.load(in) && pm_relay_uuid.load(in) &&
    pm_direct_url.load(in) && in.read_int(pm_pingdelay) &&
    in.read_int(pm_hopcount);
}

void peer_spec::save(scopira::tool::otflow_i& out) const
{
  pm_uuid.save(out);
  pm_relay_uuid.save(out);
  pm_direct_url.save(out);
  out.write_int(pm_pingdelay);
  out.write_int(pm_hopcount);
}

void peer_spec::parse_options(const std::string &options)
{
}

//
//
// network_agent
//
//

network_agent::network_agent(void)
  : dm_networkport(0),
    dm_listenthread(listen_thread_func, this),
    dm_udpthread(udp_thread_func, this),
    dm_adminthread(admin_thread_func, this)
{
  // pick an ID for myself
  dm_uuidmaker.next(dm_peerspec.pm_uuid);
  // stat the timer
  dm_age.start();

  /// figure out the network start address
  assert(basic_loop::instance()->has_config("network"));
  if (basic_loop::instance()->get_config("network") == "auto")
    dm_networkstart.set_url("scopiraudp://255.255.255.255/");
  else
    if (!dm_networkstart.set_url(basic_loop::instance()->get_config("network"))) {
      OUTPUT << "Failed to parse a valid url from network=" << basic_loop::instance()->get_config("network") << '\n';
      dm_adminarea.pm_data.pm_failedstate = failed_cantinit_c;
      return;
    }

  /// figure out the network name
  if (basic_loop::instance()->has_config("networkname"))
    dm_networkname = basic_loop::instance()->get_config("networkname");
  else
    dm_networkname = get_username();// assume the network name is the user

  // parse the listening port
  if (basic_loop::instance()->has_config("networkport")) {
    if (!string_to_int(basic_loop::instance()->get_config("networkport"), dm_networkport)) {
      OUTPUT << "Failed to parse a valid port from networkport=" << basic_loop::instance()->get_config("clusterport") << '\n';
      dm_adminarea.pm_data.pm_failedstate = failed_cantinit_c;
      return;
    }
  } else
    dm_networkport = default_port_c;

  // open the listening ports
  int i;
  for (i=0; i<100; ++i) {
    dm_listensocket.open(0, dm_networkport+i);
    dm_udpsocket.open(dm_networkport+i);
    if (!dm_listensocket.failed() && !dm_udpsocket.failed())
      break;    // success!
  }
  if (dm_listensocket.failed() || dm_udpsocket.failed()) {
    OUTPUT << "Failed to open listen port from " << dm_networkport << " on\n";
    dm_adminarea.pm_data.pm_failedstate = failed_cantinit_c;
    return;
  }
  dm_networkport = dm_networkport + i;    // record the actual port

  // ok, we're live
  dm_adminarea.pm_data.pm_failedstate = failed_ok_c;    // masters are always ok
  dm_adminarea.pm_data.pm_alive = true;

  OUTPUT << "Network Agent started.\n"
    " uuid=" << dm_peerspec.pm_uuid << "\n"
    " networkname=" << dm_networkname << "\n"
    " listenport=" << dm_networkport
    << "\n";

  dm_listenthread.start();
  dm_udpthread.start();
  dm_adminthread.start();
}

network_agent::~network_agent()
{
  int state = event_ptr<admin_area>(dm_adminarea)->pm_failedstate;

  if (state != failed_initting_c && state != failed_cantinit_c) {
    notify_stop();

    dm_listenthread.wait_stop();
    dm_udpthread.wait_stop();
    dm_adminthread.wait_stop();
  }
}

void network_agent::notify_stop(void)
{
  {
    event_ptr<admin_area> L(dm_adminarea);

    L->pm_alive = false;
    L.notify_all();
  }

  parent_type::notify_stop();
}

bool network_agent::failed(void) const
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

void network_agent::set_agenterror_reactor(agenterror_reactor_i *r)
{
  event_ptr<admin_area> L(dm_adminarea);

  L->pm_agenterror = r;
}

void* network_agent::listen_thread_func(void *herep)
{
  network_agent *here = reinterpret_cast<network_agent*>(herep);

  assert(here->is_alive_object());

  while (true)
  {
    event_ptr<admin_area> L(here->dm_adminarea);

    if (!L->pm_alive)
      return 0;
    
    L.wait();
    /*{
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
    }*/
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
class fixed_uuid
{
  public:
    unsigned char chars[16];

    void set(const scopira::tool::uuid &u) {
      assert(sizeof(fixed_uuid) == sizeof(scopira::tool::uuid));
      memcpy(this, &u, sizeof(fixed_uuid));
    }
    operator scopira::tool::uuid(void) const {
      scopira::tool::uuid ret;
      memcpy(&ret, this, sizeof(fixed_uuid));
      return ret;
    }
};
class fixed_addr
{
  public:
    unsigned char chars[4];

    void set(const scopira::tool::netaddr &a) {
      chars[0] = a[0];
      chars[1] = a[1];
      chars[2] = a[2];
      chars[3] = a[3];
    }
    operator scopira::tool::netaddr(void) const {
      return netaddr(chars[0], chars[1], chars[2], chars[3]);
    }
};

struct udpmsg_t {
  enum { magic_c = 0x3455FF33 };
  uint32_t pm_magic;
  uint16_t pm_type;
  fixed_uuid pm_srcid;

  /// CRC in the future.
  union {
    /// type-1 "where are you, master"
    /// type-2 "I am the master you seek"
    struct type2_t {
      fixed_string<50> masternetwork;
    } type2;
    /// type-3 "new master vote, i vote wirh rank X"
    struct type3_t {
      uint32_t rank;
    } type3;
    /// type-4 "send me all your known nodes for network X, and continue to send me any new ones for the next 60sec"
    struct type4_t {
      fixed_string<50> network;
    } type4;
    /// type-5 "new node notice, a responce to type-4... you may have lots of these
    struct type5_t {
      fixed_uuid id;
      fixed_addr addr;
      int32_t port;
      fixed_string<50> network;
    } type5;
    /// type-6 ping
    /// type-7 pong
  } pm_payload;

  // CRC checking here too...
  bool is_valid(scopira::tool::uuid src) const { return pm_magic == magic_c && static_cast<uuid>(pm_srcid) != src; }
  // CRC checking here too...
  void package(int ty, scopira::tool::uuid src) { pm_type = ty; pm_magic = magic_c; pm_srcid.set(src); }

  /// as bytes
  byte_t * as_bytes(void) { return reinterpret_cast<byte_t*>(this); }
  const byte_t * as_bytes(void) const { return reinterpret_cast<const byte_t*>(this); }
  size_t size(void) const { return sizeof(udpmsg_t); }
};
}//anonymous namespace

static void find_udpmaster_bcast(scopira::tool::uuid srcid, udpflow &fl, netaddr bcastaddr, int port)
{
  // prepare the bcast req message
  udpmsg_t msg;

  msg.package(1, srcid);
  // no data, send to port, port+1 and port+2
  OUTPUT << "BROADCAST: finding master port+=" << port << '\n';
  for (int x=0; x<5; ++x)
    fl.write(bcastaddr, port+x, msg.as_bytes(), msg.size());
}

static void vote_udpmaster_bcast(scopira::tool::uuid srcid, udpflow &fl, netaddr bcastaddr, int port, int myrank)
{
  // prepare the bcast req message
  udpmsg_t msg;

  msg.pm_payload.type3.rank = myrank;
  msg.package(3, srcid);
  // no data, send to port, port+1 and port+2
  OUTPUT << "BROADCAST: sending vote port+=" << port << '\n';
  for (int x=0; x<5; ++x)
    fl.write(bcastaddr, port+x, msg.as_bytes(), msg.size());
}

struct udpnode_t {
  scopira::tool::uuid id;
  std::string network;
  netaddr addr;
  int port;
  double regtime, lastping, lastpong;

  udpnode_t(void) : port(0), regtime(-1000), lastping(0), lastpong(-1) { }
};

void* network_agent::udp_thread_func(void *herep)
{
  typedef std::map<scopira::tool::uuid, udpnode_t> nodes_t;

  network_agent *here = reinterpret_cast<network_agent*>(herep);

  if (here->dm_networkstart.get_proto() != "scopiraudp")
    return 0;   // no UDP protocol, no need for this thread

  chrono clocky;
  nodes_t nodes;
  bool iammaster = false;
  bool havemaster = false;
  short state;
  double enteredstate;
  int stateinfo;
  double now, lastpingsession;
  netaddr bcastaddr, masteraddr;
  int bcastport, masterport;
  unsigned int myrank;       // this random number is used in resolving voting
  double fractionrank;

  assert(here->is_alive_object());

  {
    nethostrec hrec;
    if (!hostname_to_hostrec(here->dm_networkstart.get_hostname(), hrec)) {
      OUTPUT << "Failed to resolve: " << here->dm_networkstart.get_hostname() << '\n';

      event_ptr<admin_area> L(here->dm_adminarea);
      L->pm_failedstate = failed_cantinit_c;
      L.notify_all();
      return 0;
    }
    bcastaddr = hrec.get_addr();
  }

  bcastport = here->dm_networkstart.get_port();
  if (bcastport == 0)
    bcastport = default_port_c;

  clocky.start();
  {
    sysdev_gen gen;

    myrank = gen();
    fractionrank = static_cast<double>(myrank - gen.min()) / (gen.max() - gen.min());
  }

OUTPUT << "MYRANK = " << fractionrank << '\n';

  state = 1;
  enteredstate = clocky.get_running_time();
  lastpingsession = 0;

  // states
  //  0 - all ok
  //  1 - waiting for a master reply
  //  2 - in voting for new master mode

  /// send the initial bcast
  find_udpmaster_bcast(here->dm_peerspec.pm_uuid, here->dm_udpsocket, bcastaddr, bcastport);

  /// udp receive loop
  while (true)
  {
    {
      event_ptr<admin_area> L(here->dm_adminarea);

      if (!L->pm_alive)
        return 0;
    }

    /// wait for packet
assert(!here->dm_udpsocket.failed());
    if (!here->dm_udpsocket.read_ready(500)) {
      now = clocky.get_running_time();
OUTPUT << "now = " << now << '\n';

      // time out processing
      if (state == 1 && (now - enteredstate) > 1.0) {
        // time out: waiting for master to reply
        state = 2;
        enteredstate = now;
        stateinfo = 1;    // 1 means i was best one so far
        // send out the vote
        vote_udpmaster_bcast(here->dm_peerspec.pm_uuid, here->dm_udpsocket, bcastaddr, bcastport, myrank);
      } else if (state == 2) {
        if (stateinfo == 1 && (now - enteredstate) > 1.0) {
          // vote is over, I win, assume masterhood.
OUTPUT << "I WIN THE VOTE. AM MASTER\n";
          iammaster = true;
          havemaster = true;
          state = 0;
          enteredstate = now;
        } else if (stateinfo == 0 && (now - enteredstate) > 2.0) {
          // vote is over, I didn't win, look for new master
OUTPUT << "Lost vote\n";
          iammaster = false;
          state = 1;
          enteredstate = now;
          find_udpmaster_bcast(here->dm_peerspec.pm_uuid, here->dm_udpsocket, bcastaddr, bcastport);
        }
      }

      // do some pinging, sometimes too
      if (now - lastpingsession > 5) {
        lastpingsession = now;
        double delay = 5 + fractionrank*5;

        // ping tests
        for (nodes_t::iterator ii=nodes.begin(); ii != nodes.end(); ++ii)
          if (now - ii->second.lastping > delay+5 && now - ii->second.lastpong > delay) {
            // ping them
            ii->second.lastping = now;

            udpmsg_t pingmsg;

            pingmsg.package(6, here->dm_peerspec.pm_uuid);
OUTPUT << "!PING\n";
            here->dm_udpsocket.write(ii->second.addr, ii->second.port, pingmsg.as_bytes(), pingmsg.size());
          }
        // prune test
        nodes_t::iterator ii=nodes.begin();
        while (ii != nodes.end())
          if (now - ii->second.lastpong > delay+20) {
OUTPUT << "Removing DEAD node: " << ii->second.addr << ":" << ii->second.port << '\n';
            nodes.erase(ii);
            ii = nodes.begin();
          } else
            ++ii;
      }

      continue;
    }
    now = clocky.get_running_time();

    udpmsg_t msg;
    netaddr srcaddr;
    int srcport;
    if (msg.size() != here->dm_udpsocket.read(srcaddr, srcport, msg.as_bytes(), msg.size()))
      continue;

    // ok, parse the msg
    if (!msg.is_valid(here->dm_peerspec.pm_uuid))
      continue;
OUTPUT << " ...received msg from " << srcaddr << ":" << srcport << " type=" << msg.pm_type << '\n';

    switch (msg.pm_type) {
      case 1: { //master query?
          if (!iammaster)
            break;    // im not the master, ignore

          udpmsg_t replymsg;

          replymsg.pm_payload.type2.masternetwork.set( here->dm_networkname );

          replymsg.package(2, here->dm_peerspec.pm_uuid);
OUTPUT << "Notifying new node that I am the master\n";
          here->dm_udpsocket.write(srcaddr, srcport, replymsg.as_bytes(), replymsg.size());
        }
        break;
      case 2: {
          if (state != 1)
            break;    // i wasnt waitin for the master question, wtf

          // record the master
          state = 0;
          iammaster = false;
          havemaster = true;
          masteraddr = srcaddr;
          masterport = srcport;

          {
            // lets add the master to my node list
            udpnode_t &n = nodes[msg.pm_srcid];

            // update the node info (overwriting any previous, if any)
            n.id = msg.pm_srcid;
            n.network = msg.pm_payload.type2.masternetwork;
            n.addr = srcaddr;
            n.port = srcport;
            assert(n.port != 0);
            n.regtime = now;
          }
OUTPUT << "Found an existing master, ask for node lists.\n";

          udpmsg_t askmsg;

          askmsg.pm_payload.type4.network.set(here->dm_networkname);
          askmsg.package(4, here->dm_peerspec.pm_uuid);
          here->dm_udpsocket.write(masteraddr, masterport, askmsg.as_bytes(), askmsg.size());
          break;
        }
      case 3: { // received a voting thing
          if (state != 2)
            state = 2;      // im in the vote NOW
          enteredstate = now;
          stateinfo = myrank > msg.pm_payload.type3.rank ? 1 : 0;
          if (stateinfo == 1) // I'm better that this vote, send mine
            vote_udpmaster_bcast(here->dm_peerspec.pm_uuid, here->dm_udpsocket, bcastaddr, bcastport, myrank);
          break;
        }
      case 4: { // receive a registration msg
        std::string wantednetwork;
        udpnode_t &n = nodes[msg.pm_srcid];

        // update the node info (overwriting any previous, if any)
        n.id = msg.pm_srcid;
        wantednetwork = msg.pm_payload.type4.network;
        n.network = wantednetwork;
        n.addr = srcaddr;
        n.port = srcport;
        assert(n.port != 0);
        n.regtime = now;

        udpmsg_t newnodemsg;

        newnodemsg.pm_payload.type4.network.set(wantednetwork);

        for (nodes_t::iterator ii=nodes.begin(); ii != nodes.end(); ++ii)
          if (wantednetwork == ii->second.network) {
OUTPUT << "Sending out node notice to regged node\n";
            // this new reg'ed node would be intereted in this node
            newnodemsg.pm_payload.type5.id.set( ii->second.id );
            newnodemsg.pm_payload.type5.addr.set( ii->second.addr );
            newnodemsg.pm_payload.type5.port = ii->second.port;
            // network already set

            newnodemsg.package(5, here->dm_peerspec.pm_uuid);
            here->dm_udpsocket.write(srcaddr, srcport, newnodemsg.as_bytes(), newnodemsg.size());
          }

        newnodemsg.pm_payload.type5.id = msg.pm_srcid;
        newnodemsg.pm_payload.type5.addr.set( srcaddr );
        newnodemsg.pm_payload.type5.port = srcport;
        // network already set

        newnodemsg.package(5, here->dm_peerspec.pm_uuid);

        // ok, send it to all the people still interested in it
        for (nodes_t::iterator ii=nodes.begin(); ii != nodes.end(); ++ii)
          if (wantednetwork == ii->second.network && (now - ii->second.regtime) < 60.0) {
OUTPUT << "Sending out node notice to regged node\n";
            here->dm_udpsocket.write(ii->second.addr, ii->second.port, newnodemsg.as_bytes(), newnodemsg.size());
          }
        break;
      }
      case 5: { //received notice of a new node
        if (static_cast<uuid>(msg.pm_payload.type5.id) == here->dm_peerspec.pm_uuid)
          break;
        udpnode_t &n = nodes[msg.pm_payload.type5.id];

        n.id = msg.pm_payload.type5.id;
        n.network = msg.pm_payload.type5.network;
        n.addr = msg.pm_payload.type5.addr;
        n.port = msg.pm_payload.type5.port;
        assert(n.port != 0);

OUTPUT << "UDP node notice: " << n.addr << ":" << n.port << '\n';
        break;
      }
      case 6: { // received ping
        udpmsg_t pongmsg;

OUTPUT << "!PONG\n";
        pongmsg.package(7, here->dm_peerspec.pm_uuid);
        here->dm_udpsocket.write(srcaddr, srcport, pongmsg.as_bytes(), pongmsg.size());
        // FALL THROUGH TO 7
      }
      case 7: { // received pong, update the info
        if (nodes.count(msg.pm_srcid) == 0)
          break;

        udpnode_t &n = nodes[msg.pm_srcid];

        n.lastpong = now;
        assert(n.port != 0);
        break;
      }
    }//switch
  }//while true 

  return 0;
}

//
//
// UDP section - END
//
//

void* network_agent::admin_thread_func(void *herep)
{
  network_agent *here = reinterpret_cast<network_agent*>(herep);

  assert(here->is_alive_object());

  while (true)
  {
    event_ptr<admin_area> L(here->dm_adminarea);

    if (!L->pm_alive)
      return 0;
    
    L.wait();
  }//while true 

  return 0;
}

