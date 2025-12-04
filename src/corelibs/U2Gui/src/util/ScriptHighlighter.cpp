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

#include "ScriptHighlighter.h"

#include <QRegularExpression>

namespace U2 {

ScriptHighlighter::ScriptHighlighter(QTextDocument* parent)
    : QSyntaxHighlighter(parent) {
    HighlightingRule rule;

    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);
    QStringList keywordPatterns;
    keywordPatterns << "\\bvar\\b"
                    << "\\bArray\\b"
                    << "\\bfunction\\b"
                    << "\\breturn\\b"
                    << "\\barguments\\b"
                    << "\\bif\\b"
                    << "\\belse\\b"
                    << "\\bfor\\b"
                    << "\\bswitch\\b"
                    << "\\bcase\\b"
                    << "\\bbreak\\b"
                    << "\\bwhile\\b";

    foreach (const QString& pattern, keywordPatterns) {
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    // Values
    QTextCharFormat valueFormat;
    valueFormat.setForeground(Qt::blue);
    rule.format = valueFormat;
    rule.pattern = QRegularExpression("\\btrue\\b|\\bfalse\\b|\\b[0-9]+\\b");
    highlightingRules.append(rule);

    QTextCharFormat functionFormat;
    functionFormat.setForeground(Qt::darkBlue);
    rule.format = functionFormat;
    rule.pattern = QRegularExpression("\\b[A-Za-z0-9_]+(?=\\()");
    highlightingRules.append(rule);

    // Quotation

    QTextCharFormat quotationFormat;
    quotationFormat.setForeground(Qt::blue);
    rule.format = quotationFormat;
    rule.pattern = QRegularExpression("\"[^\"]*\"");
    highlightingRules.append(rule);

    // Single Line Comments
    QTextCharFormat singleLineCommentFormat;
    singleLineCommentFormat.setForeground(Qt::darkGreen);
    rule.format = singleLineCommentFormat;
    rule.pattern = QRegularExpression("//[^\n]*");
    highlightingRules.append(rule);

    multiLineCommentFormat.setForeground(Qt::darkGreen);

    commentStartExpression = QRegularExpression("/\\*");
    commentEndExpression = QRegularExpression("\\*/");
}

void ScriptHighlighter::highlightBlock(const QString& text) {
    for (const HighlightingRule& rule : highlightingRules) {
        QRegularExpression expression = rule.pattern;
        QRegularExpressionMatch match = expression.match(text);
        while (match.hasMatch()) {
            int index = match.capturedStart(0);
            int length = match.capturedLength(0);
            setFormat(index, length, rule.format);
            match = expression.match(text, index + length);
        }
    }
    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1) {
        QRegularExpressionMatch match = commentStartExpression.match(text);
        startIndex = match.hasMatch() ? match.capturedStart(0) : -1;
    }

    while (startIndex >= 0) {
        QRegularExpressionMatch endMatch = commentEndExpression.match(text, startIndex);
        int commentLength;
        if (!endMatch.hasMatch()) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endMatch.capturedStart(0) - startIndex + endMatch.capturedLength(0);
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);

        QRegularExpressionMatch nextMatch = commentStartExpression.match(text, startIndex + commentLength);
        startIndex = nextMatch.hasMatch() ? nextMatch.capturedStart(0) : -1;
    }
}


}  // namespace U2
