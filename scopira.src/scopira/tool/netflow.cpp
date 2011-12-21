
/*
 *  Copyright (c) 2002-2003    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#include <scopira/tool/netflow.h>

//BBlibs
//BBtargets libscopira.so

#include <algorithm>

#include <scopira/tool/platform.h>
#include <scopira/tool/iterator_imp.h>
#include <scopira/tool/util.h>

//win32 && irix
#if defined(PLATFORM_win32) || defined(PLATFORM_irix)
#define socklen_t int
#endif

//win32
#ifdef  PLATFORM_win32
#include <winsock.h>
#else
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/tcp.h>      // for TCP_NODELAY
#include <netinet/in.h>
#include <sys/un.h>
#include <sys/poll.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#endif

#if defined(DEBUG_TOOL_netflow)
#include <scopira/core/output.h>
#include <scopira/tool/hexflow.h>
#if defined(PLATFORM_UNIX)
#include <ctype.h>
#endif
#endif

#include <assert.h>

using namespace scopira::tool;
using namespace std;

#ifdef PLATFORM_win32
static short g_init_count;
#endif

//
//
// netaddr
//
//

netaddr::netaddr(void)
{
  (*this)[0] = 127;
  (*this)[1] = 0;
  (*this)[2] = 0;
  (*this)[3] = 1;
}

netaddr::netaddr(int _ip0, int _ip1, int _ip2, int _ip3)
{
  (*this)[0] = 127;
  (*this)[1] = 0;
  (*this)[2] = 0;
  (*this)[3] = 1;
}

std::string netaddr::as_string(void) const
{
  std::string ret;

  ret.reserve(16);

  ret = int_to_string((*this)[0]);

  for (int x=1; x<size(); ++x) {
    ret += '.';
    ret += int_to_string((*this)[x]);
  }

  return ret;
}

#ifdef PLATFORM_win32
// disable depreacted warnings
#pragma warning(push)
#pragma warning(disable:4996)
#endif
SCOPIRA_EXPORT bool netaddr::parse_string(const std::string &s)
{
#ifdef PLATFORM_win32
  int b[4];
#else
  // the two platforms cant even agree on scanf types, ugh
  unsigned short int b[4];
#endif

  if (4 != sscanf(s.c_str(),
    "%hd.%hd.%hd.%hd",
    &b[0], &b[1], &b[2], &b[3]))
    return false;

  (*this)[0] = static_cast<unsigned char>(b[0]);
  (*this)[1] = static_cast<unsigned char>(b[1]);
  (*this)[2] = static_cast<unsigned char>(b[2]);
  (*this)[3] = static_cast<unsigned char>(b[3]);

  return true;

}
#ifdef PLATFORM_win32
// disable depreacted warnings
#pragma warning(pop)
#endif

scopira::tool::oflow_i& operator <<(scopira::tool::oflow_i& o, const scopira::tool::netaddr &addr)
{
  o << addr[0] << '.'
    << addr[1] << '.'
    << addr[2] << '.'
    << addr[3];
  return o;
}

//
//
// nethostrec
//
//

/// constructor
nethostrec::nethostrec(void)
  : aliases(true), addrlen(0), addrlist(true)
{
}

/// destructor
nethostrec::~nethostrec(void)
{
}

/// resets the rec
void nethostrec::clear(void)
{
  name.clear();
  aliases.clear();
  addrlen = 0;
  addrlist.clear();
}

/// gets the first host addr
const netaddr & nethostrec::get_addr(void) const
{
  assert(!addrlist.empty());
  return *addrlist.begin();
}

SCOPIRA_EXPORT scopira::tool::oflow_i& operator <<(scopira::tool::oflow_i& o, const scopira::tool::nethostrec &recs)
{
  nethostrec::strlist_t::const_iterator jj, endjj;
  o << recs.name << ": ";
  endjj = recs.aliases.end();
  for (jj=recs.aliases.begin(); jj!=endjj; ++jj)
    o << *jj;
  o << "; ";
  nethostrec::addrlist_t::const_iterator ii, endii;
  endii = recs.addrlist.end();
  for (ii = recs.addrlist.begin(); ii != endii; ++ii)
    o << *ii;
  o << ".\n";

  return o;
}

//
//
// funcs
//
//

string scopira::tool::get_hostname(void)
{
  char buf[100];
  
  if (gethostname(buf, sizeof(buf)))
    return ""; //error
  else
    return buf;
}

bool scopira::tool::hostname_to_hostrec(const std::string& hname, nethostrec& rec)
{
  hostent* hp;
  char** kk;
  char* w;
  int i;
  netaddr ad;

#ifdef PLATFORM_win32
  assert(g_init_count > 0 && "[You need to have a net_loop or basic_loop to use netflows]\n");
#endif

  // look it up
  hp = gethostbyname(hname.c_str());

  // failed?
  if (!hp) {
#ifdef PLATFORM_win32
    assert(WSAGetLastError() != WSANOTINITIALISED);
#endif
    return false;
  }

  assert(hp->h_addrtype == AF_INET);

  // clear the rec
  rec.clear();

  rec.addrlen = hp->h_length;
  assert(rec.addrlen > 0);

  rec.name = hp->h_name;

  // add host aliases
  for (kk = hp->h_aliases; *kk; kk++)
    rec.aliases.push_back(*kk);

  // add ip list
  for (kk = hp->h_addr_list; *kk; kk++) {
    assert(rec.addrlen == ad.size());
    for (i=0, w = *kk; i<rec.addrlen; w++, i++)
      ad[i] = *w;
    rec.addrlist.push_back(ad);
  }

  return true;
}

//
//
// net_loop
//
//


#ifdef PLATFORM_win32
//static short g_init_count;//see top of this file

net_loop::net_loop(void)
{
  ++g_init_count;
  if (g_init_count != 1)
    return;
    
  WSADATA wsaData;
  WORD wVersionRequested = MAKEWORD(1, 1);

  // ignore return, what else can we do on error, seriously :)
  WSAStartup(wVersionRequested, &wsaData);
}

net_loop::~net_loop()
{
  --g_init_count;
  
  if (g_init_count != 0)
    return;
  WSACleanup();
}
#endif

//
//
// netflow
//
//

/// default constructor
netflow::netflow(void)
  : dm_open(false), dm_fail(true), dm_sfail(true)
{
}

/// opening constructor
netflow::netflow(const netaddr *_addr, int _port, int socket_options)
  : dm_open(false), dm_fail(true), dm_sfail(true)
{
  open(_addr, _port, socket_options);
}

/// destructor
netflow::~netflow()
{
  close();
}

/// are we in a failed state?
bool netflow::failed(void) const
{
  if (dm_open)
    return dm_fail && dm_sfail;
  else
    return true;
}

size_t netflow::read(byte_t* _buf, size_t _maxsize)
{
  size_t lastread;
  assert(_buf);
  //assert(_maxsize >= 0);
  if (failed() || _maxsize == 0)
    return 0;
    
#ifdef PLATFORM_win32
  size_t togo = _maxsize;
  // make my own MSG_WAITALL... come on already, windows
  do {
    lastread = ::recv(dm_sock, reinterpret_cast<char*>(_buf), static_cast<int>(togo), 0);
    _buf += lastread;
    togo -= lastread;
  } while (lastread > 0 && togo > 0);
  
  if (togo == _maxsize) {
    dm_fail = true;
    return 0;
  }

  return _maxsize - togo;
#else
#ifdef PLATFORM_osx
  lastread = ::recv(dm_sock, _buf, _maxsize, MSG_WAITALL);
#else
  lastread = ::recv(dm_sock, _buf, _maxsize, MSG_NOSIGNAL|MSG_WAITALL);
#endif
  
  if (lastread <= 0) {
    dm_fail = true;
    return 0;
  }

  return lastread;
#endif
}

size_t netflow::write(const byte_t* _buf, size_t _size)
{
  size_t lastsent;

  if (failed() || _size == 0)
    return 0;
  assert(_buf);
    
#ifdef PLATFORM_win32
  // simulate FULL blocking
  size_t togo = _size;
  do {
    lastsent = ::send(dm_sock, reinterpret_cast<const char*>(_buf), static_cast<int>(togo), 0);
    _buf += lastsent;
    togo -= lastsent;
  } while (lastsent>0 && togo>0);

  if (togo == _size) {
    dm_fail = true;
    return 0;
  }

  return _size - togo;
#else
#ifdef PLATFORM_osx
  lastsent = ::send(dm_sock, _buf, _size, 0);
#else
  lastsent = ::send(dm_sock, _buf, _size, MSG_NOSIGNAL);
#endif

  if (lastsent != _size) { // partial send, we didn't expect this..
    dm_fail = true;
  }

  return lastsent;
#endif
}

size_t netflow::read_short(byte_t* _buf, size_t _maxsize)
{
  size_t lastread;
  assert(_buf);
  //assert(_maxsize >= 0);
  if (failed() || _maxsize == 0)
    return 0;
    
#ifdef PLATFORM_win32
  lastread = ::recv(dm_sock, reinterpret_cast<char*>(_buf), static_cast<int>(_maxsize), 0);
#elif defined(PLATFORM_osx)
  lastread = ::recv(dm_sock, _buf, _maxsize, 0);
#else
  lastread = ::recv(dm_sock, _buf, _maxsize, MSG_NOSIGNAL);
#endif
  
  if (lastread <= 0) {
    dm_fail = true;
    return 0;
  }

  return lastread;
}

void netflow::open(const netaddr *_addr, int _port, int socket_options)
{
  sockaddr_in nam;
  int r;
  
#ifdef PLATFORM_win32
  assert(g_init_count > 0 && "[You need to have a net_loop or basic_loop to use netflows]\n");
#endif

  close();
  assert(_port > 0);

  // try to open a socket
  dm_sock = socket(AF_INET, SOCK_STREAM, 0);
#ifdef PLATFORM_win32
  if (dm_sock == INVALID_SOCKET) {
#else
  if (dm_sock < 0 || dm_sock == -1) {
#endif
    assert(false);
    return;   // bail, error
  }

  dm_server = !_addr;
  dm_port = _port;

  // connect the socket to remote host
  nam.sin_family = AF_INET;
  nam.sin_port = htons(_port);

  if (socket_options & tcp_nodelay_c) {
    int nodelay_flag = 1;
#ifdef PLATFORM_win32
    setsockopt(dm_sock, IPPROTO_TCP, TCP_NODELAY, (char*)&nodelay_flag, sizeof(nodelay_flag));
#else
    setsockopt(dm_sock, IPPROTO_TCP, TCP_NODELAY, &nodelay_flag, sizeof(nodelay_flag));
#endif
  }

  // server mode
  if (dm_server) {
    // wild card address
    // use man 7 socket/setsock SO_REUSEADDR here?
    // default uses nagle. is that good enough for us?
    nam.sin_addr.s_addr = INADDR_ANY;

#ifdef PLATFORM_linux
    int on = 0xFFFFFFFF;
    // prevent "hot ports"
    setsockopt(dm_sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
#endif

    // bind to addr
    r = bind(dm_sock, reinterpret_cast<sockaddr*>(&nam), sizeof(nam));
    if (r != 0) {
      // error, close and bail
#ifdef PLATFORM_win32
      ::closesocket(dm_sock);
      dm_sock = INVALID_SOCKET;
#else
      ::close(dm_sock);
      dm_sock = 0;
#endif // PLATFORM_win32
      return;
    }

    r = ::listen(dm_sock, 20);
    if (r != 0) {
      // error, close and bail
#ifdef PLATFORM_win32
      ::closesocket(dm_sock);
      dm_sock = INVALID_SOCKET;
#else
      ::close(dm_sock);
      dm_sock = 0;
#endif // PLATFORM_win32
      return;
    }
  } else {
    // client mode
    assert(static_cast<int>(sizeof(nam.sin_addr)) <= _addr->size());
    memcpy(&nam.sin_addr, _addr->c_array(), sizeof(nam.sin_addr));
    dm_addr = *_addr;

    // do the connect
    r = connect(dm_sock, reinterpret_cast<sockaddr*>(&nam), sizeof(nam));
    if (r != 0) {
      // error, close and bail
#ifdef PLATFORM_win32
      ::closesocket(dm_sock);
      dm_sock = INVALID_SOCKET;
#else
      ::close(dm_sock);
      dm_sock = 0;
#endif // PLATFORM_win32
      return;
    }
  }

  // all good
  dm_open = true;
  dm_fail = dm_server;
  dm_sfail = !dm_server;
}

/// close the file
void netflow::close(void)
{
  if (!dm_open)
    return;

#ifdef PLATFORM_win32 
  closesocket(dm_sock);
  dm_sock = INVALID_SOCKET;
#else
  ::close(dm_sock);
  dm_sock = 0;
#endif
  dm_open = false;
  dm_fail = true;
  dm_sfail = true;
}

bool netflow::accept(netflow& nconn)
{
  bool stat;
  sockaddr_in nam;
  socket_handle_t newsock;
#ifdef PLATFORM_win32
  int len;
#else
  socklen_t len;
#endif

  if (dm_sfail)
    return false;

  len = sizeof(nam);

  newsock = ::accept(dm_sock, reinterpret_cast<sockaddr*>(&nam), &len);
  
  if (newsock >= 0) {
    nconn.open_relay(newsock,
      reinterpret_cast<flow_i::byte_t*>(&nam.sin_addr), len, ntohs(nam.sin_port));
    stat =  true;
  } else
    stat =  false;   //failed.

  return stat;
}

bool netflow::read_ready(int msec)
{
#ifdef PLATFORM_win32 
  fd_set set;

  // 1000 micro seconds (usec) in a millisecond (msec)
  // 1000 milli seconds (msec) in a second (sec)

  timeval time;
  time.tv_sec = msec/1000;
  time.tv_usec = (msec%1000)*1000;

  FD_ZERO(&set);     // clear the fd_set
  FD_SET(dm_sock, &set); // add socket to set

  return ::select(0, &set, NULL, NULL, &time) > 0;
#else
  int x;
  struct pollfd pfd;
  
  if (failed())
    return false;

  pfd.fd = dm_sock;
  pfd.events = POLLIN | POLLPRI | POLLERR | POLLHUP | POLLNVAL;
  pfd.revents = 0;
  
  x = poll(&pfd, 1, msec);

  if (pfd.revents & POLLERR || pfd.revents & POLLHUP || pfd.revents & POLLNVAL)
    dm_fail = true;

  return x != 0;
#endif
}

void netflow::open_relay(socket_handle_t _fd, const flow_i::byte_t* _addr, int _addrlen, int _port)
{
  close();

  assert(_fd >= 0);

  dm_sock = _fd;
  dm_server = false;
  dm_open = true;
  dm_fail = false;
  dm_sfail = true;

  assert(dm_addr.size() <= _addrlen);
  memcpy(dm_addr.c_array(), _addr, dm_addr.size());
  dm_port = _port;
}

//
//
// udpflow
//
//

udpflow::udpflow(void)
  : dm_port(0), dm_fail(true), dm_open(false)
{
}

udpflow::udpflow(int _port)
  : dm_port(0), dm_fail(true), dm_open(false)
{
  open(_port);
}

udpflow::~udpflow()
{
  close();
}

#ifdef PLATFORM_win32
#define ssize_t int
#endif

size_t udpflow::read(netaddr &actualsrc, int &actualport, byte_t* _buf, size_t _maxsize)
{
  if (failed() || _maxsize == 0)
    return 0;
  assert(_buf);

  sockaddr_in sa;
  socklen_t sa_size = sizeof(sa);
  ssize_t rd;

  sa.sin_family = AF_INET;

#ifdef PLATFORM_win32
  rd = ::recvfrom(dm_sock, reinterpret_cast<char*>(_buf), static_cast<int>(_maxsize), 0, reinterpret_cast<sockaddr*>(&sa), &sa_size);
#else
  rd = ::recvfrom(dm_sock, _buf, _maxsize, 0, reinterpret_cast<sockaddr*>(&sa), &sa_size);
#endif

  if (rd == -1) {
    dm_fail = true;
    return 0;
  }

  assert(sizeof(actualsrc) == sizeof(sa.sin_addr));
  memcpy(&actualsrc, &sa.sin_addr, sizeof(actualsrc));
  actualport = ntohs(sa.sin_port);
  
  return rd;
}

size_t udpflow::write(netaddr dest, int destport, const byte_t* _buf, size_t _size)
{
assert(destport != 0);
  if (failed() || _size == 0)
    return 0;
  assert(_buf);

  sockaddr_in sa;
  ssize_t sent;

  sa.sin_family = AF_INET;
  sa.sin_port = htons(destport);
  assert(sizeof(sa.sin_addr) == sizeof(dest));
  memcpy(&sa.sin_addr, &dest, sizeof(sa.sin_addr));

#ifdef PLATFORM_win32
  sent = ::sendto(dm_sock, reinterpret_cast<const char*>(_buf), static_cast<int>(_size), 0, reinterpret_cast<sockaddr*>(&sa), sizeof(sa));
#else
  sent = ::sendto(dm_sock, _buf, _size, 0, reinterpret_cast<sockaddr*>(&sa), sizeof(sa));
#endif

  if (sent == -1) {
#ifdef PLATFORM_win32
    int er = WSAGetLastError(); 

    if (er != WSAEMSGSIZE)
      dm_fail = true;
#else
    int er = errno;

    if (er != EMSGSIZE)
      dm_fail = true;
#endif
    return 0;
  }

  assert(sent == _size);
  return _size;
}

void udpflow::open(int _port)
{
  sockaddr_in nam;
  int r;
  
#ifdef PLATFORM_win32
  assert(g_init_count > 0 && "[You need to have a net_loop or basic_loop to use udpflows]\n");
#endif

  close();

  // try to open a socket
  dm_sock = socket(AF_INET, SOCK_DGRAM, 0);
#ifdef PLATFORM_win32
  if (dm_sock == INVALID_SOCKET) {
#else
  if (dm_sock < 0 || dm_sock == -1) {
#endif
    assert(false);
    return;   // bail, error
  }

  nam.sin_family = AF_INET;
  nam.sin_addr.s_addr = INADDR_ANY;
  nam.sin_port = htons(_port);

  // bind to addr
  r = bind(dm_sock, reinterpret_cast<sockaddr*>(&nam), sizeof(nam));

  if (r != 0) {
    // error, close and bail
#ifdef PLATFORM_win32
    ::closesocket(dm_sock);
    dm_sock = INVALID_SOCKET;
#else
    ::close(dm_sock);
    dm_sock = 0;
#endif
    return;
  }

  socklen_t outlen = sizeof(nam);
  getsockname(dm_sock, reinterpret_cast<sockaddr*>(&nam), &outlen);
  dm_port = ntohs(nam.sin_port);

  int val = 1;
#ifdef PLATFORM_win32
  setsockopt(dm_sock, SOL_SOCKET, SO_BROADCAST, reinterpret_cast<const char*>(&val), sizeof(val));
#else
  setsockopt(dm_sock, SOL_SOCKET, SO_BROADCAST, &val, sizeof(val));
#endif

  dm_open = true;
  dm_fail = false;
}

void udpflow::close(void)
{
  if (!dm_open)
    return;

#ifdef PLATFORM_win32 
  closesocket(dm_sock);
  dm_sock = INVALID_SOCKET;
#else
  ::close(dm_sock);
  dm_sock = 0;
#endif

  dm_open = false;
  dm_fail = true;
}

bool udpflow::read_ready(int msec)
{
#ifdef PLATFORM_win32 
  fd_set set;

  // 1000 micro seconds (usec) in a millisecond (msec)
  // 1000 milli seconds (msec) in a second (sec)

  timeval time;
  time.tv_sec = msec/1000;
  time.tv_usec = (msec%1000)*1000;

  FD_ZERO(&set);     // clear the fd_set
  FD_SET(dm_sock, &set); // add socket to set

  return ::select(0, &set, NULL, NULL, &time) > 0;
#else
  int x;
  struct pollfd pfd;
  
  if (failed())
    return false;

  pfd.fd = dm_sock;
  pfd.events = POLLIN | POLLPRI | POLLERR | POLLHUP | POLLNVAL;
  pfd.revents = 0;
  
  x = poll(&pfd, 1, msec);

  if (pfd.revents & POLLERR || pfd.revents & POLLHUP || pfd.revents & POLLNVAL)
    dm_fail = true;

  return x != 0;
#endif
}

//
//
// unixflow
// 
//

#ifndef PLATFORM_win32 
unixflow::unixflow(void)
  : dm_open(false), dm_fail(true), dm_sfail(true)
{
}

unixflow::unixflow(const std::string &filename, bool server_mode)
  : dm_open(false), dm_fail(true), dm_sfail(true)
{
  open(filename, server_mode);
}

unixflow::~unixflow()
{
  close();
}

bool unixflow::failed(void) const
{
  if (dm_open)
    return dm_fail && dm_sfail;
  else
    return true;
}

size_t unixflow::read(byte_t* _buf, size_t _maxsize)
{
  assert(_buf);
  //assert(_maxsize >= 0);
  if (failed())
    return 0;
  if (_maxsize > 0) {
#ifndef PLATFORM_win32
#ifdef PLATFORM_irix 
    _maxsize = ::recv(dm_sock, _buf, _maxsize, 0);
#elif defined(PLATFORM_osx)
    _maxsize = ::recv(dm_sock, _buf, _maxsize, 0);
#else
    _maxsize = ::recv(dm_sock, _buf, _maxsize, MSG_NOSIGNAL);
#endif
#endif
    if (_maxsize <= 0) {
      dm_fail = true;
      return 0;
    }
  }

  return _maxsize;
}
size_t unixflow::write(const byte_t* _buf, size_t _size)
{
  assert(_buf);
  //assert(_size >= 0);
  if (failed())
    return 0;
  if (_size > 0) {
#ifdef PLATFORM_irix
    _size = ::send(dm_sock, _buf, _size, 0);
#elif defined(PLATFORM_osx)
    _size = ::send(dm_sock, _buf, _size, 0);
#else
    _size = ::send(dm_sock, _buf, _size, MSG_NOSIGNAL);
#endif

    if (_size <= 0) {
      dm_fail = true;
      return 0;
    }
  }

  return _size;
}

// this is stupid... this guy SHOULD be defined by the system
#define UNIX_PATH_MAX 108
void unixflow::open(const std::string &filename, bool server_mode)
{
  sockaddr_un nam;
  int r;
  
  close();

  assert(UNIX_PATH_MAX == sizeof(nam.sun_path));

  if (filename.size() >= UNIX_PATH_MAX) {
    assert(false);    // what to do...
    return;
  }

  // try to open a socket
  dm_sock = socket(AF_UNIX, SOCK_STREAM, 0);
  if (dm_sock < 0){
    assert(false);
    return;   // bail, error
  }

  // connect the socket to remote host
  memset(&nam, 0, sizeof(nam));
  nam.sun_family = AF_UNIX;
  // although we check for this, lets check again
  strncpy(nam.sun_path, filename.c_str(), UNIX_PATH_MAX);
  nam.sun_path[UNIX_PATH_MAX - 1] = 0;    // null terminate it

  // server mode
  if (dm_server) {
    // bind to addr
    r = bind(dm_sock, reinterpret_cast<sockaddr*>(&nam), sizeof(nam));
    if (r != 0) {
      // error, close and bail
      ::close(dm_sock);      
      return;
    }

    r = ::listen(dm_sock, 20);
    if (r != 0) {
      // error, close and bail
      ::close(dm_sock);
      return;
    }
  } else {
    // do the connect
    r = connect(dm_sock, reinterpret_cast<sockaddr*>(&nam), sizeof(nam));
    if (r != 0) {
      // error, close and bail
      ::close(dm_sock);
      return;
    }
  }

  // all good
  dm_open = true;
  dm_fail = dm_server;
  dm_sfail = !dm_server;
}

void unixflow::close(void)
{
  if (dm_open) {
    ::close(dm_sock);
    if (dm_server)
      unlink(dm_filename.c_str());   // remove the previous socket, if any

    dm_open = false;
    dm_fail = true;
    dm_sfail = true;
  }
}

bool unixflow::accept(unixflow& nconn)
{
  bool stat;
  int newsock;
  sockaddr_un nam;
  socklen_t len;

  if (dm_sfail)
    return false;

  len = sizeof(nam);

  newsock = ::accept(dm_sock, reinterpret_cast<sockaddr*>(&nam), &len);
  
  if (newsock >= 0) {
    nconn.open_relay(newsock, dm_filename);
    stat =  true;
  } else
    stat =  false;   //failed.

  return stat;
}

bool unixflow::read_ready(int msec)
{
  int x;
  struct pollfd pfd;
  
  if (failed())
    return false;

  pfd.fd = dm_sock;
  pfd.events = POLLIN | POLLPRI | POLLERR | POLLHUP | POLLNVAL;
  pfd.revents = 0;
  
  x = poll(&pfd, 1, msec);

  if (pfd.revents & POLLERR || pfd.revents & POLLHUP || pfd.revents & POLLNVAL)
    dm_fail = true;

  return x != 0;
}

void unixflow::open_relay(int _fd, const std::string &filename)
{
  close();

  assert(_fd >= 0);

  dm_sock = _fd;
  dm_filename = filename;

  dm_server = false;
  dm_open = true;
  dm_fail = false;
  dm_sfail = true;
}

#endif    //PLATFORM_win32

//
//
// test code
//
//

#ifdef SCO_DEBUG_netflow
#include <scopira/tool/fileflow.h>
int main(void)
{
  netflow::init();

#ifndef NDEBUG
  objrefcounter _duh;
  _duh.foo(); // does nothing
#endif
  fileflow fout(fileflow::stdout_c, 0);

  fout.auto_ref();

  fout << "Hello, NETWORK \"" << get_hostname() << "\"\n";
  nethostrec hrec;

  if (hostname_to_hostrec("www.microsoft.com", hrec))
    fout << hrec << '\n';

  int sz, tot;
  flow_i::byte_t buf[4096];

  tot = 0;

  netaddr meaddr(127,0,0,1);
  netflow mesock(hrec.get_addr(), 80);
  mesock << "GET /\n\n";

  meaddr.auto_ref();
  mesock.auto_ref();

  fout << "Connected to: " << mesock.get_addr() << ':' << mesock.get_port() << '\n';
  sz = mesock.read_binary(buf, sizeof(buf));
  tot = sz;
  while (sz > 0) {
    fout.write_binary(buf, sz);
    sz = mesock.read_binary(buf, sizeof(buf));
    tot += sz;
  }

  /*netaddr na(127,0,0,1);

  netflow sk(&na, 80), csk;

  sk.auto_ref();
  csk.auto_ref();

  fout << "Listening on: " << sk.get_addr() << ':' << sk.get_port() << '\n';
   if (sk.failed())
    fout << "server port bind error?\n";
  else {
    while (!sk.read_ready(1000))
      fout << "server listen, timeout\n";
    if (sk.accept(csk)) {
        fout << "Accepting on: " << csk.get_addr() << ':' << csk.get_port() << '\n';
        sz = csk.read_binary(buf, sizeof(buf));
        tot = sz;
        while (sz > 0) {
          fout.write_binary(buf, sz);
          sz = csk.read_binary(buf, sizeof(buf));
          tot += sz;
        }
        csk.close();
      }
  }
  sk.close();*/

  fout << "\n\nRead " << tot << " bytes.\n";

  netflow::cleanup();

  return 0;
}
#endif

// WSACleanup();
