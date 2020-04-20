/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2017 UniPro <ugene@unipro.ru>
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

#ifndef _HI_GUI_GTACTION_H_
#define _HI_GUI_GTACTION_H_

#include "GTGlobals.h"
#include <QAction>

class QAbstractButton;
namespace HI {

class HI_EXPORT GTAction {
public:
    // returns first QAbstractButton associated with an action with a given name
    static QAbstractButton* button(GUITestOpStatus &os, const QString &actionName, QObject *parent = NULL, const GTGlobals::FindOptions& = GTGlobals::FindOptions());

    static QAbstractButton* button(GUITestOpStatus &os, const QAction* a, QObject *parent = NULL);

    static QAction* findAction(GUITestOpStatus &os, const QString &actionName, QObject *parent = NULL, const GTGlobals::FindOptions& options = GTGlobals::FindOptions());

    static QAction* findActionByText(GUITestOpStatus &os, const QString &text, QWidget *parent = NULL);
};

} //namespace

#endif
