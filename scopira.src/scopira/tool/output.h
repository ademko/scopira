
/*
 *  Copyright (c) 2004-2008    National Research Council
 *
 *  All rights reserved.
 *
 *  This material is confidential and proprietary information of
 *  National Research Council Canada ("Confidential Information").
 *  This Confidential Information may only be used and reproduced
 *  in accordance with the terms of the license agreement.
 *
 */

#ifndef __INCLUDED_SCOPIRA_TOOL_OUTPUT_H__
#define __INCLUDED_SCOPIRA_TOOL_OUTPUT_H__

// we'll include this as a convinience to users that include output.h
#include <scopira/tool/flow.h>
#include <scopira/tool/export.h>

namespace scopira
{
  namespace tool
  {
    /**
     * A function type that returns an output stream
     * @author Aleksander Demko
     */ 
    typedef scopira::tool::oflow_i& (*output_func)(void);

    /**
     * General output string, useful for debug messages.
     *
     * By default, this will be set to _tool_output(). You (well, engines)
     * will often set this to something else.
     *
     * This returns a general, global thread where you can send output
     * to. Under a true engine, this will be engine::get_outflow(), but
     * may be something else under different environments.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT extern scopira::tool::output_func gm_tool_output;

    /**
     * Default tool_output stream handler.
     *
     * This one simply returns gm_tool_output_stream.
     * Since that's initially null, it will also set it if need
     * be (while emitting a warning) just to simplify things.
     *
     * @author Aleksander Demko
     */ 
    scopira::tool::oflow_i& tool_output(void);

    /**
     * The stream pointer used by the default handler, tool_output.
     *
     * You may set this (while leaving the default handler) if you'd like.
     * Take care to clean up whatever instance you place here.
     *
     * @author Aleksander Demko
     */ 
    SCOPIRA_EXPORT extern scopira::tool::oflow_i *gm_tool_output_stream;

    class output_stack;
  }
}

#ifndef OUTPUT
#define OUTPUT (scopira::tool::gm_tool_output())
#endif

/**
 * This is used to push new output parameters to the output system
 * while retaining the old ones.
 *
 * @author Aleksander Demko
 */ 
class scopira::tool::output_stack
{
  private:
    output_func dm_oldfunc;
    oflow_i *dm_oldflow;
  public:
    /**
     * This ctor pushes the stream based output
     * while retaingin the old settings.
     *
     * @author Aleksander Demko
     */
    output_stack(oflow_i *newflow)
      : dm_oldfunc(gm_tool_output), dm_oldflow(gm_tool_output_stream) {
      gm_tool_output = tool_output;
      gm_tool_output_stream = newflow;
    }

    /**
     * This ctor pushes a whole new handler,
     * while retaingin the old settings.
     *
     * @author Aleksander Demko
     */
    output_stack(output_func newfunc)
      : dm_oldfunc(gm_tool_output), dm_oldflow(gm_tool_output_stream) {
      gm_tool_output = newfunc;
      gm_tool_output_stream = 0;
    }

    // dtor does a stack pop, that is, restores the old values
    ~output_stack() {
      gm_tool_output = dm_oldfunc;
      gm_tool_output_stream = dm_oldflow;
    }
};

/**
  \page scopiratooloutput Debugging output

  In scopira/tool/output.h is defined OUTPUT, a basic stream
  that is almost always available. It is useful for debugging
  and other output.
  
  This stream is of type scopira::tool::oflow_i. You may also define
  your own << operators for this type. For examples, please see tool/flow.h

  \code
  #include <scopria/tool/output.h>
  int main(void)
  {
    OUTPUT << "Hello, world.\n";
  }
  \endcode

*/

#endif

