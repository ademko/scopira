
/*
 *  Copyright (c) 2001-2006    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#include <scopira/uikit/seedentry.h>

#include <scopira/tool/util.h>
#include <scopira/coreui/layout.h>
#include <scopira/coreui/label.h>

//BBtargets libscopiraui.so
//BBlibs scopira

using namespace scopira::tool;
using namespace scopira::coreui;

//
//
// seedentry
//
//

scopira::uikit::seedentry::seedentry(void)
{
  count_ptr<grid_layout> g = new grid_layout(2, 2);
  count_ptr<frame> fr = new frame("Random Generator Seed");

  dm_entry = new entry;
  dm_entry->set_text("0");
  dm_entry->set_sensitive(false);
  dm_check = new checkbutton("\"Random\" seed", true);
  dm_check->set_checkbutton_reactor(this);

  g->add_widget(new label("Specific seed:"), 0, 0);
  g->add_widget(dm_entry.get(), 1, 0);
  g->add_widget(dm_check.get(), 1, 1);

  fr->add_widget(g.get());
  widget::init_gui(fr.get());
}

void scopira::uikit::seedentry::react_checkbutton(scopira::coreui::checkbutton *source, bool checkval)
{
  dm_entry->set_sensitive(!checkval);
}

void scopira::uikit::seedentry::set_seed(int s)
{
  dm_check->set_checked(s == 0);
  dm_entry->set_sensitive(s != 0);
  if (s != 0)
    dm_entry->set_text(int_to_string(s));
}

int scopira::uikit::seedentry::get_seed(void) const
{
  if (dm_check->is_checked())
    return 0;
  else
    return string_to_int(dm_entry->get_text());
}
