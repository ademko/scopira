
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
 
#ifndef __INCLUDED_SCOPIRA_TOOL_CACHEFLOW_H__
#define __INCLUDED_SCOPIRA_TOOL_CACHEFLOW_H__

#include <scopira/tool/array.h>
#include <scopira/tool/flow.h>
#include <scopira/tool/export.h>

#include <assert.h>

namespace scopira
{
  namespace tool
  {
    template <class T> class circular_vector;

    class cacheiflow;
    class cacheoflow;
  }
}

/**
 * A cirtuclar buffer
 * @author Aleksander Demko
 */ 
template <class T> class scopira::tool::circular_vector
{
  public:
    typedef T data_type;
    typedef typename basic_array<T>::iterator iterator;
    typedef typename basic_array<T>::const_iterator const_iterator;
  private:
    typedef circular_vector<T> this_type;
    typedef basic_array<T> arr_type;
  protected:
    arr_type dm_ary;    // its actually 1 longer than we need
    iterator dm_read, dm_write;
  public:
    /// ctor
    explicit circular_vector(size_t reserve_size = 0);
    /// sets the internal buffer size
    /// resets all pointers
    void reserve(size_t reserve_size);

    iterator begin1(void) { return dm_read; }
    iterator end1(void)
      { return (dm_write<dm_read) ? dm_ary.end() : dm_write; }
    const_iterator begin1(void) const { return dm_read; }
    const_iterator end1(void) const
      { return (dm_write<dm_read) ? dm_ary.end() : dm_write; }
    iterator begin2(void) { return dm_ary.begin(); }
    iterator end2(void)
      { return (dm_write<dm_read) ? dm_write : dm_ary.begin(); }
    const_iterator begin2(void) const { return dm_ary.begin(); }
    const_iterator end2(void) const
      { return (dm_write<dm_read) ? dm_write : dm_ary.begin(); }

    /// (current) buffer empty?
    bool empty(void) const { return dm_read == dm_write; }
    /// (current) buffer full?
    bool full(void) const
      { return (dm_write+1 == dm_read) ||
        (dm_read == dm_ary.begin() && dm_write == dm_ary.end()-1); }
    /// gets the current (used) size
    size_t size(void) const;
    /// gets the total capacity of the buffer
    size_t capacity(void) const { return dm_ary.size() - 1; }
    /// how much room is left
    size_t free(void) const { return capacity() - size(); }
    /// pop_all elements
    void clear(void) { dm_read = dm_write = dm_ary.begin(); }

    /// gets the front item on the buffer
    const T & front(void) const { return *dm_read; }
    /// push one element to the back of the buffer
    void push_back(const T &item);
    /// pop one item from the front of the buffer
    void pop_front(void);
    /// pop_all elements (same as clear())
    void pop_all(void) { dm_read = dm_write = dm_ary.begin(); }
    /// push the given iterator to the buffer
    template <class ITER>
      void push_back(ITER head, ITER tail);
    /// pop and full the given output iterator
    /// returns the actual end that was filled
    template <class ITER>
      ITER pop_front(ITER head, ITER tail);

    /// special kind of push
    void push_seek(size_t sz) { dm_write += sz; assert(dm_write<dm_ary.end()); }
    /// special kind of rewind
    void short_rewind(void) { dm_read = dm_ary.begin(); }
};

//
//
// circular_vector
//
//

template <class T>
scopira::tool::circular_vector<T>::circular_vector(size_t reserve_size)
  : dm_ary(reserve_size+1)
{
  if (reserve_size>0)
    dm_write = dm_read = dm_ary.begin();
}

template <class T>
void scopira::tool::circular_vector<T>::reserve(size_t reserve_size)
{
  dm_ary.resize(reserve_size+1);
  if (reserve_size>0)
    dm_write = dm_read = dm_ary.begin();
}

template <class T>
size_t scopira::tool::circular_vector<T>::size(void) const
{
  if (dm_write >= dm_read)
    return dm_write - dm_read;
  else
    return dm_ary.size() + (dm_write - dm_read);
}

template <class T>
void scopira::tool::circular_vector<T>::push_back(const T &item)
{
  *dm_write = item;
  ++dm_write;
  assert(dm_read != dm_write);    // easier to do this check here
}

template <class T>
void scopira::tool::circular_vector<T>::pop_front(void)
{
  assert(!empty());

  ++dm_read;
  if (dm_read == dm_ary.end())
    dm_read = dm_ary.begin();
}

template <class T> template <class ITER>
void scopira::tool::circular_vector<T>::push_back(ITER head, ITER tail)
{
  assert(free() >= (tail - head));

  // 2-part write
  while (head != tail && dm_write != dm_ary.end()) {
    *dm_write = *head;
    ++head;
    ++dm_write;
  }

  if (dm_write == dm_ary.end())
    dm_write = dm_ary.begin();
  else
    return; // 1 part was enough, i guess

  while (head != tail) {
    *dm_write = *head;
    ++head;
    ++dm_write;
  }
}

template <class T> template <class ITER>
ITER scopira::tool::circular_vector<T>::pop_front(ITER head, ITER tail)
{
  iterator endend;

  endend = dm_write < dm_read ? dm_ary.end() : dm_write;
  // 2-part write
  while (head != tail && dm_read != endend) {
    *head = *dm_read;
    ++head;
    ++dm_read;
  }

  if (head != tail && !empty())
    dm_read = dm_ary.begin();
  else
    return head; // 1 part was enough, i guess

  while (head != tail && dm_read != dm_write) {
    *head = *dm_read;
    ++head;
    ++dm_read;
  }

  return head;
}
/**
 * A performancing-enhancing cache filter.
 * @author Aleksander Demko
 */ 
class scopira::tool::cacheiflow : public scopira::tool::iflow_i
{
  protected:
    count2_ptr< iflow_i > dm_in;
    circular_vector<byte_t> dm_cache;
    bool dm_failed;

  public:
    /**
     * constrcutor
     *
     * @param is the input stream to use. this object will "own" it
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT cacheiflow(bool doref, iflow_i* innie, size_t buffersize = 32768);

    /// are we in a failed state?
    SCOPIRA_EXPORT virtual bool failed(void) const { return dm_failed; }

    /// read raw block data, returns num read in
    SCOPIRA_EXPORT virtual size_t read(byte_t* _buf, size_t _maxsize);
    /// read one byte
    SCOPIRA_EXPORT virtual size_t read_byte(byte_t &out);

    /// opens a new link
    SCOPIRA_EXPORT void open(iflow_i* in);
    /// close the current link
    SCOPIRA_EXPORT void close(void);

    /// do a "short rewind", basically move the cache in pointer to the
    /// start of the cache. this only is useful if youre reading the
    /// starts of files
    SCOPIRA_EXPORT void short_rewind(void);

    /// loads the cache... you need not call this explicitly
    SCOPIRA_EXPORT void load_cache(void);
};

/**
 * A performancing-enhancing cache filter.
 * @author Aleksander Demko
 */ 
class scopira::tool::cacheoflow : public scopira::tool::oflow_i
{
  protected:
    count2_ptr< oflow_i > dm_out;
    circular_vector<byte_t> dm_cache;
    bool dm_failed;

  public:
    /**
     * constrcutor
     *
     * @param is the input stream to use. this object will "own" it
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT cacheoflow(bool doref, oflow_i* out, size_t buffersize = 32768);
    /// dtor
    SCOPIRA_EXPORT ~cacheoflow();

    /// are we in a failed state?
    SCOPIRA_EXPORT virtual bool failed(void) const { return dm_failed; }

    /// write a raw block of data
    SCOPIRA_EXPORT virtual size_t write(const byte_t* _buf, size_t _size);
    /// writes a byte
    SCOPIRA_EXPORT virtual size_t write_byte(byte_t b);

    /// opens a new link
    SCOPIRA_EXPORT void open(oflow_i* in);
    /// close the current link
    SCOPIRA_EXPORT void close(void);

    /// flushes the cache to the output stream
    /// outside users dont really need to call this, unless they
    /// want to force/sync flushes
    SCOPIRA_EXPORT void flush_cache(void);
};

#endif
