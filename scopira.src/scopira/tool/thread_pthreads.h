
/*
 *  Copyright (c) 2002-2004    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#ifndef __INCLUDED_SCOPIRA_TOOL_THREAD_PTHREADS_H__
#define __INCLUDED_SCOPIRA_TOOL_THREAD_PTHREADS_H__

#include <scopira/tool/platform.h>
#include <scopira/tool/export.h>

#include <pthread.h>
#include <errno.h>

#include <scopira/tool/object.h>

namespace scopira
{
  namespace tool
  {
    class runnable_i;
    class job_i;

    /// a function thet can be called by a thread run routine
    /// you should return 0, i guess
    typedef void* (*runfunc_t)(void*);
    class thread;

    class condition;

    class rwlock;
    class read_locker;
    class write_locker;

    /**
     * Gets the number of processors in this system.
     * 1 is returned on platforms where this could not be
     * easily detected.
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT int num_system_cpus(void);

    template <class T> class shared_area;
    template <class T> class event_area;
    template <class T> class rw_area;

    template <class T> class area_ptr;
    template <class T> class const_area_ptr;
    template <class T> class locker_ptr;
    template <class T> class event_ptr;
    template <class T> class read_locker_ptr;
    template <class T> class write_locker_ptr;
  }
}

/**
 * An Interface for stuff that can "run"
 *
 * @author Aleksander Demko
 */
class scopira::tool::runnable_i
{
  public:
    /**
     * Runs or otherwise executes this runnable task.
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT virtual void run(void) = 0;

  protected:
    virtual ~runnable_i() { }
};

/**
 * A controllable, threaded, "task".
 *
 * @author Aleksander Demko
 */
class scopira::tool::job_i : public virtual scopira::tool::object
{
  public:
    /// starts the job
    SCOPIRA_EXPORT virtual void start(void) = 0;

    /// tells the job to try to stop
    SCOPIRA_EXPORT virtual void notify_stop(void) = 0;

    /// this blocks until the job stops
    SCOPIRA_EXPORT virtual void wait_stop(void) = 0;

    /// return if the job is running
    SCOPIRA_EXPORT virtual bool is_running(void) const = 0;
};

/**
 * An operating system thread object.
 *
 * @author Aleksander Demko
 */
class scopira::tool::thread : public virtual scopira::tool::job_i,
  public virtual scopira::tool::runnable_i
{
  public:
    typedef pthread_key_t tlskey_t;

  protected:
    runnable_i *dm_target;
    runfunc_t dm_runfunc;
    void *dm_runfunc_arg;
    volatile bool dm_running;

    pthread_t dm_thread;

  public:
    /**
     * runnable_i Constructor.
     *
     * If you pass a non-null target, that target's run method
     * will be executed on start().
     *
     * If you pass null, this->run() will be run, so you should
     * descendat from thread and override it to make it do something.
     *
     * @param target the runnable target
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT thread(runnable_i* target);
    /**
      * function pointer constructor. This version uses a function with argument instead
      * runnable_i objects.
      *
      * @param func the runfunc_t... it should return 0
      * @param arg the argument that is passed to your function...
      *  this should be a this object or something, probably. This
      *  may be null (heck, it doesn't even have to be a valid pointer...)
      * @author Aleksander Demko
      */
    SCOPIRA_EXPORT thread(runfunc_t func, void *arg);

    /// destructor
    SCOPIRA_EXPORT virtual ~thread();

    /// the run core
    SCOPIRA_EXPORT virtual void run(void);

    /// starts the actually execution of the thread and its run core
    SCOPIRA_EXPORT virtual void start(void);
    /// doesnt actually do anything
    /// descendant classes might want to implement something if convinient
    SCOPIRA_EXPORT virtual void notify_stop(void);
    /// suspeds caller until this thread stops
    SCOPIRA_EXPORT virtual void wait_stop(void);
    /// is this thread currently running?
    SCOPIRA_EXPORT virtual bool is_running(void) const { return dm_running; }

    /**
     * cause the current thread to sleep
     *
     * @param msec the number of msec (1000=second) to wait
     * @return true if got a notification event, false if timed out
     */
    SCOPIRA_EXPORT static void sleep(int msec);

    /**
      * opens a TLS (thread local storage) slot in all current and future threads.
      * You must call this once before doing any set/get calls.
      * The initial value of the storage in each thread instance will be 0 (null).
      *
      * outkey is where the new key is stored.
      *
      * @author Aleksander Demko
      */
    SCOPIRA_EXPORT static void open_tls(tlskey_t &outkey);
    /**
      * close a TLS slot in all thread instances.
      *
      * @author Aleksander Demko
      */
    SCOPIRA_EXPORT static void close_tls(tlskey_t k);

    /**
      * Set a TLS slot of the CURRENT thread. TLS data values
      * default to null (0).
      * Please note that this applies to the active thread, and
      * may not be directed at individual thread objects.
      * @author Aleksander Demko
      */
    SCOPIRA_EXPORT static void set_tls(tlskey_t k, const void *val);
    /**
      * Get a TLS slot of the CURRENT thread. TLS data values
      * default to null (0).
      * Please note that this applies to the active thread, and
      * may not be directed at individual thread objects.
      * @author Aleksander Demko
      */
    SCOPIRA_EXPORT static void * get_tls(tlskey_t k);

  protected:
    SCOPIRA_EXPORT static void * voidrun(void *arg);
};

/**
 * A  condition  (short  for  ‘‘condition variable’’) is a synchronization
 * device that allows threads to suspend execution and relinquish the pro-
 * cessors  until  some  predicate  on shared data is satisfied. The basic
 * operations on conditions are: signal the condition (when the  predicate
 * becomes true), and wait for the condition, suspending the thread execu-
 * tion until another thread signals the condition.
 *
 * @author Aleksander Demko
 */
class scopira::tool::condition
{
  protected:
    pthread_cond_t dm_con;

  public:
    /**
     * constructor
    */
    SCOPIRA_EXPORT condition(void);
    /// non virtual dtor
    SCOPIRA_EXPORT ~condition();

    /**
     * Notify One waiting thread
     * You don't need to do any mutex locking for this.
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT void notify(void);
    /**
     * Notify ALL the waiting threads.
     * You don't need to do any mutex locking for this.
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT void notify_all(void);
    /**
     * Waits, indefinatly for a notification. You must have a lock
     * on the given mutex before calling this method. The method
     * does a quick unlock before waiting, and reaquires the lock
     * on notification.
     *
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT void wait(mutex &mut);
    /**
     * Waits, indefinatly for a notification. You must have a lock
     * on the given mutex before calling this method. The method
     * does a quick unlock before waiting, and reaquires the lock
     * on notification.
     *
     * @param mut the mutex that has been locked on entry
     * @param msec the number of msec (1000=second) to wait
     * @return true if timed out, false if got a notification event
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT bool wait(mutex &mut, int msec);
};

/**
 * read-write locks.
 *
 * @author Aleksander Demko
 */
class scopira::tool::rwlock
{
  private:
    pthread_rwlock_t dm_rw;

  public:
    /// ctor
    SCOPIRA_EXPORT rwlock(void);
    /// dtor
    SCOPIRA_EXPORT ~rwlock();

    /// locks the rwlock in WRITE mode
    SCOPIRA_EXPORT void write_lock(void);
    /// tries to WRITE lock the rwlock, returns true if aquired lock
    SCOPIRA_EXPORT bool try_write_lock(void);
    /// locks the rwlock in READ mode
    SCOPIRA_EXPORT void read_lock(void);
    /// tries to READ lock the rwlock, returns true if aquired lock
    SCOPIRA_EXPORT bool try_read_lock(void);
    /// unlocks the rwlock (unclocks both WRITE and READ locks)
    SCOPIRA_EXPORT void unlock(void);
};

/**
 * auto read locker, for a rwlock
 *
 * @author Aleksander Demko
 */
class scopira::tool::read_locker
{
  private:
    scopira::tool::rwlock &dm_rw;
  public:
    /// read locks the given rwlock
    read_locker(rwlock &rw) : dm_rw(rw) { dm_rw.read_lock(); }
    /// unlocks the given rwlock
    ~read_locker() { dm_rw.unlock(); }
};

/**
 * auto write locker, for a rwlock
 *
 * @author Aleksander Demko
 */
class scopira::tool::write_locker
{
  private:
    scopira::tool::rwlock &dm_rw;

  public:
    /// read locks the given rwlock
    write_locker(rwlock &rw) : dm_rw(rw) { dm_rw.write_lock(); }
    /// unlocks the given rwlock
    ~write_locker() { dm_rw.unlock(); }
};

/**
 * This class stores data that would be shared between more than one
 * thread.  The class basically associated a mutex with the given
 * data type. locker_ptr may then be called.
 *
 * @author Aleksander Demko
 */
template <class T> class scopira::tool::shared_area
{
  public:
    volatile T pm_data; /// shared data
    mutable scopira::tool::mutex      pm_mutex;
};

/**
 * This is a shared_area with an added condition data member, useful
 * for interthread notifications (event signalling).
 *
 * @author Aleksander Demko
 */ 
template <class T> class scopira::tool::event_area : public scopira::tool::shared_area<T>
{
  public:
    mutable scopira::tool::condition  pm_condition;
};

/**
 * This area type uses a rwlock lock (and not a mutix, as in shared_area).
 * read_locker_ptr and write_locker_ptr are used on this type.
 *
 * @author Aleksander Demko
 */ 
template <class T> class scopira::tool::rw_area
{
  public:
    volatile T pm_data; /// shared data
    mutable scopira::tool::rwlock      pm_rwlock;
};

/**
 * This is the basis for all locking pointer contructs.
 * This version simply converts the volatile structure
 * to a non volatile one.
 *
 * Descendants do an addition lock/unlock operation, useful
 * for concurent programming.
 *
 * This version you'd use if you were sure the are you wanted
 * was already under a locking_ptr scope.
 *
 * @author Aleksander Demko
 */ 
template <class T> class scopira::tool::area_ptr
{
  protected:
    T* dm_ptr;

  public:
    /**
     * Public ctor, for shared_area.
     * @author Aleksander Demko
     */ 
    area_ptr(const shared_area<T>& ref) : dm_ptr(const_cast<T*>(&ref.pm_data)) { }
    /**
     * Public ctor, for rw_area.
     * @author Aleksander Demko
     */ 
    area_ptr(const rw_area<T>& ref) : dm_ptr(const_cast<T*>(&ref.pm_data)) { }

  protected:
    /**
     * Inheritance ctor.
     * @author Aleksander Demko
     */ 
    area_ptr(const volatile T& ref) : dm_ptr(const_cast<T*>(&ref)) { }

  public:
    /// members
    T* get(void) { return dm_ptr; }
    /// pointer behaviour
    T& operator*(void) { return *dm_ptr; }
    /// pointer behaviour
    T* operator->(void) { return dm_ptr; }

    /**
     * reassigns the pointer. this should be called, perhaps, after
     * getting a notifiction (ie. after a wait() call on the condition)
     *
     * @author Aleksander Demko
     */ 
    void reset(void) { dm_ptr = const_cast<T*>(const_cast<volatile T*>(dm_ptr)); }  // this is whacky :)
};

/**
 * This is the basis for all locking pointer contructs.
 * This version simply converts the volatile structure
 * to a non volatile one.
 *
 * Descendants do an addition lock/unlock operation, useful
 * for concurent programming.
 *
 * This version provices const access, useful for read_locker_ptr.
 *
 * @author Aleksander Demko
 */ 
template <class T> class scopira::tool::const_area_ptr
{
  protected:
    const T* dm_ptr;

  public:
    /**
     * Public ctor, for shared_area.
     * @author Aleksander Demko
     */ 
    const_area_ptr(const shared_area<T>& ref) : dm_ptr(const_cast<const T*>(&ref.pm_data)) { }
    /**
     * Public ctor, for rw_area.
     * @author Aleksander Demko
     */ 
    const_area_ptr(const rw_area<T>& ref) : dm_ptr(const_cast<const T*>(&ref.pm_data)) { }

  protected:
    /**
     * Inheritance ctor.
     * @author Aleksander Demko
     */ 
    const_area_ptr(const volatile T& ref) : dm_ptr(const_cast<T*>(&ref)) { }

  public:
    /// members
    const T* get(void) { return dm_ptr; }
    /// pointer behaviour
    const T& operator*(void) { return *dm_ptr; }
    /// pointer behaviour
    const T* operator->(void) { return dm_ptr; }

    /**
     * reassigns the pointer. this should be called, perhaps, after
     * getting a notifiction (ie. after a wait() call on the condition)
     *
     * @author Aleksander Demko
     */ 
    void reset(void) { dm_ptr = const_cast<const T*>(const_cast<volatile T*>(dm_ptr)); }  // this is whacky :)
};

/**
 * similar to locker, but this also a "ptr" like class wrapper
 * around the given object. it removes the volatile modifier from
 * the given class for the duration of it's existance
 *
 * @author Aleksander Demko
 */
template <class T> class scopira::tool::locker_ptr : public scopira::tool::area_ptr<T>
{
  protected:
    scopira::tool::mutex & dm_mut;

  public:
    /// ctor
    locker_ptr(const shared_area<T>& ref) : area_ptr<T>(ref.pm_data), dm_mut(ref.pm_mutex) { dm_mut.lock(); }

    /// dtor
    ~locker_ptr() { dm_mut.unlock(); }
  private:
};

/**
 * This ptr builds on locker_ptr and adds condition aware helper methods.
 *
 * @author Aleksander Demko
 */ 
template <class T> class scopira::tool::event_ptr : public scopira::tool::locker_ptr<T>
{
  protected:
    scopira::tool::condition & dm_cond;

  public:
    /// ctor
    event_ptr(const event_area<T>& ref) : locker_ptr<T>(ref), dm_cond(ref.pm_condition) { }

    /**
     * Reflects the call back to the pm_condition.
     *
     * Remember, you can also call notify from outside the mutex lock region.
     *
     * @author Aleksander Demko
     */ 
    void notify(void) { dm_cond.notify(); }
    /**
     * Reflects the call back to the pm_condition.
     *
     * Remember, you can also call notify from outside the mutex lock region.
     *
     * @author Aleksander Demko
     */ 
    void notify_all(void) { dm_cond.notify_all(); }
    /**
     * Reflector.
     *
     * Calls wait() on the contained pm_condition with its matching
     * pm_mutex.
     *
     * @author Aleksander Demko
     */ 
    void wait(void) { dm_cond.wait(locker_ptr<T>::dm_mut); }
    /**
     * Reflector.
     *
     * Calls wait() on the contained pm_condition with its matching
     * pm_mutex.
     *
     * @return true if timed out, false if got a notification event
     * @author Aleksander Demko
     */ 
    bool wait(int msec) { return dm_cond.wait(locker_ptr<T>::dm_mut, msec); }
};

/**
 * Similar to locker_ptr, this on does a read_lock on a rw_area
 *
 * @author Aleksander Demko
 */ 
template <class T> class scopira::tool::read_locker_ptr : public scopira::tool::const_area_ptr<T>
{
  protected:
    scopira::tool::rwlock & dm_rw;

  public:
    /// ctor
    read_locker_ptr(const rw_area<T>& ref) : const_area_ptr<T>(ref.pm_data), dm_rw(ref.pm_rwlock) { dm_rw.read_lock(); }

    /// dtor
    ~read_locker_ptr() { dm_rw.unlock(); }
};

/**
 * Similar to locker_ptr, this on does a write_lock on a rw_area
 *
 * @author Aleksander Demko
 */ 
template <class T> class scopira::tool::write_locker_ptr : public scopira::tool::area_ptr<T>
{
  protected:
    scopira::tool::rwlock & dm_rw;

  public:
    /// ctor
    write_locker_ptr(const rw_area<T>& ref) : area_ptr<T>(ref.pm_data), dm_rw(ref.pm_rwlock) { dm_rw.write_lock(); }

    /// dtor
    ~write_locker_ptr() { dm_rw.unlock(); }
};

#endif


