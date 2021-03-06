
/*
 *  Copyright (c) 2002-2005    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#ifndef __INCLUDED_PAKIT_MATH_H__
#define __INCLUDED_PAKIT_MATH_H__

#include <scopira/basekit/math.h>
#include <scopira/basekit/narray.h>
#include <scopira/basekit/matrixmath.h>
#include <scopira/basekit/vectormath.h>

namespace pakit
{
  //calculate the mean matrix dest of the source matrix src
  //using both tranining and classes
  template <class T>
    void mean(scopira::basekit::narray<T,2> &dest,
      const scopira::basekit::narray<T,2> &src,
      const scopira::basekit::narray<bool> *src_training,
      const scopira::basekit::narray<int> *src_classes,
      int numclass);
  //calculate the mean matrix dest of the source matrix src
  //using classes only (no training)
  template <class T>
    void mean(scopira::basekit::narray<T,2> &dest,
      const scopira::basekit::narray<T,2> &src,
      const scopira::basekit::narray<int> *src_classes,
      int numclass );
  //calculate the mean matrix dest of the source matrix src for one class
  template <class T>
    void mean(scopira::basekit::narray<T,2> &dest,
    const scopira::basekit::narray<T,2> &src);
  //calculate pooled covariance matrix dest of matrix src using both training and classes
  //dest=1/(n1+n2+....+nN-N) * (R1 + R2 +...+RN)
  template <class T>
    void calc_pooled(scopira::basekit::narray<T,2>& dest,
      scopira::basekit::narray<T,2>& m_mean,
      const scopira::basekit::narray<T,2>& src,
      const scopira::basekit::narray<bool> *src_training,
      const scopira::basekit::narray<int> *src_classes,
      int numclass);
  //calculate pooled covariance matrix dest of matrix src using classes only (no training)
  //dest=1/(n1+n2+....+nN-N) * (R1 + R2 +...+RN)
  template <class T>
    void calc_pooled(scopira::basekit::narray<T,2>& dest,
      scopira::basekit::narray<T,2>& m_mean,
      const scopira::basekit::narray<T,2>& src,
      const scopira::basekit::narray<int> *src_classes,
      int numclass);
  //calculate covariance matrices dest of matrix src for each class using both training and classes
  //dest[c]=1/(nc-1) * (Rc)
  template <class T>
     void calc_covar(scopira::basekit::narray<T,2> **dest,
      scopira::basekit::narray<T,2>& m_mean,
      const scopira::basekit::narray<T,2>& src,
      const scopira::basekit::narray<bool> *src_training,
      const scopira::basekit::narray<int> *src_classes,
      int numclass);
  //calculate covariance matrices dest of matrix src for each class using classes only (no training)
  //dest[c]=1/(nc-1) * (Rc)
  template <class T>
    void calc_covar(scopira::basekit::narray<T,2> **dest,
      scopira::basekit::narray<T,2>& m_mean,
      const scopira::basekit::narray<T,2>& src,
      const scopira::basekit::narray<int> *src_classes,
      int numclass);
  //calculate covariance matrix dest of matrix src for one particular class using both training and classes
  //dest=1/(n-1) * (Rc)
  template <class T>
     void calc_covar(scopira::basekit::narray<T,2> &dest,
      scopira::basekit::narray<T,2>& m_mean,
      const scopira::basekit::narray<T,2>& src,
      const scopira::basekit::narray<bool> *src_training,
      const scopira::basekit::narray<int> *src_classes,
      int the_class);
  //calculate covariance matrix dest of matrix src for one particular class using classes only (no training)
  //dest=1/(n-1) * (Rc)
  template <class T>
    void calc_covar(scopira::basekit::narray<T,2> &dest,
      scopira::basekit::narray<T,2>& m_mean,
      const scopira::basekit::narray<T,2>& src,
      const scopira::basekit::narray<int> *src_classes,
      int the_class);
  //calculate Anderson-Bahadur covariance matrix dest of matrix src for two class data set using both training and classes
  //dest= ( 1 - weight ) * Sigma_1 + weight * Sigma_2
  template <class T>
     void calc_ander_baha_covar(scopira::basekit::narray<T,2> &dest,
      scopira::basekit::narray<T,2>& m_mean,
      const scopira::basekit::narray<T,2>& src,
      const scopira::basekit::narray<bool> *src_training,
      const scopira::basekit::narray<int> *src_classes,
      double class_weight);
  //calculate Anderson-Bahadur covariance matrix dest of matrix src for two class data set using classes only (no training)
  //dest= ( 1 - weight ) * Sigma_1 + weight * Sigma_2
  template <class T>
    void calc_ander_baha_covar(scopira::basekit::narray<T,2> &dest,
      scopira::basekit::narray<T,2>& m_mean,
      const scopira::basekit::narray<T,2>& src,
      const scopira::basekit::narray<int> *src_classes,
      double class_weight);
  //calculate Chernoff covariance matrix dest, and additional logterm,
  //of matrix src for two class data set using both training and classes
  //dest= ( 1 - weight ) * Sigma_1 + weight * Sigma_2
  //logterm=.5 log( det( (1 - weight) * Sigma_1 + weight * Sigma2 ) /
  //                ( det(Sigma_1)^(1 - weight) * det(Sigma_2)^weight ) )  
  template <class T>
     void calc_chernoff_covar(scopira::basekit::narray<T,2> &dest,
      double *logterm,
      scopira::basekit::narray<T,2>& m_mean,
      const scopira::basekit::narray<T,2>& src,
      const scopira::basekit::narray<bool> *src_training,
      const scopira::basekit::narray<int> *src_classes,
      double weight);
  //calculate Chernoff covariance matrix dest, and additional logterm,
  //of matrix src for two class data set using classes only (no training)
  //dest= ( 1 - weight ) * Sigma_1 + weight * Sigma_2
  //logterm=.5 log( det( (1 - weight) * Sigma_1 + weight * Sigma2 ) /
  //                ( det(Sigma_1)^(1 - weight) * det(Sigma_2)^weight ) )  
  template <class T>
    void calc_chernoff_covar(scopira::basekit::narray<T,2> &dest,
      double *logterm,
      scopira::basekit::narray<T,2>& m_mean,
      const scopira::basekit::narray<T,2>& src,
      const scopira::basekit::narray<int> *src_classes,
      double weight);
  //calculate Symmetric KL Divergence covariance matrices dest1 and dest2, and additional traceterm,
  //of matrix src for two class data set using both training and classes
  //dest1 = Sigma_1
  //dest2 = Sigma_2
  //traceterm = .5 tr(inv(Sigma_1)*Sigma_2 + (inv(Sigma_2)*Sigma_1) - 2*Id)
  template <class T>
     void calc_sym_kl_divergence_covar(scopira::basekit::narray<T,2> &dest1,
      scopira::basekit::narray<T,2> &dest2,
      double *traceterm,
      scopira::basekit::narray<T,2>& m_mean,
      const scopira::basekit::narray<T,2>& src,
      const scopira::basekit::narray<bool> *src_training,
      const scopira::basekit::narray<int> *src_classes);
  //calculate Symmetric KL Divergence covariance matrices dest1 and dest2, and additional traceterm,
  //of matrix src for two class data set using classes only (no training)
  //dest1 = Sigma_1
  //dest2 = Sigma_2
  //traceterm = .5 tr(inv(Sigma_1)*Sigma_2 + (inv(Sigma_2)*Sigma_1) - 2*Id)
  template <class T>
    void calc_sym_kl_divergence_covar(scopira::basekit::narray<T,2> &dest1,
      scopira::basekit::narray<T,2> &dest2,
      double *traceterm,
      scopira::basekit::narray<T,2>& m_mean,
      const scopira::basekit::narray<T,2>& src,
      const scopira::basekit::narray<int> *src_classes);
  //calculate correlation matrix
  template <class T>
    void calc_correlation(scopira::basekit::narray<T,2>& dest,
      scopira::basekit::narray<T,2>& m_mean,
      const scopira::basekit::narray<T,2>& src,
      const scopira::basekit::narray<int> *src_classes,
      int numclass);
}

//calculate the mean matrix dest of the source matrix src
//using both tranining and classes
template <class T>
  void pakit::mean(scopira::basekit::narray<T,2> &dest,
    const scopira::basekit::narray<T,2> &src,
    const scopira::basekit::narray<bool> *src_training,
    const scopira::basekit::narray<int> *src_classes,
    int numclass)
{
  typedef T data_type;
  data_type sum;
  int x,y,i, numfeat,numpat,count;

  numfeat=src.width();
  numpat=src.height();

  dest.resize(numfeat, numclass);
  dest.clear();

  //calculate for each class
  for (y=0; y<numclass; y++) {
    //iterate over all the features
    for (x=0; x<numfeat; x++) {
      sum = 0;
      count = 0;
      //iterate over all patterns
      for (i=0; i<numpat; i++)
        //if training and src element is in the current class, add it to the sum
        if((!src_training ||(src_training && (*src_training)[i]))
          && ((*src_classes)[i] == (y+1))){
          sum += src(x,i);
          count++;
        }
      //mean for current class
      dest(x,y) = sum/count;
    }
  }
}

//calculate the mean matrix dest of the source matrix src
//using classes only (no training)
template <class T>
  void pakit::mean(scopira::basekit::narray<T,2> &dest,
    const scopira::basekit::narray<T,2> &src,
    const scopira::basekit::narray<int> *src_classes,
    int numclass )
{
  pakit::mean(dest,src,0,src_classes,numclass);
}


//calculate the mean matrix dest of the source matrix src for one class
template <class T>
  void pakit::mean(scopira::basekit::narray<T,2> &dest,
    const scopira::basekit::narray<T,2> &src)
{
  pakit::mean(dest,src,0,0, 1);
}


//calculate pooled covariance matrix dest of matrix src using both training and classes
//dest=1/(n1+n2+....+nN-N) * (R1 + R2 +...+RN)
template <class T>
  void pakit::calc_pooled(scopira::basekit::narray<T,2>& dest,
    scopira::basekit::narray<T,2>& m_mean,
    const scopira::basekit::narray<T,2>& src,
    const scopira::basekit::narray<bool> *src_training,
    const scopira::basekit::narray<int> *src_classes,
    int numclass)
{
  typedef scopira::basekit::narray<T,2> data_matrix;
  typedef T data_type;
  size_t i;
  data_type sum;
  scopira::basekit::narray<int> classcount;
  data_matrix G, H;
  int numfeat,numpat;

  numfeat=src.width();
  numpat=src.height();

  dest.resize(numfeat, numfeat);
  dest.clear();

  classcount.resize(numclass);
  classcount.clear();

  // iterate over C1..CN
  for (i=0; i<numpat; i++)
    if(!src_training || (src_training && (*src_training)[i])){
      G.copy(src.xyslice(0, i, numfeat, 1));
      scopira::basekit::sub_vector(G, m_mean.xyslice(0, (*src_classes)[i]-1, numfeat, 1));
      scopira::basekit::mul_matrix(H, G, G, true, false);
      scopira::basekit::add_vector(dest, H);
      classcount[(*src_classes)[i]-1]++;
    }

  // calc 1/(n1+n2+...+nN - N) * sigme(Rk)
  sum = 0.0;
  for (i=0; i<numclass; i++) {
    if ( classcount[i] > 0 ) {
      sum += classcount[i] - 1.0;
    }
  }
  sum = 1/sum;
  scopira::basekit::mul_matrix(dest, sum);
}


//calculate pooled covariance matrix dest of matrix src using classes only (no training)
//dest=1/(n1+n2+....+nN-N) * (R1 + R2 +...+RN)
template <class T>
  void pakit::calc_pooled(scopira::basekit::narray<T,2>& dest,
    scopira::basekit::narray<T,2>& m_mean,
    const scopira::basekit::narray<T,2>& src,
    const scopira::basekit::narray<int> *src_classes,
    int numclass)
{
  pakit::calc_pooled(dest,m_mean,src,0,src_classes,numclass);
}


//calculate covariance matrices dest of matrix src for each class using both training and classes
//dest[c]=1/(nc-1) * (Rc)
template <class T>
  void pakit::calc_covar(scopira::basekit::narray<T,2> **dest,
    scopira::basekit::narray<T,2>& m_mean,
    const scopira::basekit::narray<T,2>& src,
    const scopira::basekit::narray<bool> *src_training,
    const scopira::basekit::narray<int> *src_classes,
    int numclass)
{
  typedef scopira::basekit::narray<T,2> data_matrix;
  size_t i;
  int numfeat,numpat,csidx;
  scopira::basekit::narray<int> classcount;
  data_matrix G, H;
  double denom;

  numfeat=src.width();
  numpat=src.height();

  for (csidx = 0; csidx < numclass; csidx++ ) {
    dest[csidx]->resize(numfeat, numfeat);
    dest[csidx]->clear();
  }

  classcount.resize(numclass);
  classcount.clear();

  // iterate over patterns
  for (i=0; i<numpat; i++)
    if(!src_training || (src_training && (*src_training)[i])){
      G.copy(src.xyslice(0, i, numfeat, 1));
      scopira::basekit::sub_vector(G, m_mean.xyslice(0, (*src_classes)[i]-1, numfeat, 1));
      scopira::basekit::mul_matrix(H, G, G, true, false);
      scopira::basekit::add_vector(*(dest[(*src_classes)[i]-1]), H);
      classcount[(*src_classes)[i]-1]++;
    }

  // calc 1/(nc - 1) * sigme(Rk)
  for (csidx = 0; csidx < numclass; csidx++ ){
    denom = classcount[csidx]- 1;
    scopira::basekit::mul_matrix(*(dest[csidx]), 1/denom);
  }
}


//calculate covariance matrices dest of matrix src for each class using classes only (no training)
//dest[c]=1/(nc-1) * (Rc)
template <class T>
  void pakit::calc_covar(scopira::basekit::narray<T,2> **dest,
    scopira::basekit::narray<T,2>& m_mean,
    const scopira::basekit::narray<T,2>& src,
    const scopira::basekit::narray<int> *src_classes,
    int numclass)
{
  pakit::calc_covar(dest,m_mean,src,0, src_classes,numclass);
}


//calculate covariance matrix dest of matrix src for one particular class using both training and classes
//dest=1/(n-1) * (Rc)
template <class T>
  void pakit::calc_covar(scopira::basekit::narray<T,2>& dest,
    scopira::basekit::narray<T,2>& m_mean,
    const scopira::basekit::narray<T,2>& src,
    const scopira::basekit::narray<bool> *src_training,
    const scopira::basekit::narray<int> *src_classes,
    int the_class)
{
  typedef scopira::basekit::narray<T,2> data_matrix;
  size_t i;
  int numfeat,numpat;
  int classcount;
  data_matrix G, H;
  double denom;

  numfeat=src.width();
  numpat=src.height();

  dest.resize(numfeat,numfeat);
  dest.clear();

  classcount = 0;

  // iterate over patterns
  for (i=0; i<numpat; i++) {
    if ( ((*src_classes)[i]) == the_class ) {
      if(!src_training || (src_training && (*src_training)[i])){
        G.copy(src.xyslice(0, i, numfeat, 1));
        scopira::basekit::sub_vector(G, m_mean.xyslice(0, (*src_classes)[i]-1, numfeat, 1));
        scopira::basekit::mul_matrix(H, G, G, true, false);
        scopira::basekit::add_vector(dest, H);
        classcount++;
      }
    }
  }
  // Normalize by 1/(n - 1)
  denom = classcount- 1;
  scopira::basekit::mul_matrix(dest, 1/denom);
}

//calculate covariance matrices dest of matrix src for one particular class using classes only (no training)
//dest=1/(n-1) * (Rc)
template <class T>
  void pakit::calc_covar(scopira::basekit::narray<T,2>& dest,
    scopira::basekit::narray<T,2>& m_mean,
    const scopira::basekit::narray<T,2>& src,
    const scopira::basekit::narray<int> *src_classes,
    int the_class)
{
  pakit::calc_covar(dest,m_mean,src,0, src_classes,the_class);
}


//calculate Anderson-Bahadur covariance matrix dest of matrix src for two class data set using both training and classes
//dest[c]= ( 1 - weight ) * Sigma_1 + weight * Sigma_2
template <class T>
  void pakit::calc_ander_baha_covar(scopira::basekit::narray<T,2> &dest,
    scopira::basekit::narray<T,2>& m_mean,
    const scopira::basekit::narray<T,2>& src,
    const scopira::basekit::narray<bool> *src_training,
    const scopira::basekit::narray<int> *src_classes,
    double class_weight)
{
  typedef scopira::basekit::narray<T,2> data_matrix;
  int numfeat;
  scopira::basekit::narray<int> classcount;
  data_matrix G, H;
  data_matrix covar1, covar2;

  numfeat=src.width();

  covar1.resize(numfeat, numfeat);
  covar1.clear();
  covar2.resize(numfeat, numfeat);
  covar2.clear();
  dest.resize(numfeat, numfeat);
  dest.clear();

  classcount.resize(2);
  classcount.clear();

  pakit::calc_covar( covar1, m_mean, src, src_training, src_classes, 1 );
  pakit::calc_covar( covar2, m_mean, src, src_training, src_classes, 2 );
  scopira::basekit::mul_matrix( covar1, 1.0 - class_weight );
  scopira::basekit::mul_matrix( covar2, class_weight );
  scopira::basekit::add_vector( dest, covar1 );
  scopira::basekit::add_vector( dest, covar2 );
}


//calculate Anderson-Bahadur covariance matrix dest of matrix src for two class data set using classes only (no training)
//dest= ( 1 - weight ) * Sigma_1 + weight * Sigma_2
template <class T>
  void pakit::calc_ander_baha_covar(scopira::basekit::narray<T,2> &dest,
    scopira::basekit::narray<T,2>& m_mean,
    const scopira::basekit::narray<T,2>& src,
    const scopira::basekit::narray<int> *src_classes,
    double class_weight)
{
  pakit::calc_ander_baha_covar(dest,m_mean,src,0,src_classes,class_weight);
}
//calculate Chernoff covariance matrix dest, and additional logterm,
//of matrix src for two class data set using both training and classes
//dest= ( 1 - weight ) * Sigma_1 + weight * Sigma_2
//logterm=.5 log( det( ( 1 - weight ) * Sigma_1 + weight * Sigma_2 ) /
//                ( det(Sigma_1)^(1-weight) * det(Sigma_2)^(weight) )  
template <class T>
  void pakit::calc_chernoff_covar(scopira::basekit::narray<T,2> &dest,
  double *logterm,
  scopira::basekit::narray<T,2>& m_mean,
  const scopira::basekit::narray<T,2>& src,
  const scopira::basekit::narray<bool> *src_training,
  const scopira::basekit::narray<int> *src_classes,
  double class_weight)
{
  typedef scopira::basekit::narray<T,2> data_matrix;
  int numfeat;
  scopira::basekit::narray<int> classcount;
  scopira::basekit::narray<int> vidx;
  data_matrix decomp, numerator_matrix;
  data_matrix G, H;
  data_matrix covar1, covar2;
  double numerator_determ, denom_determ1, denom_determ2;
  bool odd;

  numfeat=src.width();

  covar1.resize(numfeat, numfeat);
  covar1.clear();
  covar2.resize(numfeat, numfeat);
  covar2.clear();
  dest.resize(numfeat, numfeat);
  dest.clear();
  decomp.resize(numfeat, numfeat);
  decomp.clear();
  numerator_matrix.resize(numfeat, numfeat);
  numerator_matrix.clear();
  vidx.resize(numfeat);
  vidx.clear();

  classcount.resize(2);
  classcount.clear();

  pakit::calc_covar( covar1, m_mean, src, src_training, src_classes, 1 );
  pakit::calc_covar( covar2, m_mean, src, src_training, src_classes, 2 );
  scopira::basekit::mul_matrix( covar1, 1.0 - class_weight );
  scopira::basekit::mul_matrix( covar2, class_weight );
  scopira::basekit::add_vector( dest, covar1 );
  scopira::basekit::add_vector( dest, covar2 );

  // calc the log term
  G.copy( covar1 );
  scopira::basekit::mul_matrix( G, 1.0 - class_weight );
  H.copy( covar2 );
  scopira::basekit::mul_matrix( H, class_weight );
  for ( int aidx1 = 0; aidx1 < numfeat; aidx1++ )
    for ( int aidx2 = 0; aidx2 < numfeat; aidx2++ )
      numerator_matrix(aidx1,aidx2) = G(aidx1,aidx2) + H(aidx1,aidx2);

  scopira::basekit::lu_decomposition<double>( decomp, numerator_matrix, vidx, odd );
  numerator_determ = 1.0;
  for ( int aidx = 0; aidx < numfeat; aidx++ )
    numerator_determ *= decomp(aidx,aidx);
  if ( odd )
    numerator_determ *= -1.0;

  scopira::basekit::lu_decomposition<double>( decomp, covar1, vidx, odd );
  denom_determ1 = 1.0;
  for ( int aidx = 0; aidx < numfeat; aidx++ ) {
    denom_determ1 *= decomp(aidx,aidx);
  }
  if ( odd )
    denom_determ1 *= -1.0;

  scopira::basekit::lu_decomposition<double>( decomp, covar2, vidx, odd );
  denom_determ2 = 1.0;
  for ( int aidx = 0; aidx < numfeat; aidx++ ) {
    denom_determ2 *= decomp(aidx,aidx);
  }
  if ( odd )
    denom_determ2 *= -1.0;

  *logterm = 0.5 * log( numerator_determ / 
                        ( pow( denom_determ1, 1.0 - class_weight ) * pow( denom_determ2, class_weight ) ) );
}

//calculate Chernoff covariance matrix dest, and additional logterm,
//of matrix src for two class data set using classes only (no training)
//dest= ( 1 - weight ) * Sigma_1 + weight * Sigma_2
//logterm=.5 log( det( ( 1 - weight ) * Sigma_1 + weight * Sigma_2 ) /
//                ( det(Sigma_1)^(1-weight) * det(Sigma_2)^(weight) )  
template <class T>
  void pakit::calc_chernoff_covar(scopira::basekit::narray<T,2> &dest,
    double *logterm,
    scopira::basekit::narray<T,2>& m_mean,
    const scopira::basekit::narray<T,2>& src,
    const scopira::basekit::narray<int> *src_classes,
    double weight)
{
  pakit::calc_chernoff_covar(dest,logterm,m_mean,src,0,src_classes,weight);
}

//calculate Symmetric KL Divergence covariance matrices dest1 and dest2, and additional traceterm,
//of matrix src for two class data set using both training and classes
//dest1 = Sigma_1
//dest2 = Sigma_2
//traceterm = .5 tr(inv(Sigma_1)*Sigma_2 + (inv(Sigma_2)*Sigma_1) - 2*Id)
template <class T>
  void pakit::calc_sym_kl_divergence_covar(scopira::basekit::narray<T,2> &dest1,
  scopira::basekit::narray<T,2> &dest2,
  double *traceterm,
  scopira::basekit::narray<T,2>& m_mean,
  const scopira::basekit::narray<T,2>& src,
  const scopira::basekit::narray<bool> *src_training,
  const scopira::basekit::narray<int> *src_classes)
{
  typedef scopira::basekit::narray<T,2> data_matrix;
  int numfeat;
  scopira::basekit::narray<int> classcount;
  scopira::basekit::narray<int> vidx;
  data_matrix covar1_inv, covar2_inv, sumterm1, sumterm2;

  numfeat=src.width();

  dest1.resize(numfeat, numfeat);
  dest1.clear();
  dest2.resize(numfeat, numfeat);
  dest2.clear();
  covar1_inv.resize(numfeat,numfeat);
  covar1_inv.clear();
  covar2_inv.resize(numfeat,numfeat);
  covar2_inv.clear();
  sumterm1.resize(numfeat, numfeat);
  sumterm1.clear();
  sumterm2.resize(numfeat, numfeat);
  sumterm2.clear();

  classcount.resize(2);
  classcount.clear();

  pakit::calc_covar( dest1, m_mean, src, src_training, src_classes, 1 );
  pakit::calc_covar( dest2, m_mean, src, src_training, src_classes, 2 );

  // calc the trace term
  scopira::basekit::invert_matrix(covar1_inv, dest1);
  scopira::basekit::invert_matrix(covar2_inv, dest2);
  scopira::basekit::mul_matrix(sumterm1,covar1_inv,dest2,false,false);
  scopira::basekit::mul_matrix(sumterm2,dest1,covar2_inv,false,false);
  *traceterm = 0.0;
  for ( int aidx = 0; aidx < numfeat; aidx++ )
    *traceterm += sumterm1(aidx,aidx) + sumterm2(aidx,aidx) - 2.0;
  *traceterm *= 0.5;
}

//calculate Symmetric KL Divergence covariance matrices dest1 and dest2, and additional traceterm,
//of matrix src for two class data set using classes only (no training)
//dest1 = Sigma_1
//dest2 = Sigma_2
//traceterm = .5 tr(inv(Sigma_1)*Sigma_2 + (inv(Sigma_2)*Sigma_1) - 2*Id)
template <class T>
  void pakit::calc_sym_kl_divergence_covar(scopira::basekit::narray<T,2> &dest1,
  scopira::basekit::narray<T,2> &dest2,
  double *traceterm,
  scopira::basekit::narray<T,2>& m_mean,
  const scopira::basekit::narray<T,2>& src,
  const scopira::basekit::narray<int> *src_classes)
{
  pakit::calc_sym_kl_divergence_covar(dest1,dest2,traceterm,m_mean,src,0,src_classes);
}

//calculate correlation matrix
template <class T>
  void pakit::calc_correlation(scopira::basekit::narray<T,2>& dest,
    scopira::basekit::narray<T,2>& m_mean,
    const scopira::basekit::narray<T,2>& src,
    const scopira::basekit::narray<int>* src_classes,
    int numclass){

  typedef scopira::basekit::narray<T,2> data_matrix;
  int x, numfeat;
  data_matrix m_covariance,m_std;

  //calculate the pooled covariance matrix
  pakit::calc_pooled(m_covariance,m_mean,src,src_classes,numclass);

  numfeat=src.width();

  m_std.resize(numfeat,numfeat);
  m_std.clear();

  dest.resize(numfeat,numfeat);
  dest.clear();

  //calculate the inverse standard deviation matrix
  //get orignal vector of covariance matrix and for each of the elements S
  //calculate sqrt(s)
  for (x=0; x< numfeat; x++)
    m_std(x,x) = 1/sqrt(m_covariance(x,x));

  scopira::basekit::mul_matrix(dest,m_std,m_covariance);
  scopira::basekit::mul_matrix(dest,dest,m_std,false,true);
}
#endif
