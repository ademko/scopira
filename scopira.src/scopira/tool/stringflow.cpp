
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

#include <scopira/tool/stringflow.h>

#include <assert.h>
#include <string.h>

using namespace scopira::tool;
using namespace std;

//
//
// stringflow
//
//

/// default constructor
stringflow::stringflow(void)
  : dm_mode(0)
{
}

stringflow::stringflow(size_t res)
  : dm_mode(0)
{
  open();
  reserve(res);
}

/// opening constructor
stringflow::stringflow(const std::string& data, mode_t mode)
  : dm_mode(0)
{
  assert(mode);
  open(data, mode);
}

/// destructor
stringflow::~stringflow()
{
  close();
}

bool stringflow::failed(void) const
{
  if (dm_mode & input_c)
    return dm_index >= dm_data.size();
  else
    return !(dm_mode & output_c);
}

/// read raw block data, returns num read in
size_t stringflow::read(byte_t* _buf, size_t _maxsize)
{
  assert(dm_mode & input_c);
  assert(_buf);

  if (failed())
    return 0;

  // check to see how much we can read
  if ( (dm_data.size() - dm_index) < _maxsize)
    _maxsize = dm_data.size() - dm_index;

  if (_maxsize > 0) {
    // read it in from the string
    memcpy(_buf, dm_data.c_str() + dm_index, _maxsize);
    dm_index += _maxsize;
  }

  return _maxsize;
}

/// write a raw block of data
size_t stringflow::write(const byte_t* _buf, size_t _size)
{
  assert(dm_mode & output_c);
  assert(_buf);

  if (failed())
    return 0;

  if (_size > 0)
    dm_data.append(reinterpret_cast<const char*>(_buf), _size);
  return _size;
}

void stringflow::open(void)
{
  close();
  
  dm_mode = output_c | trunc_c;
  dm_index = 0;
  dm_data.clear();
}

void stringflow::open(const std::string& data, mode_t mode)
{
  close();
  
  assert(mode);

  // mode check
  dm_mode = mode;
  dm_index = 0;
  if (dm_mode & trunc_c)
    dm_data.clear();
  else
    dm_data = data;
}

/// close the file
void stringflow::close(void)
{
  if (dm_mode) {
    dm_data.clear();
    dm_mode = 0;
  }
}

void stringflow::reserve(size_t _size)
{
  dm_data.reserve(_size);
}

//
//
// stringiflow
//
//

stringiflow::stringiflow(const std::string* data)
  : dm_data(data), dm_idx(0)
{
}

bool stringiflow::failed(void) const
{
  return dm_idx >= dm_data->size();
}

size_t stringiflow::read(byte_t* _buf, size_t _maxsize)
{
  size_t c = _maxsize;

  assert(_buf);
  assert(dm_data);

  if (dm_idx + c > dm_data->size())
    c = dm_data->size() - dm_idx;

  memcpy(_buf, dm_data->c_str()+dm_idx, c);
  dm_idx += c;

  return c;
}

void stringiflow::open(const std::string *data)
{
  dm_data = data;
  dm_idx = 0;
}

//
//
// stringoflow
//
//

stringoflow::stringoflow(std::string* data)
  : dm_data(data)
{
}

size_t stringoflow::write(const byte_t* _buf, size_t _size)
{
  assert(_buf);
  if (_size > 0)
    dm_data->append(reinterpret_cast<const char*>(_buf), _size);
  return _size;
}

void stringoflow::open(std::string *data)
{
  dm_data = data;
}


//BBlibs
//BBtargets libscopira.so

