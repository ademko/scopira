
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

#ifndef __INCLUDED_SCOPIRA_QTKIT_PLOT2D_H__
#define __INCLUDED_SCOPIRA_QTKIT_PLOT2D_H__

#include <QWidget>

#include <scopira/basekit/narray.h>

namespace scopira
{
  namespace qtkit
  {
    /**
     * Auto calculate some nice tick marks.
     * This handy function can be used in other plotter like things too.
     *
     * @param min the start of the range in question
     * @param max the end of the range
     * @param axis_min (output) the first tick mark
     * @param axis_max (output) the last tick mark
     * @param tick_spacing (output) the spacing between the tick marks
     * @return number of tick marks
     * @author Aleksander Demko
     */ 
    int calcTickMarks(double min, double max, double &axis_min, double &axis_max, double &tick_spacing);

    class Plot2D;
  }
}

/**
 * A generic plot2d data class.
 *
 * A plot contains one or more datasets. Each dataset is a vector of (x,y) points.
 * Use ResizeData to setup the number of datasets, then SetData to load them.
 *
 * @author Aleksander Demko
 */ 
class scopira::qtkit::Plot2D : public QWidget
{
    Q_OBJECT

  public:
    Plot2D(QWidget *parent = 0);

    /**
     * Sets the label for the X axis.
     *
     * @author Aleksander Demko
     */ 
    void setXLabel(const QString &l);
    /**
     * Sets the label for the Y axis.
     *
     * @author Aleksander Demko
     */ 
    void setYLabel(const QString &l);
    /**
     * Sets the overall title of the plot.
     *
     * @author Aleksander Demko
     */ 
    void setTitle(const QString &l);

    /**
     * Sets the displayed X range to be automatically
     * calculated from the data.
     * This is the default.
     *
     * @author Aleksander Demko
     */
    void setXRangeAuto(void);
    /**
     * Explicitly set the displayed range of X values.
     *
     * @author Aleksander Demko
     */ 
    void setXRange(double min, double max);

    /**
     * Sets the displayed Y range to be automatically
     * calculated from the data.
     * This is the default.
     *
     * @author Aleksander Demko
     */
    void setYRangeAuto(void);
    /**
     * Explicitly set the displayed range of Y values.
     *
     * @author Aleksander Demko
     */ 
    void setYRange(double min, double max);

    // set labels
    // add data

    static scopira::basekit::narray<double> labelValuesIndex(size_t datalen);
    static scopira::basekit::narray<double> labelValuesMinMax(size_t datalen, double min, double max);

    /**
     * Set the number of data sets to plot.
     * This will destroy all the existing data.
     * You should call setdata to actually set the data elements after this.
     *
     * @author Aleksander Demko
     */ 
    void resizeData(int numdata);

    /**
     * Sets the data for one particlar set.
     * The Plot2D will make a copy of the given slice data.
     *
     * @param is the data set number (0 is the irst one)
     * @param xvalues the x values of the dataset
     * @param yvalues the y values of the dataset
     * @author Aleksander Demko
     */ 
    void setData(int id, scopira::basekit::const_nslice<double> xvalues, scopira::basekit::const_nslice<double> yvalues);

    // convert to screen coords, -1 on error
    int xToPixel(double x);
    int yToPixel(double y);

    // converts screen to outval, returns true on success
    bool xToPlot(int screenx, double &outval);
    bool yToPlot(int screeny, double &outval);

  protected:
    // stuff descendants can override

    /**
     * This is called before drawing the plot bars, buf after drawing the frame and any backgrounds.
     * This implementation does nothing.
     *
     * @author Aleksander Demko
     */ 
    virtual void handlePrePaint(QPainter &dc);
    /**
     * This is called after drawing the plot bars.
     * This implementation does nothing.
     *
     * @author Aleksander Demko
     */ 
    virtual void handlePostPaint(QPainter &dc);

    /**
     * This is called to draw each line in the plot.
     * Descendants may override this.
     * This default version simply called QPainter.DrawLine.
     *
     * dataidy is the dataset id (y)
     * indexx is the point within that dataset
     * xvalue and yvalue are the real values of the oint
     * x1, y1, x2, y2 are the screen coordinates of the line to draw
     *
     * @author Aleksander Demko
     */ 
    virtual void handleLinePaint(int indexx, int dataidy, double xvalue, double yvalue,
        QPainter &dc, int x1, int y1, int x2, int y2);

    /**
     * This is called to draw the points themselves in the graph.
     * Descendants may override this.
     * This default version doesn't draw anything. DrawCircle are simple, popular alternatives.
     *
     * dataidy is the dataset id (y)
     * indexx is the point within that dataset
     * xvalue and yvalue are the real values of the oint
     * x1, y1 are the screen coordinates of the point
     *
     * @author Aleksander Demko
     */ 
    virtual void handlePointPaint(int indexx, int dataidy, double xvalue, double yvalue, QPainter &dc, int x, int y);

    /**
     * This is called when rendering labels on the screen, it returns the
     * number to be displayed (screen) given the (data) number.
     * The default function simply returns the value, unchanged.
     *
     * @author Aleksander Demko
     */ 
    virtual double handleXLabel(double xdata);

    /**
     * This is called when rendering labels on the screen, it returns the
     * number to be displayed (screen) given the (data) number.
     * The default function simply returns the value, unchanged.
     *
     * @author Aleksander Demko
     */ 
    virtual double handleYLabel(double ydata);

    const QPoint & pixelOffset(void) const { return dm_offset; }
    const QSize & pixelArea(void) const { return dm_area; }

  private:
    virtual void resizeEvent(QResizeEvent *event);
    virtual void paintEvent(QPaintEvent *event);

    void setDirty(void);


    void reCalcGui(QPainter &dc);

    void drawFrame(QPainter &dc);
    void drawData(QPainter &dc);

    /**
     * Find the commended number of labels to have.
     *
     * (not using after all, can remove)
     * @author Aleksander Demko
     */
    int calcNumLabels(bool xaxis);

  private:
    const static int border = 40;

    struct Data {
      scopira::basekit::narray<double> xvalues, yvalues;
      // other properties in the future
    };
    typedef std::vector<Data> datavec_t;

    bool dm_dirty;

    QPoint dm_offset;
    QSize dm_area;

    datavec_t dm_data;

    QString dm_title, dm_xlabel, dm_ylabel;

    bool dm_emptydata, dm_xrange_auto, dm_yrange_auto;
    double dm_data_xmin, dm_data_xmax, dm_data_ymin, dm_data_ymax;
    double dm_view_xmin, dm_view_xmax, dm_view_ymin, dm_view_ymax;
};

#endif


