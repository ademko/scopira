
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

#ifndef __INCLUDED_SCOPIRA_QTKIT_ARRAYIO_H__
#define __INCLUDED_SCOPIRA_QTKIT_ARRAYIO_H__

namespace scopira
{
  namespace qtkit
  {
    template <class T>
      bool ReadArrayFromFile(T &out, const QString &filename);
  }
}

/**
 * This reads the contents of filename into the given
 * array out. out must already be presized to the proper
 * size.
 *
 * @return true on success
 * @author Aleksander Demko
 */ 
template <class T>
bool scopira::qtkit::ReadArrayFromFile(T &out, const QString &filename)
{
  QFile f(filename);

  if (!f.open(QIODevice::ReadOnly))
    return false;

  size_t sz = out.size() * sizeof(typename T::data_type);

  if (f.read(reinterpret_cast<char*>(out.c_array()), sz) != sz)
    return false;

  return true;
};

#endif

