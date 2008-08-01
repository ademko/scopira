
/*
 *  Copyright (c) 2003-2006    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#include <scopira/magick/fileio.h>

#include <scopira/core/register.h>
#include <scopira/core/project.h>
#include <scopira/basekit/narray.h>
#include <scopira/coreui/filewindow.h>
#include <scopira/coreui/viewwidget.h>
#include <scopira/coreui/messagewindow.h>
#include <scopira/uikit/models.h>


//BBtargets libscopiramagickui.so
//BBlibs scopiramagick scopiraui

namespace scopira
{
  namespace magick
  {
    class load_image_v;
    class save_image_v;
  }
}

class scopira::magick::load_image_v : public scopira::coreui::viewwidget
{
  public:
    /// ctor
    load_image_v(void);

    virtual void bind_model(scopira::core::model_i *sus);

    virtual void react_button(scopira::coreui::button *source, int actionid);

  private:
    scopira::tool::count_ptr<scopira::coreui::fileentry> dm_filename;

    scopira::core::model_ptr<scopira::core::project_i> dm_model;
};

class scopira::magick::save_image_v : public scopira::coreui::viewwidget
{
  public:
    /// ctor
    save_image_v(void);

    virtual void bind_model(scopira::core::model_i *sus);

    virtual void react_button(scopira::coreui::button *source, int actionid);

  private:
    scopira::tool::count_ptr<scopira::coreui::fileentry> dm_filename;

    scopira::core::model_ptr<scopira::uikit::narray_m<int,2> > dm_model;
};

static scopira::core::register_view<scopira::magick::load_image_v> r1("scopira::magick::load_image_v", scopira::core::project_type_c, "Basic/Open image file", scopira::core::windowonly_uimode_c);
static scopira::core::register_view<scopira::magick::save_image_v> r2("scopira::magick::save_image_v", "scopira::uikit::narray_m<int,2>", "Save as image file", scopira::core::windowonly_uimode_c);

// cpp portion

using namespace scopira::tool;
using namespace scopira::coreui;
using namespace scopira::uikit;
using namespace scopira::magick;

//
//
// load_image_v
//
//

load_image_v::load_image_v(void)
  : dm_model(this)
{
  dm_filename = new fileentry;

  //dm_filename->set_filename("infile.txt");
  dm_filename->set_filename("image.png");

  viewwidget::init_gui(dm_filename.get(), button_ok_c|button_cancel_c);

  set_view_title("Load Image");
}

void load_image_v::bind_model(scopira::core::model_i *sus)
{
  dm_model = dynamic_cast<scopira::core::project_i*>(sus);
}

void load_image_v::react_button(scopira::coreui::button *source, int actionid)
{
  if (actionid == action_apply_c)  {
    assert(dm_model.get());

    // perform the file loading
    count_ptr<narray_m<int, 2> > newimg = new narray_m<int, 2>;

    if (!load_magick_image(dm_filename->get_filename(), newimg->pm_array.ref())) {
      messagewindow::popup_error("Error loading: " + dm_filename->get_filename());
      return;
    }

    // file load ok, I guess, insert the new model
    newimg->set_title(dm_filename->get_filename());
    dm_model->add_model(0, newimg.get());
    dm_model->notify_views(this);
  }

  viewwidget::react_button(source, actionid);
}

//
//
// save_image_v
//
//

save_image_v::save_image_v(void)
  : dm_model(this)
{
  dm_filename = new fileentry;

  //dm_filename->set_filename("infile.txt");
  dm_filename->set_filename("image.png");

  viewwidget::init_gui(dm_filename.get(), button_ok_c|button_cancel_c);

  set_view_title("Save Image");
}

void save_image_v::bind_model(scopira::core::model_i *sus)
{
  dm_model = dynamic_cast<scopira::uikit::narray_m<int,2> * >(sus);
}

void save_image_v::react_button(scopira::coreui::button *source, int actionid)
{
  if (actionid == action_apply_c)  {
    assert(dm_model.get());

    // perform the save
    if (!save_magick_image(dm_filename->get_filename(), dm_model->pm_array.ref())) {
      messagewindow::popup_error("Error saving: " + dm_filename->get_filename());
      return;
    }
  }

  viewwidget::react_button(source, actionid);
}

