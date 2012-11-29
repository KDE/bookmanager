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


#ifndef BOOKDETAILSWIDGET_H
#define BOOKDETAILSWIDGET_H

#include <qwidget.h>

class QLabel;
class QTextEdit;
class QVBoxLayout;

// FIXME use the image cache also to store large thumbnails!

class BookDetailsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BookDetailsWidget(QWidget* parent = 0);
    
    QSize sizeHint() const;
    
public slots:
    void displayBookData(const QString &location, const QString &summary);
    
private:
    void handlePreviewError();
    static const QSize thumbnailSize;
    
    QLabel *m_previewLabel;
    QTextEdit *m_summaryTextEdit;
    
    QVBoxLayout *thumbnailLayout;
        
    QString m_summary;
    QString m_location;
};

#endif // BOOKDETAILSWIDGET_H
