
/*
 *  Copyright (c) 2002-2004    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#ifndef __INCLUDED_SCOPIRA_TOOL_ARRAY_H__
#define __INCLUDED_SCOPIRA_TOOL_ARRAY_H__

#include <assert.h>
#include <stddef.h>    //for size_t
#include <string.h>    //for memset

// THIS FILE HAS BEEN FULLY DOCUMENTED

namespace scopira
{
  namespace tool
  {
    template <class T> class array_view;
    template <class T> class const_array_view;

    template <class T> class basic_array;
    template <class T> class basic_matrix;
    template <class T, size_t N> class fixed_array;
    template <class T, size_t N> class hybrid_array;
  }
}

/**
 * A (non-resizable) "view" of another array
 * @author Aleksander Demko
 */ 
template <class T> class scopira::tool::array_view
{
  public:
    typedef T data_type;
    typedef T* iterator;
    typedef const T* const_iterator;
  protected:
    T *dm_ary, *dm_end;

  public:
    /**
     * Converting ctor
     * @author Aleksander Demko
     */
    array_view(basic_array<T> &rhs) : dm_ary(rhs.begin()), dm_end(rhs.end()) { }
    /**
     * Converting ctor
     * @author Aleksander Demko
     */
    array_view(T *b, T *e) : dm_ary(b), dm_end(e) { }
    /**
     * Converting ctor
     * @author Aleksander Demko
     */
    template <size_t N>
      array_view(fixed_array<T, N> &rhs) : dm_ary(rhs.begin()), dm_end(rhs.end()) { }
    /**
     * Converting ctor
     * @author Aleksander Demko
     */
    template <size_t N>
      array_view(hybrid_array<T, N> &rhs) : dm_ary(rhs.begin()), dm_end(rhs.end()) { }
    /**
     * Raw access to the array
     * @author Aleksander Demko
     */ 
    T * array(void) { return dm_ary; }
    /**
     * Raw access to the array
     * @author Aleksander Demko
     */ 
    const T * array(void) const { return dm_ary; }
    /**
     * Raw access to the array
     * @author Aleksander Demko
     */ 
    T * c_array(void) { return dm_ary; }
    /**
     * Raw access to the array
     * @author Aleksander Demko
     */ 
    const T * c_array(void) const { return dm_ary; }
    /**
     * STL-like iteration
     * @author Aleksander Demko
     */
    iterator begin(void) { return dm_ary; }
    /**
     * STL-like iteration
     * @author Aleksander Demko
     */
    iterator end(void) { return dm_end; }
    /**
     * STL-like iteration
     * @author Aleksander Demko
     */
    const_iterator begin(void) const { return dm_ary; }
    /**
     * STL-like iteration
     * @author Aleksander Demko
     */
    const_iterator end(void) const { return dm_end; }

    /**
     * Clears all the array values to binary 0. Does not
     * resize the array. This is dangerous on non-POD
     * classes.
     * @author Aleksander Demko
     */ 
    void clear_zero(void) { ::memset(dm_ary, 0, sizeof(T)*(size())); }

    /**
     * Gets the size of the array
     * @author Aleksander Demko
     */
    size_t size(void) const { return dm_end - dm_ary; }
    /**
     * Is the array empty (0 length)?
     * @author Aleksander Demko
     */ 
    bool empty(void) const { return dm_ary == dm_end; }

    /**
     * Individual element setter
     * @author Aleksander Demko
     */ 
    void set(size_t idx, const T &v) {
      assert("[array_view element access was out of bounds]" && (dm_ary+idx)<dm_end);
      dm_ary[idx] = v;
    }
    /**
     * Individual element setter
     * @author Aleksander Demko
     */ 
    const T & get(size_t idx) const {
      assert("[array_view element access was out of bounds]" && (dm_ary+idx)<dm_end);
      return dm_ary[idx];
    }
    /**
     * Individual element setter
     * @author Aleksander Demko
     */ 
    T & get(size_t idx) {
      assert("[array_view element access was out of bounds]" && (dm_ary+idx)<dm_end);
      return dm_ary[idx];
    }
    /**
     * Nice reference access
     * @author Aleksander Demko
     */ 
    const T & operator[](size_t idx) const {
      assert("[array_view element access was out of bounds]" && (dm_ary+idx)<dm_end);
      return dm_ary[idx];
    }
    /**
     * Nice reference access
     * @author Aleksander Demko
     */ 
    T & operator[](size_t idx) {
      assert("[array_view element access was out of bounds]" && (dm_ary+idx)<dm_end);
      return dm_ary[idx];
    }
};

/**
 * A (non-resizable) "view" of another array
 * @author Aleksander Demko
 */ 
template <class T> class scopira::tool::const_array_view
{
  public:
    typedef T data_type;
    typedef const T* const_iterator;
  protected:
    const T *dm_ary, *dm_end;

  public:
    /**
     * One can make const version out of non-const versions
     * (but not visa versa)
     * @author Aleksander Demko
     */
    const_array_view(array_view<T> rhs) : dm_ary(rhs.begin()), dm_end(rhs.end()) { }
    /**
     * Converting ctor
     * @author Aleksander Demko
     */
    const_array_view(const basic_array<T> &rhs) : dm_ary(rhs.begin()), dm_end(rhs.end()) { }
    /**
     * Converting ctor
     * @author Aleksander Demko
     */
    const_array_view(const T *b, const T *e) : dm_ary(b), dm_end(e) { }
    /**
     * Converting ctor
     * @author Aleksander Demko
     */
    template <size_t N>
      const_array_view(const fixed_array<T, N> &rhs) : dm_ary(rhs.begin()), dm_end(rhs.end()) { }
    /**
     * Converting ctor
     * @author Aleksander Demko
     */
    template <size_t N>
      const_array_view(const hybrid_array<T, N> &rhs) : dm_ary(rhs.begin()), dm_end(rhs.end()) { }
    /**
     * Raw access to the array
     * @author Aleksander Demko
     */ 
    const T * array(void) const { return dm_ary; }
    /**
     * Raw access to the array
     * @author Aleksander Demko
     */ 
    const T * c_array(void) const { return dm_ary; }
    /**
     * STL-like iteration
     * @author Aleksander Demko
     */
    const_iterator begin(void) const { return dm_ary; }
    /**
     * STL-like iteration
     * @author Aleksander Demko
     */
    const_iterator end(void) const { return dm_end; }

    /**
     * Gets the size of the array
     * @author Aleksander Demko
     */
    size_t size(void) const { return dm_end - dm_ary; }
    /**
     * Is the array empty (0 length)?
     * @author Aleksander Demko
     */ 
    bool empty(void) const { return dm_ary == dm_end; }

    /**
     * Individual element setter
     * @author Aleksander Demko
     */ 
    const T & get(size_t idx) const {
      assert("[const_array_view element access was out of bounds]" && (dm_ary+idx)<dm_end);
      return dm_ary[idx];
    }
    /**
     * Nice reference access
     * @author Aleksander Demko
     */ 
    const T & operator[](size_t idx) const {
      assert("[const_array_view element access was out of bounds]" && (dm_ary+idx)<dm_end);
      return dm_ary[idx];
    }
};

/**
 * A generic, copyable array class. This should always
 * be used instead of dynamic C-style arrays.
 *
 * However, unlike std::vector, this one does NOT
 * do incremental growth (or shrinkage)
 *
 * @author Aleksander Demko
 */
template <class T> class scopira::tool::basic_array
{
  public:
    typedef T data_type;
    typedef T* iterator;
    typedef const T* const_iterator;
  protected:
    T *dm_ary;
    size_t dm_len;
  public:
    /**
     * Default constructor - makes a 0-length array.
     * @author Aleksander Demko
     */ 
    basic_array(void) : dm_ary(0), dm_len(0) { }
    /**
     * Initialization constructor - makes a array
     * of the given size. Each object will
     * be initializaed via its default constructor
     * (thus primitives will have undefined values).
     *
     * @param len the length of the new array
     */
    explicit basic_array(size_t len) : dm_ary(0), dm_len(0) { resize(len); }
    /**
     * Deep-copy constructor.
     * @author Aleksander Demko
     */ 
    basic_array(const basic_array &rhs);
    /// Destructor
    ~basic_array() { resize(0); }

    /**
     * Resizes the array. Please note the following:
     * 1) If the new size is the current size, nothing
     * will be done. 2) Resizes are destructive. All old
     * values will be lost, and a new array is constructed
     * (following the initialization rules specified in
     * the constructor description)
     *
     * @param newlen the new length 
     * @author Aleksander Demko
     */  
    void resize(size_t newlen);

    /**
      * Trade internal strucuters with the given array. This is useful
      * for implementing non-destructive resize methods externally.
      * An internal implementation goes against the principals.
      * of this structure.
      *
      * @param ary The array to swap with.
      * @author Aleksander Demko
      */
    void array_swap(basic_array<T> &other) {
      T *ary = dm_ary;
      size_t len = dm_len;
      dm_ary = other.dm_ary;
      dm_len = other.dm_len;
      other.dm_ary = ary;
      other.dm_len = len;
    }
    /**
     * Raw access to the array
     * @author Aleksander Demko
     */ 
    T * array(void) { return dm_ary; }
    /**
     * Raw access to the array
     * @author Aleksander Demko
     */ 
    const T * array(void) const { return dm_ary; }
    /**
     * Raw access to the array
     * @author Aleksander Demko
     */ 
    T * c_array(void) { return dm_ary; }
    /**
     * Raw access to the array
     * @author Aleksander Demko
     */ 
    const T * c_array(void) const { return dm_ary; }
    /**
     * STL-like iteration
     * @author Aleksander Demko
     */
    iterator begin(void) { return dm_ary; }
    /**
     * STL-like iteration
     * @author Aleksander Demko
     */
    iterator end(void) { return dm_ary+dm_len; }
    /**
     * STL-like iteration
     * @author Aleksander Demko
     */
    const_iterator begin(void) const { return dm_ary; }
    /**
     * STL-like iteration
     * @author Aleksander Demko
     */
    const_iterator end(void) const { return dm_ary+dm_len; }

    /**
     * Copy Operator. Does a deep copy.
     * @author Aleksander Demko
     */ 
    basic_array & operator=(const basic_array &rhs);
    /**
     * Does a resize(0)
     * @author Aleksander Demko
     */ 
    void clear(void) { resize(0); }
    /**
     * Clears all the array values to binary 0. Does not
     * resize the array. This is dangerous on non-POD
     * classes.
     * @author Aleksander Demko
     */ 
    void clear_zero(void) { ::memset(dm_ary, 0, sizeof(T)*dm_len); }

    /**
     * Gets the size of the array
     * @author Aleksander Demko
     */
    size_t size(void) const { return dm_len; }
    /**
     * Is the array empty (0 length)?
     * @author Aleksander Demko
     */ 
    bool empty(void) const { return dm_len == 0; }

    /**
     * Individual element setter
     * @author Aleksander Demko
     */ 
    void set(size_t idx, const T &v) {
      assert("[basic_array element access was out of bounds]" && idx<dm_len);
      dm_ary[idx] = v;
    }
    /**
     * Individual element setter
     * @author Aleksander Demko
     */ 
    const T & get(size_t idx) const {
      assert("[basic_array element access was out of bounds]" && idx<dm_len);
      return dm_ary[idx];
    }
    /**
     * Individual element setter
     * @author Aleksander Demko
     */ 
    T & get(size_t idx) {
      assert("[basic_array element access was out of bounds]" && idx<dm_len);
      return dm_ary[idx];
    }
    /**
     * Nice reference access
     * @author Aleksander Demko
     */ 
    const T & operator[](size_t idx) const {
      assert("[basic_array element access was out of bounds]" && idx<dm_len);
      return dm_ary[idx];
    }
    /**
     * Nice reference access
     * @author Aleksander Demko
     */ 
    T & operator[](size_t idx) {
      assert("[basic_array element access was out of bounds]" && idx<dm_len);
      return dm_ary[idx];
    }
};

template <class T>
  scopira::tool::basic_array<T>::basic_array(const basic_array &rhs)
  : dm_ary(0), dm_len(0)
{
  if (rhs.dm_len>0) {
    resize(rhs.dm_len);
    iterator ii, endii = end();
    const_iterator jj;
    // copy
    for (ii=begin(), jj=rhs.begin(); ii != endii; ++ii, ++jj)
      *ii = *jj;
  }
}

template <class T>
  void scopira::tool::basic_array<T>::resize(size_t newlen)
{
  if (dm_len == newlen)
    return; // no need to change anything
  if (dm_len > 0)
    delete []dm_ary;
  dm_len = newlen;
  if (dm_len == 0)
    dm_ary = 0;
  else
    dm_ary = new T[dm_len];
}

template <class T>
  scopira::tool::basic_array<T> & scopira::tool::basic_array<T>::operator=(const basic_array &rhs)
{
  resize(rhs.dm_len);

  iterator ii, endii = end();
  const_iterator jj;
  // copy
  for (ii=begin(), jj=rhs.begin(); ii != endii; ++ii, ++jj)
    *ii = *jj;

  return *this;
}

/**
 * A basic_matrix adds 2 dimentional (matrix) like operations
 * to basic_array.
 * @author Aleksander Demko
 */ 
template <class T> class scopira::tool::basic_matrix : public scopira::tool::basic_array<T>
{
  private:
    typedef scopira::tool::basic_array<T> parent_type;
  protected:
    size_t dm_w, dm_h;
  public:
    /**
     * Makes a 0 by 0 matrix.
     * @author Aleksander Demko
     */ 
    basic_matrix(void) {
      dm_w = dm_h = 0;
    }
    /**
     * Makes a w by h matrix.
     * @author Aleksander Demko
     */ 
    basic_matrix(size_t w, size_t h) {
      dm_w = w;
      dm_h = h;
      resize(dm_w*dm_h);
    }
    /**
     * Resizes the matrix to the new, given dimensions.
     * Please see basic_array::resize for what happens
     * to the data.
     * @author Aleksander Demko
     */ 
    void resize(size_t w, size_t h) {
      dm_w = w;
      dm_h = h;
      resize(dm_w*dm_h);
    }

    /**
     * Returns the width of the matrix.
     * @author Aleksander Demko
     */ 
    size_t width(void) const { return dm_w; }
    /**
     * Returns the height of the matrix.
     * @author Aleksander Demko
     */ 
    size_t height(void) const { return dm_h; }

    /**
     * Individual element setter
     * @author Aleksander Demko
     */ 
    void set(size_t x, size_t y, const T &v) {
      assert("[basic_matrix x axis out of bounds]" && x<dm_w);
      assert("[basic_matrix y axis out of bounds]" && y<dm_h);
      basic_array<T>::dm_ary[y*dm_w+x] = v;
    }
    /**
     * Individual element setter
     * @author Aleksander Demko
     */ 
    const T & get(size_t x, size_t y) const {
      assert("[basic_matrix x axis out of bounds]" && x<dm_w);
      assert("[basic_matrix y axis out of bounds]" && y<dm_h);
      return basic_array<T>::dm_ary[y*dm_w+x];
    }
    /**
     * Individual element setter
     * @author Aleksander Demko
     */ 
    T & get(size_t x, size_t y) {
      assert("[basic_matrix x axis out of bounds]" && x<dm_w);
      assert("[basic_matrix y axis out of bounds]" && y<dm_h);
      return basic_array<T>::dm_ary[y*dm_w+x];
    }
    /**
     * Nice reference access
     * @author Aleksander Demko
     */ 
    const T & operator()(size_t x, size_t y) const {
      assert("[basic_matrix x axis out of bounds]" && x<dm_w);
      assert("[basic_matrix y axis out of bounds]" && y<dm_h);
      return basic_array<T>::dm_ary[y*dm_w+x];
    }
    /**
     * Nice reference access
     * @author Aleksander Demko
     */ 
    T & operator()(size_t x, size_t y) {
      assert("[basic_matrix x axis out of bounds]" && x<dm_w);
      assert("[basic_matrix y axis out of bounds]" && y<dm_h);
      return basic_array<T>::dm_ary[y*dm_w+x];
    }

  protected:
    // lets hide this from the 2D users
    void resize(size_t newlen) {
      parent_type::resize(newlen);
    }
};

/**
 * A FIXED size array of type T and length N. This is exactly
 * the same as doing T[N], but nicer.
 *
 * Default constructors and destructors are fine. This object
 * is POD-like and thus is fully copyable.
 *
 * See tool/util for a handy string to fixed_array routine (string_to_array)
 * if you'd like to use fixed_array as a safe C string.
 *
 * @author Aleksander Demko
 */
template <class T, size_t N> class scopira::tool::fixed_array
{
  public:
    typedef T data_type;
    typedef T* iterator;
    enum { size_c = N };
    typedef const T* const_iterator;
  protected:
    T dm_ary[N];

  public:
    /**
     * Raw access to the array
     * @author Aleksander Demko
     */ 
    T * array(void) { return dm_ary; }
    /**
     * Raw access to the array
     * @author Aleksander Demko
     */ 
    const T * array(void) const { return dm_ary; }
    /**
     * Raw access to the array
     * @author Aleksander Demko
     */ 
    T * c_array(void) { return dm_ary; }
    /**
     * Raw access to the array
     * @author Aleksander Demko
     */ 
    const T * c_array(void) const { return dm_ary; }
    /**
     * STL-like iteration
     * @author Aleksander Demko
     */
    iterator begin(void) { return dm_ary; }
    /**
     * STL-like iteration
     * @author Aleksander Demko
     */
    iterator end(void) { return dm_ary+N; }
    /**
     * STL-like iteration
     * @author Aleksander Demko
     */
    const_iterator begin(void) const { return dm_ary; }
    /**
     * STL-like iteration
     * @author Aleksander Demko
     */
    const_iterator end(void) const { return dm_ary+N; }
    /**
     * Clears all the array values to binary 0. Does not
     * resize the array. This is dangerous on non-POD
     * classes.
     * @author Aleksander Demko
     */ 
    void clear_zero(void) { ::memset(dm_ary, 0, sizeof(T)*N); }
    /**
     * Gets the size of the array
     * @author Aleksander Demko
     */
    size_t size(void) const { return N; }
    /**
     * Is the array empty (0 length)?
     * @author Aleksander Demko
     */ 
    bool empty(void) const { return false; }

    /**
     * Individual element setter
     * @author Aleksander Demko
     */ 
    void set(size_t idx, const T &v) {
      assert("[fixed_array element access out of bounds]" && idx<N);
      dm_ary[idx] = v;
    }
    /**
     * Individual element setter
     * @author Aleksander Demko
     */ 
    const T & get(size_t idx) const {
      assert("[fixed_array element access out of bounds]" && idx<N);
      return dm_ary[idx];
    }
    /**
     * Individual element setter
     * @author Aleksander Demko
     */ 
    T & get(size_t idx) {
      assert("[fixed_array element access out of bounds]" && idx<N);
      return dm_ary[idx];
    }
    /**
     * Nice reference access
     * @author Aleksander Demko
     */ 
    const T & operator[](size_t idx) const {
      assert("[fixed_array element access out of bounds]" && idx<N);
      return dm_ary[idx];
    }
    /**
     * Nice reference access
     * @author Aleksander Demko
     */ 
    T & operator[](size_t idx) {
      assert("[fixed_array element access out of bounds]" && idx<N);
      return dm_ary[idx];
    }
};

/**
 * This is a resizable array (like basic_array)
 * thay uses an internal fixed_array (non-heap using)
 * buffer if possible. Very useful for when the array's
 * size rarely exceeds a certain (relativly small) size,
 * and if it does, it'll switch to keep allocation.
 *
 * Note, that this class does some internally duplication. You
 * should probably only use this for simple, smaller types.
 *
 * RES is the fixed_array reserve size -- after this, the
 * heap will be used.
 *
 * @author Aleksander Demko
 */ 
template <class T, size_t RES> class scopira::tool::hybrid_array
{
  public:
    typedef T data_type;
    typedef T* iterator;
    typedef const T* const_iterator;
  protected:
    fixed_array<T, RES> dm_fixed;
    basic_array<T> dm_basic;

    T *dm_ary;
    size_t dm_len;

  public:
    /**
     * Default constructor: makes a 0-length array.
     * @author Aleksander Demko
     */ 
    hybrid_array(void);
    /**
     * Initialization constructor - makes a array
     * of the given size. Each object will
     * be initializaed via its default constructor
     * (thus primitives will have undefined values).
     *
     * @param len the length of the new array
     */
    explicit hybrid_array(size_t len);
    /**
     * Deep-copy constructor.
     * @author Aleksander Demko
     */ 
    hybrid_array(const hybrid_array &rhs);
    /**
     * Deep-copy constructor.
     * @author Aleksander Demko
     */ 
    hybrid_array(const_array_view<T> rhs);
    /// Destructor
    ~hybrid_array() { }

    /**
     * Resizes the array. Please note the following:
     * 1) If the new size is the current size, nothing
     * will be done. 2) Resizes are destructive. All old
     * values will be lost, and a new array is constructed
     * (following the initialization rules specified in
     * the constructor description)
     *
     * @param newlen the new length 
     * @author Aleksander Demko
     */  
    void resize(size_t newlen);
    /**
     * Raw access to the array
     * @author Aleksander Demko
     */ 
    T * array(void) { return dm_ary; }
    /**
     * Raw access to the array
     * @author Aleksander Demko
     */ 
    const T * array(void) const { return dm_ary; }
    /**
     * Raw access to the array
     * @author Aleksander Demko
     */ 
    T * c_array(void) { return dm_ary; }
    /**
     * Raw access to the array
     * @author Aleksander Demko
     */ 
    const T * c_array(void) const { return dm_ary; }
    /**
     * STL-like iteration
     * @author Aleksander Demko
     */
    iterator begin(void) { return dm_ary; }
    /**
     * STL-like iteration
     * @author Aleksander Demko
     */
    iterator end(void) { return dm_ary+dm_len; }
    /**
     * STL-like iteration
     * @author Aleksander Demko
     */
    const_iterator begin(void) const { return dm_ary; }
    /**
     * STL-like iteration
     * @author Aleksander Demko
     */
    const_iterator end(void) const { return dm_ary+dm_len; }

    /**
     * Copy Operator. Does a deep copy.
     * @author Aleksander Demko
     */ 
    hybrid_array & operator=(const hybrid_array &rhs);
    /**
     * Copy Operator. Does a deep copy.
     * @author Aleksander Demko
     */ 
    hybrid_array & operator=(const_array_view<T> rhs);
    /**
     * Does a resize(0)
     * @author Aleksander Demko
     */ 
    void clear(void) { resize(0); }
    /**
     * Clears all the array values to binary 0. Does not
     * resize the array. This is dangerous on non-POD
     * classes.
     * @author Aleksander Demko
     */ 
    void clear_zero(void) { ::memset(dm_ary, 0, sizeof(T)*dm_len); }

    /**
     * Gets the size of the array
     * @author Aleksander Demko
     */
    size_t size(void) const { return dm_len; }
    /**
     * Is the array empty (0 length)?
     * @author Aleksander Demko
     */ 
    bool empty(void) const { return dm_len == 0; }

    /**
     * Individual element setter
     * @author Aleksander Demko
     */ 
    void set(size_t idx, const T &v) {
      assert("[hybrid_array element access was out of bounds]" && idx<dm_len);
      dm_ary[idx] = v;
    }
    /**
     * Individual element setter
     * @author Aleksander Demko
     */ 
    const T & get(size_t idx) const {
      assert("[hybrid_array element access was out of bounds]" && idx<dm_len);
      return dm_ary[idx];
    }
    /**
     * Individual element setter
     * @author Aleksander Demko
     */ 
    T & get(size_t idx) {
      assert("[hybrid_array element access was out of bounds]" && idx<dm_len);
      return dm_ary[idx];
    }
    /**
     * Nice reference access
     * @author Aleksander Demko
     */ 
    const T & operator[](size_t idx) const {
      assert("[hybrid_array element access was out of bounds]" && idx<dm_len);
      return dm_ary[idx];
    }
    /**
     * Nice reference access
     * @author Aleksander Demko
     */ 
    T & operator[](size_t idx) {
      assert("[hybrid_array element access was out of bounds]" && idx<dm_len);
      return dm_ary[idx];
    }
};

template <class T, size_t RES>
  scopira::tool::hybrid_array<T,RES>::hybrid_array(void)
  : dm_ary(0), dm_len(0)
{
}

template <class T, size_t RES>
  scopira::tool::hybrid_array<T,RES>::hybrid_array(size_t len)
  : dm_ary(0), dm_len(0)
{
  resize(len);
}

template <class T, size_t RES>
  scopira::tool::hybrid_array<T,RES>::hybrid_array(const hybrid_array &rhs)
  : dm_ary(0), dm_len(0)
{
  resize(rhs.size());
  if (!empty()) {
    iterator ii, endii = end();
    const_iterator jj;
    // copy
    for (ii=begin(), jj=rhs.begin(); ii != endii; ++ii, ++jj)
      *ii = *jj;
  }
}

template <class T, size_t RES>
  scopira::tool::hybrid_array<T,RES>::hybrid_array(const_array_view<T> rhs)
  : dm_ary(0), dm_len(0)
{
  resize(rhs.size());
  if (!empty()) {
    iterator ii, endii = end();
    const_iterator jj;
    // copy
    for (ii=begin(), jj=rhs.begin(); ii != endii; ++ii, ++jj)
      *ii = *jj;
  }
}

template <class T, size_t RES>
  void scopira::tool::hybrid_array<T,RES>::resize(size_t newlen)
{
  if (dm_len == newlen)
    return; // no need to change anything
  if (dm_len > RES)
    dm_basic.resize(0);
  dm_len = newlen;
  if (dm_len == 0)
    dm_ary = 0;
  else if (dm_len <= RES)
    dm_ary = dm_fixed.c_array();
  else {
    dm_basic.resize(dm_len);
    dm_ary = dm_basic.c_array();
  }
}

template <class T, size_t RES>
  scopira::tool::hybrid_array<T,RES> & scopira::tool::hybrid_array<T,RES>::operator=(const hybrid_array &rhs)
{
  resize(rhs.dm_len);

  iterator ii, endii = end();
  const_iterator jj;
  // copy
  for (ii=begin(), jj=rhs.begin(); ii != endii; ++ii, ++jj)
    *ii = *jj;

  return *this;
}

template <class T, size_t RES>
  scopira::tool::hybrid_array<T,RES> & scopira::tool::hybrid_array<T,RES>::operator=(const_array_view<T> rhs)
{
  resize(rhs.size());

  iterator ii, endii = end();
  const_iterator jj;
  // copy
  for (ii=begin(), jj=rhs.begin(); ii != endii; ++ii, ++jj)
    *ii = *jj;

  return *this;
}

#endif

