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

#ifndef COLLECTION_ORGANIZER_H
#define COLLECTION_ORGANIZER_H

// Qt includes
#include <qwidget.h>

// Uis includes
#include "ui_collectionorganizerwidget.h"

// Forward declarations
class CollectionDB;

class CollectionOrganizerWidget : public QWidget, private Ui::CollectionOrganizerWidget {
public:
    CollectionOrganizerWidget (
            CollectionDB * collection,
            QWidget * parent = 0,
            Qt::WindowFlags flags = 0);
    virtual ~CollectionOrganizerWidget ();

private:
    void m_computeDiskSpace();
    CollectionDB * m_collection;
};

#endif // COLLECTION_ORGANIZER_H
