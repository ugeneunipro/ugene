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

#include <QFileInfo>

#include <U2Core/AppContext.h>

#include "TrimmomaticStep.h"
#include "TrimmomaticSupport.h"
#include "java/JavaSupport.h"
#include "steps/AvgQualStep.h"
#include "steps/CropStep.h"
#include "steps/HeadCropStep.h"
#include "steps/IlluminaClipStep.h"
#include "steps/LeadingStep.h"
#include "steps/MaxInfoStep.h"
#include "steps/MinLenStep.h"
#include "steps/SlidingWindowStep.h"
#include "steps/ToPhred33Step.h"
#include "steps/ToPhred64Step.h"
#include "steps/TrailingStep.h"

namespace U2 {

const QString TrimmomaticSupport::ET_TRIMMOMATIC = "Trimmomatic";
const QString TrimmomaticSupport::ET_TRIMMOMATIC_ID = "USUPP_TRIMMOMATIC";

TrimmomaticSupport::TrimmomaticSupport(const QString& id, const QString &name, const QString &path)
    : ExternalTool(id, name, path)
{
    toolKitName = "Trimmomatic";
    description = tr("<i>Trimmomatic</i> is a flexible read trimming tool for Illumina NGS data.");

    executableFileName = "trimmomatic.jar";
    validationArguments << "-h";
    validMessage = "PE \\[-version\\] \\[-threads <threads>\\] \\[-phred33|-phred64\\] \\[-trimlog <trimLogFile>\\]";

    toolRunnerProgram = JavaSupport::ET_JAVA_ID;
    dependencies << JavaSupport::ET_JAVA_ID;

    initTrimmomaticSteps();
}

TrimmomaticSupport::~TrimmomaticSupport() {
    LocalWorkflow::TrimmomaticStepsRegistry::releaseInstance();
}

void TrimmomaticSupport::initTrimmomaticSteps() {
    LocalWorkflow::TrimmomaticStepsRegistry *registry = LocalWorkflow::TrimmomaticStepsRegistry::getInstance();
    registry->registerEntry(new LocalWorkflow::AvgQualStepFactory());
    registry->registerEntry(new LocalWorkflow::CropStepFactory());
    registry->registerEntry(new LocalWorkflow::HeadCropStepFactory());
    registry->registerEntry(new LocalWorkflow::IlluminaClipStepFactory());
    registry->registerEntry(new LocalWorkflow::LeadingStepFactory());
    registry->registerEntry(new LocalWorkflow::MaxInfoStepFactory());
    registry->registerEntry(new LocalWorkflow::MinLenStepFactory());
    registry->registerEntry(new LocalWorkflow::SlidingWindowStepFactory());
    registry->registerEntry(new LocalWorkflow::ToPhred33StepFactory());
    registry->registerEntry(new LocalWorkflow::ToPhred64StepFactory());
    registry->registerEntry(new LocalWorkflow::TrailingStepFactory());
}

} // namespace U2
