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

#include <QHeaderView>
#include <QSplitter>
#include <QTableView>

#include <U2Lang/GrouperSlotAttribute.h>

#include "GrouperEditorWidget.h"

#include "GrouperEditor.h"

namespace U2 {
namespace Workflow {

/************************************************************************/
/* GrouperEditor */
/************************************************************************/
GrouperEditor::GrouperEditor()
: ActorConfigurationEditor(), grouperModel(NULL)
{
}

GrouperEditor::~GrouperEditor() {
}

QWidget *GrouperEditor::getWidget() {
    return createGUI();
}

QWidget *GrouperEditor::createGUI() {
    if (NULL == grouperModel) {
        return NULL;
    }

    GrouperEditorWidget *gui = new GrouperEditorWidget(grouperModel, cfg);
    connect(gui, SIGNAL(si_grouperCfgChanged()), SIGNAL(si_configurationChanged()));

    return gui;
}

void GrouperEditor::setConfiguration(Actor *actor) {
    ActorConfigurationEditor::setConfiguration(actor);
    QMap<QString, Attribute*> attrs = cfg->getParameters();

    GrouperOutSlotAttribute *gAttr = NULL;
    foreach (QString key, attrs.keys()) {
        Attribute *attr = attrs.value(key);
        if (GROUPER_SLOT_GROUP == attr->getGroup()) {
            if (NULL != gAttr) {
                assert(0);
                gAttr = NULL;
                break;
            }
            gAttr = dynamic_cast<GrouperOutSlotAttribute*>(attr);
        }
    }

    if (NULL == gAttr) {
        return;
    }

    grouperModel = new GrouperSlotsCfgModel(this, gAttr->getOutSlots());
    connect(grouperModel, SIGNAL(si_actionEdited(const GrouperOutSlot &)), SLOT(sl_onActionEdited(const GrouperOutSlot &)));
    connect(grouperModel, SIGNAL(si_slotAdded(const GrouperOutSlot &)), SLOT(sl_onSlotAdded(const GrouperOutSlot &)));
    connect(grouperModel, SIGNAL(si_slotRemoved(const QString &)), SLOT(sl_onSlotRemoved(const QString &)));
}

void GrouperEditor::sl_onActionEdited(const GrouperOutSlot &outSlot) {
    sl_onSlotAdded(outSlot);
    // TODO: send signal to unlink differently typed linked slots
}

void GrouperEditor::sl_onSlotAdded(const GrouperOutSlot &outSlot) {
    assert(1 == cfg->getOutputPorts().size());
    Port *outPort = cfg->getOutputPorts().first();
    assert(outPort->getOutputType()->isMap());
    QMap<Descriptor, DataTypePtr> outTypeMap = outPort->getOutputType()->getDatatypesMap();

    GrouperSlotAction *action = outSlot.getAction();
    DataTypePtr type = ActionTypes::getDataTypeByAction(action->getType());

    outTypeMap[outSlot.getOutSlotId()] = type;
    DataTypePtr newType(new MapDataType(dynamic_cast<Descriptor&>(*(outPort->getType())), outTypeMap));
    outPort->setNewType(newType);
}

void GrouperEditor::sl_onSlotRemoved(const QString &outSlotName) {
    assert(1 == cfg->getOutputPorts().size());
    Port *outPort = cfg->getOutputPorts().first();
    assert(outPort->getOutputType()->isMap());
    QMap<Descriptor, DataTypePtr> outTypeMap = outPort->getOutputType()->getDatatypesMap();

    outTypeMap.remove(outSlotName);
    DataTypePtr newType(new MapDataType(dynamic_cast<Descriptor&>(*(outPort->getType())), outTypeMap));
    outPort->setNewType(newType);
}

} // Workflow
} // U2
