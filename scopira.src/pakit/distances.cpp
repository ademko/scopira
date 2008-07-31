
/*
 *  Copyright (c) 2004    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#include <math.h>
#include <pakit/distances.h>

#include <sstream>

#include <scopira/basekit/narray.h>
#include <scopira/basekit/vectormath.h>
#include <scopira/basekit/matrixmath.h>
#include <scopira/tool/output.h>
#include <pakit/math.h>

//BBlibs scopira scopiraxml
//BBtargets libpakit.so

using namespace scopira::tool;
using namespace scopira::basekit;
using namespace pakit;
using namespace std;

// SIMPLE distances - no intervening covariance matrices etc.

void pakit::euclidean_distance(const nslice<double> &a,
  const nslice<double> &b, double &dist)
{
  double sum, dd;
  size_t x, l;

  assert(a.size() > 0);
  assert(b.size() > 0);
  assert(a.size() == b.size());
  l = a.size();

  sum = 0;
  for (x=0; x<l; x++) {
    dd = a[x] - b[x];
    sum += dd*dd;
  }
  
  dist = ::sqrt(sum);
//  dist = sum;
}

void pakit::cityblock_distance(const nslice<double> &a,
  const nslice<double> &b, double &dist)
{
  double sum;
  size_t x, l;

  assert(a.size() > 0);
  assert(b.size() > 0);
  assert(a.size() == b.size());
  l = a.size();

  sum = 0;
  for (x=0; x<l; x++) {
    sum += ::fabs(a[x] - b[x]);
  }
  
  dist = sum;
}

void pakit::max_distance(const nslice<double> &a,
  const nslice<double> &b, double &dist)
{
  double curr_dist;
  size_t x, l;

  assert(a.size() > 0);
  assert(b.size() > 0);
  assert(a.size() == b.size());
  l = a.size();

  dist = 0.0;
  for (x=0; x<l; x++) {
    curr_dist = ::fabs(a[x] - b[x]);
    if ( curr_dist > dist ) {
      dist = curr_dist;
    }
  }
}

// This function is called first, and it calls the others!

void pakit::calc_distance_matrix_simple(const scopira::basekit::nslice<double,2> &patterns,
    scopira::basekit::narray<double,2> &outmatrix,
    simple_distance_func_t func)
{
  size_t k, j, sz;
  double dd;

  sz = patterns.height();

  outmatrix.resize(sz, sz);

  for (k=1; k<sz; ++k)
    for (j=0; j<k; ++j) {
      func(patterns.xslice(0, k, patterns.width()),
          patterns.xslice(0, j, patterns.width()), dd);
      outmatrix(k, j) = dd;
      outmatrix(j, k) = dd;
    }

  // set the diagonal
  outmatrix.diagonal_slice().set_all(0);
}

// COMPOUND distances - intervening covariance matrices, additive and
// multiplicative terms

/*
 * Anderson-Bahadur distance
 *
 * Uses a mixture of the covariance matrices of the two user-specified classes.
 * Mixture controlled through the "class_weight" parameter
 *
 * @author Brion Dolenko
 */

//
// Calling function - interfaces to distance matrix calculation
//

bool pakit::calc_ab_distance(pakit::patterns_m* dm_model,pakit::distances_m *dm_distance, int cl1, int cl2, 
  double class_weight, string &msg)
{  
  int numclass;

    if ((dm_model->pm_data->width() < 1) || (dm_model->pm_data->height() < 2)) {
     msg="Input matrix must be at least 1X2";
     return false;
    }
    
    assert(dm_model->pm_classes->size() == dm_model->pm_data->height());
    assert(dm_model->pm_training->size() == dm_model->pm_data->height());
    assert(dm_model->pm_labels->size() == dm_model->pm_data->height());
    
    if ( ( cl1 <= 0 ) || ( cl2 <= 0 ) ) {
      msg="Classes must be positive.";
      return false;
    }
    
    max(dm_model->pm_classes.ref(), numclass);
    if ( ( cl1 > numclass ) || ( cl2 > numclass ) ) {
      msg="Classes must be "+int_to_string(numclass)+" at most.";
      return false;
    }
    
    if (!calc_distance_matrix_ab(dm_model->pm_data.ref(), dm_model->pm_classes.ref(),
        dm_model->pm_labels.ref(), dm_distance->pm_array.ref(), cl1,cl2, class_weight, msg)) 
      return false;
      
    msg ="Distance matrix (AB, based on classes "+int_to_string(cl1)+" & "+int_to_string(cl2)
      +", c = "+double_to_string(class_weight)+")";
      
    return true;
}    

//
// Distance matrix calculation
//
    
bool pakit::calc_distance_matrix_ab(narray<double,2> &inn, 
  const narray<int,1> &cls,
  const stringvector_o &labels,
  narray<double,2> &outmatrix,
  int cl1, int cl2, double class_weight, string &msg)
{
  size_t sz, h, w;
  int pidx, csidx, aidx, csidx1, csidx2;
  int centroids_present;
  stringstream tmpmsg;
  
  enum t_m_result { SUCCESS, NO_CLASS, NO_CENTROIDS, LARGE_DATA, SINGULAR } ab_result;
  
  sz = inn.height();
  outmatrix.resize(sz, sz);
  
  h = inn.height();
  w = inn.width();
  ab_result = SUCCESS;
  
  // Check for valid classes before doing anything
  bool cl1found = false;
  bool cl2found = false;
  int maxclass = 0;

  for (pidx = 0; pidx < cls.size(); ++pidx ) {
    if ( cls.get(pidx) == cl1 )
      cl1found = true;
    if ( cls.get(pidx) == cl2 )
      cl2found = true;
    if ( cls.get(pidx) > maxclass )
      maxclass = cls.get(pidx);
   }                                       
   if (( cl1 > maxclass ) || ( cl2 > maxclass )) {
     tmpmsg<<"Class out of range";
     return false;
   }
   if ( !cl1found ) {
     tmpmsg<<"Class "<<cl1<<" does not exist in the data set";
     msg=tmpmsg.str();
     return false;
   }
   if ( !cl2found ) {
     tmpmsg<<"Class "<<cl2<<" does not exist in the data set";
     msg=tmpmsg.str();
     return false;
   }
   
   // Count numbers in classes and check for too many attributes
   int numinclass1 = 0;
   int numinclass2 = 0;
   
   for (pidx = 0; pidx < cls.size(); pidx++ ) {
     if ( cls.get(pidx) == cl1 ) 
        numinclass1++;
     else if (cls.get(pidx) == cl2 ) // OK leads to an error, but not the expected one! 
        numinclass2++;
    }

    // See which classes are to be skipped over in computation (because nothing in them)
    boolvec_o placeholder_class; // set to true if nothing in the class

    placeholder_class.resize(maxclass);
    for(csidx = 0; csidx < maxclass; csidx++)
      placeholder_class.set(csidx, true);
    for(pidx = 0; pidx < cls.size(); ++pidx) {
      if ( cls.get( pidx ) >= 1 && cls.get( pidx ) <= maxclass ) {
        placeholder_class.set( cls.get( pidx ) - 1, false );
      }
    }

    doublequad_o pooleds;
    doublematrix_o pooled_covar;

    while ( ab_result == SUCCESS ) { 
      // Check for class info
      if ( maxclass < 2 ) {
        ab_result = NO_CLASS;
        break;
      }

      // This will catch both classes being the same
      if ( ( numinclass1 == 0 ) || ( numinclass2 == 0 ) ) {
        ab_result = NO_CLASS;
        break;
      }
      // Check for too many attributes
      if ( ( numinclass1 < w ) || ( numinclass2 < w ) ) {
        ab_result = LARGE_DATA;
        break;
      }

      // Check whether centroids are present
      centroids_present = 0;
      for (pidx = 0; pidx < cls.size(); pidx++ ) {
        if ( cls.get(pidx) == -cl1 && labels.get(pidx).find( "centroid" ) != string::npos ) {
          centroids_present++;
        }
        if ( cls.get(pidx) == -cl2 && labels.get(pidx).find( "centroid" ) != string::npos )
          centroids_present++;
      }
      if ( centroids_present != 2 ) {
        ab_result = NO_CENTROIDS;
        break;
      }

      // Yup, they're present.  Get the centroids
      doublematrix_o centroid; 
      doublematrix_o two_class_mean;

      centroid.resize( w, maxclass );
      two_class_mean.resize( w, 2 );
      for ( csidx = 1; csidx <= maxclass; csidx++ ) {
        for ( pidx=0; pidx<cls.size(); pidx++ ) {
          if (cls.get(pidx) == -csidx) { // Should we be assuming this?
                                          // but speeds up computation if we do
            for (aidx = 0; aidx < w; aidx++ ) {
              centroid( aidx, csidx-1 ) = inn.get( aidx, pidx );
            }
          }
        }
      }

      // Dimension the 1d arrays
      intvec_o class_list;
      intvec_o two_class_list;
      boolvec_o two_class_training_list;
      class_list.resize(h);
      two_class_list.resize(h);
      two_class_training_list.resize(h);

      // Fill the class list array
      for (pidx = 0; pidx < cls.size(); pidx++ )
        if ( cls.get(pidx) > 0 )
          class_list[pidx] = cls.get(pidx);
        else
          class_list[pidx] = 0;

      // Dimension the multi-dim arrays
      int c = maxclass;
      pooleds.resize(nindex<4>(w,w,c,c));
      pooled_covar.resize(nindex<2>(w,w));

      bool singular;

      // Loop over pairs of classes
      for (csidx1 = 0; csidx1 < maxclass; csidx1++ ) {
        if ( placeholder_class.get( csidx1 ) == true )
          continue;
        // For now, we're only computing covariance matrices for selected classes
        if ( ( csidx1+1 != cl1 ) && ( csidx1+1 != cl2 ) )
          continue;
         
        two_class_mean.xyslice(0,0,w,1).copy(centroid.xyslice(0,csidx1,w,1));

        for (csidx2 = 0; csidx2 < maxclass; csidx2++ ) {
          if ( placeholder_class.get( csidx2 ) == true )
            continue;
          if ( csidx2 == csidx1 ) {
            continue;
          }
          // For now, we're only computing covariance matrices for selected classes
          if ( ( csidx2+1 != cl1 ) && ( csidx2+1 != cl2 ) )
            continue;
          two_class_mean.xyslice(0,1,w,1).copy(centroid.xyslice(0,csidx2,w,1));
          for (pidx = 0; pidx < class_list.size(); pidx++ ) {
            if ( class_list[pidx] == csidx1+1 ) {
              two_class_list[pidx] = 1;
              two_class_training_list[pidx] = true;
            }
            else if ( class_list[pidx] == csidx2+1 ) {
              two_class_list[pidx] = 2;
              two_class_training_list[pidx] = true;
            }
            else {
              two_class_list[pidx] = 0;
              two_class_training_list[pidx] = false;
            }
          }
          
          calc_ander_baha_covar( pooled_covar, two_class_mean,inn, 
                           &two_class_training_list, &two_class_list, 
                           class_weight );
          //???? mul_matrix(pooled_covar,2.0);
          singular = !invert_matrix(pooled_covar,pooled_covar);
          if ( singular ) {
            ab_result = SINGULAR;
            break; 
          }
          (pooleds.slicer(nindex<4>(0,0,csidx1,csidx2),nindex<2>(w,w))).copy(pooled_covar);
        } // end loop over second class
        if ( ab_result == SINGULAR  || ab_result == LARGE_DATA ) {
          break; 
        }
      } // end loop over first class
      break;
    } // end while successful

    if ( ab_result == SUCCESS ) {
      doublematrix_o point_diff;
      doublematrix_o P1, P2;
      nslice<double,2> point1, point2, slice_point_diff;
      double dd;
      nslice<double,2> src = inn.all_slice();

      for (int y1=0; y1<h; ++y1) {
        point1 = src.xyslice(0, y1, w, 1);
        for (int y2=y1+1; y2<h; ++y2) {
          point2 = src.xyslice(0, y2, w, 1);
          point_diff.resize( w, 1 );
          for ( int aidx = 0; aidx < w; aidx++ ) {
            point_diff.set( aidx, 0, point1.get( aidx, 0 ) - point2.get( aidx, 0 ) );
          }
          slice_point_diff = point_diff.xyslice( 0, 0, w, 1 );
          // always use covariance matrix for selected classes 1 and 2
          pooled_covar.copy(pooleds.slicer(nindex<4>(0,0,cl1-1,cl2-1),nindex<2>(w,w)));
          scopira::basekit::mul_matrix(P1, point_diff, pooled_covar, false, false);
          scopira::basekit::mul_matrix(P2, P1, point_diff, false, true);
          dd = ::sqrt(P2(0,0));
          outmatrix.set(y1, y2, dd);
          outmatrix.set(y2, y1, dd);
        }
      }
    }
    for (int y1=0; y1<h; y1++)
      outmatrix.set(y1, y1, 0);

    switch( ab_result ) {
      case SUCCESS:
        return true;
      case NO_CLASS:
        msg= "AB Distance calculation failed - no class info";
        return false;
      case NO_CENTROIDS:
       msg="AB Distance calculation failed - need to add centroids first";
        return false;
      case LARGE_DATA:
        msg="AB Distance calculation failed - too many attributes";
        return false;
      case SINGULAR:
        msg="AB Distance calculation failed - singular matrix";
        return false;
    }
    
    // should not reach this, but stop the warning
    return false;
}

/*
 * Chernoff distance
 *
 * Uses a mixture of the covariance matrices of the two user-specified classes.
 * Also a log term involving determinants of the covariance matrices is added.
 * Mixture controlled through the "class_weight" parameter
 *
 * @author Brion Dolenko
 */

//
// Calling function - interfaces to distance matrix calculation
//

bool pakit::calc_chernoff_distance(pakit::patterns_m* dm_model,pakit::distances_m *dm_distance, int cl1, int cl2, 
  double class_weight, string &msg)
{  
  int numclass;

    if ((dm_model->pm_data->width() < 1) || (dm_model->pm_data->height() < 2)) {
     msg="Input matrix must be at least 1X2";
     return false;
    }
    
    assert(dm_model->pm_classes->size() == dm_model->pm_data->height());
    assert(dm_model->pm_training->size() == dm_model->pm_data->height());
    assert(dm_model->pm_labels->size() == dm_model->pm_data->height());
    
    if ( ( cl1 <= 0 ) || ( cl2 <= 0 ) ) {
      msg="Classes must be positive.";
      return false;
    }
    
    max(dm_model->pm_classes.ref(), numclass);
    if ( ( cl1 > numclass ) || ( cl2 > numclass ) ) {
      msg="Classes must be "+int_to_string(numclass)+" at most.";
      return false;
    }
    
    if (!calc_distance_matrix_chernoff(dm_model->pm_data.ref(), dm_model->pm_classes.ref(),
        dm_model->pm_labels.ref(), dm_distance->pm_array.ref(), cl1,cl2, class_weight, msg)) 
      return false;
      
    msg ="Distance matrix (Chernoff, based on classes "+int_to_string(cl1)+" & "+int_to_string(cl2)
      +", c = "+double_to_string(class_weight)+")";
      
    return true;
}    

//
// Distance matrix calculation
//
    
bool pakit::calc_distance_matrix_chernoff(narray<double,2> &inn, 
  const narray<int,1> &cls,
  const stringvector_o &labels,
  narray<double,2> &outmatrix,
  int cl1, int cl2, double class_weight, string &msg)
{
  size_t sz, h, w;
  double logterm;
  int pidx, csidx, aidx, csidx1, csidx2;
  int centroids_present;
  stringstream tmpmsg;
  
  enum t_m_result { SUCCESS, NO_CLASS, NO_CENTROIDS, LARGE_DATA, SINGULAR } chernoff_result;
  
  sz = inn.height();
  outmatrix.resize(sz, sz);
  
  h = inn.height();
  w = inn.width();
  chernoff_result = SUCCESS;
  
  // Check for valid classes before doing anything
  bool cl1found = false;
  bool cl2found = false;
  int maxclass = 0;

  for (pidx = 0; pidx < cls.size(); ++pidx ) {
    if ( cls.get(pidx) == cl1 )
      cl1found = true;
    if ( cls.get(pidx) == cl2 )
      cl2found = true;
    if ( cls.get(pidx) > maxclass )
      maxclass = cls.get(pidx);
   }                                       
   if (( cl1 > maxclass ) || ( cl2 > maxclass )) {
     tmpmsg<<"Class out of range";
     return false;
   }
   if ( !cl1found ) {
     tmpmsg<<"Class "<<cl1<<" does not exist in the data set";
     msg=tmpmsg.str();
     return false;
   }
   if ( !cl2found ) {
     tmpmsg<<"Class "<<cl2<<" does not exist in the data set";
     msg=tmpmsg.str();
     return false;
   }
   
   // Count numbers in classes and check for too many attributes
   int numinclass1 = 0;
   int numinclass2 = 0;
   
   for (pidx = 0; pidx < cls.size(); pidx++ ) {
     if ( cls.get(pidx) == cl1 ) 
        numinclass1++;
     else if (cls.get(pidx) == cl2 ) // OK leads to an error, but not the expected one! 
        numinclass2++;
    }

    // See which classes are to be skipped over in computation (because nothing in them)
    boolvec_o placeholder_class; // set to true if nothing in the class

    placeholder_class.resize(maxclass);
    for(csidx = 0; csidx < maxclass; csidx++)
      placeholder_class.set(csidx, true);
    for(pidx = 0; pidx < cls.size(); ++pidx) {
      if ( cls.get( pidx ) >= 1 && cls.get( pidx ) <= maxclass ) {
        placeholder_class.set( cls.get( pidx ) - 1, false );
      }
    }

    doublequad_o pooleds;
    doublematrix_o pooled_covar;
    doublematrix_o logterms;

    while ( chernoff_result == SUCCESS ) { 
      // Check for class info
      if ( maxclass < 2 ) {
        chernoff_result = NO_CLASS;
        break;
      }

      // This will catch both classes being the same
      if ( ( numinclass1 == 0 ) || ( numinclass2 == 0 ) ) {
        chernoff_result = NO_CLASS;
        break;
      }
      // Check for too many attributes
      if ( ( numinclass1 < w ) || ( numinclass2 < w ) ) {
        chernoff_result = LARGE_DATA;
        break;
      }

      // Check whether centroids are present
      centroids_present = 0;
      for (pidx = 0; pidx < cls.size(); pidx++ ) {
        if ( cls.get(pidx) == -cl1 && labels.get(pidx).find( "centroid" ) != string::npos ) {
          centroids_present++;
        }
        if ( cls.get(pidx) == -cl2 && labels.get(pidx).find( "centroid" ) != string::npos )
          centroids_present++;
      }
      if ( centroids_present != 2 ) {
        chernoff_result = NO_CENTROIDS;
        break;
      }

      // Yup, they're present.  Get the centroids
      doublematrix_o centroid; 
      doublematrix_o two_class_mean;

      centroid.resize( w, maxclass );
      two_class_mean.resize( w, 2 );
      for ( csidx = 1; csidx <= maxclass; csidx++ ) {
        for ( pidx=0; pidx<cls.size(); pidx++ ) {
          if (cls.get(pidx) == -csidx) { // Should we be assuming this?
                                          // but speeds up computation if we do
            for (aidx = 0; aidx < w; aidx++ ) {
              centroid( aidx, csidx-1 ) = inn.get( aidx, pidx );
            }
          }
        }
      }

      // Dimension the 1d arrays
      intvec_o class_list;
      intvec_o two_class_list;
      boolvec_o two_class_training_list;
      class_list.resize(h);
      two_class_list.resize(h);
      two_class_training_list.resize(h);

      // Fill the class list array
      for (pidx = 0; pidx < cls.size(); pidx++ )
        if ( cls.get(pidx) > 0 )
          class_list[pidx] = cls.get(pidx);
        else
          class_list[pidx] = 0;

      // Dimension the multi-dim arrays
      int c = maxclass;
      pooleds.resize(nindex<4>(w,w,c,c));
      pooled_covar.resize(nindex<2>(w,w));
      logterms.resize(c,c);

      bool singular;

      // Loop over pairs of classes
      for (csidx1 = 0; csidx1 < maxclass; csidx1++ ) {
        if ( placeholder_class.get( csidx1 ) == true )
          continue;
        // For now, we're only computing covariance matrices for selected classes
        if ( ( csidx1+1 != cl1 ) && ( csidx1+1 != cl2 ) )
          continue;

        two_class_mean.xyslice(0,0,w,1).copy(centroid.xyslice(0,csidx1,w,1));

        for (csidx2 = 0; csidx2 < maxclass; csidx2++ ) {
          if ( placeholder_class.get( csidx2 ) == true )
            continue;
          if ( csidx2 == csidx1 ) {
            continue;
          }
          // For now, we're only computing covariance matrices for selected classes
          if ( ( csidx2+1 != cl1 ) && ( csidx2+1 != cl2 ) )
            continue;
          two_class_mean.xyslice(0,1,w,1).copy(centroid.xyslice(0,csidx2,w,1));
          for (pidx = 0; pidx < class_list.size(); pidx++ ) {
            if ( class_list[pidx] == csidx1+1 ) {
              two_class_list[pidx] = 1;
              two_class_training_list[pidx] = true;
            }
            else if ( class_list[pidx] == csidx2+1 ) {
              two_class_list[pidx] = 2;
              two_class_training_list[pidx] = true;
            }
            else {
              two_class_list[pidx] = 0;
              two_class_training_list[pidx] = false;
            }
          }
          
          calc_chernoff_covar( pooled_covar, &logterm, two_class_mean,inn, 
                           &two_class_training_list, &two_class_list, 
                           class_weight );
          //???? mul_matrix(pooled_covar,2.0);
          singular = !invert_matrix(pooled_covar,pooled_covar);
          if ( singular ) {
            chernoff_result = SINGULAR;
            break; 
          }
          (pooleds.slicer(nindex<4>(0,0,csidx1,csidx2),nindex<2>(w,w))).copy(pooled_covar);
          logterms.set(csidx1,csidx2,logterm);
        } // end loop over second class
        if ( chernoff_result == SINGULAR  || chernoff_result == LARGE_DATA ) {
          break; 
        }
      } // end loop over first class
      break;
    } // end while successful

    if ( chernoff_result == SUCCESS ) {
      doublematrix_o point_diff;
      doublematrix_o P1, P2;
      nslice<double,2> point1, point2, slice_point_diff;
      double dd;
      nslice<double,2> src = inn.all_slice();

      for (int y1=0; y1<h; ++y1) {
        point1 = src.xyslice(0, y1, w, 1);
        for (int y2=y1+1; y2<h; ++y2) {
          point2 = src.xyslice(0, y2, w, 1);
          point_diff.resize( w, 1 );
          for ( int aidx = 0; aidx < w; aidx++ ) {
            point_diff.set( aidx, 0, point1.get( aidx, 0 ) - point2.get( aidx, 0 ) );
          }
          slice_point_diff = point_diff.xyslice( 0, 0, w, 1 );
          // always use covariance matrix for selected classes 1 and 2
          pooled_covar.copy(pooleds.slicer(nindex<4>(0,0,cl1-1,cl2-1),nindex<2>(w,w)));
          scopira::basekit::mul_matrix(P1, point_diff, pooled_covar, false, false);
          scopira::basekit::mul_matrix(P2, P1, point_diff, false, true);
          dd = ::sqrt(0.5 * class_weight * (1.0 - class_weight) * P2(0,0) + logterms.get(cl1-1,cl2-1));
          outmatrix.set(y1, y2, dd);
          outmatrix.set(y2, y1, dd);
        }
      }
    }
    for (int y1=0; y1<h; y1++)
      outmatrix.set(y1, y1, 0);

    switch( chernoff_result ) {
      case SUCCESS:
        return true;
      case NO_CLASS:
        msg= "Chernoff Distance calculation failed - no class info";
        return false;
      case NO_CENTROIDS:
       msg="Chernoff Distance calculation failed - need to add centroids first";
        return false;
      case LARGE_DATA:
        msg="Chernoff Distance calculation failed - too many attributes";
        return false;
      case SINGULAR:
        msg="Chernoff Distance calculation failed - singular matrix";
        return false;
    }
    
    // should not reach this, but stop the warning
    return false;
}

/*
 * Symmetric KL Divergence distance
 *
 * Uses a mixture of the covariance matrices of the two user-specified classes.
 * Also a trace term involving inverses of the covariance matrices is added.
 *
 * @author Brion Dolenko
 */
    
//
// Calling function - interfaces to distance matrix calculation
//

bool pakit::calc_sym_kl_divergence_distance(pakit::patterns_m* dm_model,pakit::distances_m *dm_distance, int cl1, int cl2, 
  double class_weight, string &msg)
{  
  int numclass;

    if ((dm_model->pm_data->width() < 1) || (dm_model->pm_data->height() < 2)) {
     msg="Input matrix must be at least 1X2";
     return false;
    }
    
    assert(dm_model->pm_classes->size() == dm_model->pm_data->height());
    assert(dm_model->pm_training->size() == dm_model->pm_data->height());
    assert(dm_model->pm_labels->size() == dm_model->pm_data->height());
    
    if ( ( cl1 <= 0 ) || ( cl2 <= 0 ) ) {
      msg="Classes must be positive.";
      return false;
    }
    
    max(dm_model->pm_classes.ref(), numclass);
    if ( ( cl1 > numclass ) || ( cl2 > numclass ) ) {
      msg="Classes must be "+int_to_string(numclass)+" at most.";
      return false;
    }
    
    if (!calc_distance_matrix_sym_kl_divergence(dm_model->pm_data.ref(), dm_model->pm_classes.ref(),
        dm_model->pm_labels.ref(), dm_distance->pm_array.ref(), cl1,cl2, msg)) 
      return false;
      
    msg ="Distance matrix (sym_kl_divergence, based on classes "+int_to_string(cl1)+" & "+int_to_string(cl2) + ")";
      
    return true;
}    

//
// Distance matrix calculation
//

bool pakit::calc_distance_matrix_sym_kl_divergence(narray<double,2> &inn, 
  const narray<int,1> &cls,
  const stringvector_o &labels,
  narray<double,2> &outmatrix,
  int cl1, int cl2, string &msg)
{
  size_t sz, h, w;
  double traceterm;
  int pidx, csidx, aidx, csidx1, csidx2;
  int centroids_present;
  stringstream tmpmsg;
  
  enum t_m_result { SUCCESS, NO_CLASS, NO_CENTROIDS, LARGE_DATA, SINGULAR } sym_kl_divergence_result;
  
  sz = inn.height();
  outmatrix.resize(sz, sz);
  
  h = inn.height();
  w = inn.width();
  sym_kl_divergence_result = SUCCESS;
  
  // Check for valid classes before doing anything
  bool cl1found = false;
  bool cl2found = false;
  int maxclass = 0;

  for (pidx = 0; pidx < cls.size(); ++pidx ) {
    if ( cls.get(pidx) == cl1 )
      cl1found = true;
    if ( cls.get(pidx) == cl2 )
      cl2found = true;
    if ( cls.get(pidx) > maxclass )
      maxclass = cls.get(pidx);
   }                                       
   if (( cl1 > maxclass ) || ( cl2 > maxclass )) {
     tmpmsg<<"Class out of range";
     return false;
   }
   if ( !cl1found ) {
     tmpmsg<<"Class "<<cl1<<" does not exist in the data set";
     msg=tmpmsg.str();
     return false;
   }
   if ( !cl2found ) {
     tmpmsg<<"Class "<<cl2<<" does not exist in the data set";
     msg=tmpmsg.str();
     return false;
   }
   
   // Count numbers in classes and check for too many attributes
   int numinclass1 = 0;
   int numinclass2 = 0;
   
   for (pidx = 0; pidx < cls.size(); pidx++ ) {
     if ( cls.get(pidx) == cl1 ) 
        numinclass1++;
     else if (cls.get(pidx) == cl2 ) // OK leads to an error, but not the expected one! 
        numinclass2++;
    }

    // See which classes are to be skipped over in computation (because nothing in them)
    boolvec_o placeholder_class; // set to true if nothing in the class

    placeholder_class.resize(maxclass);
    for(csidx = 0; csidx < maxclass; csidx++)
      placeholder_class.set(csidx, true);
    for(pidx = 0; pidx < cls.size(); ++pidx) {
      if ( cls.get( pidx ) >= 1 && cls.get( pidx ) <= maxclass ) {
        placeholder_class.set( cls.get( pidx ) - 1, false );
      }
    }

    doublequad_o covars_1;
    doublequad_o covars_2;
    doublematrix_o covar1;
    doublematrix_o covar2;
    doublematrix_o traceterms;

doublematrix_o tmp_inverse;

    while ( sym_kl_divergence_result == SUCCESS ) { 
      // Check for class info
      if ( maxclass < 2 ) {
        sym_kl_divergence_result = NO_CLASS;
        break;
      }

      // This will catch both classes being the same
      if ( ( numinclass1 == 0 ) || ( numinclass2 == 0 ) ) {
        sym_kl_divergence_result = NO_CLASS;
        break;
      }
      // Check for too many attributes
      if ( ( numinclass1 < w ) || ( numinclass2 < w ) ) {
        sym_kl_divergence_result = LARGE_DATA;
        break;
      }

      // Check whether centroids are present
      centroids_present = 0;
      for (pidx = 0; pidx < cls.size(); pidx++ ) {
        if ( cls.get(pidx) == -cl1 && labels.get(pidx).find( "centroid" ) != string::npos ) {
          centroids_present++;
        }
        if ( cls.get(pidx) == -cl2 && labels.get(pidx).find( "centroid" ) != string::npos )
          centroids_present++;
      }
      if ( centroids_present != 2 ) {
        sym_kl_divergence_result = NO_CENTROIDS;
        break;
      }

      // Yup, they're present.  Get the centroids
      doublematrix_o centroid; 
      doublematrix_o two_class_mean;

      centroid.resize( w, maxclass );
      two_class_mean.resize( w, 2 );
      for ( csidx = 1; csidx <= maxclass; csidx++ ) {
        for ( pidx=0; pidx<cls.size(); pidx++ ) {
          if (cls.get(pidx) == -csidx) { // Should we be assuming this?
                                          // but speeds up computation if we do
            for (aidx = 0; aidx < w; aidx++ ) {
              centroid( aidx, csidx-1 ) = inn.get( aidx, pidx );
            }
          }
        }
      }

      // Dimension the 1d arrays
      intvec_o class_list;
      intvec_o two_class_list;
      boolvec_o two_class_training_list;
      class_list.resize(h);
      two_class_list.resize(h);
      two_class_training_list.resize(h);

      // Fill the class list array
      for (pidx = 0; pidx < cls.size(); pidx++ )
        if ( cls.get(pidx) > 0 )
          class_list[pidx] = cls.get(pidx);
        else
          class_list[pidx] = 0;

      // Dimension the multi-dim arrays
      int c = maxclass;
      covars_1.resize(nindex<4>(w,w,c,c));
      covars_2.resize(nindex<4>(w,w,c,c));
      covar1.resize(nindex<2>(w,w));
      covar2.resize(nindex<2>(w,w));
      traceterms.resize(c,c);

      bool singular;

      // Loop over pairs of classes
      for (csidx1 = 0; csidx1 < maxclass; csidx1++ ) {
        if ( placeholder_class.get( csidx1 ) == true )
          continue;
        // For now, we're only computing covariance matrices for selected classes
        if ( ( csidx1+1 != cl1 ) && ( csidx1+1 != cl2 ) )
          continue;

        two_class_mean.xyslice(0,0,w,1).copy(centroid.xyslice(0,csidx1,w,1));

        for (csidx2 = 0; csidx2 < maxclass; csidx2++ ) {
          if ( placeholder_class.get( csidx2 ) == true )
            continue;
          if ( csidx2 == csidx1 ) {
            continue;
          }
          // For now, we're only computing covariance matrices for selected classes
          if ( ( csidx2+1 != cl1 ) && ( csidx2+1 != cl2 ) )
            continue;
          two_class_mean.xyslice(0,1,w,1).copy(centroid.xyslice(0,csidx2,w,1));
          for (pidx = 0; pidx < class_list.size(); pidx++ ) {
            if ( class_list[pidx] == csidx1+1 ) {
              two_class_list[pidx] = 1;
              two_class_training_list[pidx] = true;
            }
            else if ( class_list[pidx] == csidx2+1 ) {
              two_class_list[pidx] = 2;
              two_class_training_list[pidx] = true;
            }
            else {
              two_class_list[pidx] = 0;
              two_class_training_list[pidx] = false;
            }
          }
          
          calc_sym_kl_divergence_covar( covar1, covar2, &traceterm, two_class_mean,inn, 
                           &two_class_training_list, &two_class_list);

          singular = !invert_matrix(covar1,covar1);
          if ( singular ) {
            sym_kl_divergence_result = SINGULAR;
            break; 
          }
          singular = !invert_matrix(covar2,covar2);
          if ( singular ) {
            sym_kl_divergence_result = SINGULAR;
            break; 
          }
          (covars_1.slicer(nindex<4>(0,0,csidx1,csidx2),nindex<2>(w,w))).copy(covar1);
          (covars_2.slicer(nindex<4>(0,0,csidx1,csidx2),nindex<2>(w,w))).copy(covar2);
          traceterms.set(csidx1,csidx2,traceterm);
        } // end loop over second class
        if ( sym_kl_divergence_result == SINGULAR  || sym_kl_divergence_result == LARGE_DATA ) {
          break; 
        }
      } // end loop over first class
      break;
    } // end while successful

    if ( sym_kl_divergence_result == SUCCESS ) {
      doublematrix_o point_diff;
      doublematrix_o P1, P2;
      doublematrix_o inv_sum;
      nslice<double,2> point1, point2, slice_point_diff;
      double dd;
      nslice<double,2> src = inn.all_slice();
      inv_sum.clear();
      inv_sum.resize(w,w);

      for (int y1=0; y1<h; ++y1) {
        point1 = src.xyslice(0, y1, w, 1);
        for (int y2=y1+1; y2<h; ++y2) {
          point2 = src.xyslice(0, y2, w, 1);
          point_diff.resize( w, 1 );
          for ( int aidx = 0; aidx < w; aidx++ ) {
            point_diff.set( aidx, 0, point1.get( aidx, 0 ) - point2.get( aidx, 0 ) );
          }
          slice_point_diff = point_diff.xyslice( 0, 0, w, 1 );
          // always use covariance matrix for selected classes 1 and 2
          covar1.copy(covars_1.slicer(nindex<4>(0,0,cl1-1,cl2-1),nindex<2>(w,w)));
          covar2.copy(covars_2.slicer(nindex<4>(0,0,cl1-1,cl2-1),nindex<2>(w,w)));
          for ( int aidx1 = 0; aidx1 < w; aidx1++ )
            for ( int aidx2 = 0; aidx2 < w; aidx2++ )
              inv_sum(aidx1,aidx2) = covar1(aidx1,aidx2) + covar2(aidx1,aidx2); 
          scopira::basekit::mul_matrix(P1, point_diff, inv_sum, false, false);
          scopira::basekit::mul_matrix(P2, P1, point_diff, false, true);
          dd = ::sqrt(0.5 * P2(0,0) + traceterms.get(cl1-1,cl2-1));
          outmatrix.set(y1, y2, dd);
          outmatrix.set(y2, y1, dd);
        }
      }
    }
    for (int y1=0; y1<h; y1++)
      outmatrix.set(y1, y1, 0);

    switch( sym_kl_divergence_result ) {
      case SUCCESS:
        return true;
      case NO_CLASS:
        msg= "sym_kl_divergence Distance calculation failed - no class info";
        return false;
      case NO_CENTROIDS:
       msg="sym_kl_divergence Distance calculation failed - need to add centroids first";
        return false;
      case LARGE_DATA:
        msg="sym_kl_divergence Distance calculation failed - too many attributes";
        return false;
      case SINGULAR:
        msg="sym_kl_divergence Distance calculation failed - singular matrix";
        return false;
    }
    
    // should not reach this, but stop the warning
    return false;
}

/*
 * Patrick-Fisher distance
 *
 * Takes the exponential of a covariance term, multiplies by a constant and
 * adds a different constant
 *
 * @author Brion Dolenko
 */
 
//
// Calling function - interfaces to distance matrix calculation
//

bool pakit::calc_patrick_fisher_distance(pakit::patterns_m* dm_model,pakit::distances_m *dm_distance, int cl1, int cl2, 
  double class_weight, string &msg)
{  
  int numclass;

    if ((dm_model->pm_data->width() < 1) || (dm_model->pm_data->height() < 2)) {
     msg="Input matrix must be at least 1X2";
     return false;
    }
    
    assert(dm_model->pm_classes->size() == dm_model->pm_data->height());
    assert(dm_model->pm_training->size() == dm_model->pm_data->height());
    assert(dm_model->pm_labels->size() == dm_model->pm_data->height());
    
    if ( ( cl1 <= 0 ) || ( cl2 <= 0 ) ) {
      msg="Classes must be positive.";
      return false;
    }
    
    max(dm_model->pm_classes.ref(), numclass);
    if ( ( cl1 > numclass ) || ( cl2 > numclass ) ) {
      msg="Classes must be "+int_to_string(numclass)+" at most.";
      return false;
    }
    
    if (!calc_distance_matrix_patrick_fisher(dm_model->pm_data.ref(), dm_model->pm_classes.ref(),
        dm_model->pm_labels.ref(), dm_distance->pm_array.ref(), cl1,cl2, msg)) 
      return false;
      
    msg ="Distance matrix (Patrick-Fisher, based on classes "+int_to_string(cl1)+" & "+int_to_string(cl2) + ")";
      
    return true;
}    

//
// Distance matrix calculation
//

bool pakit::calc_distance_matrix_patrick_fisher(narray<double,2> &inn, 
  const narray<int,1> &cls,
  const stringvector_o &labels,
  narray<double,2> &outmatrix,
  int cl1, int cl2, string &msg)
{
  size_t sz, h, w;
  int pidx, csidx, aidx, csidx1, csidx2;
  int centroids_present;
  stringstream tmpmsg;
  
  enum t_m_result { SUCCESS, NO_CLASS, NO_CENTROIDS, LARGE_DATA, SINGULAR } patrick_fisher_result;
  
  sz = inn.height();
  outmatrix.resize(sz, sz);
  
  h = inn.height();
  w = inn.width();
  patrick_fisher_result = SUCCESS;
  
  // Check for valid classes before doing anything
  bool cl1found = false;
  bool cl2found = false;
  int maxclass = 0;

  for (pidx = 0; pidx < cls.size(); ++pidx ) {
    if ( cls.get(pidx) == cl1 )
      cl1found = true;
    if ( cls.get(pidx) == cl2 )
      cl2found = true;
    if ( cls.get(pidx) > maxclass )
      maxclass = cls.get(pidx);
   }                                       
   if (( cl1 > maxclass ) || ( cl2 > maxclass )) {
     tmpmsg<<"Class out of range";
     return false;
   }
   if ( !cl1found ) {
     tmpmsg<<"Class "<<cl1<<" does not exist in the data set";
     msg=tmpmsg.str();
     return false;
   }
   if ( !cl2found ) {
     tmpmsg<<"Class "<<cl2<<" does not exist in the data set";
     msg=tmpmsg.str();
     return false;
   }
   
   // Count numbers in classes and check for too many attributes
   int numinclass1 = 0;
   int numinclass2 = 0;
   
   for (pidx = 0; pidx < cls.size(); pidx++ ) {
     if ( cls.get(pidx) == cl1 ) 
        numinclass1++;
     else if (cls.get(pidx) == cl2 ) // OK leads to an error, but not the expected one! 
        numinclass2++;
    }

    // See which classes are to be skipped over in computation (because nothing in them)
    boolvec_o placeholder_class; // set to true if nothing in the class

    placeholder_class.resize(maxclass);
    for(csidx = 0; csidx < maxclass; csidx++)
      placeholder_class.set(csidx, true);
    for(pidx = 0; pidx < cls.size(); ++pidx) {
      if ( cls.get( pidx ) >= 1 && cls.get( pidx ) <= maxclass ) {
        placeholder_class.set( cls.get( pidx ) - 1, false );
      }
    }

    doublequad_o covars_1;
    doublequad_o covars_2;
    doublequad_o covar_sums;
    doublequad_o inv_covar_sums;
    doublematrix_o covar1;
    doublematrix_o covar2;
    doublematrix_o covar_sum;
    doublematrix_o inv_covar_sum;

    while ( patrick_fisher_result == SUCCESS ) { 
      // Check for class info
      if ( maxclass < 2 ) {
        patrick_fisher_result = NO_CLASS;
        break;
      }

      // This will catch both classes being the same
      if ( ( numinclass1 == 0 ) || ( numinclass2 == 0 ) ) {
        patrick_fisher_result = NO_CLASS;
        break;
      }
      // Check for too many attributes
      if ( ( numinclass1 < w ) || ( numinclass2 < w ) ) {
        patrick_fisher_result = LARGE_DATA;
        break;
      }

      // Check whether centroids are present
      centroids_present = 0;
      for (pidx = 0; pidx < cls.size(); pidx++ ) {
        if ( cls.get(pidx) == -cl1 && labels.get(pidx).find( "centroid" ) != string::npos ) {
          centroids_present++;
        }
        if ( cls.get(pidx) == -cl2 && labels.get(pidx).find( "centroid" ) != string::npos )
          centroids_present++;
      }
      if ( centroids_present != 2 ) {
        patrick_fisher_result = NO_CENTROIDS;
        break;
      }

      // Yup, they're present.  Get the centroids
      doublematrix_o centroid; 
      doublematrix_o two_class_mean;

      centroid.resize( w, maxclass );
      two_class_mean.resize( w, 2 );
      for ( csidx = 1; csidx <= maxclass; csidx++ ) {
        for ( pidx=0; pidx<cls.size(); pidx++ ) {
          if (cls.get(pidx) == -csidx) { // Should we be assuming this?
                                          // but speeds up computation if we do
            for (aidx = 0; aidx < w; aidx++ ) {
              centroid( aidx, csidx-1 ) = inn.get( aidx, pidx );
            }
          }
        }
      }

      // Dimension the 1d arrays
      intvec_o class_list;
      intvec_o two_class_list;
      boolvec_o two_class_training_list;
      class_list.resize(h);
      two_class_list.resize(h);
      two_class_training_list.resize(h);

      // Fill the class list array
      for (pidx = 0; pidx < cls.size(); pidx++ )
        if ( cls.get(pidx) > 0 )
          class_list[pidx] = cls.get(pidx);
        else
          class_list[pidx] = 0;

      // Dimension the multi-dim arrays
      int c = maxclass;
      covars_1.resize(nindex<4>(w,w,c,c));
      covars_2.resize(nindex<4>(w,w,c,c));
      covar_sums.resize(nindex<4>(w,w,c,c));
      inv_covar_sums.resize(nindex<4>(w,w,c,c));
      covar1.resize(nindex<2>(w,w));
      covar2.resize(nindex<2>(w,w));
      covar_sum.resize(nindex<2>(w,w));
      inv_covar_sum.resize(nindex<2>(w,w));

      bool singular;

      // Loop over pairs of classes
      for (csidx1 = 0; csidx1 < maxclass; csidx1++ ) {
        if ( placeholder_class.get( csidx1 ) == true )
          continue;
        // For now, we're only computing covariance matrices for selected classes
        if ( ( csidx1+1 != cl1 ) && ( csidx1+1 != cl2 ) )
          continue;

        two_class_mean.xyslice(0,0,w,1).copy(centroid.xyslice(0,csidx1,w,1));

        for (csidx2 = 0; csidx2 < maxclass; csidx2++ ) {
          if ( placeholder_class.get( csidx2 ) == true )
            continue;
          if ( csidx2 == csidx1 ) {
            continue;
          }
          // For now, we're only computing covariance matrices for selected classes
          if ( ( csidx2+1 != cl1 ) && ( csidx2+1 != cl2 ) )
            continue;
          two_class_mean.xyslice(0,1,w,1).copy(centroid.xyslice(0,csidx2,w,1));
          for (pidx = 0; pidx < class_list.size(); pidx++ ) {
            if ( class_list[pidx] == csidx1+1 ) {
              two_class_list[pidx] = 1;
              two_class_training_list[pidx] = true;
            }
            else if ( class_list[pidx] == csidx2+1 ) {
              two_class_list[pidx] = 2;
              two_class_training_list[pidx] = true;
            }
            else {
              two_class_list[pidx] = 0;
              two_class_training_list[pidx] = false;
            }
          }
          
          calc_covar( covar1, two_class_mean, inn, &two_class_training_list, &two_class_list, 1 );
          calc_covar( covar2, two_class_mean, inn, &two_class_training_list, &two_class_list, 2 );
          for ( int aidx1 = 0; aidx1 < w; aidx1++ )
            for ( int aidx2 = 0; aidx2 < w; aidx2++ )
              covar_sum(aidx1, aidx2) = covar1(aidx1, aidx2) + covar2(aidx1, aidx2);
          singular = !invert_matrix(inv_covar_sum,covar_sum);
          if ( singular ) {
            patrick_fisher_result = SINGULAR;
            break; 
          }

          (covars_1.slicer(nindex<4>(0,0,csidx1,csidx2),nindex<2>(w,w))).copy(covar1);
          (covars_2.slicer(nindex<4>(0,0,csidx1,csidx2),nindex<2>(w,w))).copy(covar2);
          (covar_sums.slicer(nindex<4>(0,0,csidx1,csidx2),nindex<2>(w,w))).copy(covar_sum);
          (inv_covar_sums.slicer(nindex<4>(0,0,csidx1,csidx2),nindex<2>(w,w))).copy(inv_covar_sum);
        } // end loop over second class
        if ( patrick_fisher_result == SINGULAR  || patrick_fisher_result == LARGE_DATA ) {
          break; 
        }
      } // end loop over first class
      break;
    } // end while successful

    if ( patrick_fisher_result == SUCCESS ) {
      doublematrix_o point_diff;
      doublematrix_o P1, P2;
      doublematrix_o decomp;
      nslice<double,2> point1, point2, slice_point_diff;
      double dd;
      nslice<double,2> src = inn.all_slice();
      scopira::basekit::narray<int> vidx;
      bool odd;
      double det_term1, det_term2, det_term3, pi_term;

      decomp.resize(w,w);
      decomp.clear();
      vidx.resize(w);
      vidx.clear();

      // always use covariance matrix for selected classes 1 and 2
      covar1.copy(covars_1.slicer(nindex<4>(0,0,cl1-1,cl2-1),nindex<2>(w,w)));
      covar2.copy(covars_2.slicer(nindex<4>(0,0,cl1-1,cl2-1),nindex<2>(w,w)));
      covar_sum.copy(covar_sums.slicer(nindex<4>(0,0,cl1-1,cl2-1),nindex<2>(w,w)));
      inv_covar_sum.copy(inv_covar_sums.slicer(nindex<4>(0,0,cl1-1,cl2-1),nindex<2>(w,w)));

      scopira::basekit::mul_matrix(covar1, 2.0 );
      scopira::basekit::lu_decomposition( decomp, covar1, vidx, odd );
      det_term1 = 1.0;
      for ( int aidx = 0; aidx < w; aidx++ )
        det_term1 *= decomp( aidx, aidx ); 
      if ( odd )
        det_term1 *= -1.0;
      assert( det_term1 >= 0.0 );

      scopira::basekit::mul_matrix(covar2, 2.0 );
      scopira::basekit::lu_decomposition( decomp, covar2, vidx, odd );
      det_term2 = 1.0;
      for ( int aidx = 0; aidx < w; aidx++ )
        det_term2 *= decomp( aidx, aidx ); 
      if ( odd )
        det_term2 *= -1.0;
      assert( det_term2 >= 0.0 );

      scopira::basekit::lu_decomposition( decomp, covar_sum, vidx, odd );
      det_term3 = 1.0;
      for ( int aidx = 0; aidx < w; aidx++ )
        det_term3 *= decomp( aidx, aidx ); 
      if ( odd )
        det_term3 *= -1.0;
      assert( det_term3 >= 0.0 );

      pi_term = pow( 2 * M_PI, w * -0.5 );
      for (int y1=0; y1<h; ++y1) {
        point1 = src.xyslice(0, y1, w, 1);
        for (int y2=y1+1; y2<h; ++y2) {
          point2 = src.xyslice(0, y2, w, 1);
          point_diff.resize( w, 1 );
          for ( int aidx = 0; aidx < w; aidx++ ) {
            point_diff.set( aidx, 0, point1.get( aidx, 0 ) - point2.get( aidx, 0 ) );
          }
          slice_point_diff = point_diff.xyslice( 0, 0, w, 1 );
          scopira::basekit::mul_matrix(P1, point_diff, inv_covar_sum, false, false);
          scopira::basekit::mul_matrix(P2, P1, point_diff, false, true);
          dd = ::sqrt( pi_term * ( pow( det_term1, -0.5 ) + pow( det_term2, -0.5 )
                                   - 2.0 * pow( det_term3, -0.5 ) * exp(-0.5 * P2(0,0)) ) );
          outmatrix.set(y1, y2, dd);
          outmatrix.set(y2, y1, dd);
        }
      }
    }
    for (int y1=0; y1<h; y1++)
      outmatrix.set(y1, y1, 0);

    switch( patrick_fisher_result ) {
      case SUCCESS:
        return true;
      case NO_CLASS:
        msg= "Patrick-Fisher Distance calculation failed - no class info";
        return false;
      case NO_CENTROIDS:
       msg="Patrick-Fisher Distance calculation failed - need to add centroids first";
        return false;
      case LARGE_DATA:
        msg="Patrick-Fisher Distance calculation failed - too many attributes";
        return false;
      case SINGULAR:
        msg="Patrick-Fisher Distance calculation failed - singular matrix";
        return false;
    }
    
    // should not reach this, but stop the warning
    return false;
}

//
// Common code for all the compound distance measures!
//

void pakit::calc_distance_matrix_compound(scopira::basekit::narray<double,2> &patterns,
    const scopira::basekit::narray<double,2> &matrix_inverse,
    const double multiplicative_term, const double additive_term,
    scopira::basekit::narray<double,2> &outmatrix) {

  doublematrix_o point_diff;
  doublematrix_o P1, P2;
  nslice<double,2> point1, point2, slice_point_diff;
  double dd;
  size_t h, w;
  nslice<double,2> src = patterns.all_slice();
  h = patterns.height();
  w = patterns.width();

  for (int y1=0; y1<h; ++y1) {
    point1 = src.xyslice(0, y1, w, 1);
    for (int y2=y1+1; y2<h; ++y2) {
      point2 = src.xyslice(0, y2, w, 1);
      point_diff.resize( w, 1 );
      for ( int aidx = 0; aidx < w; aidx++ ) {
        point_diff.set( aidx, 0, point1.get( aidx, 0 ) - point2.get( aidx, 0 ) );
      }
      slice_point_diff = point_diff.xyslice( 0, 0, w, 1 );
      // always use covariance matrix for selected classes 1 and 2
      scopira::basekit::mul_matrix(P1, point_diff, matrix_inverse, false, false);
      scopira::basekit::mul_matrix(P2, P1, point_diff, false, true);
      dd = ::sqrt(multiplicative_term * P2(0,0) + additive_term);
      outmatrix.set(y1, y2, dd);
      outmatrix.set(y2, y1, dd);
    }
  }
  for (int y1=0; y1<h; y1++)
    outmatrix.set(y1, y1, 0);
}
