
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

#include <scopira/uikit/directio.h>

#include <scopira/tool/util.h>
#include <scopira/core/register.h>
#include <scopira/core/project.h>
#include <scopira/coreui/label.h>
#include <scopira/coreui/layout.h>
#include <scopira/coreui/messagewindow.h>
#include <scopira/uikit/models.h>

//BBlibs scopiraxml
//BBtargets libscopiraui.so

using namespace scopira::tool;
using namespace scopira::basekit;
using namespace scopira::coreui;
using namespace scopira::uikit;

static scopira::core::register_view<new_array_v> x1("scopira::uikit::new_array_v",
    scopira::core::project_type_c, "Basic/New array",
    scopira::core::windowonly_uimode_c);

static scopira::core::register_view<load_array_v> x2("scopira::uikit::load_array_v",
    scopira::core::project_type_c, "Basic/Open binary array file",
    scopira::core::windowonly_uimode_c);

static scopira::core::register_view<export_array_v<char,1> > c1("scopira::uikit::export_array_v<char,1>",
    "scopira::uikit::narray_m<char,1>", "Save as data file",
    scopira::core::windowonly_uimode_c);
static scopira::core::register_view<export_array_v<short,1> > r1("scopira::uikit::export_array_v<short,1>",
    "scopira::uikit::narray_m<short,1>", "Save as data file",
    scopira::core::windowonly_uimode_c);
static scopira::core::register_view<export_array_v<int,1> > r2("scopira::uikit::export_array_v<int,1>",
    "scopira::uikit::narray_m<int,1>", "Save as data file",
    scopira::core::windowonly_uimode_c);
static scopira::core::register_view<export_array_v<float,1> > r3("scopira::uikit::export_array_v<float,1>",
    "scopira::uikit::narray_m<float,1>", "Save as data file",
    scopira::core::windowonly_uimode_c);
static scopira::core::register_view<export_array_v<double,1> > r4("scopira::uikit::export_array_v<double,1>",
    "scopira::uikit::narray_m<double,1>", "Save as data file",
    scopira::core::windowonly_uimode_c);

static scopira::core::register_view<export_array_v<char,2> > c5("scopira::uikit::export_array_v<char,2>",
    "scopira::uikit::narray_m<char,2>", "Save as data file",
    scopira::core::windowonly_uimode_c);
static scopira::core::register_view<export_array_v<short,2> > r5("scopira::uikit::export_array_v<short,2>",
    "scopira::uikit::narray_m<short,2>", "Save as data file",
    scopira::core::windowonly_uimode_c);
static scopira::core::register_view<export_array_v<int,2> > r6("scopira::uikit::export_array_v<int,2>",
    "scopira::uikit::narray_m<int,2>", "Save as data file",
    scopira::core::windowonly_uimode_c);
static scopira::core::register_view<export_array_v<float,2> > r7("scopira::uikit::export_array_v<float,2>",
    "scopira::uikit::narray_m<float,2>", "Save as data file",
    scopira::core::windowonly_uimode_c);
static scopira::core::register_view<export_array_v<double,2> > r8("scopira::uikit::export_array_v<double,2>",
    "scopira::uikit::narray_m<double,2>", "Save as data file",
    scopira::core::windowonly_uimode_c);

static scopira::core::register_view<export_array_v<char,3> > c9("scopira::uikit::export_array_v<char,3>",
    "scopira::uikit::narray_m<char,3>", "Save as data file",
    scopira::core::windowonly_uimode_c);
static scopira::core::register_view<export_array_v<short,3> > r9("scopira::uikit::export_array_v<short,3>",
    "scopira::uikit::narray_m<short,3>", "Save as data file",
    scopira::core::windowonly_uimode_c);
static scopira::core::register_view<export_array_v<int,3> > r10("scopira::uikit::export_array_v<int,3>",
    "scopira::uikit::narray_m<int,3>", "Save as data file",
    scopira::core::windowonly_uimode_c);
static scopira::core::register_view<export_array_v<float,3> > r11("scopira::uikit::export_array_v<float,3>",
    "scopira::uikit::narray_m<float,3>", "Save as data file",
    scopira::core::windowonly_uimode_c);
static scopira::core::register_view<export_array_v<double,3> > r12("scopira::uikit::export_array_v<double,3>",
    "scopira::uikit::narray_m<double,3>", "Save as data file",
    scopira::core::windowonly_uimode_c);

static scopira::core::register_view<export_array_v<char,4> > c13("scopira::uikit::export_array_v<char,4>",
    "scopira::uikit::narray_m<char,4>", "Save as data file",
    scopira::core::windowonly_uimode_c);
static scopira::core::register_view<export_array_v<short,4> > r13("scopira::uikit::export_array_v<short,4>",
    "scopira::uikit::narray_m<short,4>", "Save as data file",
    scopira::core::windowonly_uimode_c);
static scopira::core::register_view<export_array_v<int,4> > r14("scopira::uikit::export_array_v<int,4>",
    "scopira::uikit::narray_m<int,4>", "Save as data file",
    scopira::core::windowonly_uimode_c);
static scopira::core::register_view<export_array_v<float,4> > r15("scopira::uikit::export_array_v<float,4>",
    "scopira::uikit::narray_m<float,4>", "Save as data file",
    scopira::core::windowonly_uimode_c);
static scopira::core::register_view<export_array_v<double,4> > r16("scopira::uikit::export_array_v<double,4>",
    "scopira::uikit::narray_m<double,4>", "Save as data file",
    scopira::core::windowonly_uimode_c);

//
//
// new_array_v
//
//

new_array_v::new_array_v(void)
{
  count_ptr<grid_layout> g;

  g = new grid_layout(2, 3);
  dm_type = new dropdown;
  dm_dimen = new entry;

  dm_type->add_selection(char_c, "Int 8-bit (\"char\")");
  dm_type->add_selection(short_c, "Int 16-bit (\"short\")");
  dm_type->add_selection(int_c, "Int 32-bit");
  dm_type->add_selection(float_c, "Float 32-bit");
  dm_type->add_selection(double_c, "Float 64-bit (\"double\")");
  dm_type->set_selection(double_c);

  dm_dimen->set_text("10 20");

  g->add_widget(dm_type.get(), 1, 0);
  g->add_widget(dm_dimen.get(), 1, 1);
  g->add_widget(new label("Data type:"), 0, 0);
  g->add_widget(new label("Dimensions:"), 0, 1);
  g->add_widget(new label("Example dimensions:"), 0, 2);
  g->add_widget(new label("10\n20 30\n30 30 30"), 1, 2);

  set_view_title("New array");

  viewwidget::init_gui(g.get(), button_ok_c|button_apply_c|button_close_c);
}

void new_array_v::bind_model(scopira::core::model_i *sus)
{
  dm_model = dynamic_cast<scopira::core::project_i*>(sus);
}

void new_array_v::react_button(scopira::coreui::button *source, int actionid)
{
  if (actionid == action_apply_c) {
    source->flush_actions(true);    // assume failure, until proven innocent

    std::vector<std::string> vals;
    std::vector<size_t> nvals;
    count_ptr<scopira::core::model_i> newmod;
    int i;

    // parse the dimen string
    string_tokenize(dm_dimen->get_text(), vals, " ,;");

    if (vals.empty()) {
      messagewindow::popup_error("The dimension specification cannot be empty");
      return;
    }
    if (vals.size() > 4) {
      messagewindow::popup_error("The maximum number of dimensions is 4");
      return;
    }

    nvals.resize(vals.size());
    for (i=0; i<vals.size(); ++i)
      if (! (string_to_size_t(vals[i], nvals[i]) && nvals[i] > 0) ) {
        messagewindow::popup_error(vals[i] + " is not a valid number");
        return;
      }

    newmod = make_array(dm_type->get_selection(), nvals);
    assert(newmod.get());

    // add it to the project
    newmod->set_title("New array");
    dm_model->add_model(0, newmod.get());
    dm_model->notify_views(0);

    source->flush_actions(false);
  }

  // propagate
  viewwidget::react_button(source, actionid);
}

scopira::core::model_i * new_array_v::make_array(int ty, const std::vector<size_t> &dimen)
{
  // mapping from run time to compile time is always tedious...
  switch (ty) {
    case char_c:
      switch (dimen.size()) {
        case 1: return new narray_m<char,1>(nindex<1>(dimen[0]));
        case 2: return new narray_m<char,2>(nindex<2>(dimen[0],dimen[1]));
        case 3: return new narray_m<char,3>(nindex<3>(dimen[0],dimen[1],dimen[2]));
        case 4: return new narray_m<char,4>(nindex<4>(dimen[0],dimen[1],dimen[2],dimen[3]));
      }
    case short_c:
      switch (dimen.size()) {
        case 1: return new narray_m<short,1>(nindex<1>(dimen[0]));
        case 2: return new narray_m<short,2>(nindex<2>(dimen[0],dimen[1]));
        case 3: return new narray_m<short,3>(nindex<3>(dimen[0],dimen[1],dimen[2]));
        case 4: return new narray_m<short,4>(nindex<4>(dimen[0],dimen[1],dimen[2],dimen[3]));
      }
    case int_c:
      switch (dimen.size()) {
        case 1: return new narray_m<int,1>(nindex<1>(dimen[0]));
        case 2: return new narray_m<int,2>(nindex<2>(dimen[0],dimen[1]));
        case 3: return new narray_m<int,3>(nindex<3>(dimen[0],dimen[1],dimen[2]));
        case 4: return new narray_m<int,4>(nindex<4>(dimen[0],dimen[1],dimen[2],dimen[3]));
      }
    case float_c:
      switch (dimen.size()) {
        case 1: return new narray_m<float,1>(nindex<1>(dimen[0]));
        case 2: return new narray_m<float,2>(nindex<2>(dimen[0],dimen[1]));
        case 3: return new narray_m<float,3>(nindex<3>(dimen[0],dimen[1],dimen[2]));
        case 4: return new narray_m<float,4>(nindex<4>(dimen[0],dimen[1],dimen[2],dimen[3]));
      }
    case double_c:
      switch (dimen.size()) {
        case 1: return new narray_m<double,1>(nindex<1>(dimen[0]));
        case 2: return new narray_m<double,2>(nindex<2>(dimen[0],dimen[1]));
        case 3: return new narray_m<double,3>(nindex<3>(dimen[0],dimen[1],dimen[2]));
        case 4: return new narray_m<double,4>(nindex<4>(dimen[0],dimen[1],dimen[2],dimen[3]));
      }
  }

  assert(false);
  return 0;
}

//
//
// load_array_v
//
//

load_array_v::load_array_v(void)
{
  count_ptr<grid_layout> g;

  g = new grid_layout(2, 2);
  dm_type = new dropdown;
  dm_filename = new fileentry;

  dm_type->add_selection(load_directio_c, "Open DirectIO");
  dm_type->add_selection(bind_directio_c, "Open DirectIO (via Memory Map)");

  dm_filename->set_filename("outfile.xml");

  g->add_widget(dm_type.get(), 1, 0);
  g->add_widget(dm_filename.get(), 1, 1);
  g->add_widget(new label("File type:"), 0, 0);
  g->add_widget(new label("File name:"), 0, 1);

  set_view_title("Open binary array file");

  viewwidget::init_gui(g.get(), button_ok_c|button_apply_c|button_close_c);
}

void load_array_v::bind_model(scopira::core::model_i *sus)
{
  dm_model = dynamic_cast<scopira::core::project_i*>(sus);
}

void load_array_v::react_button(scopira::coreui::button *source, int actionid)
{
  if (actionid == action_apply_c) {
    source->flush_actions(true);    // assume failure, until proven innocent

    std::string etype;
    std::vector<size_t> dimen;
    count_ptr<scopira::core::model_i> newmod;
    int ety;

    if (!query_directio_file(dm_filename->get_filename(), etype, dimen)) {
      messagewindow::popup_error("Failed to open file for inspection: " + dm_filename->get_filename());
      return;
    }

    if (etype == "float64")
      ety = double_c;
    else if (etype == "float32")
      ety = float_c;
    else if (etype == "int32")
      ety = int_c;
    else if (etype == "int16")
      ety = short_c;
    else if (etype == "int8")
      ety = char_c;
    else
      ety = error_c;

    if (ety == error_c) {
      messagewindow::popup_error("Unkown element type: " + etype);
      return;
    }

    newmod = make_array(dm_type->get_selection(), ety, dimen.size(), dm_filename->get_filename());
    if (newmod.is_null()) {
      messagewindow::popup_error("Failed to load file: " + dm_filename->get_filename());
      return;
    }

    // add it to the project
    assert(newmod.get());
    newmod->set_title(dm_filename->get_filename());
    dm_model->add_model(0, newmod.get());
    dm_model->notify_views(0);

    source->flush_actions(false);
  }

  // propagate
  viewwidget::react_button(source, actionid);
}

namespace {
template <class T, int DIM>
inline scopira::core::model_i * goteam_load(const std::string &filename) {
  narray_m<T,DIM> *m = new narray_m<T,DIM>;

  if (load_directio_file(filename, m->pm_array.ref()))
    return m;
  else {
    delete m;
    return 0;
  }
}
template <class T, int DIM>
inline scopira::core::model_i * goteam_bind(const std::string &filename) {
  dio_narray_m<T,DIM> *m = new dio_narray_m<T,DIM>;

  if (bind_directio_file(filename, m->pm_array.ref(), false)) {
    m->pm_diofilename = filename;
    return m;
  } else {
    delete m;
    return 0;
  }
}
}

scopira::core::model_i * load_array_v::make_array(int fty, int ety, int numdim, const std::string &filename)
{
  switch (fty) {
    case load_directio_c:
      switch (ety) {
        case char_c:
          switch (numdim) {
            case 1: return goteam_load<char,1>(filename);
            case 2: return goteam_load<char,2>(filename);
            case 3: return goteam_load<char,3>(filename);
            case 4: return goteam_load<char,4>(filename);
          }
        case short_c:
          switch (numdim) {
            case 1: return goteam_load<short,1>(filename);
            case 2: return goteam_load<short,2>(filename);
            case 3: return goteam_load<short,3>(filename);
            case 4: return goteam_load<short,4>(filename);
          }
        case int_c:
          switch (numdim) {
            case 1: return goteam_load<int,1>(filename);
            case 2: return goteam_load<int,2>(filename);
            case 3: return goteam_load<int,3>(filename);
            case 4: return goteam_load<int,4>(filename);
          }
        case float_c:
          switch (numdim) {
            case 1: return goteam_load<float,1>(filename);
            case 2: return goteam_load<float,2>(filename);
            case 3: return goteam_load<float,3>(filename);
            case 4: return goteam_load<float,4>(filename);
          }
        case double_c:
          switch (numdim) {
            case 1: return goteam_load<double,1>(filename);
            case 2: return goteam_load<double,2>(filename);
            case 3: return goteam_load<double,3>(filename);
            case 4: return goteam_load<double,4>(filename);
          }
      }
    case bind_directio_c:
      switch (ety) {
        case char_c:
          switch (numdim) {
            case 1: return goteam_bind<char,1>(filename);
            case 2: return goteam_bind<char,2>(filename);
            case 3: return goteam_bind<char,3>(filename);
            case 4: return goteam_bind<char,4>(filename);
          }
        case short_c:
          switch (numdim) {
            case 1: return goteam_bind<short,1>(filename);
            case 2: return goteam_bind<short,2>(filename);
            case 3: return goteam_bind<short,3>(filename);
            case 4: return goteam_bind<short,4>(filename);
          }
        case int_c:
          switch (numdim) {
            case 1: return goteam_bind<int,1>(filename);
            case 2: return goteam_bind<int,2>(filename);
            case 3: return goteam_bind<int,3>(filename);
            case 4: return goteam_bind<int,4>(filename);
          }
        case float_c:
          switch (numdim) {
            case 1: return goteam_bind<float,1>(filename);
            case 2: return goteam_bind<float,2>(filename);
            case 3: return goteam_bind<float,3>(filename);
            case 4: return goteam_bind<float,4>(filename);
          }
        case double_c:
          switch (numdim) {
            case 1: return goteam_bind<double,1>(filename);
            case 2: return goteam_bind<double,2>(filename);
            case 3: return goteam_bind<double,3>(filename);
            case 4: return goteam_bind<double,4>(filename);
          }
      }
  }
  return 0;
}

//
//
// export_array_base
//
//

export_array_base::export_array_base(void)
{
  count_ptr<grid_layout> g;

  g = new grid_layout(2, 2);
  dm_type = new dropdown;
  dm_filename = new fileentry;

  dm_type->add_selection(directio_c, "Binary (xml + binary)");
  dm_type->add_selection(textfile_c, "Text file");

  dm_filename->set_filename("outfile.xml");

  g->add_widget(dm_type.get(), 1, 0);
  g->add_widget(dm_filename.get(), 1, 1);
  g->add_widget(new label("File type:"), 0, 0);
  g->add_widget(new label("File ename:"), 0, 1);

  set_view_title("Save as data file");
  viewwidget::init_gui(g.get(), button_ok_c|button_apply_c|button_close_c);
}

/*void export_array_base::bind_model(scopira::core::model_i *sus)
{
  dm_model = dynamic_cast<scopira::core::project_i*>(sus);
}*/

void export_array_base::react_button(scopira::coreui::button *source, int actionid)
{
  if (actionid == action_apply_c) {
    source->flush_actions(true);    // assume failure, until proven innocent

    count_ptr<scopira::core::model_i> newmod;

    if (!export_file(dm_type->get_selection(), dm_filename->get_filename())) {
      messagewindow::popup_error("The export failed");
      return;
    }

    source->flush_actions(false);
  }

  // propagate
  viewwidget::react_button(source, actionid);
}

