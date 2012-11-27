/*
    Copyright (C) 2012  Riccardo Bellini <ricky88ykcir@gmail.com>

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


#include "booktreeview.h"
#include "collectiontreemodel.h"

#include <kdebug.h>

#include <qevent.h>

BookTreeView::BookTreeView(QWidget* parent)
:QTreeView(parent)
{
    
}


bool BookTreeView::viewportEvent(QEvent* event)
{
    if (event->type() == QEvent::ToolTip) {
        QHelpEvent *helpEvent = (QHelpEvent *) event;
        QModelIndex book = indexAt(helpEvent->pos());
        
        // if it's not a book don't show anything
        if (book.data(CollectionTreeModel::UrlRole).isNull()) {
           return  QTreeView::viewportEvent(event);
        }
        
        QString location = book.data(CollectionTreeModel::UrlRole).toString();
        QString summary = book.data(CollectionTreeModel::SummaryRole).toString();
        
        emit dataRequested(location, summary);
        
        return true;
    } else {
        return QTreeView::viewportEvent(event);
    }
}


void BookTreeView::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape) {
        emit hideDetails();
    }
    
    QTreeView::keyPressEvent(event);
}