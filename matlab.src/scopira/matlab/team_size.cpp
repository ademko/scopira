
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

//BBtargets libteam_size.so
//BBlibs scopiramatlab

#include <scopira/matlab/bind.h>
#include <scopira/matlab/team.h>

using namespace scopira::matlab;

static scopira::matlab::link_loop looper;

extern "C" void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
  if (nlhs == 0)
    return;

  scopira::basekit::narray<double,2> ret;

  bind_output_matlab_array(1, 1, plhs[0], ret);

  ret(0,0) = looper.client_instance()->team_size();
}

