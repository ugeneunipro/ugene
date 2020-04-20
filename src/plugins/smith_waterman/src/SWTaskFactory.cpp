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

#include "SWTaskFactory.h"
#include "SWAlgorithmTask.h"

#include <U2Core/GUrl.h>
#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/UserApplicationsSettings.h>
#include <U2Core/U2SafePoints.h>

#include <U2Algorithm/SmithWatermanReportCallback.h>
#include <U2Algorithm/SWResultFilterRegistry.h>

namespace U2 {

SWTaskFactory::SWTaskFactory(SW_AlgType _algType) {
    algType = _algType;
}

SWTaskFactory::~SWTaskFactory() {
}

Task* SWTaskFactory::getTaskInstance( const SmithWatermanSettings& config, const QString& taskName) const {
    return new SWAlgorithmTask(config, taskName, algType);
}

bool SWTaskFactory::isValidParameters(const SmithWatermanSettings& sWatermanConfig,  SequenceWalkerSubtask* t) const {
    Q_UNUSED(sWatermanConfig);
    Q_UNUSED(t);
    return true;                //not realized
}

PairwiseAlignmentSmithWatermanTaskFactory::PairwiseAlignmentSmithWatermanTaskFactory(SW_AlgType _algType) :
    AbstractAlignmentTaskFactory(), algType(_algType) {
}

PairwiseAlignmentSmithWatermanTaskFactory::~PairwiseAlignmentSmithWatermanTaskFactory() {
}

AbstractAlignmentTask* PairwiseAlignmentSmithWatermanTaskFactory::getTaskInstance(AbstractAlignmentTaskSettings* _settings) const {
    PairwiseAlignmentTaskSettings* pairwiseSettings = dynamic_cast<PairwiseAlignmentTaskSettings*>(_settings);
    SAFE_POINT(pairwiseSettings != NULL,
        "Pairwise alignment: incorrect settings", NULL);
    PairwiseAlignmentSmithWatermanTaskSettings* settings = new PairwiseAlignmentSmithWatermanTaskSettings(*pairwiseSettings);
    SAFE_POINT(false == settings->inNewWindow || false == settings->resultFileName.isEmpty(),
               "Pairwise alignment: incorrect settings, empty output file name", NULL);
    if (settings->inNewWindow == true) {
        settings->reportCallback = new SmithWatermanReportCallbackMAImpl(settings->resultFileName.dirPath() + "/",
                                                                         settings->resultFileName.baseFileName(),
                                                                         settings->firstSequenceRef,
                                                                         settings->secondSequenceRef,
                                                                         settings->msaRef);
    } else {
        if (settings->msaRef.isValid()) {
            settings->reportCallback = new SmithWatermanReportCallbackMAImpl(settings->firstSequenceRef,
                                                                             settings->secondSequenceRef,
                                                                             settings->msaRef);
        }
    }

    settings->resultListener = new SmithWatermanResultListener;
    SWResultFilterRegistry* resFilterReg = AppContext::getSWResultFilterRegistry();
    SAFE_POINT(NULL != resFilterReg, "SWResultFilterRegistry is NULL.", NULL);
    settings->resultFilter = resFilterReg->getFilter(PairwiseAlignmentSmithWatermanTaskSettings::PA_SW_DEFAULT_RESULT_FILTER);
    settings->percentOfScore = PairwiseAlignmentSmithWatermanTaskSettings::PA_SW_DEFAULT_PERCENT_OF_SCORE;
    if (settings->convertCustomSettings()) {
        return new PairwiseAlignmentSmithWatermanTask(settings, algType);
    }
    return NULL;
}

} // namespace
