
/*
 *  Copyright (c) 2007-2008    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#include <scopira/agent/register.h>

#include <scopira/core/register.h>

//BBtargets libscopira.so

using namespace scopira::tool;
using namespace scopira::agent;

register_service::register_service(const std::string &taskclassname, scopira::tool::uuid serviceid)
{
  if (!service_registrar::instance())
    new service_registrar;    // it saves itself in its static singleton instance variable

  service_registrar::instance()->add_service(taskclassname, serviceid);
}

static scopira::tool::register_flow<service_registrar> r1("scopira::agent::service_registrar", 1);

service_registrar *service_registrar::dm_instance;

service_registrar::service_registrar(void)
  : scopira::tool::object(true)
{
  assert(dm_instance == 0);
  dm_instance = this;
}

service_registrar * service_registrar::instance(void)
{
  return dm_instance;
}

service_registrar::~service_registrar()
{
  assert(dm_instance == 0);
  dm_instance = 0;
}

void service_registrar::add_service(const std::string &taskclassname, scopira::tool::uuid serviceid)
{
  dm_services.insert(std::make_pair(taskclassname, serviceid));
}

void service_registrar::get_service(const std::string &taskclassname, std::vector<scopira::tool::uuid> &out)
{
  out.clear();

  service_map::iterator ii, endii;

  ii = dm_services.lower_bound(taskclassname);
  endii = dm_services.upper_bound(taskclassname);

  for (; ii != endii; ++ii)
    out.push_back(ii->second);
}

