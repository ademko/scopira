
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
 
#include <scopira/tool/printflow.h>

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>

#include <scopira/tool/util.h>

#ifdef PLATFORM_win32
// disable depreacted warnings
#pragma warning(disable:4996)
#endif

using namespace std;
using namespace scopira::tool;


printiflow::printiflow(bool doref, iflow_i* in)
  : dm_in(doref, 0)
{
  if (in)
    open(in);
}

/// destructor
printiflow::~printiflow(void)
{
  close();
}

bool printiflow::failed(void) const
{
  if (dm_in.get())
    return dm_in->failed();
  else
    return true;
}

/// read raw block data, returns num read in
size_t printiflow::read(byte_t* _buf, size_t _maxsize)
{
  assert(dm_in.get());
  // relay
  return dm_in->read(_buf, _maxsize);
}

bool printiflow::read_bool(bool& ret)
{
  if (failed())
    return false;

  char c;
  bool success = read_char(c);
  if (success)
    ret = (c == 'T');

  return success;
}

bool printiflow::read_char(char& ret)
{
  assert(dm_in.get());
  return dm_in->read(reinterpret_cast<byte_t*>(&ret), 1) > 0;
}

bool printiflow::read_short(short& ret)
{
  int x;

  if (!read_int(x))
    return false;

  ret = static_cast<short>(x);

  return true;
}

bool printiflow::read_int(int& ret)
{
  if (failed())
    return false;

  byte_t b = non_whitespace();
  std::string s(reinterpret_cast<char*>(&b), 1);
  while ( dm_in->read_byte(b)>0 && !is_whitespace(b))
    s.push_back(b);

  return string_to_int(s, ret);
}

bool printiflow::read_size_t(size_t& ret)
{
  if (failed())
    return false;

  byte_t b = non_whitespace();
  std::string s(reinterpret_cast<char*>(&b), 1);
  while ( dm_in->read_byte(b)>0 && !is_whitespace(b))
    s.push_back(b);

  return string_to_size_t(s, ret);
}

bool printiflow::read_int64_t(int64_t& ret)
{
  if (failed())
    return false;

  byte_t b = non_whitespace();
  std::string s(reinterpret_cast<char*>(&b), 1);
  while ( dm_in->read_byte(b)>0 && !is_whitespace(b))
    s.push_back(b);

  return string_to_int64_t(s, ret);
}

/// read an long
bool printiflow::read_long(long& ret)
{
  if (failed())
    return false;

  byte_t b = non_whitespace();
  std::string s(reinterpret_cast<char*>(&b), 1);
  while ( dm_in->read_byte(b)>0 && !is_whitespace(b))
    s.push_back(b);

  return string_to_long(s, ret);
}

bool printiflow::read_float(float& ret)
{
  double x;

  if (!read_double(x))
    return false;

  ret = static_cast<float>(x);

  return true;
}

bool printiflow::read_double(double& ret)
{
  if (failed())
    return false;

  byte_t b = non_whitespace();
  std::string s(reinterpret_cast<char*>(&b), 1);
  while ( dm_in->read_byte(b)>0 && !is_whitespace(b))
    s.push_back(b);

  return string_to_double(s, ret);
}

/// read a stl string
bool printiflow::read_string(string& out)
{
  if (failed())
    return false;

  assert(dm_in.get());

  out.clear();
  bool success = true;
  char c;
  while ( (success = read_char(c)) && (c != '\n') && (c != '\r') )
    out.append(&c, 1);
  if (c == '\r')  //Win32 file... read one more terminator
    success = read_char(c);

  return success;
}

bool printiflow::read_string_word(std::string &ret)
{
  char c;

  ret.clear();

  if (failed())
    return false;

  while (!failed() && read_char(c) && is_whitespace(c))
    ;//nothing

  while (!failed() && !is_whitespace(c)) {
    ret.append(&c, 1);
    read_char(c);
  }

  return true;
}

/// opens a new link
void printiflow::open(iflow_i* in)
{
  assert(in);
  close();
  dm_in.set(in);
}

/// close the current link
void printiflow::close(void)
{
  dm_in.set(0);
}

/// read a stl string
bool printiflow::read_string(string& out, int _max)
{
  if (failed())
    return false;

  assert(dm_in.get());

  out.clear();
  char c;
  bool success = true;
  for (int i=0; i<_max && success; i++) {
    success = read_char(c);
    out.append(&c, 1);
  }

  return success;
}

/// is it whitespace?
bool printiflow::is_whitespace(byte_t b)
{
  return isspace(b) != 0;
}

/// advance stream to next non whitespace and return it
printiflow::byte_t printiflow::non_whitespace(void)
{
  byte_t ret;

  ret = ' ';
  while (is_whitespace(ret) && !failed())
    dm_in->read_byte(ret);

  return ret;
}

printoflow::printoflow(bool doref, oflow_i* in)
  : dm_out(doref, 0)
{
  if (in)
    open(in);
}

/// destructor
printoflow::~printoflow(void)
{
  close();
}

bool printoflow::failed(void) const
{
  if (dm_out.get())
    return dm_out->failed();
  else
    return true;
}

/// write a raw block of data
size_t printoflow::write(const byte_t* _buf, size_t _size)
{
  assert(dm_out.get());

  // relay
  return dm_out->write(_buf, _size);
}

void printoflow::write_bool(bool val)
{
  write_char(val ? 'T' : 'F');
}

void printoflow::write_char(char val)
{
  dm_out->write(reinterpret_cast<const byte_t*>(&val), 1);
}

void printoflow::write_short(short val)
{
  write_int(val);
}

void printoflow::write_int(int val)
{
  write_string(int_to_string(val));
}

void printoflow::write_size_t(size_t val)
{
  write_string(size_t_to_string(val));
}

void printoflow::write_int64_t(int64_t val)
{
  write_string(int64_t_to_string(val));
}

void printoflow::write_long(long val)
{
  write_string(long_to_string(val));
}

void printoflow::write_float(float val)
{
  write_double(val);
}

void printoflow::write_double(double val)
{
  write_string(double_to_string(val));
}

/// write a STL string
void printoflow::write_string(const std::string& val)
{
  assert(dm_out.get());

  dm_out->write(reinterpret_cast<const byte_t*>(val.c_str()), val.size());
}

/// opens a new link
void printoflow::open(oflow_i* in)
{
  assert(in);
  close();
  dm_out.set(in);
}

/// close the current link
void printoflow::close(void)
{
  dm_out.set(0);
}

#ifdef PLATFORM_win32
#define vsnprintf _vsnprintf
#endif

/// printf style
void printoflow::printf(const char* fmt, ...)
{
  char buf[200], *b2;
  va_list ap;
  int len, max;

  assert(dm_out.get());

  // most of the time, my stack should suffice
  b2 = 0;
  max = 200;

  va_start(ap, fmt);
  len = vsnprintf(buf, max, fmt, ap);
  va_end(ap);

  if ( (len>=0) && (len<max) ) {
    // quick write, and exit
    dm_out->write(reinterpret_cast<const byte_t*>(buf), len);
    return;
  }

  while ( (len<0) || (len>=max) ) {
    max *= 4; // inc max
    delete []b2;    // delete previous, delete is safe on nulls remember
    b2 = new char[max];
    assert(b2);

    va_start(ap, fmt);
    len = vsnprintf(b2, max, fmt, ap);
    va_end(ap);
  }
  //write and clean
  dm_out->write(reinterpret_cast<const byte_t*>(b2), len);
  delete []b2;
}

/// printf style
void scopira::tool::flow_printf(oflow_i& o, const char* fmt, ...)
{
  char buf[200], *b2;
  va_list ap;
  int len, max;

  // most of the time, my stack should suffice
  b2 = 0;
  max = 200;

  va_start(ap, fmt);
  len = vsnprintf(buf, max, fmt, ap);
  va_end(ap);

  if ( (len>=0) && (len<max) ) {
    // quick write, and exit
    o.write(reinterpret_cast<const flow_i::byte_t *>(buf), len);
    return;
  }

  while ( (len<0) || (len>=max) ) {
    max *= 4; // inc max
    delete []b2;    // delete previous, delete is safe on nulls remember
    b2 = new char[max];
    assert(b2);

    va_start(ap, fmt);
    len = vsnprintf(b2, max, fmt, ap);
    va_end(ap);
  }
  //write and clean
  o.write(reinterpret_cast<const flow_i::byte_t*>(b2), len);
  delete []b2;
}

//BBlibs
//BBtargets libscopira.so

