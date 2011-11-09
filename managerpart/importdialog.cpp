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

#include "importdialog.h"

#include <klocale.h>
#include <kicon.h>
#include <QString>
#include <KUrl>
#include <KIO/NetAccess>

ImportDialog::ImportDialog(QWidget *parent)
    : KDialog(parent)
{
    setupUi(this);
    connect(ImportButton, SIGNAL(clicked()),
            this, SLOT(slotImportClicked()));

    //test to see if the file actually exists before adding it
    connect(locationUrlRequestor, SIGNAL(textChanged(QString)),
            this, SLOT(checkUrl(QString)));

    connect(this, SIGNAL(urlIsGood()),
            this, SLOT(slotEnableImport()));
}

void ImportDialog::setText(dbusBook* book)
{
    titleEdit->setText(book->title);
    descEdit->setText(book->summary);
    authorEdit->setText(book->author);
    relNumEdit->setText(book->release);
    relDateEdit->setText(book->releaseDate);
    seriesEdit->setText(book->series);
    volumeEdit->setText(book->volume);
    genreEdit->setText(book->genre);
    locationUrlRequestor->setUrl(KUrl(book->url));

    //change the text of the button to save from import
    ImportButton->setText(i18n("&Save"));
}


void ImportDialog::slotImportClicked()
{
    dbusBook book;
    book.title = titleEdit->text();
    book.summary = descEdit->toPlainText();
    book.author = authorEdit->text();
    book.release = relNumEdit->text();
    book.releaseDate = relDateEdit->text();
    book.genre = genreEdit->text();
    book.series = seriesEdit->text();
    book.volume = volumeEdit->text();
    book.url = locationUrlRequestor->url().url();
    emit signalNewBook(book);
    close();
}

void ImportDialog::slotEnableImport()
{
    ImportButton->setEnabled(true);
}


void ImportDialog::checkUrl(QString url)
{
    KUrl file = KUrl(url);
    bool read = true;
    if (KIO::NetAccess::exists(file, read, NULL)) {
        emit urlIsGood();
    }
}
