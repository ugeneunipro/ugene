#include "PairwiseAlignmentHirschbergTaskFactory.h"

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UserApplicationsSettings.h>

#include "PairwiseAlignmentHirschbergTask.h"

namespace U2 {

PairwiseAlignmentHirschbergTaskFactory::PairwiseAlignmentHirschbergTaskFactory() {
}

PairwiseAlignmentHirschbergTaskFactory::~PairwiseAlignmentHirschbergTaskFactory() {
}

AbstractAlignmentTask *PairwiseAlignmentHirschbergTaskFactory::getTaskInstance(AbstractAlignmentTaskSettings *settings) const {
    PairwiseAlignmentTaskSettings *pairwiseSettings = dynamic_cast<PairwiseAlignmentTaskSettings *>(settings);
    SAFE_POINT_EXT(pairwiseSettings != nullptr, delete settings, nullptr);
    SAFE_POINT_EXT(!pairwiseSettings->inNewWindow || !pairwiseSettings->resultFileName.isEmpty(), delete settings, nullptr);
    auto hirschbergTaskSettings = new PairwiseAlignmentHirschbergTaskSettings(*pairwiseSettings);
    delete settings;
    return new PairwiseAlignmentHirschbergTask(hirschbergTaskSettings);
}

}  // namespace U2
