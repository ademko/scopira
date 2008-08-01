
/*
 *  Copyright (c) 2006    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#ifndef __INCLUDED_SCOPIRA_PVM_UTIL_H__
#define __INCLUDED_SCOPIRA_PVM_UTIL_H__

#include <string>

#include <scopira/basekit/narray.h>

namespace scopira
{
  namespace pvm
  {
    /**
     * Spawns the given process as sub tasks.
     *
     * @return returns the task id, or 0 on error
     * @author Aleksander Demko
     */ 
    int spawn_one(const std::string &fullfilename);
    /**
     * Spawns the given process as sub tasks.
     * outids will have the task tids, or be an empty list on error.
     *
     * @author Aleksander Demko
     */ 
    void spawn_many(const std::string &fullfilename, int numtasks, scopira::basekit::narray<int> &out);

    /**
     * Finds a given int in the given array. Returns its index.
     * Returns -1 if its not found.
     * Handy for peer lists, as made from spawn_many().
     *
     * @author Aleksander Demko
     */ 
    int find_index(const scopira::basekit::narray<int> &peers, int tid);

    /**
     * Does this task have a parent task?
     * ie was it spawned via spawn* or pvm_*
     *
     * @author Aleksander Demko
     */ 
    bool has_parent_task(void) { return pvm_parent() != PvmNoParent; }

    /**
     * Returns the recommended number of CPU-bound tasks to launch.
     * Usually, this simply reports the number of hosts * their cpus.
     * However, this PVM version only counts the number of hosts.
     * It cannot factor in the number of CPUs PER host (a shame, yes, I know).
     *
     * @author Aleksander Demko
     */ 
    int default_group_size(void);
  }
}

#endif

