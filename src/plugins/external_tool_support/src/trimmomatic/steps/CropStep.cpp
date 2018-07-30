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

#include "CropStep.h"
#include "trimmomatic/util/LengthSettingsWidget.h"

namespace U2 {
namespace LocalWorkflow {

const QString CropStepFactory::ID = "CROP";

CropStep::CropStep()
    : TrimmomaticStep(CropStepFactory::ID)
{
    name = "CROP";
    description = tr("<html><head></head><body>"
                     "<h4>CROP</h4>"
                     "<p>This step removes bases regardless of quality from the end of the read, "
                     "so that the read has maximally the specified length after this step has been "
                     "performed. Steps performed after CROP might of course further shorten the read.</p>"
                     "<p>Input the following values:</p>"
                     "<ul>"
                     "<li><b>Length</b>: the number of bases to keep, from the start of the read.</li>"
                     "</ul>"
                     "</body></html>");
}

TrimmomaticStepSettingsWidget *CropStep::createWidget() const {
    return new LengthSettingsWidget(tr("The number of bases to keep, from the start of the read."));
}

QString CropStep::serializeState(const QVariantMap &widgetState) const {
    return LengthSettingsWidget::serializeState(widgetState);
}

QVariantMap CropStep::parseState(const QString &command) const {
    return LengthSettingsWidget::parseState(command, id);
}

CropStepFactory::CropStepFactory()
    : TrimmomaticStepFactory(ID)
{

}

CropStep *CropStepFactory::createStep() const {
    return new CropStep();
}

}   // namespace LocalWorkflow
}   // namespace U2
