
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

#ifndef __INCLUDED_SCOPIRA_QTKIT_FILEHEADER_H__
#define __INCLUDED_SCOPIRA_QTKIT_FILEHEADER_H__

#include <vector>

#include <QDomDocument>

#include <scopira/basekit/narray.h>

namespace scopira
{
  namespace qtkit
  {
    // traits stuff
    template <class T> class directio_traints { public: typedef T data_type; };
    template <> class directio_traints<float> { public: static const char * get_name(void) { return "float32"; } };
    template <> class directio_traints<double> { public: static const char * get_name(void) { return "float64"; } };
    template <> class directio_traints<unsigned char> { public: static const char * get_name(void) { return "uint8"; } };
    template <> class directio_traints<char> { public: static const char * get_name(void) { return "int8"; } };
    template <> class directio_traints<short> { public: static const char * get_name(void) { return "int16"; } };
    template <> class directio_traints<int> { public: static const char * get_name(void) { return "int32"; } };

    class FileHeader;
  }
}

/**
 * A file that can be used to read the OCT-specific XML files
 *
 * To open a file, you:
 *  - instantiate an instance of FileHeader
 *  - call LoadHeader
 *  - if LoadHeader returned true, access the members and/or call LoadData
 *
 * To save a file, you:
 *  - instantiate an instance of FileHeader
 *  - write to the members, and/or call SaveData
 *  - call SaveHeader
 *
 *
 * @author Aleksander Demko
 */ 
class scopira::qtkit::FileHeader : public QDomDocument
                   //disable object inheritance to try header only scopira linking
                   //, public virtual scopira::tool::object
{
  private:
    typedef QDomDocument parent_type;

  public:
    /**
     * This will quietly try to load the XML header file too.
     * If you need to verify the load succeeded, call LoadHeader.
     *
     * @author Aleksander Demko
     */ 
    FileHeader(const QString &filename);

    /**
     * The filename of the last LoadHeader or SaveHeader call.
     *
     * @author Aleksander Demko
     */
    const QString &fileName(void) const { return dm_filename; }

    /**
     * Opens the given header file, returns true on success.
     * Note that this doesn't load any data for the secondary files.
     *
     * @author Aleksander Demko
     */ 
    bool loadHeader(void);

    /**
     * Saves the contents of this header to the given file.
     * Note that this doesn't save any data to the secondary files.
     * Returns true on success.
     *
     * @author Aleksander Demko
     */ 
    bool saveHeader(void) const;

    /**
     * Saves the contents of this header to the given file.
     * Note that this doesn't save any data to the secondary files.
     * Returns true on success.
     *
     * Uses the given filename. Changes the filename to this new
     * filename, too.
     *
     * @author Aleksander Demko
     */ 
    bool saveHeader(const QString &newfilename);

    /**
     * Query and see if the given data file exists.
     *
     * @param fieldname the fieldname of this sub data item
     * @param elementtype the element type (int32, float64, etc)
     * @param dimen the dimensions of the on disk file
     * @return true on successful query
     * @author Aleksander Demko
     */
    bool queryData(const QString &fieldname, QString &elem_type, std::vector<size_t> &dimen);

    /**
     * Creates an zeroed out data file of the given dimen.
     *
     * @author Aleksander Demko
     */ 
    template <class T, int DIM>
      bool createData(const QString &fieldname, scopira::basekit::nindex<DIM> sz)
      {
        return createDataImp(fieldname, DIM, sz.c_array(), sizeof(T), directio_traints<T>::get_name());
      }

    /**
     * Save a data array.
     *
     * @param fieldname is the data type name (it'll be used for the field and file names (it should basically be just lower case letters)
     * @param in the actual data
     * @return true on success
     * @author Aleksander Demko
     */ 
    template <class T, int DIM>
      bool saveData(const QString &fieldname,
          const scopira::basekit::narray<T,DIM> &in) {
        return saveDataImp(fieldname, DIM, in.dimen().c_array(), sizeof(T), directio_traints<T>::get_name(), in.c_array());
      }

    /**
     * Loads a data array
     *
     * @param fieldname is the data type name (the tag within the XML file)
     * @param out where to load the data
     * @return true on success
     * @author Aleksander Demko
     */ 
    template <class T, int DIM>
      bool loadData(const QString &fieldname,
          scopira::basekit::narray<T,DIM> &out) {
        scopira::basekit::nindex<DIM> d;

        void *baseary;
        if (!loadDataImp(fieldname, DIM, d.c_array(), sizeof(T), directio_traints<T>::get_name(), baseary))
          return false;
        out.resize_direct(d, reinterpret_cast<T*>(baseary), scopira::basekit::normal_narray_delete);
        return true;
      }

    /**
     * Saves only a slice of a given file.
     * The data must already have either been previously saved with SaveData
     * or created with CreateData.
     *
     * @param fieldname is the data type name in the XML file
     * @param slicez the slice index
     * @param in the actual data, must be one dimension less than what is in the file
     * @return true on success
     * @author Aleksander Demko
     */ 
    template <class T, int DIM>
      bool saveSlice(const QString &fieldname, size_t slicez,
          const scopira::basekit::narray<T,DIM> &in) {
        return saveSliceImp(fieldname, slicez, DIM, in.dimen().c_array(), sizeof(T), directio_traints<T>::get_name(), in.c_array());
      }

    /**
     * Loads a slice from the given file.
     *
     * Unlike the general LoadSlice method, this function already expects the
     * array to be of the proper dimensions. It will not resize it.
     *
     * The data must already have either been previously saved with SaveData
     * or created with CreateData.
     *
     * @param fieldname is the data type name in the XML file
     * @param slicez the slice index
     * @param in the actual data, must be one dimension less than what is in the file
     * @return true on success
     * @author Aleksander Demko
     */ 
    template <class T, int DIM>
      bool loadSlice(const QString &fieldname, size_t slicez,
          scopira::basekit::narray<T,DIM> &out) {
        return loadSliceImp(fieldname, slicez, DIM, out.dimen().c_array(), sizeof(T), directio_traints<T>::get_name(), out.c_array());
      }

    /**
     * The out array must always be one DIM less then the source.
     *
     * @author Aleksander Demko
     */ 
    template <class T, int DIM>
      bool loadSliceRow(const QString &fieldname, size_t yrow, size_t slicez,
          scopira::basekit::narray<T,DIM> &out) {
        return loadSliceRowImp(fieldname, yrow, slicez, DIM, out.dimen().c_array(), sizeof(T), directio_traints<T>::get_name(), out.c_array());
      }

  private:
    bool createDataImp(const QString &filename, short numdimen, size_t *dimen, size_t elemsize,
        const char *id);

    bool saveDataImp(const QString &fieldname, short numdimen, const size_t *dimen,
        size_t elemsize, const char *id, const void *data);
    bool loadDataImp(const QString &fieldname, short numdimen, size_t *dimen, size_t elemsize,
        const char *id, void * &outary);

    bool saveSliceImp(const QString &fieldname, size_t slicez, short numdimen, const size_t *dimen,
        size_t elemsize, const char *id, const void *data);
    bool loadSliceImp(const QString &fieldname, size_t slicez, short numdimen, const size_t *dimen,
        size_t elemsize, const char *id, void *data);

    bool loadSliceRowImp(const QString &fieldname, size_t yrow, size_t slicez, short numdimen, const size_t *dimen,
        size_t elemsize, const char *id, void *data);

  private:
    QString dm_filename;
    bool dm_okctor;
};

#endif

