
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

#ifndef __INCLUDED_SCOPIRA_LAB_OUTPUT_H__
#define __INCLUDED_SCOPIRA_LAB_OUTPUT_H__

#include <scopira/tool/flow.h>
#include <scopira/tool/output.h>
#include <scopira/coreui/output.h>
#include <scopira/lab/window.h>

namespace scopira
{
  namespace lab
  {
    class output_window;
    class console_window;
  }
}

/**
 * A window that you can write output too, and it'll be displayed
 * on the screen.
 *
 * @author Aleksander Demko
 */
class scopira::lab::output_window : public scopira::lab::lab_window
{
  public:
    /**
     * Constructor.
     *
     * @author Aleksander Demko
     */ 
    output_window(void);
    /// destructor
    virtual ~output_window();

    /**
     * Returns the output stream. When you write to this, it gets show
     * to the window. Quite magically.
     *
     * @author Aleksander Demko
     */ 
    scopira::tool::oflow_i &output(void) { return dm_streamer; }

    /**
     * A helper routine that'll pop up the given string in a window
     * and show it.
     *
     * @author Aleksander Demko
     */
    static void popup(const std::string &msg);

  private:
    class streamer : public scopira::tool::oflow_i
    {
      private:
        output_window *dm_parent;
      public:
        streamer(output_window *parent);
        bool failed(void) const { return false; }   // i never fail!
        size_t write(const scopira::tool::byte_t *_buf, size_t _size)
          { return dm_parent->buffer_write(_buf, _size); }
    };

    struct buffer_area
    {
      bool alive;
      scopira::tool::circular_vector<scopira::tool::byte_t> buffer;
    };

    /// the output stream (maybe in the future, itll do input too (how the heck would that work now?)
    streamer dm_streamer;

    /// the shared buffer
    scopira::tool::event_area<buffer_area> dm_buffer;

    /// the thread thatll hit buffer_flush once in awhile
    scopira::tool::thread dm_flushthread;

    /// the output widget itself
    scopira::tool::count_ptr<scopira::coreui::outputwidget> dm_outputwidget;

  private:
    void init_gui(void);

    /// called by streamer to queue data to the buffer (with a possible flush())
    size_t buffer_write(const scopira::tool::byte_t *_buf, size_t _size);

    static void* flush_run(void *dat);
};

/**
 * A specifial version of output_window that also override the default
 * OUTPUT handler.
 *
 * You may only have one console window open at any time.
 *
 * @author Aleksander Demko
 */
class scopira::lab::console_window : public scopira::lab::output_window
{
  public:
    /**
     * Constructor
     *
     * @author Aleksander Demko
     */
    console_window(void);

    /// destructor
    virtual ~console_window();

    /**
     * popups and shows the console window.
     *
     * @author Aleksander Demko
     */
    static void popup(void);

  private:
    /// the console instance
    static console_window *dm_console_instance;

    /// the old one
    scopira::tool::output_stack dm_outputstack;

    /// will be used in gm_tool_output
    static scopira::tool::oflow_i& console_output_func(void);
};

#endif

