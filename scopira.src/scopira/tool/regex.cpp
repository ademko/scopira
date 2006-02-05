
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

#include <scopira/tool/regex.h>

#ifndef PLATFORM_win32

//BBlibs
//BBtargets libscopira.so

using namespace scopira::tool;

regmatch::regmatch(size_t sz)
  : dm_ary(sz)
{
}

size_t regmatch::size_match(void) const
{
  size_t r = 0;
  size_t i;

  for (i=0; i<size(); ++i)
    if (is_valid(i))
      r++;
    else
      return r;
  return r;
}

regex::regex(void)
  : dm_valid(false)
{
}

regex::regex(const std::string &pattern)
  : dm_valid(false)
{
  compile(pattern);
}

regex::~regex()
{
  if (dm_valid)
    ::regfree(&dm_re);
}

bool regex::compile(const std::string &pattern)
{
  if (dm_valid)
    ::regfree(&dm_re);

  dm_valid = 0 == ::regcomp(&dm_re, pattern.c_str(), REG_EXTENDED | REG_ICASE);

  return dm_valid;
}

bool regex::match(const std::string &s) const
{
  return 0 == ::regexec(&dm_re, s.c_str(), 0, 0, 0);
}

bool regex::match(const std::string &s, regmatch &mat) const
{
  return 0 == ::regexec(&dm_re, s.c_str(), mat.dm_ary.size(), mat.dm_ary.c_array(), 0);
}

#endif

