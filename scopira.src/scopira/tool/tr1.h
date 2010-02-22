
/*
 *  Copyright (c) 2010    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#ifndef __INCLUDED_SCOPIRA_TOOL_TR1_H__
#define __INCLUDED_SCOPIRA_TOOL_TR1_H__

/*

   This header imports some usefil stuff from TR1
   and puts them in the std namespace.

   This is done to iron out some inconsistancies
   on TR1 handling between VC++ and GNU C++

*/

#include <scopira/tool/platform.h>

#ifdef PLATFORM_win32
#include <memory>
#include <functional>
#else
#include <tr1/memory>
#include <tr1/functional>
#endif

// fold the tr1 namespace into std
namespace std
{
  using namespace std::tr1;
  using namespace std::tr1::placeholders;
}

#endif

