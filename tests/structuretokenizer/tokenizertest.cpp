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
    QVERIFY(result.size() == 1);

}

void TokenizerTest::testSeparators()
{
    TokenList_t result = st.tokenize("test/with/separators");
    QVERIFY(result.size() == 5);
    QVERIFY(result.at(0).type == tokenizer::Other);
    QVERIFY(result.at(1).type == tokenizer::Separator);
    QVERIFY(result.at(2).type == tokenizer::Other);
    QVERIFY(result.at(3).type == tokenizer::Separator);
    QVERIFY(result.at(4).type == tokenizer::Other);
}

void TokenizerTest::testMixed()
{
    TokenList_t result = st.tokenize("mixing%title%tokens_and_other");
    QVERIFY(result.size() == 3);
    QVERIFY(result.at(0).type == tokenizer::Other);
    QVERIFY(result.at(1).type == tokenizer::Title);
    QVERIFY(result.at(2).type == tokenizer::Other);
    
    result = st.tokenize("%Author%%author%%genre%");
    QVERIFY(result.size() == 3);
    QVERIFY(result.at(0).type == tokenizer::AuthorInitial);
    QVERIFY(result.at(1).type == tokenizer::Author);
    QVERIFY(result.at(2).type == tokenizer::Genre);

    result = st.tokenize("othertext%title%%author%");
    QVERIFY(result.size() == 3);
    QVERIFY(result.at(0).type == tokenizer::Other);
    QVERIFY(result.at(1).type == tokenizer::Title);
    QVERIFY(result.at(2).type == tokenizer::Author);
}

void TokenizerTest::testMixedWithSeparators()
{
    TokenList_t result = st.tokenize("test/mixed/%title%with/separators/%author%");
    QVERIFY(result.size() == 10);
    QVERIFY(result.at(0).type == tokenizer::Other);
    QVERIFY(result.at(1).type == tokenizer::Separator);
    QVERIFY(result.at(2).type == tokenizer::Other);
    QVERIFY(result.at(3).type == tokenizer::Separator);
    QVERIFY(result.at(4).type == tokenizer::Title);
    QVERIFY(result.at(5).type == tokenizer::Other);
    QVERIFY(result.at(6).type == tokenizer::Separator);
    QVERIFY(result.at(7).type == tokenizer::Other);
    QVERIFY(result.at(8).type == tokenizer::Separator);
    QVERIFY(result.at(9).type == tokenizer::Author);
}

QTEST_MAIN(TokenizerTest)
#include "tokenizertest.moc"
