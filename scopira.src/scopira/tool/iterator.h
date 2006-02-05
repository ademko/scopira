
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

#ifndef __INCLUDED__SCOPIRA_TOOL_ITERATOR_H__
#define __INCLUDED__SCOPIRA_TOOL_ITERATOR_H__

#include <assert.h>

#include <scopira/tool/object.h>

namespace scopira
{
  namespace tool
  {
    // general interface
    template <class T> class iterator_g;
    // empty iterator
    template <class T> class null_iterator_imp_g;

    // imp
    template <class T> class iterator_imp_g;
  }
}

/**
 * the implementation class that will be developed
 *
 * @author Aleksander Demko
 */
template <class T> class scopira::tool::iterator_imp_g : public virtual scopira::tool::object
{
  public:
    typedef T data_type;

    /// gets the current item
    virtual T current(void) = 0;
    /// is there a next one?
    virtual bool valid(void) const = 0;
    /// advance to the next item
    virtual void next(void) = 0;
};

/**
 * an iterator_imp that does nothing
 *
 * @author Aleksander Demko
 */
template <class T> class scopira::tool::null_iterator_imp_g : public scopira::tool::iterator_imp_g<T>
{
  public:
    /// gets the current item
    virtual T current(void) { return 0; } //hopefully, a valid-compile time cast (should never be executed at runtime)
    /// is there a next one?
    virtual bool valid(void) const { return false; }
    /// advance to the next item
    virtual void next(void) { }
};

/**
 * a virtual iterator spec - this is really an autoptr
 * around a simpler, more specialized iterator-core
 * that is implemented by the developer per use
 *
 * make a const one?
 *
 * @author Aleksander Demko
 */
template <class T> class scopira::tool::iterator_g
{
  public:
    typedef T data_type;
    typedef null_iterator_imp_g<T> null_type;

    /// default ctor - can be used as a 0-element iterator or end-iterator comparator
    iterator_g(void)
      { }
    /// initing ctor
    iterator_g(iterator_imp_g<T> *imp)
      : m_imp(imp) { }
    /// copy ctor
    iterator_g(const iterator_g &src)
      : m_imp(src.m_imp) { }
    /// dtor
    ~iterator_g() { }

    /// get current 
    T operator *(void) {
      assert(m_imp.get());
      return m_imp->current();
    }
    /// better than comparing to a specific EOF iterator
    bool valid(void) const {
      if (m_imp.get())
        return m_imp->valid();
      else  
        return 0;
    }
    /// for comparing to EOF only
    bool operator ==(const iterator_g &rhs) const {
      assert(!rhs.m_imp.get());
      return !valid();
    }
    /// for comparing to EOF only
    bool operator ==(int x) const {
      assert(x == 0);
      return !valid();
    }
    /// for comparing to EOF only
    bool operator !=(const iterator_g &rhs) const {
      assert(!rhs.m_imp.get());
      return valid();
    }
    /// for comparing to EOF only
    bool operator !=(int x) const {
      assert(x == 0);
      return valid();
    }
    iterator_g& operator ++(void) {
      assert(m_imp.get());
      assert(m_imp->valid());
      m_imp->next();
      return *this;
    }
    iterator_g& operator =(const iterator_g &rhs) {
      m_imp = rhs.m_imp;
      return *this;
    }

  private:
    scopira::tool::count_ptr< iterator_imp_g<T> > m_imp;
};

#endif

