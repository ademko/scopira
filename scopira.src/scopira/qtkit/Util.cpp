
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

#include <scopira/qtkit/Util.h>

QDebug operator <<(QDebug out, QDebugIndent i)
{
  for (int x=0; x<i.indent(); ++x)
    out << ' ';
  return out;
}

QTextStream & operator << (QTextStream &o, const QPoint &p)
{
  return o << "QPoint(" << p.x() << ',' << p.y() << ')';
}

