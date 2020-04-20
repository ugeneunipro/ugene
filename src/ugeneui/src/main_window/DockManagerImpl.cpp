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

#include <QDockWidget>
#include <QToolBar>

#include <U2Core/AppContext.h>
#include <U2Core/Settings.h>
#include <U2Core/U2SafePoints.h>

#include "DockManagerImpl.h"
#include "DockWidgetPainter.h"
#include "MainWindowImpl.h"
#include "task_view/TaskStatusBar.h"

namespace U2 {

#define DOCK_SETTINGS  QString("mwdockview/")

DockWrapWidget::DockWrapWidget(QWidget* _w) : w(_w) {
    QVBoxLayout* l = new QVBoxLayout();
    l->setMargin(0);
    l->setSpacing(0);
    setLayout(l);
    l->addWidget(w);
    setWindowTitle(w->windowTitle());
    setWindowIcon(w->windowIcon());
//    setAttribute(Qt::WA_DeleteOnClose);
}

DockWrapWidget::~DockWrapWidget() {
    w = NULL; //for breakpoint
}

MWDockManagerImpl::MWDockManagerImpl(MainWindowImpl* _mw) 
: MWDockManager(_mw), mwImpl(_mw), mw(_mw->getQMainWindow())
{
	for (int i=0;i<MWDockArea_MaxDocks;i++) {
        activeDocks[i] = NULL;
        toggleDockState[i] = NULL;
    }
    
    mw->setDockOptions(QMainWindow::AnimatedDocks);

    //prepare dock-toolbars
    dockLeft = new QToolBar("left_dock_bar", mw);
    dockLeft->setObjectName("left_dock_bar");
    dockLeft->setMovable(false);
    dockLeft->setFloatable(false);
    dockLeft->hide();
    mw->addToolBar(Qt::LeftToolBarArea, dockLeft);

    dockRight = new QToolBar("right_dock_bar",mw);
    dockRight->setObjectName("right_dock_bar");
    dockRight->setMovable(false);
    dockRight->setFloatable(false);
    dockRight->hide();
    mw->addToolBar(Qt::RightToolBarArea, dockRight);

    dockBottom= new QToolBar("bottom_dock_bar", mw);
    dockBottom->setObjectName("bottom_dock_bar");
    dockBottom->setMovable(false);
    dockBottom->setFloatable(false);
    mw->addToolBar(Qt::BottomToolBarArea, dockBottom);

    statusBarAction = dockBottom->addWidget(new TaskStatusBar());
    
    readLastActiveDocksState();

    QAction* tga = new QAction(this);
    tga->setShortcut(QKeySequence("Alt+`"));
    tga->setShortcutContext(Qt::ApplicationShortcut);
    connect(tga, SIGNAL(triggered()), SLOT(sl_toggleDocks()));
    mw->addAction(tga);

    mainWindowIsHidden = false;
    mw->installEventFilter(this);

}

MWDockManagerImpl::~MWDockManagerImpl() {
    saveLastActiveDocksState();
    foreach(DockData* d, docks) {
        destroyDockData(d);
    }
}

void MWDockManagerImpl::readLastActiveDocksState() {
    Settings* s = AppContext::getSettings();
    for (int i=0;i<MWDockArea_MaxDocks; i++) {
        lastActiveDocksState[i] = s->getValue(DOCK_SETTINGS + "dockTitle"+i).toString();
    }
}

void MWDockManagerImpl::saveLastActiveDocksState() {
    Settings* s = AppContext::getSettings();
    for (int i=0;i<MWDockArea_MaxDocks; i++) {
        s->setValue(DOCK_SETTINGS + "dockTitle"+i, lastActiveDocksState[i]);
    }
} 

QToolBar* MWDockManagerImpl::getDockBar(MWDockArea a) const {
    switch(a) {
        case MWDockArea_Left: return dockLeft;
        case MWDockArea_Right: return dockRight;
        case MWDockArea_Bottom: return dockBottom;  
        default: break;
    }
    return NULL;
}

static bool ksInUse(const QKeySequence& ks, const QList<DockData*>& docks) {
    foreach(DockData* d, docks) {
        if (d->action!=NULL && d->action->shortcut() == ks) {
            return true;
        }
    }
    return false;
}

QAction* MWDockManagerImpl::registerDock(MWDockArea area, QWidget* w, const QKeySequence& ks) {
    bool showDock = w->objectName() == lastActiveDocksState[area];

    QToolBar* tb = getDockBar(area);
    DockData* data = new DockData();
    data->area = area;
    data->label = new QLabel(tb);
    data->wrapWidget = new DockWrapWidget(w);
    data->wrapWidget->setObjectName("wrap_widget_"+w->objectName());
    data->label->setObjectName("doc_lable_"+w->objectName());
    data->label->installEventFilter(this);
    if (area != MWDockArea_Bottom) {
        tb->addWidget(data->label);
    } else {
        tb->insertWidget(statusBarAction, data->label);
    }
    connect(w, SIGNAL(destroyed()), SLOT(sl_widgetDestroyed()));

    QString ttip = w->windowTitle();
    if (!ks.isEmpty() && !ksInUse(ks, docks)) {
        data->action = new QAction(data->label);
        data->action->setShortcut(ks);
        data->action->setShortcutContext(Qt::ApplicationShortcut);
        connect(data->action, SIGNAL(triggered()), SLOT(sl_toggleDock()));
        data->label->addAction(data->action);
        ttip+=" ("+ks.toString()+")";
    }
    data->label->setToolTip(ttip);

    DockWidgetPainter::updateLabel(data, false);

    docks.append(data);
    
    
    if (tb->isHidden()) {
        tb->show();
    }
    
    if (showDock) {
        openDock(data);
    } else {
        data->wrapWidget->setVisible(false);
    }
    return NULL;
}

QWidget* MWDockManagerImpl::toggleDock(const QString& widgetObjName) {
    DockData* d = findDockByName(widgetObjName);
    if (d!=NULL) { 
        toggleDock(d);
        return d->wrapWidget->w;
    }
    return NULL;

}

void MWDockManagerImpl::sl_toggleDock() {
    QAction* a = qobject_cast<QAction*>(sender());
    QLabel* l = qobject_cast<QLabel*>(a->parent());
    DockData* d = findDockByLabel(l);
    toggleDock(d);
}

void MWDockManagerImpl::updateTB(MWDockArea a) {
    if (a == MWDockArea_Bottom) {
        return; //bottom TB is always on;
    }

    int nChilds = 0;
    foreach (DockData* d, docks) {
        if (d->area == a) {
            nChilds++;
        }
    }
    QToolBar* tb = getDockBar(a);
    if (nChilds == 0 && tb->isVisible()) {
        tb->hide();
    } else if (nChilds != 0 && tb->isHidden()) {
        tb->show();
    }
}

QWidget* MWDockManagerImpl::findWidget(const QString& widgetObjName) {
    DockData* d = findDockByName(widgetObjName);
    return d == NULL ? NULL : d->wrapWidget->w;
}

QWidget* MWDockManagerImpl::getActiveWidget(MWDockArea a) {
    DockData* d = getActiveDock(a);
    return d == NULL ? NULL : d->wrapWidget->w;
}


DockData* MWDockManagerImpl::getActiveDock(MWDockArea a) const {
	DockData* d = activeDocks[a];
	assert(d == NULL || d->dock!=NULL);
    assert(d == NULL || d->wrapWidget != NULL);
	return d;
}

QWidget* MWDockManagerImpl::activateDock(const QString& widgetObjName) {
    DockData* d = findDockByName(widgetObjName);
    if (d!=NULL) {
        openDock(d);
        return d->wrapWidget->w;
    }
    return NULL;
}


void MWDockManagerImpl::openDock(DockData* d) {
	//check if already opened
	if (getActiveDock(d->area)==d) {
		return;
	}
    assert(d->wrapWidget->isHidden());
	
	//hide active dock if exists
	DockData* activeDock = getActiveDock(d->area);
	if (activeDock!=NULL) {
        closeDock(activeDock);
	}
	assert(getActiveDock(d->area) == NULL);

	//open new dock
    assert(d->wrapWidget != NULL);
    DockWidgetPainter::updateLabel(d, true);
    restoreDockGeometry(d);
	d->dock = new QDockWidget();
    d->dock->setObjectName("mw_docArea");
	d->dock->setFeatures(QDockWidget::DockWidgetClosable);
    d->dock->setWidget(d->wrapWidget);
    d->dock->setWindowTitle(d->wrapWidget->windowTitle());
    d->dock->setWindowIcon(d->wrapWidget->windowIcon());
	activeDocks[d->area] = d;
	connect(d->dock, SIGNAL(visibilityChanged(bool)), SLOT(sl_dockVisibilityChanged(bool)));

    Qt::DockWidgetArea mwarea = d->area == MWDockArea_Left ? Qt::LeftDockWidgetArea : 
        d->area == MWDockArea_Right ? Qt::RightDockWidgetArea : Qt::BottomDockWidgetArea;
    
    d->dock->setAttribute(Qt::WA_DeleteOnClose);

	mw->addDockWidget(mwarea, d->dock);
	lastActiveDocksState[d->area] = d->wrapWidget->w->objectName();
}

void MWDockManagerImpl::closeDock(DockData* d) {
    activeDocks[d->area] = NULL;
    if (d->wrapWidget!=NULL) { //widget is closed manually by user ->detach it from its parent to avoid deletion on d->dock->close();
        DockWidgetPainter::updateLabel(d, false);
        saveDockGeometry(d);
        lastActiveDocksState[d->area].clear(); 
        d->wrapWidget->setParent(NULL);
        d->wrapWidget->setVisible(false);
    } 
    d->dock->close(); // will delete dock widget because of Qt::WA_DeleteOnClose
	d->dock = NULL;
}

void MWDockManagerImpl::destroyDockData(DockData* d) {
    if (d->dock!=NULL) {
        saveDockGeometry(d);
    }
    delete d->label;
    d->wrapWidget->deleteLater();
    d->wrapWidget = NULL;
    if (d->dock!=NULL) {
        d->dock->close();
    } 
    docks.removeOne(d);
    updateTB(d->area);
    delete d;
}

void MWDockManagerImpl::sl_dockVisibilityChanged(bool visible) {
    if (visible) {
		return; 
	}
    if (mw->isMinimized() || mainWindowIsHidden) {
        return;
    }
    QDockWidget* dock = qobject_cast<QDockWidget*>(sender());
    DockData* dd = findDockByDockWidget(dock);
    assert(dd != NULL);
	closeDock(dd);
}

void MWDockManagerImpl::sl_widgetDestroyed() {
    QWidget* w = qobject_cast<QWidget*>(sender());
    foreach(DockData* d, docks) {
        if (d->wrapWidget->w == w) {
            destroyDockData(d);
            break;
        }
    }
}

DockData* MWDockManagerImpl::findDockByName(const QString& objName) const  {
    foreach(DockData* d, docks) {
        if (d->wrapWidget->w->objectName() == objName) {
            return d;
        }
    }
    return NULL;
}

DockData* MWDockManagerImpl::findDockByLabel(QLabel* l) const  {
    foreach(DockData* d, docks) {
        if (d->label == l) {
            return d;
        }
    }
    return NULL;
}


DockData* MWDockManagerImpl::findDockByDockWidget(QDockWidget* dock) const  {
    foreach(DockData* d, docks) {
        if (d->dock == dock) {
            return d;
        }
    }
    return NULL;
}

bool MWDockManagerImpl::eventFilter(QObject *obj, QEvent *event) {

    if (obj == mw) {
        if (event->type() == QEvent::Hide ) {
            mainWindowIsHidden = true;
        } else if (event->type() == QEvent::Show) {
            mainWindowIsHidden = false;
        } else {
            return false;
        }
    }

    if (event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent* me = (QMouseEvent*)event;
        if (me->button() == Qt::LeftButton) {
            QLabel *label = qobject_cast<QLabel *>(obj);
            SAFE_POINT(NULL != label, "Can't cast obj to QLabel *", false);
            DockData* data = findDockByLabel(label);
            assert(data);
            if (!data) {
                return false;
            }
            toggleDock(data);
        }
    }

    return false;
}

void MWDockManagerImpl::toggleDock(DockData* d) {
    if (d->dock!=NULL) {
        closeDock(d);
    } else {
        openDock(d);
    }
}

void MWDockManagerImpl::dontActivateNextTime(MWDockArea a) {
    lastActiveDocksState[a] = "";
}


void MWDockManagerImpl::saveDockGeometry(DockData* dd) {
	const QString& id = dd->wrapWidget->w->objectName();
    const QSize& size = dd->wrapWidget->w->size();
	Settings* s = AppContext::getSettings();
	s->setValue(DOCK_SETTINGS+id+"/size", size);
}

void MWDockManagerImpl::restoreDockGeometry(DockData* dd) {
    Settings* s = AppContext::getSettings();
    dd->wrapWidget->hint= s->getValue(DOCK_SETTINGS+dd->wrapWidget->w->objectName()+"/size", QSize(300, 200)).toSize();
}

void MWDockManagerImpl::sl_toggleDocks()
{
    bool isOpen = false;
    for(int i = 0; i < MWDockArea_MaxDocks; i++) {
        if (activeDocks[i] != NULL) {
            isOpen = true;
            break;
        }
    }
    if (isOpen) {
        for(int i = 0; i < MWDockArea_MaxDocks; i++) {
            if ((toggleDockState[i] = activeDocks[i]) != NULL) {
                closeDock(activeDocks[i]);
            }
        }
    } else {
        for(int i = 0; i < MWDockArea_MaxDocks; i++) {
            if (toggleDockState[i]) {
                openDock(toggleDockState[i]);
            }
        }
    }
}

} //namespace
