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

#ifndef _U2_WORKFLOW_PALETTE_H_
#define _U2_WORKFLOW_PALETTE_H_

#include <U2Lang/ActorModel.h>
#include <U2Lang/ActorPrototypeRegistry.h>

#include <ui_PaletteWidget.h>

#include <QAction>
#include <QTreeWidget>

namespace U2 {
using namespace Workflow;
class ExternalProcessConfig;
class NameFilterLayout;
class WorkflowView;
class WorkflowScene;
class WorkflowPaletteElements;

class WorkflowPalette : public QWidget, Ui_PaletteWidget
{
    Q_OBJECT

public:
    static const QString MIME_TYPE;

    WorkflowPalette(ActorPrototypeRegistry* reg, SchemaConfig* schemaConfig, QWidget *parent = 0);
    QMenu * createMenu(const QString &name);
    void createMenu(QMenu *menu);

    QVariant saveState() const;
    void restoreState(const QVariant&);

    QString createPrototype();
    bool editPrototype(ActorPrototype *proto);

public slots:
    void resetSelection();

signals:
    void processSelected(Workflow::ActorPrototype*, bool);
    void si_prototypeIsAboutToBeRemoved(Workflow::ActorPrototype *proto);
    void si_protoChanged();
    void si_protoListModified();
private:
    NameFilterLayout *nameFilter;
    WorkflowPaletteElements *elementsList;
    friend class PaletteDelegate;
};

class WorkflowPaletteElements : public QTreeWidget {
    Q_OBJECT

public:

    WorkflowPaletteElements(ActorPrototypeRegistry* reg, SchemaConfig* schemaConfig, QWidget *parent = 0);
    QMenu * createMenu(const QString &name);
    void createMenu(QMenu *menu);

    QVariant saveState() const;
    void restoreState(const QVariant&);

    QString createPrototype();
    bool editPrototype(ActorPrototype *proto);

public slots:
    void resetSelection();
    void sl_nameFilterChanged(const QString &filter);

signals:
    void processSelected(Workflow::ActorPrototype*, bool putToScene);
    void si_prototypeIsAboutToBeRemoved(Workflow::ActorPrototype *proto);
    void si_protoChanged();
    void si_protoListModified();

protected:
    void contextMenuEvent(QContextMenuEvent *e);

    void mouseMoveEvent(QMouseEvent* event);
    void mousePressEvent ( QMouseEvent * event );
    void leaveEvent ( QEvent * event );

private slots:
    void handleItemAction();
    void sl_selectProcess(bool checked);
    void rebuild();
    void editElement();
    bool removeElement();
    void sl_prototypeIsAboutToBeRemoved(Workflow::ActorPrototype *proto);

private:
    QTreeWidgetItem* createItemWidget(QAction* a);
    QAction* createItemAction(Workflow::ActorPrototype* item);
    QAction *getActionByProto(Workflow::ActorPrototype *proto) const;
    void setContent(ActorPrototypeRegistry*);
    void sortTree();
    QVariant changeState(const QVariant& v);
    void removePrototype(Workflow::ActorPrototype *proto);
    bool editPrototypeWithoutElementRemoving(Workflow::ActorPrototype* proto, ExternalProcessConfig* newConfig);
    void replaceConfigFiles(Workflow::ActorPrototype* proto, ExternalProcessConfig* newConfig);
    void replaceOldConfigWithNewConfig(ExternalProcessConfig* oldConfig, ExternalProcessConfig* newConfig);
    bool isExclusivePrototypeUsage(ActorPrototype* proto) const;

private:
    QMap<QString,QList<QAction*> > categoryMap;
    QMap<QAction*, QTreeWidgetItem*> actionMap;
    QTreeWidgetItem *overItem;
    QAction* currentAction;
    QPoint dragStartPosition;
    QString nameFilter;
    QString oldNameFilter;

    ActorPrototypeRegistry *protoRegistry;
    QVariantMap expandState;
    SchemaConfig* schemaConfig;

    friend class PaletteDelegate;
};


}//namespace

Q_DECLARE_METATYPE(QAction *)
Q_DECLARE_METATYPE(U2::Workflow::ActorPrototype *)

#endif
