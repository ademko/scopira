
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

#ifndef __INCLUDED_SCOPIRA_QTKIT_IMAGEPLOT_H__
#define __INCLUDED_SCOPIRA_QTKIT_IMAGEPLOT_H__

#include <QWidget>
#include <QMenu>
#include <QScrollBar>

namespace scopira
{
  namespace qtkit
  {
    /**
     * Calculates aspect ratios.
     *
     * @param C current columns
     * @param R current rows
     * @param WC wanted columns
     * @param WR wanted rows
     * @param c output, calculated columns
     * @param r output, calculated rows
     * @author Aleksander Demko
     */ 
    void calcAspectRatio(int C, int R, int WC, int WR, int &c, int &r);

    static inline QSize makeSize(ptrdiff_t w, ptrdiff_t h) { return QSize(static_cast<int>(w), static_cast<int>(h)); }
    static inline QPoint makePoint(ptrdiff_t x, ptrdiff_t y) { return QPoint(static_cast<int>(x), static_cast<int>(y)); }

    class ImagePlot;
  }
}

/**
 * A base class for making widgets that view "widgets".
 * Additional functionality includes scrolling and tagging, etc.
 *
 * @author Aleksander Demko
 */ 
class scopira::qtkit::ImagePlot : public QWidget
{
    Q_OBJECT

  public:
    const static int BORDER = 20;

  protected:
    /// constructor
    ImagePlot(QWidget *parent = 0);

  protected:
    // Pixel is in screen-pixel coordinates
    // Plot is in data coordinates

    /**
     * Returns the actual drawing area widget.
     *
     * You can call update() on this to redraw the actual
     * area (useful on mouse moves)
     *
     * @author Aleksander Demko
     */ 
    QWidget * drawingArea(void) const;

    void updateDrawingArea(void) const { drawingArea()->update(); }

    /// the offset from top-left that the bitmap area starts
    const QPoint & pixelOffset(void) const { return dm_offset; }
    /// the size of the bitmap area
    const QSize & pixelArea(void) const { return dm_area; }
    /// the mouse click location on right down
    const QPoint & pixelMenuClick(void) const { return dm_menu_click; }

    /**
     * Sets the selection color.
     * Caller must call setDirty or something after.
     * Should this be public?
     *
     * @author Aleksander Demko
     */ 
    void setSelectionColor(const QColor &col);
    void setPointColor(const QColor &col);
    void setBackgroundColor(const QColor &col);

    QColor selectionColor(void) const { return dm_color_selection; }
    QColor pointColor(void) const { return dm_color_point; }
    QColor backgroundColor(void) const { return dm_color_bg; }

    /**
     * Is there a selection point currently active/selected?
     *
     * @author Aleksander Demko
     */ 
    bool hasPlotSelectionPoint(void) const;

    /**
     * Get the selection point (one point cross hair selection thingie)
     *
     * @author Aleksander Demko
     */ 
    const QPoint & getPlotSelectionPoint(void);

    /**
     * Manually set the selection point, often the result
     * of some other selection in another widget, or whatever.
     *
     * @author Aleksander Demko
     */ 
    void setPlotSelectionPoint(const QPoint &plotpoint);
    /**
     * Clears the plot selection point.
     *
     * @author Aleksander Demko
     */ 
    void clearPlotSelectionPoint(void);

    /**
     * Sets the dirty flag and requests a refresh.
     *
     * This can be called from decendants' constructors.
     *
     * @author Aleksander Demko
     */ 
    void setDirty(void);

    /**
     * Called by decent classes when they have a dataset.
     * This class needs to know its dimensions so that proper offset
     * calculations can be done. Setting w=0 and h=0 says "I don't have any
     * data right now".
     *
     * This also triggers a call to SetDirty.
     *
     * This can be called from decendants' constructors.
     *
     * @author Aleksander Demko
     */
    void setPlotSize(QSize sz);

    // convert pixel coords to in-array
    // returns true on success, ie the point is in the box
    bool xToPlot(int screenx, int &datax);
    bool yToPlot(int screeny, int &datay);

    bool xyToPlot(const QPoint &screenp, QPoint &plotp) {
      return xToPlot(screenp.x(), plotp.rx()) && yToPlot(screenp.y(), plotp.ry());
    }

    // converts from in-array data coords to screen widget-pixel coords
    int xToPixel(int datax);
    int yToPixel(int datay);

    QPoint xyToPixel(const QPoint &plotp) {
      return QPoint(xToPixel(plotp.x()), yToPixel(plotp.y()));
    }

    /**
     * Builds the standard menu.
     *
     * @return true if any menu entries where added
     * @author Aleksander Demko
     */ 
    bool buildZoomMenu(QMenu &menu);

    /**
     * Called by OnPaint when the screen is "dirty".
     * The subclass must override this and provide the drawing logic.
     * backbuffer should be resized to Area() dimensions.
     *
     * @return the packedrgb colour for this cell
     * @author Aleksander Demko
     */ 
    virtual int handleGetDataAsRGB(int x, int y) = 0;

    /// called before full painting
    /// this implementation does nothing
    virtual void handlePrePaint(QPainter &dc);
    /// called after full painting
    /// this implementation does nothing
    virtual void handlePostPaint(QPainter &dc);
    /**
     * This is called first, before any painting is done.
     * If it returns true (meaning that this function did something),
     * then no other painting is performed.
     * This is useful for quickly drawing parts of the widget.
     * The default implementation always returns false.
     *
     * @author Aleksander Demko
     */ 
    virtual bool handleQuickPaint(QPainter &dc);

    /**
     * Called when the left button is pressed, released and moved.
     *
     * The default implementation handles the single pixel and rectangle selection.
     *
     * @param event the mouse event
     * @param isdrag true if we're in a motion event while the button is down (a drag)
     * @param isdown for non-drag events, this will be true upon click-press and false
     * on click-release. During drag events this is always true
     * @author Aleksander Demko
     */ 
    virtual void handleLeftButton(QMouseEvent *event, bool isdrag, bool isdown);

    /**
     * This is called when the right mouse button is pressed.
     *
     * The default version does nothing (and defers to the default Qt contextMenuEvent system)
     *
     * @author Aleksander Demko
     */ 
    virtual void handleRightButton(QMouseEvent *event, bool isdrag, bool isdown);

    /**
     * Called when the middle button is pressed, released and moved.
     *
     * The default implementation handles the motion and panning of the
     * image.
     *
     * @param event the mouse event
     * @param isdrag true if we're in a motion event while the button is down (a drag)
     * @param isdown for non-drag events, this will be true upon click-press and false
     * on click-release. During drag events this is always true
     * @author Aleksander Demko
     */ 
    virtual void handleMidButton(QMouseEvent *event, bool isdrag, bool isdown);
    /**
     * Called when on mouse movement events when no buttons are down.
     *
     * This default version does nothing.
     *
     * @author Aleksander Demko
     */
    virtual void handleMouseMotion(QMouseEvent *event);

    /**
     * If the default left mouse click handling is used, then
     * this will be called when a new single-point selection has been
     * made.
     *
     * The default implementation does nothing.
     *
     * @author Aleksander Demko
     */ 
    virtual void handlePlotSelectionPoint(const QPoint &plotpoint);

    /**
     * Called by Qt to pop up a menu.
     *
     * The default implementation pops up a basic menu build with buildZoomMenu();
     *
     * @author Aleksander Demko
     */ 
    virtual void contextMenuEvent(QContextMenuEvent * event);

  protected:
    // these events handle the mouse/paint events of the actual drawing area child widget
    // descendans may override them if needed

    virtual void drawingResizeEvent(QResizeEvent *event);
    virtual void drawingPaintEvent(QPaintEvent *event);

    virtual void drawingMouseMoveEvent(QMouseEvent *event);
    virtual void drawingMousePressEvent(QMouseEvent *event);
    virtual void drawingMouseReleaseEvent(QMouseEvent *event);

  private slots:
    void onZoomIn(void);
    void onZoomOut(void);
    void onZoomReset(void);
    void onZoomSelected(void);

    void onHScroll(int newval);
    void onVScroll(int newval);

  private:
    /// returns true on ok dimensions
    bool updateBitmap(QPainter &dc);
    void updateCroppedBitmap(void);
    void drawSelection(QPainter &dc);

    bool dataSizeEmpty(void) const;

    void resetScrollBars(void);
    void updateScrollBars(void);

  private:
    class Drawing;

    // widgets
    Drawing *dm_drawing;
    QScrollBar *dm_hscroll, *dm_vscroll;

    QColor dm_color_selection, dm_color_point, dm_color_bg;

    bool dm_dirty;
    
    QSize dm_datasize;     // the size of the user's data set

    QPoint dm_offset;      // the offset of the full area (with aspect side bars)
    QSize dm_area;         // the full area that is usable by the backbuffer, which may include
                            // 'side' or 'top' bars if the aspect ration requires it

    // menu stuff
    QPoint dm_menu_click;  // the mouse location upon right-mouse button click

    // zoom/scrolling stuff
    bool dm_zoom_fit;                 // auto fit data to window
    double dm_pixper;                 // pixels per data point (auto calculated if dm_zoom_fit is set)

    QPoint dm_panpt;   // the top-left corner (in data-matrix coords) of the currently selected/panned window

    // select point and select rectangle stuff
    // current "select" point (possibly part of a rectangle)
    bool dm_has_select, dm_is_drawingrect;
    QPoint dm_startpoint;    // in data-set coordinates
    // the last point in a rect, if any
    bool dm_has_endpoint;  // if this is set, then we have a full rect and not just one selected point (set when the drag operation is done)
    QPoint dm_endpoint;    // in data-set coordinates

    // mouse stuff
    // the start of a middle click panning operation, both the screen location and the data
    bool dm_is_panning;
    QPoint dm_panning_anchor_screen;
    QPoint dm_panning_anchor_data;

    QImage dm_backbuffer;   // matches the on screen buffer, always of size dm_area (so it includes the side bars, if any)
};

#endif

