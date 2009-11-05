
/*
 *  Copyright (c) 2005-2007    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */
 
#include <scopira/tool/hash.h>

#include <scopira/tool/hexflow.h>

#include "sha1_impl.h"

//BBtargets libscopira.so

using namespace scopira::tool;

std::string scopira::tool::hexhash(const scopira::tool::basic_array<byte_t> &h)
{
  std::string s;

  s.resize(h.size() * 2);

  for (int x=0; x<h.size(); ++x) {
    s[x*2] = lowerhexchars[ h[x] >> 4 ];
    s[x*2+1] = lowerhexchars[ h[x] & 0x0F ];
  }

  return s;
}

std::string scopira::tool::close_and_hexhash(hashoflow &o)
{
  basic_array<byte_t> h;

  o.close(h);

  return hexhash(h);
}

std::string scopira::tool::hexhash_string(const std::string &s, int type)
{
  hashoflow h(type);

  h.write(reinterpret_cast<const byte_t*>(s.data()), s.size());

  return close_and_hexhash(h);
}

hashoflow::hashoflow(int alg)
  : dm_impl(0)
{
  open(alg);
}

hashoflow::~hashoflow()
{
  close();
}

size_t hashoflow::write(const byte_t* _buf, size_t _size)
{
  if (!dm_impl)
    return 0;

  SHA1_Update(reinterpret_cast<SHA_CTX*>(dm_impl), _buf, _size);

  return _size;
}

void hashoflow::open(int alg)
{
  assert(alg == sha1_c);

  close();

  dm_impl = new SHA_CTX;

  SHA1_Init(reinterpret_cast<SHA_CTX*>(dm_impl));
}

void hashoflow::close(void)
{
  if (!dm_impl)
    return;

  delete reinterpret_cast<SHA_CTX*>(dm_impl);

  dm_impl = 0;
}

void hashoflow::close(scopira::tool::basic_array<byte_t> &hash)
{
  if (!dm_impl)
    return;

  hash.resize(20);
  SHA1_Final(hash.c_array(), reinterpret_cast<SHA_CTX*>(dm_impl));

  delete reinterpret_cast<SHA_CTX*>(dm_impl);

  dm_impl = 0;
}

