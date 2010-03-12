
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
 
#ifndef __INCLUDED_SCOPIRA_TOOL_FLOW_H__
#define __INCLUDED_SCOPIRA_TOOL_FLOW_H__

#include <string>

#include <scopira/tool/object.h>
#include <scopira/tool/export.h>

// THIS FILE HAS BEEN FULLY DOCUMENTED

//
// the *file_i* based streams provide "simple persistance" (rouge wave
// guide, ver 7, p124). conceptually then are somewhere above
// C FILE and slightly below iostreams (although theyre more
// virtual/runtime polymorphic than iostrmea). theyre modeled after
// Java File.
//
// @author Aleksander Demko
//

//
// the flow_i interfaces allow polymorphic persinance, often wrapped
// around a file_i based driver. depending on the driver (see polyflow.h)
//
// @author Aleksander Demko
//


namespace scopira
{
  namespace tool
  {
    class flow_i;
    class iflow_i;
    class oflow_i;
    class otflow_i;
    class itflow_i;
    class iobjflow_i;
    class oobjflow_i;

    typedef unsigned char byte_t;
  }
}

/**
 * This is the base of all flows (both input and output).
 * It introduced a few constants, inheritance from object
 * and a failed state checking method.
 *
 * @author Aleksander Demko
 */
class scopira::tool::flow_i :  public virtual scopira::tool::object
{
  public:
    /**
     * An 8-bit octect/byte.
     *
     * @author Aleksander Demko
     */
    typedef scopira::tool::byte_t byte_t;
    /**
     * A bit mask of modes.
     * @author Aleksander Demko
     */ 
    typedef int mode_t;

    enum {
      /// Open flow for input
      input_c = 1,
      /// Open flow for output
      output_c = 2
    };

      // by powers of 2. bits 0-9 are reserved by this
      // header file. your first bit should be 10 (2^10 == 1024)
      // other decentdants may reserve
      // more
      
    // common methods... seek, close, eof-testing, size ?

    /**
     * Is the stream in a failed state?
     *
     * @return true if it is in a failed state
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT virtual bool failed(void) const = 0;
};

/**
 * A raw input stream of bytes
 *
 * @author Aleksander Demko
 */
class scopira::tool::iflow_i : public virtual scopira::tool::flow_i
{
  public:
    /**
     * Read a raw block of bytes.
     *
     * @param _buf the buffer to write out to
     * @param _maxsize the maximume number of bytes to read into the buffer
     * @return the number of bytes actually read
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT virtual size_t read(byte_t *_buf, size_t _maxsize) = 0;
    /**
     * Reads one byte
     *
     * @param out where to write the byte
     * @return 1 if a byte was read, 0 if not
     * @author Aleksander Demko
     */ 
    virtual size_t read_byte(byte_t &out) {
      return read(&out, 1);
    }

    /**
     * Nice wrapper around the raw read() method.
     * Lets you read raw arrays of a particular type.
     * POD types only. Returns the number of elelments actually read.
     *
     * @author Aleksander Demko
     */
    template <class TT>
      size_t read_array(TT *_buf, size_t _numelem) {
        return read(reinterpret_cast<byte_t*>(_buf), _numelem*sizeof(TT))/sizeof(TT);
      }

    /**
     * Reads a raw block of bytes, using a void*
     *
     * @param _buf the buffer to write out to
     * @param _maxsize the maximume number of bytes to read into the buffer
     * @return the number of bytes actually read
     * @author Aleksander Demko
     */ 
    inline size_t read_void(void *_buf, size_t _maxsize) { return read(reinterpret_cast<byte_t*>(_buf), _maxsize); }
};

/**
 * A raw output stream of bytes
 *
 * @author Aleksander Demko
 */
class scopira::tool::oflow_i : public virtual scopira::tool::flow_i
{
  public:
    /**
     * Writes a block of bytes
     *
     * @param _buf the bytes
     * @param _size the size of the above buffer
     * @return the number of bytes actually written
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT virtual size_t write(const byte_t *_buf, size_t _size) = 0;
    /**
     * Writes one byte
     *
     * @param b the byte to write
     * @return 1 if the byte was written to the flow, 0 if not
     * @author Aleksander Demko
     */ 
    virtual size_t write_byte(byte_t b) {
      return write(&b, sizeof(b));
    }

    /**
     * Nice wrapper around the raw write() method.
     * Lets you write raw arrays of a particular type.
     * POD types only. Returns the number of elelments actually written.
     *
     * @author Aleksander Demko
     */
    template <class TT>
      size_t write_array(const TT *_buf, size_t _numelem) {
        return write(reinterpret_cast<const byte_t*>(_buf), _numelem*sizeof(TT))/sizeof(TT);
      }

    /**
     * Writes a block of bytes, using a void*
     *
     * @param _buf the bytes
     * @param _size the size of the above buffer
     * @return the number of bytes actually written
     * @author Aleksander Demko
     */ 
    inline size_t write_void(const void *_buf, size_t _size) { return write(reinterpret_cast<const byte_t*>(_buf), _size); }
};

/**
 * This adds type-aware input routines to an iflow_i.
 * The types include numeric (both floating and integer)
 * types aswell as std::string.
 *
 * @author Aleksander Demko
 */
class scopira::tool::itflow_i : public virtual scopira::tool::iflow_i
{
  public:
    /**
     * Reads an bool, returns true on success
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT virtual bool read_bool(bool&) = 0;
    /**
     * Reads an char, returns true on success
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT virtual bool read_char(char&) = 0;
    /**
     * Reads an short, returns true on success
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT virtual bool read_short(short&) = 0;
    /**
     * Reads an int, returns true on success
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT virtual bool read_int(int&) = 0;
    /**
     * Reads a size_t, returns true on success
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT virtual bool read_size_t(size_t&) = 0;
    /**
     * Reads an int64_t, returns true on success
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT virtual bool read_int64_t(int64_t&) = 0;
    /**
     * Reads an long, returns true on success
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT virtual bool read_long(long&) = 0;
    /**
     * Reads a float, returns true on success
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT virtual bool read_float(float&) = 0;
    /**
     * Reads a double, returns true on success
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT virtual bool read_double(double&) = 0;
    /**
     * Reads an STL string, returns true on success
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT virtual bool read_string(std::string&) = 0;
    /**
     * Reads any type, aslong as it has a flowtraits_g
     * @author Aleksander Demko
     */ 
    template <class TT>
      inline bool read_generic(TT &v);
      // implementation in traits.h to avoid circular refs
};

/**
 * This adds type-aware output routines to an oflow_i.
 * The types include numeric (both floating and integer)
 * types aswell as std::string.
 *
 * @author Aleksander Demko
 */
class scopira::tool::otflow_i : public virtual scopira::tool::oflow_i
{
  public:
    /**
     * Writes a bool
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT virtual void write_bool(bool) = 0;
    /**
     * Writes a char
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT virtual void write_char(char) = 0;
    /**
     * Writes a short
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT virtual void write_short(short) = 0;
    /**
     * Writes an int
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT virtual void write_int(int) = 0;
    /**
     * Writes a size_t
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT virtual void write_size_t(size_t) = 0;
    /**
     * Writes a int64_t
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT virtual void write_int64_t(int64_t) = 0;
    /**
     * Writes an long
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT virtual void write_long(long) = 0;
    /**
     * Write a float
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT virtual void write_float(float) = 0;
    /***
     * Writes a double
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT virtual void write_double(double) = 0;
    /**
     * Writes a STL string
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT virtual void write_string(const std::string&) = 0;
    /**
     * Writes any type, aslong as it has a flowtraits_g
     * @author Aleksander Demko
     */ 
    template <class TT>
      inline void write_generic(const TT &v);
      // implementation in traits.h to avoid circular refs
};


/**
 * Input object serialization. This interface adds a virtual-object
 * read method to itflow_t. In total - via inheritance -
 * this interface may read objects, primitive types (including
 * streams) and raw bytes.
 *
 * @author Aleksander Demko
 */
class scopira::tool::iobjflow_i : public virtual scopira::tool::itflow_i
{
  public:

    /**
     * Reads a virtual object from the flow.
     *
     * The method may succeed (by returning true) and still set out
     * to be null. This is normal, and just means that a null pointer
     * was stored in the stream.
     *
     * Always pass a real object*. Do not attempt to pass (via a
     * cast) yourobject* to this method. You will be bitten by
     * subtlies in the way C++ moves pointers around on multiple
     * or virtual inheritance heiarchies.
     *
     * @param out this will be set to the read object
     * @return true on success, false on failure.
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT virtual bool read_object(object* &out) = 0;

    /**
     * This is a type safe helper around read_object.
     * It will do read_object on its own temporary object*
     * and verify (via assert) that it can be cast to your
     * type TT.
     * @param out the output pointer to set, may be null on success
     * @return true on success
     * @author Aleksander Demko
     */ 
    template <class TT> bool read_object_type(TT * &out);
    /**
     * Similar to read_object_type, but reads into an auto pointer
     * directly.
     * @author Aleksander Demko
     */ 
    template <class TT> bool read_object_type(count_ptr<TT> &out);
};
template <class TT> bool scopira::tool::iobjflow_i::read_object_type(TT * &out)
{
  object *o;

  if (!read_object(o))
    return false;
  assert(!o || dynamic_cast<TT*>(o));
  out = dynamic_cast<TT*>(o);
  return true;
};
template <class TT> bool scopira::tool::iobjflow_i::read_object_type(count_ptr<TT> &out)
{
  object *o;

  if (!read_object(o))
    return false;
  assert(!o || dynamic_cast<TT*>(o));
  out = dynamic_cast<TT*>(o);
  return true;
};


/**
 * Output object serialization. This interface adds a virtual-object
 * write method to otflow_t. In total - via inheritance -
 * this interface may write objects, primitive types (including
 * streams) and raw bytes.
 *
 * @author Aleksander Demko
 */
class scopira::tool::oobjflow_i : public virtual scopira::tool::otflow_i
{
  public:
    /**
     * Writes the given object to stream.
     * @param o the object to write. may be null.
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT virtual void write_object(const scopira::tool::object* o) = 0;
    /**
     * Writes the given object to stream, from an auto pointer.
     * @param o the object to write. may be null.
     * @author Aleksander Demko
     */ 
    template <class L>
      void write_object_type(const scopira::tool::count_ptr<L> &o)
        { write_object(o.get()); }
};

//
// Friend functions to allow visual ascii conversion of basic types
// (like printoflow)
//

/**
 * Flow output operator for type const char* (C string literals)
 * @author Aleksander Demko
 */ 
SCOPIRA_EXPORT scopira::tool::oflow_i& operator<<(scopira::tool::oflow_i& o, const char* val);
/**
 * Flow output operator for STL strings
 * @author Aleksander Demko
 */ 
SCOPIRA_EXPORT scopira::tool::oflow_i& operator<<(scopira::tool::oflow_i& o, const std::string& val);
/**
 * Flow output operator for char
 * @author Aleksander Demko
 */ 
SCOPIRA_EXPORT scopira::tool::oflow_i& operator<<(scopira::tool::oflow_i& o, char val);
/**
 * Flow output operator for bool
 * @author Aleksander Demko
 */ 
SCOPIRA_EXPORT scopira::tool::oflow_i& operator<<(scopira::tool::oflow_i& o, bool val);
/**
 * Flow output operator int
 * @author Aleksander Demko
 */ 
SCOPIRA_EXPORT scopira::tool::oflow_i& operator<<(scopira::tool::oflow_i& o, int val);
/**
 * Flow output operator long
 * @author Aleksander Demko
 */ 
SCOPIRA_EXPORT scopira::tool::oflow_i& operator<<(scopira::tool::oflow_i& o, long val);
/**
 * Flow output operator double
 * @author Aleksander Demko
 */ 
SCOPIRA_EXPORT scopira::tool::oflow_i& operator<<(scopira::tool::oflow_i& o, double val);
/**
 * Flow output operator unsigned int
 * @author Aleksander Demko
 */ 
SCOPIRA_EXPORT scopira::tool::oflow_i& operator<<(scopira::tool::oflow_i& o, unsigned int val);
/**
 * Flow output operator for unsigned long
 * @author Aleksander Demko
 */ 
SCOPIRA_EXPORT scopira::tool::oflow_i& operator<<(scopira::tool::oflow_i& o, unsigned long val);
/**
 * Flow output operator long long
 * @author Aleksander Demko
 */ 
SCOPIRA_EXPORT scopira::tool::oflow_i& operator<<(scopira::tool::oflow_i& o, long long val);
/**
 * Flow output operator unsigned long long
 * @author Aleksander Demko
 */ 
SCOPIRA_EXPORT scopira::tool::oflow_i& operator<<(scopira::tool::oflow_i& o, unsigned long long val);
/**
 * Flow output operator for iflow_i.
 * This does a raw (byte based) read from the input stream to the given output stream.
 * It will read from the input stream until it provides no more bytes.
 *
 * @param i the input stream to read full.
 * @param o the output stream to write to
 * @return the same output stream, namely o
 * @author Aleksander Demko
 */ 
SCOPIRA_EXPORT scopira::tool::oflow_i& operator<<(scopira::tool::oflow_i& o, scopira::tool::iflow_i& i);

/**
  \page scopiratoolflowpage Input/output flows

  Scopira includes its own input/output streaming flow facility. Features include:

  - Easy output for many type with the << operator.
  - Polymorphic architecture allows for the construction of layered, filtered data flows.
  - Fully serialization (object persistence) capabilities, include the ability to handle circular references.

  \section interfacessec Interfaces

  Six primary interfaces are provided in the flow system. Three interfaces are for input:
  - scopira::tool::iflow_i reads raw bytes via two read() methods. A failed() method tests if 
    the flow is in a failed state.
  - scopira::tool::itflow_i builds on scopira::tool::iflow_i by adding primitive type aware read methods. Primitives 
    such as strings are included with the usual ints and doubles.
  - scopira::tool::iobjflow_i builds on scopira::tool::itflow_i by adding a scopira::tool::object-based serialization 
    read method. This method is able to construct previously serialized objects. 

  And three interfaces are for output:
  - scopira::tool::ioflow_i writes raw bytes via two write() methods. A failed() method tests 
    if the flow is in a failed state.
  - scopira::tool::iotflow_i builds on scopira::tool::ioflow_i by adding primitive type aware write methods. Primitives 
    such as strings are included with the usual ints and doubles.
  - scopira::tool::ioobjflow_i builds on scopira::tool::iotflow_i by adding a scopira::tool::object-based serialization 
    write method. This method stores type information about the object for later reconstruction by 
    iiobjflow}. 


  \section endflowsec End Flows

  End flows are flow objects that take and produce data from concrete locations. These include:
  - Reading and writing to a file on disk with scopira::tool::fileflow.
  - Reading and writing to a string in memory with scopira::tool::stringflow. This class uses its own 
    internal string storage. Two variants read scopira::tool::stringiflow and write scopira::tool::stringoflow
    to strings that you provide.
  - Reading and writing to a buffer of bytes with scopira::tool::bufferflow.
  - Reading and writing to a TCP/IP network socket with scopira::tool::netflow.
  - A flow that produces nothing and accepts everything with scopira::tool::nullflow.

  \section filteringsec Filtering Flows

  Filtering flows provide flow classes that perform some kind of operations on the data flow as it 
  passes to another flow. Filtering flows must always be connected to other flows and may be chained to 
  perform stacked operations. Filters are organized into three tiers. 

  At the byte level (descendants from scopira::tool::iflow_i and scopira::tool::oflow_i) transformations are done on the raw byte 
  stream itself:
  - scopira::tool::hexiflow and tool::hexiflow transform the stream into ASCII hex numbers. 
    This is useful for storing arbitrary binary data in a text form.
  - scopira::tool::distoflow multiplexes its output to multiple tool::oflow_i objects.
  - scopira::tool::commentiflow is used to filter out comments from a file. A comment is defined as 
    a # character in the first column, until end of line.
  - Future binary filtering flows could include those for checksum calculation, encryption and compression. 

  Type-level filters (those that descend from scopira::tool::itflow and scopira::tool::otflow) operate on primitive
  types and convert them to and from bytes. They connect to binary filters or end flows directly. They 
  include:
  - Direct binary storage via scopira::tool::biniflow and scopira::tool::binoflow. This is the fastest 
    and most compressed conversion possible.
  - scopira::tool::textiflow and scopira::tool::textoflow provide straightforward, reversible ASCII 
    encodings for the primitive types.
  - scopira::tool::printiflow and tool::printoflow aid in the parsing and production of ASCII 
    strings. Their formations are not always reversable.
  - Filters specific to the produce of property files - scopira::tool::propiflow and scopira::tool::propoflow. 

  Finally, object-level filters (those that descend from scopira::tool::iobjflow_i and scopira::tool::oobjflow_i) convert
  objects to a series of primitives. 
  They connect to type-level filters. Two serialization (persistent) capable filters are provided:
  - scopira::tool::polyiflow and scopira::tool::polyoflow perform straight polymorphic serialization. 
    This is the faster of the two streams but does not handle circular references, and stores duplicate object instances multiple 
    times in the stream.
  - scopira::tool::isoiflow and scopira::tool::isooflow build on the polymorphic streams. They support circular references and record 
    only references to previously saved object instances. This pair should be preferred when the data being serialized is potentially 
    complex or circular. 

  \section setobjsec Serializable Objects

  To make your own objects serialization, see \ref scopiracoreserializationpage

*/

#endif

