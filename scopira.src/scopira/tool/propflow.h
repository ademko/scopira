
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
#include <scopira/tool/export.h>

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
     * @param doref should this class reference count in
     * @param in the input stream to use. this object will "own" it
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT propiflow(bool doref, iflow_i* in);
    /// destructor
    SCOPIRA_EXPORT virtual ~propiflow(void);

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

    /// object reader
    SCOPIRA_EXPORT virtual bool read_object(object* &out);

    /**
     * reads any string, both keyword-style and quoted
     *
     * @param ret the read in string
     * @param keyword true, if readin in keyword, false if quoted
     * @return success reading keyword
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT bool read_keyword(std::string& ret, bool& keyword);

    /// opens a new link
    SCOPIRA_EXPORT void open(iflow_i* in);
    /// close the current link
    SCOPIRA_EXPORT void close(void);

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
     * @param doref should this class reference count in
     * @param in the output stream to use. this object will "own" it
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT propoflow(bool doref, oflow_i* out);
    /// destructor
    SCOPIRA_EXPORT virtual ~propoflow(void);
      
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

    /// object writer
    SCOPIRA_EXPORT virtual void write_object(const scopira::tool::object* o);

    /// writes an unescaped keyword
    SCOPIRA_EXPORT void write_keyword(const std::string& i);
    /// write n spaces
    SCOPIRA_EXPORT void write_indent(int n);

    /// opens a new link
    SCOPIRA_EXPORT void open(oflow_i* out);
    /// close the current link
    SCOPIRA_EXPORT void close(void);

    /// writes a whole property tree
    SCOPIRA_EXPORT void write_property(property* rp, int indent = 0);
};

#endif

