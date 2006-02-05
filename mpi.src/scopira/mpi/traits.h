
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

#ifndef __INCLUDED_SCOPIRA_MPI_TRAITS_H__
#define __INCLUDED_SCOPIRA_MPI_TRAITS_H__

#include <mpi.h>

// make a virtual type
#ifndef MPI_SIZET
#define MPI_SIZET (MPI_UNSIGNED_LONG)
#endif

namespace scopira
{
  namespace mpi
  {
    /**
     * Trait calsses for mapping types to MPI_constants
     *
     * @author Aleksander Demko
     */ 
    template <class T> class mpi_traints { public: typedef T data_type; };

    template <> class mpi_traints<bool> { public: static MPI_Datatype tid(void) { return MPI_UNSIGNED_CHAR; } };
    template <> class mpi_traints<short> { public: static MPI_Datatype tid(void) { return MPI_SHORT; } };
    template <> class mpi_traints<int> { public: static MPI_Datatype tid(void) { return MPI_INT; } };
    template <> class mpi_traints<size_t> { public: static MPI_Datatype tid(void) { return MPI_SIZET; } };
    template <> class mpi_traints<float> { public: static MPI_Datatype tid(void) { return MPI_FLOAT; } };
    template <> class mpi_traints<double> { public: static MPI_Datatype tid(void) { return MPI_DOUBLE; } };

    // add more as needed
  }
}

#endif

