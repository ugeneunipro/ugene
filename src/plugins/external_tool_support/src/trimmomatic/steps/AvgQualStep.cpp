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

#include "AvgQualStep.h"
#include "trimmomatic/util/QualitySettingsWidget.h"

namespace U2 {
namespace LocalWorkflow {

const QString AvgQualStepFactory::ID = "AVGQUAL";

AvgQualStep::AvgQualStep()
    : TrimmomaticStep(AvgQualStepFactory::ID)
{
    name = "AVGQUAL";
    description = tr("<html><head></head><body>"
                     "<h4>AVGQUAL</h4>"
                     "<p>This step drops a read if the average quality is below the specified level.</p>"
                     "<p>Input the following values:</p>"
                     "<ul>"
                     "<li><b>Quality threshold</b>: the minimum average quality required to keep a read.</li>"
                     "</ul>"
                     "</body></html>");
}

TrimmomaticStepSettingsWidget *AvgQualStep::createWidget() const {
    return new QualitySettingsWidget(tr("The minimum average quality required to keep a read."));
}

QString AvgQualStep::serializeState(const QVariantMap &widgetState) const {
    return QualitySettingsWidget::serializeState(widgetState);
}

QVariantMap AvgQualStep::parseState(const QString &command) const {
    return QualitySettingsWidget::parseState(command, id);
}

AvgQualStepFactory::AvgQualStepFactory()
    : TrimmomaticStepFactory(ID)
{

}

AvgQualStep *AvgQualStepFactory::createStep() const {
    return new AvgQualStep();
}

}   // namespace LocalWorkflow
}   // namespace U2
