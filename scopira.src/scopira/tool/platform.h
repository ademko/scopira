
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

#ifndef __INCLUDED__SCOPIRA_TOOL_PLATFORM_H__
#define __INCLUDED__SCOPIRA_TOOL_PLATFORM_H__

//
// This header defines platform dependant macros
//
// PLATFORM_64       defined only if 64 bit arch (size(void*) == 8)
// PLATFORM_32       defined only if 32 bit arch (size(void*) == 4)
// PLATFORM_BYTESWAP defined only if byte order is opposite to that of x86
// PLATFORM_UNIX     defined is OS family is UNIXy (all but windows)
// PLATFORM_DEBUG    defined if building a debug set
//
// ONE of the following will always be defined
// PLATFORM_linux    linux on x86 or x86_64 (AMD64)
// PLATFORM_irix     SGI IRIX on MIPS
// PLATFORM_win32    Microsoft Windows on x86 (32-bit only, for now)
// PLATFORM_osx      Apple Macintosh OS X (not yet used)
//

#define SCOPIRA_VERSION "0.9.9"
#define SCOPIRA_VERSION_MAJOR 0
#define SCOPIRA_VERSION_MINOR 9
#define SCOPIRA_VERSION_SUBMINOR 9

#if !defined(PLATFORM_win32) && defined(WIN32)
#define PLATFORM_win32
#endif

#if !defined(PLATFORM_win32) && !defined(PLATFORM_UNIX)
#define PLATFORM_UNIX
#endif

#ifdef PLATFORM_irix
#define PLATFORM_BYTESWAP
#endif

#ifdef BUILDBOSS_32
#define PLATFORM_32
#else
#ifdef BUILDBOSS_64
#define PLATFORM_64
#else
#define PLATFORM_32
#endif
#endif

#ifndef NDEBUG
#define PLATFORM_DEBUG
#endif

#ifndef PLATFORM_DESC
#ifdef PLATFORM_linux
#define PLATFORM_DESC "linux"
#elif defined(PLATFORM_win32)
#define PLATFORM_DESC "ms_win32"
#elif defined(PLATFORM_irix)
#define PLATFORM_DESC "irix"
#elif defined(PLATFORM_osx)
#define PLATFORM_DESC "mac_osx"
#else
#error You must define a valid PLATFORM_
#endif
#endif

#ifdef PLATFORM_win32
#include <process.h>
#include <windows.h>
#else
// have to wait for studio 05 for this to work on win32 too
#include <stdint.h>
#endif

// win32 crap that used to be in win_undef
#ifdef PLATFORM_win32
//comiple error of <limits>
#undef max
#undef min
#define random rand
#define srandom srand
#endif

    // related to http://www.opengroup.org/onlinepubs/009695399/basedefs/stdint.h.html
#ifdef PLATFORM_win32
// make this up until VC supports stdint.h
typedef signed __int8 int8_t;
typedef signed __int16 int16_t;
typedef signed __int32 int32_t;
typedef signed __int64 int64_t;
typedef unsigned __int8 uint8_t;
typedef unsigned __int16 uint16_t;
typedef unsigned __int32 uint32_t;
typedef unsigned __int64 uint64_t;
//change these for when we do 64-bit win32 builds
typedef signed __int32 intptr_t;
typedef unsigned __int32 uintptr_t;
typedef float float32_t;
typedef double float64_t;
#endif

#ifdef PLATFORM_win32
// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/vccelng/htm/msmod_6.asp
// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/vccelng/htm/msmod_20.asp
#define PLATFORM_INDLL __declspec(dllexport)
#define PLATFORM_OUTDLL __declspec(dllimport)
#else
#ifdef PLATFORM_visibility
// perhaps do -fvisibility=hidden and "__attribute__ ((visibility("default")))"
// see: http://www.nedprod.com/programs/gccvisibility.html
//
// dont use this yet (well, you can try but you'll see why its not a nice
// drop in solution yet - ie. im missing typeinfos and vtables because I don't (since I
// can't for win32) do full class marking)
//
#define PLATFORM_INDLL __attribute__ ((visibility("default")))
#define PLATFORM_OUTDLL __attribute__ ((visibility("default")))
#else
#define PLATFORM_INDLL
#define PLATFORM_OUTDLL
#endif
#endif


#endif

