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

#include "SequenceWalkerTests.h"

#include <U2Core/U2Region.h>
#include <U2Core/GObject.h>
#include <U2Core/GObjectTypes.h>

/* TRANSLATOR U2::GTest */

namespace U2 {

#define CHUNK_ATTR      "chunk"
#define OVERLAP_ATTR    "overlap"
#define EXTRA_LEN_ATTR  "extra_len"
#define REGION_ATTR     "region"
#define REVERSE_ATTR    "reverse"
#define RESULT_ATTR     "result"

void GTest_SW_CheckRegion::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);


    QString stepStr = el.attribute(CHUNK_ATTR);
    if (stepStr.isEmpty()) {
        failMissingValue(CHUNK_ATTR);
        return;
    }
    bool ok=false;
    chunkSize = stepStr.toInt(&ok);
    if(!ok) {
        failMissingValue(CHUNK_ATTR);
        return;
    }
    QString overlapStr= el.attribute(OVERLAP_ATTR);
    if (overlapStr.isEmpty()) {
        failMissingValue(OVERLAP_ATTR);
        return;
    }
    ok=false;
    overlap = overlapStr.toInt(&ok);
    if(!ok) {
        failMissingValue(OVERLAP_ATTR);
        return;
    }
    QString extraLenStr = el.attribute(EXTRA_LEN_ATTR);
    if (extraLenStr.isEmpty()) {
        failMissingValue(EXTRA_LEN_ATTR);
        return;
    }
    ok=false;
    extraLen = extraLenStr.toInt(&ok);
    if(!ok) {
        failMissingValue(EXTRA_LEN_ATTR);
        return;
    }
    QString regionStr = el.attribute(REGION_ATTR);
    if (regionStr.isEmpty()) {
        failMissingValue(REGION_ATTR);
        return;
    }
    QStringList regNums = regionStr.split("..");
    if (regNums.size()!=2) {
        failMissingValue(REGION_ATTR);
        return;
    }
    ok=false;
    region.startPos = regNums[0].toInt(&ok);
    if(!ok) {
        failMissingValue(REGION_ATTR);
        return;
    }
    region.length= regNums[1].toInt(&ok)-region.startPos;
    if(!ok) {
        failMissingValue(REGION_ATTR);
        return;
    }
    QString reventNorm_str = el.attribute(REVERSE_ATTR);
    if (reventNorm_str == "true") {
        reventNorm = true;
    } else if(reventNorm_str == "false") {
        reventNorm = false;
    } else {
        failMissingValue(REVERSE_ATTR);
        return;
    }
    QString regions_str = el.attribute(RESULT_ATTR);
    if (regions_str.isEmpty()) {
        failMissingValue(RESULT_ATTR);
        return;
    }
    QRegExp rx("(\\d+)(..)(\\d+)");
    QStringList list;
    int pos = 0;
    while ((pos = rx.indexIn(regions_str, pos)) != -1) {
        int start=rx.cap(1).toInt();
        int end=rx.cap(3).toInt();
        result.append(U2Region(start,end-start));
        pos += rx.matchedLength();
    }
}

Task::ReportResult GTest_SW_CheckRegion::report() {
    if (hasError()) {
        return ReportResult_Finished;
    }
    QVector<U2Region> chunks = SequenceWalkerTask::splitRange(region, chunkSize, overlap, extraLen, reventNorm);
    int n = result.size();
    if (n!=chunks.size()) {
        stateInfo.setError(QString("number of regions not matched: %1, expected %2").arg(chunks.size()).arg(n));
        return ReportResult_Finished;
    }
    for(int i=0; i<n; i++) {
        const U2Region& region = result[i];
        const U2Region& chunk = chunks[i];
        if (chunk!=region) {
            stateInfo.setError(QString("location not matched, idx=%1, \"%2..%3\", expected \"%4..%5\"")
                .arg(i).arg(chunk.startPos).arg(chunk.endPos()).arg(region.startPos).arg(region.endPos()));
            return ReportResult_Finished;
        }
    }

    return ReportResult_Finished;
}

QList<XMLTestFactory*> SequenceWalkerTests::createTestFactories() {
    QList<XMLTestFactory*> res;
    res.append(GTest_SW_CheckRegion::createFactory());
    return res;
}

}//namespace
