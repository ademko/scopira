
/*
 *  Copyright (c) 2004    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#include <scopira/basekit/directio.h>

#include <scopira/tool/file.h>
#include <scopira/tool/fileflow.h>
#include <scopira/tool/xml.h>
#include <scopira/tool/output.h>

//BBtargets libscopira.so

using namespace scopira::tool;

namespace {
class direct_file_narray_delete : public scopira::basekit::narray_delete_i, public scopira::tool::object
{
  public:
    count_ptr<fileflow> pm_file;
    filememory pm_mapper;
  public:
    virtual void narray_delete(void *mem, size_t len);
};
}

void direct_file_narray_delete::narray_delete(void *mem, size_t len)
{
  assert(is_alive_object());
  pm_mapper.close();    // make sure this happens before pm_file destruction
  // yes, i ref count myself
  sub_ref();      // (CITE1)
}

bool scopira::basekit::bind_direct_file_impl(scopira::tool::fileflow *f, scopira::tool::byte_t * &baseary, size_t len, narray_delete_i * &func, bool readonly)
{
  count_ptr<direct_file_narray_delete> dafunc;
  assert(f);
  assert(len>0);

  dafunc = new direct_file_narray_delete;
  dafunc->pm_file = f;

  if (!dafunc->pm_mapper.open(*f, len, filememory::read_c | (readonly?0:filememory::write_c)))
    return false;

  // pass if off
  dafunc->add_ref();    // you see, the narray_delete function will undo this  (CITE1)
  func = dafunc.get();
  baseary = reinterpret_cast<byte_t*>(dafunc->pm_mapper.c_array());

  return true;
}

bool scopira::basekit::create_directio_file_impl(const std::string &filename, short numdimen, size_t *dimen, size_t elemsize, const char *id)
{
  fileflow fi, secondfile;
  size_t totsize;
  bool split_mode;

  assert(numdimen>0);
  assert(dimen);

  totsize = dimen[0];
  for (short d=1; d<numdimen; ++d)
    totsize *= dimen[d];

  // totsize is allowed to be 0, if you beleive that
  split_mode = file::has_ext(filename, ".XML");

  if (split_mode)
    fi.open(filename, fileflow::output_c|fileflow::trunc_c);
  else
    fi.open(filename, fileflow::input_c|fileflow::output_c|fileflow::trunc_c);

  if (fi.failed())
    return false;

  if (split_mode) {
    secondfile.open(filename + ".data", fileflow::input_c|fileflow::output_c|fileflow::trunc_c);

    if (secondfile.failed())
      return false;

    secondfile.resize(totsize*elemsize);
  } else {
    // none split mode... all in one combined DIO
    fi.resize(totsize*elemsize + 4096);
  }

  if (!split_mode && !fi.resize(totsize*elemsize + 4096))
    return false;

  // finally, lets compose some xml
  xml_doc thedoc;
  xml_node rootnode, xnode;
  std::string thestring, localname, dummy;

  thestring.reserve(4096);

  thedoc.init("scopira_dio");
  rootnode = thedoc.get_root();

  // required stuff
  xnode = rootnode.add_child("elem_type");
  xnode.add_content_child(id);

  xnode = rootnode.add_child("num_dimen");
  xnode.add_content_child(int_to_string(numdimen));

  for (short d=0; d<numdimen; ++d) {
    xnode = rootnode.add_child("size_" + int_to_string(d));
    xnode.add_content_child(size_t_to_string(dimen[d]));
  }

  file::split_path(filename + ".data", dummy, localname);

  if (split_mode) {
    xnode = rootnode.add_child("data_file");
    xnode.add_content_child(localname);
    xnode.add_attrib("offset", "0");
  } else {
    xnode = rootnode.add_child("data_file");
    //xnode.add_content_child(localname); // no filename for none split files
    xnode.add_attrib("offset", "4096");
  }

  // optional stuff
  xnode = rootnode.add_child("elem_size");
  xnode.add_content_child(long_to_string(elemsize));
  xnode.add_attrib("optional", "1");

  xnode = rootnode.add_child("saved_as_filename");
  xnode.add_content_child(filename);
  xnode.add_attrib("optional", "1");

  // copy it out to the mapped file
  thedoc.save_string(thestring);

  assert(thestring.size() < 4096);

  // pad the string with spaces, if need be
  if (!split_mode) {
    while (thestring.size() < (4096-1))
      thestring.push_back(' ');
    thestring.push_back('\0');
  }

  fi << thestring;

  return true;
}

bool scopira::basekit::save_directio_file_impl(const std::string &filename, short numdimen, const size_t *dimen, size_t elemsize, const char *id, const void *data)
{
  fileflow fi, secondfile;
  size_t totsize;
  bool split_mode;

  assert(numdimen>0);
  assert(dimen);

  totsize = dimen[0];
  for (short d=1; d<numdimen; ++d)
    totsize *= dimen[d];

  // totsize is allowed to be 0, if you beleive that
  split_mode = file::has_ext(filename, ".XML");

  if (split_mode)
    fi.open(filename, fileflow::output_c|fileflow::trunc_c);
  else
    fi.open(filename, fileflow::input_c|fileflow::output_c|fileflow::trunc_c);

  if (fi.failed())
    return false;

  if (split_mode) {
    secondfile.open(filename + ".data", fileflow::input_c|fileflow::output_c|fileflow::trunc_c);

    if (secondfile.failed())
      return false;
  }

  // finally, lets compose some xml
  xml_doc thedoc;
  xml_node rootnode, xnode;
  std::string thestring, localname, dummy;

  thestring.reserve(4096);

  thedoc.init("scopira_dio");
  rootnode = thedoc.get_root();

  // required stuff
  xnode = rootnode.add_child("elem_type");
  xnode.add_content_child(id);

  xnode = rootnode.add_child("num_dimen");
  xnode.add_content_child(int_to_string(numdimen));

  for (short d=0; d<numdimen; ++d) {
    xnode = rootnode.add_child("size_" + int_to_string(d));
    xnode.add_content_child(size_t_to_string(dimen[d]));
  }

  file::split_path(filename + ".data", dummy, localname);

  if (split_mode) {
    xnode = rootnode.add_child("data_file");
    xnode.add_content_child(localname);
    xnode.add_attrib("offset", "0");
  } else {
    xnode = rootnode.add_child("data_file");
    //xnode.add_content_child(localname); // no filename for none split files
    xnode.add_attrib("offset", "4096");
  }

  // optional stuff
  xnode = rootnode.add_child("elem_size");
  xnode.add_content_child(long_to_string(elemsize));
  xnode.add_attrib("optional", "1");

  xnode = rootnode.add_child("saved_as_filename");
  xnode.add_content_child(filename);
  xnode.add_attrib("optional", "1");

  // copy it out to the mapped file
  thedoc.save_string(thestring);

  assert(thestring.size() < 4096);

  // pad the string with spaces, if need be
  if (!split_mode) {
    while (thestring.size() < (4096-1))
      thestring.push_back(' ');
    thestring.push_back('\0');
  }

  fi << thestring;

  if (split_mode)
    secondfile.write_void(data, totsize*elemsize);
  else
    fi.write_void(data, totsize*elemsize);

  return true;
}

bool scopira::basekit::load_directio_file_impl(const std::string &filename, short numdimen, size_t *dimen, size_t elemsize, const char *id, void * &outary)
{
  count_ptr<fileflow> f;
  std::string header, fullfinalfilename;
  byte_t c;
  fileflow::offset_t totsize, rawdatasize, actsize;
  size_t skipbo;

  fullfinalfilename = filename;

  f = new fileflow;

  f->open(fullfinalfilename, fileflow::input_c);
  if (f->failed())
    return false;

  // read the header
  header.reserve(4096*2);
  while ( f->read_byte(c) > 0 && c != 0) {
    assert(c != 0);
    header.push_back(c);

    if (header.size() > 100000) {                     // this limit can be increased... it's purely informative
#ifndef NDEBUG
      OUTPUT << "Aborted read of XML header, getting too large: " << fullfinalfilename << '\n';
#endif
      return false;
    }
  }

  // parse the header
  xml_doc doc;
  xml_node rootnode, xnode, elemtypenode, numdimennode, datafile;
  //OUTPUT << "((" << header << "))" << '\n';

  if (!doc.load_string(header)) {
#ifndef NDEBUG
    OUTPUT << "Failed to parse XML header: " << fullfinalfilename << '\n';
#endif
    return false;
  }

  rootnode = doc.get_root();
  assert(!rootnode.is_null());

  // extract some needed fields
  elemtypenode = rootnode.get_first_child_name("elem_type");
  if (elemtypenode.is_null() || ::strcmp(elemtypenode.get_text_c_content(), id) != 0) {
#ifndef NDEBUG
    OUTPUT << "Bad type stored in XML header: \"" << elemtypenode.get_text_c_content() << "\" want \"" << id << "\"\n";
#endif
    return false;
  }

  numdimennode = rootnode.get_first_child_name("num_dimen");
  if (numdimennode.is_null() || string_to_int(numdimennode.get_text_c_content()) != numdimen) {
#ifndef NDEBUG
    OUTPUT << "Bad number of dimensions in XML header: " << numdimennode.get_text_c_content() << " (wanted " << numdimen << ")\n";
#endif
    return false;
  }

  totsize = 1;
  for (short d=0; d<numdimen; ++d) {
    xnode = rootnode.get_first_child_name("size_" + int_to_string(d));
    if (xnode.is_null() || !string_to_size_t(xnode.get_text_c_content(), dimen[d])) {
#ifndef NDEBUG
      OUTPUT << "Failed to parse a dimension size from the XML header: size_" << d << '\n';
#endif
      return false;
    }
    totsize *= dimen[d];
  }

  // by default, assume single DIO file
  rawdatasize = totsize*elemsize;
  skipbo = header.size() + 1;    // the offset of the data within the file

  datafile = rootnode.get_first_child_name("data_file");
  if (datafile.is_valid() && datafile.get_text_c_content()[0]) {   // make sure the string is none empty
    std::string originalpath, dummy, secondfilename;
    // use alternate data file
    // we will switch to a twinned read now
    secondfilename = datafile.get_text_c_content();

    if (!secondfilename.empty()) {
      file::split_path(filename, originalpath, dummy);

      f->close();
      fullfinalfilename = secondfilename;
//OUTPUT << "TRY " << fullfinalfilename << '\n';
      if (file(fullfinalfilename).exists())
        f->open(fullfinalfilename, fileflow::existing_c | fileflow::input_c);

      if (f->failed()) {
        fullfinalfilename = originalpath + secondfilename;
//OUTPUT << "TRY " << fullfinalfilename << '\n';
        if (file(fullfinalfilename).exists())
          f->open(fullfinalfilename, fileflow::existing_c | fileflow::input_c);
      }

      if (f->failed()) {
#ifndef NDEBUG
        OUTPUT << "Failed to open secondary data file, tried " << secondfilename
          << " and " << fullfinalfilename << '\n';
#endif
        return false;
      }
    }

    if (!string_to_size_t(datafile.get_attrib_c_text("offset"), skipbo))
      skipbo = 0;
  }
  if ( (actsize = file(fullfinalfilename).size()) < skipbo+rawdatasize) {
#ifndef NDEBUG
    OUTPUT << "File is too short (minimum " << skipbo+rawdatasize
      << " actual is " << actsize << ")\n"
      << "File tried: " << fullfinalfilename << '\n';
#endif
    return false;
  }
OUTPUT << "Opening: " << fullfinalfilename << " (skipbo=" << skipbo << ",rawdatasize="
  << rawdatasize << ")\n";

  // ok, set up the function
  char *outary_cver = new char[rawdatasize];
  outary = outary_cver;
  f->seek(skipbo);
  if (f->read_void(outary, rawdatasize) != rawdatasize) {
    delete []outary_cver;
    return false;
  }

  return true;
}

bool scopira::basekit::bind_directio_file_impl(const std::string &filename, short numdimen,
    size_t *dimen, size_t elemsize, const char *id, bool readonly, void * &outary, narray_delete_i * &outfunc)
{
  count_ptr<fileflow> f;
  std::string header, fullfinalfilename;
  byte_t c;
  fileflow::offset_t totsize, rawdatasize, actsize;
  size_t skipbo;

  fullfinalfilename = filename;

  f = new fileflow;

  f->open(fullfinalfilename, fileflow::input_c | (readonly?0:fileflow::output_c));
  if (f->failed())
    return false;

  // read the header
  header.reserve(4096*2);
  while ( f->read_byte(c) > 0 && c != 0) {
    assert(c != 0);
    header.push_back(c);

    if (header.size() > 100000) {                     // this limit can be increased... it's purely informative
#ifndef NDEBUG
      OUTPUT << "Aborted read of XML header, getting too large: " << fullfinalfilename << '\n';
#endif
      return false;
    }
  }

  // parse the header
  xml_doc doc;
  xml_node rootnode, xnode, elemtypenode, numdimennode, datafile;
  //OUTPUT << "((" << header << "))" << '\n';

  if (!doc.load_string(header)) {
#ifndef NDEBUG
    OUTPUT << "Failed to parse XML header: " << fullfinalfilename << '\n';
#endif
    return false;
  }

  rootnode = doc.get_root();
  assert(!rootnode.is_null());

  // extract some needed fields
  elemtypenode = rootnode.get_first_child_name("elem_type");
  if (elemtypenode.is_null() || ::strcmp(elemtypenode.get_text_c_content(), id) != 0) {
#ifndef NDEBUG
    OUTPUT << "Bad type stored in XML header: \"" << elemtypenode.get_text_c_content() << "\" want \"" << id << "\"\n";
#endif
    return false;
  }

  numdimennode = rootnode.get_first_child_name("num_dimen");
  if (numdimennode.is_null() || string_to_int(numdimennode.get_text_c_content()) != numdimen) {
#ifndef NDEBUG
    OUTPUT << "Bad number of dimensions in XML header: " << numdimennode.get_text_c_content() << " (wanted " << numdimen << ")\n";
#endif
    return false;
  }

  totsize = 1;
  for (short d=0; d<numdimen; ++d) {
    xnode = rootnode.get_first_child_name("size_" + int_to_string(d));
    if (xnode.is_null() || !string_to_size_t(xnode.get_text_c_content(), dimen[d])) {
#ifndef NDEBUG
      OUTPUT << "Failed to parse a dimension size from the XML header: size_" << d << '\n';
#endif
      return false;
    }
    totsize *= dimen[d];
  }

  // by default, assume single DIO file
  rawdatasize = totsize*elemsize;
  skipbo = header.size() + 1;    // the offset of the data within the file

  datafile = rootnode.get_first_child_name("data_file");
  if (datafile.is_valid() && datafile.get_text_c_content()[0]) {   // make sure the string is none empty
    std::string originalpath, dummy, secondfilename;
    // use alternate data file
    // we will switch to a twinned read now
    secondfilename = datafile.get_text_c_content();

    if (!secondfilename.empty()) {
      file::split_path(filename, originalpath, dummy);

      f->close();
      fullfinalfilename = secondfilename;
//OUTPUT << "TRY " << fullfinalfilename << '\n';
      if (file(fullfinalfilename).exists())
        f->open(fullfinalfilename, fileflow::existing_c | fileflow::input_c | (readonly?0:fileflow::output_c));

      if (f->failed()) {
        fullfinalfilename = originalpath + secondfilename;
//OUTPUT << "TRY " << fullfinalfilename << '\n';
        if (file(fullfinalfilename).exists())
          f->open(fullfinalfilename, fileflow::existing_c | fileflow::input_c | (readonly?0:fileflow::output_c));
      }

      if (f->failed()) {
#ifndef NDEBUG
        OUTPUT << "Failed to open secondary data file, tried " << secondfilename
          << " and " << fullfinalfilename << '\n';
#endif
        return false;
      }
    }

    if (!string_to_size_t(datafile.get_attrib_c_text("offset"), skipbo))
      skipbo = 0;
  }
  if ( (actsize = file(fullfinalfilename).size()) < skipbo+rawdatasize) {
#ifndef NDEBUG
    OUTPUT << "File is too short (minimum " << skipbo+rawdatasize
      << " actual is " << actsize << ")\n"
      << "File tried: " << fullfinalfilename << '\n';
#endif
    return false;
  }
OUTPUT << "Opening: " << fullfinalfilename << " (skipbo=" << skipbo << ",rawdatasize="
  << rawdatasize << "," << (readonly?"RO":"RW") << ")\n";

  // ok, set up the function
  count_ptr<direct_file_narray_delete> dafunc;
  dafunc = new direct_file_narray_delete;
  dafunc->pm_file = f;

  // header + null + datasize
  if (!dafunc->pm_mapper.open(*f, skipbo+rawdatasize, filememory::read_c | (readonly?0:filememory::write_c)))
    return false;

  // pass if off
  dafunc->add_ref();    // you see, the narray_delete function will undo this  (CITE1)
  outfunc = dafunc.get();
  outary = reinterpret_cast<byte_t*>(dafunc->pm_mapper.c_array()) + skipbo;

  return true;
}

void scopira::basekit::sync_directio_file(narray_delete_i *d)
{
  assert(d);

  direct_file_narray_delete *dd = dynamic_cast<direct_file_narray_delete*>(d);

  assert(dd);
  assert(dd->is_alive_object());

  dd->pm_mapper.sync();
}

bool scopira::basekit::query_directio_file(const std::string &filename,
        std::string &elementtype, std::vector<size_t> &dimen)
{
  count_ptr<fileflow> f;
  std::string header;
  byte_t c;

  f = new fileflow;

  f->open(filename, fileflow::input_c);
  if (f->failed())
    return false;

  // read the header
  header.reserve(4096*2);
  while ( f->read_byte(c) > 0 && c != 0) {
    assert(c != 0);
    header.push_back(c);

    if (header.size() > 100000) {                     // this limit can be increased... it's purely informative
#ifndef NDEBUG
      OUTPUT << "Aborted read of XML header, getting too large: " << filename << '\n';
#endif
      return false;
    }
  }

  // parse the header
  xml_doc doc;
  xml_node rootnode;
  std::string val;
  int nval;

  if (!doc.load_string(header)) {
#ifndef NDEBUG
    OUTPUT << "Failed to parse XML header: " << filename << '\n';
#endif
    return false;
  }

  val.reserve(128);

  rootnode = doc.get_root();
  assert(!rootnode.is_null());

  val = rootnode.get_leaf_text_content("elem_type");
  if (val.empty())
    return false;
  elementtype = val;

  if (!string_to_int(rootnode.get_leaf_text_content("num_dimen"), nval) || nval<=0)
    return false;
  dimen.resize(nval);

  for (nval=0; nval<dimen.size(); ++nval)
    if (!string_to_size_t(rootnode.get_leaf_text_content("size_" + int_to_string(nval)), dimen[nval]))
      return false;

  return true;
}

