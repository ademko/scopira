
/*
 *  Copyright (c) 2002-2003    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#include <scopira/basekit/narray.h>

#include <scopira/core/register.h>

//BBtargets libscopira.so

using namespace scopira::tool;

//
//
// nindex<3>
//
//

scopira::basekit::nindex<3>::nindex(size_t _x, size_t _y, size_t _z)
{
  dm_ary[0] = _x;
  dm_ary[1] = _y;
  dm_ary[2] = _z;
}

scopira::basekit::nindex<3>::nindex(size_t v)
{
  dm_ary[0] = v;
  dm_ary[1] = v;
  dm_ary[2] = v;
}

scopira::basekit::nindex<3>::nindex(const this_type &rhs)
  : parent_type(rhs)
{
}

//
//
// nindex<4>
//
//

scopira::basekit::nindex<4>::nindex(size_t _x, size_t _y, size_t _z, size_t _t)
{
  dm_ary[0] = _x;
  dm_ary[1] = _y;
  dm_ary[2] = _z;
  dm_ary[3] = _t;
}

scopira::basekit::nindex<4>::nindex(size_t v)
{
  dm_ary[0] = v;
  dm_ary[1] = v;
  dm_ary[2] = v;
  dm_ary[3] = v;
}

scopira::basekit::nindex<4>::nindex(const this_type &rhs)
  : parent_type(rhs)
{
}

//
// misc
//

namespace scopira
{
  namespace basekit
  {
    class null_narray_delete_t : public scopira::basekit::narray_delete_i
    {
      public:
        virtual void narray_delete(void *mem, size_t len) { }
    };
    class normal_narray_delete_t : public scopira::basekit::narray_delete_i
    {
      public:
        virtual void narray_delete(void *mem, size_t len) { }
    };

    null_narray_delete_t null_narray_delete_t_instance;
    normal_narray_delete_t normal_narray_delete_t_instance;

    // the externally available one
    narray_delete_i *null_narray_delete = &null_narray_delete_t_instance;
    narray_delete_i *normal_narray_delete = &normal_narray_delete_t_instance;
  }
}

// registration objects

static scopira::core::register_flow< scopira::basekit::narray_o<bool,1> > f1("scopira::basekit::narray_o<bool,1>");
static scopira::core::register_flow< scopira::basekit::narray_o<char,1> > f2("scopira::basekit::narray_o<char,1>");
static scopira::core::register_flow< scopira::basekit::narray_o<short,1> > f3("scopira::basekit::narray_o<short,1>");
static scopira::core::register_flow< scopira::basekit::narray_o<int,1> > f4("scopira::basekit::narray_o<int,1>");
static scopira::core::register_flow< scopira::basekit::narray_o<float,1> > f5("scopira::basekit::narray_o<float,1>");
static scopira::core::register_flow< scopira::basekit::narray_o<double,1> > f6("scopira::basekit::narray_o<double,1>");

static scopira::core::register_flow< scopira::basekit::narray_o<bool,2> > f7("scopira::basekit::narray_o<bool,2>");
static scopira::core::register_flow< scopira::basekit::narray_o<char,2> > f8("scopira::basekit::narray_o<char,2>");
static scopira::core::register_flow< scopira::basekit::narray_o<short,2> > f9("scopira::basekit::narray_o<short,2>");
static scopira::core::register_flow< scopira::basekit::narray_o<int,2> > f10("scopira::basekit::narray_o<int,2>");
static scopira::core::register_flow< scopira::basekit::narray_o<float,2> > f11("scopira::basekit::narray_o<float,2>");
static scopira::core::register_flow< scopira::basekit::narray_o<double,2> > f12("scopira::basekit::narray_o<double,2>");

static scopira::core::register_flow< scopira::basekit::narray_o<bool,3> > f13("scopira::basekit::narray_o<bool,3>");
static scopira::core::register_flow< scopira::basekit::narray_o<char,3> > f14("scopira::basekit::narray_o<char,3>");
static scopira::core::register_flow< scopira::basekit::narray_o<short,3> > f15("scopira::basekit::narray_o<short,3>");
static scopira::core::register_flow< scopira::basekit::narray_o<int,3> > f16("scopira::basekit::narray_o<int,3>");
static scopira::core::register_flow< scopira::basekit::narray_o<float,3> > f17("scopira::basekit::narray_o<float,3>");
static scopira::core::register_flow< scopira::basekit::narray_o<double,3> > f18("scopira::basekit::narray_o<double,3>");

static scopira::core::register_flow< scopira::basekit::narray_o<bool,4> > f19("scopira::basekit::narray_o<bool,4>");
static scopira::core::register_flow< scopira::basekit::narray_o<char,4> > f20("scopira::basekit::narray_o<char,4>");
static scopira::core::register_flow< scopira::basekit::narray_o<short,4> > f21("scopira::basekit::narray_o<short,4>");
static scopira::core::register_flow< scopira::basekit::narray_o<int,4> > f22("scopira::basekit::narray_o<int,4>");
static scopira::core::register_flow< scopira::basekit::narray_o<float,4> > f23("scopira::basekit::narray_o<float,4>");
static scopira::core::register_flow< scopira::basekit::narray_o<double,4> > f24("scopira::basekit::narray_o<double,4>");

