
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

#ifndef __INCLUDED_SCOPIRA_BASEKIT_ISLICE_H__
#define __INCLUDED_SCOPIRA_BASEKIT_ISLICE_H__

#include <scopira/tool/array.h>

namespace scopira
{
  namespace basekit
  {
    template <class T> class islice_vec_iterator_g;
    template <class T> class const_islice_vec_iterator_g;
    template <class T> class islice_vec_g;
  }
}

/**
 * iterator for islice
 * @author Aleksander Demko
 */ 
template <class T> class scopira::basekit::islice_vec_iterator_g
{
  public:
    typedef T data_type;
  private:
    T * const * dm_ptr;
  public:
    /// ctor
    islice_vec_iterator_g(T * const * ptr) : dm_ptr(ptr) { }
    const T & operator *(void) const { return **dm_ptr; }
    bool operator ==(const islice_vec_iterator_g<T> &rhs) const { return dm_ptr == rhs.dm_ptr; }
    bool operator !=(const islice_vec_iterator_g<T> &rhs) const { return dm_ptr != rhs.dm_ptr; }

    islice_vec_iterator_g<T>& operator++(void) { dm_ptr++; return *this; }
};

/**
 * iterator for islice
 * @author Aleksander Demko
 */ 
template <class T> class scopira::basekit::const_islice_vec_iterator_g
{
  public:
    typedef T data_type;
  private:
    const T * const * dm_ptr;
  public:
    /// ctor
    const_islice_vec_iterator_g(const T * const * ptr) : dm_ptr(ptr) { }
    const T & operator *(void) const { return **dm_ptr; }
    bool operator ==(const const_islice_vec_iterator_g<T> &rhs) const { return dm_ptr == rhs.dm_ptr; }
    bool operator !=(const const_islice_vec_iterator_g<T> &rhs) const { return dm_ptr != rhs.dm_ptr; }

    const_islice_vec_iterator_g<T>& operator++(void) { dm_ptr++; return *this; }
};

/**
 * an irregular slice that looks like a vector
 *
 * @author Aleksander Demko
 */
template <class T> class scopira::basekit::islice_vec_g : private scopira::tool::basic_array<T*>
{
  public:
    typedef scopira::tool::basic_array<T*> parent_type;
    typedef T data_type;
    typedef islice_vec_iterator_g<T> iterator;
    typedef const_islice_vec_iterator_g<T> const_iterator;

  public:
    // default default ctor - ok

    /// size initing ctor
    explicit islice_vec_g(size_t len) : parent_type(len) { }

    /// size
    size_t size(void) const { return parent_type::size(); }
    /// empty?
    bool empty(void) const { return parent_type::empty(); }
    /// resizer
    void resize(size_t len) { parent_type::resize(len); }
    /// clear
    void clear(void) { parent_type::clear(); }

    /// begin-stl like iterator
    iterator begin(void) { return islice_vec_iterator_g<T>(dm_ary); }
    /// end-stl like iterator
    iterator end(void) { return islice_vec_iterator_g<T>(dm_ary+dm_len); }
    /// begin-stl like iterator
    const_iterator begin(void) const { return const_islice_vec_iterator_g<T>(dm_ary); }
    /// end-stl like iterator
    const_iterator end(void) const { return const_islice_vec_iterator_g<T>(dm_ary+dm_len); }

    /// generic copy, uses size() and iterators
    template <class L>
      void copy(const L & rhs);

    /// sets the address
    void set_ptr(size_t idx, T * ref) { parent_type::set(idx, ref); }

    /// access
    T &operator[](size_t idx) { return *parent_type::get(idx); }
    /// access
    const T &operator[](size_t idx) const { return *parent_type::get(idx); }
    /// access
    T & get(size_t idx) { return *parent_type::get(idx); }
    /// access
    const T & get(size_t idx) const { return *parent_type::get(idx); }
    /// access
    void set(size_t idx, const T &val) { *parent_type::get(idx) = val; }
};
template <class T> template <class L>
void scopira::basekit::islice_vec_g<T>::copy(const L &rhs)
{
  typename L::const_iterator ii;
  size_t i;
  resize( rhs.size() );
  for (i=0, ii=rhs.begin(); ii != rhs.end(); ++i, ++ii)
    set(i, *ii);
}

#endif

