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

Collection::Collection(QWidget* parent)
  : QTableView(parent)
{
  m_db = new CollectionDB();
  
    //initialize the model and signal
  const int titleColumn = 2;
  m_model = new QSqlTableModel();
  m_model->setTable("collection");
  m_model->setSort(titleColumn, Qt::DescendingOrder);
  m_model->select();
  kDebug()<< m_model->lastError().text();
  
  setModel(m_model);
  setEditTriggers(QAbstractItemView::NoEditTriggers);
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

