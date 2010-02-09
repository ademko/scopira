
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

#include <scopira/qtkit/Plot2D.h>

#include <QPainter>
#include <QTextStream>
#include <QDebug>

#include <scopira/basekit/vectormath.h>

#include <scopira/qtkit/LocalPainter.h>

using namespace scopira::basekit;
using namespace scopira::qtkit;

Plot2D::Plot2D(QWidget *parent)
  : QWidget(parent), dm_area(0,0)
{
  dm_dirty = true;
  dm_xrange_auto = true;
  dm_yrange_auto = true;
}

void Plot2D::setXLabel(const QString &l)
{
  dm_xlabel = l;
  setDirty();
}

void Plot2D::setYLabel(const QString &l)
{
  dm_ylabel = l;
  setDirty();
}

void Plot2D::setTitle(const QString &l)
{
  dm_title = l;
  setDirty();
}

void Plot2D::setXRangeAuto(void)
{
  dm_xrange_auto = true;
  dm_view_xmin = dm_data_xmin;
  dm_view_xmax = dm_data_xmax;
  setDirty();
}

void Plot2D::setXRange(double min, double max)
{
  dm_xrange_auto = false;
  dm_view_xmin = min;
  dm_view_xmax = max;
  setDirty();
}

void Plot2D::setYRangeAuto(void)
{
  dm_yrange_auto = true;
  dm_view_ymin = dm_data_ymin;
  dm_view_ymax = dm_data_ymax;
  setDirty();
}

void Plot2D::setYRange(double min, double max)
{
  dm_yrange_auto = false;
  dm_view_ymin = min;
  dm_view_ymax = max;
  setDirty();
}

scopira::basekit::narray<double> Plot2D::labelValuesIndex(size_t datalen)
{
  narray<double> R;

  R.resize(datalen);

  for (size_t x=0; x<R.size(); ++x)
    R[x] = x;

  return R;
}

scopira::basekit::narray<double> Plot2D::labelValuesMinMax(size_t datalen, double min, double max)
{
  narray<double> R;
  double delta;

  R.resize(datalen);

  delta = (max-min)/datalen;

  R[0] = min;
  R[R.size()-1] = max;
  for (size_t x=1; (x+1)<R.size(); ++x)
    R[x] = R[x-1] + delta;

  return R;
}

void Plot2D::resizeData(int numdata)
{
  assert(numdata>=0);
  dm_data.resize(numdata);
  setDirty();
}

void Plot2D::setData(int id, scopira::basekit::const_nslice<double> xvalues, scopira::basekit::const_nslice<double> yvalues)
{
  assert(id < dm_data.size());

  assert(xvalues.size() == yvalues.size());

  dm_data[id].xvalues.copy(xvalues);
  dm_data[id].yvalues.copy(yvalues);

  setDirty();
}

int Plot2D::xToPixel(double x)
{
  if (x<dm_view_xmin || x>dm_view_xmax)
    return -1;
  return static_cast<int>((x-dm_view_xmin)/(dm_view_xmax-dm_view_xmin)*dm_area.width()) + dm_offset.x();
}

int Plot2D::yToPixel(double y)
{
  if (y<dm_view_ymin || y>dm_view_ymax)
    return -1;
  return static_cast<int>(dm_area.height() - (y-dm_view_ymin)/(dm_view_ymax-dm_view_ymin)*dm_area.height()) + dm_offset.y();
}

bool Plot2D::xToPlot(int screenx, double &outval)
{
  if (screenx<dm_offset.x() || screenx>dm_offset.x()+dm_area.width())
    return false;

  outval = static_cast<double>(screenx-dm_offset.x())/dm_area.width() * (dm_view_xmax-dm_view_xmin) + dm_view_xmin;
  return true;
}

bool Plot2D::yToPlot(int screeny, double &outval)
{
  if (screeny<dm_offset.y() || screeny>dm_offset.y()+dm_area.height())
    return false;

  outval = (1- static_cast<double>(screeny-dm_offset.y())/dm_area.height()) * (dm_view_ymax-dm_view_ymin) + dm_view_ymin;
  return true;
}

void Plot2D::handlePrePaint(QPainter &dc)
{
}

void Plot2D::handlePostPaint(QPainter &dc)
{
}

void Plot2D::handleLinePaint(int indexx, int dataidy, double xvalue, double yvalue,
        QPainter &dc, int x1, int y1, int x2, int y2)
{
  dc.drawLine(x1, y1, x2, y2);
}

void Plot2D::handlePointPaint(int indexx, int dataidy, double xvalue, double yvalue,
        QPainter &dc, int x, int y)
{
}

double Plot2D::handleXLabel(double xdata)
{
  return xdata;
}

double Plot2D::handleYLabel(double ydata)
{
  return ydata;
}

void Plot2D::resizeEvent(QResizeEvent *event)
{
  dm_dirty = true;
}

void Plot2D::paintEvent(QPaintEvent *event)
{
  QPainter dc(this);
  // the double buffering stuff doesnt work to well right now if you show the dialog a 2nd time
  //wxAutoBufferedPaintDC dc(this);   // double buffering stuff
  //wxBufferedPaintDC dc(this, wxBUFFER_VIRTUAL_AREA);   // double buffering stuff

  //std::cerr << dc.GetSize().width() << " xxx " << dc.GetSize().height() << '\n';
  if (dm_dirty)
    reCalcGui(dc);

  drawFrame(dc);

  handlePrePaint(dc);

  if (dm_emptydata)
    dc.drawText(dm_offset.x()+5, dm_offset.y()+
        QFontMetrics(dc.font()).height(), "No data to plot");
  else {
    drawData(dc);
    handlePostPaint(dc);
  }
}

void Plot2D::setDirty(void)
{
  dm_dirty = true;
  update();
}

void Plot2D::reCalcGui(QPainter &dc)
{
  dm_dirty = false;

  // calc the off set stuff
  dm_offset.rx() = border*2;
  dm_offset.ry() = border;

  dm_area = QSize(dc.device()->width(), dc.device()->height());
  dm_area.rwidth() += -3 * border;
  dm_area.rheight() += -2 * border;

  // calc the min max
  bool clean_slate = true;
  for (int i=0; i<dm_data.size(); ++i) {
    double d;

    if (dm_data[i].xvalues.empty())
      continue;

    scopira::basekit::min(dm_data[i].xvalues, d);
    if (clean_slate || d < dm_data_xmin)
      dm_data_xmin = d;
    scopira::basekit::max(dm_data[i].xvalues, d);
    if (clean_slate || d > dm_data_xmax)
      dm_data_xmax = d;
    scopira::basekit::min(dm_data[i].yvalues, d);
    if (clean_slate || d < dm_data_ymin)
      dm_data_ymin = d;
    scopira::basekit::max(dm_data[i].yvalues, d);
    if (clean_slate || d > dm_data_ymax)
      dm_data_ymax = d;
    clean_slate = false;
  }

  dm_emptydata = clean_slate || dm_data_xmin >= dm_data_xmax || dm_data_ymin >= dm_data_ymax;

  if (dm_xrange_auto) {
    dm_view_xmin = dm_data_xmin;
    dm_view_xmax = dm_data_xmax;
  }
  if (dm_yrange_auto) {
    dm_view_ymin = dm_data_ymin;
    dm_view_ymax = dm_data_ymax;
  }
}

void Plot2D::drawFrame(QPainter &dc)
{
  LocalPainter p(dc);

  dc.setBrush(QColor(0xFFFFFF));

  dc.drawRect(dm_offset.x(), dm_offset.y(), dm_area.width(), dm_area.height());

  int x, y, i, ticknum;
  int last_font;
  double axis_min, axis_max, tick_spacing;

  // draw x axis
  last_font = -1;
  ticknum = calcTickMarks(dm_view_xmin, dm_view_xmax, axis_min, axis_max, tick_spacing);
  for (i=0; i<=ticknum; ++i) {
    x = xToPixel(axis_min + i*tick_spacing);
    if (x == -1)
      continue;
    y = dm_offset.y()+dm_area.height();
    dc.drawLine(x, y, x, y+6);

    QString label;
    QTextStream s(&label);
    s << handleXLabel(axis_min + i*tick_spacing);
    QSize ext = QFontMetrics(dc.font()).size(Qt::TextSingleLine, label);

    if (last_font == -1 || x-ext.width()/2 > last_font) {
      dc.drawLine(x, y, x, y+12);   // draw a longer line
      dc.drawText(x-ext.width()/2, y+12+ext.height(), label);
      last_font = x+ext.width()/2;
      last_font += 10;  // add some buffer space
    }
  }

  // draw y axis
  last_font = -1;
  ticknum = calcTickMarks(dm_view_ymin, dm_view_ymax, axis_min, axis_max, tick_spacing);
  for (i=0; i<=ticknum; ++i) {
    y = yToPixel(axis_min + i*tick_spacing);
    if (y == -1)
      continue;
    x = dm_offset.x();
    dc.drawLine(x, y, x-4, y);

    QString label;
    QTextStream s(&label);
    s << handleYLabel(axis_min + i*tick_spacing);
    QSize ext = QFontMetrics(dc.font()).size(Qt::TextSingleLine, label);

    if (last_font == -1 || y+ext.height()/2 < last_font) {
      dc.drawLine(x, y, x-10, y);   //draw a longer line
      dc.drawText(x-10-4-ext.width(), y+ext.height()/2-4, label);
      last_font = y+ext.height()/2;
      last_font -= 20;  // add some buffer space
    }
  }

  if (!dm_title.isEmpty()) {
    QSize ext = QFontMetrics(dc.font()).size(Qt::TextSingleLine, dm_title);
    dc.drawText((dc.device()->width()-ext.width())/2, 10, dm_title);
  }
  if (!dm_xlabel.isEmpty()) {
    QSize ext = QFontMetrics(dc.font()).size(Qt::TextSingleLine, dm_xlabel);
    //dc.DrawRotatedText(dm_xlabel, (dm_area.width()-ext.width())/2+dm_offset.x(), dc.device()->height()-20, 0);
    dc.drawText((dm_area.width()-ext.width())/2+dm_offset.x(), dc.device()->height(), dm_xlabel);
  }
  if (!dm_ylabel.isEmpty()) {
    QSize ext = QFontMetrics(dc.font()).size(Qt::TextSingleLine, dm_ylabel);

    LocalPainter rotatedcontext(dc);    // cuz we'redoing a rotate

    dc.translate(4+ext.height(), dm_offset.y()+dm_area.height()-(dm_area.height()-ext.width())/2);
    dc.rotate(-90);

    //dc.DrawRotatedText(dm_ylabel, 4, dm_offset.y()+dm_area.height()-(dm_area.height()-ext.width())/2, 90);
    //dc.drawText(4, dm_offset.y()+dm_area.height()-(dm_area.height()-ext.width())/2, dm_ylabel);
    dc.drawText(0, 0, dm_ylabel);
  }
}

void Plot2D::drawData(QPainter &dc)
{
  for (int datai=0; datai<dm_data.size(); ++datai) {
    Data &cur = dm_data[datai];

    for (int i=1; i<cur.xvalues.size(); ++i) {
      int x, y;

      handleLinePaint(i-1, datai, cur.xvalues[i-1], cur.yvalues[i-1],
          dc,
          xToPixel(cur.xvalues[i-1]), yToPixel(cur.yvalues[i-1]),
          x=xToPixel(cur.xvalues[i]), y=yToPixel(cur.yvalues[i]));
      handlePointPaint(i, datai, cur.xvalues[i], cur.yvalues[i], dc, x, y);
    }
    // draw the first point now
    handlePointPaint(0, datai, cur.xvalues[0], cur.yvalues[0],
        dc,
        xToPixel(cur.xvalues[0]), yToPixel(cur.yvalues[0]));
  }
}

int Plot2D::calcNumLabels(bool xaxis)
{
  int len;

  if (xaxis)
    len = dm_area.width();
  else
    len = dm_area.height();

  return len / 30;
}

int Plot2D::calcTickMarks(double min, double max, double &axis_min, double &axis_max, double &tick_spacing)
{
  double s_dist;  //scaled distance between tick marks
  double scale; //actual tick mark distance is scale*sDist

  int i;
  int tot_ticks = 1;
  int des_num_ticks = 11; //kludge: seems to work reasonably well!
  int num_sp_factor = 4; //kludge: seems to work reasonably well!

  scopira::tool::basic_array<double> spacing_factor;

  spacing_factor.resize(4);
  spacing_factor[0]=1.0;
  spacing_factor[1]=2.0;
  spacing_factor[2]=5.0;
  spacing_factor[3]=10.0;

  axis_min = min;
  axis_max = max;
  tick_spacing = 0.0;

  //compute scale and scaled distance
  s_dist = (max - min) / des_num_ticks;
  scale = pow(10.0, floor(log(s_dist) / log(10.0)));
  s_dist /= scale;

  if (s_dist < 1.0){
    s_dist *= 10.0;
    scale /= 10.0;
  }

  if (s_dist >= 10.0){
    s_dist /= 10.0;
    scale *= 10.0;
  }

  //determine which spacing factor to use
  for (i = 0; spacing_factor[i] < s_dist && i < num_sp_factor; i++)
    ;//empty body

  //compute "sensible" distance
  tick_spacing = spacing_factor[i] * scale;

  //compute "sensible" tick mark positions
  axis_min = ceil(min / tick_spacing) * tick_spacing;

  for (axis_max = axis_min; axis_max < max; tot_ticks++) {
    axis_max = floor(axis_max / tick_spacing + 0.5) * tick_spacing;
    axis_max += tick_spacing;
  }

  //adjust sensible min to avoid clipping min_val.
  if (axis_min > min) {
    axis_min -= tick_spacing;
    tot_ticks++;
  }

  return tot_ticks;
}

