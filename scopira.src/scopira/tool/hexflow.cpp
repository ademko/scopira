
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
 
#include <scopira/tool/hexflow.h>

#include <scopira/tool/array.h>

#include <assert.h>

using namespace scopira::tool;

const char* scopira::tool::hexchars = "0123456789ABCDEF";

hexiflow::hexiflow(bool doref, iflow_i* in)
  : dm_in(doref, 0)
{
  if (in)
    open(in);
}

bool hexiflow::failed(void) const
{
  if (dm_in.get())
    return dm_in->failed();
  else
    return true;
}

size_t hexiflow::read(byte_t* _buf, size_t _maxsize)
{
  switch (_maxsize) {
    case 1: return read_byte(_buf[0]);
    case 0: return 0;
    default: {
        fixed_array<byte_t, 1024> buffy;
        fixed_array<byte_t, 1024>::iterator ii, endii;
        size_t togo = _maxsize;
        size_t r;

        while (togo > 0) {
          r = togo*2>buffy.size() ? buffy.size() : togo*2;

          dm_in->read(buffy.begin(), r);

          endii = buffy.begin() + r;
          for (ii=buffy.begin(); ii != endii; ii += 2) {
            *_buf = (char_to_hex(ii[0]) << 4) | char_to_hex(ii[1]);
            ++_buf;
          }

          togo -= r/2;
        }
        return _maxsize;
      }
  }
}

size_t hexiflow::read_byte(byte_t &out)
{
  byte_t r, r2;

  assert(dm_in.get());

  if (dm_in->read_byte(r)>0 && dm_in->read_byte(r2)>0) {
    out = (char_to_hex(r) << 4) | char_to_hex(r2);
    return 1;
  } else
    return 0;
}

void hexiflow::open(iflow_i* in)
{
  assert(in);
  dm_in.set(in);
}

void hexiflow::close(void)
{
  dm_in.set(0);
}

hexoflow::hexoflow(bool doref, oflow_i* out)
  : dm_out(doref, 0)
{
  if (out)
    open(out);
}

bool hexoflow::failed(void) const
{
  if (dm_out.get())
    return dm_out->failed();
  else
    return true;
}

size_t hexoflow::write(const byte_t* _buf, size_t _size)
{
  switch (_size) {
    case 1:
      return write_byte(_buf[0]);
    case 0:
      return 0;
    default: {
        fixed_array<byte_t, 1024> buffy;
        fixed_array<byte_t, 1024>::iterator ii;
        const byte_t *E = _buf + _size;

        while (_buf != E) {
          // load the buffer
          ii = buffy.begin();
          while (ii != buffy.end() && _buf != E) {

            *ii = hexchars[*_buf >> 4];
            ++ii;
            *ii = hexchars[*_buf & 0x0F];
            ++ii;

            ++_buf;
          }
          dm_out->write(buffy.c_array(), ii - buffy.begin());
        }
        return _size;
      }
  }
}

size_t hexoflow::write_byte(byte_t b)
{
  assert(dm_out.get());

  dm_out->write_byte( hexchars[b >> 4] );
  dm_out->write_byte( hexchars[b & 0x0F] );

  return 1;
}

void hexoflow::open(oflow_i* out)
{
  assert(out);
  dm_out.set(out);
}

void hexoflow::close(void)
{
  dm_out = 0;
}

//BBlibs
//BBtargets libscopira.so

