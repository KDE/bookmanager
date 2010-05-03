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
#include <KTabWidget>
#include <KUrl>
#include <KMimeTypeTrader>
#include <kmimetype.h>

#include "importdialog.h"
#include "shell.h"
#include "readerpage.h"

//PUBLIC
Shell::Shell(QWidget *parent)
        : KParts::MainWindow(parent, Qt::Window)
{
    //need set up the initial tab before the main window
    mainView = new KTabWidget(this);


    m_collect = new Collection(mainView);
    mainView->addTab(m_collect, i18n("Collection"));

    setCentralWidget(mainView);
    setupActions();

    //parent should be mainview as that is that is where the tabs are stored, rather than "this",
    //which is the actual parent as the tab switching is where the actually part changing is going
    //to come from...
    m_manager = new KParts::PartManager(mainView);


    connect(m_collect, SIGNAL(loadBook(KUrl*)),
            this, SLOT(slotReaderTab(KUrl*)));

    //make tabs closable
    mainView->setTabsClosable(true);
    connect(mainView, SIGNAL(tabCloseRequested(int)),
            mainView, SLOT(removeTab(int)));

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
void Shell::slotGetNewStuff()
{

    KNS3::DownloadDialog downDialog(widget());
    downDialog.exec();
    //this will pop up an import dialog for each changed entry that is installed
    //may need to filter for updated entries but I don't think books get updated often
    //without changing the release date and number, which would warrant a new entry anyway maybe?
    //do books have minor version updates?
    foreach(KNS3::Entry  entries, downDialog.changedEntries() ) {
        if (entries.status() == KNS3::Entry::Installed) {
            QString nil = "";//placeholder for empty info
            ImportDialog *impDialog = new ImportDialog(widget());
            connect(impDialog, SIGNAL(signalNewBook(QString,QString,QString,QString,QString,QString,KUrl*)),
                    m_collect, SLOT(createBook(QString,QString,QString,QString,QString,QString,KUrl*)));
            KUrl *location = new KUrl(KUrl(entries.installedFiles().first()));
            impDialog->init(entries.name(), entries.summary(),nil,
                            entries.version(),nil, nil, location);
            impDialog->show();
        }

    }
}

void Shell::slotImport()
{
    ImportDialog *dialog = new ImportDialog(this);
    connect(dialog, SIGNAL(signalNewBook(QString,QString,QString,QString,QString,QString,KUrl*)),
            m_collect, SLOT(createBook(QString,QString,QString,QString,QString,QString,KUrl*)));
    dialog->show();
}

//creates a new tab
void Shell::slotReaderTab(KUrl *url)
{
    //create a tab with the correct reader for the mimetype.
    readerTab(url);
}


//PRIVATE

void Shell::setupActions()
{
    ghns = new KAction(this);
    ghns->setText(i18n("&Get Books From Internet..."));
    ghns->setIcon(KIcon("get-hot-new-stuff"));
    ghns->setShortcut(Qt::Key_G);
    actionCollection()->addAction("ghns", ghns);
    connect( ghns, SIGNAL (triggered(bool)),
             this, SLOT(slotGetNewStuff()));

    import = new KAction(this);
    import->setText(i18n("&Import a new Book"));
    import->setShortcut(Qt::Key_I);
    actionCollection()->addAction("import", import);
    connect(import, SIGNAL(triggered()),
            this, SLOT(slotImport()));

    remove = new KAction(this);
    remove->setText(i18n("&Remove a book"));
    actionCollection()->addAction("remove", remove);
    connect(remove, SIGNAL(triggered()),
            m_collect, SLOT(remBook()));

    KStandardAction::quit ( kapp, SLOT ( quit() ),
                            actionCollection() );
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
        mainView->addTab(curPage, filename );
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
