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
#include <KNS3/DownloadDialog>
#include <KNS3/Entry>
#include <KTabWidget>
#include <KUrl>
#include <KMimeTypeTrader>
#include <kmimetype.h>
#include <kfiledialog.h>

#include "importdialog.h"

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
void Shell::slotGetNewStuff()
{

    QPointer<KNS3::DownloadDialog> downDialog = new KNS3::DownloadDialog(this);
    downDialog->exec();
    //this will pop up an import dialog for each changed entry that is installed
    //may need to filter for updated entries but I don't think books get updated often
    //without changing the release date and number, which would warrant a new entry anyway maybe?
    //do books have minor version updates?
    foreach(const KNS3::Entry &entries, downDialog->changedEntries()) {
        if (entries.status() == KNS3::Entry::Installed) {
            QString nil = "";//placeholder for empty info
            ImportDialog *impDialog = new ImportDialog(widget());
            connect(impDialog, SIGNAL(signalNewBook(QString, QString, QString, QString, QString, QString, KUrl*)),
                    m_collect, SLOT(createBook(QString, QString, QString, QString, QString, QString, KUrl*)));
            KUrl *location = new KUrl(KUrl(entries.installedFiles().first()));
            impDialog->init(entries.name(), entries.summary(), nil,
                            entries.version(), nil, nil, location);
            impDialog->show();
        }

    }
    delete downDialog;
}

void Shell::slotImport()
{
    ImportDialog *dialog = new ImportDialog(this);
    connect(dialog, SIGNAL(signalNewBook(QString, QString, QString, QString, QString, QString, KUrl*)),
            m_collect, SLOT(createBook(QString, QString, QString, QString, QString, QString, KUrl*)));
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
    //File menu
    open = KStandardAction::open(this, SLOT(slotOpenFile()), actionCollection());
    //add the action manually so it doesn't add a toolbar action
    actionCollection()->addAction("open", open);

    ghns = new KAction(this);
    ghns->setText(i18n("&Get Books From Internet..."));
    ghns->setIcon(KIcon("get-hot-new-stuff"));
    ghns->setShortcut(Qt::Key_G);
    actionCollection()->addAction("ghns", ghns);
    connect(ghns, SIGNAL(triggered(bool)),
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

    KStandardAction::quit(kapp, SLOT(quit()),
                          actionCollection());
    //Window menu
    showCollection = new KToggleAction(this);
    showCollection->setText(i18n("Collection Manager"));
    //FIXME make this remember state from last use
    showCollection->setChecked(true); //show the collection by default
    actionCollection()->addAction("showCollection", showCollection);
    connect(showCollection, SIGNAL(triggered()),
            this, SLOT(slotToggleCollection()));

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

void Shell::slotToggleCollection()
{
    //open and close the collection tab
    if (showCollection->isChecked()) {
        mainView->addTab(m_collect, i18n("Collection"));
    } else {
        mainView->removeTab(mainView->indexOf(m_collect));
    }
}

void Shell::slotRemoveTab(int index)
{
    //check to see if we are closing the collection tab, so we can toggle the toggle
    if (index == mainView->indexOf(m_collect)) {
        showCollection->setChecked(false);
        slotToggleCollection();
    } else {
        mainView->removeTab(index);
    }
}

void Shell::slotOpenFile()
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