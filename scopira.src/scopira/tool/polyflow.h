
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

#ifndef __INCLUDED_SCOPIRA_TOOL_POLYFLOW_H__
#define __INCLUDED_SCOPIRA_TOOL_POLYFLOW_H__

#include <scopira/tool/flow.h>
#include <scopira/tool/export.h>

#include <typeinfo>
#include <map>

//
// two polymorphic flow implementations
// (rouge wave guide, ver 7, p124)
//
// @author Aleksander Demko
//

namespace scopira
{
  namespace tool
  {
    class polyiflow;
    class polyoflow;
    class isoiflow;
    class isooflow;
  }
}

/**
 * polymorphic input
 *
 * a polymorphic flow implementation. this implementation
 * does NOT handle morphic pointers. that is, it saves
 * each object completly, with not regard for circular
 * pointer relationships
 *
 * @author Aleksander Demko
 */
class scopira::tool::polyiflow : public scopira::tool::iobjflow_i
{
  protected:
    bool read_object_impl(object* &out);
    typedef std::map<int, const std::type_info*> typemap_t;
    count2_ptr< itflow_i > dm_file;
    typemap_t dm_typemap;

  public:
  
    /// constructor + open
    SCOPIRA_EXPORT polyiflow(bool doref, itflow_i* rz);
    
    /// destructor
    SCOPIRA_EXPORT virtual ~polyiflow();

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
    
    /// reads a virtual object from the stream
    SCOPIRA_EXPORT virtual bool read_object(object* &out);
    
    /**
     * opens a flow on the given file stream. the flow will
     * "own" the file stream
     *
     * @param fi the file to operate on
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT void open(itflow_i* fi);
    
    /// closes the current file
    SCOPIRA_EXPORT void close(void);
};

/**
 * polymorphic output
 *
 * a polymorphic flow implementation. this implementation
 * does NOT handle morphic pointers. that is, it saves
 * each object completly, with not regard for circular
 * pointer relationships
 *
 * @author Aleksander Demko
 */
class scopira::tool::polyoflow : public scopira::tool::oobjflow_i
{   
  protected:
    typedef std::map<std::string, int> typemap_t;
    count2_ptr< otflow_i > dm_file;
    int dm_next_typeid;
    typemap_t dm_typemap;

  public:
  
    /// constructor + open
    SCOPIRA_EXPORT polyoflow(bool doref, otflow_i* rz);
    
    /// destructor
    SCOPIRA_EXPORT virtual ~polyoflow();

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
    
    /// saves the object to stream
    SCOPIRA_EXPORT virtual void write_object(const scopira::tool::object* o);
    
    /**
     * opens a flow on the given file stream. the flow will
     * "own" the file stream
     *
     * @param fi the file to operate on
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT void open(otflow_i* fi);
    
    /// closes the current file
    SCOPIRA_EXPORT void close(void);
};

/**
 * This is an polyoflow implementation that implements full serialazation
 * of objects, and has support for multi references. That is, if an object
 * is refered to by multiple pointers, it will only be written to disk
 * once.
 *
 * Reading the objects back from the stream also reproduces one instance.
 * rather than many.
 *
 * This is stream is almost always prefeered over scopira::tool::polyiflow
 *
 * @author Aleksander Demko
 */ 
class scopira::tool::isoiflow : public scopira::tool::polyiflow
{
  protected:
    typedef std::map<int, scopira::tool::object*> idmap_t;
    idmap_t dm_idmap;

  public:  
    /// constructor + open
    SCOPIRA_EXPORT isoiflow(bool doref, itflow_i* rz);
    
    /// destructor
    SCOPIRA_EXPORT virtual ~isoiflow();
    
    /// reads a virtual object from the stream
    SCOPIRA_EXPORT virtual bool read_object(object* &out);
};

/**
 * This is an polyoflow implementation that implements full serialazation
 * of objects, and has support for multi references. That is, if an object
 * is refered to by multiple pointers, it will only be written to disk
 * once.
 *
 * Reading the objects back from the stream also reproduces one instance.
 * rather than many.
 *
 * This is stream is almost always prefeered over scopira::tool::polyoflow
 *
 * @author Aleksander Demko
 */ 
class scopira::tool::isooflow : public scopira::tool::polyoflow
{
  protected:
    typedef std::map<const scopira::tool::object*, int> ptrmap_t;
    int dm_nextid;
    ptrmap_t dm_ptrmap;

  public:
    /// constructor + open
    SCOPIRA_EXPORT isooflow(bool doref, otflow_i* rz);
    
    /// destructor
    SCOPIRA_EXPORT virtual ~isooflow();
    
    /// saves the object to stream
    SCOPIRA_EXPORT virtual void write_object(const scopira::tool::object* o);
};

#endif

