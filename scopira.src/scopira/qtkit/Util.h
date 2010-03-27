
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

#ifndef __INCLUDED_SCOPIRA_QTKIT_UTIL_H__
#define __INCLUDED_SCOPIRA_QTKIT_UTIL_H__

#include <string>

#include <QTextStream>
#include <QPoint>
#include <QDebug>

//polute the top namespace deliberatly (and with a Q-prefix)
class QDebugIndent
{
  public:
    QDebugIndent(int indent) : dm_indent(indent) { }

    int indent(void) const { return dm_indent; }

  private:
    int dm_indent;
};

QDebug operator <<(QDebug out, const QDebugIndent i);

QTextStream & operator << (QTextStream &o, const QPoint &p);
QTextStream & operator << (QTextStream &o, const std::string &s);

#endif

