
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

#ifndef __INCLUDED_SCOPIRA_QTKIT_LOCALPAINTER_H__
#define __INCLUDED_SCOPIRA_QTKIT_LOCALPAINTER_H__

#include <QPainter>

namespace scopira
{
  namespace qtkit
  {
    class LocalPainter;
  }
}

/**
 * This is an RAII object that automatically
 * saves and restores the state of the given QPainter
 * during its construction and destruction.
 *
 * @author Aleksander Demko
 */ 
class scopira::qtkit::LocalPainter
{
  public:
    LocalPainter(QPainter &painter) : dm_painter(painter) { dm_painter.save(); }
    ~LocalPainter() { dm_painter.restore(); }

  private:
    QPainter &dm_painter;
};

#endif

