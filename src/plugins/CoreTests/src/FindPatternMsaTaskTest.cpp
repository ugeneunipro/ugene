/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2019 UniPro <ugene@unipro.ru>
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

#include "FindPatternMsaTaskTest.h"

#include <U2Core/DocumentModel.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>

namespace U2 {

#define IN_OBJECT_NAME "in"
#define SEARCH_REGION "searchRegion"
#define PATTERNS "patterns"
#define REMOVE_OVERLAPS "removeOverlaps"
#define MATCH_VALUE "matchValue"
#define MAX_RESULTS_TO_FIND "maxResultsToFind"
#define MAX_RESULT_REGEXP_LENGTH "maxResultRegExpLength"
#define ALGORITHM "algorithm"
#define EXPECTED_RESULTS_SIZE "resultsSize"
#define EXPECTED_REGIONS_IN_RESULTS "expectedRegionsInResults"

void GTest_FindPatternMsa::init(XMLTestFormat *tf, const QDomElement &el) {
    Q_UNUSED(tf);

    settings.findSettings.maxRegExpResultLength = 10000;
    settings.findSettings.maxResult2Find = 100000;

    inputObjectName = el.attribute(IN_OBJECT_NAME);
    if (inputObjectName.isEmpty()) {
        failMissingValue(IN_OBJECT_NAME);
        return;
    }

    QStringList patterns = el.attribute(PATTERNS).split(";");
    if (patterns.isEmpty()) {
        failMissingValue(PATTERNS);
        return;
    }

    foreach(const QString & pattern, patterns) {
        settings.patterns.append(QPair<QString, QString>("", pattern));
    }

    QString tmp = el.attribute(REMOVE_OVERLAPS);
    if (!tmp.isEmpty()) {
        if (tmp.toLower() == "true") {
            settings.removeOverlaps = true;
        }
    }

    tmp = el.attribute(MATCH_VALUE);
    if (!tmp.isEmpty()) {
        bool ok = false;
        int value = tmp.toInt(&ok);
        if (ok) {
            settings.matchValue = value;
        } else {
            wrongValue(MATCH_VALUE);
            return;
        }
    }

    tmp = el.attribute(MAX_RESULTS_TO_FIND);
    if (!tmp.isEmpty()) {
        bool ok = false;
        int value = tmp.toInt(&ok);
        if (ok) {
            settings.findSettings.maxResult2Find = value;
        } else {
            wrongValue(MAX_RESULTS_TO_FIND);
            return;
        }
    }
    
    tmp = el.attribute(MAX_RESULT_REGEXP_LENGTH);
    if (!tmp.isEmpty()) {
        bool ok = false;
        int value = tmp.toInt(&ok);
        if (ok) {
            settings.findSettings.maxRegExpResultLength = value;
        } else {
            wrongValue(MAX_RESULT_REGEXP_LENGTH);
            return;
        }
    }

    tmp = el.attribute(ALGORITHM).toLower();
    if (tmp.isEmpty()) {
        failMissingValue(ALGORITHM);
        return;
    }

    tmp = el.attribute(SEARCH_REGION);
    QStringList bounds = tmp.split(QRegExp("\\.."));
    if (bounds.size() != 2) {
        stateInfo.setError(QString("wrong value for %1").arg(SEARCH_REGION));
        return;
    }
    bool startOk, finishOk;
    int start = bounds.first().toInt(&startOk), finish = bounds.last().toInt(&finishOk);
    if (!startOk || !finishOk) {
        stateInfo.setError(QString("wrong value for %1").arg(SEARCH_REGION));
        return;
    }
    start--;
    settings.findSettings.searchRegion = U2Region(start, finish - start);

    tmp = el.attribute(ALGORITHM);
    if (tmp.isEmpty()) {
        failMissingValue(ALGORITHM);
        return;
    }
    if (tmp == "exact") {
        settings.findSettings.patternSettings = FindAlgorithmPatternSettings_Exact;
    } else if (tmp == "insdel") {
        settings.findSettings.patternSettings = FindAlgorithmPatternSettings_InsDel;
    } else if (tmp == "regexp") {
        settings.findSettings.patternSettings = FindAlgorithmPatternSettings_RegExp;
    } else if (tmp == "subst") {
        settings.findSettings.patternSettings = FindAlgorithmPatternSettings_Subst;
    } else {
        wrongValue(ALGORITHM);
        return;
    }

    tmp = el.attribute(EXPECTED_RESULTS_SIZE).toLower();
    if (!tmp.isEmpty()) {
        bool ok = false;
        int value = tmp.toInt(&ok);
        if (ok) {
            expectedResultsSize = value;
        } else {
            wrongValue(EXPECTED_RESULTS_SIZE);
        }
    } else {
        failMissingValue(EXPECTED_RESULTS_SIZE);
        return;
    }

    QString expected = el.attribute(EXPECTED_REGIONS_IN_RESULTS);
    if (!expected.isEmpty()) {
        QStringList expectedList = expected.split(QRegExp("\\,"));
        foreach (QString region, expectedList) {
            QStringList bounds = region.split(QRegExp("\\.."));
            if (bounds.size() != 2) {
                stateInfo.setError(QString("wrong value for %1").arg(EXPECTED_REGIONS_IN_RESULTS));
                return;
            }
            bool startOk, finishOk;
            int start = bounds.first().toInt(&startOk), finish = bounds.last().toInt(&finishOk);
            if (!startOk || !finishOk) {
                stateInfo.setError(QString("wrong value for %1").arg(EXPECTED_REGIONS_IN_RESULTS));
                return;
            }
            start--;
            regionsToCheck.append(U2Region(start, finish - start));
        }
    }
}

void GTest_FindPatternMsa::prepare() {
    doc = getContext<Document>(this, inputObjectName);
    if (doc == NULL) {
        stateInfo.setError(QString("context not found %1").arg(inputObjectName));
        return;
    }

    QList<GObject *> list = doc->findGObjectByType(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT);
    if (list.size() == 0) {
        stateInfo.setError(QString("container of object with type \"%1\" is empty").arg(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT));
        return;
    }

    GObject *obj = list.first();
    if (obj == NULL) {
        stateInfo.setError(QString("object with type \"%1\" not found").arg(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT));
        return;
    }
    assert(obj != NULL);
    msaObj = qobject_cast<MultipleSequenceAlignmentObject *>(obj);
    if (msaObj == NULL) {
        stateInfo.setError(QString("error can't cast to multiple alignment from GObject"));
        return;
    }

    settings.msaObj = msaObj;
    findPatternTask = new FindPatternMsaTask(settings);
    addSubTask(findPatternTask);
}

Task::ReportResult GTest_FindPatternMsa::report() {
    if (findPatternTask->hasError()) {
        return ReportResult_Finished;
    }
    const QList<FindPatternInMsaResult>& results = findPatternTask->getResults();
    int resultsCounter = 0;
    foreach (const FindPatternInMsaResult& result, results) {
        resultsCounter += result.regions.size();
    }
    if (resultsCounter != expectedResultsSize) {
        stateInfo.setError(QString("Expected and Actual lists of results are different: %1 %2")
                                   .arg(expectedResultsSize)
                                   .arg(results.size()));
        return ReportResult_Finished;
    }
    foreach (const FindPatternInMsaResult& result, results) {
        foreach(const U2Region &region, result.regions) {
            if (regionsToCheck.contains(region)) {
                regionsToCheck.removeOne(region);
            }
            if (regionsToCheck.isEmpty()) {
                break;
            }
        }
    }
    if (!regionsToCheck.isEmpty()) {
        stateInfo.setError("Not all listed regions present in result");
    }
    return ReportResult_Finished;
}

QList<XMLTestFactory *> FindPatternMsaTests::createTestFactories() {
    QList<XMLTestFactory *> res;
    res.append(GTest_FindPatternMsa::createFactory());
    return res;
}

}
