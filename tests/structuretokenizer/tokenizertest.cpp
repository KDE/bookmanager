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

// Book Manager test includes
#include "tokenizertest.h"


void TokenizerTest::testOtherTokens()
{
    TokenList_t result = st.tokenize("other");
    QCOMPARE(result.size(), 1);

}

void TokenizerTest::testSeparators()
{
    TokenList_t result = st.tokenize("test/with/separators");
    QCOMPARE(result.size(), 5);
    QCOMPARE(result.at(0).type, tokenizer::Other);
    QCOMPARE(result.at(1).type, tokenizer::Separator);
    QCOMPARE(result.at(2).type, tokenizer::Other);
    QCOMPARE(result.at(3).type, tokenizer::Separator);
    QCOMPARE(result.at(4).type, tokenizer::Other);
    QCOMPARE(result.at(0).tkString, QString("test"));
    QCOMPARE(result.at(2).tkString, QString("with"));
    QCOMPARE(result.at(4).tkString, QString("separators"));
}

void TokenizerTest::testMixed()
{
    TokenList_t result = st.tokenize("mixing%title%tokens_and_other");
    QCOMPARE(result.size(), 3);
    QCOMPARE(result.at(0).type, tokenizer::Other);
    QCOMPARE(result.at(1).type, tokenizer::Title);
    QCOMPARE(result.at(2).type, tokenizer::Other);
    QCOMPARE(result.at(0).tkString, QString("mixing"));
    QCOMPARE(result.at(2).tkString, QString("tokens_and_other"));

    result = st.tokenize("%Author%%author%%genre%");
    QCOMPARE(result.size(), 3);
    QCOMPARE(result.at(0).type, tokenizer::AuthorInitial);
    QCOMPARE(result.at(1).type, tokenizer::Author);
    QCOMPARE(result.at(2).type, tokenizer::Genre);

    result = st.tokenize("othertext%title%%author%");
    QCOMPARE(result.size(), 3);
    QCOMPARE(result.at(0).type, tokenizer::Other);
    QCOMPARE(result.at(1).type, tokenizer::Title);
    QCOMPARE(result.at(2).type, tokenizer::Author);
    QCOMPARE(result.at(0).tkString, QString("othertext"));
    QCOMPARE(result.at(1).tkString, QString("%title%"));
    QCOMPARE(result.at(2).tkString, QString("%author%"));
}

void TokenizerTest::testMixedWithSeparators()
{
    TokenList_t result = st.tokenize("test/mixed/%title%with/separators/%author%");
    QCOMPARE(result.size(), 10);
    QCOMPARE(result.at(0).type, tokenizer::Other);
    QCOMPARE(result.at(1).type, tokenizer::Separator);
    QCOMPARE(result.at(2).type, tokenizer::Other);
    QCOMPARE(result.at(3).type, tokenizer::Separator);
    QCOMPARE(result.at(4).type, tokenizer::Title);
    QCOMPARE(result.at(5).type, tokenizer::Other);
    QCOMPARE(result.at(6).type, tokenizer::Separator);
    QCOMPARE(result.at(7).type, tokenizer::Other);
    QCOMPARE(result.at(8).type, tokenizer::Separator);
    QCOMPARE(result.at(9).type, tokenizer::Author);
    QCOMPARE(result.at(1).tkString, QString("/"));
}

QTEST_MAIN(TokenizerTest)
#include "tokenizertest.moc"
