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

#ifndef COLLECTION_SIZE_CALCULATOR_H
#define COLLECTION_SIZE_CALCULATOR_H

#include <qobject.h>

// Forward declaration
namespace ThreadWeaver {
class Job;
}


namespace utils {

    class CollectionSizeCalculatorInternal;

    class CollectionSizeCalculator : public QObject {
        Q_OBJECT
        public:
            explicit CollectionSizeCalculator(QObject * parent = 0);

            void calculateSize();

        private slots:
            void done(ThreadWeaver::Job *);

        signals:
            void sizeComputed(quint64 size);
            void finished();
    };

    // middle-level class, necessary to receive signals from worker thread
    // asynchronously. See ThreadWeaver docs for more info,
    class CollectionSizeCalculatorInternal : public QObject {
        Q_OBJECT
        public:
            explicit CollectionSizeCalculatorInternal(QObject * parent = 0);

            void calculateSize();
            
        // these signals are only propagated to the CollectionSizeCalculator
        // interface class, thus are not visible from outside.
        signals:
            void sizeAvailable(quint64);
    };

}

#endif // COLLECTION_SIZE_CALCULATOR_H
