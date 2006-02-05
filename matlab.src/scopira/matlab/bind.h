
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

#ifndef __INCLUDED_SCOPIRA_MATLAB_BIND_H
#define __INCLUDED_SCOPIRA_MATLAB_BIND_H

#include "mex.h"

#include <scopira/basekit/narray.h>

namespace scopira
{
  namespace matlab
  {
    bool bind_input_matlab_array(const mxArray *a, scopira::basekit::narray<double,2> &o);
    void bind_output_matlab_array(size_t w, size_t h, mxArray * &a, scopira::basekit::narray<double,2> &o);

    bool get_input_matlab_string(const mxArray *a, std::string &outstring);
  }
}

#endif

