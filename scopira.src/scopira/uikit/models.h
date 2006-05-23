
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

#ifndef __INCLUDED_SCOPIRA_UIKIT_MODELS_H__
#define __INCLUDED_SCOPIRA_UIKIT_MODELS_H__

#include <scopira/core/model.h>
#include <scopira/basekit/narray.h>
#include <scopira/basekit/directio.h>
#include <scopira/coreui/export.h>

namespace scopira
{
  /**
   * This namespace houses more complex UI elelements, aswell
   * as all the core models and views.
   *
   * @author Aleksander Demko
   */ 
  namespace uikit
  {
    template <class T, int DIM=1> class narray_m;
        
    class palette_m;
    class shapes_m;

    template <class T, int DIM=1> class dio_narray_m;
  }
}

/**
 * A templated model that simply wraps a narray_o.
 * This is preregistered for char|short|int|float|double of dimen 1,2,3,4.
 *
 * @author Aleksander Demko
 */ 
template <class T, int DIM>
class scopira::uikit::narray_m : public virtual scopira::core::model_i
{
  public:
    scopira::tool::count_ptr<scopira::basekit::narray_o<T, DIM> > pm_array;
  public:
    /// ctor
    narray_m(void);
    /// copy ctor
    narray_m(const narray_m<T,DIM> &src);
    /// creates a new one, of the given type. will zero it too
    narray_m(const scopira::basekit::nindex<DIM> &sz);

    virtual bool load(tool::iobjflow_i& in);
    virtual void save(tool::oobjflow_i& out) const;
};

template <class T, int DIM>
scopira::uikit::narray_m<T,DIM>::narray_m(void)
  : pm_array(new scopira::basekit::narray_o<T,DIM>)
{
}

template <class T, int DIM>
scopira::uikit::narray_m<T,DIM>::narray_m(const narray_m<T,DIM> &src)
  : scopira::core::model_i(src),
  pm_array(new scopira::basekit::narray_o<T,DIM>(src.pm_array.ref()))
{
}

template <class T, int DIM>
scopira::uikit::narray_m<T,DIM>::narray_m(const scopira::basekit::nindex<DIM> &sz)
  : pm_array(new scopira::basekit::narray_o<T,DIM>(sz))
{
  pm_array->set_all(0);
}

template <class T, int DIM>
bool scopira::uikit::narray_m<T,DIM>::load(tool::iobjflow_i& in)
{  
  return scopira::core::model_i::load(in) &&      
         in.read_object_type(pm_array);
}

template <class T, int DIM>
void scopira::uikit::narray_m<T,DIM>::save(tool::oobjflow_i& out) const
{
  scopira::core::model_i::save(out);
  out.write_object_type(pm_array);
}

/**
 * A specifuc palette type.
 *
 * @author Aleksander Demko
 */ 
class scopira::uikit::palette_m : public scopira::uikit::narray_m<int,1>
{
  public:
    int dm_action;
    
  public:
    /// ctor
    SCOPIRAUI_EXPORT palette_m(void);
    /// copy ctor
    SCOPIRAUI_EXPORT palette_m(const palette_m &src);
};

/**
 * A specifuc palette type.
 *
 * @author Aleksander Demko
 */ 
class scopira::uikit::shapes_m : public scopira::uikit::narray_m<int,1>
{
  public:
    int dm_action;
    
  public:
    SCOPIRAUI_EXPORT shapes_m(void);
    /// copy ctor
    SCOPIRAUI_EXPORT shapes_m(const shapes_m &src);
};

/**
 * A templated model that simply wraps a narray_o.
 * This is preregistered for char|short|int|float|double of dimen 1,2,3,4.
 *
 * @author Aleksander Demko
 */ 
template <class T, int DIM>
class scopira::uikit::dio_narray_m : public scopira::uikit::narray_m<T,DIM>
{
  private:
    typedef scopira::uikit::narray_m<T,DIM> parent_type;
  public:
    std::string pm_diofilename;
  public:
    /// ctor
    dio_narray_m(void);

    virtual bool load(tool::iobjflow_i& in);
    virtual void save(tool::oobjflow_i& out) const;
  private:
    /// copy ctor (disabled)
    dio_narray_m(const dio_narray_m<T,DIM> &src) { }
};

template <class T, int DIM>
scopira::uikit::dio_narray_m<T,DIM>::dio_narray_m(void)
{
}

template <class T, int DIM>
bool scopira::uikit::dio_narray_m<T,DIM>::load(tool::iobjflow_i& in)
{  
  bool b;

  b = scopira::core::model_i::load(in) && in.read_string(pm_diofilename);

  if (!b)
    return false;

  if (pm_diofilename.empty())
    return true;

  // I dont consider a DIO error to be a load() erro
  scopira::tool::count_ptr<scopira::basekit::narray_o<T,DIM> >
    d = new scopira::basekit::narray_o<T,DIM>;

  if (scopira::basekit::bind_directio_file(pm_diofilename, *d, false))
    parent_type::pm_array = d;

  return true;
}

template <class T, int DIM>
void scopira::uikit::dio_narray_m<T,DIM>::save(tool::oobjflow_i& out) const
{
  scopira::core::model_i::save(out);
  // SKIP data save, as we are BOUND
  out.write_string(pm_diofilename);
}

#endif

