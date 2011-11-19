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


#ifndef QUERYENGINE_H
#define QUERYENGINE_H

#include <QObject>

// Thanks to Amarok code (tag dialog part) for the idea of a
// separate thread.

// FIXME try to find a better way to do this.
// In this way, query are executed every time an import dialog is created,
// and even if they are run in different threads, they have to fetch all the
// authors/genres in the database. Behaviour is undefined with huge db!

namespace ThreadWeaver {
    class Job;
}

// keep in a separate namespace to avoid name conflict.

namespace Query {

    // the field to be selected and passed to the view.
    enum QueryType { Author = 0, Genre };

    class QueryEngineInternal;

    // interface for the execution of select queries for autocompletion.
    class QueryEngine : public QObject
    {
        Q_OBJECT
    public:
        QueryEngine(QueryType type, QObject *parent = 0);

        void setType(QueryType type)
        {
            m_type = type;
        }

        void runQuery();

    private slots:
        void done(ThreadWeaver::Job *);

        // these signals are visible from outside the class.
    signals:
        // new author fetched.
        void authorAvailable(const QString &);
        // new genre fetched.
        void genreAvailable(const QString &);

        // all stuff finished!
        void finished();

    private:
        QueryType m_type;
    };

    // middle-level class, necessary to receive signals from worker thread
    // asynchronously. See ThreadWeaver docs for more info,
    class QueryEngineInternal : public QObject
    {
        Q_OBJECT
    public:
        QueryEngineInternal(QueryType type, QObject *parent = 0);

        void runQuery();

        // these signals are only propagated to the QueryEngine interface class,
        // thus are not visible from outside.
    signals:
        void authorAvailable(const QString &);
        void genreAvailable(const QString &);

    private:
        void buildQuery();

        QString m_query;
        QueryType m_type;
    };

}

#endif // QUERYENGINE_H
