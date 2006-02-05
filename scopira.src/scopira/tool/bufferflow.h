
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
 
#ifndef __INCLUDED_SCOPIRA_TOOL_BUFFERFLOW_H__
#define __INCLUDED_SCOPIRA_TOOL_BUFFERFLOW_H__

#include <scopira/tool/flow.h>
#include <scopira/tool/export.h>

#include <assert.h>

#include <vector>

namespace scopira
{
  namespace tool
  {
    class bufferflow;
    class bufferiflow;
    class bufferoflow;
  }
}

/**
 * A flow that reads and writes to an internal byte buffer (ie
 * to memory).
 *
 * @author Aleksander Demko
 */
class scopira::tool::bufferflow : public scopira::tool::iflow_i, public scopira::tool::oflow_i
{
  public:
    typedef std::vector<tool::flow_i::byte_t> byte_buffer_t;

  private:
    /// disallow the use of assigning bufferflows
    bufferflow& operator=(const bufferflow& rhs) { assert(false); return *this; }
    /// disallow the use of copying bufferflows
    bufferflow(const bufferflow& rhs) { assert(false); }

    /// buffer index
    size_t dm_index;
    /// internal buffer
    byte_buffer_t dm_buffer;

  public:

    /// default constructor
    SCOPIRA_EXPORT bufferflow(void);
    /// destructor
    SCOPIRA_EXPORT virtual ~bufferflow(void);

    /// are we in a failed state?
    SCOPIRA_EXPORT virtual bool failed(void) const;

    /// read raw block data, returns num read in
    SCOPIRA_EXPORT virtual size_t read(byte_t* buf, size_t maxsize);

    /// write a raw block of data
    SCOPIRA_EXPORT virtual size_t write(const byte_t* buf, size_t size);

    /// returns a pointer to the internal buffer
    /// is this legal? -ademko

    /// returns the buffer as a raw c array
    byte_t* c_array(void) { return &dm_buffer[0]; }
    /// returns the buffer as a raw c array
    const byte_t* c_array(void) const { return &dm_buffer[0]; }

    /// return the size of the internal buffer
    size_t size(void) const { return dm_buffer.size(); }

    /// resets the buffer index
    SCOPIRA_EXPORT void reset(void);
    /// reserve an internal buffer
    SCOPIRA_EXPORT void reserve(size_t _size);
    /// resizes the internal buffer. this also resets the index pointer
    SCOPIRA_EXPORT void reset_resize(size_t _size);
};

/**
 * An iflow that reads from an existing byte array.
 *
 * @author Aleksander Demko
 */
class scopira::tool::bufferiflow : public scopira::tool::iflow_i
{
  protected:
    /// the data string
    const byte_t *dm_data, *dm_cur, *dm_end;

  public:
    /// opening constructor
    SCOPIRA_EXPORT bufferiflow(const byte_t *data, size_t sz);

    /// are we in a failed state?
    SCOPIRA_EXPORT virtual bool failed(void) const;

    /// read raw block data, returns num read in
    SCOPIRA_EXPORT virtual size_t read(byte_t* _buf, size_t _maxsize);
};

/**
 * A string oflow, that writes directly to an existing string.
 *
 * @author Aleksander Demko
 */
class scopira::tool::bufferoflow : public scopira::tool::oflow_i
{
  protected:
    /// the data string
    byte_t *dm_data, *dm_cur, *dm_end;

  public:
    /// opening constructor, i always append
    SCOPIRA_EXPORT bufferoflow(byte_t *data, size_t sz);

    /// are we in a failed state?
    SCOPIRA_EXPORT virtual bool failed(void) const;

    /// write a raw block of data
    SCOPIRA_EXPORT virtual size_t write(const byte_t* _buf, size_t _size);
};

#endif

