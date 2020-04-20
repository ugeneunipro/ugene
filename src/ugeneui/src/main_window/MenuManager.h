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

#ifndef _U2_MWMENU_MANAGER_IMPL_H_
#define _U2_MWMENU_MANAGER_IMPL_H_

#include <U2Gui/MainWindow.h>

#include <QMenuBar>
#include <QMenu>
#include <QEvent>

namespace U2 {

class MWMenuManagerImpl: public QObject {
    Q_OBJECT
public:
    MWMenuManagerImpl(QObject* p, QMenuBar* mb);

    QMenu* getTopLevelMenu(const QString& sysName) const;
    void setMenuBarEnabled(bool enable);
    void registerAction(QAction *action);

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private:
    void unlinkTopLevelMenu(QMenu*);
    void linkTopLevelMenu(QMenu*);
    QMenu* createTopLevelMenu(const QString& sysName, const QString& title, const QString& afterSysName = QString::null);
    void updateTopLevelMenuVisibility(QMenu* m);
    QMenuBar* menuBar;
    QList<QMenu*> toplevelMenus;
    QList<QPointer<QAction> > additionalActions;
};

}//namespace

#endif
