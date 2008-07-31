
/*
 *  Copyright (c) 2004    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#include <scopira/core/loop.h>

#ifdef PLATFORM_win32
// disable depreacted warnings
#pragma warning(disable:4996)
#endif

#include <ctype.h>

#include <scopira/tool/archiveflow.h>
#include <scopira/tool/output.h>
#include <scopira/tool/file.h>
#include <scopira/tool/stringflow.h>
#include <scopira/tool/util.h>
#include <scopira/tool/dll.h>
#include <scopira/tool/diriterator.h>
#include <scopira/core/register.h>
#include <scopira/core/objecttype.h>

//BBtargets libscopira.so

using namespace scopira::tool;
using namespace scopira::core;

basic_loop *basic_loop::dm_instance;

basic_loop::basic_loop(int &argc, char **&argv)
  : dm_default_out(fileflow::stdout_c, 0), dm_outputstack(&dm_default_out), dm_configdir("")
{
  assert(dm_instance == 0);
  dm_instance = this;

  dm_default_out.open(fileflow::stdout_c, 0);

  // prepare objflowloader
  dm_objloader = scopira::tool::objflowloader::instance();

  // calculate the app context name
  if (argc == 0 || !argv)
    dm_appctx = "untitled";
  else {
    std::string dummy, junk;

    file::split_path(argv[0], dummy, junk);
    file::split_ext(junk, dm_appctx, dummy);

    if (dm_appctx.empty())
      dm_appctx = "untitled";
  }

  dm_parse_context.pm_creatorid = 1;
  dm_parse_context.pm_disk = false;

  // parse the configuration stuff
  parse_config(argc, argv);

  // do default param processing
  process_config();
}

basic_loop::~basic_loop()
{
  assert(dm_instance == this);
  dm_instance = 0;

  commit_config();
}

void basic_loop::list_config(std::vector<std::string> &out) const
{
  configmap_t::const_iterator ii;

  out.clear();

  for (ii=dm_configmap.begin(); ii != dm_configmap.end(); ++ii)
    out.push_back(ii->first);
}

bool basic_loop::has_config(const std::string &key) const
{
  return dm_configmap.count(key) == 1;
}

const std::string & basic_loop::get_config(const std::string &key) const
{
  if (dm_configmap.count(key) == 1)
    return dm_configmap.find(key)->second.pm_val;
  return dm_emptrystring;
}

bool basic_loop::get_config(const std::string &key, std::string &out) const
{
  if (dm_configmap.count(key) == 1) {
    out = dm_configmap.find(key)->second.pm_val;
    return true;
  } else
    return false;
}

void basic_loop::set_config(const std::string &key, const std::string &val)
{
  dm_configmap[key] = value_t(val, dm_parse_context.pm_creatorid);   // simple insert
}

void basic_loop::set_config_default(const std::string &key, const std::string &val)
{
  if (dm_configmap.count(key) == 0)
    set_config(key, val);
}

void basic_loop::set_config_save(const std::string &key, const std::string &val)
{
  set_config(key, val);
  dm_configmap[key].pm_disk = true;
}

void basic_loop::commit_config(void)
{
  configmap_t::iterator ii;
  fileflow out;
  bool opened = false;

  for (ii=dm_configmap.begin(); ii!=dm_configmap.end(); ++ii)
    if (ii->second.pm_disk) {
      if (!opened) {
        if (!dm_configmap.empty()) {
          // create the configuration directory
          // a replacement should simply "make sure dm_configmap exists" and not assuming anything
          const char * home_dir = ::getenv("HOME");
          if (!home_dir)
            home_dir = ::getenv("USERPROFILE");   //win32
          if (home_dir)
            file::create_directory(home_dir + std::string("/.scopira"));
        }

        out.open(get_config_dir() + get_context_name() + ".config.saved", fileflow::output_c);
        if (out.failed()) {
          OUTPUT << "Failed to open config file for writing: " <<
            get_config_dir() << get_context_name() << ".config.saved";
        }
        opened = true;
      }//opened

      // ok, write the element
      out << ii->first << '=' << ii->second.pm_val << '\n';
    }
}

std::string basic_loop::config_expand(const std::string &s)
{
  std::string ret(s);

  file::expand_homedir(ret);
  file::expand_envvar(ret);

  return ret;
}

objecttype * basic_loop::get_root_objecttype(void)
{
  if (dm_root_objecttype.get())
    return dm_root_objecttype.get();
  build_objecttype_tree(dm_root_objecttype);
  assert(dm_root_objecttype.get());
  return dm_root_objecttype.get();
}

static void clean_filename(std::string &filename)
{
  for (int i=0; i<filename.size(); ++i)
    if (!isalpha(filename[i]) && !isdigit(filename[i]) && filename[i] != '_')
      filename[i] = '_';
}

bool basic_loop::save_object(const std::string &name, scopira::tool::object *o)
{
  std::string filtered_name(name);
  std::string full_name;

  clean_filename(filtered_name);

  full_name = dm_configdir + get_context_name() + "." + filtered_name + ".object";

  archiveoflow outf;
  
  outf.open(full_name, "SCOOBJ100");

  if (outf.failed())
    return false;

  outf.write_object(o);

  return true;
}

bool basic_loop::load_object(const std::string &name, scopira::tool::count_ptr<scopira::tool::object> &out)
{
  std::string filtered_name(name);
  std::string full_name;

  clean_filename(filtered_name);

  full_name = dm_configdir + get_context_name() + "." + filtered_name + ".object";

  archiveiflow inf;
  
  inf.open(full_name, "SCOOBJ100");

  if (inf.failed())
    return false;

  return inf.read_object_type<object>(out);
}

void basic_loop::parse_config(int argc, char **argv)
{
  const char *cc = ::getenv("HOME");
  std::string upper_ctx = uppercase(get_context_name());

  if (!cc)
    cc = ::getenv("USERPROFILE");   //win32

  // prep dm_configdir
  if (cc) {
    dm_configdir = cc;
    dm_configdir += "/.scopira/";
  }

  // process environtment (general)
  cc = ::getenv("SCOPIRA_CONFIG");
  if (cc) {
    dm_parse_context.pm_creatorid++;
    parse_config_string(cc, false);
  }
  // process environtment (app)
  cc = ::getenv((upper_ctx + "_CONFIG").c_str());
  if (cc) {
    dm_parse_context.pm_creatorid++;
    parse_config_string(cc, false);
  }
  // process config file (general)
  dm_parse_context.pm_creatorid++;
  parse_config_file(dm_configdir + "config", false);
  // process config file (app)
  dm_parse_context.pm_creatorid++;
  parse_config_file(dm_configdir + get_context_name() + ".config", false);
  // process config file (app, user-saved)
  dm_parse_context.pm_creatorid++;
  dm_parse_context.pm_disk = true;
  parse_config_file(dm_configdir + get_context_name() + ".config.saved", false);
  dm_parse_context.pm_disk = false;

  // in the future, add registry checks under win32? :)
  
  // finally, do the command line
  dm_parse_context.pm_creatorid = 10;
  if (argv)
    for (; *argv; ++argv)
      parse_config_pair(*argv);

  dm_parse_context.pm_creatorid = 100;
}

void basic_loop::parse_config_file(const std::string &filename, bool cryonerrors)
{
  fileflow in;

  in.open(filename, fileflow::input_c);

  if (in.failed()) {
    if (cryonerrors)
      OUTPUT << "Failed to open config file: " << filename << '\n';
    return;
  }

  stringflow out(4096);
  // read the file
  out << in;

  // parse it all (yes, the "string" is also parsed along new lines
  parse_config_string(out.str(), true);
}

void basic_loop::parse_config_string(const std::string &s, bool newlinesonly)
{
  std::vector<std::string> out;

  string_tokenize(s, out, newlinesonly?"\n":" \t\n");

  for (std::vector<std::string>::iterator ii=out.begin(); ii != out.end(); ++ii)
    parse_config_pair(*ii);
}

static void clean_win32newlines(std::string &s)
{
  int x = 0;
  
  while (x < s.size())
    if (s[x] == '\r')
      s.erase(s.begin() + x);
    else
      ++x;
}

void basic_loop::parse_config_pair(const std::string &s)
{
  std::string left, right, withplusses;
  configmap_t::iterator ii;

  // perhaps in the future add addtional += operators (or just allow multiple specifications?
  // then how do you override?)
  // and unset operator?

  if (!split_char(s, '=', left, right)) {
    left = "param";
    right = s;
  }
    
  clean_win32newlines(left);
  clean_win32newlines(right);
  file::expand_homedir(right);
  file::expand_envvar(right);

  // add + to the end of the name, to prevent same level collisions
  ii = dm_configmap.find(left);
  withplusses = left;
  while ( ii != dm_configmap.end() && ii->second.pm_creatorid == dm_parse_context.pm_creatorid ) {
    withplusses += "+";
    ii = dm_configmap.find(withplusses);
  }
  // finally, add the key
  dm_configmap[withplusses] = value_t(right, dm_parse_context.pm_creatorid);
  if (dm_parse_context.pm_disk)
    dm_configmap[withplusses].pm_disk = true;

  // check for sub processing
  if (left == "config")
    parse_config_file(right, true);
  // might aswell do lib processing here too
  // although technically we should allow multiple entries of the same key somehow
  // and process them later
  if (left == "lib")
    process_lib(right);
  if (left == "libdir")
    process_libdir(right);
}

void basic_loop::process_config(void)
{
  // do some reporting
  if (get_config("debug") == "1") {
    OUTPUT << "Configuration settings [context=" << get_context_name() <<
      ",config_dir=" << get_config_dir() << "]\n";

    for (configmap_t::iterator ii=dm_configmap.begin(); ii !=dm_configmap.end(); ++ii)
      OUTPUT << ' ' << ii->first << '=' << ii->second.pm_val << '\n';

    get_root_objecttype()->print_tree(); // debug info
  }

  // hide console?
#ifdef PLATFORM_win32
  if (get_config("noconsole") == "1")
    FreeConsole();
#endif
}

void basic_loop::process_lib(const std::string &libname)
{
  count_ptr<dll> d = new dll;

  if (!d->load_dll(libname)) {
    OUTPUT << " *** Plug-in library load failed: " << libname << " (" << d->get_error() << ")\n";
    return;
  }

  OUTPUT << " plug-in: " << libname << '\n';

  dm_dynlibs.push_back(d);
}

void basic_loop::process_libdir(const std::string &dirname)
{
  std::string filename, fullname;
  dir_iterator dir;

  filename.reserve(4000);
  fullname.reserve(4000);

  if (!dir.open(dirname))
    return;   // silent failure
  
  while (dir.next(filename)) {
    if (file::has_ext(filename, dll_ext_c)) {
      fullname = dirname;
      fullname += tool::dir_seperator_c;
      fullname += filename;

      process_lib(fullname);
    }//if
  }
}

