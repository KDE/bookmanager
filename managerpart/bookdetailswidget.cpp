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


#include "bookdetailswidget.h"

#include <kurl.h>
#include <klocalizedstring.h>

#include <qscrollarea.h>
#include <QVBoxLayout>
#include <qstringbuilder.h>
#include <qlabel.h>
#include <qtextedit.h>

#include <poppler/qt4/poppler-qt4.h>

const QSize BookDetailsWidget::thumbnailSize(300, 350);

BookDetailsWidget::BookDetailsWidget(QWidget* parent)
:QWidget(parent)
{
    m_previewLabel = new QLabel;
    m_summaryTextEdit = new QTextEdit;
    
    m_previewLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    
    m_summaryTextEdit->setReadOnly(true);
    m_summaryTextEdit->setAcceptRichText(true);
    m_summaryTextEdit->setFrameShadow(QFrame::Sunken);
    
    thumbnailLayout = new QVBoxLayout;
    thumbnailLayout->addWidget(m_previewLabel);
    thumbnailLayout->setAlignment(m_previewLabel, Qt::AlignHCenter | Qt::AlignVCenter);
    
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(thumbnailLayout);
    mainLayout->addWidget(m_summaryTextEdit);
    
    setLayout(mainLayout);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
}


void BookDetailsWidget::displayBookData(const QString& location, const QString& summary)
{
    // if location is already set do not display everything again
    if (m_location == location) {
        show();
        return;
    }
    
    m_location = location;
    
    m_summary = i18n("<i>Summary:</i>") % QString(" ") % summary;
    
    m_previewLabel->clear();
    m_summaryTextEdit->clear();
    
    KUrl locationUrl(m_location);
    
    m_summaryTextEdit->setText(m_summary);
    
    bool error = false;
    
    Poppler::Document *document = Poppler::Document::load(locationUrl.path());
    Poppler::Page *page = 0;
    if (document) {
        page = document->page(0);
        
        if (page) {
            QImage image = page->renderToImage().scaled(thumbnailSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            
            if (!image.isNull()) {
                m_previewLabel->setPixmap(QPixmap::fromImage(image));
            } else {
                error = true;
            }
        } else {
            error = true;
        }
        
    } else {
        error = true;
    }
    
    if (error) {
        handlePreviewError();
    }
    
    delete page;
    delete document;
    
    show();
}


void BookDetailsWidget::handlePreviewError()
{
    m_previewLabel->setFixedSize(thumbnailSize);
    m_previewLabel->setText(i18n("<i>No preview available</i>"));
    
    show();
}


QSize BookDetailsWidget::sizeHint() const
{
    return QSize(300, height());
}
