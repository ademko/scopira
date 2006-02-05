
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
    /**
     * This simply does push_configstack and pop_configstack
     * in its ctor/dtor
     * @author Aleksander Demko
     */ 
    class configstack_level
    {
      public:
        /// ctor
        configstack_level(void)
        { instance()->push_configstack(); }
        ~configstack_level(void)
        { instance()->pop_configstack(); }
    };
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
      std::string pm_val;
      bool pm_disk;
      int pm_stacklevel;
      std::auto_ptr<value_t> pm_stack;

      value_t(void) : pm_disk(false), pm_stacklevel(0) { }
      value_t(const std::string &val, int stacklevel) : pm_val(val), pm_disk(false),
        pm_stacklevel(stacklevel) { }
      value_t(const value_t &src) : pm_val(src.pm_val), pm_disk(src.pm_disk),
        pm_stacklevel(src.pm_stacklevel),
        pm_stack(std::auto_ptr<value_t>(src.pm_stack.get() ? new value_t(*src.pm_stack) : 0)) { }
      value_t & operator = (const value_t &src) {
        pm_val = src.pm_val;
        pm_disk = src.pm_disk;
        pm_stacklevel = src.pm_stacklevel;
        pm_stack = std::auto_ptr<value_t>(src.pm_stack.get() ? new value_t(*src.pm_stack) : 0);
        return *this;
      }
    };

    typedef std::map<std::string, value_t> configmap_t;
    configmap_t dm_configmap;
    int dm_currentstacklevel;
    std::string dm_emptrystring;
    std::string dm_appctx;
    std::string dm_configdir;

    scopira::tool::count_ptr<objecttype> dm_root_objecttype;

    std::vector<scopira::tool::count_ptr< scopira::tool::dll > > dm_dynlibs;
    
#ifdef PLATFORM_win32
    scopira::tool::net_loop dm_netlooper;
#endif

  private:
    /**
     * Push the current config stack level
     *
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT void push_configstack(void);
    /**
     * Pop the current config task level
     *
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT void pop_configstack(void);

  private:
    void parse_config(int argc, char **argv);
    void parse_config_file(const std::string &filename, bool cryonerrors);
    void parse_config_string(const std::string &s, bool newlinesonly);
    void parse_config_pair(const std::string &s);
    void process_config(void);
    void process_lib(const std::string &libname);
    void process_libdir(const std::string &dirname);
};

#endif

