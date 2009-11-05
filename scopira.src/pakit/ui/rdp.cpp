
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

#include <pakit/ui/rdp.h>

#include <stdlib.h>

#include <scopira/core/register.h>
#include <scopira/basekit/vectormath.h>
#include <scopira/basekit/color.h>
#include <scopira/coreui/label.h>
#include <scopira/coreui/viewmenu.h>
#include <scopira/coreui/messagewindow.h>

#include <pakit/rdp.h>
#include <pakit/util.h>

//BBlibs scopiraui
//BBtargets libpauikit.so

using namespace scopira::tool;
using namespace scopira::coreui;
using namespace scopira::basekit;
using namespace scopira::uikit;
using namespace pakit;

//
//
// register
//
//

//Hide this for now, as the new system will use more patterns_m
static scopira::core::register_view<rdp_space_v> r1(
    "pakit::rdp_space_v", "pakit::distances_m", "Calc RDP Projections/Old style",
    scopira::core::no_uimode_c);
static scopira::core::register_view<rdp_space_patterns_v> r1B(
    "pakit::rdp_space_patterns_v", "pakit::distances_m", "Calc RDP Projections/Into a data set",
    scopira::core::windowonly_uimode_c);
static scopira::core::register_view<rdp_scatterplot_space_patterns_v> r1C(
    "pakit::rdp_scatterplot_space_patterns_v", "pakit::distances_m", "Calc RDP Projections/Pair of reference pairs (scatterplot-style), into a data set",
    scopira::core::windowonly_uimode_c);
static scopira::core::register_view<rdp_2d_plot_v> r2(
    "pakit::2d_plot_v", "pakit::rdp_2d_m", "View RDP Projection");

//
//
// rdp_space_v
//
//

void rdp_space_v::bind_model(scopira::core::model_i *sus)
{
  if (!sus)
    return;

  distances_m *dist;
  patterns_m *pat;
  count_ptr<rdp_2d_m> out2;
  count_ptr<rdp_3d_m> out3;

  dist = dynamic_cast<distances_m*>(sus);
  assert(dist);
  pat = dist->pm_patterns_link.get();

  out2 = new rdp_2d_m;
  out3 = new rdp_3d_m;

  out2->pm_patterns_link = pat;
  out2->pm_distances_link = dist;
  out2->calc_rdp();
  out2->set_title_auto();
  out2->notify_views(this);

  if (dist->get_project()) {
    dist->get_project()->add_model(dist, out2.get());
    dist->get_project()->notify_views(this);
  }

  out3->pm_patterns_link = pat;
  out3->pm_distances_link = dist;
  if (!out3->calc_rdp()) {
    //set_error_label("Bad distance matrix of 3D RDP space");
    return;
  }
  out3->set_title_auto();
  out3->notify_views(this);

  if (dist->get_project()) {
    dist->get_project()->add_model(dist, out3.get());
    dist->get_project()->notify_views(this);
  }
}

//
//
// rdp_space_patterns_v
//
//

rdp_space_patterns_v::rdp_space_patterns_v(void)
  : dm_model(this)
{
  // init_gui follows:
  count_ptr<grid_layout> g = new grid_layout(2, 2);

  dm_n1 = new spinbutton(0, 99999, 1);
  dm_n2 = new spinbutton(0, 99999, 1);

  g->add_widget(new label("N1:"), 0, 0);
  g->add_widget(dm_n1.get(), 1, 0);
  g->add_widget(new label("N2:"), 0, 1);
  g->add_widget(dm_n2.get(), 1, 1);

  viewwidget::init_gui(g.get(), button_ok_c|button_cancel_c);
}

void rdp_space_patterns_v::bind_model(scopira::core::model_i *sus)
{
  dm_model = dynamic_cast<distances_m*>(sus);
}

void rdp_space_patterns_v::react_button(scopira::coreui::button *source, int actionid)
{
  if (actionid != action_apply_c) {
    viewwidget::react_button(source, actionid);
    return;
  }

  assert(dm_model.get());

  patterns_m *pat;
  count_ptr<patterns_m> out2;
  int n1, n2;
  //count_ptr<rdp_3d_m> out3;

  pat = dm_model->pm_patterns_link.get();
  assert(pat);
  n1 = dm_n1->get_value_as_int();
  n2 = dm_n2->get_value_as_int();

  // verify the n1/n2
  if (n1 < 0 || n2 < 0) {
    messagewindow::popup_error("N1 and N2 must be non-negative");
    source->flush_actions();
    return;
  }
  if (n1 >= dm_model->pm_array->width() || n2 >= dm_model->pm_array->width()) {
    messagewindow::popup_error("N1 and N2 must be less than " + int_to_string(dm_model->pm_array->width()));
    source->flush_actions();
    return;
  }
  if (n1 == n2) {
    messagewindow::popup_error("N1 and N2 may not be the same");
    source->flush_actions();
    return;
  }

  // copy the patterns
//  out2 = new patterns_m(*pat);
  out2 = new patterns_m;
  int npats = pat->pm_data->height(); // number of patterns
  int nattrs = pat->pm_data->width(); // number of data attributes per pattern
  out2->pm_data->resize(nattrs,npats+1);
  out2->pm_classes->resize(npats+1);
  out2->pm_training->resize(npats+1);
  out2->pm_labels->resize(npats+1);
  out2->pm_classes->xslice(0, npats).copy( pat->pm_classes->xslice(0, npats ) );
  out2->pm_training->xslice(0, npats).copy( pat->pm_training->xslice(0, npats ) );
  for ( int pidx = 0; pidx < npats; pidx++ ) {
    out2->pm_labels->set( pidx, pat->pm_labels->get(pidx) );
  }
  // ...don't bother copying data - rdp_calc_2d_space overwrites anyway
  // special case for final "pattern" - the reference points' centroid
  int maxclass;
  max( pat->pm_classes.ref(), maxclass );
  out2->pm_classes.ref()[npats] = -maxclass - 1; // for the new class centroid
  out2->pm_training.ref()[npats] = false;
  out2->pm_labels->set( npats, "centroid_refpoints" );

  // do the work
  rdp_calc_2d_space(dm_model->pm_array.ref(), n1, n2, out2->pm_data.ref());

  // override the training vector for samples n1 and n2
  out2->pm_training.ref()[n1] = false;
  out2->pm_training.ref()[n2] = false;

  // also override the desired class vector.  Set to one higher than highest class
  max( out2->pm_classes.ref(), maxclass );
  out2->pm_classes.ref()[n1] = maxclass + 1;
  out2->pm_classes.ref()[n2] = maxclass + 1;

  // class centroid for reference points
  out2->pm_data.ref().set( 0, npats, ( out2->pm_data.ref().get(0,n1) + 
                                       out2->pm_data.ref().get(0,n2) ) / 2.0 );
  out2->pm_data.ref().set( 1, npats, ( out2->pm_data.ref().get(1,n1) + 
                                       out2->pm_data.ref().get(1,n2) ) / 2.0 );

  out2->set_title(pat->get_title() + "_n1_" + int_to_string(n1) + "_n2_" + int_to_string(n2));
  out2->pm_comment += "; n1=" + int_to_string(n1) + " n2=" + int_to_string(n2);

  if (dm_model->get_project()) {
    dm_model->get_project()->add_model(dm_model.get(), out2.get());
    dm_model->get_project()->notify_views(this);
  }

  viewwidget::react_button(source, actionid);
}

//
//
// rdp_scatterplot_space_patterns_v
//
//

rdp_scatterplot_space_patterns_v::rdp_scatterplot_space_patterns_v(void)
  : dm_model(this)
{
  // init_gui follows:
  count_ptr<grid_layout> g = new grid_layout(1, 1);

  count_ptr<box_layout> box, box2;
  count_ptr<frame> pair1_frame, pair2_frame;

  dm_ref_frame = new frame("Reference Points:");
  g->add_widget(dm_ref_frame.get(),0,0);

  box = new box_layout(true, false); //horiz,homogeneous,spacing
  dm_ref_frame->add_widget(box.get());
  
  // pair 1
  pair1_frame = new frame("Pair 1:");
  box->add_widget(pair1_frame.get(),true,true,2); //wid, expand,fill, padding
  dm_pair1_box = new grid_layout(2,2); // width, heigth, bool homo
  pair1_frame->add_widget(dm_pair1_box.get());

  // pair 2
  pair2_frame = new frame("Pair 2:");
  box->add_widget(pair2_frame.get(),true,true,2); //wid, expand,fill, padding
  dm_pair2_box = new grid_layout(2,2); // width, heigth, bool homo
  pair2_frame->add_widget(dm_pair2_box.get());

  // pair1 n1 entry
  box2 = new box_layout(true, false); //horiz,homogeneous,spacing
  dm_pair1_box->add_widget(box2.get(), 0, 0, 1, 1, true,false); //wid, expand,fill, padding 
  dm_pair1_n1_label = new label("N1:");
  box2->add_widget(dm_pair1_n1_label.get(),false,false,2); //wid, expand,fill, padding
  dm_pair1_n1_entry = new entry();
//  dm_pair1_n1_entry->set_entry_reactor(this);
  box2->add_widget(dm_pair1_n1_entry.get(),true,false); //wid, expand,fill, padding

  // pair1 n2 entry
  box2 = new box_layout(true, false); //horiz,homogeneous,spacing
  dm_pair1_box->add_widget(box2.get(), 0, 1, 1, 1, true,false); //wid, expand,fill, padding 
  dm_pair1_n2_label = new label("N2:");
  box2->add_widget(dm_pair1_n2_label.get(),false,false,2); //wid, expand,fill, padding
  dm_pair1_n2_entry = new entry();
//  dm_pair1_n2_entry->set_entry_reactor(this);  
  box2->add_widget(dm_pair1_n2_entry.get(),true,false); //wid, expand,fill, padding
  
  // pair2 n1 entry
  box2 = new box_layout(true, false); //horiz,homogeneous,spacing
  dm_pair2_box->add_widget(box2.get(), 0, 0, 1, 1, true,false); //wid, expand,fill, padding 
  dm_pair2_n1_label = new label("N1:");
  box2->add_widget(dm_pair2_n1_label.get(),false,false,2); //wid, expand,fill, padding
  dm_pair2_n1_entry = new entry();
//  dm_pair2_n1_entry->set_entry_reactor(this);
  box2->add_widget(dm_pair2_n1_entry.get(),true,false); //wid, expand,fill, padding

  // pair2 n2 entry
  box2 = new box_layout(true, false); //horiz,homogeneous,spacing
  dm_pair2_box->add_widget(box2.get(), 0, 1, 1, 1, true,false); //wid, expand,fill, padding 
  dm_pair2_n2_label = new label("N2:");
  box2->add_widget(dm_pair2_n2_label.get(),false,false,2); //wid, expand,fill, padding
  dm_pair2_n2_entry = new entry();
//  dm_pair2_n2_entry->set_entry_reactor(this);  
  box2->add_widget(dm_pair2_n2_entry.get(),true,false); //wid, expand,fill, padding

  viewwidget::init_gui(g.get(), button_ok_c|button_cancel_c);
}

void rdp_scatterplot_space_patterns_v::bind_model(scopira::core::model_i *sus)
{
  dm_model = dynamic_cast<distances_m*>(sus);
}

void rdp_scatterplot_space_patterns_v::react_button(scopira::coreui::button *source, int actionid)
{
  if (actionid != action_apply_c) {
    viewwidget::react_button(source, actionid);
    return;
  }

  assert(dm_model.get());

  patterns_m *pat;
  count_ptr<patterns_m> out2;
  count_ptr<patterns_m> tmpdata;
  int pair1_n1, pair1_n2;
  int pair2_n1, pair2_n2;
  //count_ptr<rdp_3d_m> out3;

  pat = dm_model->pm_patterns_link.get();
  assert(pat);

  if ( !string_to_int(dm_pair1_n1_entry->get_text(), pair1_n1) ||
       !string_to_int(dm_pair1_n2_entry->get_text(), pair1_n2) ||
       !string_to_int(dm_pair2_n1_entry->get_text(), pair2_n1) ||
       !string_to_int(dm_pair2_n2_entry->get_text(), pair2_n2) )
    return;

  // verify the entries
  if (pair1_n1 < 0 || pair1_n2 < 0 || pair2_n1 < 0 || pair2_n2 < 0) {
    messagewindow::popup_error("Entries must be non-negative");
    source->flush_actions();
    return;
  }
  if (pair1_n1 >= dm_model->pm_array->width() || pair1_n2 >= dm_model->pm_array->width() ||
      pair2_n1 >= dm_model->pm_array->width() || pair2_n2 >= dm_model->pm_array->width()) {
    messagewindow::popup_error("Entries must be less than " + int_to_string(dm_model->pm_array->width()));
    source->flush_actions();
    return;
  }
  if (pair1_n1 == pair1_n2) {
    messagewindow::popup_error("Invalid reference pair 1");
    source->flush_actions();
    return;
  }
  if (pair2_n1 == pair2_n2) {
    messagewindow::popup_error("Invalid reference pair 2");
    source->flush_actions();
    return;
  }

  // copy the patterns
//  out2 = new patterns_m(*pat);
  out2 = new patterns_m;
  tmpdata = new patterns_m;
  int npats = pat->pm_data->height(); // number of patterns
  int nattrs = pat->pm_data->width(); // number of data attributes per pattern
  tmpdata->pm_data->resize(nattrs,npats+1);
  out2->pm_data->resize(2,npats+1);
  out2->pm_classes->resize(npats+1);
  out2->pm_training->resize(npats+1);
  out2->pm_labels->resize(npats+1);
  out2->pm_classes->xslice(0, npats).copy( pat->pm_classes->xslice(0, npats ) );
  out2->pm_training->xslice(0, npats).copy( pat->pm_training->xslice(0, npats ) );
  for ( int pidx = 0; pidx < npats; pidx++ ) {
    out2->pm_labels->set( pidx, pat->pm_labels->get(pidx) );
  }
  // ...don't bother copying data - will be overwritten anyway
  // special case for final "pattern" - the reference points' centroid
  int maxclass;
  max( pat->pm_classes.ref(), maxclass );
  out2->pm_classes.ref()[npats] = -maxclass - 1; // for the new class centroid
  out2->pm_training.ref()[npats] = false;
  out2->pm_labels->set( npats, "centroid_refpoints" );

  // reference pair 1 - rdp coordinates are x coordinates for scatterplot
  rdp_calc_2d_space(dm_model->pm_array.ref(), pair1_n1, pair1_n2, tmpdata->pm_data.ref());
  out2->pm_data->yslice(0,0,npats).copy(tmpdata->pm_data->yslice(0,0,npats));
  // reference pair 2 - rdp coordinates are y coordinates for scatterplot
  rdp_calc_2d_space(dm_model->pm_array.ref(), pair2_n1, pair2_n2, tmpdata->pm_data.ref());
  out2->pm_data->yslice(1,0,npats).copy(tmpdata->pm_data->yslice(0,0,npats));

  // override the training vector for reference samples
  out2->pm_training.ref()[pair1_n1] = false;
  out2->pm_training.ref()[pair1_n2] = false;
  out2->pm_training.ref()[pair2_n1] = false;
  out2->pm_training.ref()[pair2_n2] = false;

  // also override the desired class vector.  Set to one higher than highest class
  max( out2->pm_classes.ref(), maxclass );
  out2->pm_classes.ref()[pair1_n1] = maxclass + 1;
  out2->pm_classes.ref()[pair1_n2] = maxclass + 1;
  out2->pm_classes.ref()[pair2_n1] = maxclass + 1;
  out2->pm_classes.ref()[pair2_n2] = maxclass + 1;

  // class centroid for reference points
  out2->pm_data.ref().set( 0, npats, ( out2->pm_data.ref().get(0,pair1_n1) + 
                                       out2->pm_data.ref().get(0,pair1_n2) +
                                       out2->pm_data.ref().get(0,pair2_n1) +
                                       out2->pm_data.ref().get(0,pair2_n2)) / 4.0 );
  out2->pm_data.ref().set( 1, npats, ( out2->pm_data.ref().get(1,pair1_n1) + 
                                       out2->pm_data.ref().get(1,pair1_n2) +
                                       out2->pm_data.ref().get(1,pair2_n1) +
                                       out2->pm_data.ref().get(1,pair2_n2)) / 4.0 );

  out2->set_title(pat->get_title() + "_p1n1_" + int_to_string(pair1_n1) + "_p1n2_" + int_to_string(pair1_n2)
                                   + "_p2n1_" + int_to_string(pair2_n1) + "_p2n2_" + int_to_string(pair2_n2));
  out2->pm_comment += "; p1n1=" + int_to_string(pair1_n1) + " p1n2=" + int_to_string(pair1_n2)
                     + " p2n1=" + int_to_string(pair2_n1) + " p2n2=" + int_to_string(pair2_n2);

  if (dm_model->get_project()) {
    dm_model->get_project()->add_model(dm_model.get(), out2.get());
    dm_model->get_project()->notify_views(this);
  }

  viewwidget::react_button(source, actionid);
}

//
//
// rdp_2d_pick_v
//
//

rdp_2d_pick_v::rdp_2d_pick_v(void)
  : dm_model(this)
{
  count_ptr<grid_layout> g = new grid_layout(2, 2);

  dm_n1 = new spinbutton;
  dm_n1->set_value(0);
  dm_n2 = new spinbutton;
  dm_n2->set_value(1);

  g->add_widget(new label("N1:"), 0, 0, 1, 1, true, false);
  g->add_widget(dm_n1.get(), 1, 0, 1, 1, true, false);
  g->add_widget(new label("N2:"), 0, 1, 1, 1, true, false);
  g->add_widget(dm_n2.get(), 1, 1, 1, 1, true, false);

  viewwidget::init_gui(g.get(), button_apply_c|button_close_c);
}

void rdp_2d_pick_v::bind_model(scopira::core::model_i *sus)
{
  dm_model = dynamic_cast<rdp_2d_m*>(sus);
}

void rdp_2d_pick_v::react_model_update(scopira::core::model_i *sus, scopira::core::view_i *src)
{
  if (dm_model.is_null())
    return;

  dm_n1->set_value(dm_model->pm_n1);
  dm_n2->set_value(dm_model->pm_n2);
}

void rdp_2d_pick_v::react_button(scopira::coreui::button *source, int actionid)
{
  if (actionid == action_apply_c && dm_model.get()) {
    int n1, n2;

    n1 = dm_n1->get_value_as_int();
    n2 = dm_n2->get_value_as_int();

    if (!dm_model->is_valid_n(n1, n2)) {
      //set_error_label("Bad N1, N2");
      return;
    }

    // make it so
    dm_model->set_n_calc_rdp(this, n1, n2);
  }

  viewwidget::react_button(source, actionid);
}

//
//
// rdp_3d_pick_v
//
//

rdp_3d_pick_v::rdp_3d_pick_v(void)
  : dm_model(this)
{
  count_ptr<grid_layout> g = new grid_layout(2, 3);

  dm_n1 = new spinbutton;
  dm_n1->set_value(0);
  dm_n2 = new spinbutton;
  dm_n2->set_value(1);
  dm_n3 = new spinbutton;
  dm_n3->set_value(2);

  g->add_widget(new label("N1:"), 0, 0, 1, 1, true, false);
  g->add_widget(dm_n1.get(), 1, 0, 1, 1, true, false);
  g->add_widget(new label("N2:"), 0, 1, 1, 1, true, false);
  g->add_widget(dm_n2.get(), 1, 1, 1, 1, true, false);
  g->add_widget(new label("N3:"), 0, 2, 1, 1, true, false);
  g->add_widget(dm_n3.get(), 1, 2, 1, 1, true, false);

  viewwidget::init_gui(g.get(), button_apply_c|button_close_c);
}

void rdp_3d_pick_v::bind_model(scopira::core::model_i *sus)
{
  dm_model = dynamic_cast<rdp_3d_m*>(sus);
}

void rdp_3d_pick_v::react_model_update(scopira::core::model_i *sus, scopira::core::view_i *src)
{
  if (dm_model.is_null())
    return;

  dm_n1->set_value(dm_model->pm_n1);
  dm_n2->set_value(dm_model->pm_n2);
  dm_n3->set_value(dm_model->pm_n3);
}

void rdp_3d_pick_v::react_button(scopira::coreui::button *source, int actionid)
{
  if (actionid == action_apply_c && dm_model.get()) {
    int n1, n2, n3;

    n1 = dm_n1->get_value_as_int();
    n2 = dm_n2->get_value_as_int();
    n3 = dm_n3->get_value_as_int();

    if (!dm_model->is_valid_n(n1, n2, n3)) {
      //set_error_label("Bad N1, N2, N3");
      return;
    }

    // make it so
    dm_model->set_n_calc_rdp(this, n1, n2, n3);
  }

  viewwidget::react_button(source, actionid);
}

//
//
// rdp_2d_plot_v
//
//

rdp_2d_plot_v::rdp_2d_plot_v(void)
  : dm_data(this), dm_colors(this), dm_shapes(this)
{
  dm_data = new rdp_2d_m;
  
  
  dm_colors = new palette_m;
  dm_colors->pm_array = new narray_o<int,1> (64);
  make_pick_palette(dm_colors->pm_array->all_slice());

  dm_shapes = new shapes_m;

  dm_n_click.index = -1;
  dm_n_sel.index = -1;
  dm_gooddata = "new data";
  dm_zoom = 500;

  button_layout *bl = new button_layout(make_drawing_area());
  dm_basewidget = bl;

  bl->add_button(new view_button("Colours", dm_colors));
  bl->add_button(new view_button("Shapes", dm_shapes));

  widget::init_gui(dm_basewidget->get_widget());
}

void rdp_2d_plot_v::handle_menu(scopira::coreui::widget_canvas &v, intptr_t menuid)
{
}

void rdp_2d_plot_v::handle_init(scopira::coreui::widget_canvas &v)
{
  dm_pen.set_widget(this);
}

void rdp_2d_plot_v::handle_resize(scopira::coreui::widget_canvas &v)
{
  dm_gooddata = "resize";
  paint(v);
}

void rdp_2d_plot_v::handle_repaint(scopira::coreui::widget_canvas &v)
{
  paint(v);
}

void rdp_2d_plot_v::handle_press(scopira::coreui::widget_canvas &v, const scopira::coreui::mouse_event &mevt)
{
  int clo;

  if (!mevt.press || mevt.double_press || dm_gooddata!=0)
    return;

  // first click of two?
  if (dm_n_click.index == -1) {
    clo = find_closest_xy(mevt.x, mevt.y);
    if (clo != -1) {
      dm_n_click = dm_coords[clo];

      dm_pen.set_foreground(textcol_c);
      v.draw_rectangle(dm_pen, false, dm_n_click.x-8, dm_n_click.y-8, 13, 13);
    }
    return;
  }

  // must be 2nd click of two then
  clo = find_closest_xy(mevt.x, mevt.y);

  if (clo == -1)
    return; //bail

  dm_n_sel.index = -1;
  if (dm_coords[clo].index == dm_n_click.index) {
    // selected itself, show its numbah
    dm_n_sel = dm_n_click;
    dm_n_click.index = -1;
    paint(v);

    return;
  }

  // we can assume clo != dm_cursel
  // replace, recalc and redraw
  if (dm_data->is_valid_n(dm_n_click.index, dm_coords[clo].index))
    dm_data->set_n_calc_rdp(this, dm_n_click.index, dm_coords[clo].index);

  dm_gooddata = "new n1,n2";

  // reset sel and redraw
  dm_n_click.index = -1;
  paint(v);
}

void rdp_2d_plot_v::handle_zoom(int newfactor)
{
  dm_zoom = newfactor;
  dm_gooddata = "fresh zoom";
  request_redraw();
}

void rdp_2d_plot_v::bind_model(scopira::core::model_i *sus)
{
  if (!sus)
    return;

  dm_data = dynamic_cast<rdp_2d_m*>(sus);
  assert(dm_data.get());
  dm_distancem = dm_data->pm_distances_link->pm_array;
  assert(dm_distancem.get());
  dm_classes = dm_data->pm_patterns_link->pm_classes;
  assert(dm_classes.get());
  calc_numclass();

  request_redraw();
}

void rdp_2d_plot_v::react_model_update(scopira::core::model_i *sus, scopira::core::view_i *src)
{
  dm_gooddata = "inspection";
  request_redraw();
}

const char * rdp_2d_plot_v::prep_data(scopira::coreui::widget_canvas &v)
{
  if (dm_distancem.is_null())
    return "No data (distance matrix)";
  if (dm_data->pm_array->empty())
    return "No points to plot";
  if (dm_numclass <= 0)
    return "You don't have any >0 classes";
  if (dm_colors->pm_array->empty())
    return "Your palette is empty";

  dm_basep.x = 10;
  dm_basep.y = 20;
  dm_sizep.x = v.width()-2*dm_basep.x;
  dm_sizep.y = 3*v.height()/4-2*dm_basep.y;

  if (dm_basep.x>=v.width() || dm_basep.y>=v.height() || dm_sizep.x<20 || dm_sizep.y<20)
    return "You display window is too small";

  int num_pts, N1, N2, numbin, i, oldsel, oldclick;
  double distAB;

  N1 = dm_data->pm_n1;
  N2 = dm_data->pm_n2;

  // dm_zoom is in [0..1000], but well do [0..100]
  // basex + sizew/2*99/100 - (zoom/10)^2/100^2 * sizew/2 * 98/100
  //                 ^ should be one more than...          ^
  dm_A.x = dm_basep.x+dm_sizep.x*99/(2*100) -
    (dm_zoom/10)*(dm_zoom/10)*dm_sizep.x*98/(100*100*2*100);
  //dm_A.x = dm_basep.x + (18*(dm_zoom+10)*(dm_zoom+10)/(19*19)+1)*dm_sizep.x/(2*20);
  //dm_A.x = dm_basep.x + (dm_zoom+10)*dm_sizep.x/(2*20);
  dm_A.y = dm_basep.y + dm_sizep.y - 10;
  dm_B.y = dm_A.y;
  //dm_A.x = dm_basep.y + 9*dm_sizep.y/10;
  dm_B.x = dm_basep.x + dm_sizep.x - (dm_A.x-dm_basep.x);
  // important distance
  distAB = dm_distancem->get(N1, N2);
  // calculate the all important resolution ratio
  dm_perpix = distAB / (dm_B.x - dm_A.x);

  // prep the datum
  num_pts = dm_distancem->width();

  oldclick = dm_n_click.index;
  oldsel = dm_n_sel.index;
  dm_n_click.index = -1;
  dm_n_sel.index = -1;

  // prep the list of points that actually get pixeled (displayed on screen)
  dm_coords.clear();
  dm_coords.reserve(num_pts);

  //
  // histogram stuffs
  //
  dm_hbasep.x = dm_basep.x;
  dm_hbasep.y = dm_basep.y*2 + dm_sizep.y;
  dm_hsizep.x = dm_sizep.x;
  dm_hsizep.y = 1*v.height()/4-2*dm_basep.y;
  dm_binsize = dm_hsizep.x / 20;
  dm_maxbincount = 0;
  assert(dm_binsize>2);

  // set up countvec
  numbin = dm_hsizep.x / dm_binsize;
  assert(dm_numclass>0);
  assert(numbin>0);

  dm_countmatrix.resize(dm_numclass, numbin);
  dm_countmatrix.set_all(0);

  // go through all the data points
  for (i=0; i<num_pts; ++i) {
    pixpoint p;
    int klass_y, x;
    // calc the pixel coords for this point
    p.index = i;
    p.x = static_cast<int>(dm_data->pm_array->get(0, i) / dm_perpix) + dm_A.x;
    p.y = dm_A.y - static_cast<int>(dm_data->pm_array->get(1, i) / dm_perpix);

    // is this point plottable? if so, queue it
    if ((p.x-dm_basep.x)>=0 && (p.x-dm_basep.x)<dm_sizep.x && (p.y-dm_basep.y)>=0 && (p.y-dm_basep.y)<dm_sizep.y) {
      if (i == oldclick)
        dm_n_click = p;
      if (i == oldsel)
        dm_n_sel = p;
      dm_coords.push_back(p);
    }

    // add this point to the histogram
    klass_y = (dm_coords[i].x-dm_hbasep.x)/dm_binsize;
    if (dm_classes->get(i) <= 0)
      continue;   // classless, we dont do these
    if (klass_y<0)
      klass_y = 0;    // left bin spill over
    else if (klass_y>=dm_countmatrix.height())
      klass_y = dm_countmatrix.height() - 1;  // right bin spill over
    x = dm_classes->get(i);
    if (x>0) {
      // we cant possibly bin other class types
      assert(x>0 && x<=dm_countmatrix.width());
      if (++(dm_countmatrix(x-1, klass_y)) > dm_maxbincount)
        dm_maxbincount = dm_countmatrix(x-1, klass_y);
    }
  }

  // title
  dm_title = "Pts: " + int_to_string(N1) + ", " + int_to_string(N2)
    + "   %-Shown: " + int_to_string(100*dm_coords.size()/dm_distancem->height())
    + "   Display-size: (" + double_to_string(dm_sizep.x*dm_perpix) + ",  "
    + double_to_string(dm_sizep.y*dm_perpix)
    + ")  Base-distance: " + double_to_string(distAB);

  return 0;   // all good!
}

void rdp_2d_plot_v::paint(scopira::coreui::widget_canvas &v)
{
  coords_t::const_iterator ii, endii;
  int mbin;
  std::string sep_title;

  // clear
  dm_pen.set_foreground(backcol_c);
  v.clear(dm_pen);
  dm_pen.set_foreground(textcol_c);

  // check the data
  if (dm_gooddata)
    dm_gooddata = prep_data(v);
  if (dm_gooddata) {
    v.draw_text(dm_pen, dm_font, 5, 5, dm_gooddata);
    return;
  }

  // bounding box
  v.draw_rectangle(dm_pen, false, dm_basep, dm_sizep);
  v.draw_rectangle(dm_pen, false, dm_A.x, dm_basep.y, dm_B.x-dm_A.x, dm_sizep.y);
  v.draw_line(dm_pen, dm_A, dm_B);

  //draw rectangle around the selected point
  if (dm_n_click.index != -1)
    v.draw_rectangle(dm_pen, false, dm_n_click.x-8,
      dm_n_click.y-8, 13, 13);

  mbin = dm_binsize / dm_numclass;

  assert(mbin > 2);

  endii = dm_coords.end();
  for (ii=dm_coords.begin(); ii != endii; ++ii)
    paint_point(v, (*ii).index, (*ii).x, (*ii).y);

  //
  // histogram
  //

  // paint histogram
  for (int clsi=0; clsi<dm_countmatrix.width(); ++clsi) {
    for (int binnum=0; binnum<dm_countmatrix.height(); ++binnum) {
      int ht;

      ht = dm_hsizep.y * dm_countmatrix(clsi,binnum)/dm_maxbincount;
      if (ht>0) {
        dm_pen.set_foreground(dm_colors->pm_array->get(clsi % dm_colors->pm_array->size()));
        v.draw_rectangle(dm_pen, true, binnum*dm_binsize+dm_hbasep.x+clsi*mbin,
          dm_hbasep.y+dm_hsizep.y-ht, mbin, ht);
        dm_pen.set_foreground(textcol_c);
        v.draw_rectangle(dm_pen, false, binnum*dm_binsize+dm_hbasep.x+clsi*mbin,
          dm_hbasep.y+dm_hsizep.y-ht, mbin, ht);
      } else
        dm_pen.set_foreground(textcol_c);
      v.draw_line(dm_pen, (binnum+1)*dm_binsize+dm_hbasep.x, dm_hbasep.y,
        (binnum+1)*dm_binsize+dm_hbasep.x, dm_hbasep.y+dm_hsizep.y);
      if (dm_countmatrix(clsi,binnum)>0)
        v.draw_text(dm_pen, dm_font, 2+dm_hbasep.x+binnum*dm_binsize+clsi*mbin,
          dm_hbasep.y+dm_hsizep.y+2,
          int_to_string(dm_countmatrix(clsi,binnum)));
    }//inner for
  }//outer for

  dm_pen.set_foreground(textcol_c);
  v.draw_rectangle(dm_pen, false, dm_hbasep, dm_hsizep);

  // title
  v.draw_text(dm_pen, dm_font, dm_basep.x, 1, dm_title);

  // anything selected?
  if (dm_n_sel.index != -1) {
    v.draw_text(dm_pen, dm_font, dm_n_sel.x, dm_n_sel.y , int_to_string(dm_n_sel.index));
    v.draw_text(dm_pen, dm_font, dm_basep.x, dm_basep.y+dm_sizep.y+2,
      "Pts: " + int_to_string(dm_n_sel.index)
      + "   Distances: " + double_to_string(dm_distancem->get(dm_data->pm_n1, dm_n_sel.index)) + ", " +
      double_to_string(dm_distancem->get(dm_data->pm_n2, dm_n_sel.index)) );
  }
}

void rdp_2d_plot_v::paint_point(scopira::coreui::widget_canvas &v, int idx, int x, int y)
{
  if (dm_classes->get(idx) <= 0)
    dm_pen.set_foreground(textcol_c);
  else
    dm_pen.set_foreground(dm_colors->pm_array->get((::abs(dm_classes->get(idx))-1) % dm_colors->pm_array->size()));

  v.draw_rectangle(dm_pen, true, x-5, y-5, 8, 8);

  if (dm_classes->get(idx)<=0 || idx == dm_data->pm_n1 || idx == dm_data->pm_n2) {
    dm_pen.set_foreground(textcol_c);
    v.draw_text(dm_pen, dm_font, x, y, int_to_string(idx));
  }
}

int rdp_2d_plot_v::find_closest_xy(int x, int y)
{
  int best_distq, best_idx, dist, i;

  if (dm_coords.empty())
    return -1;  // no datum

  // no need to sqaure, since we'return just comparing
  best_idx = 0;
  best_distq = (dm_coords[0].x-x)*(dm_coords[0].x-x) + (dm_coords[0].y-y)*(dm_coords[0].y-y);

  for (i=1; i<dm_coords.size(); ++i) {
    dist = (dm_coords[i].x-x)*(dm_coords[i].x-x) + (dm_coords[i].y-y)*(dm_coords[i].y-y);
    if (dist < best_distq) {
      best_distq = dist;
      best_idx = i;
    }
  }

  return best_idx;
}

void rdp_2d_plot_v::calc_numclass(void)
{
  if (dm_classes->empty())
    dm_numclass = 0;
  else
    max(dm_classes.ref(), dm_numclass);
}

