/*
    Copyright (C) <2010>  Brian Korbein <bri.kor.21@gmail.com>
    Copyright (C) 2011  Riccardo Bellini <ricky88ykcir@gmail.com>

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
#include <klocale.h>
#include <kurl.h>
#include <kconfiggroup.h>

//QT includes
#include <QSqlQuery>
#include <QString>
#include <QFile>
#include <QSqlError>

#include <iostream>
using namespace std;

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
        KMessageBox::error(this, i18n("Unable to open or create a collection"),
                           m_db.lastError().text());
        return; //if we don't have a database connection we can't do anything useful so... bye :)
    }
    //create the initial database structure if it not done already...
    if (createCollection == true) {
        initDB();
    } else {
        // check the config file for the db version to use
        // this is just for not migrating the db at every start
        KConfigGroup dbConfig = KGlobal::config()->group("Collection");
        QString currentVersion = dbConfig.readEntry("version");

        if (currentVersion.isEmpty() || currentVersion.toFloat() < 0.1) {
            // the db structure needs to be migrated from previous version
            // inform the user that a newer version is available
            KMessageBox::information(this, i18n("Your collection will now be "
                                                "migrated to the latest version, with support for book series and "
                                                "volume numbers. There will not be any data loss, however, you might "
                                                "be willing to edit some information after the process completes."),
                                     i18n("New version available"));

            // TODO maybe it would be nicer to show a progress dialog, since
            // the operation may take a lot of time for large databases.

            // prepare the query and the version number
            QSqlQuery copyTableQuery;
            float version = 0.1;
            // create temporary table to copy data into
            copyTableQuery.prepare("CREATE TABLE collection_copy ("
                                   "title TEXT, "
                                   "summary TEXT, "
                                   "author TEXT, "
                                   "release TEXT, "
                                   "releaseDate TEXT, "
                                   "genre TEXT, "
                                   "url TEXT)");
            copyTableQuery.exec();
            if (!copyTableQuery.isActive()) {
                // error while creating table, print debug and return
                kDebug() << copyTableQuery.lastError().text();
                return;
            }

            copyTableQuery.prepare("SELECT title, summary, author, release, "
                                   "releaseDate, genre, url FROM collection");
            copyTableQuery.exec();
            if (copyTableQuery.isActive()) {
                QSqlQuery addToCopyQuery;
                while (copyTableQuery.next()) {
                    // insert into the backup table the current tuple
                    addToCopyQuery.prepare("INSERT INTO collection_copy ("
                                           "title, summary, author, release, releaseDate, genre, "
                                           "url) VALUES (:title, :summary, :author, :release, "
                                           ":releaseDate, :genre, :url)");
                    addToCopyQuery.bindValue(0, copyTableQuery.value(0));
                    addToCopyQuery.bindValue(1, copyTableQuery.value(1));
                    addToCopyQuery.bindValue(2, copyTableQuery.value(2));
                    addToCopyQuery.bindValue(3, copyTableQuery.value(3));
                    addToCopyQuery.bindValue(4, copyTableQuery.value(4));
                    addToCopyQuery.bindValue(5, copyTableQuery.value(5));
                    addToCopyQuery.bindValue(6, copyTableQuery.value(6));

                    addToCopyQuery.exec();
                }
            }

            // now we have two copies of our data, destroy "collection" table
            QSqlQuery restoreQuery;
            restoreQuery.exec("DROP TABLE collection");
            // create a new table collection, with series and volume number
            initDB();

            // copy data back into the collection table, restoring from
            // the backup
            restoreQuery.prepare("SELECT title, summary, author, release, "
                                 "releaseDate, genre, url FROM collection_copy");
            restoreQuery.exec();
            if (restoreQuery.isActive()) {
                QSqlQuery addToOriginalQuery;
                while (restoreQuery.next()) {
                    addToOriginalQuery.prepare("INSERT INTO collection ("
                                               "title, summary, author, release, releaseDate, genre, url) "
                                               "VALUES (:title, :summary, :author, :release, "
                                               ":releaseDate, :genre, :url)");
                    addToOriginalQuery.bindValue(0, restoreQuery.value(0));
                    addToOriginalQuery.bindValue(1, restoreQuery.value(1));
                    addToOriginalQuery.bindValue(2, restoreQuery.value(2));
                    addToOriginalQuery.bindValue(3, restoreQuery.value(3));
                    addToOriginalQuery.bindValue(4, restoreQuery.value(4));
                    addToOriginalQuery.bindValue(5, restoreQuery.value(5));
                    addToOriginalQuery.bindValue(6, restoreQuery.value(6));

                    addToOriginalQuery.exec();
                }
            }

            // drop the backup table
            restoreQuery.exec("DROP TABLE collection_copy");

            // write the version number in the configuration file
            dbConfig.writeEntry("version", version);
            dbConfig.sync();
        }

    }
}

CollectionDB::~CollectionDB()
{
    //may want to verify that all changes have been written if close doesn't do that already?
    m_db.close();
}

//PUBLIC SLOTS
void CollectionDB::addBook(dbusBook book)
{
    int id;
    QSqlQuery query;
    if (checkdupe(&book, id)) {
        //this is a dupe, which means we're editing rather than creating, so use update
        query.prepare
        ("UPDATE collection SET title = :title, summary = :summary,"
         " author = :author, release = :release, releaseDate = :releaseDate,"
         " genre = :genre, series = :series, volume = :volume, url = :url "
         " WHERE id = :id");
        query.bindValue(0, book.title);
        query.bindValue(1, book.summary);
        query.bindValue(2, book.author);
        query.bindValue(3, book.release);
        query.bindValue(4, book.releaseDate);
        query.bindValue(5, book.genre);
        query.bindValue(6, book.series);
        query.bindValue(7, book.volume);
        query.bindValue(8, book.url);
        query.bindValue(9, id);
        query.exec();
    } else {
        //set id to NULL for sqlite to autoincrement the id, we don't care about the id's so...
        query.prepare("INSERT INTO collection (title, summary, author, release, releaseDate, genre, "
                      "series, volume, url) "
                      "VALUES (:title, :summary, :author, :release, :releaseDate, :genre, "
                      ":series, :volume, :url)");
        query.bindValue(0, book.title);
        query.bindValue(1, book.summary);
        query.bindValue(2, book.author);
        query.bindValue(3, book.release);
        query.bindValue(4, book.releaseDate);
        query.bindValue(5, book.genre);
        query.bindValue(6, book.series);
        query.bindValue(7, book.volume);
        query.bindValue(8, book.url);
        query.exec();
    }
    query.finish();
    emit isDirty();
}


//PRIVATE
bool CollectionDB::initDB()
{
    // Create collection table
    // since the db doesn't exist, it is safe to create directly the version 0.1
    // of the collection.
    bool ret = false;
    bool ret2 = false;
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
                         "series text, "
                         "volume text, "
                         "url text)");
        query.finish();

        // create an index on author attribute, to make lookup for
        // auto-completion faster.
        ret2 = query.exec("create index authorIdx on collection(author)");
    }
    return ret && ret2;
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
    if (query.first()) {
        id = query.value(0).toInt();
        query.finish();
        return true;
    }
    query.finish();
    return false;
}
