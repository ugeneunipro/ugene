/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2018 UniPro <ugene@unipro.ru>
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

#include <U2Core/AppContext.h>
#include <U2Core/AppResources.h>
#include <U2Core/AppSettings.h>
#include <U2Core/U2SafePoints.h>

#include "TopHatSettings.h"

namespace U2 {

const QString TopHatSettings::INDEX = "Index";
const QString TopHatSettings::SEQUENCE = "Sequence";

TopHatInputData::TopHatInputData()
: paired(false), fromFiles(false), workflowContext(NULL)
{
}

int TopHatInputData::size() const {
    return fromFiles ? urls.size() : seqIds.size();
}

void TopHatInputData::cleanupReads() {
    urls.clear();
    pairedUrls.clear();
    seqIds.clear();
    pairedSeqIds.clear();
}

TopHatSettings::TopHatSettings()
    : mateInnerDistance(0),
    mateStandardDeviation(0),
    noNovelJunctions(false),
    maxMultihits(0),
    segmentLength(0),
    fusionSearch(false),
    transcriptomeOnly(false),
    transcriptomeMaxHits(0),
    prefilterMultihits(false),
    minAnchorLength(0),
    spliceMismatches(0),
    readMismatches(0),
    segmentMismatches(0),
    solexa13quals(false),
    bowtieMode(vMode),
    useBowtie1(false){}

void TopHatSettings::cleanupReads() {
    data.cleanupReads();
}

Workflow::WorkflowContext * TopHatSettings::workflowContext() const {
    return data.workflowContext;
}

Workflow::DbiDataStorage * TopHatSettings::storage() const {
    CHECK(NULL != workflowContext(), NULL);
    return workflowContext()->getDataStorage();
}

uint TopHatSettings::getThreadsCount() {
    AppSettings *settings = AppContext::getAppSettings();
    SAFE_POINT(NULL != settings, "NULL settings", 1);
    AppResourcePool *pool = settings->getAppResourcePool();
    SAFE_POINT(NULL != pool, "NULL resource pool", 1);

    uint threads = pool->getIdealThreadCount();
    CHECK(0 != threads, 1);
    return threads;
}
}
