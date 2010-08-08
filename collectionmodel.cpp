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
#include <klocale.h>
#include <QStringList>
#include <qheaderview.h>
#include <kdebug.h>
#include "collectionmodel.h"

CollectionModel::CollectionModel()
        : QSqlTableModel()
{
    setTable("collection");
    setSort(Title, Qt::DescendingOrder);

    //probably an easier way to do this but...fix the header and make it translatable. I hope.
    QStringList headerNames;
    headerNames << ki18n("ID").toString() << ki18n("Title").toString()
    << ki18n("Summary").toString() << ki18n("Author").toString()
    << ki18n("Edition").toString() << ki18n("Release Date").toString()
    << ki18n("Genre").toString() << ki18n("Disk Location").toString();
    const int colCount = columnCount();
    for (int i = 0; i < colCount; i++) {
        setHeaderData(i, Qt::Horizontal, headerNames.at(i));
    }

    select();

}

CollectionModel::~CollectionModel()
{
    clear();
}
