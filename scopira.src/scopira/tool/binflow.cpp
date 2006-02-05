
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
 
#include <scopira/tool/binflow.h>

#include <assert.h>

#include <scopira/tool/platform.h>
#include <scopira/tool/util.h>

//BBlibs
//BBtargets libscopira.so

using namespace std;
using namespace scopira::tool;

biniflow::biniflow(bool doref, iflow_i* in)
  : dm_in(doref, 0)
{
  if (in)
    open(in);
}

/// destructor
biniflow::~biniflow(void)
{
  close();
}

bool biniflow::failed(void) const
{
  if (dm_in.get())
    return dm_in->failed();
  else
    return true;
}

/// read raw block data, returns num read in
size_t biniflow::read(byte_t* _buf, size_t _maxsize)
{
  assert(dm_in.get());
  return dm_in->read(_buf, _maxsize);
}

bool biniflow::read_bool(bool& ret)
{
  if (failed())
    return false;

  assert(dm_in.get());

  dm_in->read(reinterpret_cast<byte_t*>(&ret), sizeof(ret));

  return true;
}

bool biniflow::read_char(char& ret)
{
  if (failed())
    return false;

  assert(dm_in.get());

  dm_in->read(reinterpret_cast<byte_t*>(&ret), sizeof(ret));

  return true;
}

bool biniflow::read_short(short& ret)
{
  if (failed())
    return false;

  assert(dm_in.get());

  dm_in->read(reinterpret_cast<byte_t*>(&ret), sizeof(ret));
#ifdef PLATFORM_BYTESWAP
  ret = byte_swap<short>(ret);
#endif

  return true;
}

bool biniflow::read_int(int& ret)
{
  if (failed())
    return false;

  assert(dm_in.get());

  dm_in->read(reinterpret_cast<byte_t*>(&ret), sizeof(ret));
#ifdef PLATFORM_BYTESWAP
  ret = byte_swap<int>(ret);
#endif

  return true;
}

bool biniflow::read_size_t(size_t& ret)
{
  if (failed())
    return false;

  assert(dm_in.get());

  dm_in->read(reinterpret_cast<byte_t*>(&ret), sizeof(ret));
#ifdef PLATFORM_BYTESWAP
  ret = byte_swap<size_t>(ret);
#endif

  return true;
}

bool biniflow::read_long(long& ret)
{
  if (failed())
    return false;

  assert(dm_in.get());

  dm_in->read(reinterpret_cast<byte_t*>(&ret), sizeof(ret));
#ifdef PLATFORM_BYTESWAP
  ret = byte_swap<long>(ret);
#endif

  return true;
}

bool biniflow::read_float(float& ret)
{
  if (failed())
    return false;

  assert(dm_in.get());
  
  dm_in->read(reinterpret_cast<byte_t*>(&ret), sizeof(ret));

  return true;
}

bool biniflow::read_double(double& ret)
{
  if (failed())
    return false;

  assert(dm_in.get());
  
  dm_in->read(reinterpret_cast<byte_t*>(&ret), sizeof(ret));

  return true;
}

/// read a stl string
bool biniflow::read_string(string& out)
{
  if (failed())
    return false;

  assert(dm_in.get());

  int len;
  if (!read_int(len) || len < 0) {
    assert(len >= 0);
    return false;
  }

  // clear and resize out string
  out.resize(len);
  // read contents into string
  dm_in->read(reinterpret_cast<byte_t*>(&out[0]), len);

  return true;
}


/// opens a new link
void biniflow::open(iflow_i* in)
{
  assert(in);
  close();
  dm_in.set(in);
}

/// close the current link
void biniflow::close(void)
{
  dm_in.set(0);
}

binoflow::binoflow(bool doref, oflow_i* in)
  : dm_out(doref, 0)
{
  if (in)
    open(in);
}

/// destructor
binoflow::~binoflow(void)
{
  close();
}

bool binoflow::failed(void) const
{
  if (dm_out.get())
    return dm_out->failed();
  else
    return true;
}

/// write a raw block of data
size_t binoflow::write(const byte_t* _buf, size_t _size)
{
  assert(dm_out.get());
  return dm_out->write(_buf, _size);
}

void binoflow::write_bool(bool val)
{
  assert(dm_out.get());
  dm_out->write(reinterpret_cast<const byte_t*>(&val), sizeof(val));
}

void binoflow::write_char(char val)
{
  assert(dm_out.get());
  dm_out->write(reinterpret_cast<const byte_t*>(&val), sizeof(val));
}

void binoflow::write_short(short val)
{
  assert(dm_out.get());
#ifdef PLATFORM_BYTESWAP
  val = byte_swap<short>(val);
#endif
  dm_out->write(reinterpret_cast<const byte_t*>(&val), sizeof(val));
}

void binoflow::write_int(int val)
{
  assert(dm_out.get());
#ifdef PLATFORM_BYTESWAP
  val = byte_swap<int>(val);
#endif
  dm_out->write(reinterpret_cast<const byte_t*>(&val), sizeof(val));
}

void binoflow::write_size_t(size_t val)
{
  assert(dm_out.get());
#ifdef PLATFORM_BYTESWAP
  val = byte_swap<size_t>(val);
#endif
  dm_out->write(reinterpret_cast<const byte_t*>(&val), sizeof(val));
}

void binoflow::write_long(long val)
{
  assert(dm_out.get());
#ifdef PLATFORM_BYTESWAP
  val = byte_swap<long>(val);
#endif
  dm_out->write(reinterpret_cast<const byte_t*>(&val), sizeof(val));
}

void binoflow::write_float(float val)
{
  assert(dm_out.get());
  dm_out->write(reinterpret_cast<const byte_t*>(&val), sizeof(val));
}

void binoflow::write_double(double val)
{
  assert(dm_out.get());
  dm_out->write(reinterpret_cast<const byte_t*>(&val), sizeof(val));
}

/// write a STL string
void binoflow::write_string(const std::string& val)
{
  assert(dm_out.get());
  write_int(val.size());
  if (val.size() > 0)
    dm_out->write(reinterpret_cast<const byte_t*>(val.c_str()), val.size());
}

/// opens a new link
void binoflow::open(oflow_i* in)
{
  assert(in);
  close();
  dm_out.set(in);
}

/// close the current link
void binoflow::close(void)
{
  dm_out.set(0);
}

bin64iflow::bin64iflow(bool doref, iflow_i* in)
  : biniflow(doref, in)
{
}

bool bin64iflow::read_size_t(size_t &ret)
{
  if (failed())
    return false;

  assert(dm_in.get());

  // 32 bit code work around
  uint64_t bigval;

  dm_in->read(reinterpret_cast<byte_t*>(&bigval), sizeof(bigval));
#ifdef PLATFORM_BYTESWAP
  bigval = byte_swap<uint64>(bigval);
#endif
  ret = static_cast<size_t>(bigval);
  return true;
}

bool bin64iflow::read_long(long &ret)
{
  if (failed())
    return false;

  assert(dm_in.get());

  // 32 bit code work around
  int64_t bigval;

  dm_in->read(reinterpret_cast<byte_t*>(&bigval), sizeof(bigval));
#ifdef PLATFORM_BYTESWAP
  bigval = byte_swap<int64_t>(bigval);
#endif
  ret = static_cast<long>(bigval);
  return true;
}

bin64oflow::bin64oflow(bool doref, oflow_i* in)
  : binoflow(doref, in)
{
}

void bin64oflow::write_size_t(size_t val)
{
  assert(dm_out.get());
  // 32 bit work around
  uint64_t bigval = val;
#ifdef PLATFORM_BYTESWAP
  bigval = byte_swap<uint64_t>(bigval);
#endif
  dm_out->write(reinterpret_cast<const byte_t*>(&bigval), sizeof(bigval));
}

void bin64oflow::write_long(long val)
{
  assert(dm_out.get());
  // 32 bit work around
  int64_t bigval = val;
#ifdef PLATFORM_BYTESWAP
  bigval = byte_swap<int64_t>(bigval);
#endif
  dm_out->write(reinterpret_cast<const byte_t*>(&bigval), sizeof(bigval));
}

