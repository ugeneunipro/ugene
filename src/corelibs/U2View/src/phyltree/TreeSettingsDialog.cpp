/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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

#include "TreeSettingsDialog.h"

#include <QPushButton>

#include <U2Core/U2SafePoints.h>
#include <U2Core/global.h>

#include <U2Gui/HelpButton.h>

namespace U2 {

TreeSettingsDialog::TreeSettingsDialog(QWidget* parent, const OptionsMap& settings, bool isRectLayout)
    : BaseSettingsDialog(parent) {
    setupUi(this);
    new HelpButton(this, buttonBox, "65929724");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("OK"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    heightSlider->setValue(settings[HEIGHT_COEF].toUInt());
    widthlSlider->setValue(settings[WIDTH_COEF].toUInt());

    heightSlider->setEnabled(isRectLayout);

    scaleSpinBox->setValue(settings[SCALEBAR_RANGE].toDouble());

    treeViewCombo->addItem(getDefaultTreeModeText());
    treeViewCombo->addItem(getPhylogramTreeModeText());
    treeViewCombo->addItem(getCladogramTreeModeText());

    switch (settings[BRANCHES_TRANSFORMATION_TYPE].toUInt()) {
        case DEFAULT:
            treeViewCombo->setCurrentIndex(treeViewCombo->findText(getDefaultTreeModeText()));
            break;
        case PHYLOGRAM:
            treeViewCombo->setCurrentIndex(treeViewCombo->findText(getPhylogramTreeModeText()));
            break;
        case CLADOGRAM:
            treeViewCombo->setCurrentIndex(treeViewCombo->findText(getCladogramTreeModeText()));
            break;
        default:
            assert(false && "Unexpected tree type value.");
    }

    connect(treeViewCombo, SIGNAL(currentIndexChanged(int)), SLOT(sl_treeTypeChanged(int)));
}

void TreeSettingsDialog::sl_treeTypeChanged(int value) {
    scaleLabel->setEnabled(value == PHYLOGRAM);
    scaleSpinBox->setEnabled(value == PHYLOGRAM);
}

void TreeSettingsDialog::accept() {
    updatedSettings[HEIGHT_COEF] = heightSlider->value();
    updatedSettings[WIDTH_COEF] = widthlSlider->value();

    if (treeViewCombo->currentText() == getDefaultTreeModeText()) {
        updatedSettings[BRANCHES_TRANSFORMATION_TYPE] = DEFAULT;
    } else if (treeViewCombo->currentText() == getPhylogramTreeModeText()) {
        updatedSettings[BRANCHES_TRANSFORMATION_TYPE] = PHYLOGRAM;
    } else if (treeViewCombo->currentText() == getCladogramTreeModeText()) {
        updatedSettings[BRANCHES_TRANSFORMATION_TYPE] = CLADOGRAM;
    } else {
        FAIL("Unexpected tree type value", );
    }
    if (scaleSpinBox->isEnabled()) {
        updatedSettings[SCALEBAR_RANGE] = scaleSpinBox->value();
    }

    QDialog::accept();
}

QString TreeSettingsDialog::getDefaultTreeModeText() {
    return tr("Default");
}

QString TreeSettingsDialog::getPhylogramTreeModeText() {
    return tr("Phylogram");
}

QString TreeSettingsDialog::getCladogramTreeModeText() {
    return tr("Cladogram");
}

}  // namespace U2
