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


#include "bookmanagerpart.h"

#include "bookstruct.h"
#include "searchpage.h"
#include "importdialog.h"

//generated by qt4_add_dbus_adapter from the xml file
#include "bookmanagerpartadaptor.h"

#include <KAction>
#include <kactioncollection.h>
#include <kdemacros.h>
#include <kparts/genericfactory.h>
#include <KPluginFactory>
#include <KPluginLoader>
#include <QMenu>

//Thank you kontact plugin writing instructions for this next part :)
K_PLUGIN_FACTORY(BookManagerPartFactory, registerPlugin<BookManagerPart>();)
//I think this is right? my component is named "bookmanagerpart" and i think the
//catalog would be the factory? that looks like how gwenview does it?
K_EXPORT_PLUGIN(BookManagerPartFactory("bookmanagerpart", "BookManager"))

//qwidget and qvariant are added here even though we don't use them
//otherwise it doesn't build :(
BookManagerPart::BookManagerPart(QWidget *, QObject * parent, const QVariantList&) : Part(parent)
{
    //set up the dbus bits --taken from the techbase tutorial
    new BookManagerPartAdaptor(this);
    QDBusConnection dbus = QDBusConnection::sessionBus();
    dbus.registerObject("/BookManagerPart", this);
    dbus.registerService("org.bookmanager.BookManagerPart");

    //set up the widget



    m_searchpage = new SearchPage;
    setWidget(m_searchpage);

    //connect the loadBook signal in the collection to the the one
    //here so it will get emitted properly by dbus
    //FIXME probably a better way to do this
    connect(m_searchpage, SIGNAL(loadBook(QString)),
            this, SIGNAL(loadBook(QString)));

    //turn on context menus
    m_searchpage->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_searchpage, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(ShowContextMenu(QPoint)));

    setupActions();
}

BookManagerPart::~BookManagerPart()
{
    m_searchpage->deleteLater();
}

void BookManagerPart::setupActions()
{
    //initialize the context menu
    m_contextMenu = new QMenu(this->widget());

    //setup the actions
    import = new KAction(this);
    import->setText(i18n("&Import a new Book"));
    import->setShortcut(Qt::Key_I);
    actionCollection()->addAction("import", import);
    m_contextMenu->addAction(import);
    connect(import, SIGNAL(triggered()),
            this, SLOT(slotImport()));

    remove = new KAction(this);
    remove->setText(i18n("&Remove a book"));
    actionCollection()->addAction("remove", remove);
    m_contextMenu->addAction(remove);
    connect(remove, SIGNAL(triggered()),
            m_searchpage, SLOT(remBook()));

    search = new KAction(this);
    search->setText(i18n("Start a new &Search"));
    actionCollection()->addAction("search", search);
    m_contextMenu->addAction(search);
    connect(search, SIGNAL(triggered()),
            m_searchpage, SLOT(resetQuery()));

    //for use in the context menu
    openSelected = new KAction(this);
    openSelected->setText(i18n("&Open this book"));
    m_contextMenu->addAction(openSelected);
    connect(openSelected, SIGNAL(triggered()),
            m_searchpage, SLOT(openBook()));

    edit = new KAction(this);
    edit->setText(i18n("&Edit this book's entry"));
    m_contextMenu->addAction(edit);
    connect(edit, SIGNAL(triggered()),
            m_searchpage, SLOT(slotEditBooks()));


    //set the ui resource file
    setXMLFile("managerpart.rc");

}

void BookManagerPart::slotImport()
{
    ImportDialog *dialog = new ImportDialog();
    connect(dialog, SIGNAL(signalNewBook(dbusBook*)),
            m_searchpage, SLOT(createBook(dbusBook *)));
    dialog->show();
}


void BookManagerPart::ShowContextMenu(const QPoint& pos)
{
    //use indexat to get our index from the qpoint
    QModelIndex curIndex = m_searchpage->indexAt(pos);
    if (!curIndex.isValid()) {
        //the user clicked in the whitespace so we need to disable open and remove
        openSelected->setEnabled(false);
        remove->setEnabled(false);
        m_contextMenu->exec(QCursor::pos());
        //after we close the menu, turn open and remove back on
        openSelected->setEnabled(true);
        remove->setEnabled(true);
        edit->setEnabled(true);
    } else {
        m_contextMenu->exec(QCursor::pos());
    }
}



