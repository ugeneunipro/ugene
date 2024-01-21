/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2024 UniPro <ugene@unipro.ru>
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

#include <base_dialogs/GTFileDialog.h>
#include <base_dialogs/MessageBoxFiller.h>
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTAction.h>
#include <primitives/GTTextEdit.h>
#include <primitives/GTToolbar.h>
#include <primitives/GTWidget.h>
#include <primitives/PopupChooser.h>
#include <system/GTClipboard.h>
#include <system/GTFile.h>

#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QMdiArea>
#include <QTemporaryDir>
#include <QWizard>

#include <U2Core/AppContext.h>
#include <U2Core/ExternalToolRegistry.h>
#include <U2Core/U2SafePoints.h>

#include "GTTestsWorkflowDashboard.h"
#include "GTUtilsDashboard.h"
#include "GTUtilsExternalTools.h"
#include "GTUtilsLog.h"
#include "GTUtilsMdi.h"
#include "GTUtilsTask.h"
#include "GTUtilsTaskTreeView.h"
#include "GTUtilsWorkflowDesigner.h"
#include "runnables/ugene/plugins/workflow_designer/CreateElementWithCommandLineToolFiller.h"
#include "runnables/ugene/plugins/workflow_designer/DashboardsManagerDialogFiller.h"
#include "runnables/ugene/plugins/workflow_designer/StartupDialogFiller.h"

namespace U2 {

namespace GUITest_common_scenarios_workflow_dashboard {
using namespace HI;

static QString getExternalToolPath(const QString& toolName) {
    ExternalToolRegistry* etRegistry = AppContext::getExternalToolRegistry();
    CHECK_SET_ERR_RESULT(etRegistry != nullptr, "ExternalToolRegistry is nullptr", QString());

    ExternalTool* tool = etRegistry->getByName(toolName);
    CHECK_SET_ERR_RESULT(tool != nullptr, QString("'%1' tool not found in the registry is nullptr").arg(toolName), QString());

    QString toolPath = tool->getPath();
    CHECK_SET_ERR_RESULT(!toolPath.isEmpty(), QString("'%1' tool path is empty").arg(toolName), QString());
    CHECK_SET_ERR_RESULT(tool->isValid(), QString("'%1' tool is invalid").arg(toolName), QString());

    return toolPath;
}

static QString getExternalToolDirPath(const QString& toolName, const QString& dirNamePart) {
    QString toolPath = getExternalToolPath(toolName);
    QDir toolDir = QFileInfo(toolPath).dir();
    while (!toolDir.dirName().contains(dirNamePart)) {
        if (!toolDir.cdUp()) {
            CHECK_SET_ERR_RESULT(false, QString("Can't find tool '%1' dir that should contain '%2'. The tool path: '%3'").arg(toolName).arg(dirNamePart).arg(toolPath), QString());
        }
    }
    return toolDir.path();
}

static QString putToolToFolderWithSpaces(const QString& toolName, const QString& toolDirPath, const QString& sandboxDir) {
    QString toolPath = getExternalToolPath(toolName);
    CHECK(!toolPath.contains(" "), toolPath);

    QTemporaryDir dirWithSpaces(QFileInfo(sandboxDir).absolutePath() + "/folder XXXXXX");
    dirWithSpaces.setAutoRemove(false);

    const QString newToolDirPath = dirWithSpaces.path() + "/" + QFileInfo(toolDirPath).fileName();
    QString newToolPath = toolPath;
    newToolPath.replace(toolDirPath, newToolDirPath);
    GTFile::copyDir(toolDirPath, newToolDirPath);

    return newToolPath;
}

static QString putToolToFolderWithoutSpaces(const QString& toolName, const QString& toolDirPath, const QString& sandboxDir) {
    const QString toolPath = getExternalToolPath(toolName);
    CHECK(toolPath.contains(" "), toolPath);

    CHECK_SET_ERR_RESULT(!sandboxDir.contains(" "),
                         QString("Sandbox dir path contains spaces, can't find an acceptable place to copy tool '%1'. Sandbox dir path: '%2'")
                             .arg(toolName)
                             .arg(sandboxDir),
                         QString());

    QTemporaryDir dirWithoutSpaces(sandboxDir + "/folderXXXXXX");
    dirWithoutSpaces.setAutoRemove(false);

    const QString newToolDirPath = dirWithoutSpaces.path() + "/" + QFileInfo(toolDirPath).fileName();
    QString newToolPath = toolPath;
    newToolPath.replace(toolDirPath, newToolDirPath);
    CHECK_SET_ERR_RESULT(!newToolPath.contains(" "),
                         QString("Can't copy tool '%1' to a folder without spaces in the path: the tool contains spaces in its inner folders. Supposed new tool path: '%2'")
                             .arg(toolName)
                             .arg(newToolPath),
                         QString());

    GTFile::copyDir(toolDirPath, newToolDirPath);

    return newToolPath;
}

static QMap<QString, QList<QPair<QString, QStringList>>> getNodesTexts() {
    const int firstLevelNodesCount = GTUtilsDashboard::getChildrenNodesCount(GTUtilsDashboard::TREE_ROOT_ID);
    QMap<QString, QList<QPair<QString, QStringList>>> nodesTexts;
    for (int i = 0; i < firstLevelNodesCount; i++) {
        const QString firstLevelNodeId = GTUtilsDashboard::getChildNodeId(GTUtilsDashboard::TREE_ROOT_ID, i);
        const QString firstLevelNodeText = GTUtilsDashboard::getNodeText(firstLevelNodeId);

        const int secondLevelNodesCount = GTUtilsDashboard::getChildrenNodesCount(firstLevelNodeId);
        QList<QPair<QString, QStringList>> subtree;
        for (int j = 0; j < secondLevelNodesCount; j++) {
            const QString secondLevelNodeId = GTUtilsDashboard::getChildNodeId(firstLevelNodeId, j);
            const QString secondLevelNodeText = GTUtilsDashboard::getNodeText(secondLevelNodeId);

            const int thirdLevelNodesCount = GTUtilsDashboard::getChildrenNodesCount(secondLevelNodeId);
            QStringList thirdLevelNodesTexts;
            for (int k = 0; k < thirdLevelNodesCount; k++) {
                thirdLevelNodesTexts << GTUtilsDashboard::getNodeText(GTUtilsDashboard::getChildNodeId(secondLevelNodeId, k));
            }
            subtree << qMakePair(secondLevelNodeText, thirdLevelNodesTexts);
        }
        nodesTexts.insert(firstLevelNodeText, subtree);
    }
    return nodesTexts;
}

static void checkTreeStructure(const QMap<QString, QList<QPair<QString, QStringList>>>& expectedNodesTexts) {
    const QMap<QString, QList<QPair<QString, QStringList>>> nodesTexts = getNodesTexts();

    // Do explicitely comparison to be able to log the incorrect values
    CHECK_SET_ERR(nodesTexts.size() == expectedNodesTexts.size(),
                  QString("Unexpected first level nodes count: expected %1, got %2")
                      .arg(expectedNodesTexts.size())
                      .arg(nodesTexts.size()));

    foreach (const QString& expectedFirstLevelNodeText, expectedNodesTexts.keys()) {
        CHECK_SET_ERR(nodesTexts.contains(expectedFirstLevelNodeText),
                      QString("An expected first level node with text '%1' is not found in the tree")
                          .arg(expectedFirstLevelNodeText));

        const QList<QPair<QString, QStringList>>& expectedSecondLevelNodes = expectedNodesTexts[expectedFirstLevelNodeText];
        const QList<QPair<QString, QStringList>>& secondLevelNodes = nodesTexts[expectedFirstLevelNodeText];

        CHECK_SET_ERR(expectedSecondLevelNodes.size() == secondLevelNodes.size(),
                      QString("Unexpected second level nodes count for the first level node with text '%1': expected %2, got %3")
                          .arg(expectedFirstLevelNodeText)
                          .arg(expectedSecondLevelNodes.size())
                          .arg(secondLevelNodes.size()));

        for (int i = 0, n = expectedSecondLevelNodes.size(); i < n; i++) {
            const QPair<QString, QStringList>& expectedSecondLevelNode = expectedSecondLevelNodes[i];
            const QPair<QString, QStringList>& secondLevelNode = secondLevelNodes[i];

            const QString expectedSecondLevelNodeText = expectedSecondLevelNode.first;
            const QString secondLevelNodeText = secondLevelNode.first;
            CHECK_SET_ERR(expectedSecondLevelNodeText == secondLevelNodeText,
                          QString("%1 (zero-based) child of the first level node with text '%2' has unexpected text: expected '%3', got '%4'")
                              .arg(i)
                              .arg(expectedFirstLevelNodeText)
                              .arg(expectedSecondLevelNodeText)
                              .arg(secondLevelNodeText));

            const QStringList& expectedThirdLevelNodes = expectedSecondLevelNode.second;
            const QStringList& thirdLevelNodes = secondLevelNode.second;

            CHECK_SET_ERR(expectedThirdLevelNodes.size() == thirdLevelNodes.size(),
                          QString("Unexpected third level nodes count for the second level node with text '%1', which is %2 (zero-based) child of the first level node with text '%3': expected %4, got %5")
                              .arg(expectedSecondLevelNodeText)
                              .arg(i)
                              .arg(expectedFirstLevelNodeText)
                              .arg(expectedThirdLevelNodes.size())
                              .arg(thirdLevelNodes.size()));

            for (int j = 0, m = expectedThirdLevelNodes.size(); j < m; j++) {
                CHECK_SET_ERR(expectedThirdLevelNodes[j] == thirdLevelNodes[j],
                              QString("%1 (zero-based) child of the second level node with text '%2', which is %3 (zero-based) child of "
                                      "the first node with text '%4', has unexpected text: expected '%5', got '%6'")
                                  .arg(j)
                                  .arg(expectedSecondLevelNodeText)
                                  .arg(i)
                                  .arg(expectedFirstLevelNodeText)
                                  .arg(expectedThirdLevelNodes[j])
                                  .arg(thirdLevelNodes[j]));
            }
        }
    }
}

static QString getQuotedString(const QString& string) {
    if (string.contains(QRegularExpression("\\s"))) {
        return "\"" + string + "\"";
    }
    return string;
}

GUI_TEST_CLASS_DEFINITION(misc_test_0001) {
    GTUtilsDialog::waitForDialog(new StartupDialogFiller());

    //    1. Open "_common_data/workflow/dashboard/external_tools_free_worfklow.uwl".
    GTFileDialog::openFile(testDir + "_common_data/workflow/dashboard/external_tools_free_worfklow.uwl");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Click "Read File URL(s)" element.
    GTUtilsWorkflowDesigner::click("Read File URL(s)");

    //    3. Add "data/samples/FASTA/human_T1.fa" file to "Dataset 1" dataset.
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "samples/FASTA/human_T1.fa", true);

    //    4. Launch the workflow.
    GTUtilsWorkflowDesigner::runWorkflow();

    //    5. Wait the workflow execution finish.
    GTUtilsTaskTreeView::waitTaskFinished();

    //    Expected result: a dashboard appears, it has no "External Tools" tab.
    const bool externalToolsTabExists = GTUtilsDashboard::hasTab(GTUtilsDashboard::ExternalTools);
    CHECK_SET_ERR(!externalToolsTabExists, "'External tools' dashboard tab unexpectedly is present on the dashboard");
}

GUI_TEST_CLASS_DEFINITION(misc_test_0002) {
    GTUtilsDialog::waitForDialog(new StartupDialogFiller());

    //    1. Open "_common_data/workflow/dashboard/clustulo.uwl".
    GTFileDialog::openFile(testDir + "_common_data/workflow/dashboard/clustulo.uwl");
    GTUtilsDialog::checkNoActiveWaiters();

    //    2. Click "Read Alignment" element.
    GTUtilsWorkflowDesigner::click("Read Alignment");

    //    3. Add "data/samples/CLUSTALW/COI.aln" file to "Dataset 1" dataset.
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "samples/CLUSTALW/COI.aln", true);

    //    4. Launch the workflow.
    GTUtilsWorkflowDesigner::runWorkflow();

    //    5. Wait the workflow execution finish.
    GTUtilsTaskTreeView::waitTaskFinished();

    //    Expected result: a dashboard appears, it has "External Tools" tab.
    bool externalToolsTabExists = GTUtilsDashboard::hasTab(GTUtilsDashboard::ExternalTools);
    CHECK_SET_ERR(externalToolsTabExists, "'External tools' dashboard tab unexpectedly is not present on the dashboard");
}

GUI_TEST_CLASS_DEFINITION(misc_test_0003) {
    GTUtilsDialog::waitForDialog(new StartupDialogFiller());

    //    1. Open "_common_data/workflow/dashboard/clustulo.uwl".
    GTFileDialog::openFile(testDir + "_common_data/workflow/dashboard/clustulo.uwl");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Click "Read Alignment" element.
    GTUtilsWorkflowDesigner::click("Read Alignment");

    //    3. Add "data/samples/CLUSTALW/COI.aln" file to "Dataset 1" dataset.
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "samples/CLUSTALW/COI.aln", true);

    //    4. Launch the workflow.
    GTUtilsWorkflowDesigner::runWorkflow();

    //    5. Wait the workflow execution finish.
    GTUtilsTaskTreeView::waitTaskFinished();

    //    6. Switch to an "External Tools" tab.
    GTUtilsDashboard::openTab(GTUtilsDashboard::ExternalTools);

    //    Expected result:
    //    The tree looks like:
    //    Align with ClustalO
    //    └Align with ClustalO run 1
    //      └ClustalO run

    int childrenCount = GTUtilsDashboard::getChildrenNodesCount(GTUtilsDashboard::TREE_ROOT_ID);
    int expectedChildrenCount = 1;
    CHECK_SET_ERR(expectedChildrenCount == childrenCount, QString("There is an unexpected children count of node with ID '%1': expected %2, got %3").arg(GTUtilsDashboard::TREE_ROOT_ID).arg(expectedChildrenCount).arg(childrenCount));

    QString nodeId = GTUtilsDashboard::getChildNodeId(GTUtilsDashboard::TREE_ROOT_ID, 0);
    QString nodeText = GTUtilsDashboard::getNodeText(nodeId);
    QString expectedNodeText = "Align with ClustalO";
    CHECK_SET_ERR(expectedNodeText == nodeText, QString("There is unexpected text of node with ID '%1': expected '%2', got '%3'").arg(nodeId).arg(expectedNodeText).arg(nodeText));

    childrenCount = GTUtilsDashboard::getChildrenNodesCount(nodeId);
    expectedChildrenCount = 1;
    CHECK_SET_ERR(expectedChildrenCount == childrenCount, QString("There is an unexpected children count of node with ID '%1': expected %2, got %3").arg(nodeId).arg(expectedChildrenCount).arg(childrenCount));

    nodeId = GTUtilsDashboard::getChildNodeId(nodeId, 0);
    nodeText = GTUtilsDashboard::getNodeText(nodeId);
    expectedNodeText = "Align with ClustalO run 1";
    CHECK_SET_ERR(expectedNodeText == nodeText, QString("There is unexpected text of node with ID '%1': expected '%2', got '%3'").arg(nodeId).arg(expectedNodeText).arg(nodeText));

    childrenCount = GTUtilsDashboard::getChildrenNodesCount(nodeId);
    expectedChildrenCount = 1;
    CHECK_SET_ERR(expectedChildrenCount == childrenCount, QString("There is an unexpected children count of node with ID '%1': expected %2, got %3").arg(nodeId).arg(expectedChildrenCount).arg(childrenCount));

    nodeId = GTUtilsDashboard::getChildNodeId(nodeId, 0);
    nodeText = GTUtilsDashboard::getNodeText(nodeId);
    expectedNodeText = "ClustalO run";
    CHECK_SET_ERR(expectedNodeText == nodeText, QString("There is unexpected text of node with ID '%1': expected '%2', got '%3'").arg(nodeId).arg(expectedNodeText).arg(nodeText));
}

GUI_TEST_CLASS_DEFINITION(misc_test_0004) {
    GTUtilsDialog::waitForDialog(new StartupDialogFiller());

    //    1. Open "_common_data/workflow/dashboard/samtools_clustulo_clustalw.uwl".
    GTFileDialog::openFile(testDir + "_common_data/workflow/dashboard/samtools_clustulo_clustalw.uwl");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Click "Read File URL(s)" element.
    GTUtilsWorkflowDesigner::click("Read File URL(s)");

    //    3. Add "data/samples/Assembly/chrM.sam.bam" file to "Dataset 1" dataset.
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "samples/Assembly/chrM.sorted.bam", true);

    //    4. Create a new dataset with name "Dataset 2".
    GTUtilsWorkflowDesigner::createDataset("Dataset 2");

    //    5. Add "data/samples/Assembly/chrM.sam.bam" file to "Dataset 2" dataset.
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "samples/Assembly/chrM.sam", true);

    //    6. Click "Call Variants with SAMtools" element.
    GTUtilsWorkflowDesigner::click("Call Variants with SAMtools");

    //    7. Set "Reference" attribute value to "data/samples/Assembly/chrM.fa".
    GTUtilsWorkflowDesigner::setParameter("Reference", dataDir + "samples/Assembly/chrM.fa", GTUtilsWorkflowDesigner::textValue);

    //    8. Set "Output variants file" attribute value to "misc_test_0004.vcf".
    GTUtilsWorkflowDesigner::setParameter("Output variants file", QFileInfo(sandBoxDir + "misc_test_0004.vcf").absoluteFilePath(), GTUtilsWorkflowDesigner::textValue);

    //    9. Click "Read Alignment" element.
    GTUtilsWorkflowDesigner::click("Read Alignment");

    //    10. Add "data/samples/CLUSTALW/COI.aln" file to "Dataset 1" dataset.
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "samples/CLUSTALW/COI.aln", true);

    //    11. Add "data/samples/CLUSTALW/COI.aln" file to "Dataset 1" dataset.
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "samples/CLUSTALW/COI.aln", true);

    //    12. Launch the workflow.
    GTUtilsWorkflowDesigner::runWorkflow();

    //    13. Wait the workflow execution finish.
    GTUtilsTaskTreeView::waitTaskFinished();

    //    14. Switch to an "External Tools" tab.
    GTUtilsDashboard::openTab(GTUtilsDashboard::ExternalTools);

    //    Expected result:
    //    The tree looks like:
    //    Call Variants with SAMtools
    //    ├Call Variants with SAMtools run 1
    //    |├SAMtools run
    //    |├BCFtools run
    //    |└vcfutils run
    //    └Call Variants with SAMtools run 2
    //     ├SAMtools run
    //     ├BCFtools run
    //     └vcfutils run
    //    Align with ClustalO
    //    ├Align with ClustalO run 1
    //    |└ClustalO run
    //    └Align with ClustalO run 2
    //     └ClustalO run
    //    Align with ClustalW
    //    ├Align with ClustalW run 1
    //    |└ClustalW run
    //    └Align with ClustalW run 2
    //     └ClustalW run

    // root
    int childrenCount = GTUtilsDashboard::getChildrenNodesCount(GTUtilsDashboard::TREE_ROOT_ID);
    int expectedChildrenCount = 3;
    CHECK_SET_ERR(expectedChildrenCount == childrenCount, QString("There is an unexpected children count of node with ID '%1': expected %2, got %3").arg(GTUtilsDashboard::TREE_ROOT_ID).arg(expectedChildrenCount).arg(childrenCount));

    // root / Call Variants with SAMtools
    QString nodeLevel1Id = GTUtilsDashboard::getChildNodeId(GTUtilsDashboard::TREE_ROOT_ID, 0);
    QString nodeText = GTUtilsDashboard::getNodeText(nodeLevel1Id);
    QString expectedNodeText = "Call Variants with SAMtools";
    CHECK_SET_ERR(expectedNodeText == nodeText, QString("There is unexpected text of node with ID '%1': expected '%2', got '%3'").arg(nodeLevel1Id).arg(expectedNodeText).arg(nodeText));

    childrenCount = GTUtilsDashboard::getChildrenNodesCount(nodeLevel1Id);
    expectedChildrenCount = 2;
    CHECK_SET_ERR(expectedChildrenCount == childrenCount, QString("There is an unexpected children count of node with ID '%1': expected %2, got %3").arg(nodeLevel1Id).arg(expectedChildrenCount).arg(childrenCount));

    // root / Call Variants with SAMtools / Call Variants with SAMtools run 1
    QString nodeLevel2Id = GTUtilsDashboard::getChildNodeId(nodeLevel1Id, 0);
    nodeText = GTUtilsDashboard::getNodeText(nodeLevel2Id);
    expectedNodeText = "Call Variants with SAMtools run 1";
    CHECK_SET_ERR(expectedNodeText == nodeText, QString("There is unexpected text of node with ID '%1': expected '%2', got '%3'").arg(nodeLevel2Id).arg(expectedNodeText).arg(nodeText));

    childrenCount = GTUtilsDashboard::getChildrenNodesCount(nodeLevel2Id);
    expectedChildrenCount = 3;
    CHECK_SET_ERR(expectedChildrenCount == childrenCount, QString("There is an unexpected children count of node with ID '%1': expected %2, got %3").arg(nodeLevel2Id).arg(expectedChildrenCount).arg(childrenCount));

    // root / Call Variants with SAMtools / Call Variants with SAMtools run 1 / SAMtools run
    QString nodeLevel3Id = GTUtilsDashboard::getChildNodeId(nodeLevel2Id, 0);
    nodeText = GTUtilsDashboard::getNodeText(nodeLevel3Id);
    expectedNodeText = "SAMtools run";
    CHECK_SET_ERR(expectedNodeText == nodeText, QString("There is unexpected text of node with ID '%1': expected '%2', got '%3'").arg(nodeLevel3Id).arg(expectedNodeText).arg(nodeText));

    // root / Call Variants with SAMtools / Call Variants with SAMtools run 1 / BCFtools run
    nodeLevel3Id = GTUtilsDashboard::getChildNodeId(nodeLevel2Id, 1);
    nodeText = GTUtilsDashboard::getNodeText(nodeLevel3Id);
    expectedNodeText = "BCFtools run";
    CHECK_SET_ERR(expectedNodeText == nodeText, QString("There is unexpected text of node with ID '%1': expected '%2', got '%3'").arg(nodeLevel3Id).arg(expectedNodeText).arg(nodeText));

    // root / Call Variants with SAMtools / Call Variants with SAMtools run 1 / vcfutils run
    nodeLevel3Id = GTUtilsDashboard::getChildNodeId(nodeLevel2Id, 2);
    nodeText = GTUtilsDashboard::getNodeText(nodeLevel3Id);
    expectedNodeText = "vcfutils run";
    CHECK_SET_ERR(expectedNodeText == nodeText, QString("There is unexpected text of node with ID '%1': expected '%2', got '%3'").arg(nodeLevel3Id).arg(expectedNodeText).arg(nodeText));

    // root / Call Variants with SAMtools / Call Variants with SAMtools run 2
    nodeLevel2Id = GTUtilsDashboard::getChildNodeId(nodeLevel1Id, 1);
    nodeText = GTUtilsDashboard::getNodeText(nodeLevel2Id);
    expectedNodeText = "Call Variants with SAMtools run 2";
    CHECK_SET_ERR(expectedNodeText == nodeText, QString("There is unexpected text of node with ID '%1': expected '%2', got '%3'").arg(nodeLevel2Id).arg(expectedNodeText).arg(nodeText));

    childrenCount = GTUtilsDashboard::getChildrenNodesCount(nodeLevel2Id);
    expectedChildrenCount = 3;
    CHECK_SET_ERR(expectedChildrenCount == childrenCount, QString("There is an unexpected children count of node with ID '%1': expected %2, got %3").arg(nodeLevel2Id).arg(expectedChildrenCount).arg(childrenCount));

    // root / Call Variants with SAMtools / Call Variants with SAMtools run 2 / SAMtools run
    nodeLevel3Id = GTUtilsDashboard::getChildNodeId(nodeLevel2Id, 0);
    nodeText = GTUtilsDashboard::getNodeText(nodeLevel3Id);
    expectedNodeText = "SAMtools run";
    CHECK_SET_ERR(expectedNodeText == nodeText, QString("There is unexpected text of node with ID '%1': expected '%2', got '%3'").arg(nodeLevel3Id).arg(expectedNodeText).arg(nodeText));

    // root / Call Variants with SAMtools / Call Variants with SAMtools run 2 / BCFtools run
    nodeLevel3Id = GTUtilsDashboard::getChildNodeId(nodeLevel2Id, 1);
    nodeText = GTUtilsDashboard::getNodeText(nodeLevel3Id);
    expectedNodeText = "BCFtools run";
    CHECK_SET_ERR(expectedNodeText == nodeText, QString("There is unexpected text of node with ID '%1': expected '%2', got '%3'").arg(nodeLevel3Id).arg(expectedNodeText).arg(nodeText));

    // root / Call Variants with SAMtools / Call Variants with SAMtools run 2 / vcfutils run
    nodeLevel3Id = GTUtilsDashboard::getChildNodeId(nodeLevel2Id, 2);
    nodeText = GTUtilsDashboard::getNodeText(nodeLevel3Id);
    expectedNodeText = "vcfutils run";
    CHECK_SET_ERR(expectedNodeText == nodeText, QString("There is unexpected text of node with ID '%1': expected '%2', got '%3'").arg(nodeLevel3Id).arg(expectedNodeText).arg(nodeText));

    // root / Align with ClustalO
    nodeLevel1Id = GTUtilsDashboard::getChildNodeId(GTUtilsDashboard::TREE_ROOT_ID, 1);
    nodeText = GTUtilsDashboard::getNodeText(nodeLevel1Id);
    expectedNodeText = "Align with ClustalO";
    CHECK_SET_ERR(expectedNodeText == nodeText, QString("There is unexpected text of node with ID '%1': expected '%2', got '%3'").arg(nodeLevel1Id).arg(expectedNodeText).arg(nodeText));

    childrenCount = GTUtilsDashboard::getChildrenNodesCount(nodeLevel1Id);
    expectedChildrenCount = 2;
    CHECK_SET_ERR(expectedChildrenCount == childrenCount, QString("There is an unexpected children count of node with ID '%1': expected %2, got %3").arg(nodeLevel1Id).arg(expectedChildrenCount).arg(childrenCount));

    // root / Align with ClustalO / Align with ClustalO run 1
    nodeLevel2Id = GTUtilsDashboard::getChildNodeId(nodeLevel1Id, 0);
    nodeText = GTUtilsDashboard::getNodeText(nodeLevel2Id);
    expectedNodeText = "Align with ClustalO run 1";
    CHECK_SET_ERR(expectedNodeText == nodeText, QString("There is unexpected text of node with ID '%1': expected '%2', got '%3'").arg(nodeLevel2Id).arg(expectedNodeText).arg(nodeText));

    childrenCount = GTUtilsDashboard::getChildrenNodesCount(nodeLevel2Id);
    expectedChildrenCount = 1;
    CHECK_SET_ERR(expectedChildrenCount == childrenCount, QString("There is an unexpected children count of node with ID '%1': expected %2, got %3").arg(nodeLevel2Id).arg(expectedChildrenCount).arg(childrenCount));

    // root / Align with ClustalO / Align with ClustalO run 1 / ClustalO run
    nodeLevel3Id = GTUtilsDashboard::getChildNodeId(nodeLevel2Id, 0);
    nodeText = GTUtilsDashboard::getNodeText(nodeLevel3Id);
    expectedNodeText = "ClustalO run";
    CHECK_SET_ERR(expectedNodeText == nodeText, QString("There is unexpected text of node with ID '%1': expected '%2', got '%3'").arg(nodeLevel3Id).arg(expectedNodeText).arg(nodeText));

    // root / Align with ClustalO / Align with ClustalO run 2
    nodeLevel2Id = GTUtilsDashboard::getChildNodeId(nodeLevel1Id, 1);
    nodeText = GTUtilsDashboard::getNodeText(nodeLevel2Id);
    expectedNodeText = "Align with ClustalO run 2";
    CHECK_SET_ERR(expectedNodeText == nodeText, QString("There is unexpected text of node with ID '%1': expected '%2', got '%3'").arg(nodeLevel2Id).arg(expectedNodeText).arg(nodeText));

    childrenCount = GTUtilsDashboard::getChildrenNodesCount(nodeLevel2Id);
    expectedChildrenCount = 1;
    CHECK_SET_ERR(expectedChildrenCount == childrenCount, QString("There is an unexpected children count of node with ID '%1': expected %2, got %3").arg(nodeLevel2Id).arg(expectedChildrenCount).arg(childrenCount));

    // root / Align with ClustalO / Align with ClustalO run 2 / ClustalO run
    nodeLevel3Id = GTUtilsDashboard::getChildNodeId(nodeLevel2Id, 0);
    nodeText = GTUtilsDashboard::getNodeText(nodeLevel3Id);
    expectedNodeText = "ClustalO run";
    CHECK_SET_ERR(expectedNodeText == nodeText, QString("There is unexpected text of node with ID '%1': expected '%2', got '%3'").arg(nodeLevel3Id).arg(expectedNodeText).arg(nodeText));

    // root / Align with ClustalW
    nodeLevel1Id = GTUtilsDashboard::getChildNodeId(GTUtilsDashboard::TREE_ROOT_ID, 2);
    nodeText = GTUtilsDashboard::getNodeText(nodeLevel1Id);
    expectedNodeText = "Align with ClustalW";
    CHECK_SET_ERR(expectedNodeText == nodeText, QString("There is unexpected text of node with ID '%1': expected '%2', got '%3'").arg(nodeLevel1Id).arg(expectedNodeText).arg(nodeText));

    childrenCount = GTUtilsDashboard::getChildrenNodesCount(nodeLevel1Id);
    expectedChildrenCount = 2;
    CHECK_SET_ERR(expectedChildrenCount == childrenCount, QString("There is an unexpected children count of node with ID '%1': expected %2, got %3").arg(nodeLevel1Id).arg(expectedChildrenCount).arg(childrenCount));

    // root / Align with ClustalW / Align with ClustalW run 1
    nodeLevel2Id = GTUtilsDashboard::getChildNodeId(nodeLevel1Id, 0);
    nodeText = GTUtilsDashboard::getNodeText(nodeLevel2Id);
    expectedNodeText = "Align with ClustalW run 1";
    CHECK_SET_ERR(expectedNodeText == nodeText, QString("There is unexpected text of node with ID '%1': expected '%2', got '%3'").arg(nodeLevel2Id).arg(expectedNodeText).arg(nodeText));

    childrenCount = GTUtilsDashboard::getChildrenNodesCount(nodeLevel2Id);
    expectedChildrenCount = 1;
    CHECK_SET_ERR(expectedChildrenCount == childrenCount, QString("There is an unexpected children count of node with ID '%1': expected %2, got %3").arg(nodeLevel2Id).arg(expectedChildrenCount).arg(childrenCount));

    // root / Align with ClustalW / Align with ClustalW run 1 / ClustalW run
    nodeLevel3Id = GTUtilsDashboard::getChildNodeId(nodeLevel2Id, 0);
    nodeText = GTUtilsDashboard::getNodeText(nodeLevel3Id);
    expectedNodeText = "ClustalW run";
    CHECK_SET_ERR(expectedNodeText == nodeText, QString("There is unexpected text of node with ID '%1': expected '%2', got '%3'").arg(nodeLevel3Id).arg(expectedNodeText).arg(nodeText));

    // root / Align with ClustalO / Align with ClustalW run 2
    nodeLevel2Id = GTUtilsDashboard::getChildNodeId(nodeLevel1Id, 1);
    nodeText = GTUtilsDashboard::getNodeText(nodeLevel2Id);
    expectedNodeText = "Align with ClustalW run 2";
    CHECK_SET_ERR(expectedNodeText == nodeText, QString("There is unexpected text of node with ID '%1': expected '%2', got '%3'").arg(nodeLevel2Id).arg(expectedNodeText).arg(nodeText));

    childrenCount = GTUtilsDashboard::getChildrenNodesCount(nodeLevel2Id);
    expectedChildrenCount = 1;
    CHECK_SET_ERR(expectedChildrenCount == childrenCount, QString("There is an unexpected children count of node with ID '%1': expected %2, got %3").arg(nodeLevel2Id).arg(expectedChildrenCount).arg(childrenCount));

    // root / Align with ClustalO / Align with ClustalW run 2 / ClustalW run
    nodeLevel3Id = GTUtilsDashboard::getChildNodeId(nodeLevel2Id, 0);
    nodeText = GTUtilsDashboard::getNodeText(nodeLevel3Id);
    expectedNodeText = "ClustalW run";
    CHECK_SET_ERR(expectedNodeText == nodeText, QString("There is unexpected text of node with ID '%1': expected '%2', got '%3'").arg(nodeLevel3Id).arg(expectedNodeText).arg(nodeText));
}

static void createNodesElement(const QString& testDir) {
    CreateElementWithCommandLineToolFiller::ElementWithCommandLineSettings settings;
    settings.elementName = "Nodes";
    settings.tooltype = CreateElementWithCommandLineToolFiller::CommandLineToolType::ExecutablePath;
    settings.tool = testDir + "_common_data/workflow/dashboard/fake_tools/nodes.py";

    // Second page
    QList<CreateElementWithCommandLineToolFiller::InOutData> input;
    CreateElementWithCommandLineToolFiller::InOutDataType inDataType;
    inDataType.first = CreateElementWithCommandLineToolFiller::String;
    inDataType.second = "String data value";
    input << CreateElementWithCommandLineToolFiller::InOutData("in", inDataType);
    settings.input = input;

    // Fourth page
    QList<CreateElementWithCommandLineToolFiller::InOutData> output;
    CreateElementWithCommandLineToolFiller::InOutDataType outDataType;
    outDataType.first = CreateElementWithCommandLineToolFiller::String;
    outDataType.second = "Output URL";
    output << CreateElementWithCommandLineToolFiller::InOutData("out", outDataType);
    settings.output = output;

    // Fifth page
    settings.command = "%USUPP_PYTHON3% %TOOL_PATH% --input $in --output $out";
    GTUtilsDialog::waitForDialog(new CreateElementWithCommandLineToolFiller(settings));
    GTWidget::click(GTAction::button("createElementWithCommandLineTool"));
}

GUI_TEST_CLASS_DEFINITION(tree_nodes_creation_test_0002) {
    // 1. Create a custom element called Nodes
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    auto nodesElement = GTUtilsWorkflowDesigner::findTreeItem("Nodes", GTUtilsWorkflowDesigner::tab::algorithms, true, false);
    if (nodesElement == nullptr) {
        createNodesElement(testDir);
    }

    //    2. Open "_common_data/workflow/dashboard/nodes.uwl".
    GTFileDialog::openFile(testDir + "_common_data/workflow/dashboard/nodes.uwl");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    3. Click "Read Sequence" element.
    GTUtilsWorkflowDesigner::click("Read Sequence");

    //    4. Add "data/samples/FASTQ/eas.fastq" file 100 times to "Dataset 1" dataset.
    const QString inputDir = QFileInfo(sandBoxDir + "tree_nodes_creation_test_0002_input").absoluteFilePath();
    QDir().mkpath(inputDir);
    QStringList inputFiles;
    for (int i = 0; i < 100; i++) {
        QString inputFile = inputDir + "/" + QString("eas_%2.fastq").arg(i);
        inputFiles << inputFile;
        GTFile::copy(dataDir + "samples/FASTQ/eas.fastq", inputFile);
    }
    GTUtilsWorkflowDesigner::setDatasetInputFiles(inputFiles);
    //GTUtilsWorkflowDesigner::setDatasetInputFolder(inputDir);

    //    5. Launch the workflow.
    GTUtilsWorkflowDesigner::runWorkflow();

    //    6. Wait the workflow execution finish.
    GTUtilsTaskTreeView::waitTaskFinished();

    //    7. Switch to an "External Tools" tab.
    GTUtilsDashboard::openTab(GTUtilsDashboard::ExternalTools);

    //    Expected result:
    //    The tree looks like:
    //     -----------
    //    Nodes
    //    ├Nodes run 1
    //     |└Nodes run
    //    ├Nodes run 2
    //     |└Nodes run
    //    ...
    //    └Nodes run 100
    //      └Nodes run
    QMap<QString, QList<QPair<QString, QStringList>>> expectedNodesTexts;
    QList<QPair<QString, QStringList>> secondLevelNodes;
    for (int i = 1; i < 101; i++) {
        secondLevelNodes << qMakePair(QString("Nodes run %1").arg(i), QStringList({"Nodes run"}));
    }
    expectedNodesTexts.insert("Nodes", secondLevelNodes);
    checkTreeStructure(expectedNodesTexts);
}

GUI_TEST_CLASS_DEFINITION(tree_nodes_creation_test_0003) {
    // 1. Import a custom element from _common_data/workflow/dashboard/Nodes.etc if it is not
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    auto nodesElement = GTUtilsWorkflowDesigner::findTreeItem("Nodes", GTUtilsWorkflowDesigner::tab::algorithms, true, false);
    if (nodesElement == nullptr) {
        createNodesElement(testDir);
    }

    //    2. Open "_common_data/workflow/dashboard/nodes.uwl".
    GTFileDialog::openFile(testDir + "_common_data/workflow/dashboard/50_nodes.uwl");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    3. Click "Read Sequence" element.
    GTUtilsWorkflowDesigner::click("Read Sequence");

    //    4. Add "_common_data/fastq/eas.fastq" and "_common_data/fastq/eas_broken.fastq" files to "Dataset 1" dataset.
    GTUtilsWorkflowDesigner::setDatasetInputFiles({testDir + "_common_data/fastq/eas.fastq", testDir + "_common_data/fastq/eas_broken.fastq"});

    //    5. Launch the workflow.
    GTUtilsWorkflowDesigner::runWorkflow();

    //    6. Wait the workflow execution finish.
    GTUtilsTaskTreeView::waitTaskFinished();

    //    7. Switch to an "External Tools" tab.
    GTUtilsDashboard::openTab(GTUtilsDashboard::ExternalTools);

    //    Expected result:
    //    The tree looks like (note, that the elements order is not fixed):
    //     -----------
    //    Nodes 1
    //    ├Nodes 1 run 1
    //     |└Nodes run
    //    └Nodes 1 run 2
    //      └Nodes run
    //    Nodes 2
    //    ├Nodes 2 run 1
    //     |└Nodes run
    //    └Nodes 2 run 2
    //      └Nodes run
    //    ...
    //    Nodes 50
    //    ├Nodes 50 run 1
    //     |└Nodes run
    //    └Nodes 50 run 2
    //      └Nodes run
    QMap<QString, QList<QPair<QString, QStringList>>> expectedNodesTexts;
    QList<QPair<QString, QStringList>> secondLevelNodes;
    for (int i = 1; i < 51; i++) {
        expectedNodesTexts.insert(QString("Nodes %1").arg(i), {qMakePair(QString("Nodes %1 run 1").arg(i), QStringList({"Nodes run"})), qMakePair(QString("Nodes %1 run 2").arg(i), QStringList({"Nodes run"}))});
    }
    checkTreeStructure(expectedNodesTexts);
}

GUI_TEST_CLASS_DEFINITION(tree_nodes_creation_test_0004) {
    // 1. Create a custom element called Nodes
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    auto nodesElement = GTUtilsWorkflowDesigner::findTreeItem("Nodes", GTUtilsWorkflowDesigner::tab::algorithms, true, false);
    if (nodesElement == nullptr) {
        createNodesElement(testDir);
    }

    //    2. Open "_common_data/workflow/dashboard/nodes.uwl".
    GTFileDialog::openFile(testDir + "_common_data/workflow/dashboard/101_nodes.uwl");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    3. Click "Read Sequence" element.
    GTUtilsWorkflowDesigner::click("Read Sequence");

    //    4. Add "data/samples/FASTQ/eas.fastq" file to "Dataset 1" dataset.
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "samples/FASTQ/eas.fastq", true);

    //    5. Launch the workflow.
    GTUtilsWorkflowDesigner::runWorkflow();

    //    6. Wait the workflow execution finish.
    GTUtilsTaskTreeView::waitTaskFinished();

    //    7. Switch to an "External Tools" tab.
    GTUtilsDashboard::openTab(GTUtilsDashboard::ExternalTools);

    //    Expected result:
    //    The tree looks like (note, that the elements order is not fixed):
    //     -----------
    //    Nodes 1
    //    └Nodes 1 run 1
    //      └Nodes run
    //    Nodes 2
    //    └Nodes 2 run 1
    //      └Nodes run
    //    ...
    //    Nodes 101
    //    └Nodes 101 run 1
    //      └Nodes run
    //    Messages limit on the dashboard exceeded. See log files, if required.
    //     -----------
    //    "log files" in a message is a link to a dir with tools execution logs.
    QMap<QString, QList<QPair<QString, QStringList>>> expectedNodesTexts;
    QList<QPair<QString, QStringList>> secondLevelNodes;
    for (int i = 1; i <= 100; i++) {
        expectedNodesTexts.insert(QString("Nodes %1").arg(i), {qMakePair(QString("Nodes %1 run 1").arg(i), QStringList({"Nodes run"}))});
    }
    checkTreeStructure(expectedNodesTexts);

    bool limitationMessageNodeExists = GTUtilsDashboard::hasLimitationMessage(GTUtilsDashboard::TREE_ROOT_ID);
    CHECK_SET_ERR(limitationMessageNodeExists, QString("Message node unexpectedly not found, parent node ID: '%1'").arg(GTUtilsDashboard::TREE_ROOT_ID));

    QString messageNodeText = GTUtilsDashboard::getLimitationMessage(GTUtilsDashboard::TREE_ROOT_ID);
    QString expectedMessageNodeText = "Messages limit on the dashboard exceeded.";
    CHECK_SET_ERR(messageNodeText.startsWith(expectedMessageNodeText),
                  QString("Unexpected message node text: expected '%1', got '%2'; parent node ID: '%3'")
                      .arg(expectedMessageNodeText)
                      .arg(messageNodeText)
                      .arg(GTUtilsDashboard::TREE_ROOT_ID));

    QString logUrl = GTUtilsDashboard::getLogUrlFromNodeLimitationMessage(GTUtilsDashboard::TREE_ROOT_ID);

    QFileInfo fileInfo(logUrl);
    CHECK_SET_ERR(fileInfo.exists(), QString("Dir with URL '%1' doesn't exist").arg(logUrl));
    CHECK_SET_ERR(fileInfo.isDir(), QString("Entry with URL '%1' is not a dir").arg(logUrl));
    CHECK_SET_ERR(!QDir(logUrl).entryList(QDir::Files).isEmpty(), QString("Dir with URL '%1' doesn't contain log files").arg(logUrl));
}

GUI_TEST_CLASS_DEFINITION(tool_launch_nodes_test_0001) {
    GTUtilsDialog::waitForDialog(new StartupDialogFiller());

    //    1. Open "_common_data/workflow/dashboard/clustulo_clustalw.uwl".
    GTFileDialog::openFile(testDir + "_common_data/workflow/dashboard/clustulo_clustalw.uwl");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Click "Read Alignment" element.
    GTUtilsWorkflowDesigner::click("Read Alignment");

    //    3. Add "data/samples/CLUSTALW/COI.aln" file to "Dataset 1" dataset.
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "samples/CLUSTALW/COI.aln", true);

    //    4. Launch the workflow.
    GTLogTracer lt;
    GTUtilsWorkflowDesigner::runWorkflow();

    //    5. Wait the workflow execution finish.
    GTUtilsTaskTreeView::waitTaskFinished();

    //    6. Switch to an "External Tools" tab.
    GTUtilsDashboard::openTab(GTUtilsDashboard::ExternalTools);

    //    Expected result:
    //        the third-level node "ClustalO run" has a button to copy the command. The icon size is greater than 10x10 pixels.
    //        the third-level node "ClustalW run" has a button to copy the command. The icon size is greater than 10x10 pixels.
    const QString clustaloRunNodeId = GTUtilsDashboard::getDescendantNodeId(GTUtilsDashboard::TREE_ROOT_ID, {0, 0, 0});
    QSize copyButtonSize = GTUtilsDashboard::getCopyButtonSize(clustaloRunNodeId);
    QSize minimumCopyButtonSize(10, 10);

    CHECK_SET_ERR(minimumCopyButtonSize.width() <= copyButtonSize.width(),
                  QString("Copy button width of node with ID '%1' is less than %2 pixels: it's width is %3 pixels")
                      .arg(clustaloRunNodeId)
                      .arg(minimumCopyButtonSize.width())
                      .arg(copyButtonSize.width()));
    CHECK_SET_ERR(minimumCopyButtonSize.height() <= copyButtonSize.height(),
                  QString("Copy button height of node with ID '%1' is less than %2 pixels: it's height is %3 pixels")
                      .arg(clustaloRunNodeId)
                      .arg(minimumCopyButtonSize.height())
                      .arg(copyButtonSize.height()));

    const QString clustalwRunNodeId = GTUtilsDashboard::getDescendantNodeId(GTUtilsDashboard::TREE_ROOT_ID, {1, 0, 0});
    copyButtonSize = GTUtilsDashboard::getCopyButtonSize(clustalwRunNodeId);

    CHECK_SET_ERR(minimumCopyButtonSize.width() <= copyButtonSize.width(),
                  QString("Copy button width of node with ID '%1' is less than %2 pixels: it's width is %3 pixels")
                      .arg(clustalwRunNodeId)
                      .arg(minimumCopyButtonSize.width())
                      .arg(copyButtonSize.width()));
    CHECK_SET_ERR(minimumCopyButtonSize.height() <= copyButtonSize.height(),
                  QString("Copy button height of node with ID '%1' is less than %2 pixels: it's height is %3 pixels")
                      .arg(clustalwRunNodeId)
                      .arg(minimumCopyButtonSize.height())
                      .arg(copyButtonSize.height()));

    //    7. Expand the third-level node "ClustalO run".
    GTUtilsDashboard::expandNode(clustaloRunNodeId);

    //    Expected result:
    //        the third-level node "ClustalO run" has a child node "Command".
    //        the "Command" node has a child node with the command that was used to launch the "ClustalO" external tool.
    //        log contains the command that is on the dashboard.
    QString clustaloRunCommandNodeId = GTUtilsDashboard::getChildNodeId(clustaloRunNodeId, 0);
    CHECK_SET_ERR(GTUtilsDashboard::isNodeVisible(clustaloRunCommandNodeId),
                  QString("Node with ID '%1' is invisible after parent node expanding")
                      .arg(clustaloRunCommandNodeId));

    QString nodeText = GTUtilsDashboard::getNodeText(clustaloRunCommandNodeId);
    QString expectedNodeText = "Command";
    CHECK_SET_ERR(expectedNodeText == nodeText, QString("There is unexpected text of node with ID '%1': expected '%2', got '%3'").arg(clustaloRunCommandNodeId).arg(expectedNodeText).arg(nodeText));

    const QString clustaloRunCommandContentNodeId = GTUtilsDashboard::getChildNodeId(clustaloRunCommandNodeId, 0);
    nodeText = GTUtilsDashboard::getNodeText(clustaloRunCommandContentNodeId);
    QString expectedNodeTextPart = "clustalo";
    CHECK_SET_ERR(nodeText.contains(expectedNodeTextPart),
                  QString("Tool run command doesn't contain the following expected part: '%1'. Full command: '%2'")
                      .arg(expectedNodeText)
                      .arg(nodeText));
    CHECK_SET_ERR(lt.hasMessage(nodeText), QString("Log doesn't contain the following expected message: '%1'").arg(nodeText));

    //    8. Click to the "Copy command" button that is in the third-level node "ClustalO run".
    GTUtilsDashboard::clickCopyButton(clustaloRunNodeId);

    //    Expected result: the clipboard contains the same command as the child of "Command" node that belongs to "ClustalO run" does.
    QString clipboardData = GTClipboard::text();
    CHECK_SET_ERR(clipboardData == nodeText,
                  QString("Clipboard data is not the same as 'Command' node text: clipboard data is '%1', node text is '%2'")
                      .arg(clipboardData)
                      .arg(nodeText));

    //    9. Expand the third-level node "ClustalW run".
    GTUtilsDashboard::collapseNode(clustaloRunNodeId);
    GTUtilsDashboard::expandNode(clustalwRunNodeId);

    //    Expected result:
    //        the third-level node "ClustalW run" has a child node "Command".
    //        the "Command" node has a child node with the command that was used to launch the "ClustalW" external tool.
    //        log contains the command that is on the dashboard.
    const QString clustalwRunCommandNodeId = GTUtilsDashboard::getChildNodeId(clustalwRunNodeId, 0);

    CHECK_SET_ERR(GTUtilsDashboard::isNodeVisible(clustalwRunCommandNodeId),
                  QString("Node with ID '%1' is invisible after parent node expanding")
                      .arg(clustalwRunCommandNodeId));

    nodeText = GTUtilsDashboard::getNodeText(clustalwRunCommandNodeId);
    expectedNodeText = "Command";
    CHECK_SET_ERR(expectedNodeText == nodeText, QString("There is unexpected text of node with ID '%1': expected '%2', got '%3'").arg(clustalwRunCommandNodeId).arg(expectedNodeText).arg(nodeText));

    const QString clustalwRunCommandContentNodeId = GTUtilsDashboard::getChildNodeId(clustalwRunCommandNodeId, 0);
    nodeText = GTUtilsDashboard::getNodeText(clustalwRunCommandContentNodeId);
    expectedNodeTextPart = "clustalw";
    CHECK_SET_ERR(nodeText.contains(expectedNodeTextPart),
                  QString("Tool run command doesn't contain the following expected part: '%1'. Full command: '%2'")
                      .arg(expectedNodeText)
                      .arg(nodeText));
    CHECK_SET_ERR(lt.hasMessage(nodeText), QString("Log doesn't contain the following expected message: '%1'").arg(nodeText));

    //    10. Click to the "Copy command" button that is in the third-level node "ClustalW run".
    GTUtilsDashboard::clickCopyButton(clustalwRunNodeId);

    //    Expected result: the clipboard contains the same command as the child of "Command" node that belongs to "ClustalW run" does.
    clipboardData = GTClipboard::text();
    CHECK_SET_ERR(clipboardData == nodeText,
                  QString("Clipboard data is not the same as 'Command' node text: clipboard data is '%1', node text is '%2'")
                      .arg(clipboardData)
                      .arg(nodeText));
}

GUI_TEST_CLASS_DEFINITION(tool_launch_nodes_test_0002) {
    GTUtilsDialog::waitForDialog(new StartupDialogFiller());

    //    1. Ensure that "ClustalO" tool doesn't contain spaces in its paths.
    const QString toolName = "ClustalO";
    QString toolPath = getExternalToolPath(toolName);
    if (toolPath.contains(" ")) {
        const QString newToolPath = putToolToFolderWithoutSpaces(toolName, getExternalToolDirPath(toolName, "clustalo"), sandBoxDir);
        GTUtilsExternalTools::setToolUrl(toolName, QDir::toNativeSeparators(newToolPath));

        GTUtilsTaskTreeView::waitTaskFinished();
        toolPath = getExternalToolPath(toolName);
        CHECK_SET_ERR(QDir::toNativeSeparators(toolPath) == QDir::toNativeSeparators(newToolPath),
                      QString("'%1' tool path wasn't set properly: expected '%2', got '%3'")
                          .arg(toolName)
                          .arg(newToolPath)
                          .arg(toolPath));
    }

    //    2. Open "_common_data/workflow/dashboard/clustulo.uwl".
    GTFileDialog::openFile(testDir + "_common_data/workflow/dashboard/clustulo.uwl");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    3. Click "Read Alignment" element.
    GTUtilsWorkflowDesigner::click("Read Alignment");

    //    4. Add "data/samples/CLUSTALW/COI.aln" file to "Dataset 1" dataset.
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "samples/CLUSTALW/COI.aln", true);

    //    5. Launch the workflow.
    GTUtilsWorkflowDesigner::runWorkflow();

    //    6. Wait the workflow execution finish.
    GTUtilsTaskTreeView::waitTaskFinished();

    //    7. Switch to an "External Tools" tab.
    GTUtilsDashboard::openTab(GTUtilsDashboard::ExternalTools);

    //    8. Expand the third-level node "ClustalO run".
    const QString clustaloRunNodeId = GTUtilsDashboard::getDescendantNodeId(GTUtilsDashboard::TREE_ROOT_ID, {0, 0, 0});
    GTUtilsDashboard::expandNode(clustaloRunNodeId);

    //    Expected result: the third-level node "ClustalO run" has a child node "Command", which has a child node with the command. The first argument in the command is the path to the "ClustalO" that was set at the beginning, the path is not wrapped with double quotes.
    const QString clustaloRunCommandNodeId = GTUtilsDashboard::getChildNodeId(clustaloRunNodeId, 0);
    CHECK_SET_ERR(GTUtilsDashboard::isNodeVisible(clustaloRunCommandNodeId),
                  QString("Node with ID '%1' is invisible after parent node expanding").arg(clustaloRunCommandNodeId));

    QString nodeText = GTUtilsDashboard::getNodeText(clustaloRunCommandNodeId);
    QString expectedNodeText = "Command";
    CHECK_SET_ERR(expectedNodeText == nodeText,
                  QString("There is unexpected text of node with ID '%1': expected '%2', got '%3'").arg(clustaloRunCommandNodeId).arg(expectedNodeText).arg(nodeText));

    const QString clustaloRunCommandContentNodeId = GTUtilsDashboard::getChildNodeId(clustaloRunCommandNodeId, 0);
    nodeText = GTUtilsDashboard::getNodeText(clustaloRunCommandContentNodeId);
    const QString expectedNodeTextPart = QString("%1 ").arg(toolPath);
    CHECK_SET_ERR(nodeText.startsWith(expectedNodeTextPart),
                  QString("Tool run command doesn't start with the following expected part: '%1'. Full command: '%2'")
                      .arg(expectedNodeTextPart)
                      .arg(nodeText));
}

GUI_TEST_CLASS_DEFINITION(tool_launch_nodes_test_0003) {
    GTUtilsDialog::waitForDialog(new StartupDialogFiller());

    //    1. Copy external tool "ClustalO" to a folder with spaces in its path.
    //    2. Set the copied "ClustalO" in the "Application Settings".
    const QString toolName = "ClustalO";
    QString toolPath = getExternalToolPath(toolName);
    if (!toolPath.contains(" ")) {
        const QString newToolPath = putToolToFolderWithSpaces(toolName, getExternalToolDirPath(toolName, "clustalo"), sandBoxDir);
        GTUtilsExternalTools::setToolUrl(toolName, QDir::toNativeSeparators(newToolPath));

        GTUtilsTaskTreeView::waitTaskFinished();
        toolPath = getExternalToolPath(toolName);
        CHECK_SET_ERR(QDir::toNativeSeparators(toolPath) == QDir::toNativeSeparators(newToolPath),
                      QString("'%1' tool path wasn't set properly: expected '%2', got '%3'")
                          .arg(toolName)
                          .arg(newToolPath)
                          .arg(toolPath));
    }

    //    3. Open "_common_data/workflow/dashboard/clustulo.uwl".
    GTFileDialog::openFile(testDir + "_common_data/workflow/dashboard/clustulo.uwl");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    4. Click "Read Alignment" element.
    GTUtilsWorkflowDesigner::click("Read Alignment");

    //    5. Add "data/samples/CLUSTALW/COI.aln" file to "Dataset 1" dataset.
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "samples/CLUSTALW/COI.aln", true);

    //    6. Launch the workflow.
    GTUtilsWorkflowDesigner::runWorkflow();

    //    7. Wait the workflow execution finish.
    GTUtilsTaskTreeView::waitTaskFinished();

    //    8. Switch to an "External Tools" tab.
    GTUtilsDashboard::openTab(GTUtilsDashboard::ExternalTools);

    //    9. Expand the third-level node "ClustalO run".
    const QString clustaloRunNodeId = GTUtilsDashboard::getDescendantNodeId(GTUtilsDashboard::TREE_ROOT_ID, {0, 0, 0});
    GTUtilsDashboard::expandNode(clustaloRunNodeId);

    //    Expected result: the third-level node "ClustalO run" has a child node "Command", which has a child node with the command. The first argument in the command is the path to the "ClustalO" that was set at the beginning, the path is wrapped with double quotes.
    const QString clustaloRunCommandNodeId = GTUtilsDashboard::getChildNodeId(clustaloRunNodeId, 0);
    CHECK_SET_ERR(GTUtilsDashboard::isNodeVisible(clustaloRunCommandNodeId),
                  QString("Node with ID '%1' is invisible after parent node expanding")
                      .arg(clustaloRunCommandNodeId));

    QString nodeText = GTUtilsDashboard::getNodeText(clustaloRunCommandNodeId);
    QString expectedNodeText = "Command";
    CHECK_SET_ERR(expectedNodeText == nodeText, QString("There is unexpected text of node with ID '%1': expected '%2', got '%3'").arg(clustaloRunCommandNodeId).arg(expectedNodeText).arg(nodeText));

    const QString clustaloRunCommandContentNodeId = GTUtilsDashboard::getChildNodeId(clustaloRunCommandNodeId, 0);
    nodeText = GTUtilsDashboard::getNodeText(clustaloRunCommandContentNodeId);

    const QString expectedNodeTextPart = getQuotedString(toolPath) + " ";
    CHECK_SET_ERR(nodeText.startsWith(expectedNodeTextPart),
                  QString("Tool run command doesn't start with the following expected part: '%1'. Full command: '%2'")
                      .arg(expectedNodeTextPart)
                      .arg(nodeText));
}

GUI_TEST_CLASS_DEFINITION(tool_launch_nodes_test_0004) {
    GTUtilsDialog::waitForDialog(new StartupDialogFiller());

    //    1. Ensure that neither "python" nor "cutadapt" contain spaces in their paths.
    const QString pythonToolName = "Python 3";
    QString pythonToolPath = getExternalToolPath(pythonToolName);
    if (pythonToolPath.contains(" ")) {
        const QString newToolPath = putToolToFolderWithSpaces(pythonToolName, getExternalToolDirPath(pythonToolName, "python"), sandBoxDir);
        GTUtilsExternalTools::setToolUrl(pythonToolName, QDir::toNativeSeparators(newToolPath));

        GTUtilsTaskTreeView::waitTaskFinished();
        pythonToolPath = getExternalToolPath(pythonToolName);
        CHECK_SET_ERR(QDir::toNativeSeparators(pythonToolPath) == QDir::toNativeSeparators(newToolPath),
                      QString("'%1' tool path wasn't set properly: expected '%2', got '%3'")
                          .arg(pythonToolName)
                          .arg(newToolPath)
                          .arg(pythonToolPath));
    }

    //    1. Open "_common_data/workflow/dashboard/cutadapt.uwl".
    GTFileDialog::openFile(testDir + "_common_data/workflow/dashboard/cutadapt.uwl");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Click "Read File URL(s)" element.
    GTUtilsWorkflowDesigner::click("Read File URL(s)");

    //    3. Add "data/samples/FASTQ/eas.fastq" file to "Dataset 1" dataset.
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "samples/FASTQ/eas.fastq", true);

    //    4. Launch the workflow.
    GTUtilsWorkflowDesigner::runWorkflow();

    //    5. Wait the workflow execution finish.
    GTUtilsTaskTreeView::waitTaskFinished();

    //    6. Switch to an "External Tools" tab.
    GTUtilsDashboard::openTab(GTUtilsDashboard::ExternalTools);

    //    7. Expand the third-level node "cutadapt run".
    const QString cutadaptRunNodeId = GTUtilsDashboard::getDescendantNodeId(GTUtilsDashboard::TREE_ROOT_ID, {0, 0, 0});
    GTUtilsDashboard::expandNode(cutadaptRunNodeId);

    //    Expected result: the third-level node "cutadapt run" has a child node "Command", which has a child node with the command. The first argument in the command is the path to the "python", the second argument in the command is the path to "cutadapt". Neither the first nor the second argument is wrapped with double quotes.
    QString cutadaptRunCommandNodeId = GTUtilsDashboard::getChildNodeId(cutadaptRunNodeId, 0);
    CHECK_SET_ERR(GTUtilsDashboard::isNodeVisible(cutadaptRunCommandNodeId),
                  QString("Node with ID '%1' is invisible after parent node expanding")
                      .arg(cutadaptRunCommandNodeId));

    QString nodeText = GTUtilsDashboard::getNodeText(cutadaptRunCommandNodeId);
    QString expectedNodeText = "Command";
    CHECK_SET_ERR(expectedNodeText == nodeText, QString("There is unexpected text of node with ID '%1': expected '%2', got '%3'").arg(cutadaptRunCommandNodeId).arg(expectedNodeText).arg(nodeText));

    QString cutadaptRunCommandContentNodeId = GTUtilsDashboard::getChildNodeId(cutadaptRunCommandNodeId, 0);
    nodeText = GTUtilsDashboard::getNodeText(cutadaptRunCommandContentNodeId).replace("\"",""); // If the test environment contains spaces the params will be wrapped with quotes.
    CHECK_SET_ERR(nodeText.startsWith(pythonToolPath),
                  QString("Tool run command doesn't start with the following expected part: '%1'. Full command: '%2'")
                      .arg(pythonToolPath)
                      .arg(nodeText));
}

GUI_TEST_CLASS_DEFINITION(tool_launch_nodes_test_0005) {
    GTUtilsDialog::waitForDialog(new StartupDialogFiller());

    //    1. Copy external tool "python" to a folder with spaces in its path.
    //    2. Set the copied "python" in the "Application Settings".
    //       As far as Cutadat is a Python moduke either they both contain spaces or not
    const QString pythonToolName = "Python 3";
    QString pythonToolPath = getExternalToolPath(pythonToolName);
    if (!pythonToolPath.contains(" ")) {
        const QString newToolPath = putToolToFolderWithSpaces(pythonToolName, getExternalToolDirPath(pythonToolName, "python"), sandBoxDir);
        GTUtilsExternalTools::setToolUrl(pythonToolName, QDir::toNativeSeparators(newToolPath));

        GTUtilsTaskTreeView::waitTaskFinished();
        pythonToolPath = getExternalToolPath(pythonToolName);
        CHECK_SET_ERR(QDir::toNativeSeparators(pythonToolPath) == QDir::toNativeSeparators(newToolPath),
                      QString("'%1' tool path wasn't set properly: expected '%2', got '%3'")
                          .arg(pythonToolName)
                          .arg(newToolPath)
                          .arg(pythonToolPath));
    }

    //    3. Open "_common_data/workflow/dashboard/cutadapt.uwl".
    GTFileDialog::openFile(testDir + "_common_data/workflow/dashboard/cutadapt.uwl");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    4. Click "Read File URL(s)" element.
    GTUtilsWorkflowDesigner::click("Read File URL(s)");

    //    5. Add "data/samples/FASTQ/eas.fastq" file to "Dataset 1" dataset.
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "samples/FASTQ/eas.fastq", true);

    //    6. Launch the workflow.
    GTUtilsWorkflowDesigner::runWorkflow();

    //    7. Wait the workflow execution finish.
    GTUtilsTaskTreeView::waitTaskFinished();

    //    8. Switch to an "External Tools" tab.
    GTUtilsDashboard::openTab(GTUtilsDashboard::ExternalTools);

    //    9. Expand the third-level node "cutadapt run".
    const QString cutadaptRunNodeId = GTUtilsDashboard::getDescendantNodeId(GTUtilsDashboard::TREE_ROOT_ID, {0, 0, 0});
    GTUtilsDashboard::expandNode(cutadaptRunNodeId);

    //    Expected result: the third-level node "cutadapt run" has a child node "Command", which has a child node with the command. The first argument in the command is the path to the "python" that was set at the beginning, the path is wrapped with double quotes.
    const QString cutadaptRunCommandNodeId = GTUtilsDashboard::getChildNodeId(cutadaptRunNodeId, 0);
    CHECK_SET_ERR(GTUtilsDashboard::isNodeVisible(cutadaptRunCommandNodeId),
                  QString("Node with ID '%1' is invisible after parent node expanding")
                      .arg(cutadaptRunCommandNodeId));

    QString nodeText = GTUtilsDashboard::getNodeText(cutadaptRunCommandNodeId);
    QString expectedNodeText = "Command";
    CHECK_SET_ERR(expectedNodeText == nodeText, QString("There is unexpected text of node with ID '%1': expected '%2', got '%3'").arg(cutadaptRunCommandNodeId).arg(expectedNodeText).arg(nodeText));

    const QString cutadaptRunCommandContentNodeId = GTUtilsDashboard::getChildNodeId(cutadaptRunCommandNodeId, 0);
    nodeText = GTUtilsDashboard::getNodeText(cutadaptRunCommandContentNodeId);
    const QString expectedNodeTextPart = getQuotedString(pythonToolPath) + " ";
    CHECK_SET_ERR(nodeText.startsWith(expectedNodeTextPart),
                  QString("Tool run command doesn't start with the following expected part: '%1'. Full command: '%2'")
                      .arg(expectedNodeTextPart)
                      .arg(nodeText));
}

GUI_TEST_CLASS_DEFINITION(tool_launch_nodes_test_0008) {
    GTUtilsDialog::waitForDialog(new StartupDialogFiller());

    //    1. Ensure that file "data/samples/FASTQ/eas.fastq' is in the folder without spaces in the path.
    QString inputFileUrl = QFileInfo(dataDir + "samples/FASTQ/eas.fastq").absoluteFilePath();
    if (inputFileUrl.contains(" ")) {
        QString newInputFileUrl = QFileInfo(sandBoxDir + "eas.fastq").absoluteFilePath();
        CHECK_SET_ERR(!newInputFileUrl.contains(" "),
                      QString("Can't copy input file to a folder without spaces in the path, sandbox dir path contains spaces: %1").arg(sandBoxDir));
        GTFile::copy(inputFileUrl, newInputFileUrl);
        inputFileUrl = newInputFileUrl;
    }

    //    2. Open "_common_data/workflow/dashboard/cutadapt.uwl".
    GTFileDialog::openFile(testDir + "_common_data/workflow/dashboard/cutadapt.uwl");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    3. Click "Read File URL(s)" element.
    GTUtilsWorkflowDesigner::click("Read File URL(s)");

    //    4. Add "eas.fastq" from the step 1 file to "Dataset 1" dataset.
    GTUtilsWorkflowDesigner::setDatasetInputFile(inputFileUrl, true);

    //    5. Launch the workflow.
    GTUtilsWorkflowDesigner::runWorkflow();

    //    6. Wait the workflow execution finish.
    GTUtilsTaskTreeView::waitTaskFinished();

    //    7. Switch to an "External Tools" tab.
    GTUtilsDashboard::openTab(GTUtilsDashboard::ExternalTools);

    //    8. Expand the third-level node "cutadapt run".
    const QString cutadaptRunNodeId = GTUtilsDashboard::getDescendantNodeId(GTUtilsDashboard::TREE_ROOT_ID, {0, 0, 0});
    GTUtilsDashboard::expandNode(cutadaptRunNodeId);

    //    Expected result: the third-level node "cutadapt run" has a child node "Command", which has a child node with the command. The last argument in the command is the path to the input file. It is not wrapped with double quotes.
    const QString cutadaptRunCommandNodeId = GTUtilsDashboard::getChildNodeId(cutadaptRunNodeId, 0);
    CHECK_SET_ERR(GTUtilsDashboard::isNodeVisible(cutadaptRunCommandNodeId),
                  QString("Node with ID '%1' is invisible after parent node expanding")
                      .arg(cutadaptRunCommandNodeId));

    QString nodeText = GTUtilsDashboard::getNodeText(cutadaptRunCommandNodeId);
    QString expectedNodeText = "Command";
    CHECK_SET_ERR(expectedNodeText == nodeText, QString("There is unexpected text of node with ID '%1': expected '%2', got '%3'").arg(cutadaptRunCommandNodeId).arg(expectedNodeText).arg(nodeText));

    const QString cutadaptRunCommandContentNodeId = GTUtilsDashboard::getChildNodeId(cutadaptRunCommandNodeId, 0);
    nodeText = GTUtilsDashboard::getNodeText(cutadaptRunCommandContentNodeId);
    const QString expectedNodeTextPart = QString(" %1").arg(inputFileUrl);
    CHECK_SET_ERR(nodeText.endsWith(expectedNodeTextPart),
                  QString("Tool run command doesn't end with the following expected part: '%1'. Full command: '%2'")
                      .arg(expectedNodeTextPart)
                      .arg(nodeText));
}

GUI_TEST_CLASS_DEFINITION(tool_launch_nodes_test_0009) {
    GTUtilsDialog::waitForDialog(new StartupDialogFiller());

    //    1. Copy "data/samples/FASTQ/eas.fastq" to a folder with spaces in its path.
    QString inputFileUrl = QFileInfo(dataDir + "samples/FASTQ/eas.fastq").absoluteFilePath();
    if (!inputFileUrl.contains(" ")) {
        QString newInputFileUrl = QFileInfo(sandBoxDir + "tool launch nodes test 0009").absoluteFilePath();
        QDir().mkpath(newInputFileUrl);
        newInputFileUrl += "/" + QFileInfo(inputFileUrl).fileName();
        GTFile::copy(inputFileUrl, newInputFileUrl);
        inputFileUrl = newInputFileUrl;
    }

    //    2. Open "_common_data/workflow/dashboard/cutadapt.uwl".
    GTFileDialog::openFile(testDir + "_common_data/workflow/dashboard/cutadapt.uwl");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    3. Click "Read File URL(s)" element.
    GTUtilsWorkflowDesigner::click("Read File URL(s)");

    //    4. Add the copied file to "Dataset 1" dataset.
    GTUtilsWorkflowDesigner::setDatasetInputFile(inputFileUrl, true);

    //    5. Launch the workflow.
    GTUtilsWorkflowDesigner::runWorkflow();

    //    6. Wait the workflow execution finish.
    GTUtilsTaskTreeView::waitTaskFinished();

    //    7. Switch to an "External Tools" tab.
    GTUtilsDashboard::openTab(GTUtilsDashboard::ExternalTools);

    //    8. Expand the third-level node "cutadapt run".
    const QString cutadaptRunNodeId = GTUtilsDashboard::getDescendantNodeId(GTUtilsDashboard::TREE_ROOT_ID, {0, 0, 0});
    GTUtilsDashboard::expandNode(cutadaptRunNodeId);

    //    Expected result: the third-level node "cutadapt run" has a child node "Command", which has a child node with the command. The last argument in the command is the path to the input file. It is wrapped with double quotes.
    const QString cutadaptRunCommandNodeId = GTUtilsDashboard::getChildNodeId(cutadaptRunNodeId, 0);
    CHECK_SET_ERR(GTUtilsDashboard::isNodeVisible(cutadaptRunCommandNodeId),
                  QString("Node with ID '%1' is invisible after parent node expanding")
                      .arg(cutadaptRunCommandNodeId));

    QString nodeText = GTUtilsDashboard::getNodeText(cutadaptRunCommandNodeId);
    QString expectedNodeText = "Command";
    CHECK_SET_ERR(expectedNodeText == nodeText, QString("There is unexpected text of node with ID '%1': expected '%2', got '%3'").arg(cutadaptRunCommandNodeId).arg(expectedNodeText).arg(nodeText));

    const QString cutadaptRunCommandContentNodeId = GTUtilsDashboard::getChildNodeId(cutadaptRunCommandNodeId, 0);
    nodeText = GTUtilsDashboard::getNodeText(cutadaptRunCommandContentNodeId);
    const QString expectedNodeTextPart = QString(" \"%1\"").arg(inputFileUrl);
    CHECK_SET_ERR(nodeText.endsWith(expectedNodeTextPart),
                  QString("Tool run command doesn't end with the following expected part: '%1'. Full command: '%2'")
                      .arg(expectedNodeTextPart)
                      .arg(nodeText));
}

GUI_TEST_CLASS_DEFINITION(tool_launch_nodes_test_0010) {
    GTUtilsDialog::waitForDialog(new StartupDialogFiller());

    //    1. Set the "_common_data/workflow/dashboard/fake_tools/fake_spades_no_output.py" as "SPAdes" external tool in the "Application Settings".
    const QString spadesToolName = "SPAdes";
    const QString spadesToolPath = QDir::toNativeSeparators(QFileInfo(testDir + "_common_data/workflow/dashboard/fake_tools/fake_spades_no_output.py").absoluteFilePath());
    GTUtilsExternalTools::setToolUrl(spadesToolName, spadesToolPath);
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Open "_common_data/workflow/dashboard/spades.uwl".
    GTFileDialog::openFile(testDir + "_common_data/workflow/dashboard/spades.uwl");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    3. Click "Read File URL(s)" element.
    GTUtilsWorkflowDesigner::click("Read File URL(s)");

    //    4. Add "data/samples/FASTQ/eas.fastq" file to "Dataset 1" dataset.
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "samples/FASTQ/eas.fastq", true);

    //    5. Launch the workflow.
    GTUtilsWorkflowDesigner::runWorkflow();

    //    6. Wait the workflow execution finish.
    GTUtilsTaskTreeView::waitTaskFinished();

    //    7. Switch to an "External Tools" tab.
    GTUtilsDashboard::openTab(GTUtilsDashboard::ExternalTools);

    //    8. Expand the third-level node "SPAdes run".
    const QString spadesRunNodeId = GTUtilsDashboard::getDescendantNodeId(GTUtilsDashboard::TREE_ROOT_ID, {0, 0, 0});
    GTUtilsDashboard::expandNode(spadesRunNodeId);

    //    Expected result: the third-level node "SPAdes run" doesn't have child nodes "Output log (stdout)" and "Output log (stderr)".
    const int spadesRunNodeChildrenCount = GTUtilsDashboard::getChildrenNodesCount(spadesRunNodeId);
    const int expectedChildrenCount = 1;
    CHECK_SET_ERR(expectedChildrenCount == spadesRunNodeChildrenCount,
                  QString("Node with ID '%1' has unexpected children count: expected %2, got %3")
                      .arg(spadesRunNodeId)
                      .arg(expectedChildrenCount)
                      .arg(spadesRunNodeChildrenCount));

    const QString spadesRunCommandNodeId = GTUtilsDashboard::getChildNodeId(spadesRunNodeId, 0);
    CHECK_SET_ERR(GTUtilsDashboard::isNodeVisible(spadesRunCommandNodeId),
                  QString("Node with ID '%1' is invisible after parent node expanding")
                      .arg(spadesRunCommandNodeId));

    QString nodeText = GTUtilsDashboard::getNodeText(spadesRunCommandNodeId);
    QString expectedNodeText = "Command";
    CHECK_SET_ERR(expectedNodeText == nodeText, QString("There is unexpected text of node with ID '%1': expected '%2', got '%3'").arg(spadesRunCommandNodeId).arg(expectedNodeText).arg(nodeText));
}

GUI_TEST_CLASS_DEFINITION(tool_launch_nodes_test_0011) {
    GTUtilsDialog::waitForDialog(new StartupDialogFiller());

    //    1. Set the "_common_data/workflow/dashboard/fake_tools/fake_spades_stdout_only.py" as "SPAdes" external tool in the "Application Settings".
    const QString spadesToolName = "SPAdes";
    const QString spadesToolPath = QDir::toNativeSeparators(QFileInfo(testDir + "_common_data/workflow/dashboard/fake_tools/fake_spades_stdout_only.py").absoluteFilePath());
    GTUtilsExternalTools::setToolUrl(spadesToolName, spadesToolPath);
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Open "_common_data/workflow/dashboard/spades.uwl".
    GTFileDialog::openFile(testDir + "_common_data/workflow/dashboard/spades.uwl");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    3. Click "Read File URL(s)" element.
    GTUtilsWorkflowDesigner::click("Read File URL(s)");

    //    4. Add "data/samples/FASTQ/eas.fastq" file to "Dataset 1" dataset.
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "samples/FASTQ/eas.fastq", true);

    //    5. Launch the workflow.
    GTUtilsWorkflowDesigner::runWorkflow();

    //    6. Wait the workflow execution finish.
    GTUtilsTaskTreeView::waitTaskFinished();

    //    7. Switch to an "External Tools" tab.
    GTUtilsDashboard::openTab(GTUtilsDashboard::ExternalTools);

    //    8. Expand the third-level node "SPAdes run".
    const QString spadesRunNodeId = GTUtilsDashboard::getDescendantNodeId(GTUtilsDashboard::TREE_ROOT_ID, {0, 0, 0});
    GTUtilsDashboard::expandNode(spadesRunNodeId);

    //    Expected result: the third-level node "SPAdes run" have a child node "Output log (stdout)" and doesn't have a child node "Output log (stderr)".
    const int spadesRunNodeChildrenCount = GTUtilsDashboard::getChildrenNodesCount(spadesRunNodeId);
    const int expectedChildrenCount = 2;
    CHECK_SET_ERR(expectedChildrenCount == spadesRunNodeChildrenCount,
                  QString("Node with ID '%1' has unexpected children count: expected %2, got %3")
                      .arg(spadesRunNodeId)
                      .arg(expectedChildrenCount)
                      .arg(spadesRunNodeChildrenCount));

    const QString spadesRunCommandNodeId = GTUtilsDashboard::getChildNodeId(spadesRunNodeId, 0);
    CHECK_SET_ERR(GTUtilsDashboard::isNodeVisible(spadesRunCommandNodeId),
                  QString("Node with ID '%1' is invisible after parent node expanding")
                      .arg(spadesRunCommandNodeId));

    QString nodeText = GTUtilsDashboard::getNodeText(spadesRunCommandNodeId);
    QString expectedNodeText = "Command";
    CHECK_SET_ERR(expectedNodeText == nodeText, QString("There is unexpected text of node with ID '%1': expected '%2', got '%3'").arg(spadesRunCommandNodeId).arg(expectedNodeText).arg(nodeText));

    const QString spadesRunStdoutNodeId = GTUtilsDashboard::getChildNodeId(spadesRunNodeId, 1);
    CHECK_SET_ERR(GTUtilsDashboard::isNodeVisible(spadesRunStdoutNodeId),
                  QString("Node with ID '%1' is invisible after parent node expanding")
                      .arg(spadesRunStdoutNodeId));

    nodeText = GTUtilsDashboard::getNodeText(spadesRunStdoutNodeId);
    expectedNodeText = "Output log (stdout)";
    CHECK_SET_ERR(expectedNodeText == nodeText, QString("There is unexpected text of node with ID '%1': expected '%2', got '%3'").arg(spadesRunStdoutNodeId).arg(expectedNodeText).arg(nodeText));
}

GUI_TEST_CLASS_DEFINITION(tool_launch_nodes_test_0012) {
    GTUtilsDialog::waitForDialog(new StartupDialogFiller());

    //    1. Set the "_common_data/workflow/dashboard/fake_tools/fake_spades_stderr_only.py" as "SPAdes" external tool in the "Application Settings".
    const QString spadesToolName = "SPAdes";
    const QString spadesToolPath = QDir::toNativeSeparators(QFileInfo(testDir + "_common_data/workflow/dashboard/fake_tools/fake_spades_stderr_only.py").absoluteFilePath());
    GTUtilsExternalTools::setToolUrl(spadesToolName, spadesToolPath);
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Open "_common_data/workflow/dashboard/spades.uwl".
    GTFileDialog::openFile(testDir + "_common_data/workflow/dashboard/spades.uwl");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    3. Click "Read File URL(s)" element.
    GTUtilsWorkflowDesigner::click("Read File URL(s)");

    //    4. Add "data/samples/FASTQ/eas.fastq" file to "Dataset 1" dataset.
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "samples/FASTQ/eas.fastq", true);

    //    5. Launch the workflow.
    GTUtilsWorkflowDesigner::runWorkflow();

    //    6. Wait the workflow execution finish.
    GTUtilsTaskTreeView::waitTaskFinished();

    //    7. Switch to an "External Tools" tab.
    GTUtilsDashboard::openTab(GTUtilsDashboard::ExternalTools);

    //    8. Expand the third-level node "SPAdes run".
    const QString spadesRunNodeId = GTUtilsDashboard::getDescendantNodeId(GTUtilsDashboard::TREE_ROOT_ID, {0, 0, 0});
    GTUtilsDashboard::expandNode(spadesRunNodeId);

    //    Expected result: the third-level node "SPAdes run" has a child node "Output log (stderr)" and doesn't have a child node "Output log (stdout)".
    const int spadesRunNodeChildrenCount = GTUtilsDashboard::getChildrenNodesCount(spadesRunNodeId);
    const int expectedChildrenCount = 2;
    CHECK_SET_ERR(expectedChildrenCount == spadesRunNodeChildrenCount,
                  QString("Node with ID '%1' has unexpected children count: expected %2, got %3")
                      .arg(spadesRunNodeId)
                      .arg(expectedChildrenCount)
                      .arg(spadesRunNodeChildrenCount));

    const QString spadesRunCommandNodeId = GTUtilsDashboard::getChildNodeId(spadesRunNodeId, 0);
    CHECK_SET_ERR(GTUtilsDashboard::isNodeVisible(spadesRunCommandNodeId),
                  QString("Node with ID '%1' is invisible after parent node expanding")
                      .arg(spadesRunCommandNodeId));

    QString nodeText = GTUtilsDashboard::getNodeText(spadesRunCommandNodeId);
    QString expectedNodeText = "Command";
    CHECK_SET_ERR(expectedNodeText == nodeText, QString("There is unexpected text of node with ID '%1': expected '%2', got '%3'").arg(spadesRunCommandNodeId).arg(expectedNodeText).arg(nodeText));

    const QString spadesRunStderrNodeId = GTUtilsDashboard::getChildNodeId(spadesRunNodeId, 1);
    CHECK_SET_ERR(GTUtilsDashboard::isNodeVisible(spadesRunStderrNodeId),
                  QString("Node with ID '%1' is invisible after parent node expanding")
                      .arg(spadesRunStderrNodeId));

    nodeText = GTUtilsDashboard::getNodeText(spadesRunStderrNodeId);
    expectedNodeText = "Output log (stderr)";
    CHECK_SET_ERR(expectedNodeText == nodeText, QString("There is unexpected text of node with ID '%1': expected '%2', got '%3'").arg(spadesRunStderrNodeId).arg(expectedNodeText).arg(nodeText));
}

GUI_TEST_CLASS_DEFINITION(tool_launch_nodes_test_0013) {
    GTUtilsDialog::waitForDialog(new StartupDialogFiller());

    //    1. Set the "_common_data/workflow/dashboard/fake_tools/fake_spades_stdout_and_stderr.py" as "SPAdes" external tool in the "Application Settings".
    const QString spadesToolName = "SPAdes";
    const QString spadesToolPath = QDir::toNativeSeparators(QFileInfo(testDir + "_common_data/workflow/dashboard/fake_tools/fake_spades_stdout_and_stderr.py").absoluteFilePath());
    GTUtilsExternalTools::setToolUrl(spadesToolName, spadesToolPath);
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Open "_common_data/workflow/dashboard/spades.uwl".
    GTFileDialog::openFile(testDir + "_common_data/workflow/dashboard/spades.uwl");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    3. Click "Read File URL(s)" element.
    GTUtilsWorkflowDesigner::click("Read File URL(s)");

    //    4. Add "data/samples/FASTQ/eas.fastq" file to "Dataset 1" dataset.
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "samples/FASTQ/eas.fastq", true);

    //    5. Launch the workflow.
    GTUtilsWorkflowDesigner::runWorkflow();

    //    6. Wait the workflow execution finish.
    GTUtilsTaskTreeView::waitTaskFinished();

    //    7. Switch to an "External Tools" tab.
    GTUtilsDashboard::openTab(GTUtilsDashboard::ExternalTools);

    //    8. Expand the third-level node "SPAdes run".
    const QString spadesRunNodeId = GTUtilsDashboard::getDescendantNodeId(GTUtilsDashboard::TREE_ROOT_ID, {0, 0, 0});
    GTUtilsDashboard::expandNode(spadesRunNodeId);

    //    Expected result: the third-level node "SPAdes run" has child nodes "Output log (stdout)" and "Output log (stderr)".
    const int spadesRunNodeChildrenCount = GTUtilsDashboard::getChildrenNodesCount(spadesRunNodeId);
    const int expectedChildrenCount = 3;
    CHECK_SET_ERR(expectedChildrenCount == spadesRunNodeChildrenCount,
                  QString("Node with ID '%1' has unexpected children count: expected %2, got %3")
                      .arg(spadesRunNodeId)
                      .arg(expectedChildrenCount)
                      .arg(spadesRunNodeChildrenCount));

    const QString spadesRunCommandNodeId = GTUtilsDashboard::getChildNodeId(spadesRunNodeId, 0);
    CHECK_SET_ERR(GTUtilsDashboard::isNodeVisible(spadesRunCommandNodeId),
                  QString("Node with ID '%1' is invisible after parent node expanding")
                      .arg(spadesRunCommandNodeId));

    QString nodeText = GTUtilsDashboard::getNodeText(spadesRunCommandNodeId);
    QString expectedNodeText = "Command";
    CHECK_SET_ERR(expectedNodeText == nodeText, QString("There is unexpected text of node with ID '%1': expected '%2', got '%3'").arg(spadesRunCommandNodeId).arg(expectedNodeText).arg(nodeText));

    const QString oneOutNodeId = GTUtilsDashboard::getChildNodeId(spadesRunNodeId, 1);
    const QString anotherOutNodeId = GTUtilsDashboard::getChildNodeId(spadesRunNodeId, 2);

    QString spadesRunStdoutNodeId;
    QString spadesRunStderrNodeId;
    if (oneOutNodeId.contains("stderr")) {
        spadesRunStdoutNodeId = anotherOutNodeId;
        spadesRunStderrNodeId = oneOutNodeId;
    } else {
        spadesRunStdoutNodeId = oneOutNodeId;
        spadesRunStderrNodeId = anotherOutNodeId;
    }

    CHECK_SET_ERR(GTUtilsDashboard::isNodeVisible(spadesRunStdoutNodeId),
                  QString("Node with ID '%1' is invisible after parent node expanding")
                      .arg(spadesRunStdoutNodeId));

    nodeText = GTUtilsDashboard::getNodeText(spadesRunStdoutNodeId);
    expectedNodeText = "Output log (stdout)";
    CHECK_SET_ERR(expectedNodeText == nodeText, QString("There is unexpected text of node with ID '%1': expected '%2', got '%3'").arg(spadesRunStdoutNodeId).arg(expectedNodeText).arg(nodeText));

    CHECK_SET_ERR(GTUtilsDashboard::isNodeVisible(spadesRunStderrNodeId),
                  QString("Node with ID '%1' is invisible after parent node expanding")
                      .arg(spadesRunStderrNodeId));

    nodeText = GTUtilsDashboard::getNodeText(spadesRunStderrNodeId);
    expectedNodeText = "Output log (stderr)";
    CHECK_SET_ERR(expectedNodeText == nodeText, QString("There is unexpected text of node with ID '%1': expected '%2', got '%3'").arg(spadesRunStderrNodeId).arg(expectedNodeText).arg(nodeText));
}

GUI_TEST_CLASS_DEFINITION(tool_launch_nodes_test_0015) {
    GTUtilsDialog::waitForDialog(new StartupDialogFiller());

    //    1. Set the "_common_data/workflow/dashboard/fake_tools/fake_spades_stdout_only.py" as "SPAdes" external tool in the "Application Settings".
    const QString spadesToolName = "SPAdes";
    const QString spadesToolPath = QDir::toNativeSeparators(QFileInfo(testDir + "_common_data/workflow/dashboard/fake_tools/fake_spades_stdout_only.py").absoluteFilePath());
    GTUtilsExternalTools::setToolUrl(spadesToolName, spadesToolPath);
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Open "_common_data/workflow/dashboard/spades.uwl".
    GTFileDialog::openFile(testDir + "_common_data/workflow/dashboard/spades.uwl");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    3. Click "Read File URL(s)" element.
    GTUtilsWorkflowDesigner::click("Read File URL(s)");

    //    4. Add "data/samples/FASTQ/eas.fastq" file to "Dataset 1" dataset.
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "samples/FASTQ/eas.fastq", true);

    //    5. Launch the workflow.
    GTUtilsWorkflowDesigner::runWorkflow();

    //    6. Wait the workflow execution finish.
    GTUtilsTaskTreeView::waitTaskFinished();

    //    7. Switch to an "External Tools" tab.
    GTUtilsDashboard::openTab(GTUtilsDashboard::ExternalTools);

    //    8. Expand the third-level node "SPAdes run".
    const QString spadesRunNodeId = GTUtilsDashboard::getDescendantNodeId(GTUtilsDashboard::TREE_ROOT_ID, {0, 0, 0});
    GTUtilsDashboard::expandNode(spadesRunNodeId);

    //    Expected result:
    //        the third-level node "SPAdes run" has child node "Output log (stdout)".
    const QString stdoutTitleNodeId = GTUtilsDashboard::getChildNodeId(spadesRunNodeId, 1);

    QString nodeText = GTUtilsDashboard::getNodeText(stdoutTitleNodeId);
    QString expectedNodeText = "Output log (stdout)";
    CHECK_SET_ERR(nodeText.startsWith(expectedNodeText),
                  QString("Node with ID '%1' has unexpected text: expected '%2', got '%3'")
                      .arg(stdoutTitleNodeId)
                      .arg(expectedNodeText)
                      .arg(nodeText));

    const QString stdoutDataNodeId = GTUtilsDashboard::getChildNodeId(stdoutTitleNodeId, 0);

    //        the "Output log (stdout)" node content has significant size (~100 kb of text).
    nodeText = GTUtilsDashboard::getNodeText(stdoutDataNodeId);
    int expectedLength = 80000;
    CHECK_SET_ERR(nodeText.size() > expectedLength,
                  QString("Node with ID '%1' text is not long enough: expected length is at least %2, actual text length is %3")
                      .arg(stdoutDataNodeId)
                      .arg(expectedLength)
                      .arg(nodeText.size()));

    //        the "Output log (stdout)" node content ends with the following message: "The external tool output is too large and can't be visualized on the dashboard. Find full output in this file.", where "this file" is a link to the log file.
    QString expectedNodeTextPart = "The external tool output is too large and can't be visualized on the dashboard.";
    CHECK_SET_ERR(nodeText.contains(expectedNodeTextPart),
                  QString("Node with ID '%1' text doesn't contains the expected part: expected part '%2', last 300 symbols of the text '%3'")
                      .arg(stdoutDataNodeId)
                      .arg(expectedNodeTextPart)
                      .arg(nodeText.right(300)));

    //        the link from the previous item is correct: it points to a file with the same data; the file size is not less than the node contents size; the file name contains "stdout".
    const QString url = GTUtilsDashboard::getLogUrlFromOutputContent(stdoutDataNodeId);

    const QFileInfo fileInfo(url);
    CHECK_SET_ERR(fileInfo.exists(), QString("File with URL '%1' doesn't exist").arg(url));
    CHECK_SET_ERR(fileInfo.size() >= nodeText.size(),
                  QString("File with URL '%1' size is less than node '%2' text")
                      .arg(url)
                      .arg(stdoutDataNodeId));

    const QString expectedFileNamePart = "stdout";
    CHECK_SET_ERR(fileInfo.fileName().contains(expectedFileNamePart),
                  QString("File name '%1' doesn't contain '%2'")
                      .arg(url)
                      .arg(expectedFileNamePart));

    QString fileData = GTFile::readAll(url).replace("\n", "<br/>").replace("\r", "");
    CHECK_SET_ERR(fileData.startsWith(nodeText.left(500)),
                  QString("File '%1' content is not equal to the expected text: '%2', file: '%3'")
                      .arg(url)
                      .arg(nodeText.left(500))
                      .arg(fileData.left(500)));
}

GUI_TEST_CLASS_DEFINITION(tool_launch_nodes_test_0016) {
    GTUtilsDialog::waitForDialog(new StartupDialogFiller());

    //    1. Set the "_common_data/workflow/dashboard/fake_tools/fake_spades_stderr_only.py" as "SPAdes" external tool in the "Application Settings".
    const QString spadesToolName = "SPAdes";
    const QString spadesToolPath = QDir::toNativeSeparators(QFileInfo(testDir + "_common_data/workflow/dashboard/fake_tools/fake_spades_stderr_only.py").absoluteFilePath());
    GTUtilsExternalTools::setToolUrl(spadesToolName, spadesToolPath);
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Open "_common_data/workflow/dashboard/spades.uwl".
    GTFileDialog::openFile(testDir + "_common_data/workflow/dashboard/spades.uwl");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    3. Click "Read File URL(s)" element.
    GTUtilsWorkflowDesigner::click("Read File URL(s)");

    //    4. Add "data/samples/FASTQ/eas.fastq" file to "Dataset 1" dataset.
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "samples/FASTQ/eas.fastq", true);

    //    5. Launch the workflow.
    GTUtilsWorkflowDesigner::runWorkflow();

    //    6. Wait the workflow execution finish.
    GTUtilsTaskTreeView::waitTaskFinished();

    //    7. Switch to an "External Tools" tab.
    GTUtilsDashboard::openTab(GTUtilsDashboard::ExternalTools);

    //    8. Expand the third-level node "SPAdes run".
    const QString spadesRunNodeId = GTUtilsDashboard::getDescendantNodeId(GTUtilsDashboard::TREE_ROOT_ID, {0, 0, 0});
    GTUtilsDashboard::expandNode(spadesRunNodeId);

    //    Expected result:
    //        the third-level node "SPAdes run" has child node "Output log (stderr)".
    const QString stderrTitleNodeId = GTUtilsDashboard::getChildNodeId(spadesRunNodeId, 1);

    QString nodeText = GTUtilsDashboard::getNodeText(stderrTitleNodeId);
    QString expectedNodeText = "Output log (stderr)";
    CHECK_SET_ERR(expectedNodeText == nodeText,
                  QString("Node with ID '%1' has unexpected text: expected '%2', got '%3'")
                      .arg(stderrTitleNodeId)
                      .arg(expectedNodeText)
                      .arg(nodeText));

    const QString stderrDataNodeId = GTUtilsDashboard::getChildNodeId(stderrTitleNodeId, 0);

    //        the "Output log (stderr)" node content has significant size (~100 kb of text).
    nodeText = GTUtilsDashboard::getNodeText(stderrDataNodeId);
    int expectedLength = 80000;
    CHECK_SET_ERR(nodeText.size() > expectedLength,
                  QString("Node with ID '%1' text is not long enough: expected length is at least %2, actual text length is %3")
                      .arg(stderrDataNodeId)
                      .arg(expectedLength)
                      .arg(nodeText.size()));

    //        the "Output log (stderr)" node content ends with the following message: "The external tool output is too large and can't be visualized on the dashboard. Find full output in this file", where "this file" is a link to the log file.
    const QString expectedNodeTextPart = "The external tool output is too large and can't be visualized on the dashboard.";
    CHECK_SET_ERR(nodeText.contains(expectedNodeTextPart),
                  QString("Node with ID '%1' text doesn't ends with the expected part: expected part '%2', last 300 symbols of the text '%3'")
                      .arg(stderrDataNodeId)
                      .arg(expectedNodeTextPart)
                      .arg(nodeText.right(300)));

    //        the link from the previous item is correct: it points to a file with the same data; the file size is not less than the node contents size; the file name contains "stderr".
    const QString url = GTUtilsDashboard::getLogUrlFromOutputContent(stderrDataNodeId);

    const QFileInfo fileInfo(url);
    CHECK_SET_ERR(fileInfo.exists(), QString("File with URL '%1' doesn't exist").arg(url));
    CHECK_SET_ERR(fileInfo.size() >= nodeText.size(),
                  QString("File with URL '%1' size is less than node '%2' text")
                      .arg(url)
                      .arg(stderrDataNodeId));

    const QString expectedFileNamePart = "stderr";
    CHECK_SET_ERR(fileInfo.fileName().contains(expectedFileNamePart),
                  QString("File name '%1' doesn't contain '%2'")
                      .arg(url)
                      .arg(expectedFileNamePart));

    QString fileData = GTFile::readAll(url).replace("\n", "<br/>").replace("\r", "");
    ;
    CHECK_SET_ERR(fileData.startsWith(nodeText.left(500)),
                  QString("File '%1' content is not equal to the expected text: '%2', file: '%3'")
                      .arg(url)
                      .arg(nodeText.left(500))
                      .arg(fileData.left(500)));
}

GUI_TEST_CLASS_DEFINITION(tool_launch_nodes_test_0017) {
    GTUtilsDialog::waitForDialog(new StartupDialogFiller());

    //    1. Set the "_common_data/workflow/dashboard/fake_tools/fake_spades_stdout_and_stderr.py" as "SPAdes" external tool in the "Application Settings".
    const QString spadesToolName = "SPAdes";
    const QString spadesToolPath = QDir::toNativeSeparators(QFileInfo(testDir + "_common_data/workflow/dashboard/fake_tools/fake_spades_stdout_and_stderr.py").absoluteFilePath());
    GTUtilsExternalTools::setToolUrl(spadesToolName, spadesToolPath);
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Open "_common_data/workflow/dashboard/spades.uwl".
    GTFileDialog::openFile(testDir + "_common_data/workflow/dashboard/spades.uwl");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    3. Click "Read File URL(s)" element.
    GTUtilsWorkflowDesigner::click("Read File URL(s)");

    //    4. Add "data/samples/FASTQ/eas.fastq" file to "Dataset 1" dataset.
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "samples/FASTQ/eas.fastq", true);

    //    5. Launch the workflow.
    GTUtilsWorkflowDesigner::runWorkflow();

    //    6. Wait the workflow execution finish.
    GTUtilsTaskTreeView::waitTaskFinished();

    //    7. Switch to an "External Tools" tab.
    GTUtilsDashboard::openTab(GTUtilsDashboard::ExternalTools);

    //    8. Expand the third-level node "SPAdes run".
    const QString spadesRunNodeId = GTUtilsDashboard::getDescendantNodeId(GTUtilsDashboard::TREE_ROOT_ID, {0, 0, 0});
    GTUtilsDashboard::expandNode(spadesRunNodeId);

    //    Expected result:
    //        the third-level node "SPAdes run" has child nodes "Output log (stdout)" and "Output log (stderr)".
    const QString oneOutNodeId = GTUtilsDashboard::getChildNodeId(spadesRunNodeId, 1);
    const QString anotherOutNodeId = GTUtilsDashboard::getChildNodeId(spadesRunNodeId, 2);

    QString stdoutTitleNodeId;
    QString stderrTitleNodeId;
    if (oneOutNodeId.contains("stderr")) {
        stdoutTitleNodeId = anotherOutNodeId;
        stderrTitleNodeId = oneOutNodeId;
    } else {
        stdoutTitleNodeId = oneOutNodeId;
        stderrTitleNodeId = anotherOutNodeId;
    }

    QString nodeText = GTUtilsDashboard::getNodeText(stderrTitleNodeId);
    QString expectedNodeText = "Output log (stderr)";
    CHECK_SET_ERR(expectedNodeText == nodeText,
                  QString("Node with ID '%1' has unexpected text: expected '%2', got '%3'")
                      .arg(stderrTitleNodeId)
                      .arg(expectedNodeText)
                      .arg(nodeText));

    nodeText = GTUtilsDashboard::getNodeText(stdoutTitleNodeId);
    expectedNodeText = "Output log (stdout)";
    CHECK_SET_ERR(expectedNodeText == nodeText,
                  QString("Node with ID '%1' has unexpected text: expected '%2', got '%3'")
                      .arg(stdoutTitleNodeId)
                      .arg(expectedNodeText)
                      .arg(nodeText));

    const QString stdoutDataNodeId = GTUtilsDashboard::getChildNodeId(stdoutTitleNodeId, 0);
    const QString stderrDataNodeId = GTUtilsDashboard::getChildNodeId(stderrTitleNodeId, 0);

    //        both "Output log (stdout)" and "Output log (stderr)" nodes contents have significant size (~100 kb of text).
    QString stdoutNodeText = GTUtilsDashboard::getNodeText(stdoutDataNodeId);
    int expectedLength = 80000;
    CHECK_SET_ERR(stdoutNodeText.size() > expectedLength,
                  QString("Node with ID '%1' text is not long enough: expected length is at least %2, actual text length is %3")
                      .arg(stdoutDataNodeId)
                      .arg(expectedLength)
                      .arg(stdoutNodeText.size()));

    QString stderrNodeText = GTUtilsDashboard::getNodeText(stderrDataNodeId);
    expectedLength = 80000;
    CHECK_SET_ERR(stderrNodeText.size() > expectedLength,
                  QString("Node with ID '%1' text is not long enough: expected length is at least %2, actual text length is %3")
                      .arg(stderrDataNodeId)
                      .arg(expectedLength)
                      .arg(stderrNodeText.size()));

    //        both "Output log (stdout)" and "Output log (stderr)" nodes contents ends with the following message: "The external tool output is too large and can't be visualized on the dashboard. Find full output in  %logUrl% this file", where %logUrl% is a link to log file.
    const QString expectedNodeTextPart = "The external tool output is too large and can't be visualized on the dashboard.";
    CHECK_SET_ERR(stdoutNodeText.contains(expectedNodeTextPart),
                  QString("Node with ID '%1' text doesn't ends with the expected part: expected part '%2', last 300 symbols of the text '%3'")
                      .arg(stderrDataNodeId)
                      .arg(expectedNodeTextPart)
                      .arg(stdoutNodeText.right(300)));

    CHECK_SET_ERR(stderrNodeText.contains(expectedNodeTextPart),
                  QString("Node with ID '%1' text doesn't ends with the expected part: expected part '%2', last 300 symbols of the text '%3'")
                      .arg(stderrDataNodeId)
                      .arg(expectedNodeTextPart)
                      .arg(stderrNodeText.right(300)));

    //        the links from the previous item are correct: they point to files with the same data as in nodes; the file sizes are not less than the appropriate nodes contents size; the appropriate files names contain "stdout" and "stderr".
    const QString stdoutLogUrl = GTUtilsDashboard::getLogUrlFromOutputContent(stdoutDataNodeId);

    const QFileInfo stdoutFileInfo(stdoutLogUrl);
    CHECK_SET_ERR(stdoutFileInfo.exists(), QString("File with URL '%1' doesn't exist").arg(stdoutLogUrl));
    CHECK_SET_ERR(stdoutFileInfo.size() >= stdoutNodeText.size(),
                  QString("File with URL '%1' size is less than node '%2' text")
                      .arg(stdoutLogUrl)
                      .arg(stdoutDataNodeId));

    QString expectedFileNamePart = "stdout";
    CHECK_SET_ERR(stdoutFileInfo.fileName().contains(expectedFileNamePart),
                  QString("File name '%1' doesn't contain '%2'")
                      .arg(stdoutLogUrl)
                      .arg(expectedFileNamePart));

    QString fileData = GTFile::readAll(stdoutLogUrl).replace("\n", "<br/>").replace("\r", "");
    CHECK_SET_ERR(fileData.startsWith(stdoutNodeText.left(500)),
                  QString("File '%1' content is not equal to the expected text: '%2', file: '%3'")
                      .arg(stdoutLogUrl)
                      .arg(stdoutNodeText.left(500))
                      .arg(fileData.left(500)));

    const QString stderrLogUrl = GTUtilsDashboard::getLogUrlFromOutputContent(stderrDataNodeId);
    const QFileInfo stderrFileInfo(stderrLogUrl);
    CHECK_SET_ERR(stderrFileInfo.exists(), QString("File with URL '%1' doesn't exist").arg(stderrLogUrl));
    CHECK_SET_ERR(stderrFileInfo.size() >= stderrNodeText.size(),
                  QString("File with URL '%1' size is less than node '%2' text")
                      .arg(stderrLogUrl)
                      .arg(stderrDataNodeId));

    expectedFileNamePart = "stderr";
    CHECK_SET_ERR(stderrFileInfo.fileName().contains(expectedFileNamePart),
                  QString("File name '%1' doesn't contain '%2'")
                      .arg(stderrLogUrl)
                      .arg(expectedFileNamePart));

    fileData = GTFile::readAll(stderrLogUrl).replace("\n", "<br/>").replace("\r", "");
    ;
    CHECK_SET_ERR(fileData.startsWith(stderrNodeText.left(500)),
                  QString("File '%1' content is not equal to the expected text: '%2', file: '%3'")
                      .arg(stderrLogUrl)
                      .arg(stderrNodeText.left(500))
                      .arg(fileData.left(500)));
}

GUI_TEST_CLASS_DEFINITION(view_opening_test_0001) {
    GTUtilsWorkflowDesigner::prepareDashboardsDirState("empty_workflow_output");

    //    2. Wait for scan task finish.
    GTUtilsTaskTreeView::waitTaskFinished();

    //    3. Open Workflow Designer.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    //    Expected result:
    //        - "Dashboards manager" button on the toolbar is active.
    //        - There is no "Go to Dashboard" button on the toolbar.
    QWidget* dashboardsManagerButton = GTToolbar::getWidgetForActionTooltip(GTToolbar::getToolbar(MWTOOLBAR_ACTIVEMDI), "Dashboards manager");
    CHECK_SET_ERR(dashboardsManagerButton != nullptr, "'Dashboards manager' is nullptr");
    CHECK_SET_ERR(dashboardsManagerButton->isEnabled(), "'Dashboards manager' button is unexpectedly disabled");

    auto viewSwitchButton = GTUtilsWorkflowDesigner::getGotoDashboardButton();
    CHECK_SET_ERR(!viewSwitchButton->isVisible(), "'Go to Dashboards' button is unexpectedly invisible");

    //    4. Click to the "Dashboards manager" button on the toolbar.
    //    5. Expected result: a messagebox appears. It contains the following text: "You do not have any dashboards yet. You need to run some workflow to use Dashboards Manager."
    //    6. Close the messagebox.
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller("OK", "You do not have any dashboards yet. You need to run some workflow to use Dashboards Manager."));
    GTWidget::click(dashboardsManagerButton);
}

GUI_TEST_CLASS_DEFINITION(view_opening_test_0002) {
    GTUtilsWorkflowDesigner::prepareDashboardsDirState("two_visible_dashboards");

    //    2. Wait for scan task finish.
    GTUtilsTaskTreeView::waitTaskFinished();

    //    3. Open Workflow Designer.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    //    Expected result:
    //        - "Dashboards manager" button on the toolbar is active.
    //        - There is "Go to Dashboard" button on the toolbar. The button text is exactly as written.
    QWidget* dashboardsManagerButton = GTToolbar::getWidgetForActionTooltip(GTToolbar::getToolbar(MWTOOLBAR_ACTIVEMDI), "Dashboards manager");
    CHECK_SET_ERR(dashboardsManagerButton != nullptr, "'Dashboards manager' is nullptr");
    CHECK_SET_ERR(dashboardsManagerButton->isEnabled(), "'Dashboards manager' button is unexpectedly disabled");

    auto viewSwitchButton = GTUtilsWorkflowDesigner::getGotoDashboardButton();
    CHECK_SET_ERR(viewSwitchButton->isVisible(), "View switch button is unexpectedly invisible");
    CHECK_SET_ERR(viewSwitchButton->isEnabled(), "View switch button is unexpectedly disabled");

    QString expectedButtonText = "Go to Dashboard";
    QString actualButtonText = viewSwitchButton->text();
    CHECK_SET_ERR(expectedButtonText == actualButtonText,
                  QString("View switch button has an unexpected text: expected '%1', got '%2'")
                      .arg(expectedButtonText)
                      .arg(actualButtonText));

    //    4. Click to the "Dashboards manager" button on the toolbar.
    //    Expected result: the "Dashboards Manager" dialog appears. It contains two items, both of them are checked. Their names are "Extract consensus as sequence 1" and "Extract consensus as sequence 2".
    //    5. Cancel the dialog.
    class Scenario : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            const QList<QPair<QString, bool>> expectedDashboardsState({qMakePair(QString("Extract consensus as sequence 1"), true),
                                                                       qMakePair(QString("Extract consensus as sequence 2"), true)});
            const QList<QPair<QString, bool>> actualDashboardsState = DashboardsManagerDialogFiller::getDashboardsState();

            CHECK_SET_ERR(expectedDashboardsState.size() == actualDashboardsState.size(),
                          QString("Expected dashboards count is not equal to the actual dashboards list size: expected %1, got %2")
                              .arg(expectedDashboardsState.size())
                              .arg(actualDashboardsState.size()));

            for (int i = 0; i < expectedDashboardsState.size(); ++i) {
                const QString expectedDashboardName = expectedDashboardsState[i].first;
                const QString actualDashboardName = actualDashboardsState[i].first;
                CHECK_SET_ERR(expectedDashboardName == actualDashboardName,
                              QString("Dashboard number %1 has an unexpected name: expected '%2', got '%3'")
                                  .arg(i)
                                  .arg(expectedDashboardName)
                                  .arg(actualDashboardName));

                const bool expectedDashboardState = expectedDashboardsState[i].second;
                const bool actualDashboardState = actualDashboardsState[i].second;
                CHECK_SET_ERR(expectedDashboardState == actualDashboardState,
                              QString("Dashboard number %1 has an unexpected state: it should be %2, but it is '%3'")
                                  .arg(i)
                                  .arg(expectedDashboardState ? "visible" : "invisible")
                                  .arg(actualDashboardState ? "visible" : "invisible"));
            }

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(new DashboardsManagerDialogFiller(new Scenario()));
    GTWidget::click(dashboardsManagerButton);

    //    6. Click to the "Go to Dashboards" button on the toolbar.
    GTWidget::click(viewSwitchButton);

    //    Expected result:
    //        - There is "To Workflow Designer" button on the toolbar. The button text is exactly as written.
    //        - There are two tabs with dashboards. Their names are "Extract consensus as sequence 1" and "Extract consensus as sequence 2".
    //        - The "Extract consensus as sequence 2" dashboard is active.
    //        - The dashboard is correctly displayed.
    CHECK_SET_ERR(viewSwitchButton->isVisible(), "View switch button is unexpectedly invisible");
    CHECK_SET_ERR(viewSwitchButton->isEnabled(), "View switch button is unexpectedly disabled");

    expectedButtonText = "To Workflow Designer";
    actualButtonText = viewSwitchButton->text();
    CHECK_SET_ERR(expectedButtonText == actualButtonText,
                  QString("View switch button has an unexpected text: expected '%1', got '%2'")
                      .arg(expectedButtonText)
                      .arg(actualButtonText));

    QTabWidget* dashboardsView = GTUtilsDashboard::getTabWidget();
    CHECK_SET_ERR(dashboardsView != nullptr, "Dashboards view is nullptr");

    int expectedTabsCount = 2;
    int actualTabsCount = dashboardsView->count();
    CHECK_SET_ERR(expectedTabsCount == actualTabsCount,
                  QString("There is an incorrect count of tabs in the Dashboard View: expected %1, got %2")
                      .arg(expectedTabsCount)
                      .arg(actualTabsCount));

    int expectedActiveTabIndex = 1;
    int actualActiveTabIndex = dashboardsView->currentIndex();
    CHECK_SET_ERR(expectedActiveTabIndex == actualActiveTabIndex,
                  QString("There is an incorrect active tab: expected index is %1, actual index is %2")
                      .arg(expectedActiveTabIndex)
                      .arg(actualActiveTabIndex));

    QString expectedTabName = "Extract consensus as sequence 2";
    QString actualTabName = GTUtilsDashboard::getDashboardName(expectedActiveTabIndex);
    CHECK_SET_ERR(expectedTabName == actualTabName,
                  QString("Active dashboard has an unexpected name: expect '%1', got '%2'")
                      .arg(expectedTabName)
                      .arg(actualTabName));

    QStringList outputFiles = GTUtilsDashboard::getOutputFiles();
    CHECK_SET_ERR(!outputFiles.isEmpty(), "Active dashboard is not displayed properly");

    //    7. Click to the "To Workflow Designer" button on the toolbar.
    GTWidget::click(viewSwitchButton);

    //    Expected result: there is "Go to Dashboard" button on the toolbar. The button text is exactly as written.
    CHECK_SET_ERR(viewSwitchButton->isVisible(), "View switch button is unexpectedly invisible");
    CHECK_SET_ERR(viewSwitchButton->isEnabled(), "View switch button is unexpectedly disabled");

    expectedButtonText = "Go to Dashboard";
    actualButtonText = viewSwitchButton->text();
    CHECK_SET_ERR(expectedButtonText == actualButtonText,
                  QString("View switch button has an unexpected text: expected '%1', got '%2'")
                      .arg(expectedButtonText)
                      .arg(actualButtonText));
}

GUI_TEST_CLASS_DEFINITION(view_opening_test_0003) {
    GTUtilsWorkflowDesigner::prepareDashboardsDirState("one_visible_one_invisible");

    //    3. Open Workflow Designer.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    //    Expected result:
    //        - "Dashboards manager" button on the toolbar is active.
    //        - There is "Go to Dashboard" button on the toolbar. The button text is exactly as written.
    QWidget* dashboardsManagerButton = GTToolbar::getWidgetForActionTooltip(GTToolbar::getToolbar(MWTOOLBAR_ACTIVEMDI), "Dashboards manager");
    CHECK_SET_ERR(dashboardsManagerButton != nullptr, "'Dashboards manager' is nullptr");
    CHECK_SET_ERR(dashboardsManagerButton->isEnabled(), "'Dashboards manager' button is unexpectedly disabled");

    auto viewSwitchButton = GTUtilsWorkflowDesigner::getGotoDashboardButton();
    CHECK_SET_ERR(viewSwitchButton->isVisible(), "View switch button is unexpectedly invisible");
    CHECK_SET_ERR(viewSwitchButton->isEnabled(), "View switch button is unexpectedly disabled");

    QString expectedButtonText = "Go to Dashboard";
    QString actualButtonText = viewSwitchButton->text();
    CHECK_SET_ERR(expectedButtonText == actualButtonText,
                  QString("View switch button has an unexpected text: expected '%1', got '%2'")
                      .arg(expectedButtonText)
                      .arg(actualButtonText));

    //    4. Click to the "Dashboards manager" button on the toolbar.
    //    Expected result: the "Dashboards Manager" dialog appears. It contains two items, the first one is unchecked, the second one is checked. Their names are "Extract consensus as sequence 1" and "Extract consensus as sequence 2".
    //    5. Cancel the dialog.
    class Scenario : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            const QList<QPair<QString, bool>> expectedDashboardsState({qMakePair(QString("Extract consensus as sequence 1"), false),
                                                                       qMakePair(QString("Extract consensus as sequence 2"), true)});
            const QList<QPair<QString, bool>> actualDashboardsState = DashboardsManagerDialogFiller::getDashboardsState();

            CHECK_SET_ERR(expectedDashboardsState.size() == actualDashboardsState.size(),
                          QString("Expected dashboards count is not equal to the actual dashboards list size: expected %1, got %2")
                              .arg(expectedDashboardsState.size())
                              .arg(actualDashboardsState.size()));

            for (int i = 0; i < expectedDashboardsState.size(); ++i) {
                const QString expectedDashboardName = expectedDashboardsState[i].first;
                const QString actualDashboardName = actualDashboardsState[i].first;
                CHECK_SET_ERR(expectedDashboardName == actualDashboardName,
                              QString("Dashboard number %1 has an unexpected name: expected '%2', got '%3'")
                                  .arg(i)
                                  .arg(expectedDashboardName)
                                  .arg(actualDashboardName));

                const bool expectedDashboardState = expectedDashboardsState[i].second;
                const bool actualDashboardState = actualDashboardsState[i].second;
                CHECK_SET_ERR(expectedDashboardState == actualDashboardState,
                              QString("Dashboard number %1 has an unexpected state: it should be %2, but it is '%3'")
                                  .arg(i)
                                  .arg(expectedDashboardState ? "visible" : "invisible")
                                  .arg(actualDashboardState ? "visible" : "invisible"));
            }

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(new DashboardsManagerDialogFiller(new Scenario()));
    GTWidget::click(dashboardsManagerButton);

    //    6. Click to the "Go to Dashboards" button on the toolbar.
    GTWidget::click(viewSwitchButton);

    //    Expected result:
    //        - There is "To Workflow Designer" button on the toolbar. The button text is exactly as written.
    //        - There is one tab with dashboard. Its name is "Extract consensus as sequence 2".
    //        - The "Extract consensus as sequence 2" dashboard is active.
    //        - The dashboard is correctly displayed.
    CHECK_SET_ERR(viewSwitchButton->isVisible(), "View switch button is unexpectedly invisible");
    CHECK_SET_ERR(viewSwitchButton->isEnabled(), "View switch button is unexpectedly disabled");

    expectedButtonText = "To Workflow Designer";
    actualButtonText = viewSwitchButton->text();
    CHECK_SET_ERR(expectedButtonText == actualButtonText,
                  QString("View switch button has an unexpected text: expected '%1', got '%2'")
                      .arg(expectedButtonText)
                      .arg(actualButtonText));

    QTabWidget* dashboardsView = GTUtilsDashboard::getTabWidget();
    CHECK_SET_ERR(dashboardsView != nullptr, "Dashboards view is nullptr");

    int expectedTabsCount = 1;
    int actualTabsCount = dashboardsView->count();
    CHECK_SET_ERR(expectedTabsCount == actualTabsCount,
                  QString("There is an incorrect count of tabs in the Dashboard View: expected %1, got %2")
                      .arg(expectedTabsCount)
                      .arg(actualTabsCount));

    int expectedActiveTabIndex = 0;
    int actualActiveTabIndex = dashboardsView->currentIndex();
    CHECK_SET_ERR(expectedActiveTabIndex == actualActiveTabIndex,
                  QString("There is an incorrect active tab: expected index is %1, actual index is %2")
                      .arg(expectedActiveTabIndex)
                      .arg(actualActiveTabIndex));

    QString expectedTabName = "Extract consensus as sequence 2";
    QString actualTabName = GTUtilsDashboard::getDashboardName(expectedActiveTabIndex);
    CHECK_SET_ERR(expectedTabName == actualTabName,
                  QString("Active dashboard has an unexpected name: expect '%1', got '%2'")
                      .arg(expectedTabName)
                      .arg(actualTabName));

    QStringList outputFiles = GTUtilsDashboard::getOutputFiles();
    CHECK_SET_ERR(!outputFiles.isEmpty(), "Active dashboard is not displayed properly");

    //    7. Click to the "To Workflow Designer" button on the toolbar.
    GTWidget::click(viewSwitchButton);

    //    Expected result: there is "Go to Dashboard" button on the toolbar. The button text is exactly as written.
    CHECK_SET_ERR(viewSwitchButton->isVisible(), "View switch button is unexpectedly invisible");
    CHECK_SET_ERR(viewSwitchButton->isEnabled(), "View switch button is unexpectedly disabled");

    expectedButtonText = "Go to Dashboard";
    actualButtonText = viewSwitchButton->text();
    CHECK_SET_ERR(expectedButtonText == actualButtonText,
                  QString("View switch button has an unexpected text: expected '%1', got '%2'")
                      .arg(expectedButtonText)
                      .arg(actualButtonText));
}

GUI_TEST_CLASS_DEFINITION(view_opening_test_0004) {
    GTUtilsWorkflowDesigner::prepareDashboardsDirState("two_invisible_dashboards");

    //    3. Open Workflow Designer.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    //    Expected result:
    //        - "Dashboards manager" button on the toolbar is active.
    //        - There is no "Go to Dashboard" button on the toolbar.
    QWidget* dashboardsManagerButton = GTToolbar::getWidgetForActionTooltip(GTToolbar::getToolbar(MWTOOLBAR_ACTIVEMDI), "Dashboards manager");
    CHECK_SET_ERR(dashboardsManagerButton != nullptr, "'Dashboards manager' is nullptr");
    CHECK_SET_ERR(dashboardsManagerButton->isEnabled(), "'Dashboards manager' button is unexpectedly disabled");

    auto viewSwitchButton = GTUtilsWorkflowDesigner::getGotoDashboardButton();
    CHECK_SET_ERR(!viewSwitchButton->isVisible(), "'Go to Dashboards' button is unexpectedly invisible");

    //    4. Click to the "Dashboards manager" button on the toolbar.
    //    Expected result: the "Dashboards Manager" dialog appears. It contains two items, both of them are unchecked. Their names are "Extract consensus as sequence 1" and "Extract consensus as sequence 2".
    //    5. Cancel the dialog.
    class Scenario : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            const QList<QPair<QString, bool>> expectedDashboardsState({qMakePair(QString("Extract consensus as sequence 1"), false),
                                                                       qMakePair(QString("Extract consensus as sequence 2"), false)});
            const QList<QPair<QString, bool>> actualDashboardsState = DashboardsManagerDialogFiller::getDashboardsState();

            CHECK_SET_ERR(expectedDashboardsState.size() == actualDashboardsState.size(),
                          QString("Expected dashboards count is not equal to the actual dashboards list size: expected %1, got %2")
                              .arg(expectedDashboardsState.size())
                              .arg(actualDashboardsState.size()));

            for (int i = 0; i < expectedDashboardsState.size(); ++i) {
                const QString expectedDashboardName = expectedDashboardsState[i].first;
                const QString actualDashboardName = actualDashboardsState[i].first;
                CHECK_SET_ERR(expectedDashboardName == actualDashboardName,
                              QString("Dashboard number %1 has an unexpected name: expected '%2', got '%3'")
                                  .arg(i)
                                  .arg(expectedDashboardName)
                                  .arg(actualDashboardName));

                const bool expectedDashboardState = expectedDashboardsState[i].second;
                const bool actualDashboardState = actualDashboardsState[i].second;
                CHECK_SET_ERR(expectedDashboardState == actualDashboardState,
                              QString("Dashboard number %1 has an unexpected state: it should be %2, but it is '%3'")
                                  .arg(i)
                                  .arg(expectedDashboardState ? "visible" : "invisible")
                                  .arg(actualDashboardState ? "visible" : "invisible"));
            }

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(new DashboardsManagerDialogFiller(new Scenario()));
    GTWidget::click(dashboardsManagerButton);
}

GUI_TEST_CLASS_DEFINITION(output_dir_scanning_test_0001) {
    GTUtilsWorkflowDesigner::prepareDashboardsDirState("two_visible_two_invisible");
    //    3. Open Workflow Designer.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    int tabIndex1 = GTUtilsMdi::getCurrentTab();

    //    4. Open Workflow Designer.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    //    5. Click to the "Go to Dashboards" button on the toolbar.
    auto viewSwitchButton = GTUtilsWorkflowDesigner::getGotoDashboardButton();
    CHECK_SET_ERR(viewSwitchButton->isVisible(), "View switch button is unexpectedly invisible");
    CHECK_SET_ERR(viewSwitchButton->isEnabled(), "View switch button is unexpectedly disabled");
    GTWidget::click(viewSwitchButton);

    //    6. Set "_common_data/workflow/dashboard/workflow_outputs/empty_workflow_output" as workflow output folder in the "Application Settings".
    GTUtilsWorkflowDesigner::prepareDashboardsDirState("empty_workflow_output");

    //    Expected result:
    //        - The Workflow Designer switches to the scene view mode.
    //        - There is no scene/dashboards switch button on the toolbar.
    //        - The "Dashboards manager" button on the toolbar is active.
    QWidget* dashboardsManagerButton = GTToolbar::getWidgetForActionTooltip(GTToolbar::getToolbar(MWTOOLBAR_ACTIVEMDI), "Dashboards manager");
    CHECK_SET_ERR(dashboardsManagerButton != nullptr, "'Dashboards manager' is nullptr");
    CHECK_SET_ERR(dashboardsManagerButton->isEnabled(), "'Dashboards manager' button is unexpectedly disabled");

    auto viewSwitchButton2 = GTUtilsWorkflowDesigner::getGotoDashboardButton();
    CHECK_SET_ERR(!viewSwitchButton2->isVisible(), "'Go to Dashboards' is visible");

    //    8. Click to the "Dashboards manager" button on the toolbar.
    //    Expected result: the "Dashboards Manager" dialog appears. It contains the following text: "You do not have any dashboards yet. You need to run some workflow to use Dashboards Manager."
    //    9. Close the messagebox.
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller("OK", "You do not have any dashboards yet. You need to run some workflow to use Dashboards Manager."));
    GTWidget::click(dashboardsManagerButton);

    //    10. Switch to another Workflow Designer.
    coreLog.info(QString("Try to switch to tab %1(%2)")
                     .arg(tabIndex1)
                     .arg(GTUtilsMdi::getTabBar()->tabText(tabIndex1)));
    GTUtilsMdi::clickTab(tabIndex1);

    //    Expected result:
    //      - The Workflow Designer is in the scene view mode.
    //      - There is no scene/dashboards switch button on the toolbar.
    //      - The "Dashboards manager" button on the toolbar is active.
    dashboardsManagerButton = GTToolbar::getWidgetForActionTooltip(GTToolbar::getToolbar(MWTOOLBAR_ACTIVEMDI), "Dashboards manager");
    CHECK_SET_ERR(dashboardsManagerButton != nullptr, "'Dashboards manager' is nullptr");
    CHECK_SET_ERR(dashboardsManagerButton->isEnabled(), "'Dashboards manager' button is unexpectedly disabled");

    viewSwitchButton2 = GTUtilsWorkflowDesigner::getGotoDashboardButton();
    CHECK_SET_ERR(!viewSwitchButton2->isVisible(), "'Go to Dashboards' is visible");

    //    11. Click to the "Dashboards manager" button on the toolbar.
    //    Expected result: a messagebox appears. It contains the following text: "You do not have any dashboards yet. You need to run some workflow to use Dashboards Manager."
    //    12. Close the messagebox.
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller("OK", "You do not have any dashboards yet. You need to run some workflow to use Dashboards Manager."));
    GTWidget::click(dashboardsManagerButton);
}

GUI_TEST_CLASS_DEFINITION(output_dir_scanning_test_0002) {
    GTUtilsWorkflowDesigner::prepareDashboardsDirState("two_visible_two_invisible");

    //    3. Open Workflow Designer.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    int tabIndex1 = GTUtilsMdi::getCurrentTab();

    //    4. Open Workflow Designer.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    //    5. Click to the "Go to Dashboards" button on the toolbar.
    auto viewSwitchButton = GTUtilsWorkflowDesigner::getGotoDashboardButton();
    CHECK_SET_ERR(viewSwitchButton->isVisible(), "View switch button is unexpectedly invisible");
    CHECK_SET_ERR(viewSwitchButton->isEnabled(), "View switch button is unexpectedly disabled");
    GTWidget::click(viewSwitchButton);

    //    6. Set "_common_data/workflow/dashboard/workflow_outputs/two_visible_dashboards" as workflow output folder in the "Application Settings".
    GTUtilsWorkflowDesigner::prepareDashboardsDirState("two_visible_dashboards");

    //    Expected result:
    //    - The Workflow Designer is in the dashboards view mode.
    //    - There are two tabs with dashboards. Their names are "Extract consensus as sequence 1" and "Extract consensus as sequence 2".
    //    - The "Extract consensus as sequence 2" dashboard is active.
    //    - The dashboard is correctly displayed.
    //    - The "Dashboards manager" button on the toolbar is active.
    CHECK_SET_ERR(viewSwitchButton->isVisible(), "View switch button is unexpectedly invisible");
    CHECK_SET_ERR(viewSwitchButton->isEnabled(), "View switch button is unexpectedly disabled");

    QString expectedButtonText = "To Workflow Designer";
    QString actualButtonText = viewSwitchButton->text();
    CHECK_SET_ERR(expectedButtonText == actualButtonText,
                  QString("View switch button has an unexpected text: expected '%1', got '%2'")
                      .arg(expectedButtonText)
                      .arg(actualButtonText));

    coreLog.info("Trying get GTUtilsDashboard::getTabWidget()");
    QTabWidget* dashboardsView = GTUtilsDashboard::getTabWidget();
    CHECK_SET_ERR(dashboardsView != nullptr, "Dashboards view is nullptr");
    coreLog.info("Successfully got GTUtilsDashboard::getTabWidget()");

    const int expectedTabsCount = 2;
    const int actualTabsCount = dashboardsView->count();
    CHECK_SET_ERR(expectedTabsCount == actualTabsCount,
                  QString("There is an incorrect count of tabs in the Dashboard View: expected %1, got %2")
                      .arg(expectedTabsCount)
                      .arg(actualTabsCount));

    const int expectedActiveTabIndex = 1;
    const int actualActiveTabIndex = dashboardsView->currentIndex();
    CHECK_SET_ERR(expectedActiveTabIndex == actualActiveTabIndex,
                  QString("There is an incorrect active tab: expected index is %1, actual index is %2")
                      .arg(expectedActiveTabIndex)
                      .arg(actualActiveTabIndex));

    const QString expectedTabName = "Extract consensus as sequence 2";
    const QString actualTabName = GTUtilsDashboard::getDashboardName(expectedActiveTabIndex);
    CHECK_SET_ERR(expectedTabName == actualTabName,
                  QString("Active dashboard has an unexpected name: expect '%1', got '%2'")
                      .arg(expectedTabName)
                      .arg(actualTabName));

    const QStringList outputFiles = GTUtilsDashboard::getOutputFiles();
    CHECK_SET_ERR(!outputFiles.isEmpty(), "Active dashboard is not displayed properly");

    QWidget* dashboardsManagerButton = GTToolbar::getWidgetForActionTooltip(GTToolbar::getToolbar(MWTOOLBAR_ACTIVEMDI), "Dashboards manager");
    CHECK_SET_ERR(dashboardsManagerButton != nullptr, "'Dashboards manager' is nullptr");
    CHECK_SET_ERR(dashboardsManagerButton->isEnabled(), "'Dashboards manager' button is unexpectedly disabled");

    //    8. Click to the "Dashboards manager" button on the toolbar.
    //    Expected result: the "Dashboards Manager" dialog appears. It contains two items, both of them are checked. Their names are "Extract consensus as sequence 1" and "Extract consensus as sequence 2".
    //    9. Close the messagebox.
    class Scenario : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            const QList<QPair<QString, bool>> expectedDashboardsState({qMakePair(QString("Extract consensus as sequence 1"), true),
                                                                       qMakePair(QString("Extract consensus as sequence 2"), true)});
            const QList<QPair<QString, bool>> actualDashboardsState = DashboardsManagerDialogFiller::getDashboardsState();

            CHECK_SET_ERR(expectedDashboardsState.size() == actualDashboardsState.size(),
                          QString("Expected dashboards count is not equal to the actual dashboards list size: expected %1, got %2")
                              .arg(expectedDashboardsState.size())
                              .arg(actualDashboardsState.size()));

            for (int i = 0; i < expectedDashboardsState.size(); ++i) {
                const QString expectedDashboardName = expectedDashboardsState[i].first;
                const QString actualDashboardName = actualDashboardsState[i].first;
                CHECK_SET_ERR(expectedDashboardName == actualDashboardName,
                              QString("Dashboard number %1 has an unexpected name: expected '%2', got '%3'")
                                  .arg(i)
                                  .arg(expectedDashboardName)
                                  .arg(actualDashboardName));

                const bool expectedDashboardState = expectedDashboardsState[i].second;
                const bool actualDashboardState = actualDashboardsState[i].second;
                CHECK_SET_ERR(expectedDashboardState == actualDashboardState,
                              QString("Dashboard number %1 has an unexpected state: it should be %2, but it is '%3'")
                                  .arg(i)
                                  .arg(expectedDashboardState ? "visible" : "invisible")
                                  .arg(actualDashboardState ? "visible" : "invisible"));
            }

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(new DashboardsManagerDialogFiller(new Scenario()));
    GTWidget::click(dashboardsManagerButton);

    //    10. Switch to another Workflow Designer.
    coreLog.info(QString("Try to switch to tab %1(%2)")
                     .arg(tabIndex1)
                     .arg(GTUtilsMdi::getTabBar()->tabText(tabIndex1)));
    GTUtilsMdi::clickTab(tabIndex1);

    //    Expected result:
    //      - The Workflow Designer is in the scene view mode.
    //      - The "Dashboards manager" button on the toolbar is active.
    dashboardsManagerButton = GTToolbar::getWidgetForActionTooltip(GTToolbar::getToolbar(MWTOOLBAR_ACTIVEMDI), "Dashboards manager");
    CHECK_SET_ERR(dashboardsManagerButton != nullptr, "'Dashboards manager' is nullptr");
    CHECK_SET_ERR(dashboardsManagerButton->isEnabled(), "'Dashboards manager' button is unexpectedly disabled");

    auto viewSwitchButton2 = GTUtilsWorkflowDesigner::getGotoDashboardButton();
    CHECK_SET_ERR(viewSwitchButton2->isVisible(), "'Go to Dashboards' is invisible");

    //    11. Click to the "Dashboards manager" button on the toolbar.
    //    12. Cancel the dialog
    //    Expected result:
    //    - There are two tabs with dashboards. Their names are "Extract consensus as sequence 1" and "Extract consensus as sequence 2".
    //    - The "Extract consensus as sequence 2" dashboard is active.
    //    - The dashboard is correctly displayed.
    GTUtilsDialog::waitForDialog(new DashboardsManagerDialogFiller(new Scenario()));
    GTWidget::click(dashboardsManagerButton);

    //    13. Click to the "Go to Dashboards" button on the toolbar.
    //    Expected result:
    //    - There are two tabs with dashboards. Their names are "Extract consensus as sequence 1" and "Extract consensus as sequence 2".
    //    - The "Extract consensus as sequence 2" dashboard is active.
    //    - The dashboard is correctly displayed.
    viewSwitchButton = GTUtilsWorkflowDesigner::getGotoDashboardButton();
    CHECK_SET_ERR(viewSwitchButton->isVisible(), "View switch button is unexpectedly invisible");
    CHECK_SET_ERR(viewSwitchButton->isEnabled(), "View switch button is unexpectedly disabled");
    GTWidget::click(viewSwitchButton);

    dashboardsView = GTUtilsDashboard::getTabWidget();
    CHECK_SET_ERR(dashboardsView != nullptr, "Dashboards view is nullptr");

    const int expectedTabsCount2 = 2;
    const int actualTabsCount2 = dashboardsView->count();
    CHECK_SET_ERR(expectedTabsCount2 == actualTabsCount2,
                  QString("There is an incorrect count of tabs in the Dashboard View: expected %1, got %2")
                      .arg(expectedTabsCount2)
                      .arg(actualTabsCount2));

    const int expectedActiveTabIndex2 = 1;
    const int actualActiveTabIndex2 = dashboardsView->currentIndex();
    CHECK_SET_ERR(expectedActiveTabIndex2 == actualActiveTabIndex2,
                  QString("There is an incorrect active tab: expected index is %1, actual index is %2")
                      .arg(expectedActiveTabIndex2)
                      .arg(actualActiveTabIndex2));

    const QString expectedTabName2 = "Extract consensus as sequence 2";
    const QString actualTabName2 = GTUtilsDashboard::getDashboardName(expectedActiveTabIndex2);
    CHECK_SET_ERR(expectedTabName2 == actualTabName2,
                  QString("Active dashboard has an unexpected name: expect '%1', got '%2'")
                      .arg(expectedTabName2)
                      .arg(actualTabName2));

    const QStringList outputFiles2 = GTUtilsDashboard::getOutputFiles();
    CHECK_SET_ERR(!outputFiles2.isEmpty(), "Active dashboard is not displayed properly");
}

GUI_TEST_CLASS_DEFINITION(output_dir_scanning_test_0003) {
    //    1. Set "_common_data/workflow/dashboard/workflow_outputs/two_visible_two_invisible" as workflow output folder in the "Application Settings".
    GTUtilsWorkflowDesigner::prepareDashboardsDirState("two_visible_two_invisible");

    //    3. Open Workflow Designer.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    int tabIndex1 = GTUtilsMdi::getCurrentTab();
    GTUtilsTaskTreeView::waitTaskFinished();

    //    4. Open Workflow Designer.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsTaskTreeView::waitTaskFinished();

    //    5. Click to the "Go to Dashboards" button on the toolbar.
    auto viewSwitchButton = GTUtilsWorkflowDesigner::getGotoDashboardButton();
    CHECK_SET_ERR(viewSwitchButton->isVisible(), "View switch button is unexpectedly invisible");
    CHECK_SET_ERR(viewSwitchButton->isEnabled(), "View switch button is unexpectedly disabled");
    GTWidget::click(viewSwitchButton);

    //    6. Set "_common_data/workflow/dashboard/workflow_outputs/one_visible_one_invisible" as workflow output folder in the "Application Settings".
    GTUtilsWorkflowDesigner::prepareDashboardsDirState("one_visible_one_invisible");

    //    Expected result:
    //    - The Workflow Designer is in the dashboards view mode.
    //    - There is one tab with a dashboard. Its name is "Extract consensus as sequence 2".
    //    - The "Extract consensus as sequence 2" dashboard is active.
    //    - The dashboard is correctly displayed.
    //    - The "Dashboards manager" button on the toolbar is active.
    CHECK_SET_ERR(viewSwitchButton->isVisible(), "View switch button is unexpectedly invisible");
    CHECK_SET_ERR(viewSwitchButton->isEnabled(), "View switch button is unexpectedly disabled");

    QString expectedButtonText = "To Workflow Designer";
    QString actualButtonText = viewSwitchButton->text();
    CHECK_SET_ERR(expectedButtonText == actualButtonText,
                  QString("View switch button has an unexpected text: expected '%1', got '%2'")
                      .arg(expectedButtonText)
                      .arg(actualButtonText));

    QTabWidget* dashboardsView = GTUtilsDashboard::getTabWidget();
    CHECK_SET_ERR(dashboardsView != nullptr, "Dashboards view is nullptr");

    const int expectedTabsCount = 1;
    const int actualTabsCount = dashboardsView->count();
    CHECK_SET_ERR(expectedTabsCount == actualTabsCount,
                  QString("There is an incorrect count of tabs in the Dashboard View: expected %1, got %2")
                      .arg(expectedTabsCount)
                      .arg(actualTabsCount));

    const int expectedActiveTabIndex = 0;
    const int actualActiveTabIndex = dashboardsView->currentIndex();
    CHECK_SET_ERR(expectedActiveTabIndex == actualActiveTabIndex,
                  QString("There is an incorrect active tab: expected index is %1, actual index is %2")
                      .arg(expectedActiveTabIndex)
                      .arg(actualActiveTabIndex));

    const QString expectedTabName = "Extract consensus as sequence 2";
    const QString actualTabName = GTUtilsDashboard::getDashboardName(expectedActiveTabIndex);
    CHECK_SET_ERR(expectedTabName == actualTabName,
                  QString("Active dashboard has an unexpected name: expect '%1', got '%2'")
                      .arg(expectedTabName)
                      .arg(actualTabName));

    const QStringList outputFiles = GTUtilsDashboard::getOutputFiles();
    CHECK_SET_ERR(!outputFiles.isEmpty(), "Active dashboard is not displayed properly");

    QWidget* dashboardsManagerButton = GTToolbar::getWidgetForActionTooltip(GTToolbar::getToolbar(MWTOOLBAR_ACTIVEMDI), "Dashboards manager");
    CHECK_SET_ERR(dashboardsManagerButton != nullptr, "'Dashboards manager' is nullptr");
    CHECK_SET_ERR(dashboardsManagerButton->isEnabled(), "'Dashboards manager' button is unexpectedly disabled");

    //    8. Click to the "Dashboards manager" button on the toolbar.
    //    Expected result: the "Dashboards Manager" dialog appears. It contains two items, both of them are checked. Their names are "Extract consensus as sequence 1" and "Extract consensus as sequence 2".
    //    9. Close the messagebox.
    class Scenario : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            const QList<QPair<QString, bool>> expectedDashboardsState({qMakePair(QString("Extract consensus as sequence 1"), false),
                                                                       qMakePair(QString("Extract consensus as sequence 2"), true)});
            const QList<QPair<QString, bool>> actualDashboardsState = DashboardsManagerDialogFiller::getDashboardsState();

            CHECK_SET_ERR(expectedDashboardsState.size() == actualDashboardsState.size(),
                          QString("Expected dashboards count is not equal to the actual dashboards list size: expected %1, got %2")
                              .arg(expectedDashboardsState.size())
                              .arg(actualDashboardsState.size()));

            for (int i = 0; i < expectedDashboardsState.size(); ++i) {
                const QString expectedDashboardName = expectedDashboardsState[i].first;
                const QString actualDashboardName = actualDashboardsState[i].first;
                CHECK_SET_ERR(expectedDashboardName == actualDashboardName,
                              QString("Dashboard number %1 has an unexpected name: expected '%2', got '%3'")
                                  .arg(i)
                                  .arg(expectedDashboardName)
                                  .arg(actualDashboardName));

                const bool expectedDashboardState = expectedDashboardsState[i].second;
                const bool actualDashboardState = actualDashboardsState[i].second;
                CHECK_SET_ERR(expectedDashboardState == actualDashboardState,
                              QString("Dashboard number %1 has an unexpected state: it should be %2, but it is '%3'")
                                  .arg(i)
                                  .arg(expectedDashboardState ? "visible" : "invisible")
                                  .arg(actualDashboardState ? "visible" : "invisible"));
            }

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(new DashboardsManagerDialogFiller(new Scenario()));
    GTWidget::click(dashboardsManagerButton);

    //    10. Switch to another Workflow Designer.
    coreLog.info(QString("Try to switch to tab %1(%2)")
                     .arg(tabIndex1)
                     .arg(GTUtilsMdi::getTabBar()->tabText(tabIndex1)));
    GTUtilsMdi::clickTab(tabIndex1);

    //    Expected result:
    //      - The Workflow Designer is in the scene view mode.
    //      - The "Dashboards manager" button on the toolbar is active.
    dashboardsManagerButton = GTToolbar::getWidgetForActionTooltip(GTToolbar::getToolbar(MWTOOLBAR_ACTIVEMDI), "Dashboards manager");
    CHECK_SET_ERR(dashboardsManagerButton != nullptr, "'Dashboards manager' is nullptr");
    CHECK_SET_ERR(dashboardsManagerButton->isEnabled(), "'Dashboards manager' button is unexpectedly disabled");

    //    11. Click to the "Dashboards manager" button on the toolbar.
    //    12. Cancel the dialog
    //    Expected result:
    //    - There are two tabs with dashboards. Their names are "Extract consensus as sequence 1" and "Extract consensus as sequence 2".
    //    - The "Extract consensus as sequence 2" dashboard is active.
    //    - The dashboard is correctly displayed.
    GTUtilsDialog::waitForDialog(new DashboardsManagerDialogFiller(new Scenario()));
    GTWidget::click(dashboardsManagerButton);

    //    13. Click to the "Go to Dashboards" button on the toolbar.
    //    Expected result:
    //    - There is one tab with a dashboard. Its name is "Extract consensus as sequence 2".
    //    - The "Extract consensus as sequence 2" dashboard is active.
    //    - The dashboard is correctly displayed.
    viewSwitchButton = GTUtilsWorkflowDesigner::getGotoDashboardButton();
    CHECK_SET_ERR(viewSwitchButton->isVisible(), "View switch button is unexpectedly invisible");
    CHECK_SET_ERR(viewSwitchButton->isEnabled(), "View switch button is unexpectedly disabled");
    GTWidget::click(viewSwitchButton);

    dashboardsView = GTUtilsDashboard::getTabWidget();
    CHECK_SET_ERR(dashboardsView != nullptr, "Dashboards view is nullptr");

    const int expectedTabsCount2 = 1;
    const int actualTabsCount2 = dashboardsView->count();
    CHECK_SET_ERR(expectedTabsCount2 == actualTabsCount2,
                  QString("There is an incorrect count of tabs in the Dashboard View: expected %1, got %2")
                      .arg(expectedTabsCount2)
                      .arg(actualTabsCount2));

    const int expectedActiveTabIndex2 = 0;
    const int actualActiveTabIndex2 = dashboardsView->currentIndex();
    CHECK_SET_ERR(expectedActiveTabIndex2 == actualActiveTabIndex2,
                  QString("There is an incorrect active tab: expected index is %1, actual index is %2")
                      .arg(expectedActiveTabIndex2)
                      .arg(actualActiveTabIndex2));

    const QString expectedTabName2 = "Extract consensus as sequence 2";
    const QString actualTabName2 = GTUtilsDashboard::getDashboardName(expectedActiveTabIndex2);
    CHECK_SET_ERR(expectedTabName2 == actualTabName2,
                  QString("Active dashboard has an unexpected name: expect '%1', got '%2'")
                      .arg(expectedTabName2)
                      .arg(actualTabName2));

    const QStringList outputFiles2 = GTUtilsDashboard::getOutputFiles();
    CHECK_SET_ERR(!outputFiles2.isEmpty(), "Active dashboard is not displayed properly");
}

GUI_TEST_CLASS_DEFINITION(output_dir_scanning_test_0004) {
    //    1. Set "_common_data/workflow/dashboard/workflow_outputs/two_visible_two_invisible" as workflow output folder in the "Application Settings".
    GTUtilsWorkflowDesigner::prepareDashboardsDirState("two_visible_two_invisible");

    //    3. Open Workflow Designer.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    int tabIndex1 = GTUtilsMdi::getCurrentTab();
    GTUtilsTaskTreeView::waitTaskFinished();

    //    4. Open Workflow Designer.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsTaskTreeView::waitTaskFinished();

    //    5. Click to the "Go to Dashboards" button on the toolbar.
    auto viewSwitchButton = GTUtilsWorkflowDesigner::getGotoDashboardButton();
    CHECK_SET_ERR(viewSwitchButton->isVisible(), "View switch button is unexpectedly invisible");
    CHECK_SET_ERR(viewSwitchButton->isEnabled(), "View switch button is unexpectedly disabled");
    GTWidget::click(viewSwitchButton);

    GTUtilsWorkflowDesigner::prepareDashboardsDirState("two_invisible_dashboards");

    //    Expected result:
    //    - The Workflow Designer is in the scene view mode.
    //    - There is no scene/dashboards switch button on the toolbar.
    //    - The "Dashboards manager" button on the toolbar is active.
    viewSwitchButton = GTUtilsWorkflowDesigner::getGotoDashboardButton();
    CHECK_SET_ERR(viewSwitchButton != nullptr, "'Go to Dashboards' is nullptr");
    CHECK_SET_ERR(!viewSwitchButton->isVisible(), "View switch button is unexpectedly visible");

    QTabWidget* dashboardsView = GTUtilsDashboard::getTabWidget();
    CHECK_SET_ERR(dashboardsView != nullptr, "Dashboards view is nullptr");

    QWidget* dashboardsManagerButton = GTToolbar::getWidgetForActionTooltip(GTToolbar::getToolbar(MWTOOLBAR_ACTIVEMDI), "Dashboards manager");
    CHECK_SET_ERR(dashboardsManagerButton != nullptr, "'Dashboards manager' is nullptr");
    CHECK_SET_ERR(dashboardsManagerButton->isEnabled(), "'Dashboards manager' button is unexpectedly disabled");

    //    8. Click to the "Dashboards manager" button on the toolbar.
    //    Expected result: the "Dashboards Manager" dialog appears. It contains two items, both of them are unchecked. Their names are "Extract consensus as sequence 1" and "Extract consensus as sequence 2".
    //    9. Close the messagebox.
    class Scenario : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            const QList<QPair<QString, bool>> expectedDashboardsState({qMakePair(QString("Extract consensus as sequence 1"), false),
                                                                       qMakePair(QString("Extract consensus as sequence 2"), false)});
            const QList<QPair<QString, bool>> actualDashboardsState = DashboardsManagerDialogFiller::getDashboardsState();

            CHECK_SET_ERR(expectedDashboardsState.size() == actualDashboardsState.size(),
                          QString("Expected dashboards count is not equal to the actual dashboards list size: expected %1, got %2")
                              .arg(expectedDashboardsState.size())
                              .arg(actualDashboardsState.size()));

            for (int i = 0; i < expectedDashboardsState.size(); ++i) {
                const QString expectedDashboardName = expectedDashboardsState[i].first;
                const QString actualDashboardName = actualDashboardsState[i].first;
                CHECK_SET_ERR(expectedDashboardName == actualDashboardName,
                              QString("Dashboard number %1 has an unexpected name: expected '%2', got '%3'")
                                  .arg(i)
                                  .arg(expectedDashboardName)
                                  .arg(actualDashboardName));

                const bool expectedDashboardState = expectedDashboardsState[i].second;
                const bool actualDashboardState = actualDashboardsState[i].second;
                CHECK_SET_ERR(expectedDashboardState == actualDashboardState,
                              QString("Dashboard number %1 has an unexpected state: it should be %2, but it is '%3'")
                                  .arg(i)
                                  .arg(expectedDashboardState ? "visible" : "invisible")
                                  .arg(actualDashboardState ? "visible" : "invisible"));
            }

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(new DashboardsManagerDialogFiller(new Scenario()));
    GTWidget::click(dashboardsManagerButton);

    //    10. Switch to another Workflow Designer.
    coreLog.info(QString("Try to switch to tab %1(%2)")
                     .arg(tabIndex1)
                     .arg(GTUtilsMdi::getTabBar()->tabText(tabIndex1)));
    GTUtilsMdi::clickTab(tabIndex1);

    //    Expected result:
    //      - The Workflow Designer is in the scene view mode.
    //      - There is no scene/dashboards switch button on the toolbar.
    //      - The "Dashboards manager" button on the toolbar is active.
    viewSwitchButton = GTUtilsWorkflowDesigner::getGotoDashboardButton();
    CHECK_SET_ERR(viewSwitchButton == nullptr || !viewSwitchButton->isVisible(), "View switch button is unexpectedly visible");

    QTabWidget* dashboardsView2 = GTUtilsDashboard::getTabWidget();
    CHECK_SET_ERR(dashboardsView2 != nullptr, "Dashboards view is nullptr");

    dashboardsManagerButton = GTToolbar::getWidgetForActionTooltip(GTToolbar::getToolbar(MWTOOLBAR_ACTIVEMDI), "Dashboards manager");
    CHECK_SET_ERR(dashboardsManagerButton != nullptr, "'Dashboards manager' is nullptr");
    CHECK_SET_ERR(dashboardsManagerButton->isEnabled(), "'Dashboards manager' button is unexpectedly disabled");

    //    11. Click to the "Dashboards manager" button on the toolbar.
    //    12. Cancel the dialog
    //    Expected result:
    //    Expected result: the "Dashboards Manager" dialog appears. It contains two items, both of them are unchecked. Their names are "Extract consensus as sequence 1" and "Extract consensus as sequence 2".
    GTUtilsDialog::waitForDialog(new DashboardsManagerDialogFiller(new Scenario()));
    GTWidget::click(dashboardsManagerButton);
}

GUI_TEST_CLASS_DEFINITION(output_dir_scanning_test_0005_1) {
    GTUtilsWorkflowDesigner::prepareDashboardsDirState("two_visible_two_invisible");

    //    3. Open Workflow Designer.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsTaskTreeView::waitTaskFinished();
    int tabIndex1 = GTUtilsMdi::getCurrentTab();

    //    4. Open "Align sequence with MUSCLE" sample.
    GTUtilsWorkflowDesigner::addSample("Align sequences with MUSCLE", GTUtilsMdi::activeWindow());

    //    5. Cancel the wizard.
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    //    6. Click to the "Read alignment" element.
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Read alignment"));
    GTMouseDriver::click();

    //    7. Add "_common_data/clustal/200_sequences.aln" file to "Dataset 1" dataset.
    GTUtilsWorkflowDesigner::setDatasetInputFile(testDir + "_common_data/clustal/200_sequences.aln");

    //    8. Open Workflow Designer.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsTaskTreeView::waitTaskFinished();
    int tabIndex2 = GTUtilsMdi::getCurrentTab();

    //    9. Open "Align sequence with MUSCLE" sample.
    GTUtilsWorkflowDesigner::addSample("Align sequences with MUSCLE", GTUtilsMdi::activeWindow());

    //    10. Cancel the wizard.
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    //    11. Click to the "Read alignment" element.
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Read alignment"));
    GTMouseDriver::click();

    //    12. Add "_common_data/clustal/200_sequences.aln" file to "Dataset 1" dataset.
    GTUtilsWorkflowDesigner::setDatasetInputFile(testDir + "_common_data/clustal/200_sequences.aln");

    //    13. Open Workflow Designer.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsTaskTreeView::waitTaskFinished();

    //    14. Click to the "Go to Dashboards" button on the toolbar.
    auto viewSwitchButton = GTUtilsWorkflowDesigner::getGotoDashboardButton();
    CHECK_SET_ERR(viewSwitchButton->isVisible(), "View switch button is unexpectedly invisible");
    CHECK_SET_ERR(viewSwitchButton->isEnabled(), "View switch button is unexpectedly disabled");
    GTWidget::click(viewSwitchButton);

    //    15. Switch to first Workflow Designer.
    coreLog.info(QString("Try to switch to tab %1(%2)")
                     .arg(tabIndex1)
                     .arg(GTUtilsMdi::getTabBar()->tabText(tabIndex1)));
    GTUtilsMdi::clickTab(tabIndex1);

    //    16. Launch the workflow. Do not wait for the task finish.
    coreLog.info("Try to start workflow #1");
    GTUtilsWorkflowDesigner::runWorkflow();
    coreLog.info("It seems that workflow was started");

    //    17. Switch to the second Workflow Designer.
    coreLog.info(QString("Try to switch to tab %1(%2)")
                     .arg(tabIndex2)
                     .arg(GTUtilsMdi::getTabBar()->tabText(tabIndex2)));
    GTUtilsMdi::clickTab(tabIndex2);

    //    18. Launch the workflow. Do not wait for the task finish.
    coreLog.info("Try to start workflow #2");
    GTUtilsWorkflowDesigner::runWorkflow();
    coreLog.info("It seems that workflow was started");

    // Cancel a long-running task before exiting from the test.
    GTUtilsTask::cancelAllTasks();
}

static int setUpMuscleSchemeInNewWdWindow(const QString& file) {
    //    Open Workflow Designer.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsTaskTreeView::waitTaskFinished();
    int tabIndex = GTUtilsMdi::getCurrentTab();

    // Open "Align sequence with MUSCLE" sample.
    GTUtilsWorkflowDesigner::addSample("Align sequences with MUSCLE", GTUtilsMdi::activeWindow());

    // Cancel the wizard.
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    // Click to the "Read alignment" element.
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Read alignment"));
    GTMouseDriver::click();

    //  Add "file" file to "Dataset 1" dataset.
    GTUtilsWorkflowDesigner::setDatasetInputFile(file);

    return tabIndex;
}

GUI_TEST_CLASS_DEFINITION(output_dir_scanning_test_0005) {
    GTUtilsWorkflowDesigner::prepareDashboardsDirState("two_visible_two_invisible");

    QString msaFilePath = testDir + "_common_data/clustal/100_sequences.aln";
    int tabIndex1 = setUpMuscleSchemeInNewWdWindow(msaFilePath);
    int tabIndex2 = setUpMuscleSchemeInNewWdWindow(msaFilePath);

    //   Open en empty Workflow Designer window (number 3).
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    int tabIndex3 = GTUtilsMdi::getCurrentTab();
    GTUtilsTaskTreeView::waitTaskFinished();

    //   Click to the "Go to Dashboards" button on the toolbar.
    auto viewSwitchButton = GTUtilsWorkflowDesigner::getGotoDashboardButton();
    CHECK_SET_ERR(viewSwitchButton->isVisible(), "View switch button is unexpectedly invisible");
    CHECK_SET_ERR(viewSwitchButton->isEnabled(), "View switch button is unexpectedly disabled");
    GTWidget::click(viewSwitchButton);

    //    Switch to first Workflow Designer.
    coreLog.info(QString("Try to switch to tab %1(%2)").arg(tabIndex1).arg(GTUtilsMdi::getTabBar()->tabText(tabIndex1)));
    GTUtilsMdi::clickTab(tabIndex1);

    //    16. Launch the workflow. Do not wait for the task finish.
    coreLog.info("Try to start workflow #1");
    GTUtilsWorkflowDesigner::runWorkflow();
    coreLog.info("It seems that workflow was started");

    //    Switch to the second Workflow Designer.
    coreLog.info(QString("Try to switch to tab %1(%2)").arg(tabIndex2).arg(GTUtilsMdi::getTabBar()->tabText(tabIndex2)));
    GTUtilsMdi::clickTab(tabIndex2);

    //    Launch the workflow. Do not wait for the task finish.
    coreLog.info("Try to start workflow #2");
    GTUtilsWorkflowDesigner::runWorkflow();
    coreLog.info("It seems that workflow was started");

    GTUtilsWorkflowDesigner::prepareDashboardsDirState("empty_workflow_output");

    viewSwitchButton = GTUtilsWorkflowDesigner::getGotoWorkflowButton();
    CHECK_SET_ERR(viewSwitchButton->isVisible(), "View switch button is unexpectedly invisible");
    CHECK_SET_ERR(viewSwitchButton->isEnabled(), "View switch button is unexpectedly disabled");

    QString expectedButtonText = "To Workflow Designer";
    QString actualButtonText = viewSwitchButton->text();
    CHECK_SET_ERR(actualButtonText == expectedButtonText,
                  QString("View switch button has an unexpected text: expected '%1', got '%2'")
                      .arg(expectedButtonText)
                      .arg(actualButtonText));

    coreLog.info("Trying get GTUtilsDashboard::getTabWidget()");
    QTabWidget* dashboardsView = GTUtilsDashboard::getTabWidget();
    coreLog.info("Successfully got GTUtilsDashboard::getTabWidget()");

    int expectedTabsCount = 2;
    int actualTabsCount = dashboardsView->count();
    CHECK_SET_ERR(actualTabsCount == expectedTabsCount,
                  QString("There is an incorrect count of tabs in the Dashboard View: expected %1, got %2")
                      .arg(expectedTabsCount)
                      .arg(actualTabsCount));

    int expectedActiveTabIndex = 0;
    int actualActiveTabIndex = dashboardsView->currentIndex();
    CHECK_SET_ERR(expectedActiveTabIndex == actualActiveTabIndex,
                  QString("There is an incorrect active tab: expected index is %1, actual index is %2")
                      .arg(expectedActiveTabIndex)
                      .arg(actualActiveTabIndex));

    QString expectedTabName = "Align sequences with MUSCLE 2";
    QString actualTabName = GTUtilsDashboard::getDashboardName(expectedActiveTabIndex);
    CHECK_SET_ERR(expectedTabName == actualTabName,
                  QString("Active dashboard has an unexpected name: expect '%1', got '%2'")
                      .arg(expectedTabName)
                      .arg(actualTabName));

    QStringList outputFiles = GTUtilsDashboard::getOutputFiles();
    CHECK_SET_ERR(!outputFiles.isEmpty(), "Active dashboard is not displayed properly");

    QWidget* dashboardsManagerButton = GTToolbar::getWidgetForActionTooltip(GTToolbar::getToolbar(MWTOOLBAR_ACTIVEMDI), "Dashboards manager");
    CHECK_SET_ERR(dashboardsManagerButton->isEnabled(), "'Dashboards manager' button is unexpectedly disabled");

    //    Click to the "Dashboards manager" button on the toolbar.
    //    Expected result: the "Dashboards Manager" dialog appears. It contains two items, both of them are checked. Their names are "Align sequence with MUSCLE 1" and "Align sequence with MUSCLE 2".
    //    Cancel the dialog
    class Scenario : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            const QList<QPair<QString, bool>> expectedDashboardsState({qMakePair(QString("Align sequences with MUSCLE 1"), true),
                                                                       qMakePair(QString("Align sequences with MUSCLE 2"), true)});
            const QList<QPair<QString, bool>> actualDashboardsState = DashboardsManagerDialogFiller::getDashboardsState();

            CHECK_SET_ERR(expectedDashboardsState.size() == actualDashboardsState.size(),
                          QString("Expected dashboards count is not equal to the actual dashboards list size: expected %1, got %2")
                              .arg(expectedDashboardsState.size())
                              .arg(actualDashboardsState.size()));

            for (int i = 0; i < expectedDashboardsState.size(); ++i) {
                const QString expectedDashboardName = expectedDashboardsState[i].first;
                const QString actualDashboardName = actualDashboardsState[i].first;
                CHECK_SET_ERR(expectedDashboardName == actualDashboardName,
                              QString("Dashboard number %1 has an unexpected name: expected '%2', got '%3'")
                                  .arg(i)
                                  .arg(expectedDashboardName)
                                  .arg(actualDashboardName));

                const bool expectedDashboardState = expectedDashboardsState[i].second;
                const bool actualDashboardState = actualDashboardsState[i].second;
                CHECK_SET_ERR(expectedDashboardState == actualDashboardState,
                              QString("Dashboard number %1 has an unexpected state: it should be %2, but it is '%3'")
                                  .arg(i)
                                  .arg(expectedDashboardState ? "visible" : "invisible")
                                  .arg(actualDashboardState ? "visible" : "invisible"));
            }

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(new DashboardsManagerDialogFiller(new Scenario()));
    GTWidget::click(dashboardsManagerButton);

    //    Switch to the first Workflow Designer.
    //    Expected result:
    //     - The Workflow Designer is in the dashboards view mode.
    //     - There are two dashboard tabs. Their names are "Align sequence with MUSCLE 1" and "Align sequence with MUSCLE 2".
    //     - The "Align sequence with MUSCLE 1" dashboard is active.
    //     - The "Dashboards manager" button on the toolbar is active.
    coreLog.info(QString("Try to switch to tab %1(%2)").arg(tabIndex1).arg(GTUtilsMdi::getTabBar()->tabText(tabIndex1)));
    GTUtilsMdi::clickTab(tabIndex1);
    GTUtilsTaskTreeView::waitTaskFinished();

    viewSwitchButton = GTUtilsWorkflowDesigner::getGotoWorkflowButton();
    CHECK_SET_ERR(viewSwitchButton->isVisible(), "View switch button is unexpectedly invisible");
    CHECK_SET_ERR(viewSwitchButton->isEnabled(), "View switch button is unexpectedly disabled");

    expectedButtonText = "To Workflow Designer";
    actualButtonText = viewSwitchButton->text();
    CHECK_SET_ERR(expectedButtonText == actualButtonText,
                  QString("View switch button has an unexpected text: expected '%1', got '%2'")
                      .arg(expectedButtonText)
                      .arg(actualButtonText));

    coreLog.info("Trying get GTUtilsDashboard::getTabWidget()");
    dashboardsView = GTUtilsDashboard::getTabWidget();
    coreLog.info("Successfully got GTUtilsDashboard::getTabWidget()");

    expectedTabsCount = 2;
    actualTabsCount = dashboardsView->count();
    CHECK_SET_ERR(expectedTabsCount == actualTabsCount,
                  QString("There is an incorrect count of tabs in the Dashboard View: expected %1, got %2")
                      .arg(expectedTabsCount)
                      .arg(actualTabsCount));

    expectedActiveTabIndex = 0;
    actualActiveTabIndex = dashboardsView->currentIndex();
    CHECK_SET_ERR(expectedActiveTabIndex == actualActiveTabIndex,
                  QString("There is an incorrect active tab: expected index is %1, actual index is %2")
                      .arg(expectedActiveTabIndex)
                      .arg(actualActiveTabIndex));

    expectedTabName = "Align sequences with MUSCLE 1";
    actualTabName = GTUtilsDashboard::getDashboardName(expectedActiveTabIndex);
    CHECK_SET_ERR(expectedTabName == actualTabName,
                  QString("Active dashboard has an unexpected name: expect '%1', got '%2'")
                      .arg(expectedTabName)
                      .arg(actualTabName));

    outputFiles = GTUtilsDashboard::getOutputFiles();
    CHECK_SET_ERR(!outputFiles.isEmpty(), "Active dashboard is not displayed properly");

    dashboardsManagerButton = GTToolbar::getWidgetForActionTooltip(GTToolbar::getToolbar(MWTOOLBAR_ACTIVEMDI), "Dashboards manager");
    CHECK_SET_ERR(dashboardsManagerButton->isEnabled(), "'Dashboards manager' button is unexpectedly disabled");

    //    Click to the "Dashboards manager" button on the toolbar.
    //    Expected result:
    //     - the "Dashboards Manager" dialog appears.
    //       It contains two items, both of them are checked.
    //       Their names are "Align sequence with MUSCLE 1" and "Align sequence with MUSCLE 2".
    //     Cancel the dialog
    GTUtilsDialog::waitForDialog(new DashboardsManagerDialogFiller(new Scenario()));
    GTWidget::click(dashboardsManagerButton);

    //    Switch to the third Workflow Designer.
    //    Expected result:
    //     - The Workflow Designer is in the scene view mode.
    //     - The "Dashboards manager" button on the toolbar is active.
    coreLog.info(QString("Try to switch to tab %1(%2)").arg(tabIndex3).arg(GTUtilsMdi::getTabBar()->tabText(tabIndex3)));
    GTUtilsMdi::clickTab(tabIndex3);

    dashboardsManagerButton = GTToolbar::getWidgetForActionTooltip(GTToolbar::getToolbar(MWTOOLBAR_ACTIVEMDI), "Dashboards manager");
    CHECK_SET_ERR(dashboardsManagerButton->isEnabled(), "'Dashboards manager' button is unexpectedly disabled");

    viewSwitchButton = GTUtilsWorkflowDesigner::getGotoDashboardButton();
    CHECK_SET_ERR(viewSwitchButton->isVisible(), "'Go to Dashboards' is invisible");

    //    Click to the "Dashboards manager" button on the toolbar.
    //    Expected result:
    //     - the "Dashboards Manager" dialog appears.
    //     - It contains two items, both of them are checked.
    //     - Their names are "Align sequence with MUSCLE 1" and "Align sequence with MUSCLE 2".
    //     Cancel the dialog.
    GTUtilsDialog::waitForDialog(new DashboardsManagerDialogFiller(new Scenario()));
    GTWidget::click(dashboardsManagerButton);

    //    Click to the "Go to Dashboards" button on the toolbar.
    //    Expected result:
    //     - There are two dashboard tabs.
    //       Their names are "Align sequence with MUSCLE 1" and "Align sequence with MUSCLE 2" (warning: two last tabs can be swapped,
    //       it depends on the task finish order, it is a correct situation).
    //     - The first dashboard is active.
    viewSwitchButton = GTUtilsWorkflowDesigner::getGotoDashboardButton();
    CHECK_SET_ERR(viewSwitchButton->isVisible(), "View switch button is unexpectedly invisible");
    CHECK_SET_ERR(viewSwitchButton->isEnabled(), "View switch button is unexpectedly disabled");
    GTWidget::click(viewSwitchButton);

    coreLog.info("Trying get GTUtilsDashboard::getTabWidget()");
    dashboardsView = GTUtilsDashboard::getTabWidget();
    CHECK_SET_ERR(dashboardsView != nullptr, "Dashboards view is nullptr");
    coreLog.info("Successfully got GTUtilsDashboard::getTabWidget()");

    expectedTabsCount = 2;
    actualTabsCount = dashboardsView->count();
    CHECK_SET_ERR(expectedTabsCount == actualTabsCount,
                  QString("There is an incorrect count of tabs in the Dashboard View: expected %1, got %2")
                      .arg(expectedTabsCount)
                      .arg(actualTabsCount));

    expectedActiveTabIndex = 0;
    actualActiveTabIndex = dashboardsView->currentIndex();
    CHECK_SET_ERR(expectedActiveTabIndex == actualActiveTabIndex,
                  QString("There is an incorrect active tab: expected index is %1, actual index is %2")
                      .arg(expectedActiveTabIndex)
                      .arg(actualActiveTabIndex));

    expectedTabName = "Align sequences with MUSCLE 1";
    actualTabName = GTUtilsDashboard::getDashboardName(expectedActiveTabIndex);
    CHECK_SET_ERR(expectedTabName == actualTabName,
                  QString("Active dashboard has an unexpected name: expect '%1', got '%2'")
                      .arg(expectedTabName)
                      .arg(actualTabName));
}

GUI_TEST_CLASS_DEFINITION(output_dir_scanning_test_0006) {
    GTUtilsWorkflowDesigner::prepareDashboardsDirState("two_visible_two_invisible");

    //    3. Open Workflow Designer.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsTaskTreeView::waitTaskFinished();
    int tabIndex1 = GTUtilsMdi::getCurrentTab();

    //    4. Open "Align sequence with MUSCLE" sample.
    GTUtilsWorkflowDesigner::addSample("Align sequences with MUSCLE", GTUtilsMdi::activeWindow());

    //    5. Cancel the wizard.
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    //    6. Click to the "Read alignment" element.
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Read alignment"));
    GTMouseDriver::click();

    //    7. Add "_common_data/clustal/200_sequences.aln" file to "Dataset 1" dataset.
    GTUtilsWorkflowDesigner::setDatasetInputFile(testDir + "_common_data/clustal/100_sequences.aln");

    //    8. Open Workflow Designer.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsTaskTreeView::waitTaskFinished();
    int tabIndex2 = GTUtilsMdi::getCurrentTab();

    //    9. Open "Align sequence with MUSCLE" sample.
    GTUtilsWorkflowDesigner::addSample("Align sequences with MUSCLE", GTUtilsMdi::activeWindow());

    //    10. Cancel the wizard.
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    //    11. Click to the "Read alignment" element.
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Read alignment"));
    GTMouseDriver::click();

    //    12. Add "_common_data/clustal/200_sequences.aln" file to "Dataset 1" dataset.
    GTUtilsWorkflowDesigner::setDatasetInputFile(testDir + "_common_data/clustal/100_sequences.aln");

    //    13. Open Workflow Designer.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    int tabIndex3 = GTUtilsMdi::getCurrentTab();
    GTUtilsTaskTreeView::waitTaskFinished();

    //    14. Click to the "Go to Dashboards" button on the toolbar.
    auto viewSwitchButton = GTUtilsWorkflowDesigner::getGotoDashboardButton();
    CHECK_SET_ERR(viewSwitchButton->isVisible(), "View switch button is unexpectedly invisible");
    CHECK_SET_ERR(viewSwitchButton->isEnabled(), "View switch button is unexpectedly disabled");
    GTWidget::click(viewSwitchButton);

    //    15. Switch to first Workflow Designer.
    coreLog.info(QString("Try to switch to tab %1(%2)")
                     .arg(tabIndex1)
                     .arg(GTUtilsMdi::getTabBar()->tabText(tabIndex1)));
    GTUtilsMdi::clickTab(tabIndex1);

    //    16. Launch the workflow. Do not wait for the task finish.
    coreLog.info("Try to start workflow #1");
    GTUtilsWorkflowDesigner::runWorkflow();
    coreLog.info("It seems that workflow was started");

    //    17. Switch to the second Workflow Designer.
    coreLog.info(QString("Try to switch to tab %1(%2)")
                     .arg(tabIndex2)
                     .arg(GTUtilsMdi::getTabBar()->tabText(tabIndex2)));
    GTUtilsMdi::clickTab(tabIndex2);

    //    18. Launch the workflow. Do not wait for the task finish.
    coreLog.info("Try to start workflow #2");
    GTUtilsWorkflowDesigner::runWorkflow();
    coreLog.info("It seems that workflow was started");

    GTUtilsWorkflowDesigner::prepareDashboardsDirState("two_visible_dashboards");
    //    20. Wait for all tasks finish. The scan task is supposed to finish before align tasks.
    //        Expected result:
    //          - The Workflow Designer is in the dashboards view mode.
    //          - There are 4 dashboard tabs.
    //            Their names are "Align sequence with MUSCLE 2",
    //                            "Extract consensus as sequence 1",
    //                            "Extract consensus as sequence 2",
    //                            "Align sequence with MUSCLE 1".
    //          - The "Align sequence with MUSCLE 2" dashboard is active.
    //          - The "Dashboards manager" button on the toolbar is active.

    viewSwitchButton = GTUtilsWorkflowDesigner::getGotoWorkflowButton();
    CHECK_SET_ERR(viewSwitchButton->isVisible(), "View switch button is unexpectedly invisible");
    CHECK_SET_ERR(viewSwitchButton->isEnabled(), "View switch button is unexpectedly disabled");

    QString expectedButtonText = "To Workflow Designer";
    QString actualButtonText = viewSwitchButton->text();
    CHECK_SET_ERR(expectedButtonText == actualButtonText,
                  QString("View switch button has an unexpected text: expected '%1', got '%2'")
                      .arg(expectedButtonText)
                      .arg(actualButtonText));

    coreLog.info("Trying get GTUtilsDashboard::getTabWidget()");
    QTabWidget* dashboardsView = GTUtilsDashboard::getTabWidget();
    CHECK_SET_ERR(dashboardsView != nullptr, "Dashboards view is nullptr");
    coreLog.info("Successfully got GTUtilsDashboard::getTabWidget()");

    int expectedTabsCount = 4;
    int actualTabsCount = dashboardsView->count();
    CHECK_SET_ERR(expectedTabsCount == actualTabsCount,
                  QString("There is an incorrect count of tabs in the Dashboard View: expected %1, got %2")
                      .arg(expectedTabsCount)
                      .arg(actualTabsCount));

    int expectedActiveTabIndex = 0;
    int actualActiveTabIndex = dashboardsView->currentIndex();
    CHECK_SET_ERR(expectedActiveTabIndex == actualActiveTabIndex,
                  QString("There is an incorrect active tab: expected index is %1, actual index is %2")
                      .arg(expectedActiveTabIndex)
                      .arg(actualActiveTabIndex));

    QString expectedTabName = "Align sequences with MUSCLE 2";
    QString actualTabName = GTUtilsDashboard::getDashboardName(expectedActiveTabIndex);
    CHECK_SET_ERR(expectedTabName == actualTabName,
                  QString("Active dashboard has an unexpected name: expect '%1', got '%2'")
                      .arg(expectedTabName)
                      .arg(actualTabName));

    QStringList outputFiles = GTUtilsDashboard::getOutputFiles();
    CHECK_SET_ERR(!outputFiles.isEmpty(), "Active dashboard is not displayed properly");

    QWidget* dashboardsManagerButton = GTToolbar::getWidgetForActionTooltip(GTToolbar::getToolbar(MWTOOLBAR_ACTIVEMDI), "Dashboards manager");
    CHECK_SET_ERR(dashboardsManagerButton != nullptr, "'Dashboards manager' is nullptr");
    CHECK_SET_ERR(dashboardsManagerButton->isEnabled(), "'Dashboards manager' button is unexpectedly disabled");

    //    21. Click to the "Dashboards manager" button on the toolbar.
    //    Expected result: the "Dashboards Manager" dialog appears. It contains four items, all of them are checked. Their names are "Extract consensus as sequence 1", "Extract consensus as sequence 2", "Align sequence with MUSCLE 1", "Align sequence with MUSCLE 2".
    //    22. Cancel the dialog
    class Scenario : public CustomScenario {
    public:
        static bool sorting(const QPair<QString, bool>& e1, const QPair<QString, bool>& e2) {
            if (e1.first < e2.first)
                return true;
            return false;
        }

        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            QList<QPair<QString, bool>> expectedDashboardsState({qMakePair(QString("Align sequences with MUSCLE 1"), true),
                                                                 qMakePair(QString("Align sequences with MUSCLE 2"), true),
                                                                 qMakePair(QString("Extract consensus as sequence 1"), true),
                                                                 qMakePair(QString("Extract consensus as sequence 2"), true)});
            std::sort(expectedDashboardsState.begin(), expectedDashboardsState.end(), sorting);

            QList<QPair<QString, bool>> actualDashboardsState = DashboardsManagerDialogFiller::getDashboardsState();
            std::sort(actualDashboardsState.begin(), actualDashboardsState.end(), sorting);

            CHECK_SET_ERR(expectedDashboardsState.size() == actualDashboardsState.size(),
                          QString("Expected dashboards count is not equal to the actual dashboards list size: expected %1, got %2")
                              .arg(expectedDashboardsState.size())
                              .arg(actualDashboardsState.size()));

            for (int i = 0; i < expectedDashboardsState.size(); ++i) {
                const QString expectedDashboardName = expectedDashboardsState[i].first;
                const QString actualDashboardName = actualDashboardsState[i].first;
                CHECK_SET_ERR(expectedDashboardName == actualDashboardName,
                              QString("Dashboard number %1 has an unexpected name: expected '%2', got '%3'")
                                  .arg(i)
                                  .arg(expectedDashboardName)
                                  .arg(actualDashboardName));

                const bool expectedDashboardState = expectedDashboardsState[i].second;
                const bool actualDashboardState = actualDashboardsState[i].second;
                CHECK_SET_ERR(expectedDashboardState == actualDashboardState,
                              QString("Dashboard number %1 has an unexpected state: it should be %2, but it is '%3'")
                                  .arg(i)
                                  .arg(expectedDashboardState ? "visible" : "invisible")
                                  .arg(actualDashboardState ? "visible" : "invisible"));
            }

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(new DashboardsManagerDialogFiller(new Scenario()));
    GTWidget::click(dashboardsManagerButton);

    //    23. Switch to the first Workflow Designer.
    //    Expected result:
    //     - The Workflow Designer is in the dashboards view mode.
    //     - There are four dashboard tabs. Their names are "Align sequence with MUSCLE 1", "Extract consensus as sequence 1", "Extract consensus as sequence 2", "Align sequence with MUSCLE 2".
    //     - The "Align sequence with MUSCLE 1" dashboard is active.
    //     - The "Dashboards manager" button on the toolbar is active.
    coreLog.info(QString("Try to switch to tab %1(%2)")
                     .arg(tabIndex1)
                     .arg(GTUtilsMdi::getTabBar()->tabText(tabIndex1)));
    GTUtilsMdi::clickTab(tabIndex1);
    GTUtilsTaskTreeView::waitTaskFinished();

    viewSwitchButton = GTUtilsWorkflowDesigner::getGotoWorkflowButton();
    CHECK_SET_ERR(viewSwitchButton->isVisible(), "View switch button is unexpectedly invisible");
    CHECK_SET_ERR(viewSwitchButton->isEnabled(), "View switch button is unexpectedly disabled");

    expectedButtonText = "To Workflow Designer";
    actualButtonText = viewSwitchButton->text();
    CHECK_SET_ERR(expectedButtonText == actualButtonText,
                  QString("View switch button has an unexpected text: expected '%1', got '%2'")
                      .arg(expectedButtonText)
                      .arg(actualButtonText));

    coreLog.info("Trying get GTUtilsDashboard::getTabWidget()");
    dashboardsView = GTUtilsDashboard::getTabWidget();
    CHECK_SET_ERR(dashboardsView != nullptr, "Dashboards view is nullptr");
    coreLog.info("Successfully got GTUtilsDashboard::getTabWidget()");

    expectedTabsCount = 4;
    actualTabsCount = dashboardsView->count();
    CHECK_SET_ERR(expectedTabsCount == actualTabsCount,
                  QString("There is an incorrect count of tabs in the Dashboard View: expected %1, got %2")
                      .arg(expectedTabsCount)
                      .arg(actualTabsCount));

    expectedActiveTabIndex = 0;
    actualActiveTabIndex = dashboardsView->currentIndex();
    CHECK_SET_ERR(expectedActiveTabIndex == actualActiveTabIndex,
                  QString("There is an incorrect active tab: expected index is %1, actual index is %2")
                      .arg(expectedActiveTabIndex)
                      .arg(actualActiveTabIndex));

    expectedTabName = "Align sequences with MUSCLE 1";
    actualTabName = GTUtilsDashboard::getDashboardName(expectedActiveTabIndex);
    CHECK_SET_ERR(expectedTabName == actualTabName,
                  QString("Active dashboard has an unexpected name: expect '%1', got '%2'")
                      .arg(expectedTabName)
                      .arg(actualTabName));

    outputFiles = GTUtilsDashboard::getOutputFiles();
    CHECK_SET_ERR(!outputFiles.isEmpty(), "Active dashboard is not displayed properly");

    dashboardsManagerButton = GTToolbar::getWidgetForActionTooltip(GTToolbar::getToolbar(MWTOOLBAR_ACTIVEMDI), "Dashboards manager");
    CHECK_SET_ERR(dashboardsManagerButton != nullptr, "'Dashboards manager' is nullptr");
    CHECK_SET_ERR(dashboardsManagerButton->isEnabled(), "'Dashboards manager' button is unexpectedly disabled");

    //    24. Click to the "Dashboards manager" button on the toolbar.
    //    Expected result:
    //     - the "Dashboards Manager" dialog appears.
    //       It contains 4 items, all of them are checked.
    //       Their names are "Extract consensus as sequence 1", "Extract consensus as sequence 2", "Align sequence with MUSCLE 1", "Align sequence with MUSCLE 2".
    //    25. Cancel the dialog
    GTUtilsDialog::waitForDialog(new DashboardsManagerDialogFiller(new Scenario()));
    GTWidget::click(dashboardsManagerButton);

    //    26. Switch to the third Workflow Designer.
    //    Expected result:
    //     - The Workflow Designer is in the dashboard view mode.
    //     - The "Dashboards manager" button on the toolbar is active.
    //     - There are four dashboard tabs. Their names are "Extract consensus as sequence 1", "Extract consensus as sequence 2", "Align sequence with MUSCLE 1", "Align sequence with MUSCLE 2" (warning: two last tabs can be swapped, it depends on the task finish order, it is a correct situation).
    //     - The "Extract consensus as sequence 2" dashboard is active.
    coreLog.info(QString("Try to switch to tab %1(%2)")
                     .arg(tabIndex3)
                     .arg(GTUtilsMdi::getTabBar()->tabText(tabIndex3)));
    GTUtilsMdi::clickTab(tabIndex3);
    GTUtilsTaskTreeView::waitTaskFinished();

    viewSwitchButton = GTUtilsWorkflowDesigner::getGotoWorkflowButton();
    CHECK_SET_ERR(viewSwitchButton->isVisible(), "View switch button is unexpectedly invisible");
    CHECK_SET_ERR(viewSwitchButton->isEnabled(), "View switch button is unexpectedly disabled");

    expectedButtonText = "To Workflow Designer";
    actualButtonText = viewSwitchButton->text();
    CHECK_SET_ERR(expectedButtonText == actualButtonText,
                  QString("View switch button has an unexpected text: expected '%1', got '%2'")
                      .arg(expectedButtonText)
                      .arg(actualButtonText));

    coreLog.info("Trying get GTUtilsDashboard::getTabWidget()");
    dashboardsView = GTUtilsDashboard::getTabWidget();
    CHECK_SET_ERR(dashboardsView != nullptr, "Dashboards view is nullptr");
    coreLog.info("Successfully got GTUtilsDashboard::getTabWidget()");

    expectedTabsCount = 4;
    actualTabsCount = dashboardsView->count();
    CHECK_SET_ERR(expectedTabsCount == actualTabsCount,
                  QString("There is an incorrect count of tabs in the Dashboard View: expected %1, got %2")
                      .arg(expectedTabsCount)
                      .arg(actualTabsCount));

    actualActiveTabIndex = dashboardsView->currentIndex();
    CHECK_SET_ERR(actualActiveTabIndex >= 0 && actualActiveTabIndex < expectedTabsCount,
                  QString("There is an incorrect active tab: expected index is %1, actual index is %2")
                      .arg(expectedActiveTabIndex)
                      .arg(actualActiveTabIndex));

    expectedTabName = "Extract consensus as sequence 2";
    actualTabName = GTUtilsDashboard::getDashboardName(actualActiveTabIndex);
    CHECK_SET_ERR(expectedTabName == actualTabName,
                  QString("Active dashboard has an unexpected name: expect '%1', got '%2'")
                      .arg(expectedTabName)
                      .arg(actualTabName));

    outputFiles = GTUtilsDashboard::getOutputFiles();
    CHECK_SET_ERR(!outputFiles.isEmpty(), "Active dashboard is not displayed properly");

    dashboardsManagerButton = GTToolbar::getWidgetForActionTooltip(GTToolbar::getToolbar(MWTOOLBAR_ACTIVEMDI), "Dashboards manager");
    CHECK_SET_ERR(dashboardsManagerButton != nullptr, "'Dashboards manager' is nullptr");
    CHECK_SET_ERR(dashboardsManagerButton->isEnabled(), "'Dashboards manager' button is unexpectedly disabled");

    //    27. Click to the "Dashboards manager" button on the toolbar.
    //    Expected result:
    //     - the "Dashboards Manager" dialog appears.
    //     - It contains two items, both of them are checked.
    //     - Their names are "Align sequence with MUSCLE 1" and "Align sequence with MUSCLE 2".
    //    28. Cancel the dialog.
    GTUtilsDialog::waitForDialog(new DashboardsManagerDialogFiller(new Scenario()));
    GTWidget::click(dashboardsManagerButton);
}

GUI_TEST_CLASS_DEFINITION(output_dir_scanning_test_0007) {
    GTUtilsWorkflowDesigner::prepareDashboardsDirState("two_visible_two_invisible");
    //    3. Open Workflow Designer.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsTaskTreeView::waitTaskFinished();
    int tabIndex1 = GTUtilsMdi::getCurrentTab();

    //    4. Open "Align sequence with MUSCLE" sample.
    GTUtilsWorkflowDesigner::addSample("Align sequences with MUSCLE", GTUtilsMdi::activeWindow());

    //    5. Cancel the wizard.
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    //    6. Click to the "Read alignment" element.
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Read alignment"));
    GTMouseDriver::click();

    //    7. Add "_common_data/clustal/200_sequences.aln" file to "Dataset 1" dataset.
    GTUtilsWorkflowDesigner::setDatasetInputFile(testDir + "_common_data/clustal/100_sequences.aln");

    //    8. Open Workflow Designer.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsTaskTreeView::waitTaskFinished();
    int tabIndex2 = GTUtilsMdi::getCurrentTab();

    //    9. Open "Align sequence with MUSCLE" sample.
    GTUtilsWorkflowDesigner::addSample("Align sequences with MUSCLE", GTUtilsMdi::activeWindow());

    //    10. Cancel the wizard.
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    //    11. Click to the "Read alignment" element.
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Read alignment"));
    GTMouseDriver::click();

    //    12. Add "_common_data/clustal/200_sequences.aln" file to "Dataset 1" dataset.
    GTUtilsWorkflowDesigner::setDatasetInputFile(testDir + "_common_data/clustal/100_sequences.aln");

    //    13. Open Workflow Designer.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    int tabIndex3 = GTUtilsMdi::getCurrentTab();
    GTUtilsTaskTreeView::waitTaskFinished();

    //    14. Click to the "Go to Dashboards" button on the toolbar.
    auto viewSwitchButton = GTUtilsWorkflowDesigner::getGotoDashboardButton();
    CHECK_SET_ERR(viewSwitchButton->isVisible(), "View switch button is unexpectedly invisible");
    CHECK_SET_ERR(viewSwitchButton->isEnabled(), "View switch button is unexpectedly disabled");
    GTWidget::click(viewSwitchButton);

    //    15. Switch to first Workflow Designer.
    coreLog.info(QString("Try to switch to tab %1(%2)")
                     .arg(tabIndex1)
                     .arg(GTUtilsMdi::getTabBar()->tabText(tabIndex1)));
    GTUtilsMdi::clickTab(tabIndex1);

    //    16. Launch the workflow. Do not wait for the task finish.
    coreLog.info("Try to start workflow #1");
    GTUtilsWorkflowDesigner::runWorkflow();
    coreLog.info("It seems that workflow was started");

    //    17. Switch to the second Workflow Designer.
    coreLog.info(QString("Try to switch to tab %1(%2)")
                     .arg(tabIndex2)
                     .arg(GTUtilsMdi::getTabBar()->tabText(tabIndex2)));
    GTUtilsMdi::clickTab(tabIndex2);

    //    18. Launch the workflow. Do not wait for the task finish.
    coreLog.info("Try to start workflow #2");
    GTUtilsWorkflowDesigner::runWorkflow();
    coreLog.info("It seems that workflow was started");

    //    19. Set "_common_data/workflow/dashboard/workflow_outputs/one_visible_one_invisible" as workflow output folder in the "Application Settings".
    GTUtilsWorkflowDesigner::prepareDashboardsDirState("one_visible_one_invisible");
    //    20. Wait for all tasks finish. The scan task is supposed to finish before align tasks.
    //        Expected result:
    //          - The Workflow Designer is in the dashboards view mode.
    //          - There are 3 dashboard tabs.
    //            Their names are "Align sequence with MUSCLE 2",
    //                            "Extract consensus as sequence 2",
    //                            "Align sequence with MUSCLE 1".
    //          - The "Align sequence with MUSCLE 2" dashboard is active.
    //          - The "Dashboards manager" button on the toolbar is active.

    viewSwitchButton = GTUtilsWorkflowDesigner::getGotoWorkflowButton();
    CHECK_SET_ERR(viewSwitchButton->isVisible(), "View switch button is unexpectedly invisible");
    CHECK_SET_ERR(viewSwitchButton->isEnabled(), "View switch button is unexpectedly disabled");

    QString expectedButtonText = "To Workflow Designer";
    QString actualButtonText = viewSwitchButton->text();
    CHECK_SET_ERR(expectedButtonText == actualButtonText,
                  QString("View switch button has an unexpected text: expected '%1', got '%2'")
                      .arg(expectedButtonText)
                      .arg(actualButtonText));

    coreLog.info("Trying get GTUtilsDashboard::getTabWidget()");
    QTabWidget* dashboardsView = GTUtilsDashboard::getTabWidget();
    CHECK_SET_ERR(dashboardsView != nullptr, "Dashboards view is nullptr");
    coreLog.info("Successfully got GTUtilsDashboard::getTabWidget()");

    int expectedTabsCount = 3;
    int actualTabsCount = dashboardsView->count();
    CHECK_SET_ERR(expectedTabsCount == actualTabsCount,
                  QString("There is an incorrect count of tabs in the Dashboard View: expected %1, got %2")
                      .arg(expectedTabsCount)
                      .arg(actualTabsCount));

    int expectedActiveTabIndex = 0;
    int actualActiveTabIndex = dashboardsView->currentIndex();
    CHECK_SET_ERR(expectedActiveTabIndex == actualActiveTabIndex,
                  QString("There is an incorrect active tab: expected index is %1, actual index is %2")
                      .arg(expectedActiveTabIndex)
                      .arg(actualActiveTabIndex));

    QString expectedTabName = "Align sequences with MUSCLE 2";
    QString actualTabName = GTUtilsDashboard::getDashboardName(expectedActiveTabIndex);
    CHECK_SET_ERR(expectedTabName == actualTabName,
                  QString("Active dashboard has an unexpected name: expect '%1', got '%2'")
                      .arg(expectedTabName)
                      .arg(actualTabName));

    QStringList outputFiles = GTUtilsDashboard::getOutputFiles();
    CHECK_SET_ERR(!outputFiles.isEmpty(), "Active dashboard is not displayed properly");

    QWidget* dashboardsManagerButton = GTToolbar::getWidgetForActionTooltip(GTToolbar::getToolbar(MWTOOLBAR_ACTIVEMDI), "Dashboards manager");
    CHECK_SET_ERR(dashboardsManagerButton != nullptr, "'Dashboards manager' is nullptr");
    CHECK_SET_ERR(dashboardsManagerButton->isEnabled(), "'Dashboards manager' button is unexpectedly disabled");

    //    21. Click to the "Dashboards manager" button on the toolbar.
    //    Expected result: the "Dashboards Manager" dialog appears. It contains 3 items, not all of them are checked. Their names are "Extract consensus as sequence 1", "Extract consensus as sequence 2", "Align sequence with MUSCLE 1", "Align sequence with MUSCLE 2".
    //    22. Cancel the dialog
    class Scenario : public CustomScenario {
    public:
        static bool sorting(const QPair<QString, bool>& e1, const QPair<QString, bool>& e2) {
            if (e1.first < e2.first)
                return true;
            return false;
        }

        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            QList<QPair<QString, bool>> expectedDashboardsState({qMakePair(QString("Align sequences with MUSCLE 1"), true),
                                                                 qMakePair(QString("Align sequences with MUSCLE 2"), true),
                                                                 qMakePair(QString("Extract consensus as sequence 1"), false),
                                                                 qMakePair(QString("Extract consensus as sequence 2"), true)});
            std::sort(expectedDashboardsState.begin(), expectedDashboardsState.end(), sorting);

            QList<QPair<QString, bool>> actualDashboardsState = DashboardsManagerDialogFiller::getDashboardsState();
            std::sort(actualDashboardsState.begin(), actualDashboardsState.end(), sorting);

            CHECK_SET_ERR(expectedDashboardsState.size() == actualDashboardsState.size(),
                          QString("Expected dashboards count is not equal to the actual dashboards list size: expected %1, got %2")
                              .arg(expectedDashboardsState.size())
                              .arg(actualDashboardsState.size()));

            for (int i = 0; i < expectedDashboardsState.size(); ++i) {
                const QString expectedDashboardName = expectedDashboardsState[i].first;
                const QString actualDashboardName = actualDashboardsState[i].first;
                CHECK_SET_ERR(expectedDashboardName == actualDashboardName,
                              QString("Dashboard number %1 has an unexpected name: expected '%2', got '%3'")
                                  .arg(i)
                                  .arg(expectedDashboardName)
                                  .arg(actualDashboardName));

                const bool expectedDashboardState = expectedDashboardsState[i].second;
                const bool actualDashboardState = actualDashboardsState[i].second;
                CHECK_SET_ERR(expectedDashboardState == actualDashboardState,
                              QString("Dashboard number %1 has an unexpected state: it should be %2, but it is '%3'")
                                  .arg(i)
                                  .arg(expectedDashboardState ? "visible" : "invisible")
                                  .arg(actualDashboardState ? "visible" : "invisible"));
            }

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(new DashboardsManagerDialogFiller(new Scenario()));
    GTWidget::click(dashboardsManagerButton);

    //    23. Switch to the first Workflow Designer.
    //    Expected result:
    //     - The Workflow Designer is in the dashboards view mode.
    //     - There are 3 dashboard tabs. Their names are "Align sequence with MUSCLE 1", "Extract consensus as sequence 2", "Align sequence with MUSCLE 2".
    //     - The "Align sequence with MUSCLE 1" dashboard is active.
    //     - The "Dashboards manager" button on the toolbar is active.
    coreLog.info(QString("Try to switch to tab %1(%2)")
                     .arg(tabIndex1)
                     .arg(GTUtilsMdi::getTabBar()->tabText(tabIndex1)));
    GTUtilsMdi::clickTab(tabIndex1);
    GTUtilsTaskTreeView::waitTaskFinished();

    viewSwitchButton = GTUtilsWorkflowDesigner::getGotoWorkflowButton();
    CHECK_SET_ERR(viewSwitchButton->isVisible(), "View switch button is unexpectedly invisible");
    CHECK_SET_ERR(viewSwitchButton->isEnabled(), "View switch button is unexpectedly disabled");

    expectedButtonText = "To Workflow Designer";
    actualButtonText = viewSwitchButton->text();
    CHECK_SET_ERR(expectedButtonText == actualButtonText,
                  QString("View switch button has an unexpected text: expected '%1', got '%2'")
                      .arg(expectedButtonText)
                      .arg(actualButtonText));

    coreLog.info("Trying get GTUtilsDashboard::getTabWidget()");
    dashboardsView = GTUtilsDashboard::getTabWidget();
    CHECK_SET_ERR(dashboardsView != nullptr, "Dashboards view is nullptr");
    coreLog.info("Successfully got GTUtilsDashboard::getTabWidget()");

    expectedTabsCount = 3;
    actualTabsCount = dashboardsView->count();
    CHECK_SET_ERR(expectedTabsCount == actualTabsCount,
                  QString("There is an incorrect count of tabs in the Dashboard View: expected %1, got %2")
                      .arg(expectedTabsCount)
                      .arg(actualTabsCount));

    expectedActiveTabIndex = 0;
    actualActiveTabIndex = dashboardsView->currentIndex();
    CHECK_SET_ERR(expectedActiveTabIndex == actualActiveTabIndex,
                  QString("There is an incorrect active tab: expected index is %1, actual index is %2")
                      .arg(expectedActiveTabIndex)
                      .arg(actualActiveTabIndex));

    expectedTabName = "Align sequences with MUSCLE 1";
    actualTabName = GTUtilsDashboard::getDashboardName(expectedActiveTabIndex);
    CHECK_SET_ERR(expectedTabName == actualTabName,
                  QString("Active dashboard has an unexpected name: expect '%1', got '%2'")
                      .arg(expectedTabName)
                      .arg(actualTabName));

    outputFiles = GTUtilsDashboard::getOutputFiles();
    CHECK_SET_ERR(!outputFiles.isEmpty(), "Active dashboard is not displayed properly");

    dashboardsManagerButton = GTToolbar::getWidgetForActionTooltip(GTToolbar::getToolbar(MWTOOLBAR_ACTIVEMDI), "Dashboards manager");
    CHECK_SET_ERR(dashboardsManagerButton != nullptr, "'Dashboards manager' is nullptr");
    CHECK_SET_ERR(dashboardsManagerButton->isEnabled(), "'Dashboards manager' button is unexpectedly disabled");

    //    24. Click to the "Dashboards manager" button on the toolbar.
    //    Expected result:
    //     - the "Dashboards Manager" dialog appears.
    //       It contains 4 items, all of them are checked.
    //       Their names are "Extract consensus as sequence 1", "Extract consensus as sequence 2", "Align sequence with MUSCLE 1", "Align sequence with MUSCLE 2".
    //    25. Cancel the dialog
    GTUtilsDialog::waitForDialog(new DashboardsManagerDialogFiller(new Scenario()));
    GTWidget::click(dashboardsManagerButton);

    //    26. Switch to the third Workflow Designer.
    //    Expected result:
    //     - The Workflow Designer is in the dashboard view mode.
    //     - The "Dashboards manager" button on the toolbar is active.
    //     - There are 3 dashboard tabs. Their names are "Extract consensus as sequence 2", "Align sequence with MUSCLE 1", "Align sequence with MUSCLE 2" (warning: two last tabs can be swapped, it depends on the task finish order, it is a correct situation).
    //     - The "Extract consensus as sequence 2" dashboard is active.
    coreLog.info(QString("Try to switch to tab %1(%2)")
                     .arg(tabIndex3)
                     .arg(GTUtilsMdi::getTabBar()->tabText(tabIndex3)));
    GTUtilsMdi::clickTab(tabIndex3);
    GTUtilsTaskTreeView::waitTaskFinished();

    viewSwitchButton = GTUtilsWorkflowDesigner::getGotoWorkflowButton();
    CHECK_SET_ERR(viewSwitchButton->isVisible(), "View switch button is unexpectedly invisible");
    CHECK_SET_ERR(viewSwitchButton->isEnabled(), "View switch button is unexpectedly disabled");

    expectedButtonText = "To Workflow Designer";
    actualButtonText = viewSwitchButton->text();
    CHECK_SET_ERR(expectedButtonText == actualButtonText,
                  QString("View switch button has an unexpected text: expected '%1', got '%2'")
                      .arg(expectedButtonText)
                      .arg(actualButtonText));

    coreLog.info("Trying get GTUtilsDashboard::getTabWidget()");
    dashboardsView = GTUtilsDashboard::getTabWidget();
    CHECK_SET_ERR(dashboardsView != nullptr, "Dashboards view is nullptr");
    coreLog.info("Successfully got GTUtilsDashboard::getTabWidget()");

    expectedTabsCount = 3;
    actualTabsCount = dashboardsView->count();
    CHECK_SET_ERR(expectedTabsCount == actualTabsCount,
                  QString("There is an incorrect count of tabs in the Dashboard View: expected %1, got %2")
                      .arg(expectedTabsCount)
                      .arg(actualTabsCount));

    actualActiveTabIndex = dashboardsView->currentIndex();
    CHECK_SET_ERR(actualActiveTabIndex >= 0 && actualActiveTabIndex < expectedTabsCount,
                  QString("There is an incorrect active tab: expected index is %1, actual index is %2")
                      .arg(expectedActiveTabIndex)
                      .arg(actualActiveTabIndex));

    expectedTabName = "Extract consensus as sequence 2";
    actualTabName = GTUtilsDashboard::getDashboardName(actualActiveTabIndex);
    CHECK_SET_ERR(expectedTabName == actualTabName,
                  QString("Active dashboard has an unexpected name: expect '%1', got '%2'")
                      .arg(expectedTabName)
                      .arg(actualTabName));

    outputFiles = GTUtilsDashboard::getOutputFiles();
    CHECK_SET_ERR(!outputFiles.isEmpty(), "Active dashboard is not displayed properly");

    dashboardsManagerButton = GTToolbar::getWidgetForActionTooltip(GTToolbar::getToolbar(MWTOOLBAR_ACTIVEMDI), "Dashboards manager");
    CHECK_SET_ERR(dashboardsManagerButton != nullptr, "'Dashboards manager' is nullptr");
    CHECK_SET_ERR(dashboardsManagerButton->isEnabled(), "'Dashboards manager' button is unexpectedly disabled");

    //    27. Click to the "Dashboards manager" button on the toolbar.
    //    Expected result:
    //     - the "Dashboards Manager" dialog appears.
    //     - It contains 4 items, not all of them are checked.
    //     - Their names are "Extract consensus as sequence 1", "Extract consensus as sequence 2", "Align sequence with MUSCLE 1" and "Align sequence with MUSCLE 2".
    //    28. Cancel the dialog.
    GTUtilsDialog::waitForDialog(new DashboardsManagerDialogFiller(new Scenario()));
    GTWidget::click(dashboardsManagerButton);
}

GUI_TEST_CLASS_DEFINITION(output_dir_scanning_test_0008) {
    //    1. Set "_common_data/workflow/dashboard/workflow_outputs/two_visible_two_invisible" as workflow output folder in the "Application Settings".
    GTUtilsWorkflowDesigner::prepareDashboardsDirState("two_visible_two_invisible");

    //    3. Open Workflow Designer.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsTaskTreeView::waitTaskFinished();
    int tabIndex1 = GTUtilsMdi::getCurrentTab();

    //    4. Open "Align sequence with MUSCLE" sample.
    GTUtilsWorkflowDesigner::addSample("Align sequences with MUSCLE", GTUtilsMdi::activeWindow());

    //    5. Cancel the wizard.
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    //    6. Click to the "Read alignment" element.
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Read alignment"));
    GTMouseDriver::click();

    //    7. Add "_common_data/clustal/200_sequences.aln" file to "Dataset 1" dataset.
    GTUtilsWorkflowDesigner::setDatasetInputFile(testDir + "_common_data/clustal/100_sequences.aln");

    //    8. Open Workflow Designer.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsTaskTreeView::waitTaskFinished();
    int tabIndex2 = GTUtilsMdi::getCurrentTab();

    //    9. Open "Align sequence with MUSCLE" sample.
    GTUtilsWorkflowDesigner::addSample("Align sequences with MUSCLE", GTUtilsMdi::activeWindow());

    //    10. Cancel the wizard.
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    //    11. Click to the "Read alignment" element.
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Read alignment"));
    GTMouseDriver::click();

    //    12. Add "_common_data/clustal/200_sequences.aln" file to "Dataset 1" dataset.
    GTUtilsWorkflowDesigner::setDatasetInputFile(testDir + "_common_data/clustal/100_sequences.aln");

    //    13. Open Workflow Designer.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    int tabIndex3 = GTUtilsMdi::getCurrentTab();
    GTUtilsTaskTreeView::waitTaskFinished();

    //    14. Click to the "Go to Dashboards" button on the toolbar.
    auto viewSwitchButton = GTUtilsWorkflowDesigner::getGotoDashboardButton();
    CHECK_SET_ERR(viewSwitchButton->isVisible(), "View switch button is unexpectedly invisible");
    CHECK_SET_ERR(viewSwitchButton->isEnabled(), "View switch button is unexpectedly disabled");
    GTWidget::click(viewSwitchButton);

    //    15. Switch to first Workflow Designer.
    coreLog.info(QString("Try to switch to tab %1(%2)")
                     .arg(tabIndex1)
                     .arg(GTUtilsMdi::getTabBar()->tabText(tabIndex1)));
    GTUtilsMdi::clickTab(tabIndex1);

    //    16. Launch the workflow. Do not wait for the task finish.
    coreLog.info("Try to start workflow #1");
    GTUtilsWorkflowDesigner::runWorkflow();
    coreLog.info("It seems that workflow was started");

    //    17. Switch to the second Workflow Designer.
    coreLog.info(QString("Try to switch to tab %1(%2)")
                     .arg(tabIndex2)
                     .arg(GTUtilsMdi::getTabBar()->tabText(tabIndex2)));
    GTUtilsMdi::clickTab(tabIndex2);

    //    18. Launch the workflow. Do not wait for the task finish.
    coreLog.info("Try to start workflow #2");
    GTUtilsWorkflowDesigner::runWorkflow();
    coreLog.info("It seems that workflow was started");

    //    19. Set "_common_data/workflow/dashboard/workflow_outputs/two_invisible_dashboards" as workflow output folder in the "Application Settings".
    GTUtilsWorkflowDesigner::prepareDashboardsDirState("two_invisible_dashboards");
    //    20. Wait for all tasks finish. The scan task is supposed to finish before align tasks.
    //        Expected result:
    //          - The Workflow Designer is in the dashboards view mode.
    //          - There are 2 dashboard tabs.
    //            Their names are "Align sequence with MUSCLE 2",
    //                            "Align sequence with MUSCLE 1".
    //          - The "Align sequence with MUSCLE 2" dashboard is active.
    //          - The "Dashboards manager" button on the toolbar is active.

    viewSwitchButton = GTUtilsWorkflowDesigner::getGotoWorkflowButton();
    CHECK_SET_ERR(viewSwitchButton->isVisible(), "View switch button is unexpectedly invisible");
    CHECK_SET_ERR(viewSwitchButton->isEnabled(), "View switch button is unexpectedly disabled");

    QString expectedButtonText = "To Workflow Designer";
    QString actualButtonText = viewSwitchButton->text();
    CHECK_SET_ERR(expectedButtonText == actualButtonText,
                  QString("View switch button has an unexpected text: expected '%1', got '%2'")
                      .arg(expectedButtonText)
                      .arg(actualButtonText));

    coreLog.info("Trying get GTUtilsDashboard::getTabWidget()");
    QTabWidget* dashboardsView = GTUtilsDashboard::getTabWidget();
    CHECK_SET_ERR(dashboardsView != nullptr, "Dashboards view is nullptr");
    coreLog.info("Successfully got GTUtilsDashboard::getTabWidget()");

    int expectedTabsCount = 2;
    int actualTabsCount = dashboardsView->count();
    CHECK_SET_ERR(expectedTabsCount == actualTabsCount,
                  QString("There is an incorrect count of tabs in the Dashboard View: expected %1, got %2")
                      .arg(expectedTabsCount)
                      .arg(actualTabsCount));

    int expectedActiveTabIndex = 0;
    int actualActiveTabIndex = dashboardsView->currentIndex();
    CHECK_SET_ERR(expectedActiveTabIndex == actualActiveTabIndex,
                  QString("There is an incorrect active tab: expected index is %1, actual index is %2")
                      .arg(expectedActiveTabIndex)
                      .arg(actualActiveTabIndex));

    QString expectedTabName = "Align sequences with MUSCLE 2";
    QString actualTabName = GTUtilsDashboard::getDashboardName(expectedActiveTabIndex);
    CHECK_SET_ERR(expectedTabName == actualTabName,
                  QString("Active dashboard has an unexpected name: expect '%1', got '%2'")
                      .arg(expectedTabName)
                      .arg(actualTabName));

    QStringList outputFiles = GTUtilsDashboard::getOutputFiles();
    CHECK_SET_ERR(!outputFiles.isEmpty(), "Active dashboard is not displayed properly");

    QWidget* dashboardsManagerButton = GTToolbar::getWidgetForActionTooltip(GTToolbar::getToolbar(MWTOOLBAR_ACTIVEMDI), "Dashboards manager");
    CHECK_SET_ERR(dashboardsManagerButton != nullptr, "'Dashboards manager' is nullptr");
    CHECK_SET_ERR(dashboardsManagerButton->isEnabled(), "'Dashboards manager' button is unexpectedly disabled");

    //    21. Click to the "Dashboards manager" button on the toolbar.
    //    Expected result: the "Dashboards Manager" dialog appears. It contains four items, not all of them are checked. Their names are "Extract consensus as sequence 1", "Extract consensus as sequence 2", "Align sequence with MUSCLE 1", "Align sequence with MUSCLE 2".
    //    22. Cancel the dialog
    class Scenario : public CustomScenario {
    public:
        static bool sorting(const QPair<QString, bool>& e1, const QPair<QString, bool>& e2) {
            if (e1.first < e2.first)
                return true;
            return false;
        }

        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            QList<QPair<QString, bool>> expectedDashboardsState({qMakePair(QString("Align sequences with MUSCLE 1"), true),
                                                                 qMakePair(QString("Align sequences with MUSCLE 2"), true),
                                                                 qMakePair(QString("Extract consensus as sequence 1"), false),
                                                                 qMakePair(QString("Extract consensus as sequence 2"), false)});
            std::sort(expectedDashboardsState.begin(), expectedDashboardsState.end(), sorting);

            QList<QPair<QString, bool>> actualDashboardsState = DashboardsManagerDialogFiller::getDashboardsState();
            std::sort(actualDashboardsState.begin(), actualDashboardsState.end(), sorting);

            CHECK_SET_ERR(expectedDashboardsState.size() == actualDashboardsState.size(),
                          QString("Expected dashboards count is not equal to the actual dashboards list size: expected %1, got %2")
                              .arg(expectedDashboardsState.size())
                              .arg(actualDashboardsState.size()));

            for (int i = 0; i < expectedDashboardsState.size(); ++i) {
                const QString expectedDashboardName = expectedDashboardsState[i].first;
                const QString actualDashboardName = actualDashboardsState[i].first;
                CHECK_SET_ERR(expectedDashboardName == actualDashboardName,
                              QString("Dashboard number %1 has an unexpected name: expected '%2', got '%3'")
                                  .arg(i)
                                  .arg(expectedDashboardName)
                                  .arg(actualDashboardName));

                const bool expectedDashboardState = expectedDashboardsState[i].second;
                const bool actualDashboardState = actualDashboardsState[i].second;
                CHECK_SET_ERR(expectedDashboardState == actualDashboardState,
                              QString("Dashboard number %1 has an unexpected state: it should be %2, but it is '%3'")
                                  .arg(i)
                                  .arg(expectedDashboardState ? "visible" : "invisible")
                                  .arg(actualDashboardState ? "visible" : "invisible"));
            }

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(new DashboardsManagerDialogFiller(new Scenario()));
    GTWidget::click(dashboardsManagerButton);

    //    23. Switch to the first Workflow Designer.
    //    Expected result:
    //     - The Workflow Designer is in the dashboards view mode.
    //     - There are 2 dashboard tabs. Their names are "Extract consensus as sequence 2", "Align sequence with MUSCLE 2".
    //     - The "Align sequence with MUSCLE 1" dashboard is active.
    //     - The "Dashboards manager" button on the toolbar is active.
    coreLog.info(QString("Try to switch to tab %1(%2)")
                     .arg(tabIndex1)
                     .arg(GTUtilsMdi::getTabBar()->tabText(tabIndex1)));
    GTUtilsMdi::clickTab(tabIndex1);
    GTUtilsTaskTreeView::waitTaskFinished();

    viewSwitchButton = GTUtilsWorkflowDesigner::getGotoWorkflowButton();
    CHECK_SET_ERR(viewSwitchButton->isVisible(), "View switch button is unexpectedly invisible");
    CHECK_SET_ERR(viewSwitchButton->isEnabled(), "View switch button is unexpectedly disabled");

    expectedButtonText = "To Workflow Designer";
    actualButtonText = viewSwitchButton->text();
    CHECK_SET_ERR(expectedButtonText == actualButtonText,
                  QString("View switch button has an unexpected text: expected '%1', got '%2'")
                      .arg(expectedButtonText)
                      .arg(actualButtonText));

    coreLog.info("Trying get GTUtilsDashboard::getTabWidget()");
    dashboardsView = GTUtilsDashboard::getTabWidget();
    CHECK_SET_ERR(dashboardsView != nullptr, "Dashboards view is nullptr");
    coreLog.info("Successfully got GTUtilsDashboard::getTabWidget()");

    expectedTabsCount = 2;
    actualTabsCount = dashboardsView->count();
    CHECK_SET_ERR(expectedTabsCount == actualTabsCount,
                  QString("There is an incorrect count of tabs in the Dashboard View: expected %1, got %2")
                      .arg(expectedTabsCount)
                      .arg(actualTabsCount));

    expectedActiveTabIndex = 0;
    actualActiveTabIndex = dashboardsView->currentIndex();
    CHECK_SET_ERR(expectedActiveTabIndex == actualActiveTabIndex,
                  QString("There is an incorrect active tab: expected index is %1, actual index is %2")
                      .arg(expectedActiveTabIndex)
                      .arg(actualActiveTabIndex));

    expectedTabName = "Align sequences with MUSCLE 1";
    actualTabName = GTUtilsDashboard::getDashboardName(expectedActiveTabIndex);
    CHECK_SET_ERR(expectedTabName == actualTabName,
                  QString("Active dashboard has an unexpected name: expect '%1', got '%2'")
                      .arg(expectedTabName)
                      .arg(actualTabName));

    outputFiles = GTUtilsDashboard::getOutputFiles();
    CHECK_SET_ERR(!outputFiles.isEmpty(), "Active dashboard is not displayed properly");

    dashboardsManagerButton = GTToolbar::getWidgetForActionTooltip(GTToolbar::getToolbar(MWTOOLBAR_ACTIVEMDI), "Dashboards manager");
    CHECK_SET_ERR(dashboardsManagerButton != nullptr, "'Dashboards manager' is nullptr");
    CHECK_SET_ERR(dashboardsManagerButton->isEnabled(), "'Dashboards manager' button is unexpectedly disabled");

    //    24. Click to the "Dashboards manager" button on the toolbar.
    //    Expected result:
    //     - the "Dashboards Manager" dialog appears.
    //       It contains 4 items, not all of them are checked.
    //       Their names are "Extract consensus as sequence 1", "Extract consensus as sequence 2", "Align sequence with MUSCLE 1", "Align sequence with MUSCLE 2".
    //    25. Cancel the dialog
    GTUtilsDialog::waitForDialog(new DashboardsManagerDialogFiller(new Scenario()));
    GTWidget::click(dashboardsManagerButton);

    //    26. Switch to the third Workflow Designer.
    //    Expected result:
    //     - The Workflow Designer is in the scene view mode.
    //     - The "Dashboards manager" button on the toolbar is active.
    coreLog.info(QString("Try to switch to tab %1(%2)")
                     .arg(tabIndex3)
                     .arg(GTUtilsMdi::getTabBar()->tabText(tabIndex3)));
    GTUtilsMdi::clickTab(tabIndex3);

    dashboardsManagerButton = GTToolbar::getWidgetForActionTooltip(GTToolbar::getToolbar(MWTOOLBAR_ACTIVEMDI), "Dashboards manager");
    CHECK_SET_ERR(dashboardsManagerButton != nullptr, "'Dashboards manager' is nullptr");
    CHECK_SET_ERR(dashboardsManagerButton->isEnabled(), "'Dashboards manager' button is unexpectedly disabled");

    viewSwitchButton = GTUtilsWorkflowDesigner::getGotoDashboardButton();
    CHECK_SET_ERR(viewSwitchButton->isVisible(), "'Go to Dashboards' is invisible");

    //    27. Click to the "Dashboards manager" button on the toolbar.
    //    Expected result:
    //     - the "Dashboards Manager" dialog appears. It contains four items. Their names are "Extract consensus as sequence 1", "Extract consensus as sequence 2", "Align sequence with MUSCLE 1", "Align sequence with MUSCLE 2".
    //     - "Extract consensus as sequence 1" and "Extract consensus as sequence 2" are unchecked, all other are checked.
    //    28. Cancel the dialog.
    GTUtilsDialog::waitForDialog(new DashboardsManagerDialogFiller(new Scenario()));
    GTWidget::click(dashboardsManagerButton);

    //    29. Click to the "Go to Dashboards" button on the toolbar.
    //    Expected result:
    //     - There are two dashboard tabs. Their names are "Align sequence with MUSCLE 1", "Align sequence with MUSCLE 2" (warning: two last tabs can be swapped, it depends on the task finish order, it is a correct situation).
    //     - The first dashboard is active.
    viewSwitchButton = GTUtilsWorkflowDesigner::getGotoDashboardButton();
    CHECK_SET_ERR(viewSwitchButton->isVisible(), "View switch button is unexpectedly invisible");
    CHECK_SET_ERR(viewSwitchButton->isEnabled(), "View switch button is unexpectedly disabled");
    GTWidget::click(viewSwitchButton);

    coreLog.info("Trying get GTUtilsDashboard::getTabWidget()");
    dashboardsView = GTUtilsDashboard::getTabWidget();
    CHECK_SET_ERR(dashboardsView != nullptr, "Dashboards view is nullptr");
    coreLog.info("Successfully got GTUtilsDashboard::getTabWidget()");

    expectedTabsCount = 2;
    actualTabsCount = dashboardsView->count();
    CHECK_SET_ERR(expectedTabsCount == actualTabsCount,
                  QString("There is an incorrect count of tabs in the Dashboard View: expected %1, got %2")
                      .arg(expectedTabsCount)
                      .arg(actualTabsCount));

    expectedActiveTabIndex = 0;
    actualActiveTabIndex = dashboardsView->currentIndex();
    CHECK_SET_ERR(expectedActiveTabIndex == actualActiveTabIndex,
                  QString("There is an incorrect active tab: expected index is %1, actual index is %2")
                      .arg(expectedActiveTabIndex)
                      .arg(actualActiveTabIndex));

    expectedTabName = "Align sequences with MUSCLE 1";
    actualTabName = GTUtilsDashboard::getDashboardName(expectedActiveTabIndex);
    CHECK_SET_ERR(expectedTabName == actualTabName,
                  QString("Active dashboard has an unexpected name: expect '%1', got '%2'")
                      .arg(expectedTabName)
                      .arg(actualTabName));
}

}  // namespace GUITest_common_scenarios_workflow_dashboard

}  // namespace U2
