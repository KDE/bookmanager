/*
 Unit Tests for managerpart/Collectiondb
 Copyright (C) <2013>  Brian Korbein <bri.kor.21@gmail.com>
 
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

#include "collectiondbtests.h"
#include "../managerpart/collectiondb.h"
#include "../managerpart/bookstruct.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <qsqlrecord.h>
#include <qtest_kde.h>

QTEST_KDEMAIN(CollectiondbTests, GUI);

void CollectiondbTests::initTestCase()
{
    // Called before the first testfunction is executed
}

void CollectiondbTests::cleanupTestCase()
{
    // Called after the last testfunction was executed
}

void CollectiondbTests::init()
{
    // Called before each testfunction is executed
}

void CollectiondbTests::cleanup()
{
    // Called after every testfunction
}

void CollectiondbTests::Construction()
{
    //all we really have testable in the constructor right now is the 
    //connection. Migration and the failure modes really need to get 
    //pulled out of the constructor so they can be tested...
    //TODO
    
    m_db = new CollectionDB();
    QStringList connectionNames = QSqlDatabase::connectionNames ();
    //there should only be one connection
    QCOMPARE(connectionNames.length(), 1);
    //and it should be the default
    QCOMPARE(connectionNames.first(), QString("qt_sql_default_connection"));
}

void CollectiondbTests::addbook()
{
    //we'll create a test book with a hopefully unique title 
    //(I mashed the keyboard) and then verify it with a db query
    //this should also cover the isDirty signal.
    dbusBook testBook;
    testBook.title = "asfgkasg;kjashg;kaonvwewtvttwqp9";
    QSignalSpy spy(m_db, SIGNAL(isDirty()));
    m_db->addBook(testBook);
    //Verify the signal got emitted, once
    QCOMPARE(spy.count(), 1);
    //now build a nice sql query to make sure we created out book
    QSqlQuery query;
    query.prepare("SELECT title FROM collection WHERE title = 'asfgkasg;kjashg;kaonvwewtvttwqp9'");
    query.exec();
    //Verify that we have a result, and get ready to verify it
    QCOMPARE(query.first(), true);
    //and finally verify that it's the title we added
    QCOMPARE(query.value(0).toString(),
             QString("asfgkasg;kjashg;kaonvwewtvttwqp9"));
    
}

void CollectiondbTests::checkDupe()
{
    //need to modify the code so this isn't popup before testing...
    //TODO
}

#include "collectiondbtests.moc"
