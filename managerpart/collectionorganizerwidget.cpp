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
#include "collectiondb.h"

// KDE includes
#include <kurl.h>


CollectionOrganizerWidget::CollectionOrganizerWidget(CollectionDB * collection,
        QWidget * parent,
        Qt::WindowFlags flags)
    : QWidget(parent, flags), m_collection(collection)
{
    setupUi(this);
}


CollectionOrganizerWidget::~CollectionOrganizerWidget()
{

}


// private methods
void CollectionOrganizerWidget::m_computeDiskSpace()
{
    // check if collection is valid
    if (!m_collection) {
        return;
    }
    KUrl::List urlsList = m_collection->getBookUrlsList();

    // TODO
    // FIXME move computation of required disk space in a separate thread,
    // since with a lot of books it may slow down significantly the user
    // interface when showing this widget
}
