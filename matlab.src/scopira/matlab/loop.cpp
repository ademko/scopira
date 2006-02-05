
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

#include <scopira/matlab/loop.h>

//BBtargets libscopiramatlab.so
//BBlibs scopiraui

scopira::matlab::matlab_loop::matlab_loop(bool needgui)
{
  int argc = 0;
  char **argv = 0;
  if (scopira::lab::lab_loop::instance())
    dm_loop = scopira::lab::lab_loop::instance();
  else
    dm_loop = new scopira::lab::lab_loop(false, argc, argv);
  assert(dm_loop.get());

  if (needgui && !dm_loop->has_display()) {
    bool ret = dm_loop->open_display(argc, argv);
    assert(ret && "[cant open display from within matlab_loop]");
  }
}

scopira::matlab::matlab_loop::~matlab_loop()
{
}

