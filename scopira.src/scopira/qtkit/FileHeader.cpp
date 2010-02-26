
/*
 *  Copyright (c) 2010    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#include <scopira/qtkit/FileHeader.h>

#include <QFile>
#include <QFileInfo>
#include <QTextStream>

#include <scopira/qtkit/NodePath.h>

using namespace scopira::qtkit;

// filename helpers

static QString justfilename(const QString &filename)
{
  return QFileInfo(filename).fileName();
}

// returns filename, but using srcpath's path
static QString swappath(const QString &filename, const QString &srcpath)
{
  return
    QFileInfo(srcpath).path() + "/" +
    QFileInfo(filename).fileName();
}

static bool isjustfilename(const QString &filename)
{
  return QFileInfo(filename).isRelative();
}

// end of filename helpers

//
//
// FileHeader
//
//

FileHeader::FileHeader(const QString &filename)
  : dm_filename(filename)
{
  QFile device(filename);

  dm_okctor = true;

  if (device.open(QIODevice::ReadOnly) && parent_type::setContent(&device)) {
    if (documentElement().tagName() != "nrc_image_set")
      dm_okctor = false;
  } else {
    appendChild(createElement("nrc_image_set"));
  }
}

bool FileHeader::loadHeader(void)
{
  if (!dm_okctor)
    return false;   // file already exists as something else

  QFile device(dm_filename);
  return device.open(QIODevice::ReadOnly) && parent_type::setContent(&device);
}

bool FileHeader::saveHeader(void) const
{
  if (!dm_okctor)
    return false;   // file already exists as something else

  QFile device(dm_filename);

  if (!device.open(QIODevice::WriteOnly))
    return false;

  QTextStream out(&device);
  parent_type::save(out, 2);

  return true;
}

bool FileHeader::saveHeader(const QString &newfilename)
{
  dm_filename = newfilename;

  return saveHeader();
}

bool FileHeader::queryData(const QString &fieldname, QString &elem_type, std::vector<size_t> &dimen)
{
  try {
    elem_type = NodePath(*this)(NodePath::sanitizeField(fieldname)).getPropAsString("elem_type");

    dimen.clear();
    try {
      int i=0;
      while (true) {
        QString field;
        QTextStream out(&field);

        out << "size_" << i;

        size_t d = NodePath(*this)(NodePath::sanitizeField(fieldname)).getPropAsLong(field);
        dimen.push_back(d);

        ++i;
      }
    }
    catch (const NodePath::error&) {}

    return true;
  }
  catch (const NodePath::error&) {}

  return false;
}

bool FileHeader::createDataImp(const QString &fieldname, short numdimen, size_t *dimen, size_t elemsize,
        const char *id)
{
  QString filename(dm_filename + "." + fieldname);
  QString jname(justfilename(filename));

  NodePath(*this)[NodePath::sanitizeField(fieldname)] = jname;
  NodePath(*this)[NodePath::sanitizeField(fieldname)].setPropVal("elem_type", id);

  for (short d=0; d<numdimen; ++d) {
    QString field, val;
    QTextStream out(&field);

    out << "size_" << d;
    val.setNum(dimen[d]);
    NodePath(*this)(NodePath::sanitizeField(fieldname)).setPropVal(field, val);
  }

  return true;
}

bool FileHeader::saveDataImp(const QString &fieldname, short numdimen, const size_t *dimen,
        size_t elemsize, const char *id, const void *data)
{
  QString filename(dm_filename + "." + fieldname);
  QString jname(justfilename(filename));
  //size_t offset = 0;
  size_t numelem = 1;

  NodePath(*this)[NodePath::sanitizeField(fieldname)] = jname;
  NodePath(*this)[NodePath::sanitizeField(fieldname)].setPropVal("elem_type", id);

  for (short d=0; d<numdimen; ++d) {
    QString field, val;
    QTextStream out(&field);

    out << "size_" << d;
    val.setNum(dimen[d]);
    NodePath(*this)(NodePath::sanitizeField(fieldname)).setPropVal(field, val);

    numelem *= dimen[d];
  }

  NodePath(*this)[NodePath::sanitizeField(fieldname)].setPropVal("elem_type", id);

  // save the data file porttion
  QFile outbin(filename);

  if (!outbin.open(QIODevice::WriteOnly))
    return false;

  outbin.write(static_cast<const char*>(data), elemsize*numelem);

  return true;
}

bool FileHeader::loadDataImp(const QString &fieldname, short numdimen, size_t *dimen, size_t elemsize,
        const char *id, void * &outary)
{
  try {
    QString datafile;
    size_t numelem = 1;

    NodePath::verify(NodePath(*this)(NodePath::sanitizeField(fieldname)).getPropAsString("elem_type") == id);

    datafile = NodePath(*this)(NodePath::sanitizeField(fieldname)).asString();

    for (short d=0; d<numdimen; ++d) {
      QString field;
      QTextStream out(&field);

      out << "size_" << d;
      dimen[d] = NodePath(*this)(NodePath::sanitizeField(fieldname)).getPropAsLong(field);

      numelem *= dimen[d];
    }

    NodePath::verify(numelem>0);   // maybe 0 is ok in the future?

    QFile infile(datafile);

    if (!infile.open(QIODevice::ReadOnly)) {
      infile.setFileName(swappath(datafile, dm_filename));
      if (!infile.open(QIODevice::ReadOnly))
        return false;
    }

    char *c;

    assert(elemsize>0);
    assert(numelem>0);

    c = new char[elemsize*numelem];
    if (infile.read(c, elemsize*numelem) != elemsize*numelem) {
      delete []c;
      return false;
    }
    outary = c;
    return true;
  }
  catch (const NodePath::error&) {
    // fall through to the return false
  }
  return false;
}

bool FileHeader::saveSliceImp(const QString &fieldname, size_t slicez, short numdimen, const size_t *dimen,
        size_t elemsize, const char *id, const void *data)
{
  try {
    size_t slice_size = 1;
    size_t maxz = 0;

    // in the future, cache this information in some local structs?
    QString filename = NodePath(*this)(NodePath::sanitizeField(fieldname)).asString();
    NodePath::verify(NodePath(*this)(NodePath::sanitizeField(fieldname)).getPropAsString("elem_type") == id);

    for (short d=0; d<=numdimen; ++d) {
      QString field;
      size_t val;

      QTextStream fieldout(&field);

      fieldout << "size_" << d;
      val = NodePath(*this)(NodePath::sanitizeField(fieldname)).getPropAsLong(field);

      if (d<numdimen) {
        slice_size *= val;
        if (val != dimen[d])
          return false;
      } else
        maxz = val;
    }

    if (slicez>=maxz)
      return false;   // out of bounds

    // save this slice
    QString fullfilename = filename;

    if (isjustfilename(filename))
      filename = swappath(filename, dm_filename);

    QFile outfile(filename);

    outfile.open(QIODevice::WriteOnly | QIODevice::Append);

    outfile.seek(static_cast<int64_t>(slice_size) * slicez * elemsize);
    outfile.write(static_cast<const char*>(data), elemsize*slice_size);

    return true;
  }
  catch (const NodePath::error&) { }
  return false;
}

bool FileHeader::loadSliceImp(const QString &fieldname, size_t slicez, short numdimen, const size_t *dimen,
        size_t elemsize, const char *id, void *data)
{
  try {
    size_t disk_slice_size = 1;
    size_t mem_slice_size = 1;
    size_t maxz = 0;

    // in the future, cache this information in some local structs?
    QString filename = NodePath(*this)(NodePath::sanitizeField(fieldname)).asString();
    NodePath::verify(NodePath(*this)(NodePath::sanitizeField(fieldname)).getPropAsString("elem_type") == id);

    // numdimen is the dimen of the slice, the data is ofcourse a set of those (dimen+1)
    for (short d=0; d<=numdimen; ++d) {
      QString field;
      size_t val;
      QTextStream fieldout(&field);

      fieldout << "size_" << d;
      val = NodePath(*this)(NodePath::sanitizeField(fieldname)).getPropAsLong(field);

      if (d<numdimen) {
        disk_slice_size *= val;
        mem_slice_size *= dimen[d];
        if (d+1 < numdimen) {
          if (val != dimen[d])    // the memory size has to match for all but the last dimen
            return false;
        } else {
          // but the last dimen in the slice only has to be less than disk size
          if (val < dimen[d])
            return false;
        }
      } else {
        // this is d == numdimen, the z or slice index
        maxz = val;
      }
    }

    if (slicez>=maxz)
      return false;   // out of bounds

    assert(mem_slice_size <= disk_slice_size);

    // load this slice
    QString fullfilename = filename;

    if (isjustfilename(filename))
      filename = swappath(filename, dm_filename);

    QFile infile(filename);

    infile.open(QIODevice::ReadOnly);

    infile.seek(static_cast<int64_t>(disk_slice_size) * slicez * elemsize);

    return infile.read(static_cast<char*>(data), elemsize*mem_slice_size) == elemsize*mem_slice_size;
  }
  catch (const NodePath::error&) { }
  return false;
}

bool FileHeader::loadSliceRowImp(const QString &fieldname, size_t yrow, size_t slicez, short numdimen, const size_t *dimen,
        size_t elemsize, const char *id, void *data)
{
  try {
    size_t slice_size = 0;
    size_t row_slice_size = 1;
    // remeber, this row_slice is one less than the slice which is one less than the volume
    size_t volume_numdimen = numdimen+2;
    size_t max_yrow = 0, max_z = 0;

    // in the future, cache this information in some local structs?
    // TODO yes, definatly optimize this in the future
    QString filename = NodePath(*this)(NodePath::sanitizeField(fieldname)).asString();
    NodePath::verify(NodePath(*this)(NodePath::sanitizeField(fieldname)).getPropAsString("elem_type") == id);

    // verify the dimensions
    for (short d=0; d<volume_numdimen; ++d) {
      QString field;
      size_t val;
      QTextStream fieldout(&field);

      fieldout << "size_" << d;
      val = NodePath(*this)(NodePath::sanitizeField(fieldname)).getPropAsLong(field);

      if (d<numdimen) {
        row_slice_size *= val;
        if (val != dimen[d])
          return false;
      }
      if (d == volume_numdimen-2)
        max_yrow = val;
      if (d == volume_numdimen-1)
        max_z = val;
    }
    slice_size = row_slice_size*max_yrow;

    if (yrow >= max_yrow)
      return false;   // out of bounds

    if (slicez >= max_z)
      return false;   // out of bounds

    QString fullfilename = filename;

    if (isjustfilename(filename))
      filename = swappath(filename, dm_filename);

    QFile infile(filename);

    infile.open(QIODevice::ReadOnly);

    infile.seek(static_cast<int64_t>(slice_size) * slicez * elemsize
        + row_slice_size * yrow * elemsize);

    return infile.read(static_cast<char*>(data), elemsize*row_slice_size) == elemsize*row_slice_size;
  }
  catch (const NodePath::error&) { }
  return false;
}

/* test code for the previous function
  {//test
    scopira::basekit::narray<float,2> MAT;
    scopira::basekit::narray<float,1> ROW;

    MAT.resize(vol->Width(), vol->Height());
    ROW.resize(vol->Width());

    ROW.set_all(-1);

    for (int zz=0; zz<vol->NumSlice(); ++zz) {
std::cerr << "loading zz=" << zz << '\n';
      vol->LoadSlice(zz, MAT);
      for (int yy=0; yy<vol->Height(); ++yy) {
std::cerr << " yy=" << yy << '\n';
        vol->LoadSliceRow(yy, zz, ROW);

        // do the compare
        for (int xx=0; xx<ROW.size(); ++xx)
          assert(ROW[xx] == MAT(xx,yy));
      }
    }
  }
*/

