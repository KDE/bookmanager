/*
    Copyright (C) 2011  Riccardo Bellini <ricky88ykcir@gmail.com>

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


#include "queryengine.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

#include "threadweaver/Job.h"
#include "threadweaver/ThreadWeaver.h"
#include <kdebug.h>

namespace Query {

    class QueryProcesserPrivate : public ThreadWeaver::Job
    {
    public:
        QueryProcesserPrivate(QueryEngineInternal *engine)
        :m_engine(engine)
        {

        }

    protected:
        virtual void run()
        {
            m_engine->runQuery();
        }

    private:
        QueryEngineInternal *m_engine;
    };

}

Query::QueryEngine::QueryEngine(Query::QueryType type, QObject* parent)
:QObject(parent)
{
    m_type = type;
}

void Query::QueryEngine::runQuery()
{
    // we are in the public method called from outside.
    // create the internal query engine and the worker thread,
    // schedule it for running.
    // Also, propagate signals from internal engine
    QueryEngineInternal *internalEngine = new QueryEngineInternal(m_type, this);

    connect(internalEngine, SIGNAL(authorAvailable(QString)), this,
            SIGNAL(authorAvailable(QString)), Qt::DirectConnection);
    connect(internalEngine, SIGNAL(genreAvailable(QString)), this,
            SIGNAL(genreAvailable(QString)), Qt::DirectConnection);

    QueryProcesserPrivate *job = new QueryProcesserPrivate(internalEngine);
    connect(job, SIGNAL(done(ThreadWeaver::Job*)),
            SLOT(done(ThreadWeaver::Job*)));
    // enqueue as a job
    ThreadWeaver::Weaver::instance()->enqueue(job);
}

void Query::QueryEngine::done(ThreadWeaver::Job *job)
{
    // dequeue and delete completed job
    ThreadWeaver::Weaver::instance()->dequeue(job);
    job->deleteLater();

    emit finished();
}


Query::QueryEngineInternal::QueryEngineInternal(Query::QueryType type,
                                                QObject *parent)
: QObject(parent)
{
    m_type = type;

    buildQuery();
}

void Query::QueryEngineInternal::buildQuery()
{
    m_query.clear();

    m_query += "SELECT ";

    switch(m_type) {
        case Query::Author:
            m_query += "author";
            break;
        case Query::Genre:
            m_query += "genre";
            break;
        default:
            break;
    }

    m_query += " FROM collection";
}

void Query::QueryEngineInternal::runQuery()
{
    QSqlQuery new_query;

    // optimize the execution of the query
    new_query.setForwardOnly(true);
    new_query.exec(m_query);
    if (new_query.isActive()) {
        // our query is running, start emitting results
        while (new_query.next()) {
            QString value = new_query.value(0).toString();
            switch (m_type) {
                case Query::Author:
                    emit authorAvailable(value);
                    break;
                case Query::Genre:
                    emit genreAvailable(value);
                    break;
                default:
                    break;
            }
        }
    }
}

