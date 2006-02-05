
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

#include <pakit/ui/patternsort.h>

#include <scopira/tool/sort_imp.h>
#include <scopira/core/register.h>
#include <scopira/coreui/label.h>

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

static scopira::core::register_view<pattern_sort_v> r1("pakit::pattern_sort_v",
  "pakit::patterns_m", "Sort Patterns", scopira::core::windowonly_uimode_c);


//
//
// pattern_sort_v
//
//

pattern_sort_v::pattern_sort_v(void)
  : dm_model(this)
{
  count_ptr<grid_layout> g;
  
  g = new grid_layout(2,1);
  dm_drop = new dropdown;

  g->add_widget(new label("Sort by column:"), 0, 0);
  g->add_widget(dm_drop.get(), 1, 0);

  viewwidget::init_gui(g.get(), button_apply_c|button_close_c);
}

void pattern_sort_v::bind_model(scopira::core::model_i *sus)
{
  dm_model = dynamic_cast<patterns_m*>(sus);
  update_gui();
}

void pattern_sort_v::react_model_update(scopira::core::model_i *sus, scopira::core::view_i *src)
{
  update_gui();
}


void pattern_sort_v::react_button(scopira::coreui::button *source, int actionid)
{
  if (dm_model.get() && actionid == action_apply_c) {
    // make sure the column is valid
    assert(dm_drop->get_selection() < static_cast<long>(dm_model->pm_data->width()));  // we need signs

    patterns_t pat(false);

    pat.copy_patterns(dm_model.ref(), true);

    mysort m(dm_drop->get_selection(), pat);
    scopira::tool::qsort(m, 0, dm_model->pm_data->height() - 1);

    dm_model->copy_patterns(pat, false);
    dm_model->notify_views(this);
  }

  viewwidget::react_button(source, actionid);
}

void pattern_sort_v::update_gui(void)
{
  dm_drop->clear_selections();

  if (dm_model.is_null())
    return;

  dm_drop->add_selection(-1, "Title");
  dm_drop->add_selection(-2, "Test/Training");
  dm_drop->add_selection(-3, "Class");

  for (int x=0; x<dm_model->pm_data->width(); ++x)
    dm_drop->add_selection(x, "Column " + int_to_string(x));
}





mysort::mysort(int row, patterns_t &pat)
  : dm_row(row), dm_pat(pat)
{
  dm_string.reserve(100);
  dm_temp.resize(pat.pm_data->width());
}

int mysort::compare_element(int lidx, int ridx) const
{
  int retval;
  switch (dm_row) {
    case -1:
      // compare titles
      return compare<std::string>(dm_pat.pm_labels->get(lidx), dm_pat.pm_labels->get(ridx));
    case -2:
      // compare training
      retval = compare<bool>(dm_pat.pm_training->get(lidx), dm_pat.pm_training->get(ridx));
      break;
    case -3:
      // compare class
      retval = compare<int>(dm_pat.pm_classes->get(lidx), dm_pat.pm_classes->get(ridx));
      if ( ( dm_pat.pm_classes->get(lidx) <= 0 && dm_pat.pm_classes->get(ridx) <= 0 ) &&
           ( dm_pat.pm_labels->get(lidx).find(/*C*/"entroid") != std::string::npos || dm_pat.pm_labels->get(lidx).find(/*P*/"laceholder") != std::string::npos ) &&
           ( dm_pat.pm_labels->get(ridx).find(/*C*/"entroid") != std::string::npos || dm_pat.pm_labels->get(ridx).find(/*P*/"laceholder") != std::string::npos ) )
        retval *= -1;
      break;
    default:
      // data column
      retval = compare<double>(dm_pat.pm_data->get(dm_row, lidx), dm_pat.pm_data->get(dm_row, ridx));
  }
  if ( retval == 0 )
    // titles are always the tiebreaker
    return compare<std::string>(dm_pat.pm_labels->get(lidx), dm_pat.pm_labels->get(ridx));
  else
    return retval;
}

void mysort::swap_element(int lidx, int ridx)
{
  // swap time
  dm_string = dm_pat.pm_labels->get(lidx);
  dm_pat.pm_labels->set(lidx, dm_pat.pm_labels->get(ridx));
  dm_pat.pm_labels->set(ridx, dm_string);

  std::swap(dm_pat.pm_training.ref()[lidx], dm_pat.pm_training.ref()[ridx]);
  std::swap(dm_pat.pm_classes.ref()[lidx], dm_pat.pm_classes.ref()[ridx]);

  // swap rows
  dm_temp.copy( dm_pat.pm_data->xslice(0, lidx, dm_temp.size()) );
  dm_pat.pm_data->xslice(0, lidx, dm_temp.size()).copy( dm_pat.pm_data->xslice(0, ridx, dm_temp.size()) );
  dm_pat.pm_data->xslice(0, ridx, dm_temp.size()).copy( dm_temp );
}

