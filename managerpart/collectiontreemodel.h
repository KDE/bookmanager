/*
    Copyright (C) 2012  Brian k. <Bri.kor.21@gmail.com>

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


#ifndef COLLECTIONTREEMODEL_H
#define COLLECTIONTREEMODEL_H

#include <QStandardItemModel>
#include "bookstruct.h"

class CollectionModel;

class CollectionTreeModel : public QStandardItemModel
{
    Q_OBJECT
public:
    CollectionTreeModel(QObject* parent = 0);
    virtual ~CollectionTreeModel();

    bool removeRow(QString key);
    dbusBook getBook(QString key);
    
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;
    
    //Qt::UserRole aliases
    enum dataRole { TitleRole = Qt::UserRole + 1,
                    SummaryRole = Qt::UserRole + 2,
                    AuthorRole = Qt::UserRole + 3,
                    RelNumberRole = Qt::UserRole + 4,
                    RelDateRole = Qt::UserRole + 5,
                    GenreRole = Qt::UserRole + 6,
                    SeriesRole = Qt::UserRole + 7,
                    VolumeRole = Qt::UserRole + 8,
                    UrlRole = Qt::UserRole + 9,
                    UnknownAuthorRole = Qt::UserRole + 10,
                    KeyRole = Qt::UserRole + 11,
                    PreviewRole = Qt::UserRole + 12,
                    AuthorBookCountRole = Qt::UserRole + 13
    };
    
public slots:
    void query(QString *queryText, QString *columnName);
    void bookIconReady(const QString &filename);

signals:
    void repeatQuery(QString *query, QString *column);

private slots:
    void rebuildModel(); 
private:
    
    CollectionModel *m_collectionModel;
    QStandardItem *m_rootItem;
    
    void createAuthorModel();
    void attachCollectionModel();
    void createMergedModel();
    QModelIndex findIndexByFilename(QString filename);
    
    enum columnLayout {ID, Title, Summary, Author, Release, ReleaseDate, Genre,
    Series, Volume, Location};
};

#endif // COLLECTIONTREEMODEL_H
