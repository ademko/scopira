
/*
 *  Copyright (c) 2002-2004    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */
 
#include <scopira/tool/flow.h>

#include <stdio.h>

#include <scopira/tool/util.h>
#include <scopira/tool/output.h>
#include <scopira/tool/fileflow.h>

#ifdef PLATFORM_win32
// disable depreacted warnings
#pragma warning(disable:4996)
#endif

//BBtargets libscopira.so

using namespace scopira::tool;

oflow_i& operator<<(oflow_i& o, const char* val)
{
  if (val) {
    o.write(reinterpret_cast<const flow_i::byte_t*>(val), strlen(val));
  }
  return o;
}

oflow_i& operator<<(oflow_i& o, const std::string& val)
{
  o.write(reinterpret_cast<const flow_i::byte_t*>(val.c_str()), val.size());

  return o;
}

oflow_i& operator<<(oflow_i& o, char val)
{
  o.write(reinterpret_cast<const flow_i::byte_t*>(&val), 1);

  return o;
}

oflow_i& operator<<(oflow_i& o, bool val)
{
  return o << (val ? "true" : "false");
}

oflow_i& operator<<(oflow_i& o, int val)
{
  return o << int_to_string(val);
}

oflow_i& operator<<(oflow_i& o, long val)
{
  return o << long_to_string(val);
}

oflow_i& operator<<(oflow_i& o, double val)
{
  return o << double_to_string(val);
}

oflow_i& operator<<(oflow_i& o, unsigned int val)
{
  char buf[90];

  buf[0] = 0;
#ifdef PLATFORM_win32
  _snprintf(buf, 90, "%u", val);
#else
  snprintf(buf, 90, "%u", val);
#endif

  return o << buf;
}

oflow_i& operator<<(oflow_i& o, unsigned long val)
{
  char buf[90];

  buf[0] = 0;
#ifdef PLATFORM_win32
  _snprintf(buf, 90, "%lu", val);
#else
  snprintf(buf, 90, "%lu", val);
#endif

  return o << buf;
}

scopira::tool::oflow_i& operator<<(scopira::tool::oflow_i& o, long long val)
{
  char buf[90];

  buf[0] = 0;
#ifdef PLATFORM_win32
  _snprintf(buf, 90, "%lld", val);
#else
  snprintf(buf, 90, "%lld", val);
#endif

  return o << buf;
}

scopira::tool::oflow_i& operator<<(scopira::tool::oflow_i& o, unsigned long long val)
{
  char buf[90];

  buf[0] = 0;
#ifdef PLATFORM_win32
  _snprintf(buf, 90, "%llu", val);
#else
  snprintf(buf, 90, "%llu", val);
#endif

  return o << buf;
}

oflow_i& operator<<(oflow_i& o, iflow_i& i)
{
  iflow_i::byte_t buf[1024*8];
  size_t sz;

  while ( (sz = i.read(buf, 1024*8)) > 0)
    o.write(buf, sz);
  
  return o;
}

