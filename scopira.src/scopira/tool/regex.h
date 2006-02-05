
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

#ifndef __INCLUDED__SCOPIRA_TOOL_REGEX_H__
#define __INCLUDED__SCOPIRA_TOOL_REGEX_H__
#include <scopira/tool/platform.h>
#ifndef PLATFORM_win32

#include <stddef.h>

#if (GCC_VERSION_MAJOR<3) || (GCC_VERSION_MAJOR==3 && GCC_VERSION_MINOR<3)
// this is a hack. gcc 3 turns on some regex() prototype stuff that
// gcc3 likes, but g++<3.3 doesnt. predefine this trickery here
#define __restrict_arr
#endif

#include <regex.h>

#include <string>

#include <scopira/tool/array.h>

namespace scopira
{
  namespace tool
  {
    class regmatch;
    class regex;
  }
}

/**
 * where the results of a match are stored
 *
 * @author Aleksander Demko
 */ 
class scopira::tool::regmatch
{
  public:
    /// ctor
    explicit regmatch(size_t sz);

    /// is this particular match valid?
    bool is_valid(size_t idx) const { return dm_ary[idx].rm_so != -1; }

    /// get the base of a match
    size_t begin(size_t idx) { return dm_ary[idx].rm_so; }
    /// get the end of the math
    size_t end(size_t idx) { return dm_ary[idx].rm_eo; }

    /// gets the size
    size_t size(void) const { return dm_ary.size(); }

    /// gets the number of matches
    size_t size_match(void) const;

    /// convinience, get the substring
    std::string substr(const std::string &base, size_t idx)
      { assert(is_valid(idx)); return base.substr(begin(idx), end(idx)-begin(idx)); }

    friend class regex;

  private:
    /// the actual array
    scopira::tool::basic_array<regmatch_t> dm_ary;
};

/**
 * a compiled regular expression
 *
 * @author Aleksander Demko
 */
class scopira::tool::regex
{
  public:
    /// default ctor, call compile() after
    regex(void);
    /// initing ctor that calls compile()
    regex(const std::string &pattern);
    /// dtor
    ~regex();

    /// in a failed state?
    bool failed(void) const { return !dm_valid; }

    /// compile the given pattern. clears any previous ones, true on success
    bool compile(const std::string &pattern);

    /// check for a match (inline this?)
    bool match(const std::string &s) const;

    /// check for a match, spitting out the output into the match class
    bool match(const std::string &s, regmatch &mat) const;

  private:
    bool dm_valid;
    regex_t dm_re;

  private:
    // not implemented - not legal
    regex(const regex &);
    // not implemented - not legal
    void operator = (const regex&);
};

#endif
#endif

