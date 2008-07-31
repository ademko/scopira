
/*
 *  Copyright (c) 2004-2005    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#ifndef __INCLUDED_SCOPIRA_CORE_LOOP_H__
#define __INCLUDED_SCOPIRA_CORE_LOOP_H__

#include <string>
#include <vector>
#include <map>

#include <scopira/tool/export.h>
#include <scopira/tool/output.h>
#include <scopira/tool/fileflow.h>
#include <scopira/tool/netflow.h>
#include <scopira/tool/objflowloader.h>

namespace scopira
{
  namespace tool
  {
    class dll; //fwd
  }
  /**
   * General non-ui stuff useful to all Scopira applicationa
   * @author Aleksander Demko
   */ 
  namespace core
  {
    class basic_loop;

    class objecttype; //fwd
  }
}

/**
 * A basic, non-gui mainloop.
 * This does objloader and OUTPUT stream preparation, aswell
 * as config parameter parsing.
 *
 * Build in options, so far:
 *  debug=1   show some debug information on startup
 *  noui=1    disable any gui (only lab_loop looks at this)
 *  config=filename sepecifies a text based config file to parse (standard key=value format, just like the command line)
 *  noconsole=1 closes the terminal on win32, ignored option on other platforms
 *  lib=filename.so  loads a dynamic library, explictly
 *  libdir=directory process all the .so files in the given directory with _lib
 *
 *
 * These arent implemented, but are possibilities....
 *  configdir=dir  override whole config dir?
 *
 * @author Aleksander Demko
 */ 
class scopira::core::basic_loop : public virtual scopira::tool::object
{
  public:
    /// returns the static instance
    static basic_loop * instance(void) { return dm_instance; }

    /**
     * Basic constructor.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT basic_loop(int &argc, char **&argv);

    /// destructor
    SCOPIRA_EXPORT ~basic_loop();

    /**
     * Enumerates all the current config keys
     * to the given output string vector.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT void list_config(std::vector<std::string> &out) const;

    /**
     * Does the configutation system have the given key set?
     *
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT bool has_config(const std::string &key) const;

    /**
     * Get the vlaue of the given configutation flag. Empty
     * string will be returned if it's not set.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT const std::string & get_config(const std::string &key) const;

    /**
     * Gets the configutation value of the given key and put it into out.
     * If there is no value for the given key, out is untouched and false
     * is returned.
     * Non name-valued parameters (like lone files names and the program name
     * itself will be stored in the keys param, param+, param++, param+++, etc.
     * The program nameitself is usually param, so you'll want to start with param+.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT bool get_config(const std::string &key, std::string &out) const;

    /**
     * Sets the value of the given configurations flag.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT void set_config(const std::string &key, const std::string &val);

    /**
     * Sets the value of the given configurations flag, but only
     * if it was unset previously.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT void set_config_default(const std::string &key, const std::string &val);

    /**
     * Sets the value of the given configurations flag and mark the item
     * for saving to disk (and thus reloading next time) for the next commit_config().
     *
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT void set_config_save(const std::string &key, const std::string &val);


    /**
     * Commit all the flags that have been marked for saving to disk now.
     * This is also called by lab_loop's destructor.
     *
     * If none have been marked, this will do nothing (not even create an empty file).
     *
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT void commit_config(void);

    /**
     * Returns a string that has been "expanded", useful for various configuration
     * settings. For example, ~ will be expanded to the users home directory,
     * and $ENV style environmental variables will be expanded.
     *
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT static std::string config_expand(const std::string &s);

    /**
     * Get the root objecttype in the type-information-tree.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT objecttype * get_root_objecttype(void);

    /**
     * Gets the context name. This is the name of the application, calculated
     * as arv[0] sans path and extension.
     *
     * @author Aleksander Demko
     */
    const std::string &get_context_name(void) const { return dm_appctx; }

    /**
     * Returns the location of all the configuration files.
     *
     * @author Aleksander Demko
     */ 
    const std::string &get_config_dir(void) const { return dm_configdir; }

    /**
     * Saves the given object to disk.
     * name cannot be "".
     * The object may be null.
     *
     * Returns true on success (although, if it fails, I'm not how you'd want to handle it)
     *
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT bool save_object(const std::string &name, scopira::tool::object *o);

    /**
     * Reads an object from disk into the given out ptr.
     * Returns true on success.
     * Note that this function may return true and set out to null, if the given
     * object saved to disk was infact a null object.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT bool load_object(const std::string &name, scopira::tool::count_ptr<scopira::tool::object> &out);

  private:
    /// singleton instance
    SCOPIRA_EXPORT static basic_loop *dm_instance;

#ifndef NDEBUG
    // used to balance reference counts in debug mode only
    scopira::tool::objrefcounter dm_refcounter;
#endif
    scopira::tool::fileflow dm_default_out;
    scopira::tool::output_stack dm_outputstack;
    scopira::tool::count_ptr<scopira::tool::objflowloader> dm_objloader;

    struct value_t
    {
      std::string pm_val;                 // the current value
      bool pm_disk;                       // is thie value marked for saving
      int pm_creatorid;                  // the stack level that this value was created at (used in config stacks)

      value_t(void) : pm_disk(false), pm_creatorid(0) { }
      value_t(const std::string &val, int creatorid) : pm_val(val), pm_disk(false),
        pm_creatorid(creatorid) { }
    };

    typedef std::map<std::string, value_t> configmap_t;
    configmap_t dm_configmap;

    // the current creator id to assign to new values
    // its incremented between each "phase" or level or context of parsing
    struct parse_context_t {
      int pm_creatorid;
      bool pm_disk;       // mark new nodes for writing (sets the value_t->pm_disk)
    };
    
    parse_context_t dm_parse_context;

    std::string dm_emptrystring;
    std::string dm_appctx;
    std::string dm_configdir;

    scopira::tool::count_ptr<objecttype> dm_root_objecttype;

    std::vector<scopira::tool::count_ptr< scopira::tool::dll > > dm_dynlibs;
    
#ifdef PLATFORM_win32
    scopira::tool::net_loop dm_netlooper;
#endif

  private:
    void parse_config(int argc, char **argv);
    void parse_config_file(const std::string &filename, bool cryonerrors);
    void parse_config_string(const std::string &s, bool newlinesonly);
    void parse_config_pair(const std::string &s);
    void process_config(void);
    void process_lib(const std::string &libname);
    void process_libdir(const std::string &dirname);
};

/**
  \page scopiracoreloop The main loop

  \section introsec Introduction

  Scopira uses the concept of stackable singleton objects "loops" to manage the
  core global variables needed various subsystem layers. Loop objects
  are either implicitly created, created by being a base class of another loop
  and directly instantied by you in your main() function.

  You don't always need a loop object, however.

  The scopira::core::basic_loop is the core loop in almost all loops. It can also
  be used directly but may also be a base class by variou other groups.

  \section features Features

  scopira::core::basic_loop provides the following key features:
    - Command line and configuration file parsing and processing (get_config(), etc)
    - Dynamic library loading (plug-ins) via lib= parameters.

  basic_loop is a singleton. It's instance can always be accessed via
  scopira::core::basic_loop::instance().

  \section examplesec Example

  A basic, non-gui Scopira program would be as follows:

  \code
  #include <scopira/core/loop.h>

  int main(int argc, char **argv)
  {
    scopira::core::basic_loop l(argc, argv);

    return 0;
  }
  \endcode


*/

/**
  \page scopiraconfigpage Command line parameter processing

  Scopira based applications can take command line options. All options are in the form of option=value

  \section sysoptsec System Options

  System options are options that are available to all Scopira based applications. They are:

    - debug=1 show some debug information on startup (like the object registration tree)
    - noui=1 (Lab applications only) disable GUI, and use only paramters supplied via the command line
    - config=filename sepecifies a config file to parse (one key=value per line)
    - lib=filename.so loads a dynamic library (a plug in)
    - libdir=directory process all the .so files in the given directory with lib=
    - mpi=1 (MPI enabled applications only) enable and use MPI 

  Your application may introduce other options. For example, project based applications may also have:

    - project=file load the given projectfile upon start up

  \section configfilessec Configuration Files

  In addition to the command line, Scopira based applications also search a few additional locations for paramters. These locations are based on your APPNAME. The APPNAME is the name of the binary that was run, sans extension and path. For example, you you run /usr/bin/evident.exe, your APPNAME is simply, evident.

  The following locations are searched in the following order:

    - environment variable: SCOPIRA_CONFIG
    - environment variable: APPNAME_CONFIG (in this context, your APPNAME will be converted to upper case)
    - file: ~/.scopira/config
    - file: ~/.scopira/APPNAME.config
    - file: ~/.scopira/APPNAME.config.saved (this file is created and maintained by Scopira programs)
    - command line

  \section examplesec An example

  \verbatim
  ./scopira.exe debug=1
  echo lib=/path/to/some/libplugin.so >> ~/.scopira/scopira.config
  export SCOPIRA_CONFIG=mpi=1
  \endverbatim

*/

#endif

