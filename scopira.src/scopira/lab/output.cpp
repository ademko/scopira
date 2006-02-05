
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

#include <scopira/lab/output.h>

//BBtargets libscopiraui.so

using namespace scopira::tool;
using namespace scopira::lab;

//
// output_window::streamer
//

output_window::streamer::streamer(output_window *parent)
  : dm_parent(parent)
{
  assert(dm_parent);
}

//
//
// output_window
//
//

output_window::output_window(void)
  : dm_streamer(this), dm_flushthread(flush_run, this)
{
  {
    event_ptr<buffer_area> L(dm_buffer);

    L->alive = true;
    L->buffer.reserve(128*1024);
  }

  init_gui();

  dm_flushthread.start();
}

output_window::~output_window()
{
  {
    event_ptr<buffer_area> L(dm_buffer);

    L->alive = false;

    L.notify();
  }

  dm_flushthread.wait_stop();
}

void output_window::popup(const std::string &msg)
{
  count_ptr<output_window> out;

  out = new output_window;
  out->output() << msg;

  out->show_window();
}

void output_window::init_gui(void)
{
  lab_lock LL;

  dm_outputwidget = new scopira::coreui::outputwidget;

  dm_outputwidget->set_output("");

  lab_window::init_gui(dm_outputwidget.get(), button_close_c);

  lab_window::init_title("Output");
}

size_t output_window::buffer_write(const scopira::tool::byte_t *_buf, size_t _size)
{
  if (_size == 0)
    return 0;

  event_ptr<buffer_area> L(dm_buffer);

  if (L->buffer.free() >= _size) {
    lab_lock LL;    // we are doin GUI stuff, after all

    // flush the existing stuff
    dm_outputwidget->append_output(reinterpret_cast<const char *>(L->buffer.begin1()), L->buffer.end1() - L->buffer.end1());
    dm_outputwidget->append_output(reinterpret_cast<const char *>(L->buffer.begin2()), L->buffer.end2() - L->buffer.end2());
    L->buffer.clear();

    // and now write my doozer
    dm_outputwidget->append_output(reinterpret_cast<const char *>(_buf), _size);
    dm_outputwidget->move_to_end();
  } else
    L->buffer.push_back(_buf, _buf + _size);    // fits in the buffer, put it there

  return _size;
}

void* output_window::flush_run(void *dat)
{
  output_window * here = reinterpret_cast<output_window*>(dat);

  assert(here->is_alive_object());

  {
    event_ptr<buffer_area> L(here->dm_buffer);

    if (!L->alive)
      return 0;

    // do the flush
    if (!L->buffer.empty()) {
      lab_lock LL;    // we are doin GUI stuff, after all

      // flush the existing stuff
      here->dm_outputwidget->append_output(reinterpret_cast<const char *>(L->buffer.begin1()), L->buffer.end1() - L->buffer.end1());
      here->dm_outputwidget->append_output(reinterpret_cast<const char *>(L->buffer.begin2()), L->buffer.end2() - L->buffer.end2());
      L->buffer.clear();

      here->dm_outputwidget->move_to_end();
    }

    // and now wait
    L.wait(1000);
  }

  return 0;
}

//
//
// console_window
//
//

console_window * console_window::dm_console_instance;

console_window::console_window(void)
  : dm_outputstack(console_output_func)
{
  assert(dm_console_instance == 0);   // you can only have one console_window open at any time?
  dm_console_instance = this;
}

console_window::~console_window()
{
  assert(dm_console_instance == this);
  dm_console_instance = false;
}

void console_window::popup(void)
{
  (new console_window)->show_window();
}

scopira::tool::oflow_i& console_window::console_output_func(void)
{
  assert(dm_console_instance);
  return dm_console_instance->output();
}

