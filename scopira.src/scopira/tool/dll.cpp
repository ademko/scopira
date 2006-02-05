
/*
 *  Copyright (c) 2001    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#include <scopira/tool/dll.h>
#include <assert.h>

#ifdef PLATFORM_win32
#include <windows.h> // for LoadLibrary(), GetProcAddress()
#else
#include <dlfcn.h>   // for dlopen() and dlsym()
#endif

#include <scopira/tool/fileflow.h>

//BBtargets libscopira.so

using namespace scopira::tool;

const char * scopira::tool::dll_ext_c
#ifdef PLATFORM_win32    
  = ".DLL";    
#else
  = ".SO";
#endif

dll::dll(void)
{
  dm_open = false;
  dm_lasterror = 0;
}

dll::~dll()
{
  if (dm_open)
    dll_close();
}

bool dll::load_dll(std::string _name)
{
  if (dm_open) {
    dll_close();
    dm_open = false;
  }

  return dll_init(_name);
}

void * dll::get_sym(const std::string & _name) const
{
#ifdef PLATFORM_win32
  return (void *)GetProcAddress(dm_handle, _name.c_str());
#else
  return dlsym(dm_handle, _name.c_str());
#endif
}

bool dll::dll_init(const std::string & _name)
{
  void *h;
  LIB_HANDLE lh;

  assert(!dm_open);

#ifdef PLATFORM_win32
  lh = LoadLibrary(_name.c_str());
#else
  lh = dlopen(_name.c_str(), RTLD_LAZY|RTLD_GLOBAL); // should this be RTLD_LAZY? or back to _NOW?
#endif

  if (!lh) {
    scopira::tool::fileflow outflow(scopira::tool::fileflow::stderr_c,0);
    check_error();
    outflow << "Error on dlopen(): " << get_error() << '\n';
    return false; // failed now
  }

  // set the impl up
  dm_handle = lh;

  // call init func
#ifdef PLATFORM_win32
  h = (void *)GetProcAddress(dm_handle, "dll_init");
#else
  h = dlsym(dm_handle, "dll_init");
#endif
  if (h)
    ((voidfunc_t)h)();      // we use C-style to avoid a ISO-C++ error. such a kludge

  dm_open = true;
  dm_lasterror = 0;

  return true;
}

void dll::dll_close(void)
{
  return;
  int er;
  void *f;
  
  assert(dm_open);
  
  // call close func
#ifdef PLATFORM_win32
  f = (void *)GetProcAddress(dm_handle, "dll_close");
#else
  f = dlsym(dm_handle, "dll_close");
#endif
  if (f)
    ((voidfunc_t)f)();      // we use C-style to avoid a ISO-C++ error. such a kludge

  // kill the dll
#ifdef PLATFORM_win32
//  er = FreeLibrary(dm_handle);
er = 0;
#else
  er = dlclose(dm_handle);
#endif
  dm_open = false;

  if (er != 0) {
    scopira::tool::fileflow outflow(scopira::tool::fileflow::stderr_c,0);
    outflow << "Error on dlclose(): " << er << '\n';
  }
}

void dll::check_error(void)
{
#ifdef PLATFORM_win32
  dm_lasterror = 0;
#else
  const char *e = dlerror();
  if (e)
    dm_lasterror = e;
#endif
}
