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

#include <QDomElement>

#include <U2Core/TextObject.h>

#include "TextObjectTests.h"

namespace U2 {

#define OBJ_ATTR        "obj"
#define STRING_ATTR     "string"
#define WITH_LINE_BREAK "whole_line"
#define MUST_EXIST      "must_exist"
#define NEWLINES        "newlines"

void GTest_CheckStringExists::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);

    objContextName = el.attribute(OBJ_ATTR);
    if (objContextName.isEmpty()) {
        failMissingValue(OBJ_ATTR);
        return;
    }

    stringToCheck = el.attribute(STRING_ATTR);
    if (stringToCheck.isNull()) {
        failMissingValue(STRING_ATTR);
        return;
    }

    wholeLine = (el.attribute(WITH_LINE_BREAK) == "true");
    mustExist = (el.attribute(MUST_EXIST) == "true");
    newlines = (el.attribute(NEWLINES) == "true");

    while (newlines && stringToCheck.contains("\\n")) {
        stringToCheck.replace("\\n", "\n");
    }
}

Task::ReportResult GTest_CheckStringExists::report() {
    TextObject *obj = getContext<TextObject>(this, objContextName);
    if (obj == NULL) {
        stateInfo.setError(QString("invalid object context"));
        return ReportResult_Finished;
    }

    QString stringToFind = QRegExp::escape(stringToCheck);
    if (wholeLine) {
        stringToFind = "^(.*\\n)?" + QRegExp::escape(stringToCheck) + "(\\n.*)?$";
    }

    const QString text = obj->getText();
    int index = text.indexOf(QRegExp(stringToFind));

    if (mustExist) {
        if (-1 == index) {
            stateInfo.setError(QString("String doesn't exist: '%1'").arg(stringToCheck));
        }
    } else {
        if (-1 != index) {
            stateInfo.setError(QString("String unexpectedly exists: '%1' at position %2").arg(stringToCheck).arg(index));
        }
    }

    return ReportResult_Finished;
}

QList<XMLTestFactory*> TextObjectTests::createTestFactories() {
    QList<XMLTestFactory*> res;
    res.append(GTest_CheckStringExists::createFactory());
    return res;
}

}   // namespace U2
