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


#ifndef COLLECTION_H
#define COLLECTION_H


#include "collectiondb.h"
#include "bookstruct.h"

#include <QTableView>
#include <QtDBus>


class CollectionModel;
class Collection : public QTableView
{
    Q_OBJECT
    //I'm pretty sure this is correct? I'd make it org.kde but
    //I think I'd need to actually be part of kde proper to use it?
    Q_CLASSINFO("D-Bus Interface", "org.bookmanager.BookManagerPart")
    
public:
    Collection(QWidget* parent = 0);

public slots:
    void createBook(dbusBook *book);
    void remBook();
    void updateModel();

private slots:
    void openBook(QModelIndex index);
    void openBook();

signals:
    void newBook(dbusBook *book);
    //can't emit a Kurl, so emit it as a string
   Q_SCRIPTABLE void loadBook(QString url);
private:
    CollectionDB *m_db;
    CollectionModel *m_model;

    enum columnLayout {ID, Title, Summary, Author, Release, ReleaseDate, Genre, Location};



};

#endif // COLLECTION_H
