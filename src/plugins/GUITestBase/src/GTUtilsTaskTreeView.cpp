/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2026 UniPro <ugene@unipro.ru>
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

#include "GTUtilsTaskTreeView.h"
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTTreeWidget.h>
#include <primitives/GTWidget.h>

#include <QTimer>
#include <QTreeWidget>

#include <U2Core/AppContext.h>
#include <U2Core/Task.h>

#include <U2Gui/MainWindow.h>

#include "primitives/PopupChooser.h"
#include "utils/GTThread.h"

namespace U2 {
using namespace HI;

#define GT_CLASS_NAME "GTUtilsTaskTreeView"

const QString GTUtilsTaskTreeView::widgetName = DOCK_TASK_TREE_VIEW;

void GTUtilsTaskTreeView::waitTaskFinished(int timeoutMillis) {
    TaskScheduler* scheduler = AppContext::getTaskScheduler();
    QList<Task*> topLevelTasks = scheduler->getTopLevelTasks();
    if (topLevelTasks.isEmpty() && !GTThread::isMainThread()) {
        // Give QT a chance to process all events first.
        // The result of this processing may be new tasks we will wait for.
        GTThread::waitForMainThread();
        topLevelTasks = scheduler->getTopLevelTasks();
    }

    // Wait up to 'timeoutMillis' for all tasks to finish.

    for (int time = 0; time < timeoutMillis && !topLevelTasks.isEmpty(); time += GT_OP_CHECK_MILLIS) {
        GTGlobals::sleep(GT_OP_CHECK_MILLIS);
        topLevelTasks = scheduler->getTopLevelTasks();
    }
    if (!topLevelTasks.isEmpty()) {
        GT_FAIL("waitTaskFinished tasks: " + getTasksInfo(topLevelTasks, 0), );
    }
}

QString GTUtilsTaskTreeView::getTasksInfo(QList<Task*> tasks, int level) {
    QString result;
    QMap<Task::State, QString> stateMap;
    stateMap.insert(Task::State_New, "State_New");
    stateMap.insert(Task::State_Prepared, "State_Prepared");
    stateMap.insert(Task::State_Running, "State_Running");
    stateMap.insert(Task::State_Finished, "State_Finished");
    foreach (Task* t, tasks) {
        for (int i = 0; i < level; i++) {
            result.append("  ");
        }
        result.append(QString("%1:  %2\n").arg(t->getTaskName()).arg(stateMap.value(t->getState())));
        result.append(getTasksInfo(t->getPureSubtasks(), level + 1));
    }
    return result;
}

QTreeWidget* GTUtilsTaskTreeView::openView() {
    auto taskTreeView = getTreeWidget();
    if (taskTreeView == nullptr) {
        toggleView();
        taskTreeView = getTreeWidget(true);
    }
    return taskTreeView;
}

bool GTUtilsTaskTreeView::isViewOpened() {
    return getTreeWidget() != nullptr;
}

void GTUtilsTaskTreeView::toggleView() {
    GTKeyboardDriver::keyClick('2', Qt::AltModifier);
}

QTreeWidgetItem* GTUtilsTaskTreeView::getTreeWidgetItem(const QString& itemName, bool failOnNull) {
    QTreeWidget* treeWidget = getTreeWidget();
    GT_CHECK_RESULT(treeWidget != nullptr, "Tree widget not found", nullptr);
    return GTTreeWidget::findItem(treeWidget, itemName, nullptr, 0, {failOnNull});
}

QTreeWidgetItem* GTUtilsTaskTreeView::getTreeWidgetItem(QTreeWidget* tree, const QString& itemName) {
    if (itemName.isEmpty()) {
        return nullptr;
    }

    QList<QTreeWidgetItem*> treeItems = getTaskTreeViewItems(tree->invisibleRootItem());
    foreach (QTreeWidgetItem* item, treeItems) {
        QString treeItemName = item->text(0);
        if (treeItemName == itemName) {
            return item;
        }
    }

    return nullptr;
}

QTreeWidget* GTUtilsTaskTreeView::getTreeWidget(bool failIfNotFound) {
    return GTWidget::findTreeWidget(widgetName, nullptr, {failIfNotFound});
}

QList<QTreeWidgetItem*> GTUtilsTaskTreeView::getTaskTreeViewItems(QTreeWidgetItem* root, bool recursively) {
    QList<QTreeWidgetItem*> treeItems;

    for (int i = 0; i < root->childCount(); i++) {
        treeItems.append(root->child(i));
        if (recursively) {
            treeItems.append(getTaskTreeViewItems(root->child(i)));
        }
    }

    return treeItems;
}

void GTUtilsTaskTreeView::cancelTask(const QString& itemName, bool failIfNotFound, const QStringList& parentTaskNames) {
    openView();
    for (const QString& parentTaskName : qAsConst(parentTaskNames)) {
        QTreeWidgetItem* parentTaskItem = getTreeWidgetItem(parentTaskName, failIfNotFound);
        if (parentTaskName == nullptr) {
            break;
        }
        GTTreeWidget::expand(parentTaskItem);
    }
    auto item = getTreeWidgetItem(itemName, failIfNotFound);
    if (item == nullptr) {
        return;
    }
    QPoint itemCoordinate = GTTreeWidget::getItemCenter(item);
    GTMouseDriver::moveTo(itemCoordinate);

    GTUtilsDialog::waitForDialog(new PopupChooser({"Cancel task"}, GTGlobals::UseMouse));
    GTMouseDriver::click(Qt::RightButton);

    checkTaskIsPresent(itemName, false);
}

void GTUtilsTaskTreeView::click(const QString& itemName, Qt::MouseButton b) {
    moveTo(itemName);
    GTMouseDriver::click(b);
}

void GTUtilsTaskTreeView::doubleClick(const QString& itemName) {
    moveTo(itemName);
    GTMouseDriver::doubleClick();
}

void GTUtilsTaskTreeView::moveTo(const QString& itemName) {
    openView();
    moveToOpenedView(itemName);
}

int GTUtilsTaskTreeView::getTopLevelTasksCount() {
    openView();

    return getTreeWidget(true)->invisibleRootItem()->childCount();
}

void GTUtilsTaskTreeView::moveToOpenedView(const QString& itemName) {
    QPoint p = getTreeViewItemPosition(itemName);
    GTMouseDriver::moveTo(p);
}

QPoint GTUtilsTaskTreeView::getTreeViewItemPosition(const QString& itemName) {
    QTreeWidget* treeWidget = getTreeWidget();
    GT_CHECK_RESULT(treeWidget != nullptr, "treeWidget is NULL", QPoint());
    for (int time = 0; time < GT_OP_WAIT_MILLIS; time += GT_OP_CHECK_MILLIS) {
        GTGlobals::sleep(time > 0 ? GT_OP_CHECK_MILLIS : 0);
        QTreeWidgetItem* item = getTreeWidgetItem(itemName, false);
        if (item != nullptr) {
            return GTTreeWidget::getItemCenter(item);
        }
    }
    GT_FAIL("Tree item not found: " + itemName, {});
}

void GTUtilsTaskTreeView::checkTaskIsPresent(const QString& topLevelTaskName, bool checkIfPresent) {
    auto treeWidget = openView();
    GT_CHECK_RESULT(treeWidget != nullptr, "Tree widget not found", );

    for (int time = 0; time < GT_OP_WAIT_MILLIS; time += GT_OP_CHECK_MILLIS) {
        GTGlobals::sleep(time > 0 ? GT_OP_CHECK_MILLIS : 0);
        QTreeWidgetItem* item = nullptr;
        for (int i = 0; i < treeWidget->topLevelItemCount(); i++) {
            QTreeWidgetItem* candidateItem = treeWidget->topLevelItem(i);
            if (candidateItem->text(0).contains(topLevelTaskName, Qt::CaseInsensitive)) {
                item = candidateItem;
                break;
            };
        }
        if (checkIfPresent && item != nullptr) {
            return;
        }
        if (!checkIfPresent && item == nullptr) {
            return;
        }
    }
    GT_FAIL(QString("checkTaskWithWait failed: ") + (checkIfPresent ? "Item was not found: " : "Item is present: ") + topLevelTaskName, );
}

int GTUtilsTaskTreeView::countTasks(const QString& itemName) {
    openView();
    int result = 0;
    QList<QTreeWidgetItem*> treeItems = getTaskTreeViewItems(getTreeWidget()->invisibleRootItem());
    for (QTreeWidgetItem* item : qAsConst(treeItems)) {
        QString treeItemName = item->text(0);
        if (treeItemName == itemName) {
            result++;
        }
    }
    return result;
}

QString GTUtilsTaskTreeView::getTaskStatus(const QString& itemName) {
    openView();
    GTGlobals::sleep(500);
    return getTreeWidgetItem(itemName)->text(1);
}

SchedulerListener::SchedulerListener()
    : QObject(nullptr),
      registeredTaskCount(0) {
    connect(AppContext::getTaskScheduler(), SIGNAL(si_topLevelTaskRegistered(Task*)), SLOT(sl_taskRegistered()));
}

int SchedulerListener::getRegisteredTaskCount() const {
    return registeredTaskCount;
}

void SchedulerListener::reset() {
    registeredTaskCount = 0;
}

void SchedulerListener::sl_taskRegistered() {
    registeredTaskCount++;
}

#undef GT_CLASS_NAME

}  // namespace U2
