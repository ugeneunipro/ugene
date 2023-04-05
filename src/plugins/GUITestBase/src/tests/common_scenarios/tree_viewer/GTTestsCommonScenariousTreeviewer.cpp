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
#include <base_dialogs/ColorDialogFiller.h>
#include <base_dialogs/GTFileDialog.h>
#include <base_dialogs/MessageBoxFiller.h>
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTAction.h>
#include <primitives/GTCheckBox.h>
#include <primitives/GTMenu.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTWidget.h>
#include <primitives/PopupChooser.h>
#include <system/GTFile.h>
#include <QColor>
#include <QGraphicsItem>
#include <QMainWindow>
#include <QRgb>

#include <U2Core/AppContext.h>

#include <U2View/MSAEditor.h>
#include <U2View/TvNodeItem.h>
#include <U2View/TvRectangularBranchItem.h>

#include "GTTestsCommonScenariousTreeviewer.h"
#include "GTUtilsBookmarksTreeView.h"
#include "GTUtilsLog.h"
#include "GTUtilsMdi.h"
#include "GTUtilsMsaEditorSequenceArea.h"
#include "GTUtilsOptionsPanelPhyTree.h"
#include "GTUtilsPhyTree.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsTaskTreeView.h"
#include "runnables/ugene/corelibs/U2Gui/ExportImageDialogFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/BuildTreeDialogFiller.h"
#include "runnables/ugene/plugins_3rdparty/umuscle/MuscleDialogFiller.h"

namespace U2 {

namespace GUITest_common_scenarios_tree_viewer {
using namespace HI;

GUI_TEST_CLASS_DEFINITION(test_0001_1) {
    // Export tree image to file (via toolbar button).

    GTFileDialog::openFile(os, dataDir + "samples/Newick/COI.nwk");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Use "Capture tree" button on toolbar to make screenshots
    QString fileName = testDir + "_common_data/scenarios/sandbox/GUITest_common_scenarios_tree_viewer_test_0001_1.jpg";
    GTUtilsDialog::add(os, new PopupChooser(os, {"saveVisibleViewToFileAction"}));
    GTUtilsDialog::add(os, new ExportImage(os, fileName, "JPG", 50));
    GTWidget::click(os, GTAction::button(os, "treeImageActionsButtonMenuAction"));

    qint64 fileSize = GTFile::getSize(os, fileName);
    CHECK_SET_ERR(fileSize > 10000, "File is not found or is too small: " + QString::number(fileSize));
}

GUI_TEST_CLASS_DEFINITION(test_0001_2) {
    // Export tree image to file (via main menu).

    GTFileDialog::openFile(os, dataDir + "samples/Newick/COI.nwk");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Use "Capture tree" button on toolbar to make screenshots
    QString fileName = testDir + "_common_data/scenarios/sandbox/GUITest_common_scenarios_tree_viewer_test_0001_2.png";
    GTUtilsDialog::waitForDialog(os, new ExportImage(os, fileName, "PNG"));
    GTMenu::clickMainMenuItem(os, {"Actions", "Tree image", "Save visible area to file..."});

    qint64 fileSize = GTFile::getSize(os, fileName);
    CHECK_SET_ERR(fileSize > 10000, "File is not found or is too small: " + QString::number(fileSize));
}

GUI_TEST_CLASS_DEFINITION(test_0002) {
    // Rebuilding tree after removing tree file

    // 1. Open file samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Click on "Build tree" button on toolbar "Build Tree"
    // Expected state: "Create Phylogenetic Tree" dialog appears
    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, testDir + "_common_data/scenarios/sandbox/COI.nwk"));
    GTWidget::click(os, GTAction::button(os, "Build Tree"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 3. Set save path to _common_data/scenarios/sandbox/COI.nwk Click  OK button
    // Expected state: Phylogenetic tree appears
    GTWidget::findGraphicsView(os, "treeView");

    // 4. Remove document "COI.nwk" from project view.
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "COI.nwk"));
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick(Qt::Key_Delete);

    GTUtilsProjectTreeView::findIndex(os, "COI.nwk", {false});
    // Expected state: document "COI.nwk" not presents at project tree, tree editor view window closes

    // 5. Double click on COI object.
    // Expected state: MSA editor view window opens
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "COI.aln"));
    GTMouseDriver::doubleClick();

    // 6. Click on "Build tree" button on toolbar
    // Expected state: "Create Phylogenetic Tree" dialog appears
    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, testDir + "_common_data/scenarios/sandbox/COI.nwk"));
    GTWidget::click(os, GTAction::button(os, "Build Tree"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 7. Click  OK button
    // Expected state: Phylogenetic tree appears
    GTWidget::findWidget(os, "treeView");
}

GUI_TEST_CLASS_DEFINITION(test_0002_1) {
    // Rebuilding tree after removing tree file
    // 1. Open file samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Click on "Build tree" button on toolbar "Build Tree"
    // Expected state: "Create Phylogenetic Tree" dialog appears
    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, testDir + "_common_data/scenarios/sandbox/COI.nwk"));

    // DIFFERENCE: Context menu is used for building tree
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {MSAE_MENU_TREES, "Build Tree"}));

    GTMenu::showContextMenu(os, GTUtilsMdi::activeWindow(os));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 3. Set save path to _common_data/scenarios/sandbox/COI.nwk Click  OK button
    // Expected state: Phylogenetic tree appears
    GTWidget::findGraphicsView(os, "treeView");

    // 4. Remove document "COI.nwk" from project view.
    // GTUtilsDialog::waitForDialog(os,new MessageBoxDialogFiller(os,QMessageBox::No));
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "COI.nwk"));
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    auto w = GTWidget::findWidget(os, "treeView", nullptr, {false});
    CHECK_SET_ERR(w == nullptr, "treeView not deleted")

    GTUtilsProjectTreeView::findIndex(os, "COI.nwk", {false});
    // Expected state: document "COI.nwk" not presents at project tree, tree editor view window closes

    // 5. Double-click on COI object.
    // Expected state: MSA editor view window opens
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "COI.aln"));
    GTMouseDriver::doubleClick();

    // 6. Click on "Build tree" button on toolbar
    // Expected state: "Create Phylogenetic Tree" dialog appears
    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, testDir + "_common_data/scenarios/sandbox/COI.nwk"));
    // DIFFERENCE: Context menu is used for building tree
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {MSAE_MENU_TREES, "Build Tree"}));

    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTMenu::showContextMenu(os, GTUtilsMdi::activeWindow(os));

    // 7. Click  OK button
    // Expected state: phylogenetic tree appears
    GTWidget::findWidget(os, "treeView");
}

GUI_TEST_CLASS_DEFINITION(test_0002_2) {
    // Rebuilding tree after removing the tree file.
    // DIFFERENCE from 0002_1: main menu is used for building tree.
    // 1. Open file samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Click on "Build tree" button on toolbar "Build Tree"
    // Expected state: "Create Phylogenetic Tree" dialog appears
    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, testDir + "_common_data/scenarios/sandbox/test_0002_2.nwk"));
    GTMenu::clickMainMenuItem(os, {"Actions", "Tree", "Build Tree"});
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 3. Set save path to _common_data/scenarios/sandbox/COI.nwk Click OK button.
    // Expected state: phylogenetic tree appears
    auto treeView = GTWidget::findWidget(os, "treeView");

    // 4. Remove document "COI.nwk" from project view.
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "test_0002_2.nwk"));
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: document "COI.nwk" not presents at project tree, tree editor view window closes
    treeView = GTWidget::findWidget(os, "treeView", nullptr, {false});
    CHECK_SET_ERR(treeView == nullptr, "treeView not deleted")

    GTUtilsProjectTreeView::checkNoItem(os, "test_0002_2.nwk");

    // 5. Double-click on COI object.
    // Expected state: MSA editor view window opens
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "COI.aln"));
    GTMouseDriver::doubleClick();
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 6. Click on "Build tree" button on toolbar
    // Expected state: "Create Phylogenetic Tree" dialog appears
    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, testDir + "_common_data/scenarios/sandbox/test_0002_2.nwk"));
    GTMenu::clickMainMenuItem(os, {"Actions", "Tree", "Build Tree"});
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 7. Click  OK button
    // Expected state: phylogenetic tree appears.
    GTWidget::findWidget(os, "treeView");
}

GUI_TEST_CLASS_DEFINITION(test_0003) {
    //    Building tree with specific parameters
    //    1. Open file samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTLogTracer lt;

    //    2. Click on "Build tree" button on toolbar
    //    Expected state: "Create Phylogenetic Tree" dialog appears

    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, testDir + "_common_data/scenarios/sandbox/COI.nwk", 2, 52));

    GTWidget::click(os, GTAction::button(os, "Build Tree"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    3. Fill next fields in dialog:
    //    {Distance matrix model:} jukes-cantor
    //    {Gamma distributed rates across sites} [checked]
    //    {Coefficient of variation of substitution rat among sites:} 99
    //    {Path to file:} _common_data/scenarios/sandbox/COI.nwk

    //    4. Click  OK button
    CHECK_SET_ERR(lt.hasErrors(), "Expected to have errors in the log, but no errors found");
    //    Expected state: no crash, phylogenetic tree not appears
    //    Error message in the log: "Calculated weight matrix is invalid"
}

GUI_TEST_CLASS_DEFINITION(test_0004) {
    //    Disabling views
    //    1. Open file samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Click on "Build tree" button on toolbar
    //    Expected state: "Create Phylogenetic Tree" dialog appears
    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, testDir + "_common_data/scenarios/sandbox/COI.nwk"));
    QAbstractButton* tree = GTAction::button(os, "Build Tree");
    GTWidget::click(os, tree);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    3. Set save path to _common_data/scenarios/sandbox/COI.nwk Click  OK button
    //    Expected state: phylogenetic tree appears
    //    4. Disable "Show sequence name"
    //    Expected state: sequence name labels are not shown
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Show Names"}));
    GTWidget::click(os, GTWidget::findWidget(os, "Show Labels"));

    auto treeView = GTWidget::findGraphicsView(os, "treeView");
    QList<QGraphicsItem*> list = treeView->scene()->items();

    for (const QGraphicsItem* item : qAsConst(list)) {
        auto node = qgraphicsitem_cast<const QGraphicsSimpleTextItem*>(item);
        if (node != nullptr && node->isVisible()) {
            CHECK_SET_ERR(!node->text().contains("o") || !node->text().contains("a"), QString("names are visible: %1").arg(node->text()));
        }
    }
    //    5. Disable "Show distance labels".
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Show Distances"}));
    GTWidget::click(os, GTWidget::findWidget(os, "Show Labels"));

    for (const QGraphicsItem* item : qAsConst(list)) {
        auto node = qgraphicsitem_cast<const QGraphicsSimpleTextItem*>(item);
        if (node != nullptr && node->isVisible()) {
            if (node->text() != "0.011") {
                CHECK_SET_ERR(!node->text().contains("0."), "Distances are visible");
            }
        }
    }
    //    Expected state: distance labels are not shown
}

GUI_TEST_CLASS_DEFINITION(test_0005) {
    // Align with muscle, then build tree
    // 1. Open file samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Do menu {Actions->Align->Align With Muscle}
    // Expected state: "Align with muscle" dialog appears

    // 3. Click "Align" button
    GTUtilsDialog::waitForDialog(os, new MuscleDialogFiller(os));
    GTMenu::clickMainMenuItem(os, {"Actions", "Align", "Align with MUSCLE…"}, GTGlobals::UseMouse);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 4. Click on "Build tree" button on toolbar
    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, testDir + "_common_data/scenarios/sandbox/COI.nwk"));
    GTWidget::click(os, GTAction::button(os, "Build Tree"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: "Create Phylogenetic Tree" dialog appears
    // 5. Set save path to _common_data/scenarios/sandbox/COI.nwk Click  OK button
    // Expected state: phylogenetic tree appears
    GTWidget::findGraphicsView(os, "treeView");
}

// int getCoord(HI::GUITestOpStatus& os, QGraphicsSimpleTextItem* node) {
//     auto treeView = GTWidget::findGraphicsView(os, "treeView");
//     QPointF sceneCoord = node->mapToScene(node->boundingRect().bottomLeft());
//     QPoint globalCoord = treeView->mapToGlobal(sceneCoord.toPoint());
//     return globalCoord.y();
// }

GUI_TEST_CLASS_DEFINITION(test_0006) {
    //    Tree layouts test
    //    1. Open file _common_data/scenario/tree_view/COI.nwk
    //    Expected state: Phylogenetic tree appears
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/tree_view/", "COI.nwk");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Click on {Layout->Circilar layout} button on toolbar
    // GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList()<<"Circular"));
    // GTWidget::click(os,GTWidget::findWidget(os,"Layout"));
    //    Expected state: tree view type changed to circular
    QList<TvNodeItem*> list = GTUtilsPhyTree::getNodes(os);
    // QList<QGraphicsSimpleTextItem*> labelsList = GTUtilsPhyTree::getLabels(os);
    QStringList labelList = GTUtilsPhyTree::getLabelsText(os);
    // QList<QGraphicsSimpleTextItem*> distancesList = GTUtilsPhyTree::getDistances(os);
    QList<double> dList = GTUtilsPhyTree::getDistancesValues(os);

    /*QGraphicsView* treeView = qobject_cast<QGraphicsView*>(GTWidget::findWidget(os, "treeView"));
    QList<QGraphicsItem*> list = treeView->scene()->items();

    QString s;
    QGraphicsSimpleTextItem * highestNode = new QGraphicsSimpleTextItem();
    QGraphicsSimpleTextItem * lowestNode = new QGraphicsSimpleTextItem();

    foreach(QGraphicsItem* item, list){
        QGraphicsSimpleTextItem * node = qgraphicsitem_cast<QGraphicsSimpleTextItem *>(item);
        if(node && node->text().length()>5){
            if(getCoord(os, highestNode)<getCoord(os, node)){highestNode = node;}
            if(getCoord(os, lowestNode)>getCoord(os, node)){lowestNode = node;}

            //s.append("\n" + node->text() + "  " + QString().setNum(globalCoord.y()));
            //item->moveBy(200,0);
            //GTGlobals::sleep(500);
        }
    }
//    3. Click on {Layout->Unrooted layout} button on toolbar
//    Expected state: tree view type changed to unrooted
    s.append("\n" + highestNode->text() + "  " + QString().setNum(getCoord(os, highestNode)));
    s.append("\n" + lowestNode->text() + "  " + QString().setNum(getCoord(os, lowestNode)));
    os.setError(s);
//    4. Click on {Layout->Rectangular layout} button on toolbar
//    Expected state: tree view type changed to rectangular*/
}

GUI_TEST_CLASS_DEFINITION(test_0007) {
    // Labels aligniment test

    // 1. Open file _common_data/scenario/tree_view/COI.nwk
    // Expected state: Phylogenetic tree appears
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/tree_view/", "COI.nwk");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    auto treeView = GTWidget::findGraphicsView(os, "treeView");
    QList<QGraphicsItem*> list = treeView->scene()->items();

    QList<int> initPos;
    foreach (QGraphicsItem* item, list) {
        auto node = qgraphicsitem_cast<QGraphicsSimpleTextItem*>(item);
        if (node && node->boundingRect().width() > 100) {
            QPointF sceneCoord = node->mapToScene(node->boundingRect().bottomRight());
            QPoint globalCoord = treeView->mapToGlobal(sceneCoord.toPoint());
            initPos.append(globalCoord.x());
        }
    }
    // 2. Click on "Align name labels" button on toolbar
    GTMenu::clickMainMenuItem(os, {"Actions", "Align Labels"});
    // GTWidget::click(os, GTAction::button(os,"Align Labels"));

    int i = 0;
    foreach (QGraphicsItem* item, list) {
        auto node = qgraphicsitem_cast<QGraphicsSimpleTextItem*>(item);
        if (node && node->boundingRect().width() > 100 && i == 0) {
            QPointF sceneCoord = node->mapToScene(node->boundingRect().bottomRight());
            QPoint globalCoord = treeView->mapToGlobal(sceneCoord.toPoint());
            i = globalCoord.x();
        }
        if (node && node->boundingRect().width() > 100) {
            QPointF sceneCoord = node->mapToScene(node->boundingRect().bottomRight());
            QPoint globalCoord = treeView->mapToGlobal(sceneCoord.toPoint());
            CHECK_SET_ERR(i == globalCoord.x(), "elements are not aligned");
        }
    }
    // Expected state: sequence labels aligned at right side of the screen

    // 3. Click on "Align name labels" button on toolbar
    GTMenu::clickMainMenuItem(os, {"Actions", "Align Labels"});
    QList<int> finalPos;
    foreach (QGraphicsItem* item, list) {
        auto node = qgraphicsitem_cast<QGraphicsSimpleTextItem*>(item);
        if (node && node->boundingRect().width() > 100) {
            QPointF sceneCoord = node->mapToScene(node->boundingRect().bottomRight());
            QPoint globalCoord = treeView->mapToGlobal(sceneCoord.toPoint());
            finalPos.append(globalCoord.x());
        }
    }
    CHECK_SET_ERR(initPos == finalPos, "items aligned wrong");
    // Expected state: sequence label aligned near end of its branches
}

GUI_TEST_CLASS_DEFINITION(test_0008) {
    // Sequence labels test

    // 1. Open file _common_data/scenario/tree_view/COI.nwk
    // Expected state: Phylogenetic tree appears
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/tree_view/", "COI.nwk");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    // 2. Click on "Show sequence names" button on toolbar
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Show Names"}));
    GTWidget::click(os, GTWidget::findWidget(os, "Show Labels"));

    auto treeView = GTWidget::findGraphicsView(os, "treeView");
    QList<QGraphicsItem*> list = treeView->scene()->items();

    foreach (QGraphicsItem* item, list) {
        auto node = qgraphicsitem_cast<QGraphicsSimpleTextItem*>(item);
        if (node && node->isVisible()) {
            CHECK_SET_ERR(!node->text().contains("o") || !node->text().contains("a"), "names are visiable");
        }
    }
    // Expected state: sequence name labels disappers

    // 3. Click on "Show distance labels" button on toolbar
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Show Distances"}));
    GTWidget::click(os, GTWidget::findWidget(os, "Show Labels"));

    foreach (QGraphicsItem* item, list) {
        auto node = qgraphicsitem_cast<QGraphicsSimpleTextItem*>(item);
        if (node && node->isVisible()) {
            if (node->text() != "0.011") {
                CHECK_SET_ERR(!node->text().contains("0."), "Distances are visiable");
            }
        }
    }
    // Expected state: distance labels disappers

    // 4. Click on "Show sequence names" button on toolbar
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Show Names"}));
    GTWidget::click(os, GTWidget::findWidget(os, "Show Labels"));
    int i = 0;

    QString s;
    foreach (QGraphicsItem* item, list) {
        QGraphicsSimpleTextItem* node = qgraphicsitem_cast<QGraphicsSimpleTextItem*>(item);
        if (node) {
            s.append("\n" + node->text());
        }
        if (node && node->isVisible() && node->text().contains("o")) {
            i++;
        }
    }
    // CHECK_SET_ERR(i==18, "Names are not shown" + QString().setNum(i) + s);
    // Expected state: sequence name labels appers

    // 5. Click on "Show distance labels" button on toolbar
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Show Distances"}));
    GTWidget::click(os, GTWidget::findWidget(os, "Show Labels"));
    i = 0;

    QString s1;
    foreach (QGraphicsItem* item, list) {
        QGraphicsSimpleTextItem* node = qgraphicsitem_cast<QGraphicsSimpleTextItem*>(item);
        if (node && node->isVisible() && node->text().contains("0.")) {
            s1.append("\n" + node->text());
            i++;
        }
    }
    CHECK_SET_ERR(i == 31, "distances are not shown" + QString().setNum(i) + s1);
    // Expected state: distance labels appers
}

GUI_TEST_CLASS_DEFINITION(test_0008_1) {  // difference: main menu is used
    // Sequence labels test

    // 1. Open file _common_data/scenario/tree_view/COI.nwk
    // Expected state: Phylogenetic tree appears
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/tree_view/", "COI.nwk");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    // 2. Click on "Show sequence names" button on toolbar
    GTMenu::clickMainMenuItem(os, {"Actions", "Show Labels", "Show Names"}, GTGlobals::UseMouse);

    auto treeView = GTWidget::findGraphicsView(os, "treeView");
    QList<QGraphicsItem*> list = treeView->scene()->items();

    foreach (QGraphicsItem* item, list) {
        QGraphicsSimpleTextItem* node = qgraphicsitem_cast<QGraphicsSimpleTextItem*>(item);
        if (node && node->isVisible()) {
            CHECK_SET_ERR(!node->text().contains("o") || !node->text().contains("a"), "names are visiable");
        }
    }
    // Expected state: sequence name labels disappers

    // 3. Click on "Show distance labels" button on toolbar
    GTMenu::clickMainMenuItem(os, {"Actions", "Show Labels", "Show Distances"}, GTGlobals::UseMouse);

    foreach (QGraphicsItem* item, list) {
        QGraphicsSimpleTextItem* node = qgraphicsitem_cast<QGraphicsSimpleTextItem*>(item);
        if (node && node->isVisible()) {
            if (node->text() != "0.011") {
                CHECK_SET_ERR(!node->text().contains("0."), "Distances are visiable");
            }
        }
    }
    // Expected state: distance labels disappers

    // 4. Click on "Show sequence names" button on toolbar
    GTMenu::clickMainMenuItem(os, {"Actions", "Show Labels", "Show Names"}, GTGlobals::UseMouse);
    int i = 0;

    foreach (QGraphicsItem* item, list) {
        QGraphicsSimpleTextItem* node = qgraphicsitem_cast<QGraphicsSimpleTextItem*>(item);
        if (node && node->isVisible() && node->text().contains("o")) {
            i++;
        }
    }
    // CHECK_SET_ERR(i==18, "Names are not shown");
    // Expected state: sequence name labels appers

    // 5. Click on "Show distance labels" button on toolbar
    GTMenu::clickMainMenuItem(os, {"Actions", "Show Labels", "Show Distances"}, GTGlobals::UseMouse);
    i = 0;

    foreach (QGraphicsItem* item, list) {
        QGraphicsSimpleTextItem* node = qgraphicsitem_cast<QGraphicsSimpleTextItem*>(item);
        if (node && node->isVisible() && node->text().contains("0.")) {
            i++;
        }
    }
    CHECK_SET_ERR(i == 31, "distances are not shown");
    // Expected state: distance labels appers
}

GUI_TEST_CLASS_DEFINITION(test_0009) {
    // UGENE crashes when tree view bookmark is activated (0001431)

    // 1. Open Newick file (.NWK)
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/tree_view/", "COI.nwk");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    // 2. Create new bookmark for the file
    QPoint p = GTUtilsBookmarksTreeView::getItemCenter(os, "Tree [COI.nwk]");
    GTMouseDriver::moveTo(p);
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {ACTION_ADD_BOOKMARK}, GTGlobals::UseMouse));
    GTMouseDriver::click(Qt::RightButton);

    GTKeyboardDriver::keyClick('a', Qt::ControlModifier);
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTKeyboardDriver::keySequence("start bookmark");
    GTKeyboardDriver::keyClick(Qt::Key_Enter);

    GTUtilsMdi::click(os, GTGlobals::Close);

    p = GTUtilsBookmarksTreeView::getItemCenter(os, "start bookmark");
    GTMouseDriver::moveTo(p);
    GTMouseDriver::doubleClick();

    GTWidget::findWidget(os, "treeView");
    // 3. Close the opened view

    // 4. Activate bookmark
    // Expected state: UGENE not crash
}

GUI_TEST_CLASS_DEFINITION(test_0010) {
    // PhyTree branch settings

    // Open file _common_data/scenario/tree_view/COI.nwk
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/tree_view/COI.nwk");
    GTUtilsPhyTree::checkTreeViewerWindowIsActive(os);
    // Expected state: phylogenetic tree appears

    // Open Tree Settins Options Panel tab
    GTUtilsOptionPanelPhyTree::openTab(os);

    auto lineWeightSpinBox = GTWidget::findSpinBox(os, "lineWeightSpinBox");
    auto treeView = GTWidget::findGraphicsView(os, "treeView");
    QList<TvNodeItem*> nodeList = GTUtilsPhyTree::getNodes(os);
    CHECK_SET_ERR(!nodeList.isEmpty(), "nodeList is empty");

    QGraphicsItem* node = nodeList.last();
    QPointF sceneCoord = node->mapToScene(node->boundingRect().center());
    QPoint viewCord = treeView->mapFromScene(sceneCoord);
    QPoint globalCoord = treeView->mapToGlobal(viewCord);

    // Change thickness and color
    GTMouseDriver::moveTo(globalCoord);
    GTMouseDriver::click();

    GTUtilsPhyTree::setBranchColor(os, 0, 0, 255);

    // Expected state: color changed
    QString colorName = "#0000ff";
    double initPercent = GTUtilsPhyTree::getColorPercent(os, treeView, colorName);
    CHECK_SET_ERR(initPercent > 0, "color not changed");

    // Change  line Weight
    GTSpinBox::setValue(os, lineWeightSpinBox, 30);
    double finalPercent = GTUtilsPhyTree::getColorPercent(os, treeView, colorName);
    CHECK_SET_ERR(finalPercent > initPercent * 10, "branches width changed not enough");
}

GUI_TEST_CLASS_DEFINITION(test_0011) {
    //    Collapse/expand action in phylogenetic tree (0002168)

    //    1. Open file _common_data/scenario/tree_view/COI.nwk
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/tree_view/", "COI.nwk");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: Phylogenetic tree appears
    auto treeView = GTWidget::findGraphicsView(os, "treeView");
    QPoint globalCoord = GTUtilsPhyTree::getGlobalCenterCoord(os, GTUtilsPhyTree::getNodeByBranchText(os, "0.023", "0.078"));

    //    2. Do context menu {Collapse} for any node
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Collapse"}));
    GTMouseDriver::moveTo(globalCoord);
    GTMouseDriver::click();
    GTMouseDriver::click(Qt::RightButton);

    QList<QGraphicsSimpleTextItem*> branchList;
    QList<QGraphicsItem*> list = treeView->scene()->items();
    foreach (QGraphicsItem* item, list) {
        auto textItem = qgraphicsitem_cast<QGraphicsSimpleTextItem*>(item);
        if (textItem && (textItem->text().contains("0.052") || textItem->text().contains("0.045") ||
                         textItem->text().contains("bicolor") || textItem->text().contains("roeseli"))) {
            branchList.append(textItem);
        }
    }

    foreach (QGraphicsSimpleTextItem* item, branchList) {
        CHECK_SET_ERR(!item->isVisible(), item->text() + " is visible");
    }
    //    Expected state: this node's branches has dispersed

    //    3. Do context menu {Expand} for same
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Collapse"}));
    GTMouseDriver::moveTo(globalCoord);
    GTMouseDriver::click(Qt::RightButton);

    foreach (QGraphicsSimpleTextItem* item, branchList) {
        CHECK_SET_ERR(item->isVisible(), item->text() + " is not visible");
    }
    //    Expected state: this node's branches has dissapered
}

GUI_TEST_CLASS_DEFINITION(test_0011_1) {
    //    Collapse/expand action in phylogenetic tree (0002168)

    //    1. Open file _common_data/scenario/tree_view/COI.nwk
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/tree_view/", "COI.nwk");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: Phylogenetic tree appears
    auto treeView = GTWidget::findGraphicsView(os, "treeView");

    QPoint globalCoord = GTUtilsPhyTree::getGlobalCenterCoord(os, GTUtilsPhyTree::getNodeByBranchText(os, "0.023", "0.078"));

    //    2. Do context menu {Collapse} for any node
    GTMouseDriver::moveTo(globalCoord);
    GTMouseDriver::doubleClick();

    QList<QGraphicsSimpleTextItem*> branchList;
    QList<QGraphicsItem*> list = treeView->scene()->items();
    foreach (QGraphicsItem* item, list) {
        auto textItem = qgraphicsitem_cast<QGraphicsSimpleTextItem*>(item);
        if (textItem && (textItem->text().contains("0.052") || textItem->text().contains("0.045") ||
                         textItem->text().contains("bicolor") || textItem->text().contains("roeseli"))) {
            branchList.append(textItem);
        }
    }

    foreach (QGraphicsSimpleTextItem* item, branchList) {
        CHECK_SET_ERR(!item->isVisible(), item->text() + " is visible");
    }
    //    Expected state: this node's branches has dissapered

    //    3. Do context menu {Expand} for same
    GTMouseDriver::moveTo(globalCoord);
    GTMouseDriver::doubleClick();

    foreach (QGraphicsSimpleTextItem* item, branchList) {
        CHECK_SET_ERR(item->isVisible(), item->text() + " is not visible");
    }
    //    Expected state: this node's branches has dissapered
}

GUI_TEST_CLASS_DEFINITION(test_0011_2) {
    //    Collapse/expand action in phylogenetic tree (0002168)

    //    1. Open file _common_data/scenario/tree_view/COI.nwk
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/tree_view", "COI.nwk");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: phylogenetic tree appears
    auto treeView = GTWidget::findGraphicsView(os, "treeView");

    //    2. Do context menu {Collapse} for any node
    TvNodeItem* node = GTUtilsPhyTree::getNodeByBranchText(os, "0.023", "0.078");
    GTUtilsPhyTree::doubleClickNode(os, node);

    QList<QGraphicsSimpleTextItem*> branchList;
    QList<QGraphicsItem*> list = treeView->scene()->items();
    foreach (QGraphicsItem* item, list) {
        auto textItem = qgraphicsitem_cast<QGraphicsSimpleTextItem*>(item);
        if (textItem && !textItem->text().contains("0.106") && !textItem->text().contains("0.007") &&
            !textItem->text().contains("0.103") && !textItem->text().contains("0") &&
            !textItem->text().contains("Phaneroptera_falcata") && !textItem->text().contains("Isophya_altaica_EF540820") &&
            !textItem->text().isEmpty()) {
            branchList.append(textItem);
        }
    }

    foreach (QGraphicsSimpleTextItem* item, branchList) {
        CHECK_SET_ERR(!item->isVisible() || !item->text().isEmpty(), item->text() + " is visible");
    }

    //    Expected state: this node's branches has disappeared

    //    3. Do context menu {Expand} for same
    GTUtilsPhyTree::doubleClickNode(os, node);

    foreach (QGraphicsSimpleTextItem* item, branchList) {
        if (item->text() == "0.011") {
            continue;
        }
        CHECK_SET_ERR(item->isVisible(), item->text() + " is not visible");
    }
    //    Expected state: this node's branches has disparaged
}

GUI_TEST_CLASS_DEFINITION(test_0012) {
    // 1. Run Ugene.
    //    Open file _common_data/scenarios/tree_view/D120911.tre
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/tree_view/", "D120911.tre");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //   Expected state: phylogenetic tree appears
    auto treeView = GTWidget::findGraphicsView(os, "treeView");

    // 2. Make sure the tree doesn't look like a vertical line. It should have some width
    double w = 0;
    QList<QGraphicsItem*> list = treeView->scene()->items();
    foreach (QGraphicsItem* item, list) {
        if (!qgraphicsitem_cast<QGraphicsSimpleTextItem*>(item)) {
            w = qMax(w, item->boundingRect().width());
        }
    }

    CHECK_SET_ERR(w > 100, "tree seems to be too narrow");
    // 3. Choose any node and do the context menu command "Swap siblings"
    QGraphicsItem* node = GTUtilsPhyTree::getNodeByBranchText(os, "0.003", "0.022");
    QPointF sceneCoord = node->mapToScene(node->boundingRect().center() - QPoint(-2, 0));  // Hack for tree button items: they are not hoverable on the right side.
    QPoint viewCord = treeView->mapFromScene(sceneCoord);
    QPoint globalCoord = treeView->mapToGlobal(viewCord);

    GTMouseDriver::moveTo(globalCoord);
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Swap Siblings"}));
    GTMouseDriver::click(Qt::RightButton);

    qreal finalW = 0;
    list = treeView->scene()->items();
    foreach (QGraphicsItem* item, list) {
        if (!qgraphicsitem_cast<QGraphicsSimpleTextItem*>(item)) {
            finalW = qMax(finalW, item->boundingRect().width());
        }
    }
    CHECK_SET_ERR(w == finalW, "tree weights seems to be changed: before: " + QString::number(w) + ", after: " + QString::number(finalW));
    //   Expected state: again, tree should have some width
}

GUI_TEST_CLASS_DEFINITION(test_0023) {
    // 1. Open the file "data/CLUSTALW/COI.aln"
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Open the "Tree settings" tab in the options panel
    auto optionsPanelWidget = GTWidget::findWidget(os, "OP_MSA_ADD_TREE_WIDGET");
    GTWidget::click(os, optionsPanelWidget);

    auto optionsPanelContainer = GTWidget::findWidget(os, "AddTreeWidget");

    auto openButton = GTWidget::findButtonByText(os, QObject::tr("Open tree"), optionsPanelContainer);

    // 3. Press the first one
    // Expected state: the "Select files to open..." dialog has appeared
    // 4. Specify a path to the file "data/samples/Newick/COI.nwk", press the "Open" button
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, dataDir + "samples/Newick/", "COI.nwk"));
    GTWidget::click(os, openButton);

    // Expected state: tree view has appeared together with the alignment
    GTWidget::findWidget(os, "treeView");

    // 5. Close the tree view
    GTUtilsMdi::click(os, GTGlobals::Close);
    GTMouseDriver::click();

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "COI"));
    GTMouseDriver::doubleClick();

    optionsPanelWidget = GTWidget::findWidget(os, "OP_MSA_ADD_TREE_WIDGET");
    GTWidget::click(os, optionsPanelWidget);

    optionsPanelContainer = GTWidget::findWidget(os, "AddTreeWidget");

    QWidget* buildButton = GTWidget::findButtonByText(os, QObject::tr("Build tree"), optionsPanelContainer);
    CHECK_SET_ERR(nullptr != buildButton, "The \"Build Tree\" button is not found");

    // 6. On the "Tree settings" tab press the "Build tree" button
    // Expected state: the "Build Phylogenetic Tree" dialog has appeared
    // 7. Press the "Build" button
    QString outputDirPath(testDir + "_common_data/scenarios/sandbox");
    QDir outputDir(outputDirPath);
    GTUtilsDialog::waitForDialog(os,
                                 new BuildTreeDialogFiller(os, outputDir.absolutePath() + "/COI.nwk", 0, 0.0, true));
    GTWidget::click(os, buildButton);

    // Expected state: a new file with tree has been created and has appeared along with the alignment
    GTWidget::findWidget(os, "treeView");
}

GUI_TEST_CLASS_DEFINITION(test_0024) {
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, testDir + "_common_data/scenarios/sandbox/2298.nwk", 0, 0, true));
    QAbstractButton* tree = GTAction::button(os, "Build Tree");
    GTWidget::click(os, tree);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsProjectTreeView::toggleView(os);

    // Check the "Align labels" checkbox in the "Labels" section.
    GTCheckBox::setChecked(os, "alignLabelsCheck", true);
    GTThread::waitForMainThread();
    QImage imageWithAlignBefore = GTUtilsPhyTree::captureTreeImage(os);

    // Hide names.
    GTCheckBox::setChecked(os, "showNamesCheck", false);
    GTThread::waitForMainThread();
    QImage imageWithNoNames = GTUtilsPhyTree::captureTreeImage(os);
    CHECK_SET_ERR(imageWithNoNames != imageWithAlignBefore, "Error: imageWithNoNames is equal to imageWithAlignBefore");

    // Show names back: initial alignment view should be restored.
    GTCheckBox::setChecked(os, "showNamesCheck", true);
    GTThread::waitForMainThread();
    QImage imageWithAlignAfter = GTUtilsPhyTree::captureTreeImage(os);
    CHECK_SET_ERR(imageWithAlignAfter == imageWithAlignBefore, "Error: imageWithAlignAfter is not equal to imageWithAlignBefore");
}

GUI_TEST_CLASS_DEFINITION(test_0025) {
    //     for UGENE-4089
    //     1. Open or build the tree
    //     Expected state: nothing is selected, collapse, swap and re-root actions are disabled
    //     2. Select the root item
    //     Expected state: all actions are still disabled.
    //     3. Select the other node in the middle of the tree
    //     Expected state: all three actions are enabled
    //     4. Click "Collapse"
    //     Expected state: subtree is collapsed, collapse button transformed into expand button
    //     5. Select the other node
    //     Expected state: "expand" button is "collapse" button now, because the subtree of the selected node is not collapsed
    //     6. Select the collapsed node and expand the subtree
    //     Expected state: again "expand" button is available

    GTFileDialog::openFile(os, dataDir + "/samples/Newick/COI.nwk");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    auto collapseButton = GTWidget::findButtonByText(os, "Collapse");
    auto swapButton = GTWidget::findButtonByText(os, "Swap Sibling");
    auto rerootButton = GTWidget::findButtonByText(os, "Reroot");

    CHECK_SET_ERR(!collapseButton->isEnabled(), "Collapse action is unexpectedly enabled");
    CHECK_SET_ERR(!swapButton->isEnabled(), "Swap action is unexpectedly enabled");
    CHECK_SET_ERR(!rerootButton->isEnabled(), "Reroot action is unexpectedly enabled");

    TvNodeItem* rootNode = GTUtilsPhyTree::getRootNode(os);
    GTUtilsPhyTree::clickNode(os, rootNode);
    CHECK_SET_ERR(!collapseButton->isEnabled(), "Collapse action is unexpectedly enabled for root node");
    CHECK_SET_ERR(!swapButton->isEnabled(), "Swap action is unexpectedly enabled for root node");
    CHECK_SET_ERR(!rerootButton->isEnabled(), "Re-root action is unexpectedly enabled for root node");

    TvNodeItem* middleNode = GTUtilsPhyTree::getNodeByBranchText(os, "0.023", "0.078");
    GTUtilsPhyTree::clickNode(os, middleNode);
    CHECK_SET_ERR(collapseButton->isEnabled(), "Collapse action is unexpectedly disabled for middle node");
    CHECK_SET_ERR(swapButton->isEnabled(), "Swap action is unexpectedly disabled for middle node");
    CHECK_SET_ERR(rerootButton->isEnabled(), "Re-root action is unexpectedly disabled for middle node");

    GTWidget::click(os, collapseButton);
    CHECK_SET_ERR(collapseButton->text() == "Expand", "No Expand action found after collapsing middle node");

    TvNodeItem* leafNode = GTUtilsPhyTree::getNodeByBranchText(os, "0.067", "0.078");
    GTUtilsPhyTree::clickNode(os, leafNode);
    CHECK_SET_ERR(collapseButton->text() == "Collapse", "No Collapse action for leaf node");

    GTUtilsPhyTree::clickNode(os, middleNode);
    CHECK_SET_ERR(collapseButton->text() == "Expand", "No Expand action for middle node");
    GTWidget::click(os, collapseButton);
    CHECK_SET_ERR(collapseButton->text() == "Collapse", "No Collapse action after expanding middle node");
}

GUI_TEST_CLASS_DEFINITION(test_0026) {
    //    Reroot action.

    //    1. Open file "data/samples/Newick/COI.nwk".
    //    Expected state: a Phylogenetic tree appears.
    GTFileDialog::openFile(os, dataDir + "samples/Newick/COI.nwk");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Select the parent node of "Bicolorana_bicolor_EF540830" and "Roeseliana_roeseli".
    double distanceBefore = GTUtilsPhyTree::getNodeDistance(os, GTUtilsPhyTree::getNodes(os)[10]);
    CHECK_SET_ERR(distanceBefore > 0, "Wrong node selected");

    GTUtilsPhyTree::clickNode(os, GTUtilsPhyTree::getNodeByBranchText(os, "0.052", "0.045"));
    CHECK_SET_ERR(!GTUtilsPhyTree::getSelectedNodes(os).isEmpty(), "A clicked node wasn't selected");

    //    3. Do the context menu command "Reroot tree".
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, {"Reroot tree"}));
    GTMouseDriver::click(Qt::RightButton);

    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: the tree is rerooted. The selected node parent node becomes a new tree root.
    double distanceAfter = GTUtilsPhyTree::getNodeDistance(os, GTUtilsPhyTree::getNodes(os)[10]);
    CHECK_SET_ERR(distanceBefore != distanceAfter, "Distances are not changed. The tree was not rerooted?")
}

GUI_TEST_CLASS_DEFINITION(test_0027) {
    // Swap siblings action.

    // Open file "data/samples/Newick/COI.nwk".
    // Expected state: a phylogenetic tree appears.
    GTFileDialog::openFile(os, dataDir + "samples/Newick/COI.nwk");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Select the parent node of "Bicolorana_bicolor_EF540830" and "Roeseliana_roeseli".
    TvNodeItem* node = GTUtilsPhyTree::getNodeByBranchText(os, "0.052", "0.045");
    GTUtilsPhyTree::clickNode(os, node);
    QList<TvNodeItem*> selectedNodes = GTUtilsPhyTree::getSelectedNodes(os);  // 1 node & 2 tips.
    CHECK_SET_ERR(selectedNodes.length() == 3 && selectedNodes.contains(node), "A clicked node wasn't selected");

    // Other node: must not change during swap-siblings action. If it changes it won't be found.
    GTUtilsPhyTree::getNodeByBranchText(os, "0.067", "0.078");

    // Do the context menu command "Swap siblings".
    GTUtilsDialog::add(os, new PopupChooserByText(os, {"Swap Siblings"}));
    GTMouseDriver::click(Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: left & right branches swapped. If not found -> there was no swap.
    GTUtilsPhyTree::getNodeByBranchText(os, "0.045", "0.052");

    // This node must not change. If it changes it won't be found.
    GTUtilsPhyTree::getNodeByBranchText(os, "0.067", "0.078");
}

GUI_TEST_CLASS_DEFINITION(test_0028) {
    //    Swap siblings action in MSA Editor.

    //    1. Open the file "data/samples/CLUSTALW/COI.aln"
    //    Expected state: a MSAEditor appears.
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsProjectTreeView::toggleView(os);  // Let more space for the tree view.

    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, testDir + "_common_data/scenarios/sandbox/2298.nwk", 0, 0, true));
    GTWidget::click(os, GTAction::button(os, "Build Tree"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Select the parent node of "Bicolorana_bicolor_EF540830" and "Roeseliana_roeseli".
    TvNodeItem* node = GTUtilsPhyTree::getNodeByBranchText(os, "0.052", "0.045");
    GTUtilsPhyTree::clickNode(os, node);
    QList<TvNodeItem*> selectedNodes = GTUtilsPhyTree::getSelectedNodes(os);
    CHECK_SET_ERR(selectedNodes.length() == 3 && selectedNodes.contains(node), "A clicked node wasn't selected");

    // Do the context menu command "Swap siblings".
    GTUtilsDialog::add(os, new PopupChooserByText(os, {"Swap Siblings"}));
    GTMouseDriver::click(Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: left & right branches swapped. If not found -> there was no swap.
    GTUtilsPhyTree::getNodeByBranchText(os, "0.045", "0.052");
}

GUI_TEST_CLASS_DEFINITION(test_0029) {
    //    Reroot action.

    //    1. Open the file "data/samples/CLUSTALW/COI.aln"
    //    Expected state: a MSAEditor appears.
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsProjectTreeView::toggleView(os);  // Let more space for the tree view.

    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, testDir + "_common_data/scenarios/sandbox/2298.nwk", 0, 0, true));
    QAbstractButton* tree = GTAction::button(os, "Build Tree");
    GTWidget::click(os, tree);

    //    2. Select the parent node of "Bicolorana_bicolor_EF540830" and "Roeseliana_roeseli".
    GTWidget::click(os, GTUtilsPhyTree::getTreeViewerUi(os));
    QList<TvNodeItem*> nodes = GTUtilsPhyTree::getOrderedRectangularNodes(os);
    CHECK_SET_ERR(!nodes.isEmpty(), "Tree nodes are not found");
    double distanceBefore = GTUtilsPhyTree::getNodeDistance(os, nodes[10]);
    CHECK_SET_ERR(distanceBefore > 0, "Wrong node selected");

    TvNodeItem* node = GTUtilsPhyTree::getNodeByBranchText(os, "0.052", "0.045");
    GTUtilsPhyTree::clickNode(os, node);
    CHECK_SET_ERR(!GTUtilsPhyTree::getSelectedNodes(os).isEmpty(), "A clicked node wasn't selected");

    //    3. Do the context menu command "Reroot tree".
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, {"Reroot tree"}));
    GTMouseDriver::click(Qt::RightButton);

    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: the tree is rerooted. The selected node parent node becomes a new tree root.
    nodes = GTUtilsPhyTree::getOrderedRectangularNodes(os);
    CHECK_SET_ERR(!nodes.isEmpty(), "Tree nodes are not found");

    double distanceAfter = GTUtilsPhyTree::getNodeDistance(os, nodes[10]);
    CHECK_SET_ERR(distanceBefore != distanceAfter, "Distances are not changed. The tree was not rerooted?")
}

GUI_TEST_CLASS_DEFINITION(test_0030) {
    // Check zoom-in/zoom-out/reset-zoom buttons.

    GTFileDialog::openFile(os, testDir + "_common_data/newick/COXII CDS tree.newick");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    int original100Width = GTUtilsPhyTree::getSceneWidth(os);
    GTUtilsPhyTree::clickZoomFitButton(os);
    GTUtilsPhyTree::clickZoomFitButton(os);
    GTUtilsPhyTree::clickZoomFitButton(os);

    int originalFitWidth = GTUtilsPhyTree::getSceneWidth(os);

    int prevStepWidth = originalFitWidth;
    for (int i = 0; i < 3; i++) {
        GTUtilsPhyTree::clickZoomInButton(os);
        int sceneWidth = GTUtilsPhyTree::getSceneWidth(os);
        CHECK_SET_ERR(sceneWidth > prevStepWidth, "Unexpected scene width on zoom in");
        prevStepWidth = sceneWidth;
    }

    for (int i = 0; i < 5; i++) {
        GTUtilsPhyTree::clickZoomOutButton(os);
        int sceneWidth = GTUtilsPhyTree::getSceneWidth(os);
        CHECK_SET_ERR(sceneWidth < prevStepWidth, "Unexpected scene width on zoom out");
        prevStepWidth = sceneWidth;
    }

    GTUtilsPhyTree::clickZoomFitButton(os);
    GTUtilsPhyTree::clickZoomFitButton(os);
    GTUtilsPhyTree::clickZoomFitButton(os);

    int sceneWidth = GTUtilsPhyTree::getSceneWidth(os);
    CHECK_SET_ERR(sceneWidth == originalFitWidth, "Unexpected scene width on fit zoom: " + QString::number(sceneWidth) + ", expected: " + QString::number(originalFitWidth));

    GTUtilsPhyTree::clickZoom100Button(os);
    sceneWidth = GTUtilsPhyTree::getSceneWidth(os);
    CHECK_SET_ERR(sceneWidth == original100Width, "Unexpected scene width on 100 zoom: " + QString::number(sceneWidth) + ", expected: " + QString::number(original100Width));
}

GUI_TEST_CLASS_DEFINITION(test_0031) {
    // Check that show/hide node shape option works.

    GTFileDialog::openFile(os, testDir + "_common_data/newick/COXII CDS tree.newick");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsOptionPanelPhyTree::openTab(os);

    GTCheckBox::checkState(os, "showNodeShapeCheck", false);
    QImage originalImage = GTUtilsPhyTree::captureTreeImage(os);

    GTCheckBox::setChecked(os, "showNodeShapeCheck", true);
    QImage imageWithNodes = GTUtilsPhyTree::captureTreeImage(os);
    CHECK_SET_ERR(imageWithNodes != originalImage, "imageWithNodes != originalImage failed");

    GTCheckBox::setChecked(os, "showNodeShapeCheck", false);
    QImage imageWithNoNodes = GTUtilsPhyTree::captureTreeImage(os);
    CHECK_SET_ERR(imageWithNoNodes == originalImage, "imageWithNoNodes == originalImage failed");

    // Now check the same but with selected nodes.
    GTUtilsPhyTree::clickNode(os, GTUtilsPhyTree::getNodeByBranchText(os, "0.003", "0.038"));
    GTMouseDriver::moveTo(GTMouseDriver::getMousePosition() + QPoint(20, 0));  // Remove hover effect from node.
    QImage originalImageWithSelection = GTUtilsPhyTree::captureTreeImage(os);
    CHECK_SET_ERR(originalImageWithSelection != originalImage, "imageWithSelection != originalImage failed");

    GTCheckBox::setChecked(os, "showNodeShapeCheck", true);
    QImage imageWithSelectionWithNodes = GTUtilsPhyTree::captureTreeImage(os);
    CHECK_SET_ERR(imageWithSelectionWithNodes != originalImageWithSelection, "imageWithSelectionWithNodes != originalImageWithSelection failed");

    GTCheckBox::setChecked(os, "showNodeShapeCheck", false);
    QImage imageWithSelectionWithNoNodes = GTUtilsPhyTree::captureTreeImage(os);
    CHECK_SET_ERR(imageWithSelectionWithNoNodes == originalImageWithSelection, "imageWithSelectionWithNoNodes == originalImageWithSelection failed");
}

GUI_TEST_CLASS_DEFINITION(test_0032) {
    // Check that 'Show node labels' option works as expected.

    // Check that if there are no node labels in the model the option is not shown.
    GTFileDialog::openFile(os, testDir + "_common_data/newick/sample1.newick");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    auto optionPanel = GTUtilsOptionPanelPhyTree::openTab(os);
    auto showNodeLabelsCheckbox = GTWidget::findCheckBox(os, "showNodeLabelsCheck", optionPanel);
    CHECK_SET_ERR(!showNodeLabelsCheckbox->isVisible(), "showNodeLabelsCheck is visible for a tree with no labels");

    // Now check the tree with labels.
    GTFileDialog::openFile(os, testDir + "_common_data/newick/node-labels.nwk");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    optionPanel = GTUtilsOptionPanelPhyTree::openTab(os);
    showNodeLabelsCheckbox = GTWidget::findCheckBox(os, "showNodeLabelsCheck", optionPanel);
    CHECK_SET_ERR(showNodeLabelsCheckbox->isVisible(), "showNodeLabelsCheck is not visible for a tree with no labels");
    CHECK_SET_ERR(showNodeLabelsCheckbox->isEnabled(), "showNodeLabelsCheck is not enabled for a tree with no labels");

    GTCheckBox::checkState(os, showNodeLabelsCheckbox, false);
    QImage imageWithoutLabels = GTUtilsPhyTree::captureTreeImage(os);

    GTCheckBox::setChecked(os, showNodeLabelsCheckbox, true);
    QImage imageWithLabels = GTUtilsPhyTree::captureTreeImage(os);
    CHECK_SET_ERR(imageWithLabels != imageWithoutLabels, "Image with no node labels is the same with the image with node labels");

    GTCheckBox::setChecked(os, showNodeLabelsCheckbox, false);
    QImage image = GTUtilsPhyTree::captureTreeImage(os);
    CHECK_SET_ERR(image == imageWithoutLabels, "Image with no node labels does not match the original image");
}

}  // namespace GUITest_common_scenarios_tree_viewer
}  // namespace U2
