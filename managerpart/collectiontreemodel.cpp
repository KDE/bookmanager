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
#include "collectiontreemodel.h"
#include "collectionmodel.h"

#include <kdebug.h>
#include <klocalizedstring.h>
#include <kdemacros.h>

#include <QStringBuilder>
#include <QStandardItem>
#include <QStringList>

#include <poppler/qt4/poppler-qt4.h>

const float ScaleFactor = 0.16;

CollectionTreeModel::CollectionTreeModel(QObject* parent): QStandardItemModel(parent)
{
    m_collectionModel = new CollectionModel();
    m_rootItem = 0;
    createMergedModel();

    connect(this, SIGNAL(repeatQuery(QString*,QString*)),
            m_collectionModel, SLOT(query(QString*,QString*)));
    connect(m_collectionModel, SIGNAL(newFilter()),
            this, SLOT(rebuildModel()));
}
CollectionTreeModel::~CollectionTreeModel()
{
    //use delete later so qt can delete them at it's leisure,
    m_collectionModel->deleteLater();
}


void CollectionTreeModel::createAuthorModel()
{
    //there may be an easier way to find dupes, but i'm just going to use a string list to remove them if they exist...
    QStringList* authorlist = new QStringList;
    bool needUnknown = 0;
    for(int row = 0; row < m_collectionModel->rowCount(); row++){
        //check if the string isEmpty, then negate it so we can pass the Krazy test
        if(!m_collectionModel->data(m_collectionModel->index(row, Author)).toString().isEmpty()){
            authorlist->append(m_collectionModel->data(m_collectionModel->index(row, Author)).toString());
        } else {
            needUnknown = 1;
        }
    }
    authorlist->removeDuplicates();
    for(int i = 0; i < authorlist->size();i++){
        QStandardItem *author = new QStandardItem;
        author->setData(authorlist->at(i), Qt::DisplayRole);
        m_rootItem->appendRow(author);
    }
    /* now we create a special "unknown" item in the author model, Unknown Author should be unique enough that
     * it won't interfere with peoples collections, i hope.
     */
    if(needUnknown){
        QStandardItem *unknown = new QStandardItem;
        unknown->setData(i18nc("A placeholder entry for books with no known author", "Unknown Author"), Qt::DisplayRole);
        unknown->setData(true, UnknownAuthorRole);
        m_rootItem->appendRow(unknown);
    }
    /*
     * if there are no entries in the list then we need to make a "No Documents Found" entry
     */
    if(authorlist->isEmpty()){
        QStandardItem *none = new QStandardItem;
        none->setData(i18n("No Documents Found"), Qt::DisplayRole);
        m_rootItem->appendRow(none);
    }
}

void CollectionTreeModel::createMergedModel()
{
    if(m_rootItem){
        m_rootItem->removeRows(0, m_rootItem->rowCount());
    }
    m_rootItem = invisibleRootItem();
    createAuthorModel();
    attachCollectionModel();
}

void CollectionTreeModel::attachCollectionModel()
{
    //create a qhash cache of authors, with pointers to them as the value, so we can add children
    
    QHash<QString, QStandardItem*> authorCache;
    for(int row = 0; row < m_rootItem->rowCount(); row++){
        /* in order to allow for the translation of the unknown author entry i believe i need to go
         * through the extra check here, since we have no guarantee that the datastring is going to be
         * "unknown author"
         */
        if(m_rootItem->child(row)->data(UnknownAuthorRole).toBool()){
            authorCache.insert("Unknown Author", m_rootItem->child(row));
        } else {
            authorCache.insert(m_rootItem->child(row)->data(Qt::DisplayRole).toString(), m_rootItem->child(row));
        }
    }
    for(int row = 0; row < m_collectionModel->rowCount(); row++){
        QString author = m_collectionModel->data(m_collectionModel->index(row,Author)).toString();
        QStandardItem *tempAuthor;
        if(author.isEmpty()){
            tempAuthor = authorCache.value("Unknown Author");
        } else {
            tempAuthor = authorCache.value(author);
        }
        /*
         * In the following I'm going to create a copy of the data from the collectionmodel in a single
         * long string with the header info embedded. The idea being to prepend things like "author:" or
         * "title:" to that information so that it can all be displayed in a more catalog entry like
         * fashion. This will probably need a delegate to get it to look right.
         * The finished output should look like:
         *
         * Title:
         * Author:
         * Release Number:         Release Date:
         * Series:                 Volume:
         * Genre:
         * Summary:
         *
         * I'm going to be kind of arbitrary with the field widths for the 2 column rows since there's
         * no real standard that i know of saying how long a series name/number can be.
         */
        QString tempDataString =
        i18nc("This book's title", "Title: ") % m_collectionModel->data(m_collectionModel->index(row,Title)).toString() % QString("\n") %
        i18nc("A person who writes books","Author: ") % m_collectionModel->data(m_collectionModel->index(row,Author)).toString() % QString("\n") %
        i18nc("The books Edition or Release number", "Release Number: ") % m_collectionModel->data(m_collectionModel->index(row,Release)).toString() % QString(" ") %
        i18nc("This book's Release Date", "Release Date: ") % m_collectionModel->data(m_collectionModel->index(row,ReleaseDate)).toString() % QString("\n") %
        i18nc("The Series of a book", "Series: ") % m_collectionModel->data(m_collectionModel->index(row,Series)).toString() % QString(" ") %
        i18nc("The Volume number of a book", "Volume: ") % m_collectionModel->data(m_collectionModel->index(row,Volume)).toString() % QString("\n") %
        i18nc("The type or style of a book, examples: Science Fiction, History","Genre: ") %
        m_collectionModel->data(m_collectionModel->index(row,Genre)).toString() % QString("\n") %
        i18nc("A short summary of this book","Summary: ") % m_collectionModel->data(m_collectionModel->index(row,Summary)).toString() % QString("\n");
        QStandardItem *tempBook = new QStandardItem;
        tempBook->setData(tempDataString, Qt::DisplayRole);
        tempBook->setData(m_collectionModel->data(m_collectionModel->index(row,Location)), UrlRole);
        tempBook->setData(m_collectionModel->data(m_collectionModel->index(row, ID)), KeyRole);
        
        // set non valid data for PreviewRole
        tempBook->setData(QString(), PreviewRole);
        
        tempAuthor->setChild(tempAuthor->rowCount(), tempBook);
    }
}

void CollectionTreeModel::query(QString* queryText, QString* columnName)
{
    /* this just re-emits the query after connecting the signal repeatQuery to the slot
     * in the collectionmodel.I'm going through all this mainly to avoid doing a cast of
     * m_collectionModel to collectionmodel and calling the slot directly. It may be slower but
     * I don't like casting things often for some reason I can't remember right now.
     */
    emit repeatQuery(queryText, columnName);
}


void CollectionTreeModel::bookIconReady(const QString& filename)
{
    if (KDE_ISUNLIKELY(filename.isNull() || filename.isEmpty())) {
        return;
    }
    
    QModelIndex book = findIndexByFilename(filename);
    
    // the key of the image in the cache could be different from filename,
    // due to the encoding of the file. For this reason, you should fetch the image
    // by creating a KUrl object and call path() member function, which returns
    // the correct key. Therefore we store in the PreviewRole the encoded path
    // of the image.
    KUrl locationUrl(filename);
    
    setData(book, locationUrl.path(), PreviewRole);
}


void CollectionTreeModel::rebuildModel()
{
    /*
     * this will call createMergedModel to recreate the model if anything changes in
     * the underlying collectionmodel, be it a new filter(search)  or a change to the db.
     * The docs say to emit beginResetModel, and endResetModel when the underlying
     * model is altered, so do that here too...
     */
    emit beginResetModel();
    createMergedModel();
    emit endResetModel();
}

bool CollectionTreeModel::removeRow(QString key)
{
    /*
     * Use the key from the db to map the entry back to its database entry and delete it.
     * I'm going to use the filter functionality of qsqltablemodel rather than iterating over them
     * entire list since it is likely faster for large lists and we need to recreate the model
     * afterward anyway it shouldn't really add any extra overhead.
     */
    QString oldFilter = m_collectionModel->filter();
    QString newFilter = "id = ";
    newFilter.append(key);
    m_collectionModel->setFilter(newFilter);
    //since there should only be 1 row, we can just remove it without checking, I hope
    bool success = m_collectionModel->removeRow(0);
    //now that everything's deleted we just need to rebuild the model
    m_collectionModel->setFilter(oldFilter);
    rebuildModel();
    // returning the status of the removal just to duplicated the removerow functionality we're replacing
    return success;
}
dbusBook CollectionTreeModel::getBook(QString key)
{
    /*
     * much like in removeRow above, use the collectionmodel's filter method to filter out all but the
     * desired book, create the dbusbook from it, then return the filter to its previous state.
     */
    QString oldFilter = m_collectionModel->filter();
    QString newFilter = "id = ";
    newFilter.append(key);
    m_collectionModel->setFilter(newFilter);
    
    //create a dbusBook from the record so we can play with individual values
    dbusBook book;
    book.author = m_collectionModel->data(m_collectionModel->index(0, Author)).toString();
    book.genre = m_collectionModel->data(m_collectionModel->index(0, Genre)).toString();
    book.series = m_collectionModel->data(m_collectionModel->index(0, Series)).toString();
    book.volume = m_collectionModel->data(m_collectionModel->index(0, Volume)).toString();
    book.release = m_collectionModel->data(m_collectionModel->index(0, Release)).toString();
    book.releaseDate = m_collectionModel->data(m_collectionModel->index(0, ReleaseDate)).toString();
    book.summary = m_collectionModel->data(m_collectionModel->index(0, Summary)).toString();
    book.title = m_collectionModel->data(m_collectionModel->index(0, Title)).toString();
    book.url = m_collectionModel->data(m_collectionModel->index(0, Location)).toString();

    m_collectionModel->setFilter(oldFilter);
    return book;
    
}
QModelIndex CollectionTreeModel::findIndexByFilename(QString filename)
{
    // This is based on findItems, but searches in the urlRole, rather than in the displayRole
    // since there should only be one entry per file, return the first match (and stop looking after 1 match)
    QModelIndexList indexes = match(index(0, 0, QModelIndex()),
                                    UrlRole, filename, 1, Qt::MatchRecursive);
    
  return indexes.first();
}
