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

#include "CoveredRegionsManager.h"
#include <algorithm>
#include <iterator>

namespace U2 {

const int CoveredRegionsManager::DESIRED_REGION_LENGTH = 100;

CoveredRegionsManager::CoveredRegionsManager(const U2Region & visibleRegion_, const U2AssemblyCoverageStat & coverageInfo) :
visibleRegion(visibleRegion_)  {
    assert(!coverageInfo.empty());
    assert(!visibleRegion.isEmpty());

    //convert coverage info to covered regions
    //splitting visible region to coverageInfo.size() "covered" regions
    double step = double(visibleRegion.length)/coverageInfo.size(); //precise length of the region
    // But if regions are too small, reduce their number by joining
    int regionsToJoin = 1;
    int regionsCount = coverageInfo.size();
    if(step < DESIRED_REGION_LENGTH) {
        regionsToJoin = qRound(DESIRED_REGION_LENGTH/step);
        regionsCount /= regionsToJoin;
        step *= regionsToJoin;
    }
    for(int i = 0; i < regionsCount; ++i) {
        qint32 maxCoverage = 0;
        for(int j = 0; j < regionsToJoin; ++j) {
            maxCoverage = qMax(maxCoverage, coverageInfo[i*regionsToJoin + j]);
        }
        U2Region region(step*i, qint64(step));
        CoveredRegion coveredRegion(region, maxCoverage);
        //coreLog.trace(QString("Assembly: adding covered region %1 - %2, coverage %3").arg(region.startPos).arg(region.endPos()).arg(maxCoverage));

        allRegions.push_back(coveredRegion);
    }

};

QList<CoveredRegion> CoveredRegionsManager::getCoveredRegions(qint64 coverageLevel) const {
    QList<CoveredRegion> coveredRegions;
    foreach(CoveredRegion cr, allRegions) {
        if(cr.coverage >= coverageLevel) {
            coveredRegions.push_back(cr);
        }
    }
    return coveredRegions;
}

QList<CoveredRegion> CoveredRegionsManager::getTopCoveredRegions(int topMax, qint64 coverageLevel/*=0*/) const {
    assert(topMax > 0);
    QMultiMap<qint64, CoveredRegion> topCovered;

    for(int i = 0; i < allRegions.size(); ++i) {
        const CoveredRegion & cr = allRegions.at(i);
        if(cr.coverage >= coverageLevel) {
            topCovered.insert(cr.coverage, cr);
        }
        if(topCovered.size() > topMax) {
            topCovered.erase(topCovered.begin());
        }
    }
    assert(topCovered.size() <= topMax);
    QList<CoveredRegion> topCoveredList = topCovered.values();
    QList<CoveredRegion> result;
    //reverse copy, since topCoveredList is sorted in the ascending order
    std::copy(topCoveredList.begin(), topCoveredList.end(), std::front_inserter(result));
    return result;
}

}
