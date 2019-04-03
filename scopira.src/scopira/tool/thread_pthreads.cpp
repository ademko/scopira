
/*
 *  Copyright (c) 2002-2003    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#include <scopira/tool/thread.h>

#include <scopira/tool/platform.h>
#include <limits>

// All but Windows
#ifndef PLATFORM_win32
#include <sys/time.h>
#include <unistd.h>
#endif

#ifdef PLATFORM_osx
// for num_system_cpus for mac
#include <sys/types.h>
#include <sys/sysctl.h>
#endif

#include <stdio.h>
#include <assert.h>

#include <scopira/tool/fileflow.h>
#include <scopira/tool/printflow.h>

//BBlibs
//BBtargets libscopira.so

using namespace scopira::tool;

//
//
// mutex (from mutex.h)
//
//

mutex::mutex(void)
{
  pthread_mutex_init(&dm_mut, 0);
}

mutex::~mutex(void)
{
  pthread_mutex_destroy(&dm_mut);
}

//
//
// thread
//
//

thread::thread(runnable_i *target)
{
  dm_runfunc = voidrun;
  dm_runfunc_arg = static_cast<thread*>(this); // this is probably not needed

  dm_target = target;
  dm_running = false;
}

thread::thread(runfunc_t func, void *arg)
{
  dm_runfunc = func;
  assert(func);
  dm_runfunc_arg = arg;

  dm_target = 0;
  dm_running = false;
}

thread::~thread()
{
}

void thread::start(void)
{
  pthread_create(&dm_thread, 0, dm_runfunc, dm_runfunc_arg);
}

void thread::notify_stop(void)
{
  // nothing needed
}

void thread::sleep(int msec)
{
#ifdef PLATFORM_win32
  Sleep(msec);
#else
  struct timespec t;
  t.tv_sec = msec/1000;
  t.tv_nsec = msec%1000 * 1000000;
  nanosleep(&t, 0);
#endif
}

void thread::open_tls(tlskey_t &outkey)
{
#ifndef NDEBUG
  int ret =
#endif
  pthread_key_create(&outkey, 0);   // no dtor
  assert(ret == 0);
}

void thread::close_tls(tlskey_t k)
{
#ifndef NDEBUG
  int ret =
#endif

  pthread_key_delete(k);
  assert(ret == 0);
}

void thread::set_tls(tlskey_t k, const void *val)
{
#ifndef NDEBUG
  int ret =
#endif

  pthread_setspecific(k, val);
  assert(ret == 0);
}

void * thread::get_tls(tlskey_t k)
{
  return pthread_getspecific(k);
}

void thread::wait_stop(void)
{
  pthread_join(dm_thread, 0);   // 0/null -> don't care about return
}

void thread::run(void)
{
  // provide a valid target or override 'run' in a derived class
  assert(dm_target);
  dm_target->run();
}

void * thread::voidrun(void *arg)
{
  thread *t;

  assert(arg);
  t = reinterpret_cast<thread*>(arg);
  assert(t->is_alive_object());

  // dispatch
  t->dm_running = true;
  t->run();
  t->dm_running = false;

  return 0;
}

//
//
// condition
//
//

condition::condition()
{
  pthread_cond_init(&dm_con, 0);
}

condition::~condition()
{
  pthread_cond_destroy(&dm_con);
}

void condition::notify(void)
{
  pthread_cond_signal(&dm_con);
}

void condition::notify_all(void)
{
  pthread_cond_broadcast(&dm_con);
}

void condition::wait(mutex &mut)
{
  pthread_cond_wait(&dm_con, mut.get_os_mutex());
}

#ifdef PLATFORM_win32
static int gettimeofday (struct timeval *tv, void* tz)
{
  union {
    long long ns100; /*time since 1 Jan 1601 in 100ns units */
    FILETIME ft;
  } now;

  GetSystemTimeAsFileTime (&now.ft);
  tv->tv_usec = (long) ((now.ns100 / 10LL) % 1000000LL);
  tv->tv_sec = (long) ((now.ns100 - 116444736000000000LL) / 10000000LL);
  return (0);
}
#endif

bool condition::wait(mutex &mut, int msec)
{
  bool ret;

  struct timeval now;
  struct timespec t;

  // mili/micro/nano/pico
  gettimeofday(&now, 0);
  t.tv_sec = now.tv_sec + msec/1000;
  t.tv_nsec = now.tv_usec * 1000 + msec%1000 * 1000000;
  if (t.tv_nsec > 1000000000) {
    t.tv_sec += 1;
    t.tv_nsec -= 1000000000;
  }
  ret = ETIMEDOUT == pthread_cond_timedwait(&dm_con, mut.get_os_mutex(), &t);

  return ret;
}

///
/// rwlock
///
rwlock::rwlock(void)
{
#ifndef NDEBUG
  int r =
#endif

  pthread_rwlock_init(&dm_rw, 0);
  assert(r == 0);
}

/// dtor
rwlock::~rwlock()
{
#ifndef NDEBUG
  int r =
#endif

  pthread_rwlock_destroy(&dm_rw);
  assert(r == 0);
}

void rwlock::write_lock(void)
{
  pthread_rwlock_wrlock(&dm_rw);
}

bool rwlock::try_write_lock(void)
{
  return EBUSY != pthread_rwlock_trywrlock(&dm_rw);
}

void rwlock::read_lock(void)
{
  pthread_rwlock_rdlock(&dm_rw);
}

bool rwlock::try_read_lock(void)
{
  return EBUSY != pthread_rwlock_tryrdlock(&dm_rw);
}

void rwlock::unlock(void)
{
  pthread_rwlock_unlock(&dm_rw);
}

#ifdef PLATFORM_linux
int scopira::tool::num_system_cpus(void)
{
  return sysconf(_SC_NPROCESSORS_CONF);
}
#elif defined(PLATFORM_osx)
int scopira::tool::num_system_cpus(void)
{
  int np;
  size_t length = sizeof( np );
  if( sysctlbyname("hw.ncpu", &np, &length, NULL, 0) )
    np = 1;
  return np;
}
#else
int scopira::tool::num_system_cpus(void)
{
  return pthread_num_processors_np();
}
#endif

#ifdef SCOPIRA_THREAD_TEST_CODE

#include <scopira/tool/output.h>
#include <scopira/tool/thread.h>

using namespace scopira::tool;

mutex outmut;
mutex dmut;
volatile int data;
condition dsem;

class counter : public virtual runnable_i
{
  protected:
    const char *name;
    int max;
  public:
    counter(const char *_name, int _max) : name(_name), max(_max) { }
    virtual void run(void) {
      int i;

      for (i=0; i<max; i++) {
        thread::sleep(1000);//or a busy loop here
        {
          locker l(outmut);

          fprintf(stderr,"PROD: %s:%d\n", name, i);
        }
      }
    }
};

class prodi : public virtual runnable_i
{
  protected:
    int max;
  public:
    prodi(int _max) : max(_max) { }
    virtual void run(void) {
      int i;

      {
        locker bb(outmut);
        fprintf(stderr,"PROD: starting up\n");
      }
      for (i=1; i<max; i++) {
        {
          locker bb(outmut);
          fprintf(stderr,"PROD: starting loop\n");
        }
        thread::sleep(500);
        {
          locker cc(dmut);

          {
            locker bb(outmut);
            fprintf(stderr,"PROD: %d\n", i);
          }
          data = i*10;
          dsem.notify();  // notify one
        }
      }
      {
        locker bb(outmut);
        fprintf(stderr,"PROD: shuttin down\n");
      }
    }

};

class coni : public virtual runnable_i
{
  protected:
    const char *name;
    int max;
  public:
    coni(const char *_name, int _max) : name(_name), max(_max) { }
    virtual void run(void) {
      int i, v;

      for (i=0; i<max; i++) {
        {
          locker kk(dmut);
          dsem.wait(dmut);
          v = data;
        }
        {
          locker l(outmut);
          fprintf(stderr, "Got data %s: %d\n", name, v);
        }
        thread::sleep(1000);
      }
    }
};

int main(void)
{
  //counter c1("one", 20), c2("two", 10), c3("three", 30);
  prodi c1(50); coni c2("Alpha", 20), c3("Beta", 20);
  thread t1(&c1), t2(&c2), t3(&c3);

  data = 0;

  t1.start();
  t2.start();
  t3.start();

  t1.wait_stop();
  t2.wait_stop();
  t3.wait_stop();

  return 0;
}
#endif
