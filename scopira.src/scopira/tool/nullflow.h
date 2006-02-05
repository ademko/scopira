
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
 
#ifndef __INCLUDED_SCOPIRA_TOOL_NULLFLOW_H__
#define __INCLUDED_SCOPIRA_TOOL_NULLFLOW_H__

#include <scopira/tool/flow.h>
#include <scopira/tool/export.h>

namespace scopira
{
  namespace tool
  {
    class nullflow;

    class countoflow;
  }
}

/**
 * This flow accepts all output and produces no input.
 *
 * @author Aleksander Demko
 */
class scopira::tool::nullflow : public scopira::tool::iflow_i, public scopira::tool::oflow_i
{
  public:
    /// default constructor
    SCOPIRA_EXPORT nullflow(void);

    /// are we in a failed state. always false (or should this be truee?
    virtual bool failed(void) const { return false; }

    /// read raw block data, returns num read in
    virtual size_t read(byte_t* _buf, size_t _maxsize) { return 0; }
    /// write a raw block of data
    virtual size_t write(const byte_t* _buf, size_t _size) { return 0; }
};

/**
 * An output flow that simply counts all the bytes that have
 * been sent through it.
 *
 * @author Aleksander Demko
 */
class scopira::tool::countoflow : public scopira::tool::oflow_i
{
  private:
    size_t dm_count;
  public:
    /// default constructor
    SCOPIRA_EXPORT countoflow(void);

    /// are we in a failed state. always false (or should this be truee?
    virtual bool failed(void) const { return false; }

    /// write a raw block of data
    virtual size_t write(const byte_t* _buf, size_t _size) { dm_count += _size; return _size; }

    /// gets the current count
    size_t get_count(void) const { return dm_count; }
    /// sets the current count
    void set_count(size_t new_count = 0) { dm_count = new_count; }
};

#endif

