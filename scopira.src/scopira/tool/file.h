
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

#ifndef __INCLUDED_SCOPIRA_TOOL_FILE_H__
#define __INCLUDED_SCOPIRA_TOOL_FILE_H__

#include <sys/stat.h>

#include <string>

#include <scopira/tool/export.h>

namespace scopira
{
  namespace tool
  {
    class file;

    const char dir_seperator_c = '/';

    /// gets the username of the logged in system, may return "" (but never null)
    const char * get_username(void);
    /// gets the user's home directory, may return "." (but never null)
    const char * get_homedir(void);
  }
}

/**
 * File class. use to get information about a file on disk
 * (kinda like the Java class by the same name)
 *
 * This should probably be eventually upgraded to stat64 and int64_t (especialy
 * for size()).
 *
 * @author Aleksander Demko
 */
class scopira::tool::file
{
  public:
    typedef ::off_t offset_t;
  private:
    //const std::string m_filename;   // dont need this?
    bool dm_error;       // was there an error on stat? if so, reading from m_stat is useless
    struct stat dm_stat;

  public:
    /// default ctor and copy are fine
    SCOPIRA_EXPORT file(void);

    /**
     * ctor. this object is mostly const-ish
     *
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT file(const std::string &fname);

    /**
     * set it to a new file
     *
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT void set(const std::string &fname);

    /**
     * Does this "file" exist? In this context, file can be anything (eg directory, symlink, etc)
     *
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT bool exists(void) const;

    /**
     * Is this a file?
     *
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT bool is_file(void) const;

    /**
     * Is it a directory?
     *
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT bool is_dir(void) const;

    /**
     * return the size, in bytes
     *
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT offset_t size(void) const;

    /**
     * check if the given path is nice. nice means, no "/."
     *
     * @return true if string is clean/ok
     */
    SCOPIRA_EXPORT static bool check_path(const std::string& path);

    /**
     * check if the file ends (case insensitivly) with the given exntesion.
     * ext must be all upper case. Example: ".XML".
     * The "extension" may have multiple periods (this function just does
     * a end-string comparison -- periods have no special meaning)
     *
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT static bool has_ext(const std::string &fname, const std::string &ext);
    
    /**
     * expand the give path for home directory references.
     * in particular, [0] == '~', replace it with $HOME (home directory)
     *
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT static void expand_homedir(std::string &path);
    
    /**
     * Find all references to $ENVVAR and replace them
     * with the corresponding variable from the environment.
     * 
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT static void expand_envvar(std::string &path);
    
    /**
     * Splits a full path from its extension.
     * For example, /path/filename.ext
     * will be split into /path/filename
     * and .ext.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT static void split_ext(const std::string &fullname, std::string &sansext, std::string &ext);
    
    /**
     * Splits a filename from it's path.
     * For example, /path/filename.ext becomes
     * /path/ and filename.ext.
     *
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT static void split_path(const std::string &fullname, std::string &path, std::string &name);

    /**
     * Wrapper around stdlib realpath(). Mainly, removes the trailing / or \,
     * removes . or .. and resolves all symbolice links.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT static std::string realpath(const std::string &name);
    
    /**
     * Creates a directory.
     *
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT static void create_directory(const std::string &dirname);
};

#endif

