/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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

static QPoint getTreeItemClickPoint(QTreeWidget *treeWidget, QTreeWidgetItem *item) {
    QPoint itemCenter = treeWidget->visualItemRect(item).center();
    itemCenter.setY(itemCenter.y() + treeWidget->visualItemRect(item).height() + 5);  //+ height because of header item; +5 because height is not enough
    return treeWidget->mapToGlobal(itemCenter);
}

#define GT_METHOD_NAME "waitTaskFinished"
void GTUtilsTaskTreeView::waitTaskFinished(HI::GUITestOpStatus &os, long timeoutMillis) {
    TaskScheduler *scheduler = AppContext::getTaskScheduler();
    if (scheduler->getTopLevelTasks().isEmpty() && !GTThread::isMainThread()) {
        // Give QT a chance to process all events first.
        // The result of this processing may be new tasks we will wait for.
        GTThread::waitForMainThread();
    }

    // Wait up to 'timeoutMillis' for all tasks to finish.
    for (int time = 0; time < timeoutMillis && !scheduler->getTopLevelTasks().isEmpty(); time += GT_OP_CHECK_MILLIS) {
        GTGlobals::sleep(GT_OP_CHECK_MILLIS);
    }

    GT_CHECK_RESULT(scheduler->getTopLevelTasks().isEmpty(),
                    "waitTaskFinished failed, there are active tasks: " + getTasksInfo(scheduler->getTopLevelTasks(), 0), );
}
#undef GT_METHOD_NAME

QString GTUtilsTaskTreeView::getTasksInfo(QList<Task *> tasks, int level) {
    QString result;
    QMap<Task::State, QString> stateMap;
    stateMap.insert(Task::State_New, "State_New");
    stateMap.insert(Task::State_Prepared, "State_Prepared");
    stateMap.insert(Task::State_Running, "State_Running");
    stateMap.insert(Task::State_Finished, "State_Finished");
    foreach (Task *t, tasks) {
        for (int i = 0; i < level; i++) {
            result.append("  ");
        }
        result.append(QString("%1:  %2\n").arg(t->getTaskName()).arg(stateMap.value(t->getState())));
        result.append(getTasksInfo(t->getPureSubtasks(), level + 1));
    }
    return result;
}

QTreeWidget *GTUtilsTaskTreeView::openView(HI::GUITestOpStatus &os) {
    QTreeWidget *taskTreeView = GTWidget::findExactWidget<QTreeWidget *>(os, widgetName, nullptr, {false});
    if (taskTreeView == nullptr) {
        toggleView(os);
        taskTreeView = GTWidget::findExactWidget<QTreeWidget *>(os, widgetName);
    }
    return taskTreeView;
}

void GTUtilsTaskTreeView::toggleView(HI::GUITestOpStatus & /*os*/) {
    GTKeyboardDriver::keyClick('2', Qt::AltModifier);
}

#define GT_METHOD_NAME "getTreeWidgetItem"
QTreeWidgetItem *GTUtilsTaskTreeView::getTreeWidgetItem(HI::GUITestOpStatus &os, const QString &itemName, bool failOnNull) {
    QTreeWidget *treeWidget = getTreeWidget(os);
    GT_CHECK_RESULT(treeWidget != nullptr, "Tree widget not found", nullptr);

    QTreeWidgetItem *item = getTreeWidgetItem(treeWidget, itemName);
    if (failOnNull) {
        GT_CHECK_RESULT(item != nullptr, "Item " + itemName + " not found in tree widget", nullptr);
    }

    return item;
}
#undef GT_METHOD_NAME

QTreeWidgetItem *GTUtilsTaskTreeView::getTreeWidgetItem(QTreeWidget *tree, const QString &itemName) {
    if (itemName.isEmpty()) {
        return nullptr;
    }

    QList<QTreeWidgetItem *> treeItems = getTaskTreeViewItems(tree->invisibleRootItem());
    foreach (QTreeWidgetItem *item, treeItems) {
        QString treeItemName = item->text(0);
        if (treeItemName == itemName) {
            return item;
        }
    }

    return nullptr;
}

QTreeWidget *GTUtilsTaskTreeView::getTreeWidget(HI::GUITestOpStatus &os) {
    GTGlobals::FindOptions options;
    options.failIfNotFound = false;

    QTreeWidget *treeWidget = static_cast<QTreeWidget *>(GTWidget::findWidget(os, widgetName, nullptr, options));
    return treeWidget;
}

QList<QTreeWidgetItem *> GTUtilsTaskTreeView::getTaskTreeViewItems(QTreeWidgetItem *root, bool recursively) {
    QList<QTreeWidgetItem *> treeItems;

    for (int i = 0; i < root->childCount(); i++) {
        treeItems.append(root->child(i));
        if (recursively) {
            treeItems.append(getTaskTreeViewItems(root->child(i)));
        }
    }

    return treeItems;
}
#define GT_METHOD_NAME "cancelTask"
void GTUtilsTaskTreeView::cancelTask(HI::GUITestOpStatus &os, const QString &itemName, bool failIfNotFound) {
    auto treeWidget = openView(os);
    auto item = getTreeWidgetItem(os, itemName, failIfNotFound);
    if (item == nullptr) {
        return;
    }
    QPoint itemCoordinate = getTreeItemClickPoint(treeWidget, item);
    GTMouseDriver::moveTo(itemCoordinate);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Cancel task"}, GTGlobals::UseMouse));
    GTMouseDriver::click(Qt::RightButton);

    checkTaskWithWait(os, itemName, false);
}
#undef GT_METHOD_NAME

void GTUtilsTaskTreeView::click(HI::GUITestOpStatus &os, const QString &itemName, Qt::MouseButton b) {
    moveTo(os, itemName);
    GTMouseDriver::click(b);
}

void GTUtilsTaskTreeView::moveTo(HI::GUITestOpStatus &os, const QString &itemName) {
    openView(os);
    moveToOpenedView(os, itemName);
}

#define GT_METHOD_NAME "getTopLevelTasksCount"
int GTUtilsTaskTreeView::getTopLevelTasksCount(HI::GUITestOpStatus &os) {
    openView(os);

    QTreeWidget *treeWidget = getTreeWidget(os);
    GT_CHECK_RESULT(treeWidget != nullptr, "Tree widget not found", -1);

    return getTaskTreeViewItems(treeWidget->invisibleRootItem(), false).size();
}
#undef GT_METHOD_NAME

void GTUtilsTaskTreeView::moveToOpenedView(HI::GUITestOpStatus &os, const QString &itemName) {
    QPoint p = getTreeViewItemPosition(os, itemName);
    GTMouseDriver::moveTo(p);
}

#define GT_METHOD_NAME "getTreeViewItemPosition"
QPoint GTUtilsTaskTreeView::getTreeViewItemPosition(HI::GUITestOpStatus &os, const QString &itemName) {
    QTreeWidget *treeWidget = getTreeWidget(os);
    GT_CHECK_RESULT(treeWidget != nullptr, "treeWidget is NULL", QPoint());
    for (int time = 0; time < GT_OP_WAIT_MILLIS; time += GT_OP_CHECK_MILLIS) {
        GTGlobals::sleep(time > 0 ? GT_OP_CHECK_MILLIS : 0);
        QTreeWidgetItem *item = getTreeWidgetItem(os, itemName, false);
        if (item != nullptr) {
            return getTreeItemClickPoint(treeWidget, item);
        }
    }
    GT_FAIL("Tree item not found: " + itemName, {});
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkTask"
bool GTUtilsTaskTreeView::checkTask(HI::GUITestOpStatus &os, const QString &itemName) {
    openView(os);
    QTreeWidgetItem *item = getTreeWidgetItem(os, itemName, false);
    return item != nullptr;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkTaskWithWait"
void GTUtilsTaskTreeView::checkTaskWithWait(HI::GUITestOpStatus &os, const QString &itemName, bool checkIfPresent) {
    openView(os);
    for (int time = 0; time < GT_OP_WAIT_MILLIS; time += GT_OP_CHECK_MILLIS) {
        GTGlobals::sleep(time > 0 ? GT_OP_CHECK_MILLIS : 0);
        QTreeWidgetItem *item = getTreeWidgetItem(os, itemName, false);
        if (checkIfPresent && item != nullptr) {
            return;
        }
        if (!checkIfPresent && item == nullptr) {
            return;
        }
    }
    GT_FAIL(QString("checkTaskWithWait failed: ") + (checkIfPresent ? "Item was not found: " : "Item is present: ") + itemName, );
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "countTasks"
int GTUtilsTaskTreeView::countTasks(HI::GUITestOpStatus &os, const QString &itemName) {
    openView(os);
    int result = 0;
    QList<QTreeWidgetItem *> treeItems = getTaskTreeViewItems(getTreeWidget(os)->invisibleRootItem());
    foreach (QTreeWidgetItem *item, treeItems) {
        QString treeItemName = item->text(0);
        if (treeItemName == itemName) {
            result++;
        }
    }
    return result;
}
#undef GT_METHOD_NAME

QString GTUtilsTaskTreeView::getTaskStatus(GUITestOpStatus &os, const QString &itemName) {
    openView(os);
    GTGlobals::sleep(500);
    return getTreeWidgetItem(os, itemName)->text(1);
}

SchedulerListener::SchedulerListener()
    : QObject(nullptr),
      registeredTaskCount(0) {
    connect(AppContext::getTaskScheduler(), SIGNAL(si_topLevelTaskRegistered(Task *)), SLOT(sl_taskRegistered()));
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
