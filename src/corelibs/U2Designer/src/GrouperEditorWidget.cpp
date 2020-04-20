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

#include <U2Designer/GrouperEditor.h>

#include <U2Lang/BaseTypes.h>
#include <U2Lang/CoreLibConstants.h>
#include <U2Lang/IntegralBusModel.h>

#include "GrouperEditorWidget.h"
#include "NewGrouperSlotDialog.h"

namespace U2 {

using namespace Workflow;
static QMap<Descriptor, DataTypePtr> getBusMap(Port *inPort) {
    QMap<Port*,Link*> links = inPort->getLinks();
    if (links.size() != 1) {
        return QMap<Descriptor, DataTypePtr>();
    }
    QMap<Descriptor, DataTypePtr> busMap;
    {
        Port *src = links.keys().first();
        assert(src->isOutput());
        IntegralBusPort *bus = dynamic_cast<IntegralBusPort*>(src);
        assert(NULL != bus);
        DataTypePtr type = bus->getType();
        busMap = type->getDatatypesMap();
    }
    return busMap;
}

GrouperEditorWidget::GrouperEditorWidget(GrouperSlotsCfgModel *grouperModel, Actor *grouper, QWidget *parent)
: QWidget(parent), grouperModel(grouperModel), grouper(grouper)
{
    assert(1 == grouper->getInputPorts().size());
    inPort = grouper->getInputPorts().first();
    QString groupSlot = grouper->getParameter(CoreLibConstants::GROUPER_SLOT_ATTR)->getAttributeValueWithoutScript<QString>();
    groupSlot = GrouperOutSlot::readable2busMap(groupSlot);
    QString groupOp = grouper->getParameter(CoreLibConstants::GROUPER_OPER_ATTR)->getAttributeValueWithoutScript<QString>();

    setupUi(this);
    {
        slotsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
        slotsTable->horizontalHeader()->setSectionsClickable(false);
        slotsTable->horizontalHeader()->setStretchLastSection(true);
        slotsTable->verticalHeader()->hide();
        slotsTable->verticalHeader()->setDefaultSectionSize(QFontMetrics(QFont()).height() + 6);
    }
    slotsTable->setModel(grouperModel);

    QMap<Descriptor, DataTypePtr> busMap = getBusMap(inPort);
    {
        slotBox->addItem(tr("<Unset>"));
        int currentIdx = 1;
        foreach (const Descriptor &d, busMap.keys()) {
            DataTypePtr type = busMap.value(d);
            if (BaseTypes::DNA_SEQUENCE_TYPE() == type ||
                BaseTypes::MULTIPLE_ALIGNMENT_TYPE() == type ||
                BaseTypes::STRING_TYPE() == type) {

                slotBox->addItem(d.getDisplayName(), d.getId());
                if (groupSlot == d.getId()) {
                    slotBox->setCurrentIndex(currentIdx);
                }
                currentIdx++;
            }
        }
        sl_onGroupSlotChanged(slotBox->currentIndex());
    }

    int idx = slotBox->currentIndex();
    setupGroupOpBox(idx, groupOp, busMap);

    editButton->setEnabled(false);
    removeButton->setEnabled(false);

    connect(addButton, SIGNAL(clicked()), SLOT(sl_onAddButtonClicked()));
    connect(editButton, SIGNAL(clicked()), SLOT(sl_onEditButtonClicked()));
    connect(removeButton, SIGNAL(clicked()), SLOT(sl_onRemoveButtonClicked()));

    connect(slotBox, SIGNAL(currentIndexChanged(int)), SLOT(sl_onGroupSlotChanged(int)));
    connect(operationBox, SIGNAL(currentIndexChanged(int)), SLOT(sl_onGroupOpChanged(int)));
    connect(slotsTable, SIGNAL(entered(const QModelIndex &)), SLOT(sl_onItemEntered(const QModelIndex &)));
    connect(slotsTable, SIGNAL(pressed(const QModelIndex &)), SLOT(sl_onItemSelected(const QModelIndex &)));
}

void GrouperEditorWidget::setupGroupOpBox(int slotIdx, const QString &groupOp, const QMap<Descriptor, DataTypePtr> &busMap) {
    operationBox->model()->removeRows(0, operationBox->count());

    operationBox->addItem(tr("By value"), "by-value"); // for all types

    QString slotId = slotBox->itemData(slotIdx).toString();
    operationBox->setDisabled(slotId.isEmpty());
    DataTypePtr type = busMap.value(slotId);
    if (BaseTypes::DNA_SEQUENCE_TYPE() == type) {
        operationBox->addItem(tr("By id"), "by-id");
        operationBox->addItem(tr("By name"), "by-name");
    }

    for (int i=0; i<operationBox->count(); i++) {
        QString data = operationBox->itemData(i).toString();
        if (data == groupOp) {
            operationBox->setCurrentIndex(i);
            break;
        }
    }
    sl_onGroupOpChanged(operationBox->currentIndex());
}

void GrouperEditorWidget::sl_onGroupSlotChanged(int idx) {
    QString groupSlot = slotBox->itemData(idx).toString();
    groupSlot = GrouperOutSlot::busMap2readable(groupSlot);

    Attribute *a = grouper->getParameter(CoreLibConstants::GROUPER_SLOT_ATTR);
    a->setAttributeValue(groupSlot);

    QMap<Descriptor, DataTypePtr> busMap = getBusMap(inPort);
    QString groupOp = grouper->getParameter(CoreLibConstants::GROUPER_OPER_ATTR)->getAttributeValueWithoutScript<QString>();
    setupGroupOpBox(idx, groupOp, busMap);

    emit si_grouperCfgChanged();

    // TODO: send signal to unlink differently typed linked slots
}

void GrouperEditorWidget::sl_onGroupOpChanged(int idx) {
    QString groupOp = operationBox->itemData(idx).toString();

    Attribute *a = grouper->getParameter(CoreLibConstants::GROUPER_OPER_ATTR);
    a->setAttributeValue(groupOp);

    emit si_grouperCfgChanged();
}

void GrouperEditorWidget::sl_onItemEntered(const QModelIndex &idx) {
    Qt::MouseButtons bs = QApplication::mouseButtons();
    if (bs.testFlag(Qt::LeftButton)) {
        sl_onItemSelected(idx);
    }
}

void GrouperEditorWidget::sl_onItemSelected(const QModelIndex &) {
    editButton->setEnabled(true);
    removeButton->setEnabled(true);
}

void GrouperEditorWidget::sl_onAddButtonClicked() {
    QMap<Descriptor, DataTypePtr> busMap = getBusMap(inPort);
    QList<Descriptor> descs;
    foreach (const Descriptor &d, busMap.keys()) {
        DataTypePtr type = busMap.value(d);
        if (BaseTypes::DNA_SEQUENCE_TYPE() == type ||
            BaseTypes::MULTIPLE_ALIGNMENT_TYPE() == type ||
            BaseTypes::STRING_TYPE() == type ||
            BaseTypes::ANNOTATION_TABLE_LIST_TYPE() == type ||
            BaseTypes::ANNOTATION_TABLE_TYPE() == type) {
            descs << d;
        }
    }
    if (descs.isEmpty()) {
        return;
    }
    QStringList names;
    for (int i=0; i<grouperModel->rowCount(QModelIndex()); i++) {
        QModelIndex idx = grouperModel->index(i, 0);
        names.append(grouperModel->data(idx).toString());
    }

    QObjectScopedPointer<NewGrouperSlotDialog> dlg = new NewGrouperSlotDialog(this, descs, names);
    const int dialogResult = dlg->exec();
    CHECK(!dlg.isNull(), );

    if (QDialog::Accepted == dialogResult) {
        QString inSlotId = dlg->getInSlotId();
        QString outSlotName = dlg->getOutSlotName();
        DataTypePtr type = busMap.value(inSlotId);
        inSlotId = GrouperOutSlot::busMap2readable(inSlotId);

        QObjectScopedPointer<ActionDialog> aDlg = ActionDialog::getActionDialog(this, NULL, type, grouperModel);
        CHECK(!aDlg.isNull(), );
        const int dialogResult = aDlg->exec();
        CHECK(!aDlg.isNull(), );

        if (QDialog::Accepted == dialogResult) {
            GrouperSlotAction action = aDlg->getAction();
            GrouperOutSlot newSlot(outSlotName, inSlotId);
            newSlot.setAction(action);

            GrouperSlotsCfgModel *model = dynamic_cast<GrouperSlotsCfgModel*>(grouperModel);
            assert(NULL != model);
            model->addGrouperSlot(newSlot);
        }
    }

    emit si_grouperCfgChanged();
}

void GrouperEditorWidget::sl_onEditButtonClicked() {
    QItemSelectionModel *m = slotsTable->selectionModel();
    QModelIndexList selected = m->selectedRows();
    if (1 != selected.size()) {
        return;
    }
    QModelIndex leftIdx = selected.first();
    QModelIndex rightIdx = leftIdx.child(leftIdx.row(), 1);

    GrouperSlotsCfgModel *model = dynamic_cast<GrouperSlotsCfgModel*>(grouperModel);
    assert(NULL != model);
    QString outSlotName = model->data(leftIdx).toString();
    QString inSlotId = GrouperOutSlot::readable2busMap(model->data(rightIdx).toString());
    GrouperSlotAction *action = model->getSlotAction(outSlotName);

    QMap<Descriptor, DataTypePtr> busMap = getBusMap(inPort);
    DataTypePtr type = busMap.value(inSlotId);

    QObjectScopedPointer<ActionDialog> aDlg = ActionDialog::getActionDialog(this, action, type, model);
    CHECK(!aDlg.isNull(), );
    const int dialogResult = aDlg->exec();
    CHECK(!aDlg.isNull(), );

    if (QDialog::Accepted == dialogResult) {
        GrouperSlotAction action = aDlg->getAction();
        model->setNewAction(outSlotName, action);
    }

    emit si_grouperCfgChanged();
}

void GrouperEditorWidget::sl_onRemoveButtonClicked() {
    QItemSelectionModel *m = slotsTable->selectionModel();
    QModelIndexList selected = m->selectedRows();
    if (1 != selected.size()) {
        return;
    }

    grouperModel->removeRows(selected.first().row(), 1, selected.first());

    emit si_grouperCfgChanged();
}

/************************************************************************/
/* GrouperSlotsCfgModel */
/************************************************************************/
GrouperSlotsCfgModel::GrouperSlotsCfgModel(QObject *parent, QList<GrouperOutSlot> &outSlots)
: QAbstractTableModel(parent), outSlots(outSlots)
{

}

QVariant GrouperSlotsCfgModel::data(const QModelIndex &index, int role) const {
    if (Qt::DisplayRole == role || Qt::ToolTipRole == role) {
        const GrouperOutSlot &slot = outSlots.at(index.row());

        if (0 == index.column()) {
            return slot.getOutSlotId();
        } else if (1 == index.column()) {
            return slot.getInSlotStr();
        }
    } else {
        return QVariant();
    }
    return QVariant();
}

int GrouperSlotsCfgModel::columnCount(const QModelIndex &) const {
    return 2;
}

int GrouperSlotsCfgModel::rowCount(const QModelIndex &) const {
    return outSlots.size();
}

Qt::ItemFlags GrouperSlotsCfgModel::flags( const QModelIndex & ) const {
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant GrouperSlotsCfgModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch(section) {
        case 0:
            return tr("Output slot name");
        case 1:
            return tr("Source data slot");
        default:
            assert(false);
        }
    }
    // unreachable code
    return QVariant();
}

bool GrouperSlotsCfgModel::setData( const QModelIndex &, const QVariant &, int ) {
    return true;
}

bool GrouperSlotsCfgModel::removeRows(int row, int count, const QModelIndex &) {
    if (1 != count) {
        return true;
    }
    if (row > outSlots.size() - 1) {
        return true;
    }

    const GrouperOutSlot &slot = outSlots.at(row);
    QString outSlotName = slot.getOutSlotId();
    beginRemoveRows(QModelIndex(), row, row+count-1);
    outSlots.removeOne(slot);
    endRemoveRows();

    emit si_slotRemoved(outSlotName);

    return true;
}

QStringList GrouperSlotsCfgModel::getMergeSeqSlotsNames() const {
    QStringList result;
    foreach (const GrouperOutSlot &slot, outSlots) {
        if (ActionTypes::MERGE_SEQUENCE == slot.getAction()->getType()) {
            result << slot.getOutSlotId();
        }
    }
    return result;
}

GrouperSlotAction *GrouperSlotsCfgModel::getSlotAction(const QString &outSlotName) const {
    foreach (const GrouperOutSlot &slot, outSlots) {
        if (slot.getOutSlotId() == outSlotName) {
            return slot.getAction();
        }
    }

    return NULL;
}

void GrouperSlotsCfgModel::addGrouperSlot(const GrouperOutSlot &newSlot) {
    int rows = rowCount(QModelIndex());
    rows = rows>0 ? rows-1 : 0;
    beginInsertRows(QModelIndex(), 0, rows);
    outSlots << newSlot;
    endInsertRows();
    emit si_slotAdded(newSlot);
}

void GrouperSlotsCfgModel::setNewAction(const QString &outSlotName, const GrouperSlotAction &action) {
    QList<GrouperOutSlot>::iterator i = outSlots.begin();
    for (; i != outSlots.end(); ++i) {
        if (i->getOutSlotId() == outSlotName) {
            i->setAction(action);
            emit si_actionEdited(*i);
            break;
        }
    }
}

} // U2
