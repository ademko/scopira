
/*
 *  Copyright (c) 2001-2004    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#ifndef __INCLUDED_SCOPIRA_TOOL_MUTEX_PTHREADS_H__
#define __INCLUDED_SCOPIRA_TOOL_MUTEX_PTHREADS_H__

#include <scopira/tool/platform.h>
#include <scopira/tool/export.h>

#include <pthread.h>

#include <errno.h>

namespace scopira
{
  namespace tool
  {
    class mutex;
    class locker;
  }
}

/**
 * A  mutex  is  a  MUTual  EXclusion device, and is useful for protecting
 * shared data structures from concurrent modifications, and  implementing
 * critical sections and monitors.
 *
 * @author Aleksander Demko
 */
class scopira::tool::mutex
{
  protected:
    pthread_mutex_t dm_mut;

  public:
    /// basi
    SCOPIRA_EXPORT mutex(void);
    SCOPIRA_EXPORT ~mutex(void);

    /**
     * Aquires and locks the mutex.
     * The call blocks if its locked by another thread.
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT void lock(void) { pthread_mutex_lock(&dm_mut); }

    /**
     * Unlocks a previously locked mutex.
     * Always make sure you match your locks and unlocks (use
     * the locker utility for help)
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT void unlock(void) { pthread_mutex_unlock(&dm_mut); }

    /**
     * Bahaves just like lock(), except that if the mutex
     * is already locked, this function will return false
     * rather than perform a blocking wait.
     *
     * True will be returned if the lock was aquired. Don't forget
     * to unlock() it.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT bool try_lock(void) { return EBUSY != pthread_mutex_trylock(&dm_mut); }

    /**
     * Returns the internal operating system primitive.
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT inline pthread_mutex_t * get_os_mutex(void) { return &dm_mut; }
};

/**
 * This automatic variable locks and unllocks mutexs on construction
 * and destruction. This is for auto-pointer like critical sections
 * in scope blocks and makes sur eyou don't forget
 * to unlock a mutex on ALL scope block exits.
 *
 * @author Aleksander Demko
 */
class scopira::tool::locker
{
  private:
    mutex &dm_mut;
  public:
    /**
     * Locks the given mutex on construction.
     * @author Aleksander Demko
     */ 
    locker(mutex &mut) : dm_mut(mut) { dm_mut.lock(); }
    /**
     * Unlocks the mutex on destruction.
     * @author Aleksander Demko
     */ 
    ~locker() { dm_mut.unlock(); }
};

#endif

