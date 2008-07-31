
/*
 *  Copyright (c) 2003-2008    National Research Council
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
#include <scopira/tool/export.h>

#ifdef PLATFORM_win32
#include <windows.h>
#else
#include <dirent.h>
#endif

#include <string>

// THIS FILE HAS BEEN FULLY DOCUMENTED

namespace scopira
{
  namespace tool
  {
    class dir_iterator;
  }
}

/**
 * This class iterates of a collection of files in a directory, giving
 * you a list of file names.
 *
 * Typical usage of this class:
 *
 * @code
 *   dir_iterator dd;
 *   std::string filename;
 *
 *   if (dd.open("somedir"))
 *     while (dd.next(filename))
 *       OUTPUT << "A filename: " << filename << '\n';
 * @endcode
 *
 * @author Aleksander Demko
 */
class scopira::tool::dir_iterator
{
  private:
    bool dm_failed;                  // currently failed?
#ifdef PLATFORM_win32
    HANDLE dm_dir;
    WIN32_FIND_DATA dm_dirdata;
    bool dm_haveprime;
#else
    DIR *dm_dir;                     // DIR handle
#endif

  public:
    /**
     * Constructor.
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT dir_iterator(void);
    SCOPIRA_EXPORT ~dir_iterator();

    /**
     * Is this iterator in a failed state?
     *
     * @author Aleksander Demko
     */ 
    bool failed(void) const { return dm_failed; }

    /**
     * Opens a directory for iteration.
     *
     * @param dirname the directory to open
     * @return true on success
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT bool open(const std::string& dirname);
    /**
     * Closes the current iteration.
     *
     * You typically never need to call this as the iterator
     * will close the stream upon its destruction or if you
     * open another iteration.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT void close(void);

    /**
     * Get the next filename in the iteration.
     * Only the filename will be returned, there will be no path
     * element attached.
     * You typically put this call in a while loop.
     * The dir_iterator must have previously been succesfully opened.
     *
     * @param out the output string (useful only if this call is succesful)
     * @return true if successful, false on failure (for example, if there are no more files in the directory)
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT bool next(std::string& out);
};

#endif

