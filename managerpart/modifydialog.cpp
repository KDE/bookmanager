/*
    Copyright (C) 2011-2012  Riccardo Bellini <ricky88ykcir@gmail.com>

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


#include "modifydialog.h"
#include "importdialog.h"
#include "collectionmodel.h"

#include <QCheckBox>
#include <KDebug>

ModifyDialog::ModifyDialog(QList<dbusBook> booklist,
                           QWidget* parent)
    : KDialog(parent)
{
    m_booklist = booklist;
    //i'd rather use iterators for everything, but for some reason they don't seem to want to work?
    //probably because I'm playing with non-qt types
    m_currentCount = 0;
    m_last = booklist.size();

    setButtons(KDialog::Ok | KDialog::Cancel | KDialog::User1 | KDialog::User2);
    setButtonText(KDialog::User2, i18nc("go to the Previous entry", "Previous"));
    setButtonGuiItem(KDialog::User2, KStandardGuiItem::back());
    setButtonText(KDialog::User1, i18nc("go to the next entry", "Next"));
    setButtonGuiItem(KDialog::User1, KStandardGuiItem::forward());

    mainWidget = createMainWidget();
    setMainWidget(mainWidget);
    setCaption(i18n("Edit Books"));

    connect(this, SIGNAL(user1Clicked()), SLOT(next()));
    connect(this, SIGNAL(user2Clicked()), SLOT(previous()));

    setupMappings();
}


QWidget* ModifyDialog::createMainWidget()
{
    QWidget *result = new QWidget(this);
    importWidget = new ImportWidget(this);

    QVBoxLayout *layout = new QVBoxLayout(result);
    // reuse import widget
    layout->addWidget(importWidget);

    return result;
}


void ModifyDialog::setupMappings()
{
    importWidget->titleEdit->setText(m_booklist.at(m_currentCount).title);
    importWidget->descEdit->setText(m_booklist.at(m_currentCount).summary);
    importWidget->authorEdit->setText(m_booklist.at(m_currentCount).author);
    importWidget->relNumEdit->setText(m_booklist.at(m_currentCount).release);
    importWidget->relDateEdit->setText(m_booklist.at(m_currentCount).releaseDate);
    importWidget->seriesEdit->setText(m_booklist.at(m_currentCount).series);
    importWidget->volumeEdit->setText(m_booklist.at(m_currentCount).volume);
    importWidget->genreEdit->setText(m_booklist.at(m_currentCount).genre);
    importWidget->locationUrlRequestor->setUrl(KUrl(m_booklist.at(m_currentCount).url));

    updateButtons();

}


void ModifyDialog::previous()
{
    // submit all changes to current book before switching, otherwise they are
    // lost forever. This also updates m_booklist
    updateBooklist();
    m_currentCount--;
    setupMappings();
}


void ModifyDialog::next()
{
    // submit all changes to current book before switching, otherwise they are
    // lost forever. This also updates m_booklist
    updateBooklist();
    m_currentCount++;
    setupMappings();
}

void ModifyDialog::updateBooklist()
{
    //update the booklist entry, but _DO NOT_ update the underlying db
    m_booklist[m_currentCount].title = importWidget->titleEdit->text();
    m_booklist[m_currentCount].summary = importWidget->descEdit->toPlainText();
    m_booklist[m_currentCount].author = importWidget->authorEdit->text();
    m_booklist[m_currentCount].release = importWidget->relNumEdit->text();
    m_booklist[m_currentCount].releaseDate = importWidget->relDateEdit->text();
    m_booklist[m_currentCount].genre = importWidget->genreEdit->text();
    m_booklist[m_currentCount].series = importWidget->seriesEdit->text();
    m_booklist[m_currentCount].volume = importWidget->volumeEdit->text();
    m_booklist[m_currentCount].url = importWidget->locationUrlRequestor->url().url();
}

void ModifyDialog::updateAndSubmit()
{
    //submit all the modified books to the db
    foreach(dbusBook curBook, m_booklist) {
        emit signalUpdateBook(curBook);
    }
}
void ModifyDialog::updateButtons()
{
    // if we are at the first element, disable "Previous" button
    enableButton(KDialog::User2, m_currentCount != 0);

    // if we are at the last element, disable "Next" button
    //we need to subtract 1 from m_last, because we want to know if their is a next entry,
    //not if we're the last entry
    enableButton(KDialog::User1, m_currentCount != m_last - 1);
}

void ModifyDialog::accept()
{
    // update current book here, otherwise it will not be modified
    updateBooklist();
    updateAndSubmit();
    KDialog::accept();
}


void ModifyDialog::reject()
{
    // abort modifications
    KDialog::reject();
}
