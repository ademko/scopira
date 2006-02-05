
/*
 *  Copyright (c) 2001    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#include <scopira/coreui/app.h>

#include <assert.h>

//BBlibs scopira
//BBtargets libscopiraui.so

using namespace scopira::coreui;

// external static
app_i *app_i::dm_app;

void app_i::set_app(app_i *a)
{
  assert(!a || !dm_app);    // can only toggle from null to not null
  dm_app = a;
}

