
/*
 *  Copyright (c) 2004-2005    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#include <pakit/types.h>

//BBlibs scopira
//BBtargets libpakit.so

using namespace scopira::basekit;
using namespace pakit;

patterns_t::patterns_t(bool init)
{
  if (init) {
    pm_data = new narray_o<double,2>;
    pm_classes = new narray_o<int,1>;
    pm_training = new narray_o<bool,1>;
    pm_labels = new stringvector_o;
  }
}

patterns_t::patterns_t(const patterns_t &rhs, bool deepcopy)
{
  pm_comment = rhs.pm_comment;

  if (deepcopy) {
    pm_data = new narray_o<double,2>(rhs.pm_data.ref());
    pm_classes = new narray_o<int,1>(rhs.pm_classes.ref());
    pm_training = new narray_o<bool,1>(rhs.pm_training.ref());
    pm_labels = new stringvector_o(rhs.pm_labels.ref());
  } else {
    pm_data = rhs.pm_data;
    pm_classes = rhs.pm_classes;
    pm_training = rhs.pm_training;
    pm_labels = rhs.pm_labels;
  }
}

void patterns_t::copy_patterns(const patterns_t &rhs, bool deepcopy)
{
  pm_comment = rhs.pm_comment;

  if (deepcopy) {
    pm_data = new narray_o<double,2>(rhs.pm_data.ref());
    pm_classes = new narray_o<int,1>(rhs.pm_classes.ref());
    pm_training = new narray_o<bool,1>(rhs.pm_training.ref());
    pm_labels = new stringvector_o(rhs.pm_labels.ref());
  } else {
    pm_data = rhs.pm_data;
    pm_classes = rhs.pm_classes;
    pm_training = rhs.pm_training;
    pm_labels = rhs.pm_labels;
  }
}

bool patterns_t::load(scopira::tool::iobjflow_i& in)
{
  int version;

  return
    in.read_int(version) &&

    in.read_object_type(pm_data) &&
    in.read_object_type(pm_classes) &&
    in.read_object_type(pm_training) &&
    in.read_object_type(pm_labels) &&
    in.read_string(pm_comment);
}

void patterns_t::save(scopira::tool::oobjflow_i& out) const
{
  out.write_int(1);

  out.write_object_type(pm_data);
  out.write_object_type(pm_classes);
  out.write_object_type(pm_training);
  out.write_object_type(pm_labels);
  out.write_string(pm_comment);
}

