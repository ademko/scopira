
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

// should this be in scopira::tool?

// older stuff
//#include <functional>

//using namespace std::tr1;
//using namespace std::tr1::placeholders;

// linux: import the tr1 namespace to just std (for shared_ptr)

#include <tr1/memory>

// fold the tr1 namespace into std
namespace std
{
  using namespace std::tr1;
}

#endif

