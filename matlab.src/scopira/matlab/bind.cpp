
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

#include <scopira/matlab/bind.h>

//BBtargets libscopiramatlab.so
//BBlibs mex scopiraui

namespace {
class direct_matlab_narray_delete : public scopira::basekit::narray_delete_i
{
  private:
    const mxArray *dm_a;    // this can be null or not null, its not used right now
    mxArray *dm_dyna;       // if this is non null, it will be mxDestroyArray'ed on destruction
  public:
    direct_matlab_narray_delete(const mxArray *a) : dm_a(a), dm_dyna(0) { }
    direct_matlab_narray_delete(mxArray *a, int dummy) : dm_a(0), dm_dyna(a) { }
    virtual void narray_delete(void *mem, size_t len);
};
}

void direct_matlab_narray_delete::narray_delete(void *mem, size_t len)
{
  if (dm_dyna)
    mxDestroyArray(dm_dyna);
  delete this;
}

bool scopira::matlab::bind_input_matlab_array(const mxArray *a, scopira::basekit::narray<double,2> &o)
{
  double *dat;
  size_t w, h;

  assert(a);

  if (!mxIsDouble(a) || mxIsComplex(a))
    return false;

  w = mxGetM(a);
  h = mxGetN(a);
  dat = mxGetPr(a);
  assert(dat);

  o.resize_direct(scopira::basekit::nindex<2>(w,h), dat, new direct_matlab_narray_delete(a));

  return true;
}

void scopira::matlab::bind_output_matlab_array(size_t w, size_t h, mxArray * &a, scopira::basekit::narray<double,2> &o)
{
  double *dat;

  assert(w>=0);
  assert(h>=0);

  a = mxCreateNumericMatrix(w, h, mxDOUBLE_CLASS, mxREAL);
  assert(a);
  dat = mxGetPr(a);
  assert(dat);

  // yes, we dont want this array to be deleted as its being RETURNED to matlab
  o.resize_direct(scopira::basekit::nindex<2>(w,h), dat, new direct_matlab_narray_delete(a));
}

bool scopira::matlab::get_input_matlab_string(const mxArray *a, std::string &outstring)
{
  assert(a);
  if (!mxIsChar(a))
    return false;

  outstring.resize(mxGetN(a));

  // yeah, const_cast, but it saves a copy :P
  mxGetString(a, const_cast<char*>(outstring.c_str()), outstring.size() + 1);

  return true;
}

