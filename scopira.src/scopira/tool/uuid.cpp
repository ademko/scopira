
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

#include <scopira/tool/hexflow.h> // for hex stuff in non-lib implementation
#include <scopira/tool/output.h>

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
  : dm_core(time_seed())
{
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

  int *ids = reinterpret_cast<int*>(out.dm_id);

  assert(sizeof(int)*4 == sizeof(out.dm_id));

  ids[0] = dm_core();
  ids[1] = dm_core();
  ids[2] = dm_core();
  ids[3] = dm_core();
#endif
}

//
// misc
//

scopira::tool::oflow_i& operator << (scopira::tool::oflow_i& o, const scopira::tool::uuid &id)
{
  o << id.as_string();

  return o;
}

#if !defined(PLATFORM_E2UUID) && !defined(PLATFORM_win32)

uuid::uuid(void)
{
  set_zero();
}

void uuid::set_zero(void)
{
  dm_id[0] = 0;
  dm_id[1] = 0;
}

uuid::uuid(const char *s)
{
#ifndef NDEBUG
  bool b =
#endif
  parse_string(s);
  assert(b);
}

bool uuid::is_zero(void) const
{
  return dm_id[0] == 0 && dm_id[1] == 0;
}

bool uuid::operator == (const uuid &rhs) const
{
  return dm_id[0] == rhs.dm_id[0] && dm_id[1] == rhs.dm_id[1];
}

bool uuid::operator != (const uuid &rhs) const
{
  return dm_id[0] != rhs.dm_id[0] || dm_id[1] != rhs.dm_id[1];
}

bool uuid::operator < (const uuid &rhs) const
{
  return
    (dm_id[0] < rhs.dm_id[0]) ||
    (dm_id[0] == rhs.dm_id[0] && dm_id[1] < rhs.dm_id[1]);
}

bool uuid::load(scopira::tool::itflow_i& in)
{
  return
    in.read_int64_t(dm_id[0]) && in.read_int64_t(dm_id[1]);
}

void uuid::save(scopira::tool::otflow_i& out) const
{
  out.write_int64_t(dm_id[0]);
  out.write_int64_t(dm_id[1]);
}

std::string uuid::as_string(void) const
{
  std::string ret;
  const byte_t *data = reinterpret_cast<const byte_t *>(dm_id);

  ret.reserve(128/8+6);
  //ret.push_back('{');
  for (int i=0; i<(128/8); ++i) {
    ret.push_back(hexchars[data[i] >> 4]);
    ret.push_back(hexchars[data[i] & 0x0F]);
    if (i == 3 || i == 5 || i == 7 || i == 9)
      ret.push_back('-');
  }
  //ret.push_back('}');

  return ret;
}

bool uuid::parse_string(const std::string &s)
{
  byte_t *data = reinterpret_cast<byte_t *>(dm_id);
  int i = 0;
  int c = 0;
  bool inhex = false;
  byte_t b = 0;

  while (i < (128/8)) {
    if (c>=s.size())
      return false;

    // process the next char
    if (inhex) {
      inhex = false;
      if (is_hex(s[c])) {
        b |= char_to_hex(s[c]);
        // append it
        data[i] = b;
        i++;
      }
    } else {
      if (is_hex(s[c])) {
        b = char_to_hex(s[c]) << 4;
        inhex = true;
      }
      // else, do nothing
    }
    c++;
  }

  return true;
}

#endif

