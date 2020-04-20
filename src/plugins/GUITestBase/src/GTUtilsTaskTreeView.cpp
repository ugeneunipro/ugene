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

#include <U2Core/global.h>
#include "GTUtilsTaskTreeView.h"
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTWidget.h>
#include "primitives/PopupChooser.h"
#include "utils/GTThread.h"

#include <U2Core/Task.h>
#include <U2Gui/MainWindow.h>
#include <U2Core/AppContext.h>
#include <QTimer>

#include <QTreeWidget>

namespace U2 {
using namespace HI;

#define GT_CLASS_NAME "GTUtilsTaskTreeView"

const QString GTUtilsTaskTreeView::widgetName = DOCK_TASK_TREE_VIEW;


void GTUtilsTaskTreeView::waitTaskFinished(HI::GUITestOpStatus &os, long timeout) {
    GTGlobals::sleep(500);
    TaskScheduler* scheduller = AppContext::getTaskScheduler();
    int i = 0;
    while(!scheduller->getTopLevelTasks().isEmpty()){
       GTGlobals::sleep(1000);
       i++;
       if(i > (timeout/1000)){
           os.setError(os.getError() + getTasksInfo(scheduller->getTopLevelTasks(),0));
           break;
       }
    }
    GTGlobals::sleep(1000);
}

QString GTUtilsTaskTreeView::getTasksInfo(QList<Task *> tasks, int level){
    QString result;
    QMap<Task::State, QString> stateMap;
    stateMap.insert(Task::State_New, "State_New");
    stateMap.insert(Task::State_Prepared, "State_Prepared");
    stateMap.insert(Task::State_Running, "State_Running");
    stateMap.insert(Task::State_Finished, "State_Finished");
    foreach(Task* t, tasks){
        for (int i=0; i<level; i++){
            result.append("  ");
        }
        result.append(QString("%1:  %2\n").arg(t->getTaskName()).arg(stateMap.value(t->getState())));
        result.append(getTasksInfo(t->getPureSubtasks(),level+1));
    }
    return result;
}
void GTUtilsTaskTreeView::openView(HI::GUITestOpStatus& os) {
    GTGlobals::FindOptions options;
    options.failIfNotFound = false;

    QWidget *documentTreeWidget = GTWidget::findWidget(os, widgetName, NULL, options);
    if (!documentTreeWidget) {
        toggleView(os);
        GTGlobals::sleep(500);
        GTThread::waitForMainThread();
    }
}

void GTUtilsTaskTreeView::toggleView(HI::GUITestOpStatus& /*os*/) {
    GTKeyboardDriver::keyClick('2', Qt::AltModifier);
}

#define GT_METHOD_NAME "getTreeWidgetItem"
QTreeWidgetItem* GTUtilsTaskTreeView::getTreeWidgetItem( HI::GUITestOpStatus &os, const QString &itemName, bool failOnNull){
    QTreeWidget *treeWidget = getTreeWidget(os);
    GT_CHECK_RESULT(treeWidget != NULL, "Tree widget not found", NULL);

    QTreeWidgetItem *item = getTreeWidgetItem(treeWidget, itemName);
    if (failOnNull) {
        GT_CHECK_RESULT(item != NULL, "Item " + itemName + " not found in tree widget", NULL);
    }

    return item;
}
#undef GT_METHOD_NAME

QTreeWidgetItem* GTUtilsTaskTreeView::getTreeWidgetItem( QTreeWidget* tree, const QString &itemName ){
    if (itemName.isEmpty()) {
        return NULL;
    }

    QList<QTreeWidgetItem*> treeItems = getTaskTreeViewItems(tree->invisibleRootItem());
    foreach (QTreeWidgetItem* item, treeItems) {
        QString treeItemName = item->text(0);
        if (treeItemName == itemName) {
            return item;
        }
    }

    return NULL;
}

QTreeWidget* GTUtilsTaskTreeView::getTreeWidget(HI::GUITestOpStatus &os) {
    GTGlobals::FindOptions options;
    options.failIfNotFound = false;

    QTreeWidget *treeWidget = static_cast<QTreeWidget*>(GTWidget::findWidget(os, widgetName, NULL, options));
    return treeWidget;
}

QList<QTreeWidgetItem*> GTUtilsTaskTreeView::getTaskTreeViewItems(QTreeWidgetItem* root, bool recursively) {

    QList<QTreeWidgetItem*> treeItems;

    for (int i=0; i<root->childCount(); i++) {
        treeItems.append(root->child(i));
        if (recursively) {
            treeItems.append(getTaskTreeViewItems(root->child(i)));
        }
    }

    return treeItems;
}

void GTUtilsTaskTreeView::cancelTask( HI::GUITestOpStatus& os, const QString &itemName ){
    Runnable *popupChooser = new PopupChooser(os, QStringList() << "Cancel task", GTGlobals::UseMouse);
    GTUtilsDialog::waitForDialog(os, popupChooser);
    click(os, itemName, Qt::RightButton);
    GTGlobals::sleep(3000);
}

void GTUtilsTaskTreeView::click( HI::GUITestOpStatus &os, const QString &itemName, Qt::MouseButton b ){
    moveTo(os, itemName);
    GTMouseDriver::click(b);
}

void GTUtilsTaskTreeView::moveTo(HI::GUITestOpStatus &os,const QString &itemName) {
    openView(os);
    moveToOpenedView(os, itemName);
}

#define GT_METHOD_NAME "getTopLevelTasksCount"
int GTUtilsTaskTreeView::getTopLevelTasksCount(HI::GUITestOpStatus &os) {
    openView(os);

    QTreeWidget *treeWidget = getTreeWidget(os);
    GT_CHECK_RESULT(treeWidget != NULL, "Tree widget not found", -1);

    return getTaskTreeViewItems(treeWidget->invisibleRootItem(), false).size();
}
#undef GT_METHOD_NAME

void GTUtilsTaskTreeView::moveToOpenedView( HI::GUITestOpStatus& os, const QString &itemName ){
    QPoint p = getTreeViewItemPosition(os, itemName);
    GTMouseDriver::moveTo(p);
}

#define GT_METHOD_NAME "getTreeViewItemPosition"
QPoint GTUtilsTaskTreeView::getTreeViewItemPosition( HI::GUITestOpStatus &os, const QString &itemName ){
    QTreeWidget *treeWidget = getTreeWidget(os);
    GT_CHECK_RESULT(treeWidget != NULL, "treeWidget is NULL", QPoint());
    QTreeWidgetItem *item = getTreeWidgetItem(os, itemName);

    QPoint p = treeWidget->rect().center();
    if (item) {
        p = treeWidget->visualItemRect(item).center();
        p.setY(p.y() + treeWidget->visualItemRect(item).height() + 5); //+ height because of header item; +5 because height is not enough
    }

    return treeWidget->mapToGlobal(p);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkTask"
bool GTUtilsTaskTreeView::checkTask(HI::GUITestOpStatus &os, const QString &itemName){
    openView(os);
    QTreeWidgetItem* item = getTreeWidgetItem(os, itemName, false);
    return item != NULL;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "countTasks"
int GTUtilsTaskTreeView::countTasks(HI::GUITestOpStatus &os, const QString &itemName){
    openView(os);
    int result = 0;
    QList<QTreeWidgetItem*> treeItems = getTaskTreeViewItems(getTreeWidget(os)->invisibleRootItem());
    foreach (QTreeWidgetItem* item, treeItems) {
        QString treeItemName = item->text(0);
        if (treeItemName == itemName) {
            result++;
        }
    }
    return result;
}
#undef GT_METHOD_NAME

QString GTUtilsTaskTreeView::getTaskStatus(GUITestOpStatus &os, const QString &itemName){
    openView(os);
    GTGlobals::sleep(500);
    return getTreeWidgetItem(os, itemName)->text(1);
}

SchedulerListener::SchedulerListener() :
    QObject(NULL),
    registeredTaskCount(0)
{
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

}
