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
#include "shell.h"

#include <KApplication>
#include <KAction>
#include <KLocale>
#include <KActionCollection>
#include <KStandardAction>
#include <KMessageBox>
#include <KDebug>
#include <KTabWidget>
#include <KUrl>
#include <KMimeTypeTrader>
#include <kmimetype.h>
#include <kfiledialog.h>


#include "readerpage.h"

//PUBLIC
Shell::Shell(QWidget *parent)
        : KParts::MainWindow(parent, Qt::Window)
{
    //need set up the initial tab before the main window
    mainView = new KTabWidget(this);
    setCentralWidget(mainView);

    setupActions();
    
    //this is probably wrong FIXME
    m_manager = new KParts::PartManager(mainView);

    //make tabs closable
    mainView->setTabsClosable(true);
    connect(mainView, SIGNAL(tabCloseRequested(int)),
            this, SLOT(slotRemoveTab(int)));
    
    //make the menu's change when the part does, except it doesn't seem to work :( FIXME
    m_manager->setAllowNestedParts(true);
    connect(mainView, SIGNAL(currentChanged(int)),
	    this, SLOT(slotUpdateMenu(int)));
    
    connect(m_manager, SIGNAL(activePartChanged(KParts::Part*)),
            this, SLOT(createGUI(KParts::Part*)));
}
Shell::~Shell()
{
//TODO
}


// PRIVATE SLOTS
//creates a new tab
void Shell::slotReaderTab(KUrl *url)
{
    //create a tab with the correct reader for the mimetype.
    readerTab(url);
}


//PRIVATE

void Shell::setupActions()
{
    //File menu
    open = KStandardAction::open(this, SLOT(slotOpenFile()), actionCollection());
    actionCollection()->addAction("open", open);
    
    openNewTab = new KAction(this);
    openNewTab->setText(i18n("Open file(s) in new tab(s)"));
    actionCollection()->addAction("openNewTab", openNewTab);
    connect(openNewTab, SIGNAL(triggered(bool)),
        this, SLOT(slotOpenFileNewTab()));

    KStandardAction::quit(kapp, SLOT(quit()),
                          actionCollection());

    setupGUI();
}


void Shell::readerTab(const KUrl *url)
{
    //reader page will open appropriate kpart based on the url, and
    //using partmanager will keep the menus current for the displayed tab
    ReaderPage * curPage = new ReaderPage(url, this);
    if (curPage) {
        m_manager->addPart(curPage->getPart());
        QString filename = url->fileName();
        mainView->addTab(curPage, filename);
    }

}

void Shell::slotRemoveTab(int index)
{
        mainView->removeTab(index);
}
void Shell::slotOpenFile()
{
    //TODO
}

void Shell::slotOpenFileNewTab()
{
    foreach(const QString &filename, KFileDialog::getOpenFileNames()) {
        KUrl tempUrl = filename;
        slotReaderTab(&tempUrl);
    }
}

void Shell::slotUpdateMenu(int index)
{
  if(index > 0) {
    ReaderPage *curPage = qobject_cast<ReaderPage *>(mainView->widget(index)); 
    KParts::ReadOnlyPart *curpart = curPage->getPart();
    setupGUI(Keys | ToolBar | Save);
    m_manager->setActivePart(curpart);
  }
}