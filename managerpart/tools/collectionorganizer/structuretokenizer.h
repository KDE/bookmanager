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

#ifndef STRUCTURE_TOKENIZER_H
#define STRUCTURE_TOKENIZER_H

// Book Manager includes
#include "token.h"

// Qt includes
#include <qobject.h>
#include <qhash.h>


namespace tokenizer
{
    /**
     * @class tokenizer::StructureTokenizer
     * @brief This class represent an object that retrieves token from a
     * structure
     */
    class StructureTokenizer : public QObject {
        public:
            /**
             * @brief Constructor
             *
             * @param parent parent QObject
             */
            explicit StructureTokenizer(QObject * parent = 0);

            /**
             * @brief Divide the structure in tokens
             *
             * @param structure the specified structure
             *
             * @return the list of tokens contained in the structure
             * or empty if no token are contained in the structure
             */
            TokenList_t tokenize(const QString & structure);

            /**
             * @brief get the last list of tokens computed by this object
             *
             * @return the list of tokens, empty if no list was
             * previously computed
             */
            TokenList_t getLastTokenList() const;

        private:
            void m_addRegexForType(Type tokenType, const QString & pattern);
            Type m_guessTokenFromString(const QString & tokenStr) const;
            TokenList_t m_lastTokenList;

            QHash<Type, QRegExp> m_regexForType;
    };
}


#endif // STRUCTURE_TOKENIZER_H
