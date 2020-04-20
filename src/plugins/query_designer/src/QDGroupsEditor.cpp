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

#include <QHeaderView>
#include <QInputDialog>
#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>

#include <U2Lang/QDScheme.h>

#include "QueryViewController.h"
#include "QDGroupsEditor.h"

namespace U2 {

QDGroupsEditor::QDGroupsEditor( QueryViewController* p ) : view(p) {
    header()->hide();

    setColumnCount(2);

    initActions();

    rebuild();

    connect(view->getScheme(), SIGNAL(si_schemeChanged()), SLOT(rebuild()));
}

void QDGroupsEditor::initActions() {
    addGroupAction = new QAction(tr("Add Group"), this);
    connect(addGroupAction, SIGNAL(triggered(bool)), SLOT(sl_addGroup()));

    removeGroupAction = new QAction(tr("Remove Group"), this);
    connect(removeGroupAction, SIGNAL(triggered(bool)), SLOT(sl_removeGroup()));

    addActorAction = new QAction(tr("Add Element"), this);
    connect(addActorAction, SIGNAL(triggered(bool)), SLOT(sl_addActor()));

    removeActorAction = new QAction(tr("Remove Element"), this);
    connect(removeActorAction, SIGNAL(triggered(bool)), SLOT(sl_removeActor()));

    setGroupReqNumAction = new QAction(tr("Set Required Number"), this);
    connect(setGroupReqNumAction, SIGNAL(triggered(bool)), SLOT(sl_setReqNum()));
}

void QDGroupsEditor::rebuild() {
    clear();
    QDScheme* scheme = view->getScheme();
    foreach(const QString& group, scheme->getActorGroups()) {
        QStringList grpItemTexts;
        int grpSize = scheme->getActors(group).size();
        int reqNum = grpSize ? scheme->getRequiredNumber(group) : 0;
        QString countLbl = QString("%1/%2")
            .arg(reqNum)
            .arg(grpSize);
        grpItemTexts << group << countLbl;
        QTreeWidgetItem* groupItem = new QTreeWidgetItem(this, grpItemTexts);
        addTopLevelItem(groupItem);
        foreach(QDActor const* actor, scheme->getActors(group)) {
            const QString& actorLabel = actor->getParameters()->getLabel();
            /*QTreeWidgetItem* actorItem = */new QTreeWidgetItem(groupItem, QStringList(actorLabel));
        }
    }
}

void QDGroupsEditor::mousePressEvent( QMouseEvent *event ) {
    if (event->buttons()&Qt::RightButton) {
        QTreeWidgetItem* item = currentItem();// = itemAt(event->pos());
        QMenu m;
        if (!item) {
            m.addAction(addGroupAction);
        } else if (item->parent()){
            m.addAction(addActorAction);
            m.addAction(removeActorAction);
        } else {
            m.addAction(addActorAction);
            m.addAction(addGroupAction);
            m.addAction(removeGroupAction);
            m.addAction(setGroupReqNumAction);
        }
        m.exec(event->globalPos());
    }
    QTreeWidget::mousePressEvent(event);
}

void QDGroupsEditor::sl_addGroup() {
    QDScheme* scheme = view->getScheme();
    bool ok;
    QString text = QInputDialog::getText(this, tr("Create element group"), tr("Group name"), QLineEdit::Normal, QString(), &ok);
    if (!ok) {
        return;
    }
    if (scheme->getActorGroups().contains(text)) {
        QMessageBox::critical(NULL, QObject::tr("Query Designer"), tr("Group '%1' already exists!").arg(text));
        return;
    }
    if (!scheme->validateGroupName(text)) {
        QMessageBox::critical(NULL, QObject::tr("Query Designer"), tr("Invalid group name!"));
        return;
    }
    scheme->createActorGroup(text);
}

void QDGroupsEditor::sl_removeGroup() {
    assert(!currentItem()->parent());
    view->getScheme()->removeActorGroup(currentItem()->text(0));
}

void QDGroupsEditor::sl_addActor() {
    QDScheme* scheme = view->getScheme();
    QTreeWidgetItem* item = currentItem();
    assert(item);
    if (item->parent()) {
        item = item->parent();
    }
    QString group = item->text(0);

    QStringList list;
    foreach(QDActor* a, scheme->getActors()) {
        list << a->getParameters()->getLabel();
    }

    if (list.isEmpty()) {
        return;
    }

    bool ok;
    QString label = QInputDialog::getItem(this, tr("Add Element To Group '%1'").arg(group), tr("Element:"), list, 0, false, &ok);
    if (!ok) {
        return;
    }

    QDActor* sel = scheme->getActorByLabel(label);
    assert(sel);

    if (!scheme->getActorGroup(sel).isEmpty()) {
        QMessageBox::critical(this, QObject::tr("Query Designer"), tr("Actor is already in group!"));
        return;
    }
    scheme->addActorToGroup(sel, group);
    //item->setExpanded(true);
}

void QDGroupsEditor::sl_removeActor() {
    QDScheme* scheme = view->getScheme();
    assert(currentItem());
    assert(currentItem()->parent());
    QDActor* sel = scheme->getActorByLabel(currentItem()->text(0));
    scheme->removeActorFromGroup(sel);
}

void QDGroupsEditor::sl_setReqNum() {
    QDScheme* scheme = view->getScheme();
    assert(currentItem());
    assert(!currentItem()->parent());
    QString group = currentItem()->text(0);

    bool ok;
    int num = QInputDialog::getInt(this,
        tr("Set required number for '%1'").arg(group),
        tr("Number:"), 1, 1, scheme->getActors(group).size(), 1, &ok);

    if (ok) {
        scheme->setRequiredNum(group, num);
    }
}

}//namespace
