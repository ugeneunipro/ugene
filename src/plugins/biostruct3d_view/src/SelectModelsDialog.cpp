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

#include "SelectModelsDialog.h"

#include <QMessageBox>

#include <U2Gui/HelpButton.h>

namespace U2 {

SelectModelsDialog::SelectModelsDialog(const QList<int>& _modelIds, const QList<int>& initiallySelectedModelIds, QWidget* parent)
    : QDialog(parent), Ui_SelectModelsDialog(), modelIds(_modelIds) {
    setupUi(this);
    new HelpButton(this, buttonBox, "65929544");

    for (int modelId : qAsConst(modelIds)) {
        auto it = new QListWidgetItem(QString::number(modelId));
        it->setCheckState(initiallySelectedModelIds.contains(modelId) ? Qt::Checked : Qt::Unchecked);
        modelsList->addItem(it);
    }

    connect(modelsList, &QListWidget::itemDoubleClicked, this, &SelectModelsDialog::sl_onItemDoubleClicked);
    connect(selectAllButton, &QPushButton::clicked, this, &SelectModelsDialog::sl_onSelectAll);
    connect(invertSelectionButton, &QPushButton::clicked, this, &SelectModelsDialog::sl_onInvertSelection);
}

/** Toggle item by double click */
void SelectModelsDialog::sl_onItemDoubleClicked(QListWidgetItem* item) {
    item->setCheckState(item->checkState() == Qt::Unchecked ? Qt::Checked : Qt::Unchecked);
}

void SelectModelsDialog::sl_onSelectAll() {
    for (int i = 0; i < modelsList->count(); ++i) {
        modelsList->item(i)->setCheckState(Qt::Checked);
    }
}

void SelectModelsDialog::sl_onInvertSelection() {
    for (int i = 0; i < modelsList->count(); ++i) {
        QListWidgetItem* item = modelsList->item(i);
        item->setCheckState(item->checkState() == Qt::Unchecked ? Qt::Checked : Qt::Unchecked);
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

}  // namespace U2
