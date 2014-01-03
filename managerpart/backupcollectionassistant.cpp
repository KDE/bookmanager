/*
    Copyright (C) 2014  Riccardo Bellini <riccardo.bellini1988@gmail.com>

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

// Book Manager includes
#include "backupcollectionassistant.h"
#include "collectionorganizerwidget.h"
#include "collectiondb.h"

// Qt includes
#include <qcheckbox.h>

// KDE includes
#include <klocalizedstring.h>


BackupCollectionAssistant::BackupCollectionAssistant(CollectionDB * collection,
        QWidget * parent,
        Qt::WindowFlags flags)
    : KAssistantDialog(parent, flags), m_collection(collection)
{
    setCaption (i18n("Backup collection assistant"));
    m_introductionPage = new IntroductionPage;
    m_introductionPageItem = addPage (m_introductionPage,
            i18n("Introduction"));

    m_organizeCollectionPage = new OrganizeCollectionPage(m_collection);
    m_backupCollectionPageItem = addPage (m_organizeCollectionPage,
            i18n("Backup collection"));
}


BackupCollectionAssistant::~BackupCollectionAssistant()
{

}

// IntroductionPage methods
IntroductionPage::IntroductionPage(QWidget * parent, Qt::WindowFlags flags)
    : QWidget(parent, flags)
{
    setupUi(this);
}


IntroductionPage::~IntroductionPage()
{

}


// BackupCollectionPage methods
OrganizeCollectionPage::OrganizeCollectionPage(CollectionDB * collection,
        QWidget * parent,
        Qt::WindowFlags flags)
    : QWidget(parent, flags), m_collection(collection)
{
    QVBoxLayout * mainLayout = new QVBoxLayout();
    
    m_introductionLabel = new QLabel;
    m_introductionLabel->setText(i18n("Configure the output folder and the structure for the organization of your collection. "
        "After the configuration click \"Organize collection\" to begin the copy of your books."));
    m_introductionLabel->setWordWrap(true);
    
    mainLayout->addWidget(m_introductionLabel);
    
    m_collectionOrganizerWidget = new CollectionOrganizerWidget(m_collection, this);

    mainLayout->addWidget(m_collectionOrganizerWidget);

    m_organizedCheckBox = new QCheckBox();
    m_organizedCheckBox->setText(i18n("Collection already organized"));
    m_organizedCheckBox->setChecked(false);
    connect(m_organizedCheckBox, SIGNAL(clicked(bool)), SLOT(collectionOrganizedClicked(bool)));

    mainLayout->addWidget(m_organizedCheckBox);
    
    QHBoxLayout * organizeCollectionLayout = new QHBoxLayout;
    organizeCollectionLayout->addStretch();
    
    m_organizeCollectionPushButton = new QPushButton;
    m_organizeCollectionPushButton->setText(i18n("Organize collection"));
    
    organizeCollectionLayout->addWidget(m_organizeCollectionPushButton);
    organizeCollectionLayout->addStretch();
    mainLayout->addLayout(organizeCollectionLayout);

    setLayout(mainLayout);
}


OrganizeCollectionPage::~OrganizeCollectionPage()
{

}


// private slots
void OrganizeCollectionPage::collectionOrganizedClicked(bool checked)
{
    m_collectionOrganizerWidget->setEnabled(!checked);
    m_organizeCollectionPushButton->setEnabled(!checked);
}

