
/*
 *  Copyright (c) 2003-2004    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#include <scopira/tool/file.h>

#include <scopira/tool/output.h>    // for OUTPUT << xxx, only in debug mode

#include <scopira/tool/util.h>
#include <scopira/tool/platform.h>

#include <ctype.h>

#ifdef PLATFORM_win32
// disable depreacted warnings
#pragma warning(disable:4996)
#endif

//BBlibs
//BBtargets libscopira.so

using namespace scopira;

const char * scopira::tool::get_username(void)
{
  const char *cc = ::getenv("USER");

  if (!cc)
    cc = ::getenv("USERNAME");   //win32

  if (cc)
    return cc;
  else
    return "";
}

const char * scopira::tool::get_homedir(void)
{
  const char *cc = ::getenv("HOME");

  if (!cc)
    cc = ::getenv("USERPROFILE");   //win32

  if (cc)
    return cc;
  else
    return ".";
}

tool::file::file(void)
  : dm_error(false)
{
}
  
tool::file::file(const std::string &fname)
  : dm_error(false)
{
  set(fname);
}

void tool::file::set(const std::string &fname)
{
  dm_error = stat(fname.c_str(), &dm_stat) != 0;
}

bool tool::file::exists(void) const
{
  if (dm_error)
    return false;
  else
#ifdef PLATFORM_win32
    return (dm_stat.st_mode & _S_IFREG) != 0;
#else
    return S_ISREG(dm_stat.st_mode);
#endif
}

bool tool::file::is_dir(void) const
{
  if (dm_error)
    return false;
  else
#ifdef PLATFORM_win32
    return (dm_stat.st_mode & _S_IFDIR) != 0;
#else
    return S_ISDIR(dm_stat.st_mode);
#endif
}

tool::file::offset_t tool::file::size(void) const
{
  if (dm_error)
    return 0;
  else
    return dm_stat.st_size;
}

bool tool::file::check_path(const std::string& path)
{
  std::string::size_type i; // position of the last '/' character
  
  if (path.empty())
    return true;
  
  if (path[0] == '.')
    return false;

  for (i=1; i < path.size(); i++)
    if (path[i] == '.' && path[i-1] == '/')
      return false;

  return true;
}

bool tool::file::has_ext(const std::string &fname, const std::string &ext)
{
  if (fname.size() < ext.size())
    return false;

  std::string myext(tool::uppercase(fname.substr(fname.size()-ext.size(), ext.size())));

  return ext == myext;
}

void tool::file::expand_homedir(std::string &path)
{
  if (path.empty() || path[0] != '~')
    return;   // nothing needed

  path = get_homedir() + path.substr(1);
}

void tool::file::expand_envvar(std::string &path)
{
  bool didsomething = true;
  std::string::size_type x, e;
  const char *VAR;

  while (didsomething) {
    didsomething = false;

    // find a $
    x = path.find('$');

    if (x != std::string::npos) {
      didsomething = true;

      // find the first non-alpah numeric
      e = x + 1;

      while (e<path.size() && (::isalnum(path[e]) || path[e] == '_'))
        ++e;

      if (x+1 == e)
        VAR = 0;    // we didnt even move!
#ifdef PLATFORM_64
      else if (x+3 == e && path[x+1] == '6' && path[x+2] == '4')
        VAR = "64";
#else
      else if (x+3 == e && path[x+1] == '3' && path[x+2] == '2')
        VAR = "32";
#endif
      else
        VAR = ::getenv(path.substr(x+1, e - (x+1)).c_str());

      // finally, do the replace
      path.replace(x, e-x, VAR?VAR:"");
    }
  }//while
}

void tool::file::split_ext(const std::string &fullname, std::string &sansext, std::string &ext)
{
  int dot;

  dot = fullname.find_last_of('.');

  if (dot == std::string::npos) {
    sansext = fullname;
    ext.clear();
    return;
  }

  sansext = fullname.substr(0, dot);
  ext = fullname.substr(dot);
}

void tool::file::split_path(const std::string &fullname, std::string &path, std::string &name)
{
  int slash;
  slash = fullname.find_last_of(dir_seperator_c);
   
#ifdef PLATFORM_win32
  if (slash == std::string::npos)
    slash = fullname.find_last_of('\\');
#endif
   
  if (slash == std::string::npos) {
    name = fullname;
    path.clear();
    return;
  }

  ++slash;    // move to one after the slash

  path = fullname.substr(0, slash);
  if (slash == fullname.size())
    name.clear();
  else
    name = fullname.substr(slash);
}

void tool::file::create_directory(const std::string &dirname)
{
#ifdef PLATFORM_win32
  CreateDirectory(dirname.c_str(), 0);
#else
  ::mkdir(dirname.c_str(), 0700);
#endif

}
