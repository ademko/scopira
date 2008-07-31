 
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
 
#ifndef __INCLUDED_SCOPIRA_TOOL_FILEFLOW_H__
#define __INCLUDED_SCOPIRA_TOOL_FILEFLOW_H__

#include <fcntl.h>

#include <scopira/tool/platform.h>

#include <scopira/tool/flow.h>
#include <scopira/tool/export.h>

namespace scopira
{
  namespace tool
  {
    class fileflow;

    class read_flocker;
    class write_flocker;

    class filememory;

    enum {
      copy_c = 0,
      copysoftlink_c = 1,
      copyhardlink_c = 2,
    };

    /**
     * Copies the given srcfile to the given destination (file or dir).
     * Copytype can be linking instead (only used under UNIX)
     *
     * @author Aleksander Demko
     */ 
    bool copy_file(const std::string &srcfile, const std::string &destfile, short copytype = copy_c);
  }
}

/**
 * The is a binary/raw flow stream object that reads and writes files to disk.
 *
 * This class is often the base of many chains, and can also be used by itself for basic
 * file reading and writing.
 *
 * @see @ref scopiratoolflowpage
 * @author Aleksander Demko
 */
class scopira::tool::fileflow : public scopira::tool::iflow_i, public scopira::tool::oflow_i
{
  protected:
    /// current modes, 0 for no-file
    mode_t dm_mode;
    /// the file handle
    int dm_hand;
    /// currently failed?
    bool dm_fail;

  public:

    /// access levels
    enum {
      /// append to any existing file
      append_c = 1024,
      /// this is the default, for output files
      trunc_c = 2048,
      /// internal, you dont need to specifiy this
      linked_c = 4096,
      /// open an existinf file. do not trunc. do not append.
      existing_c = 8192,
    };
    /// special file descriptor (fd) numbers
    enum {
      /// this file descriptor represents the standard input stream, stdin
      stdin_c = -1,
      /// this file descriptor represents the standard output stream, stdout
      stdout_c = -2,
      /// this file descriptor represents the standard error stream, stderr
      stderr_c = -3
    };
    enum seek_dir_t {
      seek_start_c = 0,
      seek_end_c = 1,
      seek_cur_c
    };
    typedef ::off_t offset_t;

    /**
     * The default constructor.
     *
     * You should then use open() to open a file.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT fileflow(void);
    /**
     * A constructor that also opens a file.
     * See the corresponding open() call for parameter information.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT fileflow(const std::string& filename, mode_t mode);
    /**
     * A constructor that also opens a file.
     * See the corresponding open() call for parameter information.
     *
     * @param fd the file descriptor (OS specific)
     * @param mode the file mode to use
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT fileflow(int fd, mode_t mode);

    SCOPIRA_EXPORT virtual ~fileflow();

    /**
     * Check if the file stream is in a failed state.
     *
     * @return true if the stream is in a failed state.
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT virtual bool failed(void) const;

    /**
     * Reads a raw array of bytes from the file.
     *
     * @param _buf the buffer to read into.
     * @param _maxsize the maximum number of bytes to read
     * @return the actual number of bytes read
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT virtual size_t read(byte_t* _buf, size_t _maxsize);
    /**
     * Writes a raw array of bytes from the file.
     *
     * @param _buf the buffer to write from.
     * @param _size the size of the buffer
     * @return the actual number of bytes written
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT virtual size_t write(const byte_t* _buf, size_t _size);

    /**
     * Opens the given on disk file.
     *
     * The mode is a bitmask (or-ed) of  file modes.
     *
     * It usually contains either (or both) of:
     *  - input_c (for reading from a file)
     *  - output_c (for writing to a file)
     *
     * And possible some of:
     *  - append_c (append to any existing file)
     *  - trunc_c (truncate - delete - any existing file)
     *  - existing_c (open an existinf file, and fail if it doesn't exist)
     *
     * Use failed() to check if the open was successfull.
     *
     * @param filename is the filename
     * @param mode is the file mode
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT void open(const std::string& filename, mode_t mode);
    /**
     * Opens a file via an existing OS file descriptor.
     * File descriptors are typically low level OS objects that you typically do not want to use.
     * This method is provided for the few times users would want streams that reflect
     * into standard OS file descriptors.
     *
     * You can either pass it a file descriptor (fd) or one of the standard (std*_c) file descriptors.
     * If you pass one of the standard streams (std*._c), mode is ignored)
     *
     * Use failed() to check if the open was successfull.
     *
     * @param fd the file descriptor (OS specific)
     * @param mode the file mode to use
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT void open(int fd,  mode_t mode);
    /**
     * Close the currently opened file.
     *
     * You normally do not need to call this as the file will be closed
     * automatically if you:
     *  - try to open() another file
     *  - the fileflow object is destroyed
     *
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT void close(void);

    // gets the raw OS object. use with care
    int get_os_handle(void) const { return dm_hand; }

    // additional fileflow specific stuff: seeking

    /// seekers
    SCOPIRA_EXPORT offset_t seek(offset_t howmuch, seek_dir_t dir = seek_start_c);
    /// seeks to the start
    SCOPIRA_EXPORT void seek_reset(void) { seek(0); };

    /// tells the current location of the file pointer
    SCOPIRA_EXPORT offset_t tell(void) const;

    /**
     * Resize the file (see ftruncate).
     *
     * Returns true on success.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT bool resize(offset_t newsz);

    // additional fileflow specific stuff: locking

    /**
     * Locks the (whole) file for (shared) reading.
     * Blocks until the lock is acquired.
     * Don't forget to unlock!
     * File must be open for reading!
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT void read_lock(void);
    /**
     * Checks if a read lock is doable, right now.
     * Returns true if a read lock is doable.
     * Never blocks.
     * File must be open for reading!
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT bool try_read_lock(void);
    /**
     * Locks the (whole) file for (exclusive) writing.
     * Blocks until the lock is acquired.
     * Don't forget to unlock!
     * File must be open for writing!
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT void write_lock(void);
    /**
     * Checks if a write lock is doable, right now.
     * Returns true if a write lock is doable.
     * Never blocks.
     * File must be open for writing!
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT bool try_write_lock(void);
    /**
     * Unlocks the current write or read lock.
     * File must be open!
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT void unlock(void);
};

/**
 * auto read fileflow locker
 * @author Aleksander Demko
 */ 
class scopira::tool::read_flocker
{
  protected:
    scopira::tool::fileflow &dm_file;
  public:
    /// locking ctor
    read_flocker(fileflow &f) : dm_file(f) { dm_file.read_lock(); }
    /// unlock dot
    ~read_flocker() { dm_file.unlock(); }
};

/**
 * auto write fileflow locker
 * @author Aleksander Demko
 */ 
class scopira::tool::write_flocker
{
  protected:
    scopira::tool::fileflow &dm_file;
  public:
    /// locking ctor
    write_flocker(fileflow &f) : dm_file(f) { dm_file.write_lock(); }
    /// unlock dot
    ~write_flocker() { dm_file.unlock(); }
};

/**
 * A file memory mapped.
 *
 * @author Aleksander Demko
 */ 
class scopira::tool::filememory
{
  public:
    typedef ::off_t offset_t;

    enum {
      read_c = 1,
      write_c = 2,
      private_c = 4,      // does MAP_PRIVATE instead of MAP_SHARED
    };
  private:
    void *dm_ptr;
    size_t dm_len;
#ifdef PLATFORM_win32
    HANDLE dm_mappingobj;
#endif
  public:
    /// constructor
    SCOPIRA_EXPORT filememory(void);
    /// destructor
    SCOPIRA_EXPORT ~filememory();

    /// gets the memory pointer, null for none right now
    void *c_array(void) const { return dm_ptr; }

    /**
     * Opens a new file mappping. Will close any existing ones.
     * Mapping will start from the start of the file and be (alteast) len bytes long.
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT bool open(fileflow &f, size_t len, int flags = read_c|write_c);
    /// explicitply close the map
    SCOPIRA_EXPORT void close(void);

    /// sync the map with the disk, uses async
    SCOPIRA_EXPORT void sync(void);
};

#endif

