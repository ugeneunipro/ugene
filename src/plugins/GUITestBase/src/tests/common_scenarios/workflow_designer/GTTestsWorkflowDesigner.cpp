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

#include <GTGlobals.h>
#include <base_dialogs/GTFileDialog.h>
#include <base_dialogs/MessageBoxFiller.h>
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTAction.h>
#include <primitives/GTMenu.h>
#include <primitives/GTTableView.h>
#include <primitives/GTWidget.h>
#include <system/GTFile.h>

#include <QApplication>
#include <QDesktopWidget>
#include <QDir>
#include <QFileInfo>
#include <QGraphicsItem>
#include <QScreen>

#include <U2Core/AppContext.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/ToolsMenu.h>

// TODO: GUI test plugin depends on another plugin!
#include "../../workflow_designer/src/WorkflowViewItems.h"
#include "GTTestsWorkflowDesigner.h"
#include "GTUtilsLog.h"
#include "GTUtilsMdi.h"
#include "GTUtilsProject.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsTaskTreeView.h"
#include "GTUtilsWorkflowDesigner.h"
#include "runnables/ugene/corelibs/U2Gui/AppSettingsDialogFiller.h"
#include "runnables/ugene/plugins/external_tools/SnpEffDatabaseDialogFiller.h"
#include "runnables/ugene/plugins/workflow_designer/AliasesDialogFiller.h"
#include "runnables/ugene/plugins/workflow_designer/StartupDialogFiller.h"
#include "runnables/ugene/ugeneui/SequenceReadingModeSelectorDialogFiller.h"

namespace U2 {

// 8 - text
// 65536 - frame without ports
// 65537 - frame with ports
// 65538 - ports

namespace GUITest_common_scenarios_workflow_designer {
using namespace HI;

GUI_TEST_CLASS_DEFINITION(test_0002_1) {
    GTUtilsDialog::waitForDialog(new StartupDialogFiller());
    // 1. Start UGENE. Open workflow schema file from data\cmdline\pfm-build.uws
    GTFileDialog::openFile(dataDir + "cmdline/", "pwm-build.uwl");
    GTUtilsTaskTreeView::waitTaskFinished();
    //  Expected state: workflow schema opened in Workflow designer
    //    2. Change item style (Minimal - Extended - Minimal - Extended)
    auto sceneView = GTWidget::findGraphicsView("sceneView");
    QList<QGraphicsItem*> items = sceneView->items();
    QList<QPointF> posList;

    foreach (QGraphicsItem* item, items) {
        posList.append(item->pos());
    }

    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Write weight matrix"));
    GTMouseDriver::doubleClick();

    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Write weight matrix"));
    GTMouseDriver::doubleClick();

    //  Expected state: all arrows in schema still unbroken
    items = sceneView->items();
    foreach (QGraphicsItem* item, items) {
        QPointF p = posList.takeFirst();
        CHECK_SET_ERR(p == item->pos(), QString("some item changed position from %1, %2 to %3, %4").arg(p.x()).arg(p.y()).arg(item->pos().x()).arg(item->pos().y()));
    }
}

GUI_TEST_CLASS_DEFINITION(test_0003) {
    GTUtilsDialog::waitForDialog(new StartupDialogFiller());
    //    1. Start UGENE. Open workflow schema file from \common data\workflow\remoteDBReaderTest.uws
    GTFileDialog::openFile(testDir + "_common_data/workflow/", "remoteDBReaderTest.uws");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    Expected state: workflow schema opened in Workflow designer
    auto table = GTWidget::findTableView("table");
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Write Genbank"));
    GTMouseDriver::click();
    GTMouseDriver::moveTo(GTTableView::getCellPosition(table, 1, 3));
    GTMouseDriver::click();
    QString s = QDir().absoluteFilePath(testDir + "_common_data/scenarios/sandbox/");
    GTKeyboardDriver::keySequence(s + "T1.gb");
    GTWidget::click(GTUtilsMdi::activeWindow());

    GTWidget::click(GTAction::button("Run workflow"));
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. If you don't want result file (T1.gb) in UGENE run folder, change this property in write genbank worker.Run schema.
    //    Expected state: T1.gb file is saved to your disc
    GTFileDialog::openFile(testDir + "_common_data/scenarios/sandbox/", "T1.gb");
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_0005) {
    // 1. Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    // 2. Press button Validate schema
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok, "Nothing to run: empty workflow"));
    GTWidget::click(GTAction::button("Validate workflow"));
    // Expected state: message box which warns of validating empty schema has appeared
}

GUI_TEST_CLASS_DEFINITION(test_0006) {
    // 1. Do menu Settings->Preferences
    GTUtilsDialog::waitForDialog(new AppSettingsDialogFiller(AppSettingsDialogFiller::minimal));
    GTMenu::clickMainMenuItem({"Settings", "Preferences..."});

    // 2. Open WD settings
    // 3. Change Default visualization Item style from Extended to Minimal.
    // 4. Click OK button

    // 5. Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    // 6. Load any scheme from samples tab
    GTUtilsWorkflowDesigner::addAlgorithm("read alignment");
    // Expected state: item style on loaded schema must be Minimal
    StyleId id;
    auto sceneView = GTWidget::findGraphicsView("sceneView");
    QList<QGraphicsItem*> items = sceneView->items();
    foreach (QGraphicsItem* item, items) {
        auto s = qgraphicsitem_cast<WorkflowProcessItem*>(item);
        if (s) {
            id = s->getStyle();
            CHECK_SET_ERR(id == "simple", "items style is not minimal");
        }
    }
}

GUI_TEST_CLASS_DEFINITION(test_0006_1) {
    // 1. Do menu Settings->Preferences
    GTUtilsDialog::waitForDialog(new AppSettingsDialogFiller(AppSettingsDialogFiller::extended));
    GTMenu::clickMainMenuItem({"Settings", "Preferences..."});

    // 2. Open WD settings
    // 3. Change Default visualization Item style from Extended to Minimal.
    // 4. Click OK button

    // 5. Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    // 6. Load any scheme from samples tab
    GTUtilsWorkflowDesigner::addAlgorithm("read alignment");
    // Expected state: item style on loaded schema must be Minimal
    StyleId id;
    auto sceneView = GTWidget::findGraphicsView("sceneView");
    QList<QGraphicsItem*> items = sceneView->items();
    foreach (QGraphicsItem* item, items) {
        auto s = qgraphicsitem_cast<WorkflowProcessItem*>(item);
        if (s) {
            id = s->getStyle();
            CHECK_SET_ERR(id == "ext", "items style is not minimal");
        }
    }
}

GUI_TEST_CLASS_DEFINITION(test_0007) {
    // Activate WD preferences page. Change Background color for workers.
    GTUtilsDialog::waitForDialog(new AppSettingsDialogFiller(255, 0, 0));
    GTMenu::clickMainMenuItem({"Settings", "Preferences..."});

    // Open WD and place any worker on working area.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    // Expected state: workers background color must be same as in preferences
    GTUtilsWorkflowDesigner::addAlgorithm("read alignment");
    QImage image = GTGlobals::takeScreenShot();

    QPoint samplePoint(GTUtilsWorkflowDesigner::getItemLeft("Read Alignment") + 10, GTUtilsWorkflowDesigner::getItemTop("Read Alignment") + 10);
    QRgb rgb = image.pixel(samplePoint);
    QColor color(rgb);
    CHECK_SET_ERR(color.name() == "#ffbfbf", QString("Expected: #ffbfbf, found: %1").arg(color.name()));
}

GUI_TEST_CLASS_DEFINITION(test_0009) {
    //    1. Open schema from examples
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsWorkflowDesigner::addSample("call variants");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
    //    2. Clear dashboard (select all + del button)
    auto sceneView = GTWidget::findGraphicsView("sceneView");
    QList<QGraphicsItem*> items = sceneView->items();
    QList<QPointF> posList;

    foreach (QGraphicsItem* item, items) {
        if (qgraphicsitem_cast<WorkflowProcessItem*>(item))
            posList.append(item->pos());
    }

    GTWidget::setFocus(GTWidget::findWidget("sceneView"));
    GTKeyboardDriver::keyClick('a', Qt::ControlModifier);
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    //    3. Open this schema from examples
    GTUtilsWorkflowDesigner::addSample("call variants");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
    //    Expected state: items and links between them painted correctly
    QList<QGraphicsItem*> items1 = sceneView->items();
    QList<QPointF> posList1;

    foreach (QGraphicsItem* item, items1) {
        if (qgraphicsitem_cast<WorkflowProcessItem*>(item))
            posList1.append(item->pos());
    }

    CHECK_SET_ERR(posList == posList1, "some workers changed positions");
}

GUI_TEST_CLASS_DEFINITION(test_0010) {
    //    1. Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    //    2. Place 3 HMM build workflow elements on scheme
    GTUtilsWorkflowDesigner::addAlgorithm("Read Sequence", true);
    GTUtilsWorkflowDesigner::addAlgorithm("Write Sequence", true);

    WorkflowProcessItem* read = GTUtilsWorkflowDesigner::getWorker("Read Sequence");
    WorkflowProcessItem* write = GTUtilsWorkflowDesigner::getWorker("Write Sequence");
    GTUtilsWorkflowDesigner::connect(read, write);
    /*GTUtilsWorkflowDesigner::addAlgorithm("hmm build");

    GTUtilsWorkflowDesigner::addAlgorithm("hmm build");

    GTUtilsWorkflowDesigner::addAlgorithm("hmm build");


//    Expected state: there 3 element with names "HMM build" "HMM build 1" "HMM build 2"
    QGraphicsItem* hmm = GTUtilsWorkflowDesigner::getWorker("hmm build");
    CHECK_SET_ERR(hmm,"hmm not found");
    hmm = GTUtilsWorkflowDesigner::getWorker("hmm build 1");
    CHECK_SET_ERR(hmm,"hmm 1 not found");
    hmm = GTUtilsWorkflowDesigner::getWorker("hmm build 2");
    CHECK_SET_ERR(hmm,"hmm 2 not found");*/
}

GUI_TEST_CLASS_DEFINITION(test_0013) {
    //    1. Load any sample in WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsWorkflowDesigner::addSample("call variants");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
    //    2. Select output port.
    WorkflowProcessItem* gr = GTUtilsWorkflowDesigner::getWorker("Call Variants");
    auto sceneView = GTWidget::findGraphicsView("sceneView");
    QList<WorkflowPortItem*> list = gr->getPortItems();
    foreach (WorkflowPortItem* p, list) {
        if (p && p->getPort()->getId() == "out-variations") {
            QPointF scenePButton = p->mapToScene(p->boundingRect().center());
            QPoint viewP = sceneView->mapFromScene(scenePButton);
            QPoint globalBottomRightPos = sceneView->viewport()->mapToGlobal(viewP);
            GTMouseDriver::moveTo(globalBottomRightPos);
            GTMouseDriver::click();
        }
    }
    auto doc = GTWidget::findTextEdit("doc");
    CHECK_SET_ERR(doc->document()->toPlainText().contains("Output port \"Output variations"), "expected text not found");

    //    Expected state: in property editor 'Output port' item appears

    //    3. Select input port.
    WorkflowPortItem* in = GTUtilsWorkflowDesigner::getPortById(gr, "in-assembly");
    QPointF scenePButton = in->mapToScene(in->boundingRect().center());
    QPoint viewP = sceneView->mapFromScene(scenePButton);
    QPoint globalBottomRightPos = sceneView->viewport()->mapToGlobal(viewP);
    GTMouseDriver::moveTo(globalBottomRightPos);
    GTMouseDriver::click();

    doc = GTWidget::findTextEdit("doc");
    CHECK_SET_ERR(doc->document()->toPlainText().contains("Input port \"Input assembly"), "expected text not found");
    //    Expected state: in property editor 'Input port' item appears
}

GUI_TEST_CLASS_DEFINITION(test_0015) {
    //    1. open WD.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    //    2. Select any worker on palette.
    GTUtilsWorkflowDesigner::addSample("call variants");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Call Variants"));
    GTMouseDriver::click();
    CHECK_SET_ERR(GTWidget::findWidget("table"), "parameters table not found");
    CHECK_SET_ERR(GTWidget::findWidget("doc"), "element documentation widget not found");
    CHECK_SET_ERR(GTWidget::findWidget("inputScrollArea"), "input data table not found");
    CHECK_SET_ERR(GTWidget::findWidget("propDoc"), "property documentation widget not found");

    //    Expected state: Actor info (parameters, input data ...) will be displayed at the right part of window
}

GUI_TEST_CLASS_DEFINITION(test_0015_1) {  // DIFFERENCE:file is loaded
    GTUtilsDialog::waitForDialog(new StartupDialogFiller());
    //    1. open WD.
    GTFileDialog::openFile(dataDir + "cmdline/", "pwm-build.uwl");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Select any worker on palette.
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Write Weight Matrix"));
    GTMouseDriver::click();
    CHECK_SET_ERR(GTWidget::findWidget("table"), "parameters table not found");
    CHECK_SET_ERR(GTWidget::findWidget("doc"), "element documentation widget not found");
    CHECK_SET_ERR(GTWidget::findWidget("table2"), "input data table not found");
    CHECK_SET_ERR(GTWidget::findWidget("propDoc"), "property documentation widget not found");

    //    Expected state: Actor info (parameters, input data ...) will be displayed at the right part of window
}
GUI_TEST_CLASS_DEFINITION(test_0016) {
    //    1. open WD.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    //    2. Place Read align element on schema
    GTUtilsWorkflowDesigner::addAlgorithm("read alignment");
    //    3. Press button "Configure command line aliases"
    QMap<QPoint*, QString> map;
    QPoint p(1, 0);
    // map.i
    map[&p] = "qqq";
    // map.insert(p,QString("qqq"));
    GTUtilsDialog::waitForDialog(new AliasesDialogFiller(map));
    GTWidget::click(GTAction::button("Configure parameter aliases"));
    //    4. Add command line alias 'qqq' for schema parameter 'Input files'

    //    5. Save schema.

    //    6. Press button "Configure command line aliases"

    //    7. Change command line alias from 'qqq' to 'zzz'

    //    8. Save schema.

    //    9 Close and open this schema again.

    //    10. Press button "Configure command line aliases"
    //    Expected state: alias must be named 'zzz'
}

GUI_TEST_CLASS_DEFINITION(test_0017) {
    // Test for UGENE-2202
    GTLogTracer lt;
    GTUtilsDialog::waitForDialog(new StartupDialogFiller(testDir + "_common_data/scenarios/sandbox/somedir"));
    // 1. Open Workflow Designer
    GTMenu::clickMainMenuItem({"Tools", "Workflow Designer..."});

    // 2. Write the path to the folder which does not exist(in the StartupDialogFiller).
    // 3. Click OK(in the StartupDialogFiller).
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_0058) {
    // 1. Click the menu {File -> New workflow}
    // Expected: Workflow Designer is opened.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    QWidget* wdView = GTUtilsMdi::activeWindow();

    CHECK_SET_ERR(wdView->objectName() == "Workflow Designer", "Wrong mdi window " + wdView->objectName());
}

GUI_TEST_CLASS_DEFINITION(test_0059) {
    // Test for UGENE-1505
    // 1. Open WD
    // 2. Create scheme: Read sequence --> Get seq.by annotation --> Write sequence
    // 3. Input data: sars.gb
    // 4. Run workflow
    // 5. Open result file
    // Expected state: all sequence objects has the corresponding region in its name
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    WorkflowProcessItem* readSeq = GTUtilsWorkflowDesigner::addElement("Read Sequence", true);
    WorkflowProcessItem* seqByAnns = GTUtilsWorkflowDesigner::addElement("Get Sequences by Annotations", true);
    WorkflowProcessItem* writeSeq = GTUtilsWorkflowDesigner::addElement("Write Sequence", true);

    GTUtilsWorkflowDesigner::connect(readSeq, seqByAnns);
    GTUtilsWorkflowDesigner::connect(seqByAnns, writeSeq);

    GTUtilsWorkflowDesigner::addInputFile("Read Sequence", dataDir + "/samples/Genbank/sars.gb");
    GTUtilsWorkflowDesigner::click("Write Sequence");
    GTUtilsWorkflowDesigner::setParameter("Output file", QDir().absoluteFilePath(sandBoxDir) + "wd_test_0059.fa", GTUtilsWorkflowDesigner::textValue);

    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new SequenceReadingModeSelectorDialogFiller(SequenceReadingModeSelectorDialogFiller::Separate));
    GTUtilsProject::openFile(sandBoxDir + "wd_test_0059.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    CHECK_SET_ERR(GTUtilsProjectTreeView::checkItem("NC_004718 1..29751 source"), "Sequence not found");
    CHECK_SET_ERR(GTUtilsProjectTreeView::checkItem("NC_004718 27638..27772 gene"), "Sequence not found");
}

GUI_TEST_CLASS_DEFINITION(test_0060) {
    //    UGENE-3703
    //    1. Open "Intersect annotations" sample
    //    2. Input data: "_common_data/bedtools/introns.bed" (A), "_common_data/bedtool/mutations.gff" (B)
    //    3. Run workflow
    //    4. Open result file
    //    Expected state: sample works as it stated (the result of default run(format should be BED) on that data is "_common_data/bedtools/out17.bed"

    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsWorkflowDesigner::addSample("Intersect annotations");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    GTUtilsWorkflowDesigner::click("Read Annotations A");
    GTUtilsWorkflowDesigner::setDatasetInputFile(testDir + "/_common_data/bedtools/introns.bed");

    GTUtilsWorkflowDesigner::click("Read Annotations B");
    GTUtilsWorkflowDesigner::setDatasetInputFile(testDir + "/_common_data/bedtools/mutation.gff");

    GTUtilsWorkflowDesigner::click("Write Annotations");
    GTUtilsWorkflowDesigner::setParameter("Document format", "BED", GTUtilsWorkflowDesigner::comboValue);
    QString s = QFileInfo(testDir + "_common_data/scenarios/sandbox").absoluteFilePath();
    GTUtilsWorkflowDesigner::setParameter("Output file", QVariant(s + "/wd_test_0060"), GTUtilsWorkflowDesigner::textValue);

    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();

    CHECK_SET_ERR(GTFile::equals(QDir(sandBoxDir).absolutePath() + "/wd_test_0060", QDir(testDir).absolutePath() + "/_common_data/bedtools/out17.bed"), "Output is incorrect");
}

GUI_TEST_CLASS_DEFINITION(test_0061) {
    // UGENE-5162
    // 1. Open WD
    // 2. Add element "Call variants with SAM tools" on the scene
    // Expected state: the parameter "Use reference from" is set to File, the element has one port and parameter "Reference"
    // 3. Open "Call variants with SAM tools" sample
    // Expected state: "Call variants" element has two ports, "Use reference from" is set to "Port", there is no parameter "Reference"
    // 4. Set "Use reference from" to "Port"
    // Expected state: the second port and its link disappeared
    // 5. Remove "Read seqeunce" elements
    // 6. Set input data:
    //    Reference: /sampls/Assembly/chrM.fa
    //    Input assembly dataset 1: /sampls/Assembly/chrM.sam
    //    Input assembly dataset 2: /sampls/Assembly/chrM.sam
    // 7. Run the workflow
    // Expected state: there are two result files (for each dataset)

    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    WorkflowProcessItem* item = GTUtilsWorkflowDesigner::addElement("Call Variants with SAMtools");
    CHECK_SET_ERR(item != nullptr, "Failed to add Call variants element");
    GTUtilsWorkflowDesigner::click("Call Variants with SAMtools");

    WorkflowPortItem* port = GTUtilsWorkflowDesigner::getPortById(item, "in-sequence");
    CHECK_SET_ERR(port != nullptr, "Cannot get in-sequence port 1");
    CHECK_SET_ERR(!port->isVisible(), "In-sequence port is unexpectedly visible");
    CHECK_SET_ERR(GTUtilsWorkflowDesigner::isParameterVisible("Reference"), "Reference parameter is not visible");

    GTUtilsWorkflowDesigner::addSample("Call variants with SAMtools");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
    GTUtilsWorkflowDesigner::click("Call Variants");
    CHECK_SET_ERR(!GTUtilsWorkflowDesigner::isParameterVisible("Reference"), "Reference parameter is unexpectedly visible");
    item = GTUtilsWorkflowDesigner::getWorker("Call Variants");
    CHECK_SET_ERR(item != nullptr, "Cannot find Call variants with SAMtools element");
    port = GTUtilsWorkflowDesigner::getPortById(item, "in-sequence");
    CHECK_SET_ERR(port != nullptr, "Cannot get in-sequence port 2");
    CHECK_SET_ERR(port->isVisible(), "In-sequence port is enexpectedly not visible");

    GTUtilsWorkflowDesigner::removeItem("Read Sequence");
    GTUtilsWorkflowDesigner::removeItem("To FASTA");

    GTUtilsWorkflowDesigner::click("Call Variants");
    GTUtilsWorkflowDesigner::setParameter("Use reference from", "File", GTUtilsWorkflowDesigner::comboValue);
    GTUtilsWorkflowDesigner::setParameter("Reference", QDir().absoluteFilePath(dataDir + "samples/Assembly/chrM.fa"), GTUtilsWorkflowDesigner::lineEditWithFileSelector);
    GTUtilsWorkflowDesigner::setParameter("Output variants file", QDir().absoluteFilePath(sandBoxDir + "/test_ugene_5162.vcf"), GTUtilsWorkflowDesigner::lineEditWithFileSelector);

    GTUtilsWorkflowDesigner::click("Read Assembly (BAM/SAM)");
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "/samples/Assembly/chrM.sam");

    GTUtilsWorkflowDesigner::createDataset();
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "/samples/Assembly/chrM.sam");
    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();

    CHECK_SET_ERR(GTFile::check(sandBoxDir + "/test_ugene_5162.vcf"), "No resut file 1");
    CHECK_SET_ERR(GTFile::check(sandBoxDir + "/test_ugene_5163.vcf"), "No resut file 2");
}

GUI_TEST_CLASS_DEFINITION(test_0062) {
    // Test for SnpEff genome parameter
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    WorkflowProcessItem* snpEffItem = GTUtilsWorkflowDesigner::addElement("SnpEff Annotation and Filtration");
    CHECK_SET_ERR(snpEffItem != nullptr, "Failed to add SnpEff Annotation and Filtration element");

    GTUtilsDialog::waitForDialog(new SnpEffDatabaseDialogFiller("hg19"));
    GTUtilsWorkflowDesigner::setParameter("Genome", QVariant(), GTUtilsWorkflowDesigner::customDialogSelector);

    GTUtilsDialog::waitForDialog(new SnpEffDatabaseDialogFiller("fake_snpeff_genome123", false));
    GTUtilsWorkflowDesigner::setParameter("Genome", QVariant(), GTUtilsWorkflowDesigner::customDialogSelector);
}

}  // namespace GUITest_common_scenarios_workflow_designer

}  // namespace U2
