
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

#ifndef __INCLUDED_SCOPIRA_MPI_FLOW_H__
#define __INCLUDED_SCOPIRA_MPI_FLOW_H__

#include <scopira/tool/flow.h>

namespace scopira
{
  namespace mpi
  {
    class mpioflow;
    class mpiiflow;
  }
}

/**
 * An output flow that streams its bytes over MPI.
 * This should be complemented by a mpiiflow.
 *
 * @author Aleksander Demko
 */ 
class scopira::mpi::mpioflow : public scopira::tool::oflow_i
{
  private:
    int dm_destid;
    bool dm_failed;
  public:
    /// ctor
    mpioflow(int destid);
    /// dtor, sends end packet
    virtual ~mpioflow();

    virtual bool failed(void) const { return dm_failed; }

    virtual size_t write(const byte_t* _buf, size_t _size);
};

/**
 * An output flow that streams its bytes over MPI.
 * This should be complemented by a mpioflow.
 *
 * @author Aleksander Demko
 */ 
class scopira::mpi::mpiiflow : public scopira::tool::iflow_i
{
  private:
    int dm_srcid;
    bool dm_failed;
  public:
    /// ctor
    mpiiflow(int srcid);
    /// dtor
    virtual ~mpiiflow();

    virtual bool failed(void) const { return dm_failed; }

    virtual size_t read(byte_t* _buf, size_t _maxsize);
};

#endif

