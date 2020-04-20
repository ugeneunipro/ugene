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

#include "RegionSelectorWithExcludedRegion.h"

#include "ui_RegionSelectorWithExcludedRegion.h"


namespace U2 {

RegionSelectorWithExludedRegion::RegionSelectorWithExludedRegion(QWidget* parent,
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

RegionSelectorWithExludedRegion::~RegionSelectorWithExludedRegion() {
    delete ui;
}

U2Region RegionSelectorWithExludedRegion::getIncludeRegion(bool *ok) const {
    return includeController->getRegion(ok);
}

U2Region RegionSelectorWithExludedRegion::getExcludeRegion(bool *ok) const {
    if (ui->excludeCheckBox->isChecked()) {
        return excludeController->getRegion(ok);
    } else {
        if (ok != NULL) {
            *ok = true;
        }
        return U2Region();
    }
}

void RegionSelectorWithExludedRegion::setIncludeRegion(const U2Region &r) {
    includeController->setRegion(r);
}

void RegionSelectorWithExludedRegion::setExcludeRegion(const U2Region &r) {
    excludeController->setRegion(r);
}

void RegionSelectorWithExludedRegion::setExcludedCheckboxChecked(bool checked) {
    ui->excludeCheckBox->setChecked(checked);
}

bool RegionSelectorWithExludedRegion::hasError() const {
    return !getErrorMessage().isEmpty();
}

QString RegionSelectorWithExludedRegion::getErrorMessage() const {
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

void RegionSelectorWithExludedRegion::connectSlots() {
    connect(ui->excludeCheckBox, SIGNAL(toggled(bool)), ui->excludeWidget, SLOT(setEnabled(bool)));

}

} // namespace
