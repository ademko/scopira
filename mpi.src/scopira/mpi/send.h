
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

#ifndef __INCLUDED_SCOPIRA_MPI_SEND_H__
#define __INCLUDED_SCOPIRA_MPI_SEND_H__

#include <scopira/basekit/narray.h>
#include <scopira/mpi/traits.h>

namespace scopira
{
  namespace mpi
  {
    /// string mpi send
    void mpi_send(int destid, const std::string &s);
    /// string mpi recv
    std::string mpi_recv(int srcid);
    /// scalar mpi send
    template <class T> void mpi_send(int destid, T d);
    /// scalar mpi recv
    template <class T> T mpi_recv(int srcid);
    /// scalar mpi recv> this variant will list for a send from ANY rank
    /// and return the rank that sent it via recvid
    template <class T> T mpi_recv_any(int &recvid);
    /// array mpi send
    template <class T> void mpi_send(int destid, const T *buf, size_t numelem);
    /// array mpi recv
    template <class T> size_t mpi_recv(int srcid, T *buf, size_t bufsize_in_elem);
    /// narray mpi send
    template <class T, int DIM> void mpi_send(int destid, const scopira::basekit::narray<T,DIM> &data);
    /// narray mpi recv
    template <class T, int DIM> void mpi_recv(int srcid, scopira::basekit::narray<T,DIM> &data);
    /// narray_o mpi send
    template <class T, int DIM> void mpi_send(int destid, const scopira::basekit::narray_o<T,DIM> &data);
    /// narray_o mpi recv
    template <class T, int DIM> void mpi_recv(int srcid, scopira::basekit::narray_o<T,DIM> &data);
  }
}

template <class T>
void scopira::mpi::mpi_send(int destid, T d)
{
  MPI_Send(&d, 1, mpi_traints<T>::tid(), destid, 0, MPI_COMM_WORLD);
}

template <class T>
T scopira::mpi::mpi_recv(int srcid)
{
  T ret;
  MPI_Status st;
  MPI_Recv(&ret, 1, mpi_traints<T>::tid(), srcid, 0, MPI_COMM_WORLD, &st);
  return ret;
}

template <class T>
T scopira::mpi::mpi_recv_any(int &recvid)
{
  T ret;
  MPI_Status st;
  MPI_Recv(&ret, 1, mpi_traints<T>::tid(), MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &st);
  recvid = st.MPI_SOURCE;
  return ret;
}

template <class T>
void scopira::mpi::mpi_send(int destid, const T *buf, size_t numelem)
{
  MPI_Send(buf, numelem, mpi_traints<T>::tid(), destid, 0, MPI_COMM_WORLD);
}

// returns number read
template <class T>
size_t scopira::mpi::mpi_recv(int srcid, T *buf, size_t bufsize_in_elem)
{
  MPI_Status st;
  int count;
  MPI_Recv(&buf, bufsize_in_elem, mpi_traints<T>::tid(), srcid, 0, MPI_COMM_WORLD, &st);
  MPI_Get_count(&st, mpi_traints<T>::tid(), &count);
  assert(count>=0 && count<=bufsize_in_elem);
  return count;
}

template <class T, int DIM>
void scopira::mpi::mpi_send(int destid, const scopira::basekit::narray<T,DIM> &data)
{
  // debug send tagging of some sort?
  MPI_Send(const_cast<size_t*>(data.dimen().c_array()), DIM, MPI_SIZET, destid, 0, MPI_COMM_WORLD);
  if (!data.empty())
    MPI_Send(const_cast<T*>(data.c_array()), data.size(), mpi_traints<T>::tid(), destid, 0, MPI_COMM_WORLD);
}

template <class T, int DIM>
void scopira::mpi::mpi_recv(int srcid, scopira::basekit::narray<T,DIM> &data)
{
  scopira::basekit::nindex<DIM> d;
  MPI_Status st;
  MPI_Recv(d.c_array(), DIM, MPI_SIZET, srcid, 0, MPI_COMM_WORLD, &st);
  data.resize(d);
  if (!data.empty())
    MPI_Recv(data.c_array(), data.size(), mpi_traints<T>::tid(), srcid, 0, MPI_COMM_WORLD, &st);
}

template <class T, int DIM>
void scopira::mpi::mpi_send(int destid, const scopira::basekit::narray_o<T,DIM> &data)
{
  // debug send tagging of some sort?
  MPI_Send(const_cast<size_t*>(data.dimen().c_array()), DIM, MPI_SIZET, destid, 0, MPI_COMM_WORLD);
  if (!data.empty())
    MPI_Send(const_cast<T*>(data.c_array()), data.size(), mpi_traints<T>::tid(), destid, 0, MPI_COMM_WORLD);
}

template <class T, int DIM>
void scopira::mpi::mpi_recv(int srcid, scopira::basekit::narray_o<T,DIM> &data)
{
  scopira::basekit::nindex<DIM> d;
  MPI_Status st;
  MPI_Recv(d.c_array(), DIM, MPI_SIZET, srcid, 0, MPI_COMM_WORLD, &st);
  data.resize(d);
  if (!data.empty())
    MPI_Recv(data.c_array(), data.size(), mpi_traints<T>::tid(), srcid, 0, MPI_COMM_WORLD, &st);
}

#endif


