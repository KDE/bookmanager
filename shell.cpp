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

#include <qdesktopwidget.h>
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
#include <kinputdialog.h>
#include <QDBusConnection>
#include <QDBusInterface>

#include "readerpage.h"
#include <qdbusconnection.h>
#include "bookmanagerconfig.h"
#include <kparts/browserextension.h>
#include <kstandarddirs.h>

//PUBLIC
Shell::Shell(QWidget *parent)
    : KParts::MainWindow(parent, Qt::Window)
{
    //need set up the initial tab before the main window
    mainView = new KTabWidget(this);
    setCentralWidget(mainView);
    
    // set up the path of session files
    m_sessionDirPath = KStandardDirs::locateLocal("appdata", "sessions/");

    setupActions();

    m_manager = new KParts::PartManager(mainView);

    //make tabs closable
    mainView->setTabsClosable(true);

    //zero out the collection, and curPart so it can be properly checked for existence later on
    m_collection = 0;
    curPart = 0;

    connect(mainView, SIGNAL(tabCloseRequested(int)),
            this, SLOT(slotRemoveTab(int)));

    m_manager->setAllowNestedParts(true);
    connect(mainView, SIGNAL(currentChanged(int)),
            this, SLOT(slotUpdateMenu(int)));

    //make sure the partmanager is connected before we start loading parts!
    connect(m_manager, SIGNAL(activePartChanged(KParts::Part*)),
            this, SLOT(createGUI(KParts::Part*)));
    connect(m_manager, SIGNAL(activePartChanged(KParts::Part*)),
            this, SLOT(slotPartConnect(KParts::Part*)));

    //check if the collection toggle is turned on in the config, load the collection if it is
    showCollection->setChecked(BookManagerConfig::collection());
    if (showCollection->isChecked()) {
        loadCollection();

    }
}
Shell::~Shell()
{
    if (m_collection) {
        m_collection->deleteLater();
    }
    if (m_manager) {
        m_manager->deleteLater();
    }
    mainView->deleteLater();
}


QSize Shell::sizeHint() const
{
    return QApplication::desktop()->availableGeometry(this).size() * 0.75;
}

// PRIVATE SLOTS
//creates a new tab
void Shell::slotReaderTab(KUrl *url)
{
    //create a tab with the correct reader for the mimetype.
    readerTab(url);
}


//PRIVATE

void Shell::slotPartConnect(KParts::Part* newPart)
{
    //if the currentpart is set, disconnect it before initializing the new part
    if (curPart) {
        slotPartDisconnect(curPart->browserExtension());
    }

    //cast the part into a readonly part, this should be safe so long as we never load a
    //non readonly part derived part
    KParts::ReadOnlyPart * ro_part = qobject_cast< KParts::ReadOnlyPart * >(newPart);
    if (ro_part == 0 || !ro_part->browserExtension()) {
        return;//if the cast fails (ro_part = 0) then we can't do anything.
    }
    KParts::BrowserExtension *be = ro_part->browserExtension();

    //check if the newpart has a browserExtension, if it does we will want to
    //activate some actions. This is based on konquerors kongmainwindow.cpp
    if (be) {
        KParts::BrowserExtension::ActionSlotMap *slotmap = KParts::BrowserExtension::actionSlotMapPtr();
        KParts::BrowserExtension::ActionSlotMap::const_iterator it = slotmap->constBegin();
        KParts::BrowserExtension::ActionSlotMap::const_iterator itEnd = slotmap->constEnd();

        //iterate over the slotmap, activating actions as we go
        for (; it != itEnd; ++it) {
            QAction *act = actionCollection()->action(it.key().data());
            if (act) {
                //check for the existence of the slot within this kpart
                if (be->metaObject()->indexOfSlot(it.key() + "()") != -1) {
                    connect(act, SIGNAL(triggered()), be, it.value());
                    act->setEnabled(be->isActionEnabled(it.key()));
                    const QString text = be->actionText(it.key());
                    if (!text.isEmpty()) {
                        act->setText(text);
                    }
                } else {
                    act->setEnabled(false);
                }
            }
        }
    }
    //set the currentpart to the newpart
    curPart = ro_part;
}

void Shell::slotPartDisconnect(KParts::BrowserExtension *be)
{
    //if we got here then the part has no browserExtension, so we need to remove the actions
    //from the collection

    KParts::BrowserExtension::ActionSlotMap *slotmap = KParts::BrowserExtension::actionSlotMapPtr();
    KParts::BrowserExtension::ActionSlotMap::const_iterator it = slotmap->constBegin();
    KParts::BrowserExtension::ActionSlotMap::const_iterator itEnd = slotmap->constEnd();
    //iterate over the slotmap deactivating as we go... also copied from kongmainwindow.cpp
    for (; it != itEnd ; ++it) {
        QAction *act = actionCollection()->action(it.key().data());
        if (act && be->metaObject()->indexOfSlot(it.key() + "()") != -1) {
            act->disconnect(be);
            act->setDisabled(true);
        }
    }

}


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

    //since we only need to provide a print action for use with browserExtensions
    //create and disable it right away
    print = actionCollection()->addAction(KStandardAction::Print, "print", 0, 0);
    print->setDisabled(true);

    KStandardAction::quit(kapp, SLOT(quit()),
                          actionCollection());
    //Window menu
    showCollection = new KToggleAction(this);
    showCollection->setText(i18n("Collection Manager"));
    actionCollection()->addAction("showCollection", showCollection);
    connect(showCollection, SIGNAL(triggered()),
            this, SLOT(slotToggleCollection()));
    
    saveSession = new KAction(this);
    saveSession->setText(i18n("Save current session"));
    saveSession->setIcon(KIcon("document-save.png"));
    actionCollection()->addAction("saveSession", saveSession);
    connect(saveSession, SIGNAL(triggered()), SLOT(slotSaveSession()));
    
    openSession = new KAction(this);
    openSession->setText(i18n("Open session"));
    openSession->setIcon(KIcon("document-open.png"));
    actionCollection()->addAction("openSession", openSession);
    connect(openSession, SIGNAL(triggered()), SLOT(slotOpenSession()));
    
    removeSession = new KAction(this);
    removeSession->setText(i18n("Remove session"));
    removeSession->setIcon(KIcon("list-remove.png"));
    actionCollection()->addAction("removeSession", removeSession);
    connect(removeSession, SIGNAL(triggered()), SLOT(slotRemoveSession()));

    setupGUI();
}


void Shell::readerTab(const KUrl *url)
{
    //reader page will open appropriate kpart based on the url, and
    //using partmanager will keep the menus current for the displayed tab
    ReaderPage * curPage = new ReaderPage(url, this);
    KParts::ReadOnlyPart *thisPart = 0;
    //get the part first, so we can check and see if it was successfully created
    thisPart = curPage->getPart();
    if (thisPart != 0) {
        m_manager->addPart(thisPart);
        // add the reader page to the list of open pages
        openPagesList.append(curPage);
        QString filename = url->fileName();
        int index = mainView->addTab(curPage, filename);

        //then switch to our new tab so we can start reading :D
        mainView->setCurrentIndex(index);
    } else {
        //if we couldn't open the part, display an error
        KMessageBox::error(this, i18n("Book Manager was unable to load the file: %1", url->prettyUrl()), url->prettyUrl());

    }
}

void Shell::slotRemoveTab(int index)
{
    // FIXME delete also the reader pages and parts from the manager, it seems that removeTab
    // does not delete the widget
    mainView->removeTab(index);
    //uncheck the collection toggle if we just hid the collection
    if (index == 0 && showCollection->isChecked()) {
        showCollection->toggle();
    }
}
void Shell::slotOpenFile()
{
    //FIXME this is getting far to deeply indented... maybe some of it should be pulled out?
    //check if there is an open tab...
    if (mainView->currentIndex() == -1) {
        //no tabs open so we can just use the openFileNewTab slot :D
        slotOpenFileNewTab();
    } else {
        //check to see if the collection manager is open, and if thats our current tab
        //since we can't open files with the collection manager...
        if (mainView->indexOf(m_collection->widget()) == mainView->currentIndex()) {
            //we're trying to open things in the collection... which is bad, so use a new tab :D
            slotOpenFileNewTab();
        } else {
            //we have an open reader tab so...
            //first get the filename from the user
            QString filename =  KFileDialog::getOpenFileName();
            //check if the filename is null, open the file if its NOT null
            if (!filename.isNull()) {
                KUrl tempUrl = filename;
                //might be a better way to get the part, but it works elsewhere so...
                ReaderPage *curPage = qobject_cast<ReaderPage *>(mainView->widget(mainView->currentIndex()));
                //this is kind of naive as most kparts support multiple mimetypes
                //but i couldn't find an easy way to get a list of supported types
                //from a kpart... FIXME
                QString newMimeType = KMimeType::findByUrl(tempUrl)->name();
                KParts::ReadOnlyPart *curpart = curPage->getPart();
                if (curPage->getMimeType() == newMimeType) {
                    curpart->openUrl(tempUrl);
                } else {
                    //if the mimetype has changed we need to delete the currentpage and open a replacement
                    ReaderPage *newPage = new ReaderPage(&tempUrl, this);
                    if (newPage) {
                        int index = mainView->currentIndex();
                        mainView->removeTab(index);
                        mainView->insertTab(index, newPage, tempUrl.fileName());
                        m_manager->replacePart(curpart, newPage->getPart());
                        // remove the old reader page from the open list and add the new page
                        openPagesList.replace(openPagesList.indexOf(curPage), newPage);
                        delete(curPage);
                    }
                }
            }
        }
    }
}

void Shell::slotOpenFileNewTab()
{
    foreach(const QString & filename, KFileDialog::getOpenFileNames()) {
        KUrl tempUrl = filename;
        slotReaderTab(&tempUrl);
    }
}
void Shell::slotOpenFileNewTab(QString filename)
{
    KUrl tempUrl = filename;
    slotReaderTab(&tempUrl);
}

void Shell::slotUpdateMenu(int index)
{
    //make sure we are looking at a readerpage before poking readerpage to get the widget
    if (m_collection->widget() == mainView->widget(index)) {
        //if we aren't looking at a reader page then we must be looking
        //at the collection so we can just set that as the current active part
        setupGUI(Keys | ToolBar | Save);
        m_manager->setActivePart(m_collection);
    } else {
        ReaderPage *curPage = qobject_cast<ReaderPage *>(mainView->widget(index));
        if (curPage) {
            KParts::ReadOnlyPart *curpart = curPage->getPart();
            setupGUI(Keys | ToolBar | Save);
            m_manager->setActivePart(curpart);
        }
    }
}

void Shell::slotToggleCollection()
{
    //open and close the collection tab
    if (showCollection->isChecked()) {
        //redisplay the collection and switch to its tab
        loadCollection();
    } else {
        mainView->removeTab(mainView->indexOf(m_collection->widget()));
    }
    //save the current state of the tab to the config file
    slotSaveConfig();
}

void Shell::loadCollection()
{
    int index = 0;
    //check to see if the part is already loaded
    if (!m_collection) {
        //load the collection manager by name, like the okular shell does
        //since we don't really have any other way to find the part
        KPluginFactory *factory = KPluginLoader("bookmanagerpart").factory();
        if (!factory) {
            //alert the user of failure
            KMessageBox::error(this, i18n("Unable to load the collection."));
            //then return
            m_collection = 0;
            return;
        }
        //if we get to here then everything is working :D and we can cast our part into a part
        m_collection = factory->create<KParts::Part>(this);
        if (m_collection) {
            //and now we can perform some setup for our collection so the user can actually use it :D
            //first we need to tell the part manager that we have a new part for it
            m_manager->addPart(m_collection);
            //then we stuff the collection into a tab...
            //always use tab 0 for the collection, this makes it easy to check if the collection is open,
            //and find it so we don't have to do mainview->indexof... every time we open something to track
            //it down and make sure we aren't clobbering it with file->open, which causes a crash :(
            index = mainView->insertTab(0, m_collection->widget(), i18n("Collection"));

            //connect to the loadbook signal
            QDBusConnection bus = QDBusConnection::sessionBus();
            QDBusInterface *interface =
                new QDBusInterface("org.bookmanager.BookManagerPart", "/BookManagerPart",
                                   "", bus, this);
            connect(interface, SIGNAL(loadBook(QString)),
                    this, SLOT(slotOpenFileNewTab(QString)));
        }
    } else {
        //if the collection already exists just reload it
        index = mainView->insertTab(0, m_collection->widget(), i18n("Collection"));
    }
    //switch to the collection tab, which should always be 0 but use the index anyway...
    mainView->setCurrentIndex(index);
}

bool Shell::readConfig()
{
    //read the config file to check for the users collection loading preference, defaults to show collection
    return BookManagerConfig::collection();
}


void Shell::slotSaveConfig()
{
//save the users collection loading preference
    BookManagerConfig::setCollection(showCollection->isChecked());
    BookManagerConfig::self()->writeConfig();
}

void Shell::slotSaveSession()
{
    if (openPagesList.isEmpty()) {
        KMessageBox::information(this, i18n("No open documents to save in a new session"), i18n("Book Manager"));
        return;
    }
    bool ok;
    QString sessionFile = KInputDialog::getText(i18n("Enter session name"), i18n("Enter the session name:"), QString(), &ok);
    if ((sessionFile.isNull() || sessionFile.isEmpty()) && ok) {
        KMessageBox::error(this, i18n("Invalid session name specified"));
        return;
    }
    // remove whitespaces from both ends of the session name
    sessionFile = sessionFile.trimmed();
    
    QString sessionRelativePath = "sessions/" + sessionFile;
    QString sessionFullPath = KStandardDirs::locateLocal("appdata", sessionRelativePath, true);
    QFile file(sessionFullPath);
    if (file.exists()) {
        KMessageBox::error(this, i18n("A session file named %1 already exists").arg(sessionFile), i18n("File already exists"));
        return;
    }
    if (!file.open(QIODevice::WriteOnly | QFile::Text)) {
        KMessageBox::error(this, i18n("Unable to open file %1").arg(sessionFile), i18n("Error"));
        return;
    }
    QTextStream out(&file);
    foreach(ReaderPage *page, openPagesList) {
        KUrl url = page->getPart()->url();
        out << url.toLocalFile() << "\n";
    }
    file.close();
}

void Shell::slotOpenSession()
{
    QDir sessionDir = QDir(m_sessionDirPath);
    QStringList sessionFiles = sessionDir.entryList(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    if (sessionFiles.isEmpty()) {
        KMessageBox::information(this, i18n("There are no saved sessions to open"), i18n("Book Manager"));
        return;
    }
    int result = KMessageBox::questionYesNo(this, i18n("Opening a saved session will close all your documents currently opened.\n"
    "Are you sure you wish to continue?"), i18n("Book Manager"), KStandardGuiItem::yes(),
                                                       KStandardGuiItem::no(), i18n("Do not ask again"));
    if (result == KMessageBox::Yes) {
        bool ok;
        QString sessionToOpen = KInputDialog::getItem(i18n("Select session"), i18n("Select the session you wish to open"),
                                                                                   sessionFiles, 0, false, &ok);
        if (!ok) {
            // the user cancelled, return
            return;
        }
        QFile file(KStandardDirs::locateLocal("appdata", "sessions/" + sessionToOpen));
        if (!file.open(QIODevice::ReadOnly | QFile::Text)) {
            KMessageBox::error(this, i18n("Unable to open file %1").arg(file.fileName()), i18n("Error"));
            return;
        }
        // first, remove all the tab except the collection (if it's shown)
        int indexes = mainView->count();
        int collectionIndex = mainView->indexOf(m_collection->widget());
        // start removing, skip if collection widget is found
        for (int i = indexes - 1; i >= 0; --i) {
            if (i != collectionIndex) {
                slotRemoveTab(i);
            }
        }
        if (!showCollection->isChecked()) {
            // remove also the index 0
            slotRemoveTab(collectionIndex);
        }
        // flush the list of open pages
        openPagesList.clear();
        // proceed and open the session
        KUrl::List filesUrlList;
        QTextStream in(&file);
        while (!in.atEnd()) {
            filesUrlList.append(KUrl(in.readLine()));
        }
        foreach (KUrl url, filesUrlList) {
            readerTab(&url);
        }
    }
}

void Shell::slotRemoveSession()
{
    QDir sessionDir = QDir(m_sessionDirPath);
    QStringList sessionFiles = sessionDir.entryList(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    if (sessionFiles.isEmpty()) {
        KMessageBox::information(this, i18n("There are no saved sessions to open"), i18n("Book Manager"));
        return;
    }
    bool ok;
    QString sessionToRemove = KInputDialog::getItem(i18n("Select session"), i18n("Select the session you wish to remove"),
                                                    sessionFiles, 0, false, &ok);
    if (!ok) {
        // the user cancelled, return
        return;
    }
    int result = KMessageBox::questionYesNo(this, i18n("Are you sure you want to remove session %1?").arg(sessionToRemove), i18n("Book Manager"));
    if (result == KMessageBox::No) {
        return;
    }
    QFile file(KStandardDirs::locateLocal("appdata", "sessions/" + sessionToRemove));
    // further check if file exists
    if (!file.exists()) {
        KMessageBox::error(this, i18n("File does not exist!"), i18n("Error"));
        return;
    }
    // remove the file
    bool removeResult = sessionDir.remove(sessionToRemove);
    if (!removeResult) {
        KMessageBox::error(this, i18n("Error while removing session file %1").arg(sessionToRemove), i18n("Error"));
        return;
    }
}
