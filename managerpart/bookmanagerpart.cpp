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
#include "collection.h"
#include "importdialog.h"

#include <KAction>
#include <kactioncollection.h>
#include <kdemacros.h>
#include <kparts/genericfactory.h>
#include <KPluginFactory>
#include <KPluginLoader>
 
//Thank you kontact plugin writing instructions for this next part :)
K_PLUGIN_FACTORY(BookManagerPartFactory, registerPlugin<BookManagerPart>();)
//I think this is right? my component is named "bookmanagerpart" and i think the
//catalog would be the factory? that looks like how gwenview does it?
K_EXPORT_PLUGIN(BookManagerPartFactory("bookmanagerpart", "BookManager"))

//qwidget and qvariant are added here even though we don't use them
//otherwise it doesnt build :(
BookManagerPart::BookManagerPart(QWidget *,QObject * parent, const QVariantList&) : Part(parent)
{
    m_collect = new Collection;
    setWidget(m_collect);
}

void BookManagerPart::setupActions()
{

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

}

void BookManagerPart::slotImport()
{
    ImportDialog *dialog = new ImportDialog();
    connect(dialog, SIGNAL(signalNewBook(book *)),
            m_collect, SLOT(createBook(book *)));
    dialog->show();
}
