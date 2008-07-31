
/*
 *  Copyright (c) 2002-2003    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#ifndef __INCLUDED_SCOPIRA_BASEKIT_VECTORMATH_H__
#define __INCLUDED_SCOPIRA_BASEKIT_VECTORMATH_H__

#include <math.h>

#include <scopira/basekit/narray.h>

// THIS FILE HAS BEEN FULLY DOCUMENTED

namespace scopira
{
  namespace basekit
  {
    /**
     * Adds val to every element in v. v can be any
     * class that uses iterators.
     *
     * @param v the vector to operator one
     * @param val the value to add to each element in v
     * @author Aleksander Demko
     */ 
    template <class T>
      void add_scalar(T &v, typename T::data_type val);
    /**
     * Subtracts val to every element in v. v can be any
     * class that uses iterators.
     *
     * @param v the vector to operator one
     * @param val the value to add to each element in v
     * @author Aleksander Demko
     */ 
    template <class T>
      void sub_scalar(T &v, typename T::data_type val);
    /**
     * Multiples val to every element in v. v can be any
     * class that uses iterators.
     *
     * @param v the vector to operator one
     * @param val the value to add to each element in v
     * @author Aleksander Demko
     */ 
    template <class T>
      void mul_scalar(T &v, typename T::data_type val);
    /**
     * Divides val to every element in v. v can be any
     * class that uses iterators.
     *
     * @param v the vector to operator one
     * @param val the value to add to each element in v
     * @author Aleksander Demko
     */ 
    template <class T>
      void div_scalar(T &v, typename T::data_type val);
 

    // not counst parms for muffinization
    // probably wont need a switcher function because of micro-functions
    // median (TODO)

    /**
     * Finds the minimum.
     *
     * @param v any container that has iteration facilities
     * @param out the output scalar
     * @return the index of the min
     * @author Aleksander Demko
     */ 
    template <class V, class T>
      size_t min(const V &v, T& out);
    /**
     * Finds the maximum.
     *
     * @param v any container that has iteration facilities
     * @param out the output scalar
     * @return the index of the max
     * @author Aleksander Demko
     */ 
    template <class V, class T>
      size_t max(const V &v, T& out);
    /**
     * Calculates the sum
     *
     * @param v any container that has iteration facilities
     * @param out the output scalar
     * @author Aleksander Demko
     */ 
    template <class V, class T>
      void sum(const V &v, T& out);
    /**
     * Calculates the mean
     *
     * @param v any container that has iteration facilities
     * @param out the output scalar
     * @author Aleksander Demko
     */ 
    template <class V, class T>
      void mean(const V &v, T& out);
    /**
     * Calculates variance
     *
     * @param v any container that has iteration facilities
     * @param out the output scalar
     * @author Aleksander Demko
     */ 
    template <class V, class T>
      void variance(const V &v, T& out);
    /**
     * Calculates standard deviation
     *
     * @param v any container that has iteration facilities
     * @param out the output scalar
     * @author Aleksander Demko
     */ 
    template <class V, class T>
      void stddev(const V &v, T& out)
        { variance<V,T>(v, out); out = static_cast<T>(::sqrt(out)); }
    /**
     * Adds each element in delta to each element in target
     *
     * @param target target iterator-supporting container
     * @param delta iterator-supporting delta vector
     * @author Aleksander Demko
     */ 
    template <class LH, class RH>
      void add_vector(LH &target, const RH& delta);
    /**
     * Subtracts each element in delta to each element in target
     *
     * @param target target iterator-supporting container
     * @param delta iterator-supporting delta vector
     * @author Aleksander Demko
     */ 
    template <class LH, class RH>
      void sub_vector(LH &target, const RH& delta);
    /**
     * Multiplies each element in delta to each element in target
     *
     * @param target target iterator-supporting container
     * @param delta iterator-supporting delta vector
     * @author Aleksander Demko
     */ 
    template <class LH, class RH>
      void mul_vector(LH &target, const RH& delta);
    /**
     * Calculates the absolute value of each element in source,
     * and writes it to target.
     *
     * target and source may be the same instance.
     *
     * @param target target iterator-supporting container
     * @param source iterator-supporting source vector
     * @author Brion Dolenko
     */ 
    template <class LH, class RH>
      void abs_vector(LH &target, const RH& source);
    /**
     * Prints all the elements in the given iterator supporting container
     *
     * @param o the output stream
     * @param vv the container that supports iteration
     * @author Aleksander Demko
     */ 
    template <class V>
      void print_vector(scopira::tool::oflow_i &o, const V &vv);
    /**
     * Calculates pearson.
     *
     * @author Mark Alexiuk
     * @author Rodrigo Vivanco  
     * @author Conrad Wiebe
     */
    template <class T>
      void pearson_scalar(T &x, T &y, double& rho);

  	/**
  	 * Constructs an index for a vector
  	 * 
  	 * @param array the vector to be sorted
  	 * @param index the index array
  	 * 
  	 * @author Shantha Ramachandran
  	 */
  	template <class T>
  		void construct_index(narray<T> &array, narray<int> &index);  

    template <class DATAT, class REFT>
      class vecindex_sortable;
  }
}

template <class T>
  void scopira::basekit::add_scalar(T &v, typename T::data_type val)
{
  typedef typename T::iterator iter;
  iter ii=v.begin(), endii = v.end();

  for (; ii != endii; ++ii)
    (*ii) += val;
}

template <class T>
  void scopira::basekit::sub_scalar(T &v, typename T::data_type val)
{
  typedef typename T::iterator iter;
  iter ii=v.begin(), endii = v.end();

  for (; ii != endii; ++ii)
    (*ii) -= val;
}

template <class T>
  void scopira::basekit::mul_scalar(T &v, typename T::data_type val)
{
  typedef typename T::iterator iter;
  iter ii=v.begin(), endii = v.end();

  for (; ii != endii; ++ii)
    (*ii) *= val;
}

template <class T>
  void scopira::basekit::div_scalar(T &v, typename T::data_type val)
{
  typedef typename T::iterator iter;
  iter ii=v.begin(), endii = v.end();

  for (; ii != endii; ++ii)
    (*ii) /= val;
}

template <class V, class T>
  size_t scopira::basekit::min(const V &v, T& out)
{
  typedef typename V::const_iterator iter;
  iter ii, endii;
  size_t idx, cur;
  T m;

  ii = v.begin();
  endii = v.end();
  assert(ii != endii);
  m = *ii;
  cur = idx = 0;
  for (++ii, ++cur; ii!=endii; ++ii, ++cur)
    if (*ii < m) {
      m = *ii;
      idx = cur;
    }
  out = m;
  return idx;
}

template <class V, class T>
  size_t scopira::basekit::max(const V &v, T& out)
{
  typedef typename V::const_iterator iter;
  iter ii, endii;
  size_t idx, cur;
  T m;

  ii = v.begin();
  endii = v.end();
  assert(ii != endii);
  m = *ii;
  cur = idx = 0;
  for (++ii, ++cur; ii!=endii; ++ii, ++cur)
    if (*ii > m) {
      m = *ii;
      idx = cur;
    }
  out = m;
  return idx;
}

template <class V, class T>
  void scopira::basekit::sum(const V &v, T& out)
{
  typedef typename V::const_iterator iter;
  iter ii, endii;
  T m;

  m = 0;
  ii = v.begin();
  endii = v.end();
  for (; ii!=endii; ++ii)
    m += *ii;
  out = m;
}

template <class V, class T>
  void scopira::basekit::mean(const V &v, T& out)
{
  if (v.empty()) {
    out = 0;
    return;
  }
  sum(v, out);
  out = out / v.size();
}

template <class V, class T>
  void scopira::basekit::variance(const V &v, T& out)
{
  typedef typename V::const_iterator iter;
  iter ii, endii;
  size_t mx;
  T avg, sum, tt;

  mx = v.size();
  if (mx <= 1) {
    out = 0; //special, stupid case
    return;
  }

  mean(v, avg);

  sum = 0;
  endii = v.end();
  for (ii=v.begin(); ii != endii; ++ii) {
    tt = *ii - avg;
    sum += tt * tt;
  }

  out = sum / (mx - 1);
}
template <class LH, class RH>
  void scopira::basekit::add_vector(LH &target, const RH& delta)
{
  typename LH::iterator tar, endtar;
  typename RH::const_iterator src, endsrc;

  tar = target.begin();
  endtar = target.end();
  src = delta.begin();
  endsrc = delta.end();
  while (tar != endtar) {
    assert(src != endsrc);
    *tar += *src;
    ++tar;
    ++src;
  }
}

template <class LH, class RH>
  void scopira::basekit::sub_vector(LH &target, const RH& delta)
{
  typename LH::iterator tar, endtar;
  typename RH::const_iterator src, endsrc;

  tar = target.begin();
  endtar = target.end();
  src = delta.begin();
  endsrc = delta.end();
  while (tar != endtar) {
    assert(src != endsrc);
    *tar -= *src;
    ++tar;
    ++src;
  }
}

template <class LH, class RH>
  void scopira::basekit::mul_vector(LH &target, const RH& delta)
{
  typename LH::iterator tar, endtar;
  typename RH::const_iterator src, endsrc;

  tar = target.begin();
  endtar = target.end();
  src = delta.begin();
  endsrc = delta.end();
  while (tar != endtar) {
    assert(src != endsrc);
    *tar *= *src;
    ++tar;
    ++src;
  }
}

template <class LH, class RH>
  void scopira::basekit::abs_vector(LH &target, const RH& source)
{
  typename LH::iterator tar, endtar;
  typename RH::const_iterator src, endsrc;

  tar = target.begin();
  endtar = target.end();
  src = source.begin();
  endsrc = source.end();
  while (tar != endtar) {
    assert(src != endsrc);
    *tar = (*src < 0.0) ? (*src * -1) : *src;
    ++tar;
    ++src;
  }
}

template <class V>
  void scopira::basekit::print_vector(scopira::tool::oflow_i &o, const V &vv)
{
  typedef typename V::const_iterator iter;
  iter ii = vv.begin(), endii = vv.end();
  o << '(';
  for (; ii != endii; ++ii)
    o << *ii << ',';
  o << ")\n";
}

/**
 * Pearson coefficient of correlation.
 * 
 * @param x first sample to compare
 * @param y second sample to compare
 * @param rho pearson correlation
 * 
 * @author Mark Alexiuk
 * @author Rodrigo Vivanco  
 * @author Conrad Wiebe
 */  
// see Evident Scorpio implementation
template <class T>
  void scopira::basekit::pearson_scalar(T &x, T &y, double& rho)
{
	double sX;		// sum of X pts
	double sXX;	  // sum of squares
	double sXY;
	double sY;
	double sYY;
	double top, btm;

	sX = sXX = sY = sYY = sXY = 0;
  int n = x.size();

  assert(x.size() == y.size());

	for (long i=0; i<n; i++) 
	{
		sX += x[i];
		sXX += x[i] * x[i];
		sXY += x[i] * y[i];
		sY += y[i];
		sYY += y[i] * y[i];
	}

	top = sXY - (sX*sY/n);
	btm = (sXX-sX*sX/n) * (sYY-sY*sY/n);

	if (top==0 || btm==0)
    rho = 0;
  else
  	rho = top / sqrt(btm);
}

// constructs an index for a vector
template <class T>
	void scopira::basekit::construct_index(narray<T> &array, narray<int> &index)
{
	int i, j, k, n, l=0, ir, temp_index, temp;
	int stack_index = -1;
	T array_val;
	narray<int> stack;
	
	stack.resize(50);
	n = array.size();
	ir = n-1;
	
	for (j=0; j<n; j++)
		index[j] = j;
	while (true) {
		if (ir-l < 7) {
			for (j=l+1; j<=ir; j++) {
				temp_index = index[j];
				array_val = array[temp_index];
				for (i=j-1; i>=1; i--) {
					if (array[index[i]] <= array_val)
						break;
					index[i+1] = index[i]; 
				}
				index[i+1] = temp_index;
			}
			if (stack_index < 0)
				break;
			ir = stack[stack_index--];
			l = stack[stack_index--];
		} else {
			k = (l+ir) >> 1;
			temp = index[k];  index[k] = index[l+1];  index[l+1] = temp;
			if (array[index[l+1]] > array[index[ir]]) {
				temp = index[l+1];  index[l+1] = index[ir];  index[ir] = temp;
			}
			if (array[index[l]] > array[index[ir]]) {
				temp = index[l];  index[l] = index[ir];  index[ir] = temp;
			}
			if (array[index[l+1]] > array[index[l]]) {
				temp = index[l+1];  index[l+1] = index[l];  index[l] = temp;
			}
			i = l+1;
			j = ir;
			temp_index = index[l+1];
			array_val = array[temp_index];
			while (true) {
				i++;
				while (array[index[i]] < array_val)
					i++;
				j--;
				while (array[index[j]] > array_val)
					j--;
				if (j < i)
					break;
				temp = index[i];  index[i] = index[j];  index[j] = temp;
			}
			index[l+1] = index[j];
			index[j] = temp_index;
			stack_index += 2;
			if (stack_index >= 50) {
				//KERROR << "Stack size too small in constructing index\n";
				return;
			}
			if (ir-i+1 >- j-l) {
				stack[stack_index] = ir;
				stack[stack_index-1] = i;
				ir = j-1;
			} else {
				stack[stack_index] = j-1;
				stack[stack_index-1] = l;
				l = i;
			}
		}
	}
}

/**
 * This template class is a "sortable" object (passable to
 * tool::qsort, in tool/sort_imp.h)
 *
 * data (of type DATAT) is a vector of INDECIES into ref (of type REFT).
 * This will sort data (the indicies) based on their refered to values in ref.
 * ref will not be modified.
 *
 * @author Aleksander Demko
 */ 
template <class DATAT, class REFT>
  class scopira::basekit::vecindex_sortable
{
  public:
    typedef DATAT data_type;
    typedef REFT ref_type;
  private:
    data_type &dm_data;
    const ref_type &dm_ref;
  public:
    vecindex_sortable(DATAT &data, REFT &ref) : dm_data(data), dm_ref(ref) { }
    int compare_element(int lidx, int ridx) const {
      typedef typename ref_type::data_type t;
      t l, r;

      l = dm_ref[dm_data[lidx]];
      r = dm_ref[dm_data[ridx]];
      if (l<r)
        return -1;
      else if (l>r)
        return 1;
      else
        return 0;
    }
    void swap_element(int lidx, int ridx) {
      typedef typename data_type::data_type t;
      t tmp;

      tmp = dm_data[lidx];
      dm_data[lidx] = dm_data[ridx];
      dm_data[ridx] = tmp;
    }
};

#endif


