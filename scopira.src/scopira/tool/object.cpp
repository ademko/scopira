
/*
 *  Copyright (c) 2001-2004    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#include <scopira/tool/object.h>

#include <assert.h>

#include <scopira/tool/flow.h>
#include <scopira/tool/platform.h>

#ifndef PLATFORM_win32
#include <scopira/tool/thread.h>
#endif


//BBlibs scopiraflags
//BBtargets libscopira.so

using namespace scopira::tool;

object::object(void)
{
#ifndef NDEBUG
  dm_use_debug_ref_counter = objrefcounter::has_counter();
  if (dm_use_debug_ref_counter) {
    objrefcounter::add_ref();
    objrefcounter::add_real();
  }
  dm_object_magic = -236944;
#endif
  dm_refcount = 0;
}

object::object(bool neverusecounter)
{
#ifndef NDEBUG
  dm_use_debug_ref_counter = false;
  /*if (dm_use_debug_ref_counter) {
    objrefcounter::add_ref();
    objrefcounter::add_real();
  }*/
  dm_object_magic = -236944;
#endif
  dm_refcount = 0;
}

object::object(const object &o)
{
  assert(false && "[attempted to use the default object() copy ctor]");
}

object::~object()
{
#ifndef NDEBUG
  assert(is_alive_object());
  if (dm_use_debug_ref_counter) {
    if (dm_refcount>0)
      objrefcounter::bad_del();
    objrefcounter::sub_ref();
    objrefcounter::sub_real();
  }
  dm_object_magic = 5529022;
#endif
  // does nothing
}

int object::add_ref(void) const
{
  locker lk(dm_refcount_mutex);

  assert(is_alive_object());
  if (dm_refcount >= 0) {
 #ifndef NDEBUG
    if (dm_use_debug_ref_counter)
      objrefcounter::add_ref();
 #endif
     ++dm_refcount;
  }

  return dm_refcount;
}

bool object::sub_ref(void) const
{
  bool dodel = false;
  {
    locker lk(dm_refcount_mutex);
    // this is reimplemented in thr_object too, so follow any changes there too!
    assert(is_alive_object());
    if (dm_refcount >= 0) {
#ifndef NDEBUG
      if (dm_use_debug_ref_counter) {
        //if (dm_refcount > 0)      // this is debatable in ref count theory
          objrefcounter::sub_ref();
      }
#endif
      dm_refcount--;

      if (dm_refcount <= 0)
        dodel = true;
    }//if
  }//lock
  if (dodel)
    delete this; // wowza!
  return dodel;
}

void object::auto_ref(void) const
{
  locker lk(dm_refcount_mutex);

  assert(is_alive_object());
  dm_refcount = -100;
}

int object::current_ref(void) const
{
  // no lock, this might not be so smart
  return dm_refcount;
}

oflow_i & object::print(oflow_i &o) const
{
  assert(is_alive_object());
  return o << "(object)";
}

bool object::load(iobjflow_i &in)
{
  assert(is_alive_object());
  // this method should never be called. if it is called,
  // it means that the given object is not serializable
  // and was erronously (attempted) to be serialzied
  assert(false);
  return false;
}


void object::save(oobjflow_i &out) const
{
  assert(is_alive_object());
  // this method should never be called. if it is called,
  // it means that the given object is not serializable
  // and was erronously (attempted) to be serialzied
  assert(false);
}


oflow_i & operator << (oflow_i &o, const object &vl)
{
  return vl.print(o);
}

oflow_i & operator << (oflow_i &o, const object *vl)
{
  if (vl)
    vl->print(o);
    
  return o;
}

//
// *** DEBUG CODE ***
//

#ifndef NDEBUG
#include <stdio.h>       // cant use my streams here cuz it screws up the mid-counts

using namespace std;

int scopira::tool::objrefcounter::dm_ref;
int scopira::tool::objrefcounter::dm_peak;
int scopira::tool::objrefcounter::dm_tot;
int scopira::tool::objrefcounter::dm_baddel;
int scopira::tool::objrefcounter::dm_real;

//
// mutex for objrefcounter
//
static mutex *objrefcounter_mutex;

objrefcounter::objrefcounter(void)
{
  dm_ref = 0;
  dm_peak = 0;
  dm_tot = 0;
  dm_baddel = 0;
  dm_real = 0;
  assert(!objrefcounter_mutex);
  objrefcounter_mutex = new mutex;
  //fprintf(stderr, "OBJREFCOUNTER: START\n");
}

objrefcounter::~objrefcounter()
{
  output();

  if (dm_ref != 0)
    fprintf(stderr, "OBJREFCOUNTER: ***FATAL REFERENCE COUNT ERROR*** : %d\n", dm_ref);
  if (dm_baddel != 0)
    fprintf(stderr, "OBJREFCOUNTER: ***BAD DEL COUNT*** : %d\n", dm_baddel);
  if (dm_real != 0)
    fprintf(stderr, "OBJREFCOUNTER: ***FATAL REAL COUNT ERROR*** : %d\n", dm_real);
  fflush(stderr);

  delete objrefcounter_mutex;
  objrefcounter_mutex = 0;
}

bool objrefcounter::has_counter(void)
{
  return objrefcounter_mutex != 0;
}

void objrefcounter::add_ref(void)
{
  if (!objrefcounter_mutex)
    return;
  locker lk(*objrefcounter_mutex);

  dm_ref++;
  if (dm_ref > dm_peak)
    dm_peak = dm_ref;
  dm_tot++;
}

void objrefcounter::sub_ref(void)
{
  if (!objrefcounter_mutex)
    return;
  locker lk(*objrefcounter_mutex);

  dm_ref--;
}

void objrefcounter::bad_del(void)
{
  if (!objrefcounter_mutex)
    return;
  locker lk(*objrefcounter_mutex);

  dm_baddel++;
}

void objrefcounter::output(void)
{
  if (!objrefcounter_mutex)
    return;
  locker lk(*objrefcounter_mutex);

  fprintf(stderr, "OBJREFCOUNTER: current:%d peak:%d total:%d baddel:%d real:%d\n",
    dm_ref, dm_peak, dm_tot, dm_baddel, dm_real);
}

void objrefcounter::add_real(void)
{
  if (!objrefcounter_mutex)
    return;
  locker lk(*objrefcounter_mutex);

  dm_real++;
}

void objrefcounter::sub_real(void)
{
  if (!objrefcounter_mutex)
    return;
  locker lk(*objrefcounter_mutex);

  dm_real--;
}

#endif // NDEBUG

