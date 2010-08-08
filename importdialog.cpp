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
    //should really be setting these in the ui file...
    //cancelButton->setIcon(KIcon("dialog-close"));
    //ImportButton->setIcon(KIcon("dialog-ok-apply"));

    //resume the regularly scheduled setting up the dialog
    setupUi(this);
    connect(ImportButton, SIGNAL(clicked()),
            this, SLOT(slotImportClicked()));

    //test to see if the file actually exists before adding it
    connect(locationUrlRequestor, SIGNAL(textChanged(QString)),
            this, SLOT(checkUrl(QString)));

    connect(this, SIGNAL(urlIsGood()),
            this, SLOT(slotEnableImport()));
}

void ImportDialog::init(QString title, QString summary, QString author, QString release, QString releaseDate, QString genre, KUrl* url)
{
    //we set these to an empty string so setting them without testing to verify
    //real values should be ok? exept for the url, which can't be zero so... ima check that
    titleEdit->setText(title);
    descEdit->setText(summary);
    authorEdit->setText(author);
    relNumEdit->setText(release);
    relDateEdit->setText(releaseDate);
    genreEdit->setText(genre);
    if (url->url() != "0") {
        locationUrlRequestor->setUrl(*url);
    }

}

void ImportDialog::slotImportClicked()
{
    QString title = titleEdit->text();
    QString summary = descEdit->toPlainText();
    QString author = authorEdit->text();
    QString release = relNumEdit->text();
    QString releaseDate = relDateEdit->text();
    QString genre = genreEdit->text();
    KUrl url = locationUrlRequestor->url();
    emit signalNewBook(title, summary, author, release, releaseDate, genre, &url);
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
