
/*
 *  Copyright (c) 2004    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#ifndef __INCLUDED_SCOPIRA_BASEKIT_DIRECTIO_H__
#define __INCLUDED_SCOPIRA_BASEKIT_DIRECTIO_H__

#include <vector>

#include <scopira/tool/platform.h>
#include <scopira/tool/export.h>
#include <scopira/tool/fileflow.h>
#include <scopira/basekit/narray.h>

namespace scopira
{
  namespace basekit
  {
    struct directio_header_t;

    SCOPIRA_EXPORT bool save_directio_file_impl(const std::string &filename, short numdimen, const size_t *dimen, size_t elemsize, const char *id, const void *data);
    SCOPIRA_EXPORT bool load_directio_file_impl(const std::string &filename, short numdimen, size_t *dimen, size_t elemsize, const char *id, void * &outary);
    SCOPIRA_EXPORT bool bind_direct_file_impl(scopira::tool::fileflow *f, scopira::tool::byte_t * &baseary, size_t len, narray_delete_i * &func, bool readonly);
    SCOPIRA_EXPORT bool create_directio_file_impl(const std::string &filename, short numdimen, size_t *dimen, size_t elemsize, const char *id);
    SCOPIRA_EXPORT bool bind_directio_file_impl(const std::string &filename, short numdimen, size_t *dimen, size_t elemsize, const char *id, bool readonly, void * &outary, narray_delete_i * &outfunc);
    SCOPIRA_EXPORT void sync_directio_file(narray_delete_i *d);

    // traits stuff
    template <class T> class directio_traints { public: typedef T data_type; };
    template <> class directio_traints<float> { public: static const char * get_name(void) { return "float32"; } };
    template <> class directio_traints<double> { public: static const char * get_name(void) { return "float64"; } };
    template <> class directio_traints<char> { public: static const char * get_name(void) { return "int8"; } };
    template <> class directio_traints<short> { public: static const char * get_name(void) { return "int16"; } };
    template <> class directio_traints<int> { public: static const char * get_name(void) { return "int32"; } };

    /**
     * This lets you query a DIO file for its dimention information.
     * If successful, true will be returned and elementtype and dimen
     * will be populated.
     *
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT bool query_directio_file(const std::string &filename,
        std::string &elementtype, std::vector<size_t> &dimen);

    /**
     * Reads a binary file from disk in DIO format.
     * This however, doesn't do any direct IO/mmaping itself, but just
     * uses the file format.
     *
     * Returns true on success.
     * 
     * @author Aleksander Demko
     */
    template <class T, int DIM>
      bool load_directio_file(const std::string &filename,
          scopira::basekit::narray<T,DIM> &out) {
        nindex<DIM> d;

        void *baseary;
        if (!load_directio_file_impl(filename, DIM, d.c_array(), sizeof(T), directio_traints<T>::get_name(), baseary))
          return false;
        /// baseary has been newed... pass this one to the narray for responsibility
        out.resize_direct(d, reinterpret_cast<T*>(baseary), normal_narray_delete);
        return true;
      }

    /**
     * Writes a binary file to disk in DIO format.
     * This however, doesn't do any direct IO/mmaping itself, but just
     * uses the file format.
     *
     * Returns true on success.
     * 
     * @author Aleksander Demko
     */
    template <class T, int DIM>
      bool save_directio_file(const std::string &filename,
          const scopira::basekit::narray<T,DIM> &in) {
        return save_directio_file_impl(filename, DIM, in.dimen().c_array(), sizeof(T), directio_traints<T>::get_name(), in.c_array());
      }

    /**
     * Binds the current file and offset to the output narray, at the given dimensions.
     * f must be reference counted (and will be further reference counted.
     *
     * Returns true on success.
     * 
     * @author Aleksander Demko
     */ 
    template <class T, int DIM>
      bool bind_direct_file(scopira::tool::fileflow *f,
          scopira::basekit::nindex<DIM> sz, scopira::basekit::narray<T,DIM> &out, bool readonly = false, scopira::tool::fileflow::offset_t skipb = 0)
      {
        scopira::tool::byte_t *baseary;
        narray_delete_i *func;
        out.resize(nindex<DIM>(0));
        if (!bind_direct_file_impl(f, baseary, sz.product()*sizeof(T)+skipb, func, readonly))
          return false;
        baseary += skipb;
        out.resize_direct(sz, reinterpret_cast<T*>(baseary), func);
        return true;
      }

    /**
     * Initialize a ondisk directio file for later usage.
     * Any previous versions will be overwritten.
     *
     * Returns true on success.
     * 
     * @author Aleksander Demko
     */ 
    template <class T, int DIM>
      bool inline create_directio_file(const std::string &filename, nindex<DIM> sz)
      {
        return create_directio_file_impl(filename, DIM, sz.c_array(), sizeof(T), directio_traints<T>::get_name());
      }
    /**
     * Binds an ondisk directio file to a narray.
     * You should have created the file with create_directio_file.
     *
     * Returns true on success.
     *
     * @author Aleksander Demko
     */
    template <class T, int DIM>
      bool bind_directio_file(const std::string &filename, narray<T,DIM> &out, bool readonly = false) {
        nindex<DIM> d;

        void *baseary;
        narray_delete_i *func;
        if (!bind_directio_file_impl(filename, DIM, d.c_array(), sizeof(T), directio_traints<T>::get_name(), readonly, baseary, func))
          return false;
        out.resize_direct(d, reinterpret_cast<T*>(baseary), func);
        return true;
      }
    template <class T, int DIM>
      inline void sync_directio_file(const narray<T,DIM> &out) {
        sync_directio_file(out.get_direct());
      }
  }
}

#endif

