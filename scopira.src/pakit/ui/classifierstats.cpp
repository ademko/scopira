
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

#include <pakit/ui/classifierstats.h>

#include <scopira/basekit/math.h>
#include <scopira/basekit/narray.h>
#include <scopira/core/register.h>

//BBtargets libpauikit.so

using namespace scopira::basekit;
using namespace scopira::coreui;
using namespace scopira::tool;
using namespace pakit;

/* Assign a text string to an agreement measure */

void assignAgreementLabel( char *labelstr, double agreement ) {

    if ( agreement <= 0.0 )
        strcpy( labelstr, "poor" );
    else if ( agreement <= 0.20 )
        strcpy( labelstr, "slight" );
    else if ( agreement <= 0.40 )
        strcpy( labelstr, "fair" );
    else if ( agreement <= 0.60 )
        strcpy( labelstr, "moderate" );
    else if ( agreement <= 0.80 )
        strcpy( labelstr, "substantial" );
    else if ( agreement < 1.00 )
        strcpy( labelstr, "almost perfect" );
    else
        strcpy( labelstr, "perfect" );
    return;
}

void updateParmAvg( double &parmavg, double newparm, int iter ) {

	double floatiter = (double)iter;
	if ( newparm == N_A ) // treat as zero
		parmavg = floatiter * parmavg / (floatiter+1.0);
	else
		parmavg = floatiter * parmavg / (floatiter+1.0) + newparm / (floatiter+1.0);		
}

void updateParmDevn( double &parmdevn, double parmavg, double newparm, int iter ) {

	double floatiter = (double)iter;
	if ( newparm == N_A ) // treat as zero
		parmdevn = sqrt( floatiter * ( pow( parmdevn, 2.0 ) / ( floatiter + 1.0 ) +
	                 	      pow( ( parmavg ) / ( floatiter + 1.0 ), 2.0 ) ) );
	else
		parmdevn = sqrt( floatiter * ( pow( parmdevn, 2.0 ) / ( floatiter + 1.0 ) +
	                 	      pow( ( parmavg - newparm ) / ( floatiter + 1.0 ), 2.0 ) ) );
}

/* Clear the statistics including arrays - also called by constructor */

void classifierstats_d::set_defaults() {

	  iterations = 0;
    totalAssigned = 0;
    realTotalAssigned = 0.0;
    agreement = 0.0;
    agreementInvl = 0.0;
    overallAccuracy = 0.0;
    overallPercentCrisp = 0.0;
    Q1 = 0.0;
    Q2 = 0.0;
    if ( nclas <= 0 )
        return;
    for ( int csidx1 = 0; csidx1 <= nclas; csidx1++ ) {
        npatsInClass[csidx1] = 0;
        realNpatsInClass[csidx1] = 0.0;
        npatsAssignedToClass[csidx1] = 0;
        probTableColumnSum[csidx1] = 0.0;
        acc[csidx1] = 0.0;
        lift[csidx1] = 0.0;
        npv[csidx1] = 0.0;
        percentCrisp[csidx1] = 0.0;
        ppv[csidx1] = 0.0;
        specificity[csidx1] = 0.0;
        K0Agreement[csidx1] = 0.0;
        K1Agreement[csidx1] = 0.0;
        for ( int csidx2 = 0; csidx2 <= nclas; csidx2++ ) {
            classificationTable(csidx1,csidx2) = 0;
            probTable(csidx1,csidx2) = 0.0;
        }
    }
}

/* Clear everything except arrays */

void classifierstats_d::set_non_pointer_defaults() {

	  iterations = 0;
    totalAssigned = 0;
    realTotalAssigned = 0.0;
    agreement = 0.0;
    agreementInvl = 0.0;
    overallAccuracy = 0.0;
    overallPercentCrisp = 0.0;
    Q1 = 0.0;
    Q2 = 0.0;
    if ( nclas <= 0 )
        return;
}

/* Constructor */

classifierstats_d::classifierstats_d(bool init, int numClass, int resType )
{
  if (init) {
    nclas = numClass;
    resultType = resType;
    npatsInClass.resize(nclas+1);
    realNpatsInClass.resize(nclas+1);
    npatsAssignedToClass.resize(nclas+1);
    acc.resize(nclas+1);
    lift.resize(nclas+1);
    npv.resize(nclas+1);
    percentCrisp.resize(nclas+1);
    ppv.resize(nclas+1);
    probTableColumnSum.resize(nclas+1);
    specificity.resize(nclas+1);    
    K0Agreement.resize(nclas+1);
    K1Agreement.resize(nclas+1);
    classificationTable.resize( nclas+1, nclas+1 );
    probTable.resize( nclas+1, nclas+1 );
    set_defaults();
  }
  else {
    nclas = 0;
    resultType = 0;
    set_non_pointer_defaults();
  }
}

classifierstats_d::classifierstats_d() {

    nclas = 0;
    resultType = 0;
    set_non_pointer_defaults();
}

/* "Copy constructor" */

void classifierstats_d::copy_stats( const classifierstats_d &copiedStats, bool deepcopy )
{
    nclas = copiedStats.nclas;
	
    npatsInClass.resize(nclas+1);
    realNpatsInClass.resize(nclas+1);
    npatsAssignedToClass.resize(nclas+1);
    acc.resize(nclas+1);
    lift.resize(nclas+1);
    npv.resize(nclas+1);
    percentCrisp.resize(nclas+1);
    ppv.resize(nclas+1);
    probTableColumnSum.resize(nclas+1);
    specificity.resize(nclas+1);
    K0Agreement.resize(nclas+1);
    K1Agreement.resize(nclas+1);
    classificationTable.resize( nclas+1, nclas+1 );
    probTable.resize( nclas+1, nclas+1 );

    // Now copy the data

    resultType = copiedStats.resultType;
    totalAssigned = copiedStats.totalAssigned;
    realTotalAssigned = copiedStats.realTotalAssigned;
    agreement = copiedStats.agreement;
    agreementInvl = copiedStats.agreementInvl;
    overallAccuracy = copiedStats.overallAccuracy;
    overallPercentCrisp = copiedStats.overallPercentCrisp;
    Q1 = copiedStats.Q1;
    Q2 = copiedStats.Q2;

    if ( deepcopy ) {
      for ( int csidx1 = 0; csidx1 <= nclas; csidx1++ ) {
        npatsInClass[csidx1] = copiedStats.npatsInClass[csidx1];
        realNpatsInClass[csidx1] = copiedStats.realNpatsInClass[csidx1];
        npatsAssignedToClass[csidx1] = copiedStats.npatsAssignedToClass[csidx1];
        acc[csidx1] = copiedStats.acc[csidx1];
        lift[csidx1] = copiedStats.lift[csidx1];
        npv[csidx1] = copiedStats.npv[csidx1];
        percentCrisp[csidx1] = copiedStats.percentCrisp[csidx1];
        ppv[csidx1] = copiedStats.ppv[csidx1];
        probTableColumnSum[csidx1] = copiedStats.probTableColumnSum[csidx1];
        specificity[csidx1] = copiedStats.specificity[csidx1];
        K0Agreement[csidx1] = copiedStats.K0Agreement[csidx1];
        K1Agreement[csidx1] = copiedStats.K1Agreement[csidx1];
        for ( int csidx2 = 0; csidx2 <= nclas; csidx2++ ) {
          classificationTable(csidx1,csidx2) = 
            copiedStats.classificationTable(csidx1,csidx2);
          probTable(csidx1,csidx2) = copiedStats.probTable(csidx1,csidx2);
        }
      }
    }
    else {
      npatsInClass = copiedStats.npatsInClass;
      realNpatsInClass = copiedStats.realNpatsInClass;
      npatsAssignedToClass = copiedStats.npatsAssignedToClass;
      acc = copiedStats.acc;
      lift = copiedStats.lift;
      npv = copiedStats.npv;
      percentCrisp = copiedStats.percentCrisp;
      ppv = copiedStats.ppv;
      probTableColumnSum = copiedStats.probTableColumnSum;
      specificity = copiedStats.specificity;
      K0Agreement = copiedStats.K0Agreement;
      K1Agreement = copiedStats.K1Agreement;
      classificationTable = copiedStats.classificationTable;
      probTable = copiedStats.probTable;
    }
    iterations = copiedStats.iterations;
}

/* Compute the statistics, given that the classification table has
   already been filled. 

   Return value is the total number assigned to any class */

int classifierstats_d::computeClassificationStats() {

    double agreementVarn;         // variance of agreement measure
    double bigTerm;               // a term used in the calculation of variance
                                //    of agreement measure
    double chanceAgreement;       // probability of agreement by chance
    double realentry;             // double version of classification table entry
    double sumterm1, sumterm2;    // summation terms involved in the calculation
                                //    of variance of agreement measure

    int csidx, csidx1, csidx2, csidx3;  // indexes over classes
    double fn, fp;                 // false negative and false positive
    //int right, wrong;		// numbers right and wrong for one class
    int thisentry;		// one entry in confusion matrix
                                // ("classification table")
    double probentry;             // one entry in probability matrix
    double tn, tp;                 // true negative and true positive
    //int totalright;		// total right over all classes
    //int totalwrong;		// total wrong over all classes

    // First compute the numbers in each class, and fill the probability table.

    for ( csidx1 = 0; csidx1 <= nclas; csidx1++ ) {
        for ( csidx2 = 0; csidx2 <= nclas; csidx2++ ) {
            thisentry = classificationTable(csidx1,csidx2);
            npatsInClass[csidx1] += thisentry;
            if ( ( csidx1 != nclas ) && ( csidx2 != nclas ) ) {
                npatsAssignedToClass[csidx2] += thisentry;
                totalAssigned += thisentry;
            }
        }
        realNpatsInClass[csidx1] = (double)npatsInClass[csidx1];
        for ( csidx2 = 0; csidx2 <= nclas; csidx2++ ) {
            if ( npatsInClass[csidx1] > 0 ) {
                probTable(csidx1,csidx2) = classificationTable(csidx1,csidx2) /
                                            realNpatsInClass[csidx1];
                if ( ( csidx1 != nclas ) && ( csidx2 != nclas ) )
                    probTableColumnSum[csidx2] += probTable(csidx1,csidx2);
                if ( csidx2 == csidx1 )
                    acc[csidx1] = probTable(csidx1,csidx2) * 100.0;
            }
            else {
                probTable(csidx1,csidx2) = 0.0;
                acc[csidx1] = N_A;
            }
        }
    }
    realTotalAssigned = (double)totalAssigned;

    // Compute the overall accuracy

/*  NEW METHOD (takes care of zero patterns in a class, possible on
                test sets!) */
/*  Not using this any more... takes straight average of class-by-class
    accuracies to get overall accuracy...
    
    int classesFound = 0;
//    for ( csidx1 = 0; csidx1 <= nclas; csidx1++ ) {
    for ( csidx1 = 0; csidx1 < nclas; csidx1++ ) {
        if ( acc[csidx1] != N_A ) {
            classesFound++;
            overallAccuracy *= (double)(classesFound - 1) / (double)classesFound;
            overallAccuracy += acc[csidx1] / (double)classesFound;
        }
    }
    if ( ( acc[nclas] == N_A ) && ( npatsAssignedToClass[nclas] > 0 ) )
        classesFound++; // Can't be ignored for agreement measure calculation!
*/

/*  OLD METHOD (proper overall accuracy calculation) */

    int classesFound = 0;
    overallAccuracy = 0.0;
//    for ( csidx1 = 0; csidx1 <= nclas; csidx1++ ) {
    for ( csidx1 = 0; csidx1 < nclas; csidx1++ ) {
        if ( acc[csidx1] != N_A ) {
            classesFound++;
            overallAccuracy += acc[csidx1] * realNpatsInClass[csidx1] / 
                                             realTotalAssigned;
        }
    }
//    if ( ( acc[nclas] == N_A ) && ( npatsAssignedToClass[nclas] > 0 ) )
//        classesFound++; // Can't be ignored for agreement measure calculation!

    /* Compute the lift */

    for ( csidx1 = 0; csidx1 < nclas; csidx1++ ) {
        if ( npatsAssignedToClass[csidx1] == 0 ) {
            lift[csidx1] = N_A;
        }
        else if ( npatsInClass[csidx1] == 0 ) {
            lift[csidx1] = N_A;
        }
        else {
            lift[csidx1] =
                ((double)classificationTable(csidx1,csidx1)/(double)npatsAssignedToClass[csidx1]) /
                (realNpatsInClass[csidx1]/realTotalAssigned);
        }
    }

    /* Compute the specificity, PPV, and NPV.
       Formerly used the probTable to compensate for mismatches between
       class sizes; now we're back to using the standard method of using
       (integer) classification table entries. */

//    for ( csidx1 = 0; csidx1 <= nclas; ++csidx1 ) {
    for ( csidx1 = 0; csidx1 < nclas; ++csidx1 ) {
        fn = fp = tn = tp = 0.0;
//        for ( csidx2 = 0; csidx2 <= nclas; csidx2++ ) {
        for ( csidx2 = 0; csidx2 < nclas; csidx2++ ) {
//            for ( csidx3 = 0; csidx3 <= nclas; csidx3++ ) {
            for ( csidx3 = 0; csidx3 < nclas; csidx3++ ) {
                probentry = classificationTable(csidx2,csidx3);
                if ( ( csidx2 == csidx3 ) && ( csidx2 == csidx1 ) )
                    // correctly classified to current class (csidx1)
                    tp += probentry;
                else if ( csidx2 == csidx1 ) // incorrectly classified to
				// something other than current class
                    fn += probentry;
                else if ( csidx3 == csidx1 ) // incorrectly classified to
					     // current class
                    fp += probentry;
                else                         // correctly classified to
                                // something other than current class
                    tn += probentry;
            }
        }
        if ( tn + fp > 0.0 )
            specificity[csidx1] = tn / ( tn + fp ) * 100.0;
        else
            specificity[csidx1] = N_A;
        if ( tp + fp > 0.0 )
            ppv[csidx1] = tp / ( tp + fp ) * 100.0;
        else
            ppv[csidx1] = N_A;
        if ( tn + fn > 0.0 )
            npv[csidx1] = tn / ( tn + fn ) * 100.0;
        else
            npv[csidx1] = N_A;
        if ( ( acc[csidx1] != N_A ) && ( tp + fp > 0.0 ) )
            K1Agreement[csidx1] = ( acc[csidx1] - ( tp + fp ) ) / ( 100.0 - ( tp + fp ) );
        else
            K1Agreement[csidx1] = N_A;
        if ( ( specificity[csidx1] != N_A ) && ( tp + fp > 0.0 ) )
            K0Agreement[csidx1] = ( specificity[csidx1] - ( 100.0 - ( tp + fp ) ) ) / ( tp + fp );
        else
            K0Agreement[csidx1] = N_A;
    }

    /* Compute the agreement statistics, unless one of the
       known classes had nothing in it! */

//  if ( ( classesFound == nclas+1 ) ||
//       ( ( classesFound == nclas ) && ( npatsInClass[nclas] == 0 ) ) ) {
    if ( classesFound == nclas ) {

    // For this we want the overall accuracy to be in [0,1]
    overallAccuracy /= 100.0;

//    int savenclas = 0;
//    if ( npatsInClass[nclas] == 0 ) { // Nothing in reject class.
//	     || ( resultType & OUTPUTRES_NONFUZZY ) ) { // Don't count reject class for
		                                         // crisp-only calculation.
//        savenclas = nclas;
//        nclas--; // Ignore reject class
//    }

/*  NEW METHOD */

//    chanceAgreement = 1.0 / (double)(nclas+1);

/*  OLD METHOD
    ...is new once again! */

    chanceAgreement = 0.0;
//    for ( csidx = 0; csidx <= nclas; csidx++ )
    for ( csidx = 0; csidx < nclas; csidx++ )
        chanceAgreement += realNpatsInClass[csidx] * (double)npatsAssignedToClass[csidx];
    chanceAgreement /= realTotalAssigned * realTotalAssigned;

    agreement = ( overallAccuracy - chanceAgreement ) /
                ( 1.0 - chanceAgreement );
    sumterm1 = 0.0;
    sumterm2 = 0.0;
    bigTerm = overallAccuracy * chanceAgreement
              - 2.0 * chanceAgreement
              + overallAccuracy;
    bigTerm = bigTerm * bigTerm;

/*  NEW METHOD
    Not using this right now; (integer) classification table entries are the
    standard

    for ( csidx1 = 0; csidx1 <= nclas; csidx1++ ) {
        realentry = probTable(csidx1,csidx1) / (double)(nclas+1);
        sumterm1 += realentry * pow( ( 1.0 - chanceAgreement ) -
                    (double)( probTableColumnSum[csidx1] + 1.0 )
                    / (double)(nclas+1)
                    * ( 1.0 - overallAccuracy ), 2.0 );
        for ( csidx2 = 0; csidx2 <= nclas; csidx2++ ) {
            if ( csidx2 == csidx1 )
                continue;
            realentry = probTable(csidx1,csidx2) / (double)(nclas+1);
            sumterm2 += realentry * (
                       (double)( probTableColumnSum[csidx1] + 1.0 ) *
                       (double)( probTableColumnSum[csidx1] + 1.0 )
                       / (double)( (nclas+1) * (nclas+1) ) );
        }
    }
    agreementVarn = 1.0 / ( realTotalAssigned // <= (nclas+1)?? // * pow( ( 1.0 - chanceAgreement ), 4.0 ) )
                    * ( sumterm1 +
                      ( 1.0 - overallAccuracy ) * ( 1.0 - overallAccuracy ) *
                        sumterm2 - bigTerm );
*/
/*  OLD METHOD
    ... is new once again! */

//    for ( csidx1 = 0; csidx1 <= nclas; csidx1++ ) {
    for ( csidx1 = 0; csidx1 < nclas; csidx1++ ) {
        realentry = classificationTable(csidx1,csidx1) / realTotalAssigned;
        sumterm1 += realentry * pow( ( 1.0 - chanceAgreement ) -
                    (double)( npatsAssignedToClass[csidx1] + npatsInClass[csidx1] )
                    / realTotalAssigned
                    * ( 1.0 - overallAccuracy ), 2.0 );
//        for ( csidx2 = 0; csidx2 <= nclas; csidx2++ ) {
        for ( csidx2 = 0; csidx2 < nclas; csidx2++ ) {
            if ( csidx2 == csidx1 )
                continue;
            realentry = classificationTable(csidx1,csidx2) / realTotalAssigned;
            sumterm2 += realentry * (
                       (double)( npatsAssignedToClass[csidx2] + npatsInClass[csidx1] ) *
                       (double)( npatsAssignedToClass[csidx2] + npatsInClass[csidx1] )
                       / ( realTotalAssigned * realTotalAssigned ) );
        }
    }
    agreementVarn = 1.0 / ( realTotalAssigned * pow( ( 1.0 - chanceAgreement ), 4.0 ) )
                    * ( sumterm1 +
                      ( 1.0 - overallAccuracy ) * ( 1.0 - overallAccuracy ) *
                        sumterm2 - bigTerm );
        
    agreementInvl = 1.96 * sqrt( agreementVarn );

//    if ( savenclas > 0 )
//        nclas = savenclas;

    // Return the overall accuracy to [0,100]
    overallAccuracy *= 100.0;
    
  }
  else {
    agreement = N_A;
    agreementInvl = N_A;
  }
  return totalAssigned;
}

/* Compute the quality measures, given that we have a value for overall
   percent crisp */
   
void classifierstats_d::computeQuality() {

    int classesFound = 0;
    double PPVsum = 0.0;
    int csidx1;
    
//    for ( csidx1 = 0; csidx1 <= nclas; csidx1++ ) {
    for ( csidx1 = 0; csidx1 < nclas; csidx1++ ) {
        if ( acc[csidx1] != N_A ) {
            classesFound++;
        }
    }
    if ( ( acc[nclas] == N_A ) && ( npatsAssignedToClass[nclas] > 0 ) )
        classesFound++; // Can't be ignored for quality index calculation!

    /* Compute the quality indexes, unless one of the
       known classes had nothing in it! */

//    if ( classesFound == nclas+1 ) {
//        for ( csidx1 = 0; csidx1 <= nclas; csidx1++ )
    if ( classesFound == nclas ) {
        for ( csidx1 = 0; csidx1 < nclas; csidx1++ )
            PPVsum += ppv[csidx1];
        Q1 = 100.0 * agreement * sqrt( overallPercentCrisp / 100.0 );
        Q2 = ( 100.0 * agreement + overallAccuracy + PPVsum ) / ( nclas + 3.0 );
    }
    else if ( ( classesFound == nclas ) && ( npatsInClass[nclas] == 0 ) ) {
        for ( csidx1 = 0; csidx1 < nclas; csidx1++ )
            PPVsum += ppv[csidx1];
        Q1 = 100.0 * agreement * sqrt( overallPercentCrisp / 100.0 );
        Q2 = ( 100.0 * agreement + overallAccuracy + PPVsum ) / ( nclas + 2.0 );
    }
    else {
        Q1 = 0.0;
        Q2 = 0.0;
    }
}

int classifierstats_d::outputStats( scopira::tool::printoflow &outfile )
{
    char headerstr[BUFSIZ];     // output header indicating type of stats
    char overallaccstr[BUFSIZ]; // output line indicating overall accuracy
                                // on what
    char agreementLabel[HEADERLEN];	// a label assigned to the agreement
    char hiConfAgreementLabel[HEADERLEN];  // label assigned to the upper bound
                                           // on the agreement's conf. interval
    char loConfAgreementLabel[HEADERLEN];  // label assigned to the lower bound
                                           // of the agreement's conf. interval

    double chanceAgreement;       // probability of agreement by chance
    double hiConfAgreement;       // upper bound on the agreement's confidence
                                //    interval
    double loConfAgreement;       // lower bound of the agreement's confidence
                                //    interval

    int csidx, csidx1, csidx2;  // indexes over classes
    int thisentry;		// one entry in confusion matrix
                                // ("classification table") 

 /*
    if ( resultType & ( OUTPUTRES_AVERAGED | OUTPUTRES_STDDEVN ) ) {
    	cxModAlert( "outputStats() called for wrong stats object." );
    	return 0;
    }
 */
    assert(!( resultType & ( OUTPUTRES_AVERAGED | OUTPUTRES_STDDEVN ) ));

    /* Print the header */

	  headerstr[0] = '\0';
    if ( !( resultType & OUTPUTRES_NONFUZZY ) )
        strcat( headerstr, " FULL" );
    if ( resultType & OUTPUTRES_TRAIN )
        strcat( headerstr, " TRAINING DATA" );
    else if ( resultType & OUTPUTRES_TEST )
        strcat( headerstr, " TEST DATA" );
    else
        strcat( headerstr, " VALIDATION DATA" );
    if ( resultType & OUTPUTRES_NONFUZZY )
        strcat( headerstr, " NON-FUZZY" );
    strcat( headerstr, " CLASSIFICATION TABLE" );
    if ( resultType & OUTPUTRES_GROUPED )
        strcat( headerstr, " (GROUPED):" );
    else
        strcat( headerstr, ":" );

    flow_printf( outfile, "\n%s\n", headerstr );
    flow_printf( outfile, "-------------------------------------------------------------------------------\n");
    flow_printf( outfile, "Desired          Assigned Class\n");
    flow_printf( outfile, " Class  ");

    /* Print the column headings */

    for ( csidx = 0; csidx < nclas; ++csidx )
        flow_printf( outfile, "%5d", csidx+1 );
//    flow_printf( outfile, "Other" );
    flow_printf( outfile, "  %%Correct  of   SP(%%)  PPV(%%) NPV(%%) K(1,0) K(0,0) Lift" );
    if ( resultType & OUTPUTRES_NONFUZZY )
        flow_printf( outfile, " %%Crisp\n\n" );
    else
        flow_printf( outfile, "\n\n" );

    /* Print the class-by-class stats */

//    for ( csidx1 = 0; csidx1 <= nclas; ++csidx1 ) {
    for ( csidx1 = 0; csidx1 < nclas; ++csidx1 ) {
        if ( csidx1 < nclas )
            flow_printf( outfile, "%4d    ", csidx1+1 );
        else
            flow_printf( outfile, "Other   " );
//        for ( csidx2 = 0; csidx2 <= nclas; ++csidx2 ) {
        for ( csidx2 = 0; csidx2 < nclas; ++csidx2 ) {
            thisentry = classificationTable(csidx1,csidx2);
            flow_printf( outfile, "%5d", thisentry );
        }
        if ( acc[csidx1] != N_A )
            flow_printf( outfile, "   %5.1f", acc[csidx1] );
        else
            flow_printf( outfile, "     N/A" );
        flow_printf( outfile, "  %4d", npatsInClass[csidx1] );

        /* Print the specificity, PPV, NPV, lift, kappa(1,0), kappa(0,0), and (if applicable) percent crisp */

        if ( specificity[csidx1] != N_A )
            flow_printf( outfile, "   %5.1f", specificity[csidx1] );
        else
            flow_printf( outfile, "    N/A " );
        if ( ppv[csidx1] != N_A )
            flow_printf( outfile, "  %5.1f", ppv[csidx1] );
        else
            flow_printf( outfile, "   N/A " );
        if ( npv[csidx1] != N_A )
            flow_printf( outfile, "  %5.1f", npv[csidx1] );
        else
            flow_printf( outfile, "   N/A" );
        if ( K1Agreement[csidx1] != N_A )
            flow_printf( outfile, " %6.3f", K1Agreement[csidx1] );
        else
            flow_printf( outfile, "   N/A " );
        if ( K0Agreement[csidx1] != N_A )
            flow_printf( outfile, " %6.3f", K0Agreement[csidx1] );
        else
            flow_printf( outfile, "   N/A " );
        if ( lift[csidx1] != N_A )
            flow_printf( outfile, "  %4.2f", lift[csidx1] );
        else
            flow_printf( outfile, "   N/A" );
        if ( resultType & OUTPUTRES_NONFUZZY ) {
            if ( percentCrisp[csidx1] != N_A )
                flow_printf( outfile, "  %5.1f\n", percentCrisp[csidx1] );
            else
                flow_printf( outfile, "   N/A\n" );
        }
        else
            flow_printf( outfile, "\n" );
    }
    flow_printf( outfile, "Other   " );
    for ( csidx2 = 0; csidx2 < nclas; ++csidx2 ) {
        thisentry = classificationTable(csidx1,csidx2);
        flow_printf( outfile, "%5d", thisentry );
    }
    flow_printf( outfile, "\n" );

    /* Print the totals assigned to each class */

    flow_printf( outfile, "\nTotals  " );
//    for ( csidx = 0; csidx <= nclas; ++csidx )
    for ( csidx = 0; csidx < nclas; ++csidx )
        flow_printf( outfile, "%5d", npatsAssignedToClass[csidx] );
    flow_printf( outfile, "          %4d\n", totalAssigned );

    /* Print the overall accuracy */

    sprintf( overallaccstr, "Overall accuracy on" );
    if ( resultType & OUTPUTRES_TRAIN )
        strcat( overallaccstr, " training data" );
    else if ( resultType & OUTPUTRES_TEST )
        strcat( overallaccstr, " test data" );
    else
        strcat( overallaccstr, " validation data" );
    if ( resultType & OUTPUTRES_NONFUZZY )
        strcat( overallaccstr, " (excl. fuzzy classifications)" );
    if ( totalAssigned > 0 )
        flow_printf( outfile, "\n%s = %5.1f%%\n", overallaccstr, overallAccuracy );
    else
        flow_printf( outfile, "\n%s =   N/A\n", overallaccstr, overallAccuracy );

    /* Print the agreement statistics */

    flow_printf( outfile, "\nAgreement measure:  95%% confidence interval\n" );

    //int savenclas = 0;
    if ( agreement != N_A ) {
/*
        savenclas = 0;
        if ( npatsInClass[nclas] == 0 ) { // Nothing in reject class.
//	         || ( resultType & OUTPUTRES_NONFUZZY ) ) { // Don't count reject class for
		                                                // crisp-only calculation.
            savenclas = nclas;
            nclas--; // Ignore reject class
        }
*/
/* NEW METHOD */

//        chanceAgreement = 1.0 / (double)(nclas+1);

/* OLD METHOD
   ...is new once again! */
   
        chanceAgreement = 0.0;
//        for ( csidx = 0; csidx <= nclas; csidx++ )
        for ( csidx = 0; csidx < nclas; csidx++ )
            chanceAgreement += (double)npatsInClass[csidx] * (double)npatsAssignedToClass[csidx];
        chanceAgreement /= (double)( totalAssigned * totalAssigned );

        hiConfAgreement = agreement + agreementInvl;
        if ( hiConfAgreement > 1.0 )
            hiConfAgreement = 1.0;
        loConfAgreement = agreement - agreementInvl;
        if ( loConfAgreement < -chanceAgreement / ( 1.0 - chanceAgreement ) )
            loConfAgreement = -chanceAgreement / ( 1.0 - chanceAgreement );

        assignAgreementLabel( agreementLabel, agreement );
        assignAgreementLabel( hiConfAgreementLabel, hiConfAgreement );
        assignAgreementLabel( loConfAgreementLabel, loConfAgreement );
        flow_printf( outfile, "\t= %lf { %s } +/- %lf\n", agreement, agreementLabel, agreementInvl );
        flow_printf( outfile, "\t= ( %lf { %s }, %lf { %s } ).\n",
                loConfAgreement, loConfAgreementLabel, hiConfAgreement, hiConfAgreementLabel );
//        if ( savenclas > 0 )
//		    nclas = savenclas;
    }
    else {
        flow_printf( outfile, "\t= N/A +/- N/A\n" );
        flow_printf( outfile, "\t= ( N/A, N/A ).\n" );
    }
    return totalAssigned;
}
