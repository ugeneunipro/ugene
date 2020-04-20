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

#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/U2SafePoints.h>

#include <U2Designer/EditMarkerGroupDialog.h>
#include <U2Designer/MarkerEditor.h>

#include <U2Lang/Marker.h>
#include <U2Lang/MarkerUtils.h>

#include "MarkerEditorWidget.h"

namespace U2 {

MarkerEditorWidget::MarkerEditorWidget(QAbstractTableModel *markerModel, QWidget *parent)
: QWidget(parent), markerModel(markerModel)
{
    setupUi(this);
    {
        markerTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
        markerTable->horizontalHeader()->setSectionsClickable(false);
        markerTable->horizontalHeader()->setStretchLastSection(true);
        markerTable->verticalHeader()->hide();
        markerTable->verticalHeader()->setDefaultSectionSize(QFontMetrics(QFont()).height() + 6);
    }

    markerTable->setModel(markerModel);

    editButton->setEnabled(false);
    removeButton->setEnabled(false);

    connect(addButton, SIGNAL(clicked()), SLOT(sl_onAddButtonClicked()));
    connect(editButton, SIGNAL(clicked()), SLOT(sl_onEditButtonClicked()));
    connect(removeButton, SIGNAL(clicked()), SLOT(sl_onRemoveButtonClicked()));
    connect(markerTable, SIGNAL(entered(const QModelIndex &)), SLOT(sl_onItemEntered(const QModelIndex &)));
    connect(markerTable, SIGNAL(pressed(const QModelIndex &)), SLOT(sl_onItemSelected(const QModelIndex &)));

}

void MarkerEditorWidget::sl_onAddButtonClicked() {
    Workflow::MarkerGroupListCfgModel *model = dynamic_cast<Workflow::MarkerGroupListCfgModel*>(markerTable->model());
    QObjectScopedPointer<EditMarkerGroupDialog> dlg = new EditMarkerGroupDialog(true, NULL, model, this);
    const int dialogResult = dlg->exec();
    CHECK(!dlg.isNull(), );

    if (QDialog::Accepted == dialogResult) {
        Marker *newMarker = dlg->getMarker();
        model->addMarker(newMarker);
    }
}

void MarkerEditorWidget::sl_onEditButtonClicked() {
    QItemSelectionModel *m = markerTable->selectionModel();
    QModelIndexList selected = m->selectedRows();
    if (1 != selected.size()) {
        return;
    }

    Workflow::MarkerGroupListCfgModel *model = dynamic_cast<Workflow::MarkerGroupListCfgModel*>(markerTable->model());
    QObjectScopedPointer<EditMarkerGroupDialog> dlg = new EditMarkerGroupDialog(false, model->getMarker(selected.first().row()), model, this);
    const int dialogResult = dlg->exec();
    CHECK(!dlg.isNull(), );

    if (QDialog::Accepted == dialogResult) {
        Marker *newMarker = dlg->getMarker();
        model->replaceMarker(selected.first().row(), newMarker);
    }
}

void MarkerEditorWidget::sl_onRemoveButtonClicked() {
    QItemSelectionModel *m = markerTable->selectionModel();
    QModelIndexList selected = m->selectedRows();
    if (1 != selected.size()) {
        return;
    }

    markerModel->removeRows(selected.first().row(), 1, selected.first());

    SAFE_POINT(markerTable->model(), "cant retrieve table model count", );
    if(markerTable->model()->rowCount() == 0){
        editButton->setEnabled(false);
        removeButton->setEnabled(false);
    }
}

void MarkerEditorWidget::sl_onItemEntered(const QModelIndex &idx) {
    Qt::MouseButtons bs = QApplication::mouseButtons();
    if (bs.testFlag(Qt::LeftButton)) {
        sl_onItemSelected(idx);
    }
}

void MarkerEditorWidget::sl_onItemSelected(const QModelIndex &) {
    editButton->setEnabled(true);
    removeButton->setEnabled(true);
}

bool MarkerEditorWidget::checkEditMarkerGroupResult(const QString &oldName, Marker *newMarker, QString &message) {
    Workflow::MarkerGroupListCfgModel *model = dynamic_cast<Workflow::MarkerGroupListCfgModel*>(markerTable->model());
    QList<Marker*> &markers = model->getMarkers();


    if (oldName != newMarker->getName()) {
        foreach (Marker *m, markers) {
            if (m->getName() == newMarker->getName()) {
                message.append(tr("Duplicate marker's name: %1").arg(newMarker->getName()));
                return false;
            }
        }
    }

    return true;
}

bool MarkerEditorWidget::checkAddMarkerGroupResult(Marker *newMarker, QString &message) {
    Workflow::MarkerGroupListCfgModel *model = dynamic_cast<Workflow::MarkerGroupListCfgModel*>(markerTable->model());
    QList<Marker*> &markers = model->getMarkers();

    foreach (Marker *m, markers) {
        if (m->getName() == newMarker->getName()) {
            message.append(tr("Duplicate marker's name: %1").arg(newMarker->getName()));
            return false;
        }
    }

    QRegExp rx("\\s");
    if (rx.indexIn(newMarker->getName()) >= 0) {
        message.append(tr("Marker's name contains spaces: %1").arg(newMarker->getName()));
        return false;
    }

    return true;
}

} // U2
