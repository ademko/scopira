
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

#ifndef __INCLUDED_SCOPIRA_BASEKIT_STRINGVECTOR_H__
#define __INCLUDED_SCOPIRA_BASEKIT_STRINGVECTOR_H__

#include <assert.h>

#include <string>

#include <scopira/tool/object.h>
#include <scopira/tool/array.h>
#include <scopira/tool/export.h>

// THIS FILE HAS BEEN FULLY DOCUMENTED

namespace scopira
{
  namespace basekit
  {
    class stringvector_o;
  }
}

/**
 * A vector of strings. Serializable
 *
 * @author Aleksander Demko
 */
class scopira::basekit::stringvector_o : public virtual scopira::tool::object,
  public scopira::tool::basic_array<std::string>
{
  private:
    typedef scopira::tool::basic_array<std::string> parent_type;
  public:
    /// Default, 0-length construtor
    SCOPIRA_EXPORT stringvector_o(void);
    /// copy ctor
    SCOPIRA_EXPORT stringvector_o(const stringvector_o &rhs);
    /**
     * Creates a vector of the given size.
     * Elements will be initilized to empty strings.
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT explicit stringvector_o(size_t len);

    /// Printer implementation
    SCOPIRA_EXPORT virtual scopira::tool::oflow_i & print(scopira::tool::oflow_i &o) const;
    /// Serialization implementation
    SCOPIRA_EXPORT virtual bool load(scopira::tool::iobjflow_i &in);
    /// Serialization implementation
    SCOPIRA_EXPORT virtual void save(scopira::tool::oobjflow_i &out) const;
};

#endif

