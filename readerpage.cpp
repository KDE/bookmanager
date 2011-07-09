/*
    Copyright (C) <2010>  Brian Korbein <bri.kor.21@gmail.com>

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

#include "readerpage.h"
#include "kurl.h"
#include "kmimetype.h"
#include "kmimetypetrader.h"
#include <KDebug>
#include "kparts/part.h"
#include "QVBoxLayout"

ReaderPage::ReaderPage(const KUrl* url, QWidget* parent)
    : QWidget(parent)
{
    QVBoxLayout *mainlayout = new QVBoxLayout;
    QString currentType = KMimeType::findByUrl(*url)->name();
    m_part = KMimeTypeTrader::createPartInstanceFromQuery<KParts::ReadOnlyPart>(currentType, this, parent);
    if (m_part) {
        m_part->widget()->setFocusPolicy(Qt::StrongFocus);
        m_part->openUrl(*url);
        mainlayout->addWidget(m_part->widget());
        setLayout(mainlayout);
    }
}

ReaderPage::~ReaderPage()
{
    m_part->deleteLater();
}


KParts::ReadOnlyPart* ReaderPage::getPart()
{
    return m_part;
}

QString ReaderPage::getMimeType()
{
    return currentType;
}
