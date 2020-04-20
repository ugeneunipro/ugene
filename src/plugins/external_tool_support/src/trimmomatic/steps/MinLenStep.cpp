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

#include "MinLenStep.h"
#include "trimmomatic/util/LengthSettingsWidget.h"

namespace U2 {
namespace LocalWorkflow {

const QString MinLenStepFactory::ID = "MINLEN";

MinLenStep::MinLenStep()
    : TrimmomaticStep(MinLenStepFactory::ID)
{
    name = "MINLEN";
    description = tr("<html><head></head><body>"
                     "<h4>MINLEN</h4>"
                     "<p>This step removes reads that fall below the specified minimal length. "
                     "If required, it should normally be after all other processing steps. "
                     "Reads removed by this step will be counted and included in "
                     "the \"dropped reads\" count.</p>"
                     "<p>Input the following values:</p>"
                     "<ul>"
                     "<li><b>Length</b>: the minimum length of reads to be kept.</li>"
                     "</ul>"
                     "</body></html>");
}

TrimmomaticStepSettingsWidget *MinLenStep::createWidget() const {
    return new LengthSettingsWidget(tr("The minimum length of reads to be kept."));
}

QString MinLenStep::serializeState(const QVariantMap &widgetState) const {
    return LengthSettingsWidget::serializeState(widgetState);
}

QVariantMap MinLenStep::parseState(const QString &command) const {
    return LengthSettingsWidget::parseState(command, id);
}

MinLenStepFactory::MinLenStepFactory()
    : TrimmomaticStepFactory(ID)
{

}

MinLenStep *MinLenStepFactory::createStep() const {
    return new MinLenStep();
}

}   // namespace LocalWorkflow
}   // namespace U2
