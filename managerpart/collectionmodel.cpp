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
#include "collectionmodel.h"

#include <klocale.h>
#include <QStringList>
#include <qheaderview.h>
#include <QSqlQuery>
#include <QString>

CollectionModel::CollectionModel()
    : QSqlTableModel()
{
    setTable("collection");
    setSort(Title, Qt::DescendingOrder);

    select();

}

CollectionModel::~CollectionModel()
{
    clear();
}

void CollectionModel::query(QString* queryText, QString* columnName)
{
    /*
     * In order for us to do case insensitive searchs, and partial word searchs
     * we will use sqlite3's built in LIKE operator, documented at
     * http://www.sqlite.org/lang_expr.html
     * which, combined with appended % at the start and end of the search string
     * will give us partial, case insensitive matching.      */

    //prepare the filter
    QString filterText;
    filterText.append(columnName->toLower()).append(" LIKE ")
    .append("\"%").append(queryText).append("%\"");

    //run the filter
    setFilter(filterText);
    emit newFilter();
}
