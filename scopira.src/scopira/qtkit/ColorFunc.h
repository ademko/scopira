
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

#ifndef __INCLUDED_SCOPIRA_QTKIT_COLORFUNC_H__
#define __INCLUDED_SCOPIRA_QTKIT_COLORFUNC_H__

#include <map>
#include <vector>

#include <QColor>

namespace scopira
{
  namespace qtkit
  {
    class ColorFunc;
  }
}

/**
 * A Colour transfer function.
 * This takes a collection of colour transfer points
 * and is able to emit a (interpolated) descrete palette suitable for
 * plotting.
 *
 * @author Aleksander Demko
 */ 
class scopira::qtkit::ColorFunc
{
  public:
    // position and colour list
    // position is [0..1], and colors are RGB [0..255]
    typedef std::map<double, QColor> ColorList;

    // the output of interpolatePalette
    typedef std::vector<QColor> ColorPalette;

  public:
    /**
     * Constructor. Builds a default (gray scale) palette
     * with two control points.
     *
     * @author Aleksander Demko
     */
    ColorFunc(void);

    /// gets the list of colours
    ColorList & colors(void) { return dm_colors; }
    /// gets the list of colours
    const ColorList & colors(void) const { return dm_colors; }

    /**
     * Expands the current colour scheme to the given output vector,
     * interpolating the colours as needed.
     * It will fill the given out vector's length. If the length if empty,
     * it will resize the vector to numcol elements.
     *
     * @author Aleksander Demko
     */
    void interpolatePalette(ColorPalette &out, size_t numcol, QColor::Spec spec = QColor::Rgb) const;

    /**
     * Helper function.
     * Populates len elements at base, filling a nice gradient that starts with startcol
     * and ends with endcol.
     *
     * @author Aleksander Demko
     */
    static void fillColourGradient(QColor *base, size_t len, QColor startcol, QColor endcol, QColor::Spec spec = QColor::Rgb);

  public:
    ColorList dm_colors;
};

#endif

