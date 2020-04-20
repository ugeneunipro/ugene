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

#ifndef _DOCK_MANAGER_IMPL_H_
#define _DOCK_MANAGER_IMPL_H_

#include <QIcon>

#include <U2Gui/MainWindow.h>

class QLabel;

namespace U2 {

class MainWindowImpl;

//used to save/restore geometry of the dock
class DockWrapWidget : public QWidget {
public:
    DockWrapWidget(QWidget* w);
    ~DockWrapWidget();

    virtual QSize sizeHint() const {return hint.isNull() ? w->sizeHint() : hint;}
    QWidget* w;
    QSize hint;
};

class DockData {
public:
    DockData() {area = MWDockArea_Left; label = NULL; dock = NULL; wrapWidget = NULL; action = NULL;}
    MWDockArea          area;
    QLabel*             label;
    QDockWidget*        dock;
    DockWrapWidget*     wrapWidget;
    QIcon               dockIcon;
    QAction*            action;
};

class MWDockManagerImpl : public MWDockManager {
    Q_OBJECT
public:
    MWDockManagerImpl(MainWindowImpl* _mw);
    ~MWDockManagerImpl();

    virtual QAction* registerDock(MWDockArea area, QWidget* w, const QKeySequence& ks = QKeySequence());

    virtual QWidget* findWidget(const QString& widgetObjName);

    virtual QWidget* getActiveWidget(MWDockArea a);

    virtual QWidget* activateDock(const QString& widgetObjName);

    virtual QWidget* toggleDock(const QString& widgetObjName);

    virtual void dontActivateNextTime(MWDockArea a);

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private slots:
    void sl_dockVisibilityChanged(bool);
    void sl_widgetDestroyed();
    void sl_toggleDock();
    void sl_toggleDocks();
    
private:
	
    QToolBar* getDockBar(MWDockArea a) const;
    DockData* findDockByLabel(QLabel* l) const ;
    DockData* findDockByName(const QString& objName) const ;
    DockData* findDockByDockWidget(QDockWidget* d) const ;
    void toggleDock(DockData* d);
    void closeDock(DockData* d);
    void openDock(DockData* d);
    void destroyDockData(DockData* d);
    
    void readLastActiveDocksState();
    void saveLastActiveDocksState();
	void saveDockGeometry(DockData* dd);
    void restoreDockGeometry(DockData* dd);
	
    void updateTB(MWDockArea a);
	
    DockData* getActiveDock(MWDockArea area) const;
    
    MainWindowImpl* mwImpl;
    QMainWindow*    mw;
    DockData*       activeDocks[MWDockArea_MaxDocks];
    DockData*       toggleDockState[MWDockArea_MaxDocks];

    QList<DockData*>    docks;

    QToolBar* dockLeft;
    QToolBar* dockRight;
    QToolBar* dockBottom;
    QAction*  statusBarAction;

    // This variable is required in order to avoid unnessesary closing of dock widgets.
    // It is controlled with the eventFilter function.
    bool mainWindowIsHidden;

    QString lastActiveDocksState[MWDockArea_MaxDocks];
};

}//namespace

#endif
