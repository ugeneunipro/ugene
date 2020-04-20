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

#include "FormatDetectionTests.h"

#include <QDomElement>

namespace U2 {

/* attributes */
static const QString URL("url");                // file to check
static const QString FORMAT("format");          // check detection score for this format

// GTest_CheckScore specific
static const QString EQUAL_TO("equal-to");      // score should be equal to
static const QString LESS_THAN("less-than");    // score should be less than
static const QString MORE_THAN("more-than");    // score should be more than


/* class GTest_CheckScore : public XmlTest */

static bool equal_to(int a, int b) { return a == b; }
static bool less_than(int a, int b) { return a < b; }
static bool more_than(int a, int b) { return a > b; }

typedef bool(*CmpFun)(int,int);
static CmpFun getCmpFun(const QString &name) {
    if (name == EQUAL_TO) {
        return equal_to;
    }
    else if (name == LESS_THAN) {
        return less_than;
    }
    else if (name == MORE_THAN) {
        return more_than;
    }

    return 0;
}

void GTest_CheckScore::init(XMLTestFormat*, const QDomElement& el) {
    QString fileURLStr = el.attribute(URL);
    if (fileURLStr.isEmpty()) {
        failMissingValue(URL);
    }

    QString commonDataDir = env->getVar("COMMON_DATA_DIR");
    fileURL = commonDataDir + "/" + fileURLStr;

    QString expectedFormatStr = el.attribute(FORMAT);
    if (expectedFormatStr.isEmpty()) {
        failMissingValue(FORMAT);
    }
    // WARNING expectedFormat here may be invalid foramat Id
    // it's normal test will just fail
    expectedFormat = DocumentFormatId(expectedFormatStr);

    // There is no ability to check if one constraint given multiple times
    // last value will be taken in this case
    const QDomNamedNodeMap &attrs = el.attributes();
    int constraintn = 0;
    constraintn += attrs.contains(EQUAL_TO);
    constraintn += attrs.contains(LESS_THAN);
    constraintn += attrs.contains(MORE_THAN);

    if (constraintn < 1 || constraintn > 1) {
        stateInfo.setError("Bad constraints given");
        return;
    }

    // now there exactly one constraint
    cmpFunStr = (attrs.contains(EQUAL_TO)) ? EQUAL_TO : cmpFunStr;
    cmpFunStr = (attrs.contains(LESS_THAN)) ? LESS_THAN : cmpFunStr;
    cmpFunStr = (attrs.contains(MORE_THAN)) ? MORE_THAN : cmpFunStr;

    QString valueStr = el.attribute(cmpFunStr);
    bool ok;
    value = valueStr.toInt(&ok);
    if (!ok) {
        stateInfo.setError("Bad constraint value given");
        return;
    }
}

void GTest_CheckScore::run() {
    FormatDetectionConfig conf;
    conf.useImporters = true;
    conf.bestMatchesOnly = false;

    matchedFormats = DocumentUtils::detectFormat(fileURL, conf);
}

Task::ReportResult GTest_CheckScore::report() {
    CmpFun cmpFun = getCmpFun(cmpFunStr);
    assert(cmpFun);

    int score = 0;
    foreach (const FormatDetectionResult &dr, matchedFormats) {
        if (dr.format->getFormatId() == expectedFormat) {
            score = dr.score();
        }
    }

    if (!cmpFun(score, value)) {
        setError(QString("Matching score for %1 is %2, expected %3 %4").arg(expectedFormat).arg(score).arg(cmpFunStr).arg(value));
    }

    return ReportResult_Finished;
}


/*class GTest_PerfectMatch : public XmlTest */

void GTest_PerfectMatch::init(XMLTestFormat*, const QDomElement& el) {
    QString fileURLStr = el.attribute(URL);
    if (fileURLStr.isEmpty()) {
        failMissingValue(URL);
    }

    QString commonDataDir = env->getVar("COMMON_DATA_DIR");
    fileURL = commonDataDir + "/" + fileURLStr;

    QString expectedFormatStr = el.attribute(FORMAT);
    if (expectedFormatStr.isEmpty()) {
        failMissingValue(FORMAT);
    }
    // WARNING expectedFormat here may be invalid foramat Id
    // it's normal test will just fail
    expectedFormat = DocumentFormatId(expectedFormatStr);
}

void GTest_PerfectMatch::run() {
    FormatDetectionConfig conf;
    conf.useImporters = false;
    conf.bestMatchesOnly = false;

    matchedFormats = DocumentUtils::detectFormat(fileURL, conf);
}

Task::ReportResult GTest_PerfectMatch::report() {
    if (hasError()) {
        return ReportResult_Finished;
    }

    if (!matchedFormats.isEmpty()) {
        if (matchedFormats.size() > 1 &&
            (matchedFormats[0].score() == matchedFormats[1].score()
            || (matchedFormats[1].score() > FormatDetection_AverageSimilarity && matchedFormats[0].score() < FormatDetection_Matched)
            || (matchedFormats[0].score() <= FormatDetection_AverageSimilarity)))
        {
            // matched to multipe formats
            QString matchedFormatsStr;
            foreach (const FormatDetectionResult &dr, matchedFormats) {
                assert(dr.format && "Importers should be disabled");
                matchedFormatsStr += QString("%1 (score: %2), ").arg(dr.format->getFormatId()).arg(dr.score());
            }
            matchedFormatsStr.chop(2);

            stateInfo.setError(QString("Matched to multiple formats: %1; expected %2").arg(matchedFormatsStr).arg(expectedFormat));
        }
        else {
            // matched exactly
            FormatDetectionResult &dr =  matchedFormats.first();
            assert(dr.format && "Importers should be disabled");

            if (dr.format->getFormatId() != expectedFormat) {
                stateInfo.setError(QString("Matched to %1 (score: %2) format, expected %3").arg(dr.format->getFormatId()).arg(dr.score()).arg(expectedFormat));
            }
        }
    }
    else {
        // not matched
        stateInfo.setError(QString("Not matched to any format, expected: %1").arg(expectedFormat));
    }

    return ReportResult_Finished;
}

/* class FormatDetectionTests */
QList<XMLTestFactory*> FormatDetectionTests::createTestFactories() {
    QList<XMLTestFactory*> res;

    res.append(GTest_CheckScore::createFactory());
    res.append(GTest_PerfectMatch::createFactory());

    return res;
}

}   // namespace U2
