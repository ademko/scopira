
/*
 *  Copyright (c) 2002-2007    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#include <scopira/tool/polyflow.h>

#include <assert.h>

#include <scopira/tool/objflowloader.h>

//BBlibs
//BBtargets libscopira.so

using namespace scopira::tool;
using namespace std;

// object type constats
const static int null_c = 210;
const static int obj_c = 211;
const static int type_c = 212;
const static int ref_c = 213;
const static int oldref_c = 214;

/// constructor + open
polyiflow::polyiflow(bool doref, itflow_i* rz)
  : dm_file(doref,0)
{
  if (rz)
    open(rz);
}
    
/// destructor
polyiflow::~polyiflow()
{
  close();
}
    
bool polyiflow::failed(void) const
{
  assert(dm_file.get());
  return dm_file->failed();
}

bool polyiflow::read_bool(bool& ret)
{
  assert(dm_file.get());
  return dm_file->read_bool(ret);
}

bool polyiflow::read_char(char& ret)
{
  assert(dm_file.get());
  return dm_file->read_char(ret);
}

bool polyiflow::read_short(short& ret)
{
  assert(dm_file.get());
  return dm_file->read_short(ret);
}

bool polyiflow::read_int(int& ret)
{
  assert(dm_file.get());
  return dm_file->read_int(ret);
}

bool polyiflow::read_size_t(size_t& ret)
{
  assert(dm_file.get());
  return dm_file->read_size_t(ret);
}

bool polyiflow::read_int64_t(int64_t& ret)
{
  assert(dm_file.get());
  return dm_file->read_int64_t(ret);
}

/// read an long
bool polyiflow::read_long(long& ret)
{
  assert(dm_file.get());
  return dm_file->read_long(ret);
}

bool polyiflow::read_float(float& ret)
{
  assert(dm_file.get());
  return dm_file->read_float(ret);
}

bool polyiflow::read_double(double& ret)
{
  assert(dm_file.get());
  return dm_file->read_double(ret);
}

/// read a stl string
bool polyiflow::read_string(string& out)
{
  assert(dm_file.get());
  return dm_file->read_string(out);
}

/// read raw block data, returns num read in
size_t polyiflow::read(byte_t* _buf, size_t _maxsize)
{
  assert(dm_file.get());
  return dm_file->read(_buf, _maxsize);
}
    
/// reads a virtual object from the stream
bool polyiflow::read_object(object* &ret)
{
  bool val;
  if ( (val = read_object_impl(ret)) ) {
    if ( !(val = ret->load(*this) ) )
      delete ret;
  }

  return val;;
}

#include <scopira/tool/output.h>

/// reads an object. caller must still called load object!
bool polyiflow::read_object_impl(object* &ret)
{
  int t, cid;

  assert(dm_file.get());

  if (!read_int(t))
    return false;

  // check for null file
  if (t == null_c) {
    ret = 0;  // return the null object
    return true;
  }

  // check if its a new type definition
  if (t == type_c)
  {
    if (!read_int(cid) || cid <= 0) {
      assert(cid > 0);
      return false;
    }
    
    string s;
    if (!read_string(s) || s.empty() ) {
      assert(!s.empty() && "[Unregistered item found in serialization stream]");   // if this fails, then the item wasnt found in the registered item regsitry
      return false;
    }

    // add the new mapping
    dm_typemap[cid] = &(objflowloader::instance()->get_typeinfo(s));
    // read next flag
    if (!read_int(t))
      return false;
  }

  if (t != obj_c)
  {
    assert(t == obj_c);
    return false;
  }
  
  if (!read_int(cid) || cid <= 0)
  {
    assert(cid > 0);
    return false;
  }
  
  // we have the id, get the typeinfo
  if (dm_typemap.count(cid) <= 0)
  {
    assert(dm_typemap.count(cid) > 0);
    return false;
  }

  // return the loaded object
  ret = objflowloader::instance()->load_object(*dm_typemap[cid]);

  return ret!=0;
}
    
void polyiflow::open(itflow_i* fi)
{
  close();
  dm_file.set(fi);
  
  if (dm_file.get())
    dm_typemap.clear();
}
    
/// closes the current file
void polyiflow::close(void)
{
  if (dm_file.get()) {
    dm_file.set(0);
    dm_typemap.clear();
  }
}

/// constructor + open
polyoflow::polyoflow(bool doref, otflow_i* rz)
  : dm_file(doref,0)
{
  if (rz)
    open(rz);
}
    
/// destructor
polyoflow::~polyoflow()
{
  close();
}

bool polyoflow::failed(void) const
{
  assert(dm_file.get());
  return dm_file->failed();
}

void polyoflow::write_bool(bool val)
{
  assert(dm_file.get());
  dm_file->write_bool(val);
}

void polyoflow::write_char(char val)
{
  assert(dm_file.get());
  dm_file->write_char(val);
}

void polyoflow::write_short(short val)
{
  assert(dm_file.get());
  dm_file->write_short(val);
}

void polyoflow::write_int(int val)
{
  assert(dm_file.get());
  dm_file->write_int(val);
}

void polyoflow::write_size_t(size_t val)
{
  assert(dm_file.get());
  dm_file->write_size_t(val);
}

void polyoflow::write_int64_t(int64_t val)
{
  assert(dm_file.get());
  dm_file->write_int64_t(val);
}

/// write an long
void polyoflow::write_long(long val)
{
  assert(dm_file.get());
  dm_file->write_long(val);
}

void polyoflow::write_float(float val)
{
  assert(dm_file.get());
  dm_file->write_float(val);
}

void polyoflow::write_double(double val)
{
  assert(dm_file.get());
  dm_file->write_double(val);
}

/// write a STL string
void polyoflow::write_string(const std::string& val)
{
  assert(dm_file.get());
  dm_file->write_string(val);
}

/// write a raw block of data
size_t polyoflow::write(const byte_t* _buf, size_t _size)
{
  assert(dm_file.get());
  return dm_file->write(_buf, _size);
}
    
/// saves the object to stream
void polyoflow::write_object(const object* o)
{
  const std::string* cname;
  int cid;
  const std::type_info* tinfo;
  
  assert(dm_file.get());
  
  // null check
  if (!o) {
    // null object, write flag and done
    dm_file->write_int(null_c);
    return;
  }
  
  // get the type_info (RTTI!)
  tinfo = &typeid(*o);
  // get the numeric id for this string classname
  assert(objflowloader::instance()->has_typeinfo(*tinfo) && "[trying to serialize an unregistered type]\n");
  cname = &objflowloader::instance()->get_name(*tinfo); // get the string name of this class
  if (dm_typemap.count(*cname) > 0)
    // we already streamed the typename-typenumid. get the cached value
    cid = dm_typemap[*cname];
  else {
    // we have to stream this typename-typenumid to file
    cid = dm_next_typeid++;
    dm_typemap[*cname] = cid;
    // write out the new type mapping
    dm_file->write_int(type_c);
    dm_file->write_int(cid);
    dm_file->write_string(*cname);
  }
  assert(cid>0);
  // write object
  dm_file->write_int(obj_c);
  dm_file->write_int(cid);
  o->save(*this);
}
    
/**
 * opens a flow on the given file stream. the flow will
 * "own" the file stream
 *
 * @param fi the file to operate on
 * @author Aleksander Demko
 */
void polyoflow::open(otflow_i* fi)
{
  close();
  dm_file.set(fi);
  
  if (dm_file.get()) {
    // open her up
    dm_typemap.clear();
    dm_next_typeid = 20;
  }
}
    
/// closes the current file
void polyoflow::close(void)
{
  if (dm_file.get()) {
    // close her down
    dm_typemap.clear();
    dm_file.set(0);
  }
}

/// constructor + open
isoiflow::isoiflow(bool doref, itflow_i* rz)
  : polyiflow(doref, rz)
{
}
    
/// destructor
isoiflow::~isoiflow()
{
}
    
/// reads a virtual object from the stream
bool isoiflow::read_object(object* &ret)
{
  int ty, id;

  if (!read_int(ty))
    return false;

  // check for null
  if (ty == null_c) {
    ret = 0;
    return true;
  }

  // grab the int
  if (!read_int(id))
    return false;
  assert(id > 0);

  if (ty == oldref_c) {
    // found an (looped) old reference
    assert(dm_idmap.count(id) > 0);
    ret = dm_idmap[id];
    return true;
  }

  // new ref then
  assert(ty == ref_c);
  // get it normaly
  if (!read_object_impl(ret))
    return false;
  assert(ret);
  // add it to my map, its vital that this be done before the load
  // as to support recursive strucutres
  dm_idmap[id] = ret;

  // load it up
  if ( !ret->load(*this) ) {
    // failed to serial load
    delete ret;
    return false;
  }

  return true;
}

/// constructor + open
isooflow::isooflow(bool doref, otflow_i* rz)
  : polyoflow(doref, rz)
{
  dm_nextid = 20;
}
    
/// destructor
isooflow::~isooflow()
{
}
    
/// saves the object to stream
void isooflow::write_object(const scopira::tool::object* o)
{
  if (!o) {
    // let poly handle nulls
    write_int(null_c);
    return;
  }

  if (dm_ptrmap.count(o) > 0) {
    int v;

    v = dm_ptrmap[o];
    // write ref for the already writen object
    write_int(oldref_c);
    write_int(v);
  } else {
    write_int(ref_c);
    write_int(dm_nextid);
    dm_ptrmap[o] = dm_nextid;
    dm_nextid++;
    // write object normally now
    polyoflow::write_object(o);
  }
}

#ifdef SCO_DEBUG_polyflow
#include <iostream>
#include <scopira/tool/polyflow.h>
#include <scopira/tool/textflow.h>
#include <scopira/tool/fileflow.h>
int main(void)
{
#ifndef NDEBUG
  objrefcounter _duh;
  _duh.foo(); // does nothing
#endif
  objflowloader loader;
  oobjflow_i *outtie;
  stringobj s1;
  intobj i1;
  doubleobj d1;
  objlist *list;
  objmap *map;
  
  cerr << "polyflow-start\n";
  
  s1.set("data file one");
  i1.set(666);
  d1.set(3.14);
  
  outtie = new polyoflow(new textoflow(new fileflow("out2.stream",  fileflow::output_c)));

  // write map
  map = new objmap(true, true);
  map->set(new intobj(50), new intobj(53));
  map->set(new intobj(30), new intobj(33));
  map->set(new intobj(60), new intobj(63));
  outtie->write_object(map);
  delete map;

  // write list
  list = new objlist(true);
  list->add(new stringobj("first list element"));
  list->add(new intobj(2));
  list->add(new stringobj("hahaha"));
  list->add(new stringobj("last list element"));
  outtie->write_object(list);
  delete list;

  outtie->write_object(&s1);
  outtie->write_object(&i1);
  outtie->write_object(&d1);
  outtie->write_object(&i1);
  outtie->write_object(&s1);
  delete outtie;
  
  polyiflow innie(new textiflow(new fileflow("out2.stream", fileflow::input_c)));
  map = dynamic_cast<objmap*>(innie.read_object());
  assert(map);
  list = dynamic_cast<objlist*>(innie.read_object());
  assert(list);
  stringobj *blah;
  blah = dynamic_cast<stringobj*>(innie.read_object());
  assert(blah);
  cerr << "blah (data file one?): \"" << blah->cstr() << "\"\n";
  delete list;
  delete blah;
  delete map;

  cerr << "polyflow-end\n";
  
  return 0;
}
#endif

#ifdef SCO_DEBUG_isoflow
#include <iostream>
#include <scopira/tool/binflow.h>
#include <scopira/tool/textflow.h>
#include <scopira/tool/fileflow.h>
int main(void)
{
#ifndef NDEBUG
  objrefcounter _duh;
  _duh.foo(); // does nothing
#endif
  objflowloader loader;
  isooflow outtie;
  isoiflow innie;
  objlist l(true);
  stringobj *s, *s2;
  stringobj title("The title track");
  stringobj name("Aleks");

  cerr << "iso test line.\n";

  outtie.open(new textoflow(new fileflow("out3.stream", fileflow::output_c)));
  outtie.write_object(&title);
  outtie.write_object(&title);
  outtie.write_object(&name);
  outtie.write_object(&title);
  outtie.write_object(&name);
  outtie.close();

  cerr << "now reading back...\n";

  innie.open(new textiflow(new fileflow("out3.stream", fileflow::input_c)));
  s = dynamic_cast<stringobj*>(innie.read_object());
  assert(s);
  s2 = dynamic_cast<stringobj*>(innie.read_object());
  assert(s2);
  assert(s == s2);
  l.add(s);
  l.add(s2);
  s2 = dynamic_cast<stringobj*>(innie.read_object());
  assert(s2);
  assert(s != s2);
  l.add(s2);
  s = dynamic_cast<stringobj*>(innie.read_object());
  assert(s);
  assert(s != s2);
  l.add(s);
  s = dynamic_cast<stringobj*>(innie.read_object());
  assert(s);
  assert(s == s2);
  l.add(s);
  innie.close();
  
  cerr << "asserts ok!\n";

  return 0;
}
#endif

