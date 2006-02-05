
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

#ifndef __INCLUDED_SCOPIRA_CORE_VIEWTYPE_H__
#define __INCLUDED_SCOPIRA_CORE_VIEWTYPE_H__

#include <scopira/core/objecttype.h>
#include <scopira/tool/export.h>

// implementation for all of this is in objecttype.cpp....

namespace scopira
{
  namespace core
  {
    class viewtype;
  }
}

/**
 * A object representing a view's "type".
 *
 * @author Aleksander Demko
 */ 
class scopira::core::viewtype : public scopira::core::objecttype
{
  private:
    std::string dm_menuname;
    objecttype * dm_modeltype;
    int dm_vgmask;

  public:
    /// ctor
    SCOPIRA_EXPORT viewtype(const std::string &name, copyfunc_t cp, const std::string &menuname, int vgmask);

    SCOPIRA_EXPORT void set_modeltype(objecttype *mt);
    objecttype * get_modeltype(void) const { return dm_modeltype; }

    const std::string &get_menuname(void) const { return dm_menuname; }

    int get_vgmask(void) const { return dm_vgmask; }

  protected:
    virtual void print_tree_this(int indent);
};

#endif

