/*
    <one line to give the program's name and a brief idea of what it does.>
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


#ifndef BOOKICONBUILDER_H
#define BOOKICONBUILDER_H

#include <qobject.h>
#include <qmap.h>
#include <threadweaver/Job.h>

/*
 * To use this class:
 * SETUP
 * 1) Create a KImageCache -- this is where the resulting icons will me stored
 * 2) Create an instance of the BookIconBuilder class (you only need 1)
 * 3) Connect to the iconReady signal to find out when your icons are ready--
 *      they will be stored in the Kimagecache you used to create the BookIconBuilder
 *      using the filename as the key
 * USE
 * 1) check to see if your file is a pdf, since this only supports pdfs, for now
 * 2) check the KImageCache to see if an icon already exists (you may have created on already)
 * 3) if it doesnt exist, fill the spot with a placeholder(or not)
 * 4) call buildIcon with the filename to create an icon
 * 5) when IconReady triggers, replace the placeholder with your new icon
 *
 */

class KImageCache;


namespace Iconbuilder
{

// Internal class to keep the signals asynchronous, see threadweavers class docs for details
// or the queryengine class, for another example, since that's what all this is based off of...
class IconBuilderInternal : public QObject
{
    Q_OBJECT
public:
    explicit IconBuilderInternal(const QMap<QString, QString> &books, KImageCache *cache, QObject *parent = 0);

    void buildIcons();

    //NOTE: This signal is only visible to the iconbuilderjob class.
signals:
    void iconReady(const QString &, const QString &);

private:
    KImageCache  *m_cache;
    QMap<QString, QString> m_books;
};



class IconBuilderJob : public ThreadWeaver::Job
{
    Q_OBJECT
public:
    explicit IconBuilderJob(const QMap<QString, QString> &books, KImageCache *cache, QObject *parent = 0);

signals:
    void iconReady(const QString &, const QString &);

protected:
    virtual void run();

private:
    IconBuilderInternal *m_builder;
    KImageCache *cache;
    QMap<QString, QString> books;
};

}

#endif
