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

#include "GUICrazyUserTest.h"
#include "GTRandomGUIActionFactory.h"
#include <drivers/GTKeyboardDriver.h>
#include <primitives/GTWidget.h>
#include <base_dialogs/GTFileDialog.h>
#include "GTRandomGUIActionFactory.h"

#include <QApplication>
#include <U2Core/U2SafePoints.h>

namespace U2 {

namespace GUITest_crazy_user {

void GTCrazyUserMonitor::checkActiveWidget() {
    QWidget* widget = QApplication::activePopupWidget();
    if (NULL == widget || 0 == qrand()%20) {
        widget = QApplication::activeModalWidget();
        if (NULL == widget) {
            widget = QApplication::activeWindow();
        }
    }
    SAFE_POINT(NULL != widget, "",);

    U2OpStatus2Log os;

    GTCrazyUserMonitor* monitor = new GTCrazyUserMonitor();

    QList<GTAbstractGUIAction*> actionList = formGUIActions(widget);
    bool actionListEmpty = actionList.isEmpty();
    SAFE_POINT(false == actionListEmpty, "", );

    qSort(actionList.begin(), actionList.end(), GTAbstractGUIAction::lessThan);
    uiLog.trace("sorted actionList:");
    foreach (GTAbstractGUIAction* a, actionList) {
        uiLog.trace(QString("Action for %1 with priority %2").arg(a->objectClassName()).arg(a->getPriority()));
    }

    GTAbstractGUIAction* action = NULL;
    if (GTAbstractGUIAction::Priority_High == actionList.first()->getPriority()) {
        action = actionList.first();
    } else {
        int randListId = randInt(0, actionList.size()-1);
        uiLog.trace(QString("actionList.size(): %1, randListId = %2").arg(actionList.size()).arg(randListId));

        action = actionList.at(randListId);
    }

    SAFE_POINT(NULL != action, "",);
    action->run();

    qDeleteAll(actionList);
    monitor->deleteLater();
}

QList<GTAbstractGUIAction*> GTCrazyUserMonitor::formGUIActions(QWidget* widget) const {
    QList<GTAbstractGUIAction*> actionList;
    SAFE_POINT(NULL != widget, "", actionList);

    QObjectList objectList = widget->findChildren<QObject*>();
    objectList.append(widget);

    foreach(QObject* o, objectList) {
        GTAbstractGUIAction* guiAction = GTRandomGUIActionFactory::create(o);
        if (NULL != guiAction) {
            actionList.append(guiAction);
        }
    }

    return actionList;
}

GUI_TEST_CLASS_DEFINITION(simple_crazy_user) {
    Q_UNUSED(os);

    qsrand(QTime().msecsTo(QTime::currentTime()));
    GTCrazyUserMonitor m;

    static const int defaultTimeSeconds = 60;
    int time = qgetenv("UGENE_GUI_TEST_CRAZY_USER_TIME").toInt();
    if (0 == time) {
        time = defaultTimeSeconds;
    }

    QEventLoop loop;
    QTimer::singleShot(time*1000, &loop, SLOT(quit()));
    loop.exec();
}

}

}
