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

#ifndef TOKEN_H
#define TOKEN_H

// Qt includes
#include <qlist.h>
#include <qstring.h>


/**
 * @namespace tokenizer
 * @brief This namespace contains data types related to tokens, and
 * methods to extract them.
 * The structure of the collection chosen during the organization is
 * determined by a series of tokens, which can be combined arbitrarily by the
 * user to customize the folder and file structure
 */
namespace tokenizer
{
    /**
     * @enum tokenizer::Type
     * @brief This enum determines the type of a token
     */
    enum Type {
        Title = 0,
        Author,
        AuthorInitial,
        Genre,
        Series,
        Volume,
        Separator,
        Other
    };

    /**
     * @struct tokenizer::Token
     * @brief This structure represents a token
     */
    struct Token {
        Type type; /// type of the token
        QString tkString; /// string of the token
        int startIdx; /// start index of token in the string
        int endIdx; /// end index of token in the string
    };
}


/**
 * @typedef TokenList_t
 * @brief Typedef to rename a QList of Token elements
 */
typedef QList<tokenizer::Token> TokenList_t;

#endif // TOKEN_H
