
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

#define WANT_UIKIT_MATRIX_MACROS
#include <sstream>
#include <pakit/ui/patterneditor.h>

//BBlibs scopiraui
//BBtargets libpauikit.so

#include <algorithm>

#include <scopira/tool/util.h>
#include <scopira/core/register.h>

using namespace scopira::basekit;
using namespace scopira::tool;
using namespace pakit;

//
//
// register stuff
//
//

static scopira::core::register_view<pattern_editor_v> r1("pakit::pattern_editor_v", "pakit::patterns_m", "Edit Patterns");

//
//
// pattern_editor_v
//
//

pattern_editor_v::pattern_editor_v(void)
  : parent_type(dm_commands, this), dm_model(this)
{
  init_gui();
  init_menu(dm_menu);
}

void pattern_editor_v::build_centroid_map(void)
{
  pattern_editor_v *here = this;
  std::map<int,int>::iterator cent_map_iter;
  int cent_class;

  here->centroid_map.clear();
  for (int k=0; k<here->dm_classes.size(); ++k) {
    if ( ( ( here->dm_titles[k].find("centroid") != std::string::npos ) ||
           ( here->dm_titles[k].find("Placeholder") != std::string::npos ) ) &&
         ( here->dm_classes[k] <= 0 ) ) {
      cent_class = -(here->dm_classes[k]);
      cent_map_iter = here->centroid_map.find( cent_class );
      if ( cent_map_iter == here->centroid_map.end()) {
        here->centroid_map[cent_class] = k;
      }
    }
  }
}

void pattern_editor_v::adjust_centroids(parm_t &p, int darow)
{
  pattern_editor_v *here = p.get_here<pattern_editor_v>();
  narray_o<double,1> overall_sum;
  std::stringstream placeholder_string;
  int daclass, dacentroid;
  int numinclass, npats;

  // overall centroid calculation might come in handy later
  overall_sum.resize( here->dm_feat.width() );
  overall_sum.clear();
  npats = 0;
  for ( int pidx = 0; pidx < here->dm_feat.height(); pidx++ ) {
    if ( pidx == darow )
      continue;
    if ( here->dm_classes[pidx] > 0 ) {
      npats++;
      for ( int aidx = 0; aidx < here->dm_feat.width(); aidx++ ) {
        overall_sum[aidx] += here->dm_feat.get(aidx,pidx);
      }
    }
  }
  assert( npats > 0 );

  daclass = here->dm_classes[darow];
  if ( here->centroid_map.find(daclass) != here->centroid_map.end() ) {
    dacentroid = here->centroid_map[daclass];
    numinclass = 0;
    for ( int pidx = 0; pidx < here->dm_feat.height(); pidx++ ) {
      if ( here->dm_classes[pidx] == daclass ) {
        numinclass++;
      }
    }
    if ( numinclass == 1 ) { // Centroid becomes a placeholder.
      // Set values to overall centroid
      placeholder_string << "Placeholder_" << daclass;
      here->dm_titles.set( dacentroid, placeholder_string.str() );
      for ( int aidx = 0; aidx < here->dm_feat.width(); aidx++ ) {
        here->dm_feat.set( aidx, dacentroid, overall_sum[aidx] / npats );
      }
    }
    else {
      for ( int aidx = 0; aidx < here->dm_feat.width(); aidx++ ) {
        (here->dm_feat).set(aidx,dacentroid, ( (here->dm_feat).get(aidx,dacentroid) * numinclass - (here->dm_feat).get(aidx,darow) )
                            / ( static_cast<double>(numinclass) - 1.0  ) );
      }
    }
  }

  // Recompute the overall centroid too if present.  Otherwise just leave.
  if ( here->centroid_map.find(0) == here->centroid_map.end() )
    return;
  dacentroid = here->centroid_map[0];
  for ( int aidx = 0; aidx < here->dm_feat.width(); aidx++ ) {
    (here->dm_feat).set(aidx,dacentroid, overall_sum[aidx] / npats );
  }
}

void pattern_editor_v::compute_overall_centroid()
{
  narray_o<double,1> overall_sum;
  int dacentroid;
  int npats;
  pattern_editor_v *here = this;

  if ( here->centroid_map.find(0) == here->centroid_map.end() )
    return;

  overall_sum.resize( here->dm_feat.width() );
  overall_sum.clear();
  npats = 0;
  dacentroid = here->centroid_map[0];
  for ( int pidx = 0; pidx < here->dm_feat.height(); pidx++ ) {
    if ( here->dm_classes[pidx] > 0 ) {
      npats++;
      for ( int aidx = 0; aidx < here->dm_feat.width(); aidx++ ) {
        overall_sum[aidx] += here->dm_feat.get(aidx,pidx);
      }
    }
  }
  for ( int aidx = 0; aidx < here->dm_feat.width(); aidx++ ) {
    here->dm_feat.set( aidx, dacentroid, overall_sum[aidx] / npats );
  }
}

void pattern_editor_v::compute_centroid(int cent_class)
{
  int dacentroid;
  int numinclass, npats;
  narray_o<double,1> sumclass;
  narray_o<double,1> overall_sum;
  std::stringstream placeholder_string;

  if ( cent_class == 0 ) {
    compute_overall_centroid();
    return;
  }
  pattern_editor_v *here = this;

  if ( here->centroid_map.find(cent_class) == here->centroid_map.end() )
    return;

  sumclass.resize( here->dm_feat.width() );
  sumclass.clear();
  overall_sum.resize( here->dm_feat.width() );
  overall_sum.clear();
  dacentroid = here->centroid_map[cent_class];
  numinclass = 0;
  for ( int pidx = 0; pidx < here->dm_feat.height(); pidx++ ) {
    if ( here->dm_classes[pidx] == cent_class ) {
      numinclass++;
      for ( int aidx = 0; aidx < here->dm_feat.width(); aidx++ ) {
        sumclass[aidx] += here->dm_feat.get(aidx,pidx);
      }
    }
  }

  // If no samples associated with class, it's a placeholder.
  // Use the overall centroid
  if ( numinclass == 0 ) {
    overall_sum.resize( here->dm_feat.width() );
    overall_sum.clear();
    npats = 0;
    for ( int pidx = 0; pidx < here->dm_feat.height(); pidx++ ) {
      if ( here->dm_classes[pidx] > 0 ) {
        npats++;
        for ( int aidx = 0; aidx < here->dm_feat.width(); aidx++ ) {
          overall_sum[aidx] += here->dm_feat.get(aidx,pidx);
        }
      }
    }
    assert( npats > 0 ); // Bombs for resize, if only centroids are left.
                         // Think I've handled all other cases - BD
    placeholder_string << "Placeholder_" << cent_class;
    here->dm_titles.set( dacentroid, placeholder_string.str() );
    for ( int aidx = 0; aidx < here->dm_feat.width(); aidx++ ) {
      here->dm_feat.set( aidx, dacentroid, overall_sum[aidx] / npats );
    }
  }
  else { // all is well
    for ( int aidx = 0; aidx < here->dm_feat.width(); aidx++ ) {
      here->dm_feat.set( aidx, dacentroid, sumclass[aidx] / numinclass );
    }
  }
}

int pattern_editor_v::get_data_width(void) const
{
  return dm_feat.width() + 3;
}

int pattern_editor_v::get_data_height(void) const
{
  return dm_feat.height();
}

void pattern_editor_v::get_data_text(int x, int y, std::string &out) const
{
  switch (x) {
    case 0:
      if (dm_titles.empty())
        out = "N/A";
      else {
        out = "\"";
        const std::string &ins( dm_titles.get(y%dm_titles.size()) );
        for (int jj=0; jj<ins.size(); ++jj) {
          if (ins[jj] == '"')
            out += "\\";
          out += ins[jj];
        }
        out += "\"";
      }
      break;
    case 1:
      if (dm_training.empty())
        out = "N/A";
      else
        out = dm_training.get(y%dm_training.size())?"1":"0";
      break;
    case 2:
      if (dm_classes.empty())
        out = "N/A";
      else
        int_to_string(dm_classes.get(y%dm_classes.size()), out);
      break;
    default:
      assert(x-basex_c<dm_feat.width());
      assert(y<dm_feat.height());
      double_to_string(dm_feat(x-basex_c,y), out);
      break;
  }
}

void pattern_editor_v::get_data_stat(std::string &out) const
{
  out = "Data size = ";
  out += int_to_string(dm_feat.width());
  out += " by ";
  out += int_to_string(dm_feat.height());
}

void pattern_editor_v::get_data_header(int x, std::string &out) const
{
  switch (x) {
    case 0:
      out = "Label";
      break;
    case 1:
      out = "Training?";
      break;
    case 2:
      out = "Class";
      break;
    default:
      int_to_string(x-basex_c, out);
  }
}

bool pattern_editor_v::get_data_double(int x, int y, double &out) const
{
  switch (x) {
    case 0:
      return false;
    case 1:
      if (dm_training.empty())
        return false;
      else
        out = dm_training.get(y % dm_training.size())?1:0;
      break;
    case 2:
      if (dm_classes.empty())
        return false;
      else
        out = dm_classes.get(y % dm_classes.size());
      break;
    default:
      out = dm_feat(x-basex_c, y);
  }
  return true;
}

bool pattern_editor_v::get_data_string(int x, int y, std::string &out) const
{
  if (x==0 && !dm_titles.empty()) {
    out = dm_titles.get(y%dm_titles.size());
    return true;
  } else
    return false;
}

void pattern_editor_v::prepare_undo(void)
{
}

void pattern_editor_v::bind_model(scopira::core::model_i *sus)
{
  dm_model = dynamic_cast<patterns_m*>(sus);
}

void pattern_editor_v::react_model_update(scopira::core::model_i *sus, scopira::core::view_i *src)
{
  assert(dm_model.get());

  std::map<int,int>::iterator cent_map_iter;

  dm_titles.resize(dm_model->pm_labels->size());
  for (int k=0; k<dm_titles.size(); ++k)
    dm_titles[k] = dm_model->pm_labels->get(k);

  dm_training.copy(dm_model->pm_training.ref());

  dm_classes.copy(dm_model->pm_classes.ref());

  dm_feat.copy(dm_model->pm_data.ref());

  prep_data();

  // Remap and recompute the centroids
  build_centroid_map();
  for ( cent_map_iter = centroid_map.begin(); cent_map_iter != centroid_map.end(); cent_map_iter++ ) {
    compute_centroid( (*cent_map_iter).first );
  }
}

void pattern_editor_v::react_button(scopira::coreui::button *source, int actionid)
{
  if (dm_model.get() && actionid == action_apply_c) {
    // copy the data over and signal the inspectors
    dm_model->pm_labels = new stringvector_o(dm_titles);
    dm_model->pm_classes = new narray_o<int,1> (dm_classes);
    dm_model->pm_training = new narray_o<bool,1>(dm_training);
    dm_model->pm_data = new narray_o<double,2> (dm_feat);
    dm_model->notify_views(this);
  }

  // send it up for further processing
  view_i::react_view_action(actionid);
}

void pattern_editor_v::undo(void)
{
}

void pattern_editor_v::init_menu(popup_menu &b)
{
  b.push_menu("Edit");

  b.push_menu("Modifiers");
  b.add_item("Add (+)", ptr_to_int("P+ "));   //ugh, cast
  b.add_item("Subtract (-)", ptr_to_int("P- "));   //ugh, cast
  b.add_item("Multiply (*)", ptr_to_int("P* "));   //ugh, cast
  b.add_item("Divide (/)", ptr_to_int("P/ "));   //ugh, cast
  b.add_item("Integer divide (div)", ptr_to_int("Pdiv "));   //ugh, cast
  b.add_item("Integer modulo (mod)", ptr_to_int("Pmod "));   //ugh, cast
  b.pop_menu();
  b.add_separator();

  b.add_item("Clear to zero (s 0)", ptr_to_int("Ys 0"));   //ugh, cast
  b.add_item("Set all to 1 (s or set)", ptr_to_int("Xs 1"));   //ugh, cast

  b.pop_menu();

  b.push_menu("Dimension");
  b.add_item("Resize w h (resize or re)", ptr_to_int("Xresize 10 10"));   //ugh, cast
  b.add_item("Crop (crop)", ptr_to_int("Ycrop"));   //ugh, cast
  b.add_separator();
  b.add_item("Delete selected feature columns (delcols)", ptr_to_int("Ydelcols"));   //ugh, cast
  b.add_item("Delete selected rows (delrows)", ptr_to_int("Ydelrows"));   //ugh, cast
  b.add_separator();
  b.add_item("Delete a row (delrow)", ptr_to_int("Xdelrow 1"));   //ugh, cast
  b.pop_menu();

  b.push_menu("Selection");

  b.push_menu("Group");
  b.add_item("Add/Union (add)", ptr_to_int("Padd "));   //ugh, cast
  b.add_item("Subtract (sub)", ptr_to_int("Psub "));   //ugh, cast
  b.add_item("Intersection (and)", ptr_to_int("Pand "));   //ugh, cast
  b.pop_menu();
  b.add_separator();

  b.add_item("All (all)", ptr_to_int("Yall"));   //ugh, cast
  b.add_item("None (none)", ptr_to_int("Ynone"));   //ugh, cast
  b.add_item("Rectangle (rect)", ptr_to_int("Xrect 0 0 2 2"));   //ugh, cast
  b.add_item("One cell (sel)", ptr_to_int("Xsel 0 0"));   //ugh, cast
  b.add_item("Invert selection (selinvert)", ptr_to_int("Yselinvert"));   //ugh, cast
  b.add_item("One Row (selrow)", ptr_to_int("Xselrow 1"));  //ugh, cast
  b.add_separator();
  b.add_item("Expand columns (cols)", ptr_to_int("Ycols"));   //ugh, cast
  b.add_item("Expand rows (rows)", ptr_to_int("Yrows"));   //ugh, cast
  b.add_item("Expand rectangle (expandrect)", ptr_to_int("Yexpandrect"));   //ugh, cast

  b.pop_menu();

  b.add_separator();

  b.push_menu("Label Column");
  b.add_item("Select (collabel)", ptr_to_int("Ycollabel"));
  b.add_item("Auto Fill by Row Number (labelfill)", ptr_to_int("Ylabelfill"));
  b.pop_menu();
  b.push_menu("Training Column");
  b.add_item("Select (coltrain)", ptr_to_int("Ycoltrain"));
  b.add_item("Select Training (coltrain and = 1)", ptr_to_int("Ycoltrain and = 1"));
  b.add_item("Select Test (coltrain and = 0)", ptr_to_int("Ycoltrain and = 0"));
  b.add_item("Flip Traing/Test (fliptrain)", ptr_to_int("Yfliptrain"));
  b.pop_menu();
  b.push_menu("Class Column");
  b.add_item("Select (colclass)", ptr_to_int("Ycolclass"));
  b.pop_menu();
  b.push_menu("Feature Data");
  b.add_item("Select (features)", ptr_to_int("Yfeatures"));
  b.pop_menu();
}

void pattern_editor_v::cmd_sel_row(parm_t &p)
{
  pattern_editor_v *here = p.get_here<pattern_editor_v>();
  narray_o<bool,2> newsel(here->dm_sel.width(), here->dm_sel.height());
  int darow;

  ASSERT_INT(p.token+1, "Parameter must be an integer");
  ASSERT_NONNEG_INT(p.token+1, "Parameter must be nonnegative");

  darow = (p.token+1)->ivalue;

  if (darow >= here->dm_feat.height()) {
    p.error = "Parameter extends beyond the height of the datum";
    return;
  }

  newsel.set_all( false );
  newsel.xslice(0, darow, newsel.width()).set_all( true );
  p.merge_sel(newsel);
}

void pattern_editor_v::cmd_sel_col(parm_t &p)
{
  pattern_editor_v *here = p.get_here<pattern_editor_v>();
  narray_o<bool,2> newsel(here->dm_sel.width(), here->dm_sel.height());
  int dacol;

  ASSERT_INT(p.token+1, "Parameter must be an integer");
  ASSERT_NONNEG_INT(p.token+1, "Parameter must be nonnegative");

  dacol = (p.token+1)->ivalue;

  if (dacol >= here->dm_feat.width()) {
    p.error = "Parameter extends beyond the width of the datum";
    return;
  }

  newsel.set_all( false );
  newsel.yslice(dacol+3, 0, newsel.height()).set_all( true );
  p.merge_sel(newsel);
}

void pattern_editor_v::cmd_set_one(parm_t &p)
{
  pattern_editor_v *here = p.get_here<pattern_editor_v>();
  std::map<int,int>::iterator cent_map_iter;
  int x, y;
  tok_t *delta = p.token+3;

  assert((p.token+1)->isint);
  assert((p.token+2)->isint);
  x =  (p.token+1)->ivalue;
  y =  (p.token+2)->ivalue;

  if ( here->dm_titles[y].find("Placeholder") != std::string::npos ) {
    p.error = "Placeholder data can't be changed manually";
    return;
  }
  if ( here->dm_titles[y].find("centroid") != std::string::npos ) {
    p.error = "Centroid data can't be changed manually";
    return;
  }

  switch (x) {
    case 0:
      if (!here->dm_titles.empty())
        here->dm_titles.set(y % here->dm_titles.size(), delta->dequote_c_array());
      break;
    case 1:
      if (!here->dm_training.empty() && delta->isint)
        here->dm_training.set(y % here->dm_training.size(), delta->ivalue!=0?1:0);
      break;
    case 2:
      if (!here->dm_classes.empty() && delta->isint)
        here->dm_classes.set(y % here->dm_classes.size(), delta->ivalue);
      break;
    default:
      p.apply_op(delta->dvalue, here->dm_feat(x - basex_c, y));
  }

  // Remap and recompute the centroids
  here->build_centroid_map();
  for ( cent_map_iter = here->centroid_map.begin(); cent_map_iter != here->centroid_map.end(); cent_map_iter++ ) {
    here->compute_centroid( (*cent_map_iter).first );
  }
}

void pattern_editor_v::cmd_set(parm_t &p)
{
  pattern_editor_v *here = p.get_here<pattern_editor_v>();
  std::map<int,int>::iterator cent_map_iter;
  tok_t *delta;
  int x, y;

  // Check that we are not messing with centroids
  for ( y = 0; y < here->dm_titles.size(); ++y ) {
    if (here->dm_sel(0, y)) {
      if ( here->dm_titles[y].find("Placeholder") != std::string::npos ) {
        p.error = "Placeholder data can't be changed manually";
        return;
      }
      if ( here->dm_titles[y].find("centroid") != std::string::npos ) {
        p.error = "Centroid data can't be changed manually";
        return;
      }
    }
  }

  delta = p.token+1;

  if (delta->name[0] == '"') {
    std::string news(delta->dequote_c_array());
    // just do labels
    for (y=0; y<std::min(here->dm_titles.size(), here->dm_feat.height()); ++y)
      if (here->dm_sel(0, y))
        here->dm_titles.set(y, news);
  } else if (delta->isdouble) {
    if (delta->isint) {
      // int? means we can do training and classes
      bool newb = delta->ivalue != 0;

      for (y=0; y<std::min(here->dm_training.size(), here->dm_feat.height()); ++y)
        if (here->dm_sel(1, y))
          here->dm_training.set(y, newb);
      for (y=0; y<std::min(here->dm_classes.size(), here->dm_feat.height()); ++y)
        if (here->dm_sel(2, y))
          p.apply_op(delta->ivalue, here->dm_classes[y]);
    }
    // do data
    for (y=0; y<here->dm_feat.height(); ++y)
      for (x=0; x<here->dm_feat.width(); ++x)
        if (here->dm_sel.get(x+basex_c,y))
          p.apply_op(delta->dvalue, here->dm_feat(x, y));
    p.reset_op();
  }

  // Remap and recompute the centroids
  here->build_centroid_map();
  for ( cent_map_iter = here->centroid_map.begin(); cent_map_iter != here->centroid_map.end(); cent_map_iter++ ) {
    here->compute_centroid( (*cent_map_iter).first );
  }
}

void pattern_editor_v::cmd_collabel(parm_t &p)
{
  pattern_editor_v *here = p.get_here<pattern_editor_v>();
  narray_o<bool,2> newsel(here->dm_sel.width(), here->dm_sel.height());

  newsel.set_all( false );
  newsel.yslice(0, 0, newsel.height()).set_all( true );

  p.merge_sel(newsel);
}

void pattern_editor_v::cmd_fliptrain(parm_t &p)
{
  pattern_editor_v *here = p.get_here<pattern_editor_v>();

  std::string dalabel;
  int y, my;
  my = here->dm_sel.height();

  for (y=0; y<my; ++y)
    if (here->dm_sel(1, y)) {
      dalabel = here->dm_titles.get(y);
      // Keep centroids false
      if ( dalabel.find( "centroid" ) == std::string::npos &&
           dalabel.find( "Placeholder" ) == std::string::npos )
        here->dm_training[y] = !here->dm_training[y];
      else
        here->dm_training[y] = false;
    }
}

void pattern_editor_v::cmd_coltrain(parm_t &p)
{
  pattern_editor_v *here = p.get_here<pattern_editor_v>();
  narray_o<bool,2> newsel(here->dm_sel.width(), here->dm_sel.height());

  newsel.set_all( false );
  newsel.yslice(1, 0, newsel.height()).set_all( true );

  p.merge_sel(newsel);
}

void pattern_editor_v::cmd_colclass(parm_t &p)
{
  pattern_editor_v *here = p.get_here<pattern_editor_v>();
  narray_o<bool,2> newsel(here->dm_sel.width(), here->dm_sel.height());

  newsel.set_all( false );
  newsel.yslice(2, 0, newsel.height()).set_all( true );

  p.merge_sel(newsel);
}

void pattern_editor_v::cmd_features(parm_t &p)
{
  pattern_editor_v *here = p.get_here<pattern_editor_v>();
  narray_o<bool,2> newsel(here->dm_sel.width(), here->dm_sel.height());

  newsel.set_all( true );
  newsel.xyslice(0, 0, 3, newsel.height()).set_all( false );

  p.merge_sel(newsel);
}

void pattern_editor_v::cmd_labelfill(parm_t &p)
{
  pattern_editor_v *here = p.get_here<pattern_editor_v>();
  int y, my;

  my = std::min(here->dm_titles.size(), here->dm_feat.height());
  for (y=0; y<my; ++y)
    if (here->dm_sel(0, y))
      here->dm_titles.set(y, int_to_string(y));
}

void pattern_editor_v::cmd_resize(parm_t &p)
{
  pattern_editor_v *here = p.get_here<pattern_editor_v>();
  std::map<int,int>::iterator cent_map_iter;
  tok_t *w, *h;
  int minw, minh;

  w = p.token+1;
  h = p.token+2;

  ASSERT_INT(w, "Width must be an integer");
  ASSERT_INT(h, "Height must be an integer");
  ASSERT_POSITIVE_INT(w, "Non-positive width given to resize");
  ASSERT_POSITIVE_INT(h, "Non-positive height given to resize");

  here->dm_sel.resize(w->ivalue+basex_c, h->ivalue);
  here->dm_sel.set_all(false);

  // resize titles
  if (here->dm_titles.size() != h->ivalue) {
    stringvector_o oldtitles(here->dm_titles);

    here->dm_titles.resize(h->ivalue);

    minh = std::min<int>(oldtitles.size(), h->ivalue);

    for (int j=0; j<minh; ++j)
      here->dm_titles[j] = oldtitles[j];
  }

  // resize training vector
  narray<bool> oldtrain(here->dm_training);

  here->dm_training.resize(h->ivalue);
  here->dm_training.clear();

  minh = std::min<int>(oldtrain.size(), h->ivalue);

  if (minh>0)
    here->dm_training.xslice(0, minh).copy( oldtrain.xslice(0, minh) );

  // resize class vector
  narray<int> oldclass(here->dm_classes);

  here->dm_classes.resize(h->ivalue);
  here->dm_classes.clear();

  minh = std::min<int>(oldclass.size(), h->ivalue);

  if (minh>0)
    here->dm_classes.xslice(0, minh).copy( oldclass.xslice(0, minh) );

  // resize feature data
  narray<double,2> olddata(here->dm_feat);

  here->dm_feat.resize(w->ivalue, h->ivalue);
  here->dm_feat.clear();

  minw = std::min<int>(olddata.width(), w->ivalue);
  minh = std::min<int>(olddata.height(), h->ivalue);
  here->dm_feat.xyslice(0, 0, minw, minh).copy( olddata.xyslice(0, 0, minw, minh) );

  // Remap and recompute the centroids
  here->build_centroid_map();
  for ( cent_map_iter = here->centroid_map.begin(); cent_map_iter != here->centroid_map.end(); cent_map_iter++ ) {
    here->compute_centroid( (*cent_map_iter).first );
  }
}

void pattern_editor_v::cmd_crop(parm_t &p)
{
  pattern_editor_v *here = p.get_here<pattern_editor_v>();
  std::map<int,int>::iterator cent_map_iter;
  int x, y, w, h, minx, miny, maxx, maxy;

  w = here->dm_sel.width();
  h = here->dm_sel.height();
  minx = w-1;
  maxx = 0;
  miny = h-1;
  maxy = 0;

  for (y=0; y<h; ++y)
    for (x=0; x<w; ++x)
      if (here->dm_sel.get(x,y)) {
        if (x<minx)
          minx = x;
        if (x>maxx)
          maxx = x;
        if (y<miny)
          miny = y;
        if (y>maxy)
          maxy = y;
      }

  // bounding rect found
  if (minx == 0 && miny == 0 && maxx == (here->dm_sel.width()-1) && maxy == (here->dm_sel.height())-1) {
    p.error = "You usually want to crop a SUBSET of your current data";
    return;
  }

  if (maxx>=minx && maxy>=miny) {
    int k, HH;
    if (maxx<basex_c) {
      p.error = "You must keep atleast one of the data columns";
      return;
    }

    // Check that valid data is being kept, not just centroids
    // TODO same check, for resize
    bool valid_data = false;
    for ( int y = 0; y < here->dm_titles.size(); ++y ) {
      if (here->dm_sel(0, y)) {
        if ( here->dm_titles[y].find("Placeholder") == std::string::npos &&
             here->dm_titles[y].find("centroid") == std::string::npos &&
             here->dm_classes[y] > 0 ) {
          valid_data = true;
          break;
        }
      }
    }
    if ( !valid_data ) {
      p.error = "Must keep some non-centroid data with nonnegative class labels";
      return;
    }

    minx -= basex_c;    // ofset the Xs just for features
    if (minx<0)
      minx = 0;
    maxx -= basex_c;
    w = maxx - minx + 1;
    h = maxy - miny + 1;

    here->dm_sel.resize(w+basex_c, h);
    here->dm_sel.set_all(false);

    // carry over titles
    HH = std::min<int>(h, here->dm_titles.size() - miny);
    if (HH<=0) {
      // class vector way too short
      here->dm_titles.resize(h);
      here->dm_titles.clear();
    } else {
      stringvector_o oldtitles;
      oldtitles.resize(HH);
      for (k=0; k<HH; ++k)
        oldtitles[k] = here->dm_titles[k+miny];
      here->dm_titles.resize(HH);
      for (k=0; k<HH; ++k)
        here->dm_titles[k] = oldtitles[k];
      for (; k<h; ++k)
        oldtitles[k].clear();
    }

    // carry over training
    HH = std::min<int>(h, here->dm_training.size() - miny);
    if (HH<=0) {
      // class vector way too short
      here->dm_training.resize(h);
      here->dm_training.clear();
    } else {
      narray<bool> oldtraining;
      oldtraining.copy(here->dm_training.xslice(miny, HH));
      if (HH == h)
        here->dm_training.copy(oldtraining);
      else {
        // sub set
        here->dm_training.clear();
        here->dm_training.xslice(0, HH).copy(oldtraining);
      }
    }

    // carry over classes
    HH = std::min<int>(h, here->dm_classes.size() - miny);
    if (HH<=0) {
      // class vector way too short
      here->dm_classes.resize(h);
      here->dm_classes.clear();
    } else {
      narray<int> oldclass;
      oldclass.copy(here->dm_classes.xslice(miny, HH));
      if (HH == h)
        here->dm_classes.copy(oldclass);
      else {
        // sub set
        here->dm_classes.clear();
        here->dm_classes.xslice(0, HH).copy(oldclass);
      }
    }

    // carry over features
    narray<double,2> olddata;
    olddata.copy(here->dm_feat.xyslice(minx, miny, w, h));
    here->dm_feat.copy(olddata);
  }

  // Remap and recompute the centroids
  here->build_centroid_map();
  for ( cent_map_iter = here->centroid_map.begin(); cent_map_iter != here->centroid_map.end(); cent_map_iter++ ) {
    here->compute_centroid( (*cent_map_iter).first );
  }
}

void pattern_editor_v::cmd_delcols(parm_t &p)
{
  pattern_editor_v *here = p.get_here<pattern_editor_v>();
  int j, k, mj;
  nslice<bool> sl;
  std::vector<int> keepers;
  bool selected;

  mj = here->dm_feat.width();
  keepers.reserve(mj);

  for (j=0; j<mj; ++j) {
    sl = here->dm_sel.yslice(j+basex_c, 0, here->dm_sel.height());
    selected = false;
    for ( k = 0; k < here->dm_sel.height(); ++k ) {
      if ( sl[k] == true ) {
        selected = true;
        break;
      }
    }
// This iterator found to be broken for large datasets
//    if (std::find(sl.begin(), sl.end(), true) == sl.end())
    if ( selected == false ) 
      keepers.push_back(j);
  }

  if (keepers.empty()) {
    p.error = "An empty matrix would result";
    return;
  }
  if (keepers.size() == here->dm_feat.width()) {
    p.error = "Select some columns for deletion";
    return;
  }

  here->dm_sel.resize(keepers.size()+basex_c, here->dm_sel.height());
  here->dm_sel.set_all(false);

  narray<double,2> old(here->dm_feat);

  here->dm_feat.resize(keepers.size(), here->dm_feat.height());
  for (j=0; j<keepers.size(); ++j)
    here->dm_feat.yslice(j, 0, here->dm_feat.height()).copy(
        old.yslice(keepers[j], 0, old.height()) );
}

void pattern_editor_v::cmd_delrows(parm_t &p)
{
  pattern_editor_v *here = p.get_here<pattern_editor_v>();
  int j, k, mj;
  bool centroid_deleted;
  nslice<bool> sl;
  std::vector<int> keepers;
  bool selected;
  std::string dalabel;
  std::map<int,int>::iterator cent_map_iter;
  int npats;

  mj = here->dm_feat.height();
  keepers.reserve(mj);

  npats = 0;
  centroid_deleted = false;
  for (j=0; j<mj; ++j) {
    sl = here->dm_sel.xslice(0, j, here->dm_sel.width());
    selected = false;
    for ( k = 0; k < here->dm_sel.width(); ++k ) {
      if ( sl[k] == true ) {
        selected = true;
        break;
      }
    }
// This iterator might be broken for large datasets
//    if (std::find(sl.begin(), sl.end(), true) == sl.end()) {
    if ( selected == false ) {
      keepers.push_back(j); // if not selected it's a keeper
      npats++;
      for ( cent_map_iter = here->centroid_map.begin(); cent_map_iter != here->centroid_map.end(); cent_map_iter++ ) {
        if ( (*cent_map_iter).second == j ) {
          npats--; // centroids don't count towards data total
        }
      }
    }
    else { // Check whether we are deleting any centroids
      for ( cent_map_iter = here->centroid_map.begin(); cent_map_iter != here->centroid_map.end(); cent_map_iter++ ) {
        if ( (*cent_map_iter).second == j ) {
          centroid_deleted = true;
        }
      }
    }
  }
  if (npats == 0) {
    p.error = "An empty matrix would result";
    return;
  }
  if (keepers.size() == here->dm_feat.width()) {
    p.error = "Select some rows for deletion";
    return;
  }

  if ( centroid_deleted == true ) {
    // Erase centroid map entries now that we are sure we have valid selections
    for (j=0; j<mj; ++j) {
      sl = here->dm_sel.xslice(0, j, here->dm_sel.width());
      if (std::find(sl.begin(), sl.end(), true) != sl.end()) {
        for ( cent_map_iter = here->centroid_map.begin(); cent_map_iter != here->centroid_map.end(); cent_map_iter++ ) {
          if ( (*cent_map_iter).second == j ) {
            here->centroid_map.erase((*cent_map_iter).first);
          }
        }
      }
    }
  }

  here->dm_sel.resize(here->dm_sel.width(), keepers.size());
  here->dm_sel.set_all(false);

  stringvector_o oldtitles(here->dm_titles);
  narray<bool> oldtrain(here->dm_training);
  narray<int> oldclass(here->dm_classes);
  narray<double,2> oldfeat(here->dm_feat);

  here->dm_titles.resize(keepers.size());
  here->dm_training.resize(keepers.size());
  here->dm_classes.resize(keepers.size());
  here->dm_feat.resize(here->dm_feat.width(), keepers.size());

  for (j=0; j<keepers.size(); ++j) {
    if (!oldtitles.empty())
      here->dm_titles[j] = oldtitles[keepers[j]%oldtitles.size()];
    if (!oldtrain.empty())
      here->dm_training[j] = oldtrain[keepers[j]%oldtrain.size()];
    if (!oldclass.empty())
      here->dm_classes[j] = oldclass[keepers[j]%oldclass.size()];
    here->dm_feat.xslice(0, j, here->dm_feat.width()).copy(
        oldfeat.xslice(0, keepers[j], oldfeat.width()) );
  }

  // Remap (if necessary) and recompute centroids
  if ( centroid_deleted == true ) {
    here->build_centroid_map();
  }
  for ( cent_map_iter = here->centroid_map.begin(); cent_map_iter != here->centroid_map.end(); cent_map_iter++ ) {
    here->compute_centroid( (*cent_map_iter).first );
  }
}

void pattern_editor_v::cmd_delrow(parm_t &p)
{
  pattern_editor_v *here = p.get_here<pattern_editor_v>();
  int darow;
  int deleted_centroid_class;
  int npats;
  bool centroid_selected;
  std::map<int,int>::iterator cent_map_iter;

  ASSERT_INT(p.token+1, "Parameter must be an integer");
  ASSERT_NONNEG_INT(p.token+1, "Parameter must be nonnegative");

  darow = (p.token+1)->ivalue;

  if (darow >= here->dm_feat.height()) {
    p.error = "Parameter extends beyond the height of the datum";
    return;
  }

  // Centroids and data handled differently
  centroid_selected = false;
  deleted_centroid_class = 1;
  for ( cent_map_iter = here->centroid_map.begin(); cent_map_iter != here->centroid_map.end(); cent_map_iter++ )
    if ( (*cent_map_iter).second == darow ) {
      centroid_selected = true;
      deleted_centroid_class = (*cent_map_iter).first;
      here->centroid_map.erase((*cent_map_iter).first);
      break;
    }
  if ( centroid_selected == false ) { 
    npats = here->dm_feat.height() - here->centroid_map.size();
    if ( npats == 1 ) {
      p.error = "An empty matrix would result";
      return;
    }
    here->adjust_centroids(p, darow);
  }

  // ok, delete the one row now
  stringvector_o oldtitles(here->dm_titles);
  narray<bool> oldtrain(here->dm_training);
  narray<int> oldclass(here->dm_classes);
  narray<double, 2> oldfeat(here->dm_feat);
  narray<bool, 2> oldsel(here->dm_sel);

  // stringvecs are always a pain
  here->dm_titles.resize(oldtitles.size() - 1);
  for (int xo=0, x=0; x<oldtitles.size(); ++x)
    if (x != darow)
      here->dm_titles[xo++] = oldtitles[x];

  here->dm_training.resize(oldtrain.size() - 1);
  here->dm_training.xslice(0, darow).copy(oldtrain.xslice(0, darow));
  here->dm_training.xslice(darow, here->dm_training.size()-darow).copy(oldtrain.xslice(darow+1, oldtrain.size()-darow-1));
  here->dm_classes.resize(oldclass.size() - 1);
  here->dm_classes.xslice(0, darow).copy(oldclass.xslice(0, darow));
  here->dm_classes.xslice(darow, here->dm_classes.size()-darow).copy(oldclass.xslice(darow+1, oldclass.size()-darow-1));
  here->dm_feat.resize(oldfeat.width(), oldfeat.height() - 1);
  here->dm_feat.xyslice(0, 0, here->dm_feat.width(), darow).copy(
      oldfeat.xyslice(0, 0, oldfeat.width(), darow));
  here->dm_feat.xyslice(0, darow, here->dm_feat.width(), here->dm_feat.height()-darow).copy(
      oldfeat.xyslice(0, darow+1, oldfeat.width(), oldfeat.height() - darow - 1));
  here->dm_sel.resize(oldsel.width(), oldsel.height() - 1);
  here->dm_sel.xyslice(0, 0, here->dm_sel.width(), darow).copy(
      oldsel.xyslice(0, 0, oldsel.width(), darow));
  here->dm_sel.xyslice(0, darow, here->dm_sel.width(), here->dm_sel.height()-darow).copy(
      oldsel.xyslice(0, darow+1, oldsel.width(), oldsel.height() - darow - 1));

  if ( centroid_selected == true ) {
    // Crazy case - what if we've uncovered a second centroid by deleting an
    // old one.  Have to recompute the centroid.
    here->build_centroid_map();
    for ( cent_map_iter = here->centroid_map.begin(); cent_map_iter != here->centroid_map.end(); cent_map_iter++ ) {
      if ( (*cent_map_iter).first == deleted_centroid_class ) {
        here->compute_centroid(deleted_centroid_class);
      }
    }
  }
}

scopira::uikit::matrix_editor_base::cmd_t pattern_editor_v::dm_commands[] =
{
  {"XSETONE", 3, cmd_set_one},     // special version, x,y,val
  {"XSETALL", 1, cmd_set},     // special version
  {"s", 1, cmd_set},
  {"set", 1, cmd_set},

  {"+", 0, cmd_mod_add},
  {"-", 0, cmd_mod_sub},
  {"*", 0, cmd_mod_mul},
  {"/", 0, cmd_mod_div},
  {"div", 0, cmd_mod_idiv},
  {"mod", 0, cmd_mod_imod},

  {"add", 0, cmd_sel_add},
  {"sub", 0, cmd_sel_sub},
  {"and", 0, cmd_sel_inter},

  {"selinvert", 0, cmd_sel_invert},
  {"sel", 2, cmd_sel},
  {"rect", 4, cmd_sel_rect},
  {"all", 0, cmd_sel_all},
  {"none", 0, cmd_sel_none},
  {"selrow", 1, cmd_sel_row},
  {"selcol", 1, cmd_sel_col},

  {"cols", 0, cmd_expandcols},
  {"rows", 0, cmd_expandrows},
  {"expandrect", 0, cmd_expandrect},

  {"=", 1, cmd_equals},
  {"!=", 1, cmd_notequals},
  {">", 1, cmd_greaterthan},
  {">=", 1, cmd_greaterthan_equal},
  {"<", 1, cmd_lessthan},
  {"<=", 1, cmd_lessthan_equal},


  {"collabel", 0, cmd_collabel},
  {"coltrain", 0, cmd_coltrain},
  {"fliptrain", 0, cmd_fliptrain},
  {"colclass", 0, cmd_colclass},
  {"features", 0, cmd_features},

  {"labelfill", 0, cmd_labelfill},

  {"re", 2, cmd_resize},
  {"resize", 2, cmd_resize},
  {"crop", 0, cmd_crop},
  {"delcols", 0, cmd_delcols},
  {"delrows", 0, cmd_delrows},
  {"delrow", 1, cmd_delrow},

  {0, 0, 0}
};

