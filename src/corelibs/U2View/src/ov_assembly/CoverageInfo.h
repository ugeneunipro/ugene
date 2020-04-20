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

#ifndef __COVERAGE_INFO_H__
#define __COVERAGE_INFO_H__

#include "AssemblyModel.h"

#include <U2Core/BackgroundTaskRunner.h>
#include <U2Core/U2Region.h>

#include <QVector>
#include <QSharedPointer>


namespace U2 {

struct CoverageInfo {
    CoverageInfo() : averageCoverage(0.), maxCoverage(0), minCoverage(0) {}
    inline bool isEmpty() {
        return coverageInfo.empty();
    }
    // calculates min, max and average
    void updateStats();

    U2Region region;
    U2AssemblyCoverageStat coverageInfo;
    double averageCoverage;
    qint32 maxCoverage;
    qint32 minCoverage;
};

struct CalcCoverageInfoTaskSettings {
    QSharedPointer<AssemblyModel> model;
    U2Region visibleRange;
    int regions;
};

class CalcCoverageInfoTask : public BackgroundTask<CoverageInfo> {
    Q_OBJECT
public:
    CalcCoverageInfoTask(const CalcCoverageInfoTaskSettings & settings);
    virtual void run();
private:
    CalcCoverageInfoTaskSettings settings;
};

}

#endif
