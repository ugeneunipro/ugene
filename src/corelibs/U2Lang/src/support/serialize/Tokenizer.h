/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
 * http://ugene.net
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#pragma once

#include <QTextStream>

#include <U2Core/StrPackUtils.h>
#include <U2Core/global.h>

namespace U2 {
namespace WorkflowSerialize {

class Tokenizer {
public:
    void tokenizeSchema(const QString& data);
    void tokenize(const QString& data, int unparseableBlockDepth = INT_MAX);
    void tokenizeLine(const QString& line, QTextStream& s);
    void tokenizeBlock(const QString& line, QTextStream& s);
    void addToken(const QString& t);
    void appendToken(const QString& t, bool skipEmpty = true);
    void removeCommentTokens();
    void assertToken(const QString& etalon);

    QString take();
    QString look() const;
    bool notEmpty() const {
        return !tokens.isEmpty();
    }

    enum States {
        START_WORD
    };  // States

    QStringList tokens;
    int depth;
};  // Tokenizer

class ParsedPairs {
public:
    ParsedPairs(Tokenizer& tokenizer, bool bigBlocks = false);
    ParsedPairs(const QString& data, int unparseableBlockDepth = INT_MAX);
    ParsedPairs() {
    }
    QMap<QString, QString> equalPairs;
    QMultiMap<QString, QString> blockPairs;

    QList<StrStrPair> equalPairsList;
    QList<StrStrPair> blockPairsList;

    static QPair<QString, QString> parseOneEqual(Tokenizer& tokenizer);
    static QString skipBlock(Tokenizer& tokenizer);

private:
    void init(Tokenizer& tokenizer, bool bigBlocks);
};  // ParsedPairs

}  // namespace WorkflowSerialize
}  // namespace U2
