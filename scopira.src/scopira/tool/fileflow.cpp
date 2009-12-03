
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

#include <scopira/tool/fileflow.h>

#include <scopira/tool/platform.h>
#include <scopira/tool/file.h> //for copy_file

#ifdef PLATFORM_win32
#include <io.h>
// disable depreacted warnings
#pragma warning(disable:4996)
#else
#include <stdio.h>
#include <unistd.h>
#endif

#include <assert.h>

#ifdef PLATFORM_UNIX
#include <sys/mman.h>
#endif

#include <scopira/tool/platform.h>

#ifdef PLATFORM_linux
// for locking
#include <fcntl.h>
#endif

//BBlibs
//BBtargets libscopira.so

using namespace scopira::tool;
using namespace std;

//
// function
//

bool scopira::tool::copy_file(const std::string &srcfile, const std::string &destfile, short copytype)
{
  std::string realdestfile;

  if (scopira::tool::file(destfile).is_dir()) {
    std::string dummy, justname;

    file::split_path(srcfile, dummy, justname);

    realdestfile = destfile + dir_seperator_c + justname;
  } else
    realdestfile = destfile;

#ifdef PLATFORM_UNIX
  if (copytype == copysoftlink_c)
    return ::symlink(srcfile.c_str(), realdestfile.c_str()) == 0;
  if (copytype == copyhardlink_c)
    return ::link(srcfile.c_str(), realdestfile.c_str()) == 0;
#endif
  // ok, do a full copy
  fileflow inf, outf;

  inf.open(srcfile, scopira::tool::fileflow::input_c);

  if (inf.failed())
    return false;

  outf.open(realdestfile, scopira::tool::fileflow::output_c);

  if (outf.failed())
    return false;

  outf << inf;

  return true;
}

//
//
// fileflow
//
//

/// default constructor
fileflow::fileflow(void)
  : dm_mode(0)
{
}

/// opening constructor
fileflow::fileflow(const std::string& filename, mode_t mode)
  : dm_mode(0)
{
  open(filename, mode);
}

/// existing fs/link constructor
fileflow::fileflow(int fd,  mode_t mode)
  : dm_mode(0)
{
  open(fd, mode);
}

/// destructor
fileflow::~fileflow()
{
  close();
}

/// are we in a failed state?
bool fileflow::failed(void) const
{
  if (dm_mode)
    return dm_fail;
  else
    return true;
}

/// read raw block data, returns num read in
size_t fileflow::read(byte_t* _buf, size_t _maxsize)
{
  if (failed())
    return 0;
  assert(dm_mode & input_c);
  if (_maxsize > 0) {
    assert(_buf);
#ifdef PLATFORM_win32
    _maxsize = ::read(dm_hand, _buf,  static_cast<int>(_maxsize));
#else
    _maxsize = ::read(dm_hand, _buf,  _maxsize);
#endif
    if (_maxsize <= 0) {
      dm_fail = true;
      return 0;
    }
  }

  return _maxsize;
}

/// write a raw block of data
size_t fileflow::write(const byte_t* _buf, size_t _size)
{
  //assert(dm_mode & output_c);
  assert(_buf);
  if (failed())
    return 0;
  if (_size > 0) {
#ifdef PLATFORM_win32
    _size = ::write(dm_hand, _buf, static_cast<int>(_size));
#else
    _size = ::write(dm_hand, _buf, _size);
#endif
    if (_size <= 0) {
      dm_fail = true;
      return 0;
    }
  }

  return _size;
}


/// opens a file
void fileflow::open(const std::string& filename, mode_t mode)
{
  int fmode;

  close();
  
  assert(mode & (input_c|output_c));

  // set the fmode
  dm_mode = mode;
#ifdef PLATFORM_linux
  fmode = O_LARGEFILE;
#else
  fmode = 0;
#endif

  if ( (dm_mode & input_c) && (dm_mode & output_c) )
    fmode |= O_RDWR;
  else if (dm_mode & input_c)
    fmode |= O_RDONLY;    // this is actually 0, so fmode==0 is valid
  else if (dm_mode & output_c)
    fmode |= O_WRONLY;

  if (dm_mode & output_c) {
    fmode |= O_CREAT;
    if (dm_mode & input_c || dm_mode & existing_c)
      ; // do nothing if they have both R and O
    else if (dm_mode & append_c)
      fmode |= O_APPEND;
    else
      fmode |= O_TRUNC;
  }

#ifdef PLATFORM_win32
  fmode |= _O_BINARY;
#endif

  dm_hand = ::open(filename.c_str(), fmode, 0666);

  // failed?
  dm_fail = dm_hand == -1;
  if (dm_fail)
    dm_mode = 0;
}

/// existing fs/link opener
void fileflow::open(int fd, mode_t mode)
{
  close();
  
  // check for special
  switch (fd) {
    case stdin_c:
#ifdef PLATFORM_win32
      fd = fileno(stdin);
#else
      fd = STDIN_FILENO;
#endif
      mode = input_c;
      break;
    case stdout_c:
#ifdef PLATFORM_win32
      fd = fileno(stdout);
#else
      fd = STDOUT_FILENO;
#endif
      mode = output_c;
      break;
    case stderr_c:
#ifdef PLATFORM_win32
      fd = fileno(stderr);
#else
      fd = STDERR_FILENO;
#endif
      mode = output_c;
      break;
  }

  assert(mode);

  // marked as linked
  dm_mode = mode | linked_c;

  dm_hand = fd;
  
  dm_fail = false;
}

/// close the file
void fileflow::close(void)
{
  if (dm_mode) {
    if (!(dm_mode & linked_c))    // close only if not linked
      ::close(dm_hand);
    dm_mode = 0;
  }
}

fileflow::offset_t fileflow::seek(offset_t howmuch, seek_dir_t dir)
{
  offset_t ret;

  int whence;
  switch (dir) {
    case seek_start_c: whence = SEEK_SET; break;
    case seek_cur_c: whence = SEEK_CUR; break;
    default: whence = SEEK_END;
  }
     
  // TODO use llseek(2) under linux for 64bit addressibility

  assert(!failed());
  ret = ::lseek(dm_hand, howmuch, whence);  // do the seek
  assert(ret != static_cast<off_t>(-1));    // see man lseek()

  return ret;
}

fileflow::offset_t fileflow::tell(void) const
{
  return ::lseek(dm_hand, 0, SEEK_CUR);
}

bool fileflow::resize(offset_t newsz)
{
#ifdef PLATFORM_UNIX
  return ::ftruncate(dm_hand, newsz) == 0;
#else
  assert(false);
  return false;
#endif
}

void fileflow::read_lock(void)
{
#ifdef PLATFORM_linux
  ::flock f;

  assert(!failed());

  f.l_type = F_RDLCK;
  f.l_whence = SEEK_SET;
  f.l_start = 0;
  f.l_len = 0;
  f.l_pid = 0;

#ifndef NDEBUG
  int ret =
#endif
  fcntl(dm_hand, F_SETLKW, &f);

  assert(ret == 0);
#endif
}

bool fileflow::try_read_lock(void)
{
#ifdef PLATFORM_linux
  ::flock f;

  assert(!failed());

  f.l_type = F_WRLCK;
  f.l_whence = SEEK_SET;
  f.l_start = 0;
  f.l_len = 0;
  f.l_pid = 0;

#ifndef NDEBUG
  int ret =
#endif
  fcntl(dm_hand, F_SETLKW, &f);

  assert(ret == 0);

  return f.l_type == F_UNLCK;
#else
  // non-linux
  return false;
#endif
}

void fileflow::write_lock(void)
{
#ifdef PLATFORM_linux
  ::flock f;

  assert(!failed());

  f.l_type = F_WRLCK;
  f.l_whence = SEEK_SET;
  f.l_start = 0;
  f.l_len = 0;
  f.l_pid = 0;

#ifndef NDEBUG
  int ret =
#endif
  fcntl(dm_hand, F_SETLKW, &f);

  assert(ret == 0);
#endif
}

bool fileflow::try_write_lock(void)
{
#ifdef PLATFORM_linux
  ::flock f;

  assert(!failed());

  f.l_type = F_WRLCK;
  f.l_whence = SEEK_SET;
  f.l_start = 0;
  f.l_len = 0;
  f.l_pid = 0;

#ifndef NDEBUG
  int ret =
#endif
  fcntl(dm_hand, F_SETLKW, &f);

  assert(ret == 0);

  return f.l_type == F_UNLCK;
#else
  // non-linux
  return false;
#endif
}

void fileflow::unlock(void)
{
#ifdef PLATFORM_linux
  ::flock f;

  assert(!failed());

  f.l_type = F_UNLCK;
  f.l_whence = SEEK_SET;
  f.l_start = 0;
  f.l_len = 0;
  f.l_pid = 0;

#ifndef NDEBUG
  int ret =
#endif
  fcntl(dm_hand, F_SETLKW, &f);

  assert(ret == 0);
#endif
}

//
//
// filememory
//
//

filememory::filememory(void)
  : dm_ptr(0), dm_len(0)
{
}

filememory::~filememory()
{
  close();
}

bool filememory::open(fileflow &f, size_t len, int flags)
{
  assert(len > 0);
#ifdef PLATFORM_UNIX
  int prot = 0;

  close();

  if (flags & read_c)
    prot |= PROT_READ;
  if (flags & write_c)
    prot |= PROT_WRITE;
  if (prot == 0)
    prot = PROT_NONE;   // probably redundant, but we cant be sure

  dm_len = len;
  dm_ptr = ::mmap(0, len, prot, (flags&private_c?MAP_PRIVATE:MAP_SHARED), f.get_os_handle(), 0);

  if (dm_ptr == MAP_FAILED) {
    ::perror("mmap failed!");
    dm_ptr = 0;
    return false;
  }

  return true;
#else
  DWORD prot = 0;
  HANDLE h;

  close();

  assert(flags & read_c);   /// or just silently take this?
  if (flags & write_c)
    prot = PAGE_READWRITE;
  else
    prot = PAGE_READONLY;

  assert((HANDLE)_get_osfhandle(f.get_os_handle()) != INVALID_HANDLE_VALUE);

  dm_len = len;
  h = CreateFileMapping((HANDLE)_get_osfhandle(f.get_os_handle()), 0, prot, 0, (DWORD)len, 0);
  if (h == INVALID_HANDLE_VALUE) {
    ::perror("CreateFileMapping() failed!");
    dm_ptr = 0;
    return false;
  }
  dm_mappingobj = h;

  dm_ptr = MapViewOfFile(dm_mappingobj, (flags & write_c) ? FILE_MAP_ALL_ACCESS : FILE_MAP_READ, 0, 0, len);
  if (!dm_ptr) {
    ::perror("MapViewOfFile() failed!");
    dm_ptr = 0;
    return false;
  }

  return true;
#endif
}

void filememory::close(void)
{
  if (dm_ptr) {
#ifdef PLATFORM_UNIX
#ifndef NDEBUG
    int ret =
#endif
    ::munmap(dm_ptr, dm_len);
    assert(ret == 0);
#else
#ifndef NDEBUG
    BOOL ret =
#endif
    UnmapViewOfFile(dm_ptr);
    assert(ret);
#ifndef NDEBUG
    ret =
#endif
    CloseHandle(dm_mappingobj);
    assert(ret);
#endif

    dm_ptr = 0;
  }
}

void filememory::sync(void)
{
#ifdef PLATFORM_UNIX
  ::msync(dm_ptr, dm_len, MS_ASYNC|MS_INVALIDATE);
#else
  FlushViewOfFile(dm_ptr, dm_len);
#endif
}

