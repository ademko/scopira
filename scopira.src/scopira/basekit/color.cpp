
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

#include <scopira/basekit/color.h>

//BBlibs
//BBtargets libscopira.so

using namespace scopira;

int basekit::clamp_color(int c)
{
  if (c > 255)
    return 255;
  if (c < 0)
    return 0;
  return c;
}

int basekit::color_to_gray(int c)
{
  //return ( color_red(c) + color_green(c) + color_blue(c) ) / 3;
  return std::max( std::max( color_red(c),color_green(c) ), color_blue(c) );
}

int basekit::color_scale(int factor, int packedcol)
{
  int a, r, g, b;

  a = color_alpha(packedcol)*factor/1000;
  r = color_red(packedcol)*factor/1000;
  g = color_green(packedcol)*factor/1000;
  b = color_blue(packedcol)*factor/1000;

  return color_argb(clamp_color(a),clamp_color(r),clamp_color(g),clamp_color(b));
}

int basekit::layer_color(int basecolor, int layercolor)
{
  int a = color_alpha(layercolor);

  if (a == 0)
    return layercolor;
  if (a == 255)
    return basecolor;

  // not easy, do some blending
  return color_argb(0,
      (a*color_red(basecolor) + (255-a)*color_red(layercolor)) / 255,
      (a*color_green(basecolor) + (255-a)*color_green(layercolor)) / 255,
      (a*color_blue(basecolor) + (255-a)*color_blue(layercolor)) / 255
    );
}

void basekit::fill_color_pick(nslice<int> target, int offset)
{
  nslice<int>::iterator ii, endii;
  int *p;
  static int loop[] =
    {0xFF0000, 0x0000FF, 0x00FFFF, 0x00FF00,
    //{0xFF7700, 0xFF00FF, 0x00FFFF, 0x00FF00,
    0xFFFF00, 0xFF00FF, 0xFFFF77, 0xFF77FF, 0x77FFFF, -1};
    /*0x0000FF, 0x00FF00,
    0xFFFF77, 0xFF77FF, 0x77FFFF, 0xFF7700, 0x7700FF, 0x00FF77,
    0x77FF00, 0xFF0077, 0x0077FF, 0x770000, 0x000077, 0x007700, -1};*/

  if (target.empty())
    return;

  if ( offset > 8 ) // change depending on size of loop array above
    offset = 0;
  p = loop + offset;
  endii = target.end();
  for (ii=target.begin(); ii!=endii; ++ii) {
    if (*p == -1)
      p = loop;
    *ii = *p;
    p++;
  }
}

void basekit::fill_color_gradient(nslice<int> target, int startpcol, int endpcol)
{
  int x, a, r, g, b;
  double ra, rr, rg, rb;

  if (target.empty())
    return;

  // use 16bit precision for a better gradient
  // use int math instead?

  a = color_alpha(startpcol) << 8;
  r = color_red(startpcol) << 8;
  g = color_green(startpcol) << 8;
  b = color_blue(startpcol) << 8;
  ra = ((color_alpha(endpcol)<<8) - static_cast<double>(a)) / target.size();
  rr = ((color_red(endpcol)<<8) - static_cast<double>(r)) / target.size();
  rg = ((color_green(endpcol)<<8) - static_cast<double>(g)) / target.size();
  rb = ((color_blue(endpcol)<<8) - static_cast<double>(b)) / target.size();

  for (x=0; x<target.size(); ++x)
    target[x] = color_argb(
      static_cast<int>(a+x*ra) >> 8,
      static_cast<int>(r+x*rr) >> 8,
      static_cast<int>(g+x*rg) >> 8,
      static_cast<int>(b+x*rb) >> 8);
}

void basekit::make_gray_palette(const nslice<int> &pal)
{
  fill_color_gradient(pal, black_c, white_c);
}

void basekit::make_red_palette(const nslice<int> &pal)
{
  fill_color_gradient(pal, black_c, red_c);
}

void basekit::make_green_palette(const nslice<int> &pal)
{
  fill_color_gradient(pal, black_c, green_c);
}

void basekit::make_blue_palette(const nslice<int> &pal)
{
  fill_color_gradient(pal, black_c, blue_c);
}

void basekit::make_pick_palette(const nslice<int> &pal)
{
  fill_color_pick(pal);
}

void basekit::make_heat_palette(const nslice<int> &pal)
{
  if (pal.size()<=3) {
    // dunno what to do here
    make_red_palette(pal);
    return;
  }

  int seglen = pal.size()/3;
  int lastseg = seglen + pal.size()%seglen;

  fill_color_gradient(pal.xslice(0, seglen), black_c, red_c);
  fill_color_gradient(pal.xslice(seglen, seglen), red_c, yellow_c);
  fill_color_gradient(pal.xslice(seglen*2, lastseg), yellow_c, white_c);
}

void basekit::make_rainbow_palette(const nslice<int> &pal)
{
  if (pal.size()<=5) {
    // dunno what to do here
    make_red_palette(pal);
    return;
  }

  int seglen = pal.size()/4;
  int lastseg = seglen + pal.size()%seglen;

  fill_color_gradient(pal.xslice(0, seglen), blue_c, cyan_c);
  fill_color_gradient(pal.xslice(seglen, seglen), cyan_c, green_c);
  fill_color_gradient(pal.xslice(seglen*2, seglen), green_c, yellow_c);
  fill_color_gradient(pal.xslice(seglen*3, lastseg), yellow_c, red_c);
}

void basekit::make_bluered_palette(const nslice<int> &pal)
{
  fill_color_gradient(pal, blue_c, red_c);
}

void basekit::make_bone_palette(const nslice<int> &pal)
{
  fill_color_gradient(pal, black_c, 0xc4ddff);
}

void basekit::make_copper_palette(const nslice<int> &pal)
{
  fill_color_gradient(pal, black_c, 0xf5d03e);
}

void basekit::make_spectra_palette(const nslice<int> &pal)
{
  if (pal.size()<=4) {
    // dunno what to do here
    make_red_palette(pal);
    return;
  }
  int seglen = pal.size()/3;
  int lastseg = seglen + pal.size()%seglen;

  fill_color_gradient(pal.xslice(0, seglen), blue_c, green_c);
  fill_color_gradient(pal.xslice(seglen, seglen), green_c, yellow_c);
  fill_color_gradient(pal.xslice(seglen*2, lastseg), yellow_c, red_c);
}


void basekit::make_opus_palette(const nslice<int> &pal)
{
  int smallseg = pal.size()/8;

  if (smallseg == 0) {
    // dunno what to do here
    make_red_palette(pal);
    return;
  }

  fill_color_gradient(pal.xslice(0, smallseg*2), 0x000044, blue_c);
  fill_color_gradient(pal.xslice(smallseg*2, smallseg), blue_c, cyan_c);
  fill_color_gradient(pal.xslice(smallseg*3, smallseg), cyan_c, green_c);
  fill_color_gradient(pal.xslice(smallseg*4, smallseg), green_c, 0xFF7700);
  fill_color_gradient(pal.xslice(smallseg*5, smallseg), 0xFF7700, red_c);
  fill_color_gradient(pal.xslice(smallseg*6, smallseg*2), red_c, purple_c);
}

