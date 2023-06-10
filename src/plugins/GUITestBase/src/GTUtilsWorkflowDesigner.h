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

#include <GTGlobals.h>

#include <QGraphicsItem>
#include <QTreeWidget>

class QTableView;
class QTableWidget;

namespace U2 {
using namespace HI;
class WorkflowProcessItem;
class WorkflowPortItem;
class WorkflowBusItem;

class GTUtilsWorkflowDesigner {
public:
    enum tab { algorithms,
               samples };
    enum valueType {
        spinValue,
        doubleSpinValue,
        comboValue,
        textValue,
        ComboChecks,
        comboWithFileSelector,
        lineEditWithFileSelector,
        customDialogSelector
    };

    // returns algorithms or samples
    static tab currentTab();

    // activates the tab t
    static void setCurrentTab(tab t);

    /* Opens WorkflowDesigner. If 'waitForStartupDialog' is true handles optional StartupDialog. */
    static void openWorkflowDesigner(bool waitForStartupDialog = true);

    /** Returns active WD window or fails if no active WD window is found. */
    static QWidget* getActiveWorkflowDesignerWindow();

    /** Checks that WD window is active. */
    static void checkWorkflowDesignerWindowIsActive();

    static void loadWorkflow(const QString& fileUrl);
    static void saveWorkflow();
    static void saveWorkflowAs(const QString& fileUrl, const QString& workflowName);

    static void validateWorkflow();
    static void runWorkflow();
    static void stopWorkflow();
    static void returnToWorkflow();

    // returns item from samples or algorithms tab
    static QTreeWidgetItem* findTreeItem(const QString& itemName, tab t, bool exactMatch = false, bool failIfNULL = true);

    // returns item from samples or algorithms tab
    static QList<QTreeWidgetItem*> getVisibleSamples();

    // expands samples/Elements tabwidget if collapsed
    static void expandTabs(QWidget* parentWidget = nullptr);

    static void findByNameFilter(const QString& elementName);

    static void cleanNameFilter();

    static void clickOnPalette(const QString& itemName, Qt::MouseButton mouseButton = Qt::LeftButton);

    static QTreeWidgetItem* getPaletteGroup(const QString& groupName);
    static QList<QTreeWidgetItem*> getPaletteGroups();
    static QStringList getPaletteGroupNames();
    static QList<QTreeWidgetItem*> getPaletteGroupEntries(QTreeWidgetItem* groupItem);
    static QList<QTreeWidgetItem*> getPaletteGroupEntries(const QString& groupName);
    static QStringList getPaletteGroupEntriesNames(const QString& groupName);

    // add to scene
    static void addSample(const QString& sampName, QWidget* parentWidget = nullptr);
    static void addAlgorithm(const QString& algName, bool exactMatch = false, bool useDragAndDrop = false);
    static WorkflowProcessItem* addElement(const QString& algName, bool exactMatch = false);
    static WorkflowProcessItem* addElementByUsingNameFilter(const QString& elementName, bool exactMatch = false);

    // returns center of worker on workflow scene in global coordinates
    static QPoint getItemCenter(const QString& itemName);

    static void removeItem(const QString& itemName);

    static void click(const QString& itemName, QPoint p = QPoint(0, 0), Qt::MouseButton button = Qt::LeftButton);
    static void click(QGraphicsItem* item, QPoint p = QPoint(0, 0), Qt::MouseButton button = Qt::LeftButton);

    // returs worker placed on workflow scene which contains item name
    static WorkflowProcessItem* getWorker(const QString& itemName, const GTGlobals::FindOptions& options = {});

    static QString getWorkerText(const QString& itemName, const GTGlobals::FindOptions& options = {});

    static void clickLink(const QString& itemName, Qt::MouseButton button = Qt::LeftButton, int step = 10);

    static bool isWorkerExtended(const QString& itemName);

    // returns WorkflowPortItem of worker which with ID "id"
    static WorkflowPortItem* getPortById(WorkflowProcessItem* worker, QString id);

    static QList<WorkflowPortItem*> getPorts(WorkflowProcessItem* worker);

    // connects worker "from" with worker "to"
    static void connect(WorkflowProcessItem* from, WorkflowProcessItem* to);
    static void disconect(WorkflowProcessItem* from, WorkflowProcessItem* to);

    static QList<WorkflowBusItem*> getAllConnectionArrows();

    static WorkflowBusItem* getConnectionArrow(WorkflowProcessItem* from, WorkflowProcessItem* to);
    static QGraphicsTextItem* getArrowHint(WorkflowBusItem* arrow);

    // returns all workers placed on workflow scene
    static QList<WorkflowProcessItem*> getWorkers();

    static QWidget* getDatasetsListWidget();
    static QWidget* getCurrentDatasetWidget();

    static void createDataset(QString datasetName = "");

    /**
     * Sets input file with path "filePath" to the current dataset.
     * This method should be called after selecting worker which contains dataset on scene.
     * If datasetWidget is not provided it is looked up the current WD window.
     */
    static void setDatasetInputFile(const QString& filePath, bool pastePath = false, QWidget* datasetWidget = nullptr);

    /**
     * Adds input files with path "filePaths" to the current dataset.
     * This method should be called after selecting worker which contains dataset on scene.
     * If datasetWidget is not provided it is looked up the current WD window.
     */

    static void setDatasetInputFiles(const QStringList& filePaths, QWidget* datasetWidget = nullptr);

    static void addInputFile(const QString& elementName, const QString& url);

    /**
     * Sets input folder with path "filePath" to dataset.
     * If datasetWidget is not provided it is looked up the current WD window.
     */
    static void setDatasetInputFolder(const QString& filePath, QWidget* datasetWidget = nullptr);

    /**
     * Adds all folders the to the dataset widget.
     * If datasetWidget is not provided it is looked up the current WD window.
     */
    static void setDatasetInputFolders(const QStringList& dirPaths, QWidget* datasetWidget = nullptr);

    // sets oneparameter worker parameter
    static void setParameter(QString parameter, QVariant value, valueType type, GTGlobals::UseMethod method = GTGlobals::UseMouse);
    static void setTableValue(QString parameter, QVariant value, valueType type, QTableWidget* table, GTGlobals::UseMethod method = GTGlobals::UseMouse);
    static QString getCellValue(QString parameter, QTableWidget* table);
    static void setCellValue(QWidget* parent, QVariant value, valueType type, GTGlobals::UseMethod method);
    static QStringList getAllParameters();
    static QStringList getComboBoxParameterValues(QString parameter);
    static QList<QPair<QString, bool>> getCheckableComboboxValuesFromInputPortTable(int tableIndex, const QString& slotName);

    static QTableWidget* getInputPortsTable(int index, bool failIfNotFound = true);
    static QTableWidget* getOutputPortsTable(int index);

    static void scrollInputPortsWidgetToTableRow(int tableIndex, const QString& slotName);

    // gets oneparameter worker parameter
    static QString getParameter(QString parameter, bool exactMatch = false);
    static bool isParameterEnabled(QString parameter);
    static bool isParameterRequired(const QString& parameter);
    static bool isParameterVisible(const QString& parameter);
    static void clickParameter(const QString& parameter);
    static QTableView* getParametersTable();

    static void setParameterScripting(QString parameter, QString scriptMode, bool exactMatch = false);

    // returns number of items in error list which contain "error"
    static int checkErrorList(QString error);
    static QStringList getErrors();

    static int getItemLeft(const QString& itemName);
    static int getItemRight(const QString& itemName);
    static int getItemTop(const QString& itemName);
    static int getItemBottom(const QString& itemName);

    // breakpoints
    static void toggleDebugMode(bool enable = true);
    static void toggleBreakpointManager();
    static void setBreakpoint(const QString& itemName);
    static QStringList getBreakpointList();

    static void removeCmdlineWorkerFromPalette(const QString& workerName);

    static void changeInputPortBoxHeight(int offset);

    // custom elements
    static void importCmdlineBasedElement(const QString& path);

    /** Returns a button to switch from WD to the dashboards view. */
    static QAbstractButton* getGotoDashboardButton();

    /** Returns a button to switch from the dashboards view to WD. */
    static QAbstractButton* getGotoWorkflowButton();

private:
    static void selectSample(QTreeWidgetItem* sample, QWidget* parentWidget = nullptr);
    static QRect getItemRect(const QString& itemName);
    static QTreeWidget* getCurrentTabTreeWidget();

    static const int verticalShift;
};

}  // namespace U2
Q_DECLARE_METATYPE(QAction*)
