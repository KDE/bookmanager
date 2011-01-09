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
    
    //make sure the partmanager is connected before we start loading parts!
    connect(mainView, SIGNAL(tabCloseRequested(int)),
            this, SLOT(slotRemoveTab(int)));
    
    m_manager->setAllowNestedParts(true);
    connect(mainView, SIGNAL(currentChanged(int)),
	    this, SLOT(slotUpdateMenu(int)));
    
    connect(m_manager, SIGNAL(activePartChanged(KParts::Part*)),
            this, SLOT(createGUI(KParts::Part*)));
    
    
    //check if the collection toggle is turned on, load the collection if it is
    
    if(showCollection->isChecked()){
        loadCollection();
    }
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

void Shell::slotRemoveTab(int index)
{
        mainView->removeTab(index);
}
void Shell::slotOpenFile()
{
    //FIXME this is getting far to deeply indented... maybe some of it should be pulled out?
    //check if there is an open tab...
    if(mainView->currentIndex() == -1){
        //no tabs open so we can just use the openFileNewTab slot :D
        slotOpenFileNewTab();
    } else {
        //check to see if the collection manager is open, and if thats our current tab
        //since we can't open files with the collection manager...
        if(showCollection->isChecked() && (mainView->currentIndex() == 0)){
            //we're trying to open things in the collection... which is bad, so use a new tab :D
            slotOpenFileNewTab();
        } else {
            //we have an open reader tab so...
            //first get the filename from the user
            QString filename =  KFileDialog::getOpenFileName();
            //check if the filename is null, open the file if its NOT null
            if(!filename.isNull()){
                KUrl tempUrl = filename;
                //might be a better way to get the part, but it works elsewhere so...       
                ReaderPage *curPage = qobject_cast<ReaderPage *>(mainView->widget(mainView->currentIndex())); 
                //this is kind of naive as most kparts support multiple mimetypes
                //but i couldn't find an easy way to get a list of supported types
                //from a kpart... FIXME
                QString newMimeType = KMimeType::findByUrl(tempUrl)->name();
                KParts::ReadOnlyPart *curpart = curPage->getPart();
                if(curPage->getMimeType() == newMimeType){
                    curpart->openUrl(tempUrl);
                } else {
                    //if the mimetype has changed we need to delete the currentpage and open a replacement
                    ReaderPage *newPage = new ReaderPage(&tempUrl, this);
                    if(newPage) {
                        int index = mainView->currentIndex();
                        mainView->removeTab(index);
                        mainView->insertTab(index, newPage, tempUrl.fileName());
                        m_manager->replacePart(curpart, newPage->getPart());
                        delete(curPage);
                    }
                }
            }
        }
    }
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

void Shell::slotToggleCollection()
{
    //open and close the collection tab
    if(showCollection->isChecked()){
        loadCollection();
    } else {
        mainView->removeTab( mainView->indexOf(m_collection->widget()) );
    }
}

void Shell::loadCollection()
{
    //load the collection manager by name, like the okular shell does
    //since we don't really have any other way to find the part
    KPluginFactory *factory = KPluginLoader("bookmanagerpart").factory();
    if(!factory){
        //alert the user of failure
        KMessageBox::error(this, i18n("Unable to load the collection."));
        //then return
        m_collection = 0;
        return;
    }
    //if we get to here then everything is working :D and we can cast our part into a part
    m_collection = factory->create<KParts::Part>(this);
    if(m_collection){
        //and now we can perform some setup for our collection so the user can actually use it :D
        //first we need to tell the part manager that we have a new part for it
        m_manager->addPart(m_collection);
        //then we stuff the collection into a tab...
        //always use tab 0 for the collection, this makes it easy to check if the collection is open, 
        //and find it so we don't have to do mainview->indexof... everytime we open something to track
        //it down and make sure we aren't clobbering it with file->open, which causes a crash :(
        mainView->insertTab(0, m_collection->widget(), i18n("Collection"));
        //TODO set up communication between the part and the shell
        //so we can actually open files with it :D
    }
}










