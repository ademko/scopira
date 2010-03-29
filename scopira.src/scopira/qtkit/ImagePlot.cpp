
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

#include <scopira/qtkit/ImagePlot.h>

#include <assert.h>

#include <math.h>

#include <algorithm>

#include <QMouseEvent>
#include <QPainter>
#include <QGridLayout>
#include <QDebug>

#include <scopira/qtkit/LocalPainter.h>

using namespace scopira::qtkit;

void scopira::qtkit::calcAspectRatio(int C, int R, int WC, int WR, int &c, int &r)
{
  double scr, scc;

  scr = static_cast<double>(WR)/R;
  scc = static_cast<double>(WC)/C;
  // assume scc is the smaller
  if (scr < scc)
    scc = scr; //its not?

  r = static_cast<int>(scc * R);
  c = static_cast<int>(scc * C);
}

//
//
// ImagePlot::Drawing
//
//

class ImagePlot::Drawing : public QWidget
{
  public:
    Drawing(ImagePlot *parent);

  protected:
    virtual void resizeEvent(QResizeEvent *event) { dm_parent->drawingResizeEvent(event); }
    virtual void paintEvent(QPaintEvent *event) { dm_parent->drawingPaintEvent(event); }

    virtual void mouseMoveEvent(QMouseEvent *event) { dm_parent->drawingMouseMoveEvent(event); }
    virtual void mousePressEvent(QMouseEvent *event) { dm_parent->drawingMousePressEvent(event); }
    virtual void mouseReleaseEvent(QMouseEvent *event) { dm_parent->drawingMouseReleaseEvent(event); }

  private:
    ImagePlot *dm_parent;
};

ImagePlot::Drawing::Drawing(ImagePlot *parent)
  : dm_parent(parent)
{
  setMinimumSize(100,100);

  setMouseTracking(true);   // want to receive mouse motion events too
  setAttribute(Qt::WA_OpaquePaintEvent, true);
  //setAttribute(Qt::WA_NoSystemBackground, true);
}

//
//
// ImagePlot
//
//

ImagePlot::ImagePlot(QWidget *parent)
  : QWidget(parent), dm_datasize(0,0)   // qt doesnt init QSize to 0,0!
{
  dm_dirty = true;
  dm_zoom_fit = true;
  dm_is_panning = false;

  dm_has_select = false;
  dm_is_drawingrect = false;
  dm_has_endpoint = false;

  QGridLayout *lay = new QGridLayout;
  QBoxLayout *vbox, *hbox;

  dm_drawing = new Drawing(this);
  dm_hscroll = new QScrollBar(Qt::Horizontal);
  dm_vscroll = new QScrollBar(Qt::Vertical);
  vbox = new QBoxLayout(QBoxLayout::LeftToRight);
  hbox = new QBoxLayout(QBoxLayout::TopToBottom);

  vbox->addWidget(dm_hscroll, 1);
  hbox->addWidget(dm_vscroll, 1);

  lay->setSpacing(0);
  lay->setContentsMargins(0, 0, 0, 0);

  lay->addWidget(dm_drawing, 0, 0);
  //lay->addWidget(dm_hscroll, 1, 0, Qt::AlignRight);
  //lay->addWidget(dm_vscroll, 0, 1, Qt::AlignBottom);
  lay->addLayout(vbox, 1, 0, Qt::AlignRight);
  lay->addLayout(hbox, 0, 1, Qt::AlignBottom);

  connect(dm_hscroll, SIGNAL(valueChanged(int)), this, SLOT(onHScroll(int)));
  connect(dm_vscroll, SIGNAL(valueChanged(int)), this, SLOT(onVScroll(int)));

  setLayout(lay);

  resetScrollBars();
}

QWidget * ImagePlot::drawingArea(void) const
{
  return dm_drawing;
}

bool ImagePlot::hasPlotSelectionPoint(void) const
{
  return (dm_has_select && !dm_is_drawingrect && !dm_has_endpoint);
}

const QPoint & ImagePlot::getPlotSelectionPoint(void)
{
  return dm_startpoint;
}

void ImagePlot::setPlotSelectionPoint(const QPoint &plotpoint)
{
  assert(!dm_is_drawingrect);   // and if true, perhaps rethink this logic?

  dm_has_select = true;
  dm_has_endpoint = false;

  dm_startpoint = plotpoint;
}

void ImagePlot::clearPlotSelectionPoint(void)
{
  assert(!dm_is_drawingrect);   // and if true, perhaps rethink this logic?

  dm_has_select = false;
  dm_has_endpoint = false;
}

void ImagePlot::setDirty(void)
{
  dm_dirty = true;
  dm_drawing->update();
  update();
}

void ImagePlot::setPlotSize(QSize sz)
{
  //std::cerr << "SetDataSize(" << sz << ")\n";
  dm_datasize = sz;
  setDirty();
}

bool ImagePlot::xToPlot(int screenx, int &datax)
{
  datax = static_cast<int>(floor((screenx-dm_offset.x())/dm_pixper)) + dm_panpt.x();

  return datax >= 0 && datax < dm_datasize.width();
}

bool ImagePlot::yToPlot(int screeny, int &datay)
{
  datay = static_cast<int>(floor((screeny-dm_offset.y())/dm_pixper)) + dm_panpt.y();

  return datay >= 0 && datay < dm_datasize.height();
}

int ImagePlot::xToPixel(int datax)
{
  return static_cast<int>(dm_offset.x() + (datax-dm_panpt.x())*dm_pixper);
}

int ImagePlot::yToPixel(int datay)
{
  return static_cast<int>(dm_offset.y() + (datay-dm_panpt.y())*dm_pixper);
}

bool ImagePlot::buildZoomMenu(QMenu &menu)
{
  if (dataSizeEmpty())
    return false; // no data

  if (dm_has_endpoint)
    connect(menu.addAction("Zoom Selected"), SIGNAL(triggered()), this, SLOT(onZoomSelected()));
  connect(menu.addAction("Zoom In"), SIGNAL(triggered()), this, SLOT(onZoomIn()));
  connect(menu.addAction("Zoom Out"), SIGNAL(triggered()), this, SLOT(onZoomOut()));
  if (!dm_zoom_fit)
    connect(menu.addAction("Zoom Reset"), SIGNAL(triggered()), this, SLOT(onZoomReset()));

  return true;
}

void ImagePlot::handlePrePaint(QPainter &dc)
{
}

void ImagePlot::handlePostPaint(QPainter &dc)
{
}

bool ImagePlot::handleQuickPaint(QPainter &dc)
{
  return false;
}

void ImagePlot::handleLeftButton(QMouseEvent *event, bool isdrag, bool isdown)
{
  if (isdrag) {
    QPoint last_point;
    bool last_good = xToPlot(event->x(), last_point.rx()) && yToPlot(event->y(), last_point.ry());

    if (dm_has_select && dm_is_drawingrect && last_good) {
      dm_has_endpoint = true;

      dm_endpoint = last_point;

      dm_drawing->update();    // need a full redraw
    }
    return;
  }

  // non-drag
  if (isdown) {
    dm_has_select = xToPlot(event->x(), dm_startpoint.rx()) && yToPlot(event->y(), dm_startpoint.ry());
    dm_is_drawingrect = true;
    dm_has_endpoint = false;
  } else {
    // up/release
    dm_is_drawingrect = false;

    if (hasPlotSelectionPoint())
      handlePlotSelectionPoint(getPlotSelectionPoint());
    dm_drawing->update();
  }
}

void ImagePlot::handleRightButton(QMouseEvent *event, bool isdrag, bool isdown)
{
  //if (! (!isdrag && isdown))
    //return;
}

void ImagePlot::handleMidButton(QMouseEvent *event, bool isdrag, bool isdown)
{
  if (dm_zoom_fit)
    return;

  if (isdrag) {
    assert(dm_is_panning);  // or perhaps just return;
    // calc new panpt
    dm_panpt.rx() = static_cast<int>(dm_panning_anchor_data.x() + (dm_panning_anchor_screen.x()-event->x())/dm_pixper);
    dm_panpt.ry() = static_cast<int>(dm_panning_anchor_data.y() + (dm_panning_anchor_screen.y()-event->y())/dm_pixper);

    //SetScrollPos(wxHORIZONTAL, dm_panpt.x() + dm_scrolloffset.x);
    //SetScrollPos(wxVSCROLL, dm_panpt.y() + dm_scrolloffset.y);

    //updateCroppedBitmap();
    updateScrollBars();
    //dm_drawing->update();
    setDirty();

    return;
  }

  // not drag event
  if (isdown) {
    // button pressed

    dm_is_panning = true;

    dm_panning_anchor_screen = event->pos();
    dm_panning_anchor_data = dm_panpt;
  } else {
    // button released
    dm_is_panning = false;
  }
}

void ImagePlot::handleMouseMotion(QMouseEvent *event)
{
}

void ImagePlot::handlePlotSelectionPoint(const QPoint &plotpoint)
{
}

void ImagePlot::contextMenuEvent(QContextMenuEvent * event)
{
  QMenu menu("Options");
  if (!buildZoomMenu(menu))
    return;

  menu.popup(QCursor::pos());
  menu.exec();
}

void ImagePlot::drawingResizeEvent(QResizeEvent *event)
{
  dm_dirty = true;
}

void ImagePlot::drawingPaintEvent(QPaintEvent *event)
{
  QPainter dc(dm_drawing);

  dc.setBackground(Qt::white);

  if (dataSizeEmpty()) {
    dc.fillRect(0, 0, dc.device()->width(), dc.device()->height(), QColor(0xFFFFFF));
    return; // no data
  }

  if (dm_dirty) {
    if (!updateBitmap(dc)) {
      dc.fillRect(0, 0, dc.device()->width(), dc.device()->height(), QColor(0xFFFFFF));
      return;
    }
    resetScrollBars();
    dm_dirty = false;
  }

  if (handleQuickPaint(dc))
    return;

  // the top one is often overwritten with a status message
  dc.fillRect(0, 0, dc.device()->width(), dm_offset.y(), dc.background());
  dc.fillRect(0, dm_offset.y(), dm_offset.x(), dc.device()->height()-dm_offset.y(), dc.background());
  dc.fillRect(dm_offset.x()+dm_area.width(), dm_offset.y(), dm_offset.x(),
      dc.device()->height()-dm_offset.y(), dc.background());
  dc.fillRect(dm_offset.x(), dm_offset.y()+dm_area.height(), dm_area.width(),
      dc.device()->height()-dm_offset.y()-dm_area.height(), dc.background());

  handlePrePaint(dc);

  dc.drawImage(dm_offset.x(), dm_offset.y(), dm_backbuffer);

  drawSelection(dc);

  handlePostPaint(dc);
}

bool ImagePlot::updateBitmap(QPainter &dc)
{
  assert(dm_datasize.width() > 0);
  assert(dm_datasize.height() > 0);

  dm_offset.rx() = BORDER*2;
  dm_offset.ry() = BORDER;
  dm_area = QSize(dc.device()->width(), dc.device()->height());
  dm_area.rwidth() += -4 * BORDER;
  dm_area.rheight() += -2 * BORDER;

  //qDebug() << dm_offset << dm_area << " device(" << dc.device()->width() << "," << dc.device()->height() << ")";

  if (dm_zoom_fit) {
    int cols, rows;

    calcAspectRatio(dm_datasize.width(), dm_datasize.height(),
        dm_area.width(), dm_area.height(),
        cols, rows);

    if (cols < 4 || rows < 4)
      return false;

    dm_pixper = static_cast<double>(cols) / dm_datasize.width();
    assert(dm_pixper > 0);

    // now, reset the panpoint so that the image is centered
    dm_panpt.rx() = static_cast<int>(-(dm_area.width()-cols)/2 / dm_pixper);
    dm_panpt.ry() = static_cast<int>(-(dm_area.height()-rows)/2 / dm_pixper);
    //dm_panpt.x() = 0;
    //dm_panpt.y() = 0;
  }

  updateCroppedBitmap();

  return true;
}

void ImagePlot::updateCroppedBitmap(void)
{
  QPoint packed_topleft; // in the full data set
  QSize packed_size;     // in plot units too

  packed_topleft.rx() = std::max(0, dm_panpt.x());
  packed_topleft.ry() = std::max(0, dm_panpt.y());

  // we need the 0.0001 "nudge" factor because when the division is a perfect integer, it seems to round
  // down. Probably something to do with precesion/floating/rouding
  // the + 1 is so fractional cells on zooms still get shown
  packed_size.setWidth(std::min(
      dm_datasize.width() - std::max(0,dm_panpt.x()),
      static_cast<int>(dm_area.width()/dm_pixper + 0.0001) + 1 + std::min(0,dm_panpt.x())
      ));
  packed_size.setHeight(std::min(
      dm_datasize.height() - std::max(0,dm_panpt.y()),
      static_cast<int>(dm_area.height()/dm_pixper + 0.0001) + 1 + std::min(0,dm_panpt.y())
      ));

  //qDebug() << "divisor" << (dm_area.height()/dm_pixper) << "divisor-int" << static_cast<int>(dm_area.height()/dm_pixper) << "dm_panpt.y()" << dm_panpt.y() << "packed_size.y()" << packed_size.height();

  /*std::cerr << "datasize: " << dm_datasize << " area=" << dm_area << '\n';
  std::cerr << "panpt=" << dm_panpt << "pixper: " << dm_pixper << '\n';
  std::cerr << "packed:" << packed_topleft << " *** " << packed_size << '\n';
  std::cerr << '\n';*/

  // ok, now get the image data we need from the decendant class

  // lots of potensial for optimization here
  //  1) use scanline/bulk pixel settings of QImage
  //  2) get more pixel data at a time from the decendant time (at the expense of a more confusing call syntax)

  // now render the back buffer
  dm_backbuffer = QImage(dm_area.width(), dm_area.height(), QImage::Format_RGB888);
  int x, w, y, h;
  int clipped_x, clipped_y;

  //qDebug() << "packed_topleft" << packed_topleft << "packed_size" << packed_size << "pixper" << dm_pixper << "dm_panpt" << dm_panpt;

  w = dm_backbuffer.width();
  h = dm_backbuffer.height();

  for (y=0; y<h; ++y) {
    clipped_y = static_cast<int>(floor(y/dm_pixper)) + dm_panpt.y() - packed_topleft.y();
    for (x=0; x<w; ++x) {
      // replace with better scaling? or atleast faster? maybe NN is more accurate?
      clipped_x = static_cast<int>(floor(x/dm_pixper)) + dm_panpt.x() - packed_topleft.x();

      if (clipped_x<0 || clipped_x>=packed_size.width() || clipped_y<0
          || clipped_y>=packed_size.height()) {
        // out of bounds, show white
        dm_backbuffer.setPixel(x, y, 0xFFFFFF);
        continue;
      }

      int ppix = handleGetDataAsRGB(packed_topleft.x()+clipped_x, packed_topleft.y()+clipped_y);
      dm_backbuffer.setPixel(x, y, ppix);
      //dm_backbuffer.setPixel(x, y, 0x00FF00);
    }//x
  }//y
}

void ImagePlot::drawSelection(QPainter &dc)
{
  // is there a selected point?
  if (dm_has_select && !dm_is_drawingrect && !dm_has_endpoint) {
    LocalPainter p(dc);

    dc.setPen(Qt::white);
    dc.setClipRect(dm_offset.x(), dm_offset.y(), dm_area.width(), dm_area.height());

    int x = xToPixel(dm_startpoint.x()) + static_cast<int>(dm_pixper/2);
    int y = yToPixel(dm_startpoint.y()) + static_cast<int>(dm_pixper/2);

    dc.drawLine(x-5, y, x+5, y);
    dc.drawLine(x, y-5, x, y+5);
  }
  if (dm_has_select && dm_has_endpoint) {
    LocalPainter p(dc);
    QPoint topleft, botright;

    dc.setPen(Qt::white);
    dc.setBrush(Qt::NoBrush);
    dc.setClipRect(dm_offset.x(), dm_offset.y(), dm_area.width(), dm_area.height());

    topleft.rx() = std::min(dm_startpoint.x(), dm_endpoint.x());
    topleft.ry() = std::min(dm_startpoint.y(), dm_endpoint.y());
    botright.rx() = std::max(dm_startpoint.x(), dm_endpoint.x());
    botright.ry() = std::max(dm_startpoint.y(), dm_endpoint.y());

    int actual_x = xToPixel(topleft.x());
    int actual_y = yToPixel(topleft.y());

    dc.drawRect(actual_x, actual_y, xToPixel(botright.x())+static_cast<int>(dm_pixper+1)-actual_x,
        yToPixel(botright.y())+static_cast<int>(dm_pixper+1)-actual_y);
  }
}

bool ImagePlot::dataSizeEmpty(void) const
{
  return !(dm_datasize.width() > 0 && dm_datasize.height() > 0);
}

void ImagePlot::drawingMouseMoveEvent(QMouseEvent *event)
{
  if (event->buttons() & Qt::LeftButton)
    handleLeftButton(event, true, true);
  else if (event->buttons() & Qt::RightButton)
    handleRightButton(event, true, true);
  else if (event->buttons() & Qt::MidButton)
    handleMidButton(event, true, true);
  else
    handleMouseMotion(event);
}

void ImagePlot::drawingMousePressEvent(QMouseEvent *event)
{
  dm_menu_click = event->pos();

  if (event->button() == Qt::LeftButton)
    handleLeftButton(event, false, true);
  else if (event->button() == Qt::RightButton)
    handleRightButton(event, false, true);
  else if (event->button() == Qt::MidButton)
    handleMidButton(event, false, true);
}

void ImagePlot::drawingMouseReleaseEvent(QMouseEvent *event)
{
  if (event->button() == Qt::LeftButton)
    handleLeftButton(event, false, false);
  else if (event->button() == Qt::RightButton)
    handleRightButton(event, false, false);
  else if (event->button() == Qt::MidButton)
    handleMidButton(event, false, false);
}

void ImagePlot::onZoomIn(void)
{
  double old_pixper = dm_pixper;

  dm_zoom_fit = false;
  dm_pixper *= 2;

  dm_panpt.rx() += static_cast<int>(dm_area.width()/old_pixper - dm_area.width()/dm_pixper)/2;
  dm_panpt.ry() += static_cast<int>(dm_area.height()/old_pixper - dm_area.height()/dm_pixper)/2;

  setDirty();
}

void ImagePlot::onZoomOut(void)
{
  double old_pixper = dm_pixper;

  dm_zoom_fit = false;
  dm_pixper /= 2;

  dm_panpt.rx() -= static_cast<int>(dm_area.width()/dm_pixper - dm_area.width()/old_pixper)/2;
  dm_panpt.ry() -= static_cast<int>(dm_area.height()/dm_pixper - dm_area.height()/old_pixper)/2;

  setDirty();
}

void ImagePlot::onZoomReset(void)
{
  dm_zoom_fit = true;

  setDirty();
}

void ImagePlot::onZoomSelected(void)
{
  QPoint topleft, botright;

  topleft.rx() = std::min(dm_startpoint.x(), dm_endpoint.x());
  topleft.ry() = std::min(dm_startpoint.y(), dm_endpoint.y());
  botright.rx() = std::max(dm_startpoint.x(), dm_endpoint.x());
  botright.ry() = std::max(dm_startpoint.y(), dm_endpoint.y());

  // see how large the new selection is
  int new_w = botright.x() - topleft.x() + 1;
  int new_h = botright.y() - topleft.y() + 1;
  double alt_pixper;

  // and calculate the new zoom rate
  dm_zoom_fit = false;
  dm_pixper = static_cast<double>(dm_area.width()) / new_w;
  alt_pixper = static_cast<double>(dm_area.height()) / new_h;

  if (alt_pixper < dm_pixper)
    dm_pixper = alt_pixper;

  // prefill the offset so the zoom is in the same spot
  dm_panpt = topleft;
  // center the new view
  if ( dm_area.width()/dm_pixper > new_w)
    dm_panpt.rx() -= static_cast<int>(dm_area.width()/dm_pixper - new_w)/2;
  if ( dm_area.height()/dm_pixper > new_h)
    dm_panpt.ry() -= static_cast<int>(dm_area.height()/dm_pixper - new_h)/2;

  setDirty();
}

void ImagePlot::onHScroll(int newval)
{
  if (!dm_dirty)    // this prevents a feedback loop when we're buildin the bitmap
    dm_panpt.rx() = newval;
  setDirty();
}

void ImagePlot::onVScroll(int newval)
{
  if (!dm_dirty)    // this prevents a feedback loop when we're buildin the bitmap
    dm_panpt.ry() = newval;
  setDirty();
}

void ImagePlot::resetScrollBars(void)
{
  if (dm_zoom_fit) {
    // deactivate scroll bars
    dm_hscroll->setMinimum(0);
    dm_hscroll->setMaximum(0);
    dm_hscroll->setValue(0);
    dm_hscroll->setPageStep(100);
    dm_hscroll->setEnabled(false);
    dm_vscroll->setMinimum(0);
    dm_vscroll->setMaximum(0);
    dm_vscroll->setValue(0);
    dm_vscroll->setPageStep(100);
    dm_vscroll->setEnabled(false);
    return;
  }

  dm_hscroll->setMinimum(static_cast<int>(-dm_area.width()/dm_pixper));
  dm_hscroll->setMaximum(dm_datasize.width());
  dm_hscroll->setPageStep(static_cast<int>(dm_area.width()/dm_pixper));
  dm_hscroll->setValue(dm_panpt.x());
  dm_hscroll->setEnabled(true);

  dm_vscroll->setMinimum(static_cast<int>(-dm_area.height()/dm_pixper));
  dm_vscroll->setMaximum(dm_datasize.height());
  dm_vscroll->setPageStep(static_cast<int>(dm_area.height()/dm_pixper));
  dm_vscroll->setValue(dm_panpt.y());
  dm_vscroll->setEnabled(true);
}

void ImagePlot::updateScrollBars(void)
{
  if (dm_zoom_fit)
    return;

  dm_hscroll->setValue(dm_panpt.x());
  dm_vscroll->setValue(dm_panpt.y());
}

