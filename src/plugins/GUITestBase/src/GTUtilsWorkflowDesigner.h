/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2018 UniPro <ugene@unipro.ru>
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

#include <QTreeWidget>
#include <QGraphicsItem>

#include <GTGlobals.h>

class QTableView;
class QTableWidget;

namespace U2 {
using namespace HI;
class WorkflowProcessItem;
class WorkflowPortItem;
class WorkflowBusItem;

class GTUtilsWorkflowDesigner {
public:
    enum tab {algoriths,samples};
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

    //returns algoriths or samples
    static tab currentTab(HI::GUITestOpStatus &os);

    //activates the tab t
    static void setCurrentTab(HI::GUITestOpStatus &os, tab t);

    //opens WorkflowDesigner
    static void openWorkflowDesigner(HI::GUITestOpStatus &os);

    static void loadWorkflow(HI::GUITestOpStatus &os, const QString &fileUrl);
    static void saveWorkflow(HI::GUITestOpStatus &os);
    static void saveWorkflowAs(HI::GUITestOpStatus &os, const QString &fileUrl, const QString &workflowName);

    static void validateWorkflow(HI::GUITestOpStatus &os);
    static void runWorkflow(HI::GUITestOpStatus &os);
    static void returnToWorkflow(HI::GUITestOpStatus &os);

    //returns item from samples or algorithms tab
    static QTreeWidgetItem* findTreeItem(HI::GUITestOpStatus &os, QString itemName, tab t, bool exactMatch = false, bool failIfNULL = true);

    //returns item from samples tab
    static QTreeWidgetItem* findSamplesTreeItem(HI::GUITestOpStatus &os, QString itemName);

    //returns item from samples or algorithms tab
    static QList<QTreeWidgetItem*> getVisibleSamples(HI::GUITestOpStatus &os);

    //expands samples/Elements tabwidget if collapsed
    static void expandTabs(HI::GUITestOpStatus &os);

    static void clickOnPalette(HI::GUITestOpStatus &os, const QString &itemName, Qt::MouseButton mouseButton = Qt::LeftButton);

    static QTreeWidgetItem * getPaletteGroup(HI::GUITestOpStatus &os, const QString &groupName);
    static QList<QTreeWidgetItem *> getPaletteGroups(HI::GUITestOpStatus &os);
    static QStringList getPaletteGroupNames(HI::GUITestOpStatus &os);
    static QList<QTreeWidgetItem *> getPaletteGroupEntries(HI::GUITestOpStatus &os, QTreeWidgetItem *groupItem);
    static QList<QTreeWidgetItem *> getPaletteGroupEntries(HI::GUITestOpStatus &os, const QString &groupName);
    static QStringList getPaletteGroupEntriesNames(HI::GUITestOpStatus &os, const QString &groupName);

    //add to scene
    static void addSample(HI::GUITestOpStatus &os, const QString &sampName);
    static void addAlgorithm(HI::GUITestOpStatus &os, QString algName, bool exactMatch = false, bool useDragAndDrop = false);
    static WorkflowProcessItem * addElement(HI::GUITestOpStatus &os, const QString &algName, bool exactMatch = false);

    //returns center of worker on workflow scene in global coordinates
    static QPoint getItemCenter(HI::GUITestOpStatus &os,QString itemName);

    static void removeItem(HI::GUITestOpStatus &os,QString itemName);

    static void click(HI::GUITestOpStatus &os, QString itemName,  QPoint p = QPoint(0,0), Qt::MouseButton button = Qt::LeftButton);
    static void click(HI::GUITestOpStatus &os, QGraphicsItem *item,  QPoint p = QPoint(0,0), Qt::MouseButton button = Qt::LeftButton);

    //returs worker placed on workflow scene which contains item name
    static WorkflowProcessItem* getWorker(HI::GUITestOpStatus &os, QString itemName,const GTGlobals::FindOptions &options = GTGlobals::FindOptions());

    static QString getWorkerText(HI::GUITestOpStatus &os, QString itemName, const GTGlobals::FindOptions &options = GTGlobals::FindOptions());

    static void clickLink(HI::GUITestOpStatus &os, QString itemName, Qt::MouseButton button = Qt::LeftButton, int step = 10);

    static bool isWorkerExtended(HI::GUITestOpStatus &os, const QString &itemName);

    //returns WorkflowPortItem of worker which with ID "id"
    static WorkflowPortItem* getPortById(HI::GUITestOpStatus &os,WorkflowProcessItem* worker, QString id);

    static QList<WorkflowPortItem*> getPorts(HI::GUITestOpStatus &os,WorkflowProcessItem* worker);

    //connects worker "from" with worker "to"
    static void connect(HI::GUITestOpStatus &os, WorkflowProcessItem* from, WorkflowProcessItem* to);
    static void disconect(HI::GUITestOpStatus &os, WorkflowProcessItem* from, WorkflowProcessItem* to);

    static QList<WorkflowBusItem*> getAllConnectionArrows(HI::GUITestOpStatus &os);

    static WorkflowBusItem* getConnectionArrow(HI::GUITestOpStatus &os, WorkflowProcessItem* from, WorkflowProcessItem* to);
    static QGraphicsTextItem* getArrowHint(HI::GUITestOpStatus &os, WorkflowBusItem* arrow);

    //returns all workers placed on workflow scene
    static QList<WorkflowProcessItem*> getWorkers(HI::GUITestOpStatus &os);

    static void createDataset(HI::GUITestOpStatus& os, QString datasetName = "");
    //sets input file with path "filePath" to the current dataset
    //this method should be called after selecting worker which contains dataset on scene
    static void setDatasetInputFile(HI::GUITestOpStatus &os, const QString &filePath, bool pastePath = false);

    static void addInputFile(HI::GUITestOpStatus &os, const QString &elementName, const QString &url);

    //sets input folder with path "filePath" to dataset
    static void setDatasetInputFolder(HI::GUITestOpStatus &os, QString filePath);

    //sets oneparameter worker parameter
    static void setParameter(HI::GUITestOpStatus& os, QString parameter, QVariant value, valueType type, GTGlobals::UseMethod method = GTGlobals::UseMouse);
    static void setTableValue(HI::GUITestOpStatus& os, QString parameter, QVariant value, valueType type, QTableWidget* table, GTGlobals::UseMethod method = GTGlobals::UseMouse);
    static QString getCellValue(HI::GUITestOpStatus& os, QString parameter, QTableWidget* table);
    static void setCellValue(HI::GUITestOpStatus& os, QWidget* parent, QVariant value, valueType type, GTGlobals::UseMethod method);
    static QStringList getAllParameters(HI::GUITestOpStatus& os);

    static QTableWidget* getInputPortsTable(HI::GUITestOpStatus &os, int index);
    static QTableWidget* getOutputPortsTable(HI::GUITestOpStatus &os, int index);

    //gets oneparameter worker parameter
    static QString getParameter(HI::GUITestOpStatus& os, QString parameter, bool exactMatch = false);
    static bool isParameterEnabled(HI::GUITestOpStatus& os, QString parameter);
    static bool isParameterRequired(HI::GUITestOpStatus &os, const QString &parameter);
    static bool isParameterVisible(HI::GUITestOpStatus &os, const QString &parameter);
    static void clickParameter(HI::GUITestOpStatus &os, const QString &parameter);
    static QTableView *getParametersTable(HI::GUITestOpStatus &os);

    static void setParameterScripting(HI::GUITestOpStatus& os, QString parameter, QString scriptMode, bool exactMatch = false);

    //returns number of items in error list which contain "error"
    static int checkErrorList(HI::GUITestOpStatus &os, QString error);
    static QStringList getErrors(HI::GUITestOpStatus &os);

    static int getItemLeft(HI::GUITestOpStatus &os, QString itemName);
    static int getItemRight(HI::GUITestOpStatus &os, QString itemName);
    static int getItemTop(HI::GUITestOpStatus &os, QString itemName);
    static int getItemBottom(HI::GUITestOpStatus &os, QString itemName);

    // breakpoints
    static void toggleDebugMode(HI::GUITestOpStatus &os, bool enable = true);
    static void toggleBreakpointManager(HI::GUITestOpStatus &os);
    static void setBreakpoint(HI::GUITestOpStatus &os, const QString &itemName);
    static QStringList getBreakpointList(HI::GUITestOpStatus &os);

    static void removeCmdlineWorkerFromPalette(HI::GUITestOpStatus &os, const QString &workerName);

private:
    static void selectAlgorithm(HI::GUITestOpStatus &os, QTreeWidgetItem *algorithm);
    static void selectSample(HI::GUITestOpStatus &os, QTreeWidgetItem *sample);
    static QRect getItemRect(HI::GUITestOpStatus &os,QString itemName);
    static QTreeWidget * getCurrentTabTreeWidget(HI::GUITestOpStatus &os);

    static const int verticalShift;
};

} // namespace
Q_DECLARE_METATYPE( QAction* )
#endif // _U2_GT_UTILS_WORKFLOW_DESIGNER_H_
