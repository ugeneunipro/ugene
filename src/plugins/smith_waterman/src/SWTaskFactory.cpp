/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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

#include "SWTaskFactory.h"

#include <U2Algorithm/SWResultFilterRegistry.h>
#include <U2Algorithm/SmithWatermanReportCallback.h>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UserApplicationsSettings.h>

#include "SWAlgorithmTask.h"

namespace U2 {

SWTaskFactory::SWTaskFactory(SW_AlgType _algType) {
    algType = _algType;
}

SWTaskFactory::~SWTaskFactory() {
}

Task *SWTaskFactory::getTaskInstance(const SmithWatermanSettings &config, const QString &taskName) const {
    return new SWAlgorithmTask(config, taskName, algType);
}

PairwiseAlignmentSmithWatermanTaskFactory::PairwiseAlignmentSmithWatermanTaskFactory(SW_AlgType _algType)
    : AbstractAlignmentTaskFactory(), algType(_algType) {
}

PairwiseAlignmentSmithWatermanTaskFactory::~PairwiseAlignmentSmithWatermanTaskFactory() {
}

AbstractAlignmentTask *PairwiseAlignmentSmithWatermanTaskFactory::getTaskInstance(AbstractAlignmentTaskSettings *settings) const {
    PairwiseAlignmentTaskSettings *pairwiseSettings = dynamic_cast<PairwiseAlignmentTaskSettings *>(settings);
    SAFE_POINT_EXT(pairwiseSettings != nullptr, delete settings, nullptr);
    SAFE_POINT_EXT(!settings->inNewWindow || !settings->resultFileName.isEmpty(), delete settings, nullptr);
    auto smithWatermanTaskSettings = new PairwiseAlignmentSmithWatermanTaskSettings(*pairwiseSettings);
    delete settings;

    if (smithWatermanTaskSettings->inNewWindow) {
        smithWatermanTaskSettings->reportCallback = new SmithWatermanReportCallbackMAImpl(smithWatermanTaskSettings->resultFileName.dirPath() + "/",
                                                                                          smithWatermanTaskSettings->resultFileName.baseFileName(),
                                                                                          smithWatermanTaskSettings->firstSequenceRef,
                                                                                          smithWatermanTaskSettings->secondSequenceRef,
                                                                                          smithWatermanTaskSettings->msaRef);
    } else if (smithWatermanTaskSettings->msaRef.isValid()) {
        smithWatermanTaskSettings->reportCallback = new SmithWatermanReportCallbackMAImpl(smithWatermanTaskSettings->firstSequenceRef,
                                                                                          smithWatermanTaskSettings->secondSequenceRef,
                                                                                          smithWatermanTaskSettings->msaRef);
    }

    SWResultFilterRegistry *resFilterReg = AppContext::getSWResultFilterRegistry();
    SAFE_POINT_EXT(resFilterReg != nullptr, delete smithWatermanTaskSettings, nullptr);
    smithWatermanTaskSettings->resultFilter = resFilterReg->getFilter(PairwiseAlignmentSmithWatermanTaskSettings::PA_SW_DEFAULT_RESULT_FILTER);
    smithWatermanTaskSettings->percentOfScore = PairwiseAlignmentSmithWatermanTaskSettings::PA_SW_DEFAULT_PERCENT_OF_SCORE;
    smithWatermanTaskSettings->resultListener = new SmithWatermanResultListener();
    SAFE_POINT_EXT(smithWatermanTaskSettings->convertCustomSettings(), delete smithWatermanTaskSettings, nullptr)
    return new PairwiseAlignmentSmithWatermanTask(smithWatermanTaskSettings, algType);
}

}  // namespace U2
