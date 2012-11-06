/*
    Copyright (C) <2011>  Brian Korbein <bri.kor.21@gmail.com>
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

#ifndef SEARCHPAGE_H
#define SEARCHPAGE_H

#include <QWidget>

#include "ui_searchpage.h"
#include "bookstruct.h"

class ImportDialog;


class CollectionDB;
class CollectionTreeModel;
class QModelIndex;

class BookDelegate;

class KImageCache;

namespace ThreadWeaver {
    class Weaver;
    class Job;
}

class SearchPage : public QWidget, public Ui_SearchPage
{
    Q_OBJECT
    //I'm pretty sure this is correct? I'd make it org.kde but
    //I think I'd need to actually be part of kde proper to use it?
    Q_CLASSINFO("D-Bus Interface", "org.bookmanager.BookManagerPart")

public:
    SearchPage(QWidget *parent = 0);
    virtual ~SearchPage();

    QModelIndex indexAt(const QPoint& pos);

public slots:
    void createBook(dbusBook book);
    void slotEditBooks();
    void remBook();
    void updateModel();
    void newQuery();
    void resetQuery();

private slots:
    void openBook(QModelIndex index);
    void openBook();
    
    void fetchIcons(const QModelIndex &author);
    
    void deleteJob(ThreadWeaver::Job *previewJob);

signals:
    void query(QString *query, QString *column);
    void newBook(dbusBook book);
    //can't emit a Kurl, so emit it as a string
    Q_SCRIPTABLE void loadBook(QString url);
private:
    void fixHeaders();
    QStringList getAuthorBooks(const QModelIndex &author);
    ImportDialog *m_import;
    CollectionDB *m_db;
    CollectionTreeModel *m_model;
    
    BookDelegate *bookDelegate;
    
    KImageCache *m_image_cache;
    
    ThreadWeaver::Weaver *previewsFetchingQueue;

    const QPoint mapToViewport(const QPoint& pos);
    dbusBook getBook(QModelIndex index);

    enum columnLayout {ID, Title, Summary, Author, Release, ReleaseDate, Genre,
                       Series, Volume, Location};
    //Qt::UserRole aliases
    enum dataRole { UrlRole = Qt::UserRole+2, UnknownAuthorRole = Qt::UserRole+3, KeyRole = Qt::UserRole + 4};
};

#endif // SEARCHPAGE_H
