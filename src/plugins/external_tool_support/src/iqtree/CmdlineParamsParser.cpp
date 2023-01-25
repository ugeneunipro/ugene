/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include "CmdlineParamsParser.h"

#include <QRegularExpression>

#include <U2Core/U2OpStatus.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {

QStringList CmdlineParamsParser::parse(U2OpStatus& os, const QString& rawParamsString) {
    static const QString MASKED_QUOTE_CHARACTER = QChar(0x200B);  // A character we use to keep escaped quotes: zero-width space.

    CHECK(!rawParamsString.isEmpty(), {});

    QString line = rawParamsString;
    QRegularExpression maskingQuoteRegExp(R"((?<!\\)\\")");  // Escaped quotes. Check that it is a quote is escaped but not a symbol before.
    line.replace(maskingQuoteRegExp, MASKED_QUOTE_CHARACTER);

    CHECK_EXT(line.count("\"") % 2 == 0, os.setError(tr("Failed to parse parameters: found anon-terminated quote")), {});

    // See https://stackoverflow.com/questions/4031900/split-a-string-by-whitespace-keeping-quoted-segments-allowing-escaped-quotes.
    QRegularExpression regExp(R"((?=\S)[^"\s]*(?:"[^\\"]*(?:\\[\s\S][^\\"]*)*"[^"\s]*)*)");
    QStringList tokens;
    for (auto it = regExp.globalMatch(line); it.hasNext();) {
        QStringList matchTokens = it.next().capturedTexts();
        for (QString token : qAsConst(matchTokens)) {
            token.replace("\"", "");
            token = token.trimmed();
            if (!token.isEmpty()) {
                tokens << token;
            }
        }
    }
    return tokens;
}

QString CmdlineParamsParser::getParameterValue(const QString& name, const QStringList& parameters) {
    int parameterIndex = parameters.indexOf(name);
    CHECK(parameterIndex >= 0 && parameterIndex < parameters.length() - 1, "")
    return parameters[parameterIndex + 1];
}

void CmdlineParamsParser::removeParameterNameAndValue(const QString& name, QStringList& parameters) {
    int nameIndex = parameters.indexOf(name);
    CHECK(nameIndex >= 0, );
    parameters.removeAt(nameIndex);  // Remove name.
    if (parameters.length() > nameIndex) {
        parameters.removeAt(nameIndex);  // Remove value.
    }
}

}  // namespace U2
