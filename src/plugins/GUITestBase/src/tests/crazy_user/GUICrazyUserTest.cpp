/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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
#include <base_dialogs/GTFileDialog.h>
#include <drivers/GTKeyboardDriver.h>
#include <primitives/GTWidget.h>

#include <QApplication>
#include <QRandomGenerator>

#include <U2Core/U2SafePoints.h>

#include "GTRandomGUIActionFactory.h"

namespace U2 {

namespace GUITest_crazy_user {

static QRandomGenerator& rnd() {
    static QRandomGenerator instance(static_cast<quint32>(QDateTime::currentMSecsSinceEpoch()));
    return instance;
}

void GTCrazyUserMonitor::checkActiveWidget() {
    QWidget* widget = QApplication::activePopupWidget();
    if (widget == nullptr || rnd().bounded(20) == 0) {
        widget = QApplication::activeModalWidget();
        if (widget == nullptr) {
            widget = QApplication::activeWindow();
        }
    }
    SAFE_POINT(widget != nullptr, "", );

    U2OpStatus2Log os;

    auto monitor = new GTCrazyUserMonitor();

    QList<GTAbstractGUIAction*> actionList = formGUIActions(widget);
    bool actionListEmpty = actionList.isEmpty();
    SAFE_POINT(false == actionListEmpty, "", );

    std::sort(actionList.begin(), actionList.end(), GTAbstractGUIAction::lessThan);
    uiLog.trace("sorted actionList:");
    foreach (GTAbstractGUIAction* a, actionList) {
        uiLog.trace(QString("Action for %1 with priority %2").arg(a->objectClassName()).arg(a->getPriority()));
    }

    GTAbstractGUIAction* action = nullptr;
    if (GTAbstractGUIAction::Priority_High == actionList.first()->getPriority()) {
        action = actionList.first();
    } else {
        int randListId = randInt(0, actionList.size() - 1);
        uiLog.trace(QString("actionList.size(): %1, randListId = %2").arg(actionList.size()).arg(randListId));

        action = actionList.at(randListId);
    }

    SAFE_POINT(action != nullptr, "", );
    action->run();

    qDeleteAll(actionList);
    monitor->deleteLater();
}

QList<GTAbstractGUIAction*> GTCrazyUserMonitor::formGUIActions(QWidget* widget) const {
    QList<GTAbstractGUIAction*> actionList;
    SAFE_POINT(widget != nullptr, "", actionList);

    QObjectList objectList = widget->findChildren<QObject*>();
    objectList.append(widget);

    foreach (QObject* o, objectList) {
        GTAbstractGUIAction* guiAction = GTRandomGUIActionFactory::create(o);
        if (guiAction != nullptr) {
            actionList.append(guiAction);
        }
    }

    return actionList;
}

GUI_TEST_CLASS_DEFINITION(simple_crazy_user) {
    GTCrazyUserMonitor m;

    static const int defaultTimeSeconds = 60;
    int time = qgetenv("UGENE_GUI_TEST_CRAZY_USER_TIME").toInt();
    if (time == 0) {
        time = defaultTimeSeconds;
    }

    QEventLoop loop;
    QTimer::singleShot(time * 1000, &loop, SLOT(quit()));
    loop.exec();
}

}  // namespace GUITest_crazy_user

}  // namespace U2
