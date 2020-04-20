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

#ifndef _U2_GUI_TASK_TREE_VIEW_UTILS_H_
#define _U2_GUI_TASK_TREE_VIEW_UTILS_H_

#include "GTGlobals.h"

class QTreeWidget;
class QTreeWidgetItem;

namespace U2 {
class Task;

class SchedulerListener : public QObject {
    Q_OBJECT
public:
    SchedulerListener();

    int getRegisteredTaskCount() const;
    void reset();

private slots:
    void sl_taskRegistered();

private:
    int registeredTaskCount;
};

class GTUtilsTaskTreeView {
public:
    static void waitTaskFinished(HI::GUITestOpStatus &os, long timeout=180000);
    static void click(HI::GUITestOpStatus &os, const QString &itemName, Qt::MouseButton b = Qt::LeftButton);
    static void openView(HI::GUITestOpStatus& os);
    static void toggleView(HI::GUITestOpStatus& os);
    static void cancelTask(HI::GUITestOpStatus& os, const QString &itemName);
    static QTreeWidgetItem* getTreeWidgetItem(HI::GUITestOpStatus &os, const QString &itemName, bool failOnNull = true);
    static QTreeWidget* getTreeWidget(HI::GUITestOpStatus &os);
    static void moveToOpenedView(HI::GUITestOpStatus& os, const QString &itemName);
    static QPoint getTreeViewItemPosition(HI::GUITestOpStatus &os, const QString &itemName);
    static void moveTo(HI::GUITestOpStatus &os,const QString &itemName);
    static int getTopLevelTasksCount(HI::GUITestOpStatus &os);
    static bool checkTask(HI::GUITestOpStatus& os, const QString &itemName);
    static int countTasks(HI::GUITestOpStatus& os, const QString &itemName);
    static QString getTaskStatus(HI::GUITestOpStatus& os, const QString &itemName);

    static const QString widgetName;

private:
    static QTreeWidgetItem* getTreeWidgetItem(QTreeWidget* tree, const QString &itemName);
    static QList<QTreeWidgetItem*> getTaskTreeViewItems(QTreeWidgetItem* root, bool recursively = true);
    static QString getTasksInfo(QList<U2::Task *> tasks, int level);

};

} //ns

#endif
