
/*
 *  Copyright (c) 2002-2007    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#ifndef __INCLUDED_SCOPIRA_TOOL_PRINTFLOW_H__
#define __INCLUDED_SCOPIRA_TOOL_PRINTFLOW_H__

#include <scopira/tool/flow.h>
#include <scopira/tool/export.h>
#include <scopira/tool/export.h>

namespace scopira
{
  namespace tool
  {
    class printiflow;
    class printoflow;

    /// printf style
    SCOPIRA_EXPORT void flow_printf(oflow_i& o, const char* fmt, ...);
  }
}

/**
 * A type flow that helps parse ASCII input streams.
 *
 * Please note that this not a "symmetrical" tflow like all the others, that
 * is you shouldn't use it in building a serialization stack. ie. write_string
 * by printoflow may not always translate to the same read_string in printiflow.
 *
 * @author Aleksander Demko
 */
class scopira::tool::printiflow : public scopira::tool::itflow_i
{
  protected:
    /// is it whitespace?
    static bool is_whitespace(byte_t b);
    /// advance stream to next non whitespace and return it
    byte_t non_whitespace(void);
    /// input flow
    count2_ptr< iflow_i > dm_in;
    
  public:
    /**
     * Constructor.
     *
     * @param doref should this class ref count in
     * @param in the input stream to use
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT printiflow(bool doref, iflow_i* in);
    /// destructor
    SCOPIRA_EXPORT virtual ~printiflow(void);

    /// are we in a failed state?
    SCOPIRA_EXPORT virtual bool failed(void) const;

    /// read raw block data, returns num read in
    SCOPIRA_EXPORT virtual size_t read(byte_t* _buf, size_t _maxsize);

    // parsing routines

    SCOPIRA_EXPORT virtual bool read_bool(bool&);
    SCOPIRA_EXPORT virtual bool read_char(char&);
    SCOPIRA_EXPORT virtual bool read_short(short&);
    SCOPIRA_EXPORT virtual bool read_int(int&);
    SCOPIRA_EXPORT virtual bool read_size_t(size_t&);
    SCOPIRA_EXPORT virtual bool read_int64_t(int64_t&);
    SCOPIRA_EXPORT virtual bool read_long(long&);
    SCOPIRA_EXPORT virtual bool read_float(float&);
    SCOPIRA_EXPORT virtual bool read_double(double&);

    /**
     * Reads a string. Specifically, the next line of next up to the line terminator.
     * UNIX and Win32 line terminators are supported. The line terminator(s) will NOT
     * be in the resulting string.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT virtual bool read_string(std::string&);

    /// opens a new link
    SCOPIRA_EXPORT void open(iflow_i* in);
    /// close the current link
    SCOPIRA_EXPORT void close(void);

    // extra routines

    /**
     * Reads a string. Specifically, read the next _max chars into the string.
     * Anything other than _max chars is considered failure.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT virtual bool read_string(std::string& ret, int _max);

    /**
     * Reads the next "word", that is stream of non-whitespace characters.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT bool read_string_word(std::string &ret);
};

/**
 * binary output stream. decendant classes must still
 * implement write_binary
 *
 * @author Aleksander Demko
 */
class scopira::tool::printoflow : public scopira::tool::otflow_i
{
  protected:
    /// output link
    count2_ptr< oflow_i > dm_out;

  public:
    /**
     * constrcutor
     *
     * @param doref should this class reference count in
     * @param in the output stream to use. this object will "own" it
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT printoflow(bool doref, oflow_i* in);
    /// destructor
    SCOPIRA_EXPORT virtual ~printoflow(void);
      
    /// are we in a failed state?
    SCOPIRA_EXPORT virtual bool failed(void) const;

    /// write a raw block of data
    SCOPIRA_EXPORT virtual size_t write(const byte_t* _buf, size_t _size);

    SCOPIRA_EXPORT virtual void write_bool(bool val);
    SCOPIRA_EXPORT virtual void write_char(char val);
    SCOPIRA_EXPORT virtual void write_short(short val);
    SCOPIRA_EXPORT virtual void write_int(int val);
    SCOPIRA_EXPORT virtual void write_size_t(size_t val);
    SCOPIRA_EXPORT virtual void write_int64_t(int64_t val);
    SCOPIRA_EXPORT virtual void write_long(long val);
    SCOPIRA_EXPORT virtual void write_float(float val);
    SCOPIRA_EXPORT virtual void write_double(double val);
    SCOPIRA_EXPORT virtual void write_string(const std::string& val);

    /// opens a new link
    SCOPIRA_EXPORT void open(oflow_i* in);
    /// close the current link
    SCOPIRA_EXPORT void close(void);

    /// printf style
    SCOPIRA_EXPORT void printf(const char* fmt, ...);
};

#endif

