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

#include "Bowtie2Tests.h"

#include <U2Formats/BAMUtils.h>

namespace U2 {

#define FILE1_ATTR "file1"
#define FILE2_ATTR "file2"
#define FILE3_ATTR "file3"
#define IS_BAM_ATTR "isbam"

void GTest_Bowtie2::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);

    file1Url = el.attribute(FILE1_ATTR);
    if (file1Url.isEmpty()) {
        failMissingValue(FILE1_ATTR);
        return;
    }
    file1Url = env->getVar("TEMP_DATA_DIR") + "/" + file1Url;

    file2Url = el.attribute(FILE2_ATTR);
    if (file2Url.isEmpty()) {
        failMissingValue(FILE2_ATTR);
        return;
    }
    file2Url = env->getVar("COMMON_DATA_DIR") + "/" + file2Url;

    file3Url = el.attribute(FILE3_ATTR);
    if (file3Url.isEmpty()) {
        failMissingValue(FILE3_ATTR);
        return;
    }
    file3Url = env->getVar("COMMON_DATA_DIR") + "/" + file3Url;

    QString isBamAtr = el.attribute(IS_BAM_ATTR);
    if (!isBamAtr.isEmpty()) {
        isBam = true;
    }else{
        isBam = false;
    }
}

Task::ReportResult GTest_Bowtie2::report() {
    bool res = BAMUtils::isEqualByLength(file1Url, file2Url, stateInfo, isBam);
    if (!res) {
        stateInfo.setError("");
        BAMUtils::isEqualByLength(file1Url, file3Url, stateInfo, isBam);
    }

    return ReportResult_Finished;
}

QList<XMLTestFactory*> Bowtie2Tests::createTestFactories() {
    QList<XMLTestFactory*> res;
    res.append(GTest_Bowtie2::createFactory());
    return res;
}
}

