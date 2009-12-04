
/*
 *  Copyright (c) 2009    National Research Council
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
#endif

#include <stdio.h>
#include <assert.h>

#include <scopira/tool/fileflow.h>
#include <scopira/tool/printflow.h>

#include <QThreadStorage>
#include <QMap>

using namespace scopira::tool;

//
//
// mutex (from mutex.h)
//
//

mutex::mutex(void)
{
}

mutex::~mutex(void)
{
}

//
//
// thread
//
//

thread::thread(runnable_i *target)
  : dm_thread(this)
{
  dm_target = target;

  dm_runfunc = 0;
}

thread::thread(runfunc_t func, void *arg)
  : dm_thread(this)
{
  dm_target = 0;

  assert(func);
  dm_runfunc = func;
  dm_runfunc_arg = arg;
}

thread::~thread()
{
  assert(!is_running());
}

void thread::start(void)
{
  dm_thread.start();
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

typedef QThreadStorage<QMap<thread::tlskey_t, void *> * > tlsmap_t;

static tlsmap_t & tlsmap(void)
{
  static tlsmap_t tlsmap;

  return tlsmap;
}

void thread::open_tls(tlskey_t &outkey)
{
  if (!tlsmap().hasLocalData())
    tlsmap().setLocalData(new QMap<tlskey_t, void *>);

  (*tlsmap().localData())[outkey = tlsmap().localData()->size()] = 0;
}

void thread::close_tls(tlskey_t k)
{
  tlsmap().localData()->remove(k);
}

// had to drop the const from the const void *
void thread::set_tls(tlskey_t k, void *val)
{
  (*tlsmap().localData())[k] = val;
}

void * thread::get_tls(tlskey_t k)
{
  return (*tlsmap().localData())[k];
}

void thread::wait_stop(void)
{
  dm_thread.wait();
  assert(!is_running());
}

void thread::run(void)
{
  if (dm_target)
    dm_target->run();
  else
    dm_runfunc(dm_runfunc_arg);
}

//
//
// condition
//
//

condition::condition()
{
}

condition::~condition()
{
}

void condition::notify(void)
{
  dm_con.wakeOne();
}

void condition::notify_all(void)
{
  dm_con.wakeAll();
}

void condition::wait(mutex &mut)
{
  dm_con.wait(mut.get_os_mutex());
}

bool condition::wait(mutex &mut, int msec)
{
  return !dm_con.wait(mut.get_os_mutex(), msec);
}

//
//
// rwlock
//
//

rwlock::rwlock(void)
{
}

/// dtor
rwlock::~rwlock()
{
}

void rwlock::write_lock(void)
{
  dm_rw.lockForWrite();
}

bool rwlock::try_write_lock(void)
{
  return dm_rw.tryLockForWrite();
}

void rwlock::read_lock(void)
{
  dm_rw.lockForRead();
}

bool rwlock::try_read_lock(void)
{
  return dm_rw.tryLockForRead();
}

void rwlock::unlock(void)
{
  dm_rw.unlock();
}

int scopira::tool::num_system_cpus(void)
{
  return QThread::idealThreadCount();
}

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
