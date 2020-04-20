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

#include "ToPhred33Step.h"
#include "trimmomatic/util/NoSettingsWidget.h"

namespace U2 {
namespace LocalWorkflow {

const QString ToPhred33StepFactory::ID = "TOPHRED33";

ToPhred33Step::ToPhred33Step()
    : TrimmomaticStep(ToPhred33StepFactory::ID)
{
    name = "TOPHRED33";
    description = tr("<html><head></head><body>"
                     "<h4>TOPHRED33</h4>"
                     "<p>This step (re)encodes the quality part of the FASTQ file to base 33.</p>"
                     "</body></html>");
}

TrimmomaticStepSettingsWidget *ToPhred33Step::createWidget() const {
    return new NoSettingsWidget();
}

QString ToPhred33Step::serializeState(const QVariantMap &widgetState) const {
    return NoSettingsWidget::serializeState(widgetState);
}

QVariantMap ToPhred33Step::parseState(const QString &command) const {
    return NoSettingsWidget::parseState(command);
}

ToPhred33StepFactory::ToPhred33StepFactory()
    : TrimmomaticStepFactory(ID)
{

}

ToPhred33Step *ToPhred33StepFactory::createStep() const {
    return new ToPhred33Step();
}

}   // namespace LocalWorkflow
}   // namespace U2
