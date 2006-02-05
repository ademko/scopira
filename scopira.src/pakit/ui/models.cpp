
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

#include <pakit/ui/models.h>

#include <scopira/basekit/math.h>
#include <scopira/core/register.h>

#include <pakit/rdp.h>

//BBlibs scopiraui
//BBtargets libpauikit.so

using namespace scopira::tool;
using namespace scopira::basekit;
using namespace pakit;

//
//
// register stuff
//
//

static scopira::core::register_model<patterns_m > x2("pakit::patterns_m", 0, scopira::core::copyable_model_type());
static scopira::core::register_model<patterns_project_m > x2b("pakit::patterns_project_m", scopira::core::project_type_c);
static scopira::core::register_model<distances_m > x3("pakit::distances_m", "scopira::uikit::narray_m<double,2>", scopira::core::copyable_model_type());
static scopira::core::register_model<rdp_2d_m > x4("pakit::rdp_2d_m", "scopira::uikit::narray_m<double,2>", scopira::core::copyable_model_type());
static scopira::core::register_model<rdp_3d_m > x5("pakit::rdp_3d_m", "scopira::uikit::narray_m<double,2>", scopira::core::copyable_model_type());
static scopira::core::register_model<probabilities_m> x6("pakit::probabilities_m", "scopira::uikit::narray_m<double,2>", scopira::core::copyable_model_type());

//
//
// patterns_m
//
//

patterns_m::patterns_m(void)
  : patterns_t(true)
{
}

patterns_m::patterns_m(const patterns_m &src)
  : model_i(src), patterns_t(src, true)
{
}

bool patterns_m::load(iobjflow_i& in)
{
  int version;

  return
    model_i::load(in) &&
    patterns_t::load(in) &&

    in.read_int(version) &&

    in.read_string(pm_title);
}

void patterns_m::save(oobjflow_i& out) const
{
  model_i::save(out);
  patterns_t::save(out);

  out.write_int(1);     // version

  out.write_string(pm_title);
}

void patterns_m::sort_by_class(void)
{
  assert(false); //What goes here?
}

//
//
// patterns_project_m
//
//

patterns_project_m::patterns_project_m(void)
{
}

bool patterns_project_m::load(scopira::tool::iobjflow_i& in)
{
  int version;

  return project_base::load(in) && in.read_int(version);
}

void patterns_project_m::save(scopira::tool::oobjflow_i& out) const
{
  project_base::save(out);

  out.write_int(1); // version
}

//
//
// distances_m
//
//


distances_m::distances_m(void)
{
}

distances_m::distances_m(const distances_m &src)
  : parent_type(src), pm_patterns_link(src.pm_patterns_link)
{
}

bool distances_m::load(scopira::tool::iobjflow_i& in)
{
  int version;
  return parent_type::load(in) && in.read_int(version) && in.read_object_type(pm_patterns_link);
}

void distances_m::save(scopira::tool::oobjflow_i& out) const
{
  parent_type::save(out);
  out.write_int(1);
  out.write_object_type(pm_patterns_link);
}

//
//
// probabilities_m
//
//


probabilities_m::probabilities_m(void)
{
}

probabilities_m::probabilities_m(const probabilities_m &src)
  : parent_type(src), pm_patterns_link(src.pm_patterns_link), pm_classes(src.pm_classes), pm_training(src.pm_training)
{
}

bool probabilities_m::load(scopira::tool::iobjflow_i& in)
{
  int version;
  bool b;

  b = in.read_int(version) &&
      parent_type::load(in) &&
      in.read_object_type(pm_patterns_link) &&
      in.read_object_type(pm_classes) &&
      in.read_object_type(pm_training);
      
  if ( !b )
    return false;

  return true;
}

void probabilities_m::save(scopira::tool::oobjflow_i& out) const
{
  int ver;

  ver = 1;
  out.write_int(ver);
  parent_type::save(out);
  out.write_object_type(pm_patterns_link);
  out.write_object_type(pm_classes);
  out.write_object_type(pm_training);
}

//
//
// rdp_2d_m
//
//

rdp_2d_m::rdp_2d_m(void)
{
  pm_n1 = 0;
  pm_n2 = 1;
}

rdp_2d_m::rdp_2d_m(const rdp_2d_m &src)
  : parent_type(src),
  pm_patterns_link(src.pm_patterns_link), pm_distances_link(src.pm_distances_link),
  pm_distances(src.pm_distances),
  pm_n1(src.pm_n1), pm_n2(src.pm_n2)
{
}

bool rdp_2d_m::load(scopira::tool::iobjflow_i& in)
{
  int version;
  return
    parent_type::load(in) &&

    in.read_int(version) &&

    in.read_object_type(pm_patterns_link) &&
    in.read_object_type(pm_distances_link) &&
    in.read_int(pm_n1) &&
    in.read_int(pm_n2);
}

void rdp_2d_m::save(scopira::tool::oobjflow_i& out) const
{
  parent_type::save(out);

  out.write_int(1);

  out.write_object_type(pm_patterns_link);
  out.write_object_type(pm_distances_link);
  out.write_int(pm_n1);
  out.write_int(pm_n2);
}

bool rdp_2d_m::is_valid_n(int n1, int n2, scopira::basekit::narray<double,2> *dmatrix) const
{
  if (!dmatrix)
    dmatrix = pm_distances.get();
  if (!dmatrix)
    dmatrix = pm_distances_link->pm_array.get();
  assert(dmatrix);

  return n1 != n2 && n1>=0 && n2>=0 && n1<dmatrix->width() && n2<dmatrix->width()
    && !is_zero(dmatrix->get(n1,n2));
}

void rdp_2d_m::set_n_calc_rdp(scopira::core::view_i *src, int n1, int n2)
{
  pm_n1 = n1;
  pm_n2 = n2;
  calc_rdp();
  notify_views(src);
  set_title_auto();
}

void rdp_2d_m::calc_rdp(scopira::basekit::narray<double,2> *dmatrix)
{
  if (!dmatrix)
    dmatrix = pm_distances.get();
  if (!dmatrix)
    dmatrix = pm_distances_link->pm_array.get();
  assert(dmatrix);

  count_ptr<narray_o<double,2> > outmat = new narray_o<double,2> ;

  rdp_calc_2d_space(*dmatrix, pm_n1, pm_n2, outmat.ref());

  pm_array = outmat;
}

void rdp_2d_m::set_title_auto(void)
{
  std::string tit = "RDP2D (";

  tit += int_to_string(pm_n1) + "," + int_to_string(pm_n2) + ")";

  set_title(tit);
}

//
//
// rdp_3d_m
//
//

rdp_3d_m::rdp_3d_m(void)
{
  pm_n1 = 0;
  pm_n2 = 1;
  pm_n3 = 2;
}

rdp_3d_m::rdp_3d_m(const rdp_3d_m &src)
  : parent_type(src),
  pm_patterns_link(src.pm_patterns_link), pm_distances_link(src.pm_distances_link),
  pm_distances(src.pm_distances),
  pm_n1(src.pm_n1), pm_n2(src.pm_n2), pm_n3(src.pm_n3)
{
}

bool rdp_3d_m::load(scopira::tool::iobjflow_i& in)
{
  int version;

  return
    parent_type::load(in) &&

    in.read_int(version);

    in.read_object_type(pm_patterns_link) &&
    in.read_object_type(pm_distances_link) &&
    in.read_int(pm_n1) &&
    in.read_int(pm_n2) &&
    in.read_int(pm_n3);
}

void rdp_3d_m::save(scopira::tool::oobjflow_i& out) const
{
  parent_type::save(out);

  out.write_int(1);

  out.write_object_type(pm_patterns_link);
  out.write_object_type(pm_distances_link);
  out.write_int(pm_n1);
  out.write_int(pm_n2);
  out.write_int(pm_n3);
}

bool rdp_3d_m::is_valid_n(int n1, int n2, int n3, scopira::basekit::narray<double,2> *dmatrix) const
{
  if (!dmatrix)
    dmatrix = pm_distances.get();
  if (!dmatrix)
    dmatrix = pm_distances_link->pm_array.get();
  assert(dmatrix);

  return n1 != n2 && n1 != n3 && n2 != n3 && n1>=0 && n2>=0 && n3>=0
    && n1<dmatrix->width() && n2<dmatrix->width() && n3<dmatrix->width()
    && !is_zero(dmatrix->get(n1,n2)) && !is_zero(dmatrix->get(n1,n3))
    && !is_zero(dmatrix->get(n2,n3));
}

void rdp_3d_m::set_n_calc_rdp(scopira::core::view_i *src, int n1, int n2, int n3)
{
  pm_n1 = n1;
  pm_n2 = n2;
  pm_n3 = n3;
  calc_rdp();
  notify_views(src);
  set_title_auto();
}

bool rdp_3d_m::calc_rdp(scopira::basekit::narray<double,2> *dmatrix)
{
  if (!dmatrix)
    dmatrix = pm_distances.get();
  if (!dmatrix)
    dmatrix = pm_distances_link->pm_array.get();
  assert(dmatrix);

  count_ptr<narray_o<double,2> > outmat = new narray_o<double,2> ;

  if (!rdp_calc_3d_space(*dmatrix, pm_n1, pm_n2, pm_n3, outmat.ref()))
    return false;

  pm_array = outmat;

  return true;
}

void rdp_3d_m::set_title_auto(void)
{
  std::string tit = "RDP3D (";

  tit += int_to_string(pm_n1) + "," + int_to_string(pm_n2) + "," + int_to_string(pm_n3) + ")";

  set_title(tit);
}

