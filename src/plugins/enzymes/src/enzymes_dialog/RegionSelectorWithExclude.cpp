/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2024 UniPro <ugene@unipro.ru>
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

#include "RegionSelectorWithExclude.h"

#include <QGroupBox>
#include <QVBoxLayout>

#include <U2Gui/RegionSelector.h>

#include <U2View/ADVSequenceObjectContext.h>

#include "FindEnzymesTask.h"

namespace U2 {

RegionSelectorWithExclude::RegionSelectorWithExclude(QWidget* parent, const QPointer<ADVSequenceObjectContext>& _advSequenceContext)
    : QWidget(parent), advSequenceContext(_advSequenceContext) {

    U2Location searchLocation = FindEnzymesAutoAnnotationUpdater::getLastSearchLocationForObject(advSequenceContext->getSequenceObject());
    fixPreviousLocation(searchLocation);
    U2Region customRegion = searchLocation.data()->regions.isEmpty() ? U2Region() : searchLocation.data()->regions.first();

    QList<RegionPreset> searchPresets = {RegionPreset(RegionPreset::RegionPreset::getLocationModeDisplayName(), searchLocation)};

    auto widgetLayout = new QVBoxLayout(this);
    auto groupBox = new QGroupBox(tr("Select sequence region"), this);
    widgetLayout->addWidget(groupBox);
    auto groupBoxLayout = new QGridLayout(this);
    groupBox->setLayout(groupBoxLayout);
    setLayout(widgetLayout);

    const quint64 sequenceLength = advSequenceContext->getSequenceLength();
    regionSelector = new RegionSelector(this, sequenceLength, false, advSequenceContext->getSequenceSelection(), advSequenceContext->getSequenceObject()->isCircular(), searchPresets);
    groupBoxLayout->addWidget(regionSelector, 0, 1);
    if (customRegion != U2Region()) {
        regionSelector->setCustomRegion(customRegion);
    }

    const QVector<U2Region> selectedRegions = advSequenceContext->getSequenceSelection()->getSelectedRegions();
    if (!selectedRegions.isEmpty()) {
        regionSelector->setCurrentPreset(RegionPreset::RegionPreset::getSelectedRegionDisplayName());
    }

    U2Location excludeLocation = FindEnzymesAutoAnnotationUpdater::getLastExcludeLocationForObject(advSequenceContext->getSequenceObject());
    fixPreviousLocation(excludeLocation);
    U2Region excludeRegion = excludeLocation.data()->regions.isEmpty() ? U2Region() : excludeLocation.data()->regions.first();

    QList<RegionPreset> excludePresets = {RegionPreset(RegionPreset::RegionPreset::getLocationModeDisplayName(), excludeLocation)};
    excludeRegionSelector = new RegionSelector(this, sequenceLength, false, advSequenceContext->getSequenceSelection(), advSequenceContext->getSequenceObject()->isCircular(), excludePresets);
    groupBoxLayout->addWidget(excludeRegionSelector, 1, 1);
    excludeRegionSelector->removePreset(RegionPreset::RegionPreset::getWholeSequenceModeDisplayName());
    excludeRegionSelector->setCurrentPreset(RegionPreset::RegionPreset::getCustomRegionModeDisplayName());
    excludeRegionSelector->setObjectName("exclude_range_selector");

    excludeCheckbox = new QCheckBox(this);
    excludeCheckbox->setObjectName("excludeCheckBox");
    excludeCheckbox->setText(tr("Uncut area:"));
    excludeCheckbox->setToolTip(tr("A region that will not be cut by any of the found enzymes. If an enzyme is present in this region, it will be excluded from the flank results."));
    groupBoxLayout->addWidget(excludeCheckbox, 1, 0);

    bool excludeLocationIsEmpty = excludeLocation.data()->regions.size() == 1 && excludeLocation.data()->regions.first() == U2Region();
    bool excludeOn = FindEnzymesAutoAnnotationUpdater::getExcludeModeEnabledForObject(advSequenceContext->getSequenceObject()) && !excludeLocationIsEmpty;
    excludeCheckbox->setChecked(excludeOn);
    excludeRegionSelector->setEnabled(excludeOn);
    if (excludeOn && excludeRegion != U2Region()) {
        excludeRegionSelector->setCustomRegion(excludeRegion);
    }


    // searchRegionLayout->addWidget(regionSelector, 0, 1);
    // searchRegionLayout->addWidget(excludeCheckbox, 1, 0);
    // searchRegionLayout->addWidget(excludeRegionSelector, 1, 1);

    connect(excludeCheckbox, &QCheckBox::stateChanged, this, [this]() {
        excludeRegionSelector->setEnabled(excludeCheckbox->isChecked());
    });
}

U2Location RegionSelectorWithExclude::getRegionSelectorLocation(bool* ok) const {
    return regionSelector->getLocation(ok);
}

U2Location RegionSelectorWithExclude::getExcludeRegionSelectorLocation(bool* ok) const {
    return excludeRegionSelector->getLocation(ok);
}

bool RegionSelectorWithExclude::isExcludeCheckboxChecked() const {
    return excludeCheckbox->isChecked();
}

void RegionSelectorWithExclude::saveSettings() {
    U2SequenceObject* sequenceObject = advSequenceContext->getSequenceObject();
    // Empty search region is processed as 'Whole sequence' by auto-annotation task.
    bool ok = true;
    U2Location loc = regionSelector->isWholeSequenceSelected() ? U2Location() : regionSelector->getLocation(&ok);
    CHECK(ok, );
    FindEnzymesAutoAnnotationUpdater::setLastSearchLocationForObject(sequenceObject, loc);

    if (excludeCheckbox->isChecked()) {
        loc = excludeRegionSelector->getLocation(&ok);
        CHECK(ok, );
        FindEnzymesAutoAnnotationUpdater::setLastExcludeLocationForObject(sequenceObject, loc);
        FindEnzymesAutoAnnotationUpdater::setExcludeModeEnabledForObject(sequenceObject, true);
    } else {
        FindEnzymesAutoAnnotationUpdater::setExcludeModeEnabledForObject(sequenceObject, false);
    }
}

void RegionSelectorWithExclude::fixPreviousLocation(U2Location& prevLocation) const {
    bool fitsWell = true;
    for (const U2Region& region : qAsConst(prevLocation.data()->regions)) {
        if (!(region.length > 0 && U2Region(0, advSequenceContext->getSequenceLength()).contains(region))) {
            fitsWell = false;
            break;
        }
    }
    if (!fitsWell) {
        prevLocation.data()->regions.clear();
        prevLocation.data()->regions << U2Region();
    }
}

}  // namespace U2
