
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

#ifndef __INCLUDED_SCOPIRA_MAGICK_FILEIO_H__
#define __INCLUDED_SCOPIRA_MAGICK_FILEIO_H__

#include <string>

#include <scopira/basekit/narray.h>

namespace scopira
{
  /**
   * This namespace contains the Scopira Image Magick extension
   *
   * @author Aleksander Demko
   */ 
  namespace magick
  {
    /**
     * Loads an image file from disk, using Image Magick into the given (packed, colour) narray.
     * Returns true on success.
     *
     * @author Aleksander Demko
     */ 
    bool load_magick_image(const std::string &filename, scopira::basekit::narray<int,2> &out);

    /**
     * Saves an image file from disk, using Image Magick from the given (packed, colour) narray.
     * Returns true on success.
     *
     * @author Aleksander Demko
     */ 
    bool save_magick_image(const std::string &filename, const scopira::basekit::narray<int,2> &rgbi);
  }
}

/**
  \page scopiramagickpage Scopira Image Magick Reference

  This extension provides some utility functions for loading
  and saving artibrary images from Scopira narrays. It utilizes
  the Image Magick library for the actual file I/O.

  The UI kit provides views (and load and a save function) that
  register themselves on scopira::uikit::narray_m<int,2>

  See the namespace scopira::magick.
*/

#endif

