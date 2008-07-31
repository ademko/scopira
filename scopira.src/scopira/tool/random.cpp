
/*
 *  Copyright (c) 2002    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#include <scopira/tool/random.h>

#include <time.h>

#include <scopira/tool/fileflow.h>
#include <scopira/tool/platform.h>

#ifndef PLATFORM_win32
#include <unistd.h>
#include <sys/time.h>
#endif

using namespace scopira::tool;


int scopira::tool::time_seed(void)
{
#ifdef PLATFORM_win32
  return static_cast<int>(::time(0));
#else
  //return ::time(0) + 77777 * ::getpid();
  struct timeval tv;
  ::gettimeofday(&tv, 0);
  return 1000000 * tv.tv_sec + tv.tv_usec;  //just mash em together
#endif
}

// the default is the unlimited capacity device, using some secure hash
// try "/dev/random" for blocking when the entropy pool has drained
const char * const sysdev_gen::default_token = "/dev/urandom";

/// internal class
class sysdev_gen::imp_t
{
  public:
    imp_t(const std::string &token)
      : m_file(token, fileflow::input_c) { }
    unsigned int next(void) {
      unsigned int result;

      m_file.read(reinterpret_cast<fileflow::byte_t*>(&result), sizeof(result));

      return result;
    }
  private:
    fileflow m_file;
};

sysdev_gen::sysdev_gen(const std::string& token)
  : m_imp(new imp_t(token))
{
}

sysdev_gen::~sysdev_gen()
{
  delete m_imp;
}

sysdev_gen::result_type sysdev_gen::operator()(void)
{
  return m_imp->next();
}


sysrand_gen::sysrand_gen(unsigned int seed0)
{
  ::srand(seed0);
}

void sysrand_gen::seed(unsigned int seed0)
{
  ::srand(seed0);
}

sysrand_gen::result_type sysrand_gen::next(void)
{
  return ::rand();
}

sysrandom_gen::sysrandom_gen(unsigned int seed0)
{
  ::srandom(seed0);
}

void sysrandom_gen::seed(unsigned int seed0)
{
  ::srandom(seed0);
}

sysrandom_gen::result_type sysrandom_gen::next(void)
{
  return ::random();
}

#ifdef SCO_DEBUG_random
#include <scopira/tool/linconrandom.h>
#include <scopira/tool/distrandom.h>
int main(void)
{
  // lots of unused var warnings!
  int i;
  minstd_rand0 s_go(time_seed());
  sysrand_gen s_go2(time_seed());
  sysrandom_gen s_go3(time_seed());
  sysdev_gen s_go4;
  uni01_dist<sysdev_gen> subgen(s_go4);
  unireal_dist<sysdev_gen> subgen2(s_go4, 10, 20);
  unismallint_dist<minstd_rand0> subgen3(s_go, 5, 10);
  fileflow output(fileflow::stderr_c, 0);

  output << "MIN: " << static_cast<double>(std::numeric_limits<sysdev_gen::result_type>::min()) << '\n';
  output << "MIN2: " << static_cast<double>(s_go4.min()) << '\n';
  output << "MAX: " << static_cast<double>(std::numeric_limits<sysdev_gen::result_type>::max()) << '\n';
  output << "MAX2: " << static_cast<double>(s_go4.max()) << '\n';

  output << static_cast<double>(s_go4.max() - s_go4.min()) << '\n';

  for (i=0; i<100; ++i)
    output << ' ' << static_cast<double>(subgen3());

  return 0;
}
#endif


//BBlibs
//BBtargets libscopira.so

