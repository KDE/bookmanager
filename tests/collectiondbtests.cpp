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

#include <QSqlDatabase>
#include <QSqlQuery>
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

}

void CollectiondbTests::isDirty()
{

}

void CollectiondbTests::checkDupe()
{

}

#include "collectiondbtests.moc"
