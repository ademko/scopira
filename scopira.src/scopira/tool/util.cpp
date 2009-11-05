
/*
 *  Copyright (c) 2002-2007    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#include <scopira/tool/util.h>

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#ifdef PLATFORM_win32
// disable depreacted warnings
#pragma warning(disable:4996)
#endif

//BBlibs
//BBtargets libscopira.so

using std::string;
using namespace scopira;

std::string tool::int_to_string(int i)
{
  char buf[50];

  buf[0] = 0;
#ifdef PLATFORM_win32
  _snprintf(buf, 50, "%d", i);
#else
  snprintf(buf, 50, "%d", i);
#endif

  return buf;
}

int tool::int_to_string(int i, char *out, int buflen)
{
  int ret;
  assert(out);
  if (buflen>0) {
#ifdef PLATFORM_win32
    ret = _snprintf(out, buflen, "%d", i);
#else
    ret = snprintf(out, buflen, "%d", i);
#endif
    if (ret > buflen)
      return buflen;
    else
      return ret + 1;
  } else
    return 0;
}

int tool::string_to_int(const std::string &s)
{
  return atol(s.c_str());
}

bool tool::string_to_int(const std::string &s, int &outint)
{
  if (s.empty())
    return false;

  char *blah;

  outint = strtol(s.c_str(), &blah, 10);

  return *blah == '\0';
}

std::string tool::long_to_string(long i)
{
  char buf[90];

  buf[0] = 0;
#ifdef PLATFORM_win32
  _snprintf(buf, 90, "%ld", i);
#else
  snprintf(buf, 90, "%ld", i);
#endif

  return buf;
}

long tool::string_to_long(const std::string &s)
{
  return atol(s.c_str());
}

bool tool::string_to_long(const std::string &s, long &outlong)
{
  if (s.empty())
    return false;

  char *blah;

  outlong = strtol(s.c_str(), &blah, 10);

  return *blah == '\0';
}

std::string tool::size_t_to_string(size_t i)
{
  char buf[90];

  buf[0] = 0;
#ifdef PLATFORM_win32
  _snprintf(buf, 90, "%Iu", i);
#else
  snprintf(buf, 90, "%zu", i);
#endif

  return buf;
}

size_t tool::string_to_size_t(const std::string &s)
{
  return atol(s.c_str());
}

bool tool::string_to_size_t(const std::string &s, size_t &outsize_t)
{
  if (s.empty())
    return false;

  char *blah;

  outsize_t = strtol(s.c_str(), &blah, 10);

  return *blah == '\0';
}

std::string tool::int64_t_to_string(int64_t i)
{
  char buf[90];

  buf[0] = 0;
#ifdef PLATFORM_win32
  _snprintf(buf, 90, "%Iu", i);
#else
#ifdef PLATFORM_64
  snprintf(buf, 90, "%ld", i);
#else
  snprintf(buf, 90, "%lld", i);
#endif
#endif

  return buf;
}

int64_t tool::string_to_int64_t(const std::string &s)
{
#ifdef PLATFORM_win32
  return _atoi64(s.c_str());
#else
  return atoll(s.c_str());
#endif
}

bool tool::string_to_int64_t(const std::string &s, int64_t &outint64_t)
{
  if (s.empty())
    return false;

  char *blah;

#ifdef PLATFORM_win32
  outint64_t = _strtoi64(s.c_str(), &blah, 10);
#else
  outint64_t = strtoll(s.c_str(), &blah, 10);
#endif

  return *blah == '\0';
}

std::string tool::double_to_string(double i)
{
  char buf[50];

  buf[0] = 0;
#ifdef PLATFORM_win32
  _snprintf(buf, 50, "%g", i);
#else
  snprintf(buf, 50, "%g", i);
#endif

  return buf;
}

std::string tool::double_to_string(double i, int pres)
{
  char buf[50];

  buf[0] = 0;
#ifdef PLATFORM_win32
  _snprintf(buf, 50, "%0.*f",pres, i);
#else
  snprintf(buf, 50, "%0.*f",pres, i);
#endif

  return buf;
}

int tool::double_to_string(double i, char *out, int buflen)
{
  int ret;
  assert(out);
  if (buflen>0) {
#ifdef PLATFORM_win32
    ret = _snprintf(out, buflen, "%g", i);
#else
    ret = snprintf(out, buflen, "%g", i);
#endif
    if (ret > buflen)
      return buflen;
    else
      return ret + 1;
  } else
    return 0;
}

std::string tool::double_to_exp_string(double i, int pres)
{
  char buf[40];

  buf[0] = 0;
#ifdef PLATFORM_win32
  _snprintf(buf, 40, "%.*e",pres, i);
#else
  snprintf(buf, 40, "%.*e",pres, i);
#endif

  return buf;
}

double tool::string_to_double(const std::string &s)
{
  return atof(s.c_str());
}

bool tool::string_to_double(const std::string &s, double &outdouble)
{
  if (s.empty())
    return false;

  char *blah;

  outdouble = strtod(s.c_str(), &blah);

  return *blah == '\0';
}

void tool::uppercase(const std::string& s, std::string& upper)
{
  upper.resize(s.size());
  for (std::string::size_type i=0; i<s.size(); i++)
    upper[i] = static_cast<std::string::value_type>(toupper(s[i]));
}

std::string tool::uppercase(const std::string& s)
{
  std::string upper;

  upper.resize(s.size());
  for (std::string::size_type i=0; i<s.size(); i++)
    upper[i] = static_cast<std::string::value_type>(toupper(s[i]));
  std::string ret;

  return upper;
}

void tool::lowercase(const std::string& s, std::string& lower)
{
  lower.resize(s.size());
  for (std::string::size_type i=0; i<s.size(); i++)
    lower[i] = static_cast<std::string::value_type>(tolower(s[i]));
}

std::string tool::lowercase(const std::string& s)
{
  std::string lower;

  lower.resize(s.size());
  for (std::string::size_type i=0; i<s.size(); i++)
    lower[i] = static_cast<std::string::value_type>(tolower(s[i]));

  return lower;
}

/// justify strings
std::string tool::pad_left(const std::string &s, int width, char padchar)
{
  int diff, i;

  diff = width - s.size();
  if (diff <= 0)
    return s;
  else {
    string ret("");

    for (i=0; i<diff; i++)
      ret.append(&padchar, 1);
    return ret + s;
  }
}

void tool::impl_cstring_to_fixed_array_impl(const char *in, size_t srclen, char *out, size_t N) throw()
{
  assert(N>0);

  if (!in) {
    out[N] = 0;
    return;
  }

  size_t i;
  size_t len = srclen;

  if (len >= N)
    len = N - 1;

  for (i=0; i<len; ++i)
    out[i] = in[i];
  out[len] = 0;
}

bool tool::split_char(const std::string &src, char split,
    std::string &left_out, std::string &right_out)
{
  string::size_type sz;

  sz = src.find(split);

  // didnt find char
  if (sz == string::npos)
    return false;

  left_out.assign(src, 0, sz);
  right_out.assign(src, sz+1, src.size() - sz - 1);

  return true;
}


string tool::filename_number(const string &fname, int num)
{
  unsigned int i, numr;
  string s, ret;
  const char *u;

  // count # in fname
  numr = 0;
  for (i=0; i<fname.length(); i++)
    if (fname[i] == '#')
      numr++;

  // no # in name, just return
  if (numr == 0)
    return fname;

  // do the replace
  s = pad_left(int_to_string(num), numr, '0').c_str();
  u = s.c_str();
  ret = fname;
  numr = 0;
  for (i=0; i<ret.length(); i++)
    if (ret[i] == '#')
      ret[i] = u[numr++];

  return ret;
}

void tool::trim_left(std::string& str)
{
  // trim leading whitespace
  str.erase(0, str.find_first_not_of(" \t\n"));
}

void tool::trim_right(std::string& str)
{
  // trim trailing whitespace
  str.erase(str.find_last_not_of(" \t\n")+1);
}

