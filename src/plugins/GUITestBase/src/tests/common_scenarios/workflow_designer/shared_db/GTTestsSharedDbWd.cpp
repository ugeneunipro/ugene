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

#include <QListWidget>
#include <QDir>
#include <U2Core/global.h>
#include <U2Core/U2SafePoints.h>
#include "primitives/GTAction.h"
#include <drivers/GTKeyboardDriver.h>
#include "primitives/GTMenu.h"
#include <drivers/GTMouseDriver.h>
#include <primitives/GTWidget.h>
#include <base_dialogs/MessageBoxFiller.h>
#include "runnables/ugene/corelibs/U2Gui/EditConnectionDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ProjectTreeItemSelectorDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/SharedConnectionsDialogFiller.h"
#include "runnables/ugene/plugins/workflow_designer/StartupDialogFiller.h"
#include "runnables/ugene/plugins/workflow_designer/WorkflowMetadialogFiller.h"

#include "GTDatabaseConfig.h"
#include "GTUtilsEscClicker.h"
#include "GTUtilsLog.h"
#include "GTUtilsMdi.h"
#include "GTUtilsTaskTreeView.h"
#include "GTUtilsWorkflowDesigner.h"

#include "GTTestsSharedDbWd.h"

namespace U2 {
namespace GUITest_common_scenarios_shared_db_wd {
using namespace HI;
namespace {

void createTestConnection(HI::GUITestOpStatus &os) {
    GTLogTracer lt;
    QString conName = "ugene_gui_test";
    {
        QList<SharedConnectionsDialogFiller::Action> actions;
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::ADD);
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::CLICK, conName);
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::CONNECT, conName);
        GTUtilsDialog::waitForDialog(os, new SharedConnectionsDialogFiller(os, actions));
    }
    {
        EditConnectionDialogFiller::Parameters params;
        params.connectionName = conName;
        GTUtilsDialog::waitForDialog(os, new EditConnectionDialogFiller(os, params, EditConnectionDialogFiller::FROM_SETTINGS));
    }

    CHECK_SET_ERR_RESULT(!lt.hasError(), "errors in log", );
}

}

GUI_TEST_CLASS_DEFINITION(read_gui_test_0001) {
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsWorkflowDesigner::addAlgorithm(os, "Read Sequence", true);

    createTestConnection(os);

    QSet<GObjectType> acceptableTypes;
    acceptableTypes << GObjectTypes::SEQUENCE;
    GTUtilsDialog::waitForDialog(os, new ProjectTreeItemSelectorDialogFiller(os, "ugene_gui_test", "et0001_sequence", acceptableTypes));

    QWidget *addFromDbButton = GTWidget::findWidget(os, "addFromDbButton");
    GTWidget::click(os, addFromDbButton);

    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();

    QListWidget *datasetList = qobject_cast<QListWidget *>(GTWidget::findWidget(os, "itemsArea"));
    CHECK_SET_ERR(NULL != datasetList, "Unable to find dataset list widget");

    CHECK_SET_ERR(1 == datasetList->count(), QString("Invalid dataset item count: expect %1, got %2").arg("1").arg(datasetList->count()));
    CHECK_SET_ERR("et0001_sequence" == datasetList->item(0)->text(), "Invalid dataset item name");
}

GUI_TEST_CLASS_DEFINITION(read_gui_test_0002) {
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsWorkflowDesigner::addAlgorithm(os, "Read Alignment");

    createTestConnection(os);

    QSet<GObjectType> acceptableTypes;
    acceptableTypes << GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT;
    GTUtilsDialog::waitForDialog(os, new ProjectTreeItemSelectorDialogFiller(os, "ugene_gui_test", "et0003_alignment", acceptableTypes));

    QWidget *addFromDbButton = GTWidget::findWidget(os, "addFromDbButton");
    GTWidget::click(os, addFromDbButton);

    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();

    QListWidget *datasetList = qobject_cast<QListWidget *>(GTWidget::findWidget(os, "itemsArea"));
    CHECK_SET_ERR(NULL != datasetList, "Unable to find dataset list widget");

    CHECK_SET_ERR(1 == datasetList->count(), QString("Invalid dataset item count: expect %1, got %2").arg("1").arg(datasetList->count()));
    CHECK_SET_ERR("et0003_alignment" == datasetList->item(0)->text(), "Invalid dataset item name");
}

GUI_TEST_CLASS_DEFINITION(read_gui_test_0003) {
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsWorkflowDesigner::addAlgorithm(os, "Read Annotations");

    createTestConnection(os);

    QSet<GObjectType> acceptableTypes;
    acceptableTypes << GObjectTypes::ANNOTATION_TABLE;
    GTUtilsDialog::waitForDialog(os, new ProjectTreeItemSelectorDialogFiller(os, "ugene_gui_test", "et0002_features", acceptableTypes));

    QWidget *addFromDbButton = GTWidget::findWidget(os, "addFromDbButton");
    GTWidget::click(os, addFromDbButton);

    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();

    QListWidget *datasetList = qobject_cast<QListWidget *>(GTWidget::findWidget(os, "itemsArea"));
    CHECK_SET_ERR(NULL != datasetList, "Unable to find dataset list widget");

    CHECK_SET_ERR(1 == datasetList->count(), QString("Invalid dataset item count: expect %1, got %2").arg("1").arg(datasetList->count()));
    CHECK_SET_ERR("et0002_features" == datasetList->item(0)->text(), "Invalid dataset item name");
}

GUI_TEST_CLASS_DEFINITION(read_gui_test_0004) {
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsWorkflowDesigner::addAlgorithm(os, "Read NGS Reads Assembly");

    createTestConnection(os);

    QSet<GObjectType> acceptableTypes;
    acceptableTypes << GObjectTypes::ASSEMBLY;
    GTUtilsDialog::waitForDialog(os, new ProjectTreeItemSelectorDialogFiller(os, "ugene_gui_test", "et0004_assembly", acceptableTypes));

    QWidget *addFromDbButton = GTWidget::findWidget(os, "addFromDbButton");
    GTWidget::click(os, addFromDbButton);

    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();

    QListWidget *datasetList = qobject_cast<QListWidget *>(GTWidget::findWidget(os, "itemsArea"));
    CHECK_SET_ERR(NULL != datasetList, "Unable to find dataset list widget");

    CHECK_SET_ERR(1 == datasetList->count(), QString("Invalid dataset item count: expect %1, got %2").arg("1").arg(datasetList->count()));
    CHECK_SET_ERR("et0004_assembly" == datasetList->item(0)->text(), "Invalid dataset item name");
}

GUI_TEST_CLASS_DEFINITION(read_gui_test_0005) {
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsWorkflowDesigner::addAlgorithm(os, "Read Plain Text");

    createTestConnection(os);

    QSet<GObjectType> acceptableTypes;
    acceptableTypes << GObjectTypes::TEXT;
    GTUtilsDialog::waitForDialog(os, new ProjectTreeItemSelectorDialogFiller(os, "ugene_gui_test", "et0006_text", acceptableTypes));

    QWidget *addFromDbButton = GTWidget::findWidget(os, "addFromDbButton");
    GTWidget::click(os, addFromDbButton);

    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();

    QListWidget *datasetList = qobject_cast<QListWidget *>(GTWidget::findWidget(os, "itemsArea"));
    CHECK_SET_ERR(NULL != datasetList, "Unable to find dataset list widget");

    CHECK_SET_ERR(1 == datasetList->count(), QString("Invalid dataset item count: expect %1, got %2").arg("1").arg(datasetList->count()));
    CHECK_SET_ERR("et0006_text" == datasetList->item(0)->text(), "Invalid dataset item name");
}

GUI_TEST_CLASS_DEFINITION(read_gui_test_0006) {
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsWorkflowDesigner::addAlgorithm(os, "Read Variants");

    createTestConnection(os);

    QSet<GObjectType> acceptableTypes;
    acceptableTypes << GObjectTypes::VARIANT_TRACK;
    GTUtilsDialog::waitForDialog(os, new ProjectTreeItemSelectorDialogFiller(os, "ugene_gui_test", "et0005_variations", acceptableTypes));

    QWidget *addFromDbButton = GTWidget::findWidget(os, "addFromDbButton");
    GTWidget::click(os, addFromDbButton);

    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();

    QListWidget *datasetList = qobject_cast<QListWidget *>(GTWidget::findWidget(os, "itemsArea"));
    CHECK_SET_ERR(NULL != datasetList, "Unable to find dataset list widget");

    CHECK_SET_ERR(1 == datasetList->count(), QString("Invalid dataset item count: expect %1, got %2").arg("1").arg(datasetList->count()));
    CHECK_SET_ERR("et0005_variations" == datasetList->item(0)->text(), "Invalid dataset item name");
}

GUI_TEST_CLASS_DEFINITION(read_gui_neg_test_0007) {
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsWorkflowDesigner::addAlgorithm(os, "Read File URL(s)");

    QWidget *addFromDbButton = GTWidget::findWidget(os, "addFromDbButton");
    CHECK_SET_ERR(!addFromDbButton->isVisible(), "Unexpected button found");
}

GUI_TEST_CLASS_DEFINITION(read_gui_test_0008) {
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsWorkflowDesigner::addAlgorithm(os, "Read Alignment");

    createTestConnection(os);

    QSet<GObjectType> acceptableTypes;
    acceptableTypes << GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT;
    GTUtilsDialog::waitForDialog(os, new ProjectTreeItemSelectorDialogFiller(os, "ugene_gui_test", "pt0005_COI", acceptableTypes));

    QWidget *addFromDbButton = GTWidget::findWidget(os, "addFromDbButton");
    GTWidget::click(os, addFromDbButton);

    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();

    GTUtilsDialog::waitForDialog(os, new ProjectTreeItemSelectorDialogFiller(os, "ugene_gui_test", "export_tests"));
    GTWidget::click(os, addFromDbButton);

    QListWidget *datasetList = qobject_cast<QListWidget *>(GTWidget::findWidget(os, "itemsArea"));
    CHECK_SET_ERR(NULL != datasetList, "Unable to find dataset list widget");

    CHECK_SET_ERR(2 == datasetList->count(), QString("Invalid dataset item count: expect %1, got %2").arg("2").arg(datasetList->count()));
    CHECK_SET_ERR("pt0005_COI" == datasetList->item(0)->text(), "Invalid dataset item name_1");
    CHECK_SET_ERR("export_tests" == datasetList->item(1)->text(), "Invalid dataset item name_2");
}

GUI_TEST_CLASS_DEFINITION(read_gui_test_0009) {
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsWorkflowDesigner::addAlgorithm(os, "Read NGS Reads Assembly");

    createTestConnection(os);

    GTUtilsDialog::waitForDialog(os, new ProjectTreeItemSelectorDialogFiller(os, "ugene_gui_test", "view_test_0003"));

    QWidget *addFromDbButton = GTWidget::findWidget(os, "addFromDbButton");
    GTWidget::click(os, addFromDbButton);

    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();

    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir + "_common_data/ugenedb/Klebsislla.sort.bam.ugenedb");

    QSet<GObjectType> acceptableTypes;
    acceptableTypes << GObjectTypes::ASSEMBLY;
    GTUtilsDialog::waitForDialog(os, new ProjectTreeItemSelectorDialogFiller(os, "ugene_gui_test", "et0004_assembly", acceptableTypes));
    GTWidget::click(os, addFromDbButton);

    GTUtilsWorkflowDesigner::setDatasetInputFolder(os, QDir(testDir + "_common_data/bam").absolutePath());

    QListWidget *datasetList = qobject_cast<QListWidget *>(GTWidget::findWidget(os, "itemsArea"));
    CHECK_SET_ERR(NULL != datasetList, "Unable to find dataset list widget");

    CHECK_SET_ERR(4 == datasetList->count(), QString("Invalid dataset item count: expect %1, got %2").arg("4").arg(datasetList->count()));
    CHECK_SET_ERR("view_test_0003" == datasetList->item(0)->text(), "Invalid dataset item name_1");
    CHECK_SET_ERR("Klebsislla.sort.bam.ugenedb" == datasetList->item(1)->text(), "Invalid dataset item name_2");
    CHECK_SET_ERR("et0004_assembly" == datasetList->item(2)->text(), "Invalid dataset item name_3");
    CHECK_SET_ERR("bam" == datasetList->item(3)->text(), "Invalid dataset item name_4");
}

GUI_TEST_CLASS_DEFINITION(read_gui_test_0010) {
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsWorkflowDesigner::addAlgorithm(os, "Read Sequence", true);

    createTestConnection(os);

    QSet<GObjectType> acceptableTypes;
    acceptableTypes << GObjectTypes::SEQUENCE;
    QMap<QString, QStringList> doc2Objects;
    doc2Objects["ugene_gui_test"] << "et0001_sequence" << "et0007_seq";
    GTUtilsDialog::waitForDialog(os, new ProjectTreeItemSelectorDialogFiller(os, doc2Objects, acceptableTypes,
        ProjectTreeItemSelectorDialogFiller::Separate));

    QWidget *addFromDbButton = GTWidget::findWidget(os, "addFromDbButton");

    GTWidget::click(os, addFromDbButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();

    QMap<QString, QStringList> doc2Items;
    doc2Items["ugene_gui_test"] << "export_test_0007" << "export_test_0009";
    GTUtilsDialog::waitForDialog(os, new ProjectTreeItemSelectorDialogFiller(os, doc2Items, acceptableTypes,
        ProjectTreeItemSelectorDialogFiller::Continuous));

    GTWidget::click(os, addFromDbButton);

    QListWidget *datasetList = qobject_cast<QListWidget *>(GTWidget::findWidget(os, "itemsArea"));
    CHECK_SET_ERR(NULL != datasetList, "Unable to find dataset list widget");

    CHECK_SET_ERR(5 == datasetList->count(), QString("Invalid dataset item count: expect %1, got %2").arg("5").arg(datasetList->count()));
    CHECK_SET_ERR(!datasetList->findItems("et0001_sequence", Qt::MatchExactly).isEmpty(), "Invalid dataset item name_1");
    CHECK_SET_ERR(!datasetList->findItems("et0007_seq", Qt::MatchExactly).isEmpty(), "Invalid dataset item nam_2");
    CHECK_SET_ERR(!datasetList->findItems("export_test_0007", Qt::MatchExactly).isEmpty(), "Invalid dataset item name_3");
    CHECK_SET_ERR(!datasetList->findItems("export_test_0008", Qt::MatchExactly).isEmpty(), "Invalid dataset item name_4");
    CHECK_SET_ERR(!datasetList->findItems("export_test_0009", Qt::MatchExactly).isEmpty(), "Invalid dataset item name_5");
}

GUI_TEST_CLASS_DEFINITION(read_gui_test_0011) {
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsWorkflowDesigner::addAlgorithm(os, "Read Sequence", true);

    createTestConnection(os);

    QSet<GObjectType> acceptableTypes;
    acceptableTypes << GObjectTypes::SEQUENCE;
    QMap<QString, QStringList> doc2Objects;
    doc2Objects["ugene_gui_test"] << "et0007_seq" << "et0001_sequence";
    GTUtilsDialog::waitForDialog(os, new ProjectTreeItemSelectorDialogFiller(os, doc2Objects, acceptableTypes,
        ProjectTreeItemSelectorDialogFiller::Continuous));

    QWidget *addFromDbButton = GTWidget::findWidget(os, "addFromDbButton");

    GTWidget::click(os, addFromDbButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();

    QMap<QString, QStringList> doc2Items;
    doc2Items["ugene_gui_test"] << "pt0001_dir2" << "pt0005_human_T1" << "pt0006_dir2";
    GTUtilsDialog::waitForDialog(os, new ProjectTreeItemSelectorDialogFiller(os, doc2Items, acceptableTypes,
        ProjectTreeItemSelectorDialogFiller::Separate));

    GTWidget::click(os, addFromDbButton);

    QListWidget *datasetList = qobject_cast<QListWidget *>(GTWidget::findWidget(os, "itemsArea"));
    CHECK_SET_ERR(NULL != datasetList, "Unable to find dataset list widget");

    CHECK_SET_ERR(7 == datasetList->count(), QString("Invalid dataset item count: expect %1, got %2").arg("7").arg(datasetList->count()));
    CHECK_SET_ERR("export_test_0008" == datasetList->item(0)->text(), "Invalid dataset item name_1");
    CHECK_SET_ERR("export_test_0009" == datasetList->item(1)->text(), "Invalid dataset item name_2");
    CHECK_SET_ERR("et0007_seq" == datasetList->item(2)->text(), "Invalid dataset item name_3");
    CHECK_SET_ERR("et0001_sequence" == datasetList->item(3)->text(), "Invalid dataset item name_4");
    CHECK_SET_ERR("pt0001_dir2" == datasetList->item(4)->text(), "Invalid dataset item name_5");
    CHECK_SET_ERR("pt0006_dir2" == datasetList->item(5)->text(), "Invalid dataset item name_6");
    CHECK_SET_ERR("pt0005_human_T1" == datasetList->item(6)->text(), "Invalid dataset item name_7");
}

GUI_TEST_CLASS_DEFINITION(read_gui_neg_test_0012) {
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/", "HIV-1.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsWorkflowDesigner::addAlgorithm(os, "Read Alignment");

    createTestConnection(os);

    QSet<GObjectType> acceptableTypes;
    acceptableTypes << GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT;
    GTUtilsDialog::waitForDialog(os, new ProjectTreeItemSelectorDialogFiller(os, "ugene_gui_test", "et0003_alignment", acceptableTypes,
        ProjectTreeItemSelectorDialogFiller::Single, 1));

    QWidget *addFromDbButton = GTWidget::findWidget(os, "addFromDbButton");

    GTWidget::click(os, addFromDbButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(write_gui_test_0001_1) {
    createTestConnection(os);
    GTMenu::clickMainMenuItem(os, QStringList() << "File" << "Connect to UGENE shared database...");

    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsWorkflowDesigner::addAlgorithm(os, "Write Alignment");

    const QString dataStorageParamValue = GTUtilsWorkflowDesigner::getParameter(os, "Data storage");
    CHECK_SET_ERR("Local file system" == dataStorageParamValue, "Invalid data storage parameter value");
    CHECK_SET_ERR(GTUtilsWorkflowDesigner::isParameterVisible(os, "Output file"), "Unexpected parameter state");
    CHECK_SET_ERR(GTUtilsWorkflowDesigner::isParameterVisible(os, "Output file suffix"), "Unexpected parameter state");
    CHECK_SET_ERR(GTUtilsWorkflowDesigner::isParameterVisible(os, "Document format"), "Unexpected parameter state");
    CHECK_SET_ERR(GTUtilsWorkflowDesigner::isParameterVisible(os, "Existing file"), "Unexpected parameter state");
    CHECK_SET_ERR(5 == GTUtilsWorkflowDesigner::getAllParameters(os).count(), "Unexpected parameter count");

    GTUtilsWorkflowDesigner::setParameter(os, "Data storage", 1, GTUtilsWorkflowDesigner::comboValue);
    CHECK_SET_ERR(GTUtilsWorkflowDesigner::isParameterVisible(os, "Database"), "Unexpected parameter state");
    CHECK_SET_ERR(GTUtilsWorkflowDesigner::isParameterVisible(os, "Output path"), "Unexpected parameter state");
    CHECK_SET_ERR(3 == GTUtilsWorkflowDesigner::getAllParameters(os).count(), "Unexpected parameter count");

    GTUtilsWorkflowDesigner::setParameter(os, "Database", 0, GTUtilsWorkflowDesigner::comboValue);
    CHECK_OP(os, );
    CHECK_SET_ERR("/" == GTUtilsWorkflowDesigner::getParameter(os, "Output path"), "Unexpected parameter default value");
}

GUI_TEST_CLASS_DEFINITION(write_gui_test_0001_2) {
    createTestConnection(os);
    GTMenu::clickMainMenuItem(os, QStringList() << "File" << "Connect to UGENE shared database...");

    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsWorkflowDesigner::addAlgorithm(os, "Write Sequence");

    const QString dataStorageParamValue = GTUtilsWorkflowDesigner::getParameter(os, "Data storage");
    CHECK_SET_ERR("Local file system" == dataStorageParamValue, "Invalid data storage parameter value");
    CHECK_SET_ERR(GTUtilsWorkflowDesigner::isParameterVisible(os, "Output file"), "Unexpected parameter state");
    CHECK_SET_ERR(GTUtilsWorkflowDesigner::isParameterVisible(os, "Output file suffix"), "Unexpected parameter state");
    CHECK_SET_ERR(GTUtilsWorkflowDesigner::isParameterVisible(os, "Document format"), "Unexpected parameter state");
    CHECK_SET_ERR(GTUtilsWorkflowDesigner::isParameterVisible(os, "Existing file"), "Unexpected parameter state");
    CHECK_SET_ERR(GTUtilsWorkflowDesigner::isParameterVisible(os, "Split sequence"), "Unexpected parameter state");
    CHECK_SET_ERR(GTUtilsWorkflowDesigner::isParameterVisible(os, "Accumulate objects"), "Unexpected parameter state");
    CHECK_SET_ERR(7 == GTUtilsWorkflowDesigner::getAllParameters(os).count(), "Unexpected parameter count");

    GTUtilsWorkflowDesigner::setParameter(os, "Data storage", 1, GTUtilsWorkflowDesigner::comboValue);
    CHECK_SET_ERR(GTUtilsWorkflowDesigner::isParameterVisible(os, "Database"), "Unexpected parameter state");
    CHECK_SET_ERR(GTUtilsWorkflowDesigner::isParameterVisible(os, "Output path"), "Unexpected parameter state");
    CHECK_SET_ERR(3 == GTUtilsWorkflowDesigner::getAllParameters(os).count(), "Unexpected parameter count");

    GTUtilsWorkflowDesigner::setParameter(os, "Database", 0, GTUtilsWorkflowDesigner::comboValue);
    CHECK_OP(os, );
    CHECK_SET_ERR("/" == GTUtilsWorkflowDesigner::getParameter(os, "Output path"), "Unexpected parameter default value");

}

GUI_TEST_CLASS_DEFINITION(write_gui_test_0002) {
    createTestConnection(os);
    GTMenu::clickMainMenuItem(os, QStringList() << "File" << "Connect to UGENE shared database...");

    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsWorkflowDesigner::addAlgorithm(os, "Write FASTA");

    CHECK_SET_ERR(GTUtilsWorkflowDesigner::isParameterVisible(os, "Output file"), "Unexpected parameter state");
    CHECK_SET_ERR(GTUtilsWorkflowDesigner::isParameterVisible(os, "Output file suffix"), "Unexpected parameter state");
    CHECK_SET_ERR(GTUtilsWorkflowDesigner::isParameterVisible(os, "Existing file"), "Unexpected parameter state");
    CHECK_SET_ERR(GTUtilsWorkflowDesigner::isParameterVisible(os, "Accumulate objects"), "Unexpected parameter state");
    CHECK_SET_ERR(4 == GTUtilsWorkflowDesigner::getAllParameters(os).count(), "Unexpected parameter count");
}

GUI_TEST_CLASS_DEFINITION(write_gui_test_0003) {
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsWorkflowDesigner::addAlgorithm(os, "Write NGS Reads Assembly");

    GTUtilsWorkflowDesigner::setParameter(os, "Data storage", 1, GTUtilsWorkflowDesigner::comboValue);
    GTUtilsWorkflowDesigner::clickParameter(os, "Database");

    const QString connectionName = "ugene_gui_test";
    EditConnectionDialogFiller::Parameters params;
    params.connectionName = connectionName;
    GTUtilsDialog::waitForDialog(os, new EditConnectionDialogFiller(os, params, EditConnectionDialogFiller::FROM_SETTINGS));

    GTWidget::click(os, GTWidget::findWidget(os, "browsePathBtn"));

    GTWidget::click(os, GTWidget::findWidget(os,"sceneView"));
    GTUtilsWorkflowDesigner::click(os, "Write NGS Reads Assembly");

    GTUtilsWorkflowDesigner::setParameter(os, "Database", 1, GTUtilsWorkflowDesigner::comboValue);


    QList<SharedConnectionsDialogFiller::Action> actions;
    actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::CLICK, connectionName);
    actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::CONNECT, connectionName);
    GTUtilsDialog::waitForDialog(os, new SharedConnectionsDialogFiller(os, actions));
    GTMenu::clickMainMenuItem(os, QStringList() << "File" << "Connect to UGENE shared database...");
}

GUI_TEST_CLASS_DEFINITION(open_uwl_gui_test_0001) {
    GTLogTracer l;
    GTUtilsDialog::waitForDialogWhichMayRunOrNot(os, new StartupDialogFiller(os));
    GTFileDialog::openFile(os, testDir + "_common_data/workflow/", "shared_db_objects_input.uwl");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsLog::check(os, l);
}

GUI_TEST_CLASS_DEFINITION(open_uwl_gui_test_0002) {
    GTLogTracer l;
    GTUtilsDialog::waitForDialogWhichMayRunOrNot(os, new StartupDialogFiller(os));
    GTFileDialog::openFile(os, testDir + "_common_data/workflow/", "shared_db_folders_input.uwl");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsLog::check(os, l);
}

GUI_TEST_CLASS_DEFINITION(open_uwl_gui_test_0003) {
    GTLogTracer l;
    GTUtilsDialog::waitForDialogWhichMayRunOrNot(os, new StartupDialogFiller(os));
    GTFileDialog::openFile(os, testDir + "_common_data/workflow/", "shared_db_output.uwl");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsLog::check(os, l);
}

GUI_TEST_CLASS_DEFINITION(save_uwl_gui_test_0001) {
    GTLogTracer l;

    createTestConnection(os);
    GTMenu::clickMainMenuItem(os, QStringList() << "File" << "Connect to UGENE shared database...");

    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsWorkflowDesigner::addAlgorithm(os, "Write Variants");

    GTUtilsWorkflowDesigner::setParameter(os, "Data storage", 1, GTUtilsWorkflowDesigner::comboValue);

    GTUtilsWorkflowDesigner::setParameter(os, "Database", 1, GTUtilsWorkflowDesigner::comboValue);
    GTUtilsWorkflowDesigner::setParameter(os, "Output path", "/123", GTUtilsWorkflowDesigner::textValue);

    QString path = sandBoxDir + "save_uwl_gui_test_0001.uwl";
    GTUtilsDialog::waitForDialog(os, new WorkflowMetaDialogFiller(os, path, "New workflow"));
    GTWidget::click(os, GTAction::button(os, "Save workflow action"));

    GTUtilsLog::check(os, l);
}

GUI_TEST_CLASS_DEFINITION(save_uwl_gui_test_0002) {
    GTLogTracer l;

    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsWorkflowDesigner::addAlgorithm(os, "Read Annotations");

    createTestConnection(os);

    QMap<QString, QStringList> doc2Objects;
    doc2Objects["ugene_gui_test"] /*<< "et0002_features"*/ << "view_test_0001";
    GTUtilsDialog::waitForDialog(os, new ProjectTreeItemSelectorDialogFiller(os, doc2Objects, QSet<GObjectType>(),
        ProjectTreeItemSelectorDialogFiller::Separate));

    QWidget *addFromDbButton = GTWidget::findWidget(os, "addFromDbButton");
    GTWidget::click(os, addFromDbButton);

    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();

    QString path = sandBoxDir + "save_uwl_gui_test_0001.uwl";
    GTUtilsDialog::waitForDialog(os, new WorkflowMetaDialogFiller(os, path, "New workflow"));
    GTWidget::click(os, GTAction::button(os, "Save workflow action"));

    GTUtilsLog::check(os, l);
}

GUI_TEST_CLASS_DEFINITION(run_workflow_gui_test_0001_1) {
    GTUtilsDialog::waitForDialogWhichMayRunOrNot(os, new StartupDialogFiller(os));
    GTFileDialog::openFile(os, testDir + "_common_data/workflow/", "read_from_shared_db_no_credentials.uwl");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsWorkflowDesigner::click(os, "Write Alignment");
    GTUtilsWorkflowDesigner::setParameter(os, "Output file", "output.aln", GTUtilsWorkflowDesigner::textValue);

    GTUtilsDialog::waitForDialog(os, new AuthenticationDialogFiller(os, "invalid_user", "pass"));
    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok,
        "Please fix issues listed in the error list (located under workflow)."));

    GTWidget::click(os, GTAction::button(os, "Run workflow"));
    GTGlobals::sleep(5000);

    GTUtilsWorkflowDesigner::checkErrorList(os, "Unable to connect to the database");
}

GUI_TEST_CLASS_DEFINITION(run_workflow_gui_test_0001_2) {
    GTLogTracer l;

    GTUtilsDialog::waitForDialogWhichMayRunOrNot(os, new StartupDialogFiller(os));
    GTFileDialog::openFile(os, testDir + "_common_data/workflow/", "read_from_shared_db_no_credentials.uwl");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsWorkflowDesigner::click(os, "Write Alignment");
    GTUtilsWorkflowDesigner::setParameter(os, "Output file", "output.aln", GTUtilsWorkflowDesigner::textValue);

    GTUtilsDialog::waitForDialog(os, new AuthenticationDialogFiller(os, GTDatabaseConfig::login(), GTDatabaseConfig::password()));

    GTWidget::click(os, GTAction::button(os, "Run workflow"));
    GTGlobals::sleep(5000);

    GTUtilsLog::check(os, l);
}

GUI_TEST_CLASS_DEFINITION(run_workflow_gui_test_0002) {
    GTUtilsDialog::waitForDialogWhichMayRunOrNot(os, new StartupDialogFiller(os));
    GTFileDialog::openFile(os, testDir + "_common_data/workflow/", "read_from_inaccessible_shared_db.uwl");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsWorkflowDesigner::click(os, "Write Variations");
    GTUtilsWorkflowDesigner::setParameter(os, "Output file", "output.snp", GTUtilsWorkflowDesigner::textValue);

    GTUtilsDialog::waitForDialog(os, new AuthenticationDialogFiller(os, GTDatabaseConfig::login(), GTDatabaseConfig::password()));
    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok,
        "Please fix issues listed in the error list (located under workflow)."));

    GTWidget::click(os, GTAction::button(os, "Run workflow"));
    GTGlobals::sleep(25000);

    GTUtilsWorkflowDesigner::checkErrorList(os, "Unable to connect to the database");
}

GUI_TEST_CLASS_DEFINITION(run_workflow_gui_test_0003) {
    GTLogTracer l;

    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsWorkflowDesigner::addAlgorithm(os, "Read Annotations");

    createTestConnection(os);

    GTUtilsDialog::waitForDialog(os, new ProjectTreeItemSelectorDialogFiller(os, "ugene_gui_test", "et0002_features"));

    QWidget *addFromDbButton = GTWidget::findWidget(os, "addFromDbButton");
    GTWidget::click(os, addFromDbButton);

    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();

    GTUtilsWorkflowDesigner::addAlgorithm(os, "Write Annotations");
    GTUtilsWorkflowDesigner::setParameter(os, "Data storage", 1, GTUtilsWorkflowDesigner::comboValue);
    GTUtilsWorkflowDesigner::setParameter(os, "Database", "ugene_gui_test", GTUtilsWorkflowDesigner::comboValue);
    GTUtilsWorkflowDesigner::setParameter(os, "Output path", "/test", GTUtilsWorkflowDesigner::textValue);
    GTUtilsWorkflowDesigner::setParameter(os, "Merge annotation tables", true, GTUtilsWorkflowDesigner::comboValue);
    GTUtilsWorkflowDesigner::setParameter(os, "Annotation object name", "run_workflow_gui_test_0003", GTUtilsWorkflowDesigner::textValue);

    GTUtilsWorkflowDesigner::connect(os, GTUtilsWorkflowDesigner::getWorker(os, "Read Annotations"),
        GTUtilsWorkflowDesigner::getWorker(os, "Write Annotations"));

    GTWidget::click(os, GTAction::button(os, "Run workflow"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsLog::check(os, l);

    GTGlobals::sleep(11000);

    GTUtilsProjectTreeView::findIndex(os, "run_workflow_gui_test_0003");
}

GUI_TEST_CLASS_DEFINITION(run_workflow_gui_test_0004) {
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    GTUtilsWorkflowDesigner::addAlgorithm(os, "Read Plain Text");
    GTUtilsWorkflowDesigner::addAlgorithm(os, "Write Plain Text");
    GTUtilsWorkflowDesigner::connect(os, GTUtilsWorkflowDesigner::getWorker(os, "Read Plain Text"),
    GTUtilsWorkflowDesigner::getWorker(os, "Write Plain Text"));

    GTUtilsWorkflowDesigner::click(os, "Read Plain Text");
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir + "_common_data/text/text.txt");

    GTUtilsWorkflowDesigner::click(os, "Write Plain Text");
    GTUtilsWorkflowDesigner::setParameter(os, "Data storage", 1, GTUtilsWorkflowDesigner::comboValue);
    GTUtilsWorkflowDesigner::setParameter(os, "Database", 0, GTUtilsWorkflowDesigner::comboValue);

    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok,
        "Please fix issues listed in the error list (located under workflow)."));

    GTWidget::click(os, GTAction::button(os, "Run workflow"));
    GTGlobals::sleep(10000); //neccessary sleep

    GTUtilsWorkflowDesigner::checkErrorList(os, "You do not have write permissions to the database");
}

GUI_TEST_CLASS_DEFINITION(run_workflow_gui_test_0005_1) {
    GTLogTracer l;

    GTUtilsDialog::waitForDialogWhichMayRunOrNot(os, new StartupDialogFiller(os));
    GTFileDialog::openFile(os, testDir + "_common_data/workflow/", "write_to_shared_db_no_credentials.uwl");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsWorkflowDesigner::click(os, "Read Sequence");
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir + "_common_data/fasta/aaa.fa");

    GTUtilsDialog::waitForDialog(os, new AuthenticationDialogFiller(os, GTDatabaseConfig::login(), GTDatabaseConfig::password()));
    GTWidget::click(os, GTAction::button(os, "Run workflow"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsLog::check(os, l);
}

GUI_TEST_CLASS_DEFINITION(run_workflow_gui_test_0005_2) {
    GTUtilsDialog::waitForDialogWhichMayRunOrNot(os, new StartupDialogFiller(os));
    GTFileDialog::openFile(os, testDir + "_common_data/workflow/", "write_to_shared_db_no_credentials.uwl");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsWorkflowDesigner::click(os, "Read Sequence");
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir + "_common_data/fasta/aaa.fa");

    GTUtilsDialog::waitForDialog(os, new AuthenticationDialogFiller(os, GTDatabaseConfig::login(), "invalid_password"));
    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok,
        "Please fix issues listed in the error list (located under workflow)."));

    GTWidget::click(os, GTAction::button(os, "Run workflow"));

    GTUtilsWorkflowDesigner::checkErrorList(os, "Unable to connect to the database");
}

GUI_TEST_CLASS_DEFINITION(run_workflow_gui_test_0006) {
    GTUtilsDialog::waitForDialogWhichMayRunOrNot(os, new StartupDialogFiller(os));
    GTFileDialog::openFile(os, testDir + "_common_data/workflow/", "write_to_inaccessible_shared_db.uwl");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsWorkflowDesigner::click(os, "Read Assembly");
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir + "_common_data/ugenedb/1.bam.ugenedb");

    GTUtilsDialog::waitForDialog(os, new AuthenticationDialogFiller(os, GTDatabaseConfig::login(), GTDatabaseConfig::password()));
    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok,
        "Please fix issues listed in the error list (located under workflow)."));

    GTWidget::click(os, GTAction::button(os, "Run workflow"));
    GTGlobals::sleep();

    GTUtilsWorkflowDesigner::checkErrorList(os, "Unable to connect to the database");

    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Discard));
    GTUtilsMdi::click(os, GTGlobals::Close);
    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(test_3726) {
//    1. Open WD.
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsWorkflowDesigner::addAlgorithm(os, "Read Sequence", true);
//    2. Add "Read Sequence" to the scene.
    createTestConnection(os);
//    3. Press "Add data from shared databases" on the property editor.
//    Expected state: the "Shared Database Connections" dialog has appeared.
//    4. Double mouse click on "UGENE public database" (or any other with sequences stored in it).
//    Expected state: the dialog disappeared. In a few seconds DB has been connected and the "Select Item" dialog has appeared.
    QSet<GObjectType> acceptableTypes;
    acceptableTypes << GObjectTypes::SEQUENCE;
    GTUtilsDialog::waitForDialog(os, new ProjectTreeItemSelectorDialogFiller(os, "ugene_gui_test", "et0001_sequence", acceptableTypes));

    QWidget *addFromDbButton = GTWidget::findWidget(os, "addFromDbButton");
    GTWidget::click(os, addFromDbButton);

    GTUtilsTaskTreeView::waitTaskFinished(os);
//    5. Choose "chrI" in "UGENE public database/genomes/C. elegans (ce6)" and press "OK".
//    Expected state: the "Read Sequence" worker has got the "chrI" blue link in its body.

//    6. Right click on the link.
    GTUtilsWorkflowDesigner::clickLink(os, "Read Sequence", Qt::RightButton);
//    Current state: a context menu appears with the "Open document(s)" item. Two successive error message boxes appear if you click the item.
//    Expected state: nothing happens.
}

} // GUITest_common_scenarios_shared_db_wd
} // U2
