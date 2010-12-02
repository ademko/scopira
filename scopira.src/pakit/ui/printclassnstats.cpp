
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

#include <sstream>

#include <algorithm>

#include <pakit/ui/classifierstats.h>
#include <pakit/ui/printclassnstats.h>

#include <scopira/basekit/math.h>
#include <scopira/basekit/narray.h>
#include <scopira/core/register.h>
#include <scopira/coreui/checkbutton.h>
#include <scopira/coreui/label.h>
#include <scopira/coreui/layout.h>
#include <scopira/coreui/messagewindow.h>
#include <scopira/coreui/viewwidget.h>
#include <scopira/tool/fileflow.h>
#include <scopira/tool/nullflow.h>
#include <scopira/tool/printflow.h>
#include <scopira/tool/output.h>

//BBtargets libpauikit.so

using namespace scopira::basekit;
using namespace scopira::coreui;
using namespace scopira::tool;
using namespace scopira::uikit;
using namespace pakit;

//
//
// register
//
//

static scopira::core::register_view<printclassnstats_v> r1("pakit::printclassnstats_v", "pakit::probabilities_m", "View Classification Statistics");

// Constants for parameters

const long SET1    = 0;
const long SET2    = 1;
const long SET3    = 2;

const long PRINT_NOTHING = 0;
const long PRINT_ALL    = 1;
const long PRINT_TABLES    = 2;        // Print only the tables.
const long PRINT_ODD_PAT    = 3;    // Print only misclassified & fuzzy patterns.
const long PRINT_TABLES_AVG    = 4;    // Print the average of the tables.

const long NO_SORT = 0;
const long SORT_PROBS = 1;

const int ASCII_A = 65;

const double SMALL = 0.0001;

//
//
// printclassnstats_v
//
//

printclassnstats_v::printclassnstats_v(void)
  : dm_model(this)
{
  dm_print_what = PRINT_ALL;
  dm_which_set = SET1;
  dm_nvalidation = 0;
  init_gui();
}

void printclassnstats_v::react_button(scopira::coreui::button *source, int actionid)
{
  if ( actionid == action_clear_c )
    dm_output->set_output("");
  else if ( actionid == action_apply_c )
    update_view();

  viewwidget::react_button(source, actionid);
}

void printclassnstats_v::bind_model(scopira::core::model_i *sus)
{
  dm_model = dynamic_cast<probabilities_m*>(sus);
  
  set_view_title("Classification Statistics");
}

//  Test for a valid file name

int printclassnstats_v::validfilename( std::stringstream &fname ) {

    std::string fname_check = fname.str();
    if ((((int)(fname_check[0])) < ASCII_A) && (fname_check[0] != '/') && (fname_check[0] != '.'))
        return 0; // bad
    else
        return 1; // good
}

bool printclassnstats_v::fuzzyClassification( double *memberVec, int nclas ) {

    double hiMembership;
    double fuzzyThresh;
    int  csidx;

    hiMembership = 0.0;

    fuzzyThresh = ( nclas + 1.0 ) / ( 2.0 * nclas );

    for ( csidx = 0; csidx < nclas; ++csidx ) {
        if ( memberVec[csidx] > hiMembership ) {
            hiMembership = memberVec[csidx];
        }
    }

    if ( hiMembership < fuzzyThresh )
        return true;

    return false;
}

typedef struct s_probElem{
    double theProb;
    long theIndex;
} probElem;

bool compareProbs( const probElem &arg1, const probElem &arg2 ) {
    if ( arg1.theProb < arg2.theProb )
        return( true );
    else if ( arg1.theProb == arg2.theProb )
        return( false );
    else
        return( false );
}

//  Sort the probability array.  Sort within each class!

void printclassnstats_v::sortProbArray( doublematrix_o *probArray, // !!!!
                    intvec_o  *desClass, // !
                    int npats,
                    int nclas,
                    int sortProbs,
                    double *sortedProbArray,
                    long  *sortIndex ){

    long currclass, oldclass;   // keep track of class currently being worked on
    long formeridx;     // location of probability in unsorted array
    long npats_to_sort; // number of patterns in current class
    long origpidx;      // increment this as we work through the original
                        //   probabilities
    long startpidx;     // start location of the current class
    long pidx, csidx;   // loop indicies
    
    probElem *probVec = new probElem[npats]; // vector of probabilities
    
    if ( sortProbs == NO_SORT ) { // no sorting.  Just take copy of probability array.
        for ( csidx = 0; csidx < nclas; csidx++ ) {
            for ( pidx = 0; pidx < npats; pidx++ ) {
                sortedProbArray[csidx*npats + pidx] = probArray->get(csidx,pidx);
            }
        }
        for ( pidx = 0; pidx < npats; pidx++ ) {
            sortIndex[pidx] = pidx;
        }
    }
    else {  // Do the sorting!
        origpidx = 0;
        startpidx = 0;
        while ( origpidx < npats ) {
            currclass = desClass->get(origpidx);
            oldclass = desClass->get(origpidx);
            npats_to_sort = -1;
            while ( currclass == oldclass ) {
                npats_to_sort++;
                probVec[npats_to_sort].theProb = probArray->get(currclass-1,origpidx);
                probVec[npats_to_sort].theIndex = npats_to_sort;
                origpidx++;
                if ( origpidx == npats ) {
                    break;
                }
                oldclass = currclass;
                currclass = desClass->get(origpidx);
            }
            if ( ( oldclass <= 0 ) || ( oldclass > nclas ) ) {
                // do not sort this portion!
                for ( pidx = startpidx; pidx < origpidx; pidx++ ) {
                    for ( csidx = 0; csidx < nclas; csidx++ ) {
                        sortedProbArray[csidx*npats + pidx] = probArray->get(csidx,pidx);
                    }
                    sortIndex[pidx] = pidx;
                }    
            }
            else {
                std::sort(probVec,probVec+npats_to_sort+1,compareProbs);
                for ( pidx = startpidx; pidx < origpidx; pidx++ ) {
                    formeridx = probVec[pidx-startpidx].theIndex;
                    for ( csidx = 0; csidx < nclas; csidx++ ) {
                        sortedProbArray[csidx*npats + pidx] = probArray->get(csidx,formeridx);
                    }
                    sortIndex[pidx] = startpidx + probVec[pidx-startpidx].theIndex;
                }
            }
            startpidx = origpidx;
        }
    }              
}

void printclassnstats_v::init_gui( void )
{
  count_ptr<grid_layout> grid, grid1;
//  count_ptr<button_layout> b_layout;
  count_ptr<box_layout> box, box1;
  count_ptr<frame> stat_frame;

  // main layout manager
  grid = new grid_layout(1, 3); // width, height

  // add text window to layout manager
  dm_output = new outputwidget();
  dm_output->set_min_size(500, 350);
  grid->add_widget(dm_output.get(), 0, 0, 1, 1, true, true); //x the x/column numb, y the y/row number

  grid->add_widget(new widget(gtk_hseparator_new()), 0, 1, 1, 1, true, false);

  box = new box_layout(true, false, 2);  // (horizontal, !homogeneous, spacing)
  grid->add_widget(box.get(), 0, 2, 1, 1, true, false);

   // reference points area
  stat_frame = new frame("Classification Statistics Options:");
  box->add_widget(stat_frame.get(),true,true); //wid, expand,fill, padding
  
  grid1 = new grid_layout(2,2);
  stat_frame->add_widget(grid1.get());

  // Stats to output
  box1 = new box_layout(true, false);
  grid1->add_widget(box1.get(), 0, 0, 1, 1, false, false);
  
  box1->add_widget(new label("Stats to output:"),true,false); //wid, expand,fill, padding
  dm_stats_dropdown = new dropdown();
  dm_stats_dropdown->add_selection(0, "None");
  dm_stats_dropdown->add_selection(1, "All");
  dm_stats_dropdown->add_selection(2, "Tables Only");
  dm_stats_dropdown->add_selection(3, "Odd Patterns");
  dm_stats_dropdown->add_selection(4, "Averaged Tables");
  dm_stats_dropdown->set_selection(1);
  dm_stats_dropdown->set_dropdown_reactor(this);
  box1->add_widget(dm_stats_dropdown.get(),true,false); //wid, expand,fill, padding
  
  // Number validation entry
  box1 = new box_layout(true, false);
  grid1->add_widget(box1.get(), 1, 0, 1, 1, false, false);
    
  box1->add_widget(new label("Number validation samples:"),true,false); //wid, expand,fill, padding
  dm_nvalidation_entry = new entry();
  dm_nvalidation_entry->set_text("0");
  dm_nvalidation_entry->set_entry_reactor(this);
  box1->add_widget(dm_nvalidation_entry.get(),true,false); //wid, expand,fill, padding

  // Set to use checkboxes
  box1 = new box_layout(true, false);
  grid1->add_widget(box1.get(), 0, 1, 1, 1, false, false);
  
  box1->add_widget(new label("Set to use:"),true,false); //wid, expand,fill, padding
  dm_set_buttons = new radiobutton();
  dm_set_buttons->add_selection(0, "Training");
  dm_set_buttons->add_selection(1, "Test");
  dm_set_buttons->add_selection(2, "Validation");
  dm_set_buttons->set_radiobutton_reactor(this);
  box1->add_widget(dm_set_buttons.get(),true,false); //wid, expand,fill, padding

  // Output file
  box1 = new box_layout(true, false);
  grid1->add_widget(box1.get(), 1, 1, 1, 1, false, false);

  box1->add_widget(new label("Output file name:"),true,false); //wid, expand,fill, padding
  dm_filename = new fileentry;
  box1->add_widget(dm_filename.get(),true,false); //wid, expand,fill, padding
    
  viewwidget::init_gui(grid.get(), button_close_c|button_clear_c|button_apply_c);
}

void printclassnstats_v::react_dropdown(scopira::coreui::dropdown *source, int selection)
{
  if (!dm_model.get())
    return;

  if (dm_print_what != selection) {
    dm_print_what = selection;
  }
  update_view();
}

void printclassnstats_v::react_radiobutton(scopira::coreui::radiobutton *source, int selection)
{
  if (!dm_model.get())
    return;

  if (dm_which_set != selection) {
    dm_which_set = selection;
  }
  update_view();
}

void printclassnstats_v::react_entry(scopira::coreui::entry *source, const char *val)
{
  int intval;
  bool ok = string_to_int(val, intval);
  
  if ( ok && intval >= 0 ) {
    dm_nvalidation = intval;
    update_view();
  } 
  else 
    //reset the entry box (we entered a bad value)
    dm_nvalidation_entry->set_text("0");
}

void printclassnstats_v::react_model_update(scopira::core::model_i *sus, scopira::core::view_i *src)
{
  update_view();
}

void printclassnstats_v::update_view(void)
{
  dm_output->clear_output();   // im using outputwidget api here
  doublematrix_o *probArray;
  intvec_o *desClassIn;
  count_ptr<intvec_o> desClass;
  stringvector_o *dataLabels;
  int npats, nclas;
  int nSet1, nSet1Set2;
  int whichSet;
  int printStats;
  int sortProbs;
  std::string outfilenameIn;
  boolvec_o *training;

  probArray = dm_model->pm_array.get();
  desClassIn = dm_model->pm_classes.get();
  dataLabels = dm_model->pm_patterns_link->pm_labels.get();
  training = dm_model->pm_training.get();
  nSet1 = 0;
  nclas = 0;
  int idx;
  for ( idx = 0; idx < training->size(); idx++ ) {
    if ( training->get(idx) == true ) {
      nSet1++;
      if ( desClassIn->get(idx) > nclas ) {
        nclas = desClassIn->get(idx);
      }
    }
  }
  desClass = new intvec_o(training->size()); 
  for ( idx = 0; idx < training->size(); idx++ ) {
    desClass->set( idx, desClassIn->get(idx) );
  }
  npats = training->size();
  nSet1Set2 = npats - string_to_int(dm_nvalidation_entry->get_text());
//  whichSet = dm_set_buttons->get_selection();
//  printStats = dm_stats_dropdown->get_selection();
  whichSet = dm_which_set;
  printStats = dm_print_what;
  //TODO sortProbs = i.get_data_g<int_d>("sort-probabilities")->get();
  sortProbs = NO_SORT;
  outfilenameIn = dm_filename->get_filename();

    // Variables needed for output port data handling.
    
    // All the dimensions arrays are same-sized for all stats.  Old IRIS Explorer stuff

    static long oneDimStatsArrayDims[1];
    oneDimStatsArrayDims[0] = nclas+1;
    static long twoDimStatsArrayDims[2];
    twoDimStatsArrayDims[0] = nclas+1;
    twoDimStatsArrayDims[1] = nclas+1;
    
    // Statistics for all patterns
        
    classifierstats_d allStats;    // statistics for all data
    classifierstats_d crispStats; // statistics for crisply classified data
                        
    char correctClass[40];    /* '(cc)', where "cc" is the correct class no.
                                   Only printed for misclassifications... lets be generous with the same -ademko        */
    char fuzzySymbol;        /* '~' if fuzzy classification, ' ' otherwise */
    char misclasSymbol;        /* ' ' if correct classification,
                                   '*' if incorrect, and '?' if unknown.  */
    long totalAssigned; // number of patterns classified
    long crispAssigned; // number of non-fuzzy classifications 
    double memb;            // a class membership
    double *membVec;        // vector of class memberships for one pattern
                                //   over all classes
    double hiMembership;        // highest class membership seen on pattern
    int classAssignment;    // "winner-take-all" class assignment
    long *assignedClass;    // the assigned classes for all patterns
    long pidx, csidx;    // indexes over pattern and class
    long lopidx, hipidx;    // limits of pattern indices
    long sortedpidx;    // index of pattern in sorted probability array into 
                        //    original array
    long lastclass;     // keep checking this.  When class changes print separator.
    std::stringstream    outfilename;    // base file name with appropriate extension
    nullflow nully;
    printoflow tableoutfile(true,0); // the output file for the tables
    printoflow patoutfile(true,0); // the output file for the pat by pat output
    int isOdd;    // The pattern is misclassified, or fuzzily classified.
    
    double *sortedProbArray; // the probability array, sorted
    long  *sortIndex;       // just the positions of the sorted probs within
                            //   the original prob array
    
    nully.auto_ref();
    
    if ( whichSet == SET1 ) {
        if ( nSet1 == 0 )
          return;
        allStats.copy_stats( classifierstats_d( true, nclas, OUTPUTRES_TRAIN ), true );
        crispStats.copy_stats( classifierstats_d( true, nclas, OUTPUTRES_TRAIN | OUTPUTRES_NONFUZZY ), true );
        lopidx = 0;
        hipidx = nSet1Set2;
    }
    else if ( whichSet == SET2 ) {
        if ( nSet1Set2 == nSet1 )
          return;
        allStats.copy_stats( classifierstats_d( true, nclas, OUTPUTRES_TEST ), true );
        crispStats.copy_stats( classifierstats_d( true, nclas, OUTPUTRES_TEST | OUTPUTRES_NONFUZZY ), true );
        lopidx = 0;
        hipidx = nSet1Set2;
    }
    else {
        if ( nSet1Set2 == npats )
          return;
        allStats.copy_stats( classifierstats_d( true, nclas, OUTPUTRES_VAL ), true );
        crispStats.copy_stats( classifierstats_d( true, nclas, OUTPUTRES_VAL | OUTPUTRES_NONFUZZY ), true );
        lopidx = nSet1Set2;
        hipidx = npats;
    }
    membVec = new double [nclas];
    assignedClass = new long [npats];
    assert( membVec );
    assert( assignedClass );

    // Start forming the output file name.  Check which part of data set
    // results came from

    outfilename.str("");
    outfilename << outfilenameIn;

    if ( !validfilename(outfilename) )
        outfilename.str("");

    if ( validfilename(outfilename) ) {
        if ( whichSet == SET1 )
            outfilename << ".train";
        else if ( whichSet == SET2 )
            outfilename << ".test";
        else
            outfilename << ".val";
    }

    switch ( printStats )
        {
        case PRINT_NOTHING:
        case PRINT_TABLES_AVG:    // print nothing yet!
            tableoutfile.open(&nully);
            if ( tableoutfile.failed() ) {
              messagewindow::popup_error("Weird... Couldn't open null stream for output!");
                return;
            }
            assert(false);//fix this, what was this supposed to do?
            //patoutfile = tableoutfile;
            break;
        case PRINT_ODD_PAT:
            tableoutfile.open(&nully);
            if ( tableoutfile.failed() ) {
              messagewindow::popup_error("Weird... Couldn't open null stream for output!");
                return;
            }
            if ( !validfilename( outfilename ))
                patoutfile.open(&(dm_output->flow_output()));
            else {
                outfilename << ".misclass";
                patoutfile.open(new fileflow(outfilename.str(), fileflow::output_c));
                if ( patoutfile.failed() ) {
                  messagewindow::popup_error("Could not open output file!");
                    return;
                }
            }
            break;                
        case PRINT_ALL:
            if (!validfilename( outfilename ))
                tableoutfile.open(&(dm_output->flow_output()));
            else {
                outfilename << ".log";
                tableoutfile.open(new fileflow(outfilename.str(), fileflow::output_c));
                if ( tableoutfile.failed() ) {
                  messagewindow::popup_error("Could not open output file!");
                    return;
                }
            }                
            assert(false);//fix this, what was this supposed to do?
            //patoutfile = tableoutfile;
            break;
        case PRINT_TABLES:
            patoutfile.open(&nully);
            if ( patoutfile.failed() ) {
              messagewindow::popup_error("Weird... Couldn't open null stream for output!");
                return;
            }
            if ( !validfilename( outfilename ))
                tableoutfile.open(&(dm_output->flow_output()));
            else {
                outfilename << ".tab";
                tableoutfile.open(new fileflow(outfilename.str(), fileflow::output_c));
                if ( tableoutfile.failed() ) {
                  messagewindow::popup_error("Could not open output file!");
                    return;
                }
            }
            break;
        default:
            messagewindow::popup_error("Invalid print option encountered.");
            return;
        }
    
    // Fill the probability tables

    // Formerly I had checks in here to see whether there was actually any
    // training (or test) data.

    sortedProbArray = new double[nclas*npats];
    sortIndex = new long[npats];    
    sortProbArray( probArray, desClass.get(), npats, nclas, sortProbs, 
                   sortedProbArray, sortIndex );
            
    for ( pidx = lopidx; pidx < hipidx; pidx++ ) {
        if ( ( whichSet == SET1 ) && ( training->get(pidx) == false ) )
          continue;
        if ( ( whichSet == SET2 ) && ( training->get(pidx) == true ) )
          continue;
        sortedpidx = sortIndex[pidx];
        classAssignment = 1;
        hiMembership = 0.0;
        for ( csidx = 0; csidx < nclas; csidx++ ) {
            memb = probArray->get(csidx,sortedpidx);
            membVec[csidx] = memb;
            if (memb > hiMembership) {
                hiMembership = memb;
                classAssignment = csidx+1;
            }
        }
        if ( hiMembership < SMALL ) // "Rejected"
            classAssignment = nclas+1;
        assignedClass[pidx] = classAssignment;

        if ( ( desClass->get(pidx) > 0 ) && ( desClass->get(pidx) <= nclas ) ) {
            // ...desired class array is identical whether or not we did the
            //    sorting which is why we can get away with not using "sortpidx"
            allStats.incrTableEntry( desClass->get(pidx)-1, classAssignment-1 );
            if ( !fuzzyClassification( membVec, nclas ) )
                crispStats.incrTableEntry( desClass->get(pidx)-1, classAssignment-1 );
        }
        else { // "Unknown" (other) class.
            allStats.incrTableEntry( nclas, classAssignment-1 );
            if ( ( !fuzzyClassification( membVec, nclas ) ) ||
                 ( classAssignment == nclas+1 ) )
                crispStats.incrTableEntry( nclas, classAssignment-1 );
        }
    }
    
    // Compute and output the stats
    
    allStats.computeClassificationStats();
    crispStats.computeClassificationStats();

    // Formerly I had checks in here to see whether there was actually any
    // training (or test) data.
    
    for ( csidx = 0; csidx <= nclas; csidx++ )
        crispStats.setPercentCrisp( csidx,
                (double)crispStats.getNPatsInClass(csidx) /
                (double)allStats.getNPatsInClass(csidx) * 100.0 );

    totalAssigned = allStats.outputStats(tableoutfile);
    crispAssigned = crispStats.outputStats(tableoutfile);
   
    if ( totalAssigned > 0 ) {
        flow_printf( tableoutfile, "\nPercentage of classifications non-fuzzy = %5.1f%%\n",
                (double) crispAssigned / (double) totalAssigned * 100.0 );
        crispStats.setOverallPercentCrisp( (double) crispAssigned / (double) totalAssigned * 100.0 );
        crispStats.computeQuality();
        flow_printf( tableoutfile, "\nQ1 = %5.1f%%,  Q2 = %5.1f%%\n",
                 crispStats.getQ1(), crispStats.getQ2() );
    }    

    if ( whichSet == SET1 )
        flow_printf( patoutfile, "\nPATTERN-BY-PATTERN PERFORMANCE ON TRAINING DATA:\n" );
    else if ( whichSet == SET2 )
        flow_printf( patoutfile, "\nPATTERN-BY-PATTERN PERFORMANCE ON TEST DATA:\n" );
    else
        flow_printf( patoutfile, "\nPATTERN-BY-PATTERN PERFORMANCE ON VALIDATION DATA:\n" );
    flow_printf( patoutfile, "-------------------------------------------------------------------------------\n" );

    flow_printf( patoutfile, "        Pattern           Assigned      Class Memberships\n" );
    flow_printf( patoutfile, "Number           Name      Class   " );
    
    for ( csidx = 0; csidx < nclas; ++csidx )
        flow_printf( patoutfile, "%8d",csidx+1);
    flow_printf( patoutfile, "\n");
    flow_printf( patoutfile, "-------------------------------------------------------------------------------\n");

    lastclass = desClass->get(lopidx);    
    for ( pidx = lopidx; pidx < hipidx; pidx++ ) {
        if ( ( whichSet == SET1 ) && ( training->get(pidx) == false ) )
          continue;
        if ( ( whichSet == SET2 ) && ( training->get(pidx) == true ) )
          continue;
        isOdd = 0;
        sortedpidx = sortIndex[pidx];
/*
    causes busy output if classes not sorted
        if ( lastclass != desClass->get(pidx) ) {
            flow_printf( patoutfile, "-------------------------------------------------------------------------------\n");            
            lastclass = desClass->get(pidx);
        }        
*/
        for (csidx = 0; csidx < nclas; ++csidx) {
            membVec[csidx] = probArray->get(csidx,sortedpidx);
        }
        
        sprintf( correctClass, "    " );
        if ((desClass->get(pidx) <= 0) || (desClass->get(pidx) > nclas)) {
            if ( assignedClass[pidx] == nclas+1 )
                misclasSymbol = 'R';
            else
                misclasSymbol = '?';
            isOdd = 1;
        }
        else if (assignedClass[pidx] != desClass->get(pidx)) {
            if ( assignedClass[pidx] == nclas+1 )
                misclasSymbol = 'R';
            else
                misclasSymbol = '*';
            sprintf( correctClass, "(%2d)", desClass->get(pidx) );
            isOdd = 1;
        }
        else 
            misclasSymbol = ' ';
        
        if ( fuzzyClassification( membVec, nclas ) ) {
            fuzzySymbol = '~';
            isOdd = 1;
        }
        else
            fuzzySymbol = ' ';

        if (!(printStats == PRINT_ODD_PAT) || (isOdd)) {
            flow_printf( patoutfile, "%6d", sortedpidx );
            if ( dataLabels == NULL )
                flow_printf( patoutfile, " Pattern %4d       ", sortedpidx );
            else
                flow_printf( patoutfile, "%19s", dataLabels->get(sortedpidx).c_str() );
            flow_printf( patoutfile, " " );
            flow_printf( patoutfile, "%4d",assignedClass[pidx]);

            flow_printf( patoutfile, " %c %c %s ", misclasSymbol, fuzzySymbol, correctClass );

            for (csidx = 0; csidx < nclas; ++csidx) {
                flow_printf( patoutfile, "%5.3f",membVec[csidx]);
                if ( ( csidx != 4 ) || ( nclas != 5 ) )
                   flow_printf( patoutfile, "   ");
            }
            flow_printf( patoutfile, "\n");
        }    // end if (!(printStats == PRINT_ODD_PAT) || (isOdd))
    } // end loop over patterns
}
