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
#ifndef IMPORTDIALOG_H
#define IMPORTDIALOG_H

#include <KDialog>
#include <QString>
#include <KUrl>

//don't forget to include the ui-file
#include "ui_importwidget.h"
#include "bookstruct.h"

class ImportWidget;

class ImportDialog : public KDialog
{
    Q_OBJECT
public:
    ImportDialog(QWidget *parent = 0);
public slots:
    void setText(dbusBook *book);
private slots:
    void accept();
    void slotEnableImport();
    void checkUrl(QString);
signals:
    //connect this to the collection to add books
    void signalNewBook(dbusBook book);
    void urlIsGood();
    
private:
    ImportWidget *widget;
};

class ImportWidget : public QWidget, public Ui_importWidget
{
    Q_OBJECT
public:
    ImportWidget(QWidget *parent = 0);

private slots:
    // when we catch a signal from the query engine, we add author/genre
    // to our set.
    void newAuthorComplete(const QString &author);
    void newGenreComplete(const QString &genre);

    // when the asynchronous query has finished, we setup
    // autocompletion.
    void queryCompleted();

private:
    void setupAutocompletion();

    QSet<QString> authors;
    QSet<QString> genres;
};

#endif // IMPORTDIALOG_H
