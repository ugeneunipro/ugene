/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include "RegionSelectorWithExcludedRegion.h"

#include "ui_RegionSelectorWithExcludedRegion.h"

namespace U2 {

RegionSelectorWithExcludedRegion::RegionSelectorWithExcludedRegion(QWidget* parent,
                                                                   qint64 maxLen,
                                                                   DNASequenceSelection* selection,
                                                                   bool isCircularAvailable)
    : QWidget(parent),
      ui(new Ui_RegionSelectorWithExcludedRegion) {
    ui->setupUi(this);

    RegionSelectorGui includeGui(ui->startLineEdit, ui->endLineEdit, ui->presetsComboBox);
    RegionSelectorGui excludeGui(ui->excludeStartLineEdit, ui->excludeEndLinEdit);

    RegionSelectorSettings settings(maxLen, isCircularAvailable, selection);

    includeController = new RegionSelectorController(includeGui, settings, this);
    excludeController = new RegionSelectorController(excludeGui, settings, this);

    connectSlots();

    setObjectName("region_selector_with_excluded");
}

RegionSelectorWithExcludedRegion::~RegionSelectorWithExcludedRegion() {
    delete ui;
}

bool RegionSelectorWithExcludedRegion::isWholeSequenceSelected() const {
    return includeController->getPresetName() == RegionPreset::WHOLE_SEQUENCE();
}

U2Region RegionSelectorWithExcludedRegion::getIncludeRegion(bool* ok) const {
    return includeController->getRegion(ok);
}

U2Region RegionSelectorWithExcludedRegion::getExcludeRegion(bool* ok) const {
    if (ui->excludeCheckBox->isChecked()) {
        return excludeController->getRegion(ok);
    } else {
        if (ok != nullptr) {
            *ok = true;
        }
        return U2Region();
    }
}

void RegionSelectorWithExcludedRegion::setIncludeRegion(const U2Region& r) {
    includeController->setRegion(r);
}

void RegionSelectorWithExcludedRegion::setExcludeRegion(const U2Region& r) {
    excludeController->setRegion(r);
}

void RegionSelectorWithExcludedRegion::setExcludedCheckboxChecked(bool checked) {
    ui->excludeCheckBox->setChecked(checked);
}

bool RegionSelectorWithExcludedRegion::hasError() const {
    return !getErrorMessage().isEmpty();
}

QString RegionSelectorWithExcludedRegion::getErrorMessage() const {
    if (includeController->hasError()) {
        return includeController->getErrorMessage();
    }

    if (ui->excludeCheckBox->isChecked()) {
        if (excludeController->hasError()) {
            return excludeController->getErrorMessage();
        } else {
            if (excludeController->getRegion().contains(includeController->getRegion())) {
                return tr("'Exclude' region contains 'Search In' region. Search region is empty.");
            }
        }
    }

    return QString();
}

void RegionSelectorWithExcludedRegion::connectSlots() {
    connect(ui->excludeCheckBox, SIGNAL(toggled(bool)), ui->excludeWidget, SLOT(setEnabled(bool)));
}

}  // namespace U2
