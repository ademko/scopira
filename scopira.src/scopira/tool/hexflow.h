
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
 
#ifndef __INCLUDED__SCOPIRA_TOO_HEXFLOW__
#define __INCLUDED__SCOPIRA_TOO_HEXFLOW__

#include <scopira/tool/flow.h>

namespace scopira
{
  namespace tool
  {
    class hexiflow;
    class hexoflow;

    /// array of the 16 digits of hex
    extern const char* hexchars; // "0123...EF"
    /// array of the 16 digits of hex (lowercase, which you should probably prefer)
    extern const char* lowerhexchars; // "0123...ef"

    /// converts a '0'..'F' (or '0'..'f') to 0-15
    inline byte_t char_to_hex(char c) {
      // a == 97, A == 65, '0' == 48
      // this if-then chain order is important
      if (c <= '9')
        return c - '0';           // 0..
      else if (c <= 'F')
        return c - 'A' + 10;      // assume A..
      else
        return c - 'a' + 10;      // a..
    }
  }
}

/**
 * reads in hex data in a ascii-hex like pair form
 *
 * @author Aleksander Demko
 */
class scopira::tool::hexiflow : public scopira::tool::iflow_i
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
    hexiflow(bool doref, iflow_i* in);

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
 * writes out hex data in a ascii-hex like pair form
 *
 * @author Aleksander Demko
 */
class scopira::tool::hexoflow : public scopira::tool::oflow_i
{
  protected:
    count2_ptr< oflow_i > dm_out;

  public:
    /**
     * constrcutor
     *
     * @param is the input stream to use. this object will "own" it
     * @author Aleksander Demko
     */
    hexoflow(bool doref, oflow_i* out);

    /// are we in a failed state?
    virtual bool failed(void) const;

    /// write a raw block of data
    virtual size_t write(const byte_t* _buf, size_t _size);
    /// writes a byte
    virtual size_t write_byte(byte_t b);

    /// opens a new link
    void open(oflow_i* in);
    /// close the current link
    void close(void);
};

#endif

