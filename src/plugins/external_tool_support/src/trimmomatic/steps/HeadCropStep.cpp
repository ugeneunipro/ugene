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

#include <U2Core/U2SafePoints.h>

#include "HeadCropStep.h"
#include "trimmomatic/util/LengthSettingsWidget.h"

namespace U2 {
namespace LocalWorkflow {

const QString HeadCropStepFactory::ID = "HEADCROP";

HeadCropStep::HeadCropStep()
    : TrimmomaticStep(HeadCropStepFactory::ID)
{
    name = "HEADCROP";
    description = tr("<html><head></head><body>"
                     "<h4>HEADCROP</h4>"
                     "<p>This step removes the specified number of bases, regardless of quality, "
                     "from the beginning of the read.</p>"
                     "<p>Input the following values:</p>"
                     "<ul>"
                     "<li><b>Length</b>: the number of bases to remove from the start of the read.</li>"
                     "</ul>"
                     "</body></html>");
}

TrimmomaticStepSettingsWidget *HeadCropStep::createWidget() const {
    return new LengthSettingsWidget(tr("The number of bases to remove from the start of the read."));
}

QString HeadCropStep::serializeState(const QVariantMap &widgetState) const {
    return LengthSettingsWidget::serializeState(widgetState);
}

QVariantMap HeadCropStep::parseState(const QString &command) const {
    return LengthSettingsWidget::parseState(command, id);
}

HeadCropStepFactory::HeadCropStepFactory()
    : TrimmomaticStepFactory(ID)
{

}

HeadCropStep *HeadCropStepFactory::createStep() const {
    return new HeadCropStep();
}

}   // namespace LocalWorkflow
}   // namespace U2
