
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

#ifndef __INCLUDED__SCOPIRA_BASEKIT_COLOR_H__
#define __INCLUDED__SCOPIRA_BASEKIT_COLOR_H__

#include <scopira/basekit/narray.h>
#include <scopira/tool/export.h>

namespace scopira
{
  namespace basekit
  {
    /// constant colours
    enum {
      white_c = 0xFFFFFF,
      red_c = 0xFF0000,
      green_c = 0xFF00,
      blue_c = 0xFF,
      yellow_c = 0xFFFF00,
      cyan_c = 0xFFFF,
      purple_c = 0xFF00FF,
      black_c = 0,
    };
    /// converts a RGB triplet to a packed color
    inline int color_rgb(int r, int g, int b)
      { return r<<16 | g<<8 | b; }
    /// converts a ARGB triplet to packed color
    inline int color_argb(int a, int r, int g, int b)
      { return a<<24 | r<<16 | g<<8 | b; }
    /// expand a single 0..255 to a packed color
    inline int color_gray(int v)
      { return v<<16 | v<<8 | v; }
    /// extracts alpha
    inline int color_alpha(int packedcol) { return (packedcol&0xFF0000)>>24; }
    /// extracts red
    inline int color_red(int packedcol) { return (packedcol&0xFF0000)>>16; }
    /// extracts gree
    inline int color_green(int packedcol) { return (packedcol&0x00FF00)>>8; }
    /// extracts blue
    inline int color_blue(int packedcol) { return (packedcol&0x0000FF); }

    /// sets
    inline int set_alpha(int packedcol, int a) { return (packedcol&0x00FFFFFF) | (a<<24); }
    /// sets
    inline int set_red(int packedcol, int r) { return (packedcol&0xFF00FFFF) | (r<<16); }
    /// sets
    inline int set_green(int packedcol, int g) { return (packedcol&0xFFFF00FF) | (g<<8); }
    /// sets
    inline int set_blue(int packedcol, int b) { return (packedcol&0xFFFFFF00) | b; }

    /// clamps this number between 0 and 255
    SCOPIRA_EXPORT int clamp_color(int c);
    /// convert a colour to doulbe (0..1)... basically, c/255
    inline double color_to_double(int c) {
      return static_cast<double>(c)/255;
    }
    /// convert colour into a 0..255 (mean)
    SCOPIRA_EXPORT int color_to_gray(int c);

    /// sclaes a color. factor is 1000 for no change (alpha aware)
    SCOPIRA_EXPORT int color_scale(int factor, int packedcol);

    /// layers a colour (alpha aware)
    SCOPIRA_EXPORT int layer_color(int basecolor, int layercolor);

    /// set colors, suitable for picking
    SCOPIRA_EXPORT void fill_color_pick(nslice<int> target, int offset = 0);
    /// set a gradient (alpha aware)
    SCOPIRA_EXPORT void fill_color_gradient(nslice<int> target, int startpcol, int endpcol);

    /// makes a palette
    SCOPIRA_EXPORT void make_gray_palette(const nslice<int> &pal);
    /// makes a palette
    SCOPIRA_EXPORT void make_red_palette(const nslice<int> &pal);
    /// makes a palette
    SCOPIRA_EXPORT void make_green_palette(const nslice<int> &pal);
    /// makes a palette
    SCOPIRA_EXPORT void make_blue_palette(const nslice<int> &pal);
    /// makes a palette
    SCOPIRA_EXPORT void make_pick_palette(const nslice<int> &pal);
    /// makes a palette
    SCOPIRA_EXPORT void make_heat_palette(const nslice<int> &pal);
    /// makes a palette
    SCOPIRA_EXPORT void make_rainbow_palette(const nslice<int> &pal);
    /// makes a palette
    SCOPIRA_EXPORT void make_bluered_palette(const nslice<int> &pal);
    /// makes a palette
    SCOPIRA_EXPORT void make_bone_palette(const nslice<int> &pal);
    /// makes a palette
    SCOPIRA_EXPORT void make_copper_palette(const nslice<int> &pal);
    /// makes a palette
    SCOPIRA_EXPORT void make_spectra_palette(const nslice<int> &pal);
    /// makes a palette
    SCOPIRA_EXPORT void make_opus_palette(const nslice<int> &pal);   // yes, this is in basekit, even though its opusish
  }
}

#endif

