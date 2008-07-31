
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

//BBtargets libjoin_team.so
//BBlibs scopiramatlab

#include <scopira/matlab/bind.h>
#include <scopira/matlab/team.h>

using namespace scopira::matlab;

static scopira::matlab::link_loop looper;

extern "C" void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
  std::string teamname;

  if (nrhs != 1) {
    OUTPUT << "You need to supply a team name (string)\n";
    return;
  }

  if (!get_input_matlab_string(prhs[0], teamname))
    return;

  looper.client_instance()->join_team(teamname);
}

