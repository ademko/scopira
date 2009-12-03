
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
 
#ifndef __INCLUDED_SCOPIRA_TOOL_NETFLOW_H__
#define __INCLUDED_SCOPIRA_TOOL_NETFLOW_H__

#include <list>

#include <scopira/tool/export.h>
#include <scopira/tool/array.h>
#include <scopira/tool/flow.h>
#include <scopira/tool/platform.h>

namespace scopira
{
  namespace tool
  {
#ifdef PLATFORM_win32
    typedef SOCKET socket_handle_t;
#else
    typedef int socket_handle_t;
#endif

    class netaddr;
    class nethostrec;

    class netflow;  // TCP
    class udpflow;

    class unixflow;

    class net_loop;
    
    /// gets the hostname of this machine
    SCOPIRA_EXPORT std::string get_hostname(void);
    /// looks up the hname, and saves into rec, returns true on success
    SCOPIRA_EXPORT bool hostname_to_hostrec(const std::string& hname, nethostrec& rec);
  }
}

// win32 implementation notice:
// users dont link to the winsock lib, so DONT
// inline any winsock calls
 
/**
 * an ip address. this a simple, fully copiable pod.
 *
 * @author Aleksander Demko
 */
class scopira::tool::netaddr : public scopira::tool::fixed_array<unsigned char,4>
{
  public:
    /// default constructor, inits to local host
    SCOPIRA_EXPORT netaddr(void);
    /// quick ip4 constructor
    SCOPIRA_EXPORT netaddr(int _ip0, int _ip1, int _ip2, int _ip3);

    /// makes and returns the zero address
    static netaddr zero(void) { return netaddr(0, 0, 0, 0); }
    /// makes and returns the any address
    static netaddr any(void) { return zero(); }
    /// makes and returns the bcast address
    static netaddr broadcast(void) { return netaddr(255, 255, 255, 255); }
    /// makes and returns localhost
    static netaddr localhost(void) { return netaddr(); }

    /// return x.x.x.x form
    SCOPIRA_EXPORT std::string as_string(void) const;
    /// parse from a string
    SCOPIRA_EXPORT bool parse_string(const std::string &s);
};

SCOPIRA_EXPORT scopira::tool::oflow_i& operator <<(scopira::tool::oflow_i& o, const scopira::tool::netaddr &addr);

/**
 * A host record, from dns/lookup. Does this need to be a descendant of
 * object??
 *
 * @author Aleksander Demko
 */
class scopira::tool::nethostrec : public virtual scopira::tool::object
{
  public: // all public, like a struct

    typedef std::list < std::string > strlist_t;
    typedef std::list < netaddr > addrlist_t;

    std::string name;
    strlist_t aliases;    // list of <stringobj>
    int addrlen;        // length of an address
    addrlist_t addrlist;   // list of <netaddr>

  public:
    /// constructor
    SCOPIRA_EXPORT nethostrec(void);
    /// destructor
    SCOPIRA_EXPORT virtual ~nethostrec(void);

    /// resets the rec
    SCOPIRA_EXPORT void clear(void);

    /// gets the first host addr
    SCOPIRA_EXPORT const netaddr & get_addr(void) const;
};

SCOPIRA_EXPORT scopira::tool::oflow_i& operator <<(scopira::tool::oflow_i& o, const scopira::tool::nethostrec &recs);

/**
 * a network flow (TCP)
 *
 * @author Aleksander Demko
 */
class scopira::tool::netflow : public scopira::tool::iflow_i, public scopira::tool::oflow_i
{
  public:
    enum {
      tcp_nodelay_c = 1,
    };
  private:    // typedef socket handles to a common name
    /// the file handle
    socket_handle_t dm_sock;

    /// currently open?
    bool dm_open;
    /// currently failed?
    bool dm_fail;
    /// failed, for server
    bool dm_sfail;
    /// in server mode?
    bool dm_server;
    /// port
    int dm_port;
    /// address
    netaddr dm_addr;

  public:
    /**
     * default constructor, you can open() later.
     *
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT netflow(void);
    /**
     * Openning ctor.
     * _addr may be null for servers. The pointer is not used
     * after this call returns.
     * (ie. it can be a termporary).
     *
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT netflow(const netaddr * _addr, int _port, int socket_options = 0);
    /// destructor
    SCOPIRA_EXPORT virtual ~netflow();

    /// are we in a failed state?
    SCOPIRA_EXPORT virtual bool failed(void) const;

    /// read raw block data, returns num read in
    SCOPIRA_EXPORT virtual size_t read(byte_t* _buf, size_t _maxsize);
    /// write a raw block of data
    SCOPIRA_EXPORT virtual size_t write(const byte_t* _buf, size_t _size);

    /// read raw block data, returns num read in
    /// this variant will return as soon as any data arrives though,
    /// so the return/read in size may often be less than _maxsize
    SCOPIRA_EXPORT size_t read_short(byte_t* _buf, size_t _maxsize);

    /**
     * Opens a client socket, null _addr for server socket
     * The _addr object is not refered to after this call
     * (ie. it can be a termporary).
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT void open(const netaddr * _addr, int _port, int socket_options = 0);
    /// close the file
    SCOPIRA_EXPORT void close(void);

    /// (for server), opens the given connection to the new link
    /// returns true if a new linke came in, false on error
    SCOPIRA_EXPORT bool accept(netflow& nconn);

    /**
     * is anything in the input buffer, within the given time? (false==timeout)
     * 1000msec = 1 second
     *
     * TODO: in the future, add a select() like thing for a bunch of them
     * (and perhaps for fileflow too?) and udpflow, etc? common base class
     * for read_read'able things?
     *
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT bool read_ready(int msec);

    /// gets the address
    const netaddr& get_addr(void) const { return dm_addr; }
    /// gets the port
    int get_port(void) const { return dm_port; }

  protected:
    /// internal, used by server ::accept
    SCOPIRA_EXPORT void open_relay(socket_handle_t _fd, const flow_i::byte_t* _addr, int _addrlen, int _port);

};

/**
 * This initializes the network layer.
 * You can have multiple instances of this, it's recursive-able.
 * This is unrelated to any other loops.
 *
 * Note that basic_loop includes a net_loop, so you usually
 * never needs this.
 *
 * This is only needed under win32, but you can use it on all platforms.
 *
 * @author Aleksander Demko
 */
class scopira::tool::net_loop
{
  public:
#ifdef PLATFORM_win32
    /// ctor
    SCOPIRA_EXPORT net_loop(void);
    /// dtor
    SCOPIRA_EXPORT ~net_loop();
#else
    /// UNIX doesnt need these
    /// ctor
    SCOPIRA_EXPORT net_loop(void) { }
    /// dtor
    SCOPIRA_EXPORT ~net_loop() { }
#endif
};

/**
 * A UDP socket.
 *
 * @author Aleksander Demko
 */
class scopira::tool::udpflow : public virtual scopira::tool::object
{
  public:
    /// ctor
    SCOPIRA_EXPORT udpflow(void);
    /// ctor, on the given port, 0 for any
    SCOPIRA_EXPORT udpflow(int _port);
    /// dtor
    SCOPIRA_EXPORT virtual ~udpflow();

    /// in a failed state?
    bool failed(void) const { return dm_fail; }

    /// read raw block data, returns num read in
    /// src may be netaddr::any(). 
    /// actualsrc may be null, but if it isnt, it will be the actual src
    /// if the packet is bigger than the _maxsize, it will be trunced.
    /// if the packet is smaller than the _maxsize, the actual size is returned
    SCOPIRA_EXPORT size_t read(netaddr &actualsrc, int &actualport, byte_t* _buf, size_t _maxsize);
    /// write a raw block of data
    /// if return 0 and not failed, then your _size > MTU
    /// remember that OS's fragmentation system pushes the perceived MTU much higher
    SCOPIRA_EXPORT size_t write(netaddr dest, int destport, const byte_t* _buf, size_t _size);

    /// opens a udp socket, with the given port, 0 for any
    SCOPIRA_EXPORT void open(int _port);
    /// close the file
    SCOPIRA_EXPORT void close(void);

    /// is anything in the input buffer, within the given time? (false==timeout)
    /// 1000msec = 1 second
    SCOPIRA_EXPORT bool read_ready(int msec);

    /// gets the port
    int get_port(void) const { return dm_port; }

  private:
    int dm_port;    /// my port, 0 for none
    socket_handle_t dm_sock; /// the file handle
    bool dm_fail;
    bool dm_open;   /// is currently open?
};

#ifndef PLATFORM_win32 
/**
 * A "UNIX domain" socket.
 *
 * TODO This class has not been tested at all. It's implementation
 * is based on netflow and p372 of UNIX Systems Programming for SVR4
 *
 * @author Aleksander Demko
 */ 
class scopira::tool::unixflow : public scopira::tool::iflow_i, public scopira::tool::oflow_i
{
  protected:
    std::string dm_filename;
    bool dm_server, dm_open, dm_fail, dm_sfail;
    int dm_sock;

  public:
    /// default ctor
    unixflow(void);
    /// opening ctor
    unixflow(const std::string &filename, bool server_mode);
    /// dtor
    virtual ~unixflow();

    /// are we in a failed state?
    virtual bool failed(void) const;

    /// read raw block data, returns num read in
    virtual size_t read(byte_t* _buf, size_t _maxsize);
    /// write a raw block of data
    virtual size_t write(const byte_t* _buf, size_t _size);

    /// opens a client socket, null _addr for server socket
    void open(const std::string &filename, bool server_mode);
    /// close the file
    void close(void);

    /// (for server), opens the given connection to the new link
    /// returns true if a new linke came in, false on error
    bool accept(unixflow& nconn);

    /// is anything in the input buffer, within the given time? (false==timeout)
    /// 1000msec = 1 second
    bool read_ready(int msec);

    /// gets the address
    const std::string &get_filename(void) const { return dm_filename; }
    /// is this a server socket?
    bool is_server(void) const { return dm_server; }

  protected:
    /// internal, used by server ::accept
    void open_relay(int _fd, const std::string &filename);
};
#endif    //PLATFORM_win32

#endif
