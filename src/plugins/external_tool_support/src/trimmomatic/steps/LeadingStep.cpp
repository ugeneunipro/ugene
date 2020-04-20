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

#include <U2Core/U2SafePoints.h>

#include "LeadingStep.h"
#include "trimmomatic/util/QualitySettingsWidget.h"

namespace U2 {
namespace LocalWorkflow {

const QString LeadingStepFactory::ID = "LEADING";

LeadingStep::LeadingStep()
    : TrimmomaticStep(LeadingStepFactory::ID)
{
    name = "LEADING";
    description = tr("<html><head></head><body>"
                     "<h4>LEADING</h4>"
                     "<p>This step removes low quality bases from the beginning. "
                     "As long as a base has a value below this threshold the base is removed "
                     "and the next base will be investigated.</p>"
                     "<p>Input the following values:</p>"
                     "<ul>"
                     "<li><b>Quality threshold</b>: the minimum quality required to keep a base.</li>"
                     "</ul>"
                     "</body></html>");
}

TrimmomaticStepSettingsWidget *LeadingStep::createWidget() const {
    return new QualitySettingsWidget(tr("The minimum quality required to keep a base."));
}

QString LeadingStep::serializeState(const QVariantMap &widgetState) const {
    return QualitySettingsWidget::serializeState(widgetState);
}

QVariantMap LeadingStep::parseState(const QString &command) const {
    return QualitySettingsWidget::parseState(command, id);
}

LeadingStepFactory::LeadingStepFactory()
    : TrimmomaticStepFactory(ID)
{

}

LeadingStep *LeadingStepFactory::createStep() const {
    return new LeadingStep();
}

}   // namespace LocalWorkflow
}   // namespace U2
