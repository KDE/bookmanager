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
//KDE includes
#include <kmessagebox.h>
#include <kstandarddirs.h>

//QT includes
#include <QSqlQuery>
#include <QString>
#include <QFile>

#include "collectiondb.h"

//PUBLIC
CollectionDB::CollectionDB()
{
  //if we can't find a database in the standard data directory just create a new one for now,
  //eventually it might be possible to import databases but not now.
  QString dbPath = KStandardDirs::locateLocal("data", "bookmanager/collection.db");
  bool createCollection = !QFile(dbPath).exists();
  
  m_db = new QSqlDatabase("QSQLITE");
  m_db->setDatabaseName(dbPath);
  bool ok =  m_db->open();
  if(!ok){
    KMessageBox::error(this, "Unable to open or create a collection", 
		       QSqlDatabase::lastError().text());
    return; //if we don't have a database connection we can't do anything useful so... bye :)
  }
  //create the initial database structure if it not done already...
  if(createCollection == true){
    initDB();
  }
  
  //initialize the model and signal
  const int titleColumn = 2;
  m_model = new QSqlTableModel();
  m_model->setTable("library");
  m_model->setSort(titleColumn, Qt::DescendingOrder);
  m_model->select();
}

CollectionDB::~CollectionDB()
{
  //may want to verify that all changes have been written if close doesn't do that already?
  m_db->close();
}

//only give a const copy of the model as we only want modifications made by add/remBook
//any views using this should set setEditTriggers(QAbstractItemView::NoEditTriggers)
const QSqlTableModel CollectionDB::getModel()
{
  const QSqlTableModel usrModel = *m_model;
  return usrModel;
}

//PUBLIC SLOTS
void CollectionDB::addBook(QString title, QString summary, 
			   QString author, QString release, 
			   QString releaseDate, QString genre,
			   KUrl* url)
{
  QSqlQuery query;
  //set id to NULL for sqlite to autoincrement the id, we don't care about the id's so...
  query.prepare("INSERT INTO collection (id, title, summary, author, release, releaseDate, genre, url) "
                   "VALUES (NULL, :title, :summary, :author, :release, :releaseDate, :genre, :url)");
     query.bindValue(1, title);
     query.bindValue(2, summary);
     query.bindValue(3, author);
     query.bindValue(4, release);
     query.bindValue(5, releaseDate);
     query.bindValue(6, genre);
     query.bindValue(7, url->url());
     query.exec();
}

void CollectionDB::remBook(int row){
  m_model->removeRow(row);
}

//PRIVATE
bool CollectionDB::initDB()
{
    // Create collection table
    bool ret = false;
    if (m_db->isOpen())
    {
        QSqlQuery query;
        ret = query.exec("create table collection "
                         "(id integer primary key, "
                         "title text, "
                         "summary text, "
                         "author text, "
                         "release text, "
                         "releaseDate text, "
                         "genre text, "
                         "url text)");

    }
    return ret;
}