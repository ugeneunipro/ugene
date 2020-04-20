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

#ifndef _U2_QD_GROUPS_EDITOR_H_
#define _U2_QD_GROUPS_EDITOR_H_

#include <QTreeWidget>

namespace U2 {

class QueryViewController;

class QDGroupsEditor : public QTreeWidget {
    Q_OBJECT
public:
    QDGroupsEditor(QueryViewController* p);
protected:
    virtual void mousePressEvent(QMouseEvent *event);
private slots:
    void sl_addGroup();
    void sl_removeGroup();
    void sl_addActor();
    void sl_removeActor();
    void sl_setReqNum();
    void rebuild();
private:
    void initActions();
private:
    QueryViewController* view;

    QAction* addGroupAction;
    QAction* removeGroupAction;
    QAction* addActorAction;
    QAction* removeActorAction;
    QAction* setGroupReqNumAction;
};

}//namespace

#endif
