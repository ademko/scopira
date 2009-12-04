
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

#ifndef __INCLUDED_SCOPIRA_TOOL_THREAD_H__
#define __INCLUDED_SCOPIRA_TOOL_THREAD_H__

#include <scopira/tool/platform.h>

#ifdef PLATFORM_QTTHREADS
#include <scopira/tool/thread_qtthreads.h>
#else
#include <scopira/tool/thread_pthreads.h>
#endif

#endif

