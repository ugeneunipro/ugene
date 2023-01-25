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

#ifndef _U2_GT_UTILS_WORKFLOW_DESIGNER_H_
#define _U2_GT_UTILS_WORKFLOW_DESIGNER_H_

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
    static tab currentTab(HI::GUITestOpStatus& os);

    // activates the tab t
    static void setCurrentTab(HI::GUITestOpStatus& os, tab t);

    /* Opens WorkflowDesigner. If 'waitForStartupDialog' is true handles optional StartupDialog. */
    static void openWorkflowDesigner(HI::GUITestOpStatus& os, bool waitForStartupDialog = true);

    /** Returns active WD window or fails if no active WD window is found. */
    static QWidget* getActiveWorkflowDesignerWindow(HI::GUITestOpStatus& os);

    /** Checks that WD window is active. */
    static void checkWorkflowDesignerWindowIsActive(HI::GUITestOpStatus& os);

    static void loadWorkflow(HI::GUITestOpStatus& os, const QString& fileUrl);
    static void saveWorkflow(HI::GUITestOpStatus& os);
    static void saveWorkflowAs(HI::GUITestOpStatus& os, const QString& fileUrl, const QString& workflowName);

    static void validateWorkflow(HI::GUITestOpStatus& os);
    static void runWorkflow(HI::GUITestOpStatus& os);
    static void stopWorkflow(HI::GUITestOpStatus& os);
    static void returnToWorkflow(HI::GUITestOpStatus& os);

    // returns item from samples or algorithms tab
    static QTreeWidgetItem* findTreeItem(HI::GUITestOpStatus& os, const QString& itemName, tab t, bool exactMatch = false, bool failIfNULL = true);

    // returns item from samples or algorithms tab
    static QList<QTreeWidgetItem*> getVisibleSamples(HI::GUITestOpStatus& os);

    // expands samples/Elements tabwidget if collapsed
    static void expandTabs(HI::GUITestOpStatus& os, QWidget* parentWidget = nullptr);

    static void findByNameFilter(HI::GUITestOpStatus& os, const QString& elementName);

    static void cleanNameFilter(HI::GUITestOpStatus& os);

    static void clickOnPalette(HI::GUITestOpStatus& os, const QString& itemName, Qt::MouseButton mouseButton = Qt::LeftButton);

    static QTreeWidgetItem* getPaletteGroup(HI::GUITestOpStatus& os, const QString& groupName);
    static QList<QTreeWidgetItem*> getPaletteGroups(HI::GUITestOpStatus& os);
    static QStringList getPaletteGroupNames(HI::GUITestOpStatus& os);
    static QList<QTreeWidgetItem*> getPaletteGroupEntries(HI::GUITestOpStatus& os, QTreeWidgetItem* groupItem);
    static QList<QTreeWidgetItem*> getPaletteGroupEntries(HI::GUITestOpStatus& os, const QString& groupName);
    static QStringList getPaletteGroupEntriesNames(HI::GUITestOpStatus& os, const QString& groupName);

    // add to scene
    static void addSample(HI::GUITestOpStatus& os, const QString& sampName, QWidget* parentWidget = nullptr);
    static void addAlgorithm(HI::GUITestOpStatus& os, const QString& algName, bool exactMatch = false, bool useDragAndDrop = false);
    static WorkflowProcessItem* addElement(HI::GUITestOpStatus& os, const QString& algName, bool exactMatch = false);
    static WorkflowProcessItem* addElementByUsingNameFilter(HI::GUITestOpStatus& os, const QString& elementName, bool exactMatch = false);

    // returns center of worker on workflow scene in global coordinates
    static QPoint getItemCenter(HI::GUITestOpStatus& os, const QString& itemName);

    static void removeItem(HI::GUITestOpStatus& os, const QString& itemName);

    static void click(HI::GUITestOpStatus& os, const QString& itemName, QPoint p = QPoint(0, 0), Qt::MouseButton button = Qt::LeftButton);
    static void click(HI::GUITestOpStatus& os, QGraphicsItem* item, QPoint p = QPoint(0, 0), Qt::MouseButton button = Qt::LeftButton);

    // returs worker placed on workflow scene which contains item name
    static WorkflowProcessItem* getWorker(HI::GUITestOpStatus& os, const QString& itemName, const GTGlobals::FindOptions& options = {});

    static QString getWorkerText(HI::GUITestOpStatus& os, const QString& itemName, const GTGlobals::FindOptions& options = {});

    static void clickLink(HI::GUITestOpStatus& os, const QString& itemName, Qt::MouseButton button = Qt::LeftButton, int step = 10);

    static bool isWorkerExtended(HI::GUITestOpStatus& os, const QString& itemName);

    // returns WorkflowPortItem of worker which with ID "id"
    static WorkflowPortItem* getPortById(HI::GUITestOpStatus& os, WorkflowProcessItem* worker, QString id);

    static QList<WorkflowPortItem*> getPorts(HI::GUITestOpStatus& os, WorkflowProcessItem* worker);

    // connects worker "from" with worker "to"
    static void connect(HI::GUITestOpStatus& os, WorkflowProcessItem* from, WorkflowProcessItem* to);
    static void disconect(HI::GUITestOpStatus& os, WorkflowProcessItem* from, WorkflowProcessItem* to);

    static QList<WorkflowBusItem*> getAllConnectionArrows(HI::GUITestOpStatus& os);

    static WorkflowBusItem* getConnectionArrow(HI::GUITestOpStatus& os, WorkflowProcessItem* from, WorkflowProcessItem* to);
    static QGraphicsTextItem* getArrowHint(HI::GUITestOpStatus& os, WorkflowBusItem* arrow);

    // returns all workers placed on workflow scene
    static QList<WorkflowProcessItem*> getWorkers(HI::GUITestOpStatus& os);

    static QWidget* getDatasetsListWidget(HI::GUITestOpStatus& os);
    static QWidget* getCurrentDatasetWidget(HI::GUITestOpStatus& os);

    static void createDataset(HI::GUITestOpStatus& os, QString datasetName = "");

    /**
     * Sets input file with path "filePath" to the current dataset.
     * This method should be called after selecting worker which contains dataset on scene.
     * If datasetWidget is not provided it is looked up the current WD window.
     */
    static void setDatasetInputFile(HI::GUITestOpStatus& os, const QString& filePath, bool pastePath = false, QWidget* datasetWidget = nullptr);

    /**
     * Adds input files with path "filePaths" to the current dataset.
     * This method should be called after selecting worker which contains dataset on scene.
     * If datasetWidget is not provided it is looked up the current WD window.
     */

    static void setDatasetInputFiles(HI::GUITestOpStatus& os, const QStringList& filePaths, QWidget* datasetWidget = nullptr);

    static void addInputFile(HI::GUITestOpStatus& os, const QString& elementName, const QString& url);

    /**
     * Sets input folder with path "filePath" to dataset.
     * If datasetWidget is not provided it is looked up the current WD window.
     */
    static void setDatasetInputFolder(HI::GUITestOpStatus& os, const QString& filePath, QWidget* datasetWidget = nullptr);

    /**
     * Adds all folders the to the dataset widget.
     * If datasetWidget is not provided it is looked up the current WD window.
     */
    static void setDatasetInputFolders(HI::GUITestOpStatus& os, const QStringList& dirPaths, QWidget* datasetWidget = nullptr);

    // sets oneparameter worker parameter
    static void setParameter(HI::GUITestOpStatus& os, QString parameter, QVariant value, valueType type, GTGlobals::UseMethod method = GTGlobals::UseMouse);
    static void setTableValue(HI::GUITestOpStatus& os, QString parameter, QVariant value, valueType type, QTableWidget* table, GTGlobals::UseMethod method = GTGlobals::UseMouse);
    static QString getCellValue(HI::GUITestOpStatus& os, QString parameter, QTableWidget* table);
    static void setCellValue(HI::GUITestOpStatus& os, QWidget* parent, QVariant value, valueType type, GTGlobals::UseMethod method);
    static QStringList getAllParameters(HI::GUITestOpStatus& os);
    static QStringList getComboBoxParameterValues(HI::GUITestOpStatus& os, QString parameter);
    static QList<QPair<QString, bool>> getCheckableComboboxValuesFromInputPortTable(HI::GUITestOpStatus& os, int tableIndex, const QString& slotName);

    static QTableWidget* getInputPortsTable(HI::GUITestOpStatus& os, int index);
    static QTableWidget* getOutputPortsTable(HI::GUITestOpStatus& os, int index);

    static void scrollInputPortsWidgetToTableRow(HI::GUITestOpStatus& os, int tableIndex, const QString& slotName);

    // gets oneparameter worker parameter
    static QString getParameter(HI::GUITestOpStatus& os, QString parameter, bool exactMatch = false);
    static bool isParameterEnabled(HI::GUITestOpStatus& os, QString parameter);
    static bool isParameterRequired(HI::GUITestOpStatus& os, const QString& parameter);
    static bool isParameterVisible(HI::GUITestOpStatus& os, const QString& parameter);
    static void clickParameter(HI::GUITestOpStatus& os, const QString& parameter);
    static QTableView* getParametersTable(HI::GUITestOpStatus& os);

    static void setParameterScripting(HI::GUITestOpStatus& os, QString parameter, QString scriptMode, bool exactMatch = false);

    // returns number of items in error list which contain "error"
    static int checkErrorList(HI::GUITestOpStatus& os, QString error);
    static QStringList getErrors(HI::GUITestOpStatus& os);

    static int getItemLeft(HI::GUITestOpStatus& os, const QString& itemName);
    static int getItemRight(HI::GUITestOpStatus& os, const QString& itemName);
    static int getItemTop(HI::GUITestOpStatus& os, const QString& itemName);
    static int getItemBottom(HI::GUITestOpStatus& os, const QString& itemName);

    // breakpoints
    static void toggleDebugMode(HI::GUITestOpStatus& os, bool enable = true);
    static void toggleBreakpointManager(HI::GUITestOpStatus& os);
    static void setBreakpoint(HI::GUITestOpStatus& os, const QString& itemName);
    static QStringList getBreakpointList(HI::GUITestOpStatus& os);

    static void removeCmdlineWorkerFromPalette(HI::GUITestOpStatus& os, const QString& workerName);

    static void changeInputPortBoxHeight(HI::GUITestOpStatus& os, int offset);

    // custom elements
    static void importCmdlineBasedElement(HI::GUITestOpStatus& os, const QString& path);

private:
    static void selectSample(HI::GUITestOpStatus& os, QTreeWidgetItem* sample, QWidget* parentWidget = nullptr);
    static QRect getItemRect(HI::GUITestOpStatus& os, const QString& itemName);
    static QTreeWidget* getCurrentTabTreeWidget(HI::GUITestOpStatus& os);

    static const int verticalShift;
};

}  // namespace U2
Q_DECLARE_METATYPE(QAction*)
#endif  // _U2_GT_UTILS_WORKFLOW_DESIGNER_H_
