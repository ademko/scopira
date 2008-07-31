
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

#ifndef __INCLUDED_SCOPIRA_BASEKIT_NARRAY_H__
#define __INCLUDED_SCOPIRA_BASEKIT_NARRAY_H__

#include <iostream>
#include <iomanip>

#include <scopira/tool/platform.h>
#include <scopira/tool/flow.h>
#include <scopira/tool/traits.h>
#include <scopira/tool/printflow.h>
#include <scopira/tool/export.h>

//
// Super Fine N-dimensional data structures
//
// Has D = {1, 2, 3, 4) specializations
// Will have a runtime dynamic narray with dynamic (basic_array)
// based coords, perhaps
//

namespace scopira
{
  /**
   * basekit provides basic math, array and direct I/O facilities.
   * @author Aleksander Demko
   */ 
  namespace basekit
  {
    enum {
      x_axis_c = 0,
      y_axis_c = 1,
      z_axis_c = 2,
      t_axis_c = 3,
    };

    //template <class T, int DIM> class nslice;
    template <int DIM = 1> class nindex;
    template <> class nindex<1>;
    template <> class nindex<2>;
    template <> class nindex<3>;
    template <> class nindex<4>;

    /**
     * Deleter of externally managed memory.
     *
     * @author Aleksander Demko
     */ 
    class narray_delete_i
    {
      public:
        virtual ~narray_delete_i() { }
        /**
         * This will be called when its time to nuke a particular direct narray.
         *
         * This function should also "delete this" (or subref itself if youve used
         * ref counting).
         *
         * @author Aleksander Demko
         */ 
        virtual void narray_delete(void *mem, size_t len) = 0;
    };

    /**
     * A narray delete that does nothing.
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT extern narray_delete_i *null_narray_delete;
    /**
     * This special narray delete tells that narray to assume the array
     * and "delete" it as per usual.
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT extern narray_delete_i *normal_narray_delete;

    template <class T, int DIM = 1> class narray;

    template <class T, int DIM = 1> class nslice;
    template <class T, int DIM = 1> class niterator;

    template <class T, int DIM = 1> class const_nslice;
    template <class T, int DIM = 1> class const_niterator;

    /// element printing helper
    template <class E>
      inline void print_element(scopira::tool::oflow_i &o, E el);   // default uses <<
    /// element printing helper (specialization)
    template <>
      inline void print_element<long>(scopira::tool::oflow_i &o, long el);
    /// element printing helper (specialization)
    template <>
      inline void print_element<unsigned long>(scopira::tool::oflow_i &o, unsigned long el);
    /// element printing helper (specialization)
    template <>
      inline void print_element<int>(scopira::tool::oflow_i &o, int el);
    /// element printing helper (specialization)
    template <>
      inline void print_element<unsigned int>(scopira::tool::oflow_i &o, unsigned int el);
    /// element printing helper (specialization)
    template <>
      inline void print_element<short>(scopira::tool::oflow_i &o, short el);
    /// element printing helper (specialization)
    template <>
      inline void print_element<char>(scopira::tool::oflow_i &o, char el);
    /// element printing helper (specialization)
    template <>
      inline void print_element<double>(scopira::tool::oflow_i &o, double el);
    /// element printing helper (specialization)
    template <>
      inline void print_element<float>(scopira::tool::oflow_i &o, float el);

    /// internal
    template <class C>
      scopira::tool::oflow_i & print_vector_slice(scopira::tool::oflow_i &o, const const_nslice<C,1> &V);
    /// internal
    template <class C>
      scopira::tool::oflow_i & print_matrix_slice(scopira::tool::oflow_i &o, const const_nslice<C,2> &M);

    /// internal
    template <class C>
      std::ostream & print_vector_slice(std::ostream &o, const const_nslice<C,1> &V);
    /// internal
    template <class C>
      std::ostream & print_matrix_slice(std::ostream &o, const const_nslice<C,2> &M);

    //
    // In the future, a N-dimen base interface class could be put between
    // narray_o and object so that people can do dimen independant stuff?
    //class narray_i;

    template <class T, int DIM = 1> class narray_o;

    //
    // many type defs, for convieinence...
    //
    /**
     * Simply typedef narray<bool,1> boolvec_t;
     * @author Aleksander Demko
     */ 
    typedef narray<bool,1> boolvec_t;
    /**
     * Simply typedef narray<char,1> charvec_t;
     * @author Aleksander Demko
     */ 
    typedef narray<char,1> charvec_t;
    /**
     * Simply typedef narray<short,1> shortvec_t;
     * @author Aleksander Demko
     */ 
    typedef narray<short,1> shortvec_t;
    /**
     * Simply typedef narray<int,1> intvec_t;
     * @author Aleksander Demko
     */ 
    typedef narray<int,1> intvec_t;
    /**
     * Simply typedef narray<float,1> floatvec_t;
     * @author Aleksander Demko
     */ 
    typedef narray<float,1> floatvec_t;
    /**
     * Simply typedef narray<double,1> doublevec_t;
     * @author Aleksander Demko
     */ 
    typedef narray<double,1> doublevec_t;

    /**
     * Simply typedef narray<bool,2> boolmatrix_t;
     * @author Aleksander Demko
     */ 
    typedef narray<bool,2> boolmatrix_t;
    /**
     * Simply typedef narray<char,2> charmatrix_t;
     * @author Aleksander Demko
     */ 
    typedef narray<char,2> charmatrix_t;
    /**
     * Simply typedef narray<short,2> shortmatrix_t;
     * @author Aleksander Demko
     */ 
    typedef narray<short,2> shortmatrix_t;
    /**
     * Simply typedef narray<int,2> intmatrix_t;
     * @author Aleksander Demko
     */ 
    typedef narray<int,2> intmatrix_t;
    /**
     * Simply typedef narray<float,2> floatmatrix_t;
     * @author Aleksander Demko
     */ 
    typedef narray<float,2> floatmatrix_t;
    /**
     * Simply typedef narray<double,2> doublematrix_t;
     * @author Aleksander Demko
     */ 
    typedef narray<double,2> doublematrix_t;

    /**
     * Simply typedef narray<bool,3> boolcube_t;
     * @author Aleksander Demko
     */ 
    typedef narray<bool,3> boolcube_t;
    /**
     * Simply typedef narray<char,3> charcube_t;
     * @author Aleksander Demko
     */ 
    typedef narray<char,3> charcube_t;
    /**
     * Simply typedef narray<short,3> shortcube_t;
     * @author Aleksander Demko
     */ 
    typedef narray<short,3> shortcube_t;
    /**
     * Simply typedef narray<int,3> intcube_t;
     * @author Aleksander Demko
     */ 
    typedef narray<int,3> intcube_t;
    /**
     * Simply typedef narray<float,3> floatcube_t;
     * @author Aleksander Demko
     */ 
    typedef narray<float,3> floatcube_t;
    /**
     * Simply typedef narray<double,3> doublecube_t;
     * @author Aleksander Demko
     */ 
    typedef narray<double,3> doublecube_t;

    /**
     * Simply typedef narray<bool,4> boolquad_t;
     * @author Aleksander Demko
     */ 
    typedef narray<bool,4> boolquad_t;
    /**
     * Simply typedef narray<char,4> charquad_t;
     * @author Aleksander Demko
     */ 
    typedef narray<char,4> charquad_t;
    /**
     * Simply typedef narray<short,4> shortquad_t;
     * @author Aleksander Demko
     */ 
    typedef narray<short,4> shortquad_t;
    /**
     * Simply typedef narray<int,4> intquad_t;
     * @author Aleksander Demko
     */ 
    typedef narray<int,4> intquad_t;
    /**
     * Simply typedef narray<float,4> floatquad_t;
     * @author Aleksander Demko
     */ 
    typedef narray<float,4> floatquad_t;
    /**
     * Simply typedef narray<double,4> doublequad_t;
     * @author Aleksander Demko
     */ 
    typedef narray<double,4> doublequad_t;

    /**
     * Simply typedef narray_o<bool,1> boolvec_o;
     * @author Aleksander Demko
     */ 
    typedef narray_o<bool,1> boolvec_o;
    /**
     * Simply typedef narray_o<char,1> charvec_o;
     * @author Aleksander Demko
     */ 
    typedef narray_o<char,1> charvec_o;
    /**
     * Simply typedef narray_o<short,1> shortvec_o;
     * @author Aleksander Demko
     */ 
    typedef narray_o<short,1> shortvec_o;
    /**
     * Simply typedef narray_o<int,1> intvec_o;
     * @author Aleksander Demko
     */ 
    typedef narray_o<int,1> intvec_o;
    /**
     * Simply typedef narray_o<float,1> floatvec_o;
     * @author Aleksander Demko
     */ 
    typedef narray_o<float,1> floatvec_o;
    /**
     * Simply typedef narray_o<double,1> doublevec_o;
     * @author Aleksander Demko
     */ 
    typedef narray_o<double,1> doublevec_o;

    /**
     * Simply typedef narray_o<bool,2> boolmatrix_o;
     * @author Aleksander Demko
     */ 
    typedef narray_o<bool,2> boolmatrix_o;
    /**
     * Simply typedef narray_o<char,2> charmatrix_o;
     * @author Aleksander Demko
     */ 
    typedef narray_o<char,2> charmatrix_o;
    /**
     * Simply typedef narray_o<short,2> shortmatrix_o;
     * @author Aleksander Demko
     */ 
    typedef narray_o<short,2> shortmatrix_o;
    /**
     * Simply typedef narray_o<int,2> intmatrix_o;
     * @author Aleksander Demko
     */ 
    typedef narray_o<int,2> intmatrix_o;
    /**
     * Simply typedef narray_o<float,2> floatmatrix_o;
     * @author Aleksander Demko
     */ 
    typedef narray_o<float,2> floatmatrix_o;
    /**
     * Simply typedef narray_o<double,2> doublematrix_o;
     * @author Aleksander Demko
     */ 
    typedef narray_o<double,2> doublematrix_o;

    /**
     * Simply typedef narray_o<bool,3> boolcube_o;
     * @author Aleksander Demko
     */ 
    typedef narray_o<bool,3> boolcube_o;
    /**
     * Simply typedef narray_o<char,3> charcube_o;
     * @author Aleksander Demko
     */ 
    typedef narray_o<char,3> charcube_o;
    /**
     * Simply typedef narray_o<short,3> shortcube_o;
     * @author Aleksander Demko
     */ 
    typedef narray_o<short,3> shortcube_o;
    /**
     * Simply typedef narray_o<int,3> intcube_o;
     * @author Aleksander Demko
     */ 
    typedef narray_o<int,3> intcube_o;
    /**
     * Simply typedef narray_o<float,3> floatcube_o;
     * @author Aleksander Demko
     */ 
    typedef narray_o<float,3> floatcube_o;
    /**
     * Simply typedef narray_o<double,3> doublecube_o;
     * @author Aleksander Demko
     */ 
    typedef narray_o<double,3> doublecube_o;

    /**
     * Simply typedef narray_o<bool,4> boolquad_o;
     * @author Aleksander Demko
     */ 
    typedef narray_o<bool,4> boolquad_o;
    /**
     * Simply typedef narray_o<char,4> charquad_o;
     * @author Aleksander Demko
     */ 
    typedef narray_o<char,4> charquad_o;
    /**
     * Simply typedef narray_o<short,4> shortquad_o;
     * @author Aleksander Demko
     */ 
    typedef narray_o<short,4> shortquad_o;
    /**
     * Simply typedef narray_o<int,4> intquad_o;
     * @author Aleksander Demko
     */ 
    typedef narray_o<int,4> intquad_o;
    /**
     * Simply typedef narray_o<float,4> floatquad_o;
     * @author Aleksander Demko
     */ 
    typedef narray_o<float,4> floatquad_o;
    /**
     * Simply typedef narray_o<double,4> doublequad_o;
     * @author Aleksander Demko
     */ 
    typedef narray_o<double,4> doublequad_o;
  }
}

// NINDEX

/**
 * A n-dimen coordinate specifier.
 * Lots of stuff inherited from fixed_array
 * @author Aleksander Demko
 */
template <int DIM> class scopira::basekit::nindex : public scopira::tool::fixed_array<size_t, DIM>
{
  private:
    typedef nindex<DIM> this_type;

  public:
    /// ctor, that inits
    nindex(size_t val);
    /// ctor, that does nothing
    nindex(void) { }

    /// comparison
    bool operator < (const this_type &rhs) const;
    /// equality
    bool operator == (const this_type &rhs) const;
    /// multiplication
    size_t operator* (const this_type &rhs) const;

    /// Return the product of this coord
    size_t product(void) const;
    /// calc ofset, using this as the size
    size_t offset(const this_type &c) const;
    /// calc a stride value array, using this as the size
    this_type strides(void) const;
    /// make a stair case step wise style 0, 1, 2, 3 etc
    static this_type steps(void);
    /// returns the same, but one less
    nindex<DIM-1> shrink(void) const;
};

/**
 * a 2d coord (specialization)
 *
 * @author Aleksander Demko
 */
template <>
class scopira::basekit::nindex<1> : public scopira::tool::fixed_array<size_t, 1>
{
  private:
    typedef scopira::tool::fixed_array<size_t, 1> parent_type;
    typedef nindex<1> this_type;

  public:
    /// ctor (implicit)
    nindex(size_t v=0) { dm_ary[0] = v; }
    /// clear
    void clear(size_t v = 0) { dm_ary[0] = v; }

    /// gets it multed
    size_t product(void) const { return dm_ary[0]; }
    /// calc ofset, using this as the size
    size_t offset(const this_type &c) const { return c.dm_ary[0]; }
    /// stride offset array
    this_type strides(void) const { return this_type(1); }
    /// returns [0]
    static this_type steps(void) { return this_type(0); }
    /// returns the same, but one less
    nindex<1> shrink(void) const { return nindex<1>(); } // gotta return somethin', and definatly not a 0-array

    /// comparison
    bool operator < (const this_type &rhs) const { return dm_ary[0]<rhs.dm_ary[0]; }
    /// equality
    bool operator == (const this_type &rhs) const { return dm_ary[0]==rhs.dm_ary[0]; }
    /// mult
    size_t operator *(const this_type &rhs) const { return dm_ary[0]*rhs.dm_ary[0]; }
    /// assignment
    const this_type & operator = (const this_type &rhs) { dm_ary[0] = rhs.dm_ary[0]; return *this; }

    /// ref access
    size_t & x(void) { return dm_ary[0]; }
    /// ref access
    size_t x(void) const { return dm_ary[0]; }

    /// set
    void set(size_t _x) { dm_ary[0] = _x; }
};

/**
 * a 2d coord (specialization)
 *
 * @author Aleksander Demko
 */
template <>
class scopira::basekit::nindex<2> : public scopira::tool::fixed_array<size_t, 2>
{
  private:
    typedef scopira::tool::fixed_array<size_t, 2> parent_type;
    typedef nindex<2> this_type;

  public:
    /// ctor
    nindex(size_t _x, size_t _y) { dm_ary[0] = _x; dm_ary[1] = _y; }
    /// ctor (implicit)
    nindex(size_t v=0) { clear(v); }
    /// clear
    void clear(size_t v = 0)
      { dm_ary[0] = v; dm_ary[1] = v; }

    /// gets it multed
    size_t product(void) const { return dm_ary[0]*dm_ary[1]; }
    /// calc ofset, using this as the size
    size_t offset(const this_type &c) const { return c.dm_ary[1]*dm_ary[0] + c.dm_ary[0]; }
    /// stride offset array
    this_type strides(void) const { return this_type(1, dm_ary[0]); }
    /// returns [0,1]
    static this_type steps(void) { return this_type(0,1); }
    /// returns the same, but one less
    nindex<1> shrink(void) const { return nindex<1>(dm_ary[0]); } // gotta return somethin'

    /// comparison
    bool operator < (const this_type &rhs) const { return dm_ary[0]<rhs.dm_ary[0] && dm_ary[1]<rhs.dm_ary[1]; }
    /// equality
    bool operator == (const this_type &rhs) const { return dm_ary[0]==rhs.dm_ary[0] && dm_ary[1]==rhs.dm_ary[1]; }
    /// mult
    size_t operator *(const this_type &rhs) const { return dm_ary[0]*rhs.dm_ary[0] + dm_ary[1]*rhs.dm_ary[1]; }
    /// assignment
    const this_type & operator = (const this_type &rhs) { dm_ary[0] = rhs.dm_ary[0]; dm_ary[1] = rhs.dm_ary[1]; return *this; }

    /// ref access
    size_t & x(void) { return dm_ary[0]; }
    /// ref access
    size_t x(void) const { return dm_ary[0]; }
    /// ref access
    size_t & y(void) { return dm_ary[1]; }
    /// ref access
    size_t y(void) const { return dm_ary[1]; }

    /// 2-set
    void set_xy(size_t _x, size_t _y) { dm_ary[0] = _x; dm_ary[1] = _y; }
};

/**
 * a 3d coord (specialization)
 *
 * @author Aleksander Demko
 */
template <>
class scopira::basekit::nindex<3> : public scopira::tool::fixed_array<size_t, 3>
{
  private:
    typedef scopira::tool::fixed_array<size_t, 3> parent_type;
    typedef nindex<3> this_type;

  public:
    /// ctor
    SCOPIRA_EXPORT nindex(size_t _x, size_t _y, size_t _z=0);
    /// ctor (implicit)
    SCOPIRA_EXPORT nindex(size_t v=0);
    /// copy ctor
    SCOPIRA_EXPORT nindex(const this_type &rhs);
    /// clear
    void clear(size_t v = 0)
      { dm_ary[0] = v; dm_ary[1] = v; dm_ary[2] = v; }

    /// gets it multed
    size_t product(void) const { return dm_ary[0]*dm_ary[1]*dm_ary[2]; }
    /// calc ofset, using this as the size
    size_t offset(const this_type &c) const { return c.dm_ary[2]*dm_ary[1]*dm_ary[0] + c.dm_ary[1]*dm_ary[0] + c.dm_ary[0]; }
    /// stride offset array
    this_type strides(void) const { return this_type(1, dm_ary[0], dm_ary[0]*dm_ary[1]); }
    /// returns [0,1,2]
    static this_type steps(void) { return this_type(0,1,2); }
    /// returns the same, but one less
    nindex<2> shrink(void) const { return nindex<2>(dm_ary[0], dm_ary[1]); } // gotta return somethin'

    /// comparison
    bool operator < (const this_type &rhs) const { return dm_ary[0]<rhs.dm_ary[0] && dm_ary[1]<rhs.dm_ary[1] && dm_ary[2]<rhs.dm_ary[2]; }
    /// equality
    bool operator == (const this_type &rhs) const { return dm_ary[0]==rhs.dm_ary[0] && dm_ary[1]==rhs.dm_ary[1] && dm_ary[2]==rhs.dm_ary[2]; }
    /// mult
    size_t operator *(const this_type &rhs) const { return dm_ary[0]*rhs.dm_ary[0] + dm_ary[1]*rhs.dm_ary[1] + dm_ary[2]*rhs.dm_ary[2]; }
    /// assignment
    const this_type & operator = (const this_type &rhs) { dm_ary[0] = rhs.dm_ary[0]; dm_ary[1] = rhs.dm_ary[1]; dm_ary[2] = rhs.dm_ary[2]; return *this; }

    /// ref access
    size_t & x(void) { return dm_ary[0]; }
    /// ref access
    size_t x(void) const { return dm_ary[0]; }
    /// ref access
    size_t & y(void) { return dm_ary[1]; }
    /// ref access
    size_t y(void) const { return dm_ary[1]; }
    /// ref access
    size_t & z(void) { return dm_ary[2]; }
    /// ref access
    size_t z(void) const { return dm_ary[2]; }
};

/**
 * a 4d coord (specialization)
 *
 * @author Aleksander Demko
 */
template <>
class scopira::basekit::nindex<4> : public scopira::tool::fixed_array<size_t, 4>
{
  private:
    typedef scopira::tool::fixed_array<size_t, 4> parent_type;
    typedef nindex<4> this_type;

  public:
    /// ctor
    SCOPIRA_EXPORT nindex(size_t _x, size_t _y, size_t _z=0, size_t _t=0);
    /// ctor (implicit!)
    SCOPIRA_EXPORT nindex(size_t v=0);
    /// copy ctor
    SCOPIRA_EXPORT nindex(const this_type &rhs);
    /// clear
    void clear(size_t v = 0)
      { dm_ary[0] = v; dm_ary[1] = v; dm_ary[2] = v; dm_ary[3] = v; }

    /// gets it multed
    size_t product(void) const { return dm_ary[0]*dm_ary[1]*dm_ary[2]*dm_ary[3]; }
    /// calc ofset, using this as the size
    size_t offset(const this_type &c) const { return c.dm_ary[3]*dm_ary[2]*dm_ary[1]*dm_ary[0] + c.dm_ary[2]*dm_ary[1]*dm_ary[0] + c.dm_ary[1]*dm_ary[0] + c.dm_ary[0]; }
    /// stride offset array
    this_type strides(void) const { return this_type(1, dm_ary[0], dm_ary[0]*dm_ary[1], dm_ary[0]*dm_ary[1]*dm_ary[2]); }
    /// returns [0,1,2,3]
    static this_type steps(void) { return this_type(0,1,2,3); }
    /// returns the same, but one less
    nindex<3> shrink(void) const { return nindex<3>(dm_ary[0], dm_ary[1], dm_ary[2]); } // gotta return somethin'

    /// comparison
    bool operator < (const this_type &rhs) const { return dm_ary[0]<rhs.dm_ary[0] && dm_ary[1]<rhs.dm_ary[1] && dm_ary[2]<rhs.dm_ary[2] && dm_ary[3]<rhs.dm_ary[3]; }
    /// equality
    bool operator == (const this_type &rhs) const { return dm_ary[0]==rhs.dm_ary[0] && dm_ary[1]==rhs.dm_ary[1] && dm_ary[2]==rhs.dm_ary[2] && dm_ary[3]==rhs.dm_ary[3]; }
    /// mult
    size_t operator *(const this_type &rhs) const { return dm_ary[0]*rhs.dm_ary[0] + dm_ary[1]*rhs.dm_ary[1] + dm_ary[2]*rhs.dm_ary[2] + dm_ary[3]*rhs.dm_ary[3]; }
    /// assignment
    const this_type & operator = (const this_type &rhs) { dm_ary[0] = rhs.dm_ary[0]; dm_ary[1] = rhs.dm_ary[1]; dm_ary[2] = rhs.dm_ary[2]; dm_ary[3] = rhs.dm_ary[3]; return *this; }

    /// ref access
    size_t & x(void) { return dm_ary[0]; }
    /// ref access
    size_t x(void) const { return dm_ary[0]; }
    /// ref access
    size_t & y(void) { return dm_ary[1]; }
    /// ref access
    size_t y(void) const { return dm_ary[1]; }
    /// ref access
    size_t & z(void) { return dm_ary[2]; }
    /// ref access
    size_t z(void) const { return dm_ary[2]; }
    /// ref access
    size_t & t(void) { return dm_ary[3]; }
    /// ref access
    size_t t(void) const { return dm_ary[3]; }
};

//
//
// nindex stuff
//
//

template <int DIM> scopira::basekit::nindex<DIM>::nindex(size_t val)
{
  for (typename this_type::iterator ii = scopira::tool::fixed_array<size_t,DIM>::begin(); ii != scopira::tool::fixed_array<size_t,DIM>::end(); ++ii)
    *ii = val;
}

template <int DIM>
bool scopira::basekit::nindex<DIM>::operator < (const this_type &rhs) const
{
  for (size_t i=0; i<scopira::tool::fixed_array<size_t,DIM>::size_c; ++i)
    if (scopira::tool::fixed_array<size_t,DIM>::get(i) >= rhs.get(i))
      return false;
  return true;
}

template <int DIM>
bool scopira::basekit::nindex<DIM>::operator == (const this_type &rhs) const
{
  for (size_t i=0; i<scopira::tool::fixed_array<size_t,DIM>::size_c; ++i)
    if (scopira::tool::fixed_array<size_t,DIM>::get(i) != rhs.get(i))
      return false;
  return true;
}

template <int DIM>
size_t scopira::basekit::nindex<DIM>::operator * (const this_type &rhs) const
{
  size_t ret = 0;
  for (size_t i=0; i<scopira::tool::fixed_array<size_t,DIM>::size_c; ++i)
    ret += (*this)[i] * rhs[i];
  return ret;
}

template <int DIM>
size_t scopira::basekit::nindex<DIM>::product(void) const
{
  size_t ret = 1;
  for (typename this_type::const_iterator ii=scopira::tool::fixed_array<size_t,DIM>::begin(); ii != scopira::tool::fixed_array<size_t,DIM>::end(); ++ii)
    ret *= *ii;
  return ret;
}

template <int DIM>
size_t scopira::basekit::nindex<DIM>::offset(const this_type &c) const
{
  size_t ret = 0, val = 1, i;
  for (i=0; i<scopira::tool::fixed_array<size_t,DIM>::size_c; ++i) {
    ret += c[i] * val;
    val *= scopira::tool::fixed_array<size_t,DIM>::get(i);
  }
  return ret;
}

template <int DIM>
scopira::basekit::nindex<DIM> scopira::basekit::nindex<DIM>::strides(void) const
{
  nindex<DIM> ret;

  ret[0] = 1;
  for (size_t i=1; i<scopira::tool::fixed_array<size_t,DIM>::size_c; ++i)
    ret[i] = ret[i-1] * (*this)[i];

  return ret;
}

template <int DIM>
scopira::basekit::nindex<DIM> scopira::basekit::nindex<DIM>::steps(void)
{
  nindex<DIM> ret;

  for (size_t i=0; i<scopira::tool::fixed_array<size_t,DIM>::size_c; ++i)
    ret[i] = i;

  return ret;
}

template <int DIM>
scopira::basekit::nindex<DIM-1> scopira::basekit::nindex<DIM>::shrink(void) const
{
  nindex<DIM-1> ret;

  for (size_t i=0; i<scopira::tool::fixed_array<size_t,DIM>::size_c-1; ++i)
    ret[i] = scopira::tool::fixed_array<size_t,DIM>::dm_ary[i];
  return ret;
}

// NARRAY

/**
 * The is the core N-dimension numerical array class.
 * 
 * It a templated class that allows you to build N-dimensional arrays
 * of any type.
 *
 * See @ref scopirabasekitnarray for a introduction.
 *
 * Arrays contains the elements in a C-like fasion. You can use the access methods
 * to access the elements, STL style iteration or the c_array() function to get
 * at the data.
 *
 * The nslice class allow you to create subset views within narrays.
 * The nindex class is a tiny array class that lets you do N-dimensional coordinate referencing.
 *
 * @see @ref scopirabasekitnarray
 * @see scopira::basekit::nslice
 * @see scopira::basekit::nindex
 * @author Aleksander Demko
 */
template <class T, int DIM> class scopira::basekit::narray
{
  private:
    typedef narray<T, DIM> this_type;
  public:
    /// the type of the elements in the narray
    typedef T data_type;
    /// the index type that this narray likes (usually an nindex type)
    typedef nindex<DIM> index_type;
    /// the interator type
    typedef T* iterator;
    /// the const_interator type
    typedef const T* const_iterator;

  private:
    T* dm_ary;              /// the actual array, newed or direct
    size_t dm_len;          /// the length of the above
    index_type dm_size;     /// dimensionality (or length in all dimensions)
    narray_delete_i * dm_direct;         /// in direct mode? if non null, this will be the handler

  public:
    /// default
    narray(void);
    /// copy ctor
    narray(const this_type &src);
    /// sizing
    explicit narray(const index_type &sz);
    /// sizing
    explicit narray(size_t width, size_t height);
    /// dtor
    ~narray(void) { resize(nindex<DIM>(0)); }

    /**
     * Implements serilization via trait classes.
     * Turbo charged with binary throttling!
     * Non virtual, on purpose. Uses a slightly different flow type, on purpose.
     * @author Aleksander Demko
     */
    bool load(scopira::tool::itflow_i &in);
    /**
     * Implements serilization via trait classes.
     * Turbo charged with binary throttling!
     * Non virtual, on purpose. Uses a slightly different flow type, on purpose.
     * @author Aleksander Demko
     */
    void save(scopira::tool::otflow_i &out) const;

    /// raw c-array style access
    const T * c_array(void) const { return dm_ary; }
    /// raw c-array style access
    T * c_array(void) { return dm_ary; }

    /// begin-stl like iterator
    iterator begin(void) { return c_array(); }
    /// end-stl like iterator
    iterator end(void) { return c_array()+size(); }
    /// begin-stl like iterator
    const_iterator begin(void) const { return c_array(); }
    /// end-stl like iterator
    const_iterator end(void) const { return c_array()+size(); }

    /// empty?
    bool empty(void) const { return dm_len == 0; }
    /// gets the size (1D)
    size_t size(void) const { return dm_len; }
    /// width
    size_t width(void) const { return dm_size[0]; }
    /// height
    size_t height(void) const { return dm_size[1]; }
    /// depth
    size_t depth(void) const { return dm_size[2]; }
    /// gets the size of this array
    const index_type & dimen(void) const { return dm_size; }

    /**
     * Resize the vector to the new length.
     *
     * This destroys the previous contents.
     *
     * @param len the length of the vector (x dimension)
     * @author Aleksander Demko
     */ 
    void resize(size_t len) { resize(nindex<1>(len)); }
    /**
     * Resize the matrix to the new dimensions.
     *
     * This destroys the previous contents.
     *
     * @param neww the new width (x dimension)
     * @param newh the new width (y dimension)
     * @author Aleksander Demko
     */
    void resize(size_t neww, size_t newh) { resize(nindex<2>(neww, newh)); }
    /**
     * Resize the cube (3 dimensional array) to the new dimensions.
     *
     * This destroys the previous contents.
     *
     * @param neww the new width (x dimension)
     * @param newh the new width (y dimension)
     * @param newd the new depth (z dimension)
     * @author Aleksander Demko
     */
    void resize(size_t neww, size_t newh, size_t newd) { resize(nindex<3>(neww, newh, newd)); }
    /**
     * Generic resize function that takes a index_type.
     *
     * This destroys the previous contents.
     *
     * @author Aleksander Demko
     */ 
    void resize(const index_type &news);

    /**
     * Sets this narray to be a "direct" array. That is, the array will
     * utilize an externally managed C array.
     *
     * The current size of the narry must be empty, or another direct array.
     * An empty new direct array will revert this narray back to normal memory managment.
     * 
     * In direct mode, only resize(0) and resize(current_size) will be accepted -
     * all others trigger asserts.
     *
     * @author Aleksander Demko
     */
    void resize_direct(index_type sz, T *direct_ary, scopira::basekit::narray_delete_i *delfunc = null_narray_delete);

    /// gets the direct status/handler. non-null for direct mode
    scopira::basekit::narray_delete_i * get_direct(void) const { return dm_direct; }

    //
    // Slicer Stuff (non-const)
    //

    /// main slicer
    template <int SIM>
      nslice<T,SIM> slicer(index_type base, nindex<SIM> dimen, nindex<SIM> direction);
    /// default directions
    template <int SIM>
      nslice<T,SIM> slicer(index_type base, nindex<SIM> dimen);
    /// 1D specialized slicer, just to be nice
    nslice<T,1> slicer(index_type base, size_t len, size_t direction = x_axis_c);

    /**
     * This returns a slice vector that goes across the diagonal.
     *
     * This only works on matrix narrays (for now).
     *
     * @author Aleksander Demko
     */ 
    nslice<T,1> diagonal_slice(void);

    // slice stuff

    /// all slice
    nslice<T,DIM> all_slice(void);
    /// number of rows
    size_t size_rows(void) const { return dm_size[DIM-1]; }
    /// get a particular row
    nslice<T,DIM-1> row_slice(size_t r);

    // specific slices

    // VECTOR

    /// vector to vector
    nslice<T,1> xslice(size_t basex, size_t len)
      { return slicer(nindex<1>(basex), nindex<1>(len), nindex<1>(x_axis_c)); }

    // MATRIX

    /// vector slice
    nslice<T,1> xslice(size_t basex, size_t basey, size_t len)
      { return slicer(nindex<2>(basex, basey), nindex<1>(len), nindex<1>(x_axis_c)); }
    /// vector slice
    nslice<T,1> yslice(size_t basex, size_t basey, size_t len)
      { return slicer(nindex<2>(basex, basey), nindex<1>(len), nindex<1>(y_axis_c)); }
    /// matrix slice
    nslice<T,2> xyslice(size_t basex, size_t basey, size_t width, size_t height)
      { return slicer(nindex<2>(basex, basey), nindex<2>(width, height)); }

    // N DIMENSIONAL

    /// vector slice
    nslice<T,1> xslice(index_type base, size_t len)
      { return slicer(base, nindex<1>(len), nindex<1>(x_axis_c)); }
    /// vector slice
    nslice<T,1> yslice(index_type base, size_t len)
      { return slicer(base, nindex<1>(len), nindex<1>(y_axis_c)); }
    /// vector slice
    nslice<T,1> zslice(index_type base, size_t len)
      { return slicer(base, nindex<1>(len), nindex<1>(z_axis_c)); }
    /// vector slice
    nslice<T,1> tslice(index_type base, size_t len)
      { return slicer(base, nindex<1>(len), nindex<1>(t_axis_c)); }

    /// matrix slice
    nslice<T,2> xyslice(index_type base, size_t width, size_t height)
      { return slicer(base, nindex<2>(width, height)); }

    //
    // Slicer Stuff (const)
    //

    /// main slicer
    template <int SIM>
      const_nslice<T,SIM> slicer(index_type base, nindex<SIM> dimen,
          nindex<SIM> direction) const;
    /// default directions
    template <int SIM>
      const_nslice<T,SIM> slicer(index_type base, nindex<SIM> dimen) const;
    /// 1D specialized slicer, just to be nice
    const_nslice<T,1> slicer(index_type base, size_t len, size_t direction = x_axis_c) const;

    /// diagnol
    const_nslice<T,1> diagonal_slice(void) const;

    // slice stuff

    /// all slice
    const_nslice<T,DIM> all_slice(void) const;
    /// number of rows
    //size_t size_rows(void) const { return dm_size[DIM-1]; }
    /// get a particular row
    const_nslice<T,DIM-1> row_slice(size_t r) const;

    // specific slices

    // VECTOR

    /// vector to vector
    const_nslice<T,1> xslice(size_t basex, size_t len) const
      { return slicer(nindex<1>(basex), nindex<1>(len), nindex<1>(x_axis_c)); }

    // MATRIX

    /// vector slice
    const_nslice<T,1> xslice(size_t basex, size_t basey, size_t len) const
      { return slicer(nindex<2>(basex, basey), nindex<1>(len), nindex<1>(x_axis_c)); }
    /// vector slice
    const_nslice<T,1> yslice(size_t basex, size_t basey, size_t len) const
      { return slicer(nindex<2>(basex, basey), nindex<1>(len), nindex<1>(y_axis_c)); }
    /// matrix slice
    const_nslice<T,2> xyslice(size_t basex, size_t basey, size_t width, size_t height) const
      { return slicer(nindex<2>(basex, basey), nindex<2>(width, height)); }

    // N DIMENSIONAL

    /// vector slice
    const_nslice<T,1> xslice(index_type base, size_t len) const
      { return slicer(base, nindex<1>(len), nindex<1>(x_axis_c)); }
    /// vector slice
    const_nslice<T,1> yslice(index_type base, size_t len) const
      { return slicer(base, nindex<1>(len), nindex<1>(y_axis_c)); }
    /// vector slice
    const_nslice<T,1> zslice(index_type base, size_t len) const
      { return slicer(base, nindex<1>(len), nindex<1>(z_axis_c)); }
    /// vector slice
    const_nslice<T,1> tslice(index_type base, size_t len) const
      { return slicer(base, nindex<1>(len), nindex<1>(t_axis_c)); }

    /// matrix slice
    const_nslice<T,2> xyslice(index_type base, size_t width, size_t height) const
      { return slicer(base, nindex<2>(width, height)); }

    //
    // end of slice stuff
    //

    /// set all to 0
    void clear(void) { set_all(T()); }
    /// set all the elements
    void set_all(T v);
    /// deep copy
    void copy(const this_type &at);
    /// deep copy
    void copy(const nslice<T,DIM> &at);
    /// deep copy
    void copy(const const_nslice<T,DIM> &at);
    /// this turns out to be very handy
    void operator=(const this_type &at) { copy(at); }

    // *** nindex access ***

    /// [] ref
    T operator()(index_type c) const {
      assert("[narray element access out of bounds]" && c<dm_size );
      return dm_ary[dm_size.offset(c)];
    }
    /// [] ref
    T& operator()(index_type c) {
      assert("[narray element access out of bounds]" && c<dm_size );
      return dm_ary[dm_size.offset(c)];
    }
    /// setter
    void set(index_type c, T v) {
      assert("[narray element access out of bounds]" && c<dm_size );
      dm_ary[dm_size.offset(c)] = v;
    }
    /// getter
    T get(index_type c) const {
      assert("[narray element access out of bounds]" && c<dm_size );
      return dm_ary[dm_size.offset(c)];
    }

    // *** 1D vector like access ***

    /// Nice, referece-based element access
    const T operator[](size_t idx) const {
      assert("[narray element access out of bounds]" && (idx < dm_len) );
      return dm_ary[idx];
    }
    /// Nice, referece-based element access
    T& operator[](size_t idx) {
      assert("[narray element access out of bounds]" && (idx < dm_len) );
      return dm_ary[idx];
    }
    /// Sets the value of an element
    void set(size_t idx, T v) {
      assert("[narray element access out of bounds]" && (idx < dm_len) );
      dm_ary[idx] = v;
    }
    /// Gets the value of an element
    T get(size_t idx) const {
      assert("[narray element access out of bounds]" && (idx < dm_len) );
      return dm_ary[idx];
    }

    // *** 2D matrix like access ***

    /**
     * 2-dimensional (matrix) array access operator.
     * This is only valid on two dimentional arrays.
     *
     * @param x the x value of the element to reference
     * @param y the y value of the element to reference
     * @return a reference to the specified value
     * @author Aleksander Demko
     */
    const T operator()(size_t x, size_t y) const {
      assert("[narray element access out of bounds]" && (x<width()) && (y<height()) );
      return dm_ary[x+y*width()];
    }
    /**
     * 2-dimensional (matrix) array access operator.
     * This is only valid on two dimentional arrays.
     *
     * @param x the x value of the element to reference
     * @param y the y value of the element to reference
     * @return a reference to the specified value
     * @author Aleksander Demko
     */
    T& operator()(size_t x, size_t y) {
      assert("[narray element access out of bounds]" && (x<width()) && (y<height()) );
      return dm_ary[x+y*width()];
    }
    /**
     * Sets an individual element out of a matrix.
     * This is only valid on two dimentional arrays.
     *
     * @param x the x value of the element to reference
     * @param y the y value of the element to reference
     * @param v the new value
     * @author Aleksander Demko
     */
    void set(size_t x, size_t y, T v) {
      assert("[narray element access out of bounds]" && (x<width()) && (y<height()) );
      dm_ary[x+y*width()] = v;
    }
    /**
     * Gets an individual element out of a matrix.
     * This is only valid on two dimentional arrays.
     *
     * @param x the x value of the element to reference
     * @param y the y value of the element to reference
     * @return the value of the specified element
     * @author Aleksander Demko
     */
    T get(size_t x, size_t y) const {
      assert("[narray element access out of bounds]" && (x<width()) && (y<height()));
      return dm_ary[x+y*width()];
    }

    // 3D
    
    /**
     * 3-dimensional (cube) array access operator.
     * This is only valid on three dimentional arrays.
     *
     * @param x the x value of the element to reference
     * @param y the y value of the element to reference
     * @param z the z value of the element to reference
     * @return a reference to the specified value
     * @author Aleksander Demko
     */
    T& operator()(size_t x, size_t y, size_t z) {
      assert("[narray element access out of bounds]" && (x<width()) && (y<height()) && (z<depth()));
      return dm_ary[x+y*width()+z*width()*height()];
    }
    /**
     * Sets an individual element out of a cube.
     * This is only valid on three dimentional arrays.
     *
     * @param x the x value of the element to reference
     * @param y the y value of the element to reference
     * @param z the z value of the element to reference
     * @param v the new value
     * @author Aleksander Demko
     */
    void set(size_t x, size_t y, size_t z, T v) {
      assert("[narray element access out of bounds]" && (x<width()) && (y<height()) && (z<depth()));
      dm_ary[x+y*width()+z*width()*height()] = v;
    }
    /**
     * Gets an individual element out of a cube.
     * This is only valid on three dimentional arrays.
     *
     * @param x the x value of the element to reference
     * @param y the y value of the element to reference
     * @param z the z value of the element to reference
     * @return the value of the specified element
     * @author Aleksander Demko
     */
    T get(size_t x, size_t y, size_t z) const {
      assert("[narray element access out of bounds]" && (x<width()) && (y<height()) && (z<depth()));
      return dm_ary[x+y*width()+z*width()*height()];
    }
};

//
// narray<T>
//

template <class T, int DIM>
scopira::basekit::narray<T,DIM>::narray(void)
  : dm_ary(0), dm_len(0), dm_direct(0)
{
}

template <class T, int DIM>
scopira::basekit::narray<T,DIM>::narray(const this_type &src)
  : dm_ary(0), dm_len(0), dm_direct(0)
{
  copy( src );
}

template <class T, int DIM>
scopira::basekit::narray<T,DIM>::narray(const index_type &sz)
  : dm_ary(0), dm_len(0), dm_direct(0)
{
  resize(sz);
}

template <class T, int DIM>
scopira::basekit::narray<T,DIM>::narray(size_t width, size_t height)
  : dm_ary(0), dm_len(0), dm_direct(0)
{
  resize(nindex<2>(width, height));
}


template <class T, int DIM>
bool scopira::basekit::narray<T,DIM>::load(scopira::tool::itflow_i &in)
{
  nindex<DIM> newsize;
  size_t j;
  int v;
  bool newver;

  if (!in.read_int(v))   // read TAG, possibly
    return false;
  if (v == -10 || v == -11) {
    newver = v == -11;
    // read new style
    if (!in.read_int(v))
      return false;
    assert(v == DIM);
    if (newver)
      for (j=0; j<DIM; ++j) {
        if (!in.read_size_t(newsize[j]))
          return false;
      }
    else
      for (j=0; j<DIM; ++j) {
        if (!in.read_int(v))
          return false;
        newsize[j] = v;
      }

    resize(newsize);

    if (!empty()) {
      if (!in.read_array(dm_ary, size()))
        return false;
#ifdef PLATFORM_BYTESWAP
      // decode this thing
      scopira::tool::byte_swap_all(dm_ary, dm_ary + dm_len);
#endif
    }
  } else {
    // read OLD STYLE, soon to be deprecated
    newsize[0] = v;
    iterator ii, endii;

    assert(v>=0);
    for (j=1; j<DIM; ++j) {
      if (!in.read_int(v))
        return false;
      assert(v>=0);
      newsize[j] = v;
    }

    resize(newsize);

    endii = end();
    for (ii=begin(); ii != endii; ++ii)
      if (!scopira::tool::flowtraits_g<data_type>::load(in, *ii))
        return false;
  }
  return true;
}

template <class T, int DIM>
void scopira::basekit::narray<T,DIM>::save(scopira::tool::otflow_i &out) const
{
  // this ONLY writes in NEW STYLE
  size_t j;

  out.write_int(-11);   // my TAG (-10 == int sizes... -11 size_t sizes)
  out.write_int(DIM);    // redudant, but whatever
  for (j=0; j<DIM; ++j)
    out.write_size_t(dm_size[j]);
  // write out payload
  if (!empty()) {
#ifdef PLATFORM_BYTESWAP
    T *head, *tail;
    head = const_cast<T*>(dm_ary);   // ugly, but a must
    tail = const_cast<T*>(dm_ary + dm_len);
    scopira::tool::byte_swap_all(head, tail);
#endif
    out.write_array(dm_ary, size());
#ifdef PLATFORM_BYTESWAP
    // undo the swappage
    // I'd rather do this one extra time, then use a temporary (and thus twice the memory)
    scopira::tool::byte_swap_all(head, tail);
#endif
  }
}

template <class T, int DIM>
  void scopira::basekit::narray<T,DIM>::resize(const index_type &news)
{
  if (news == dm_size)
    return;

  if (dm_direct) {
    assert("[resize attempted on DIRECT mode narray]" && news.product() == 0);
    // call the handler to nuke this object, since we're in destruction mode
    dm_direct->narray_delete(dm_ary, dm_len);
    dm_direct = 0;
    dm_size = news;
    dm_ary = 0;
    dm_len = 0;
    return;
  }

  dm_size = news;

  if (dm_len > 0)
    delete []dm_ary;
  dm_len = dm_size.product();
  if (dm_len == 0)
    dm_ary = 0;
  else
    dm_ary = new T[dm_len];
}

template <class T, int DIM>
  void scopira::basekit::narray<T,DIM>::resize_direct(index_type sz, T *direct_ary, scopira::basekit::narray_delete_i *delfunc)
{
  assert("[resize_direct may only be called on empty narrays]" && (dm_direct || empty()));

  if (dm_direct) {
    // nuke the previous version
    dm_direct->narray_delete(dm_ary, dm_len);
  }

  dm_size = sz;
  dm_len = dm_size.product();
  dm_direct = dm_len != 0 ? delfunc : 0;
  
  if (dm_direct) {
    assert(direct_ary);
    dm_ary = direct_ary;
    if (dm_direct == normal_narray_delete)    // special case
      dm_direct = 0;
  } else {
    // clearing operation (it doesnt matter what direct_ary is in this case)
    dm_ary = 0;
  }
}

template <class T, int DIM>
  template <int SIM>
scopira::basekit::nslice<T,SIM> scopira::basekit::narray<T,DIM>::slicer(
    index_type base, nindex<SIM> dimen, nindex<SIM> direction)
{
  nindex<DIM> size_strides = dm_size.strides();
  nindex<SIM> strides;

  for (size_t i=0; i<strides.size_c; ++i)
    strides[i] = size_strides[direction[i]];

  return nslice<T,SIM>(dm_ary, dm_size.offset(base), dimen, strides);
}

template <class T, int DIM>
  template <int SIM>
scopira::basekit::nslice<T,SIM> scopira::basekit::narray<T,DIM>::slicer(
    index_type base, nindex<SIM> dimen)
{
  return slicer(base, dimen, nindex<SIM>::steps());
}

template <class T, int DIM>
scopira::basekit::nslice<T,1> scopira::basekit::narray<T,DIM>::slicer(index_type base, size_t len,
    size_t direction)
{
  return slicer(base, nindex<1>(len), nindex<1>(direction));
}

template <class T, int DIM>
scopira::basekit::nslice<T,1> scopira::basekit::narray<T,DIM>::diagonal_slice(void)
{
  assert(DIM == 2 && "[called diagonal_slice() on a non-matrix]\n");
  assert(dm_size[0] == dm_size[1] && "[diagonal_slice() matrix must be square]\n");
  return nslice<T,1>(dm_ary, 0, dm_size[0], dm_size[0]+1);
}

template <class T, int DIM>
scopira::basekit::nslice<T,DIM> scopira::basekit::narray<T,DIM>::all_slice(void)
{
  return slicer(index_type(0), dm_size, nindex<DIM>::steps());
}

template <class T, int DIM>
scopira::basekit::nslice<T,DIM-1> scopira::basekit::narray<T,DIM>::row_slice(size_t r)
{
  index_type base(0);
  base[DIM-1] = r;
  return slicer(base, dm_size.shrink(), nindex<DIM-1>::steps());
}

template <class T, int DIM>
  template <int SIM>
scopira::basekit::const_nslice<T,SIM> scopira::basekit::narray<T,DIM>::slicer(
    index_type base, nindex<SIM> dimen, nindex<SIM> direction) const
{
  nindex<DIM> size_strides = dm_size.strides();
  nindex<SIM> strides;

  for (size_t i=0; i<strides.size_c; ++i)
    strides[i] = size_strides[direction[i]];

  return const_nslice<T,SIM>(dm_ary, dm_size.offset(base), dimen, strides);
}

template <class T, int DIM>
  template <int SIM>
scopira::basekit::const_nslice<T,SIM> scopira::basekit::narray<T,DIM>::slicer(
    index_type base, nindex<SIM> dimen) const
{
  return slicer(base, dimen, nindex<SIM>::steps());
}

template <class T, int DIM>
scopira::basekit::const_nslice<T,1> scopira::basekit::narray<T,DIM>::slicer(index_type base, size_t len,
    size_t direction) const
{
  return slicer(base, nindex<1>(len), nindex<1>(direction));
}

template <class T, int DIM>
scopira::basekit::const_nslice<T,1> scopira::basekit::narray<T,DIM>::diagonal_slice(void) const
{
  assert(DIM == 2 && "[called diagonal_slice() on a non-matrix]\n");
  assert(dm_size[0] == dm_size[1] && "[diagonal_slice() matrix must be square]\n");
  return const_nslice<T,1>(dm_ary, 0, dm_size[0], dm_size[0]+1);
}

template <class T, int DIM>
scopira::basekit::const_nslice<T,DIM> scopira::basekit::narray<T,DIM>::all_slice(void) const
{
  return slicer(index_type(0), dm_size, nindex<DIM>::steps());
}

template <class T, int DIM>
scopira::basekit::const_nslice<T,DIM-1> scopira::basekit::narray<T,DIM>::row_slice(size_t r) const
{
  index_type base(0);
  base[DIM-1] = r;
  return slicer(base, dm_size.shrink(), nindex<DIM-1>::steps());
}

template <class T, int DIM>
void scopira::basekit::narray<T,DIM>::set_all(T v)
{
  typedef typename this_type::iterator I;
  I ii, endii;

  endii = end();
  for (ii=begin(); ii != endii; ++ii)
    *ii = v;
}

template <class T, int DIM>
void scopira::basekit::narray<T,DIM>::copy(const this_type &at)
{
  typedef typename this_type::iterator I;
  typedef typename this_type::const_iterator J;

  resize(at.dimen());

  I ii, endii;
  J kk;

  endii = end();
  kk = at.begin();

  for (ii=begin(); ii != endii; ++ii, ++kk)
    *ii = *kk;
}

template <class T, int DIM>
void scopira::basekit::narray<T,DIM>::copy(const nslice<T,DIM> &at)
{
  typedef typename this_type::iterator I;
  typedef typename nslice<T,DIM>::const_iterator J;

  resize(at.dimen());

  I ii, endii;
  J kk;

  endii = end();
  kk = at.begin();

  for (ii=begin(); ii != endii; ++ii, ++kk)
    *ii = *kk;
}

template <class T, int DIM>
void scopira::basekit::narray<T,DIM>::copy(const const_nslice<T,DIM> &at)
{
  typedef typename this_type::iterator I;
  typedef typename const_nslice<T,DIM>::const_iterator J;

  resize(at.dimen());

  I ii, endii;
  J kk;

  endii = end();
  kk = at.begin();

  for (ii=begin(); ii != endii; ++ii, ++kk)
    *ii = *kk;
}

// NSLICE

/**
 * The nslice class is a lightweight array-like class that lets you
 * view a subset of an narray. You can thing of them as a pointer
 * to the contents of a narray.
 *
 * Some other notes:
 *  - nslice does not itself contain data, it refers to the data in the
 *  host narray
 *  - Do not destroy (or resize) the host narray. If you do so, you
 *  must aquire new nslice instances from the narray.
 *  - nslice is fully copyable and ease to pass around
 *
 * See @ref scopirabasekitnarray for a introduction.
 *
 * @see @ref scopirabasekitnarray
 * @see scopira::basekit::nslice
 * @see scopira::basekit::nindex
 * @author Aleksander Demko
 */
template <class T, int DIM> class scopira::basekit::nslice
{
  private:
    typedef nslice<T, DIM> this_type;
    friend class scopira::basekit::const_nslice<T,DIM>;
  public:
    typedef T data_type;
    typedef nindex<DIM> index_type;
    typedef niterator<T, DIM> iterator;
    typedef niterator<T, DIM> const_iterator;
  private:
    T* dm_var;
    size_t dm_prime, dm_end_prime;
    // this is the size of each dimension
    index_type dm_size;
    // this is the striding factor
    // for each index, it contains the number of elements per-unit in that index
    // for example, dm_stride[0] is the number of elements per x (often 1)
    // for example, dm_stride[1] is the number of elements per y (often, this would be the width)
    // for example, dm_stride[2] is the number of elements per z (often, width()*height()
    index_type dm_stride;
  public:
    /// default ctor, makes a null slice
    nslice(void);
    /// (light) copy ctor
    nslice(const this_type &rhs);
    /// does an all_slice on the source narray
    nslice(narray<T,DIM> &rhs);
    /// does an all_slice on the source narray
    nslice(narray<T,DIM> *rhs);
    /// init, ctor
    nslice(T * _var, size_t _prime, index_type _size, index_type _stride);

    /**
     * Implements serilization via trait classes.
     * Wire-compatible with narray!
     * @author Aleksander Demko
     */
    bool load(scopira::tool::itflow_i &in);
    /**
     * Implements serilization via trait classes.
     * Wire-compatible with narray!
     * @author Aleksander Demko
     */
    void save(scopira::tool::otflow_i &out) const;

    /**
     * Gets the contents of the slice as a ray C array.
     *
     * This is only valid if the X stride is 1.
     *
     * @author Aleksander Demko
     */ 
    T * c_array(void) const { assert(dm_stride[0] == 1); return dm_var+dm_prime; }

    /**
     * Returns the start of the nslice, as an STL-style iterator
     *
     * @author Aleksander Demko
     */ 
    niterator<T, DIM> begin(void) const
      { return niterator<T,DIM>( dm_var+dm_prime, false, dm_size, dm_stride); }
    /**
     * Returns the end of the nslice, as an STL-style iterator
     *
     * @author Aleksander Demko
     */ 
    niterator<T, DIM> end(void) const
      { return niterator<T,DIM>( dm_var+dm_end_prime, true, dm_size, dm_stride); }

    /// is the slice "null", ie not pointing to ANYTHING
    bool is_null(void) const { return !dm_var; }
    /// sets thte slice to null
    void set_null(void) { dm_var = 0; }

    /// empty?
    bool empty(void) const { return dm_prime == dm_end_prime; }
    /// gets the size (1D)
    size_t size(void) const { return dm_size.product(); }
    /// width
    size_t width(void) const { return dm_size[0]; }
    /// height
    size_t height(void) const { return dm_size[1]; }
    /// depth
    size_t depth(void) const { return dm_size[2]; }
    /// gets the width
    const index_type & dimen(void) const { return dm_size; }

    /**
     * Resize the vector to the new length.
     *
     * This destroys the previous contents.
     *
     * @param len the length of the vector (x dimension)
     * @author Aleksander Demko
     */ 
    void resize(size_t len) { resize(nindex<1>(len)); }
    /**
     * Resize the matrix to the new dimensions.
     *
     * This destroys the previous contents.
     *
     * @param neww the new width (x dimension)
     * @param newh the new width (y dimension)
     * @author Aleksander Demko
     */
    void resize(size_t neww, size_t newh) { resize(nindex<2>(neww, newh)); }
    /**
     * Resize the cube (3 dimensional array) to the new dimensions.
     *
     * This destroys the previous contents.
     *
     * @param neww the new width (x dimension)
     * @param newh the new width (y dimension)
     * @param newd the new depth (z dimension)
     * @author Aleksander Demko
     */
    void resize(size_t neww, size_t newh, size_t newd) { resize(nindex<3>(neww, newh, newd)); }
    /**
     * Generic resize function that takes a index_type.
     *
     * This destroys the previous contents.
     *
     * @author Aleksander Demko
     */ 
    void resize(const index_type &news);

    /// slicer cire
    /// main slicer
    template <int SIM>
      nslice<T,SIM> slicer(index_type base, nindex<SIM> dimen,
          nindex<SIM> direction) const;
    /// default directions
    template <int SIM>
      nslice<T,SIM> slicer(index_type base, nindex<SIM> dimen) const;
    /// 1D specialized slicer, just to be nice
    nslice<T,1> slicer(index_type base, size_t len, size_t direction = 0) const;

    // slice stuff

    /**
     * This returns a slice vector that goes across the diagonal.
     *
     * This only works on matrix nslices (for now).
     *
     * @author Aleksander Demko
     */ 
    nslice<T,1> diagonal_slice(void);

    /// all slice
    nslice<T,DIM> all_slice(void) const;
    /// number of rows
    size_t size_rows(void) const { return dm_size[DIM-1]; }
    /// get a particular row
    nslice<T,DIM-1> row_slice(size_t r) const;

    // specific slices

    // VECTOR

    /// vector to vector
    nslice<T,1> xslice(size_t basex, size_t len) const
      { return slicer(nindex<1>(basex), nindex<1>(len), nindex<1>(x_axis_c)); }

    // MATRIX

    /// vector slice
    nslice<T,1> xslice(size_t basex, size_t basey, size_t len) const
      { return slicer(nindex<2>(basex, basey), nindex<1>(len), nindex<1>(x_axis_c)); }
    /// vector slice
    nslice<T,1> yslice(size_t basex, size_t basey, size_t len) const
      { return slicer(nindex<2>(basex, basey), nindex<1>(len), nindex<1>(y_axis_c)); }
    /// matrix slice
    nslice<T,2> xyslice(size_t basex, size_t basey, size_t width, size_t height) const
      { return slicer(nindex<2>(basex, basey), nindex<2>(width, height)); }

    // N DIMENSIONAL

    /// vector slice
    nslice<T,1> xslice(index_type base, size_t len) const
      { return slicer(base, nindex<1>(len), nindex<1>(x_axis_c)); }
    /// vector slice
    nslice<T,1> yslice(index_type base, size_t len) const
      { return slicer(base, nindex<1>(len), nindex<1>(y_axis_c)); }
    /// vector slice
    nslice<T,1> zslice(index_type base, size_t len) const
      { return slicer(base, nindex<1>(len), nindex<1>(z_axis_c)); }
    /// vector slice
    nslice<T,1> tslice(index_type base, size_t len) const
      { return slicer(base, nindex<1>(len), nindex<1>(t_axis_c)); }

    /// matrix slice
    nslice<T,2> xyslice(index_type base, size_t width, size_t height) const
      { return slicer(base, nindex<2>(width, height)); }

    /// set all to 0
    void clear(void) { set_all(0); }
    /// set all elements
    void set_all(T v) const;
    /// copy
    void copy(const narray<T,DIM> &at);
    /// deep copy
    void copy(const nslice<T,DIM> &at);
    /// deep copy
    void copy(const const_nslice<T,DIM> &at);

    /// access, by index
    T& operator()(index_type idx) const {
      assert("[nslice element access out of bounds]" && idx < dm_size && dm_var);
      return dm_var[dm_prime + (idx*dm_stride)];
    }

    // *** 1D vector like access ***

    /// Nice, referece-based element access
    T& operator[](size_t idx) const {
      assert("[nslice element access out of bounds]" &&  idx < dm_size[0] && DIM==1);
      return dm_var[dm_prime+idx*dm_stride[0]];
    }
    /// Sets the value of an element
    void set(size_t idx, T v) const {
      assert("[nslice element access out of bounds]" &&  idx < dm_size[0] && DIM==1);
      dm_var[dm_prime+idx*dm_stride[0]] = v;
    }
    /// Gets the value of an element
    T get(size_t idx) const {
      assert("[nslice element access out of bounds]" &&  idx < dm_size[0] && DIM==1);
      return dm_var[dm_prime+idx*dm_stride[0]];
    }

    // *** 2D matrix like access ***

    /**
     * 2-dimensional (matrix) array access operator.
     * This is only valid on two dimentional arrays.
     *
     * @param x the x value of the element to reference
     * @param y the y value of the element to reference
     * @return a reference to the specified value
     * @author Aleksander Demko
     */
    T& operator()(size_t x, size_t y) const {
      assert("[nslice element access out of bounds]" &&  (x<width()) && (y<height()) );
      return dm_var[dm_prime+x*dm_stride[0]+y*dm_stride[1]];
    }
    /**
     * Sets an individual element out of a matrix.
     * This is only valid on two dimentional arrays.
     *
     * @param x the x value of the element to reference
     * @param y the y value of the element to reference
     * @param v the new value
     * @author Aleksander Demko
     */
    void set(size_t x, size_t y, T v) const {
      assert("[nslice element access out of bounds]" &&  (x<width()) && (y<height()) );
      dm_var[dm_prime+x*dm_stride[0]+y*dm_stride[1]] = v;
    }
    /**
     * Gets an individual element out of a matrix.
     * This is only valid on two dimentional arrays.
     *
     * @param x the x value of the element to reference
     * @param y the y value of the element to reference
     * @return the value of the specified element
     * @author Aleksander Demko
     */
    T get(size_t x, size_t y) const {
      assert("[nslice element access out of bounds]" &&  (x<width()) && (y<height()));
      return dm_var[dm_prime+x*dm_stride[0]+y*dm_stride[1]];
    }

    // 3D

    /**
     * 3-dimensional (cube) array access operator.
     * This is only valid on three dimentional arrays.
     *
     * @param x the x value of the element to reference
     * @param y the y value of the element to reference
     * @param z the z value of the element to reference
     * @return a reference to the specified value
     * @author Aleksander Demko
     */
    T& operator()(size_t x, size_t y, size_t z) const {
      assert("[nslice element access out of bounds]" &&  (x<width()) && (y<height()) && (z<depth()));
      return dm_var[dm_prime+x*dm_stride[0]+y*dm_stride[1]+z*dm_stride[2]];
    }
    /**
     * Sets an individual element out of a cube.
     * This is only valid on three dimentional arrays.
     *
     * @param x the x value of the element to reference
     * @param y the y value of the element to reference
     * @param z the z value of the element to reference
     * @param v the new value
     * @author Aleksander Demko
     */
    void set(size_t x, size_t y, size_t z, T v) const {
      assert("[nslice element access out of bounds]" &&  (x<width()) && (y<height()) && (z<depth()));
      dm_var[dm_prime+x*dm_stride[0]+y*dm_stride[1]+z*dm_stride[2]] = v;
    }
    /**
     * Gets an individual element out of a cube.
     * This is only valid on three dimentional arrays.
     *
     * @param x the x value of the element to reference
     * @param y the y value of the element to reference
     * @param z the z value of the element to reference
     * @return the value of the specified element
     * @author Aleksander Demko
     */
    T get(size_t x, size_t y, size_t z) const {
      assert("[nslice element access out of bounds]" &&  (x<width()) && (y<height()) && (z<depth()));
      return dm_var[dm_prime+x*dm_stride[0]+y*dm_stride[1]+z*dm_stride[2]];
    }
};

//
//
// nslice
//
//

template <class T, int DIM>
scopira::basekit::nslice<T,DIM>::nslice(void)
  : dm_var(0)
{
}

template <class T, int DIM>
scopira::basekit::nslice<T,DIM>::nslice(const this_type &rhs)
  : dm_var(rhs.dm_var), dm_prime(rhs.dm_prime), dm_end_prime(rhs.dm_end_prime),
    dm_size(rhs.dm_size), dm_stride(rhs.dm_stride)
{
}

template <class T, int DIM>
scopira::basekit::nslice<T,DIM>::nslice(narray<T,DIM> &rhs)
{
  *this = rhs.all_slice();
}

template <class T, int DIM>
scopira::basekit::nslice<T,DIM>::nslice(narray<T,DIM> *rhs)
{
  *this = rhs->all_slice();
}

template <class T, int DIM>
scopira::basekit::nslice<T,DIM>::nslice(T * _var, size_t _prime, index_type _size, index_type _stride)
  : dm_var(_var), dm_prime(_prime),
    dm_size(_size), dm_stride(_stride)
{
  dm_end_prime = dm_prime + dm_stride[DIM-1]*dm_size[DIM-1];
}

template <class T, int DIM>
bool scopira::basekit::nslice<T,DIM>::load(scopira::tool::itflow_i &in)
{
  // this ONLY reads in NEW STYLE
  int i, j;
  size_t left;

  // read tag
  if (!in.read_int(i) || i != -11)
    return false;

  // read DIM
  if (!in.read_int(i) || i != DIM)
    return false;
  for (i=0; i<DIM; ++i)
    if (!in.read_size_t(left) || left != dm_size[i])
      return false;

  // read in the payload
  scopira::tool::fixed_array<T, 1024*8> buf;
  const_iterator ii=begin(), endii=end();
  T *jj, *endjj;

  left = size();
  while (left>0) {
    if (left>buf.size())
      j = buf.size();
    else
      j = left;
    jj = buf.begin();
    endjj = jj + j;

    if (!in.read_array(jj, j))
      return false;
#ifdef PLATFORM_BYTESWAP
    scopira::tool::byte_swap_all(jj, endjj);
#endif

    for (; jj != endjj; ++jj, ++ii)
      *ii = *jj;

    left -= j;
  }
  return true;
}

template <class T, int DIM>
void scopira::basekit::nslice<T,DIM>::save(scopira::tool::otflow_i &out) const
{
  // this ONLY writes in NEW STYLE
  int j;

  out.write_int(-11);   // my TAG (-10 == int sizes... -11 size_t sizes)
  out.write_int(DIM);    // redudant, but whatever
  for (j=0; j<DIM; ++j)
    out.write_size_t(dm_size[j]);
  if (empty())
    return;

  // write out payload
  scopira::tool::fixed_array<T, 1024*8> buf;
  const_iterator ii=begin(), endii=end();
  T *jj, *endjj;

  while (ii != endii) {
    for (jj=buf.begin(), endjj=buf.end(); jj != endjj && ii != endii; ++jj, ++ii)
      *jj = *ii;
#ifdef PLATFORM_BYTESWAP
    scopira::tool::byte_swap_all(jj, endjj);
#endif
    out.write_array(buf.begin(), (jj-buf.begin()));
  }
}

template <class T, int DIM>
void scopira::basekit::nslice<T,DIM>::resize(const index_type &news)
{
  dm_size = news;
  dm_end_prime = dm_prime + dm_stride[DIM-1]*dm_size[DIM-1];
}

template <class T, int DIM>
  template <int SIM>
scopira::basekit::nslice<T,SIM> scopira::basekit::nslice<T,DIM>::slicer(
    index_type base, nindex<SIM> dimen, nindex<SIM> direction) const
{
  nindex<SIM> strides;

  for (size_t i=0; i<strides.size_c; ++i)
    strides[i] = dm_stride[direction[i]];

  return nslice<T,SIM>(dm_var, dm_prime+(base*dm_stride), dimen, strides);
}

template <class T, int DIM>
  template <int SIM>
scopira::basekit::nslice<T,SIM> scopira::basekit::nslice<T,DIM>::slicer(
    index_type base, nindex<SIM> dimen) const
{
  return slicer(base, dimen, nindex<SIM>::steps());
}

template <class T, int DIM>
scopira::basekit::nslice<T,1> scopira::basekit::nslice<T,DIM>::slicer(index_type base, size_t len,
    size_t direction) const
{
  return slicer(base, nindex<1>(len), nindex<1>(direction));
}

template <class T, int DIM>
scopira::basekit::nslice<T,1> scopira::basekit::nslice<T,DIM>::diagonal_slice(void)
{
  assert(DIM == 2 && "[called diagonal_slice() on a non-matrix]\n");
  assert(dm_size[0] == dm_size[1] && "[diagonal_slice() matrix must be square]\n");
  return nslice<T,1>(dm_var, dm_prime, nindex<1>(dm_size[0]), nindex<1>(dm_stride[1]+1));
}

template <class T, int DIM>
scopira::basekit::nslice<T,DIM> scopira::basekit::nslice<T,DIM>::all_slice(void) const
{
  return slicer(index_type(0), dm_size, nindex<DIM>::steps());
}

template <class T, int DIM>
scopira::basekit::nslice<T,DIM-1> scopira::basekit::nslice<T,DIM>::row_slice(size_t r) const
{
  index_type base(0);
  base[DIM-1] = r;
  return slicer(base, dm_size.shrink(), nindex<DIM-1>::steps());
}

template <class T, int DIM>
void scopira::basekit::nslice<T,DIM>::set_all(T v) const
{
  typedef typename this_type::iterator I;
  I ii, endii;

  endii = end();
  for (ii=begin(); ii != endii; ++ii)
    *ii = v;
}

template <class T, int DIM>
void scopira::basekit::nslice<T,DIM>::copy(const narray<T,DIM> &at)
{
  typedef typename this_type::iterator I;
  typedef typename narray<T,DIM>::const_iterator J;

  resize(at.dimen());

  I ii, endii;
  J kk;

  endii = end();
  kk = at.begin();

  for (ii=begin(); ii != endii; ++ii, ++kk)
    *ii = *kk;
}

template <class T, int DIM>
void scopira::basekit::nslice<T,DIM>::copy(const nslice<T,DIM> &at)
{
  typedef typename this_type::iterator I;
  typedef typename nslice<T,DIM>::iterator J;

  resize(at.dimen());

  I ii, endii;
  J kk;

  endii = end();
  kk = at.begin();

  for (ii=begin(); ii != endii; ++ii, ++kk)
    *ii = *kk;
}

template <class T, int DIM>
void scopira::basekit::nslice<T,DIM>::copy(const const_nslice<T,DIM> &at)
{
  typedef typename this_type::iterator I;
  typedef typename const_nslice<T,DIM>::iterator J;

  resize(at.dimen());

  I ii, endii;
  J kk;

  endii = end();
  kk = at.begin();

  for (ii=begin(); ii != endii; ++ii, ++kk)
    *ii = *kk;
}

// *1niterator********************************************************************

/**
 * Slice iterators.
 * @author Aleksander Demko
 */
template <class T, int DIM>
class scopira::basekit::niterator
{
  public:
    /// the element data type of this iterator
    typedef T data_type;
  private:
    typedef niterator<T, DIM> this_type;
    T *dm_ptr;
    nindex<DIM> dm_cur, dm_size, dm_stride;
  public:
    /// ctor
    niterator(void);
    /// initing ctor
    niterator(T *ptr, bool endptr, nindex<DIM> _size, nindex<DIM> _stride);

    /// Dereference
    T& operator *(void) { return *dm_ptr; }
    /// Comparison
    bool operator ==(const this_type &rhs) const { return dm_cur == rhs.dm_cur; }
    /// Comparison
    bool operator !=(const this_type &rhs) const { return !(dm_cur == rhs.dm_cur); }

    /// Increment
    void operator++(void);
    /// Decrement
    void operator--(void);

    // for random iterators
    /// Random iteration support method
    ptrdiff_t operator-(const this_type & rhs) const {
      assert(DIM == 1);
      return (dm_cur[0] - rhs.dm_cur[0]) / dm_stride[0]; }
    /// Random iteration support method
    this_type  operator+(ptrdiff_t idx) const {
      assert(DIM == 1);
      this_type ret;
      ret.dm_ptr = dm_ptr + idx;
      ret.dm_cur[0] = dm_cur[0] + idx * dm_stride[0];
      ret.dm_size = dm_size;
      ret.dm_stride = dm_stride;
      return ret;
    }
    /// Random iteration support method
    this_type  operator-(ptrdiff_t idx) const {
      assert(DIM == 1);
      this_type ret;
      ret.dm_ptr = dm_ptr - idx;
      ret.dm_cur[0] = dm_cur[0] - idx * dm_stride[0];
      ret.dm_size = dm_size;
      ret.dm_stride = dm_stride;
      return ret;
    }
    /// Random iteration support method
    bool operator<(const this_type  &rhs) const {
      assert(DIM == 1);
      return dm_cur[0] < rhs.dm_cur[0]; }
};

template <class T, int DIM>
scopira::basekit::niterator<T,DIM>::niterator(void)
{
}

template <class T, int DIM>
scopira::basekit::niterator<T,DIM>::niterator(T *ptr, bool endptr, nindex<DIM> _size, nindex<DIM> _stride)
  : dm_ptr(ptr), dm_cur(0), dm_size(_size), dm_stride(_stride)
{
  if (endptr)
    dm_cur[DIM-1] = dm_size[DIM-1];
}

template <class T, int DIM>
void scopira::basekit::niterator<T, DIM>::operator++(void)
{
  dm_ptr += dm_stride[0];
  ++dm_cur[0];
  for (size_t j=0; dm_cur[j] >= dm_size[j] && j+1<DIM; ++j) {
    // carry over the cur digits while doing some stride magic
    dm_ptr += -1 * dm_stride[j]*dm_cur[j] + dm_stride[j + 1];
    dm_cur[j] = 0;    // reset this digit
    ++dm_cur[j+1];    // carry over the next
  }
}

template <class T, int DIM>
void scopira::basekit::niterator<T, DIM>::operator--(void)
{
  for (size_t j=0; j<DIM; ++j) {
    // decrement the current digit
    if (dm_cur[j] > 0) {
      // done... no need to borrow futher
      --dm_cur[j];
      dm_ptr -= dm_stride[j];
      return;
    }
    // dm_cur[j] == 0, so well need to borrow
    // carry over the cur digits while doing some stride magic
    dm_cur[j] = dm_size[j]-1;    // reset this digit
    dm_ptr += dm_stride[j]*dm_cur[j];
  }
}


// CONST_NSLICE

/**
 * Natural partner for narray. A light ("pointer") like
 * pointer into another narray.
 *
 * This version does NOT permit you to modify the source matrix in any way,
 * making it const-safe.
 *
 * @author Aleksander Demko
 */
template <class T, int DIM> class scopira::basekit::const_nslice
{
  private:
    typedef const_nslice<T, DIM> this_type;
  public:
    typedef T data_type;
    typedef nindex<DIM> index_type;
    typedef const_niterator<T, DIM> iterator;
    typedef const_niterator<T, DIM> const_iterator;
  private:
    const T * dm_var;
    size_t dm_prime, dm_end_prime;
    index_type dm_size, dm_stride;
  public:
    /// default ctor, makes a null slice
    const_nslice(void);
    /// copy ctor
    const_nslice(const this_type &rhs);
    /// does an all_slice on the source narray
    const_nslice(const narray<T,DIM> &rhs);
    /// does an all_slice on the source narray
    const_nslice(const narray<T,DIM> *rhs);
    /// init, ctor
    const_nslice(const T * _var, size_t _prime, index_type _size, index_type _stride);
    /**
     * Implicit convert for nslice to const_nslice
     *
     * @author Aleksander Demko
     */ 
    const_nslice(const nslice<T, DIM> &rhs);

    /**
     * Implements serilization via trait classes.
     * Wire-compatible with narray!
     * @author Aleksander Demko
     */
    void save(scopira::tool::otflow_i &out) const;

    /// gets the slice as a raw C array. only valid if the X stride is 1
    const T * c_array(void) const { assert(dm_stride[0] == 1); return dm_var; }

    /**
     * Returns the start of the const_nslice, as an STL-style iterator
     *
     * @author Aleksander Demko
     */ 
    const_niterator<T, DIM> begin(void) const
      { return const_niterator<T,DIM>( dm_var+dm_prime, false, dm_size, dm_stride); }
    /**
     * Returns the end of the const_nslice, as an STL-style iterator
     *
     * @author Aleksander Demko
     */ 
    const_niterator<T, DIM> end(void) const
      { return const_niterator<T,DIM>( dm_var+dm_end_prime, true, dm_size, dm_stride); }

    /// is the slice "null", ie not pointing to ANYTHING
    bool is_null(void) const { return !dm_var; }
    /// sets thte slice to null
    void set_null(void) { dm_var = 0; }

    /// empty?
    bool empty(void) const { return dm_prime == dm_end_prime; }
    /// gets the size (1D)
    size_t size(void) const { assert(DIM==1); return dm_size[0]; }
    /// width
    size_t width(void) const { return dm_size[0]; }
    /// height
    size_t height(void) const { return dm_size[1]; }
    /// depth
    size_t depth(void) const { return dm_size[2]; }
    /// gets the size of this array
    const index_type & dimen(void) const { return dm_size; }

    /**
     * Resize the vector to the new length.
     *
     * This destroys the previous contents.
     *
     * @param len the length of the vector (x dimension)
     * @author Aleksander Demko
     */ 
    void resize(size_t len) { resize(nindex<1>(len)); }
    /**
     * Resize the matrix to the new dimensions.
     *
     * This destroys the previous contents.
     *
     * @param neww the new width (x dimension)
     * @param newh the new width (y dimension)
     * @author Aleksander Demko
     */
    void resize(size_t neww, size_t newh) { resize(nindex<2>(neww, newh)); }
    /**
     * Resize the cube (3 dimensional array) to the new dimensions.
     *
     * This destroys the previous contents.
     *
     * @param neww the new width (x dimension)
     * @param newh the new width (y dimension)
     * @param newd the new depth (z dimension)
     * @author Aleksander Demko
     */
    void resize(size_t neww, size_t newh, size_t newd) { resize(nindex<3>(neww, newh, newd)); }
    /**
     * Generic resize function that takes a index_type.
     *
     * This destroys the previous contents.
     *
     * @author Aleksander Demko
     */ 
    void resize(const index_type &news);

    /// slicer cire
    /// main slicer
    template <int SIM>
      const_nslice<T,SIM> slicer(index_type base, nindex<SIM> dimen,
          nindex<SIM> direction) const;
    /// default directions
    template <int SIM>
      const_nslice<T,SIM> slicer(index_type base, nindex<SIM> dimen) const;
    /// 1D specialized slicer, just to be nice
    const_nslice<T,1> slicer(index_type base, size_t len, size_t direction = 0) const;

    // slice stuff

    /**
     * This returns a slice vector that goes across the diagonal.
     *
     * This only works on matrix nslices (for now).
     *
     * @author Aleksander Demko
     */ 
    const_nslice<T,1> diagonal_slice(void);

    /// all slice
    const_nslice<T,DIM> all_slice(void) const;
    /// number of rows
    size_t size_rows(void) const { return dm_size[DIM-1]; }
    /// get a particular row
    const_nslice<T,DIM-1> row_slice(size_t r) const;

    // specific slices

    // VECTOR

    /// vector to vector
    const_nslice<T,1> xslice(size_t basex, size_t len) const
      { return slicer(nindex<1>(basex), nindex<1>(len), nindex<1>(x_axis_c)); }

    // MATRIX

    /// vector slice
    const_nslice<T,1> xslice(size_t basex, size_t basey, size_t len) const
      { return slicer(nindex<2>(basex, basey), nindex<1>(len), nindex<1>(x_axis_c)); }
    /// vector slice
    const_nslice<T,1> yslice(size_t basex, size_t basey, size_t len) const
      { return slicer(nindex<2>(basex, basey), nindex<1>(len), nindex<1>(y_axis_c)); }
    /// matrix slice
    const_nslice<T,2> xyslice(size_t basex, size_t basey, size_t width, size_t height) const
      { return slicer(nindex<2>(basex, basey), nindex<2>(width, height)); }

    // N DIMENSIONAL

    /// vector slice
    const_nslice<T,1> xslice(index_type base, size_t len) const
      { return slicer(base, nindex<1>(len), nindex<1>(x_axis_c)); }
    /// vector slice
    const_nslice<T,1> yslice(index_type base, size_t len) const
      { return slicer(base, nindex<1>(len), nindex<1>(y_axis_c)); }
    /// vector slice
    const_nslice<T,1> zslice(index_type base, size_t len) const
      { return slicer(base, nindex<1>(len), nindex<1>(z_axis_c)); }
    /// vector slice
    const_nslice<T,1> tslice(index_type base, size_t len) const
      { return slicer(base, nindex<1>(len), nindex<1>(t_axis_c)); }

    /// matrix slice
    const_nslice<T,2> xyslice(index_type base, size_t width, size_t height) const
      { return slicer(base, nindex<2>(width, height)); }

    /// access, by index
    T operator()(index_type idx) const {
      assert("[const_nslice element access out of bounds]" && idx < dm_size && dm_var);
      return dm_var[dm_prime + (idx*dm_stride)];
    }

    // *** 1D vector like access ***

    /// Nice, referece-based element access
    T operator[](size_t idx) const {
      assert("[const_nslice element access out of bounds]" &&  idx < dm_size[0] && DIM==1);
      return dm_var[dm_prime+idx*dm_stride[0]];
    }
    /// Gets the value of an element
    T get(size_t idx) const {
      assert("[const_nslice element access out of bounds]" &&  idx < dm_size[0] && DIM==1);
      return dm_var[dm_prime+idx*dm_stride[0]];
    }

    // *** 2D matrix like access ***

    /**
     * 2-dimensional (matrix) array access operator.
     * This is only valid on two dimentional arrays.
     *
     * @param x the x value of the element to reference
     * @param y the y value of the element to reference
     * @return a reference to the specified value
     * @author Aleksander Demko
     */
    T operator()(size_t x, size_t y) const {
      assert("[const_nslice element access out of bounds]" &&  (x<width()) && (y<height()) );
      return dm_var[dm_prime+x*dm_stride[0]+y*dm_stride[1]];
    }
    /**
     * Gets an individual element out of a matrix.
     * This is only valid on two dimentional arrays.
     *
     * @param x the x value of the element to reference
     * @param y the y value of the element to reference
     * @return the value of the specified element
     * @author Aleksander Demko
     */
    T get(size_t x, size_t y) const {
      assert("[const_nslice element access out of bounds]" &&  (x<width()) && (y<height()) );
      return dm_var[dm_prime+x*dm_stride[0]+y*dm_stride[1]];
    }

    // 3D

    /**
     * 3-dimensional (cube) array access operator.
     * This is only valid on three dimentional arrays.
     *
     * @param x the x value of the element to reference
     * @param y the y value of the element to reference
     * @param z the z value of the element to reference
     * @return a reference to the specified value
     * @author Aleksander Demko
     */
    T& operator()(size_t x, size_t y, size_t z) const {
      assert("[nslice element access out of bounds]" &&  (x<width()) && (y<height()) && (z<depth()));
      return dm_var[dm_prime+x*dm_stride[0]+y*dm_stride[1]+z*dm_stride[2]];
    }
    /**
     * Gets an individual element out of a cube.
     * This is only valid on three dimentional arrays.
     *
     * @param x the x value of the element to reference
     * @param y the y value of the element to reference
     * @param z the z value of the element to reference
     * @return the value of the specified element
     * @author Aleksander Demko
     */
    T get(size_t x, size_t y, size_t z) const {
      assert("[nslice element access out of bounds]" &&  (x<width()) && (y<height()) && (z<depth()));
      return dm_var[dm_prime+x*dm_stride[0]+y*dm_stride[1]+z*dm_stride[2]];
    }
};

//
//
// const_nslice
//
//

template <class T, int DIM>
scopira::basekit::const_nslice<T,DIM>::const_nslice(void)
  : dm_var(0)
{
}

template <class T, int DIM>
scopira::basekit::const_nslice<T,DIM>::const_nslice(const this_type &rhs)
  : dm_var(rhs.dm_var), dm_prime(rhs.dm_prime), dm_end_prime(rhs.dm_end_prime),
    dm_size(rhs.dm_size), dm_stride(rhs.dm_stride)
{
}

template <class T, int DIM>
scopira::basekit::const_nslice<T,1> scopira::basekit::const_nslice<T,DIM>::diagonal_slice(void)
{
  assert(DIM == 2 && "[called diagonal_slice() on a non-matrix]\n");
  assert(dm_size[0] == dm_size[1] && "[diagonal_slice() matrix must be square]\n");
  return const_nslice<T,1>(dm_var, dm_prime, nindex<1>(dm_size[0]), nindex<1>(dm_stride[1]+1));
}

template <class T, int DIM>
scopira::basekit::const_nslice<T,DIM>::const_nslice(const narray<T,DIM> &rhs)
{
  *this = rhs.all_slice();
}

template <class T, int DIM>
scopira::basekit::const_nslice<T,DIM>::const_nslice(const narray<T,DIM> *rhs)
{
  *this = rhs->all_slice();
}

template <class T, int DIM>
scopira::basekit::const_nslice<T,DIM>::const_nslice(const T * _var, size_t _prime, index_type _size, index_type _stride)
  : dm_var(_var), dm_prime(_prime),
    dm_size(_size), dm_stride(_stride)
{
  dm_end_prime = dm_prime + dm_stride[DIM-1]*dm_size[DIM-1];
}

template <class T, int DIM>
scopira::basekit::const_nslice<T,DIM>::const_nslice(const nslice<T, DIM> &rhs)
  : dm_var(rhs.dm_var), dm_prime(rhs.dm_prime), dm_end_prime(rhs.dm_end_prime),
    dm_size(rhs.dm_size), dm_stride(rhs.dm_stride)
{
}

template <class T, int DIM>
void scopira::basekit::const_nslice<T,DIM>::save(scopira::tool::otflow_i &out) const
{
  // this ONLY writes in NEW STYLE
  int j;

  out.write_int(-11);   // my TAG (-10 == int sizes... -11 size_t sizes)
  out.write_int(DIM);    // redudant, but whatever
  for (j=0; j<DIM; ++j)
    out.write_size_t(dm_size[j]);
  if (empty())
    return;

  // write out payload
  scopira::tool::fixed_array<T, 1024*8> buf;
  const_iterator ii=begin(), endii=end();
  T *jj, *endjj;

  while (ii != endii) {
    for (jj=buf.begin(), endjj=buf.end(); jj != endjj && ii != endii; ++jj, ++ii)
      *jj = *ii;
#ifdef PLATFORM_BYTESWAP
    scopira::tool::byte_swap_all(jj, endjj);
#endif
    out.write_array(buf.begin(), (jj-buf.begin()));
  }
}

template <class T, int DIM>
void scopira::basekit::const_nslice<T,DIM>::resize(const index_type &news)
{
  dm_size = news;
  dm_end_prime = dm_prime + dm_stride[DIM-1]*dm_size[DIM-1];
}

template <class T, int DIM>
  template <int SIM>
scopira::basekit::const_nslice<T,SIM> scopira::basekit::const_nslice<T,DIM>::slicer(
    index_type base, nindex<SIM> dimen, nindex<SIM> direction) const
{
  nindex<SIM> strides;

  for (size_t i=0; i<strides.size_c; ++i)
    strides[i] = dm_stride[direction[i]];

  return const_nslice<T,SIM>(dm_var, dm_prime+(base*dm_stride), dimen, strides);
}

template <class T, int DIM>
  template <int SIM>
scopira::basekit::const_nslice<T,SIM> scopira::basekit::const_nslice<T,DIM>::slicer(
    index_type base, nindex<SIM> dimen) const
{
  return slicer(base, dimen, nindex<SIM>::steps());
}

template <class T, int DIM>
scopira::basekit::const_nslice<T,1> scopira::basekit::const_nslice<T,DIM>::slicer(index_type base, size_t len,
    size_t direction) const
{
  return slicer(base, nindex<1>(len), nindex<1>(direction));
}

template <class T, int DIM>
scopira::basekit::const_nslice<T,DIM> scopira::basekit::const_nslice<T,DIM>::all_slice(void) const
{
  return slicer(index_type(0), dm_size, nindex<DIM>::steps());
}

template <class T, int DIM>
scopira::basekit::const_nslice<T,DIM-1> scopira::basekit::const_nslice<T,DIM>::row_slice(size_t r) const
{
  index_type base(0);
  base[DIM-1] = r;
  return slicer(base, dm_size.shrink(), nindex<DIM-1>::steps());
}

// *1const_niterator********************************************************************

/**
 * Slice iterators.
 * @author Aleksander Demko
 */
template <class T, int DIM>
class scopira::basekit::const_niterator
{
  public:
    /// the element type this const_niterator
    typedef T data_type;
  private:
    typedef const_niterator<T, DIM> this_type;
    const T *dm_ptr;
    nindex<DIM> dm_cur, dm_size, dm_stride;
  public:
    /// ctor
    const_niterator(void);
    /// initing ctor
    const_niterator(const T *ptr, bool endptr, nindex<DIM> _size, nindex<DIM> _stride);

    /// Dereference
    const T& operator *(void) { return *dm_ptr; }
    /// Comparison
    bool operator ==(const this_type &rhs) const { return dm_cur == rhs.dm_cur; }
    /// Comparison
    bool operator !=(const this_type &rhs) const { return !(dm_cur == rhs.dm_cur); }

    /// Increment
    void operator++(void);
    /// Decrement
    void operator--(void);

    // for random iterators
    /// Random iteration support method
    ptrdiff_t operator-(const this_type & rhs) const {
      assert(DIM == 1);
      return (dm_cur[0] - rhs.dm_cur[0]) / dm_stride[0]; }
    /// Random iteration support method
    this_type  operator+(ptrdiff_t idx) const {
      assert(DIM == 1);
      this_type ret;
      ret.dm_ptr = dm_ptr + idx;
      ret.dm_cur[0] = dm_cur[0] + idx * dm_stride[0];
      ret.dm_size = dm_size;
      ret.dm_stride = dm_stride;
      return ret;
    }
    /// Random iteration support method
    this_type  operator-(ptrdiff_t idx) const {
      assert(DIM == 1);
      this_type ret;
      ret.dm_ptr = dm_ptr - idx;
      ret.dm_cur[0] = dm_cur[0] - idx * dm_stride[0];
      ret.dm_size = dm_size;
      ret.dm_stride = dm_stride;
      return ret;
    }
    /// Random iteration support method
    bool operator<(const this_type  &rhs) const {
      assert(DIM == 1);
      return dm_cur[0] < rhs.dm_cur[0]; }
};

template <class T, int DIM>
scopira::basekit::const_niterator<T,DIM>::const_niterator(void)
{
}

template <class T, int DIM>
scopira::basekit::const_niterator<T,DIM>::const_niterator(const T *ptr, bool endptr, nindex<DIM> _size, nindex<DIM> _stride)
  : dm_ptr(ptr), dm_cur(0), dm_size(_size), dm_stride(_stride)
{
  if (endptr)
    dm_cur[DIM-1] = dm_size[DIM-1];
}

template <class T, int DIM>
void scopira::basekit::const_niterator<T, DIM>::operator++(void)
{
  dm_ptr += dm_stride[0];
  ++dm_cur[0];
  for (size_t j=0; dm_cur[j] >= dm_size[j] && j+1<DIM; ++j) {
    // carry over the cur digits while doing some stride magic
    dm_ptr += -1*dm_stride[j]*dm_cur[j] + dm_stride[j + 1];
    dm_cur[j] = 0;    // reset this digit
    ++dm_cur[j+1];    // carry over the next
  }
}

template <class T, int DIM>
void scopira::basekit::const_niterator<T, DIM>::operator--(void)
{
  for (size_t j=0; j<DIM; ++j) {
    // decrement the current digit
    if (dm_cur[j] > 0) {
      // done... no need to borrow futher
      --dm_cur[j];
      dm_ptr -= dm_stride[j];
      return;
    }
    // dm_cur[j] == 0, so well need to borrow
    // carry over the cur digits while doing some stride magic
    dm_cur[j] = dm_size[j]-1;    // reset this digit
    dm_ptr += dm_stride[j]*dm_cur[j];
  }
}

// *1narray_o*********************************************************************

/**
 * This is a descendant of narray that also decends from object
 * thereby adding ref counting and registered serialization abilitites.
 *
 * The following variants of this type have been registered for serialization:
 *
 * @author Aleksander Demko
 */ 
template <class T, int DIM> class scopira::basekit::narray_o : public scopira::basekit::narray<T,DIM>,
  public scopira::tool::object
{
  private:
    typedef scopira::basekit::narray_o<T,DIM> this_type;
    typedef scopira::basekit::narray<T,DIM> narray_parent_type;
    typedef scopira::tool::object object_parent_type;
  public:
    // ctors

    /// default
    narray_o(void) { }
    /// copy ctor
    narray_o(const this_type &src)
      : narray_parent_type(src) { }
    /// copy ctor
    narray_o(const narray_parent_type &src)
      : narray_parent_type(src) { }
    /// sizing
    explicit narray_o(const nindex<DIM> &sz)
      : narray_parent_type(sz) { }
    /// sizing
    explicit narray_o(size_t width, size_t height)
      : narray_parent_type(width, height) { }

    // object stuff

    virtual bool load(scopira::tool::iobjflow_i &in) { return narray_parent_type::load(in); }
    virtual void save(scopira::tool::oobjflow_i &out) const { narray_parent_type::save(out); }
};

//
//
// misc funcs
//
//

namespace std {
/**
 * This is a traits specification class for vec_iterator_g to make it
 * random iterator compliant, among other niceties.
 * @author Aleksander Demko
 */
template <typename T> struct iterator_traits<scopira::basekit::niterator<T,1> >
{
  public:
    typedef std::random_access_iterator_tag iterator_category;
    typedef T value_type;
    typedef ptrdiff_t difference_type;
    //make these when we need 'em :)
    //typedef typename _Iterator::pointer           pointer;
    //typedef typename _Iterator::reference         reference;
};
}//namespace std

template <class E>
  inline void scopira::basekit::print_element(scopira::tool::oflow_i &o, E el)
{
  o << ' ' << el;
}

/// internal print drivers. you may need to define more if you have your own types
template <>
  inline void scopira::basekit::print_element<long>(scopira::tool::oflow_i &o, long el)
{
  flow_printf(o, " %8ld", el);
}

/// internal print drivers. you may need to define more if you have your own types
template <>
  inline void scopira::basekit::print_element<unsigned long>(scopira::tool::oflow_i &o, unsigned long el)
{
  flow_printf(o, " %8lu", el);
}

/// internal print drivers. you may need to define more if you have your own types
template <>
  inline void scopira::basekit::print_element<int>(scopira::tool::oflow_i &o, int el)
{
  flow_printf(o, " %8d", el);
}

/// internal print drivers. you may need to define more if you have your own types
template <>
  inline void scopira::basekit::print_element<unsigned int>(scopira::tool::oflow_i &o, unsigned int el)
{
  flow_printf(o, " %8u", el);
}

/// internal print drivers. you may need to define more if you have your own types
template <>
  inline void scopira::basekit::print_element<short>(scopira::tool::oflow_i &o, short el)
{
  flow_printf(o, " %8d", el);
}

/// internal print drivers. you may need to define more if you have your own types
template <>
  inline void scopira::basekit::print_element<char>(scopira::tool::oflow_i &o, char el)
{
  flow_printf(o, " %4d", el);
}

/// internal print drivers. you may need to define more if you have your own types
template <>
  inline void scopira::basekit::print_element<double>(scopira::tool::oflow_i &o, double el)
{
  flow_printf(o, " %8.2f", el);
}

/// internal print drivers. you may need to define more if you have your own types
template <>
  inline void scopira::basekit::print_element<float>(scopira::tool::oflow_i &o, float el)
{
  flow_printf(o, " %8.2f", el);
}

/// internal
template <class C>
  scopira::tool::oflow_i & scopira::basekit::print_vector_slice
  (scopira::tool::oflow_i &o, const const_nslice<C,1> &V)
{
  size_t i, mx;

  mx = V.size();
  o << "Vector, len=" << mx << ":";
  for (i=0; i<mx; i++) {
    if (i % 10 == 0)
      flow_printf(o, "\n %4d:", i);
    print_element(o, V[i]);
  }

  return o << '\n';
}

/// internal
template <class C>
  scopira::tool::oflow_i & scopira::basekit::print_matrix_slice
  (scopira::tool::oflow_i &o, const const_nslice<C,2> &M)
{
  size_t x, y, w, h;

  w = M.width();
  h = M.height();

  flow_printf(o, "Matrix, w=%d h=%d:\n       ", w, h);
  for (x=0; x<w; x++)
    flow_printf(o, " %7d:", x);
  o << '\n';
  for (y=0; y<h; y++) {
    flow_printf(o, " %4d: ", y);
    for (x=0; x<w; x++)
      print_element(o, M(x,y));
    o << '\n';
  }
  return o;
}

/**
 * Printer
 * @author Aleksander Demko
 */
template <class T, int DIM> scopira::tool::oflow_i & operator << (scopira::tool::oflow_i &o,
    const scopira::basekit::const_nslice<T, DIM> &A)
{
  size_t k, kmax;
  for (k=0, kmax=A.size_rows(); k<kmax; ++k) {
    o << "Sub Slice (depth=" << DIM << ") " << k << ":\n";
    o << A.row_slice(k) << '\n';
  }

  return o;
}

/**
 * Printer
 * @author Aleksander Demko
 */
template <class T> inline scopira::tool::oflow_i & operator << (scopira::tool::oflow_i &o,
    const scopira::basekit::const_nslice<T, 1> &A)
{
  return print_vector_slice(o, A);
}

/**
 * Printer
 * @author Aleksander Demko
 */
template <class T> inline scopira::tool::oflow_i & operator << (scopira::tool::oflow_i &o,
    const scopira::basekit::const_nslice<T, 2> &A)
{
  return print_matrix_slice(o, A);
}

/**
 * Printer
 * @author Aleksander Demko
 */
template <class T, int DIM> inline scopira::tool::oflow_i & operator << (scopira::tool::oflow_i &o,
  const scopira::basekit::nslice<T, DIM> &A)
{
  return o << scopira::basekit::const_nslice<T, DIM>(A);
}

/**
 * Printer
 * @author Aleksander Demko
 */
template <class T, int DIM>
inline
scopira::tool::oflow_i & operator << (scopira::tool::oflow_i &o,
    const scopira::basekit::narray<T, DIM> &A)
{
  return o << A.all_slice();
}

/**
 * Printer
 * @author Aleksander Demko
 */
template <class T, int DIM>
inline
scopira::tool::oflow_i & operator << (scopira::tool::oflow_i &o,
    const scopira::basekit::narray_o<T, DIM> &A)
{
  return o << A.all_slice();
}

/**
 * Printer
 * @author Aleksander Demko
 */
template <int DIM>
scopira::tool::oflow_i & operator << (scopira::tool::oflow_i &o,
    const scopira::basekit::nindex<DIM> &d)
{
  o << '(' << d[0];
  for (size_t x=1; x<DIM; ++x)
    o << ',' << d[x];
  return o << ')';
}

/// internal
template <class C>
  std::ostream & scopira::basekit::print_vector_slice(std::ostream &o, const const_nslice<C,1> &V)
{
  size_t i, mx;

  mx = V.size();
  o << "Vector, len=" << mx << ":";
  for (i=0; i<mx; i++) {
    if (i % 5 == 0)
      o << "\n " << std::setw(4) << i << ":";
    o.width(8);
    o << std::setw(14) << V[i];
  }

  return o << '\n';
}

/// internal
template <class C>
  std::ostream & scopira::basekit::print_matrix_slice
  (std::ostream &o, const const_nslice<C,2> &M)
{
  size_t x, y, w, h;

  w = M.width();
  h = M.height();

  o << "Matrix, w=" << w << " h=" << h << ":\n     ";
  for (x=0; x<w; x++)
    o << std::setw(13) << x << ':';
  o << '\n';
  for (y=0; y<h; y++) {
    o << std::setw(4) << y << ':';
    for (x=0; x<w; x++)
      o << std::setw(14) << M(x,y);
    o << '\n';
  }
  return o;
}

/**
 * Printer
 * @author Aleksander Demko
 */
template <class T, int DIM> std::ostream & operator << (std::ostream &o,
    const scopira::basekit::const_nslice<T, DIM> &A)
{
  size_t k, kmax;
  for (k=0, kmax=A.size_rows(); k<kmax; ++k) {
    o << "Sub Slice (depth=" << DIM << ") " << k << ":\n";
    o << A.row_slice(k) << '\n';
  }

  return o;
}

/**
 * Printer
 * @author Aleksander Demko
 */
template <class T> inline std::ostream & operator << (std::ostream &o,
    const scopira::basekit::const_nslice<T, 1> &A)
{
  return print_vector_slice(o, A);
}

/**
 * Printer
 * @author Aleksander Demko
 */
template <class T> inline std::ostream & operator << (std::ostream &o,
    const scopira::basekit::const_nslice<T, 2> &A)
{
  return print_matrix_slice(o, A);
}

/**
 * Printer
 * @author Aleksander Demko
 */
template <class T, int DIM> inline std::ostream & operator << (std::ostream &o,
  const scopira::basekit::nslice<T, DIM> &A)
{
  return o << scopira::basekit::const_nslice<T, DIM>(A);
}

/**
 * Printer
 * @author Aleksander Demko
 */
template <class T, int DIM>
inline
std::ostream & operator << (std::ostream &o,
    const scopira::basekit::narray<T, DIM> &A)
{
  return o << A.all_slice();
}

/**
 * Printer
 * @author Aleksander Demko
 */
template <class T, int DIM>
inline
std::ostream & operator << (std::ostream &o,
    const scopira::basekit::narray_o<T, DIM> &A)
{
  return o << A.all_slice();
}

/**
 * Printer
 * @author Aleksander Demko
 */
template <int DIM>
std::ostream & operator << (std::ostream &o,
    const scopira::basekit::nindex<DIM> &d)
{
  o << '(' << d[0];
  for (size_t x=1; x<DIM; ++x)
    o << ',' << d[x];
  return o << ')';
}

/**
  \page scopirabasekitnarray Numeric arrays

  The scopira::basekit::narray template class is the core numerical
  data array class in Scopira. It can be used to make arrays
  of any dimention and with any data type.

  \section notessec Some notes

  Data access for multi-dimention arrays follow the x,y,z,etc style.
  In particular, matrices are x,y (rather than the mathematical convention
  of rows,cols). This is deliberate as to be consistant with higher
  dimention array.

  STL-like begin()/end() iteration is supported.

  You can always get at the raw data in an narray using the scopira::basekit::narray::c_array()
  method.

  Use [] for 1-dimention (vector) access. Use () for 2-dimention (matrix) access.
  () is also used for 3+-dimention access via nindex objects.

  When compiled in debug mode, all element access is tested via asserts(). This aids in
  debugging and development. Under release builds, these check is removed and all
  accesses are inlined -- just like native arrays.

  \section examplessec Basic Example

  This is an example of some basic usage of narrays.

  \code
  #include <scopira/tool/output.h>
  #include <scopira/basekit/narray.h>
  int main(void)
  {
    scopira::basekit::narray<double,2> M;     // this is a matrix of doubles
    scopira::basekit::narray<int,1> V;        // this is a vector of ints
    scopira::basekit::narray<float> F;        // this is a vector of floats (1 is the default when no dimention is sepecified)

    // resize the arrays
    M.resize(5, 4);
    V.resize(10);

    // set some initial values
    M.set_all(1.2);
    V.set_all(555);

    // so some data access
    // note that [] can ONLY be used in 1-dimentional vectors
    // () can be used for all dimentions
    M(0,0) = 22;                                // set top left corner of the matrix
    M(4, 3); = 33;                              // set bottom right corner of the matrix
    V[1] = 55;                                  // set the 2nd element in the vector

    OUTPUT << M.width() << "x" << M.height() << "; " << V.size() << '\n';
    OUTPUT << M << V << '\n';
  }
  \endcode

  \section slicesec Slices

  scopira::basekit::nslice is a template class that provides "slicing" for narrays.
  A slice is a small pointer into a subsection of a host-narray. A nslice has no
  data of it's own, it only knows where in the host narray it should be accessing.
  It is invalid to use a nslice after its hsot narray has been destroyed (or even
  simply resized).

  A nslice has most of the same methods and access functions as an narray.

  A nslice can have less dimentions that its host narray (but never more).
  For example, you can make a vector-like nslice out of a 2-dimention matrix
  narray.

  An example:

  \code
  {
    scopira::basekit::narray<double,2> M;
    scopira::basekit::nslice<double,2> subM;
    scopira::basekit::nslice<double,1> subV;

    // initialize M
    M.resize(10, 5);
    M.set_all(99);

    // print out the top left elements (3 by 3 square)
    subM = M.xyslice(0, 0, 3, 3);
    OUTPUT << subM << '\n';

    // print out the 2nd column
    subV = M.yslice(1, 0, M.height());
    OUTPUT << subV << '\n';
  }
  \endcode

*/

/**
  \page scopirabasekitdio Scopira DIO (DirectIO) Array File Format

  This section describes the Scopira DirectIO File Format.
  This is an easy to use, extensible, XML based data file format.

  The file itself is an XML file (with the .xml extension).

  \section overviewsec Overview

  This is an example DirectIO file:

  \verbatim
    <scopira_dio>
      <elem_type>float32</elem_type>
      <num_dimen>3</num_dimen>
      <size_0>380</size_0>
      <size_1>294</size_1>
      <size_2>500</size_2>
      <data_file>contraction.xml.data</data_file>
    </scopira_dio>
  \endverbatim

  The fields (all required) are:
    - elem_type: the type of elements, this is either int or float and the number
      of bits
    - num_dimen: the number of dimensions in the array (2 for example is a matrix)
    - size_0, size_1, etc: the size of each dimension, numbering starts from 0
    - data_file: the actual data file. This (pure binary) datafile contains
      the elements in Intel byte order. This filename may be a full path
      or may be a relative filename.

  \section payloadsec Payload File

  This binary file contains the actual elements in Intel byte order.
  The user never specifies this file (except perhaps in advances dialog boxes),
  its file name is referenced from the data_file field in the main XML file.

  \section scopirafuncsec Scopira functions

  Scopira provides the following libxml2 based functions for handling
  DirectIO files.

  Basic loading and saving:
    - scopira::basekit::load_directio_file
    - scopira::basekit::save_directio_file

  Information query:
    - scopira::basekit::query_directio_file

  Memory-mapped driven loading:
    - scopira::basekit::create_directio_file
    - scopira::basekit::bind_directio_file

*/

#endif

