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

#include "ToPhred64Step.h"
#include "trimmomatic/util/NoSettingsWidget.h"

namespace U2 {
namespace LocalWorkflow {

const QString ToPhred64StepFactory::ID = "TOPHRED64";

ToPhred64Step::ToPhred64Step()
    : TrimmomaticStep(ToPhred64StepFactory::ID)
{
    name = "TOPHRED64";
    description = tr("<html><head></head><body>"
                     "<h4>TOPHRED64</h4>"
                     "<p>This step (re)encodes the quality part of the FASTQ file to base 64.</p>"
                     "</body></html>");
}

TrimmomaticStepSettingsWidget *ToPhred64Step::createWidget() const {
    return new NoSettingsWidget();
}

QString ToPhred64Step::serializeState(const QVariantMap &widgetState) const {
    return NoSettingsWidget::serializeState(widgetState);
}

QVariantMap ToPhred64Step::parseState(const QString &command) const {
    return NoSettingsWidget::parseState(command);
}

ToPhred64StepFactory::ToPhred64StepFactory()
    : TrimmomaticStepFactory(ID)
{

}

ToPhred64Step *ToPhred64StepFactory::createStep() const {
    return new ToPhred64Step();
}

}   // namespace LocalWorkflow
}   // namespace U2
