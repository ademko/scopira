
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

#include <pakit/ui/plot.h>

#include <scopira/core/register.h>
#include <scopira/coreui/spinbutton.h>
#include <scopira/coreui/label.h>

//BBlibs scopiraui
//BBtargets libpauikit.so

using namespace scopira::tool;
using namespace scopira::basekit;
using namespace scopira::core;
using namespace scopira::coreui;
using namespace pakit;

//
//
// register
//
//

static scopira::core::register_view<plot_spectra_v> r1("pakit::plot_spectra_v", "pakit::patterns_m", "View Slice");

//
//
// plot_spectra_v
//
//

plot_spectra_v::plot_spectra_v(void)
  : parent_type(0), dm_model(this)
{
	dm_slice = 0;
	
	init_gui();
}

void plot_spectra_v::react_spinbutton(scopira::coreui::spinbutton *source, int intval, double doubleval)
{
  if (dm_model.get() && intval != dm_slice) {
    if ( intval >= dm_model->pm_data->height() ) {
      if ( dm_slice == 0 ) {
        dm_slice = dm_model->pm_data->height() - 1;
      }
      else {
        dm_slice = 0;
      }
    }
    else {
      dm_slice = intval;
    }
    dm_title->set_label(dm_model->pm_labels->get(dm_slice));
    dm_class ->set_label(scopira::tool::int_to_string(dm_model->pm_classes->get(dm_slice)));
    dm_model->notify_views(this);

    update_gui();
  }
}

void plot_spectra_v::bind_model(scopira::core::model_i *sus)
{
  dm_model = dynamic_cast<patterns_m*>(sus);
  if (dm_model.get())
    dm_spinner->set_range(0, dm_model->pm_data->height());
}

void plot_spectra_v::react_model_update(scopira::core::model_i *sus, scopira::core::view_i *src)
{
  if (dm_model.get())
    update_gui();
}

void plot_spectra_v::init_gui(void)
{
  count_ptr<grid_layout> grid = new grid_layout(5, 2);

  dm_spinner = new spinbutton(0, 1, 1);;
  //dm_spinner->set_attrib("label","Slice:");
  dm_spinner->set_spinbutton_reactor(this);
  
  dm_title = new label("[title]");
  dm_class = new label("[class]");

  grid->add_widget(make_drawing_area(), 0, 0, 5, 1, true, true);
  grid->add_widget(dm_spinner.get(), 0, 1, 1, 1, false, false);
  grid->add_widget(new label("Title:"), 1, 1, 1, 1, false, false);
  grid->add_widget(dm_title.get(), 2, 1, 1, 1, false, false);
  grid->add_widget(new label("Class:"), 3, 1, 1, 1, false, false);
  grid->add_widget(dm_class.get(),4, 1, 1, 1, false, false);

  dm_basewidget = grid.get();
  widget::init_gui(dm_basewidget->get_widget());
}

void plot_spectra_v::update_gui(void)
{
  count_ptr<narray_o<double,2> > p;

  assert(dm_model.get());

  p = new narray_o<double,2> ;

  if ((dm_slice < dm_model->pm_data->height()) && (dm_slice >= 0))
    p->copy(dm_model->pm_data->xyslice(0, dm_slice,
          dm_model->pm_data->width(), 1));

  dm_spinner->set_value(dm_slice);
  dm_title->set_label(dm_model->pm_labels->get(dm_slice));
  dm_class ->set_label(scopira::tool::int_to_string(dm_model->pm_classes->get(dm_slice)));
  set_plot_data(p.get());

  request_resize();
}

