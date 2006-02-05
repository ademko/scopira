
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

#include <scopira/core/view.h>

#include <scopira/core/register.h>

//BBtargets libscopira.so

using namespace scopira::tool;
using namespace scopira::core;

//
//
// view_i
//
//



view_i::view_i(void)
{
  dm_action_handler = 0;
}

void view_i::bind_model(scopira::core::model_i *sus)
{
}

void view_i::react_model_update(scopira::core::model_i *sus, scopira::core::view_i *src)
{
}

void view_i::set_view_action_reactor(view_action_reactor_i *vh)
{
  dm_action_handler = vh;

  if (!dm_pretitle.empty()) {
    // empty our already-called title changes
    dm_action_handler->react_view_set_title(this, dm_pretitle);
    dm_pretitle.clear();
  }
}

void view_i::react_view_action(int actionID)
{
  if (dm_action_handler)
    dm_action_handler->react_view_action(this, actionID);
}

void view_i::set_view_title(const std::string &newtitle)
{
  if (dm_action_handler)
    dm_action_handler->react_view_set_title(this, newtitle);
  else
    dm_pretitle = newtitle; // we dont have a listener, so lets just save this title for when we do
}

//
// registration
//

static scopira::core::register_object<view_i> f1(view_type_c, 0, 1);

