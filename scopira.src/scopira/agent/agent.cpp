
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

#include <scopira/agent/agent.h>

#include <scopira/tool/array.h>
#include <scopira/core/loop.h>
#include <scopira/agent/localagent.h>
#include <scopira/agent/clusteragent.h>

//BBtargets libscopira.so

using namespace scopira::basekit;
using namespace scopira::agent;

//
//
// uuid_query
//
//

uuid_query::uuid_query(void)
{
  // complex case, prep the storage
  dm_exp.reserve(16);
  dm_data.reserve(4);
}

uuid_query::uuid_query(scopira::tool::uuid simplecase)
  : dm_simpleuuid(simplecase)
{
}

bool uuid_query::query(scopira::tool::uuid testid) const
{
  // handle the simple case first
  if (dm_exp.empty())
    return testid == dm_simpleuuid || dm_simpleuuid.is_zero();

  scopira::tool::fixed_array<bool, 32> tstack;    // things we do to avoid an alloc in this func :)
  int tnext;
  int expi, datai, dataend;

  tnext = 0;
  for (expi=0; expi<dm_exp.size(); ++expi)
    switch (dm_exp[expi]) {
      case match_any_c:
        tstack[tnext++] = true;
        break;
      case match_one_c:
        datai = dm_exp[++expi];
        tstack[tnext++] = testid == dm_data[datai];
        break;
      case match_group_c:
        datai = dm_exp[++expi];
        dataend = dm_exp[++expi] + datai;
        for (; datai != dataend; ++datai)
          if (testid == dm_data[datai]) {
            tstack[tnext++] = true;
            break;
          }
        if (datai == dataend)
            tstack[tnext++] = false;
        break;
      case op_and_c:
        --tnext;    // pop one
        tstack[tnext-1] = tstack[tnext-1] && tstack[tnext];
        break;
      case op_or_c:
        --tnext;    // pop one
        tstack[tnext-1] = tstack[tnext-1] || tstack[tnext];
        break;
      case op_xor_c:
        --tnext;    // pop one
        tstack[tnext-1] = tstack[tnext-1] ^ tstack[tnext];    // I really want a ^^ here, or some !! trickery
        break;
      case op_not_c:
        tstack[tnext-1] = !tstack[tnext-1];
        break;
      default : assert(false);    // known thing on our exp list!
    }

  assert(tnext==1);
  return tstack[0];
}

uuid_query & uuid_query::match_any(void)
{
  dm_exp.push_back(match_any_c);
  return *this;
}

uuid_query & uuid_query::match_one(scopira::tool::uuid id)
{
  dm_exp.push_back(match_one_c);
  dm_exp.push_back(dm_data.size());
  dm_data.push_back(id);

  return *this;
}

uuid_query & uuid_query::match_group(const_nslice<scopira::tool::uuid> ids)
{
  dm_exp.push_back(match_group_c);
  dm_exp.push_back(dm_data.size());
  dm_exp.push_back(ids.size());

  for (int x=0; x<ids.size(); ++x)
    dm_data.push_back(ids[x]);

  // in the future, SORT the sub vector on the data array, allowing for
  // binary searches on the testing end. perhaps do this for size()>32 or some shit

  return *this;
}

uuid_query & uuid_query::op_and(void)
{
  dm_exp.push_back(op_and_c);
  return *this;
}

uuid_query & uuid_query::op_or(void)
{
  dm_exp.push_back(op_or_c);
  return *this;
}

uuid_query & uuid_query::op_xor(void)
{
  dm_exp.push_back(op_xor_c);
  return *this;
}

uuid_query & uuid_query::op_not(void)
{
  dm_exp.push_back(op_not_c);
  return *this;
}

//
//
// agent_i
//
//

agent_i * agent_i::dm_instance;

agent_i::agent_i(void)
{
  assert(dm_instance == 0);
  dm_instance = this;

  // what if two ppl create agent_i's at the same time? oh oh
}

agent_i::~agent_i()
{
  assert(dm_instance == this);
  dm_instance = 0;
}

agent_i * agent_i::new_agent(void)
{
  if (scopira::core::basic_loop::instance() &&
      scopira::core::basic_loop::instance()->has_config("cluster")) {
    agent_i *server_link;

    server_link = new cluster_agent;

    // we cant use ref counting here (as wed unref the object on return as our
    // local count_ptr would go out of scope
    // obviously NOT exception safe

    if (server_link->failed()) {
      delete server_link;
      // we failed, give them a local agent instead
      // by simply dropping to the end of this function :)
      OUTPUT << "Failed to start cluster_agent; using local_agent\n";
    } else
      return server_link;
  }

  // default and on error, just return local
  return new local_agent;
}

bool agent_i::get_cluster_server_url(std::string &serverurl)
{
  cluster_agent *ca = dynamic_cast<cluster_agent*>(instance());

  if (!ca)
    return false;

  serverurl = ca->spec().pm_url.get_url();

  return true;
}

