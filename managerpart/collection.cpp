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
#include "collection.h"
#include "collectionmodel.h"

#include <QSqlTableModel>
#include <kdebug.h>
#include <QSqlError>
#include <qheaderview.h>
#include <klocale.h>
#include <QStringList>


Collection::Collection(QWidget* parent)
        : QTableView(parent)
{
    m_db = new CollectionDB();
    m_model = new CollectionModel();
    //set up the view
    setModel(m_model);
    setEditTriggers(QAbstractItemView::NoEditTriggers); //TODO make the table editable??
    setSelectionBehavior(QAbstractItemView::SelectRows);
	setSortingEnabled(true);//enable sorting for the table
    //create a prettier header for the view
    QHeaderView *head = horizontalHeader();
    head->setResizeMode(QHeaderView::Stretch);
    head->setMovable(true);

    hideColumn(ID);
    hideColumn(Location);
    show();

    //lots of qstrings... should probably be a qstringlist?
    connect(this, SIGNAL(newBook(dbusBook *)),
            m_db, SLOT(addBook(dbusBook *)));

    //don't forget to update the model if the db gets dirty
    connect(m_db, SIGNAL(isDirty()),
            this, SLOT(updateModel()));

    //load the book on doubleclick anywhere in that row?
    connect(this, SIGNAL(doubleClicked(QModelIndex)),
            this, SLOT(openBook(QModelIndex)));

}

void Collection::createBook(dbusBook *book)
{
    emit newBook(book);
}

void Collection::remBook()
{
    //the foreach loops over every column in each row so im using a counter to exit the loop when
    //it starts on the second column.
    QModelIndexList removeUs = selectedIndexes();
    //verify that we got at least one index... if the remove book command is called 
    //without having anything selected we segfault :(
    if(removeUs.length() > 0){
      int index = removeUs.at(0).row();

      foreach(const QModelIndex &removeMe,  removeUs) {
	  int row = removeMe.row();
	  if (row < index) {
	      return;
	  }
	  m_model->removeRow(row);
	  index += 1;
      }
    }
}


void Collection::updateModel()
{
    m_model->select();
}

void Collection::openBook(QModelIndex index)
{
    //use the layout enum to recreate the index with the books url selected
    //so that the model doesn't need to match the DB ordering...
    QModelIndex urlIndex = m_model->index(index.row(), Location);
    //this bit has caused a lot of issues so i'm leaving a debug in...
    kDebug() << m_model->data(urlIndex).toString();
    KUrl bookUrl = KUrl(m_model->data(urlIndex).toString());
    //emit it as a qstring so we can send it easily over dbus :D
    emit loadBook(bookUrl.url());

}

void Collection::openBook()
{
    //this uses selectedIndexes and a  foreach to let us open multiple books as needed without
    //needing to pass in the index. I should probably replace all the uses of 
    //Collection::openBook(QModelIndex index) with this, as its more flexible, in theory at least.
    
    //the foreach loops over every column in each row so im using a counter to exit the loop when
    //it starts on the second column.
    QModelIndexList openUs = selectedIndexes();
    //verify that we got at least one index... if the remove book command is called 
    //without having anything selected we segfault :(
    if(openUs.length() > 0){
      int index = openUs.at(0).row();

      foreach(const QModelIndex &openMe,  openUs) {
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
