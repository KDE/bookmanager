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
#include <QDataWidgetMapper>
#include <QStyledItemDelegate>
#include <QSqlError>
#include <QSqlRecord>

#include <KDebug>

ModifyDialog::ModifyDialog(const QModelIndexList& indexList,
                           CollectionModel *model,
                           QWidget* parent)
:KDialog(parent)
{
    this->indexList = indexList;
    this->collectionModel = model;
    
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
    if (indexList.size() == 1) {
        applyToAllCheckBox->setEnabled(false);
    }
    
    return result;
}


void ModifyDialog::applyToAllToggled(bool toggle)
{
    // FIXME rename labels to meaningful names...
    
    // we can't give the same title to many books
    importWidget->label->setEnabled(!toggle);
    importWidget->titleEdit->setEnabled(!toggle);
    
    // we can't assign the same URL to many books
    importWidget->label_7->setEnabled(!toggle);
    importWidget->locationUrlRequestor->setEnabled(!toggle);
    
    // we can't give the same volume number to many books
    importWidget->label_9->setEnabled(!toggle);
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
    // prepare widget mapper
    mapper = new QDataWidgetMapper(this);
    mapper->setModel(collectionModel);
    mapper->setItemDelegate(new QStyledItemDelegate(this));
    mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    
    // setup mappings
    mapper->addMapping(importWidget->titleEdit,
                       collectionModel->fieldIndex("title"));
    mapper->addMapping(importWidget->authorEdit,
                       collectionModel->fieldIndex("author"));
    mapper->addMapping(importWidget->relNumEdit,
                       collectionModel->fieldIndex("release"));
    mapper->addMapping(importWidget->relDateEdit,
                       collectionModel->fieldIndex("releaseDate"));
    mapper->addMapping(importWidget->seriesEdit,
                       collectionModel->fieldIndex("series"));
    mapper->addMapping(importWidget->volumeEdit,
                       collectionModel->fieldIndex("volume"));
    mapper->addMapping(importWidget->genreEdit,
                       collectionModel->fieldIndex("genre"));
    mapper->addMapping(importWidget->locationUrlRequestor,
                       collectionModel->fieldIndex("url"), "text");
    mapper->addMapping(importWidget->descEdit,
                       collectionModel->fieldIndex("summary"), "plainText");
    
    connect(mapper, SIGNAL(currentIndexChanged(int)), SLOT(updateButtons()));
    
    currentPosition = 0;
    
    mapper->setCurrentIndex(indexList.at(currentPosition).row());
    
    importWidget->locationUrlRequestor->
    setUrl(importWidget->locationUrlRequestor->text());
}


void ModifyDialog::previous()
{
    // submit all changes to current book before switching, otherwise they are
    // lost forever.
    if (!mapper->submit()) {
        kError() << "Failed to submit data:" <<
        collectionModel->lastError().text();
    }
    
    mapper->setCurrentIndex(indexList.at(--currentPosition).row());
    
    importWidget->locationUrlRequestor->
    setUrl(importWidget->locationUrlRequestor->text());
}


void ModifyDialog::next()
{
    // submit all changes to current book before switching, otherwise they are
    // lost forever.
    if (!mapper->submit()) {
        kError() << "Failed to submit data:" <<
        collectionModel->lastError().text();
    }
    
    mapper->setCurrentIndex(indexList.at(++currentPosition).row());
    
    importWidget->locationUrlRequestor->
    setUrl(importWidget->locationUrlRequestor->text());
}


void ModifyDialog::updateButtons()
{
    // if we are at the first element, disable "Previous" button
    enableButton(KDialog::User2, currentPosition > 0);
    
    // if we are at the last element, disable "Next" button
    enableButton(KDialog::User1, currentPosition < indexList.size() - 1);
}


void ModifyDialog::applyToAllPrivate()
{
    // save new data
    QString newAuthor = importWidget->authorEdit->text();
    QString newRelNum = importWidget->relNumEdit->text();
    QString newRelDate = importWidget->relDateEdit->text();
    QString newSeries = importWidget->seriesEdit->text();
    QString newGenre = importWidget->genreEdit->text();
    QString newDescription = importWidget->descEdit->toPlainText();
    
    // revert all previous changes not saved to this book
    mapper->revert();
    
    // iterate over items and modify data in the model
    foreach (QModelIndex currentModelIndex, indexList) {
        QSqlRecord currentRecord = collectionModel->record(currentModelIndex.row());
        
        // modify record data
        currentRecord.setValue("author", newAuthor);
        currentRecord.setValue("release", newRelNum);
        currentRecord.setValue("releaseDate", newRelDate);
        currentRecord.setValue("series", newSeries);
        currentRecord.setValue("genre", newGenre);
        currentRecord.setValue("summary", newDescription);
        
        // save changes to model
        collectionModel->setRecord(currentModelIndex.row(), currentRecord);
    }
}


void ModifyDialog::accept()
{
    // if apply to all is toggled, apply to all books
    if (applyToAllCheckBox->isChecked()) {
        applyToAllPrivate();
    } else {
        // try to submit data to underlying model
        if (!mapper->submit()) {
            kError() << "Failed to submit data:" <<
            collectionModel->lastError().text();
        }
    }
    KDialog::accept();
}


void ModifyDialog::reject()
{
    // abort modifications
    mapper->revert();
    KDialog::reject();
}
