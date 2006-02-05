
/*
 *  Copyright (c) 2004    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#include <scopira/tool/url.h>

#include <scopira/tool/fileflow.h>
#include <scopira/tool/util.h>

//BBlibs
//BBtargets libscopira.so

using namespace scopira::tool;

url::url(void)
{
  clear_url();
}

url::url(const std::string &fullurl)
{
  set_url(fullurl);
}

url::url(const std::string &hostname, int port)
{
  set_url(hostname, port);
}

url::url(const std::string &proto, const std::string &hostname, int port, const std::string &filename)
{
  set_url(proto, hostname, port, filename);
}

url::~url()
{
  // nothin
}

bool url::load(scopira::tool::itflow_i& in)
{
  std::string s;

  s.reserve(128);

  return in.read_string(s) && set_url(s);
}

void url::save(scopira::tool::otflow_i& out) const
{
  out.write_string(dm_full);
}

void url::clear_url(void)
{
  set_url("scopira://localhost/");
}

bool url::set_url(const std::string &fullurl)
{
  std::string proto, host, port, filename;
  int portint;
  std::string::size_type s, c;

  // try to parse the url

  // grab the proto
  s = 0;
  for (c=0; c<fullurl.size(); ++c)
    if (fullurl[c] == ':') {
      proto = fullurl.substr(s, c-s);
      break;
    }

  if (c == fullurl.size())
    return false;

  // we're at the :
  ++c;
  if (c == fullurl.size() || fullurl[c] != '/')
    return false;
  ++c;
  if (c == fullurl.size() || fullurl[c] != '/')
    return false;
  ++c;

  // grab the hostname
  s = c;
  while (c<fullurl.size() && fullurl[c] != ':' && fullurl[c] != '/')
    ++c;
  host = fullurl.substr(s, c-s);

  // port processing, if any
  if (c<fullurl.size() && fullurl[c] == ':') {
    // find the end of the port spec
    ++c;
    s = c;
    while (c<fullurl.size() && fullurl[c] != '/')
      ++c;

    port = fullurl.substr(s, c-s);
    if (!string_to_int(port, portint))
      return false;   // failed to decode an int
  } else
    portint = 0; // a default

  // finally, scoop up the remaining stuff
  ++c;
  if (c < fullurl.size())
    filename = fullurl.substr(c);

  // ALL IS WELL... copy the stuff over
  set_url(proto, host, portint, filename);

  return true;
}

void url::set_url(const std::string &hostname, int port)
{
  set_url("scopira", hostname, port, "");
}

void url::set_url(const std::string &proto, const std::string &hostname, int port, const std::string &filename)
{
  std::string portstring = int_to_string(port);

  dm_full = proto + "://" + hostname + ":" + portstring + "/" + filename;

  dm_proto_begin = 0;
  dm_proto_end = dm_proto_begin + proto.size();
  dm_host_begin = dm_proto_end + 3;
  dm_host_end = dm_host_begin + hostname.size();
  dm_port = port;
  dm_file_begin = dm_host_end + portstring.size() + 2;
  dm_file_end = dm_file_begin + filename.size();
}

std::string url::get_proto(void) const
{
  return dm_full.substr(dm_proto_begin, dm_proto_end - dm_proto_begin); 
}

std::string url::get_hostname(void) const
{
  return dm_full.substr(dm_host_begin, dm_host_end - dm_host_begin); 
}

std::string url::get_filename(void) const
{
  return dm_full.substr(dm_file_begin, dm_file_end - dm_file_begin); 
}

scopira::tool::oflow_i& operator<<(scopira::tool::oflow_i& o, const scopira::tool::url& u)
{
  return o << u.get_url();
}

