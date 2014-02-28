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
#include "collectionorganizer.h"
#include "collectiondb.h"
#include "copycollectionworker.h"
#include "structuretokenizer.h"

// Qt includes
#include <qthread.h>


CollectionOrganizer::CollectionOrganizer (CollectionDB * collectionDb,
        QObject * parent)
    : QObject (parent), m_collection(collectionDb)
{

}


CollectionOrganizer::~CollectionOrganizer ()
{

}


void CollectionOrganizer::organizeCollection()
{
    // create structure tokenizer
    tokenizer::StructureTokenizer st;
    // tokenize the structure string
    TokenList_t tokenList = st.tokenize(m_collectionStructure);
    // TODO check if collection is a valid collection
    // copyCollectionWorker is without a parent, because objects with parent can't be moved to
    // another thread
    // Despite being without a parent, the worker and the thread that manages it are deleted
    // when the organization is finished
    m_copyCollectionWorker = new CopyCollectionWorker(m_collection);
    m_copyCollectionWorker->setStructureStr(m_collectionStructure);
    m_copyCollectionWorker->setRootFolderUrl(m_rootFolderUrl);
    m_copyCollectionWorker->setTokenList(tokenList);
    // create a new thread and move the worker to that thread
    m_copyCollectionThread = new QThread;
    m_copyCollectionWorker->moveToThread(m_copyCollectionThread);
    connect(m_copyCollectionWorker, SIGNAL(bookCopied(QString)),
            this, SLOT(bookCompleted(QString)), Qt::QueuedConnection);
    connect(m_copyCollectionThread, SIGNAL(finished()),
            this, SLOT(copyFinished()));
    connect(m_copyCollectionWorker, SIGNAL(copyFinished()), m_copyCollectionThread, SLOT(quit()));
    connect(m_copyCollectionThread, SIGNAL(started()),
            m_copyCollectionWorker, SLOT(copyCollection()));
    // start thread
    m_copyCollectionThread->start();
}


// getters
KUrl CollectionOrganizer::getRootFolderUrl() const
{
    return m_rootFolderUrl;
}


QString CollectionOrganizer::getCollectionStructure() const
{
    return m_collectionStructure;
}


// setters
void CollectionOrganizer::setRootFolderUrl(const KUrl & url)
{
    m_rootFolderUrl = url;
}


void CollectionOrganizer::setCollectionStructure(const QString & structure)
{
    m_collectionStructure = structure;
}


// private slots
void CollectionOrganizer::bookCompleted(QString bookTitle)
{
    // TODO update progress bar
}


void CollectionOrganizer::copyFinished()
{
    // TODO update progress bar
    m_copyCollectionWorker->deleteLater();
    m_copyCollectionThread->deleteLater();
    // signal the widget that the collection has been organized
    emit organizationCompleted();
}
