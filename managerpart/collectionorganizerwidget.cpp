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
#include "collectionorganizerwidget.h"
#include "collectionsizecalculator.h"
#include "collectiondb.h"
#include "collectionorganizer.h"

// KDE includes
#include <kurl.h>
#include <kdiskfreespaceinfo.h>
#include <kmessagebox.h>

// Qt includes
#include <qdir.h>
#include <QRegExpValidator>


CollectionOrganizerWidget::CollectionOrganizerWidget(CollectionDB * collection,
        QWidget * parent,
        Qt::WindowFlags flags)
    : QWidget(parent, flags), m_collection(collection)
{
    setupUi(this);
    rootFolderUrlRequester->setMode(KFile::Directory | KFile::LocalOnly);
    
    // set regular expression validator for structure line edit
    //QRegExp structureRegExp;
    //QString structurePattern = "(.+|.*(\%.+\%.*)+)(/(.+|.*(\%.+\%.*)+))";
    //structureRegExp.setPattern(structurePattern);
    //structureRegExp.setMinimal(true);
    //QRegExpValidator * structureValidator = new QRegExpValidator(structureRegExp, this);
    //structureLineEdit->setValidator(structureValidator);

    m_computeDiskSpace();
}


CollectionOrganizerWidget::~CollectionOrganizerWidget()
{

}


void CollectionOrganizerWidget::organizeCollection()
{
    // check for validity of fields
    KUrl rootFolderUrl = rootFolderUrlRequester->url();
    if (rootFolderUrl.path().isEmpty()) {
        KMessageBox::error(this, i18n("Root folder must be a valid path"),
                i18n("Invalid root folder"));
        return;
    }
    if (!rootFolderUrl.isLocalFile()) {
        KMessageBox::error(this, i18n("Root folder of the collection must be a "
                    "local file"), i18n("Invalid root folder"));
        return;
    }
    // initialize CollectionOrganizer object
    CollectionOrganizer organizer(m_collection, this);
    // set root folder and collection structure
    organizer.setRootFolderUrl(rootFolderUrl);
    organizer.setCollectionStructure(structureLineEdit->text());
    organizer.organizeCollection();
}

// private slots
void CollectionOrganizerWidget::sizeComputed(quint64 size)
{
    m_requiredSpace = size;
    KLocale * locale = KGlobal::locale();
    QString sizeStr = locale->formatByteSize(m_requiredSpace);
    requiredValueLabel->setText(sizeStr);
    // WARNING possible race condition between the computation of
    // available space and required space might display an incorrect value for
    // remaining space
    m_remainingSpace = m_availableSpace - m_requiredSpace;
    QString remainingSpaceStr = locale->formatByteSize(m_remainingSpace);
    afterProcessValueLabel->setText(remainingSpaceStr);
}


// private methods
void CollectionOrganizerWidget::m_computeDiskSpace()
{
    utils::CollectionSizeCalculator * sizeCalculator = new utils::CollectionSizeCalculator(this);

    connect(sizeCalculator, SIGNAL(sizeComputed(quint64)),
            SLOT(sizeComputed(quint64)));
    
    // FIXME potential memory leak because we don't know when
    // CollectionOrganizerWidget is destroyed

    sizeCalculator->calculateSize();

    // compute available disk space
    // FIXME temporary return available space in the /home partition (which might be separated)
    KDiskFreeSpaceInfo info = KDiskFreeSpaceInfo::freeSpaceInfo(QDir::homePath());
    if (info.isValid()) {
        // display the available space
        m_availableSpace = info.available();
        KLocale * locale = KGlobal::locale();
        QString availableSpaceStr = locale->formatByteSize(m_availableSpace);
        availableValueLabel->setText(availableSpaceStr);
    }
}

