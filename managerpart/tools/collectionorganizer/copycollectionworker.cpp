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
#include "copycollectionworker.h"

// KDE includes
#include <kstandarddirs.h>

// Qt includes
#include <qsqldatabase.h>
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
    // dummy method, sleep and emit some signals
    QString titles[7] = {
        "Book1",
        "Book2",
        "Book3",
        "Book4",
        "Book5",
        "Book6",
        "Book7"
    };
    for (int i = 0; i < 7 && !m_stopped; ++i) {
        usleep(500000);
        int percentage = i / 7.0 * 100;
        emit bookCopied(titles[i], percentage);
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
