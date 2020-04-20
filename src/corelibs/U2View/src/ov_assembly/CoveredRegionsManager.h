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

#ifndef __COVERED_REGIONS_MANAGER_H__
#define __COVERED_REGIONS_MANAGER_H__

#include <QVector>
#include "CoverageInfo.h"

namespace U2 {

/**
 * Wrapper for region and its coverage. Here coverage is number of reads
 * intersecting the region.
 */
struct CoveredRegion {
    CoveredRegion(const U2Region & region_, qint64 coverage_) : region(region_), coverage(coverage_) {}
    U2Region region;
    qint64 coverage;
};


/**
 * Simple holder of coverage information, provides methods to select well-covered regions.
 */
class CoveredRegionsManager {
public:
    CoveredRegionsManager() {}
    CoveredRegionsManager(const U2Region & visibleRegion_, const U2AssemblyCoverageStat & coverageInfo_);

    inline bool isEmpty() const {return allRegions.empty();}

    inline int getSize() const {
        return allRegions.size();
    }

    /**
     * Returns list of regions with coverage >= coverageLevel.
     */
    QList<CoveredRegion> getCoveredRegions(qint64 coverageLevel) const;

    /**
     * Returns topSize most covered regions with topSize >= coverageLevel.
     */
    QList<CoveredRegion> getTopCoveredRegions(int topSize, qint64 coverageLevel=0) const;

    static const int DESIRED_REGION_LENGTH;

private:
    U2Region visibleRegion;
    QList<CoveredRegion> allRegions;
};


}

#endif
