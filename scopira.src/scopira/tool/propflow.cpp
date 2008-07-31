
/*
 *  Copyright (c) 2002    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */
 
#include <scopira/tool/propflow.h>

#include <assert.h>
#include <ctype.h>

#include <scopira/tool/util.h>
#include <scopira/tool/prop.h>

#include <scopira/tool/hexflow.h>
#include <scopira/tool/binflow.h>
#include <scopira/tool/polyflow.h>

//BBlibs
//BBtargets libscopira.so

using namespace scopira::tool;

//
// commentiflow
//

commentiflow::commentiflow(bool doref, iflow_i* in)
  : dm_in(doref, 0), dm_atzero(false)
{
  if (in)
    open(in);
}

bool commentiflow::failed(void) const
{
  if (dm_in.get())
    return dm_in->failed();
  else
    return true;
}

size_t commentiflow::read(byte_t* _buf, size_t _maxsize)
{
  size_t x;

  assert(dm_in.get());
  assert(_buf);
  assert(_maxsize>0);

  x = 0;
  while (x<_maxsize && read_byte(_buf[x]) > 0)
    x++;

  return x;
}

size_t commentiflow::read_byte(byte_t &out)
{
  byte_t b;

  if (dm_in->read_byte(b) == 0)
    return 0;

  while (true) {
    if (dm_atzero && b == '#') { // comment found, pass up to \n
      while (b != '\n' && b != '\r')
        if ( dm_in->read_byte(b) == 0)
          return 0;
      // read next, after commented \n
      if ( dm_in->read_byte(b) == 0 )
        return 0;
    } else {
      // we are at non-zero or the zero isnt #, just return it
      // read next
      dm_atzero = b == '\n' || b == '\r';
      out = b;
      return 1;
    }//else
  }//while
}

void commentiflow::open(iflow_i* in)
{
  assert(in);
  dm_in.set(in);

  dm_atzero = true;
}

void commentiflow::close(void)
{
  dm_in = 0;
}

//
// propiflow
//

propiflow::propiflow(bool doref, iflow_i* in)
  : dm_in(doref, 0)
{
  if (in)
    open(in);
}

propiflow::~propiflow(void)
{
  close();
}

bool propiflow::failed(void) const
{
  if (dm_in.get())
    return dm_in->failed();
  else
    return true;
}

size_t propiflow::read(byte_t* _buf, size_t _maxsize)
{
  // TODO
  assert(false);

  return 0;
}

bool propiflow::read_bool(bool& ret)
{
  if (failed())
    return false;

  char c;
  bool success;
  if (success = read_char(c))
    ret = (c == 'T');

  return success;
}

bool propiflow::read_char(char& ret)
{
  dm_in->read_byte(reinterpret_cast<byte_t&>(ret));
  return 1;
}

bool propiflow::read_short(short& ret)
{
  int x;

  if (!read_int(x))
    return false;

  ret = static_cast<short>(x);

  return true;
}

bool propiflow::read_int(int& ret)
{
  if (failed())
    return false;

  byte_t b = non_whitespace();
  std::string s(reinterpret_cast<char*>(&b), 1);
  while ( dm_in->read_byte(b)>0 && !is_whitespace(b))
    s.push_back(b);

  return string_to_int(s, ret);
}

bool propiflow::read_size_t(size_t& ret)
{
  if (failed())
    return false;

  byte_t b = non_whitespace();
  std::string s(reinterpret_cast<char*>(&b), 1);
  while ( dm_in->read_byte(b)>0 && !is_whitespace(b))
    s.push_back(b);

  return string_to_size_t(s, ret);
}

bool propiflow::read_int64_t(int64_t& ret)
{
  if (failed())
    return false;

  byte_t b = non_whitespace();
  std::string s(reinterpret_cast<char*>(&b), 1);
  while ( dm_in->read_byte(b)>0 && !is_whitespace(b))
    s.push_back(b);

  return string_to_int64_t(s, ret);
}

bool propiflow::read_long(long& ret)
{
  if (failed())
    return false;

  byte_t b = non_whitespace();
  std::string s(reinterpret_cast<char*>(&b), 1);
  while ( dm_in->read_byte(b)>0 && !is_whitespace(b))
    s.push_back(b);

  return string_to_long(s, ret);
}

bool propiflow::read_float(float& ret)
{
  double x;

  if (!read_double(x))
    return false;

  ret = static_cast<float>(x);

  return true;
}

bool propiflow::read_double(double& ret)
{
  if (failed())
    return false;

  byte_t b = non_whitespace();
  std::string s(reinterpret_cast<char*>(&b), 1);
  while ( dm_in->read_byte(b)>0 && !is_whitespace(b))
    s.push_back(b);

  return string_to_double(s, ret);
}

bool propiflow::read_string(std::string& out)
{
  bool b;
  return read_keyword(out, b);
}

bool propiflow::read_object(object* &out)
{
  hexiflow hexer(false, dm_in.get());
  biniflow binner(false, &hexer);
  isoiflow inner(false, &binner);
  char cc;

  //the [ has been preread for us
  //read_char(cc);    // [
  //if (cc != '[')
    //return false;

  if (!inner.read_object(out))
    return false;

  read_char(cc);    // ]
  if (cc != ']') {
    // we failed. read up to the enclosing ]
    while (!failed() && cc != ']')
      read_char(cc);
    return false;
  }

  return true;
}

bool propiflow::read_keyword(std::string& ret, bool& keyword)
{
  ret.clear();

  if (failed())
    return false;

  assert(dm_in.get());

  byte_t b = non_whitespace();
  keyword = b != '\"';

  if (b == '[') {
    // read in a [, the start of a binary segment. bail now now now!
    ret = "[";
    return true;
  }

  if (keyword) {
    // read keyword-style string
    ret.assign(reinterpret_cast<char*>(&b), 1);
    while ( dm_in->read_byte(b)>0 && !is_whitespace(b) )
      ret.append(reinterpret_cast<char*>(&b), 1);
  } else {
    // read quoted string
    dm_in->read_byte(b);
    while (b != '\"') {
      if (b == '\\')
        dm_in->read_byte(b);  //read escaped char

      ret.append(reinterpret_cast<char*>(&b), 1);

      // read next char
      dm_in->read_byte(b);
    }//while
  }//else

  return true;
}

void propiflow::open(iflow_i* in)
{
  assert(in);
  close();
  dm_in.set(in);
}

void propiflow::close(void)
{
  dm_in = 0;
}

bool propiflow::read_property(property* rp, int level)
{
  bool is_kw;
  std::string name;
  std::string val;

  name.reserve(256);
  val.reserve(256);

  // read in {
  if (level == 0)
    if (!read_keyword(name, is_kw) || !is_kw || name != "{")
      return false;

  // iterate over all the string and record properties
  if (!read_keyword(name, is_kw))
    return false;

  property *newrp;
  while (!(is_kw && name=="}")) { // loop until closing brace
    // read type
    if (!read_keyword(val, is_kw))
      return false;

    if (is_kw && val == "[") {
      count_ptr<object> o;

      if (!read_object_type(o))
        return false;
      assert(o.get());    // nulls _should_ be valid though!
      rp->add_end(name, new property(o.get()));
    } else if (is_kw && val == "{") {
      // read a record, do recursive read
      newrp = new property_node;
      // save it
      rp->add_end(name, newrp);
      // read it in
      if (!read_property(newrp, level + 1))
        return false;
    } else
      rp->add_end(name, new property(val));

    // read next property thingie
    if (!read_keyword(name, is_kw))
      return false;
  }

  return true;
}

bool propiflow::is_whitespace(byte_t b)
{
  return isspace(b) != 0;
}

propiflow::byte_t propiflow::non_whitespace(void)
{
  byte_t ret;

  ret = ' ';
  while (is_whitespace(ret) && !failed())
    dm_in->read_byte(ret);

  return ret;
}

propoflow::propoflow(bool doref, oflow_i* out)
  : dm_out(doref, 0)
{
  if (out)
    open(out);
}

/// destructor
propoflow::~propoflow(void)
{
  close();
}

bool propoflow::failed(void) const
{
  if (dm_out.get())
    return dm_out->failed();
  else
    return true;
}

size_t propoflow::write(const byte_t* _buf, size_t _size)
{
  // TODO
  assert(false);
  return 0;
}

void propoflow::write_object(const scopira::tool::object* o)
{
  hexoflow hexer(false, dm_out.get());
  bin64oflow binner(false, &hexer);
  isooflow outtie(false, &binner);

  write_char('[');
  outtie.write_object(o);
  write_char(']');
}

void propoflow::write_bool(bool val)
{
  write_char(val ? 'T' : 'F');
}

void propoflow::write_char(char val)
{
  dm_out->write(reinterpret_cast<byte_t*>(&val), 1);
}

void propoflow::write_short(short val)
{
  write_int(val);
}

void propoflow::write_int(int val)
{
  std::string s;

  assert(dm_out.get());
  s = int_to_string(val);

  dm_out->write(reinterpret_cast<const byte_t*>(s.c_str()), s.size());
}

void propoflow::write_size_t(size_t val)
{
  std::string s;

  assert(dm_out.get());
  s = size_t_to_string(val);

  dm_out->write(reinterpret_cast<const byte_t*>(s.c_str()), s.size());
}

void propoflow::write_int64_t(int64_t val)
{
  std::string s;

  assert(dm_out.get());
  s = int64_t_to_string(val) + " ";

  dm_out->write(reinterpret_cast<const byte_t*>(s.c_str()), s.size());
}

void propoflow::write_long(long val)
{
  std::string s;

  assert(dm_out.get());
  s = long_to_string(val);

  dm_out->write(reinterpret_cast<const byte_t*>(s.c_str()), s.size());
}

void propoflow::write_float(float val)
{
  write_double(val);
}

void propoflow::write_double(double val)
{
  std::string s;

  assert(dm_out.get());
  s = double_to_string(val);

  dm_out->write(reinterpret_cast<const byte_t*>(s.c_str()), s.size());
}

void propoflow::write_string(const std::string& val)
{
  const char *s = val.c_str(), *st, *cur;    // this is fast, id wager

  write_char('\"');

  st = s;
  cur = st;
  while ( *cur ) {    // while not end-of-string (eos)
    while ( *cur && (*cur != '\"') && (*cur != '\n') && (*cur != '\\') )
      cur++;
    // we've reached a " or a eos, flush her out
    if (st != cur)
      dm_out->write(reinterpret_cast<const byte_t*>(st), cur - st);
    // if we're at a bad char, escape it and move on
    while ( (*cur == '\"') || (*cur == '\n') || (*cur == '\\') ) {
      write_char('\\');
      write_char(*cur);
      cur++;
    }
    // reset start marker
    st = cur;
  }

  write_char('\"');
}

void propoflow::write_keyword(const std::string& val)
{
  assert(dm_out.get());
  if (val.size() > 0)
    dm_out->write(reinterpret_cast<const byte_t*>(val.c_str()), val.size());
}

void propoflow::write_indent(int n)
{
  int i;
  char c;

  c = ' ';
  assert(dm_out.get());
  assert(n>=0);

  for (i=0; i<n; i++)
    dm_out->write(reinterpret_cast<byte_t*>(&c), 1);
}

/// opens a new link
void propoflow::open(oflow_i* out)
{
  assert(out);
  close();
  dm_out.set(out);
}

/// close the current link
void propoflow::close(void)
{
  dm_out = 0;
}

void propoflow::write_property(property* rp, int indent)
{
  property::iterator jj;
  property::key_iterator ii;

  assert(rp);
  // write guard and header
  //write_indent(indent);
  write_keyword("{\n");

  for (ii = rp->get_key_iterator(); ii.valid(); ++ii) {
    // get a list of props for this key
    for (jj = rp->get(*(*ii)); jj.valid(); ++jj) {
      // write label
      write_indent(indent + 4);
      write_string(*(*ii));
      write_char(' ');
      if ((*jj)->is_node()) {
        write_property((*jj), indent + 4); // let a record write it
      } else {
        if ((*jj)->get_value_as_object())
          write_object((*jj)->get_value_as_object());
        else
          write_string((*jj)->get_value_as_string());
        write_char('\n');
      }
    }
  }

  write_indent(indent);
  write_keyword("}\n");
}

