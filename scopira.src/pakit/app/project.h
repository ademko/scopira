
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

#ifndef __INCLUDED_PAKIT_APP_PROJECT_H__
#define __INCLUDED_PAKIT_APP_PROJECT_H__

#include <scopira/coreui/project.h>

#include <pakit/ui/models.h>

namespace pakit
{
  class main_window;
}

class pakit::main_window : public scopira::coreui::project_window_base
{
  public:
    /// ctor
    main_window(patterns_project_m *prj = 0);

    virtual project_window_base * spawn(scopira::core::project_i *prj);

  private:
    scopira::tool::count_ptr<patterns_project_m> dm_prj;

  private:
    void init_gui(void);
};

#endif

