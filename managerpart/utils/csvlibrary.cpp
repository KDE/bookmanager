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


#include "csvlibrary.h"


CSVLibrary::CSVLibrary(const QString& fileName, QIODevice::OpenMode mode)
{
    m_fileName = fileName;
    m_mode = mode;
}


CSVLibrary::~CSVLibrary()
{
    m_file.close();
}


bool CSVLibrary::open()
{
    m_file.setFileName(m_fileName);
    if (!m_file.open(m_mode)) {
        return false;
    }
    m_stream.setDevice(&m_file);
    
    return true;
}


bool CSVLibrary::writeValues(const QStringList& values)
{
    QStringListIterator valueIt(values);
    while (valueIt.hasNext()) {
        QString currentValue = valueIt.next();
        QString currentEncodedValue = m_encodeValue(currentValue);
        m_stream << currentEncodedValue;
        if (valueIt.hasNext()) {
            m_stream << ",";
        }
    }
    m_stream << "\n";
    if (m_stream.status() == QTextStream::Ok) {
        return true;
    }
    
    return false;
}


QStringList CSVLibrary::readValues()
{
    // TODO
}


QString CSVLibrary::m_encodeValue(const QString& value) const
{
    // examine string and encode it
    QString encodedValue = value;
    // does it contain double quotes?
    QRegExp doubleQuotesRegExp("\"", Qt::CaseInsensitive);
    if (value.contains(doubleQuotesRegExp)) {
        // prepend one double quote
        encodedValue.prepend("\"");
        // find all internal double quotes and represent them as a pair of double quotes
        int startFrom = 1;
        int occurrence = doubleQuotesRegExp.indexIn(encodedValue, startFrom);
        do {
            encodedValue.insert(occurrence, "\"");
            // skip inserted double quotes
            startFrom = occurrence + 2;
            occurrence = doubleQuotesRegExp.indexIn(encodedValue, startFrom);
        } while (occurrence != -1);
        // append the final double quote
        encodedValue.append("\"");
    }
    // does it contain commas or newlines?
    QRegExp commasRegExp(",", Qt::CaseInsensitive);
    QRegExp lineBreakRegExp("\n", Qt::CaseInsensitive);
    if (value.contains(commasRegExp) || value.contains(lineBreakRegExp)) {
        // surround value with double quotes, only if they were not added previously
        if (!encodedValue.startsWith("\"") && !encodedValue.endsWith("\"")) {
            encodedValue.prepend("\"");
            encodedValue.append("\"");
        }
    }
    
    return encodedValue;
}


QString CSVLibrary::m_decodeValue(const QString& value) const
{
    // TODO
}
