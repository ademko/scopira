
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

#ifndef __INCLUDED__SCOPIRA_TOOL_ITERATOR_IMP_H__
#define __INCLUDED__SCOPIRA_TOOL_ITERATOR_IMP_H__

// the following is for developers who want to quickly build
// iterator_imps from stl based ones 

#include <scopira/tool/iterator.h>

namespace scopira
{
  namespace tool
  {
    template <class T> struct deref {
      typedef T in_type;
      T operator()(T r) const { return r; }
    };
    template <class TOUT, class DEF, class TIN = typename DEF::in_type > struct deref_first {
      typedef TIN in_type;
      TOUT operator()(TIN r) { return DEF()(r).first; }
    };
    template <class TOUT, class DEF, class TIN = typename DEF::in_type > struct deref_second {
      typedef TIN in_type;
      TOUT operator()(TIN r) { return DEF()(r).second; }
    };
    template <class TOUT, class DEF, class TIN = typename DEF::in_type > struct deref_ptr {
      typedef TIN in_type;
      TOUT operator()(TIN a) { return *DEF()(a); }
    };
    template <class TOUT, class DEF, class TIN = typename DEF::in_type > struct deref_objauto {
      typedef TIN in_type;
      TOUT operator()(TIN a) { return DEF()(a).get(); }
    };
    template <class ITER, class T, class DEF > class stl_iterator_imp_g;

    // utlity for for_each
  }
}

/**
 * an iterator_imp that does standard stl iterators
 *
 * @author Aleksander Demko
 */
template <class ITER, class T, class DEF> class scopira::tool::stl_iterator_imp_g
  : public scopira::tool::iterator_imp_g<T>
{
  protected:
    ITER m_current, m_end;

  public:
    stl_iterator_imp_g(const ITER & _start, const ITER & _end)
      : m_current(_start), m_end(_end) { }
    /// gets the current item
    virtual T current(void) {
      return DEF()(*m_current);
    }
    /// is there a next one?
    virtual bool valid(void) const {
      return m_current != m_end;
    }
    /// advance to the next item
    virtual void next(void) {
      m_current++;
    }
};

#endif

