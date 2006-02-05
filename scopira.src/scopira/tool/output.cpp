
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

#include <scopira/tool/output.h>

#include <scopira/tool/fileflow.h>

//BBtargets libscopira.so

scopira::tool::oflow_i& scopira::tool::tool_output(void)
{
  if (!gm_tool_output_stream) {
    gm_tool_output_stream = new scopira::tool::fileflow(fileflow::stdout_c, 0);
    *gm_tool_output_stream << "autoinit: default flow for OUTPUT.\n";
  }

  return *gm_tool_output_stream;
}

// output.h things
scopira::tool::output_func scopira::tool::gm_tool_output = scopira::tool::tool_output;
scopira::tool::oflow_i * scopira::tool::gm_tool_output_stream;
