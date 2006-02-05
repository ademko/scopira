
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

#ifndef __INCLUDED_SCOPIRA_MPI_LOOP_H__
#define __INCLUDED_SCOPIRA_MPI_LOOP_H__

#include <mpi.h>

#include <scopira/tool/output.h>

namespace scopira
{
  namespace mpi
  {
    class mpi_loop;
  }
}

#define MPIOUTPUT (scopira::mpi::mpi_output())

/**
 * Constructs the MPI system.
 *
 * You can do this AFTER you set up any other loop-types.
 *
 * Config params that this loop accepts:
 *  mpi=1 requests that mpi be started (only relevant in the 2nd ctor)
 *  nompi=1 cancels any mpi requests
 *
 * @author Aleksander Demko
 */ 
class scopira::mpi::mpi_loop
{
  public:
    /// ctor same as 2nd ctor with startmpibydefault=true
    mpi_loop(int &argc, char **&argv);
    /// ctor
    mpi_loop(bool startmpibydefault, int &argc, char **&argv);
    /// dtor
    ~mpi_loop();

    /**
     * Gets the rank (ID) of the running mpi node.
     * The first one (often the master) is 0.
     *
     * This call is always valid.
     *
     * @author Aleksander Demko
     */ 
    static inline int get_rank(void) { return dm_id; }   // used by function
    /**
     * Gets the total number of nodes in the cluster.
     * 0 means "mpi not running"
     *
     * This call is always valid.
     *
     * @author Aleksander Demko
     */ 
    static inline int get_rank_size(void) { return dm_numid; }   // used by function

  private:
    static int dm_id, dm_numid;
};


namespace scopira
{
  namespace mpi
  {
    // all MPI_COMM_WORLD based

    /// gets the current ID
    /// This call is always valid.
    inline int mpi_rank(void) { return mpi_loop::get_rank(); }
    /// gets the number of processors
    /// This call is always valid.
    inline int mpi_rank_size(void) { return mpi_loop::get_rank_size(); }
    /// does an MPI_Barrier
    inline void mpi_barrier(void) { MPI_Barrier(MPI_COMM_WORLD); }
    /// output function
    scopira::tool::oflow_i& mpi_output(void);
  }
}

#endif

