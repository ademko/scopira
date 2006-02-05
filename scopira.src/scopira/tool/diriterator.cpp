
/*
 *  Copyright (c) 2003    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#include <scopira/tool/diriterator.h>

#include <sys/stat.h>
#include <assert.h>

//BBlibs
//BBtargets libscopira.so

using namespace scopira;

//tool::dir_iterator
tool::dir_iterator::dir_iterator(void)
  : dm_failed(true),
#ifdef PLATFORM_win32
    dm_dir(INVALID_HANDLE_VALUE)
#else
    dm_dir(0)
#endif
{
}

tool::dir_iterator::~dir_iterator()
{
  close();
}

bool tool::dir_iterator::open(const std::string& dirname)
{
  close();

#ifdef PLATFORM_win32
  dm_dir = FindFirstFile((dirname + "\\*").c_str(), &dm_dirdata);
  dm_haveprime = true;
  dm_failed = dm_dir == INVALID_HANDLE_VALUE;
#else
  dm_dir = opendir(dirname.c_str());
  dm_failed = (dm_dir == 0);
#endif
  return !dm_failed;
}

void tool::dir_iterator::close(void)
{
#ifdef PLATFORM_win32
  if (dm_dir == INVALID_HANDLE_VALUE)
    return;
    
  FindClose(dm_dir);
  dm_dir = INVALID_HANDLE_VALUE;
#else
  if (!dm_dir)
    return;

  closedir(dm_dir);
  dm_dir = 0;
#endif

  dm_failed = true;
}

bool tool::dir_iterator::next(std::string& out)
{
#ifdef PLATFORM_win32
  if (dm_failed)
    return false;
  
  // easy, first case?  
  if (dm_haveprime) {
    out = dm_dirdata.cFileName;
    dm_haveprime = false;
    return true;
  }

  dm_failed = FindNextFile(dm_dir, &dm_dirdata) == 0;
  
  if (!dm_failed)
    out = dm_dirdata.cFileName;
#else
  if (dm_failed)
    return false;

  struct dirent *d;
  d = readdir(dm_dir);
  dm_failed = (d == 0);

  if (!dm_failed)
    out = d->d_name;
#endif

  return !dm_failed;
}
