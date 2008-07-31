
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
 
#ifndef __INCLUDED_SCOPIRA_TOOL_BINFLOW_H__
#define __INCLUDED_SCOPIRA_TOOL_BINFLOW_H__

#include <scopira/tool/flow.h>
#include <scopira/tool/export.h>

namespace scopira
{
  namespace tool
  {
    class biniflow;
    class binoflow;

    class bin64iflow;
    class bin64oflow;
  }
}

/**
 * binary input stream.
 *
 * @author Aleksander Demko
 */
class scopira::tool::biniflow : public scopira::tool::itflow_i
{
  protected:
    /// input flow
    count2_ptr< iflow_i > dm_in;
    
  public:
    /**
     * Constructor.
     *
     * @param doref should this class reference count the source stream
     * @param in source stream to use
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT biniflow(bool doref, iflow_i* in);
    /// destructor
    SCOPIRA_EXPORT virtual ~biniflow(void);

    /// are we in a failed state?
    SCOPIRA_EXPORT virtual bool failed(void) const;

    /// read raw block data, returns num read in
    SCOPIRA_EXPORT virtual size_t read(byte_t* _buf, size_t _maxsize);

    SCOPIRA_EXPORT virtual bool read_bool(bool&);
    SCOPIRA_EXPORT virtual bool read_char(char&);
    SCOPIRA_EXPORT virtual bool read_short(short&);
    SCOPIRA_EXPORT virtual bool read_int(int&);
    SCOPIRA_EXPORT virtual bool read_size_t(size_t&);
    SCOPIRA_EXPORT virtual bool read_int64_t(int64_t&);
    SCOPIRA_EXPORT virtual bool read_long(long&);
    SCOPIRA_EXPORT virtual bool read_float(float&);
    SCOPIRA_EXPORT virtual bool read_double(double&);
    SCOPIRA_EXPORT virtual bool read_string(std::string&);

    /// opens a new link
    SCOPIRA_EXPORT void open(iflow_i* in);
    /// close the current link
    SCOPIRA_EXPORT void close(void);
};

/**
 * binary output stream. decendant classes must still
 * implement write_binary
 *
 * @author Aleksander Demko
 */
class scopira::tool::binoflow : public scopira::tool::otflow_i
{
  protected:
    /// output link
    count2_ptr< oflow_i > dm_out;

  public:
    /**
     * Constructor.
     *
     * @param doref should this class reference count the source stream
     * @param in source stream to use
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT binoflow(bool doref, oflow_i* in);
    /// destructor
    SCOPIRA_EXPORT virtual ~binoflow(void);
      
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
};

/**
 * A biniflow variant that ALWAYS writes formats in 64-bit format, regardless
 * of the current architecture.
 *
 * @author Aleksander Demko
 */
class scopira::tool::bin64iflow : public scopira::tool::biniflow
{
  public:
    /**
     * Constructor.
     *
     * @param doref should this class reference count the source stream
     * @param in source stream to use
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT bin64iflow(bool doref, iflow_i* in);

    SCOPIRA_EXPORT virtual bool read_size_t(size_t&);
    SCOPIRA_EXPORT virtual bool read_long(long&);
};


/**
 * A binoflow variant that always writes files in 64-bit format, regarldless
 * of the current architecture.
 *
 * @author Aleksander Demko
 */
class scopira::tool::bin64oflow : public scopira::tool::binoflow
{
  public:
    /**
     * Constructor.
     *
     * @param doref should this class reference count the source stream
     * @param in source stream to use
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT bin64oflow(bool doref, oflow_i* in);

    SCOPIRA_EXPORT virtual void write_size_t(size_t val);
    SCOPIRA_EXPORT virtual void write_long(long val);
};

#endif

