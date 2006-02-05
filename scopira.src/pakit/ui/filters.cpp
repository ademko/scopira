
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

#include <pakit/ui/filters.h>

#include <scopira/tool/sort_imp.h>
#include <scopira/tool/fileflow.h>
#include <scopira/tool/cacheflow.h>
#include <scopira/core/register.h>
#include <scopira/coreui/layout.h>
#include <scopira/coreui/label.h>
#include <scopira/basekit/vectormath.h>

#include <math.h>

//BBtargets libpauikit.so

using namespace scopira::tool;
using namespace scopira::coreui;
using namespace scopira::basekit;
using namespace pakit;

//
//
// register
//
//

static scopira::core::register_view<pattern_whiten_v> r1(
    "pakit::pattern_whiten_v", "pakit::patterns_m", "Filters/Whiten",
    scopira::core::windowonly_uimode_c);
static scopira::core::register_view<pattern_areanorm_v> r2(
    "pakit::pattern_areanorm_v", "pakit::patterns_m", "Filters/Area Normalization",
    scopira::core::windowonly_uimode_c);
static scopira::core::register_view<pattern_firstderiv_v> r3(
    "pakit::pattern_firstderiv_v", "pakit::patterns_m", "Filters/First Derivative",
    scopira::core::no_uimode_c);
static scopira::core::register_view<pattern_secondderiv_v> r4(
    "pakit::pattern_secondderiv_v", "pakit::patterns_m", "Filters/Second Derivative",
    scopira::core::no_uimode_c);
static scopira::core::register_view<pattern_rankorder_v> r5(
    "pakit::pattern_rankorder_v", "pakit::patterns_m", "Filters/Rank Order",
    scopira::core::windowonly_uimode_c);

/*
 * Areanorm - Normalize to unit area and zero mean 
 *
 @ author Brion Dolenko
 */

pattern_areanorm_v::pattern_areanorm_v(void)
  : dm_model(this)
{
  count_ptr<grid_layout> g;
  
  g = new grid_layout(1,2);

  dm_norm_scale = new checkbutton;
  dm_norm_scale->set_label( "Normalize Scale?" );
  dm_norm_scale->set_checked( true );
  g->add_widget(dm_norm_scale.get(), 0, 0);
  dm_norm_offset = new checkbutton;
  dm_norm_offset->set_label( "Normalize Offset?" );
  dm_norm_offset->set_checked( true );
  g->add_widget(dm_norm_offset.get(), 0, 1);

  viewwidget::init_gui(g.get(), button_ok_c|button_cancel_c);
}

void pattern_areanorm_v::bind_model(scopira::core::model_i *sus)
{
  dm_model = dynamic_cast<patterns_m*>(sus);
}

void pattern_areanorm_v::react_button(scopira::coreui::button *source, int actionid)
{
  if (actionid != action_apply_c) {
    viewwidget::react_button(source, actionid);
    return;
  }

  if (dm_model.is_null())
    return;

  double spect_area, abs_spect_area;
  double mult_factor, dc_factor;
  double currpoint;
  int npats, pidx;           // number of spectra
  int nattrs, aidx;          // attributes per spectrum

  patterns_t pat(false);

  pat.copy_patterns(dm_model.ref(), true);
  nattrs = pat.pm_data->width();
  npats = pat.pm_data->height();
  for ( pidx = 0; pidx < npats; pidx++ ) {
    spect_area = 0.0;
    abs_spect_area = 0.0;
    for ( aidx = 0; aidx < nattrs; aidx++ ) {
      currpoint = pat.pm_data->get( aidx, pidx );
      spect_area += currpoint;
      abs_spect_area += (currpoint > 0) ? currpoint : -currpoint;
    }

    if ( dm_norm_scale->is_checked() )
      mult_factor = nattrs / abs_spect_area;
    else
      mult_factor = 1.0;

    if ( dm_norm_offset->is_checked() )
      dc_factor = spect_area * mult_factor / nattrs;
    else
      dc_factor = 0.0;

    for ( aidx = 0; aidx < nattrs; aidx++ ) {
      currpoint = pat.pm_data->get( aidx, pidx );
      pat.pm_data->set( aidx, pidx, mult_factor * currpoint - dc_factor );
    }
  }
  if ( dm_norm_scale->is_checked() || dm_norm_offset->is_checked() )
    dm_model->set_title( dm_model->get_title() + " (Normalized:" );
  if ( dm_norm_scale->is_checked() )
    dm_model->set_title( dm_model->get_title() + " unit area" );
  if ( dm_norm_offset->is_checked() )
    dm_model->set_title( dm_model->get_title() + " zero offset" );
  if ( dm_norm_scale->is_checked() || dm_norm_offset->is_checked() )
    dm_model->set_title( dm_model->get_title() + ")" );
  dm_model->copy_patterns(pat, false);
  dm_model->notify_views(this);

  viewwidget::react_button(source, actionid);
}

/**
 * Firstderiv - Simple first derivative
 *
 @ author Brion Dolenko
 */

void pattern_firstderiv_v::bind_model(scopira::core::model_i *sus)
{
  patterns_m *target = dynamic_cast<patterns_m*>(sus);

  if (!target)
    return;

  int npats, pidx;             // number of spectra
  int nattrs, aidx;            // attributes per spectrum

  patterns_t pat(false);

  pat.copy_patterns(*target, true);
  nattrs = pat.pm_data->width();
  npats = pat.pm_data->height();

  for ( aidx = nattrs - 1; aidx > 0; aidx-- )
    for ( pidx = 0; pidx < npats; pidx++ )
      pat.pm_data->set( aidx, pidx, pat.pm_data->get( aidx, pidx ) - pat.pm_data->get( aidx-1, pidx ) );
  for ( pidx = 0; pidx < npats; pidx++ )
    pat.pm_data->set( 0, pidx, pat.pm_data->get( 1, pidx ) );

  target->set_title( target->get_title() + " (First Derivative)" );
  target->copy_patterns(pat, false );
  target->notify_views(this);
}

/**
 * Secondderiv - Simple second derivative
 *
 @ author Brion Dolenko
 */
void pattern_secondderiv_v::bind_model(scopira::core::model_i *sus)
{
  patterns_m *target = dynamic_cast<patterns_m*>(sus);

  if (!target)
    return;


  int npats, pidx;             // number of spectra
  int nattrs, aidx;            // attributes per spectrum

  patterns_t pat(false);
  narray<double,2> newVals;

  pat.copy_patterns(*target, true);
  nattrs = pat.pm_data->width();
  npats = pat.pm_data->height();
  newVals.resize( nattrs, npats );

  for ( aidx = 1; aidx < nattrs - 1; aidx++ )
    for ( pidx = 0; pidx < npats; pidx++ )
      newVals.set( aidx, pidx, pat.pm_data->get( aidx-1, pidx ) 
                               - 2.0 * pat.pm_data->get( aidx, pidx )
                               + pat.pm_data->get( aidx+1, pidx ) );

  for ( pidx = 0; pidx < npats; pidx++ ) {
    newVals.set( 0, pidx, newVals.get( 1, pidx ) );
    newVals.set( nattrs - 1, pidx, newVals.get( nattrs - 2, pidx ) );
  }

  for ( pidx = 0; pidx < npats; pidx++ )
    for ( aidx = 0; aidx < nattrs; aidx++ )
      pat.pm_data->set( aidx, pidx, newVals.get( aidx, pidx ) );

  target->set_title( target->get_title() + " (Second Derivative)" );
  target->copy_patterns(pat, false );
  target->notify_views(this);
}

/**
 * Rankorder - perform rank ordering on the data values
 *
 @ author Brion Dolenko
 */

// Adapted from code written in IRIS Explorer
// (so were whiten, areanorm, firstderiv, and secondderiv.  But this one
// had the following comments:)
// Written by Brion Dolenko
// 	      March 1999
// Purpose:  Rank order the original spectra to get a new set of spectra.
//           Each spectrum is processed independently.  Rank ordering
//           replaces each attribute by an integer rank that is the
//           position of the attribute within a sorted list, sorted
//           in ascending order according to the attribute's original value.
//           The integer rank is replaced by a float one to allow for 
//           compatibility with the other modules.
//
//        January 2003
//           Handle ties predictably now.
//           Use the "Golf tournament purse-splitting method".  Take the sum
//           of the indexes over the ties, and divide by the number tied.
//           So if 2nd, 3rd, and 4th are tied, assign 3 to all.
//           If 4th, 5th, 6th, and 7th are tied, assign 5.5 to all.
//           Interrank (rankPointsBetween) assigns these special tie-values
//           to any identical test spectra.

pattern_rankorder_v::pattern_rankorder_v(void)
  : dm_model(this)
{
  count_ptr<grid_layout> g;
  
  g = new grid_layout(2,1);

  g->add_widget(new label("Calculate Ranks:"), 0, 0, 1, 1, false, true);

  dm_method = new radiobutton;
  dm_method->add_selection(0, "Within Patterns");
  dm_method->add_selection(1, "Between Patterns");
  g->add_widget(dm_method.get(), 1, 0);

  viewwidget::init_gui(g.get(), button_ok_c|button_cancel_c);
}

void pattern_rankorder_v::bind_model(scopira::core::model_i *sus)
{
  dm_model = dynamic_cast<patterns_m*>(sus);
}

// Process each spectrum individually, as per the comments above

void pattern_rankorder_v::rankPointsWithin(patterns_t &pat) {
  narray_o<double> rankVec;
  narray_o<int>  indexVec;
  typedef vecindex_sortable<narray_o<int> , narray_o<double> > sortable_t;
  sortable_t sortable( indexVec, rankVec );
  double checkAttr;             // Used for tiebreaking
  int checkStart, checkStop;    //   ("Golf tournament purse-splitting" method)
  int sumTiedAttrs, nattrsTied; //   "
  int npats, pidx;              // number of spectra
  int nattrs, aidx, outaidx;    // attributes per spectrum

  nattrs = pat.pm_data->width();
  npats = pat.pm_data->height();
  rankVec.resize( nattrs );
  indexVec.resize( nattrs );

  for ( pidx = 0; pidx < npats; pidx++ ) {
    for ( aidx = 0; aidx < nattrs; aidx++ ) {
      rankVec[aidx] = pat.pm_data->get( aidx, pidx );
      indexVec[aidx] = aidx;
    }
    scopira::tool::qsort(sortable, 0, nattrs-1);
    checkStart = 0;
    sumTiedAttrs = 1;
    checkAttr = rankVec[indexVec[0]];
    for ( aidx = 1; aidx < nattrs; aidx++ ) {
      if ( rankVec[indexVec[aidx]] == checkAttr ) {
        sumTiedAttrs += aidx + 1;
        continue;
      }
      checkStop = aidx;
      nattrsTied = checkStop - checkStart;
      for ( outaidx = checkStart; outaidx < checkStop; outaidx++ ) {
        pat.pm_data->set( indexVec[outaidx], pidx, sumTiedAttrs / static_cast<double>(nattrsTied) );
      }
      checkStart = checkStop;
      sumTiedAttrs = checkStop + 1;
      checkAttr = rankVec[indexVec[checkStart]];
    }
    // last attribute!
    checkStop = nattrs;
    nattrsTied = checkStop - checkStart;
    for ( outaidx = checkStart; outaidx < checkStop; outaidx++ ) {
      pat.pm_data->set( indexVec[outaidx], pidx, sumTiedAttrs / static_cast<double>(nattrsTied) );
    }
  }
}

// Process each attribute independently.  Slot the ranks for test samples between the ranks for training samples

void pattern_rankorder_v::rankPointsBetween(patterns_t &pat) {
  narray_o<double> rankVec;
  narray_o<int>  indexVec;
  typedef vecindex_sortable<narray_o<int> , narray_o<double> > sortable_t;
  sortable_t sortable( indexVec, rankVec );
  double huge = 1.0e+30;       // When rank ordering training data, substitute this for test data values
  double currattr;             // Current validation data element
  double finalidx;             // Rank for validation data
  double checkPat;             // Used for tiebreaking
  int checkStart, checkStop;   //   ("Golf tournament purse-splitting" method)
  int sumTiedPats, npatsTied;  //   "
  int startjumplen, jumplen;   // jump by this amount as we search sorted vector
  int curridx;                 // index within sorted vector
  int npats, pidx;             // number of spectra
  int ntrain, tridx, outtridx; // number of training spectra
  int nattrs, aidx;            // attributes per spectrum
  bool fluke;                  // true for exact match with training data element

  finalidx = -1; // to suppress the warning

  nattrs = pat.pm_data->width();
  npats = pat.pm_data->height();
  rankVec.resize( npats );
  indexVec.resize( npats );

  ntrain = 0;
  for ( pidx = 0; pidx < npats; pidx++ )
    if ( pat.pm_training->get(pidx) == true )
      ntrain++;

  for ( aidx = 0; aidx < nattrs; aidx++ ) {

    // First rank order the training data
    for ( pidx = 0; pidx < npats; pidx++ ) {
      if ( pat.pm_training->get(pidx) == true ) {
        rankVec[pidx] = pat.pm_data->get( aidx, pidx );
      }
      else {
        rankVec[pidx] = huge;
      }
      indexVec[pidx] = pidx;
    }
    scopira::tool::qsort(sortable, 0, npats-1);
    checkStart = 0;
    sumTiedPats = 2;
    checkPat = rankVec[indexVec[0]];
    for ( tridx = 1; tridx < ntrain; tridx++ ) {
      if ( pat.pm_data->get( aidx, indexVec[tridx] ) == checkPat ) {
        sumTiedPats += (tridx + 1) * 2;
        // ...double the pattern index so that the validation rank ordered
        // values can slot between the training values and still be kept
        // integers (usually)
        continue;
      }
      checkStop = tridx;
      npatsTied = checkStop - checkStart;
      for ( outtridx = checkStart; outtridx < checkStop; outtridx++ ) {
        pat.pm_data->set( aidx, indexVec[outtridx], sumTiedPats / static_cast<double>(npatsTied) );
      }
      checkStart = checkStop;
      sumTiedPats = (checkStop + 1) * 2;
      checkPat = pat.pm_data->get( aidx, indexVec[checkStart] );
    }
    // last pattern!
    checkStop = ntrain;
    npatsTied = checkStop - checkStart;
    for ( outtridx = checkStart; outtridx < checkStop; outtridx++ ) {
      pat.pm_data->set( aidx, indexVec[outtridx], sumTiedPats / static_cast<double>(npatsTied) );
    }

    // Rank order the validation data through interpolation.  Use binary
    // search on sorted training attributes.  Can't use bsearch() because
    // likely none of the validation attributes will match training ones

    startjumplen = static_cast<int>( pow( 2.0, int( ::log(static_cast<double>(ntrain - 1)) / ::log(2.0) ) ) );
    for ( pidx = 0; pidx < npats; pidx++ ) {
      if ( pat.pm_training->get(pidx) == true )
        continue;
      currattr = pat.pm_data->get( aidx, pidx );
      fluke = false;

      // Take care of the special cases first - off one of the ends
      if ( currattr < rankVec[indexVec[0]] ) {
        finalidx = 1;
        fluke = true;
      }
      else if ( currattr > rankVec[indexVec[ntrain-1]] ) {
        finalidx = ntrain * 2 + 1;
        fluke = true;
      }
      curridx = ntrain / 2;
      jumplen = startjumplen;

      while ( ( jumplen > 1 ) && ( fluke == false ) ) {

        jumplen /= 2;
        if ( currattr < rankVec[indexVec[curridx]] ) {
          curridx -= jumplen;
        }
        else if ( currattr > rankVec[indexVec[curridx]] ) {
          curridx += jumplen;
        }
        else { // the unlikely case of an exact match.
               // set to whatever the output is set to for the
               // corresponding training pattern (in case that was set
               // to a special value due to a tie)
          fluke = true;
          finalidx = pat.pm_data->get( aidx, indexVec[curridx] );
          // Insert random perturbation code to tweak finalidx here, 
          // if you don't want any duplicate attributes
        }
        if ( curridx < 0 ) { // must have jumped here by 2 or more,
                             // otherwise special case would have caught.
                             // Set to 1.
          curridx = 1;
        }
        else if ( curridx >= ntrain ) { // Jumped here by 2 or more.
                                        // set to ntrain-2.
          curridx = ntrain - 2;
        }
      }
      if ( fluke == false ) {
        if ( currattr < rankVec[indexVec[curridx]] ) {
          finalidx = ( ( curridx - 0.5 ) + 1 ) * 2;
        }
        else if ( currattr > rankVec[indexVec[curridx]] ) {
          finalidx = ( ( curridx + 0.5 ) + 1 ) * 2;
        }
        else { // this is a fluke
          finalidx = ( curridx + 1 ) * 2;
          // Insert random perturbation code to tweak finalidx here, 
          // if you don't want any duplicate attributes
        }
      }
      pat.pm_data->set( aidx, pidx, finalidx );
    } // end loop over validation data
  } // end loop over attributes
}
 
// Main routine

void pattern_rankorder_v::react_button(scopira::coreui::button *source, int actionid)
{
  if (actionid != action_apply_c) {
    viewwidget::react_button(source, actionid);
    return;
  }

  if (dm_model.is_null())
    return;

  // make sure the method is valid
  assert((dm_method->get_selection() >= 0) &&
         (dm_method->get_selection() <= 1));

  patterns_t pat(false);

  pat.copy_patterns(dm_model.ref(), true);
  switch(dm_method->get_selection()) {
    case 0: // within patterns
      rankPointsWithin(pat);
      dm_model->set_title( dm_model->get_title() + " (rank ordered)" );
      break;
    case 1: // mean
      rankPointsBetween(pat);
      dm_model->set_title( dm_model->get_title() + " (between-pattern rank ordered)" );
      break;
  }
  dm_model->copy_patterns(pat, false);
  dm_model->notify_views(this);

  viewwidget::react_button(source, actionid);
}

/**
 * Whiten - normalize the patterns
 *
 @ author Brion Dolenko
 */
pattern_whiten_v::pattern_whiten_v(void)
  : dm_model(this)
{
  count_ptr<grid_layout> g;
  
  g = new grid_layout(2,2);

  g->add_widget(new label("Whitening method:"), 0, 0, 1, 1, false, true);

  dm_method = new radiobutton;
  dm_method->add_selection(0, "Within-Pattern Mean");
  dm_method->add_selection(1, "Mean");
  dm_method->add_selection(2, "Median");
  dm_method->add_selection(3, "Apply parameters from file");
  g->add_widget(dm_method.get(), 1, 0);

  g->add_widget(new label("Parameter file:"), 0, 1);
  dm_parmsfile = new fileentry;
  g->add_widget(dm_parmsfile.get(), 1, 1);

  viewwidget::init_gui(g.get(), button_ok_c|button_cancel_c);
}

void pattern_whiten_v::bind_model(scopira::core::model_i *sus)
{
  dm_model = dynamic_cast<patterns_m*>(sus);
}

// The way I used to do it... More of a within-spectrum normalization really.
// For each attribute, subtract the spectrum's mean and divide by the spectrum's standard deviation
  
void pattern_whiten_v::whitenMeanWithin(patterns_t &pat, narray_o<double> &subtractVec, 
                                        narray_o<double> &divideVec) {
  int i, j;
  double currpoint;
  double sum, sumsqr;
  double avg, devn;
  int nattrs = pat.pm_data->width();
  int npats = pat.pm_data->height();
  for ( i = 0; i < npats; i++ ) {   // pattern index
    // Skip the centroids
    if ( pat.pm_classes->get(i) <= 0 )
      continue;
    sum = 0.0;
    sumsqr = 0.0;
    for ( j = 0; j < nattrs; j++ ) { // point index
      currpoint = pat.pm_data->get( j, i );
      sum += currpoint;
      sumsqr += currpoint * currpoint;
    }
    avg = sum / nattrs;
    devn = ::sqrt( sumsqr / nattrs - avg * avg );
    for ( j = 0; j < nattrs; j++ ) {
      currpoint = pat.pm_data->get( j, i );
      pat.pm_data->set( j, i, ( currpoint - avg ) / devn );
    }
  }
  // subtractVec and divideVec aren't really valid here, so just set to unity
  // in case some downstream routine is foolish enough to look at these
  for ( j = 0; j < nattrs; j++ ) {
    subtractVec.set( j, 0.0 );
    divideVec.set( j, 1.0 );
  }
}

// The "new" way of doing it (across spectra) - but the results can look like garbage!
// For each attribute, subtract the mean of that attribute and divide by standard deviation,
// both computed across all training spectra

void pattern_whiten_v::whitenMean(patterns_t &pat,
                                  narray_o<double> &subtractVec, narray_o<double> &divideVec) {
  double currpoint;
  double sum, sumsqr;
  double avg, devn;
  int i, j;
  int nattrs = pat.pm_data->width();
  int npats = pat.pm_data->height();
  int ntrain = 0;
  for ( i = 0; i < npats; i++ )
    if ( pat.pm_training->get(i) == true )
      ntrain++;
  for ( j = 0; j < nattrs; j++ ) {  // point index
    sum = 0.0;
    sumsqr = 0.0;
    for ( i = 0; i < npats; i++ ) { // pattern index
      if ( pat.pm_training->get(i) == false ) // test patterns don't count
        continue;
      currpoint = pat.pm_data->get( j, i );    
      sum += currpoint;
      sumsqr += currpoint * currpoint;
    }
    avg = sum / ntrain;
    devn = sqrt( sumsqr / ntrain - avg * avg );
    subtractVec.set( j, avg );
    divideVec.set( j, devn );
    for ( i = 0; i < npats; i++ ) {
      currpoint = pat.pm_data->get( j, i );
      pat.pm_data->set( j, i, ( currpoint - avg ) / devn );
    }
  }
}

// For each attribute, subtract the median of that attribute and divide by the interquartile difference,
// both computed across all training spectra

void pattern_whiten_v::whitenMedian(patterns_t &pat,
                                    narray_o<double> &subtractVec, narray_o<double> &divideVec) {
  narray_o<double> rankVec;
  narray_o<int>  indexVec;
  typedef vecindex_sortable<narray_o<int> , narray_o<double> > sortable_t;
  sortable_t sortable( indexVec, rankVec );
  double currpoint;
  double median, interquart;
  int sidepts;
  int i, j, trainidx;
  int medidx, iquartidx;
  int nattrs = pat.pm_data->width();
  int npats = pat.pm_data->height();
  int ntrain = 0;
  for ( i = 0; i < npats; i++ )
    if ( pat.pm_training->get(i) == true )
      ntrain++;
  if ( ntrain <= 1 ) // method won't work
    return;
  rankVec.resize( ntrain );
  indexVec.resize( ntrain );

  for ( j = 0; j < nattrs; j++ ) {  // point index
    for ( i = 0, trainidx = 0; i < npats; i++ ) {  // pattern index
      if ( pat.pm_training->get(i) == false ) // test patterns don't count
        continue;
      rankVec[trainidx] = pat.pm_data->get( j, i );
      indexVec[trainidx] = trainidx;
      trainidx++;
    }
    scopira::tool::qsort(sortable, 0, ntrain-1);
    medidx = ntrain/2;
    iquartidx = medidx/2;
    if ( ntrain % 2 == 1 ) {
      median = rankVec[indexVec[medidx]];
      sidepts = medidx+1;
      if ( sidepts % 2 == 1 ) {
        interquart = rankVec[indexVec[iquartidx+medidx]] - rankVec[indexVec[iquartidx]];
      }
      else {
        interquart = ( rankVec[indexVec[iquartidx+medidx]] + rankVec[indexVec[iquartidx+medidx+1]]
                       - rankVec[indexVec[iquartidx]] - rankVec[indexVec[iquartidx+1]] ) / 2.0;
      }
    }
    else {
      median = ( rankVec[indexVec[medidx]] + rankVec[indexVec[medidx - 1]] ) / 2.0;
      sidepts = medidx;
      if ( sidepts % 2 == 1 ) {
        interquart = rankVec[indexVec[iquartidx+medidx]] - rankVec[indexVec[iquartidx]];
      }
      else {
        interquart = ( rankVec[indexVec[iquartidx+medidx]] + rankVec[indexVec[iquartidx+medidx-1]]
                       - rankVec[indexVec[iquartidx]] - rankVec[indexVec[iquartidx-1]] ) / 2.0;
      }
    }         
    subtractVec[j] = median;
    divideVec[j] = interquart;
    for ( i = 0; i < npats; i++ ) {
      currpoint = pat.pm_data->get( j, i );
      pat.pm_data->set( j, i, ( currpoint - median ) / interquart );
    }
  }
}
 
// Save normalization vectors to file.
// One vector to subtract, and one to divide by

void pattern_whiten_v::outputParms(narray_o<double> &subtractVec,
                                   narray_o<double> &divideVec) {
  printoflow fout(true,0);
  std::string parmsFilename;

  parmsFilename = dm_parmsfile->get_filename();
  if ( parmsFilename == "" )
    return;

  int nattrs = subtractVec.size();
  if ( nattrs == 0 )
    return;
  assert( divideVec.size() == nattrs );

  fout.open(new cacheoflow(true, new fileflow(parmsFilename, fileflow::output_c)));
  if (fout.failed()) {
    //set_error_label( "Failed to open file for writing: " + parmsFilename );
    return;
  }

  for ( int aidx = 0; aidx < nattrs; aidx++ ) {
    fout.write_double( subtractVec.get(aidx) );
    fout.printf( " " );
  }
  fout.printf( "\n" );
  for ( int aidx = 0; aidx < nattrs; aidx++ ) {
    fout.write_double( divideVec.get(aidx) );
    fout.printf( " " );
  }
  fout.printf( "\n" );
  fout.close();
}

// Grab normalization vectors previously saved to file.
// One vector to subtract, and one to divide by

bool pattern_whiten_v::whitenFromFile(patterns_t &pat) {

  printiflow fin(true,0);
  std::string parmsFilename;
  narray_o<double> subtractVec;
  narray_o<double> divideVec;
  double currpoint;
  int i, j;

  parmsFilename = dm_parmsfile->get_filename();
  if ( parmsFilename == "" )
    return(false);
  fin.open(new cacheiflow(true, new fileflow(parmsFilename, fileflow::input_c)));
  if (fin.failed()) {
    //set_error_label( "Failed to open file for reading: " + parmsFilename );
    return(false);
  }
  int nattrs = pat.pm_data->width();
  int npats = pat.pm_data->height();
  subtractVec.resize( nattrs );
  divideVec.resize( nattrs );

  for ( int aidx = 0; aidx < nattrs; aidx++ ) {
    if ( fin.failed() ) {
      //set_error_label( "Incomplete whitening parameters file!" );
      return(false);
    }
    fin.read_double( subtractVec[aidx] );
  }
  for ( int aidx = 0; aidx < nattrs; aidx++ ) {
    if ( fin.failed() ) {
      //set_error_label( "Incomplete whitening parameters file!" );
      return(false);
    }
    fin.read_double( divideVec[aidx] );
  }
  fin.close();
  for ( j = 0; j < nattrs; j++ ) {
    for ( i = 0; i < npats; i++ ) {
      currpoint = pat.pm_data->get( j, i );
      pat.pm_data->set( j, i, ( currpoint - subtractVec.get(j) ) / divideVec.get(j) );
    }
  }
  return(true);
}

// Main routine

void pattern_whiten_v::react_button(scopira::coreui::button *source, int actionid)
{
  if (actionid != action_apply_c) {
    viewwidget::react_button(source, actionid);
    return;
  }

  if (dm_model.is_null())
    return;

  narray_o<double> subtractVec;   // subtract this vector from each pattern
  narray_o<double> divideVec;     // ...then divide by this vector
  std::string parmsFilename; // name of file for parameter I/O
  int nattrs;                // attributes per spectrum

  // make sure the method is valid
  assert((dm_method->get_selection() >= 0) &&
         (dm_method->get_selection() <= 3));

  patterns_t pat(false);

  pat.copy_patterns(dm_model.ref(), true);
  nattrs = pat.pm_data->width();
  subtractVec.resize(nattrs);
  divideVec.resize(nattrs);
  parmsFilename = dm_parmsfile->get_filename();

  //clear_error_label();
  switch(dm_method->get_selection()) {
    case 0: // within-pattern mean
      whitenMeanWithin(pat, subtractVec, divideVec);
      dm_model->set_title( dm_model->get_title() + " (within-pattern-mean whitened)" );
      break;
    case 1: // mean
      whitenMean(pat, subtractVec, divideVec);
      outputParms(subtractVec, divideVec);
      dm_model->set_title( dm_model->get_title() + " (mean-whitened)" );
      break;
    case 2: // median
      whitenMedian(pat, subtractVec, divideVec);
      outputParms(subtractVec, divideVec);
      dm_model->set_title( dm_model->get_title() + " (median-whitened)" );
      break;
    case 3: // from file
      if ( parmsFilename == "" ) {
        //set_error_label("Need to supply a file name");
        return;
      }
      if ( whitenFromFile(pat) )
        dm_model->set_title( dm_model->get_title() + " (whitened according to file " + parmsFilename + " )" );
      break;
  }
  dm_model->copy_patterns(pat, false);
  dm_model->notify_views(this);

  viewwidget::react_button(source, actionid);
}
