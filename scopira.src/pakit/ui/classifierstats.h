
/*
 *  Copyright (c) 2005    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#ifndef __INCLUDED_CLASSIFIER_STATISTICS_H__
#define __INCLUDED_CLASSIFIER_STATISTICS_H__

#include <scopira/core/view.h>
#include <scopira/coreui/output.h>

#include <pakit/ui/models.h>

namespace pakit
{
  class classifierstats_d;
}

// Constants for classifierStats_d and printclassnstats_v.  Ugh.

const int OLEVEL_NOOUT = 1;
const int OLEVEL_SCORESONLY = 2;
const int OLEVEL_TABLESONLY = 3;
const int OLEVEL_EVERYTHING = 4;
const int OLEVEL_INVALID = 5;

const int OUTPUTRES_TRAIN = ( 1 << 0 );
const int OUTPUTRES_TEST = ( 1 << 1 );
const int OUTPUTRES_VAL = ( 1 << 2 );
const int OUTPUTRES_NONFUZZY = ( 1 << 3 );
const int OUTPUTRES_GROUPED = ( 1 << 4 );
const int OUTPUTRES_AVERAGED = ( 1 << 5 );
const int OUTPUTRES_STDDEVN = ( 1 << 6 );

const long MODEL_TRAIN = 0;
const long MODE_TEST = 1;
const int HEADERLEN = 20;

const double N_A = -1.0e30;

/**
 * Class for storing classification statistics.
 * Ported from some reeeeealy old software.
 *
 * @author Brion Dolenko
 */

class pakit::classifierstats_d
{
  private:
    int iterations; // number of iterations for averaging, or standard deviations
    int nclas;    // number of classes
    int resultType;     // training and crisp only, test, etc.
    int totalAssigned;  // total number of data vectors classified
    double realTotalAssigned; // the above, converted to double (for averages)
    scopira::basekit::intvec_o npatsInClass;  // number of data vectors in a class
    scopira::basekit::doublevec_o realNpatsInClass; // the above, converted to double (for averages)
    scopira::basekit::intvec_o npatsAssignedToClass; // number of data vectors assigned to class
    scopira::basekit::doublevec_o acc;          // accuracy for a class
    double agreement; // measure of agreement ("kappa" or K(0.5,0))
    double agreementInvl; // half width of agreement 95 % conf. interval
    scopira::basekit::doublevec_o K1Agreement;  // K(1,0)
    scopira::basekit::doublevec_o K0Agreement;  // K(0,0)
    scopira::basekit::doublevec_o lift;     // how much the classifications are "lifted" above random
    scopira::basekit::doublevec_o npv;    // negative predictive value:  TN / ( TN + FN )
    double overallAccuracy; // percent classification accuracy
    double overallPercentCrisp; // percent crisp overall
    scopira::basekit::doublevec_o percentCrisp; // percent crisp for a class
    scopira::basekit::doublevec_o probTableColumnSum;   // sums of columns in probability table
    scopira::basekit::doublevec_o ppv;    // positive predictive value:  TP / ( TP + FP )
    scopira::basekit::doublevec_o specificity;  // TN / ( TN + FP )
    double Q1;        // 100 * agreement * overallPercentCrisp ^ 0.5
    double Q2;        // average of 100 * agreement, overall accuracy, PPV values
    // ...all on crisp data only
    scopira::basekit::intmatrix_o classificationTable;  // confusion matrix
    scopira::basekit::doublematrix_o probTable;   // confusion matrix converted to probabilities

  public:
    classifierstats_d();
    classifierstats_d( bool init, int numClass, int resType );
    void copy_stats( const classifierstats_d &copiedStats, bool deepcopy );
    void set_defaults();
    void set_non_pointer_defaults();

    // What the heck was this?
    //virtual data_i *spawn(void) const;

    int computeClassificationStats();
    void computeQuality();
    int outputStats(scopira::tool::printoflow &outfile);

    // Access functions

    // Classification table must be filled from the outside

    void incrTableEntry( int csidx1, int csidx2 )
    { classificationTable.set(csidx1,csidx2,classificationTable.get(csidx1,csidx2)+1); }

    // Percentages crisp must be set from the outside

    void setPercentCrisp( int csidx, double setting )
    { percentCrisp[csidx] = setting; }
    void setOverallPercentCrisp( double setting )
    { overallPercentCrisp = setting; }

    // When taking averages or standard deviations, accept other
    // stats classes as input.  Use their contents for updating.

    void updateStdDevn( const classifierstats_d &averageStats,
        const classifierstats_d &updateStats );
    void updateAverage( const classifierstats_d &updateStats );

    // Everything can be read from the outside, except probability table
    // is internal-only

    int getTotalAssigned() { return totalAssigned; }
    int getNPatsInClass( int csidx ) { return npatsInClass[csidx]; }
    int getNPatsAssignedToClass( int csidx ) { return npatsAssignedToClass[csidx]; }
    double getAcc( int csidx ) { return acc[csidx]; }
    double getAgreement() { return agreement; }
    double getAgreementInvl() { return agreementInvl; }
    double getK0Agreement( int csidx ) { return K0Agreement[csidx]; }
    double getK1Agreement( int csidx ) { return K1Agreement[csidx]; }
    double getLift( int csidx ) { return lift[csidx]; }
    double getNPV( int csidx ) { return npv[csidx]; }
    double getOverallAccuracy() { return overallAccuracy; }
    double getOverallPercentCrisp() { return overallPercentCrisp; }
    double getPercentCrisp( int csidx ) { return percentCrisp[csidx]; }
    double getPPV( int csidx ) { return ppv[csidx]; }
    double getSpecificity( int csidx ) { return specificity[csidx]; }
    double getQ1() { return Q1; }
    double getQ2() { return Q2; }
    int  getClassnTableEntry( int csidx1, int csidx2 )
    { return classificationTable(csidx1,csidx2); }
    double getProbTableEntry( int csidx1, int csidx2 )
    { return probTable(csidx1,csidx2); }
};

#endif

