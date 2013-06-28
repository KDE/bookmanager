/*
    Copyright (C) <2010>  Brian Korbein <bri.kor.21@gmail.com>
    Copyright (C) 2011  Riccardo Bellini <riccardo.bellini1988@gmail.com>

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

#ifndef COLLECTIONMODEL_H
#define COLLECTIONMODEL_H

#include <qsqltablemodel.h>
#include "collectiondb.h"

class QString;

class CollectionModel : public QSqlTableModel
{
    Q_OBJECT
public:
    CollectionModel();
    ~CollectionModel();

public slots:
    void query(QString *queryText, QString *columnName);
signals:
    void newFilter();

private:
    enum columnLayout {ID, Title, Summary, Author, Release, ReleaseDate, Genre,
                       Series, Volume, Location
                      };
};

#endif // COLLECTIONMODEL_H
