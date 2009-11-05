
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
 
#include <scopira/tool/bufferflow.h>

#include <assert.h>
#include <string.h>

using namespace scopira::tool;

bufferflow::bufferflow(void)
  : dm_index(0)
{
  // reserve an initial number of elements to reduce the number
  // of times the vector has to be resized.
  dm_buffer.reserve(1024);
}

/// destructor
bufferflow::~bufferflow(void)
{
}

bool bufferflow::failed(void) const
{
  return false;
}

/// read raw block data, returns num read in
size_t bufferflow::read(byte_t* buf, size_t maxsize)
{
  assert(buf);

  // check to see how much we can read
  if ((dm_buffer.size() - dm_index) < maxsize)
    maxsize = dm_buffer.size() - dm_index;

  if (maxsize > 0)
  {
    // read it in from the string
    memcpy(buf, &dm_buffer[0]+dm_index, maxsize);
    dm_index += maxsize;
  }

  return maxsize;
}

/// write a raw block of data
size_t bufferflow::write(const byte_t* buf, size_t size)
{
  assert(buf);

  if (size > 0) {
    // write data to buffer
    dm_buffer.insert(dm_buffer.begin()+dm_index, buf, buf+size);
    dm_index += size;
  }

  return size;
}

void bufferflow::reset(void)
{
  dm_index = 0;
}

void bufferflow::reserve(size_t _size)
{
  dm_buffer.reserve(_size);
}

void bufferflow::reset_resize(size_t _size)
{
  dm_index = 0;
  dm_buffer.resize(_size);
}

bufferiflow::bufferiflow(const byte_t *data, size_t sz)
{
  dm_cur = dm_data = data;
  dm_end = data + sz;
}

bool bufferiflow::failed(void) const
{
  return dm_cur >= dm_end;
}

size_t bufferiflow::read(byte_t* _buf, size_t _maxsize)
{
  size_t sz;
  const byte_t *e;

  if (_maxsize == 0 || dm_cur >= dm_end)
    return 0;

  // calc end
  e = dm_cur + _maxsize;
  if (e >= dm_end)
    e = dm_end;
  sz = e - dm_cur;

  // do the actual copy
  memcpy(_buf, dm_cur, sz);

  dm_cur = e;
  return sz;
}

bufferoflow::bufferoflow(byte_t *data, size_t sz)
{
  dm_cur = dm_data = data;
  dm_end = data + sz;
}

bool bufferoflow::failed(void) const
{
  return dm_cur >= dm_end;
}

size_t bufferoflow::write(const byte_t* _buf, size_t _size)
{
  size_t sz;
  byte_t *e;

  if (_size == 0 || dm_cur >= dm_end)
    return 0;

  // calc end
  e = dm_cur + _size;
  if (e > dm_end)
    e = dm_end;
  sz = e - dm_cur;

  // do the actual copy
  memcpy(dm_cur, _buf, sz);

  dm_cur = e;
  return sz;
}


//BBlibs
//BBtargets libscopira.so

