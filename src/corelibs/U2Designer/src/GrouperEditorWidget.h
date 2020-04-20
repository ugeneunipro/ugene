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

#ifndef _GROUPER_EDITOR_WIDGET_H_
#define _GROUPER_EDITOR_WIDGET_H_

#include <U2Lang/ActorModel.h>
#include <U2Lang/GrouperOutSlot.h>

#include "ui_GrouperEditorWidget.h"

namespace U2 {

class GrouperSlotsCfgModel;

class GrouperEditorWidget: public QWidget, public Ui_GrouperEditorWidget {
    Q_OBJECT
public:
    GrouperEditorWidget(GrouperSlotsCfgModel *gouperModel, Workflow::Actor *grouper, QWidget *parent = NULL);

signals:
    void si_grouperCfgChanged();

private:
    GrouperSlotsCfgModel *grouperModel;
    Workflow::Actor *grouper;
    Workflow::Port *inPort;

private slots:
    void sl_onAddButtonClicked();
    void sl_onEditButtonClicked();
    void sl_onRemoveButtonClicked();

    void sl_onGroupSlotChanged(int idx);
    void sl_onGroupOpChanged(int idx);
    void sl_onItemSelected(const QModelIndex &idx);
    void sl_onItemEntered(const QModelIndex &idx);

private:
    void setupGroupOpBox(int slotIdx, const QString &groupOp, const QMap<Descriptor, DataTypePtr> &busMap);
};

class GrouperSlotsCfgModel : public QAbstractTableModel {
    Q_OBJECT
public:
    GrouperSlotsCfgModel(QObject *parent, QList<GrouperOutSlot> &outSlots);

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    int columnCount(const QModelIndex &) const;
    int rowCount(const QModelIndex &) const;
    Qt::ItemFlags flags( const QModelIndex & index ) const;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

    QStringList getMergeSeqSlotsNames() const;
    GrouperSlotAction *getSlotAction(const QString &outSlotName) const;
    void addGrouperSlot(const GrouperOutSlot &newSlot);
    void setNewAction(const QString &outSlotName, const GrouperSlotAction &action);

signals:
    void si_actionEdited(const GrouperOutSlot &outSlot);
    void si_slotAdded(const GrouperOutSlot &outSlot);
    void si_slotRemoved(const QString &outSlotName);

private:
    QList<GrouperOutSlot> &outSlots;
};

} // U2

#endif // _GROUPER_EDITOR_WIDGET_H_
