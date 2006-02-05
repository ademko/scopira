
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
 
#ifndef __INCLUDED_SCOPIRA_TOOL_STRINGFLOW_H__
#define __INCLUDED_SCOPIRA_TOOL_STRINGFLOW_H__

#include <scopira/tool/flow.h>
#include <scopira/tool/export.h>

namespace scopira
{
  namespace tool
  {
    class stringflow;
    class stringiflow;
    class stringoflow;
  }
}

/**
 * a string system file
 *
 * @author Aleksander Demko
 */
class scopira::tool::stringflow : public scopira::tool::iflow_i, public scopira::tool::oflow_i
{
  protected:
    /// current modes, 0 for no-file
    mode_t dm_mode;
    /// the data string
    std::string dm_data;
    /// the current index
    size_t dm_index;

  public:

    /// access levels
    enum {
      append_c = 1024,
      trunc_c = 2048
    };

    /// default constructor
    SCOPIRA_EXPORT stringflow(void);
    /// default, open for output with the given reserve
    SCOPIRA_EXPORT explicit stringflow(size_t res);
    /// opening constructor
    SCOPIRA_EXPORT stringflow(const std::string& data, mode_t mode);
    /// destructor
    SCOPIRA_EXPORT virtual ~stringflow();

    /// are we in a failed state?
    SCOPIRA_EXPORT virtual bool failed(void) const;

    /// read raw block data, returns num read in
    SCOPIRA_EXPORT virtual size_t read(byte_t* _buf, size_t _maxsize);
    /// write a raw block of data
    SCOPIRA_EXPORT virtual size_t write(const byte_t* _buf, size_t _size);

    /// open for writing. uses truncation
    SCOPIRA_EXPORT void open(void);
    /// opens a file
    SCOPIRA_EXPORT void open(const std::string& data, mode_t mode);
    /// close the file
    SCOPIRA_EXPORT void close(void);

    /// reserve an internal buffer
    SCOPIRA_EXPORT void reserve(size_t _size);
    /// get the current string contents
    const std::string& str(void) const { return dm_data; }
};

/**
 * a string iflow, that reads directly from an existing string
 *
 * @author Aleksander Demko
 */
class scopira::tool::stringiflow : public scopira::tool::iflow_i
{
  protected:
    /// the data string
    const std::string * dm_data;
    std::string::size_type dm_idx;

  public:
    /// opening constructor, null is valid (but unusable)
    SCOPIRA_EXPORT stringiflow(const std::string* data);

    /// are we in a failed state?
    SCOPIRA_EXPORT virtual bool failed(void) const;

    /// read raw block data, returns num read in
    SCOPIRA_EXPORT virtual size_t read(byte_t* _buf, size_t _maxsize);

    /// simple opender
    SCOPIRA_EXPORT void open(const std::string *data);
};

/**
 * a string oflow, that writes directly to an existing string
 *
 * @author Aleksander Demko
 */
class scopira::tool::stringoflow : public scopira::tool::oflow_i
{
  protected:
    /// the data string
    std::string * dm_data;

  public:
    /// opening constructor, i always append
    SCOPIRA_EXPORT stringoflow(std::string* data);

    /// are we in a failed state?
    SCOPIRA_EXPORT virtual bool failed(void) const { return false; }

    /// write a raw block of data
    SCOPIRA_EXPORT virtual size_t write(const byte_t* _buf, size_t _size);

    /// simple opender
    SCOPIRA_EXPORT void open(std::string *data);
};

#endif

