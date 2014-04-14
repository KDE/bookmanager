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

// Book Manager includes
#include "collectiondb.h"
#include "copycollectionworker.h"

// KDE includes
#include <klocalizedstring.h>
#include <kstandarddirs.h>

// Qt includes
#include <qsqldatabase.h>
#include <qsqlquery.h>
#include <qsqlrecord.h>
#include <qthread.h>

#include <unistd.h>

CopyCollectionWorker::CopyCollectionWorker(QObject * parent)
    : QObject(parent), m_stopped(false)
{

}


TokenList_t CopyCollectionWorker::getTokenList() const
{
    return m_tokenList;
}


QString CopyCollectionWorker::getStructureStr() const
{
    return m_structureStr;
}


KUrl CopyCollectionWorker::getRootFolderUrl() const
{
    return m_rootFolderUrl;
}


void CopyCollectionWorker::setTokenList(const TokenList_t & tokenList)
{
    m_tokenList = tokenList;
}


void CopyCollectionWorker::setStructureStr(const QString & structureStr)
{
    m_structureStr = structureStr;
}


void CopyCollectionWorker::setRootFolderUrl(const KUrl & rootFolderUrl)
{
    m_rootFolderUrl = rootFolderUrl;
}


void CopyCollectionWorker::copyCollection()
{
    QString dbPath = KStandardDirs::locateLocal("data", "bookmanager/collection.db");
    const QString type = "QSQLITE";

    QSqlDatabase database = QSqlDatabase::addDatabase(type, "organize_connection");
    database.setDatabaseName(dbPath);
    bool ok = database.open();
    if (!ok) {
        QString errorString = "Unable to open database to copy collection";
        emit copyError(errorString);
        // exit the thread with error
        this->thread()->exit(1);
        return;
    }
    // count nesting level of folders that make up the structure
    tokenizer::Token separatorToken;
    separatorToken.type = tokenizer::Separator;
    int nestingLevel = m_tokenList.count(separatorToken);
    QSqlQuery query;
    query.exec("SELECT count(*) FROM collection");
    if (!query.isActive()) {
        QString errorString = "Unable to determine number of books";
        emit copyError(errorString);
        this->thread()->exit(1);
        return;
    }
    int rows = 0;
    if (query.next()) {
        rows = query.value(0).toInt();
    }
    QSqlQuery collectionQuery;
    collectionQuery.prepare("SELECT * FROM collection");
    collectionQuery.setForwardOnly(true);
    collectionQuery.exec();
    if (!collectionQuery.isActive()) {
        QString errorString = "Unable to fetch book data";
        emit copyError(errorString);
        this->thread()->exit(1);
        return;
    }
    for (int i = 0; i < rows && !m_stopped && collectionQuery.next(); ++i) {
        // TODO
    }
    if (m_stopped) {
        // the user stopped the program, do not emit copyFinished() signal
        emit stopped();
        return;
    }
    emit copyFinished();
}


void CopyCollectionWorker::stop()
{
    m_stopped = true;
}


// private methods
QString CopyCollectionWorker::m_getFieldFromRecordAndToken(const QSqlRecord& record,
                                                           const tokenizer::Token& token) const
{
    switch (token.type) {
        case tokenizer::Author:
            return record.value("author").toString();
            break;
        case tokenizer::AuthorInitial:
        {
            QString authorName = record.value("author").toString();
            // WARNING: organize by surname, take the last word
            // of the author's full name, if it has more than one word
            int spaceIdx = authorName.lastIndexOf(" ");
            if (spaceIdx != -1) {
                // take the first letter of the last word and uppercase it
                return authorName.right(spaceIdx + 1).at(0).toUpper();
            }
            else {
                // take the first letter and uppercase it
                return authorName.at(0).toUpper();
            }
            break;
        }
        case tokenizer::Genre:
            // if it's empty, it will be checked and skipped during the copying process
            return record.value("genre").toString();
            break;
        case tokenizer::Series:
            // if it's empty, it will be checked and skipped during the copying process
            return record.value("series").toString();
            break;
        case tokenizer::Title:
            return record.value("title").toString();
            break;
        case tokenizer::Volume:
            return record.value("volume").toString();
            break;
        case tokenizer::Other:
        case tokenizer::Separator:
            return token.tkString;
            break;
        default:
            return token.tkString;
            break;
    }
}
