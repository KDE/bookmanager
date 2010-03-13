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

Collection::Collection(QWidget* parent)
  : QTableView(parent)
{
  m_db = new CollectionDB();
  
    //initialize the model and signal
  const int titleColumn = 2;
  m_model = new QSqlTableModel();
  m_model->setTable("collection");
  m_model->setSort(titleColumn, Qt::DescendingOrder);
  //probably an easier way to do this but... fix the header and make them translatable? maybe?
  QStringList headerNames;
  headerNames << ki18n("ID").toString() << ki18n("Title").toString() 
	      << ki18n("Summary").toString() << ki18n("Author").toString() 
	      << ki18n("Edition").toString() << ki18n("Release Date").toString()
	      << ki18n("Genre").toString() << ki18n("Disk Location").toString();
  
  const int colCount = m_model->columnCount();
  for(int i = 0; i < colCount; i++){
    m_model->setHeaderData(i, Qt::Horizontal, headerNames.at(i));
  }
  
  m_model->select();
  
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
  
}

void Collection::createBook(QString title, QString summary, QString author, QString release, QString releaseDate, QString genre, KUrl* url)
{
  emit newBook(title, summary, author, release, releaseDate, genre, url);
}

void Collection::updateModel()
{
  m_model->select();
}

void Collection::openBook(QModelIndex *index)
{
  //QVariant modelData = m_model->data();
  
}
