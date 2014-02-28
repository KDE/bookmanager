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

#ifndef COPY_COLLECTION_WORKER_H
#define COPY_COLLECTION_WORKER_H

// Book Manager includes
#include "token.h"

// KDE includes
#include <kurl.h>

// Qt includes
#include <qobject.h>


class CollectionDB;

/**
 * @class CopyCollectionWorker
 * @brief This class represents the worker code that copies the
 * collection in the new location, following the given structure
 */
class CopyCollectionWorker : public QObject {
    Q_OBJECT
    public:
        /**
         * @brief Constructor
         *
         * @param collectionDB pointer to the collection database class
         * @param parent the parent of this object
         */
        explicit CopyCollectionWorker(CollectionDB * collectionDB = 0,
                QObject * parent = 0);

        /**
         * @brief This method returns the list of tokens
         *
         * @return The list of tokens
         */
        TokenList_t getTokenList() const;
        /**
         * @brief This method returns the structure to be given to the new collection
         *
         * @return The structure to be given to the new collection
         */
        QString getStructureStr() const;
        /**
         * @brief This method returns the root url of the new collection
         *
         * @return The root url of the new collection
         */
        KUrl getRootFolderUrl() const;
        /**
         * @brief This method returns the pointer to the collection database
         *
         * @return Pointer to the collection database
         */
        CollectionDB * getCollectionDB() const;

        /**
         * @brief This method sets the list of tokens
         *
         * @param tokenList The list of token to be set in the object
         */
        void setTokenList(const TokenList_t & tokenList);
        /**
         * @brief This method sets the structure to be given to the new collection
         *
         * @param structureStr The structure to be given to the new collection
         */
        void setStructureStr(const QString & structureStr);
        /**
         * @brief This method sets the root url of the new collection
         *
         * @param rootFolderUrl The root url of the new collection
         */
        void setRootFolderUrl(const KUrl & rootFolderUrl);
        /**
         * @brief This method sets the pointer to the collection
         *
         * @param collectionDB The pointer to the collection object
         */
        void setCollectionDB(CollectionDB * collectionDB);

    signals:
        void bookCopied(QString);
        void copyFinished();

    public slots:
        /**
         * @brief This slot handles the copying of the collection to the
         * new location
         */
        void copyCollection();

    private:
        TokenList_t m_tokenList;
        QString m_structureStr;
        KUrl m_rootFolderUrl;
        CollectionDB * m_collectionDB;
};

#endif // COPY_COLLECTION_WORKER_H

