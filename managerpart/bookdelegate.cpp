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
#include <qstringbuilder.h>
#include <qfontmetrics.h>

#include <KDebug>
#include <kimagecache.h>
#include <kiconloader.h>
#include <klocalizedstring.h>

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
    
    xOffsetForText = 50;
    yOffsetForText = 15;
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
        
        if (option.state & QStyle::State_Selected) {
            painter->fillRect(option.rect, option.palette.highlight());
        }
        
        // draw the image
        painter->drawImage(option.rect.x() + xOffsetForPreview,
                           option.rect.y() + yOffsetForPreview,
                           preview);
        
        QString title = index.data(CollectionTreeModel::TitleRole).toString();
        QString release = index.data(CollectionTreeModel::RelNumberRole).toString();
        QString releaseDate = index.data(CollectionTreeModel::RelDateRole).toString();
        QString genre = index.data(CollectionTreeModel::GenreRole).toString();
        QString series = index.data(CollectionTreeModel::SeriesRole).toString();
        QString volume = index.data(CollectionTreeModel::VolumeRole).toString();
        
        QFont font = painter->font();
        font.setItalic(true);
        painter->setFont(font);
        QFontMetrics italicMetrics(font);
        
        int xOffset = option.rect.x() + xOffsetForPreview + preview.width() + xOffsetForText;
        // set y-offset accordingly for positioning the data vertically centered
        // multiplied by 4 since there are four rows of text
        int yOffset = option.rect.y() + (option.rect.height() - (4 * italicMetrics.height())) / 2;
        
        yOffset += italicMetrics.height();
        QPoint mainOffset(xOffset, yOffset);
        QPoint italicSpacingPoint(0, italicMetrics.lineSpacing());
        
        // draw the "Title" string
        painter->drawText(mainOffset, i18nc("This book's title", "Title: "));
        
        // draw the "Release Number" string
        painter->drawText(mainOffset + italicSpacingPoint,
                          i18nc("The books Edition or Release number", "Release Number: "));
        
        
        // draw the "Release Date" string
        painter->drawText(mainOffset + italicSpacingPoint +
        QPoint(italicMetrics.width(i18n("Release Number: ") % release % "\t"), 0),
        i18nc("This book's Release Date", "Release Date: "));
        
        // draw the "Series" string
        painter->drawText(mainOffset + 2 * italicSpacingPoint, i18nc("The Series of a book", "Series: "));
        
        // draw the "Volume" string
        painter->drawText(mainOffset + 2 * italicSpacingPoint +
        QPoint(italicMetrics.width(i18n("Series: ") % series % "\t"), 0),
        i18nc("The Volume number of a book", "Volume: "));
        
        // draw the "Genre" string
        painter->drawText(mainOffset + 3 * italicSpacingPoint,
                          i18nc("The type or style of a book, examples: Science Fiction, History",
                                "Genre: "));
        
        
        // draw the actual data of the book
        QPoint titleOffset = mainOffset + QPoint(italicMetrics.width(i18n("Title: ")), 0);
        QPoint relNumOffset = mainOffset + italicSpacingPoint + QPoint(italicMetrics.width(i18n("Release Number: ")), 0);
        QPoint relDateOffset = relNumOffset + QPoint(italicMetrics.width(release % "\t" % i18n("Release Date: ")), 0);
        QPoint seriesOffset = mainOffset + 2 * italicSpacingPoint + QPoint(italicMetrics.width(i18n("Series: ")), 0);
        QPoint volumeOffset = seriesOffset + QPoint(italicMetrics.width(series % "\t" % i18n("Volume: ")), 0);
        QPoint genreOffset = mainOffset + 3 * italicSpacingPoint + QPoint(italicMetrics.width(i18n("Genre: ")), 0);
        
        // set appropriate font for title
        font.setItalic(false);
        font.setBold(true);
        painter->setFont(font);
        
        // draw title data
        painter->drawText(titleOffset, title);
        
        // reset font
        font.setBold(false);
        painter->setFont(font);
        
        // draw other data
        painter->drawText(relNumOffset, release);
        painter->drawText(relDateOffset, releaseDate);
        painter->drawText(seriesOffset, series);
        painter->drawText(volumeOffset, volume);
        painter->drawText(genreOffset, genre);        
    } else {
        // author
        // TODO paint author's information
        QStyledItemDelegate::paint(painter, option, index);
    }
}


QSize BookDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    if (index.parent().isValid()) {
        // book
        return QSize(option.rect.width(), ThumbnailSize.height() + 30);
    }
    
    return QStyledItemDelegate::sizeHint(option, index);
}
