/*
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


#include "modifydialog.h"
#include "importdialog.h"
#include "collectionmodel.h"

#include <QCheckBox>
#include <KDebug>

ModifyDialog::ModifyDialog(QList<dbusBook> booklist,
                           QWidget* parent)
:KDialog(parent)
{
    m_booklist = booklist;
    m_current = booklist.begin();
    //i'd rather use iterators for everything, but for some reason they don't seem to want to work?
    //probably because I'm playing with non-qt types
    m_current = 0;
    m_last = booklist.size();
    
    setButtons(KDialog::Ok | KDialog::Cancel | KDialog::User1 | KDialog::User2);
    setButtonText(KDialog::User2, i18n("< Previous"));
    setButtonText(KDialog::User1, i18n("Next >"));
    
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
    
    applyToAllCheckBox = new QCheckBox(i18n("Apply to all"));
    // add a checkbox
    layout->addWidget(applyToAllCheckBox);
    
    connect(applyToAllCheckBox, SIGNAL(toggled(bool)),
            SLOT(applyToAllToggled(bool)));
    
    applyToAllCheckBox->setChecked(false);
    
    // disable "apply to all" check box if there is only one item
    if (m_booklist.size() == 1) {
        applyToAllCheckBox->setEnabled(false);
    }
    
    return result;
}


void ModifyDialog::applyToAllToggled(bool toggle)
{    
    // we can't give the same title to many books
    importWidget->titleLabel->setEnabled(!toggle);
    importWidget->titleEdit->setEnabled(!toggle);
    
    // we can't assign the same URL to many books
    importWidget->locationLabel->setEnabled(!toggle);
    importWidget->locationUrlRequestor->setEnabled(!toggle);
    
    // we can't give the same volume number to many books
    importWidget->volumeLabel->setEnabled(!toggle);
    importWidget->volumeEdit->setEnabled(!toggle);
    
    // disable "Previous" and "Next" buttons
    enableButton(KDialog::User2, !toggle);
    enableButton(KDialog::User1, !toggle);
    
    // if unchecked, restore buttons to the correct state
    if (!toggle) {
        updateButtons();
    }
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
    updateAndSubmit();
    m_currentCount--;
    setupMappings();
}


void ModifyDialog::next()
{
    // submit all changes to current book before switching, otherwise they are
    // lost forever. This also updates m_booklist
    updateAndSubmit();
    m_currentCount++;
    setupMappings();
}
void ModifyDialog::updateAndSubmit()
{
    dbusBook updated;
    //update the booklist entry and submit the new data to the underlying db
    updated.title = importWidget->titleEdit->text();
    updated.summary = importWidget->descEdit->toPlainText();
    updated.author = importWidget->authorEdit->text();
    updated.release = importWidget->relNumEdit->text();
    updated.releaseDate = importWidget->relDateEdit->text();
    updated.genre = importWidget->genreEdit->text();
    updated.series = importWidget->seriesEdit->text();
    updated.volume = importWidget->volumeEdit->text();
    updated.url = importWidget->locationUrlRequestor->url().url();

    emit signalUpdateBook(updated);
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


void ModifyDialog::applyToAllPrivate()
{
    // iterate over items and modify data in the model
    int saved = m_currentCount;
    m_currentCount = 0;
    while(m_currentCount != m_last){
        updateAndSubmit();
    }
    m_currentCount = saved;
}


void ModifyDialog::accept()
{
    // if apply to all is toggled, apply to all books
    if (applyToAllCheckBox->isChecked()) {
        applyToAllPrivate();
    } else {
        // try to submit data to underlying model
        updateAndSubmit();
    }
    KDialog::accept();
}


void ModifyDialog::reject()
{
    // abort modifications
    KDialog::reject();
}
