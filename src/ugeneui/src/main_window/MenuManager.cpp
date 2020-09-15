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

#include "MenuManager.h"

#include <QMainWindow>

#include <U2Core/Log.h>

namespace U2 {
#define STATIC_MENU_MODEL 1

MWMenuManagerImpl::MWMenuManagerImpl(QObject *p, QMenuBar *mb)
    : QObject(p) {
    menuBar = mb;
    menuBar->setObjectName(MWMENU);
#ifdef Q_OS_MAC
    // TODO: need to check for other OS and remove #ifdef
    menuBar->setNativeMenuBar(true);
#endif
    createTopLevelMenu(MWMENU_FILE, tr("&File"));
    createTopLevelMenu(MWMENU_ACTIONS, tr("&Actions"), MWMENU_FILE);
    createTopLevelMenu(MWMENU_SETTINGS, tr("&Settings"), MWMENU_ACTIONS);
    createTopLevelMenu(MWMENU_TOOLS, tr("&Tools"), MWMENU_SETTINGS);
    createTopLevelMenu(MWMENU_WINDOW, tr("&Window"), MWMENU_TOOLS);
    createTopLevelMenu(MWMENU_HELP, tr("&Help"), MWMENU_WINDOW);
}

QMenu *MWMenuManagerImpl::getTopLevelMenu(const QString &sysName) const {
    foreach (QMenu *m, toplevelMenus) {
        if (m->menuAction()->objectName() == sysName) {
            return m;
        }
    }
    return NULL;
}

void MWMenuManagerImpl::setMenuBarEnabled(bool enable) {
    foreach (QMenu *menu, toplevelMenus) {
        menu->setEnabled(enable);
    }

    foreach (const QPointer<QAction> &action, additionalActions) {
        if (NULL != action) {
            action->setEnabled(enable);
        }
    }
}

void MWMenuManagerImpl::registerAction(QAction *action) {
    additionalActions << QPointer<QAction>(action);
}

QMenu *MWMenuManagerImpl::createTopLevelMenu(const QString &sysName, const QString &title, const QString &afterSysName) {
    QMenu *qmenu = getTopLevelMenu(sysName);
    assert(!qmenu);
    if (qmenu) {
        return qmenu;
    }
    QMenu *menuBefore = getTopLevelMenu(afterSysName);
    if (menuBefore == NULL) {
        menuBefore = getTopLevelMenu(MWMENU_TOOLS);
    }
    qmenu = new QMenu(title, menuBar);
    qmenu->setObjectName(sysName);    //??? need refactoring...
    qmenu->menuAction()->setObjectName(sysName);
    int insertPos = toplevelMenus.indexOf(menuBefore) + 1;
    if (insertPos == 0) {
        insertPos = toplevelMenus.size();
    }
    toplevelMenus.insert(insertPos, qmenu);

#ifdef STATIC_MENU_MODEL
    menuBar->addMenu(qmenu);
    //#else
    if (MWMENU_WINDOW != sysName) {
        qmenu->installEventFilter(this);
#    ifndef Q_OS_MAC
        qmenu->setEnabled(false);
#    else
//        if (!QApplication::testAttribute(Qt::AA_DontUseNativeMenuBar)) {          //UGENE-339, not tested for attribute set
//            qmenu->setEnabled(false);
//        }
#    endif
    }
#endif
    return qmenu;
}

static void touchMenu(QMenu *menu) {
    QList<QMenu *> menus = menu->findChildren<QMenu *>();
    foreach (QMenu *menu, menus) {
        if (!menu->isEnabled()) {
            continue;
        }
        menu->setEnabled(false);
        //coreLog.trace("Touched menu" + menu->menuAction()->text());
        touchMenu(menu);
        menu->setEnabled(true);
    }
}

bool MWMenuManagerImpl::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::ActionAdded || event->type() == QEvent::ActionRemoved) {
        QMenu *menu = qobject_cast<QMenu *>(obj);
        assert(menu != NULL);
        //coreLog.trace("aaa:EventFilter (Menu Manager)");
#ifndef Q_OS_MAC
        menu->setEnabled(!menu->isEmpty());
#else
        touchMenu(menu);
#endif
        //updateTopLevelMenuVisibility(menu);
    }

    return QObject::eventFilter(obj, event);
}

void MWMenuManagerImpl::updateTopLevelMenuVisibility(QMenu *m) {
    const QList<QAction *> &activeActions = menuBar->actions();
    if (m->isEmpty() && activeActions.contains(m->menuAction()) && m->menuAction()->objectName() != MWMENU_WINDOW) {
        unlinkTopLevelMenu(m);
    } else if (!m->isEmpty() && !activeActions.contains(m->menuAction())) {
        linkTopLevelMenu(m);
    }
}

void MWMenuManagerImpl::unlinkTopLevelMenu(QMenu *m) {
    Q_UNUSED(m);
    //assert(menuBar->actions().contains(m->menuAction()));
    //menuBar->removeAction(m->menuAction());
}

void MWMenuManagerImpl::linkTopLevelMenu(QMenu *m) {
    assert(!menuBar->actions().contains(m->menuAction()));
    const QList<QAction *> &activeActions = menuBar->actions();
    QAction *nextActiveAction = NULL;
    for (int i = toplevelMenus.indexOf(m) + 1; i < toplevelMenus.size(); i++) {
        QMenu *tmpM = toplevelMenus.at(i);
        if (activeActions.contains(tmpM->menuAction())) {
            nextActiveAction = tmpM->menuAction();
            break;
        }
    }
    menuBar->insertAction(nextActiveAction, m->menuAction());
}

}    // namespace U2
