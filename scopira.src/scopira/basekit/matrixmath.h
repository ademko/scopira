
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

#ifndef __INCLUDED_SCOPIRA_BASEKIT_MATRIXMATH_H__
#define __INCLUDED_SCOPIRA_BASEKIT_MATRIXMATH_H__

#include <scopira/tool/output.h>
#include <scopira/basekit/math.h>
#include <scopira/basekit/narray.h>

// THIS FILE HAS BEEN FULLY DOCUMENTED

namespace scopira
{
  namespace basekit
  {
    /*************************
      new data model stuff
    *************************/

    /**
     * Multiple each element in the given matrix-like lass by the given scalar
     *
     * @param dest the matrix like class to operate on
     * @param d the value to multiple
     * @author Aleksander Demko
     */
    template <class T, class SCA>
      void mul_matrix(T &dest, SCA d);

    /**
     * Matrix-matrix copy, with inline transposing.
     *
     * @param dest the result/destination matrix. will be resized.
     * @param m1 the first matrix
     * @param m2 the second matrix
     * @param t1 transpose the 1st matrix
     * @param t2 transpose the 2nd matrix
     * @author Aleksander Demko
     */
    template <class T, class T2, class T3>
      void mul_matrix(T &dest, const T2 &m1, const T3 &m2, bool t1 = false, bool t2 = false);

    /**
     * Inverts the matrix.
     *
     * @param src the source matrix
     * @param desc the output matrix
     * @return true if the result was non-singular (OK)
     * @author Aleksander Demko
     */ 
    template <class T>
      bool invert_matrix(T &dest, const T& src);
      
    /**
     * Transposes a matrix.
     *
     * @param src the source matrix
     * @param desc the output matrix (cannot be the same as src)
     * @author Aleksander Demko
     */ 
    template <class D, class S>
      void transpose_matrix(D &dest, const S& src);

    /**
     * Resamples the src matrix to the destination matrix, using (fast)
     * nearest neighbor.
     *
     * @author Aleksander Demko
     */
    template <class SRC, class DEST>
      void resample_nn(DEST &dest, const SRC &src);

    /**
     * Resamples the src matrix to the destination matrix, using (not so fast)
     * linear interpolation.
     *
     * @author Aleksander Demko
     */
    template <class SRC, class DEST>
      void resample_linear(DEST &dest, const SRC &src);

    /**
     *  Calaculates the LU decomposition of a rowwise permutation of the input matrix src 
     *
     * @param desc the output LU decomposition matrix
     * @param src the source matrix
     * @param idx is an output vector that records the row permutation effected by the partial pivoting
     * @param odd indicated whether the number of row interchanges was even or odd, respectively.
     *     
     * @author Marina Mandelzweig
     */ 
    template <class T> 
      bool lu_decomposition(narray<T,2>& dest, const narray<T,2>& src,narray<int>& idx, bool& odd);
    
    /*
    * Linear equation solution, backsubstitution
    * Solves the set of n linear equations a�x = b.
    * @param desc the solution vector x
    * @param inv_a is the input matrix, which is the LU decomposition of a
    * @param index is the permutation vector returned by the LU decomposition.    
    * @param b is the input right-hand side vector.    
    *     
    * @author Marina Mandelzweig
    */
    template <class T>
      void lu_backsubstitution(narray<T>& dest, const narray<T,2>& inv_a, const narray<int>& index);
  
    /**
    * construct the singular value decomposition of the matrix u
    * svd results are:Mv,Vw,Mu(src matrix modified) return false if was not 
    * able to perform svd.
    *
    * @param Mu-M x M orthogonal matrix - u (src matrix modified)
    * @param Vw- N  diagonal vector of singular values
    * @param Mv-N x M orthogonal matrix
    */
    template <class T>
      bool svd(narray<T,2>& Mu,narray<T>& Vw,narray<T,2>& Mv);
    
   /**
    * Solves linear equation for x using backsubstitution and the results of SVD
    * Solves Dx = b where D has been decomposed int Mu,Vw and Mv. 
    * Solution is in x
    * 
    * @param Mu orthoganal matrix from SVD
    * @param Vw vector of singular values from SVD
    * @param Mv orthoganal matrix from SVD
    * @param b right hand side data vector
    * @param x the solution
    */
    template <class T>
    	void svd_backsubstitution(narray<T,2> &Mu, narray<T> &Vw, narray<T,2> &Mv, narray<T> &b, narray<T> &x);
    
    /**
    * Computes Pythagorean Theorem: (a^2 + b^2)^0.5 without destructive underflow or overflow
    *
    */
    template <class DT> 
      DT pythagorean(DT a, DT b);
      
    /**
     * Reduces a general matrix to Hessenberg form
     * 
     * @param matrix the matrix to be reduced
     * 
     * @author Shantha Ramachandran
     */
		template <class T>
			void elim_hessian(narray<T,2> &matrix);
      
   /**
    * Calculates the eigenvalues of a Hessenberg matrix
    * 
    * @param matrix the Hessenberg matrix
    * @param eigen_real the real parts of the eigenvalues
    * @param eigen_imaginary the imaginary parts of the eigenvalues
    * 
    * @author Shantha Ramachandran
    */
    template <class T>
      void eigen_hessenberg(narray<T,2> &matrix, narray<T> &eigen_real, narray<T> &eigen_imaginary);
      
    /**
     * Gauss-Jordan matrix inversion and linear equations solution
     * The input matrix 'matrix' is replaced by its inverse, and the input 
     * matrix 'rhs' is replaced by the set of solution vectors
     * 
     * @param matrix the matrix to be inverted
     * @param rhs the right hand side matrix of the linear equation
     * 
     * @author Shantha Ramachandran
     */
   	template <class T>
   		void gauss_jordan(narray<T,2> &matrix, narray<T,2> &rhs);
   		
   	/**
   	 * Rearrange covariance matrix
   	 * 
   	 * @param matrix the covariance matrix
   	 * @param fitted fitting coefficients
   	 * 
   	 * @author Shantha Ramachandran
   	 */
    template <class T>
    	void sort_covariance(narray<T,2> &matrix, narray<int> &fitted, int mfit);
  }
}

template <class T, class SCA>
  void scopira::basekit::mul_matrix(T &dest, SCA d)
{
  typename T::iterator tar, endtar;

  endtar = dest.end();
  for (tar = dest.begin(); tar != endtar; ++tar)
    *tar *= d;
}

template <class T, class T2, class T3>
  void scopira::basekit::mul_matrix(T &dest, const T2 &m1, const T3 &m2, bool t1, bool t2)
{
  typedef typename T::data_type data_type;
  size_t w, h, x, y, i, len;
  size_t rx1, ry1, *x1, *y1;
  size_t rx2, ry2, *x2, *y2;
  data_type sum;

  // calc my dimensions and settings by the transpose flags
  if (t1) {
    h = m1.width();
    len = m1.height();
    x1 = &ry1;
    y1 = &rx1;
  } else {
    h = m1.height();
    len = m1.width();
    x1 = &rx1;
    y1 = &ry1;
  }
  if (t2) {
    w = m2.height();
    x2 = &ry2;
    y2 = &rx2;
  } else {
    w = m2.width();
    x2 = &rx2;
    y2 = &ry2;
  }

  assert( (t1 ? m1.height() : m1.width()) == (t2 ? m2.width() : m2.height()) );

  // resize myself
  dest.resize(w, h);

  // do the multiply
  for (y=0; y<h; y++)
    for (x=0; x<w; x++) {
      // start the accumulator
      sum = 0;
      *y1 = y;
      *x2 = x;
      for (i=0; i<len; i++) {
        *x1 = i;
        *y2 = i;
        sum += m1(rx1, ry1) * m2(rx2, ry2);
      }
      dest(x,y) = sum;
    }
}

template <class T>
  bool scopira::basekit::invert_matrix(T &dest, const T& src)
{
  typedef typename T::data_type data_type;
  size_t i, j, n;
  bool odd;
  narray<data_type,2> m_orig;
  narray<int> v_idx;
  narray<data_type> v_col;

  n = src.width();
  
  m_orig.resize(n, src.height());
  dest.resize(n, src.height());
  v_idx.resize(n);
  v_idx.clear();
  v_col.resize(n);
  v_col.clear();

  //decompose the matrix just once
  if (!scopira::basekit::lu_decomposition<data_type>(m_orig, src, v_idx, odd))       
    return false; // a singularity in the time continuum has been detected captain, abort!

  //find inverse by columns
  for (j=0; j<n; j++) {                     
    v_col.clear();
    v_col.set(j, 1.0);
    scopira::basekit::lu_backsubstitution<data_type>(v_col, m_orig, v_idx);
    for (i=0; i<n; i++)
      dest.set(j, i, v_col.get(i));
  }

  return true;
}

template <class D, class S>
  void scopira::basekit::transpose_matrix(D &dest, const S& src)
{
  size_t x, y, w, h;

  w = src.width();
  h = src.height();

  dest.resize(h, w);

  for (y=0; y<h; ++y)
    for (x=0; x<w; ++x)
      dest(y,x) = src(x,y);
}

template <class SRC, class DEST>
  void scopira::basekit::resample_nn(DEST &dest, const SRC &src)
{
  typedef typename DEST::data_type data_type;

  if (dest.empty() || src.empty())
    return;

  for (size_t y=0; y<dest.height(); ++y)
    for (size_t x=0; x<dest.width(); ++x)
      dest(x,y) = src(src.width() * x / dest.width(), src.height() * y / dest.height());
}

template <class SRC, class DEST>
  void scopira::basekit::resample_linear(DEST &dest, const SRC &src)
{
  typedef typename DEST::data_type data_type;
  double xsrc_rate, xleft, xremainer, xfer;   // all in "src" units
  double xdest_rate;
  double ysrc_rate, yleft, yremainer, yfer;   // all in "src" units
  double ydest_rate;

  if (dest.empty() || src.empty())
    return;

  dest.set_all(0);

  xsrc_rate = static_cast<double>(src.width()) / dest.width();
  ysrc_rate = static_cast<double>(src.height()) / dest.height();
  // used a mutliplier to convert src-units to dest
  xdest_rate = static_cast<double>(dest.width()) / src.width();
  ydest_rate = static_cast<double>(dest.height()) / src.height();

  yremainer = 0;
  size_t ysrc = 0;
  for (size_t ydest=0; ydest<dest.height(); ++ydest) {
    yleft = ysrc_rate;

    while (!scopira::basekit::is_zero(yleft)) {
      yfer = yleft;
      if (yfer + yremainer > 1)
        yfer = 1 - yremainer;

      xremainer = 0;
      size_t xsrc = 0;
      for (size_t xdest=0; xdest<dest.width(); ++xdest) {
        xleft = xsrc_rate;

        // while we still have work to do, iterate over the src blocks
        while (!scopira::basekit::is_zero(xleft)) {
          xfer = xleft;
          if (xfer + xremainer > 1)
            xfer = 1 - xremainer;
          dest(xdest,ydest) += src(xsrc,ysrc) * xfer * xdest_rate * yfer * ydest_rate;
          xleft -= xfer;
          xremainer += xfer;
          if (xremainer >= 1) {
            assert(scopira::basekit::is_equal(xremainer,1.0));   // >1 shouldnt happen
            xremainer = 0;
            ++xsrc;
          }
        }//while xleft
      }//for xdest

      yleft -= yfer;
      yremainer += yfer;
      if (yremainer >= 1) {
        assert(scopira::basekit::is_equal(yremainer,1.0));
        yremainer = 0;
        ++ysrc;
      }
    }//while yleft
  }//for ydest
}

// Calculates LU decomposition of a rowise permutation.
template <class T>
  bool scopira::basekit::lu_decomposition(narray<T,2>& dest, const narray<T,2>& src, narray<int>& v_idx, bool& odd)
{
  int i, max, j, k, n;
  T largest, dum, sum, tmp;
  //stores the implicit scaling of each row
  narray<T> scaling;            

  n = src.width();
  dest.copy(src);
  scaling.resize(n);
  scaling.clear();
  //no rows interchanged yet
  odd = false;           
  // to avoid compile warning    
  tmp = 0; 
  
  //loop over the rows to get the implicit scaling information
  for (i=0; i<n; i++) {
    largest = 0;
    for (j=0; j<n; j++) {
      tmp = ::fabs(dest.get(j,i));
      if (tmp > largest)
        largest = tmp;
    }
    // no non-zero largest element
    if (largest == 0)
      return false;
    //save the scaling
    scaling.set(i, 1.0/largest);          
  }
  
  //loop over columns 
  for (j=0; j<n; j++) {       
    for (i=0; i<j; i++) {
      sum = dest.get(j,i);
      for (k=0; k<i; k++)
        sum -= dest.get(k, i) * dest.get(j, k);
      dest.set(j, i, sum);
    }
    //init the search for the largest pivot element
    largest = 0;                
    max = 0;                
    for (i=j; i<n; i++) {
      sum = dest.get(j, i);
      for (k=0; k<j; k++)
        sum -= dest.get(k, i) * dest.get(j, k);
      dest.set(j, i, sum);
      //is the figure of merit for the pivot better than the best so far
      dum=scaling.get(i) * ::fabs(sum);
      if (dum >= largest) {
        largest = dum;
        max = i;
      }
    }
    //do we need to interchange rows
    if (j != max) {          
      //yes, do so...
      for (k=0; k<n; k++) {   
        dum = dest.get(k, max);
        dest.set(k, max, dest.get(k, j));
        dest.set(k, j, dum);
      }
      //...and change the parity of odd
      odd = !odd;
      //also interchange the scale factor
      scaling.set(max, scaling.get(j));      
    }
    v_idx.set(j, max);
    if (dest.get(j, j) == 0)
      dest.set(j, j, tinynum_c);
    //if the pivot element is zero, the matrix is singular (at least to the precision of
    //the algorithm. For some applications on singular matrices, it is desirable to
    //substitue tinynum_c for zero
    if (j != (n-1)) {            
       //now, finally, divide by the pivot element
      dum = 1.0 / dest.get(j, j);
      for (i=j+1; i<n; i++)
        dest.set(j, i, dest.get(j,i)*dum);
    }
  } //go back for the next column in the reduction  
  return true;
}


// Solves the set of n linear equations a�x = b.
template <class T>
  void scopira::basekit::lu_backsubstitution(narray<T>& dest, const narray<T,2>& inv_a, const narray<int>& index)
{
  int i, j,k, m, n;
  T sum;
  
  n = inv_a.width();
  m = -1;
  
  //forward substitution,
  for (i=0; i<n; i++) {
    k = index.get(i);
    sum = dest.get(k);
    dest.set(k, dest.get(i));
    if (m != -1)
      for (j=m; j<= i-1; j++)
        sum -= inv_a.get(j, i) * dest.get(j);
    else
      if (sum != 0.0)
        m=i;
    dest.set(i, sum);
  }
  
  dest.copy(dest);
  //backsubstitution,
  for (i=n-1; i>=0; i--) {    
    sum = dest.get(i);
    for (j=i+1; j<n; j++)
      sum-=inv_a.get(j,i) * dest.get(j);
    //store a component of the solution vector dest.
    dest.set(i, sum/inv_a.get(i,i));
  }
}


namespace scopira
{
  namespace basekit
  {
    template <class T>
      inline T svd_sign(T v1, T v2) { if (v2>=0.0) return fabs(v1); else return -fabs(v1); }
  }
}

//construct the singular value decomposition of the matrix u
template <class T>    
  bool scopira::basekit::svd(narray<T,2>& u,narray<T>& w,narray<T,2>& v)
{

  int i,j,k,left,height,width,nm,q,p;
  T a,b,c,num_r,h,s,scale,x,y,z;
  bool flag;
  narray<T> vec_t;
  
  width = u.width();
  height=u.height();

  v.resize(width,width);
  v.clear();
  w.resize(width);
  w.clear();
  
  b=scale=num_r=0.0;  
  left= nm = 0;
  
  vec_t.resize(width);
  vec_t.clear();
  //householder reduction to bidiagonal form 
  for(i=0;i<width;i++){
    //left-hand reduction 
    left = i+1;
    vec_t[i]=(scale*b);
    b = s = scale = 0.0;
    
    if(i < height) {
      for(k=i;k<height;k++)
        scale += fabs(u(i,k));
      if(scale != 0.0) {
        for(k=i;k<height;k++) {
          u(i,k)=u(i,k)/scale;
          s += u(i,k) * u(i,k);
        }
      
        a = u(i,i);
        b = -svd_sign(sqrt(s),a);
        h = a * b - s;
        u(i,i)=a - b;
        
        for(j=left;j<width;j++) {
          s=0.0;
          for(k=i;k<height;k++)
            s += u(i,k) * u(j,k);
          a = s / h;
          for(k=i;k<height;k++)
            u(j,k)=u(j,k)+ a * u(i,k);
        }
        for(k=i;k<height;k++)
          u(i,k)=u(i,k)*scale;
      }
    }
  
    w[i]=scale *b;
    b=s=scale=0.0;
  
    //right-hand reduction 
    if(i<height && i+1 != width) {
      
      for (k=left;k<width;k++)
        scale += fabs(u(k,i));
      
      if (scale != 0.0){
        for (k=left;k<width;k++){
          u(k,i) = u(k,i)/ scale;
          s += u(k,i) * u(k,i);
        }
   
        a=u(left,i);
        b = -svd_sign(sqrt(s),a);
        h=a * b - s;
        u(left,i)= a-b;
        for (k=left;k<width;k++)
          vec_t[k] = u(k,i)/h;
        for (j=left;j<height;j++){
          s=0.0;
          for (k=left;k<width;k++)         
            s += u(k,j)* u(k,i);
          for (k=left;k<width;k++)   
            u(k,j) = u(k,j) + s * vec_t[k];
        }
        for (k=left;k<width;k++)        
          u(k,i) = u(k,i) * scale;
      } 
    }
    num_r=std::max(num_r,(fabs(w[i])+fabs(vec_t[i])));   
  }
  
  //accumulation of right-hand transformations.
  for (i=width-1;i>=0;i--){
    if(i < width-1){
      if (b !=0.0){
        //double division to avoid possible underflow.
        for (j=left;j<width;j++)
          v(i,j)=(u(j,i) /u(left,i))/b;
          
        for (j=left;j<width;j++){
          s=0.0;
          for (k=left;k<width;k++)
            s+=u(k,i)* v(j,k);
          for (k=left;k<width;k++)
            v(j,k) = v(j,k)+ s * v(i,k);
        }
      }
      for (j=left;j<width;j++){
        v(j,i) = 0.0;
        v(i,j) = 0.0; 
      }
    }
    v(i,i) = 1.0;
    b=vec_t[i];
    left=i;
  }
  
  //accumulation of left-hand transformations.
  for (i=std::min(height,width)-1;i>=0;i--)
  {
    left=i+1;
    b=w[i];
    for (j=left;j<width;j++)
      u(j,i) = 0.0;
    if (b !=0.0){
      b = 1.0 / b;
      for (j=left;j<width;j++){
        s=0.0;
        for (k=left;k<height;k++)
          s+= u(i,k)* u(j,k);
        a=(s/u(i,i) )*b;
        for (k=i;k<height;k++)
          u(j,k) = u(j,k) + a * u(i,k);
      }
      for (j=i;j<height;j++)
        u(i,j) = u(i,j) * b;
    }
    else
      for (j=i;j<height;j++)
        u(i,j) = 0.0;

    u(i,i) = u(i,i) + 1;  
  }

  //diagonalization of the bidiagonal form: Loop over
  //singular values, and over allowed iterations.
  for (k=width-1;k>=0;k--){
    for (p=0;p<30;p++){
      flag=true;
      for (left=k;left>=0;left--){ 
        //test for splitting.
        nm = left-1;    //note that vec_t[1] is always zero.
        if (static_cast<T>(fabs(vec_t[left])+num_r) == static_cast<T>(num_r)){
          flag=false;
          break;
        }
        if (static_cast<T>(fabs(w[nm])+num_r) == static_cast<T>(num_r))
          break;
      }
      if (flag){
        c=0.0; //cancellation of vec_t[l], ifl>1.
        s=1.0;
        for (i=left;i<=k;i++) {

          a=s * vec_t[i];
          vec_t[i] = c * vec_t[i];

          if (static_cast<T>(fabs(a)+num_r) == static_cast<T>(num_r))
            break;
            
          b=w[i];
          h= pythagorean<T>(a,b);
          w[i] = h;
          h=1.0/h;
          c=b*h;
          s = -a*h;

          for (j=0;j<height;j++){
            y=u(nm,j);
            z=u(i,j);
            u(nm,j) = y*c+z*s;
            u(i,j) = z*c-y*s;
          }
        }
      }

      z=w[k];
      
      if (left == k){
        //convergence.
        if (z < 0.0){
          //singular value is made nonnegative.
          w[k] = -z;
          for (j=0;j<width;j++)
            u(k,j) =-(u(k,j));
        }
        break;
      }
      
      if (p == 29)
        return false;

      x=w[left]; //shift from bottom 2-by-2minor.
      nm=k-1;
      y=w[nm];
      b=vec_t[nm];
      h=vec_t[k];
      a=((y-z)*(y+z)+(b-h)*(b+h))/(2.0*h*y);
      b= pythagorean<T>(a,1.0);
      a = ((x - z) * (x + z) + h * ((y / (a + svd_sign(b,a))) - h)) / x;
      c=s=1.0;
      //next QR transformation:
      for (j=left;j<=nm;j++){
        i=j+1;
        b=vec_t[i];
        y=w[i];
        h=s*b;
        b=c*b;
        z= pythagorean<T>(a,h);        
        vec_t[j] = z;
        c=a/z;
        s=h/z;
        a=x*c+b*s;
        b = b*c-x*s;
        h=y*s;
        y *= c;
        for (q=0;q<width;q++){
          x=v(j,q);
          z=v(i,q);
          v(j,q) = x*c+z*s;
          v(i,q) = z*c-x*s;
        }
        z=pythagorean<T>(a,h);
        w[j] = z; //rotation can be arbitrary if z = 0.
        if (z != 0.0){
          z=1.0/z;
          c=a*z;
          s=h*z;
        }
        a=c*b+s*y;
        x=c*y-s*b;
        for (q=0;q<height;q++){
          y=u(j,q);
          z=u(i,q);
          u(j,q) = y*c+z*s;
          u(i,q) = z*c-y*s;
        }
      }
     
      vec_t[left] = 0.0;
      vec_t[k] = a;
      w[k] = x;
    } 
  }
   return true;
}

// solves Dx = b for x using backsubstitution and the results of SVD
template <class T>
 	void scopira::basekit::svd_backsubstitution(narray<T,2> &Mu, narray<T> &Vw, narray<T,2> &Mv, narray<T> &b, narray<T> &x)
{
	narray<T> temp;
  T s;
  int w, h, i, j;
  
  w = Mu.width();
  h = Mu.height();
  temp.resize(w);
  
  for (i=0; i<w; i++) {
    s = 0;
    if (Vw[i] != 0) {
      for (j=0; j<h; j++) {
        s += Mu(i,j) * b[j];
      }
      s = s/Vw[i];
    }
    temp[i] = s;
  }
  
  for (i=0; i<w; i++) {
    s = 0;
    for (j=0; j<w; j++) {
      s+= Mv(j,i) * temp[j];
    }
    x[i] = s;
  }
}

//Computes Pythagorean Theorem: (a^2 + b^2)^0.5 without destructive underflow or overflow
template <class DT> 
  DT scopira::basekit::pythagorean(DT a, DT b)
{
  DT abs_a,abs_b;

  abs_a = fabs(a);
  abs_b = fabs(b);

  if(abs_a < abs_b)
    return(abs_b == 0.0 ? 0.0 : abs_b * sqrt(pow((abs_a / abs_b),2) +1.0));
  else
    return(abs_a * sqrt(pow((abs_b/abs_a),2) +1.0));
}

// reduces a general matrix to hessenberg form
template <class T>
	void scopira::basekit::elim_hessian(narray<T,2> &matrix)
{
	int i, j, m, n;
	T x, y, temp;
	
	n = matrix.height();
	for (m=1; m<n-1; m++) {
		x = 0.0;
		i = m;
		for (j=m; j<n; j++) {
			if (fabs(matrix(m-1,j)) > fabs(x)) {
				x = matrix(m-1,j);
				i = j;
			}
		}
		if (i != m) {
			for (j = m-1; j<n; j++) {
				temp = matrix(j,i);
				matrix(j,i) = matrix(j,m);
				matrix(j,m) = temp;
			}
			for (j=0; j<n; j++) {
				temp = matrix(i,j);
				matrix(i,j) = matrix(m,j);
				matrix(m,j) = temp;
			}
		}
		if (x != 0.0) {
			for (i=m+1; i<n; i++) {
				y = matrix(m-1,i);
				if (y != 0.0) {
					y /= x;
					matrix(m-1,i) = y;
					for (j=m; j<n; j++)
						matrix(j,i) -= y*matrix(j,m);
					for (j=0; j<n; j++)
						matrix(m,j) += y*matrix(i,j);
				}
			}
		}
	}
}

// calculates the eigenvalues of a Hessenberg matrix
template <class T>
	void scopira::basekit::eigen_hessenberg(narray<T,2> &matrix, narray<T> &eigen_real, narray<T> &eigen_imaginary)
{
	int n,nn,m,l,k,mmin,i, j, iterations;
	T matrix_norm, z,y,x,w,v,u,t,s,r,q,p;;
	
	n = matrix.height();
	matrix_norm = fabs(matrix(0,0));
	for (i=1; i<n; i++) {
		for (j=i-1; j<n; j++) {
			matrix_norm += fabs(matrix(j,i));
		}
	}
	nn = n-1;
	t = 0.0;
	while (nn >= 0) {
		iterations = 0;
		do {
			for (l=nn;l>=1; l--) {
				s = fabs(matrix(l-1,l-1)) + fabs(matrix(l,l));
				if (s == 0.0)
					s = matrix_norm;
				if ((fabs(matrix(l-1,l)) + s) == s)
					break;
			}
			x = matrix(nn,nn);
			if (l == nn) {
				eigen_real[nn] = x + t;
				eigen_imaginary[nn--] = 0.0;
			} else {
				y = matrix(nn-1,nn-1);
				w = matrix(nn-1,nn)*matrix(nn,nn-1);
				if (l == (nn-1)) {
					p = 0.5*(y-x);
					q = p*p + w;
					z = sqrt(fabs(q));
					x += t;
					if (q >= 0.0) {
						z = p + SIGN(z,p);
						eigen_real[nn-1] = eigen_real[nn] = x + z;
						if (z != 0.0)
							eigen_real[nn] = x - w/z;
						eigen_imaginary[nn-1] = eigen_imaginary[nn] = 0.0;
					} else {
						eigen_real[nn-1] = eigen_real[nn] = x + p;
						eigen_imaginary[nn-1] = -(eigen_imaginary[nn] = z);
					}
					nn -= 2;
				} else {
					if (iterations == 30) {
						OUTPUT << "Too many iterations while computing Hessenberg eigenvalues\n";
						return;
					}
					if (iterations == 10 || iterations == 20) {
						t += x;
						for (i=0; i <= nn; i++)
							matrix(i,i) -= x;
						s = fabs(matrix(nn-1,nn)) + fabs(matrix(nn-2,nn-1));
						y = x = 0.75*s;
						w = -0.4375*s*s;
					}
					++iterations;
					for (m=nn-2; m>=l; m--) {
						z = matrix(m,m);
						r = x - z;
						s = y - z;
						p = (r*s-w)/matrix(m,m+1) + matrix(m+1,m);
						q = matrix(m+1,m+1) - z - r - s;
						r = matrix(m+1,m+2);
						s = fabs(p) + fabs(q) + fabs(r);
						p /= s;
						q /= s;
						r /= s;
						if (m == l)
							break;
						u = fabs(matrix(m-1,m)) *  (fabs(q) + fabs(r));
						v = fabs(p) * (fabs(matrix(m-1,m-1)) + fabs(z) + fabs(matrix(m+1,m+1)));
						if ((u + v) == v)
							break;
					}
					for (i=m+2; i<=nn; i++) {
						matrix(i-2,i) = 0.0;
						if (i != (m+2))
							matrix(i-3,i) = 0.0;
					}
					for (k=m; k<=nn-1; k++) {
						if (k != m) {
							p = matrix(k-1,k);
							q = matrix(k-1,k+1);
							r = 0.0;
							if (k != nn-1)
								r = matrix(k-1,k+2);
							if ((x = fabs(p) + fabs(q) + fabs(r)) != 0.0) {
								p /= x;
								q /=x;
								r /= x;
							}
						}
						if ((s = SIGN(sqrt(p*p + q*q + r*r),p)) != 0.0) {
							if (k == m) {
								if (l != m) 
									matrix(k-1,k) = -matrix(k-1,k);
							} else {
								matrix(k-1,k) = -s*x;
							}
							p += s;
							x = p/s;
							y = q/s;
							z = r/s;
							q /= p;
							r /= p;
							for (j=k; j<=nn; j++) {
								p = matrix(j,k) + q*matrix(j,k+1);
								if (k != nn-1) {
									p += r*matrix(j,k+2);
									matrix(j,k+2) -= p*z;
								}
								matrix(j,k+1) -= p*y;
								matrix(j,k) -= p*x;
							}
							mmin = nn < k+3 ? nn : k+3;
							for (i=l; i<=mmin; i++) {
								p = x*matrix(k,i) + y*matrix(k+1,i);
								if (k != nn-1) {
									p += z*matrix(k+2,i);
									matrix(k+2,i) -= p*r;
								}
								matrix(k+1,i) -= p*q;
								matrix(k,i) -= p;
							}
						}
					}
				}
			}
		} while (l < nn-1);
	}
}

// Gauss-Jordan matrix inversion and linear equation solution
template <class T>
	void scopira::basekit::gauss_jordan(narray<T,2> &matrix, narray<T,2> &rhs)
{
	narray<int> index_x, index_y, index_pivot;
	int i, x, y, j, k, l, ll, n, m;
	T max, pivot_inv, temp;
	
	n = matrix.height();
	m = rhs.width();
	index_x.resize(n);
	index_y.resize(n);
	index_pivot.resize(n);
	for (j=0; j<n; j++) 
		index_pivot[j] = 0;
	for (i=0; i<n; i++) {
		max = 0.0;
		for (j=0; j<n; j++) {
			if (index_pivot[j] != 1) {
				for (k=0; k<n; k++) {
					if (index_pivot[k] == 0) {
						if (fabs(matrix(k,j)) >= max) {
							max = fabs(matrix(k,j));
							x = k;
							y = j;
						}
					} else if (index_pivot[k] > 1) {
						OUTPUT << "Singular matrix-1 in Gauss-Jordan elimination\n";
						return;
					}
				}
			}
		}
		index_pivot[x]++;
		if (x != y) {
			for (l=0; l<n; l++) {
				temp = matrix(l,y);
				matrix(l,y) = matrix(l,x);
				matrix(l,x) = temp;
			}
			for (l=0; l<m; l++) {
				temp = rhs(l,y);
				rhs(l,y) = rhs(l,x);
				rhs(l,x) = temp;
			}
		}
		
		index_y[i] = y;
		index_x[i] = x;
		if (matrix(x,x) == 0.0) {
			OUTPUT << "Singular matrix-1 in Gauss-Jordan elimination\n";
			return;
		}
		pivot_inv = 1.0/matrix(x,x);
		matrix(x,x) = 1.0;
		for (l=0; l<n; l++)
			matrix(l,x) *= pivot_inv;
		for (l=0; l<m; l++)
			rhs(l,x) *= pivot_inv;
		for (ll=0; ll<n; ll++) {
			if (ll != x) {
				temp = matrix(x,ll);
				matrix(x,ll) = 0.0;
				for (l=0; l<n; l++)
					matrix(l,ll) -= matrix(l,x)*temp;
				for (l=0; l<m; l++) 
					rhs(l,ll) -= rhs(l,x)*temp;
			}
		}
	}
	for (l=n-1; l>=0; l--) {
		if (index_y[l] != index_x[l]) {
			for (k=0; k<n; k++) {
				temp = matrix(index_y[l],k);
				matrix(index_y[l],k) = matrix(index_x[l],k);
				matrix(index_x[l],k) = temp;
			}
		}
	}	
}

// rearranges covariance matrix
template <class T>
	void scopira::basekit::sort_covariance(narray<T,2> &matrix, narray<int> &fitted, int mfit)
{
	int i,j,k,n;
	T temp;
	
	n = matrix.height();
	for (i=mfit; i<n; i++) {
		for (j=0; j<=i; j++) {
			matrix(j,i) = matrix(i,j) = 0.0;
		}
	}
	k = mfit-1;
	for (j=n-1; j>=0; j--) {
		if (fitted[j]) {
			for (i=0; i<n; i++) {
				temp = matrix(k,i);
				matrix(k,i) = matrix(j,i);
				matrix(j,i) = temp;
			}
			for (i=0; i<n; i++) {
				temp = matrix(i,k);
				matrix(i,k) = matrix(i,j);
				matrix(i,j) = temp;
			}
			k--;
		}
	}
}

#endif
