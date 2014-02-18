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
        TokenList_t result;

        // first, split the structure using the directory separator,
        // to find the path list
        QList<int> separatorIdxList;
        // insert first base index at the begininng of the string
        separatorIdxList.append(0);
        QRegExp separatorRegExp("/");
        int pos = 0;
        while ((pos = separatorRegExp.indexIn(structure, pos)) != -1) {
            // add 1 to the separator position, to go to the character following
            // the separator
            separatorIdxList.append(pos + 1);
            pos += separatorRegExp.matchedLength();
        }
        // split the structure according to directory separators
        QStringList separatedStructure = structure.split(separatorRegExp, QString::KeepEmptyParts);
        // print error message if this condition is not verified
        Q_ASSERT(separatedStructure.size() == separatorIdxList.size());
        QRegExp tokenRegExp("(\%.+\%)");
        // set minimal matching behaviour of the regexp, with default matching behaviour
        // subsequent pairs of tokens
        tokenRegExp.setMinimal(true);
        // QRegExp tokenListRegExp("(.*)(%(\\w)%(.*))*");
        QListIterator<int> separatorIdxIt(separatorIdxList);
        QStringListIterator separatedStructureIt(separatedStructure);
        while (separatedStructureIt.hasNext() && separatorIdxIt.hasNext()) {
            QString currentStr = separatedStructureIt.next();
            int currentOffset = separatorIdxIt.next();
            // try to find if there are predefined tokens
            int tokenPos = 0, tokenCount = 0, previousTokenEnd = -1;
            while ((tokenPos = tokenRegExp.indexIn(currentStr, tokenPos)) != -1) {
                ++tokenCount;
                int matchingLength = tokenRegExp.matchedLength();
                // check if there is a string before the match of the token
                if (tokenPos != previousTokenEnd + 1) {
                    // a string is present before the matched token
                    // create a token with type Other and insert it
                    Token otherToken;
                    otherToken.type = Other;
                    otherToken.startIdx = currentOffset + previousTokenEnd + 1;
                    otherToken.endIdx = currentOffset + tokenPos - 1; // because a new token starts at tokenPos
                    otherToken.tkString = structure.mid(otherToken.startIdx,
                            otherToken.endIdx - otherToken.startIdx + 1);
                    // insert the token in the list
                    result.append(otherToken);
                }

                // create a new token with type depending on the matched string
                Token newToken;
                QString matchingToken = currentStr.mid(tokenPos, matchingLength);
                newToken.type = m_guessTokenFromString(matchingToken);
                newToken.startIdx = currentOffset + tokenPos;
                newToken.endIdx = newToken.startIdx + matchingLength - 1;
                newToken.tkString = structure.mid(newToken.startIdx,
                        newToken.endIdx - newToken.startIdx + 1);
                result.append(newToken);

                // move previousTokenEnd and tokenPos forward
                previousTokenEnd = tokenPos + matchingLength - 1;
                tokenPos += matchingLength;
            }
            // check if there is some text after the token
            if (tokenCount > 0 && previousTokenEnd != currentStr.length() - 1) {
                // create a new token with type Other
                Token otherToken;
                otherToken.type = Other;
                otherToken.startIdx = currentOffset + previousTokenEnd + 1;
                otherToken.endIdx = currentOffset + currentStr.length() - 1;
                otherToken.tkString = structure.mid(otherToken.startIdx,
                        otherToken.endIdx - otherToken.startIdx + 1);
                result.append(otherToken);
            }
            // if no predefined token was found, simply add a token with type Other
            if (tokenCount == 0) {
                Token otherToken;
                otherToken.type = Other;
                otherToken.startIdx = currentOffset;
                otherToken.endIdx = currentOffset + currentStr.length() - 1;
                otherToken.tkString = structure.mid(otherToken.startIdx,
                        otherToken.endIdx - otherToken.startIdx + 1);
                result.append(otherToken);
            }
            // insert the separator in the token list, if there
            // is another element in the list of separated
            if (separatedStructureIt.hasNext()) {
                Token separator;
                separator.type = Separator;
                separator.startIdx = currentOffset + currentStr.length();
                separator.endIdx = separator.startIdx;
                separator.tkString = structure.mid(separator.startIdx,
                        separator.endIdx - separator.startIdx + 1);
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

    Type StructureTokenizer::m_guessTokenFromString(const QString & tokenStr) const
    {
        QHashIterator<Type, QRegExp> typeIt(m_regexForType);
        while (typeIt.hasNext()) {
            typeIt.next();
            Type type = typeIt.key();
            const QRegExp & regex = typeIt.value();
            if (regex.indexIn(tokenStr) > -1) {
                return type;
            }
        }

        return Other;
    }
}
