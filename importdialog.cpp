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

ImportDialog::ImportDialog(QWidget* parent)
  : KDialog(parent)
{
  //should really be setting these in the ui file...
  ui->cancelButton->icon(KIcon("dialog-close"));
  ui->ImportButton->icon(KIcon("dialog-ok-apply"));
  
  //resume the regularly scheduled setting up the dialog
  ui->setupUi();
  connect(ui->ImportButton, SIGNAL(clicked()),
	  this, SLOT(slotImportClicked()));
	  
  //this really isn't a good test to see if we're ready to import...
  connect(ui->locationUrlRequestor, SIGNAL(textChanged()),
	  this, SLOT(slotEnableImport()));
}

ImportDialog::ImportDialog(QWidget* parent, QString title, QString summary, QString author, QString release, QString releaseDate, QString genre, KUrl* url): KDialog(parent, title, summary, author, release, releaseDate, genre, url)
{
  //hopefully this is a good idea...
  ImportDialog(parent);
  //we set these to an empty string so setting them without testing to verify
  //real values should be ok? exept for the url, which can't be zero so... ima check that
  ui->titleEdit->setText(title);
  ui->descEdit->setText(summary);
  ui->authorEdit->setText(author);
  ui->relNumEdit->setText(release);
  ui->relDateEdit->setText(releaseDate);
  ui->genreEdit->setText(genre);
  if(url != "0"){
    ui->locationUrlRequestor->setUrl(url);
  }
  
}

void ImportDialog::slotImportClicked()
{
 QString title = ui->titleEdit->text();
 QString summary = ui->descEdit->toPlainText();
 QString author = ui->authorEdit->text();
 QString release = ui->relNumEdit->text();
 QString releaseDate = ui->relDateEdit->text();
 QString genre = ui->genreEdit->text();
 KUrl url = ui->locationUrlRequestor->url();
 emit signalNewBook(title, summary,author,release,releaseDate,genre,&url);
}
