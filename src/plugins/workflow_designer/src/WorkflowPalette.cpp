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

#include <QDrag>
#include <QMenu>
#include <QMessageBox>

#include <U2Core/QObjectScopedPointer.h>

#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Lang/IncludedProtoFactory.h>
#include <U2Lang/QueryDesignerRegistry.h>
#include <U2Lang/WorkflowContext.h>
#include <U2Lang/WorkflowSettings.h>

#include "CreateScriptWorker.h"
#include "WorkflowPalette.h"
#include "WorkflowSamples.h"
#include "WorkflowViewController.h"
#include "library/ExternalProcessWorker.h"
#include "library/IncludedProtoFactoryImpl.h"
#include "library/ScriptWorker.h"
#include "library/create_cmdline_based_worker/CreateCmdlineBasedWorkerWizard.h"
#include "util/CustomWorkerUtils.h"

namespace U2 {

const QString WorkflowPalette::MIME_TYPE("application/x-ugene-workflow-id");

WorkflowPalette::WorkflowPalette(ActorPrototypeRegistry* reg, SchemaConfig* schemaConfig, QWidget *parent)
: QWidget(parent)
{
    setupUi(this);
    nameFilter = new NameFilterLayout(NULL);
    elementsList = new WorkflowPaletteElements(reg, schemaConfig, this);
    setFocusPolicy(Qt::NoFocus);
    setMouseTracking(true);

    QVBoxLayout *vl = dynamic_cast<QVBoxLayout*>(layout());
    vl->addLayout(nameFilter);
    vl->addWidget(elementsList);

    connect(elementsList, SIGNAL(processSelected(Workflow::ActorPrototype*, bool)), SIGNAL(processSelected(Workflow::ActorPrototype*, bool)));
    connect(elementsList, SIGNAL(si_prototypeIsAboutToBeRemoved(Workflow::ActorPrototype *)), SIGNAL(si_prototypeIsAboutToBeRemoved(Workflow::ActorPrototype *)));
    connect(elementsList, SIGNAL(si_protoChanged()), SIGNAL(si_protoChanged()));
    connect(elementsList, SIGNAL(si_protoListModified()), SIGNAL(si_protoListModified()));

    connect(nameFilter->getNameEdit(), SIGNAL(textChanged(const QString &)), elementsList, SLOT(sl_nameFilterChanged(const QString &)));
    setObjectName("palette");
    setFocusProxy(nameFilter->getNameEdit());
}

QMenu* WorkflowPalette::createMenu(const QString &name) {
    return elementsList->createMenu(name);
}

void WorkflowPalette::createMenu(QMenu *menu) {
    elementsList->createMenu(menu);
}

void WorkflowPalette::resetSelection() {
    elementsList->resetSelection();
}

QVariant WorkflowPalette::saveState() const {
    return elementsList->saveState();
}

void WorkflowPalette::restoreState(const QVariant& v) {
    elementsList->restoreState(v);
}

QString WorkflowPalette::createPrototype() {
    return elementsList->createPrototype();
}

bool WorkflowPalette::editPrototype(ActorPrototype *proto) {
    return elementsList->editPrototype(proto);
}

class PaletteDelegate: public QItemDelegate {
public:
    PaletteDelegate(WorkflowPaletteElements *view) : QItemDelegate(view), m_view(view){}

    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual QSize sizeHint(const QStyleOptionViewItem &opt, const QModelIndex &index) const;

private:
    WorkflowPaletteElements *m_view;
};

void PaletteDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    const QAbstractItemModel *model = index.model();
    Q_ASSERT(model);

    if (!model->parent(index).isValid()) {
        // this is a top-level item.
        QStyleOptionButton buttonOption;

        buttonOption.state = option.state;
#ifdef Q_OS_MAC
        buttonOption.state |= QStyle::State_Raised;
#endif
        buttonOption.state &= ~QStyle::State_HasFocus;

        buttonOption.rect = option.rect;
        buttonOption.palette = option.palette;
        buttonOption.features = QStyleOptionButton::None;
        m_view->style()->drawControl(QStyle::CE_PushButton, &buttonOption, painter, m_view);

        QStyleOptionViewItem branchOption;
        static const int i = 9; // ### hardcoded in qcommonstyle.cpp
        QRect r = option.rect;
        branchOption.rect = QRect(r.left() + i/2, r.top() + (r.height() - i)/2, i, i);
        branchOption.palette = option.palette;
        branchOption.state = QStyle::State_Children;

        if (m_view->isExpanded(index))
            branchOption.state |= QStyle::State_Open;

        m_view->style()->drawPrimitive(QStyle::PE_IndicatorBranch, &branchOption, painter, m_view);

        // draw text
        QRect textrect = QRect(r.left() + i*2, r.top(), r.width() - ((5*i)/2), r.height());
        QString text = elidedText(option.fontMetrics, textrect.width(), Qt::ElideMiddle,
            model->data(index, Qt::DisplayRole).toString());
        m_view->style()->drawItemText(painter, textrect, Qt::AlignCenter,
            option.palette, m_view->isEnabled(), text);

    } else {
        QStyleOptionToolButton buttonOption;
        buttonOption.state = option.state;
        buttonOption.state &= ~QStyle::State_HasFocus;
        buttonOption.direction = option.direction;
        buttonOption.rect = option.rect;
        buttonOption.font = option.font;
        buttonOption.fontMetrics = option.fontMetrics;
        buttonOption.palette = option.palette;
        buttonOption.subControls = QStyle::SC_ToolButton;
        buttonOption.features = QStyleOptionToolButton::None;

        QAction* action = index.data(Qt::UserRole).value<QAction*>();
        buttonOption.text = action->text();
        buttonOption.icon = action->icon();
        if (!buttonOption.icon.isNull()) {
            buttonOption.iconSize = QSize(22, 22);
        }
        if (action->isChecked()) {
            buttonOption.state |= QStyle::State_On;
            buttonOption.state |= QStyle::State_Sunken;
            buttonOption.activeSubControls = QStyle::SC_ToolButton;
        } else {
            buttonOption.state |= QStyle::State_Raised;
            buttonOption.activeSubControls = QStyle::SC_None;
        }

        if (m_view->overItem == m_view->itemFromIndex(index)) {
            buttonOption.state |= QStyle::State_MouseOver;
        }

        buttonOption.state |= QStyle::State_AutoRaise;

        buttonOption.toolButtonStyle = Qt::ToolButtonTextBesideIcon;
        m_view->style()->drawComplexControl(QStyle::CC_ToolButton, &buttonOption, painter, m_view);

        //QItemDelegate::paint(painter, option, index);
    }
}

QSize PaletteDelegate::sizeHint(const QStyleOptionViewItem &opt, const QModelIndex &index) const
{
    const QAbstractItemModel *model = index.model();
    Q_ASSERT(model);

    QStyleOptionViewItem option = opt;
    bool top = !model->parent(index).isValid();
    QSize sz = QItemDelegate::sizeHint(opt, index) + QSize(top?2:20, top?2:20);
    return sz;
}

/************************************************************************/
/* WorkflowPaletteElements */
/************************************************************************/
WorkflowPaletteElements::WorkflowPaletteElements(ActorPrototypeRegistry* reg, SchemaConfig* _schemaConfig, QWidget *parent)
: QTreeWidget(parent), overItem(NULL), currentAction(NULL), protoRegistry(reg), schemaConfig(_schemaConfig)
{
    setFocusPolicy(Qt::NoFocus);
    setSelectionMode(QAbstractItemView::NoSelection);
    setItemDelegate(new PaletteDelegate(this));
    setRootIsDecorated(false);
    //setAnimated(true);
    setMouseTracking(true);
    setColumnCount(1);
    header()->hide();
    header()->setSectionResizeMode(QHeaderView::Stretch);

    //setTextElideMode (Qt::ElideMiddle);
    setContent(reg);
    connect(reg, SIGNAL(si_registryModified()), SLOT(rebuild()));
    connect(this, SIGNAL(si_prototypeIsAboutToBeRemoved(Workflow::ActorPrototype *)), SLOT(sl_prototypeIsAboutToBeRemoved(Workflow::ActorPrototype *)));
    this->setObjectName("WorkflowPaletteElements");
}

QMenu * WorkflowPaletteElements::createMenu(const QString &name) {
    QMenu *menu = new QMenu(name, this);
    createMenu(menu);
    return menu;
}

#define MENU_ACTION_MARKER  QString("menu-action")

void WorkflowPaletteElements::createMenu(QMenu *menu) {
    menu->clear();
    QMenu *dataSink = NULL, *dataSource = NULL, *userScript = NULL, *externalTools = NULL;
    QAction *firstAction = NULL;
    QMapIterator<QString, QList<QAction*> > it(categoryMap);
    while (it.hasNext()) {
        it.next();
        QMenu* grpMenu = new QMenu(it.key(), menu);
        QMap<QString, QAction *> map;
        foreach(QAction* a, it.value()) {
            map[a->text()] = a;
        }
        QMapIterator<QString, QAction *> jt(map);
        while(jt.hasNext()) {
            jt.next();
            QAction* elementAction = jt.value();
            QAction* menuAction = new QAction(elementAction->icon(), elementAction->text(), elementAction);
            menuAction->setData(MENU_ACTION_MARKER);
            connect(menuAction, SIGNAL(triggered(bool)), SLOT(sl_selectProcess(bool)));
            grpMenu->addAction(menuAction);
        }
        if(it.key() == BaseActorCategories::CATEGORY_DATASRC().getDisplayName()) {
            dataSource = grpMenu;
        }else if (it.key() == BaseActorCategories::CATEGORY_DATASINK().getDisplayName()) {
            dataSink = grpMenu;
        } else if (it.key() == BaseActorCategories::CATEGORY_SCRIPT().getDisplayName()) {
            userScript = grpMenu;
        } else if (it.key() == BaseActorCategories::CATEGORY_EXTERNAL().getDisplayName()) {
            externalTools = grpMenu;
        } else {
            QAction * a = menu->addMenu(grpMenu);
            firstAction = firstAction ? firstAction : a;
        }
    }

    if (NULL != dataSource) {
        menu->insertMenu(firstAction, dataSource);
    }
    if (NULL != dataSink) {
        menu->insertMenu(firstAction, dataSink);
    }
    if (userScript) {
        menu->addMenu(userScript);
    }
    if (externalTools) {
        menu->addMenu(externalTools);
    }
}

void WorkflowPaletteElements::setContent(ActorPrototypeRegistry* reg) {
    QMapIterator<Descriptor, QList<ActorPrototype*> > it(reg->getProtos());
    categoryMap.clear();
    actionMap.clear();
    while (it.hasNext()) {
        it.next();
        QTreeWidgetItem* category = NULL;

        foreach(ActorPrototype* proto, it.value()) {
            QString name = proto->getDisplayName();
            if (!NameFilterLayout::filterMatched(nameFilter, name) &&
                !NameFilterLayout::filterMatched(nameFilter, it.key().getDisplayName())) {
                continue;
            }
            if (NULL == category) {
                category = new QTreeWidgetItem(this);
                category->setText(0, it.key().getDisplayName());
                category->setData(0, Qt::UserRole, it.key().getId());
                addTopLevelItem(category);
            }
            QAction* action = createItemAction(proto);

            int i = 0;
            while(category->child(i)) {
                QString s1 = category->child(i)->data(0,Qt::UserRole).value<QAction* >()->text();
                QString s2 = action->text();

                if(QString::compare(s1,s2, Qt::CaseInsensitive) > 0) {
                    categoryMap[it.key().getDisplayName()] << action;
                    category->insertChild(i,createItemWidget(action));
                    break;
                }
                i++;
            }
            if(!category->child(i)) {
                categoryMap[it.key().getDisplayName()] << action;
                category->addChild(createItemWidget(action));
            }
        }
    }
    sortTree();
}

void WorkflowPaletteElements::rebuild() {
    setMouseTracking(false);
    resetSelection();
    ActorPrototypeRegistry* reg = qobject_cast<ActorPrototypeRegistry*>(sender());
    if (!reg) {
        reg = protoRegistry;
    }

    if (reg) {
        QVariant saved = saveState();
        overItem = nullptr;
        clear();
        setContent(reg);
        QVariant changed = changeState(saved);
        restoreState(changed);
    }

    setMouseTracking(true);
    emit si_protoListModified();
}

void WorkflowPaletteElements::sortTree() {
    sortItems(0,Qt::AscendingOrder);
    int categoryIdx = 0;

    QString text = BaseActorCategories::CATEGORY_DATASRC().getDisplayName();
    QTreeWidgetItem *item;
    if(!findItems(text,Qt::MatchExactly).isEmpty()) {
        item = findItems(text,Qt::MatchExactly).first();
        takeTopLevelItem(indexFromItem(item).row());
        insertTopLevelItem(categoryIdx,item);
        categoryIdx++;
    }

    text = BaseActorCategories::CATEGORY_DATASINK().getDisplayName();
    if(!findItems(text,Qt::MatchExactly).isEmpty()) {
        item = findItems(text,Qt::MatchExactly).first();
        takeTopLevelItem(indexFromItem(item).row());
        insertTopLevelItem(categoryIdx,item);
        categoryIdx++;
    }

    text = BaseActorCategories::CATEGORY_DATAFLOW().getDisplayName();
    if (!findItems(text, Qt::MatchExactly).isEmpty())
    {
        item = findItems(text, Qt::MatchExactly).first();
        if (item) {
            takeTopLevelItem(indexFromItem(item).row());
            insertTopLevelItem(categoryIdx,item);
            categoryIdx++;
        }
    }

    text = BaseActorCategories::CATEGORY_SCRIPT().getDisplayName();
    if(!findItems(text,Qt::MatchExactly).isEmpty()) {
        item = findItems(text,Qt::MatchExactly).first();
        if(item) {
            takeTopLevelItem(indexFromItem(item).row());
            addTopLevelItem(item);
        }
    }

    text = BaseActorCategories::CATEGORY_EXTERNAL().getDisplayName();
    if (!findItems(text, Qt::MatchExactly).isEmpty())
    {
        item = findItems(text, Qt::MatchExactly).first();
        if (item)
        {
            takeTopLevelItem(indexFromItem(item).row());
            addTopLevelItem(item);
        }
    }
}

QTreeWidgetItem* WorkflowPaletteElements::createItemWidget(QAction *a) {
    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setToolTip(0, a->toolTip());
    item->setData(0, Qt::UserRole, QVariant::fromValue(a));
    actionMap[a] = item;
    connect(a, SIGNAL(triggered()), SLOT(handleItemAction()));
    connect(a, SIGNAL(toggled(bool)), SLOT(handleItemAction()));

    return item;
}

QAction* WorkflowPaletteElements::createItemAction(ActorPrototype* item) {
    QAction* a = new QAction(item->getDisplayName(), this);
    a->setToolTip(item->getDocumentation());
    a->setCheckable(true);
    if (item->getIcon().isNull()) {
        item->setIconPath(":workflow_designer/images/green_circle.png");
    }
    a->setIcon(item->getIcon());
    a->setData(QVariant::fromValue(item));
    connect(a, SIGNAL(triggered(bool)), SLOT(sl_selectProcess(bool)));
    connect(a, SIGNAL(toggled(bool)), SLOT(sl_selectProcess(bool)));
    return a;
}

QAction *WorkflowPaletteElements::getActionByProto(Workflow::ActorPrototype *proto) const {
    foreach (QAction *action, actionMap.keys()) {
        if (proto == action->data().value<ActorPrototype *>()) {
            return action;
        }
    }
    return nullptr;
}

void WorkflowPaletteElements::resetSelection() {
    if (currentAction) {
        currentAction->setChecked(false);
        currentAction = NULL;
    }
}

QVariant WorkflowPaletteElements::saveState() const {
    QVariantMap m = expandState;
    for (int i = 0, count = topLevelItemCount(); i < count; ++i) {
        QTreeWidgetItem* it = topLevelItem(i);
        m.insert(it->data(0, Qt::UserRole).toString(), it->isExpanded());
    }
    return m;
}

void WorkflowPaletteElements::restoreState(const QVariant& v) {
    expandState = v.toMap();
    QMapIterator<QString,QVariant> it(expandState);
    while (it.hasNext())
    {
        it.next();
        for (int i = 0; i < topLevelItemCount(); i++) {
            if (topLevelItem(i)->data(0, Qt::UserRole) == it.key()) {
                topLevelItem(i)->setExpanded(it.value().toBool());
                break;
            }
        }
    }
}

QString WorkflowPaletteElements::createPrototype() {
    QObjectScopedPointer<CreateCmdlineBasedWorkerWizard> dlg = new CreateCmdlineBasedWorkerWizard(schemaConfig,  this);
    dlg->exec();
    CHECK(!dlg.isNull(), QString());

    if (dlg->result() == QDialog::Accepted) {
        QScopedPointer<ExternalProcessConfig> cfg(dlg->takeConfig());
        CreateCmdlineBasedWorkerWizard::saveConfig(cfg.data());
        if (LocalWorkflow::ExternalProcessWorkerFactory::init(cfg.data())) {
            const QString id = cfg->id;
            cfg.take();
            return id;
        }
    }
    return QString();
}

bool WorkflowPaletteElements::editPrototype(ActorPrototype *proto) {
    if (!isExclusivePrototypeUsage(proto)) {
        QMessageBox::warning(this,
            tr("Unable to Edit Element"),
            tr("The element with external tool is used in other Workflow Designer window(s). "
            "Please remove these instances to be able to edit the element configuration."),
            QMessageBox::Ok);
        return false;
    }
    ExternalProcessConfig *oldCfg = WorkflowEnv::getExternalCfgRegistry()->getConfigById(proto->getId());
    QObjectScopedPointer<CreateCmdlineBasedWorkerWizard> dlg = new CreateCmdlineBasedWorkerWizard(schemaConfig, oldCfg, this);
    dlg->exec();
    CHECK(!dlg.isNull(), false);

    bool result = false;
    if (dlg->result() == QDialog::Accepted) {
        QScopedPointer<ExternalProcessConfig> newCfg(dlg->takeConfig());

        if (CreateCmdlineBasedWorkerWizard::isRequiredToRemoveElementFromScene(oldCfg, newCfg.data())) {
            removePrototype(proto);
            CreateCmdlineBasedWorkerWizard::saveConfig(newCfg.data());
            if (LocalWorkflow::ExternalProcessWorkerFactory::init(newCfg.data())) {
                newCfg.take();
                result = true;
            }
        } else {
            result = editPrototypeWithoutElementRemoving(proto, newCfg.take());
        }
    }
    if (result) {
        emit si_protoChanged();
    }

    return result;
}

void WorkflowPaletteElements::handleItemAction() {
    QAction* a = qobject_cast<QAction*>(sender());
    assert(a);
    assert(actionMap[a]);
    if (a) {
        update(indexFromItem(actionMap[a]));
    }
}

void WorkflowPaletteElements::sl_selectProcess(bool checked) {
    if (currentAction && currentAction != sender()) {
        currentAction->setChecked(false);
    }

    QAction * senderAction= qobject_cast<QAction*>(sender());
    bool fromMenu = false;
    if (senderAction->data() == MENU_ACTION_MARKER) {
        fromMenu = true;
        currentAction = qobject_cast<QAction*>(senderAction->parent());
    } else if (checked) {
        currentAction = senderAction;
    } else {
        currentAction = NULL;
    }
    if (currentAction) {
        Workflow::ActorPrototype* actor =  currentAction->data().value<Workflow::ActorPrototype*>();
        emit processSelected(actor, fromMenu);
    }

}

void WorkflowPaletteElements::editElement() {
    ActorPrototype *proto = currentAction->data().value<ActorPrototype *>();
    ActorPrototypeRegistry *reg = WorkflowEnv::getProtoRegistry();
    QMap<Descriptor, QList<ActorPrototype*> > categories = reg->getProtos();

    if(categories.value(BaseActorCategories::CATEGORY_SCRIPT()).contains(proto)) {
        QString oldName = proto->getDisplayName();
        QObjectScopedPointer<CreateScriptElementDialog> dlg = new CreateScriptElementDialog(this, proto);
        dlg->exec();
        CHECK(!dlg.isNull(), );

        if (dlg->result() == QDialog::Accepted) {
            ActorPrototypeRegistry *reg = WorkflowEnv::getProtoRegistry();
            assert(reg);

            QList<DataTypePtr > input = dlg->getInput();
            QList<DataTypePtr > output = dlg->getOutput();
            QList<Attribute*> attrs = dlg->getAttributes();
            QString name = dlg->getName();
            QString desc = dlg->getDescription();

            if(oldName != name) {
                removeElement();
            } else {
                emit si_prototypeIsAboutToBeRemoved(proto);
                reg->unregisterProto(proto->getId());
            }
            LocalWorkflow::ScriptWorkerFactory::init(input, output, attrs, name, desc, dlg->getActorFilePath());
        }
    } else { //External process category
            editPrototype(proto);
    }
}

bool WorkflowPaletteElements::removeElement() {
    QObjectScopedPointer<QMessageBox> msg = new QMessageBox(this);
    msg->setObjectName("Remove element");
    msg->setWindowTitle("Remove element");
    msg->setText("Remove this element?");
    msg->addButton(QMessageBox::Ok);
    msg->addButton(QMessageBox::Cancel);
    msg->exec();
    CHECK(!msg.isNull(), false);

    if (msg->result() == QMessageBox::Cancel) {
        return false;
    }

    removePrototype(currentAction->data().value<ActorPrototype *>());
    return true;
}

void WorkflowPaletteElements::sl_prototypeIsAboutToBeRemoved(ActorPrototype *proto) {
    QAction *action = getActionByProto(proto);

    for (auto &actionsList : categoryMap) {
        actionsList.removeAll(action);
    }

    if (currentAction == action) {
        resetSelection();
    }

    actionMap.remove(action);
}

void WorkflowPaletteElements::contextMenuEvent(QContextMenuEvent *e)
{
    QMenu menu;
    menu.addAction(tr("Expand all"), this, SLOT(expandAll()));
    menu.addAction(tr("Collapse all"), this, SLOT(collapseAll()));
    if(itemAt(e->pos()) && itemAt(e->pos())->parent() && (itemAt(e->pos())->parent()->text(0) == BaseActorCategories::CATEGORY_SCRIPT().getDisplayName()
        || itemAt(e->pos())->parent()->text(0) == BaseActorCategories::CATEGORY_EXTERNAL().getDisplayName())) {
            menu.addAction(tr("Edit"), this, SLOT(editElement()));
            menu.addAction(tr("Remove"), this, SLOT(removeElement()));
            currentAction = actionMap.key(itemAt(e->pos()));
    }
    e->accept();
    menu.exec(mapToGlobal(e->pos()));
}

void WorkflowPaletteElements::mouseMoveEvent(QMouseEvent * event) {
    if (!hasMouseTracking()) return;
    if ((event->buttons() & Qt::LeftButton) && !dragStartPosition.isNull()) {
        if ((event->pos() - dragStartPosition).manhattanLength() <= QApplication::startDragDistance()) return;
        QTreeWidgetItem* item = itemAt(event->pos());
        if (!item) return;
        QAction* action = item->data(0, Qt::UserRole).value<QAction*>();
        if (!action) return;
        ActorPrototype* proto = action->data().value<ActorPrototype*>();
        assert(proto);
        QMimeData* mime = new QMimeData();
        mime->setData(WorkflowPalette::MIME_TYPE, proto->getId().toLatin1());
        mime->setText(proto->getId());
        QDrag *drag = new QDrag(this);
        drag->setMimeData(mime);
        drag->setPixmap(action->icon().pixmap(QSize(44, 44)));

        resetSelection();
        dragStartPosition = QPoint();
        Qt::DropAction dropAction = drag->exec(Qt::CopyAction, Qt::CopyAction);
        Q_UNUSED(dropAction);
        return;
    }
    QTreeWidgetItem* prev = overItem;
    overItem = itemAt(event->pos());
    if (prev) {
        update(indexFromItem(prev));
    }
    if (overItem) {
        update(indexFromItem(overItem));
    }

    QTreeWidget::mouseMoveEvent(event);
}

void WorkflowPaletteElements::mousePressEvent(QMouseEvent * event) {
    if (!hasMouseTracking()) return;
    dragStartPosition = QPoint();
    if ((event->buttons() & Qt::LeftButton)) {
        QTreeWidgetItem* item = itemAt(event->pos());
        if (!item) return;
        event->accept();
        if (item->parent() == 0) {
            setItemExpanded(item, !isItemExpanded(item));
            return;
        }

        QAction* action = item->data(0, Qt::UserRole).value<QAction*>();
        if (action) {
            action->toggle();
            dragStartPosition = event->pos();
        }
    }
}

void WorkflowPaletteElements::leaveEvent(QEvent *) {
    if (!hasMouseTracking()) {
        return;
    }
    QTreeWidgetItem* prev = overItem;
    overItem = NULL;
    if (prev) {
        QModelIndex index = indexFromItem(prev);
        update(index);
    };
}

QVariant WorkflowPaletteElements::changeState(const QVariant& savedState){
    QVariantMap m = savedState.toMap();

    for (int i = 0, count = topLevelItemCount(); i < count; ++i) {
        QTreeWidgetItem* it = topLevelItem(i);
        bool expanded = m.value(it->data(0, Qt::UserRole).toString()).toBool();

        QRegExp nonWhitespase("\\s");
        QStringList splitNew = nameFilter.split(nonWhitespase, QString::SkipEmptyParts);
        bool hasCharsNewFilter = splitNew.size() > 0 && !splitNew.first().isEmpty();
        QStringList splitOld = oldNameFilter.split(nonWhitespase, QString::SkipEmptyParts);
        bool hasCharsOldFilter = splitOld.size() > 0 && !splitOld.first().isEmpty();

        if(hasCharsNewFilter && !hasCharsOldFilter){expanded = true;}
        else if(!hasCharsNewFilter && hasCharsOldFilter){expanded = false;}

        m.insert(it->data(0, Qt::UserRole).toString(), expanded);
    }
    return m;
}

void WorkflowPaletteElements::removePrototype(ActorPrototype *proto) {
    if (!isExclusivePrototypeUsage(proto)) {
        QMessageBox::warning(this,
            tr("Unable to Remove Element"),
            tr("The element with external tool is used in other Workflow Designer window(s). "
                "Please remove these instances to be able to remove the element configuration."),
            QMessageBox::Yes);
        return;
    }
    emit si_prototypeIsAboutToBeRemoved(proto);

    if (!QFile::remove(proto->getFilePath())) {
        uiLog.error(tr("Can't remove element '%1'").arg(proto->getDisplayName()));
    }

    delete IncludedProtoFactory::unregisterExternalToolWorker(proto->getId());
    delete WorkflowEnv::getProtoRegistry()->unregisterProto(proto->getId());
}

bool WorkflowPaletteElements::editPrototypeWithoutElementRemoving(Workflow::ActorPrototype* proto, ExternalProcessConfig* newConfig) {
    replaceConfigFiles(proto, newConfig);

    ExternalProcessConfig* currentConfig = IncludedProtoFactory::getExternalToolWorker(proto->getId());
    SAFE_POINT(nullptr != currentConfig, "ExternalProcessConfig is absent", false);

    replaceOldConfigWithNewConfig(currentConfig, newConfig);

    proto->setDisplayName(currentConfig->name);
    proto->setDocumentation(currentConfig->description);

    QStringList commandIdList = CustomWorkerUtils::getToolIdsFromCommand(currentConfig->cmdLine);
    proto->clearExternalTools();
    foreach(const QString & id, commandIdList) {
        proto->addExternalTool(id);
    }

    rebuild();

    return true;
}

void WorkflowPaletteElements::replaceConfigFiles(Workflow::ActorPrototype* proto, ExternalProcessConfig* newConfig) {
    if (!QFile::remove(proto->getFilePath())) {
        uiLog.error(tr("Can't remove element '%1'").arg(proto->getDisplayName()));
    }
    CreateCmdlineBasedWorkerWizard::saveConfig(newConfig);
    proto->setNonStandard(newConfig->filePath);
}

void WorkflowPaletteElements::replaceOldConfigWithNewConfig(ExternalProcessConfig* oldConfig, ExternalProcessConfig* newConfig) {
    oldConfig->cmdLine = newConfig->cmdLine;
    oldConfig->name = newConfig->name;
    oldConfig->description = newConfig->description;
    oldConfig->templateDescription = newConfig->templateDescription;
    oldConfig->filePath = newConfig->filePath;
    oldConfig->useIntegratedTool = newConfig->useIntegratedTool;
    oldConfig->customToolPath = newConfig->customToolPath;
    oldConfig->integratedToolId = newConfig->integratedToolId;
}

bool WorkflowPaletteElements::isExclusivePrototypeUsage(ActorPrototype* proto) const {
    WorkflowView* wv = dynamic_cast<WorkflowView*>(schemaConfig);
    CHECK(wv != nullptr, false);
    int actorWithCurrentProtoCounter = 0;
    for (auto actor : wv->getSchema()->getProcesses()) {
        if (actor->getProto() == proto) {
            actorWithCurrentProtoCounter++;
        }
    }
    Actor* currentActor = wv->getActor();
    if (currentActor != nullptr && currentActor->getProto() == proto) {
        actorWithCurrentProtoCounter++;
    }
    bool result = actorWithCurrentProtoCounter == proto->getUsageCounter();
    return result;
}

void WorkflowPaletteElements::sl_nameFilterChanged(const QString &filter) {
    overItem = NULL;
    oldNameFilter = nameFilter;
    nameFilter = filter.toLower();
    rebuild();
}

}//namespace
