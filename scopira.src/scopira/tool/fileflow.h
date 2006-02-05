 
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
  }
}

/**
 * A file system flow. Unbuffered.
 *
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
      append_c = 1024,      // append to any existing file
      trunc_c = 2048,       // this is the default, for output files
      linked_c = 4096,       // internal, you dont need to specifiy this
      existing_c = 8192,    // open an existinf file. do not trunc. do not append.
    };
    /// special fd, when passing these, mode is ignored.
    enum {
      stdin_c = -1,
      stdout_c = -2,
      stderr_c = -3
    };
    enum seek_dir_t {
      seek_start_c = 0,
      seek_end_c = 1,
      seek_cur_c
    };
    typedef ::off_t offset_t;

    /// default constructor
    SCOPIRA_EXPORT fileflow(void);
    /// opening constructor
    SCOPIRA_EXPORT fileflow(const std::string& filename, mode_t mode);
    /**
     * Constructor that uses an existing OS file descriptor.
     * If you pass one of the standard streams (std*._c), mode is ignored)
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT fileflow(int fd, mode_t mode);
    /// destructor
    SCOPIRA_EXPORT virtual ~fileflow();

    /// are we in a failed state?
    SCOPIRA_EXPORT virtual bool failed(void) const;

    /// read raw block data, returns num read in
    SCOPIRA_EXPORT virtual size_t read(byte_t* _buf, size_t _maxsize);
    /// write a raw block of data
    SCOPIRA_EXPORT virtual size_t write(const byte_t* _buf, size_t _size);

    /// opens a file
    SCOPIRA_EXPORT void open(const std::string& filename, mode_t mode);
    /// existing fs/link opener
    SCOPIRA_EXPORT void open(int fd,  mode_t mode);
    /// close the file
    SCOPIRA_EXPORT void close(void);

    /// gets the raw OS object. use with care
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

