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

#ifndef _U2_MDI_MANAGER_IMPL_H_
#define _U2_MDI_MANAGER_IMPL_H_

#include "MainWindowImpl.h"

#include <QSignalMapper>
#include <QEvent>

#include <QMdiArea>
#include <QMdiSubWindow>

namespace U2 {

class MDIItem : QWidget { 
	Q_OBJECT
public:
    MDIItem(MWMDIWindow* _w, QMdiSubWindow* _qw) : w(_w), qw(_qw){this->setObjectName(_w->objectName()+"_MDIItem");qw->setObjectName(_w->objectName()+"_SubWindow");}
	
	MWMDIWindow* w;
	QMdiSubWindow* qw;
};

typedef QList<MDIItem*> MDIItems;

class MWMDIManagerImpl : public MWMDIManager {
	Q_OBJECT
public:
    MWMDIManagerImpl(MainWindow* _mw, FixedMdiArea* _mdiArea) 
		: MWMDIManager(_mw), mw(_mw), mdiArea(_mdiArea){ prepareGUI();}

	~MWMDIManagerImpl();

	virtual void addMDIWindow(MWMDIWindow* w);

	virtual bool closeMDIWindow(MWMDIWindow* w);

	virtual QList<MWMDIWindow*> getWindows() const;

	virtual MWMDIWindow* getWindowById(int id) const;

	virtual void activateWindow(MWMDIWindow* w);
	
	virtual MWMDIWindow* getActiveWindow() const;

protected:
	bool eventFilter(QObject *obj, QEvent *event);
	
private slots:
	void sl_setActiveSubWindow(QWidget *);
	void sl_updateWindowMenu();
	void sl_onSubWindowActivated(QMdiSubWindow *);
    void sl_updateWindowLayout();
	void sl_setWindowLayoutToMultiDoc();
	void sl_setWindowLayoutToTabbed();

private:
	MDIItem* getMDIItem(int id) const;
	MDIItem* getMDIItem(MWMDIWindow* w) const;
	MDIItem* getMDIItem(QMdiSubWindow* qw) const;

	void clearMDIContent(bool addCloseAction);
	void prepareGUI();

	void updateActions();
	void updateState();

	MDIItem* getCurrentMDIItem() const ;

    void onWindowsSwitched(QMdiSubWindow *deactivated, MWMDIWindow *activated);

	MainWindow* mw;
	FixedMdiArea*	mdiArea;
	MDIItems	items;


	QAction* closeAct;
	QAction* closeAllAct;
	QAction* tileAct;
	QAction* cascadeAct;
	QAction* nextAct;
	QAction* previousAct;
	QAction* separatorAct;
    QMenu*   windowLayout;
    QAction* multipleDocsAct;
    QAction* tabbedDocsAct;

	QSignalMapper*  windowMapper;
	bool            defaultIsMaximized;
	MDIItem*        mdiContentOwner;
};

} //namespace

#endif
