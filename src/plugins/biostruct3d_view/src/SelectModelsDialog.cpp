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

#include "SelectModelsDialog.h"
#include <cassert>

#include <QMessageBox>
#include <QPushButton>

#include <U2Gui/HelpButton.h>

#include "SettingsDialog.h"

namespace U2 {

SelectModelsDialog::SelectModelsDialog(const QList<int>& _modelIds, const QList<int>& selectedModelIds, QWidget* parent /* = 0*/)
    : QDialog(parent), Ui_SelectModelsDialog(), modelIds(_modelIds) {
    setupUi(this);
    new HelpButton(this, buttonBox, "65929544");
    buttonBox_1->button(QDialogButtonBox::Cancel)->setText(::U2::SelectModelsDialog::tr("All"));
    buttonBox_1->button(QDialogButtonBox::No)->setText(::U2::SelectModelsDialog::tr("Invert"));
    buttonBox->button(QDialogButtonBox::Ok)->setText(::U2::SelectModelsDialog::tr("OK"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(::U2::SelectModelsDialog::tr("Cancel"));

    for (int modelId : qAsConst(modelIds)) {
        auto it = new QListWidgetItem(QString::number(modelId));
        it->setCheckState(selectedModelIds.contains(modelId) ? Qt::Checked : Qt::Unchecked);
        modelsList->addItem(it);
    }

    connect(modelsList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(sl_onItemDoubleClicked(QListWidgetItem*)));

    QPushButton* allButton = buttonBox_1->button(QDialogButtonBox::Cancel);
    QPushButton* invertButton = buttonBox_1->button(QDialogButtonBox::No);

    connect(allButton, SIGNAL(clicked()), this, SLOT(sl_onSlectAll()));
    connect(invertButton, SIGNAL(clicked()), this, SLOT(sl_onInvertSelection()));
}

/** Toggle item by double click */
void SelectModelsDialog::sl_onItemDoubleClicked(QListWidgetItem* item) {
    item->setCheckState((item->checkState() == Qt::Unchecked) ? Qt::Checked : Qt::Unchecked);
}

void SelectModelsDialog::sl_onSlectAll() {
    for (int i = 0; i < modelsList->count(); ++i) {
        modelsList->item(i)->setCheckState(Qt::Checked);
    }
}

void SelectModelsDialog::sl_onInvertSelection() {
    for (int i = 0; i < modelsList->count(); ++i) {
        QListWidgetItem* item = modelsList->item(i);
        item->setCheckState((item->checkState() == Qt::Unchecked) ? Qt::Checked : Qt::Unchecked);
    }
}

void SelectModelsDialog::accept() {
    for (int i = 0; i < modelsList->count(); ++i) {
        QListWidgetItem* item = modelsList->item(i);
        if (item->checkState() == Qt::Checked) {
            selectedModelIds << modelIds[i];
        }
    }

    if (selectedModelIds.isEmpty()) {
        QMessageBox::warning(this, tr("Error"), tr("At least one model should be selected."));
        return;
    }

    QDialog::accept();
}

const QList<int>& SelectModelsDialog::getSelectedModelsIds() const {
    return selectedModelIds;
}

SelectModelsDialog::~SelectModelsDialog() {
}

}  // namespace U2
