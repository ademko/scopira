
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
 
#ifndef __INCLUDED_SCOPIRA_TOOL_PROPFLOW_H__
#define __INCLUDED_SCOPIRA_TOOL_PROPFLOW_H__

#include <scopira/tool/flow.h>

namespace scopira
{
  namespace tool
  {
    class commentiflow;

    class propiflow;
    class propoflow;
    class property;
  }
}

//
// rightnow it can read from a custom and custom-old
// text format.
//  future file formats include: XML, Win32 registry
//

/**
 * filters out 0-column # comment lines
 *
 * @author Aleksander Demko
 */ 
class scopira::tool::commentiflow : public scopira::tool::iflow_i
{
  protected:
    /// input flow
    count2_ptr< iflow_i > dm_in;
    bool dm_atzero;

  public:
    /// ctor
    commentiflow(bool doref, iflow_i* in);

    /// are we in a failed state?
    virtual bool failed(void) const;

    /// read raw block data, returns num read in
    virtual size_t read(byte_t* _buf, size_t _maxsize);
    /// read one byte
    virtual size_t read_byte(byte_t &out);

    /// opens a new link
    void open(iflow_i* in);
    /// close the current link
    void close(void);
};

/**
 * data type to ascii-like converter, with functions
 * for property-like config files
 *
 * @author Aleksander Demko
 */
class scopira::tool::propiflow : public scopira::tool::iobjflow_i
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
    propiflow(bool doref, iflow_i* in);
    /// destructor
    virtual ~propiflow(void);

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

    /// object reader
    virtual bool read_object(object* &out);

    /**
     * reads any string, both keyword-style and quoted
     *
     * @param ret the read in string
     * @param keyword true, if readin in keyword, false if quoted
     * @return success reading keyword
     * @author Aleksander Demko
     */
    bool read_keyword(std::string& ret, bool& keyword);

    /// opens a new link
    void open(iflow_i* in);
    /// close the current link
    void close(void);

    /// reads a whole property tree of data and
    /// save it to the given property
    bool read_property(property* rp, int level = 0);
};

/**
 * data type to ascii-like converter, with functions
 * for property-like config files
 *
 * @author Aleksander Demko
 */
class scopira::tool::propoflow : public scopira::tool::oobjflow_i
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
    propoflow(bool doref, oflow_i* out);
    /// destructor
    virtual ~propoflow(void);
      
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

    /// object writer
    virtual void write_object(const scopira::tool::object* o);

    /// writes an unescaped keyword
    void write_keyword(const std::string& i);
    /// write n spaces
    void write_indent(int n);

    /// opens a new link
    void open(oflow_i* out);
    /// close the current link
    void close(void);

    /// writes a whole property tree
    void write_property(property* rp, int indent = 0);
};

#endif

