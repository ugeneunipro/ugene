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

#include <U2Core/AppContext.h>
#include <U2Core/Settings.h>

#include "PhyTreeDisplayOptionsWidget.h"
#include "ui_PhyTreeDisplayOptionsWidget.h"

namespace U2 {

const QString PhyTreeDisplayOptionsWidget::displayWithMsaEditor = CreatePhyTreeWidget::settingsPath() + "/display_with_msa_editor";
const QString PhyTreeDisplayOptionsWidget::synchronizeWithMsa = CreatePhyTreeWidget::settingsPath() + "/synchronize_with_msa";

PhyTreeDisplayOptionsWidget::PhyTreeDisplayOptionsWidget(QWidget *parent) :
    CreatePhyTreeWidget(parent),
    ui(new Ui_PhyTreeDisplayOptionsWidget)
{
    ui->setupUi(this);

    const bool displayWithMsa = AppContext::getSettings()->getValue(displayWithMsaEditor, true).toBool();
    if (displayWithMsa) {
        ui->displayWithAlignmentEditor->setChecked(true);
    } else {
        ui->createNewView->setChecked(true);
    }
    ui->syncCheckBox->setChecked(AppContext::getSettings()->getValue(synchronizeWithMsa, true).toBool());
}

PhyTreeDisplayOptionsWidget::~PhyTreeDisplayOptionsWidget() {
    delete ui;
}

void PhyTreeDisplayOptionsWidget::fillSettings(CreatePhyTreeSettings &settings) {
    settings.displayWithAlignmentEditor = ui->displayWithAlignmentEditor->isChecked();
    settings.syncAlignmentWithTree = ui->syncCheckBox->isChecked();
}

void PhyTreeDisplayOptionsWidget::storeSettings() {
    AppContext::getSettings()->setValue(displayWithMsaEditor, ui->displayWithAlignmentEditor->isChecked());
    AppContext::getSettings()->setValue(synchronizeWithMsa, ui->syncCheckBox->isChecked());
}

void PhyTreeDisplayOptionsWidget::restoreDefault() {
    AppContext::getSettings()->remove(displayWithMsaEditor);
    AppContext::getSettings()->remove(synchronizeWithMsa);

    ui->displayWithAlignmentEditor->setChecked(true);
    ui->syncCheckBox->setChecked(true);
}

}   // namespace U2
