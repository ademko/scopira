
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

#include <scopira/basekit/vectormath.h>
#include <scopira/core/register.h>
#include <scopira/coreui/label.h>
#include <scopira/coreui/layout.h>
#include <scopira/coreui/messagewindow.h>
#include <pakit/lda.h>
#include <pakit/ui/lda.h>
#include <pakit/ui/models.h>
#include <sstream>

using namespace scopira::basekit;
using namespace scopira::coreui;
using namespace scopira::tool;
using namespace pakit;

//BBlibs scopiraui
//BBtargets libpauikit.so

static scopira::core::register_view<pakit::lda_v> r1("pakit::lda_v",
  "pakit::patterns_m", "Classification/LDA", scopira::core::windowonly_uimode_c);

lda_v::lda_v(void)
  : dm_model(this)
{
  dm_lda_line_rotation = 0.0;
  init_gui();
}

void lda_v::init_gui(void)
{
  count_ptr<grid_layout> grid1, grid2;
  count_ptr<box_layout> box;

  grid1 = new grid_layout(1,3); // width, heigth, bool homo
  
  // grouping
  box = new box_layout(true, false);  //horiz,homogeneous,spacing
  grid1->add_widget(box.get(), 0, 0);

  box->add_widget(new label("Class groupings:"),false,false, 2); //wid, expand,fill, padding
  dm_class_groupings_entry = new entry();
  dm_class_groupings_entry->set_text("1 2");
  box->add_widget(dm_class_groupings_entry.get(),true,true); //wid, expand,fill, padding
  
  //line position
  box = new box_layout(true, false);  //horiz,homogeneous,spacing
  grid1->add_widget(box.get(), 0, 1, 1, 1, true,false ); // (x,y, width,height, expandH,expandV)
  
  box->add_widget(new label("Line Position:"),false,false, 2); //wid, expand,fill, padding
  dm_lda_position_slider = new slider(0.0, 1.0, 0.01, 2, 0, false); //min, max, step, page, page_size, vertical
  dm_lda_position_slider->show_min_max();
  dm_lda_position_slider->set_value(0.5);
  dm_lda_position_slider->set_digits(2);
  dm_lda_position_slider->set_slider_reactor(this);
  box->add_widget(dm_lda_position_slider.get(),true,true); //wid, expand,fill, padding
 
  // line rotation
  grid2 = new grid_layout(3,1); // width, int heigth, bool homo
  grid1->add_widget(grid2.get(), 0, 2, 1, 1, true,false ); // (x,y, width,height, expandH,expandV)

  // lda rotation label  
  grid2->add_widget(new label("Line Rotation:"), 0, 0, 1, 1, false,false ); // (x,y, width,height, expandH,expandV)
  // lda rotation slider
  dm_lda_rotation_slider = new slider(-90, 90, 1, 1, 0, false);
  dm_lda_rotation_slider->show_min_max();
  dm_lda_rotation_slider->set_value(0);
  dm_lda_rotation_slider->set_digits(3);
  dm_lda_rotation_slider->set_min_size(180,50);
  dm_lda_rotation_slider->set_slider_reactor(this);
  grid2->add_widget(dm_lda_rotation_slider.get(), 1, 0, 1, 1, true,true,10,2); // (x,y, width,height, expandH,expandV)
  // lda rotation entry box
  dm_lda_rotation_entry = new entry();
	dm_lda_rotation_entry->set_max_length(8);
  dm_lda_rotation_entry->set_text("0");
  dm_lda_rotation_entry->set_entry_reactor(this);
  grid2->add_widget(dm_lda_rotation_entry.get(),2, 0, 1, 1, false,false ); // (x,y, width,height, expandH,expandV)
  
  viewwidget::init_gui(grid1.get(), button_ok_c|button_cancel_c);
}

void lda_v::bind_model(scopira::core::model_i *sus)
{
  if (!sus)
    return;

  dm_model = dynamic_cast<patterns_m*>(sus);
  //count_ptr<scopira::uikit::narray_m<double,2> > probs;
  dm_probs = new probabilities_m;
  dm_probs->pm_array = new narray_o<double,2>;
  dm_probs->pm_classes = new narray_o<int,1>;
  dm_probs->pm_training = new narray_o<bool,1>;
  dm_probs->pm_patterns_link = dm_model.get();
  dm_probs->pm_classes->copy( dm_model->pm_classes.ref() );
  dm_probs->pm_training->copy( dm_model->pm_training.ref() );
  dm_coefs = new scopira::uikit::narray_m<double,2>;
}

void lda_v::react_entry(entry *source, const char *msg)
{ 
  double dval;
  if (!string_to_double(msg, dval) || dval == dm_lda_line_rotation)
    return;
  
  set_lda_rotation_val(dval);
}

void lda_v::react_slider(slider *source, double doubleVal, int intVal)
{
  if (source == dm_lda_rotation_slider.get()) {
    if (doubleVal == dm_lda_line_rotation)
      return;
    
    set_lda_rotation_val(doubleVal);
  }
}


void lda_v::set_lda_rotation_val(double doubleVal)
{  
  dm_lda_line_rotation = doubleVal;
  dm_lda_rotation_slider->set_value(doubleVal);
  dm_lda_rotation_entry->set_text(double_to_string(doubleVal));
}


void lda_v::react_button(scopira::coreui::button *source, int actionid)
{
  if (actionid == action_apply_c) {
    lda_alg<double> algie;
    const char *code;
    double class1_weight;
    double rotation;
    intvec_o classMap;
    std::string class_groupings_string;
    std::string lda_title_annotation;
    char *class_groupings_c_str;
    std::stringstream class_groupings_stream;
    int newclass;
    char *oldclasschr;
    intvec_o new_classes;
    boolvec_o new_training;

    bool staggered = false;
    const int MAXCLASSES = 20;

    assert(dm_model->pm_data.get());

    if (!dm_model->get_project())
      return;

    class1_weight = dm_lda_position_slider->get_value();
    rotation = dm_lda_line_rotation;

    // parse the class groupings
    classMap.resize(MAXCLASSES+1);
    classMap.clear();
    class_groupings_stream << dm_class_groupings_entry->get_text();
    class_groupings_stream >> class_groupings_string;
    class_groupings_c_str = const_cast<char *>(class_groupings_string.c_str());
    newclass = 1;
    int iter = 1;
    lda_title_annotation = class_groupings_string;
    while ( !class_groupings_stream.fail() ) {
      oldclasschr = strtok( class_groupings_c_str, "," );
      while ( oldclasschr ) {
        if ( *oldclasschr == '*' ) {
          for ( int csidx = 1; csidx <= MAXCLASSES; csidx++ ) {
            if ( classMap[csidx] == 0 ) {
              classMap[csidx] = newclass;
            }
          }
        }
        else {
          classMap[atoi( oldclasschr )] = newclass;
        }
        oldclasschr = strtok( NULL, "," );
      }
      class_groupings_stream >> class_groupings_string;
      if ( !class_groupings_stream.fail() ) {
        lda_title_annotation += " vs " + class_groupings_string;
      }
      class_groupings_c_str = const_cast<char *>(class_groupings_string.c_str());
      newclass++;
      iter++;
    }
    if ( class1_weight != 0.5 )
      lda_title_annotation += " - Position: " + double_to_string(class1_weight);
    if ( rotation != 0 )
      lda_title_annotation += " - Rotation: " + double_to_string(rotation);

    // change the training classes
    int classToMap;
    new_classes.resize( dm_model->pm_classes.ref().size() );
    new_training.resize( dm_model->pm_training.ref().size() );
    for ( int pidx = 0; pidx < dm_model->pm_classes.ref().size(); pidx++ ) {
      if ( dm_model->pm_classes.ref()[pidx] <= 0 ) {
        classToMap = -dm_model->pm_classes.ref()[pidx];
        new_classes.set( pidx, -classMap[classToMap] );
        new_training.set( pidx, false );
      }
      else {
        new_classes.set( pidx, classMap[dm_model->pm_classes.ref()[pidx]] );
        if ( classMap[dm_model->pm_classes.ref()[pidx]] > 0 ) {
          new_training.set( pidx, true );
        }
        else {
          new_training.set( pidx, false );
        }
      }
      dm_probs->pm_classes.ref().set( pidx, new_classes.get( pidx ) );
      dm_probs->pm_training.ref().set( pidx, new_training.get( pidx ) );
    }

    // adjust for staggered classes.  Set class labels of the test versions
    // of the selected classes to what they were before staggering.

    // First find out if staggering went on

    int maxtrainclass, maxtestclass, mintrainclass, mintestclass;
    int curr_class;
    int num_trainclass, num_testclass;
    maxtrainclass = maxtestclass = 0;
    mintestclass = mintrainclass = 999;

    // Before doing anything, see if this is an RDP projection (reference points don't count)
    std::string the_filename;
    the_filename = dm_model->get_title();
    // 2D projection?
    int n1_idx, n2_idx;
    int n1_firstchar, n1_length, n2_firstchar, n2_length;
    int n1, n2;
    n1 = n2 = -1;
    n1_idx = the_filename.find("_n1_",0);
    n2_idx = the_filename.find("_n2_",0);
    if ( n1_idx != std::string::npos && n2_idx != std::string::npos ) {
      n1_firstchar = n1_idx+4;
      n1_length = n2_idx-1 - n1_firstchar + 1;
      n2_firstchar = n2_idx+4;
      n2_length = the_filename.length() - n2_firstchar;
      n1 = string_to_int( the_filename.substr( n1_firstchar, n1_length ) );
      n2 = string_to_int( the_filename.substr( n2_firstchar, n2_length ) );
      lda_title_annotation += " - RDP reference points (" + int_to_string(n1) + "," + int_to_string(n2) +")";
    }
    // scatterplot projection?
    int p1n1_idx, p1n2_idx, p2n1_idx, p2n2_idx;
    int p1n1_firstchar, p1n1_length, p1n2_firstchar, p1n2_length;
    int p2n1_firstchar, p2n1_length, p2n2_firstchar, p2n2_length;
    int p1n1, p1n2, p2n1, p2n2;
    p1n1 = p1n2 = p2n1 = p2n2 = -1;
    p1n1_idx = the_filename.find("_p1n1_",0);
    p1n2_idx = the_filename.find("_p1n2_",0);
    p2n1_idx = the_filename.find("_p2n1_",0);
    p2n2_idx = the_filename.find("_p2n2_",0);
    if ( p1n1_idx != std::string::npos && p1n2_idx != std::string::npos &&
         p2n1_idx != std::string::npos && p2n2_idx != std::string::npos ) {
      p1n1_firstchar = p1n1_idx+6;
      p1n1_length = p1n2_idx-1 - p1n1_firstchar + 1;
      p1n2_firstchar = p1n2_idx+6;
      p1n2_length = p2n1_idx-1 - p1n2_firstchar + 1;
      p2n1_firstchar = p2n1_idx+6;
      p2n1_length = p2n2_idx-1 - p2n1_firstchar + 1;
      p2n2_firstchar = p2n2_idx+6;
      p2n2_length = the_filename.length() - p2n2_firstchar;
      p1n1 = string_to_int( the_filename.substr( p1n1_firstchar, p1n1_length ) );
      p1n2 = string_to_int( the_filename.substr( p1n2_firstchar, p1n2_length ) );
      p2n1 = string_to_int( the_filename.substr( p2n1_firstchar, p2n1_length ) );
      p2n2 = string_to_int( the_filename.substr( p2n2_firstchar, p2n2_length ) );
      lda_title_annotation += " - RDP reference points (" + int_to_string(p1n1) + "," + int_to_string(p1n2) 
                              +"),(" + int_to_string(p2n1) + "," + int_to_string(p2n2) + ")";
    }

    for (int i = 0; i < dm_model->pm_training.ref().size(); i++) {
      curr_class = dm_model->pm_classes.ref()[i];

      // don't count centroids
      if ( curr_class <= 0 )
        continue;

      // don't count reference points either
      if ( i == n1 || i == n2 )
        continue;

      // training
      if ( dm_model->pm_training.ref()[i] ) {
        if ( curr_class > maxtrainclass )
          maxtrainclass = curr_class;
        if ( curr_class < mintrainclass )
          mintrainclass = curr_class;
      }
      else {
        // test
        if ( curr_class > maxtestclass )
          maxtestclass = curr_class;
        if ( curr_class < mintestclass )
          mintestclass = curr_class;
      }
    }

    // no harm done if there were no test patterns
    if ( mintestclass > maxtrainclass ) {
      staggered = true;
      if ( maxtestclass > maxtrainclass * 2 ) {
        // assert( maxtestclass % 2 == 0 ); // allow for one fewer training class than test
        num_trainclass = maxtestclass / 2;
      }
      else
        num_trainclass = maxtrainclass;
    }
    else
      num_trainclass = maxtrainclass;

    num_testclass = (maxtestclass - mintestclass) + 1;

    // now set the class labels
    if ( staggered == true ) {
      for ( int i = 0; i < dm_model->pm_classes.ref().size(); i++ ) {
        curr_class = dm_model->pm_classes.ref()[i];
        if ( curr_class > 0 && classMap[curr_class] == 0 ) { // don't touch user-specified class mappings
          if ( curr_class - num_trainclass > 0 && curr_class <= num_trainclass * 2 ) { 
            // ...latter case allows for one more test class than training
            // set test pattern's label to correspond to training
            if ( classMap[curr_class-num_trainclass] > 0 ) {
              dm_probs->pm_classes.ref()[i] = classMap[curr_class-num_trainclass];
            }
          }
          else if ( curr_class > 0 && curr_class + num_trainclass <= num_trainclass + num_testclass ) {
            // set training pattern's label to correspond to test
            if ( classMap[curr_class+num_trainclass] > 0 ) {
              dm_probs->pm_classes.ref()[i] = classMap[curr_class+num_trainclass];
            }
          }
        }
      }
    }

    // finally, if this is an RDP projection, set the reference points to test
    // (if this hasn't been done already)
    // and also set the class labels to one higher than highest test class
    if ( n1 >= 0 ) {
      new_training.set( n1, false );
      new_classes.set( n1, maxtestclass+1 ); 
      dm_probs->pm_training.ref().set( n1, false );
      dm_probs->pm_classes.ref().set( n1, maxtestclass+1 );
    }
    if ( n2 >= 0 ) {
      new_training.set( n2, false );
      new_classes.set( n2, maxtestclass+1 ); 
      dm_probs->pm_training.ref().set( n2, false );
      dm_probs->pm_classes.ref().set( n2, maxtestclass+1 );
    }

    code = algie.init_learn(dm_model->pm_data.get(), &new_classes,
        &new_training, class1_weight, rotation);
    if (code) {
      messagewindow::popup_error( code );
      return;
    }
    // could this go before init_learn?
//    code = algie.set_training(dm_model->pm_training.get());
    code = algie.set_training(&new_training);
    if (code) {
      messagewindow::popup_error( code );
      return;
    }
    algie.init_output(dm_probs->pm_array.get(), dm_coefs->pm_array.get());

    algie.train();

    if (algie.is_singular())
      return; // bail, singular

    algie.test();

    dm_probs->set_title("LDA Probabilities - Class " + lda_title_annotation);
    dm_coefs->set_title("LDA Coefficients - Class " + lda_title_annotation);

    dm_model->get_project()->add_model(dm_model.get(), dm_probs.get());
    dm_model->get_project()->add_model(dm_model.get(), dm_coefs.get());
    dm_model->get_project()->notify_views(this);
  }

  viewwidget::react_button(source, actionid);
}

