
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


//BBlibs scopiraui pauikit
//BBtargets scopirapa.exe

#include <scopira/tool/platform.h>
#include <scopira/core/sidekickthread.h>
#include <scopira/coreui/loop.h>

#include <pakit/app/project.h>

/**
 * DEPRECATED. FLAGGED FOR DELETION.
 * @author Aleksander Demko
 */
int main(int argc, char **argv)
{
  scopira::coreui::ui_loop looper(argc, argv);
  scopira::core::sidekick_thread thr;  //optional, ofcourse, but oh so powerful
  
  pakit::main_window *mainwin = new pakit::main_window;
  
  mainwin->show_all();

  if (looper.has_config("project"))
    mainwin->load_project(looper.get_config("project"));

  // do the gui stuff
  looper.run_gui();

  return 0;
}

