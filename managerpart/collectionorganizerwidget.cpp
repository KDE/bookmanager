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

// KDE includes
#include <kurl.h>
#include <kdiskfreespaceinfo.h>

// Qt includes
#include <qdir.h>


CollectionOrganizerWidget::CollectionOrganizerWidget(CollectionDB * collection,
        QWidget * parent,
        Qt::WindowFlags flags)
    : QWidget(parent, flags), m_collection(collection)
{
    setupUi(this);
    m_computeDiskSpace();
}


CollectionOrganizerWidget::~CollectionOrganizerWidget()
{

}


// private slots
void CollectionOrganizerWidget::sizeComputed(quint64 size)
{
    m_requiredSpace = size;
    // TODO make "human readable" all sizes (KB, MB, GB)
    requiredValueLabel->setText(QString::number(m_requiredSpace) + " B");
    // WARNING possible race condition between the computation of
    // available space and required space might display an incorrect value for
    // remaining space
    m_remainingSpace = m_availableSpace - m_requiredSpace;
    afterProcessValueLabel->setText(QString::number(m_remainingSpace) + " B");
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
        availableValueLabel->setText(QString::number(m_availableSpace) + " B");
    }
}
