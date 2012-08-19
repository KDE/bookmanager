/*
    Copyright (C) <2011>  Brian Korbein <bri.kor.21@gmail.com>

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
#include "searchpage.h"

#include "collectiondb.h"
#include "bookstruct.h"
#include "importdialog.h"
#include "modifydialog.h"
#include "collectiontreemodel.h"

#include <kdebug.h>
#include <QSqlError>
#include <klocale.h>
#include <QStringList>


SearchPage::SearchPage(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this); setFocusPolicy(Qt::ClickFocus);

    m_db = new CollectionDB();
    m_model = new CollectionTreeModel();
    m_import = 0;

    //set up the view
    resultTree->setModel(m_model);
    resultTree->setEditTriggers(QAbstractItemView::NoEditTriggers);
    resultTree->setSelectionBehavior(QAbstractItemView::SelectRows);
    resultTree->setSelectionMode(QAbstractItemView::ExtendedSelection);
    resultTree->setSortingEnabled(true);//enable sorting for the table

    fixHeaders();
    
    show();

    connect(this, SIGNAL(newBook(dbusBook)),
            m_db, SLOT(addBook(dbusBook)));

    //don't forget to update the model if the db gets dirty
    connect(m_db, SIGNAL(isDirty()),
            this, SLOT(updateModel()));

    //load the book on doubleclick anywhere in that row?
    connect(resultTree, SIGNAL(doubleClicked(QModelIndex)),
            this, SLOT(openBook(QModelIndex)));

    //connect the ui signals to the query slots
    connect(searchButton, SIGNAL(clicked(bool)),
            this, SLOT(newQuery()));

    connect(resetButton, SIGNAL(clicked(bool)),
            this, SLOT(resetQuery()));
    connect(this, SIGNAL(query(QString*, QString*)),
            m_model, SLOT(query(QString*, QString*)));

}

SearchPage::~SearchPage()
{
    if(m_model){
        m_model->deleteLater();
    }
    if(m_db){
        m_db->deleteLater();
    }
        
    
}

void SearchPage::fixHeaders()
{
    //create a prettier header for the view
    QHeaderView *head = resultTree->header();
    head->setResizeMode(QHeaderView::Stretch);
    head->setMovable(true);
    m_model->setHeaderData(0, Qt::Horizontal,
                           QVariant(i18nc("A person who writes books",
                                          "Author")));
}

void SearchPage::createBook(dbusBook book)
{
    emit newBook(book);
}

void SearchPage::remBook()
{
    QModelIndexList removeUs = resultTree->selectionModel()->selectedIndexes();
    //verify that we got at least one index... if the remove book command is called
    //without having anything selected we segfault :(
    if (removeUs.length() > 0) {

        foreach(const QModelIndex & removeMe,  removeUs) {
            m_model->removeRow(m_model->data(removeMe, KeyRole).toString());
        }
    }
}


void SearchPage::updateModel()
{
    //reset the query instead of just reselecting because the filter may not show our
    //newly added book
    resetQuery();
}

void SearchPage::openBook(QModelIndex index)
{
    //this bit has caused a lot of issues so i'm leaving a debug in...
    kDebug() << m_model->data(index, UrlRole);
    KUrl bookUrl = KUrl(m_model->data(index, UrlRole).toString());
    //emit it as a qstring so we can send it easily over dbus :D
    emit loadBook(bookUrl.url());
}

void SearchPage::openBook()
{
    //this uses selectedIndexes and a  foreach to let us open multiple books as needed without
    //needing to pass in the index. I should probably replace all the uses of
    //Collection::openBook(QModelIndex index) with this, as its more flexible, in theory at least.

    //the foreach loops over every column in each row so im using a counter to exit the loop when
    //it starts on the second column.
    QModelIndexList openUs = resultTree->selectionModel()->selectedIndexes();
    //verify that we got at least one index... if the remove book command is called
    //without having anything selected we segfault :(
    if (openUs.length() > 0) {
        int index = openUs.at(0).row();

        foreach(const QModelIndex & openMe,  openUs) {
            int row = openMe.row();
            if (row < index) {
                return;
            }
            //here we cheat and just call the old version to actually do all the work.
            openBook(openMe);
            index += 1;
        }
    }

}

//this exposes the indexAt function of the tableview so the context menu
//can be contextual...
QModelIndex SearchPage::indexAt(const QPoint& pos)
{
    //we have to map the position to the parent widget because the tableview gives
    //it relative to the viewport, while we care about the whole widget
    return  resultTree->indexAt(mapToViewport(pos));
}

//map the position to the viewport from from the Translates the widget
//coordinate pos to the coordinate system of the viewport
const QPoint SearchPage::mapToViewport(const QPoint& pos)
{
    //in order to find the offsets from we need to first find the x,y position of the table
    //then add in the additional offset of the headers. This is still off by a couple pixels
    //from the borders, but it's close enough and much better than using just guessing the
    //numbers
    int x;
    int y;
    x = resultTree->pos().x();
    y = resultTree->pos().y() + resultTree->header()->height();

    //now that we have the offsets, convert the original point to a new viewport based one
    QPoint convertedpoint(pos - QPoint(x, y));

    return convertedpoint;
}


//when the user clicks the search button, pull values from the lineedit and combobox
//and emit the newQuery signal.
void SearchPage::newQuery()
{
    QString querytext;
    QString columnName;

    //get the values from the ui...
    //if the query edit is empty default to *, otherwise emit whatever is contained
    querytext = queryEdit->text();
    columnName = searchTypeBox->currentText();

    emit query(&querytext, &columnName);
}


void SearchPage::resetQuery()
{
    //clear the lineedit
    queryEdit->clear();
    //reset the query to the default by emiting a defaulted query
    QString text = "";
    QString column = "title"; //doesn't really matter which column we pick...
    emit query(&text, &column);
}

void SearchPage::slotEditBooks()
{
    QList<dbusBook> selected;
    QModelIndexList editUs = resultTree->selectionModel()->selectedIndexes();
    
    //verify that we got at least one index... if the remove book command is called
    //without having anything selected we segfault :(
    if (editUs.length() > 0) {
        foreach(const QModelIndex & editMe,  editUs) {
            //use the index to get the key, which we can use to create a bookstruct with the existing info
            //this also filters out the author index items
            QString key = m_model->data(editMe, KeyRole).toString();
            if(!key.isEmpty()){
                selected.append(m_model->getBook(key));                
            }
        }
        QPointer<ModifyDialog> modifyDialog = new ModifyDialog(selected, this);
        connect(modifyDialog, SIGNAL(signalUpdateBook(dbusBook)),
                m_db, SLOT(addBook(dbusBook)));
        modifyDialog->exec();
        
        delete modifyDialog;
    }
}
