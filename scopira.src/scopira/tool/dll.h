
/*
 *  Copyright (c) 2002-2004    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#ifndef __INCLUDED_SCOPIRA_TOOL_DLL_H__
#define __INCLUDED_SCOPIRA_TOOL_DLL_H__

#include <string>

#include <scopira/tool/platform.h>
#include <scopira/tool/object.h>
#include <scopira/tool/export.h>

namespace scopira
{
  namespace tool
  {
    class dll;

    /// either .DLL or .SO
    SCOPIRA_EXPORT extern const char *dll_ext_c;
  }
}

/**
 * a Dynamic Link Library
 *
 * @author Aleksander Demko
 */
class scopira::tool::dll : public virtual scopira::tool::object
{
  protected:
#ifdef PLATFORM_win32
    typedef HINSTANCE LIB_HANDLE;
#else
    typedef void* LIB_HANDLE;
#endif
    
    mutable LIB_HANDLE dm_handle;
    bool dm_open;
    const char * dm_lasterror;

  public:

    typedef void (*voidfunc_t)(void);

    /// default constructor
    SCOPIRA_EXPORT dll(void);
    /// destrutor
    SCOPIRA_EXPORT virtual ~dll();

    /**
     * loads a dll, unloading any current one
     *
     * @param _name the filename of the dll
     * @return true on success
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT bool load_dll(std::string _name);

    /**
     * gets a symbol from the dll
     *
     * @param _name the name of the symbol
     * @return the symbol's address
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT void * get_sym(const std::string & _name) const;

    /**
     * gets the last error, if any (null for none)
     * @author Aleksander Demko
     */ 
    const char *get_error(void) const { return dm_lasterror; }

  protected:

    /// calls when its time to construct a dll, true on success
    SCOPIRA_EXPORT virtual bool dll_init(const std::string & _name);
    /// calls when a dll is being removed
    SCOPIRA_EXPORT virtual void dll_close(void);
    /// fills dm_lasterror
    SCOPIRA_EXPORT void check_error(void);
};

#endif

