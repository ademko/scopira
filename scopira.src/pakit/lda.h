
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

#ifndef __INCLUDED_PAKIT_LDA_H__
#define __INCLUDED_PAKIT_LDA_H__

#include <scopira/basekit/math.h>
#include <scopira/tool/output.h>
#include <scopira/basekit/narray.h>
#include <scopira/basekit/vectormath.h> // RV
#include <scopira/basekit/matrixmath.h> // RV

#include <pakit/math.h>

namespace pakit
{
  template <class T> class lda_alg;

}

/**
 * Generic LDA implementation.
 *
 * @author Aleksander Demko
 * @author Marina Mandelzweig
 * @author Rodrigo Vivanco
 */
template <class T> class pakit::lda_alg
{
  public:
    typedef T data_type;
    typedef scopira::basekit::narray<T> data_vec;
    typedef scopira::basekit::narray<T,2> data_matrix;
    typedef scopira::basekit::nslice<T,2> slice_data_matrix;

    //input features: features X patterns
    const data_matrix *i_features;
    //input classes: patterns
    const scopira::basekit::narray<int> *i_classes;
    //input training: patterns
    const scopira::basekit::narray<bool> *i_training;

    //the output propabilities: classes X patterns
    data_matrix *o_prob;
    //LDA coefficients for testing
    data_matrix *o_cof;

    //number of claases
    int numclass;
    //number of patterns
    int numpat;
    //number of features
    int numfeat;
    //weight to give to class 1 when computing intercept of separation line
    //in two class problem (default 0.5, 1 = go through class 1 centroid,
    //0 = go through class 2 centroid)
    double class1_weight;
    //rotation to apply to lda separation line in two class problem.
    //(default = 0.0, converted to 1.0 in function)
    double rotation;
    
    //the number of patterns in a particular class
    scopira::basekit::narray<int> class_count;
    //the mean matrix: feature X classes
    data_matrix m_mean;
    //the pooled covariance matrix: features X features
    data_matrix m_pooled;
    //the LDA alpha coerficiants: features + 1 X classes
    data_matrix m_alpha;

    //was the matrix singular?
    bool singular;
    //are we in minimode
    bool mini_mode;
    //are we training
    bool training;

  public:
    //ctor
    lda_alg(void);

    //call this first - inits data for learn mode
    const char * init_learn(const data_matrix *in_feat, const scopira::basekit::narray<int> *in_cls);
    //call this next - inits data for learn mode with wight and train
    const char * init_learn(const data_matrix *in_feat, const scopira::basekit::narray<int> *in_cls,
      const scopira::basekit::narray<bool> *in_train,double cl1_weight,double rotation);
    
    //set training
    const char * set_training(const scopira::basekit::narray<bool> *in_traing);

    //set the output targets
    void init_output(data_matrix *out_prob, data_matrix *out_cof);
    //initiate training
    void train(void);
    //was there a singular matrix after the training?
    bool is_singular(void) const { return singular; }
    //do the testing
    void test(void);

    double get_percent_correct() { return dm_percentCorrect; }
    void view_state(void);

  private:
    //calculate first alpha
    void calc_alpha0(void);
    //calculate the rest of the alpha
    void calc_alpha(void);
    //calculate first alpha (special case)
    void calc_mini_alpha0(void);
    //calculate the rest of the alpha (special case)
    void calc_mini_alpha(void);
    //rotate the LDA line (very special case)
    void rotate_lda_line(void);

    //calculate test set
    void calc_test(void);
    //calculate test set (special case)
    void calc_mini_test(void);
    //calculate the worst possible results
    void calc_wrong(void);
    // after test creates confusion matrix, calculate percent correct answers
    void calc_percent_correct();

  private:
    double dm_percentCorrect;
};

template <class T>
  pakit::lda_alg<T>::lda_alg(void)
  : i_features(0), i_classes(0), i_training(0), o_prob(0), o_cof(0),
    numclass(0), dm_percentCorrect(0.0)
{
  training = false;
  class1_weight = -1;
  rotation = 0;
}

//checks the values if theire suitable for a training run of lda.
//return value is null if ok, else a string literal describing the
//error. out_maxclass is the number of classes found
template <class T>
  const char * pakit::lda_alg<T>::init_learn(
  const data_matrix *in_feat, const scopira::basekit::narray<int> *in_cls)
{
  int minclass;

  if (!in_feat) 
    return "bad features\n";
  if (!in_cls) 
    return "bad class labels\n";

  if (in_feat->width() < 2 || in_feat->height() < 2)
    return "Feature matrix to small\n";
  if (in_feat->height() != in_cls->size())
    return "Class vector does not match feature matrix height\n";


  max(*in_cls, numclass);
  if (numclass < 2)
    return "Class vector must have at least two classes\n";
  min(*in_cls, minclass);
  if (minclass != 1)
    return "Class vector must have a minimum value of one\n";

  i_features = in_feat;
  i_classes = in_cls;

  numpat = i_features->height();
  numfeat = i_features->width();

  singular = false;
  mini_mode = numclass == 2;  

  return 0; //ok
}

//checks the values if theire suitable for a training run of lda.
//return value is null if ok, else a string literal describing the
//error. out_maxclass is the number of classes found
template <class T>
  const char * pakit::lda_alg<T>::init_learn(
  const data_matrix *in_feat, 
  const scopira::basekit::narray<int> *in_cls,
  const scopira::basekit::narray<bool> *in_train,
  double cl1_weight, double rot)
{
  int i, minclass, this_class;
  
  if (!in_feat) 
    return "bad features\n";
  if (!in_cls)  
    return "bad class labels\n";
  if (!in_train) 
    return "bad training\n";
  
  if (in_feat->width() < 2 || in_feat->height() < 2) 
    return "Feature matrix to small\n";
  if (in_feat->height() != in_cls->size()) 
      return "Class vector does not match feature matrix height\n";
  if (in_feat->height() != in_train->size())
    return "Training vector does not match feature matrix height\n";

  numclass = 0;
  minclass = 9999;
  for (i=0; i<in_train->size(); i++ ){
    this_class = in_cls->get(i);
    if ( in_train->get(i) == true ) {
      if ( this_class > numclass ) 
        numclass = this_class;
      if ( this_class < minclass ) 
        minclass = this_class;
    }
  }

  if (numclass < 2)
    return "Class vector must have at least two classes in training set\n";
  if (minclass != 1)
    return "Class vector must have a minimum value of one in training set\n";
  
  i_features = in_feat;
  i_classes = in_cls;

  numpat = i_features->height();
  numfeat = i_features->width();
  
  singular = false;
  mini_mode = numclass == 2;

  //set training
  i_training = in_train;
  training = true;

  class1_weight = cl1_weight;
  rotation = rot;
  return 0; //ok
}


//set training
template <class T>
  const char * pakit::lda_alg<T>::set_training(
    const scopira::basekit::narray<bool> *in_traing)
{
  if (i_features->height() != in_traing->size())
    return "Training vector does not match feature matrix height\n";

  assert(in_traing);
  i_training = in_traing;
  training = true;

  return 0;
}


template <class T>
  void pakit::lda_alg<T>::init_output(data_matrix *out_prob,
      data_matrix *out_cof)
{
  assert(i_features);

  o_prob = out_prob;
  o_cof = out_cof;

  o_prob->resize(numclass, i_features->height());

  if (training) {
    if (mini_mode)
      o_cof->resize(numfeat+1, 1);
    else
      o_cof->resize(numfeat+1, numclass);
  }
}


//LDA training
template <class T>
  void pakit::lda_alg<T>::train(void)
{
  int i;
  //count the number of patterns in each class
  class_count.resize(numclass);
  class_count.clear();

  for (i=0; i<numpat; ++i)
    if ((*i_training)[i])
      class_count[(*i_classes)[i]-1]++;

  //calculate the mean matrix
  pakit::mean(m_mean,*i_features,i_training,i_classes,numclass);

  //calculate the pooled covariance matrix
  pakit::calc_pooled(m_pooled,m_mean,*i_features,i_training,i_classes,numclass);

  //calculate inverse of pooled covariance matrix
  singular = !scopira::basekit::invert_matrix(m_pooled, m_pooled);
  if (singular)
    return; //bail now
  
  if (mini_mode) {
    m_alpha.resize(numfeat+1, 1);
    calc_mini_alpha0();
    calc_mini_alpha();
    rotate_lda_line();
  } 
  else {
    m_alpha.resize(numfeat+1, numclass);
    calc_alpha0();
    calc_alpha();
  }
  (*o_cof).copy(m_alpha);
}

//Make the test set
template <class T>
  void pakit::lda_alg<T>::test(void)
{
  //simple multiplexor
  if (singular)
    calc_wrong();
  else if (mini_mode) {
    calc_mini_test();
    calc_percent_correct();
  }
  else {
    calc_test();
    calc_percent_correct();
  }
}

template <class T>
  void pakit::lda_alg<T>::view_state(void)
{
  scopira::basekit::narray<double,2> features, oprob, ocof;
  scopira::basekit::narray<double,1> classes;
  scopira::basekit::narray<int,1> training;

  //matrix2narray(*i_features,features);
  //matrix2narray(*i_features,features);
  //matrix2narray(*i_features,features);

  //mat_descr(features, "features");
  //vec_descr(classes, "classes");
  //vec_descr(training, "training");
  //mat_descr(oprob, "oprob");
  //mat_descr(ocof, "ocof");

  OUTPUT << "num classes    = " << numclass << '\n';
  OUTPUT << "num patterns   = " << numpat << '\n';
  OUTPUT << "num features   = " << numfeat << '\n';
  OUTPUT << "class count    = " << class_count << '\n';
  OUTPUT << "m mean         = " << m_mean << '\n';
  OUTPUT << "m pooled       = " << m_pooled << '\n';
  OUTPUT << "m alpha        = " << m_alpha << '\n';
  OUTPUT << "m singular     = " << singular << '\n';
  OUTPUT << "mini mode      = " << mini_mode << '\n';
  OUTPUT << "training       = " << training << '\n';
  OUTPUT << "percent correct = " << dm_percentCorrect << '\n';

}

//Trainingc-calculate first alpha
template <class T>
  void pakit::lda_alg<T>::calc_alpha0(void)
{
  size_t i;
  data_type nsum;
  data_matrix P1, P2;
  slice_data_matrix r;

  //calc sum n_i
  scopira::basekit::sum(class_count, nsum);

  //iterate through all the classes
  for (i=0; i<numclass; i++) {
    // calculating alpha_0,i = ln[n_i / sum n_i] - .5 mean_i * poolvar^-1 * mean_i^T
    r = m_mean.xyslice(0, i, numfeat, 1);
    scopira::basekit::mul_matrix(P1, r, m_pooled, false, false);
    scopira::basekit::mul_matrix(P2, P1, r, false, true);

    // merge it all
    m_alpha(0,i) = ::log(class_count[i] / nsum) - 0.5 * P2(0,0);
  }
}

//Training - calculate  alpha
template <class T>
  void pakit::lda_alg<T>::calc_alpha(void)
{
  size_t i, j;
  data_matrix P;

  for (i=0; i<numclass; ++i) {
    // calculating alpha_1+,i = mean_i * poolvar^-1    vector
    scopira::basekit::mul_matrix(P, m_mean.xyslice(0,i,numfeat,1), m_pooled, false, false);

    for (j=0; j<P.width(); ++j)
      m_alpha(j+1, i) = P(j, 0);
  }
}

//Training - calculate first alpha (special case when numclasses==2)
template <class T>
  void pakit::lda_alg<T>::calc_mini_alpha0(void)
{
  data_matrix Sub, p1, p2, weighted_r1, weighted_r2;
  slice_data_matrix r1, r2;

  // calculating alpha_0 = -0.5 * (mean_1-mean_2) poolvar^-1 (mean_1+mean_2)T - ln(n_2 / n_1)
  r1 = m_mean.xyslice(0, 0, numfeat, 1);
  r2 = m_mean.xyslice(0, 1, numfeat, 1);

  Sub.resize(r1.width(), r1.height());
  Sub.copy(r1);
  scopira::basekit::sub_vector(Sub, r2);

  scopira::basekit::mul_matrix(p1, Sub, m_pooled, false, false);
  
    // calculating alpha_0 = -0.5 * (mean_1-mean_2) poolvar^-1 (mean_1+mean_2)T - ln(n_2 / n_1)
  if (class1_weight != -1) {    
    // with class1_weight, (mean_1+mean_2) becomes (class1_weight*mean_1+(1-class1_weight)*mean2)
    weighted_r1.resize(r1.width(), r1.height());
    weighted_r1.copy(r1);
    scopira::basekit::mul_matrix(weighted_r1,class1_weight);
    weighted_r2.resize(r2.width(), r2.height());
    weighted_r2.copy(r2);
    scopira::basekit::mul_matrix(weighted_r2,1.0 - class1_weight);
    Sub.copy(weighted_r1);
    scopira::basekit::add_vector(Sub, weighted_r2);

    scopira::basekit::mul_matrix(p2, p1, Sub, false, true);
    
    //merge it all. With class1_weight, -0.5 term becomes -1.0
    m_alpha(0,0) = -1.0 * p2(0,0) - ::log(static_cast<data_type>(class_count[1]) / class_count[0]);
  }
  else {
    Sub.copy(r1);
    scopira::basekit::add_vector(Sub, r2);

    scopira::basekit::mul_matrix(p2, p1, Sub, false, true);

    //merge it all
    m_alpha(0,0) = -0.5 * p2(0,0) - ::log(static_cast<data_type>(class_count[1]) / class_count[0]);
  }
}


//Training - calculate the rest of the alpha (special case when numclasses==2)
template <class T>
  void pakit::lda_alg<T>::calc_mini_alpha(void)
{
  size_t j;
  data_matrix Sub, P;

  Sub.resize(numfeat, 1);
  Sub.copy( m_mean.xyslice(0, 0, numfeat, 1) );

  scopira::basekit::sub_vector(Sub, m_mean.xyslice(0, 1, numfeat, 1));

  scopira::basekit::mul_matrix(P, Sub, m_pooled, false, false);

  for (j=0; j<P.width(); ++j)
    m_alpha(j+1, 0) = P(j,0);
}

// Rotate the line about a pivot point (very special case when numclasses==2 and numfeatures==2)
// by adjusting a couple of alphas
template <class T>
  void pakit::lda_alg<T>::rotate_lda_line(void)
{
  double xcent1, ycent1, xcent2, ycent2; // centroids of the data
  double diffterm;                       // "difference term"
  double xpivot, ypivot;                 // pivot point coords
  double initial_angle, new_angle;       // angles with and without additional user-specified rotation

  if ( m_mean.height() != 2 || m_mean.width() != 2 )
    return;

  // calculate pivot point for rotation.  It's the intersection of:
  // - LDA line without rotation
  // - line joining the two centroids
  xcent1 = m_mean( 0, 0 );
  ycent1 = m_mean( 1, 0 );
  xcent2 = m_mean( 0, 1 );
  ycent2 = m_mean( 1, 1 );
  diffterm = ( ycent2 - ycent1 ) / ( xcent2 - xcent1 );
  xpivot = ( xcent1 * diffterm - ycent1 - m_alpha(0,0) / m_alpha(2,0) ) /
           ( diffterm + m_alpha(1,0) / m_alpha(2,0) );
  ypivot = ( -m_alpha(1,0) * xpivot - m_alpha(0,0) ) / m_alpha(2,0);

  initial_angle = atan( -m_alpha(1,0) / m_alpha(2,0) );
  new_angle = initial_angle + rotation * M_PI / 180.0;
  bool flipped = false;
  if ( new_angle > M_PI / 2.0 ) {
    new_angle -= M_PI;
    flipped = true;
  }
  if ( new_angle < -M_PI / 2.0 ) {
    new_angle += M_PI;
    flipped = true;
  }
  if ( new_angle == M_PI / 2.0 )
    m_alpha(1,0) = -10000 * m_alpha(2,0);
  else if ( new_angle == -M_PI / 2.0 )
    m_alpha(1,0) = 10000 * m_alpha(2,0);
  else
    m_alpha(1,0) = -tan( new_angle ) * m_alpha(2,0);
  m_alpha(0,0) = ( tan( new_angle ) * xpivot - ypivot ) * m_alpha(2,0);
  if ( flipped == true ) {
    // Flip coefficient signs.  Doesn't change line orientation, but if we
    // don't do this the meaning of which class is on which side of the line
    // changes.
    m_alpha(0,0) *= -1.0;
    m_alpha(1,0) *= -1.0;
    m_alpha(2,0) *= -1.0;
  }
}


//Testing - calculate the worst possible results
template <class T>
  void pakit::lda_alg<T>::calc_wrong(void)
{
  // singular matrix; rig my output to be the worse it can be
  // (or maybe just make it all zeros)
  o_prob->clear();
}

template <class T>
  void pakit::lda_alg<T>::calc_mini_test(void)
{
  size_t y, i;
  data_type sum;

  // calc the d() = alpha0 + alpha1*x1 etc
  for (y=0; y < o_prob->height(); ++y) {
    sum = o_cof->get(0,0);
    for (i=1; i < o_cof->width(); ++i)
      sum += o_cof->get(i,0) * i_features->get(i-1, y);
    o_prob->set(0,y, ::exp(sum) / (1 + exp(sum)));
    o_prob->set(1,y, 1 / (1 + exp(sum)));
  }
}

template <class T>
  void pakit::lda_alg<T>::calc_test(void)
{
  size_t x, y, i;
  data_type sum;
  scopira::basekit::narray<data_type> VB;

  VB.resize(o_prob->height());
  VB.clear();

  // calc the d() = alpha0 + alpha1*x1 etc
  // for every class X pattern
  for (y=0; y < o_prob->height(); ++y)
    for (x=0; x < o_prob->width(); ++x) {
      sum = o_cof->get(0,x);
      for (i=1; i < o_cof->width(); ++i)
        sum += o_cof->get(i,x) * i_features->get(i-1,y);
      o_prob->set(x, y, sum);
    }

  // find the max d_max() across each row in Moutput
  // and put it in VB
  for (y=0; y < o_prob->height(); ++y)
    scopira::basekit::max( o_prob->xslice(0, y, o_prob->width()), VB[y] );

  // for each class X pattern, calc the unormalized
  // probability of member ship:
  // PU = e^(d() - d_max())
  for (y=0; y < o_prob->height(); ++y)
    for (x=0; x < o_prob->width(); ++x)
      o_prob->set(x,y, ::exp(o_prob->get(x, y) - VB[y]));

  // calc the sum of the probs in each pattern
  for (y=0; y < o_prob->height(); ++y)
    scopira::basekit::sum(o_prob->xslice(0,y,o_prob->width()), VB[y]);

  // finally, normalize all the probabities in each pattern
  // against their sums
  for (y=0; y < o_prob->height(); ++y)
    for (x=0; x < o_prob->width(); ++x)
      (*o_prob)(x,y) /= VB[y];
}

/*
 *  Assumes class labels are the same as x index (width) of probability matrix,
 *  first class label being 1, second is class 2, etc... must make this assumption as
 *  currently there is no other information as the class label mechanism in data, assume
 *  classes start at 1, incrementing by one up to the number of classes.
 *
 */
template <class T>
  void pakit::lda_alg<T>::calc_percent_correct()
{
  size_t  x,y;
  double  numTestVecs = 0;  // to avoid casting for int division
  double  numCorrect = 0;   // when want floating point precision
  int     winningClass;
  double  winningProb;

  if (!training) //added by conrad, shouldn't do this if we're doing the validation stuff, since if training is false, we haven't given it a training matrix, so it will crash
    return;

  // for all vectors in probability matrix
  for (y=0; y<o_prob->height(); y++)
  {
    if ( !(*i_training)[y] )  // if not training, must be test vector
    {
      numTestVecs++;
//ENGINEOUT<<"y: "<<y<<" numTestVecs: "<<numTestVecs<<" ";
      // find the winning class, largest prob value
      winningProb = (*o_prob)(0,y);
      winningClass = 1;
//ENGINEOUT<<"class: "<<winningClass<<" prob: "<<winningProb<<" ";
      // comapre to all other class probabilities for data vector
      for (x=1; x<o_prob->width(); x++ )
      {
        if ( (*o_prob)(x,y) > winningProb ) {
          winningProb = (*o_prob)(x,y);
          winningClass = x+1;
//ENGINEOUT<<"class: "<<winningClass<<" prob: "<<winningProb<<" ";
        }
      }

      if ( winningClass == (*i_classes)[y] )
        numCorrect++;
//ENGINEOUT<<" numCorrect: "<<numCorrect<<"\n";
    }
  }
  dm_percentCorrect = numCorrect / numTestVecs;

//ENGINEOUT<<" percentCorrect: "<< dm_percentCorrect << "\n";
}

#endif

