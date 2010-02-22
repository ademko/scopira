
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

#include <scopira/qtkit/DynamicSlot.h>

using namespace scopira::qtkit;

DynamicSlot::Handler::~Handler()
{
}

void DynamicSlot::trigger(void)
{
  dm_sender = QObject::sender();
  dm_h->trigger();
  dm_sender = 0;
}

void DynamicSlot::init(void)
{
  dm_sender = 0;
}

