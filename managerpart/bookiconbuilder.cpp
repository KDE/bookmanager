/*
    Copyright (C) 2012  Brian k. <Bri.kor.21@gmail.com>
    Copyright (C) 2012  Riccardo Bellini <ricky88ykcir@gmail.com>

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

#include "bookiconbuilder.h"
#include "constants.h"

#include <KImageCache>
#include <KUrl>
#include <KDebug>

#include <QString>
#include <QThread>
#include <QImage>
#include <QStringBuilder>

#include <poppler-qt4.h>
#include <threadweaver/Job.h>
#include <threadweaver/ThreadWeaver.h>




Iconbuilder::IconBuilderJob::IconBuilderJob(const QMap<QString, QString> &books, KImageCache *cache, QObject *parent)
    : ThreadWeaver::Job(parent)
{
    this->books = books;
    this->cache = cache;
}

void Iconbuilder::IconBuilderJob::run()
{
    m_builder = new Iconbuilder::IconBuilderInternal(books, cache);

    // I think this is a queued connection
    connect(m_builder, SIGNAL(iconReady(QString,QString)), this,
            SIGNAL(iconReady(QString,QString)));

    m_builder->buildIcons();

    delete m_builder;
}


Iconbuilder::IconBuilderInternal::IconBuilderInternal(const QMap<QString, QString> &books, KImageCache *cache, QObject* parent)
    : QObject(parent)
{
    m_cache = cache;
    m_books = books;
}

void Iconbuilder::IconBuilderInternal::buildIcons()
{
    //If the file doesnt exist, we'll simply return nothing.
    //if it does exist then we run the poppler code to get a qimage for the kimagecache,
    //after which we emit the qimage's id (the filename)
    //this part is taken pretty much verbatim from poppler-qt docs

    if (m_books.isEmpty()) {
        return;
    }

    QMap<QString, QString>::const_iterator it(m_books.constBegin());
    QMap<QString, QString>::const_iterator end(m_books.constEnd());

    for (; it != end; ++it) {
        QString book = it.key();
        QString key = it.value();

        KUrl locationUrl(book);
        QString locationPath = locationUrl.path();

        // if there is already a cached image for that book skip this part
        if (m_cache->contains(key)) {
            emit iconReady(book, key);
            continue;
        }

        //load the file
        Poppler::Document* document = Poppler::Document::load(locationPath);
        if (!document || document->isLocked()) {
            return;
        }

        //get the first page
        Poppler::Page* pdfPage = document->page(0);
        if (pdfPage == 0) {
            return;
        }

        //get the image, and emit
        QImage image = pdfPage->renderToImage().scaled(ThumbnailSize,
                       Qt::IgnoreAspectRatio,
                       Qt::SmoothTransformation);

        key = QString::number(image.cacheKey());

        if (image.isNull()) {
            return;
        }
        if (m_cache->insertImage(key, image)) {
            emit iconReady(book, key);
        }

        //cleanup
        delete pdfPage;
        delete document;
    }
}
