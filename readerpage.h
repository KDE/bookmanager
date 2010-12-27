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


#ifndef READERPAGE_H
#define READERPAGE_H

#include <qwidget.h>


namespace KParts
{
class ReadOnlyPart;
}
class KUrl;

class ReaderPage : public QWidget
{
    Q_OBJECT
public:
    explicit ReaderPage(const KUrl *url, QWidget* parent = 0);

    KParts::ReadOnlyPart* getPart();
    QString getMimeType();
private:
    KParts::ReadOnlyPart *m_part;
    QString currentType;
};

#endif // READERPAGE_H
