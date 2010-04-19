/*
    Copyright (C) <2010>  Brian Korbein <bri.kor.21@gmail.com>

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


#ifndef COLLECTIONDB_H
#define COLLECTIONDB_H
//KDE includes
#include <KUrl>

//QT includes
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QString>
#include <QWidget>

class KUrl;

class CollectionDB : public QWidget
{
  Q_OBJECT
  public:
    CollectionDB();
    ~CollectionDB();

  public slots:
    void addBook(QString title, QString summary, QString author, QString release,
	       QString releaseDate, QString genre, KUrl *url);
  signals:
    void isDirty();
    
  private:
    bool initDB();
    
    QSqlDatabase m_db;
    
};

#endif // COLLECTIONDB_H
