
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

#include <scopira/qtkit/AboutDialog.h>

#include <scopira/tool/platform.h>

#include <QDialogButtonBox>
#include <QGroupBox>
#include <QGridLayout>
#include <QLabel>
#include <QDebug>

#define COPYRIGHT QString::fromUtf8("\302\251")

using namespace scopira::qtkit;

AboutDialog::AboutDialog(QWidget *parent, const QString &appname)
  : QDialog(parent), dm_appname(appname)
{
  setWindowTitle("About " + appname);

  dm_tabber = 0;

  // initGui
  QDialogButtonBox *buts = new QDialogButtonBox(QDialogButtonBox::Close);

  dm_mainlay = new QVBoxLayout;

  dm_appendlay = dm_mainlay;    // append to the main layout at the start

  dm_nexty = 0;

  dm_mainlay->addStretch();
  dm_mainlay->addWidget(buts);

  setLayout(dm_mainlay);

  connect(buts, SIGNAL(rejected()), this, SLOT(reject()));

  Q_INIT_RESOURCE(qtkit);
}

void AboutDialog::addPixmap(const QPixmap &px)
{
  QLabel *lab = new QLabel;

  lab->setPixmap(px);
  appendWidget(lab, Qt::AlignTop|Qt::AlignHCenter);
}

void AboutDialog::addNRCPixmap(void)
{
  addPixmap(QPixmap(":/scopira/qtkit/nrc_cnrc.png"));
}

void AboutDialog::addTitle(const QString &title,
        const QString &tagline,
        const QString &copyyear, const QString &copywho)
{
  if (title.isEmpty())
    return;

  QVBoxLayout *lay = new QVBoxLayout;
  QLabel *lab;

  lab = new QLabel;
  if (title[0] == ':')
    lab->setPixmap(QPixmap(title));
  else {
    lab->setText("<h1>" + title + "</h1>");
  }
  lay->addWidget(lab, 0, Qt::AlignTop|Qt::AlignHCenter);

  if (!tagline.isEmpty())
    lay->addWidget(new QLabel("<h3>" + tagline + "</h3>"),
        0, Qt::AlignTop|Qt::AlignHCenter);

  if (!copyyear.isEmpty()) {
    lay->addWidget(new QLabel(
      "Copyright " + COPYRIGHT + copyyear + " " + copywho),
        0, Qt::AlignTop|Qt::AlignHCenter);
  }

  dm_appendlay->insertLayout(dm_nexty++, lay);
}

void AboutDialog::addContact(const QString &contact, const QString &email, const QString &web)
{
  QGroupBox *gb = new QGroupBox;
  QGridLayout *grid = new QGridLayout;
  int y = 0;
  QLabel *lab;

  grid->setColumnStretch(1, 1);

  if (!contact.isEmpty()) {
    grid->addWidget(new QLabel("Contact:"), y, 0, Qt::AlignLeft|Qt::AlignTop);
    grid->addWidget(new QLabel(contact), y, 1, Qt::AlignLeft|Qt::AlignTop);
    ++y;
  }
  if (!email.isEmpty()) {
    grid->addWidget(new QLabel("E-Mail:"), y, 0, Qt::AlignLeft|Qt::AlignTop);
    grid->addWidget(new QLabel("<a href=\"" + email + "\">" + email + "</a>"), y, 1, Qt::AlignLeft|Qt::AlignTop);
    ++y;
  }
  if (!web.isEmpty()) {
    grid->addWidget(new QLabel("Web site:"), y, 0, Qt::AlignLeft|Qt::AlignTop);
    grid->addWidget(lab = new QLabel("<a href=\"" + web
          + "\">" + web + "</a>"), y, 1, Qt::AlignLeft|Qt::AlignTop);
    lab->setOpenExternalLinks(true);
    ++y;
  }

  gb->setTitle("Contact Information");
  gb->setLayout(grid);

  appendWidget(gb);
}

void AboutDialog::addVersion(const QString &version, const QString &flags, bool showqtversion, bool showstockflags, bool showbuildtime)
{
  QGroupBox *gb = new QGroupBox;
  QGridLayout *grid = new QGridLayout;
  int y = 0;

  grid->setColumnStretch(1, 1);

  if (!version.isEmpty()) {
    grid->addWidget(new QLabel("Version:"), y, 0, Qt::AlignLeft|Qt::AlignTop);
    grid->addWidget(new QLabel(version
          + (showqtversion?" (Qt " QT_VERSION_STR ")":"")
          ), y, 1, Qt::AlignLeft|Qt::AlignTop);
    ++y;
  }
  if (showbuildtime) {
    grid->addWidget(new QLabel("Build time:"), y, 0, Qt::AlignLeft|Qt::AlignTop);
    grid->addWidget(new QLabel(__DATE__ " " __TIME__), y, 1, Qt::AlignLeft|Qt::AlignTop);
    ++y;
  }

  QString curflags;

  if (showstockflags) {
    curflags =
#ifndef NDEBUG
      "Debug "
#else
      "Optimized "
#endif
#ifdef PLATFORM_64
      "64-bit "
#endif
#ifdef PLATFORM_32
      "32-bit "
#endif
      ;
  }
  if (!flags.isEmpty())
    curflags += flags;
  if (!curflags.isEmpty()) {
    grid->addWidget(new QLabel("Options:"), y, 0, Qt::AlignLeft|Qt::AlignTop);
    grid->addWidget(new QLabel(curflags), y, 1, Qt::AlignLeft|Qt::AlignTop);
    ++y;
  }

  gb->setTitle("Version Details");
  gb->setLayout(grid);

  appendWidget(gb);
}

void AboutDialog::addParagraph(const QString &title, const QString &value)
{
  QGroupBox *gb = new QGroupBox;
  QGridLayout *grid = new QGridLayout;

  grid->setColumnStretch(0, 1);

  QLabel *lab;
  grid->addWidget(lab = new QLabel(value), 0, 0, Qt::AlignLeft|Qt::AlignTop);

  lab->setWordWrap(true);

  gb->setTitle(title);
  gb->setLayout(grid);

  appendWidget(gb);
}

void AboutDialog::addDisclaimer(void)
{
  addParagraph("Disclaimer",
      "In no event shall the copyright owners or contributors be liable for any "
      "direct, indirect, incidental or consequential damages arrising from the use of "
      "this software, even if advised of the possibility of such damage.");
}

void AboutDialog::addCopyright(const QString &year, const QString &bywhom)
{
  addParagraph("Copyright Notice",
      "This software and associated materials are\n\n"
      "Copyright " + COPYRIGHT + year + " " + bywhom +
      "\nAll rights reserved.");
}

void AboutDialog::addTab(const QString &tabname)
{
  if (!dm_tabber) {
    dm_tabber = new QTabWidget;

    dm_mainlay->insertWidget(dm_nexty++, dm_tabber);
  }

  QWidget *w = new QWidget;
  dm_appendlay = new QVBoxLayout;

  dm_appendlay->addStretch();
  dm_nexty = 0;

  w->setLayout(dm_appendlay);

  dm_tabber->addTab(w, tabname);
}

void AboutDialog::appendWidget(QWidget *w, Qt::Alignment a)
{
  //dm_appendlay->insertWidget(dm_nexty++, w);//, 0, Qt::AlignHCenter|Qt::AlignTop);
  dm_appendlay->insertWidget(dm_nexty++, w, 0, a);
  //dm_appendlay->insertWidget(dm_nexty++, w, 0, Qt::AlignHCenter|Qt::AlignTop);
}

