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
#include <U2Core/AppContext.h>
#include <U2Core/U1AnnotationUtils.h>
#include <U2Core/U2Location.h>
#include <U2Formats/GenbankLocationParser.h>
#include "AnnotationUtilsTests.h"

namespace U2 {

QList<XMLTestFactory*> AnnotationUtilsTests::createTestFactories() {
    QList<XMLTestFactory*> res;
    res.append(GTest_ShiftSequence::createFactory());
    return res;
}

void GTest_ShiftSequence::init(XMLTestFormat* tf, const QDomElement& el) {
    Q_UNUSED(tf);
    bool isOk;
    locationStringBefore = el.attribute("location-before");
    locationStringAfter = el.attribute("location-after");
    shift = el.attribute("shift").toInt(&isOk);
    if (!isOk) {
        setError("Failed to parse shift value");
        return;
    }
    sequenceLength = el.attribute("sequence-length").toInt(&isOk);
    if (!isOk) {
        setError("Failed to parse sequence length value");
    }
}

Task::ReportResult GTest_ShiftSequence::report() {
    U2Location locationBefore;
    auto parsingResult = Genbank::LocationParser::parseLocation(locationStringBefore.toLatin1(),
                                                                locationStringBefore.length(),
                                                                locationBefore,
                                                                sequenceLength);
    if (parsingResult != Genbank::LocationParser::Success) {
        setError(QString("Failed to parse location before: ") + parsingResult);
        return ReportResult_Finished;
    }
    U2Location shiftedLocation = U1AnnotationUtils::shiftLocation(locationBefore, shift, sequenceLength);
    QString shiftedLocationString = U1AnnotationUtils::buildLocationString(*shiftedLocation.data());
    if (locationStringAfter != shiftedLocationString) {
        setError(QString("Expected :%1, got: %2").arg(locationStringAfter).arg(shiftedLocationString));
    }
    return ReportResult_Finished;
}

}
