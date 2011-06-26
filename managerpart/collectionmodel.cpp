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

    //probably an easier way to do this but...fix the header and make it translatable. I hope.
    //don't translate hidden values though... no one sees the id or disk location
    QStringList headerNames;
    headerNames << "ID"
                << ki18nc("This book's title", "Title").toString()
                << ki18nc("A short summary of this book", "Summary").toString()
                << ki18nc("A person who writes books", "Author").toString()
                << ki18nc("A books edition or version number", "Edition").toString()
                << ki18nc("The date the book was released", "Release Date").toString()
                << ki18nc("The type or style of a book, examples: Science Fiction, History", "Genre").toString()
                << "Disk Location";
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
}
