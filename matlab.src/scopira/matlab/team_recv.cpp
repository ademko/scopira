
/*
 *  Copyright (c) 2006    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

//BBtargets libteam_recv.so
//BBlibs scopiramatlab

#include <scopira/matlab/bind.h>
#include <scopira/matlab/team.h>

using namespace scopira::agent;
using namespace scopira::matlab;

static scopira::matlab::link_loop looper;

extern "C" void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
  std::string teamname;
  scopira::basekit::narray<double, 2> peer, data;
  int intpeer;

  if (nrhs != 1 && nlhs != 1) {
    OUTPUT << "You need to supply a team peer number (int) and return one matrix\n";
    return;
  }

  if (!bind_input_matlab_array(prhs[0], peer) || peer.empty())
    return;

  intpeer = static_cast<int>(peer(0,0));

  {
    recv_msg M(looper.client_instance()->context(), looper.client_instance()->team_id(intpeer));
    size_t w, h;

    M.read_size_t(w);
    M.read_size_t(h);

    bind_output_matlab_array(w, h, plhs[0], data);

    data.all_slice().load(M);
  }
}

