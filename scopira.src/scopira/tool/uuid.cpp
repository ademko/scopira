
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

#include <scopira/tool/uuid.h>

#ifdef PLATFORM_win32
// disable depreacted warnings
#pragma warning(disable:4996)
#endif

//BBlibs
//BBtargets libscopira.so

using namespace scopira::tool;

//
//
// uuid
//
//

#ifdef PLATFORM_win32
bool uuid::operator < (const uuid &rhs) const
{
  const uint8_t *L = reinterpret_cast<const uint8_t*>(&dm_id);
  const uint8_t *R = reinterpret_cast<const uint8_t*>(&rhs.dm_id);

  for (int x=0; x<16; ++x) {
    if (L[x] < R[x])
      return true;
    if (L[x] > R[x])
      return false;
  }
  return false; // all equal
}
#endif

#ifdef PLATFORM_E2UUID
uuid::uuid(const char *s)
{
#ifndef NDEBUG
  bool b =
#endif
  parse_string(s);
  assert(b);
}
#endif

#ifdef PLATFORM_E2UUID
std::string uuid::as_string(void) const
{
  char s[40];
  //::uuid_unparse_lower(dm_id, s);
  ::uuid_unparse(dm_id, s);

  return s;
}
#endif

#ifdef PLATFORM_E2UUID
bool uuid::parse_string(const std::string &s)
{
  return uuid_parse(s.c_str(), dm_id) == 0;
}
#endif

#ifdef PLATFORM_win32
uuid::uuid(const char *s)
{
#ifndef NDEBUG
  bool b =
#endif
  parse_string(s);
  assert(b);
}
#endif

#ifdef PLATFORM_win32
std::string uuid::as_string(void) const
{
  char s[40];
  const uint8_t *a = reinterpret_cast<const uint8_t*>(&dm_id);
  
  assert(sizeof(uint8_t)*16 == sizeof(GUID));

  _snprintf(s, 40, "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
    a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7],
    a[8], a[9], a[10], a[11], a[12], a[13], a[14], a[15]);

  return s;
}
#endif

#ifdef PLATFORM_win32
bool uuid::parse_string(const std::string &s)
{
  uint16_t b[16];
  uint8_t *a = reinterpret_cast<uint8_t*>(&dm_id);
  
  if (16 != sscanf(s.c_str(),
    "%02hx%02hx%02hx%02hx-%02hx%02hx-%02hx%02hx-%02hx%02hx-%02hx%02hx%02hx%02hx%02hx%02hx",
    &b[0], &b[1], &b[2], &b[3], &b[4], &b[5], &b[6], &b[7],
    &b[8], &b[9], &b[10], &b[11], &b[12], &b[13], &b[14], &b[15]))
    return false;
  
  for (int i=0; i<16; ++i)
    a[i] = static_cast<uint8_t>(b[i]);

  return true;
}
#endif

//
//
// uuid_generator
//
//

#if !defined(PLATFORM_E2UUID) && !defined(PLATFORM_win32)
uuid_generator::uuid_generator(void)
{
  dm_next = time_seed();    // "random". hah!
}
#endif

void uuid_generator::next(uuid &out)
{
  assert(sizeof(uuid) == 16);
#ifdef PLATFORM_E2UUID
  uuid_generate(out.dm_id);       // doesnt really need a this instance
#elif defined(PLATFORM_win32)
  CoCreateGuid(&out.dm_id);
#else
  locker L(dm_mut);

  out.dm_id = dm_next++;
#endif
}

//
// misc
//

scopira::tool::oflow_i& operator << (scopira::tool::oflow_i& o, const scopira::tool::uuid &id)
{
#ifdef PLATFORM_E2UUID
  char buf[64]; // string is actually only 36 bytes
  uuid_unparse(id.dm_id, buf);
  o << buf;
#elif defined(PLATFORM_win32)
  o << id.as_string();
#else
  o << id.dm_id;
#endif
  return o;
}

