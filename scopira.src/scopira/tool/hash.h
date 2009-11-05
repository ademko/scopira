
/*
 *  Copyright (c) 2005    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */
 
#ifndef __INCLUDED__SCOPIRA_TOOL_HASH_H__
#define __INCLUDED__SCOPIRA_TOOL_HASH_H__

#include <scopira/tool/array.h>
#include <scopira/tool/flow.h>

namespace scopira
{
  namespace tool
  {
    class hashoflow;

    /**
     * Returns the string representation of the given hash.
     *
     * @author Aleksander Demko
     */ 
    std::string hexhash(const scopira::tool::basic_array<byte_t> &hash);

    /**
     * Closes the given hashoflow and returns the hash, as a hex string
     * @author Aleksander Demko
     */
    std::string close_and_hexhash(hashoflow &o);

    //this is declared at the bottom of this .h file
    //std::string hexhash_string(const std::string &s, int type = scopira::tool::hashoflow::sha1_c);
  }
}

/**
 * Secure hashing.
 *
 * @author Aleksander Demko
 */ 
class scopira::tool::hashoflow : public scopira::tool::oflow_i
{
  public:
    enum {
      sha1_c = 1,
    };

  public:
    /// ctor, does an implicit open
    hashoflow(int alg = sha1_c);
    /// dtor
    virtual ~hashoflow();

    /// in failed state?
    virtual bool failed(void) const { return dm_impl != 0; }

    /// writes a few bytes
    virtual size_t write(const byte_t* _buf, size_t _size);

    /// opens a new hashing session, closing any previous one, if any
    void open(int alg = sha1_c);
    /// close the current link, throwing away the hash
    void close(void);
    /// close the current link, storing the final hash into the given buffer
    void close(scopira::tool::basic_array<byte_t> &hash);

  private:
    void *dm_impl;
};

namespace scopira
{
  namespace tool
  {
    /**
     * Returns a hex hash of the given string
     * @author Aleksander Demko
     */ 
    std::string hexhash_string(const std::string &s, int type = scopira::tool::hashoflow::sha1_c);
  }
}

#endif

