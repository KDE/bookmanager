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


#include "bookdelegate.h"
#include "collectiontreemodel.h"
#include "constants.h"

#include <qpainter.h>
#include <qimage.h>

#include <KDebug>
#include <kimagecache.h>
#include <kiconloader.h>

BookDelegate::BookDelegate(KImageCache *cache, QWidget* parent)
:QStyledItemDelegate(parent)
{
    this->m_cache = cache;
    
    m_iconLoader = KIconLoader::global();
    QPixmap placeHolderPixmap;
    placeHolderPixmap = m_iconLoader->loadMimeTypeIcon("application-pdf",
                                                       KIconLoader::NoGroup,
                                                       KIconLoader::SizeLarge);
    placeHolderImage = placeHolderPixmap.toImage().scaled(ThumbnailSize,
                                                          Qt::KeepAspectRatio,
                                                          Qt::SmoothTransformation);
    xOffsetForPreview = 15;
    yOffsetForPreview = 15;
}


void BookDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    if (index.parent().isValid()) {
        // book
        QImage preview;
        QString key = index.data(CollectionTreeModel::PreviewRole).toString();
        
        if (!key.isNull()) {
            m_cache->findImage(key, &preview);
        } else {
            preview = placeHolderImage;
        }
        
        // draw the image
        painter->drawImage(option.rect.x() + xOffsetForPreview,
                           option.rect.y() + yOffsetForPreview,
                           preview);
        
        // TODO paint book's information
    } else {
        // author
        // TODO paint author's information
    }
    
    QStyledItemDelegate::paint(painter, option, index);
}


QSize BookDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    if (index.parent().isValid()) {
        // book
        return QSize(option.rect.width(), ThumbnailSize.height() + 30);
    }
    
    return QStyledItemDelegate::sizeHint(option, index);
}
