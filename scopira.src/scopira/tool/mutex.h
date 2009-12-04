
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

#ifndef __INCLUDED_SCOPIRA_TOOL_MUTEX_H__
#define __INCLUDED_SCOPIRA_TOOL_MUTEX_H__

#include <scopira/tool/platform.h>

#ifdef PLATFORM_QTTHREADS
#include <scopira/tool/mutex_qtthreads.h>
#else
#include <scopira/tool/mutex_pthreads.h>
#endif

#endif

