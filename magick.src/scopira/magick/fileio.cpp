
/*
 *  Copyright (c) 2003-2006    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#include <scopira/magick/fileio.h>

#include <scopira/tool/output.h>
#include <scopira/basekit/color.h>

#include <magick/api.h>

//BBtargets libscopiramagick.so
//BBlibs scopira -lMagick

bool scopira::magick::load_magick_image(const std::string &filename, scopira::basekit::narray<int,2> &out)
{
  int x, y;
  ImageInfo *nfo;
  ExceptionInfo exp;
  Image *img;

  GetExceptionInfo(&exp);
  
  nfo = CloneImageInfo(0);
  strcpy(nfo->filename, filename.c_str());

  img = ReadImage(nfo, &exp);
  if (!img) {
    DestroyImageInfo(nfo);
    DestroyExceptionInfo(&exp);
    return false;
  }

  out.resize(img->columns, img->rows);

  // fill in the colours
  for (y=0; y<out.height(); y++)
    for (x=0; x<out.width(); x++) {
      PixelPacket p = GetOnePixel(img, x, y);
#if QuantumDepth == 8
      out(x,y) = scopira::basekit::color_rgb(p.red, p.green, p.blue);
#elif QuantumDepth == 16
      out(x,y) = scopira::basekit::color_rgb(p.red >> 8, p.green >> 8, p.blue >> 8);
#else
#error bad QuantumDepth
#endif
    }

  DestroyImage(img);
  DestroyImageInfo(nfo);
  DestroyExceptionInfo(&exp);

  return true;
}

bool scopira::magick::save_magick_image(const std::string &filename, const scopira::basekit::narray<int,2> &rgbi)
{
  ExceptionInfo exp;
  Image *img, *i2;
  ImageInfo *nfo;
  PixelPacket *p, *curp;
  int mx;
  const int *curi;

  if ((rgbi.width() == 0) || (rgbi.height() == 0)) {
    OUTPUT << "ERROR: image is null: filename = " << filename << '\n';
    //assert(rgbi.width() > 0);
    //assert(rgbi.height() > 0);
    return false;
  }

  GetExceptionInfo(&exp);
  
  nfo = CloneImageInfo(0);
  nfo->size = const_cast<char*>("2x2");      //hmmm... imagemagic might be naughty here
  strcpy(nfo->filename, filename.c_str());

  img = AllocateImage(nfo);
  assert(img);
  i2 = ScaleImage(img, rgbi.width(), rgbi.height(), &exp);
  assert(i2);

  p = SetImagePixels(i2, 0, 0, rgbi.width(), rgbi.height());
  assert(p);
  curp = p;
  curi = rgbi.c_array();
  for (mx = rgbi.width()*rgbi.height(); mx>0; --mx) {
#if QuantumDepth == 8
      curp->red = scopira::basekit::color_red(*curi);
      curp->green = scopira::basekit::color_green(*curi);
      curp->blue = scopira::basekit::color_blue(*curi);
#elif QuantumDepth == 16
      curp->red = scopira::basekit::color_red(*curi) << 8;
      curp->green = scopira::basekit::color_green(*curi) << 8;
      curp->blue = scopira::basekit::color_blue(*curi) << 8;
#else
#error bad QuantumDepth
#endif
    curi++;
    curp++;
  }
  SyncImagePixels(i2);
  //DestroyImagePixels(i2);

  WriteImage(nfo, i2);

  DestroyImage(i2);
  DestroyImage(img);
  //  DestroyImageInfo(nfo);    MDA crashes scopira!
  DestroyExceptionInfo(&exp);

  return true;
}

