
/*
 *  Copyright (c) 2004-2007    National Research Council
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
#include <scopira/tool/array.h>
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
     * The queries the DIO file for its dimensionality and element
     * type information.
     * It doesn't do any data loading.
     *
     * @see @ref scopirabasekitdio
     *
     * @param filename the filename
     * @param elementtype the element type (int32, float64, etc)
     * @param dimen the dimensions of the on disk file
     * @return true on successful query
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT bool query_directio_file(const std::string &filename,
        std::string &elementtype, std::vector<size_t> &dimen);

    /**
     * Reads an array from disk in DIO format.
     *
     * @see @ref scopirabasekitdio
     *
     * @param filename the filename
     * @param out the narray which will contain the loaded array. This variable must be initially empty.
     * @return true on success
     * @author Aleksander Demko
     */
    template <class T, int DIM>
      bool load_directio_file(const std::string &filename,
          scopira::basekit::narray<T,DIM> &out) {
        nindex<DIM> d;

        void *baseary;
        if (!load_directio_file_impl(filename, DIM, d.c_array(), sizeof(T), directio_traints<T>::get_name(), baseary))
          return false;
        out.resize_direct(d, reinterpret_cast<T*>(baseary), normal_narray_delete);
        return true;
      }

    /**
     * Writes an array to disk in DIO format.
     *
     * @see @ref scopirabasekitdio
     *
     * @param filename the filename
     * @param out the narray which will contain the loaded array. This variable must be initially empty.
     * @return true on success
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
     * Creates a DIO file of the given type and dimensionality
     * on disk. The element contents of the array on disk is undefined.
     *
     * This function is used to create DIO files for scopira::basekit::bind_directio_file,
     * as scopira::basekit::bind_directio_file can only bind to/open existing files.
     * 
     * @see @ref scopirabasekitdio
     *
     * @param filename the filename
     * @param sz the dimensions of the array to create
     * @return true on success
     * @author Aleksander Demko
     */ 
    template <class T, int DIM>
      bool inline create_directio_file(const std::string &filename, nindex<DIM> sz)
      {
        return create_directio_file_impl(filename, DIM, sz.c_array(), sizeof(T), directio_traints<T>::get_name());
      }
    /**
     * Binds an on disk DIO file to a narray.
     *
     * This function uses the operating system's memory mapping facilities to map the "virtual
     * array" to the disk file. As the elements in the array are accessed, the elements are loaded
     * from disk. As they are written, the OS will queue disk writes.
     *
     * The file must exist. If you need to create a new file, use scopira::basekit::create_directio_file.
     *
     * @see @ref scopirabasekitdio
     *
     * @param filename the filename
     * @param out the narray which will contain the loaded array. This variable must be initially empty.
     * @param readonly if true, this file will be loaded in readonly mode.
     * @return true on success
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


    class base_directio_slice;
    template <class T, int DIM> class directio_slice;
  }
}

/**
 * private concrete base class for directio_slice
 *
 * @author Aleksander Demko
 */ 
class scopira::basekit::base_directio_slice
{
  public:
    /**
     * Is the loader's file in a failed state?
     *
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT bool failed(void) const { return dm_file.failed(); }

    /**
     * Return the number of "rows" int he file, that is,
     * varying the last dimension.
     *
     * @author Aleksander Demko
     */ 
    size_t size_rows(void) const {
      assert(dm_dimen.size()>0);
      return dm_dimen[dm_dimen.size()-1];
    }

    // add custom load_array(void* ) with auto dm_baseoffset for external slice loading?

  protected:
    /// default ctor
    SCOPIRA_EXPORT base_directio_slice(void);

    /**
     * Open the given DIO file.
     * Returns true on success
     *
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT bool open_file(const std::string &filename, bool readonly,
        size_t dimlen, size_t elemsize, const char *elemname);

  protected:
    SCOPIRA_EXPORT void verify_dimen(const char *elemname, size_t subprod);
    SCOPIRA_EXPORT size_t seek_slice(size_t r, size_t elemsize);    // returns rowsize IN elements

  protected:
    scopira::tool::fileflow dm_file;
    scopira::tool::fileflow::offset_t dm_baseoffset;
    scopira::tool::basic_array<size_t> dm_dimen;  //this contains all the dimen of the file, including the last dimention
    const char *dm_elemname;
    size_t dm_dimenprod;
};

/**
 * A incremental slice loader (loads only slices at a time from a DIO file.
 *
 * @author Aleksander Demko
 */ 
template <class T, int DIM> class scopira::basekit::directio_slice : public scopira::basekit::base_directio_slice
{
  public:
    /// ctor
    directio_slice(void) { }

    /**
     * Open the given DIO file.
     * Returns true on success
     *
     * @author Aleksander Demko
     */ 
    bool open_file(const std::string &filename, bool readonly) {
      return base_directio_slice::open_file(filename, readonly, DIM, sizeof(T), directio_traints<T>::get_name());
    }

    /**
     * Returns the dimen of the whole thing, as an index.
     *
     * @author Aleksander Demko
     */
    scopira::basekit::nindex<DIM> dimen(void) const {
      assert(DIM == dm_dimen.size());
      scopira::basekit::nindex<DIM> ret;

      for (int i=0; i<ret.size(); ++i)
        ret[i] = dm_dimen[i];
      return ret;
    }

    /**
     * Returns the dimen as an index.
     *
     * @author Aleksander Demko
     */
    scopira::basekit::nindex<DIM-1> slice_dimen(void) const {
      assert(DIM == dm_dimen.size());
      scopira::basekit::nindex<DIM-1> ret;

      for (int i=0; i<ret.size(); ++i)
        ret[i] = dm_dimen[i];
      return ret;
    }

    /**
     * Load a slice from the file (0.. size_rows()-1)
     *
     * @author Aleksander Demko
     */ 
    bool load_slice(size_t r, scopira::basekit::narray<T,DIM-1> &out) {
      verify_dimen(directio_traints<T>::get_name(), out.dimen().product());
      size_t numelem = seek_slice(r, sizeof(T));
      return dm_file.read_array(out.c_array(), numelem) == numelem;
    }

    /**
     * Save a slice from the file (0.. size_rows()-1)
     *
     * @author Aleksander Demko
     */ 
    bool save_slice(size_t r, scopira::basekit::narray<T,DIM-1> &out) {
      verify_dimen(directio_traints<T>::get_name(), out.dimen().product());
      size_t numelem = seek_slice(r, sizeof(T));
      return dm_file.write_array(out.c_array(), numelem) == numelem;
    }
};

#endif

