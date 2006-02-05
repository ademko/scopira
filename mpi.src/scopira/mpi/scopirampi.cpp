
/*
 *  Copyright (c) 2005    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#include <scopira/mpi/loop.h>
#include <scopira/mpi/send.h>
#include <scopira/mpi/flow.h>

#include <scopira/tool/output.h>
#include <scopira/core/loop.h>

//BBlibs mpi scopira
//BBtargets libscopirampi.so

using namespace scopira::mpi;

int mpi_loop::dm_id, mpi_loop::dm_numid;

mpi_loop::mpi_loop(int &argc, char **&argv)
{
  assert(scopira::core::basic_loop::instance() && "[You need to have a basic_loop running before mpi_loop]\n");

  assert(dm_numid == 0);  // already called? huh?

  if (scopira::core::basic_loop::instance()->get_config("nompi") != "1") {
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &dm_id);
    MPI_Comm_size(MPI_COMM_WORLD, &dm_numid);
    assert(dm_numid>0);
  }
}

mpi_loop::mpi_loop(bool startmpibydefault, int &argc, char **&argv)
{
  assert(scopira::core::basic_loop::instance() && "[You need to have a basic_loop running before mpi_loop]\n");

  assert(dm_numid == 0);  // already called? huh?

  if ( (startmpibydefault || scopira::core::basic_loop::instance()->get_config("mpi") == "1")
      &&
      scopira::core::basic_loop::instance()->get_config("nompi") != "1") {
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &dm_id);
    MPI_Comm_size(MPI_COMM_WORLD, &dm_numid);
    assert(dm_numid>0);
  }
}

mpi_loop::~mpi_loop()
{
  if (dm_numid != 0)
    MPI_Finalize();
  dm_id = 0;
  dm_numid = 0;
}

scopira::tool::oflow_i& scopira::mpi::mpi_output(void)
{
  return scopira::tool::gm_tool_output() << "N-" << mpi_rank() << ": ";
}

void scopira::mpi::mpi_send(int destid, const std::string &s)
{
  size_t sz = s.size();
  MPI_Send(&sz, 1, MPI_SIZET, destid, 0, MPI_COMM_WORLD);
  if (!s.empty())
    MPI_Send(const_cast<char*>(s.c_str()), sz, MPI_CHAR, destid, 0, MPI_COMM_WORLD);
}

std::string scopira::mpi::mpi_recv(int srcid)
{
  MPI_Status st;
  std::string ret;
  size_t sz;

  MPI_Recv(&sz, 1, MPI_SIZET, srcid, 0, MPI_COMM_WORLD, &st);

  ret.resize(sz);
  if (!ret.empty())
    MPI_Recv(const_cast<char*>(ret.c_str()), sz, MPI_CHAR, srcid, 0, MPI_COMM_WORLD, &st);    // writing into c_str()... oy

  return ret;
}

mpioflow::mpioflow(int destid)
  : dm_destid(destid), dm_failed(false)
{
}

mpioflow::~mpioflow()
{
  size_t _size = static_cast<size_t>(-1);
  MPI_Send(&_size, 1, MPI_SIZET, dm_destid, 0, MPI_COMM_WORLD);
}

size_t mpioflow::write(const byte_t* _buf, size_t _size)
{
  MPI_Send(&_size, 1, MPI_SIZET, dm_destid, 0, MPI_COMM_WORLD);
  MPI_Send(const_cast<byte_t*>(_buf), _size, MPI_BYTE, dm_destid, 0, MPI_COMM_WORLD);
  return _size;
}

mpiiflow::mpiiflow(int srcid)
  : dm_srcid(srcid), dm_failed(false)
{
}

mpiiflow::~mpiiflow()
{
  if (dm_failed)
    return;

  // if im not in a failed state, then read the end block off the mpi stream
  size_t _size;
  MPI_Status st;

  MPI_Recv(&_size, 1, MPI_SIZET, dm_srcid, 0, MPI_COMM_WORLD, &st);

  assert(_size == static_cast<size_t>(-1));
}

size_t mpiiflow::read(byte_t* _buf, size_t _maxsize)
{
  size_t _size;
  MPI_Status st;

  if (dm_failed)
    return 0;

  MPI_Recv(&_size, 1, MPI_SIZET, dm_srcid, 0, MPI_COMM_WORLD, &st);

  if (_size == static_cast<size_t>(-1)) {
    dm_failed = true;
    return 0;
  }

  assert(_size<=_maxsize);
  MPI_Recv(_buf, _size, MPI_BYTE, dm_srcid, 0, MPI_COMM_WORLD, &st);

  return _size;
}

