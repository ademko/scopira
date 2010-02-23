
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

#ifndef __INCLUDED_SCOPIRA_QTKIT_ABOUTDIALOG_H__
#define __INCLUDED_SCOPIRA_QTKIT_ABOUTDIALOG_H__

#include <QDialog>
#include <QVBoxLayout>
#include <QTabWidget>

namespace scopira
{
  namespace qtkit
  {
    class AboutDialog;
  }
}

/**
 * A handy about box builder
 *
 * @author Aleksander Demko
 */ 
class scopira::qtkit::AboutDialog : public QDialog
{
  public:
    /// constructor
    AboutDialog(QWidget *parent, const QString &appname);

    void addPixmap(const QPixmap &px);

    void addNRCPixmap(void);

    /**
     * If title starts with :, it will be loaded as a resource image
     * instead.
     *
     * Either/or tagline and copyyear may be "".
     *
     * @author Aleksander Demko
     */ 
    void addTitle(const QString &title,
        const QString &tagline = "",
        const QString &copyyear = "", const QString &copywho = "");

    //addTitle(QString &

    void addContact(const QString &contact, const QString &email = "", const QString &web = "");
    void addVersion(const QString &version, const QString &flags = "", bool showqtversion = true, bool showstockflags = true, bool showbuildtime = true);
    void addParagraph(const QString &title, const QString &value);

    void addDisclaimer(void);
    void addCopyright(const QString &year, const QString &bywhom);

    void addTab(const QString &tabname);

  private:
    void appendWidget(QWidget *w, Qt::Alignment a = 0);

  private:
    QString dm_appname;

    QVBoxLayout *dm_mainlay, *dm_appendlay;
    QTabWidget *dm_tabber;
    int dm_nexty;
};

#endif

