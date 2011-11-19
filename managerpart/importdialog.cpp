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
#include "queryengine.h"

#include <klocale.h>
#include <kicon.h>
#include <QString>
#include <KUrl>
#include <KIO/NetAccess>

ImportDialog::ImportDialog(QWidget *parent)
    : KDialog(parent)
{
    widget = new ImportWidget(this);
    setCaption(i18n("Import a new book"));
    setButtons(KDialog::Ok | KDialog::Cancel);
    setButtonText(KDialog::Ok, i18n("Import"));
    setMainWidget(widget);

    //test to see if the file actually exists before adding it
    connect(widget->locationUrlRequestor, SIGNAL(textChanged(QString)),
            this, SLOT(checkUrl(QString)));

    connect(this, SIGNAL(urlIsGood()),
            this, SLOT(slotEnableImport()));
}

void ImportDialog::setText(dbusBook* book)
{
    widget->titleEdit->setText(book->title);
    widget->descEdit->setText(book->summary);
    widget->authorEdit->setText(book->author);
    widget->relNumEdit->setText(book->release);
    widget->relDateEdit->setText(book->releaseDate);
    widget->seriesEdit->setText(book->series);
    widget->volumeEdit->setText(book->volume);
    widget->genreEdit->setText(book->genre);
    widget->locationUrlRequestor->setUrl(KUrl(book->url));

    //change the text of the button to save from import
    setButtonText(KDialog::Ok, i18n("&Save"));
}


void ImportDialog::accept()
{
    dbusBook book;
    book.title = widget->titleEdit->text();
    book.summary = widget->descEdit->toPlainText();
    book.author = widget->authorEdit->text();
    book.release = widget->relNumEdit->text();
    book.releaseDate = widget->relDateEdit->text();
    book.genre = widget->genreEdit->text();
    book.series = widget->seriesEdit->text();
    book.volume = widget->volumeEdit->text();
    book.url = widget->locationUrlRequestor->url().url();
    emit signalNewBook(book);
    KDialog::accept();
}

void ImportDialog::slotEnableImport()
{
    enableButtonOk(true);
}


void ImportDialog::checkUrl(QString url)
{
    KUrl file = KUrl(url);
    bool read = true;
    if (KIO::NetAccess::exists(file, read, NULL)) {
        emit urlIsGood();
    }
}

ImportWidget::ImportWidget(QWidget* parent)
{
    setupUi(this);

    setupAutocompletion();
}

void ImportWidget::setupAutocompletion()
{
    Query::QueryEngine *engine = new Query::QueryEngine(Query::Author, this);

    connect(engine, SIGNAL(authorAvailable(QString)),
            SLOT(newAuthorComplete(QString)));
    connect(engine, SIGNAL(genreAvailable(QString)),
            SLOT(newGenreComplete(QString)));

    connect(engine, SIGNAL(finished()),
            SLOT(queryCompleted()));

    engine->runQuery();

    engine->setType(Query::Genre);

    engine->runQuery();
}

void ImportWidget::newAuthorComplete(const QString& author)
{
    authors.insert(author);
}

void ImportWidget::newGenreComplete(const QString& genre)
{
    genres.insert(genre);
}

void ImportWidget::queryCompleted()
{
    KCompletion *authorComp = authorEdit->completionObject();
    authorEdit->setCompletionMode(KGlobalSettings::CompletionPopup);
    authorComp->setIgnoreCase(true);
    authorComp->setOrder(KCompletion::Sorted);
    authorComp->setItems(authors.toList());

    KCompletion *genreComp = genreEdit->completionObject();
    genreEdit->setCompletionMode(KGlobalSettings::CompletionPopup);
    genreComp->setIgnoreCase(true);
    genreComp->setOrder(KCompletion::Sorted);
    genreComp->setItems(genres.toList());
}
