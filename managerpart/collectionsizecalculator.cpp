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

#include "collectionsizecalculator.h"

// Qt includes
#include <qsqlquery.h>

// KDE includes
#include <kurl.h>
#include <kio/netaccess.h>
#include <kio/udsentry.h>
#include <threadweaver/Job.h>
#include <threadweaver/ThreadWeaver.h>
#include <kdebug.h>


namespace utils {

    class CollectionSizeCalculatorPrivate : public ThreadWeaver::Job {
        public:
            CollectionSizeCalculatorPrivate(CollectionSizeCalculatorInternal * internal)
                : m_internal(internal)
            {

            }

        protected:
            virtual void run()
            {
                m_internal->calculateSize();
            } 

        private:
            CollectionSizeCalculatorInternal * m_internal;
    };

}

utils::CollectionSizeCalculator::CollectionSizeCalculator(QObject * parent)
    : QObject(parent)
{

}


void utils::CollectionSizeCalculator::calculateSize()
{
    // we are in the public method called from outside.
    // create the internal size calculator and the worker thread,
    // schedule it for running.
    // Also, propagate signals from internal engine
    CollectionSizeCalculatorInternal * internalEngine = new CollectionSizeCalculatorInternal(this);

    connect(internalEngine, SIGNAL(sizeAvailable(quint64)), this,
            SIGNAL(sizeComputed(quint64)), Qt::DirectConnection);

    CollectionSizeCalculatorPrivate * job = new CollectionSizeCalculatorPrivate(internalEngine);
    connect(job, SIGNAL(done(ThreadWeaver::Job *)), SLOT(done(ThreadWeaver::Job *)));
    // enqueue as a job
    ThreadWeaver::Weaver::instance()->enqueue(job);
}


void utils::CollectionSizeCalculator::done(ThreadWeaver::Job * job)
{
    // dequeue and delete completed job
    ThreadWeaver::Weaver::instance()->dequeue(job);
    job->deleteLater();

    emit finished();
}


utils::CollectionSizeCalculatorInternal::CollectionSizeCalculatorInternal(QObject * parent)
    : QObject(parent)
{

}


void utils::CollectionSizeCalculatorInternal::calculateSize()
{
    quint64 totalSize = 0;
    QString command = "SELECT url FROM collection";
    QSqlQuery urlQuery;
    // optimize the execution of the query
    urlQuery.setForwardOnly(true);
    urlQuery.exec(command);
    if (urlQuery.isActive()) {
        while (urlQuery.next()) {
            KUrl url(urlQuery.value(0).toString());
            QString localFilePath = url.toLocalFile();
            KIO::UDSEntry entry;
            // calculate stats of file
            bool result = KIO::NetAccess::stat(url, entry, 0);
            if (!result) {
                kDebug() << "Cannot stat file " << url.toLocalFile();
            }
            quint64 fileSize = entry.numberValue(KIO::UDSEntry::UDS_SIZE);
            totalSize += fileSize;
        }
    }
    kDebug() << "Total size: " << totalSize;
    emit sizeAvailable(totalSize);
}
