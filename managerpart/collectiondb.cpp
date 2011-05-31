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
#include "collectiondb.h"
	   
//KDE includes
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <kdebug.h>
#include <kurl.h>

//QT includes
#include <QSqlQuery>
#include <QString>
#include <QFile>
#include <QSqlError>



//PUBLIC
CollectionDB::CollectionDB()
{
    //if we can't find a database in the standard data directory just create a new one for now,
    //eventually it might be possible to import databases but not now.
    QString dbPath = KStandardDirs::locateLocal("data", "bookmanager/collection.db");
    bool createCollection = !QFile(dbPath).exists();
    const QString type = "QSQLITE";

    m_db = QSqlDatabase::addDatabase(type);
    m_db.setDatabaseName(dbPath);
    bool ok =  m_db.open();
    if (!ok) {
        KMessageBox::error(this, "Unable to open or create a collection",
                           m_db.lastError().text());
        return; //if we don't have a database connection we can't do anything useful so... bye :)
    }
    //create the initial database structure if it not done already...
    if (createCollection == true) {
        initDB();
    }
}

CollectionDB::~CollectionDB()
{
    //may want to verify that all changes have been written if close doesn't do that already?
    m_db.close();
}

//PUBLIC SLOTS
void CollectionDB::addBook(dbusBook *book)
{
    QSqlQuery query;
    //set id to NULL for sqlite to autoincrement the id, we don't care about the id's so...
    query.prepare("INSERT INTO collection (title, summary, author, release, releaseDate, genre, url) "
                  "VALUES (:title, :summary, :author, :release, :releaseDate, :genre, :url)");
    query.bindValue(0, book->title);
    query.bindValue(1, book->summary);
    query.bindValue(2, book->author);
    query.bindValue(3, book->release);
    query.bindValue(4, book->releaseDate);
    query.bindValue(5, book->genre);
    query.bindValue(6, book->url);
    query.exec();
    query.finish();
    emit isDirty();
}


//PRIVATE
bool CollectionDB::initDB()
{
    // Create collection table
    bool ret = false;
    if (m_db.isOpen()) {
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
        query.finish();

    }
    return ret;
}


bool CollectionDB::checkdupe(dbusBook* book, int &id)
// Check for duplicate entries in the database using the url and prompt the user to either overwrite
// cancel the import, use the id reference to pass the id back to the caller when it's found
{

	QSqlQuery query;
	query.prepare("Select id FROM collection WHERE url = :url");
	query.bindValue(0, book->url);
	query.exec();
	//if the query returns a result query.first will return true, and we can get the id of the original
	if(query.first()){
		id = query.value().toInt();
		query.finish();
		return true;
	}
	query.finish();
	return false;
}
