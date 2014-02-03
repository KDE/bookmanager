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

// Book Manager includes
#include "structuretokenizer.h"

// Qt includes
#include <qregexp.h>


namespace tokenizer
{
    StructureTokenizer::StructureTokenizer(QObject * parent)
        : QObject(parent)
    {
        m_addRegexForType(Title, "%title%");
        m_addRegexForType(Author, "%author%");
        m_addRegexForType(AuthorInitial, "%Author%");
        m_addRegexForType(Genre, "%genre%");
        m_addRegexForType(Series, "%series%");
        m_addRegexForType(Volume, "%volume%");
        m_addRegexForType(Separator, "/");
        m_addRegexForType(Other, ".+");
    }


    TokenList_t StructureTokenizer::tokenize(const QString & structure)
    {
        TokenList_t result;

        // first, split the structure using the directory separator,
        // to find the path list
        QList<int> separatorIdxList;
        QRegExp separatorRegExp("/");
        // TODO
        QRegExp tokenRegExp("%(\\w)%");
        QRegExp tokenListRegExp("(.*)(%(\\w)%(.*))*");
        
        return result;
    }

    
    TokenList_t StructureTokenizer::getLastTokenList() const
    {
        return m_lastTokenList;
    }


    // private methods
    void StructureTokenizer::m_addRegexForType(Type tokenType, const QString & pattern)
    {
        QRegExp regex(pattern);
        regex.setPatternSyntax(QRegExp::RegExp2);
        m_regexForType.insert(tokenType, regex);
    }
}
