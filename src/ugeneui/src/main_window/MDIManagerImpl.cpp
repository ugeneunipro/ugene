/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2024 UniPro <ugene@unipro.ru>
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

#include "MDIManagerImpl.h"

#include <QAction>
#include <QMenu>
#include <QSet>
#include <QShortcut>
#include <QToolBar>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/Log.h>
#include <U2Core/Settings.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UserApplicationsSettings.h>

namespace U2 {

#define SETTINGS_DIR QString("main_window/mdi/")

static QString getWindowName(MDIItem* mdiItem) {
    if (mdiItem == nullptr) {
        return "<no window>";
    }
    return mdiItem->w->windowTitle();
}

void MWMDIManagerImpl::prepareGUI() {
    mdiContentOwner = nullptr;

    connect(mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)), SLOT(sl_onSubWindowActivated(QMdiSubWindow*)));

    windowMapper = new QSignalMapper(this);
    connect(windowMapper, SIGNAL(mapped(QWidget*)), this, SLOT(sl_setActiveSubWindow(QWidget*)));

    // prepare Window menu
    closeAct = new QAction(tr("Close active view"), this);
    closeAct->setObjectName("Close active view");
    closeAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_W));
    closeAct->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    closeAct->setStatusTip(tr("Close active view"));
    connect(closeAct, SIGNAL(triggered()), mdiArea, SLOT(closeActiveSubWindow()));

    closeAllAct = new QAction(tr("Close all windows"), this);
    closeAllAct->setObjectName("Close all windows");
    closeAllAct->setStatusTip(tr("Close all windows"));
    connect(closeAllAct, SIGNAL(triggered()), mdiArea, SLOT(closeAllSubWindows()));

    windowLayout = new QMenu(tr("Window layout"));
    windowLayout->setObjectName("Window layout");
    windowLayout->setStatusTip(tr("Window layout"));

    bool tabbedLayout = AppContext::getAppSettings()->getUserAppsSettings()->tabbedWindowLayout();
    multipleDocsAct = new QAction(tr("Multiple documents"), this);
    multipleDocsAct->setCheckable(true);
    multipleDocsAct->setChecked(!tabbedLayout);
    connect(multipleDocsAct, SIGNAL(triggered()), SLOT(sl_setWindowLayoutToMultiDoc()));

    tabbedDocsAct = new QAction(tr("Tabbed documents"), this);
    tabbedDocsAct->setCheckable(true);
    tabbedDocsAct->setChecked(tabbedLayout);
    connect(tabbedDocsAct, SIGNAL(triggered()), SLOT(sl_setWindowLayoutToTabbed()));

    tileAct = new QAction(QIcon(":ugene/images/window_tile.png"), tr("Tile windows"), this);
    tileAct->setObjectName("Tile windows");
    tileAct->setStatusTip(tr("Tile windows"));
    connect(tileAct, SIGNAL(triggered()), mdiArea, SLOT(tileSubWindows()));

    cascadeAct = new QAction(QIcon(":ugene/images/window_cascade.png"), tr("Cascade windows"), this);
    cascadeAct->setObjectName("Cascade windows");
    cascadeAct->setStatusTip(tr("Cascade windows"));
    connect(cascadeAct, SIGNAL(triggered()), mdiArea, SLOT(cascadeSubWindows()));

    QKeySequence nextWindowKeySequence;
    QKeySequence prevWindowKeySequence;
    if (isOsMac()) {
        nextWindowKeySequence = QKeySequence(Qt::CTRL + (Qt::Key)'`');
        prevWindowKeySequence = QKeySequence(Qt::CTRL + Qt::SHIFT + (Qt::Key)'`');

        auto additionalNextShortcut = new QShortcut(QKeySequence(Qt::META + Qt::Key_Tab), mdiArea);
        connect(additionalNextShortcut, &QShortcut::activated, mdiArea, &QMdiArea::activateNextSubWindow);

        auto additionalPrevShortcut = new QShortcut(QKeySequence(Qt::META + Qt::SHIFT + Qt::Key_Tab), mdiArea);
        connect(additionalPrevShortcut, &QShortcut::activated, mdiArea, &QMdiArea::activatePreviousSubWindow);
    } else {
        nextWindowKeySequence = QKeySequence(Qt::CTRL + Qt::Key_Tab);
        prevWindowKeySequence = QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_Tab);
    }

    nextAct = new QAction(QIcon(":ugene/images/window_next.png"), tr("Next window"), this);
    nextAct->setObjectName("Next window");
    nextAct->setStatusTip(tr("Next window"));
    nextAct->setShortcut(nextWindowKeySequence);
    connect(nextAct, SIGNAL(triggered()), mdiArea, SLOT(activateNextSubWindow()));

    previousAct = new QAction(QIcon(":ugene/images/window_prev.png"), tr("Previous window"), this);
    previousAct->setObjectName("Previous window");
    previousAct->setStatusTip(tr("Previous window"));
    previousAct->setShortcut(prevWindowKeySequence);
    connect(previousAct, SIGNAL(triggered()), mdiArea, SLOT(activatePreviousSubWindow()));

    separatorAct = new QAction("-", this);
    separatorAct->setSeparator(true);

    defaultIsMaximized = AppContext::getSettings()->getValue(SETTINGS_DIR + "maximized", true).toBool();

    QMenu* windowMenu = mw->getTopLevelMenu(MWMENU_WINDOW);
    connect(windowMenu, SIGNAL(aboutToShow()), this, SLOT(sl_updateWindowMenu()));

    updateState();
    clearMDIContent(true);

    sl_updateWindowLayout();
    connect(AppContext::getAppSettings()->getUserAppsSettings(), SIGNAL(si_windowLayoutChanged()), SLOT(sl_updateWindowLayout()));
}

bool MWMDIManagerImpl::eventFilter(QObject* obj, QEvent* event) {
    QEvent::Type t = event->type();
    if (t == QEvent::Close) {
        auto qw = qobject_cast<QMdiSubWindow*>(obj);
        MDIItem* item = getMDIItem(qw);
        if (item != nullptr) {
            uiLog.trace(QString("Processing close window request for '%1'").arg(getWindowName(item)));

            // check if user really wants to close the window, ignore event if not
            if (!onCloseEvent(item->w)) {
                uiLog.trace(QString("Ignoring close window request for '%1'").arg(getWindowName(item)));
                event->ignore();
                return true;
            }

            // here we sure that window will be closed
            emit si_windowClosing(item->w);

            if (item == mdiContentOwner) {  // if 'current' window is closed -> clear MDI
                clearMDIContent(true);
            }
            items.removeAll(item);
            delete item;
            updateState();
        }
    } else if (t == QEvent::WindowStateChange) {
        auto qw = qobject_cast<QMdiSubWindow*>(obj);
        defaultIsMaximized = qw->isMaximized();
    }
    return QObject::eventFilter(obj, event);
}

void MWMDIManagerImpl::updateState() {
    updateActions();
    sl_updateWindowMenu();

    AppContext::getSettings()->setValue(SETTINGS_DIR + "maximized", defaultIsMaximized);
}

void MWMDIManagerImpl::updateActions() {
    bool hasMDIWindows = !items.empty();

    closeAct->setEnabled(hasMDIWindows);
    closeAllAct->setEnabled(hasMDIWindows);
    tileAct->setEnabled(hasMDIWindows);
    cascadeAct->setEnabled(hasMDIWindows);
    nextAct->setEnabled(hasMDIWindows);
    previousAct->setEnabled(hasMDIWindows);
    separatorAct->setVisible(hasMDIWindows);
}

void MWMDIManagerImpl::sl_updateWindowMenu() {
    QMenu* windowMenu = mw->getTopLevelMenu(MWMENU_WINDOW);
    windowMenu->clear();  // TODO: avoid cleaning 3rd party actions
    windowMenu->addMenu(windowLayout);
    windowLayout->addAction(multipleDocsAct);
    windowLayout->addAction(tabbedDocsAct);
    windowMenu->addAction(closeAct);
    windowMenu->addAction(closeAllAct);

    if (mdiArea->viewMode() == QMdiArea::SubWindowView) {
        windowMenu->addSeparator();
        windowMenu->addAction(tileAct);
        windowMenu->addAction(cascadeAct);
    }
    windowMenu->addSeparator();
    windowMenu->addAction(nextAct);
    windowMenu->addAction(previousAct);
    windowMenu->addAction(separatorAct);

    separatorAct->setVisible(!items.isEmpty());

    MDIItem* currentItem = getCurrentMDIItem();
    for (int i = 0; i < items.size(); ++i) {
        MDIItem* item = items.at(i);
        QString text;
        if (i < 9) {
            text = QString("&%1 %2").arg(i + 1).arg(item->w->windowTitle());
        } else {
            text = QString("%1 %2").arg(i + 1).arg(item->w->windowTitle());
        }
        QAction* action = windowMenu->addAction(text);
        action->setCheckable(true);
        action->setChecked(item == currentItem);
        connect(action, SIGNAL(triggered()), windowMapper, SLOT(map()));
        windowMapper->setMapping(action, item->qw);
    }
}

MDIItem* MWMDIManagerImpl::getCurrentMDIItem() const {
    QMdiSubWindow* currentSubWindow = mdiArea->currentSubWindow();
    if (currentSubWindow != nullptr) {
        return getMDIItem(currentSubWindow);
    }
    return nullptr;
}

void MWMDIManagerImpl::addMDIWindow(MWMDIWindow* w) {
    SAFE_POINT(getWindowById(w->getId()) == nullptr, "MDI window is already registered", );
    w->setParent(mdiArea);
    w->setVisible(true);
    QMdiSubWindow* qw = mdiArea->addSubWindow(w);
    qw->setWindowTitle(w->windowTitle());
    QIcon icon = w->windowIcon();
    if (icon.isNull()) {
        icon = QIcon(":/ugene/images/ugene_16.png");
    }
    qw->setWindowIcon(icon);
    if (isOsMac()) {
        qw->setAttribute(Qt::WA_NativeWindow);
    }
    items.append(new MDIItem(w, qw));
    qw->installEventFilter(this);

    uiLog.trace(QString("Adding window: '%1'").arg(w->windowTitle()));

    updateState();

    emit si_windowAdded(w);

    if (items.count() == 1 && defaultIsMaximized) {
        qw->showMaximized();
    } else {
        qw->show();
    }
    qw->raise();
}

QList<MWMDIWindow*> MWMDIManagerImpl::getWindows() const {
    QList<MWMDIWindow*> res;
    foreach (MDIItem* i, items) {
        res.append(i->w);
    }
    return res;
}

bool MWMDIManagerImpl::closeMDIWindow(MWMDIWindow* w) {
    MDIItem* i = getMDIItem(w);
    if (i == nullptr)
        return false;
    return i->qw->close();
}

MWMDIWindow* MWMDIManagerImpl::getWindowById(int id) const {
    MDIItem* i = getMDIItem(id);
    return i == nullptr ? nullptr : i->w;
}

MDIItem* MWMDIManagerImpl::getMDIItem(int id) const {
    foreach (MDIItem* i, items) {
        if (i->w->getId() == id) {
            return i;
        }
    }
    return nullptr;
}

MDIItem* MWMDIManagerImpl::getMDIItem(MWMDIWindow* w) const {
    foreach (MDIItem* i, items) {
        if (i->w == w) {
            return i;
        }
    }
    return nullptr;
}

MDIItem* MWMDIManagerImpl::getMDIItem(QMdiSubWindow* qw) const {
    foreach (MDIItem* item, items) {
        if (item->qw == qw) {
            return item;
        }
    }
    return nullptr;
}

void MWMDIManagerImpl::activateWindow(MWMDIWindow* w) {
    MDIItem* i = getMDIItem(w);
    if (i == nullptr) {
        return;
    }
    AppContext::setActiveWindowName(w->windowTitle());
    mdiArea->setActiveSubWindow(i->qw);
    updateState();
}

void MWMDIManagerImpl::sl_setActiveSubWindow(QWidget* w) {
    if (!w) {
        return;
    }
    mdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow*>(w));
}

void MWMDIManagerImpl::clearMDIContent(bool addCloseAction) {
    // clear toolbar
    mdiContentOwner = nullptr;

    QToolBar* tb = mw->getToolbar(MWTOOLBAR_ACTIVEMDI);
    QMenu* m = mw->getTopLevelMenu(MWMENU_ACTIONS);

    // delete submenus inserted to toolbar and menu
    // todo: provide a flag to enable/disable this behavior for MDI window
    QList<QAction*> allMDIActions = tb->actions() + m->actions();
    QSet<QMenu*> toDel;
    foreach (QAction* ma, allMDIActions) {
        QMenu* am = ma->menu();
        if (am != nullptr) {
            toDel.insert(am);
        }
    }

    tb->clear();
    m->clear();

    foreach (QMenu* mtd, toDel) {
        delete mtd;
    }

    if (addCloseAction) {
        m->addAction(closeAct);
    }
}

void MWMDIManagerImpl::onWindowsSwitched(QMdiSubWindow* deactivated, MWMDIWindow* activated) {
    MDIItem* deItem = getMDIItem(deactivated);
    if ((deItem != nullptr) && (deItem->w != nullptr)) {
        emit si_windowDeactivated(deItem->w);
    }
    emit si_windowActivated(activated);
}

void MWMDIManagerImpl::sl_onSubWindowActivated(QMdiSubWindow* w) {
    // Details: sub-window is activated and deactivated
    //  1) every time user switches MDI windows
    //  2) every time user switches applications
    //  Here we update mdi content only for 1 case and trying to avoid 2 case

    QMdiSubWindow* currentWindow = mdiArea->currentSubWindow();
    if (w == nullptr && currentWindow != nullptr) {  // simple deactivation, current window is not changed
        uiLog.trace(QString("Window deactivation, no MDI context switch, window: '%1'").arg(getWindowName(mdiContentOwner)));
        assert(getMDIItem(currentWindow) == mdiContentOwner);
        emit si_windowActivated(nullptr);
        return;
    }
    if (mdiContentOwner != nullptr && mdiContentOwner->qw == w) {  // simple activation, current window is not changed
        uiLog.trace(QString("Window activation, no MDI context switch, window: '%1'").arg(getWindowName(mdiContentOwner)));
        emit si_windowActivated(mdiContentOwner->w);
        return;
    }
    if (w == nullptr) {  // currentWindow is NULL here, mdiContentOwner & it's content cleaned in eventFilter(CloseEvent)
        uiLog.trace(QString("Closing active window"));
        clearMDIContent(true);  // UGENE-4987 workaround. It must be 'false' here
        emit si_windowActivated(nullptr);
        return;
    }
    MDIItem* mdiItem = getMDIItem(w);
    SAFE_POINT(mdiItem != nullptr, "The window does not belong to the MDI manager!", );
    uiLog.trace(QString("Switching active MDI window from '%1' to '%2'").arg(getWindowName(mdiContentOwner)).arg(getWindowName(mdiItem)));
    // clear old windows menu/tb content
    clearMDIContent(false);

    // add new content to menu/tb
    QToolBar* tb = mw->getToolbar(MWTOOLBAR_ACTIVEMDI);
    mdiContentOwner = mdiItem;
    SAFE_POINT(mdiContentOwner->w != nullptr, "Incorrect MDI window is detected!", );
    mdiContentOwner->w->setupMDIToolbar(tb);

    QMenu* m = mw->getTopLevelMenu(MWMENU_ACTIONS);
    SAFE_POINT(mdiContentOwner->w != nullptr, "Incorrect menu is detected!", );
    mdiContentOwner->w->setupViewMenu(m);
    m->addAction(closeAct);
    onWindowsSwitched(w, mdiItem->w);
}

MWMDIWindow* MWMDIManagerImpl::getActiveWindow() const {
    MDIItem* i = getCurrentMDIItem();
    if (i == nullptr) {
        return nullptr;
    }
    return i->w;
}

void MWMDIManagerImpl::sl_updateWindowLayout() {
    bool tabbed = AppContext::getAppSettings()->getUserAppsSettings()->tabbedWindowLayout();
    mdiArea->setViewMode(tabbed ? QMdiArea::TabbedView : QMdiArea::SubWindowView);
    multipleDocsAct->setChecked(!tabbed);
    tabbedDocsAct->setChecked(tabbed);
}

void MWMDIManagerImpl::sl_setWindowLayoutToMultiDoc() {
    UserAppsSettings* st = AppContext::getAppSettings()->getUserAppsSettings();
    st->setTabbedWindowLayout(false);
    sl_updateWindowLayout();
}

void MWMDIManagerImpl::sl_setWindowLayoutToTabbed() {
    UserAppsSettings* st = AppContext::getAppSettings()->getUserAppsSettings();
    st->setTabbedWindowLayout(true);
    sl_updateWindowLayout();
}

}  // namespace U2
