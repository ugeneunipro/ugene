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

#ifndef _SHARED_CONNECTIONS_DIALOG_FILLER_H_
#define _SHARED_CONNECTIONS_DIALOG_FILLER_H_

#ifdef DELETE
#undef DELETE
#endif

#include <QFlags>

#include "utils/GTUtilsDialog.h"
#include <base_dialogs/GTFileDialog.h>

namespace U2 {

class SharedConnectionsDialogFiller : public Filler {
public:
    class Action {
    public:
        enum Type {ADD, CLICK, EDIT, DELETE, CONNECT, DISCONNECT, CLOSE};

        Action(Type type, QString itemName = "");
        Type type;
        QString itemName;
        QString dbName;

        // for CONNECT
        enum ConnectResult {OK, WRONG_DATA, INITIALIZE, DONT_INITIALIZE, VERSION, LOGIN};
        ConnectResult expectedResult;
    };

    SharedConnectionsDialogFiller(HI::GUITestOpStatus &os, const QList<Action> &actions);
    SharedConnectionsDialogFiller(HI::GUITestOpStatus &os, CustomScenario *scenario);

    void commonScenario();

private:
    QList<Action> actions;
};

} // U2

#endif // _SHARED_CONNECTIONS_DIALOG_FILLER_H_
