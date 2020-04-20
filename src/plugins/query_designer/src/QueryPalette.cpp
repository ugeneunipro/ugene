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

#include <QAction>
#include <QApplication>
#include <QDrag>
#include <QHeaderView>
#include <QItemDelegate>
#include <QMouseEvent>

#include <U2Core/AppContext.h>
#include <U2Lang/QueryDesignerRegistry.h>

#include "QueryPalette.h"

namespace U2 {

const QString QDDistanceIds::E2S(QObject::tr("End-Start"));
const QString QDDistanceIds::S2E(QObject::tr("Start-End"));
const QString QDDistanceIds::E2E(QObject::tr("End-End"));
const QString QDDistanceIds::S2S(QObject::tr("Start-Start"));

class PaletteDelegate: public QItemDelegate
{
public:
    PaletteDelegate(QueryPalette *view) : QItemDelegate(view), m_view(view) {}

    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
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

            QAction* action = index.data(Qt::UserRole).value<QAction *>();
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

    virtual QSize sizeHint(const QStyleOptionViewItem &opt, const QModelIndex &index) const {
        const QAbstractItemModel *model = index.model();
        Q_ASSERT(model);

        QStyleOptionViewItem option = opt;
        bool top = !model->parent(index).isValid();
        QSize sz = QItemDelegate::sizeHint(opt, index) + QSize(top?2:20, top?2:20);
        return sz;
    }
private:
    QueryPalette *m_view;
};

const QString QueryPalette::MIME_TYPE("application/x-ugene-query-id");

QueryPalette::QueryPalette(QWidget* parent/* =NULL */)
: QTreeWidget(parent), overItem(NULL), currentAction(NULL) {
    setFocusPolicy(Qt::NoFocus);
    setSelectionMode(QAbstractItemView::NoSelection);
    setItemDelegate(new PaletteDelegate(this));
    setRootIsDecorated(false);
    setColumnCount(1);
    header()->hide();
    header()->setSectionResizeMode(QHeaderView::Stretch);
    setMouseTracking(true);
    setContent();
    setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Ignored));
}

void QueryPalette::setContent() {
    QTreeWidgetItem* algorithmCategory = new QTreeWidgetItem;
    algorithmCategory->setText(0, tr("Algorithms"));
    addTopLevelItem(algorithmCategory);
    algorithmCategory->setExpanded(true);


    QDActorPrototypeRegistry* qpr = AppContext::getQDActorProtoRegistry();
    foreach(QDActorPrototype* pf, qpr->getAllEntries()) {
        QAction* action = createItemAction(pf);
        QTreeWidgetItem* algMenuItem = new QTreeWidgetItem(/*algorithmCategory*/);
        actionMap[action] = algMenuItem;
        algMenuItem->setText(0, action->text());
        algMenuItem->setData(0, Qt::UserRole, qVariantFromValue(action));
        algorithmCategory->addChild(algMenuItem);
    }

    QTreeWidgetItem* constraintCategory = new QTreeWidgetItem;
    constraintCategory->setText(0, tr("Constraints"));
    addTopLevelItem(constraintCategory);
    constraintCategory->setExpanded(true);

    QList<QAction*> constraintItemActions;
    constraintItemActions << createItemAction(QDDistanceIds::E2S)
        << createItemAction(QDDistanceIds::S2E)
        << createItemAction(QDDistanceIds::E2E)
        << createItemAction(QDDistanceIds::S2S);

    foreach(QAction* a, constraintItemActions) {
        QTreeWidgetItem* linkMenuItem = new QTreeWidgetItem(constraintCategory);
        actionMap[a] = linkMenuItem;
        linkMenuItem->setText(0, a->text());
        linkMenuItem->setData(0, Qt::UserRole, qVariantFromValue(a));
        constraintCategory->addChild(linkMenuItem);
    }
}

QAction* QueryPalette::createItemAction(QDActorPrototype* item) {
    QAction* a = new QAction(item->getDisplayName(), this);
    a->setCheckable(true);
    if (!item->getIcon().isNull()) {
        a->setIcon(item->getIcon());
    }
    else {
        QIcon icon(":query_designer/images/green_circle.png");
        a->setIcon(icon);
    }
    a->setData(qVariantFromValue(item));
    connect(a, SIGNAL(triggered(bool)), SLOT(sl_selectProcess(bool)));
    connect(a, SIGNAL(toggled(bool)), SLOT(sl_selectProcess(bool)));
    return a;
}

QAction* QueryPalette::createItemAction(const QString& constraintId) {
    QAction* a = new QAction(constraintId, this);
    a->setCheckable(true);
    QIcon icon(":query_designer/images/green_circle.png");
    a->setIcon(icon);
    a->setData(qVariantFromValue(constraintId));
    connect(a, SIGNAL(triggered(bool)), SLOT(sl_selectProcess(bool)));
    connect(a, SIGNAL(toggled(bool)), SLOT(sl_selectProcess(bool)));
    return a;
}

void QueryPalette::sl_selectProcess(bool checked) {
    if (currentAction && currentAction!=sender()) {
        currentAction->setChecked(false);
    }
    if (!checked) {
        update(indexFromItem(actionMap.value(currentAction)));
        currentAction = NULL;
    }
    else {
        currentAction = qobject_cast<QAction*>(sender());
        assert(currentAction);
    }

    if (checked && currentAction && currentAction->data().type() != QVariant::String) {
        emit processSelected(currentAction->data().value<QDActorPrototype *>());
    } else {
        emit processSelected(NULL);
    }
}

void QueryPalette::mousePressEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
        QTreeWidgetItem* item = itemAt(event->pos());
        if (!item) return;
        event->accept();
        if (item->parent() == 0) {
            setItemExpanded(item, !isItemExpanded(item));
            return;
        }
        QAction* action = item->data(0, Qt::UserRole).value<QAction *>();
        if (action) {
            action->toggle();
            dragStartPosition = event->pos();
            update(indexFromItem(actionMap.value(action)));
        }
        return;
    }
}

void QueryPalette::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
        if ((event->pos()-dragStartPosition).manhattanLength()<QApplication::startDragDistance()) {
            return;
        }
        QPointF pos = event->pos();
        QTreeWidgetItem* item = itemAt(pos.toPoint());
        if(!item) {
            return;
        }
        QAction* action = item->data(0, Qt::UserRole).value<QAction *>();
        if (!action) {
            return;
        }

        QDrag *drag = new QDrag(this);
        QMimeData *mimeData = new QMimeData;
        if (action->data().type() == QVariant::String) {
            QString str = action->data().toString();
            mimeData->setText(str);
        } else {
            QDActorPrototype* proto = action->data().value<QDActorPrototype *>();
            mimeData->setText(proto->getId());
        }

        drag->setMimeData(mimeData);
        drag->exec(Qt::CopyAction);
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

void QueryPalette::leaveEvent(QEvent *) {
    QTreeWidgetItem* prev = overItem;
    overItem = NULL;
    if (prev) {
        QModelIndex index = indexFromItem(prev);
        update(index);
    };
}

QVariant QueryPalette::saveState() const {
    QVariantList l;
    for (int i = 0, count = topLevelItemCount(); i < count; i++) {
        QTreeWidgetItem* it = topLevelItem(i);
        l.append(it->isExpanded());
    }
    return l;
}

void QueryPalette::restoreState(const QVariant& v) {
    const QVariantList& l = v.toList();
    for(int i=0, n=l.size(); i<n; i++) {
        topLevelItem(i)->setExpanded(l.at(i).toBool());
    }
}

void QueryPalette::resetSelection() {
    if (currentAction) {
        currentAction->setChecked(false);
        currentAction = NULL;
    }
}

}//namespace
