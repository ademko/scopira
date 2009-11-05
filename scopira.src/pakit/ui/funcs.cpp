
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

#include <pakit/ui/funcs.h>

#include <time.h>
#include <stdlib.h>

#include <scopira/core/register.h>
#include <scopira/basekit/narray.h>
#include <scopira/coreui/label.h>
#include <scopira/coreui/messagewindow.h>
#include <pakit/util.h>

#include <sstream>

//BBlibs scopiraui pakit
//BBtargets libpauikit.so

using namespace scopira::tool;
using namespace scopira::coreui;
using namespace scopira::basekit;
using namespace pakit;

//
//
// register stuff
//
//

static scopira::core::register_view<new_ibdtext_v> r1("pakit::new_ibdtext_v", scopira::core::project_type_c, "Pattern Analysis/Load IBD Text File", scopira::core::windowonly_uimode_c);
static scopira::core::register_view<save_ibdtext_v> r2("pakit::save_ibdtext_v", "pakit::patterns_m", "Save as IBD Text File", scopira::core::windowonly_uimode_c);
static scopira::core::register_view<add_centroids_v> r3("pakit::add_centroids_v", "pakit::patterns_m", "Add Centroids", scopira::core::windowonly_uimode_c);
static scopira::core::register_view<general_report_v> r4("pakit::general_report_v", "pakit::patterns_m", "About...");

//
//
// new_ibdtext_v
//
//

new_ibdtext_v::new_ibdtext_v(void)
  : dm_model(this)
{
  count_ptr<grid_layout> g = new grid_layout(2, 2);

  dm_filename = new fileentry;
  
  
  dm_filename->set_filename("fisher.iris");
  
  dm_type = new radiobutton;
  dm_type->add_selection(0, "19-character labels");
  dm_type->add_selection(1, "variable labels");
  dm_type->set_selection(1);

  g->add_widget(new label("File Name:"), 0, 0, 1, 1, false, false);
  g->add_widget(dm_filename.get(), 1, 0, 1, 1, true, false);
  g->add_widget(new label("File Type:"), 0, 1, 1, 1, false, false);
  g->add_widget(dm_type.get(), 1, 1, 1, 1, false, false);

  set_view_title("IBD Text File Loader");
  
  viewwidget::init_gui(g.get(), button_ok_c|button_cancel_c);
}

void new_ibdtext_v::bind_model(scopira::core::model_i *sus)
{
  dm_model = dynamic_cast<scopira::core::project_i*>(sus);
}

void new_ibdtext_v::react_button(scopira::coreui::button *source, int actionid)
{
  if (actionid == action_apply_c && dm_model.get()) {
    // try to safe the file
    ibdtext_io_alg alg;
    count_ptr<patterns_m> newrec = new patterns_m;

    alg.set_filename(dm_filename->get_filename());
    alg.set_type(dm_type->get_selection());

    if (alg.load_file(newrec->pm_title, *newrec)) {
      //clear_error_label();
      newrec->set_title(dm_filename->get_filename());

      dm_model->add_model(0, newrec.get());
      dm_model->notify_views(this);
    } else {
      messagewindow::popup_error(alg.get_error());
      source->flush_actions();
      return;
    }
  }

  viewwidget::react_button(source, actionid);
}

//
//
// save_ibdtext_v
//
//

save_ibdtext_v::save_ibdtext_v(void)
  : dm_model(this)
{
  count_ptr<grid_layout> g = new grid_layout(2, 1);

  dm_filename = new fileentry;

  dm_filename->set_filename("fisher.iris");

  g->add_widget(new label("File Name:"), 0, 0, 1, 1, false, false);
  g->add_widget(dm_filename.get(), 1, 0, 1, 1, true, false);

  viewwidget::init_gui(g.get(), button_ok_c|button_cancel_c);
}

void save_ibdtext_v::bind_model(scopira::core::model_i *sus)
{
  dm_model = dynamic_cast<patterns_m*>(sus);

  if (dm_model.get())
    dm_filename->set_filename(dm_model->get_title());
}

void save_ibdtext_v::react_button(scopira::coreui::button *source, int actionid)
{
  if (actionid == action_apply_c && dm_model.get()) {
    // try to safe the file
    ibdtext_io_alg alg;

    alg.set_filename(dm_filename->get_filename());

    if (alg.save_file(dm_model->pm_title, dm_model.ref()))
      ;//clear_error_label();
    else {
      messagewindow::popup_error(alg.get_error());
      source->flush_actions();
      return;
    }
  }

  viewwidget::react_button(source, actionid);
}

//
//
// add_centroids_v
//
//

add_centroids_v::add_centroids_v(void)
  : dm_model(this)
{
  count_ptr<box_layout> box = new box_layout(false, true);

  dm_super = new checkbutton("Include global centroid");
  dm_super->set_checked(true);
  dm_perclass = new checkbutton("Include per class centoids");
  dm_perclass->set_checked(true);

  box->add_widget(dm_super.get(), false, false);
  box->add_widget(dm_perclass.get(), false, false);

  set_view_title("Add Centroids");
  
  viewwidget::init_gui(box.get(), button_ok_c|button_cancel_c);
}

void add_centroids_v::bind_model(scopira::core::model_i *sus)
{
  dm_model = dynamic_cast<patterns_m*>(sus);
}

void add_centroids_v::react_button(scopira::coreui::button *source, int actionid)
{
  if (actionid == action_apply_c && dm_model.get()) {
    if (dm_model.is_null())
      return;

    if (!dm_super->is_checked() && !dm_perclass->is_checked()) {
      messagewindow::popup_error("You must select atleast one centroid type");
      source->flush_actions();
      return;
    }

    //clear_error_label();

    patterns_t outdata(true);
    sort_data_alg alg;

    alg.copy_patterns(dm_model.ref(), false);
    alg.add_centroids(dm_super->is_checked(), dm_perclass->is_checked(), outdata);

    dm_model->copy_patterns(outdata, false);
    dm_model->notify_views(this);
  }

  viewwidget::react_button(source, actionid);
}

//
// general_report_v
//

general_report_v::general_report_v(void)
  : dm_model(this)
{
}

void general_report_v::bind_model(scopira::core::model_i *sus)
{
  std::stringstream titlestr;
  std::string filename;
  dm_model = dynamic_cast<patterns_m*>(sus);
  filename = dm_model->get_title();
  titlestr << "Information about " << filename;
  set_view_title(titlestr.str());
}

void general_report_v::react_model_update(scopira::core::model_i *sus, scopira::core::view_i *src)
{
  std::string fname, title;
  intvec_o *in_class;
  doublematrix_o *in_feat;
  boolvec_o *in_train;
  stringvector_o *in_label;
  time_t rawtime;
  int numclass,numpat, numfeat, x;
  int currclass;
  int absclass;
  std::string currlabel;
  std::map<int,one_class> nums_in_class;
  std::map<int,one_class>::iterator nic_iter;
  bool global_centroid_present = false;
  
  clear_output();

  fname = dm_model->get_title();
  title = dm_model->pm_comment;
  
  in_feat = dm_model->pm_data.get();
  in_class = dm_model->pm_classes.get();
  in_train = dm_model->pm_training.get();
  in_label = dm_model->pm_labels.get();
  
  numpat = in_feat->height();
  numfeat = in_feat->width();
  
  time( &rawtime );
  
  flow_output() << "-------------------------------\n";
  
  if (!fname.empty()) 
    flow_output() << "Filename: " << fname << "       "; 
  flow_output() << ctime(&rawtime) << '\n';  
  if (!title.empty()) 
    flow_output() << "Title: " << title << '\n';
    
  flow_output() << "Data Dimensions: " << int_to_string(numfeat) << "x" << int_to_string(numpat) << '\n';
  
  //calculate number of patterns and training in each class 
  numclass = 0;
  for (x=0; x< numpat; x++) {
    currclass = (*in_class)[x];
    absclass = abs(currclass);
    currlabel = (*in_label)[x];

    if ( currlabel.find( /*C*/"entroid" ) != std::string::npos ) {
      if ( currclass == 0 ) {
        global_centroid_present = true;
        continue;
      }
      nic_iter = nums_in_class.find(absclass);
      if (nic_iter == nums_in_class.end()) {
        numclass++;
        nums_in_class[absclass].training=0;
        nums_in_class[absclass].testing=0;
      }
      nums_in_class[absclass].centroid_present = true;
      continue;
    }
    else if ( currlabel.find( /*P*/"laceholder" ) != std::string::npos ) {
      nic_iter = nums_in_class.find(absclass);
      if (nic_iter == nums_in_class.end()) { // I would hope so
        numclass++;
        nums_in_class[absclass].training=0;
        nums_in_class[absclass].testing=0;
      }
      nums_in_class[absclass].placeholder_only = true;
      continue;
    }
    else {
      nic_iter = nums_in_class.find(currclass);
      if (nic_iter == nums_in_class.end()) {
        numclass++;
        nums_in_class[currclass].training=0;
        nums_in_class[currclass].testing=0;
        nums_in_class[currclass].centroid_present = false;
        nums_in_class[currclass].placeholder_only = false;
      }
    }
    if ((*in_train)[x])
      nums_in_class[currclass].training++;
    else
      nums_in_class[currclass].testing++;
  }
  flow_output()<<"Number of unique classes: " <<int_to_string(numclass)<<"\n\n";
    
  flow_output()<<"Class"
               <<pad_left("Training",12)
               <<pad_left("Test",8)
               <<'\n';

  for (nic_iter = nums_in_class.begin(); nic_iter != nums_in_class.end(); nic_iter++) {
    print((*nic_iter).first, (*nic_iter).second);
  }
  if ( global_centroid_present ) {
    flow_output() << " + global centroid\n"; 
  }
}

void pakit::general_report_v::print(int n, const one_class &_class)
{
  flow_output() << pad_left(int_to_string(n),3)
                << pad_left(int_to_string(_class.training), 10)
                << pad_left(int_to_string(_class.testing), 11);
  if ( _class.centroid_present ) {
    flow_output() << "   (+ centroid)";
  }
  if ( _class.placeholder_only ) {
    flow_output() << "   (placeholder only)";
  }
  flow_output() << '\n';
}
