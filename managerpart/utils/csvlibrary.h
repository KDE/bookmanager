/*
    Copyright (C) 2013  Riccardo Bellini <riccardo.bellini1988@gmail.com>

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


#ifndef CSVLIBRARY_H
#define CSVLIBRARY_H

#include <qiodevice.h>
#include <qfile.h>
#include <qstringlist.h>
#include <qtextstream.h>


class CSVLibrary
{
public:
    explicit CSVLibrary(const QString &fileName, QIODevice::OpenMode mode);
    
    bool open();
    void close();
    
    bool writeValues(const QStringList &values);
    
    QStringList readValues();

    bool endOfFile() const;
    
    virtual ~CSVLibrary();
    
private:
    QString m_encodeValue(const QString & value) const;
    QString m_decodeValue(const QString & value) const;
    
    QFile m_file;
    QIODevice::OpenMode m_mode;
    QTextStream m_stream;
    QString m_fileName;
};

#endif // CSVLIBRARY_H
