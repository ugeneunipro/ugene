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

#include "TreeSettingsDialog.h"
#include <U2Core/global.h>
#include <U2Core/U2SafePoints.h>
#include <U2Gui/HelpButton.h>
#include <QPushButton>


namespace U2 {

TreeSettingsDialog::TreeSettingsDialog(QWidget *parent, const OptionsMap &settings, bool isRectLayout)
: BaseSettingsDialog(parent) {
    setupUi(this);
    new HelpButton(this, buttonBox, "24748919");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("OK"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    heightSlider->setValue(settings[HEIGHT_COEF].toUInt());
    widthlSlider->setValue(settings[WIDTH_COEF].toUInt());

    heightSlider->setEnabled(isRectLayout);

    scaleSpinBox->setValue(settings[SCALEBAR_RANGE].toDouble());

    treeViewCombo->addItem(treeDefaultText());
    treeViewCombo->addItem(treePhylogramText());
    treeViewCombo->addItem(treeCladogramText());

    switch(settings[BRANCHES_TRANSFORMATION_TYPE].toUInt()) {
    case DEFAULT:
        treeViewCombo->setCurrentIndex(treeViewCombo->findText(treeDefaultText()));
        break;
    case PHYLOGRAM:
        treeViewCombo->setCurrentIndex(treeViewCombo->findText(treePhylogramText()));
        break;
    case CLADOGRAM:
        treeViewCombo->setCurrentIndex(treeViewCombo->findText(treeCladogramText()));
        break;
    default:
        assert(false && "Unexpected tree type value.");
        break;
    }

    connect(treeViewCombo, SIGNAL(currentIndexChanged(int)), SLOT(sl_treeTypeChanged(int)));
}

void TreeSettingsDialog::sl_treeTypeChanged(int value) {
    scaleLabel->setEnabled(value == PHYLOGRAM);
    scaleSpinBox->setEnabled(value == PHYLOGRAM);
}

void TreeSettingsDialog::accept() {
    changedSettings[HEIGHT_COEF] = heightSlider->value();
    changedSettings[WIDTH_COEF] = widthlSlider->value();

    if (treeViewCombo->currentText() == treeDefaultText()) {
        changedSettings[BRANCHES_TRANSFORMATION_TYPE] = DEFAULT;
    } else if (treeViewCombo->currentText() == treePhylogramText()) {
        changedSettings[BRANCHES_TRANSFORMATION_TYPE] = PHYLOGRAM;
    } else if (treeViewCombo->currentText() == treeCladogramText()) {
        changedSettings[BRANCHES_TRANSFORMATION_TYPE] = CLADOGRAM;
    } else {
        FAIL("Unexpected tree type value",);
    }
    if(scaleSpinBox->isEnabled()) {
        changedSettings[SCALEBAR_RANGE] = scaleSpinBox->value();
    }

    QDialog::accept();
}

QString TreeSettingsDialog::treeDefaultText() {
    return tr("Default");
}
QString TreeSettingsDialog::treePhylogramText() {
    return tr("Phylogram");
}
QString TreeSettingsDialog::treeCladogramText() {
    return tr("Cladogram");
}

} //namespace
