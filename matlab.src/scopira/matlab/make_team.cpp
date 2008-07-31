
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

//BBtargets libmake_team.so
//BBlibs scopiramatlab

#include <scopira/matlab/bind.h>
#include <scopira/matlab/team.h>

using namespace scopira::matlab;

static scopira::matlab::link_loop looper;

extern "C" void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
  std::string teamname;
  scopira::basekit::narray<double, 2> sz;
  int intsz;

  if (nrhs != 2) {
    OUTPUT << "You need to supply a team name (string) and a team size (int)\n";
    return;
  }

  if (!get_input_matlab_string(prhs[0], teamname) || !bind_input_matlab_array(prhs[1], sz)
      || sz.empty())
    return;

  intsz = static_cast<int>(sz(0,0));

  looper.client_instance()->make_team(teamname, intsz);
}

