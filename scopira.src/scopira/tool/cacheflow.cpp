
/*
 *  Copyright (c) 2003    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */
 
#include <scopira/tool/cacheflow.h>

#include <assert.h>

using namespace scopira::tool;

//
//
// cacheiflow
//
//

cacheiflow::cacheiflow(bool doref, iflow_i* innie, size_t buffersize)
  : dm_in(doref, 0), dm_cache(buffersize)
{
  assert(buffersize>1);

  if (innie)
    open(innie);
}

size_t cacheiflow::read(byte_t* _buf, size_t _maxsize)
{
  if ((_maxsize == 0) || (dm_failed && dm_cache.empty()))
    return 0;
 
  // read what we can from the cache
  byte_t * N;
  size_t togo;
  
  if (dm_cache.empty())
    N = _buf;
  else
    N = dm_cache.pop_front(_buf, _buf+_maxsize);

  if (N == _buf+_maxsize)
    return _maxsize;    // we have enough.. thanks mr cache guy

  // still some unfinished bidnez
  togo = _maxsize - (N - _buf);
  if (togo < dm_cache.capacity()/4) {
    // load the next into cache, and pop it from there
    load_cache();

    if (dm_cache.empty())   // failure
      return N - _buf;

    N = dm_cache.pop_front(N, _buf+_maxsize);

    return N - _buf;
  } else {
    // item is huge, read it direct
    togo = dm_in->read(N, togo);
    if (dm_in->failed())
      dm_failed = true;

    return (N - _buf) + togo;
  }
}

size_t cacheiflow::read_byte(byte_t &out)
{
  if (dm_cache.empty())
    load_cache();

  if (dm_cache.empty())
    return 0;
  else {
    out = dm_cache.front();
    dm_cache.pop_front();
    return 1;
  }
}

void cacheiflow::open(iflow_i* in)
{
  assert(in);
  dm_in = in;

  dm_failed = false;
  dm_cache.clear();
}

void cacheiflow::close(void)
{
  dm_in = 0;
  dm_failed = true;
}

void cacheiflow::short_rewind(void)
{
  dm_cache.short_rewind();
}

void cacheiflow::load_cache(void)
{
  size_t r;

  assert(dm_cache.empty());

  if (dm_failed)
    return;

  dm_cache.clear();   // it can be empty, but not cleared (reset) force clear

  if (dm_in->failed()) {
    dm_failed = true;
    return;
  }

  // do some black magic here so we can load DIRECT into the cache
  r = dm_in->read(dm_cache.begin1(), dm_cache.capacity());

  if (dm_in->failed())
    dm_failed = true;

  // whacky!
  dm_cache.push_seek(r);
}

//
//
// cacheoflow
//
//

cacheoflow::cacheoflow(bool doref, oflow_i* out, size_t buffersize)
  : dm_out(doref, 0), dm_cache(buffersize)
{
  assert(buffersize>1);

  if (out)
    open(out);
}


cacheoflow::~cacheoflow()
{
  flush_cache();
}

size_t cacheoflow::write(const byte_t* _buf, size_t _size)
{
  // see if we need to flush what we have so far
  if (_size == 0 || dm_failed)
    return 0;
  // is it bigger than the amount of free space we have?
  if (_size > dm_cache.free()) {
    flush_cache();
 
    // is to "too big" to nicely store in our cache at all?
    if (_size > dm_cache.capacity()/4) {
      size_t ret = dm_out->write(_buf, _size);
      if (dm_out->failed())
        dm_failed = true;
      return ret;
    }
  }
  // otherwise, add it to the end
  dm_cache.push_back(_buf, _buf+_size);
  return _size;
}

size_t cacheoflow::write_byte(byte_t b)
{
  if (dm_cache.full())
    flush_cache();

  dm_cache.push_back(b);

  return 1;
}

void cacheoflow::open(oflow_i* out)
{
  assert(out);
  dm_out = out;

  dm_failed = out->failed();
  dm_cache.clear();
}

void cacheoflow::close(void)
{
  flush_cache();
  dm_out = 0;
  dm_failed = true;
}

void cacheoflow::flush_cache(void)
{
  circular_vector<byte_t>::const_iterator ii, endii;
  size_t x;

  if (dm_failed || dm_cache.empty())
    return;

  ii = dm_cache.begin1();
  endii = dm_cache.end1();
  x = endii - ii;

  if (x>0) {
    dm_out->write(ii, x);
    if (dm_out->failed()) {
      dm_cache.pop_all();
      dm_failed = true;
      return;
    }
  }

  ii = dm_cache.begin2();
  endii = dm_cache.end2();
  x = endii - ii;

  if (x>0) {
    dm_out->write(ii, x);
    if (dm_out->failed()) {
      dm_cache.pop_all();
      dm_failed = true;
      return;
    }
  }

  dm_cache.pop_all();
}


//BBlibs
//BBtargets libscopira.so

