
/*
 *  Copyright (c) 2004-2008    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#ifndef __INCLUDED_SCOPIRA_DIRECTIO_WX_DIRECTIO_H__
#define __INCLUDED_SCOPIRA_DIRECTIO_WX_DIRECTIO_H__

/*
 * This version of directio loading uses the wxWidgets xml and files libraries.
 *
 * @author Aleksander Demko
 */ 

#include <vector>

#include <wx/string.h>

#include <scopira/tool/platform.h>
#include <scopira/tool/export.h>
#include <scopira/tool/fileflow.h>
#include <scopira/tool/array.h>
#include <scopira/basekit/narray.h>

namespace scopira
{
  /**
   * This namespace contains Scopira classes that build and utilize
   * the wxWidgets (http://www.wxwidgets.org/) library.
   *
   * @author Aleksander Demko
   */ 
  namespace wx
  {
    bool save_directio_file_impl(const wxString &filename, short numdimen, const size_t *dimen, size_t elemsize, const char *id, const void *data);
    bool load_directio_file_impl(const wxString &filename, short numdimen, size_t *dimen, size_t elemsize, const char *id, void * &outary);

    // traits stuff
    template <class T> class directio_traints { public: typedef T data_type; };
    template <> class directio_traints<float> { public: static const char * get_name(void) { return "float32"; } };
    template <> class directio_traints<double> { public: static const char * get_name(void) { return "float64"; } };
    template <> class directio_traints<char> { public: static const char * get_name(void) { return "int8"; } };
    template <> class directio_traints<short> { public: static const char * get_name(void) { return "int16"; } };
    template <> class directio_traints<int> { public: static const char * get_name(void) { return "int32"; } };

    /**
     * Reads an array from disk in DIO format.
     * This version only supports the split format. That is, the file itself is an XML
     * file containing the header information of the file, and a pointer to the actually
     * data/payload file, which is seperate.
     *
     * @see @ref scopirabasekitdio
     *
     * @param filename the filename
     * @param out the narray which will contain the loaded array. This variable must be initially empty.
     * @return true on success
     * @author Aleksander Demko
     */
    template <class T, int DIM>
      bool load_directio_file(const wxString &filename,
          scopira::basekit::narray<T,DIM> &out) {
        scopira::basekit::nindex<DIM> d;

        void *baseary;
        if (!load_directio_file_impl(filename, DIM, d.c_array(), sizeof(T), directio_traints<T>::get_name(), baseary))
          return false;
        out.resize_direct(d, reinterpret_cast<T*>(baseary), scopira::basekit::normal_narray_delete);
        return true;
      }

    /**
     * Writes an array to disk in DIO format.
     * This version only supports the split format. That is, the file itself is an XML
     * file containing the header information of the file, and a pointer to the actually
     * data/payload file, which is seperate.
     *
     * @see @ref scopirabasekitdio
     *
     * @param filename the filename
     * @param out the narray which will contain the loaded array. This variable must be initially empty.
     * @return true on success
     * @author Aleksander Demko
     */
    template <class T, int DIM>
      bool save_directio_file(const wxString &filename,
          const scopira::basekit::narray<T,DIM> &in) {
        return save_directio_file_impl(filename, DIM, in.dimen().c_array(), sizeof(T), directio_traints<T>::get_name(), in.c_array());
      }
  }
}

#endif

