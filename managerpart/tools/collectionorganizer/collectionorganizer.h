/*
    Copyright (C) 2014  Riccardo Bellini <riccardo.bellini1988@gmail.com>

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

#ifndef COLLECTION_ORGANIZER_H
#define COLLECTION_ORGANIZER_H

// KDE includes
#include <kurl.h>

// Qt includes
#include <qobject.h>


class CollectionDB;
class QThread;
class CopyCollectionWorker;

class CollectionOrganizer : public QObject {
    Q_OBJECT
public:
    CollectionOrganizer (CollectionDB * collectionDb = 0, QObject * parent = 0);
    virtual ~CollectionOrganizer ();

    void organizeCollection();

    // getters
    KUrl getRootFolderUrl() const;
    QString getCollectionStructure() const;

    // setters
    void setRootFolderUrl(const KUrl & url);
    void setCollectionStructure(const QString & structure);
    
signals:
    void organizationCompleted();

public slots:
    void bookCompleted(QString);
    void copyFinished();

private:
    CollectionDB * m_collection;
    KUrl m_rootFolderUrl;
    QString m_collectionStructure;

    QThread * m_copyCollectionThread;
    CopyCollectionWorker * m_copyCollectionWorker;
};

#endif // COLLECTION_ORGANIZER_H
