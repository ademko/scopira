
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

#include <scopira/qtkit/ColorFunc.h>

#include <assert.h>

using namespace scopira::qtkit;

ColorFunc::ColorFunc(void)
{
  dm_colors[0] = Qt::black;
  dm_colors[1] = Qt::white;
}

// d is [0..1], result will be [0..numcol)
static inline size_t rangeDoubleToInt(double d, size_t numcol)
{
  size_t r = static_cast<size_t>(d*(numcol-1));

  // sanity checks
  assert(r>=0 && r<numcol);
  return r;
}

void ColorFunc::interpolatePalette(ColorPalette &out, size_t numcol, QColor::Spec spec) const
{
  if (out.size() != numcol)
    out.resize(numcol);

  ColorList::const_iterator ii, endii, previi;

  previi = ii = dm_colors.begin();
  ++ii;
  endii = dm_colors.end();

  while (ii != endii) {
    assert(previi->first < out.size());
    assert(ii->first < out.size());
    assert(previi->first < ii->first);

    size_t start, len;

    start = rangeDoubleToInt(previi->first, numcol);
    len = rangeDoubleToInt(ii->first, numcol) - rangeDoubleToInt(previi->first, numcol) + 1;

    assert(start>=0);
    assert(start+len <= out.size());
    fillColourGradient(&out[start], len, previi->second, ii->second, spec);

    previi = ii;
    ++ii;
  }
}

void ColorFunc::fillColourGradient(QColor *base, size_t len, QColor startcol, QColor endcol, QColor::Spec spec)
{
  if (len == 0)
    return;

  if (len == 1) {
    base[0] = startcol;
    return;
  }

  double weigth;
  int x;

  for (x=0; x<len; ++x) {
    weigth = static_cast<double>(x) / (len - 1);

    switch (spec) {
      case QColor::Rgb:
        base[x] = QColor::fromRgb(
            startcol.red() * (1-weigth) + endcol.red() * weigth,
            startcol.green() * (1-weigth) + endcol.green() * weigth,
            startcol.blue() * (1-weigth) + endcol.blue() * weigth
            );
        break;
      case QColor::Hsv:
        // add this mode in the future
        assert(false);
      case QColor::Hsl:
        // add this mode in the future
        assert(false);
      default: assert(false);   // unsupported colour space
    }
  }
}

