
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

#ifndef __INCLUDED_SCOPIRA_AGENT_REGISTER_H__
#define __INCLUDED_SCOPIRA_AGENT_REGISTER_H__

#include <map>
#include <vector>

#include <scopira/tool/object.h>
#include <scopira/tool/uuid.h>

namespace scopira
{
  namespace agent
  {
    class register_service;
    class service_registrar;
  }
}

/**
 * These can be used to register than a particule task provides services.
 * Use them in addition to the normal register_object static classes.
 *
 * @author Aleksander Demko
 */ 
class scopira::agent::register_service
{
  public:
    /// the ctor
    register_service(const std::string &taskclassname, scopira::tool::uuid serviceid);
};

/**
 * @author Aleksander Demko
 */
class scopira::agent::service_registrar : public virtual scopira::tool::object
{
  private:
    // maps class names to servicesd they provide
    typedef std::multimap<std::string, scopira::tool::uuid> service_map;

    service_map dm_services;

  public:
    /// constructor
    service_registrar(void);
    /// destructor
    virtual ~service_registrar();

    /**
     * Returns the static instance, might return null.
     *
     *
     * @author Aleksander Demko
     */ 
    static service_registrar * instance(void);

    void add_service(const std::string &taskclassname, scopira::tool::uuid serviceid);

    void get_service(const std::string &taskclassname, std::vector<scopira::tool::uuid> &out);

  private:
    static service_registrar *dm_instance;
};

#endif

