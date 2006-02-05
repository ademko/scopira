
/*
 *  Copyright (c) 2002    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */
 
#include <scopira/tool/textflow.h>

#include <assert.h>
#include <ctype.h>

#include <scopira/tool/util.h>

//BBlibs
//BBtargets libscopira.so

using namespace std;
using namespace scopira::tool;

textiflow::textiflow(bool doref, iflow_i* in)
  : dm_in(doref, 0)
{
  open(in);
}

/// destructor
textiflow::~textiflow(void)
{
  close();
}

bool textiflow::failed(void) const
{
  if (dm_in.get())
    return dm_in->failed();
  else
    return true;
}

/// read raw block data, returns num read in
size_t textiflow::read(byte_t* _buf, size_t _maxsize)
{
  int len;

  if (failed())
    return 0;

  assert(dm_in.get());

  if (!read_int(len) || len < 0 || len > static_cast<int>(_maxsize))
    return 0;

  // read_int advanced past ' ', read string now
  return dm_in->read(_buf, len);
}

bool textiflow::read_bool(bool& ret)
{
  if (failed())
    return false;

  char c;
  bool success;
  if (success = read_char(c))
    ret = (c == 'T');

  return success;
}

/// reads a char
bool textiflow::read_char(char& ret)
{
  assert(dm_in.get());
  return dm_in->read(reinterpret_cast<byte_t*>(&ret), 1) > 0;
}

bool textiflow::read_short(short& ret)
{
  int x;

  if (!read_int(x))
    return false;

  ret = static_cast<short>(x);

  return true;
}

bool textiflow::read_int(int& ret)
{
  if (failed())
    return false;

  byte_t b = non_whitespace();
  std::string s(reinterpret_cast<char*>(&b), 1);
  while ( dm_in->read_byte(b)>0 && !is_whitespace(b))
    s.push_back(b);

  return string_to_int(s, ret);
}

bool textiflow::read_size_t(size_t& ret)
{
  if (failed())
    return false;

  byte_t b = non_whitespace();
  std::string s(reinterpret_cast<char*>(&b), 1);
  while ( dm_in->read_byte(b)>0 && !is_whitespace(b))
    s.push_back(b);

  return string_to_size_t(s, ret);
}

bool textiflow::read_long(long& ret)
{
  if (failed())
    return false;

  byte_t b = non_whitespace();
  std::string s(reinterpret_cast<char*>(&b), 1);
  while ( dm_in->read_byte(b)>0 && !is_whitespace(b))
    s.push_back(b);

  return string_to_long(s, ret);
}

bool textiflow::read_float(float& ret)
{
  double x;

  if (!read_double(x))
    return false;

  ret = static_cast<float>(x);

  return true;
}

bool textiflow::read_double(double& ret)
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
bool textiflow::read_string(string& out)
{
  if (failed())
    return false;

  assert(dm_in.get());

  int len;
  if (!read_int(len) || len < 0)
    return false;

  // clear and resize out string
  out.clear();
  out.resize(len);
  // read data straight into out string
  dm_in->read(reinterpret_cast<byte_t*>(&out[0]), len);

  return true;
}

/// opens a new link
void textiflow::open(iflow_i* in)
{
  assert(in);
  close();
  dm_in.set(in);
}

/// close the current link
void textiflow::close(void)
{
  dm_in.set(0);
}

/// is it whitespace?
bool textiflow::is_whitespace(byte_t b)
{
  return isspace(b) != 0;
}

/// advance stream to next non whitespace and return it
textiflow::byte_t textiflow::non_whitespace(void)
{
  byte_t ret;

  ret = ' ';
  while (is_whitespace(ret) && !failed())
    dm_in->read_byte(ret);

  return ret;
}

textoflow::textoflow(bool doref, oflow_i* out)
  : dm_out(doref, 0)
{
  open(out);
}

/// destructor
textoflow::~textoflow(void)
{
  close();
}

bool textoflow::failed(void) const
{
  if (dm_out.get())
    return dm_out->failed();
  else
    return true;
}

/// write a raw block of data
size_t textoflow::write(const byte_t* _buf, size_t _size)
{
  assert(dm_out.get());

  dm_out->write_byte('\n');

  write_int(_size);

  if (_size > 0) {
    dm_out->write(_buf, _size);
    dm_out->write_byte(' ');
  }

  return _size;
}

void textoflow::write_bool(bool val)
{
  write_char(val ? 'T' : 'F');
}

void textoflow::write_char(char val)
{
  dm_out->write(reinterpret_cast<const byte_t*>(&val), 1);
}

void textoflow::write_short(short val)
{
  write_int(val);
}

void textoflow::write_int(int val)
{
  std::string s;

  assert(dm_out.get());
  s = int_to_string(val) + " ";

  dm_out->write(reinterpret_cast<const byte_t*>(s.c_str()), s.size());
}

void textoflow::write_size_t(size_t val)
{
  std::string s;

  assert(dm_out.get());
  s = size_t_to_string(val) + " ";

  dm_out->write(reinterpret_cast<const byte_t*>(s.c_str()), s.size());
}

void textoflow::write_long(long val)
{
  std::string s;

  assert(dm_out.get());
  s = long_to_string(val) + " ";

  dm_out->write(reinterpret_cast<const byte_t*>(s.c_str()), s.size());
}

void textoflow::write_float(float val)
{
  write_double(val);
}

void textoflow::write_double(double val)
{
  std::string s;

  assert(dm_out.get());
  s = double_to_string(val) + " ";

  dm_out->write(reinterpret_cast<const byte_t*>(s.c_str()), s.size());
}

/// write a STL string
void textoflow::write_string(const std::string& val)
{
  assert(dm_out.get());

  dm_out->write_byte('\n');

  write_int(val.size());

  if (val.size() > 0) {
    dm_out->write(reinterpret_cast<const byte_t*>(val.c_str()), val.size());
    dm_out->write_byte(' ');
  }
}


/// opens a new link
void textoflow::open(oflow_i* out)
{
  assert(out);
  close();
  dm_out.set(out);
}

/// close the current link
void textoflow::close(void)
{
  dm_out.set(0);
}

#ifdef SCO_DEBUG_textflow
#include <scopira/tool/fileflow.h>
using namespace scopira::tool;
int main(void)
{
#ifndef NDEBUG
  objrefcounter _duh;
#endif
  textoflow tf(true);
  textiflow inf(true);
  char cbuf[100];
  
  fileflow cerr(fileflow::stderr_c, 0);
    
  cerr << "start\n";
  
  tf.open(new fileflow("out.text", fileflow::output_c));
  tf.write_int(5);
  tf.write_int(666);
  tf.write_double(4.1);
  tf.write_string("Aleksander");
  tf.write_int(500);
  tf.write_binary(reinterpret_cast<const flow_i::byte_t*>("Haha"), 4);
  tf.write_double(3.13444);
  tf.write_bool(true);
  tf.write_float(10.10);
  tf.write_char('X');
  tf.close();
  
  inf.open(new fileflow("out.text", fileflow::input_c));
  int i;
  if (inf.read_int(i))
    cerr << "int 5: " << i << '\n';
  if (inf.read_int(i))
    cerr << "int 666: " << i << '\n';
  double d;
  if (inf.read_double(d))
    cerr << "double 4.1: " << d << '\n';
  string sss;
  if (inf.read_string(sss))
    cerr << "string Aleksander: " << sss << '\n';
  if (inf.read_int(i))
    cerr << "int 500: " << i << '\n';
  
  i = inf.read_binary(reinterpret_cast<flow_i::byte_t*>(cbuf), 100);
  cbuf[i] = 0; // append null
  cerr << "char[] Haha: " << cbuf << '\n';
  
  if (inf.read_double(d))
    cerr << "double 3.13444: " << d << '\n';
  bool b;
  if (inf.read_bool(b))
    cerr << "bool true: " << (b?"true":"false") << '\n';
  float f;
  if (inf.read_float(f))
    cerr << "float 10.10: " << f << '\n';
  char c;
  if (inf.read_char(c))
    cerr << "char X: " << c << '\n';

  tf.close();
  
  cerr << "end\n";
  
  return 0;
}
#endif

