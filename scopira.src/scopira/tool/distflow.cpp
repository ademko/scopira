
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
 
#include <scopira/tool/distflow.h>

#include <assert.h>

#include <algorithm>

using namespace scopira::tool;

teeoflow::teeoflow(bool doref)
  : dm_a(doref, 0), dm_b(doref, 0)
{
}

bool teeoflow::failed(void) const
{
  return false;   // i never fail! im an invincible
}

size_t teeoflow::write(const byte_t* _buf, size_t _size)
{
  int ret, k;

  if (!dm_a.is_null())
    ret = dm_a->write(_buf, _size);
  else
    ret = 0;
  if (!dm_b.is_null()) {
    k = dm_b->write(_buf, _size);
    if (k>ret)
      ret = k;
  }

  return ret;
}

void teeoflow::set_flow_a(oflow_i* r)
{
  dm_a = r;
}

void teeoflow::set_flow_b(oflow_i* r)
{
  dm_b = r;
}

/// default constructor
distoflow::distoflow(bool doref)
  : dm_doref(doref)
{
}

/// are we in a failed state?
bool distoflow::failed(void) const
{
  return false;   // i never fail! im an invincible
}


/// write a raw block of data
size_t distoflow::write(const byte_t* _buf, size_t _size)
{
  int ret, k;
  oflowvec_t::iterator ii;

  ret = 0;
  for (ii=dm_flow.begin(); ii != dm_flow.end(); ii++) {
    k = (*ii)->write(_buf, _size);
    if (k>ret)
      ret = k;
  }
    
  return ret;
}

void distoflow::add_flow(oflow_i* r)
{
  dm_flow.push_back(tool::count2_ptr< scopira::tool::oflow_i >(dm_doref, r));
}

void distoflow::remove_flow(oflow_i* r)
{
  oflowvec_t::iterator ii;

  for (ii=dm_flow.begin(); (ii != dm_flow.end()) && ((*ii).get() != r); ii++)
    ; // empty loop body

  assert(ii != dm_flow.end());

  dm_flow.erase(ii);
}

void distoflow::clear_flow(void)
{
  dm_flow.clear();
}

bool distoflow::contains_flow(const oflow_i* r) const
{
  oflowvec_t::const_iterator ii;

  for (ii=dm_flow.begin(); ii!=dm_flow.end(); ii++)
    if ((*ii).get() == r)
      return true;

  return false;
}

int distoflow::size_flow(void) const
{
  return dm_flow.size();
}


//BBlibs
//BBtargets libscopira.so

