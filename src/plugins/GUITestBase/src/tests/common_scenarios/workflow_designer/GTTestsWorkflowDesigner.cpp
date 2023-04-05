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
    GTUtilsDialog::waitForDialog(os, new StartupDialogFiller(os));
    // 1. Start UGENE. Open workflow schema file from data\cmdline\pfm-build.uws
    GTFileDialog::openFile(os, dataDir + "cmdline/", "pwm-build.uwl");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //  Expected state: workflow schema opened in Workflow designer
    //    2. Change item style (Minimal - Extended - Minimal - Extended)
    auto sceneView = GTWidget::findGraphicsView(os, "sceneView");
    QList<QGraphicsItem*> items = sceneView->items();
    QList<QPointF> posList;

    foreach (QGraphicsItem* item, items) {
        posList.append(item->pos());
    }

    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter(os, "Write weight matrix"));
    GTMouseDriver::doubleClick();

    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter(os, "Write weight matrix"));
    GTMouseDriver::doubleClick();

    //  Expected state: all arrows in schema still unbroken
    items = sceneView->items();
    foreach (QGraphicsItem* item, items) {
        QPointF p = posList.takeFirst();
        CHECK_SET_ERR(p == item->pos(), QString("some item changed position from %1, %2 to %3, %4").arg(p.x()).arg(p.y()).arg(item->pos().x()).arg(item->pos().y()));
    }
}

GUI_TEST_CLASS_DEFINITION(test_0003) {
    GTUtilsDialog::waitForDialog(os, new StartupDialogFiller(os));
    //    1. Start UGENE. Open workflow schema file from \common data\workflow\remoteDBReaderTest.uws
    GTFileDialog::openFile(os, testDir + "_common_data/workflow/", "remoteDBReaderTest.uws");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: workflow schema opened in Workflow designer
    auto table = GTWidget::findTableView(os, "table");
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter(os, "Write Genbank"));
    GTMouseDriver::click();
    GTMouseDriver::moveTo(GTTableView::getCellPosition(os, table, 1, 3));
    GTMouseDriver::click();
    QString s = QDir().absoluteFilePath(testDir + "_common_data/scenarios/sandbox/");
    GTKeyboardDriver::keySequence(s + "T1.gb");
    GTWidget::click(os, GTUtilsMdi::activeWindow(os));

    GTWidget::click(os, GTAction::button(os, "Run workflow"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. If you don't want result file (T1.gb) in UGENE run folder, change this property in write genbank worker.Run schema.
    //    Expected state: T1.gb file is saved to your disc
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/sandbox/", "T1.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
}

GUI_TEST_CLASS_DEFINITION(test_0005) {
    // 1. Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    // 2. Press button Validate schema
    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok, "Nothing to run: empty workflow"));
    GTWidget::click(os, GTAction::button(os, "Validate workflow"));
    // Expected state: message box which warns of validating empty schema has appeared
}

GUI_TEST_CLASS_DEFINITION(test_0006) {
    // 1. Do menu Settings->Preferences
    GTUtilsDialog::waitForDialog(os, new AppSettingsDialogFiller(os, AppSettingsDialogFiller::minimal));
    GTMenu::clickMainMenuItem(os, {"Settings", "Preferences..."});

    // 2. Open WD settings
    // 3. Change Default visualization Item style from Extended to Minimal.
    // 4. Click OK button

    // 5. Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    // 6. Load any scheme from samples tab
    GTUtilsWorkflowDesigner::addAlgorithm(os, "read alignment");
    // Expected state: item style on loaded schema must be Minimal
    StyleId id;
    auto sceneView = GTWidget::findGraphicsView(os, "sceneView");
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
    GTUtilsDialog::waitForDialog(os, new AppSettingsDialogFiller(os, AppSettingsDialogFiller::extended));
    GTMenu::clickMainMenuItem(os, {"Settings", "Preferences..."});

    // 2. Open WD settings
    // 3. Change Default visualization Item style from Extended to Minimal.
    // 4. Click OK button

    // 5. Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    // 6. Load any scheme from samples tab
    GTUtilsWorkflowDesigner::addAlgorithm(os, "read alignment");
    // Expected state: item style on loaded schema must be Minimal
    StyleId id;
    auto sceneView = GTWidget::findGraphicsView(os, "sceneView");
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
    GTUtilsDialog::waitForDialog(os, new AppSettingsDialogFiller(os, 255, 0, 0));
    GTMenu::clickMainMenuItem(os, {"Settings", "Preferences..."});

    // Open WD and place any worker on working area.
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    // Expected state: workers background color must be same as in preferences
    GTUtilsWorkflowDesigner::addAlgorithm(os, "read alignment");
    QImage image = GTGlobals::takeScreenShot(os);

    QPoint samplePoint(GTUtilsWorkflowDesigner::getItemLeft(os, "Read Alignment") + 10, GTUtilsWorkflowDesigner::getItemTop(os, "Read Alignment") + 10);
    QRgb rgb = image.pixel(samplePoint);
    QColor color(rgb);
    CHECK_SET_ERR(color.name() == "#ffbfbf", QString("Expected: #ffbfbf, found: %1").arg(color.name()));
}

GUI_TEST_CLASS_DEFINITION(test_0009) {
    //    1. Open schema from examples
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsWorkflowDesigner::addSample(os, "call variants");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
    //    2. Clear dashboard (select all + del button)
    auto sceneView = GTWidget::findGraphicsView(os, "sceneView");
    QList<QGraphicsItem*> items = sceneView->items();
    QList<QPointF> posList;

    foreach (QGraphicsItem* item, items) {
        if (qgraphicsitem_cast<WorkflowProcessItem*>(item))
            posList.append(item->pos());
    }

    GTWidget::setFocus(os, GTWidget::findWidget(os, "sceneView"));
    GTKeyboardDriver::keyClick('a', Qt::ControlModifier);
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    //    3. Open this schema from examples
    GTUtilsWorkflowDesigner::addSample(os, "call variants");
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
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    //    2. Place 3 HMM build workflow elements on scheme
    GTUtilsWorkflowDesigner::addAlgorithm(os, "Read Sequence", true);
    GTUtilsWorkflowDesigner::addAlgorithm(os, "Write Sequence", true);

    WorkflowProcessItem* read = GTUtilsWorkflowDesigner::getWorker(os, "Read Sequence");
    WorkflowProcessItem* write = GTUtilsWorkflowDesigner::getWorker(os, "Write Sequence");
    GTUtilsWorkflowDesigner::connect(os, read, write);
    /*GTUtilsWorkflowDesigner::addAlgorithm(os,"hmm build");

    GTUtilsWorkflowDesigner::addAlgorithm(os,"hmm build");

    GTUtilsWorkflowDesigner::addAlgorithm(os,"hmm build");


//    Expected state: there 3 element with names "HMM build" "HMM build 1" "HMM build 2"
    QGraphicsItem* hmm = GTUtilsWorkflowDesigner::getWorker(os,"hmm build");
    CHECK_SET_ERR(hmm,"hmm not found");
    hmm = GTUtilsWorkflowDesigner::getWorker(os,"hmm build 1");
    CHECK_SET_ERR(hmm,"hmm 1 not found");
    hmm = GTUtilsWorkflowDesigner::getWorker(os,"hmm build 2");
    CHECK_SET_ERR(hmm,"hmm 2 not found");*/
}

GUI_TEST_CLASS_DEFINITION(test_0013) {
    //    1. Load any sample in WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsWorkflowDesigner::addSample(os, "call variants");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
    //    2. Select output port.
    WorkflowProcessItem* gr = GTUtilsWorkflowDesigner::getWorker(os, "Call Variants");
    auto sceneView = GTWidget::findGraphicsView(os, "sceneView");
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
    auto doc = GTWidget::findTextEdit(os, "doc");
    CHECK_SET_ERR(doc->document()->toPlainText().contains("Output port \"Output variations"), "expected text not found");

    //    Expected state: in property editor 'Output port' item appears

    //    3. Select input port.
    WorkflowPortItem* in = GTUtilsWorkflowDesigner::getPortById(os, gr, "in-assembly");
    QPointF scenePButton = in->mapToScene(in->boundingRect().center());
    QPoint viewP = sceneView->mapFromScene(scenePButton);
    QPoint globalBottomRightPos = sceneView->viewport()->mapToGlobal(viewP);
    GTMouseDriver::moveTo(globalBottomRightPos);
    GTMouseDriver::click();

    doc = GTWidget::findTextEdit(os, "doc");
    CHECK_SET_ERR(doc->document()->toPlainText().contains("Input port \"Input assembly"), "expected text not found");
    //    Expected state: in property editor 'Input port' item appears
}

GUI_TEST_CLASS_DEFINITION(test_0015) {
    //    1. open WD.
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    //    2. Select any worker on palette.
    GTUtilsWorkflowDesigner::addSample(os, "call variants");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter(os, "Call Variants"));
    GTMouseDriver::click();
    CHECK_SET_ERR(GTWidget::findWidget(os, "table"), "parameters table not found");
    CHECK_SET_ERR(GTWidget::findWidget(os, "doc"), "element documentation widget not found");
    CHECK_SET_ERR(GTWidget::findWidget(os, "inputScrollArea"), "input data table not found");
    CHECK_SET_ERR(GTWidget::findWidget(os, "propDoc"), "property documentation widget not found");

    //    Expected state: Actor info (parameters, input data ...) will be displayed at the right part of window
}

GUI_TEST_CLASS_DEFINITION(test_0015_1) {  // DIFFERENCE:file is loaded
    GTUtilsDialog::waitForDialog(os, new StartupDialogFiller(os));
    //    1. open WD.
    GTFileDialog::openFile(os, dataDir + "cmdline/", "pwm-build.uwl");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Select any worker on palette.
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter(os, "Write Weight Matrix"));
    GTMouseDriver::click();
    CHECK_SET_ERR(GTWidget::findWidget(os, "table"), "parameters table not found");
    CHECK_SET_ERR(GTWidget::findWidget(os, "doc"), "element documentation widget not found");
    CHECK_SET_ERR(GTWidget::findWidget(os, "table2"), "input data table not found");
    CHECK_SET_ERR(GTWidget::findWidget(os, "propDoc"), "property documentation widget not found");

    //    Expected state: Actor info (parameters, input data ...) will be displayed at the right part of window
}
GUI_TEST_CLASS_DEFINITION(test_0016) {
    //    1. open WD.
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    //    2. Place Read align element on schema
    GTUtilsWorkflowDesigner::addAlgorithm(os, "read alignment");
    //    3. Press button "Configure command line aliases"
    QMap<QPoint*, QString> map;
    QPoint p(1, 0);
    // map.i
    map[&p] = "qqq";
    // map.insert(p,QString("qqq"));
    GTUtilsDialog::waitForDialog(os, new AliasesDialogFiller(os, map));
    GTWidget::click(os, GTAction::button(os, "Configure parameter aliases"));
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
    GTUtilsDialog::waitForDialog(os, new StartupDialogFiller(os, testDir + "_common_data/scenarios/sandbox/somedir"));
    // 1. Open Workflow Designer
    GTMenu::clickMainMenuItem(os, {"Tools", "Workflow Designer..."});

    // 2. Write the path to the folder which does not exist(in the StartupDialogFiller).
    // 3. Click OK(in the StartupDialogFiller).
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_0058) {
    // 1. Click the menu {File -> New workflow}
    // Expected: Workflow Designer is opened.
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    QWidget* wdView = GTUtilsMdi::activeWindow(os);
    CHECK_OP(os, );
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
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    WorkflowProcessItem* readSeq = GTUtilsWorkflowDesigner::addElement(os, "Read Sequence", true);
    WorkflowProcessItem* seqByAnns = GTUtilsWorkflowDesigner::addElement(os, "Get Sequences by Annotations", true);
    WorkflowProcessItem* writeSeq = GTUtilsWorkflowDesigner::addElement(os, "Write Sequence", true);

    GTUtilsWorkflowDesigner::connect(os, readSeq, seqByAnns);
    GTUtilsWorkflowDesigner::connect(os, seqByAnns, writeSeq);

    GTUtilsWorkflowDesigner::addInputFile(os, "Read Sequence", dataDir + "/samples/Genbank/sars.gb");
    GTUtilsWorkflowDesigner::click(os, "Write Sequence");
    GTUtilsWorkflowDesigner::setParameter(os, "Output file", QDir().absoluteFilePath(sandBoxDir) + "wd_test_0059.fa", GTUtilsWorkflowDesigner::textValue);

    GTUtilsWorkflowDesigner::runWorkflow(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os, SequenceReadingModeSelectorDialogFiller::Separate));
    GTUtilsProject::openFile(os, sandBoxDir + "wd_test_0059.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    CHECK_SET_ERR(GTUtilsProjectTreeView::checkItem(os, "NC_004718 1..29751 source"), "Sequence not found");
    CHECK_SET_ERR(GTUtilsProjectTreeView::checkItem(os, "NC_004718 27638..27772 gene"), "Sequence not found");
}

GUI_TEST_CLASS_DEFINITION(test_0060) {
    //    UGENE-3703
    //    1. Open "Intersect annotations" sample
    //    2. Input data: "_common_data/bedtools/introns.bed" (A), "_common_data/bedtool/mutations.gff" (B)
    //    3. Run workflow
    //    4. Open result file
    //    Expected state: sample works as it stated (the result of default run(format should be BED) on that data is "_common_data/bedtools/out17.bed"

    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsWorkflowDesigner::addSample(os, "Intersect annotations");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    GTUtilsWorkflowDesigner::click(os, "Read Annotations A");
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir + "/_common_data/bedtools/introns.bed");

    GTUtilsWorkflowDesigner::click(os, "Read Annotations B");
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir + "/_common_data/bedtools/mutation.gff");

    GTUtilsWorkflowDesigner::click(os, "Write Annotations");
    GTUtilsWorkflowDesigner::setParameter(os, "Document format", "BED", GTUtilsWorkflowDesigner::comboValue);
    QString s = QFileInfo(testDir + "_common_data/scenarios/sandbox").absoluteFilePath();
    GTUtilsWorkflowDesigner::setParameter(os, "Output file", QVariant(s + "/wd_test_0060"), GTUtilsWorkflowDesigner::textValue);

    GTUtilsWorkflowDesigner::runWorkflow(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    CHECK_SET_ERR(GTFile::equals(os, QDir(sandBoxDir).absolutePath() + "/wd_test_0060", QDir(testDir).absolutePath() + "/_common_data/bedtools/out17.bed"), "Output is incorrect");
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

    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    WorkflowProcessItem* item = GTUtilsWorkflowDesigner::addElement(os, "Call Variants with SAMtools");
    CHECK_SET_ERR(item != nullptr, "Failed to add Call variants element");
    GTUtilsWorkflowDesigner::click(os, "Call Variants with SAMtools");

    WorkflowPortItem* port = GTUtilsWorkflowDesigner::getPortById(os, item, "in-sequence");
    CHECK_SET_ERR(port != nullptr, "Cannot get in-sequence port 1");
    CHECK_SET_ERR(!port->isVisible(), "In-sequence port is unexpectedly visible");
    CHECK_SET_ERR(GTUtilsWorkflowDesigner::isParameterVisible(os, "Reference"), "Reference parameter is not visible");

    GTUtilsWorkflowDesigner::addSample(os, "Call variants with SAMtools");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
    GTUtilsWorkflowDesigner::click(os, "Call Variants");
    CHECK_SET_ERR(!GTUtilsWorkflowDesigner::isParameterVisible(os, "Reference"), "Reference parameter is unexpectedly visible");
    item = GTUtilsWorkflowDesigner::getWorker(os, "Call Variants");
    CHECK_SET_ERR(item != nullptr, "Cannot find Call variants with SAMtools element");
    port = GTUtilsWorkflowDesigner::getPortById(os, item, "in-sequence");
    CHECK_SET_ERR(port != nullptr, "Cannot get in-sequence port 2");
    CHECK_SET_ERR(port->isVisible(), "In-sequence port is enexpectedly not visible");

    GTUtilsWorkflowDesigner::removeItem(os, "Read Sequence");
    GTUtilsWorkflowDesigner::removeItem(os, "To FASTA");

    GTUtilsWorkflowDesigner::click(os, "Call Variants");
    GTUtilsWorkflowDesigner::setParameter(os, "Use reference from", "File", GTUtilsWorkflowDesigner::comboValue);
    GTUtilsWorkflowDesigner::setParameter(os, "Reference", QDir().absoluteFilePath(dataDir + "samples/Assembly/chrM.fa"), GTUtilsWorkflowDesigner::lineEditWithFileSelector);
    GTUtilsWorkflowDesigner::setParameter(os, "Output variants file", QDir().absoluteFilePath(sandBoxDir + "/test_ugene_5162.vcf"), GTUtilsWorkflowDesigner::lineEditWithFileSelector);

    GTUtilsWorkflowDesigner::click(os, "Read Assembly (BAM/SAM)");
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, dataDir + "/samples/Assembly/chrM.sam");

    GTUtilsWorkflowDesigner::createDataset(os);
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, dataDir + "/samples/Assembly/chrM.sam");
    GTUtilsWorkflowDesigner::runWorkflow(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    CHECK_SET_ERR(GTFile::check(os, sandBoxDir + "/test_ugene_5162.vcf"), "No resut file 1");
    CHECK_SET_ERR(GTFile::check(os, sandBoxDir + "/test_ugene_5163.vcf"), "No resut file 2");
}

GUI_TEST_CLASS_DEFINITION(test_0062) {
    // Test for SnpEff genome parameter
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    WorkflowProcessItem* snpEffItem = GTUtilsWorkflowDesigner::addElement(os, "SnpEff Annotation and Filtration");
    CHECK_SET_ERR(snpEffItem != nullptr, "Failed to add SnpEff Annotation and Filtration element");

    GTUtilsDialog::waitForDialog(os, new SnpEffDatabaseDialogFiller(os, "hg19"));
    GTUtilsWorkflowDesigner::setParameter(os, "Genome", QVariant(), GTUtilsWorkflowDesigner::customDialogSelector);

    GTUtilsDialog::waitForDialog(os, new SnpEffDatabaseDialogFiller(os, "fake_snpeff_genome123", false));
    GTUtilsWorkflowDesigner::setParameter(os, "Genome", QVariant(), GTUtilsWorkflowDesigner::customDialogSelector);
}

}  // namespace GUITest_common_scenarios_workflow_designer

}  // namespace U2
