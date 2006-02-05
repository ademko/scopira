
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

#include <scopira/tool/time.h>

#include <string>

#ifdef PLATFORM_win32
// disable depreacted warnings
#pragma warning(disable:4996)
#else
#include <sys/time.h>
#endif

//BBtargets libscopira.so

using namespace scopira::tool;

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

timespan::timespan(int days, int hours, int minutes, int seconds)
{
  // test that units are in the valid ranges
  bool valid =
    days >=0 ||
    (hours >=0 && hours <= 23) ||
    (minutes >=0 && minutes <= 59) ||
    (seconds >=0 && seconds <= 59);

  // assert of invalid test
  assert(valid);

  // calculate time value or assign -1 if invalid
  dm_time = valid ? ((days*24*60*60) + (hours*60*60) + (minutes*60) + seconds) : -1;
}

timestamp::timestamp(int year, int month, int mday, int hour, int minute, int second, int isdst)
{
  assign(year, month, mday, hour, minute, second, isdst);
}

timestamp timestamp::current_time()
{
  time_t clock;
  ::time(&clock);

#ifdef PLATFORM_win32
  return timestamp(mktime(localtime(&clock)));
#else
  struct tm timmy;
  localtime_r(&clock, &timmy);
  return timestamp(mktime(&timmy));
#endif
}

void timestamp::format(const std::string& format, std::string& out, bool gmt) const
{
  // clear and resize var
  out.clear();
  out.resize(100);

  // dm_time must be valid
  assert(dm_time != -1);

#ifdef PLATFORM_win32
  int size = strftime(&out[0], out.size(), format.c_str(), gmt ? gmtime(&dm_time) : localtime(&dm_time));
#else
  struct tm timmy;
  if (gmt)
    gmtime_r(&dm_time, &timmy);
  else
    localtime_r(&dm_time, &timmy);
  int size = strftime(&out[0], out.size(), format.c_str(), &timmy);
#endif

  // resize to proper size
  out.resize(size);
}

void timestamp::assign(int year, int month, int mday, int hour, int minute, int second, int isdst, bool isgmt)
{
  struct tm atm;
  assert(year >= 1900);
  atm.tm_year = year-1900;
  
  assert(month >=0 && month <= 11);
  atm.tm_mon = month;
  
  assert(mday >= 1 && mday <= 31);
  atm.tm_mday = mday;
  
  assert(hour >= 0 && hour <= 23);
  atm.tm_hour = hour;

  assert(minute >= 0 && minute <= 59);
  atm.tm_min = minute;

  assert(second >= 0 && second <= 59);
  atm.tm_sec = second;

  atm.tm_isdst = isdst;

  dm_time = mktime(&atm);
  assert(dm_time != -1);

  // add the difference to the current time if the provided time is gmt
  if (isgmt) {
#ifdef PLATFORM_win32
    dm_time += (mktime(localtime(&dm_time)) - mktime(gmtime(&dm_time)));
#else
    struct tm timmy, t2;
    localtime_r(&dm_time, &timmy);
    gmtime_r(&dm_time, &t2);
    dm_time += (mktime(&timmy) - mktime(&t2));
#endif
  }
}

int timestamp::string_to_month(const std::string& month)
{
  static const char* months[] =
    {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

  for (size_t i=0; i<sizeof(months); i++) {
    if (month == months[i])
      return i;
  }

  return -1;
}

chrono::chrono(void)
{
  reset();
}

void chrono::reset(void)
{
  dm_total.tv_sec = 0;
  dm_total.tv_usec = 0;
}

void chrono::start(void)
{
  ::gettimeofday(&dm_delta, 0);
}

void chrono::stop(void)
{
  struct timeval val;

  val = dm_delta;
  gettimeofday(&dm_delta, 0);

  // modify delta
  dm_delta.tv_sec -= val.tv_sec;
  // see if we need to "borrow"
  if (dm_delta.tv_usec < val.tv_usec) {
    dm_delta.tv_sec -= 1;
    dm_delta.tv_usec += 1000000;
  }
  dm_delta.tv_usec -= val.tv_usec;

  // add to total
  dm_total.tv_sec += dm_delta.tv_sec;
  dm_total.tv_usec += dm_delta.tv_usec;
  if (dm_total.tv_usec >= 1000000) {
    dm_total.tv_sec += 1;
    dm_total.tv_usec -= 1000000;
  }
  assert(dm_total.tv_usec < 1000000);
}

double chrono::get_total_time(void) const
{
  return dm_total.tv_sec + static_cast<double>(dm_total.tv_usec)/1000000;
}

double chrono::get_lap_time(void) const
{
  return dm_delta.tv_sec + static_cast<double>(dm_delta.tv_usec)/1000000;
}

double chrono::get_running_time(void) const
{
  struct timeval rightnow;

  gettimeofday(&rightnow, 0);

  // modify delta
  rightnow.tv_sec -= dm_delta.tv_sec;
  // see if we need to "borrow"
  if (rightnow.tv_usec < dm_delta.tv_usec) {
    rightnow.tv_sec -= 1;
    rightnow.tv_usec += 1000000;
  }
  rightnow.tv_usec -= dm_delta.tv_usec;

  return rightnow.tv_sec + static_cast<double>(rightnow.tv_usec)/1000000;
}

