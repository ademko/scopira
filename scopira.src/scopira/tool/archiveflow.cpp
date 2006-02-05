
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
 
#include <scopira/tool/archiveflow.h>

#include <scopira/tool/cacheflow.h>
#include <scopira/tool/fileflow.h>
#include <scopira/tool/binflow.h>

//BBtargets libscopira.so

using namespace scopira::tool;

archiveiflow::archiveiflow(void)
  : isoiflow(true, 0)
{
}

bool archiveiflow::failed(void) const
{
  return dm_file.is_null() || isoiflow::failed();
}

std::string archiveiflow::open(const std::string &filename, const std::string &tagprefix)
{
  count_ptr<fileflow> filer;
  std::string readbuffer;
  byte_t b;
  int ver;

  close();
  readbuffer.reserve(tagprefix.size()*2);

  filer = new fileflow(filename, fileflow::input_c);

  while (!filer->failed() && filer->read_byte(b) && b != 0)
    readbuffer.push_back(b);
  filer->read_byte(b);    // toss away the newline

  // failed because of IO
  if (filer->failed())
    return "";

  // see if the prefix matches
  if (tagprefix.empty())
    ; // no prefix given, they always match
  else if (readbuffer.size()<tagprefix.size() || readbuffer.compare(0, tagprefix.size(), tagprefix) != 0)
    return ""; // prefix doesn't match

  ver = 0;
  filer->read_array(&ver, 1);

  if (ver != 100)
    return "";

  // all is well, setup the stream
  isoiflow::open(new bin64iflow(true, new cacheiflow(true, filer.get())));

  return readbuffer;
}

archiveoflow::archiveoflow(void)
  : isooflow(true, 0)
{
}

bool archiveoflow::failed(void) const
{
  return dm_file.is_null() || isooflow::failed();
}

void archiveoflow::open(const std::string &filename, const std::string &tagkey)
{
  count_ptr<fileflow> filer;

  close();

  filer = new fileflow(filename, fileflow::output_c);

  if (filer->failed())
    return;

  //build the stacked stream
  isooflow::open(new bin64oflow(true, new cacheoflow(true, filer.get())));

  // write string, newline and null
  write_array(tagkey.c_str(), tagkey.size());
  write_byte(0);
  write_byte('\n');
  // write the version of the archive
  write_int(100);
}

