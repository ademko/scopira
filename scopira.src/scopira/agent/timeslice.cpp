
/*
 *  Copyright (c) 2009    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#include <scopira/agent/timeslice.h>

#include <scopira/tool/util.h>
#include <scopira/tool/output.h>
#include <scopira/tool/stringflow.h>

using namespace scopira::agent;

//
//
// timeslice
//
//

timeslice::timeslice(const char * prefix)
  : dm_prefix(prefix)
{
  for (int x=0; x<num_timers; ++x)
    dm_numerators[x] = 0;

  dm_chrono.start();
}

timeslice::~timeslice()
{
  /*if (dm_parent) {
    dm_parent->dm_numerator += dm_parent->dm_chrono.get_running_time() - dm_numerator;
  } else {
    dm_chrono.stop();

    std::string output;
    output.reserve(128);

    if (dm_prefix)
      output += dm_prefix;

    output += "timeslice(";
    output += scopira::tool::double_to_string(dm_chrono.get_total_time());
    output += "s,";
    output += scopira::tool::double_to_string(dm_numerator/dm_chrono.get_total_time() * 100);
    output += "%)\n";

    OUTPUT << output;
  }
  */
  dm_chrono.stop();

  if (!dm_prefix)
    return;

  double tottime = dm_chrono.get_total_time();

  scopira::tool::stringflow out;

  out << dm_prefix << " " << tottime << "s ";
  for (int x=0; x<num_timers; ++x) {
    out << "[" << dm_numerators[x] << "s " << (dm_numerators[x]/tottime)*100 << "%]";
  }
  out << "\n";

  OUTPUT << out.str();
}

//
//
// global_timeslice
//
//

global_timeslice * global_timeslice::dm_instance = 0;

global_timeslice::global_timeslice(const char * prefix)
  : timeslice(prefix)
{
  assert(dm_instance == 0);

  dm_instance = this;
}

global_timeslice::~global_timeslice()
{
  assert(dm_instance == this);

  dm_instance = 0;
}

//
//
// subtimeslice
//
//

subtimeslice::subtimeslice(timeslice &parent, int index)
  : dm_parent(&parent), dm_index(index)
{
  dm_starttime = dm_parent->dm_chrono.get_running_time();
}

subtimeslice::subtimeslice(int index)
  : dm_parent(global_timeslice::instance()), dm_index(index)
{
  assert(dm_parent);
  dm_starttime = dm_parent->dm_chrono.get_running_time();
}

subtimeslice::~subtimeslice()
{
  dm_parent->dm_numerators[dm_index] += dm_parent->dm_chrono.get_running_time() - dm_starttime;
}

//
//
// simple_timeslice
//
//

simple_timeslice::simple_timeslice(void)
{
  dm_chrono.start();
}

simple_timeslice::~simple_timeslice()
{
  dm_chrono.stop();

  scopira::tool::stringflow out;

  out << "simple_timeslice(" << dm_chrono.get_total_time() << "s)\n";

  OUTPUT << out;
}

