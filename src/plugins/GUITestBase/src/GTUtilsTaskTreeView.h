/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#pragma once

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
    static void waitTaskFinished(long timeoutMillis = 180000);
    static void click(const QString& itemName, Qt::MouseButton b = Qt::LeftButton);
    // Double click on the item with @itemName. This tree item will be expanded
    static void doubleClick(const QString& itemName);

    /** Opens view if it is not opened and returns tree widget. */
    static QTreeWidget* openView();

    /** Returns true if Task tree view is opened or false if it's not. */
    static bool isViewOpened();

    static void toggleView();

    /**
     * Cancels tasks with the given name.
     * If "parentTaskNames" is provided first expands the top-level tasks so the task tree is populated.
     */
    static void cancelTask(const QString& itemName, bool failIfNotFound = true, const QStringList& parentTaskNames = {});

    static QTreeWidgetItem* getTreeWidgetItem(const QString& itemName, bool failOnNull = true);

    /** Returns instance of the task tree view if found. Asserts if not found and if 'failIfNotFound' is 'true'. */
    static QTreeWidget* getTreeWidget(bool failIfNotFound = false);

    static void moveToOpenedView(const QString& itemName);
    static QPoint getTreeViewItemPosition(const QString& itemName);
    static void moveTo(const QString& itemName);
    static int getTopLevelTasksCount();

    /** Check that there/there-is-no top-level task with the given name. */
    static void checkTaskIsPresent(const QString& topLevelTaskName, bool checkIfPresent = true);

    static int countTasks(const QString& itemName);
    static QString getTaskStatus(const QString& itemName);
    /*
     * Returns progress of the @itemName task. if @failIfNotFound is true, fails if the task with @@itemName wasn't found
     * Returns the persantage value if the task is running;
     * -1 if the task wasn't found; -2 if the task exists, but still not int the running state
     */
    static int getTaskProgress(const QString& itemName, bool failIfNotFound = true);
    /*
     * Waits until task with the name @itemName has progress more than @taskProgress
     */
    static void waitTaskProgressMoreThan(const QString& itemName, int taskProgress);

    static const QString widgetName;

private:
    static QTreeWidgetItem* getTreeWidgetItem(QTreeWidget* tree, const QString& itemName);
    static QList<QTreeWidgetItem*> getTaskTreeViewItems(QTreeWidgetItem* root, bool recursively = true);
    static QString getTasksInfo(QList<U2::Task*> tasks, int level);
};

}  // namespace U2
