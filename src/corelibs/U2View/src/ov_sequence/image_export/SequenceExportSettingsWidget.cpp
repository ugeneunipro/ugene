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

#include "SequenceExportSettingsWidget.h"

#include <U2Core/DNASequenceObject.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/RegionSelector.h>


namespace U2 {

SequenceExportSettingsWidget::SequenceExportSettingsWidget(U2SequenceObject* seqObject,
                                                           QSharedPointer<CustomExportSettings> s,
                                                           DNASequenceSelection* selection)
    : seqObject(seqObject)
{
    setupUi(this);
    settings = qSharedPointerCast<SequenceExportSettings>(s);
    SAFE_POINT(settings != NULL, tr("Cannot cast CustomExportSettings to SequenceExportSettings"), );
    SAFE_POINT(seqObject != NULL, tr("Sequence Object is NULL"), );

    regionSelector = new RegionSelector(this, seqObject->getSequenceLength(), true, selection);
    regionLayout->addWidget(regionSelector);
    regionSelector->setVisible( !currentViewButton->isChecked() );

    connect(buttonGroup, SIGNAL(buttonClicked(int)), SLOT(sl_areaChanged()));
    connect(regionSelector, SIGNAL(si_regionChanged(U2Region)), SLOT(sl_regionChanged(U2Region)));
}

SequenceExportType SequenceExportSettingsWidget::getExportType() const {
    if (zoomButton->isChecked()) {
        return ExportZoomedView;
    }
    if (detailsButton->isChecked()) {
        return ExportDetailsView;
    }
    return ExportCurrentView;
}

void SequenceExportSettingsWidget::sl_areaChanged() {
    regionSelector->setVisible( !currentViewButton->isChecked() );
    settings->setType( getExportType() );
    settings->setRegion( regionSelector->getRegion() );
}

void SequenceExportSettingsWidget::sl_regionChanged(const U2Region& r) {
    settings->setRegion(r);
}

} // namespace
