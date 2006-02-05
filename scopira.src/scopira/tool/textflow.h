
/*
 *  Copyright (c) 2002    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */
 
#ifndef __INCLUDED_SCOPIRA_TOOL_TEXTFLOW_H__
#define __INCLUDED_SCOPIRA_TOOL_TEXTFLOW_H__

#include <scopira/tool/flow.h>

namespace scopira
{
  namespace tool
  {
    class textiflow;
    class textoflow;
  }
}

/**
 * data type to ascii-like converter
 *
 * @author Aleksander Demko
 */
class scopira::tool::textiflow : public scopira::tool::itflow_i
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
     * constrcutor
     *
     * @param is the input stream to use. this object will "own" it
     * @author Aleksander Demko
     */
    textiflow(bool doref, iflow_i* in);
    /// destructor
    virtual ~textiflow(void);

    /// are we in a failed state?
    virtual bool failed(void) const;

    /// read raw block data, returns num read in
    virtual size_t read(byte_t* _buf, size_t _maxsize);

    virtual bool read_bool(bool&);
    virtual bool read_char(char&);
    virtual bool read_short(short&);
    virtual bool read_int(int&);
    virtual bool read_size_t(size_t&);
    virtual bool read_long(long&);
    virtual bool read_float(float&);
    virtual bool read_double(double&);
    virtual bool read_string(std::string&);

    /// opens a new link
    void open(iflow_i* in);
    /// close the current link
    void close(void);
};

/**
 * binary output stream. decendant classes must still
 * implement write_binary
 *
 * @author Aleksander Demko
 */
class scopira::tool::textoflow : public scopira::tool::otflow_i
{
  protected:
    /// output link
    count2_ptr< oflow_i > dm_out;

  public:
    /**
     * constrcutor
     *
     * @param is the output stream to use. this object will "own" it
     * @author Aleksander Demko
     */
    textoflow(bool doref, oflow_i* out);
    /// destructor
    virtual ~textoflow(void);
      
    /// are we in a failed state?
    virtual bool failed(void) const;

    /// write a raw block of data
    virtual size_t write(const byte_t* _buf, size_t _size);

    virtual void write_bool(bool val);
    virtual void write_char(char val);
    virtual void write_short(short val);
    virtual void write_int(int val);
    virtual void write_size_t(size_t val);
    virtual void write_long(long val);
    virtual void write_float(float val);
    virtual void write_double(double val);
    virtual void write_string(const std::string& val);

    /// opens a new link
    void open(oflow_i* out);
    /// close the current link
    void close(void);
};

#endif

