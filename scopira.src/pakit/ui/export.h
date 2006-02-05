
/*
 *  Copyright (c) 2005    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#ifndef __INCLUDED__PAKIT_UI_EXPORT__
#define __INCLUDED__PAKIT_UI_EXPORT__

#include <scopira/tool/platform.h>
// where SCOPIRA below would be replaced with the "dll" that this file is part of (SCOPIRA, SCOPIRAUI, PAKIT, etc)
#ifdef PAUIKIT_DLL
#define PAUIKIT_EXPORT PLATFORM_INDLL
#else
#define PAUIKIT_EXPORT PLATFORM_OUTDLL
#endif

#endif

