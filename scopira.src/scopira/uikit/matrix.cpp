
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

#define WANT_UIKIT_MATRIX_MACROS
#include <scopira/uikit/matrix.h>

#include <algorithm>

#include <scopira/tool/util.h>
#include <scopira/tool/random.h>
#include <scopira/tool/linconrandom.h>
#include <scopira/tool/distrandom.h>
#include <scopira/basekit/math.h>
#include <scopira/basekit/vectormath.h>
#include <scopira/core/register.h>
#include <scopira/coreui/label.h>

//BBtargets libscopiraui.so

using namespace scopira::tool;
using namespace scopira::basekit;
using namespace scopira::coreui;
using namespace scopira::uikit;

//
//
// register stuff
//
//

//static scopira::core::register_view<matrix_v> r1("scopira::uikit::matrix_v", "scopira::uikit::narray_m<double,2>", "Basic/View");
static scopira::core::register_view<scopira::uikit::vec_viewer_v> r1("scopira::uikit::vec_viewer_v", "scopira::uikit::narray_m<double,1>", "View numeric data");
static scopira::core::register_view<scopira::uikit::int_vec_viewer_v> r2("scopira::uikit::int_vec_viewer_v", "scopira::uikit::narray_m<int,1>", "View numeric data");
static scopira::core::register_view<scopira::uikit::matrix_viewer_v> r4("scopira::uikit::matrix_viewer_v", "scopira::uikit::narray_m<double,2>", "View numeric data");
static scopira::core::register_view<scopira::uikit::char_matrix_viewer_v> r5a("scopira::uikit::char_matrix_viewer_v", "scopira::uikit::narray_m<char,2>", "View numeric data");
static scopira::core::register_view<scopira::uikit::int_matrix_viewer_v> r5("scopira::uikit::int_matrix_viewer_v", "scopira::uikit::narray_m<int,2>", "View numeric data");
static scopira::core::register_view<scopira::uikit::matrix_editor_v> r6("scopira::uikit::matrix_editor_v", "scopira::uikit::narray_m<double,2>", "Edit numeric data");

//
//
// matrix_viewer_base
//
//

matrix_viewer_base::matrix_viewer_base(void)
{
  dm_titleline.reserve(128);
}

void matrix_viewer_base::handle_resize(widget_canvas &v, coord &crd)
{
  if (!is_data_empty()) {
    int pagew, pageh;

    pagew = std::min<int>(get_data_width(), (v.width()-basex_c)/cellw_c);
    pageh = std::min<int>(get_data_height(), (v.height()-basey_c)/cellh_c);

    crd.set_hbounds(0, static_cast<float>(get_data_width()), 0, 1, static_cast<float>(pagew/2), static_cast<float>(pagew));
    crd.set_vbounds(0, static_cast<float>(get_data_height()), 0, 1, static_cast<float>(pageh/2), static_cast<float>(pageh));
  }
}

void matrix_viewer_base::handle_repaint(widget_canvas &v, coord &crd)
{
  paint(v, crd);
}

void matrix_viewer_base::handle_scroll(widget_canvas &v, coord &crd)
{
  paint(v, crd);
}

void matrix_viewer_base::get_data_stat(std::string &out) const
{
  out = "Matrix is ";
  out += int_to_string(get_data_width());
  out += " by ";
  out += int_to_string(get_data_height());
}

void matrix_viewer_base::get_data_header(int x, std::string &out) const
{
  int_to_string(x, out);
}

void matrix_viewer_base::paint_cell(scopira::coreui::widget_canvas &v, scopira::coreui::widget_context &pen,
    int cellx, int celly, int datax, int datay)
{
  std::string s;

  get_data_text(datax, datay, s);

  pen.set_foreground(textcol_c);
  v.draw_text_right(pen, dm_font,
    basex_c + (cellx+1)*cellw_c-4,
    basey_c + celly*cellh_c + (cellh_c/2) - 4,  cellw_c-8,
    s);
}

void matrix_viewer_base::prep_display(void)
{
  if (is_data_empty()) {
    dm_titleline.clear();
    return;
  }

  dm_titleline.clear();

  get_data_stat(dm_titleline);

  assert(!dm_titleline.empty());
}

void matrix_viewer_base::convert_mouse_to_xy(scopira::coreui::widget_canvas &v,
    coord &crd, const scopira::coreui::mouse_event &mevt, int &outx, int &outy,
    bool *xok, bool *yok)
{
  int xmin, xmax, ymin, ymax;

  // get the seclected cell and extremes
  xmin = crd.hval();
  xmax = std::min<int>(get_data_width(), (v.width()-basex_c)/cellw_c + xmin );
  assert(xmax >= xmin);

  ymin = crd.vval();
  ymax = std::min<int>(get_data_height(), (v.height()-basey_c)/cellh_c + ymin );
  assert(ymax >= ymin);

  outx = xmin + (mevt.x-basex_c+10*cellw_c)/cellw_c - 10;
  outy = ymin + (mevt.y-basey_c+10*cellh_c)/cellh_c - 10;

  if (xok)
    *xok = outx>=xmin && outx<xmax;
  if (yok)
    *yok = outy>=ymin && outy<ymax;
}

void matrix_viewer_base::paint(widget_canvas &v, coord &crd)
{
  widget_context pen(v);
  int xmin, xmax, x, y, ymin, ymax;
  std::string s;

  s.reserve(100);

  // clear
  pen.set_foreground(backcol_c);
  v.clear(pen);

  pen.set_foreground(textcol_c);

  // check for errors
  if (is_data_empty()) {
    v.draw_text(pen, dm_font, 2, 2, "Data matrix is empty.");
    return;
  }

  xmin = crd.hval();
  xmax = std::min<int>(get_data_width(), (v.width()-basex_c)/cellw_c + xmin );
  assert(xmax > xmin);

  ymin = crd.vval();
  ymax = std::min<int>(get_data_height(), (v.height()-basey_c)/cellh_c + ymin );
  assert(ymax > ymin);

  // draw rows
  for (y=0; y<(ymax-ymin); ++y) {
    // draw cells
    for (x=0; x<(xmax-xmin); ++x)
      paint_cell(v, pen, x, y, x+xmin, y+ymin);

    v.draw_line(pen, basex_c, basey_c+y*cellh_c, basex_c+(xmax-xmin)*cellw_c, basey_c+y*cellh_c);

    //row headers
    v.draw_text_right(pen, dm_font, basex_c-4,
      basey_c + y*cellh_c + (cellh_c/2) - 4,cellw_c-8,
      tool::int_to_string(y+ymin));
  }
  v.draw_line(pen, basex_c, basey_c+(ymax-ymin)*cellh_c, basex_c+(xmax-xmin)*cellw_c, basey_c+(ymax-ymin)*cellh_c);

  // draw top cell header
  assert(!dm_titleline.empty());
  v.draw_text(pen, dm_font, basex_c, titley_c, dm_titleline.c_str());
  for (x=0; x<(xmax-xmin); ++x) {
    v.draw_line(pen, basex_c + x*cellw_c, basey_c,basex_c + x*cellw_c,basey_c+(ymax-ymin)*cellh_c);
    get_data_header(x+xmin, s);
    v.draw_text(pen, dm_font, basex_c + x*cellw_c + (cellw_c/2) - 2,basey_c -12, s.c_str());
  }
  v.draw_line(pen, basex_c + (xmax-xmin)*cellw_c, basey_c, basex_c + (xmax-xmin)*cellw_c,
    basey_c+(ymax-ymin)*cellh_c);
}

//
//
// stringvec_viewer_v
//
//

stringvec_viewer_v::stringvec_viewer_v(void)
{
  init_gui();
  set_view_title("String Vector");
}

int stringvec_viewer_v::get_data_width(void) const
{
  if (dm_data.is_null())
    return 0;
  return 10;
}

int stringvec_viewer_v::get_data_height(void) const
{
  if (dm_data.is_null())
    return 0;
  return dm_data->size()/10 + (dm_data->size()%10 == 0?0:1);
}

void stringvec_viewer_v::get_data_text(int x, int y, std::string &out) const
{
  int idx = y*10+x;

  assert(idx>=0);
  if (idx>=dm_data->size()) {
    out = "N/A";
    return;
  }
  out = dm_data->get(idx);
}

void stringvec_viewer_v::get_data_stat(std::string &out) const
{
  out = "  NumVal=";
  out += tool::int_to_string(dm_data->size());
}

//
//
// vec_viewer_v
//
//

vec_viewer_v::vec_viewer_v(void)
  : dm_model(this)
{
  init_gui();
  set_view_title("Vector");
}

void vec_viewer_v::set_vector(narray_o<double,1> *indata)
{
  dm_data = indata;

  prep_display();

  request_resize();
  request_redraw();
}

void vec_viewer_v::bind_model(scopira::core::model_i *sus)
{
  dm_model = dynamic_cast<scopira::uikit::narray_m<double,1> *>(sus);
}

void vec_viewer_v::react_model_update(scopira::core::model_i *sus, scopira::core::view_i *src)
{
  if (dm_model.get()) {
    dm_data = dm_model->pm_array;

    prep_display();

    request_resize();
    request_redraw();
  }
}

int vec_viewer_v::get_data_width(void) const
{
  if (dm_data.is_null())
    return 0;
  return 10;
}

int vec_viewer_v::get_data_height(void) const
{
  if (dm_data.is_null())
    return 0;
  return dm_data->size()/10 + (dm_data->size()%10 == 0?0:1);
}

void vec_viewer_v::get_data_text(int x, int y, std::string &out) const
{
  int idx = y*10+x;

  assert(idx>=0);
  if (idx>=dm_data->size()) {
    out = "N/A";
    return;
  }
  double_to_string(dm_data->get(idx), out);
}

void vec_viewer_v::get_data_stat(std::string &out) const
{
  size_t x;
  double d;

  out = "  NumVal=";
  out += tool::int_to_string(dm_data->size());

  if (dm_data->empty())
    return;

  x = min(*dm_data, d);
  out += "  MinVal=";
  out += tool::double_to_string(d) + " @" + tool::int_to_string(x);
  x = max(*dm_data, d);
  out += "  MaxVal=";
  out += tool::double_to_string(d) + " @" + tool::int_to_string(x);
  sum(*dm_data, d);
  out += "  Sum=";
  out += tool::double_to_string(d);
  mean(*dm_data, d);
  out += "  Mean=";
  out += tool::double_to_string(d);
}

//
//
// int_vec_viewer_v
//
//

int_vec_viewer_v::int_vec_viewer_v(void)
  : dm_model(this)
{
  init_gui();
  set_view_title("Integer Vector");
}

void int_vec_viewer_v::bind_model(scopira::core::model_i *sus)
{
  dm_model = dynamic_cast<narray_m<int,1> *>(sus);
}

void int_vec_viewer_v::react_model_update(scopira::core::model_i *sus, scopira::core::view_i *src)
{
  if (dm_model.get()) {
    dm_data = dm_model->pm_array;

    prep_display();

    request_resize();
    request_redraw();
  }
}

int int_vec_viewer_v::get_data_width(void) const
{
  if (dm_data.is_null())
    return 0;
  return 10;
}

int int_vec_viewer_v::get_data_height(void) const
{
  if (dm_data.is_null())
    return 0;
  return dm_data->size()/10 + (dm_data->size()%10 == 0?0:1);
}

void int_vec_viewer_v::get_data_text(int x, int y, std::string &out) const
{
  int idx = y*10+x;

  assert(idx>=0);
  if (idx>=dm_data->size()) {
    out = "N/A";
    return;
  }
  int_to_string(dm_data->get(idx), out);
}

void int_vec_viewer_v::get_data_stat(std::string &out) const
{
  size_t x;
  double d;

  out = "  NumVal=";
  out += tool::int_to_string(dm_data->size());

  if (dm_data->empty())
    return;

  x = min(*dm_data, d);
  out += "  MinVal=";
  out += tool::double_to_string(d) + " @" + tool::int_to_string(x);
  x = max(*dm_data, d);
  out += "  MaxVal=";
  out += tool::double_to_string(d) + " @" + tool::int_to_string(x);
  sum(*dm_data, d);
  out += "  Sum=";
  out += tool::double_to_string(d);
  mean(*dm_data, d);
  out += "  Mean=";
  out += tool::double_to_string(d);
}

//
//
// matrix_viewer_v
//
//

matrix_viewer_v::matrix_viewer_v(void)
  : dm_model(this)
{
  init_gui();
  set_view_title("Matrix");
}

void matrix_viewer_v::set_matrix(narray_o<double,2> *indata)
{
  dm_data = indata;

  prep_display();

  request_resize();
  request_redraw();
}

void matrix_viewer_v::bind_model(scopira::core::model_i *sus)
{
  dm_model = dynamic_cast<narray_m<double,2> *>(sus);
}

void matrix_viewer_v::react_model_update(scopira::core::model_i *sus, scopira::core::view_i *src)
{
  if (dm_model.get()) {
    dm_data = dm_model->pm_array;

    prep_display();

    request_resize();
    request_redraw();
  }
}

int matrix_viewer_v::get_data_width(void) const
{
  if (dm_data.is_null())
    return 0;
  return dm_data->width();
}

int matrix_viewer_v::get_data_height(void) const
{
  if (dm_data.is_null())
    return 0;
  return dm_data->height();
}

void matrix_viewer_v::get_data_text(int x, int y, std::string &out) const
{
  double_to_string(dm_data->get(x, y), out);
}

void matrix_viewer_v::get_data_stat(std::string &out) const
{
  size_t minx, miny, maxx, maxy, x, y;
  double minval, maxval;
  double d;
  int c;
  narray<double,2>::iterator ii, endii;

  out = "Width=";
  out += tool::int_to_string(dm_data->width());
  out += "  Height=";
  out += tool::int_to_string(dm_data->height());
  out += "  NumVal=";
  out += tool::int_to_string(dm_data->size());

  if (dm_data->empty())
    return;

  minx = miny = maxx = maxy = 0;
  maxval = minval = dm_data->get(0,0);
  for (y=0; y<dm_data->height(); ++y)
    for (x=0; x<dm_data->width(); ++x) {
      if (dm_data->get(x,y) < minval) {
        minval = dm_data->get(x,y);
        minx = x;
        miny = y;
      }
      if (dm_data->get(x,y) > minval) {
        maxval = dm_data->get(x,y);
        maxx = x;
        maxy = y;
      }
    }

  out += "  MinVal=";
  out += tool::double_to_string(minval) + " @" + tool::int_to_string(minx)
    + "," + tool::int_to_string(miny);
  out += "  MaxVal=";
  out += tool::double_to_string(maxval) + " @" + tool::int_to_string(maxx)
    + "," + tool::int_to_string(maxy);

  sum(*dm_data, d);
  out += "  Sum=";
  out += tool::double_to_string(d);
  mean(*dm_data, d);
  out += "  Mean=";
  out += tool::double_to_string(d);

  c = 0;
  endii = dm_data->end();
  for (ii=dm_data->begin(); ii != endii; ++ii)
    if (is_zero(*ii))
      c++;
  out += "  NumZero=";
  out += tool::int_to_string(c);
}

//
//
// char_matrix_viewer_v
//
//

char_matrix_viewer_v::char_matrix_viewer_v(void)
  : dm_model(this)
{
  init_gui();
  set_view_title("Integer Vector");
}

void char_matrix_viewer_v::set_matrix(narray_o<char,2> *indata)
{
  dm_data = indata;

  prep_display();

  request_resize();
  request_redraw();
}

void char_matrix_viewer_v::bind_model(scopira::core::model_i *sus)
{
  dm_model = dynamic_cast<narray_m<char,2> *>(sus);
}

void char_matrix_viewer_v::react_model_update(scopira::core::model_i *sus, scopira::core::view_i *src)
{
  if (dm_model.get()) {
    dm_data = dm_model->pm_array;

    prep_display();

    request_resize();
    request_redraw();
  }
}

int char_matrix_viewer_v::get_data_width(void) const
{
  if (dm_data.is_null())
    return 0;
  return dm_data->width();
}

int char_matrix_viewer_v::get_data_height(void) const
{
  if (dm_data.is_null())
    return 0;
  return dm_data->height();
}

void char_matrix_viewer_v::get_data_text(int x, int y, std::string &out) const
{
  int_to_string(dm_data->get(x, y), out);
}

void char_matrix_viewer_v::get_data_stat(std::string &out) const
{
  size_t minx, miny, maxx, maxy, x, y;
  int minval, maxval;
  int d;
  int c;
  narray_o<char,2>::iterator ii, endii;

  out = "Width=";
  out += tool::int_to_string(dm_data->width());
  out += "  Height=";
  out += tool::int_to_string(dm_data->height());
  out += "  NumVal=";
  out += tool::int_to_string(dm_data->size());

  if (dm_data->empty())
    return;

  minx = miny = maxx = maxy = 0;
  maxval = minval = dm_data->get(0,0);
  for (y=0; y<dm_data->height(); ++y)
    for (x=0; x<dm_data->width(); ++x) {
      if (dm_data->get(x,y) < minval) {
        minval = dm_data->get(x,y);
        minx = x;
        miny = y;
      }
      if (dm_data->get(x,y) > minval) {
        maxval = dm_data->get(x,y);
        maxx = x;
        maxy = y;
      }
    }

  out += "  MinVal=";
  out += tool::int_to_string(minval) + " @" + tool::int_to_string(minx)
    + "," + tool::int_to_string(miny);
  out += "  MaxVal=";
  out += tool::int_to_string(maxval) + " @" + tool::int_to_string(maxx)
    + "," + tool::int_to_string(maxy);

  sum(*dm_data, d);
  out += "  Sum=";
  out += tool::int_to_string(d);
  mean(*dm_data, d);
  out += "  Mean=";
  out += tool::int_to_string(d);

  c = 0;
  endii = dm_data->end();
  for (ii=dm_data->begin(); ii != endii; ++ii)
    if (*ii == 0)
      c++;
  out += "  NumZero=";
  out += tool::int_to_string(c);
}

//
//
// int_matrix_viewer_v
//
//

int_matrix_viewer_v::int_matrix_viewer_v(void)
  : dm_model(this)
{
  init_gui();
  set_view_title("Integer Vector");
}

void int_matrix_viewer_v::set_matrix(narray_o<int,2> *indata)
{
  dm_data = indata;

  prep_display();

  request_resize();
  request_redraw();
}

void int_matrix_viewer_v::bind_model(scopira::core::model_i *sus)
{
  dm_model = dynamic_cast<narray_m<int,2> *>(sus);
}

void int_matrix_viewer_v::react_model_update(scopira::core::model_i *sus, scopira::core::view_i *src)
{
  if (dm_model.get()) {
    dm_data = dm_model->pm_array;

    prep_display();

    request_resize();
    request_redraw();
  }
}

int int_matrix_viewer_v::get_data_width(void) const
{
  if (dm_data.is_null())
    return 0;
  return dm_data->width();
}

int int_matrix_viewer_v::get_data_height(void) const
{
  if (dm_data.is_null())
    return 0;
  return dm_data->height();
}

void int_matrix_viewer_v::get_data_text(int x, int y, std::string &out) const
{
  int_to_string(dm_data->get(x, y), out);
}

void int_matrix_viewer_v::get_data_stat(std::string &out) const
{
  size_t minx, miny, maxx, maxy, x, y;
  int minval, maxval;
  int d;
  int c;
  narray_o<int,2>::iterator ii, endii;

  out = "Width=";
  out += tool::int_to_string(dm_data->width());
  out += "  Height=";
  out += tool::int_to_string(dm_data->height());
  out += "  NumVal=";
  out += tool::int_to_string(dm_data->size());

  if (dm_data->empty())
    return;

  minx = miny = maxx = maxy = 0;
  maxval = minval = dm_data->get(0,0);
  for (y=0; y<dm_data->height(); ++y)
    for (x=0; x<dm_data->width(); ++x) {
      if (dm_data->get(x,y) < minval) {
        minval = dm_data->get(x,y);
        minx = x;
        miny = y;
      }
      if (dm_data->get(x,y) > minval) {
        maxval = dm_data->get(x,y);
        maxx = x;
        maxy = y;
      }
    }

  out += "  MinVal=";
  out += tool::int_to_string(minval) + " @" + tool::int_to_string(minx)
    + "," + tool::int_to_string(miny);
  out += "  MaxVal=";
  out += tool::int_to_string(maxval) + " @" + tool::int_to_string(maxx)
    + "," + tool::int_to_string(maxy);

  sum(*dm_data, d);
  out += "  Sum=";
  out += tool::int_to_string(d);
  mean(*dm_data, d);
  out += "  Mean=";
  out += tool::int_to_string(d);

  c = 0;
  endii = dm_data->end();
  for (ii=dm_data->begin(); ii != endii; ++ii)
    if (*ii == 0)
      c++;
  out += "  NumZero=";
  out += tool::int_to_string(c);
}

//
//
// matrix_editor_base::parser
//
//

/**
 * parser of string commands
 * @author Aleksander Demko
 */
class matrix_editor_base::parser
{
  public:
    /// ctor
    parser(cmd_t *cmds, matrix_editor_base *here) : dm_here(here) {
      if (cmds)
        dm_commands = cmds;
      else
        dm_commands = &dm_emptylist;
    }

    /// parser
    void parse(const char *s);

    /// executioner, returns an error, if any
    /// changedsel will be set to either or not sel was changed
    const char * execute(matrix_editor_base *mother, bool &changedsel, int &numdone);

  public:
    fixed_array<tok_t, 20> dm_tokens;  //0-length token string terminated

  private:
    static cmd_t dm_emptylist;
    cmd_t *dm_commands;
    matrix_editor_base *dm_here;

    const char * next_token(const char *c, std::string &buf);
};

matrix_editor_base::cmd_t matrix_editor_base::parser::dm_emptylist = {0, 0, 0};

//
//
// matrix_editor_base
//
//

matrix_editor_base::matrix_editor_base(cmd_t *commands, matrix_editor_base *here)
  : dm_menu(this)
{
  dm_ycur = -1;
  dm_xcur = -1;
  dm_xsel = -1;
  dm_ysel = -1;

  dm_commands = commands;
  dm_here = here;
  assert(here->is_alive_object());
}

void matrix_editor_base::handle_menu(scopira::coreui::widget_canvas &v, coord &crd, intptr_t menuid)
{
  const char *cmd = int_to_ptr<const char>(menuid);

  assert(cmd);
  switch (*cmd) {
    case 'E':
      //EXECUTE!
      do_execute();
      break;
    case 'U':
      //UNDO!
      undo();
      break;
    case 'X':
    case 'Y':
      // explciti set
      dm_edit->set_text(cmd+1);
      if (*cmd == 'Y')
        do_execute();
      break;
    case 'P':
      // prepend
      {
        std::string buf(cmd+1);

        buf += dm_edit->get_text();
        // prepend
        dm_edit->set_text(buf);
      }
      break;
  }
}

void matrix_editor_base::handle_press(scopira::coreui::widget_canvas &v, coord &crd, const scopira::coreui::mouse_event &mevt)
{
  int xmin, xmax, x, y, ymin, ymax, xold, yold;

  if (mevt.double_press || !mevt.press)
    return;   // i only want non-double press events, thanks

  // popup the menu on right clicks
  if (mevt.button == 2) {
    dm_menu.popup();
    return;
  }

  // get the seclected cell and extremes
  xmin = crd.hval();
  xmax = std::min<int>(dm_sel.width(), (v.width()-basex_c)/cellw_c + xmin );
  assert(xmax >= xmin);

  ymin = crd.vval();
  ymax = std::min<int>(dm_sel.height(), (v.height()-basey_c)/cellh_c + ymin );
  assert(ymax >= ymin);

  x = xmin + (mevt.x-basex_c+10*cellw_c)/cellw_c - 10;
  y = ymin + (mevt.y-basey_c+10*cellh_c)/cellh_c - 10;

  // in range?
  if (x>=xmin && x<xmax && y>=ymin && y<ymax)
    ;//do nothing
  else if (x<xmin && y<ymin) {
    // select all check
    x = (dm_xsel== -1) ? 0 : dm_sel.width()-1;
    y = (dm_xsel== -1) ? 0 : dm_sel.height()-1;
  } else if (y<ymin && x>=xmin && x<xmax)
    y = (dm_xsel== -1) ? 0 : dm_sel.height()-1;
  else if (x<xmin && y>=ymin && y<ymax)
    x = (dm_xsel== -1) ? 0 : dm_sel.width()-1;
  else
    return;//out of range then

  // check for middle button (cursor movement)
  // but not second part of region select
  if (mevt.button == 0 && dm_xsel == -1) {
    int oldx, oldy;
    oldx = dm_xcur;
    oldy = dm_ycur;
    if (x == dm_xcur && y == dm_ycur)
      clear_cursor();
    else {
      set_cursor(x, y);
      edit_cursor();
    }
    if (oldx != dm_xcur || oldy != dm_ycur) {
      widget_context pen(v);
      if (oldx != -1)
        paint_cell(v, pen, oldx - xmin, oldy - ymin, oldx, oldy);
      if (dm_xcur != -1)
        paint_cell(v, pen, dm_xcur - xmin, dm_ycur - ymin, dm_xcur, dm_ycur);
    }
    return;
  }

  if (dm_xsel == -1) {
    // first hop selection
    dm_xsel = x;
    dm_ysel = y;

    // redraw just the cell
    widget_context pen(v);
    paint_cell(v, pen, x - xmin, y - ymin, x, y);
    return;
  }

  // 2nd select. do some work
  xold = dm_xsel;
  yold = dm_ysel;
  dm_xsel = -1;
  dm_ysel = -1;

  // make x the base x (minimum)
  if (xold < x)
    std::swap(x, xold);
  if (yold < y)
    std::swap(y, yold);
  // make xold the width
  xold = xold - x + 1;
  yold = yold - y + 1;
  // call the handler
  handle_mouse_rect(mevt.button == 1, x, y, xold, yold);
}

void matrix_editor_base::react_entry(entry *source, const char *msg)
{
  do_execute();
}

void matrix_editor_base::prepare_undo(void)
{
  dm_undosel.copy(dm_sel);
}

void matrix_editor_base::undo(void)
{
  dm_sel.copy(dm_undosel);
  resync(true);
  clear_cursor();
}

void matrix_editor_base::do_execute(void)
{
  // execute command method
  parser p(dm_commands, dm_here);
  std::string err;
  const char *retmsg;
  int numdone;
  bool changedsel;

  dm_edit->get_text(err);
  if (err.empty())
    return; // empty string

  // save undo info
  prepare_undo();

  p.parse(err.c_str());
  retmsg = p.execute(this, changedsel, numdone);

  if (changedsel)
    clear_cursor();

  /*if (retmsg)
    set_error_label(retmsg);
  else
    clear_error_label();*/

  if (retmsg) {
    // on error, undo if we did anything
    if (numdone>0)
      undo(); // eeewwww
  } else {
    // ok
    // clear command box
    if (valid_cursor())
      edit_cursor();
    else
      dm_edit->set_text(0);
    // redraw screen
    resync(true); // all the time? this needed?
  }
}

void matrix_editor_base::resync(bool newsize)
{
  prep_display();
  if (newsize)
    request_resize();
  request_redraw();
}

void matrix_editor_base::paint_cell(scopira::coreui::widget_canvas &v, scopira::coreui::widget_context &pen,
    int cellx, int celly, int datax, int datay)
{
  int c;

  if (dm_xsel == datax && dm_ysel == datay)
    c = h_mark_col_c;
  else if (datax == dm_xcur && datay == dm_ycur)
    c = h_cur_col_c;
  else if (dm_sel.get(datax, datay))
    c = h_sel_col_c;
  else
    c = backcol_c;

  pen.set_foreground(c);
  v.draw_rectangle(pen, true, basex_c + cellx*cellw_c+1,
      basey_c + celly*cellh_c+1, cellw_c-1, cellh_c-1);

  parent_type::paint_cell(v, pen, cellx, celly, datax, datay);
}

void matrix_editor_base::init_gui(void)
{
  count_ptr<box_layout> box;
  count_ptr<button_layout> butbox;

  dm_menu.add_item("Execute", ptr_to_int("E"));   //ugh, cast
  dm_menu.add_item("Clear", ptr_to_int("X"));   //ugh, cast
  dm_menu.add_item("Undo", ptr_to_int("U"));   //ugh, cast
  dm_menu.add_separator();

  dm_edit = new entry;
  dm_edit->set_entry_reactor(this);

  box = new box_layout(false, false, 2);
  box->add_widget(make_drawing_area(), true, true, 0);
  box->add_widget(dm_edit.get(), false, true, 0);

  butbox = new button_layout(box.get());
  butbox->add_stock_buttons(button_layout::button_apply_c, this);

  dm_basewidget = butbox.get();
  widget::init_gui(butbox->get_widget());
  set_view_title("Matrix Data Editor");
}

void matrix_editor_base::prep_data(void)
{
  // match the size, default to true
  if (dm_sel.width() != get_data_width() || dm_sel.height() != get_data_height()) {
    dm_sel.resize(get_data_width(), get_data_height());
    dm_sel.set_all(false);
    //reset_cursor();
  }

  prep_display();
  request_resize();
  request_redraw();
}

void matrix_editor_base::handle_mouse_rect(bool immediate, int x, int y, int w, int h)
{
  int myw, myh;

  if (immediate) {
    for (myh=0; myh<h; ++myh)
      for (myw=0; myw<w; ++myw)
        dm_sel(x+myw, y+myh) = !dm_sel(x+myw, y+myh);
    clear_cursor();
    resync(false);
    return;
  }

  // just load the edit box

  std::string buf;
  buf.reserve(32);
  buf = "rect ";
  buf += int_to_string(x);
  buf += ' ';
  buf += int_to_string(y);
  buf += ' ';
  buf += int_to_string(w);
  buf += ' ';
  buf += int_to_string(h);
  buf += ' ';

  dm_edit->set_text(buf);
  resync(false);
}

void matrix_editor_base::reset_cursor(void)
{
  int x, y;

  for (y=0; y<dm_sel.height(); ++y)
    for (x=0; x<dm_sel.width(); ++x)
      if (dm_sel(x,y)) {
        // found a valid first selection
        set_cursor(x, y);
        return;
      }
  // nothing found
  set_cursor(-1, -1);
}

void matrix_editor_base::inc_cursor(void)
{
  int x, y;

  for (y=dm_ycur; y<dm_sel.height(); ++y)
    for (x= (y==dm_ycur?dm_xcur+1:0); x<dm_sel.width(); ++x)
      if (dm_sel(x,y)) {
        // found a valid next selection
        set_cursor(x, y);
        return;
      }
  // reached the end. use reset to try to wrap around
  reset_cursor();
}

void matrix_editor_base::set_cursor(int x, int y)
{
  dm_xcur = x;
  dm_ycur = y;
}

void matrix_editor_base::edit_cursor(void)
{
  // load the current value into the edit box
  std::string s;
  get_data_text(dm_xcur, dm_ycur, s);
  dm_edit->set_text(s);
  dm_edit->select_all();
}

void matrix_editor_base::parm_t::merge_sel(const narray_o<bool,2> &newsel)
{
  assert(newsel.width() == here_base->dm_sel.width());
  assert(newsel.height() == here_base->dm_sel.height());

  changed_sel = true;

  // simple case
  if (selmode == sel_set_c) {
    here_base->dm_sel.copy(newsel);
    return;   // all done!
  }

  narray_o<bool,2>::const_iterator jj;
  narray_o<bool,2>::iterator ii, endii;

  jj = newsel.begin();
  ii = here_base->dm_sel.begin();
  endii = here_base->dm_sel.end();

  switch (selmode) {
    case sel_add_c:
      while (ii != endii) {
        if (*jj)
          *ii = true;
        ++ii;
        ++jj;
      }
      break;
    case sel_sub_c:
      while (ii != endii) {
        if (*jj)
          *ii = false;
        ++ii;
        ++jj;
      }
      break;
    default: //inter_c
      while (ii != endii) {
        *ii = *ii && *jj;
        ++ii;
        ++jj;
      }
      break;
  }
  // reset the selection mode
  selmode = sel_set_c;
}

void matrix_editor_base::parm_t::apply_op(double src, double &out)
{
  switch (opmode) {
    case op_add_c: out += src; break;
    case op_sub_c: out -= src; break;
    case op_mult_c: out *= src; break;
    case op_div_c: if (!is_zero(src)) out /= src; break;
    case op_idiv_c: if (!is_zero(src)) out = static_cast<int>(out)/static_cast<int>(src); break;
    case op_imod_c: if (!is_zero(src)) out = static_cast<int>(out)%static_cast<int>(src); break;
    default: out = src; break;
  }
}

void matrix_editor_base::parm_t::apply_op(int src, int &out)
{
  switch (opmode) {
    case op_add_c: out += src; break;
    case op_sub_c: out -= src; break;
    case op_mult_c: out *= src; break;
    case op_div_c: if (!is_zero(src)) out /= src; break;
    case op_idiv_c: if (!is_zero(src)) out = static_cast<int>(out)/static_cast<int>(src); break;
    case op_imod_c: if (!is_zero(src)) out = static_cast<int>(out)%static_cast<int>(src); break;
    default: out = src; break;
  }
}

void matrix_editor_base::parser::parse(const char *s)
{
  const char *c = s;
  tok_t *cur, *end;
  std::string buf;

  buf.reserve(tok_t::name_t::size_c - 1);

  // reset
  dm_tokens.clear_zero();

  cur = dm_tokens.begin();
  end = dm_tokens.end();
  --end;  // one before last... so we always have an end sentinal

  while (*c) {
    c = next_token(c, buf);
    // fill the token
    string_to_fixed_array(buf, cur->name);
    cur->isdouble = string_to_double(buf, cur->dvalue);
    cur->isint = string_to_int(buf, cur->ivalue);
    ++cur;
  }
}

// forward, cuz i need this here
const char * matrix_editor_base::parser::execute(matrix_editor_base *mother, bool &changedsel, int &numdone)
{
  cmd_t *cmd;
  parm_t parm;
  tok_t *cur, *end;

  assert(mother);

  parm.error = 0;
  parm.selmode = parm_t::sel_set_c;
  parm.opmode = parm_t::op_set_c;
  parm.changed_sel = false;
  parm.here_base = dm_here;

  changedsel = false;
  numdone = 0;

  end = cur = dm_tokens.begin();
  while (end->name[0])
    ++end;    // find the end
  // iterate over all the tokens
  while (cur < end) {
    // find the command, if any
    cmd = dm_commands;
    numdone++;    // assume we did something
    while (cmd->keyword)
      if (!strcmp(cmd->keyword, cur->name.c_array()))
        break;
      else
        ++cmd;
    if (cmd->keyword) {
      // found something in the key table
      if ( (cur+cmd->num_parm) >= end )
        return "Not enough parameters given";
      parm.token = cur;

      cmd->func(parm);

      if (parm.changed_sel)
        changedsel = true;
      if (parm.error)
        return parm.error;
      cur += cmd->num_parm;
    } else if (cur->isdouble || cur->name[0] == '"') {
      // incremental set/data entry
      if (mother->valid_cursor()) {
        // cursor increment command
        cmd = dm_commands;
        while (cmd->keyword)
          if (!strcmp(cmd->keyword, "XSETONE"))
            break;
          else
            ++cmd;
        if (cmd->keyword) {
          // we found it!
          assert(cmd->num_parm == 3);
          fixed_array<tok_t, 4> minitok;

          minitok[0].name[0] = 0;//no name :)
          minitok[0].isdouble = false;
          minitok[0].isint = false;
          minitok[1].name[0] = 0;
          minitok[1].isdouble = false;
          minitok[1].isint = true;
          minitok[1].ivalue = mother->dm_xcur;
          minitok[2].name[0] = 0;
          minitok[2].isdouble = false;
          minitok[2].isint = true;
          minitok[2].ivalue = mother->dm_ycur;
          minitok[3] = *cur;
          parm.token = minitok.c_array();

          cmd->func(parm);

          if (parm.changed_sel)
            changedsel = true;
          if (parm.error)
            return parm.error;
          mother->inc_cursor();
        }
      } else {
        // no valid cursor, lets just set all then
        // via a special function
        cmd = dm_commands;
        while (cmd->keyword)
          if (!strcmp(cmd->keyword, "XSETALL"))
            break;
          else
            ++cmd;
        if (cmd->keyword) {
          // we found it!
          assert(cmd->num_parm == 1);
          fixed_array<tok_t, 2> minitok;

          minitok[0].name[0] = 0;//no name :)
          minitok[0].isdouble = false;
          minitok[0].isint = false;
          minitok[1] = *cur;
          parm.token = minitok.c_array();

          cmd->func(parm);

          if (parm.changed_sel)
            changedsel = true;
          if (parm.error)
            return parm.error;
        }
      }
    } else {
      // take back that ++ assumption
      numdone--;
      return "Unkown command";
    }

    ++cur;
  }

  return 0;
}

const char * matrix_editor_base::parser::next_token(const char *c, std::string &buf)
{
  assert(c);

  buf.clear();

  // skip any initial whitespace
  while (*c == ' ')
    ++c;

  if (*c == '"') {
    // do quoted string read
    buf.push_back(*(c++));
    while (*c && *c != '"') {
      if (*c == '\\') {
        ++c;
        if (*c)
          buf.push_back(*(c++));
      } else
        buf.push_back(*(c++));
    }
    if (*c)
      c++;  // i dont actually want to keep the last "
  } else while (*c && *c != ' ')
    buf.push_back(*(c++)); // read all non whitespace

  return c;
}

void matrix_editor_base::cmd_mod_add(parm_t &p)
{
  p.opmode = parm_t::op_add_c;
}

void matrix_editor_base::cmd_mod_sub(parm_t &p)
{
  p.opmode = parm_t::op_sub_c;
}

void matrix_editor_base::cmd_mod_mul(parm_t &p)
{
  p.opmode = parm_t::op_mult_c;
}

void matrix_editor_base::cmd_mod_div(parm_t &p)
{
  p.opmode = parm_t::op_div_c;
}

void matrix_editor_base::cmd_mod_idiv(parm_t &p)
{
  p.opmode = parm_t::op_idiv_c;
}

void matrix_editor_base::cmd_mod_imod(parm_t &p)
{
  p.opmode = parm_t::op_imod_c;
}

void matrix_editor_base::cmd_sel_add(parm_t &p)
{
  p.selmode = parm_t::sel_add_c;
}

void matrix_editor_base::cmd_sel_sub(parm_t &p)
{
  p.selmode = parm_t::sel_sub_c;
}

void matrix_editor_base::cmd_sel_inter(parm_t &p)
{
  p.selmode = parm_t::sel_inter_c;
}

void matrix_editor_base::cmd_sel(parm_t &p)
{
  tok_t *x, *y;

  x = p.token+1;
  y = p.token+2;

  ASSERT_INT(x, "X coordinate must be an integer");
  ASSERT_INT(y, "Y coordinate must be an integer");
  ASSERT_NONNEG_INT(x, "X coordinate must be an positive");
  ASSERT_NONNEG_INT(y, "Y coordinate must be an positive");

  if (x->ivalue >= p.here_base->get_data_width()) {
    p.error = "X coordinate is out of range";
    return;
  }

  if (y->ivalue >= p.here_base->get_data_height()) {
    p.error = "Y coordinate is out of range";
    return;
  }

  narray_o<bool,2> newsel(p.here_base->get_data_width(), p.here_base->get_data_height());

  newsel.set_all( false );
  newsel(x->ivalue, y->ivalue) = true;

  p.merge_sel(newsel);
}

void matrix_editor_base::cmd_sel_rect(parm_t &p)
{
  tok_t *x, *y, *w, *h;

  x = p.token+1;
  y = p.token+2;
  w = p.token+3;
  h = p.token+4;

  ASSERT_INT(x, "X1 coordinate must be an integer");
  ASSERT_INT(y, "Y1 coordinate must be an integer");
  ASSERT_INT(w, "Width must be an integer");
  ASSERT_INT(h, "Height must be an integer");
  ASSERT_NONNEG_INT(x, "X coordinate must be an positive");
  ASSERT_NONNEG_INT(y, "Y coordinate must be an positive");
  ASSERT_POSITIVE_INT(w, "Width must be greater than 0");
  ASSERT_POSITIVE_INT(h, "Height must be greater than 0");

  if (x->ivalue + w->ivalue > p.here_base->dm_sel.width()) {
    p.error = "X and width extend beyond the datum";
    return;
  }

  if (y->ivalue + h->ivalue > p.here_base->dm_sel.height()) {
    p.error = "Y and height extend beyond the datum";
    return;
  }

  narray_o<bool,2> newsel(p.here_base->dm_sel.width(), p.here_base->dm_sel.height());
  int myw, myh;

  newsel.set_all( false );

  for (myh=0; myh<h->ivalue; ++myh)
    for (myw=0; myw<w->ivalue; ++myw)
      newsel(x->ivalue + myw, y->ivalue + myh) = true;

  p.merge_sel(newsel);
}

void matrix_editor_base::cmd_sel_invert(parm_t &p)
{
  int x, y;
  narray_o<bool,2> newsel(p.here_base->dm_sel.width(), p.here_base->dm_sel.height());

  for (y=0; y<newsel.height(); ++y)
    for (x=0; x<newsel.width(); ++x)
      newsel(x, y) = !p.here_base->dm_sel.get(x, y);

  p.merge_sel(newsel);
}

void matrix_editor_base::cmd_sel_all(parm_t &p)
{
  narray_o<bool,2> newsel(p.here_base->dm_sel.width(), p.here_base->dm_sel.height());

  newsel.set_all( true );
  p.merge_sel(newsel);
}

void matrix_editor_base::cmd_sel_none(parm_t &p)
{
  narray_o<bool,2> newsel(p.here_base->dm_sel.width(), p.here_base->dm_sel.height());

  newsel.set_all( false );
  p.merge_sel(newsel);
}

void matrix_editor_base::cmd_equals(parm_t &p)
{
  tok_t *delta;
  int x, y, w, h;
  double val;

  delta = p.token+1;

  w = p.here_base->get_data_width();
  h = p.here_base->get_data_height();

  ASSERT_DOUBLE(delta, "Parameter must be a number");

  narray_o<bool,2> newsel(p.here_base->dm_sel.width(), p.here_base->dm_sel.height());

  newsel.set_all( false );

  for (y=0; y<h; ++y)
    for (x=0; x<w; ++x) {
      if (!p.here_base->get_data_double(x,y,val))
        continue;
      if (is_equal(val, delta->dvalue))
        newsel(x,y) = true;
    }

  p.merge_sel(newsel);
}

void matrix_editor_base::cmd_notequals(parm_t &p)
{
  tok_t *delta;
  int x, y, w, h;
  double val;

  delta = p.token+1;

  w = p.here_base->get_data_width();
  h = p.here_base->get_data_height();

  ASSERT_DOUBLE(delta, "Parameter must be a number");

  narray_o<bool,2> newsel(p.here_base->dm_sel.width(), p.here_base->dm_sel.height());

  newsel.set_all( false );

  for (y=0; y<h; ++y)
    for (x=0; x<w; ++x) {
      if (!p.here_base->get_data_double(x,y,val))
        continue;
      if (!is_equal(val, delta->dvalue))
        newsel(x,y) = true;
    }

  p.merge_sel(newsel);
}

void matrix_editor_base::cmd_greaterthan(parm_t &p)
{
  tok_t *delta;
  int x, y, w, h;
  double val;

  delta = p.token+1;

  w = p.here_base->get_data_width();
  h = p.here_base->get_data_height();

  ASSERT_DOUBLE(delta, "Parameter must be a number");

  narray_o<bool,2> newsel(p.here_base->dm_sel.width(), p.here_base->dm_sel.height());

  newsel.set_all( false );

  for (y=0; y<h; ++y)
    for (x=0; x<w; ++x) {
      if (!p.here_base->get_data_double(x,y,val))
        continue;
      if (val > delta->dvalue)
        newsel(x,y) = true;
    }

  p.merge_sel(newsel);
}

void matrix_editor_base::cmd_greaterthan_equal(parm_t &p)
{
  tok_t *delta;
  int x, y, w, h;
  double val;

  delta = p.token+1;

  w = p.here_base->get_data_width();
  h = p.here_base->get_data_height();

  ASSERT_DOUBLE(delta, "Parameter must be a number");

  narray_o<bool,2> newsel(p.here_base->dm_sel.width(), p.here_base->dm_sel.height());

  newsel.set_all( false );

  for (y=0; y<h; ++y)
    for (x=0; x<w; ++x) {
      if (!p.here_base->get_data_double(x,y,val))
        continue;
      if (val >= delta->dvalue)
        newsel(x,y) = true;
    }

  p.merge_sel(newsel);
}

void matrix_editor_base::cmd_lessthan(parm_t &p)
{
  tok_t *delta;
  int x, y, w, h;
  double val;

  delta = p.token+1;

  w = p.here_base->get_data_width();
  h = p.here_base->get_data_height();

  ASSERT_DOUBLE(delta, "Parameter must be a number");

  narray_o<bool,2> newsel(p.here_base->dm_sel.width(), p.here_base->dm_sel.height());

  newsel.set_all( false );

  for (y=0; y<h; ++y)
    for (x=0; x<w; ++x) {
      if (!p.here_base->get_data_double(x,y,val))
        continue;
      if (val < delta->dvalue)
        newsel(x,y) = true;
    }

  p.merge_sel(newsel);
}

void matrix_editor_base::cmd_lessthan_equal(parm_t &p)
{
  tok_t *delta;
  int x, y, w, h;
  double val;

  delta = p.token+1;

  w = p.here_base->get_data_width();
  h = p.here_base->get_data_height();

  ASSERT_DOUBLE(delta, "Parameter must be a number");

  narray_o<bool,2> newsel(p.here_base->dm_sel.width(), p.here_base->dm_sel.height());

  newsel.set_all( false );

  for (y=0; y<h; ++y)
    for (x=0; x<w; ++x) {
      if (!p.here_base->get_data_double(x,y,val))
        continue;
      if (val <= delta->dvalue)
        newsel(x,y) = true;
    }

  p.merge_sel(newsel);
}

void matrix_editor_base::cmd_expandcols(parm_t &p)
{
  int j, mj;
  nslice<bool> sl;
  bool found;

  narray_o<bool,2> newsel(p.here_base->dm_sel.width(), p.here_base->dm_sel.height());

  mj = newsel.width();
  for (j=0; j<mj; ++j) {
    sl = p.here_base->dm_sel.yslice(j, 0, newsel.height());
    found =  std::find(sl.begin(), sl.end(), true) != sl.end();
    newsel.yslice(j, 0, newsel.height()).set_all( found );
  }

  p.merge_sel(newsel);
}

void matrix_editor_base::cmd_expandrows(parm_t &p)
{
  int j, mj;
  nslice<bool> sl;
  bool found;

  narray_o<bool,2> newsel(p.here_base->dm_sel.width(), p.here_base->dm_sel.height());

  mj = newsel.height();
  for (j=0; j<mj; ++j) {
    sl = p.here_base->dm_sel.xslice(0, j, newsel.width());
    found =  std::find(sl.begin(), sl.end(), true) != sl.end();
    newsel.xslice(0, j, newsel.width()).set_all( found );
  }

  p.merge_sel(newsel);
}

void matrix_editor_base::cmd_expandrect(parm_t &p)
{
  int x, y, w, h, minx, miny, maxx, maxy;

  w = p.here_base->dm_sel.width();
  h = p.here_base->dm_sel.height();
  minx = w-1;
  maxx = 0;
  miny = h-1;
  maxy = 0;

  for (y=0; y<h; ++y)
    for (x=0; x<w; ++x)
      if (p.here_base->dm_sel.get(x,y)) {
        if (x<minx)
          minx = x;
        if (x>maxx)
          maxx = x;
        if (y<miny)
          miny = y;
        if (y>maxy)
          maxy = y;
      }

  if (maxx>=minx && maxy>=miny) {
    narray_o<bool,2> newsel(w, h);

    newsel.set_all(false);
    newsel.xyslice(minx, miny, maxx-minx+1, maxy-miny+1).set_all( true );
    p.merge_sel(newsel);
  }
}

//
//
// matrix_editor_v
//
//

matrix_editor_v::matrix_editor_v(void)
  : parent_type(dm_commands, this), dm_model(this)
{
  init_gui();
  init_menu(dm_menu);
}

int matrix_editor_v::get_data_width(void) const
{
  return dm_data.width();
}

int matrix_editor_v::get_data_height(void) const
{
  return dm_data.height();
}

void matrix_editor_v::get_data_text(int x, int y, std::string &out) const
{
  double_to_string(dm_data(x, y), out);
}

void matrix_editor_v::get_data_stat(std::string &out) const
{
  size_t minx, miny, maxx, maxy, x, y;
  double minval, maxval;
  double d;
  int c;
  narray_o<double,2>::const_iterator ii, endii;

  // CnP from the viewer... and plus some custom stuff at the end

  out = "Width=";
  out += tool::int_to_string(dm_data.width());
  out += "  Height=";
  out += tool::int_to_string(dm_data.height());
  out += "  NumVal=";
  out += tool::int_to_string(dm_data.size());

  if (dm_data.empty())
    return;

  minx = miny = maxx = maxy = 0;
  maxval = minval = dm_data.get(0,0);
  for (y=0; y<dm_data.height(); ++y)
    for (x=0; x<dm_data.width(); ++x) {
      if (dm_data.get(x,y) < minval) {
        minval = dm_data.get(x,y);
        minx = x;
        miny = y;
      }
      if (dm_data.get(x,y) > minval) {
        maxval = dm_data.get(x,y);
        maxx = x;
        maxy = y;
      }
    }

  out += "  MinVal=";
  out += tool::double_to_string(minval) + " @" + tool::int_to_string(minx)
    + "," + tool::int_to_string(miny);
  out += "  MaxVal=";
  out += tool::double_to_string(maxval) + " @" + tool::int_to_string(maxx)
    + "," + tool::int_to_string(maxy);

  sum(dm_data, d);
  out += "  Sum=";
  out += tool::double_to_string(d);
  mean(dm_data, d);
  out += "  Mean=";
  out += tool::double_to_string(d);

  c = 0;
  endii = dm_data.end();
  for (ii=dm_data.begin(); ii != endii; ++ii)
    if (is_zero(*ii))
      c++;
  out += "  NumZero=";
  out += tool::int_to_string(c);

  int count = 0;

  for (narray_o<bool,2>::const_iterator xx=dm_sel.begin(); xx != dm_sel.end(); ++xx)
    if (*xx)
      count++;

  out += " Selected=";
  out += int_to_string(count);
  out += "(";
  out += int_to_string(count * 100 / dm_sel.size());
  out += "%)";

  if (!dm_clipboard.empty()) {
    out += " Clipboard=";
    out += int_to_string(dm_clipboard.size());
  }
}

bool matrix_editor_v::get_data_double(int x, int y, double &out) const
{
  out = dm_data(x,y);
  return true;
}

void matrix_editor_v::bind_model(scopira::core::model_i *sus)
{
  dm_model = dynamic_cast<narray_m<double,2> *>(sus);
}

void matrix_editor_v::react_model_update(scopira::core::model_i *sus, scopira::core::view_i *src)
{
  if (dm_model.get()) {
    dm_data.copy(dm_model->pm_array.ref());

    prep_data();
  }
}

void matrix_editor_v::react_button(scopira::coreui::button *source, int actionid)
{
  if (dm_model.get()) {
    dm_model->pm_array = new narray_o<double,2>;
    dm_model->pm_array->copy(dm_data);
    dm_model->notify_views(this);
  }
}

void matrix_editor_v::prepare_undo(void)
{
  dm_undodata.copy(dm_data);

  parent_type::prepare_undo();
}

void matrix_editor_v::undo(void)
{
  dm_data.copy(dm_undodata);

  parent_type::undo();
}

void matrix_editor_v::init_menu(popup_menu &b)
{
  b.push_menu("Edit");

  b.push_menu("Modifiers");
  b.add_item("Add (+)", ptr_to_int("P+ "));   //ugh, cast
  b.add_item("Subtract (-)", ptr_to_int("P- "));   //ugh, cast
  b.add_item("Multiply (*)", ptr_to_int("P* "));   //ugh, cast
  b.add_item("Divide (/)", ptr_to_int("P/ "));   //ugh, cast
  b.add_item("Integer divide (div)", ptr_to_int("Pdiv "));   //ugh, cast
  b.add_item("Integer modulo (mod)", ptr_to_int("Pmod "));   //ugh, cast
  b.pop_menu();
  b.add_separator();

  b.add_item("Clear to zero (s 0)", ptr_to_int("Ys 0"));   //ugh, cast
  b.add_item("Set all to 1 (s or set)", ptr_to_int("Xs 1"));   //ugh, cast
  b.add_item("Incremental fill (fill)", ptr_to_int("X+ fill 1"));   //ugh, cast

  b.add_item("Randomize (r or random)", ptr_to_int("Xrandom 1"));   //ugh, cast
  b.add_item("Truncate to integers (int)", ptr_to_int("Yint"));   //ugh, cast
  b.add_item("Invert/Recipricate  (recip)", ptr_to_int("Yrecip"));   //ugh, cast
  b.add_item("Sign (sign)", ptr_to_int("Xsign"));   //ugh, cast

  b.pop_menu();

  b.push_menu("Dimension");

  b.add_item("Resize w h (resize or re)", ptr_to_int("Xresize 10 10"));   //ugh, cast
  b.add_item("Alter current size (alter)", ptr_to_int("Xalter 1 1"));   //ugh, cast
  b.add_item("Crop (crop)", ptr_to_int("Ycrop"));   //ugh, cast

  b.add_separator();

  b.add_item("Deleted selected columns (delcols)", ptr_to_int("Ydelcols"));   //ugh, cast
  b.add_item("Deleted selected rows (delrows)", ptr_to_int("Ydelrows"));   //ugh, cast

  b.add_separator();

  b.add_item("Transpose (trans)", ptr_to_int("Ytrans"));   //ugh, cast

  b.pop_menu();

  b.push_menu("Clipboard");

  b.add_item("Copy (copy)", ptr_to_int("Ycopy"));   //ugh, cast
  b.add_item("Paste (paste)", ptr_to_int("Ypaste"));   //ugh, cast

  b.add_separator();

  b.add_item("Vertical copy (vcopy)", ptr_to_int("Yvcopy"));   //ugh, cast
  b.add_item("Vertical paste (vpaste)", ptr_to_int("Yvpaste"));   //ugh, cast

  b.pop_menu();

  b.push_menu("Selection");

  b.push_menu("Group");
  b.add_item("Add/Union (add)", ptr_to_int("Padd "));   //ugh, cast
  b.add_item("Subtract (sub)", ptr_to_int("Psub "));   //ugh, cast
  b.add_item("Intersection (and)", ptr_to_int("Pand "));   //ugh, cast
  b.pop_menu();
  b.add_separator();

  b.add_item("All (all)", ptr_to_int("Yall"));   //ugh, cast
  b.add_item("None (none)", ptr_to_int("Ynone"));   //ugh, cast
  b.add_item("Rectangle (rect)", ptr_to_int("Xrect 0 0 2 2"));   //ugh, cast
  b.add_item("One cell (sel)", ptr_to_int("Xsel 0 0"));   //ugh, cast
  b.add_item("Invert selection (selinvert)", ptr_to_int("Yselinvert"));   //ugh, cast

  b.add_separator();

  b.add_item("Diagnal (diag)", ptr_to_int("Ydiag"));   //ugh, cast
  b.add_item("Lower diagnal (ldiag)", ptr_to_int("Yldiag"));   //ugh, cast
  b.add_item("Upper diagnal (udiag)", ptr_to_int("Yudiag"));   //ugh, cast

  b.add_separator();

  b.add_item("Equals to 0 (=)", ptr_to_int("X= 0"));   //ugh, cast
  b.add_item("Not equals to 0 (!=)", ptr_to_int("X!= 0"));   //ugh, cast
  b.add_item("Less than 0 (<)", ptr_to_int("X< 0"));   //ugh, cast
  b.add_item("Less than or equal to 0 (<=)", ptr_to_int("X<= 0"));   //ugh, cast
  b.add_item("Greater than 0 (>)", ptr_to_int("X> 0"));   //ugh, cast
  b.add_item("Greater than or equal to 0 (>=)", ptr_to_int("X>= 0"));   //ugh, cast

  b.add_separator();

  b.add_item("Odd columns (oddcols)", ptr_to_int("Yoddcols"));   //ugh, cast
  b.add_item("Even columns (evencols)", ptr_to_int("Yevencols"));   //ugh, cast
  b.add_item("Odd rows (oddrows)", ptr_to_int("Yoddrows"));   //ugh, cast
  b.add_item("Even rows (evenrows)", ptr_to_int("Yevenrows"));   //ugh, cast

  b.add_separator();

  b.add_item("Expand columns (cols)", ptr_to_int("Ycols"));   //ugh, cast
  b.add_item("Expand rows (rows)", ptr_to_int("Yrows"));   //ugh, cast
  b.add_item("Expand rectangle (expandrect)", ptr_to_int("Yexpandrect"));   //ugh, cast

  b.pop_menu();
}

void matrix_editor_v::cmd_resize(parm_t &p)
{
  matrix_editor_v *here = p.get_here<matrix_editor_v>();
  tok_t *w, *h;
  int minw, minh;

  w = p.token+1;
  h = p.token+2;

  ASSERT_INT(w, "Width must be an integer");
  ASSERT_INT(h, "Height must be an integer");
  ASSERT_POSITIVE_INT(w, "Non-positive width given to resize");
  ASSERT_POSITIVE_INT(h, "Non-positive height given to resize");

  here->dm_sel.resize(w->ivalue, h->ivalue);
  here->dm_sel.set_all(false);

  narray<double,2> old(here->dm_data);

  here->dm_data.resize(w->ivalue, h->ivalue);
  here->dm_data.clear();

  minw = std::min<int>(old.width(), w->ivalue);
  minh = std::min<int>(old.height(), h->ivalue);
  here->dm_data.xyslice(0, 0, minw, minh).copy( old.xyslice(0, 0, minw, minh) );
}

void matrix_editor_v::cmd_alter(parm_t &p)
{
  matrix_editor_v *here = p.get_here<matrix_editor_v>();
  tok_t *wd, *hd;
  int minw, minh, neww, newh;

  wd = p.token+1;
  hd = p.token+2;

  ASSERT_INT(wd, "Width-delta must be an integer");
  ASSERT_INT(hd, "Height-delta must be an integer");

  neww = here->dm_data.width() + wd->ivalue;
  newh = here->dm_data.height() + hd->ivalue;

  if (neww <= 0) {
    p.error = "New width will be non-positive";
    return;
  }
  if (newh <= 0) {
    p.error = "New height will be non-positive";
    return;
  }

  here->dm_sel.resize(neww, newh);
  here->dm_sel.set_all(false);

  narray<double,2> old(here->dm_data.width(), here->dm_data.height());
  old.copy(here->dm_data);

  here->dm_data.resize(neww, newh);
  here->dm_data.clear();

  minw = std::min<int>(old.width(), neww);
  minh = std::min<int>(old.height(), newh);
  here->dm_data.xyslice(0, 0, minw, minh).copy( old.xyslice(0, 0, minw, minh) );
}

void matrix_editor_v::cmd_crop(parm_t &p)
{
  matrix_editor_v *here = p.get_here<matrix_editor_v>();
  int x, y, w, h, minx, miny, maxx, maxy;

  w = here->dm_sel.width();
  h = here->dm_sel.height();
  minx = w-1;
  maxx = 0;
  miny = h-1;
  maxy = 0;

  for (y=0; y<h; ++y)
    for (x=0; x<w; ++x)
      if (here->dm_sel.get(x,y)) {
        if (x<minx)
          minx = x;
        if (x>maxx)
          maxx = x;
        if (y<miny)
          miny = y;
        if (y>maxy)
          maxy = y;
      }

  // bounding rect found
  if (minx == 0 && miny == 0 && maxx == (here->dm_sel.width()-1) && maxy == (here->dm_sel.height())-1) {
    p.error = "You usually want to crop a SUBSET of your current data";
    return;
  }

  if (maxx>=minx && maxy>=miny) {
    w = maxx - minx + 1;
    h = maxy - miny + 1;

    here->dm_sel.resize(w, h);
    here->dm_sel.set_all(false);

    narray<double,2> old;
    old.copy(here->dm_data.xyslice(minx, miny, w, h));

    here->dm_data.copy(old);
  }
}

void matrix_editor_v::cmd_delcols(parm_t &p)
{
  matrix_editor_v *here = p.get_here<matrix_editor_v>();
  int j, mj;
  nslice<bool> sl;
  std::vector<int> keepers;

  mj = here->dm_data.width();
  keepers.reserve(mj);

  for (j=0; j<mj; ++j) {
    sl = here->dm_sel.yslice(j, 0, here->dm_sel.height());
    if (std::find(sl.begin(), sl.end(), true) == sl.end())
      keepers.push_back(j);
  }

  if (keepers.empty()) {
    p.error = "An empty matrix would result";
    return;
  }
  if (keepers.size() == here->dm_data.width()) {
    p.error = "Select some columns for deletion";
    return;
  }

  here->dm_sel.resize(keepers.size(), here->dm_sel.height());
  here->dm_sel.set_all(false);

  narray<double,2> old;
  old.copy(here->dm_data);

  here->dm_data.resize(keepers.size(), here->dm_data.height());
  for (j=0; j<keepers.size(); ++j)
    here->dm_data.yslice(j, 0, here->dm_data.height()).copy(
        old.yslice(keepers[j], 0, old.height()) );
}

void matrix_editor_v::cmd_delrows(parm_t &p)
{
  matrix_editor_v *here = p.get_here<matrix_editor_v>();
  int j, mj;
  nslice<bool> sl;
  std::vector<int> keepers;

  mj = here->dm_data.height();
  keepers.reserve(mj);

  for (j=0; j<mj; ++j) {
    sl = here->dm_sel.xslice(0, j, here->dm_sel.width());
    if (std::find(sl.begin(), sl.end(), true) == sl.end())
      keepers.push_back(j);
  }

  if (keepers.empty()) {
    p.error = "An empty matrix would result";
    return;
  }

  here->dm_sel.resize(here->dm_sel.width(), keepers.size());
  here->dm_sel.set_all(false);

  narray<double,2> old;
  old.copy(here->dm_data);

  here->dm_data.resize(here->dm_data.width(), keepers.size());
  for (j=0; j<keepers.size(); ++j)
    here->dm_data.xslice(0, j, here->dm_data.width()).copy(
        old.xslice(0, keepers[j], old.width()) );
}

void matrix_editor_v::cmd_copy(parm_t &p)
{
  matrix_editor_v *here = p.get_here<matrix_editor_v>();
  int x, y, count;

  count = 0;

  for (y=0; y<here->dm_sel.height(); ++y)
    for (x=0; x<here->dm_sel.width(); ++x)
      if (here->dm_sel.get(x,y))
        count++;

  here->dm_clipboard.resize(count);

  count = 0;

  for (y=0; y<here->dm_sel.height(); ++y)
    for (x=0; x<here->dm_sel.width(); ++x)
      if (here->dm_sel.get(x,y))
        here->dm_clipboard.set(count++, here->dm_data.get(x,y));
}

void matrix_editor_v::cmd_vcopy(parm_t &p)
{
  matrix_editor_v *here = p.get_here<matrix_editor_v>();
  int x, y, count;

  count = 0;

  for (x=0; x<here->dm_sel.width(); ++x)
    for (y=0; y<here->dm_sel.height(); ++y)
      if (here->dm_sel.get(x,y))
        count++;

  here->dm_clipboard.resize(count);

  count = 0;

  for (x=0; x<here->dm_sel.width(); ++x)
    for (y=0; y<here->dm_sel.height(); ++y)
      if (here->dm_sel.get(x,y))
        here->dm_clipboard.set(count++, here->dm_data.get(x,y));
}

void matrix_editor_v::cmd_paste(parm_t &p)
{
  matrix_editor_v *here = p.get_here<matrix_editor_v>();
  int x, y, count;

  if (here->dm_clipboard.empty()) {
    p.error = "Clipboard is empty";
    return;
  }

  count = 0;

  for (y=0; y<here->dm_sel.height(); ++y)
    for (x=0; x<here->dm_sel.width(); ++x)
      if (here->dm_sel.get(x,y)) {
        p.apply_op(here->dm_clipboard.get(count++), here->dm_data(x,y));
        if (count == here->dm_clipboard.size())
          count = 0;    // support loop around
      }
  p.reset_op();
}

void matrix_editor_v::cmd_vpaste(parm_t &p)
{
  matrix_editor_v *here = p.get_here<matrix_editor_v>();
  int x, y, count;

  if (here->dm_clipboard.empty()) {
    p.error = "Clipboard is empty";
    return;
  }

  count = 0;

  for (x=0; x<here->dm_sel.width(); ++x)
    for (y=0; y<here->dm_sel.height(); ++y)
      if (here->dm_sel.get(x,y)) {
        p.apply_op(here->dm_clipboard.get(count++), here->dm_data(x,y));
        if (count == here->dm_clipboard.size())
          count = 0;    // support loop around
      }
  p.reset_op();
}

void matrix_editor_v::cmd_set_one(parm_t &p)
{
  matrix_editor_v *here = p.get_here<matrix_editor_v>();
  int x, y;
  double val;

  assert((p.token+1)->isint);
  assert((p.token+2)->isint);
  assert((p.token+3)->isdouble);
  x =  (p.token+1)->ivalue;
  y =  (p.token+2)->ivalue;
  val =  (p.token+3)->dvalue;

  p.apply_op(val, here->dm_data(x, y));
}

void matrix_editor_v::cmd_set(parm_t &p)
{
  matrix_editor_v *here = p.get_here<matrix_editor_v>();
  tok_t *delta;
  int x, y;
  double val;

  delta = p.token+1;
  val = delta->dvalue;

  ASSERT_DOUBLE(delta, "Parameter must be a number");

  for (y=0; y<here->dm_data.height(); ++y)
    for (x=0; x<here->dm_data.width(); ++x)
      if (here->dm_sel.get(x,y))
        p.apply_op(val, here->dm_data(x, y));
  p.reset_op();
}

void matrix_editor_v::cmd_fill(parm_t &p)
{
  matrix_editor_v *here = p.get_here<matrix_editor_v>();
  tok_t *delta;
  int x, y;
  double tot, cur;

  delta = p.token+1;

  ASSERT_DOUBLE(delta, "Parameter must be a number");

  tot = 0;
  cur = delta->dvalue;

  for (y=0; y<here->dm_data.height(); ++y)
    for (x=0; x<here->dm_data.width(); ++x)
      if (here->dm_sel.get(x,y)) {
        p.apply_op( tot, (here->dm_data)(x, y) );
        tot += cur;
      }
  p.reset_op();
}

void matrix_editor_v::cmd_random(parm_t &p)
{
  matrix_editor_v *here = p.get_here<matrix_editor_v>();
  tok_t *delta;
  int x, y;
  lincon_core rcore;
  lincon_01 r(rcore);

  delta = p.token+1;

  ASSERT_DOUBLE(delta, "Parameter must be a number");

  rcore.seed(time_seed());

  for (y=0; y<here->dm_data.height(); ++y)
    for (x=0; x<here->dm_data.width(); ++x)
      if (here->dm_sel.get(x,y))
        p.apply_op(r() * delta->dvalue, (here->dm_data)(x,y));
  p.reset_op();
}

void matrix_editor_v::cmd_int(parm_t &p)
{
  matrix_editor_v *here = p.get_here<matrix_editor_v>();
  int x, y;

  for (y=0; y<here->dm_data.height(); ++y)
    for (x=0; x<here->dm_data.width(); ++x)
      if (here->dm_sel.get(x,y))
        p.apply_op(static_cast<int>(here->dm_data(x,y)), (here->dm_data)(x, y) );
  p.reset_op();
}

void matrix_editor_v::cmd_recip(parm_t &p)
{
  matrix_editor_v *here = p.get_here<matrix_editor_v>();
  int x, y;

  for (y=0; y<here->dm_data.height(); ++y)
    for (x=0; x<here->dm_data.width(); ++x)
      if (here->dm_sel.get(x,y) && !is_zero(here->dm_data(x,y)))
        p.apply_op(1/here->dm_data(x,y), (here->dm_data)(x, y) );
  p.reset_op();
}

void matrix_editor_v::cmd_sign(parm_t &p)
{
  matrix_editor_v *here = p.get_here<matrix_editor_v>();
  int x, y;

  for (y=0; y<here->dm_data.height(); ++y)
    for (x=0; x<here->dm_data.width(); ++x)
      if (here->dm_sel.get(x,y) && !is_zero(here->dm_data(x,y)))
        p.apply_op(sign(here->dm_data(x,y)), (here->dm_data)(x, y) );
  p.reset_op();
}

/* dont think this one is needed
void matrix_editor_v::cmd_sel_set(parm_t &p)
{
  matrix_editor_v *here = p.get_here<matrix_editor_v>();
  here->dm_sel.de = matrix_editor_base::parser::sel_set_c;
}*/

void matrix_editor_v::cmd_trans(parm_t &p)
{
  matrix_editor_v *here = p.get_here<matrix_editor_v>();
  int x, y;

  if (here->dm_data.width() == here->dm_data.height()) {
    int mx;
    // sqaure matrix. do the swap in place
    mx = here->dm_data.width();
    for (y=0; y<mx; ++y)
      for (x=y+1; x<mx; ++x)
        std::swap( (here->dm_data)(x,y), (here->dm_data)(y,x) );
    return;
  }

  narray<double,2> old(here->dm_data);

  here->dm_data.resize(old.height(), old.width());

  here->dm_sel.resize(old.height(), old.width());
  here->dm_sel.set_all(false);

  for (y=0; y<old.height(); ++y)
    for (x=0; x<old.width(); ++x)
      here->dm_data.set(y, x, old(x,y));
}

void matrix_editor_v::cmd_diag(parm_t &p)
{
  matrix_editor_v *here = p.get_here<matrix_editor_v>();
  int x, mx;

  if (here->dm_sel.width() != here->dm_sel.height()) {
    p.error = "Matrix must be sqaure";
    return;
  }

  narray_o<bool,2> newsel(here->dm_sel.width(), here->dm_sel.height());

  newsel.set_all( false );

  mx = here->dm_sel.width();
  for (x=0; x<mx; ++x)
    newsel(x,x) = true;

  p.merge_sel(newsel);
}

void matrix_editor_v::cmd_udiag(parm_t &p)
{
  matrix_editor_v *here = p.get_here<matrix_editor_v>();
  int x, y, mx;

  if (here->dm_sel.width() != here->dm_sel.height()) {
    p.error = "Matrix must be sqaure";
    return;
  }

  narray_o<bool,2> newsel(here->dm_sel.width(), here->dm_sel.height());

  newsel.set_all( false );

  mx = here->dm_sel.width();

  for (y=0; y<mx; ++y)
    for (x=y+1; x<mx; ++x)
      newsel(x,y) = true;

  p.merge_sel(newsel);
}

void matrix_editor_v::cmd_ldiag(parm_t &p)
{
  matrix_editor_v *here = p.get_here<matrix_editor_v>();
  int x, y, mx;

  if (here->dm_sel.width() != here->dm_sel.height()) {
    p.error = "Matrix must be sqaure";
    return;
  }

  narray_o<bool,2> newsel(here->dm_sel.width(), here->dm_sel.height());

  newsel.set_all( false );

  mx = here->dm_sel.width();

  for (y=0; y<mx; ++y)
    for (x=y+1; x<mx; ++x)
      newsel(y,x) = true;

  p.merge_sel(newsel);
}

void matrix_editor_v::cmd_oddcols(parm_t &p)
{
  matrix_editor_v *here = p.get_here<matrix_editor_v>();
  int j, mj;

  narray_o<bool,2> newsel(here->dm_sel.width(), here->dm_sel.height());

  mj = newsel.width();
  for (j=0; j<mj; ++j)
    newsel.yslice(j, 0, newsel.height()).set_all( (j&1) != 0 );

  p.merge_sel(newsel);
}

void matrix_editor_v::cmd_evencols(parm_t &p)
{
  matrix_editor_v *here = p.get_here<matrix_editor_v>();
  int j, mj;

  narray_o<bool,2> newsel(here->dm_sel.width(), here->dm_sel.height());

  mj = newsel.width();
  for (j=0; j<mj; ++j)
    newsel.yslice(j, 0, newsel.height()).set_all( (j&1) == 0 );

  p.merge_sel(newsel);
}

void matrix_editor_v::cmd_oddrows(parm_t &p)
{
  matrix_editor_v *here = p.get_here<matrix_editor_v>();
  int j, mj;

  narray_o<bool,2> newsel(here->dm_sel.width(), here->dm_sel.height());

  mj = newsel.height();
  for (j=0; j<mj; ++j)
    newsel.xslice(0, j, newsel.width()).set_all( (j&1) != 0 );

  p.merge_sel(newsel);
}

void matrix_editor_v::cmd_evenrows(parm_t &p)
{
  matrix_editor_v *here = p.get_here<matrix_editor_v>();
  int j, mj;

  narray_o<bool,2> newsel(here->dm_sel.width(), here->dm_sel.height());

  mj = newsel.height();
  for (j=0; j<mj; ++j)
    newsel.xslice(0, j, newsel.width()).set_all( (j&1) == 0 );

  p.merge_sel(newsel);
}

matrix_editor_base::cmd_t matrix_editor_v::dm_commands[] =
{
  {"re", 2, cmd_resize},
  {"resize", 2, cmd_resize},
  {"alter", 2, cmd_alter},
  {"crop", 0, cmd_crop},
  {"delcols", 0, cmd_delcols},
  {"delrows", 0, cmd_delrows},

  {"copy", 0, cmd_copy},
  {"paste", 0, cmd_paste},
  {"vcopy", 0, cmd_vcopy},
  {"vpaste", 0, cmd_vpaste},

  {"XSETONE", 3, cmd_set_one},     // special version, x,y,val
  {"XSETALL", 1, cmd_set},     // special version
  {"s", 1, cmd_set},
  {"set", 1, cmd_set},
  {"fill", 1, cmd_fill},
  {"r", 1, cmd_random},
  {"random", 1, cmd_random},
  {"int", 0, cmd_int},
  {"recip", 0, cmd_recip},
  {"sign", 0, cmd_sign},

  {"+", 0, cmd_mod_add},
  {"-", 0, cmd_mod_sub},
  {"*", 0, cmd_mod_mul},
  {"/", 0, cmd_mod_div},
  {"div", 0, cmd_mod_idiv},
  {"mod", 0, cmd_mod_imod},

  //{"selset", 0, cmd_sel_set},
  {"add", 0, cmd_sel_add},
  {"sub", 0, cmd_sel_sub},
  {"and", 0, cmd_sel_inter},

  {"selinvert", 0, cmd_sel_invert},
  {"sel", 2, cmd_sel},
  {"rect", 4, cmd_sel_rect},
  {"all", 0, cmd_sel_all},
  {"none", 0, cmd_sel_none},

  {"trans", 0, cmd_trans},

  {"diag", 0, cmd_diag},
  {"udiag", 0, cmd_udiag},
  {"ldiag", 0, cmd_ldiag},

  {"=", 1, cmd_equals},
  {"!=", 1, cmd_notequals},
  {">", 1, cmd_greaterthan},
  {">=", 1, cmd_greaterthan_equal},
  {"<", 1, cmd_lessthan},
  {"<=", 1, cmd_lessthan_equal},

  {"oddcols", 0, cmd_oddcols},
  {"evencols", 0, cmd_evencols},
  {"oddrows", 0, cmd_oddrows},
  {"evenrows", 0, cmd_evenrows},

  {"cols", 0, cmd_expandcols},
  {"rows", 0, cmd_expandrows},
  {"expandrect", 0, cmd_expandrect},

  {0, 0, 0}
};

#ifdef OIJIJDJJJJJJ

//
//
// load_matrix_v
//
//

load_matrix_v::load_matrix_v(void)
{
  count_vtr<grid_layout> g = new grid_layout(2,1);

  dm_filename = new fileentry_v;
  dm_filename->set_filename("matrix.txt");

  g->add_widget(new label_v("File Name:"),0,0,1,1,false,false);
  g->add_widget(dm_filename.get(),1,0,1,1,true,true);

  applicator_type::init_gui(g.get());
}

void load_matrix_v::react_button(scopira::coreui::button *source, int actionid)
{
  if (dm_model.is_null())
    return;

  scopira::tool::printiflow fin(true,0), lin(true,0);
  std::vector<double> dlist;
  std::string s;
  int width,j,x,y;
  double d;

  // clear the matrix
  // need a new one, since the original may be read-only
  dm_model->pm_array = new narray_m<double,2>;

  // open the input file
  fin.open(new scopira::tool::cacheiflow(true, new scopira::tool::fileflow(dm_filename->get_filename(), scopira::tool::fileflow::input_c)));
  if (fin.failed()) {
    set_error_label("Could not read file: " + dm_filename->get_filename());
    return;
  }

  // read into buffers
  fin.read_string(s);
  lin.open(new scopira::tool::stringflow(s+" ", scopira::tool::stringflow::input_c));

  // read the first line
  while (lin.read_double(d))
    dlist.push_back(d);

  // get width of matrix
  width = dlist.size();

  // read rest of file
  while (fin.read_double(d))
    dlist.push_back(d);

  //assemble output matrix
  if (width == 0) {
    dm_model->pm_array->resize(0,0);
  } else {
    j=0;
    dm_model->pm_array->resize(width, dlist.size()/width);
    for (y=0; y<dlist.size()/width; y++)
      for (x=0; x<width; x++)
        dm_model->pm_array->set_double(x,y,dlist[j++]);
  }

  dm_model->notify_views(this);
}

//
//
// save_matrix_v
//
//

save_matrix_v::save_matrix_v(void)
{
  count_vtr<grid_layout> g = new grid_layout(2,1);

  dm_filename = new fileentry_v;
  dm_filename->set_filename("matrix.txt");

  g->add_widget(new label_v("File Name:"),0,0,1,1,false,false);
  g->add_widget(dm_filename.get(),1,0,1,1,true,true);

  applicator_type::init_gui(g.get());
}

void save_matrix_v::react_button(scopira::coreui::button *source, int actionid)
{
  if (dm_model.is_null())
    return;

  scopira::tool::printoflow fout(true,0);
  int x,y;

  // open the output file
  fout.open(new scopira::tool::cacheoflow(true, new scopira::tool::fileflow(dm_filename->get_filename(), scopira::tool::fileflow::output_c)));
  if (fout.failed()) {
    set_error_label("Could not open file for writing: " + dm_filename->get_filename());
    return;
  }

  // make sure we have some data to save
  if (dm_model->pm_array->width() <= 0 || dm_model->pm_array->height() <= 0)
    return;

  for (y=0; y<dm_model->pm_array->height(); y++) {
    fout.write_double(dm_model->pm_array->get_double(0, y));
    for (x = 1; x<dm_model->pm_array->width(); x++) {
      fout.write_char(' ');
      fout.write_double(dm_model->pm_array->get_double(x, y));
    }
    fout.write_char('\n');
  }

  // doing ok
  clear_error_label();
}

#endif

