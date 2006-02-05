
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

#ifndef __INCLUDED__SCOPIRA_TOOL_DIRITERATOR_HPP__
#define __INCLUDED__SCOPIRA_TOOL_DIRITERATOR_HPP__

#include <scopira/tool/platform.h>

#ifdef PLATFORM_win32
#include <windows.h>
#else
#include <dirent.h>
#endif

#include <string>

namespace scopira
{
  namespace tool
  {
    class dir_iterator;
  }
}

/**
 * iterators over a directory of file names
 *
 * @author Aleksander Demko
 */
class scopira::tool::dir_iterator
{
  private:
    bool dm_failed;                  /// currently failed?
#ifdef PLATFORM_win32
    HANDLE dm_dir;
    WIN32_FIND_DATA dm_dirdata;
    bool dm_haveprime;
#else
    DIR *dm_dir;                     /// DIR handle
#endif

  public:
    /// default ctor
    dir_iterator(void);
    /// dtor
    ~dir_iterator();

    /// are we in a failed state?
    bool failed(void) const { return dm_failed; }

    /**
     * Opens a directory for iteration.
     * @param dirname the directory to open
     * @return true on success
     * @author Aleksander Demko
     */ 
    bool open(const std::string& dirname);
    /// closes the current iteration
    void close(void);

    /**
     * Get the next filename. returns true if it was successfull
     * -- put this in a while loop
     *
     * @param out the output string
     * @return true if successful
     * @author Aleksander Demko
     */
    bool next(std::string& out);
};

#endif

