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

#include "ToolBarManager.h"

#include <QEvent>

namespace U2 {

MWToolBarManagerImpl::MWToolBarManagerImpl(QMainWindow* _mw) : QObject(_mw), mw(_mw) {
	QToolBar* tb = createToolBar(MWTOOLBAR_MAIN);
    tb->setToolButtonStyle(Qt::ToolButtonIconOnly);
	createToolBar(MWTOOLBAR_ACTIVEMDI);
}

MWToolBarManagerImpl::~MWToolBarManagerImpl() {
}


bool MWToolBarManagerImpl::eventFilter(QObject *obj, QEvent *event) {
	QEvent::Type t = event->type();
	if (t == QEvent::ActionAdded || t == QEvent::ActionRemoved) { 
#ifdef Q_OS_WIN
        // mega-hack -> update all the area below toolbar by extra show()/hide() for .NET style 
        // if OpenGL widget present (-> WA_NativeWindow is set)
        // if not done .NET style will leave artifacts on toolbar
        QToolBar* tb = qobject_cast<QToolBar*>(obj);
        tb->hide();
#endif
        updateToolbarVisibilty();
	}
	return QObject::eventFilter(obj, event);
}


QToolBar* MWToolBarManagerImpl::getToolbar(const QString& sysName) const {
	foreach(QToolBar* tb, toolbars) {
		if (tb->objectName() == sysName) {
			return tb;
		}
	}
	return NULL;
}

QToolBar* MWToolBarManagerImpl::createToolBar(const QString& sysName) {
	QToolBar* tb = new QToolBar(mw);
	//tb->setMinimumHeight(28);
	tb->setObjectName(sysName);
	tb->setAllowedAreas(Qt::TopToolBarArea);
	//tb->setMovable(false);
	
	tb->installEventFilter(this);
	toolbars.append(tb);
	mw->addToolBar(tb);

	return tb;
}

void MWToolBarManagerImpl::updateToolbarVisibilty() {
	foreach(QToolBar* tb, toolbars) {
		bool empty = tb->actions().empty();
		bool visible = tb->isVisible();
		if (empty && visible) {
			tb->hide();
		} else if (!empty && !visible) {
			tb->show();
		}
	}
}

}//namespace
