
/*
 *  Copyright (c) 2005    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#include <scopira/basekit/stringvector.h>

#include <scopira/tool/flow.h>
#include <scopira/tool/printflow.h>
#include <scopira/core/register.h>

//BBtargets libscopira.so

using namespace scopira::basekit;

//
//
// stringvector_o
//
//

stringvector_o::stringvector_o(void)
{
}

stringvector_o::stringvector_o(const stringvector_o &rhs)
  : parent_type(rhs.size())
{
  parent_type::iterator ii;
  parent_type::const_iterator ci;
  // copy, ugh
  ci = rhs.begin();
  for (ii=begin(); ii != end(); ++ii) {
    *ii = *ci;
    ++ci;
  }
}

stringvector_o::stringvector_o(size_t len)
  : parent_type(len)
{
}

scopira::tool::oflow_i & stringvector_o::print(scopira::tool::oflow_i &o) const
{
  size_t i, mx;

  mx = size();
  o << "String Vec, len=" << mx << ":";
  for (i=0; i<mx; i++) {
    if (i % 10 == 0)
      flow_printf(o, "\n %4d:", i);
    o << ' ' << get(i);
  }

  return o << '\n';
}

bool stringvector_o::load(scopira::tool::iobjflow_i &in)
{
  int sz;

  if (!in.read_int(sz))
    return false;
  resize(sz);
  for (size_t i=0; i<dm_len; i++)
    if (!in.read_string(dm_ary[i]))
      return false;
  return true;
}

void stringvector_o::save(scopira::tool::oobjflow_i &out) const
{
  // write size
  out.write_int(static_cast<int>(dm_len));
  for (size_t i=0; i<dm_len; i++)
    out.write_string(dm_ary[i]);
}

//
// register stuff
//

static scopira::core::register_object<stringvector_o> r1("scopira::basekit::stringvector_o");

