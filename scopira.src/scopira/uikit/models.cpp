
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

#include <scopira/uikit/models.h>
#include <scopira/core/register.h>

using namespace scopira::uikit;

//BBtargets libscopiraui.so

palette_m::palette_m(void)
  : scopira::uikit::narray_m<int,1>(), dm_action(0)
{
}

palette_m::palette_m(const palette_m &src)
  : scopira::uikit::narray_m<int,1>(src), dm_action(src.dm_action)
{
}

shapes_m::shapes_m(void)
  : scopira::uikit::narray_m<int,1>(), dm_action(0)
{
}

shapes_m::shapes_m(const shapes_m &src)
  : scopira::uikit::narray_m<int,1>(src), dm_action(src.dm_action)
{
}

static scopira::core::register_model<scopira::uikit::narray_m<char,1> >
  x2ccc("scopira::uikit::narray_m<char,1>", 0, scopira::core::copyable_model_type());
static scopira::core::register_model<scopira::uikit::narray_m<short,1> >
  x2c("scopira::uikit::narray_m<short,1>", 0, scopira::core::copyable_model_type());
static scopira::core::register_model<scopira::uikit::narray_m<int,1> >
  x2("scopira::uikit::narray_m<int,1>", 0, scopira::core::copyable_model_type());
static scopira::core::register_model<scopira::uikit::narray_m<float,1> >
  x1("scopira::uikit::narray_m<float,1>", 0, scopira::core::copyable_model_type());
static scopira::core::register_model<scopira::uikit::narray_m<double,1> >
  x144("scopira::uikit::narray_m<double,1>", 0, scopira::core::copyable_model_type());

static scopira::core::register_model<scopira::uikit::narray_m<char,2> >
  r2ccc("scopira::uikit::narray_m<char,2>", 0, scopira::core::copyable_model_type());
static scopira::core::register_model<scopira::uikit::narray_m<short,2> >
  r244("scopira::uikit::narray_m<short,2>", 0, scopira::core::copyable_model_type());
static scopira::core::register_model<scopira::uikit::narray_m<int,2> >
  r2("scopira::uikit::narray_m<int,2>", 0, scopira::core::copyable_model_type());
static scopira::core::register_model<scopira::uikit::narray_m<float,2> >
  r441("scopira::uikit::narray_m<float,2>", 0, scopira::core::copyable_model_type());
static scopira::core::register_model<scopira::uikit::narray_m<double,2> >
  r244df("scopira::uikit::narray_m<double,2>", 0, scopira::core::copyable_model_type());

static scopira::core::register_model<scopira::uikit::narray_m<char,3> >
  g2ccc("scopira::uikit::narray_m<char,3>", 0, scopira::core::copyable_model_type());
static scopira::core::register_model<scopira::uikit::narray_m<short,3> >
  g244("scopira::uikit::narray_m<short,3>", 0, scopira::core::copyable_model_type());
static scopira::core::register_model<scopira::uikit::narray_m<int,3> >
  g2("scopira::uikit::narray_m<int,3>", 0, scopira::core::copyable_model_type());
static scopira::core::register_model<scopira::uikit::narray_m<float,3> >
  g441("scopira::uikit::narray_m<float,3>", 0, scopira::core::copyable_model_type());
static scopira::core::register_model<scopira::uikit::narray_m<double,3> >
  g244df("scopira::uikit::narray_m<double,3>", 0, scopira::core::copyable_model_type());

static scopira::core::register_model<scopira::uikit::narray_m<char,4> >
  b2ccc("scopira::uikit::narray_m<char,4>", 0, scopira::core::copyable_model_type());
static scopira::core::register_model<scopira::uikit::narray_m<short,4> >
  b244("scopira::uikit::narray_m<short,4>", 0, scopira::core::copyable_model_type());
static scopira::core::register_model<scopira::uikit::narray_m<int,4> >
  b2("scopira::uikit::narray_m<int,4>", 0, scopira::core::copyable_model_type());
static scopira::core::register_model<scopira::uikit::narray_m<float,4> >
  b441("scopira::uikit::narray_m<float,4>", 0, scopira::core::copyable_model_type());
static scopira::core::register_model<scopira::uikit::narray_m<double,4> >
  b244df("scopira::uikit::narray_m<double,4>", 0, scopira::core::copyable_model_type());

static scopira::core::register_model<scopira::uikit::palette_m>
  h1("scopira::uikit::palette_m", "scopira::uikit::narray_m<int,1>",
  scopira::core::copyable_model_type());

static scopira::core::register_model<scopira::uikit::shapes_m>
  h2("scopira::uikit::shapes_m", "scopira::uikit::narray_m<int,1>",
  scopira::core::copyable_model_type());

static scopira::core::register_model<scopira::uikit::dio_narray_m<char,1> >
  diox2ccc("scopira::uikit::dio_narray_m<char,1>", "scopira::uikit::narray_m<char,1>");
static scopira::core::register_model<scopira::uikit::dio_narray_m<short,1> >
  diox2c("scopira::uikit::dio_narray_m<short,1>", "scopira::uikit::narray_m<short,1>");
static scopira::core::register_model<scopira::uikit::dio_narray_m<int,1> >
  diox2("scopira::uikit::dio_narray_m<int,1>", "scopira::uikit::narray_m<int,1>");
static scopira::core::register_model<scopira::uikit::dio_narray_m<float,1> >
  diox1("scopira::uikit::dio_narray_m<float,1>", "scopira::uikit::narray_m<float,1>");
static scopira::core::register_model<scopira::uikit::dio_narray_m<double,1> >
  diox144("scopira::uikit::dio_narray_m<double,1>", "scopira::uikit::narray_m<double,1>");

static scopira::core::register_model<scopira::uikit::dio_narray_m<char,2> >
  dior2ccc("scopira::uikit::dio_narray_m<char,2>", "scopira::uikit::narray_m<char,2>");
static scopira::core::register_model<scopira::uikit::dio_narray_m<short,2> >
  dior244("scopira::uikit::dio_narray_m<short,2>", "scopira::uikit::narray_m<short,2>");
static scopira::core::register_model<scopira::uikit::dio_narray_m<int,2> >
  dior2("scopira::uikit::dio_narray_m<int,2>", "scopira::uikit::narray_m<int,2>");
static scopira::core::register_model<scopira::uikit::dio_narray_m<float,2> >
  dior441("scopira::uikit::dio_narray_m<float,2>", "scopira::uikit::narray_m<float,2>");
static scopira::core::register_model<scopira::uikit::dio_narray_m<double,2> >
  dior244df("scopira::uikit::dio_narray_m<double,2>", "scopira::uikit::narray_m<double,2>");

static scopira::core::register_model<scopira::uikit::dio_narray_m<char,3> >
  diog2ccc("scopira::uikit::dio_narray_m<char,3>", "scopira::uikit::narray_m<char,3>");
static scopira::core::register_model<scopira::uikit::dio_narray_m<short,3> >
  diog244("scopira::uikit::dio_narray_m<short,3>", "scopira::uikit::narray_m<short,3>");
static scopira::core::register_model<scopira::uikit::dio_narray_m<int,3> >
  diog2("scopira::uikit::dio_narray_m<int,3>", "scopira::uikit::narray_m<int,3>");
static scopira::core::register_model<scopira::uikit::dio_narray_m<float,3> >
  diog441("scopira::uikit::dio_narray_m<float,3>", "scopira::uikit::narray_m<float,3>");
static scopira::core::register_model<scopira::uikit::dio_narray_m<double,3> >
  diog244df("scopira::uikit::dio_narray_m<double,3>", "scopira::uikit::narray_m<double,3>");

static scopira::core::register_model<scopira::uikit::dio_narray_m<char,4> >
  diob2ccc("scopira::uikit::dio_narray_m<char,4>", "scopira::uikit::narray_m<char,4>");
static scopira::core::register_model<scopira::uikit::dio_narray_m<short,4> >
  diob244("scopira::uikit::dio_narray_m<short,4>", "scopira::uikit::narray_m<short,4>");
static scopira::core::register_model<scopira::uikit::dio_narray_m<int,4> >
  diob2("scopira::uikit::dio_narray_m<int,4>", "scopira::uikit::narray_m<int,4>");
static scopira::core::register_model<scopira::uikit::dio_narray_m<float,4> >
  diob441("scopira::uikit::dio_narray_m<float,4>", "scopira::uikit::narray_m<float,4>");
static scopira::core::register_model<scopira::uikit::dio_narray_m<double,4> >
  diob244df("scopira::uikit::dio_narray_m<double,4>", "scopira::uikit::narray_m<double,4>");

