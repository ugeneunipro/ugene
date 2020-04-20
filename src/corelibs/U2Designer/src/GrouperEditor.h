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

#ifndef _GROUPER_EDITOR_H_
#define _GROUPER_EDITOR_H_

#include <U2Lang/ActorModel.h>
#include <U2Lang/Configuration.h>
#include <U2Lang/ConfigurationEditor.h>
#include <U2Lang/GrouperOutSlot.h>

namespace U2 {

class GrouperSlotsCfgModel;

namespace Workflow {

class U2DESIGNER_EXPORT GrouperEditor : public ActorConfigurationEditor {
    Q_OBJECT
public:
    GrouperEditor();
    GrouperEditor(const GrouperEditor &) : ActorConfigurationEditor(), grouperModel(NULL) {}
    virtual ~GrouperEditor();
    virtual QWidget *getWidget();
    virtual void setConfiguration(Actor *actor);
    virtual ConfigurationEditor *clone() {return new GrouperEditor(*this);}

public slots:
    void sl_onActionEdited(const GrouperOutSlot &outSlot);
    void sl_onSlotAdded(const GrouperOutSlot &outSlot);
    void sl_onSlotRemoved(const QString &outSlotName);

private:
    GrouperSlotsCfgModel *grouperModel;

    QWidget *createGUI();
}; // GrouperEditor

} // Workflow
} //U2

#endif // _GROUPER_EDITOR_H_
