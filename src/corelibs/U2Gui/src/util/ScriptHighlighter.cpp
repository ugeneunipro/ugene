/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2020 UniPro <ugene@unipro.ru>
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

#include "ScriptHighlighter.h"

namespace U2 {

ScriptHighlighter::ScriptHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);
    QStringList keywordPatterns;
    keywordPatterns << "\\bvar\\b" << "\\bArray\\b" << "\\bfunction\\b"
        << "\\breturn\\b" << "\\barguments\\b" << "\\bif\\b"
        << "\\belse\\b" << "\\bfor\\b" << "\\bswitch\\b"
        << "\\bcase\\b" << "\\bbreak\\b" << "\\bwhile\\b";


    foreach (const QString &pattern, keywordPatterns) {
        rule.pattern = QRegExp(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }



    // Values
    QTextCharFormat valueFormat;
    valueFormat.setForeground(Qt::blue);
    rule.format = valueFormat;
    rule.pattern = QRegExp("\\btrue\\b|\\bfalse\\b|\\b[0-9]+\\b");
    highlightingRules.append(rule);

    QTextCharFormat functionFormat;
    functionFormat.setForeground(Qt::darkBlue);
    rule.format = functionFormat;
    rule.pattern = QRegExp("\\b[A-Za-z0-9_]+(?=\\()");
    highlightingRules.append(rule);

    // Quotation

    QTextCharFormat quotationFormat;
    quotationFormat.setForeground(Qt::blue);
    rule.format = quotationFormat;
    rule.pattern = QRegExp("\"[^\"]*\"");
    highlightingRules.append(rule);

    // Single Line Comments
    QTextCharFormat singleLineCommentFormat;
    singleLineCommentFormat.setForeground(Qt::darkGreen);
    rule.format = singleLineCommentFormat;
    rule.pattern = QRegExp("//[^\n]*");
    highlightingRules.append(rule);

    multiLineCommentFormat.setForeground(Qt::darkGreen);

    commentStartExpression = QRegExp("/\\*");
    commentEndExpression = QRegExp("\\*/");
}

void ScriptHighlighter::highlightBlock(const QString &text) {
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }
    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1) {
        startIndex = commentStartExpression.indexIn(text);
    }

    while (startIndex >= 0) {
        int endIndex = commentEndExpression.indexIn(text, startIndex);
        int commentLength;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex + commentEndExpression.matchedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
    }
}

} // namespace U2
