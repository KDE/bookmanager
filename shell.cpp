/*
    Copyright (C) <2010>  Brian Korbein <bri.kor.21@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/
#include <KApplication>
#include <KAction>
#include <KLocale>
#include <KActionCollection>
#include <KStandardAction>
#include <KMessageBox>
#include <KDebug>
#include <KNS3/DownloadDialog>
#include <KNS3/Entry>



#include "shell.h"
//PUBLIC
Shell::Shell(QWidget* parent)
  : MainWindow(parent)
{
  m_collect = new Collection(this);
  setCentralWidget(m_collect);
  
  setupActions();
}
Shell::~Shell()
{
//TODO
}


// PRIVATE SLOTS
void Shell::slotGetNewStuff()
{

    KNS3::DownloadDialog downDialog(widget());
    downDialog.exec();
    // show the modal dialog over pageview and execute it
    foreach(KNS3::Entry  entries, downDialog.changedEntries() ){
        kDebug() << "Changed Entry: " << entries.name();
    }
}

//PRIVATE

void Shell::setupActions()
{
    KAction *ghns = new KAction(this);
    ghns->setText(i18n("&Get Books From Internet..."));
    ghns->setIcon(KIcon("get-hot-new-stuff"));
    ghns->setShortcut(Qt::Key_G);
    actionCollection()->addAction("ghns", ghns);
    connect( ghns, SIGNAL (triggered(bool)),
             this, SLOT(slotGetNewStuff()));
	     
    KStandardAction::quit ( kapp, SLOT ( quit() ),
                            actionCollection() );
    setupGUI();
}
