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

#include "CoverageInfo.h"

#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include "AssemblyModel.h"

#include <algorithm>
#include <limits>

namespace U2 {

void CoverageInfo::updateStats() {
    CHECK(!coverageInfo.isEmpty(),)
    maxCoverage = minCoverage = coverageInfo[0];

    qint64 sum = 0;

    for(int regionIndex = 0; regionIndex < coverageInfo.size(); ++regionIndex) {
        maxCoverage = std::max(maxCoverage, coverageInfo[regionIndex]);
        minCoverage = std::min(maxCoverage, coverageInfo[regionIndex]);
        sum += coverageInfo[regionIndex];
    }
    averageCoverage = (double)sum/coverageInfo.size();
}

CalcCoverageInfoTask::CalcCoverageInfoTask(const CalcCoverageInfoTaskSettings & settings_) :
BackgroundTask<CoverageInfo>("Calculate assembly coverage", TaskFlag_None), settings(settings_)
{
    tpm = Progress_Manual;
}

void CalcCoverageInfoTask::run() {

    U2AssemblyCoverageStat cachedCoverageStat;
    {
        cachedCoverageStat = settings.model->getCoverageStat(stateInfo);
        if(stateInfo.isCoR()) {
            return;
        }
    }

    qint64 modelLength = 0;
    {
        U2OpStatusImpl status;
        modelLength = settings.model->getModelLength(status);
        if(status.hasError()) {
            stateInfo.setError(status.getError());
            return;
        }
    }
    double basesPerRegion = (double)settings.visibleRange.length/settings.regions;
    double coverageStatBasesPerRegion = (double)modelLength/cachedCoverageStat.size();

    result.coverageInfo.resize(settings.regions);
    result.region = settings.visibleRange;

    if(cachedCoverageStat.isEmpty() || (coverageStatBasesPerRegion > basesPerRegion)) {
        U2AssemblyCoverageStat coverageStat;
        coverageStat.resize(settings.regions);
        {
            settings.model->calculateCoverageStat(settings.visibleRange, coverageStat, stateInfo);
            if(stateInfo.isCoR()) {
                return;
            }
        }
        assert(coverageStat.size() == settings.regions);
        for(int regionIndex = 0;regionIndex < settings.regions;regionIndex++) {
            result.coverageInfo[regionIndex] = coverageStat[regionIndex];
        }
    } else {
        for(int regionIndex = 0;regionIndex < settings.regions;regionIndex++) {
            int startPosition = qRound((settings.visibleRange.startPos + basesPerRegion*regionIndex)/coverageStatBasesPerRegion);
            int endPosition = qRound((settings.visibleRange.startPos + basesPerRegion*(regionIndex + 1))/coverageStatBasesPerRegion);
            result.coverageInfo[regionIndex] = 0;
            for(int i = startPosition;i < endPosition;i++) {
                result.coverageInfo[regionIndex] = std::max(result.coverageInfo[regionIndex], cachedCoverageStat[i]);
            }
        }
    }
    result.updateStats();
}

}
