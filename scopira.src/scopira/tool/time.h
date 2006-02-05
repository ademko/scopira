
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

#ifndef __INCLUDED__SCOPIRA_TOOL_TIME_H__
#define __INCLUDED__SCOPIRA_TOOL_TIME_H__

#include <string>
#include <time.h>
#include <assert.h>

#include <scopira/tool/platform.h>
#include <scopira/tool/export.h>

namespace scopira
{
  namespace tool
  {
    class timespan;
    class timestamp;
    class chrono;
  }
}

/**
 * time span class
 * represents a span of time
 *
 * @author Aleksander Demko
 */
class scopira::tool::timespan
{
  friend class timestamp;
  public:
    /// constructor - inits to null time
    timespan(void) : dm_time(-1) {}
    /// initing ctor
    timespan(time_t t) : dm_time(t) {}
    /// intializes a timespan with hours[0..], minutes[0..59] and seconds[0..59]
    SCOPIRA_EXPORT timespan(int days, int hours, int minutes, int seconds);

    /// checks for null time
    bool is_null(void) const { return dm_time == -1; }

    /// gets the days unit of the timespan
    int get_days(void) const          { return static_cast<int>(dm_time / (24*60*60)); }

    /// gets the hours unit of the timespan
    int get_hours(void) const         { return get_total_hours() % 24; }
    /// gets the total number of hours in the timespan
    int get_total_hours(void) const   { return static_cast<int>(dm_time / (60*60)); }

    /// gets the miutes unit of the timespan
    int get_minutes(void) const       { return get_total_minutes() % 60; }
    /// gets the total number of minutes in the timespan
    int get_total_minutes(void) const { return static_cast<int>(dm_time / 60); }

    /// gets the seconds unit of the timespan
    int get_seconds(void) const       { return static_cast<int>(dm_time % 60); }
    /// gets the total number of seconds in the timespan
    int get_total_seconds(void) const { return static_cast<int>(dm_time); }

    /// comparison operators
    bool operator< (timespan rhs) const { return dm_time <  rhs.dm_time; }
    bool operator<=(timespan rhs) const { return dm_time <= rhs.dm_time; }
    bool operator> (timespan rhs) const { return dm_time >  rhs.dm_time; }
    bool operator>=(timespan rhs) const { return dm_time >= rhs.dm_time; }
    bool operator==(timespan rhs) const { return dm_time == rhs.dm_time; }
    bool operator!=(timespan rhs) const { return dm_time != rhs.dm_time; }

    /// addition and subtraction operators
    timespan  operator+ (timespan rhs) const { return dm_time +  rhs.dm_time; }
    timespan& operator+=(timespan rhs)       { dm_time += rhs.dm_time; return *this; }
    timespan  operator- (timespan rhs) const { return dm_time -  rhs.dm_time; }
    timespan& operator-=(timespan rhs)       { dm_time -= rhs.dm_time; return *this; }

    /// assignment operator
    timespan& operator= (timespan rhs)       { dm_time = rhs.dm_time; return *this; }

  private:
    time_t dm_time;  /// time data represented in seconds
};

/**
 * time class
 *
 * @author Aleksander Demko
 */
class scopira::tool::timestamp
{
  public:
    /// constructors
    timestamp(void) : dm_time(-1) {}
    timestamp(time_t t) : dm_time(t) {}

    /// checks for null time
    bool is_null(void) const { return dm_time == -1; }

    /**
     * constructor
     * initializes a time with the following parameters
     * parameters should be based on local time
     *
     * @param year   1900..
     * @param month  0..11
     * @param day    1..31
     * @param hour   0..23
     * @param minute 0..59
     * @param second 0..59
     * @param dst <0 for default, 0 if dst not in effect, >0 if dst in effect
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT timestamp(int year, int month, int day, int hour, int minute, int second, int dst = -1);

    /**
     * formats the time using a format string
     *
     * @param format a format string (see docs for strftime for format details)
     * @out the formatted time
     * @gmt create a gmt formatted time
     * @author Aleksander Demko
     */
    SCOPIRA_EXPORT void format(const std::string& format, std::string& out, bool gmt=false) const;

    /// gets the current time for the local time zone
    SCOPIRA_EXPORT static timestamp current_time(void);

    /// gets the day of the time value (1-31)
    int get_day(void) const { return get_tm()->tm_mday; }
    /// gets the day of the week of the time value (0-6, 0=Sunday)
    int get_day_of_week(void) const { return get_tm()->tm_wday; }
    /// gets the hour of the local time value (0-23, 0=Sunday)
    int get_hour(void) const { return get_tm()->tm_hour; }
    /// gets the minute of the time value (0-59)
    int get_minute(void) const { return get_tm()->tm_min; }
    /// gets the month of the time value (0-11, 0=January)
    int get_month(void) const { return get_tm()->tm_mon; }
    /// gets the second of the time value (0-59)
    int get_second(void) const { return get_tm()->tm_sec; }
    /// gets the year of the time value (current year minus 1900)
    int get_year(void) const { return get_tm()->tm_year; }

    /// gets the current time value (seconds since January 1, 1970)
    time_t get_time(void) const { return dm_time; }

#ifdef PLATFORM_win32
// disable depreacted warnings
#pragma warning(push)
#pragma warning(disable:4996)
#endif
    /**
     * gets the a tm struct of the local time
     * Get the time data with this method and then get individual
     * data items if more than one time unit is needed.
     *
     * @return the tm struct
     * @author Aleksander Demko
     */
    tm* get_tm(void) const { return localtime(&dm_time); }
#ifdef PLATFORM_win32
// disable depreacted warnings
#pragma warning(pop)
#endif

    /// comparison operators
    bool operator< (timestamp rhs) const { return dm_time <  rhs.dm_time; }
    bool operator<=(timestamp rhs) const { return dm_time <= rhs.dm_time; }
    bool operator> (timestamp rhs) const { return dm_time >  rhs.dm_time; }
    bool operator>=(timestamp rhs) const { return dm_time >= rhs.dm_time; }
    bool operator==(timestamp rhs) const { return dm_time == rhs.dm_time; }
    bool operator!=(timestamp rhs) const { return dm_time != rhs.dm_time; }

    /// addition and subtraction operators
    /// add and subtract timespan objects from time
    timestamp operator+ (timespan rhs) const { return dm_time + rhs.dm_time; }
    timestamp operator+=(timespan rhs)       { dm_time += rhs.dm_time; return *this; }
    timestamp operator- (timespan rhs) const { return dm_time - rhs.dm_time; }
    timestamp operator-=(timespan rhs)       { dm_time -= rhs.dm_time; return *this; }

    /// assignment operators
    timestamp& operator=(timestamp rhs)   { dm_time = rhs.dm_time; return *this; }
    timestamp& operator=(time_t rhs) { dm_time = rhs; return *this; }

    /// subtraction operator to retreive the timespan difference of two times
    timespan operator-(timestamp rhs) const { return dm_time - rhs.dm_time; }

    /// converts a 3 letter string to a numberic month
    static int string_to_month(const std::string& month);

  private:
    void assign(int year, int month, int mday, int hour, int minute, int second, int isdst = -1, bool isgmt = false);

    time_t dm_time;   /// time data represented in seconds
};

/**
 * A stop watch/chrono graph.
 *
 * @author Aleksander Demko
 */ 
class scopira::tool::chrono
{
  // a stop watch
  public:
    /// ctor, total is 0
    SCOPIRA_EXPORT chrono(void);

    /**
     * Resets the stop watch (by zeroing total).
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT void reset(void);

    /// start the stop watch
    SCOPIRA_EXPORT void start(void);

    /**
     * Stops the stop watch (you must have "started" it) sometime.
     * Adds the laptime (time since the last start()) to the "total" time.
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT void stop(void);

    /**
     * Gets the total time, in seconds.
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT double get_total_time(void) const;
    /**
     * Gets the lap top, in seconds.
     * This is the time between the last start()-stop().
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT double get_lap_time(void) const;
    /**
     * Gets the current time, in seconds, since the last start().
     * This does not modify any of the chrono's states.
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT double get_running_time(void) const;

  private:
    struct timeval dm_total, dm_delta; //current total, and current start-delta
};

#endif

