
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

#ifndef __INCLUDED_SCOPIRA_UIKIT_DIRECTIO_H__
#define __INCLUDED_SCOPIRA_UIKIT_DIRECTIO_H__

#include <scopira/tool/fileflow.h>
#include <scopira/tool/cacheflow.h>
#include <scopira/basekit/directio.h>
#include <scopira/coreui/export.h>
#include <scopira/coreui/viewwidget.h>
#include <scopira/coreui/dropdown.h>
#include <scopira/coreui/entry.h>
#include <scopira/coreui/filewindow.h>
#include <scopira/uikit/models.h>

namespace scopira
{
  namespace uikit
  {
    class new_array_v;
    class load_array_v;
    class export_array_base;
    template <class T, int DIM> class export_array_v;
  }
}

/**
 * Creates new data array models.
 *
 * @author Aleksander Demko
 */
class scopira::uikit::new_array_v : public scopira::coreui::viewwidget
{
  public:
    SCOPIRAUI_EXPORT new_array_v(void);

    SCOPIRAUI_EXPORT virtual void bind_model(scopira::core::model_i *sus);

  protected:
    SCOPIRAUI_EXPORT virtual void react_button(scopira::coreui::button *source, int actionid);

  private:
    enum {
      char_c,
      short_c,
      int_c,
      float_c,
      double_c,
    };

    scopira::tool::count_ptr<scopira::core::project_i> dm_model;

    scopira::tool::count_ptr<scopira::coreui::dropdown> dm_type;
    scopira::tool::count_ptr<scopira::coreui::entry> dm_dimen;

  private:
    scopira::core::model_i * make_array(int ty, const std::vector<size_t> &dimen);
};

/**
 * Loads an array from from.
 *
 * @author Aleksander Demko
 */
class scopira::uikit::load_array_v : public scopira::coreui::viewwidget
{
  public:
    SCOPIRAUI_EXPORT load_array_v(void);

    SCOPIRAUI_EXPORT virtual void bind_model(scopira::core::model_i *sus);

  protected:
    SCOPIRAUI_EXPORT virtual void react_button(scopira::coreui::button *source, int actionid);

  private:
    enum {
      load_directio_c,
      bind_directio_c,
      // in the future, add text files?
    };
    enum {
      char_c,
      short_c,
      int_c,
      float_c,
      double_c,
      error_c,
    };

    scopira::tool::count_ptr<scopira::core::project_i> dm_model;

    scopira::tool::count_ptr<scopira::coreui::dropdown> dm_type;
    scopira::tool::count_ptr<scopira::coreui::fileentry> dm_filename;

  private:
    scopira::core::model_i * make_array(int fty, int ety, int numdim, const std::string &filename);
};

/**
 * A general exporter of narray_ms
 *
 * @author Aleksander Demko
 */
class scopira::uikit::export_array_base : public scopira::coreui::viewwidget
{
  public:
    SCOPIRAUI_EXPORT export_array_base(void);

  protected:
    SCOPIRAUI_EXPORT virtual void react_button(scopira::coreui::button *source, int actionid);

    enum {
      directio_c,
      textfile_c,
    };

    /// template decendants override this, null on failure
    SCOPIRAUI_EXPORT virtual bool export_file(int ty, const std::string &filename) = 0;

  private:
    scopira::tool::count_ptr<scopira::coreui::dropdown> dm_type;
    scopira::tool::count_ptr<scopira::coreui::fileentry> dm_filename;
};

template <class T, int DIM>
class scopira::uikit::export_array_v : public scopira::uikit::export_array_base
{
  public:
    virtual void bind_model(scopira::core::model_i *sus);
  protected:
    virtual bool export_file(int ty, const std::string &filename);
  private:
    scopira::tool::count_ptr<scopira::uikit::narray_m<T,DIM> > dm_model;
};

template <class T, int DIM>
void scopira::uikit::export_array_v<T,DIM>::bind_model(scopira::core::model_i *sus)
{
  dm_model = dynamic_cast<scopira::uikit::narray_m<T,DIM> * >(sus);
}

template <class T, int DIM>
bool scopira::uikit::export_array_v<T,DIM>::export_file(int ty,
    const std::string &filename)
{
  if (ty == directio_c)
    return scopira::basekit::save_directio_file(filename, *(dm_model->pm_array));

  assert(ty == textfile_c);

  scopira::tool::fileflow inf(filename, scopira::tool::fileflow::output_c);

  if (inf.failed())
    return false;

  if (dm_model->pm_array->empty())
    return true;

  scopira::tool::cacheoflow out(false, &inf);

  for (size_t y=0; y<dm_model->pm_array->height(); ++y) {
    out << dm_model->pm_array->get(0, y);
    for (size_t x=1; x<dm_model->pm_array->width(); ++x)
      out << ' ' << dm_model->pm_array->get(x, y);
    out << '\n';
  }

  return true;
}

#endif

