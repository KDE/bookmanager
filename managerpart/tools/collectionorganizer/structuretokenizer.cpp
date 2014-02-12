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
#include <qstringlist.h>


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
        // FIXME extensive testing of this method
        TokenList_t result;

        // first, split the structure using the directory separator,
        // to find the path list
        QList<int> separatorIdxList;
        // insert first base index at the begininng of the string
        separatorIdxList.append(0);
        QRegExp separatorRegExp("/");
        int pos = 0;
        while ((pos = separatorRegExp.indexIn(structure, pos)) != -1) {
            separatorIdxList.append(pos);
            pos += separatorRegExp.matchedLength();
        }
        // split the structure according to directory separators
        QStringList separatedStructure = structure.split(separatorRegExp, QString::KeepEmptyParts);
        // print error message if this condition is not verified
        Q_ASSERT(separatedStructure.size() == separatorIdxList.size());
        QRegExp tokenRegExp("%\\w%");
        // QRegExp tokenListRegExp("(.*)(%(\\w)%(.*))*");
        QListIterator<int> separatorIdxIt(separatorIdxList);
        QStringListIterator separatedStructureIt(separatedStructure);
        while (separatedStructureIt.hasNext() && separatorIdxIt.hasNext()) {
            QString currentStr = separatedStructureIt.next();
            int currentOffset = separatorIdxIt.next();
            // try to find if there are predefined tokens
            int tokenPos = 0, tokenCount = 0, previousTokenEnd = 0;
            while ((tokenPos = tokenRegExp.indexIn(currentStr, tokenPos)) != -1) {
                ++tokenCount;
                int matchingLength = tokenRegExp.matchedLength();
                // check if there is a string before the match of the token
                if (tokenPos != previousTokenEnd) {
                    // a string is present before the matched token
                    // create a token with type Other and insert it
                    Token otherToken;
                    otherToken.type = Other;
                    otherToken.startIdx = currentOffset + previousTokenEnd;
                    otherToken.endIdx = currentOffset + tokenPos - 1; // because a new token starts at tokenPos
                    // insert the token in the list
                    result.append(otherToken);
                }

                // create a new token with type depending on the matched string
                Token newToken;
                QString matchingToken = currentStr.mid(tokenPos, matchingLength);
                newToken.type = m_guessTokenFromString(matchingToken);
                newToken.startIdx = currentOffset + tokenPos;
                newToken.endIdx = newToken.startIdx + matchingLength;
                previousTokenEnd = newToken.endIdx;
                result.append(newToken);
            }
            // insert the separator in the token list, if there
            // is another element in the list of separated
            if (separatedStructureIt.hasNext()) {
                Token separator;
                separator.startIdx = currentOffset + currentStr.length();
                separator.endIdx = separator.startIdx;
                result.append(separator);
            }
        }
        
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
