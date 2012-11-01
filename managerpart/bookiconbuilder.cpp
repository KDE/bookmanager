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
#include <kimagecache.h>
#include <QImage>

#include <poppler/qt4/poppler-qt4.h>
#include <threadweaver/Job.h>
#include <threadweaver/ThreadWeaver.h>

#include "bookiconbuilder.h"

namespace Iconbuilder {
    class IconBuilderPrivate : public ThreadWeaver::Job
    {
    public:
        explicit IconBuilderPrivate(IconBuilderInternal *builder)
        :m_builder(builder)
        {
        
        }
    protected:
        virtual void run()
        {
            m_builder->buildIcon();
        }
    private:
        IconBuilderInternal *m_builder;
    };

}

Iconbuilder::BookIconBuilder::BookIconBuilder(KImageCache &cache, QObject* parent)
    : QObject(parent)
{
    m_cache = &cache;
}

void Iconbuilder::BookIconBuilder::buildIcon(QString filename)
{
    //create the internal builder and job, then connect them up so the signals can propagate.
    IconBuilderInternal *internal = new IconBuilderInternal(filename, m_cache, this);

    connect(internal, SIGNAL(iconReady(QString)), this,
        SIGNAL(iconReady(QString)), Qt::DirectConnection);

    IconBuilderPrivate *job = new IconBuilderPrivate(internal);

    connect(job, SIGNAL(done(ThreadWeaver::Job*)),
            SLOT(slot(ThreadWeaver::Job*)));
    //now that everything is set up, enqueue the job
    ThreadWeaver::Weaver::instance()->enqueue(job);    
}

void Iconbuilder::BookIconBuilder::done(ThreadWeaver::Job *job )
{
    //Clean up (this is effectively the destructor)
    ThreadWeaver::Weaver::instance()->dequeue(job);
    job->deleteLater();
}

Iconbuilder::IconBuilderInternal::IconBuilderInternal(QString filename, KImageCache *cache, QObject* parent)
    : QObject(parent)
{
    m_cache = cache;
    m_filename = filename;
}

void Iconbuilder::IconBuilderInternal::buildIcon()
{
    //If the file doesnt exist, we'll simply return nothing.
    //if it does exist then we run the poppler code to get a qimage for the kimagecache,
    //after which we emit the qimage's id (the filename)
    //this part is taken pretty much verbatim from poppler-qt docs

    //load the file
    Poppler::Document* document = Poppler::Document::load(m_filename);
    if(!document || document->isLocked()){
        return;
    }

    //get the first page
    Poppler::Page* pdfPage = document->page(0);
    if(pdfPage == 0) {
        return;
    }

    //get the image, and emit
    //FIXME without a way to test this, i have no idea what a sane value is!
    //Going with the defaults for now...
    QImage image = pdfPage->renderToImage();
    if(image.isNull()){
        return;
    }
    if(m_cache->insertImage(m_filename, image)){
        emit iconReady(m_filename);
    }
    
    //cleanup
    delete pdfPage;
    delete document;    
}
