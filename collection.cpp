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
#include <QSqlTableModel>

#include "collection.h"
#include <kdebug.h>
#include <QSqlError>
#include <qheaderview.h>
#include <klocale.h>
#include <QStringList>
#include "collectionmodel.h"

Collection::Collection(QWidget* parent)
  : QTableView(parent)
{
  m_db = new CollectionDB();
  m_model = new CollectionModel();  
  //set up the view
  setModel(m_model);
  setEditTriggers(QAbstractItemView::NoEditTriggers); //TODO make the table editable??
  setSelectionBehavior(QAbstractItemView::SelectRows);
  //create a prettier header for the view
  QHeaderView *head = horizontalHeader();
  head->setResizeMode(QHeaderView::Stretch);
  head->setMovable(true);
  
  hideColumn(ID);
  hideColumn(Location);
  show();
  
  //lots of qstrings... should probably be a qstringlist?
  connect(this, SIGNAL(newBook(QString,QString,QString,QString,QString,QString,KUrl*)),
	  m_db, SLOT(addBook(QString,QString,QString,QString,QString,QString,KUrl*)));
	  
  //don't forget to update the model if the db gets dirty
  connect(m_db, SIGNAL(isDirty()),
	  this, SLOT(updateModel()));
  
    //load the book on doubleclick anywhere in that row?
  connect(this, SIGNAL(doubleClicked(QModelIndex)),
	  this, SLOT(openBook(QModelIndex)));
  
}

void Collection::createBook(QString title, QString summary, QString author, QString release, QString releaseDate, QString genre, KUrl* url)
{
  emit newBook(title, summary, author, release, releaseDate, genre, url);
}

void Collection::remBook()
{
  QModelIndex removeMe;

  QModelIndexList removeUs = selectedIndexes();
  
  //the foreach loops over every column in each row so im using a counter to exit the loop when
  //it starts on the second column. 
  int index = removeUs.at(0).row();
  
  foreach(removeMe, removeUs) {
    int row = removeMe.row();
    if(row < index) {
      return;
    }
    m_model->removeRow(row);
    index += 1;
  }
}


void Collection::updateModel()
{
  m_model->select();
}

void Collection::openBook(QModelIndex index)
{
  //don't forget to add 1 to the row# as the database starts at 1 while the view starts at 0
  KUrl bookUrl =  m_db->getUrl( (index.row() + 1) );
  emit loadBook(&bookUrl);
}
