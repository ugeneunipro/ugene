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

#include <QColor>
#include <QGraphicsItem>
#include <QGraphicsView>
#include <QMainWindow>
#include <QRgb>

#include <GTGlobals.h>
#include <base_dialogs/GTFileDialog.h>
#include <base_dialogs/MessageBoxFiller.h>
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTAction.h>
#include <primitives/GTCheckBox.h>
#include <primitives/GTMenu.h>
#include <primitives/GTWidget.h>
#include <primitives/PopupChooser.h>
#include <system/GTFile.h>
#include <utils/GTKeyboardUtils.h>

#include <U2Core/AppContext.h>

#include <U2View/GraphicsButtonItem.h>
#include <U2View/GraphicsRectangularBranchItem.h>
#include <U2View/MSAEditor.h>

#include "GTTestsCommonScenariousTreeviewer.h"
#include "GTUtilsBookmarksTreeView.h"
#include "GTUtilsLog.h"
#include "GTUtilsMdi.h"
#include "GTUtilsMsaEditorSequenceArea.h"
#include "GTUtilsPhyTree.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsTaskTreeView.h"
#include "runnables/ugene/corelibs/U2Gui/ExportImageDialogFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/BranchSettingsDialogFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/BuildTreeDialogFiller.h"
#include "runnables/ugene/plugins_3rdparty/umuscle/MuscleDialogFiller.h"

namespace U2 {

namespace GUITest_common_scenarios_tree_viewer {
using namespace HI;

const int nodeWidth = 7;
GUI_TEST_CLASS_DEFINITION(test_0001){
//Screenshoting MSA editor (regression test)

//1. Open file samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(os,dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep(500);
//2. Click on "Build tree" button on toolbar
    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, testDir + "_common_data/scenarios/sandbox/COI.nwk"));

    QAbstractButton *tree= GTAction::button(os,"Build Tree");
    GTWidget::click(os,tree);
    GTGlobals::sleep(500);
//Expected state: "Create Philogenetic Tree" dialog appears

//3. Set save path to _common_data/scenarios/sandbox/COI.nwk . Click  OK button
//Expected state: philogenetic tree appears
    GTUtilsTaskTreeView::waitTaskFinished(os);

//4. Use "Capture tree" button on toolbar to make screenshots
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os,QStringList()<<"Screen Capture"));
    GTUtilsDialog::waitForDialog(os, new ExportImage(os,testDir + "_common_data/scenarios/sandbox/image.svg", "JPG",50));
    GTWidget::click(os,GTWidget::findWidget(os,"cameraMenu"));
    GTGlobals::sleep();

    GTFile::getSize(os,testDir + "_common_data/scenarios/sandbox/image.jpg");
//Expected state: images on screenshots same as on your screen
}

GUI_TEST_CLASS_DEFINITION(test_0001_1){
//Screenshoting MSA editor (regression test)

//1. Open file samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(os,dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep(500);
//2. Click on "Build tree" button on toolbar
    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, testDir + "_common_data/scenarios/sandbox/COI.nwk"));

    QAbstractButton *tree= GTAction::button(os,"Build Tree");
    GTWidget::click(os,tree);
    GTGlobals::sleep(500);
//Expected state: "Create Philogenetic Tree" dialog appears

//3. Set save path to _common_data/scenarios/sandbox/COI.nwk . Click  OK button
//Expected state: philogenetic tree appears
    GTUtilsTaskTreeView::waitTaskFinished(os);

//4. Use "Capture tree" button on toolbar to make screenshots
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os,QStringList()<<"Export Tree Image"<<"Screen Capture"));
    GTUtilsDialog::waitForDialog(os, new ExportImage(os,testDir + "_common_data/scenarios/sandbox/image.svg", "JPG",50));
    GTMenu::showContextMenu(os, GTWidget::findWidget(os,"treeView"));

    GTFile::getSize(os,testDir + "_common_data/scenarios/sandbox/image.jpg");
//Expected state: images on screenshots same as on your screen
}

GUI_TEST_CLASS_DEFINITION(test_0001_2){
//Screenshoting MSA editor (regression test)

//1. Open file samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(os,dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep(500);
//2. Click on "Build tree" button on toolbar
    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, testDir + "_common_data/scenarios/sandbox/COI.nwk"));

    QAbstractButton *tree= GTAction::button(os,"Build Tree");
    GTWidget::click(os,tree);
    GTGlobals::sleep(500);
//Expected state: "Create Philogenetic Tree" dialog appears

//3. Set save path to _common_data/scenarios/sandbox/COI.nwk . Click  OK button
//Expected state: philogenetic tree appears

//4. Use "Capture tree" button on toolbar to make screenshots
    GTUtilsDialog::waitForDialog(os, new ExportImage(os,testDir + "_common_data/scenarios/sandbox/image.svg", "JPG",50));
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions" << "Export Tree Image" << "Screen Capture...");

    GTFile::getSize(os,testDir + "_common_data/scenarios/sandbox/image.jpg");
//Expected state: images on screenshots same as on your screen
}

GUI_TEST_CLASS_DEFINITION(test_0002){
//Rebuilding tree after removing tree file

//1. Open file samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(os,dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//2. Click on "Build tree" button on toolbar "Build Tree"
//Expected state: "Create Philogenetic Tree" dialog appears
    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, testDir + "_common_data/scenarios/sandbox/COI.nwk"));
    GTWidget::click(os, GTAction::button(os, "Build Tree"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

//3. Set save path to _common_data/scenarios/sandbox/COI.nwk Click  OK button
//Expected state: philogenetic tree appears
    QGraphicsView *treeView = qobject_cast<QGraphicsView *>(GTWidget::findWidget(os, "treeView"));
    CHECK_SET_ERR(treeView != NULL, "TreeView not found")
    GTGlobals::sleep();

//4. Remove document "COI.nwk" from project view.
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os,"COI.nwk"));
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick( Qt::Key_Delete);

    GTGlobals::sleep(500);

    GTUtilsProjectTreeView::findIndex(os,"COI.nwk",GTGlobals::FindOptions(false));
//Expected state: document "COI.nwk" not presents at project tree, tree editor view window closes

//5. Double click on COI object.
//Expected state: MSA editor view window opens
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "COI.aln"));
    GTMouseDriver::doubleClick();
    GTGlobals::sleep(1000);

//6. Click on "Build tree" button on toolbar
//Expected state: "Create Philogenetic Tree" dialog appears
    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, testDir + "_common_data/scenarios/sandbox/COI.nwk"));
    GTWidget::click(os, GTAction::button(os,"Build Tree"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

//7. Click  OK button
//Expected state: philogenetic tree appears
    QWidget *w1 = GTWidget::findWidget(os, "treeView");
    CHECK_SET_ERR(w1 != NULL,"treeView not found");
}

GUI_TEST_CLASS_DEFINITION(test_0002_1){
//Rebuilding tree after removing tree file
//1. Open file samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(os,dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//2. Click on "Build tree" button on toolbar "Build Tree"
//Expected state: "Create Philogenetic Tree" dialog appears
    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, testDir + "_common_data/scenarios/sandbox/COI.nwk"));

    //DIFFERENCE: Context menu is used for building tree
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << MSAE_MENU_TREES << "Build Tree"));

    GTMenu::showContextMenu(os, GTUtilsMdi::activeWindow(os));
    GTUtilsTaskTreeView::waitTaskFinished(os);

//3. Set save path to _common_data/scenarios/sandbox/COI.nwk Click  OK button
//Expected state: philogenetic tree appears
    QGraphicsView* treeView = qobject_cast<QGraphicsView*>(GTWidget::findWidget(os, "treeView"));
    CHECK_SET_ERR(treeView != NULL,"TreeView not found")
    GTGlobals::sleep();

//4. Remove document "COI.nwk" from project view.
    //GTUtilsDialog::waitForDialog(os,new MessageBoxDialogFiller(os,QMessageBox::No));
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "COI.nwk"));
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick( Qt::Key_Delete);

    GTGlobals::sleep(500);

    QWidget* w = GTWidget::findWidget(os, "treeView", NULL, GTGlobals::FindOptions(false));
    CHECK_SET_ERR(w == 0, "treeView not deleted")

    GTUtilsProjectTreeView::findIndex(os, "COI.nwk", GTGlobals::FindOptions(false));
//Expected state: document "COI.nwk" not presents at project tree, tree editor view window closes

//5. Double click on COI object.
//Expected state: MSA editor view window opens
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "COI.aln"));
    GTGlobals::sleep(500);
    GTMouseDriver::doubleClick();

//6. Click on "Build tree" button on toolbar
//Expected state: "Create Philogenetic Tree" dialog appears
    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, testDir + "_common_data/scenarios/sandbox/COI.nwk"));
    //DIFFERENCE: Context menu is used for building tree
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << MSAE_MENU_TREES << "Build Tree"));

    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTMenu::showContextMenu(os, GTUtilsMdi::activeWindow(os));

//7. Click  OK button
//Expected state: philogenetic tree appears
    GTGlobals::sleep(500);
    QWidget* w1 = GTWidget::findWidget(os, "treeView");
    CHECK_SET_ERR(w1 != NULL,"treeView not found");
}

GUI_TEST_CLASS_DEFINITION(test_0002_2) {
//Rebuilding tree after removing tree file
//1. Open file samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(os,dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//2. Click on "Build tree" button on toolbar "Build Tree"
//Expected state: "Create Philogenetic Tree" dialog appears
    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, testDir + "_common_data/scenarios/sandbox/COI.nwk"));

    //DIFFERENCE: Main menu is used for building tree
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions" << "Tree" << "Build Tree");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//3. Set save path to _common_data/scenarios/sandbox/COI.nwk Click  OK button
//Expected state: philogenetic tree appears
    QGraphicsView *treeView = qobject_cast<QGraphicsView *>(GTWidget::findWidget(os, "treeView"));
    CHECK_SET_ERR(treeView != NULL,"TreeView not found")
    GTGlobals::sleep();

//4. Remove document "COI.nwk" from project view.
    //GTUtilsDialog::waitForDialog(os,new MessageBoxDialogFiller(os,QMessageBox::No));
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os,"COI.nwk"));
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick( Qt::Key_Delete);

    GTGlobals::sleep(500);

//Expected state: document "COI.nwk" not presents at project tree, tree editor view window closes
    QWidget *w = GTWidget::findWidget(os, "treeView", NULL, GTGlobals::FindOptions(false));
    CHECK_SET_ERR(w == NULL, "treeView not deleted")

    GTUtilsProjectTreeView::findIndex(os, "COI.nwk", GTGlobals::FindOptions(false));

//5. Double click on COI object.
//Expected state: MSA editor view window opens
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "COI.aln"));
    GTMouseDriver::doubleClick();
    GTGlobals::sleep(1000);

//6. Click on "Build tree" button on toolbar
//Expected state: "Create Philogenetic Tree" dialog appears
    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os,testDir + "_common_data/scenarios/sandbox/COI.nwk"));

    //DIFFERENCE: Main menu is used for building tree
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions" << "Tree" << "Build Tree");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//7. Click  OK button
//Expected state: philogenetic tree appears
    QWidget *w1 = GTWidget::findWidget(os, "treeView");
    CHECK_SET_ERR(w1 != NULL,"treeView not found");
}

GUI_TEST_CLASS_DEFINITION(test_0003) {
//    Building tree with specific parameters
//    1. Open file samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(os,dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTLogTracer l;

//    2. Click on "Build tree" button on toolbar
//    Expected state: "Create Philogenetic Tree" dialog appears

    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, testDir + "_common_data/scenarios/sandbox/COI.nwk",2,52));

    GTWidget::click(os, GTAction::button(os, "Build Tree"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    3. Fill next fields in dialog:
//    {Distance matrix model:} jukes-cantor
//    {Gamma distributed rates across sites} [checked]
//    {Coefficient of variation of substitution rat among sites:} 99
//    {Path to file:} _common_data/scenarios/sandbox/COI.nwk

//    4. Click  OK button
    CHECK_SET_ERR(l.hasError(), "there is no error it the log");
//    Expected state: no crash, philogenetic tree not appears
//    Error message in the log: "Calculated weight matrix is invalid"
}

GUI_TEST_CLASS_DEFINITION(test_0004){
//    Disabling views
//    1. Open file samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(os,dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep(500);
//    2. Click on "Build tree" button on toolbar
//    Expected state: "Create Philogenetic Tree" dialog appears
    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, testDir + "_common_data/scenarios/sandbox/COI.nwk"));

    QAbstractButton *tree= GTAction::button(os,"Build Tree");
    GTWidget::click(os,tree);
    GTGlobals::sleep();
//    3. Set save path to _common_data/scenarios/sandbox/COI.nwk Click  OK button
//    Expected state: philogenetic tree appears
//    4. Disable "Show sequence name"
//    Expected state: sequence name labels are not shown
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList()<<"Show Names"));
    GTWidget::click(os,GTWidget::findWidget(os,"Show Labels"));

    QGraphicsView* treeView = qobject_cast<QGraphicsView*>(GTWidget::findWidget(os, "treeView"));
    QList<QGraphicsItem*> list = treeView->scene()->items();

    foreach(QGraphicsItem* item, list){
        QGraphicsSimpleTextItem * node = qgraphicsitem_cast<QGraphicsSimpleTextItem *>(item);
        if(node && node->isVisible()){
            CHECK_SET_ERR(!node->text().contains("o")||!node->text().contains("a"), "names are visiable");
        }
    }
//    5. Disable "Show distance labels"
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList()<<"Show Distances"));
    GTWidget::click(os,GTWidget::findWidget(os,"Show Labels"));

    foreach(QGraphicsItem* item, list){
            QGraphicsSimpleTextItem * node = qgraphicsitem_cast<QGraphicsSimpleTextItem *>(item);
            if(node && node->isVisible()){
                if(node->text()!="0.011"){
                    CHECK_SET_ERR(!node->text().contains("0."), "Distances are visiable");
                }
            }
        }
//    Expected state: distance labels are not shown
}

GUI_TEST_CLASS_DEFINITION(test_0005) {
//Align with muscle, then build tree
//1. Open file samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(os,dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//2. Do menu {Actions->Align->Align With Muscle}
//Expected state: "Align with muscle" dialog appears

//3. Click "Align" button
    GTUtilsDialog::waitForDialog(os, new MuscleDialogFiller(os));
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions" << "Align" << "Align with MUSCLE...", GTGlobals::UseMouse);
    GTUtilsTaskTreeView::waitTaskFinished(os);

//4. Click on "Build tree" button on toolbar
    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, testDir + "_common_data/scenarios/sandbox/COI.nwk"));
    GTWidget::click(os, GTAction::button(os,"Build Tree"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

//Expected state: "Create Philogenetic Tree" dialog appears
//5. Set save path to _common_data/scenarios/sandbox/COI.nwk Click  OK button
    QGraphicsView* treeView = qobject_cast<QGraphicsView*>(GTWidget::findWidget(os, "treeView"));
    CHECK_SET_ERR(treeView != NULL,"TreeView not found");
//Expected state: philogenetic tree appears
}

int getCoord(HI::GUITestOpStatus &os, QGraphicsSimpleTextItem *node){
    QGraphicsView* treeView = qobject_cast<QGraphicsView*>(GTWidget::findWidget(os, "treeView"));
    QPointF sceneCoord = node->mapToScene(node->boundingRect().bottomLeft());
    QPoint globalCoord = treeView->mapToGlobal(sceneCoord.toPoint());
    return globalCoord.y();
}
GUI_TEST_CLASS_DEFINITION(test_0006){
//    Tree layouts test
//    1. Open file _common_data/scenario/tree_view/COI.nwk
//    Expected state: philogenetic tree appears
    GTFileDialog::openFile(os,testDir + "_common_data/scenarios/tree_view/", "COI.nwk");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep(500);
//    2. Click on {Layout->Circilar layout} button on toolbar
    //GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList()<<"Circular"));
    //GTWidget::click(os,GTWidget::findWidget(os,"Layout"));
//    Expected state: tree view type changed to circular
    QList<QGraphicsItem*> list = GTUtilsPhyTree::getNodes(os);
    //QList<QGraphicsSimpleTextItem*> labelsList = GTUtilsPhyTree::getLabels(os);
    QStringList labelList = GTUtilsPhyTree::getLabelsText(os);
    //QList<QGraphicsSimpleTextItem*> distancesList = GTUtilsPhyTree::getDistances(os);
    QList<double> dList = GTUtilsPhyTree::getDistancesValues(os);
    GTGlobals::sleep(500);


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

GUI_TEST_CLASS_DEFINITION(test_0007){
//Labels aligniment test

//1. Open file _common_data/scenario/tree_view/COI.nwk
//Expected state: philogenetic tree appears
    GTFileDialog::openFile(os,testDir + "_common_data/scenarios/tree_view/", "COI.nwk");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep(500);

    QGraphicsView* treeView = qobject_cast<QGraphicsView*>(GTWidget::findWidget(os, "treeView"));
    QList<QGraphicsItem*> list = treeView->scene()->items();

    QList<int> initPos;
    foreach(QGraphicsItem* item, list){
        QGraphicsSimpleTextItem * node = qgraphicsitem_cast<QGraphicsSimpleTextItem *>(item);
        if(node && node->boundingRect().width()>100){
            QPointF sceneCoord = node->mapToScene(node->boundingRect().bottomRight());
            QPoint globalCoord = treeView->mapToGlobal(sceneCoord.toPoint());
            initPos.append(globalCoord.x());
        }
    }
//2. Click on "Align name labels" button on toolbar
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions" << "Align Labels");
    GTGlobals::sleep(500);
    //GTWidget::click(os, GTAction::button(os,"Align Labels"));

    int i = 0;
    foreach(QGraphicsItem* item, list){
        QGraphicsSimpleTextItem * node = qgraphicsitem_cast<QGraphicsSimpleTextItem *>(item);
        if(node && node->boundingRect().width()>100 && i==0){
            QPointF sceneCoord = node->mapToScene(node->boundingRect().bottomRight());
            QPoint globalCoord = treeView->mapToGlobal(sceneCoord.toPoint());
            i=globalCoord.x();
        }
        if(node && node->boundingRect().width()>100){
            QPointF sceneCoord = node->mapToScene(node->boundingRect().bottomRight());
            QPoint globalCoord = treeView->mapToGlobal(sceneCoord.toPoint());
            CHECK_SET_ERR(i == globalCoord.x(), "elements are not aligned");
        }
    }
//Expected state: sequence labels aligned at right side of the screen

//3. Click on "Align name labels" button on toolbar
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions" << "Align Labels");
    GTGlobals::sleep(500);

    QList<int> finalPos;
    foreach(QGraphicsItem* item, list){
        QGraphicsSimpleTextItem * node = qgraphicsitem_cast<QGraphicsSimpleTextItem *>(item);
        if(node && node->boundingRect().width()>100){
            QPointF sceneCoord = node->mapToScene(node->boundingRect().bottomRight());
            QPoint globalCoord = treeView->mapToGlobal(sceneCoord.toPoint());
            finalPos.append(globalCoord.x());
        }
    }
    CHECK_SET_ERR(initPos==finalPos, "items aligned wrong");
//Expected state: sequence label aligned near end of its branches
}

GUI_TEST_CLASS_DEFINITION(test_0008){
//Sequence labels test

//1. Open file _common_data/scenario/tree_view/COI.nwk
//Expected state: philogenetic tree appears
    GTFileDialog::openFile(os,testDir + "_common_data/scenarios/tree_view/", "COI.nwk");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep(500);
//2. Click on "Show sequence names" button on toolbar
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList()<<"Show Names"));
    GTWidget::click(os,GTWidget::findWidget(os,"Show Labels"));

    QGraphicsView* treeView = qobject_cast<QGraphicsView*>(GTWidget::findWidget(os, "treeView"));
    QList<QGraphicsItem*> list = treeView->scene()->items();

    foreach(QGraphicsItem* item, list){
        QGraphicsSimpleTextItem * node = qgraphicsitem_cast<QGraphicsSimpleTextItem *>(item);
        if(node && node->isVisible()){
            CHECK_SET_ERR(!node->text().contains("o")||!node->text().contains("a"), "names are visiable");
        }
    }
//Expected state: sequence name labels disappers

//3. Click on "Show distance labels" button on toolbar
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList()<<"Show Distances"));
    GTWidget::click(os,GTWidget::findWidget(os,"Show Labels"));

    foreach(QGraphicsItem* item, list){
            QGraphicsSimpleTextItem * node = qgraphicsitem_cast<QGraphicsSimpleTextItem *>(item);
            if(node && node->isVisible()){
                if(node->text()!="0.011"){
                    CHECK_SET_ERR(!node->text().contains("0."), "Distances are visiable");
                }
            }
        }
//Expected state: distance labels disappers

//4. Click on "Show sequence names" button on toolbar
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList()<<"Show Names"));
    GTWidget::click(os,GTWidget::findWidget(os,"Show Labels"));
    GTGlobals::sleep(200);
    int i=0;

    QString s;
    foreach(QGraphicsItem* item, list){
        QGraphicsSimpleTextItem * node = qgraphicsitem_cast<QGraphicsSimpleTextItem *>(item);
        if(node){
            s.append("\n" + node->text());
        }
        if(node && node->isVisible()&&node->text().contains("o")){
            i++;
        }
    }
    //CHECK_SET_ERR(i==18, "Names are not shown" + QString().setNum(i) + s);
//Expected state: sequence name labels appers

//5. Click on "Show distance labels" button on toolbar
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList()<<"Show Distances"));
    GTWidget::click(os,GTWidget::findWidget(os,"Show Labels"));
    GTGlobals::sleep(200);
    i=0;

    QString s1;
    foreach(QGraphicsItem* item, list){
            QGraphicsSimpleTextItem * node = qgraphicsitem_cast<QGraphicsSimpleTextItem *>(item);
            if(node && node->isVisible() && node->text().contains("0.")){
                s1.append("\n" + node->text());
                i++;
            }
        }
    CHECK_SET_ERR(i==31, "distances are not shown" + QString().setNum(i) + s1);
//Expected state: distance labels appers
}

GUI_TEST_CLASS_DEFINITION(test_0008_1){//difference: main menu is used
//Sequence labels test

//1. Open file _common_data/scenario/tree_view/COI.nwk
//Expected state: philogenetic tree appears
    GTFileDialog::openFile(os,testDir + "_common_data/scenarios/tree_view/", "COI.nwk");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep(500);
//2. Click on "Show sequence names" button on toolbar
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions" << "Show Labels" << "Show Names", GTGlobals::UseMouse);

    QGraphicsView* treeView = qobject_cast<QGraphicsView*>(GTWidget::findWidget(os, "treeView"));
    QList<QGraphicsItem*> list = treeView->scene()->items();

    foreach(QGraphicsItem* item, list){
        QGraphicsSimpleTextItem * node = qgraphicsitem_cast<QGraphicsSimpleTextItem *>(item);
        if(node && node->isVisible()){
            CHECK_SET_ERR(!node->text().contains("o")||!node->text().contains("a"), "names are visiable");
        }
    }
//Expected state: sequence name labels disappers

//3. Click on "Show distance labels" button on toolbar
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions" << "Show Labels" << "Show Distances", GTGlobals::UseMouse);

    foreach(QGraphicsItem* item, list){
            QGraphicsSimpleTextItem * node = qgraphicsitem_cast<QGraphicsSimpleTextItem *>(item);
            if(node && node->isVisible()){
                if(node->text()!="0.011"){
                    CHECK_SET_ERR(!node->text().contains("0."), "Distances are visiable");
                }
            }
        }
//Expected state: distance labels disappers

//4. Click on "Show sequence names" button on toolbar
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions" << "Show Labels" << "Show Names", GTGlobals::UseMouse);
    GTGlobals::sleep(200);
    int i=0;

    foreach(QGraphicsItem* item, list){
        QGraphicsSimpleTextItem * node = qgraphicsitem_cast<QGraphicsSimpleTextItem *>(item);
        if(node && node->isVisible()&&node->text().contains("o")){
            i++;
        }
    }
    //CHECK_SET_ERR(i==18, "Names are not shown");
//Expected state: sequence name labels appers

//5. Click on "Show distance labels" button on toolbar
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions" << "Show Labels" << "Show Distances", GTGlobals::UseMouse);
    GTGlobals::sleep(200);
    i=0;

    foreach(QGraphicsItem* item, list){
            QGraphicsSimpleTextItem * node = qgraphicsitem_cast<QGraphicsSimpleTextItem *>(item);
            if(node && node->isVisible() && node->text().contains("0.")){
                i++;
            }
        }
    CHECK_SET_ERR(i==31, "distances are not shown");
//Expected state: distance labels appers
}

GUI_TEST_CLASS_DEFINITION(test_0009){
//UGENE crashes when tree view bookmark is activated (0001431)

//1. Open Newick file (.NWK)
    GTFileDialog::openFile(os,testDir + "_common_data/scenarios/tree_view/", "COI.nwk");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep(500);
//2. Create new bookmark for the file
    QPoint p = GTUtilsBookmarksTreeView::getItemCenter(os, "COI [tr] Tree");
    GTMouseDriver::moveTo(p);
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ACTION_ADD_BOOKMARK, GTGlobals::UseMouse));
    GTMouseDriver::click(Qt::RightButton);
    GTGlobals::sleep(500);

    GTKeyboardDriver::keyClick( 'a', Qt::ControlModifier);
    GTGlobals::sleep(500);
    GTKeyboardDriver::keyClick( Qt::Key_Delete);
    GTGlobals::sleep(500);
    GTKeyboardDriver::keySequence("start bookmark");
    GTGlobals::sleep(500);
    GTKeyboardDriver::keyClick( Qt::Key_Enter);
    GTGlobals::sleep(500);

    GTUtilsMdi::click(os, GTGlobals::Close);

    p = GTUtilsBookmarksTreeView::getItemCenter(os, "start bookmark");
    GTMouseDriver::moveTo(p);
    GTMouseDriver::doubleClick();
    GTGlobals::sleep(500);

    QWidget* treeView = GTWidget::findWidget(os,"treeView");
    CHECK_SET_ERR(treeView!=NULL, "treeView not found");
//3. Close the opened view

//4. Activate bookmark
//Expected state: UGENE not crash
}

GUI_TEST_CLASS_DEFINITION(test_0010) {
//PhyTree branch settings

//1. Open file _common_data/scenario/tree_view/COI.nwk
    GTFileDialog::openFile(os,testDir + "_common_data/scenarios/tree_view/COI.nwk");
    GTUtilsTaskTreeView::waitTaskFinished(os);
//Expected state: phylogenetic tree appears

//2. Open context menu on branch and  select {change settings} menu item
    QGraphicsView *treeView = qobject_cast<QGraphicsView *>(GTWidget::findWidget(os, "treeView"));
    QList<QGraphicsItem *> list = treeView->scene()->items();
    QList<GraphicsButtonItem *> nodeList;

    foreach (QGraphicsItem *item, list) {
        GraphicsButtonItem *buttonItem = dynamic_cast<GraphicsButtonItem *>(item);
        if (NULL != buttonItem){
            nodeList.append(buttonItem);
        }
    }

    QGraphicsItem *node = nodeList.last();
    QPointF sceneCoord = node->mapToScene(node->boundingRect().center());
    QPoint viewCord = treeView->mapFromScene(sceneCoord);
    QPoint globalCoord = treeView->mapToGlobal(viewCord);

    //Expected state: Branch settings dialog appears

    //3. Change thickness and collor to differ than standard. Click OK
    //Expected state: selected branch changed
    GTUtilsDialog::waitForDialog(os, new BranchSettingsDialogFiller(os));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "Branch Settings"));
    GTMouseDriver::moveTo(globalCoord);
    GTMouseDriver::click();
    GTMouseDriver::click(Qt::RightButton);

    globalCoord.setX(globalCoord.x() - 10);

    const QColor color = GTWidget::getColor(os, treeView, treeView->mapFromGlobal(globalCoord));
    CHECK_SET_ERR(color.name()=="#0000ff","Expected: #0000ff, found: " + color.name());
}

GUI_TEST_CLASS_DEFINITION(test_0011) {
//    Collapse/expand action in phylogenetic tree (0002168)

//    1. Open file _common_data/scenario/tree_view/COI.nwk
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/tree_view/", "COI.nwk");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep(500);

//    Expected state: philogenetic tree appears
    QGraphicsView *treeView = qobject_cast<QGraphicsView *>(GTWidget::findWidget(os, "treeView"));
    QList<QGraphicsItem *> list = treeView->scene()->items();
    QList<QGraphicsItem *> nodeList;

    foreach (QGraphicsItem *item, list) {
        uiLog.trace(QString("element width = %1").arg(item->boundingRect().width()));
        if (qRound(item->boundingRect().width()) == nodeWidth) {
            nodeList.append(item);
        }
    }

    CHECK_SET_ERR(!nodeList.isEmpty(), "nodeList is empty");
    QPoint globalCoord = GTUtilsPhyTree::getGlobalCoord(os, nodeList.last());

//    2. Do context menu {Collapse} for any node
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "Collapse"));
    GTMouseDriver::moveTo(globalCoord);
    GTMouseDriver::click();
    GTMouseDriver::click(Qt::RightButton);

    GTGlobals::sleep(500);

    QList<QGraphicsSimpleTextItem *> branchList;
    foreach (QGraphicsItem *item, list) {
            QGraphicsSimpleTextItem *textItem = qgraphicsitem_cast<QGraphicsSimpleTextItem *>(item);
            if (textItem && (textItem->text().contains("0.052") || textItem->text().contains("0.045") ||
                             textItem->text().contains("bicolor") || textItem->text().contains("roeseli"))) {
                branchList.append(textItem);
            }
        }

    foreach (QGraphicsSimpleTextItem *item, branchList) {
        CHECK_SET_ERR(!item->isVisible(), item->text() + " is visible");
    }
//    Expected state: this node's branches has dissapered

//    3. Do context menu {Expand} for same
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "Collapse"));
    GTMouseDriver::moveTo(globalCoord);
    GTMouseDriver::click(Qt::RightButton);

    foreach (QGraphicsSimpleTextItem *item, branchList) {
        CHECK_SET_ERR(item->isVisible(), item->text() + " is not visible");
    }
//    Expected state: this node's branches has dissapered
}

GUI_TEST_CLASS_DEFINITION(test_0011_1) {
//    Collapse/expand action in phylogenetic tree (0002168)

//    1. Open file _common_data/scenario/tree_view/COI.nwk
    GTFileDialog::openFile(os,testDir + "_common_data/scenarios/tree_view/", "COI.nwk");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep(500);

//    Expected state: philogenetic tree appears
    QGraphicsView *treeView = qobject_cast<QGraphicsView *>(GTWidget::findWidget(os, "treeView"));
    QList<QGraphicsItem *> list = treeView->scene()->items();
    QList<QGraphicsItem *> nodeList;

    foreach (QGraphicsItem *item, list) {
        if (qRound(item->boundingRect().width()) == nodeWidth) {
            nodeList.append(item);
        }
    }

    CHECK_SET_ERR(!nodeList.isEmpty(), "nodeList is empty");
    QPoint globalCoord = GTUtilsPhyTree::getGlobalCoord(os, nodeList.last());

//    2. Do context menu {Collapse} for any node
    GTMouseDriver::moveTo(globalCoord);
    GTMouseDriver::doubleClick();
    GTGlobals::sleep();

    QList<QGraphicsSimpleTextItem *> branchList;
    foreach (QGraphicsItem *item, list) {
            QGraphicsSimpleTextItem *textItem = qgraphicsitem_cast<QGraphicsSimpleTextItem *>(item);
            if (textItem && (textItem->text().contains("0.052") || textItem->text().contains("0.045") ||
                            textItem->text().contains("bicolor") || textItem->text().contains("roeseli"))) {
                branchList.append(textItem);
            }
        }

    foreach (QGraphicsSimpleTextItem *item, branchList) {
        CHECK_SET_ERR(!item->isVisible(), item->text() + " is visible");
    }
//    Expected state: this node's branches has dissapered

//    3. Do context menu {Expand} for same
    GTMouseDriver::moveTo(globalCoord);
    GTGlobals::sleep(1000);
    GTMouseDriver::doubleClick();
    GTGlobals::sleep(1000);

    foreach (QGraphicsSimpleTextItem *item, branchList) {
        CHECK_SET_ERR(item->isVisible(), item->text() + " is not visible");
    }
//    Expected state: this node's branches has dissapered
}

GUI_TEST_CLASS_DEFINITION(test_0011_2) {
//    Collapse/expand action in phylogenetic tree (0002168)

//    1. Open file _common_data/scenario/tree_view/COI.nwk
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/tree_view", "COI.nwk");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep(500);

//    Expected state: philogenetic tree appears
    QGraphicsView *treeView = qobject_cast<QGraphicsView *>(GTWidget::findWidget(os, "treeView"));
    QList<QGraphicsItem *> list = treeView->scene()->items();
    QList<QGraphicsItem *> nodeList;

    foreach (QGraphicsItem * item, list) {
        if (qRound(item->boundingRect().width()) == nodeWidth) {
            nodeList.append(item);
        }
    }

    CHECK_SET_ERR(!nodeList.isEmpty(), "nodeList is empty");
    QGraphicsItem *node = nodeList.at(1);
    QPoint globalCoord = GTUtilsPhyTree::getGlobalCoord(os, node);

//    2. Do context menu {Collapse} for any node
    GTMouseDriver::moveTo(globalCoord);
    GTMouseDriver::doubleClick();

    QList<QGraphicsSimpleTextItem *> branchList;
    foreach (QGraphicsItem* item, list) {
            QGraphicsSimpleTextItem *textItem = qgraphicsitem_cast<QGraphicsSimpleTextItem *>(item);
            if (textItem && !textItem->text().contains("0.106") && !textItem->text().contains("0.007") &&
                    !textItem->text().contains("0.103") && !textItem->text().contains("0") &&
                    !textItem->text().contains("Phaneroptera_falcata") && !textItem->text().contains("Isophya_altaica_EF540820")) {
                branchList.append(textItem);
            }
        }

    foreach (QGraphicsSimpleTextItem *item, branchList) {
        CHECK_SET_ERR(!item->isVisible(), item->text() + " is visible");
    }

//    Expected state: this node's branches has dissapered
    globalCoord = GTUtilsPhyTree::getGlobalCoord(os, node);

//    3. Do context menu {Expand} for same
    GTMouseDriver::moveTo(globalCoord);
    GTGlobals::sleep();
    GTMouseDriver::doubleClick();
    GTGlobals::sleep();

    foreach (QGraphicsSimpleTextItem *item, branchList) {
        if (item->text() == "0.011") {
            continue;
        }
        CHECK_SET_ERR(item->isVisible(), item->text() + " is not visible");
    }
//    Expected state: this node's branches has dissapered
}

GUI_TEST_CLASS_DEFINITION(test_0012){
//1. Run Ugene.
//   Open file _common_data/scenarios/tree_view/D120911.tre
    GTFileDialog::openFile(os,testDir + "_common_data/scenarios/tree_view/", "D120911.tre");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep(500);
//   Expected state: philogenetic tree appears
    QGraphicsView* treeView = qobject_cast<QGraphicsView*>(GTWidget::findWidget(os, "treeView"));
    QList<QGraphicsItem*> list = treeView->scene()->items();

    QList<QGraphicsSimpleTextItem *> branchList;
    foreach(QGraphicsItem* item, list){
            QGraphicsSimpleTextItem * textItem = qgraphicsitem_cast<QGraphicsSimpleTextItem *>(item);
            if(textItem && !textItem->text().contains("0.011")){
                branchList.append(textItem);
            }
        }
//2. Make sure the tree doesn't look like a vertical line. It should have some width
    QList<QGraphicsItem *> lineList;
    foreach(QGraphicsItem* item, list){
            QGraphicsSimpleTextItem * textItem = qgraphicsitem_cast<QGraphicsSimpleTextItem *>(item);
            if(!textItem){
                lineList.append(item);
            }
        }

    qreal w=0;
    foreach(QGraphicsItem * item, lineList){
        if(w<item->boundingRect().width()){
            w = item->boundingRect().width();
        }
    }

    CHECK_SET_ERR(w>100, "tree seems to be too narrow");
//3. Choose any node and do the context menu command "Swap siblings"
    QList<QGraphicsItem*> nodeList;
    foreach(QGraphicsItem* item, list){
        if(item->boundingRect().width()==nodeWidth){
            nodeList.append(item);
        }
    }

    CHECK_SET_ERR(!nodeList.isEmpty(), "nodeList is empty");
    QGraphicsItem* node = nodeList.at(1);
    QPointF sceneCoord = node->mapToScene(node->boundingRect().center());
    QPoint viewCord = treeView->mapFromScene(sceneCoord);
    QPoint globalCoord = treeView->mapToGlobal(viewCord);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList()<<"Swap Siblings"));
    GTMouseDriver::moveTo(globalCoord);
    GTMouseDriver::click();
    GTMouseDriver::click(Qt::RightButton);

    qreal finalW=0;
    foreach(QGraphicsItem * item, lineList){
        if(finalW<item->boundingRect().width()){
            finalW = item->boundingRect().width();
        }
    }
    CHECK_SET_ERR(w == finalW, "tree weights seems to be changed");
//   Expected state: again, tree should have some width

}

GUI_TEST_CLASS_DEFINITION( test_0023 ) {
    // 1. Open the file "data/CLUSTALW/COI.aln"
    GTFileDialog::openFile( os, dataDir + "samples/CLUSTALW/", "COI.aln" );
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep( 1000 );

    // 2. Open the "Tree settings" tab in the options panel
    QWidget *optionsPanelWidget = GTWidget::findWidget( os, "OP_MSA_ADD_TREE_WIDGET" );
    GTWidget::click( os, optionsPanelWidget );
    GTGlobals::sleep( 1000 );

    QWidget *optionsPanelContainer = GTWidget::findWidget( os, "AddTreeWidget" );

    QWidget *openButton = GTWidget::findButtonByText( os, QObject::tr( "Open tree" ), optionsPanelContainer );
    CHECK_SET_ERR( NULL != openButton, "The \"Open Tree\" button is not found" );

    // 3. Press the first one
    // Expected state: the "Select files to open..." dialog has appeared
    // 4. Specify a path to the file "data/samples/Newick/COI.nwk", press the "Open" button
    GTFileDialogUtils *ob = new GTFileDialogUtils( os, dataDir + "samples/Newick/", "COI.nwk" );
    GTUtilsDialog::waitForDialog( os, ob );
    GTWidget::click( os, openButton );
    GTGlobals::sleep( 2000 );

    // Expected state: tree view has appeared together with the alignment
    QWidget *treeView = qobject_cast<QWidget *>( GTWidget::findWidget( os, "treeView" ) );
    CHECK_SET_ERR( NULL != treeView, "Unable to find tree view" );

    // 5. Close the tree view
    GTUtilsMdi::click( os, GTGlobals::Close );
    GTMouseDriver::click();

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter( os, "COI" ) );
    GTMouseDriver::doubleClick();
    GTGlobals::sleep( 1000 );

    optionsPanelWidget = GTWidget::findWidget( os, "OP_MSA_ADD_TREE_WIDGET" );
    GTWidget::click( os, optionsPanelWidget );

    optionsPanelContainer = GTWidget::findWidget( os, "AddTreeWidget" );

    QWidget *buildButton = GTWidget::findButtonByText( os, QObject::tr( "Build tree" ),
        optionsPanelContainer );
    CHECK_SET_ERR( NULL != buildButton, "The \"Build Tree\" button is not found" );

    // 6. On the "Tree settings" tab press the "Build tree" button
    // Expected state: the "Build Phylogenetic Tree" dialog has appeared
    // 7. Press the "Build" button
    QString outputDirPath( testDir + "_common_data/scenarios/sandbox" );
    QDir outputDir( outputDirPath );
    GTUtilsDialog::waitForDialog( os,
        new BuildTreeDialogFiller( os, outputDir.absolutePath( ) + "/COI.nwk", 0, 0.0, true ) );
    GTWidget::click( os, buildButton );
    GTGlobals::sleep( 500 );

    // Expected state: a new file with tree has been created and has appeared along with the alignment
    treeView = qobject_cast<QWidget *>( GTWidget::findWidget( os, "treeView" ) );
    CHECK_SET_ERR( NULL != treeView, "Unable to find tree view" );
}

GUI_TEST_CLASS_DEFINITION( test_0024 ) {
    //1. Open "data/samples/CLUSTALW/COI.aln"
    GTFileDialog::openFile( os, dataDir + "samples/CLUSTALW/", "COI.aln" );
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep( 500 );

    //2. Use context menu items { Tree -> Build tree }
    //Expected: the "Build Phylogenetic Tree" dialog has appeared
    //3. Press the "Build" button in the dialog
    GTUtilsDialog::waitForDialog( os,
        new PopupChooser( os, QStringList( ) << MSAE_MENU_TREES << "Build Tree" ) );

    QString outputDirPath( testDir + "_common_data/scenarios/sandbox" );
    QDir outputDir( outputDirPath );
    GTUtilsDialog::waitForDialog( os,
        new BuildTreeDialogFiller( os, outputDir.absolutePath( ) + "/COI.nwk", 0, 0.0, true ) );

    GTMenu::showContextMenu( os, GTUtilsMdi::activeWindow( os ) );
    GTGlobals::sleep( 500 );

    QGraphicsView *treeView = qobject_cast<QGraphicsView *>( GTWidget::findWidget( os, "treeView" ) );
    CHECK_SET_ERR( NULL != treeView, "Unable to find tree view" );

    //4. Open the "Tree Setting" option panel tab
    // it does automatically
    GTUtilsProjectTreeView::openView(os);
    GTUtilsProjectTreeView::toggleView(os);
    MsaEditorWgt* ui = AppContext::getMainWindow()->getQMainWindow()->findChild<MsaEditorWgt*>();
    QSplitter* splitter = ui->findChild<QSplitter*>();
    splitter->setSizes(QList<int>()<<100<<0<<0);
    GTGlobals::sleep(500);
    //5. Check the "Align labels" checkbox in the "Labels" section
    QCheckBox *alignLabelsButton = dynamic_cast<QCheckBox *>(
        GTWidget::findWidget( os, "alignLabelsCheck" ) );
    CHECK_SET_ERR( NULL != alignLabelsButton, "The \"Align labels\" button is not found" );
    GTCheckBox::setChecked(  os, alignLabelsButton, true );

    const QList<QGraphicsItem *> treeViewItems = treeView->items( );
    QMap<const QGraphicsItem *, QRectF> initialLocations;
    foreach ( const QGraphicsItem *item, treeViewItems ) {
        initialLocations[item] = item->boundingRect( );
    }
    // GTWidget::getAllWidgetsInfo(os);

    //GTGlobals::sleep(2000);
    //6. Check the "Show names" checkbox twice
    QCheckBox *showNamesButton = dynamic_cast<QCheckBox *>(
        GTWidget::findWidget( os, "showNamesCheck" ) );
    CHECK_SET_ERR( NULL != showNamesButton, "The \"Show names\" button is not found" );
    GTCheckBox::setChecked(  os, showNamesButton, false );
    GTGlobals::sleep( 500 );

    GTCheckBox::setChecked(  os, showNamesButton, true );
    GTGlobals::sleep( 500 );

    //Expected state: labels on the tree view have kept their location
    foreach ( const QGraphicsItem *item, treeViewItems ) {
        CHECK_SET_ERR( initialLocations[item] == item->boundingRect( ),
            "Graphics item's position has changed!" );
    }
}

GUI_TEST_CLASS_DEFINITION( test_0025 ) {
//     for UGENE-4089
//     1. Open or build the tree
//     Expected state: nothing is selected, collapse, swap and reroot actino are disabled
//     2. Select the root item
//     Expected state: only swap action is available
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
    GTGlobals::sleep();

    QAbstractButton* collapse = GTWidget::findButtonByText(os, "Collapse"); //GTAction::button(os, "Collapse");
    QAbstractButton* swap = GTWidget::findButtonByText(os, "Swap Sibling");//GTAction::button(os, "Swap Siblings");
    QAbstractButton* reroot = GTWidget::findButtonByText(os, "Reroot");// GTAction::button(os, "Reroot tree");

    CHECK_SET_ERR( collapse != NULL, "collapse action button not found");
    CHECK_SET_ERR( swap != NULL, "swap action button not found");
    CHECK_SET_ERR( reroot != NULL, "reroot action button not found");

    CHECK_SET_ERR( !collapse->isEnabled(), "Collapse action is unexpectedly enabled");
    CHECK_SET_ERR( !swap->isEnabled(), "Swap action is unexpectedly enabled");
    CHECK_SET_ERR( !reroot->isEnabled(), "Reroot action is unexpectedly enabled");

    QList<QGraphicsItem*> nodes = GTUtilsPhyTree::getNodes(os);
    CHECK_SET_ERR( !nodes.isEmpty(), "No nodes found");

    QPoint p = GTUtilsPhyTree::getGlobalCoord(os, nodes.first());
    GTMouseDriver::moveTo(p);
    GTMouseDriver::click();

    CHECK_SET_ERR( !collapse->isEnabled(), "Collapse action is unexpectedly enabled");
    CHECK_SET_ERR( swap->isEnabled(), "Swap action is unexpectedly disnabled");
    CHECK_SET_ERR( !reroot->isEnabled(), "Reroot action is unexpectedly enabled");

    p = GTUtilsPhyTree::getGlobalCoord(os, nodes[5]);
    GTMouseDriver::moveTo(p);
    GTMouseDriver::click();

    CHECK_SET_ERR( collapse->isEnabled(), "Collapse action is unexpectedly disabled");
    CHECK_SET_ERR( swap->isEnabled(), "Swap action is unexpectedly disabled");
    CHECK_SET_ERR( reroot->isEnabled(), "Reroot action is unexpectedly disabled");

    GTWidget::click(os, collapse);
    CHECK_SET_ERR( collapse->text() == "Expand", "No Expand action");

    p = GTUtilsPhyTree::getGlobalCoord(os, nodes[3]);
    GTMouseDriver::moveTo(p);
    GTMouseDriver::click();
    CHECK_SET_ERR( collapse->text() == "Collapse", "No Collapse action");

    p = GTUtilsPhyTree::getGlobalCoord(os, nodes[5]);
    GTMouseDriver::moveTo(p);
    GTMouseDriver::click();
    CHECK_SET_ERR( collapse->text() == "Expand", "No Expand action");

    GTWidget::click(os, collapse);
    CHECK_SET_ERR( collapse->text() == "Collapse", "No Collapse action");
}

GUI_TEST_CLASS_DEFINITION(test_0026) {
//    Reroot action.

//    1. Open file "data/samples/Newick/COI.nwk".
//    Expected state: a philogenetic tree appears.
    GTFileDialog::openFile(os, dataDir + "samples/Newick/COI.nwk");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    2. Select the parent node of "Bicolorana_bicolor_EF540830" and "Roeseliana_roeseli".
    QList<GraphicsButtonItem *> nodes = GTUtilsPhyTree::getOrderedRectangularNodes(os);
    CHECK_SET_ERR(!nodes.isEmpty(), "Tree nodes are not found");
    const qreal firstNodeDistance = GTUtilsPhyTree::getNodeDistance(os, nodes.first());
    GTUtilsPhyTree::clickNode(os, nodes.first());
    CHECK_SET_ERR(!GTUtilsPhyTree::getSelectedNodes(os).isEmpty(), "A clicked node wasn't selected");

//    3. Do the context menu command "Reroot tree".
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os,  QStringList() << "Reroot tree"));
    GTMouseDriver::click(Qt::RightButton);

    GTUtilsTaskTreeView::waitTaskFinished(os);

//    Expected state: the tree is rerooted. The selected node parent node becomes a new tree root.
    nodes = GTUtilsPhyTree::getOrderedRectangularNodes(os);
    CHECK_SET_ERR(!nodes.isEmpty(), "Tree nodes are not found");
    const qreal firstNodeDistanceNew = GTUtilsPhyTree::getNodeDistance(os, nodes.first());

    CHECK_SET_ERR(firstNodeDistance != firstNodeDistanceNew, "Distances are not changed. The tree was not rerooted?")
}

GUI_TEST_CLASS_DEFINITION(test_0027) {
//    Swap siblings action.

//    1. Open file "data/samples/Newick/COI.nwk".
//    Expected state: a phylogenetic tree appears.
    GTFileDialog::openFile(os, dataDir + "samples/Newick/COI.nwk");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    2. Select the parent node of "Bicolorana_bicolor_EF540830" and "Roeseliana_roeseli".
    QList<qreal> distances = GTUtilsPhyTree::getOrderedRectangularBranchesDistances(os);
    CHECK_SET_ERR(!distances.isEmpty(), "Distances array is empty");
    distances.swap(1, 2);

    QList<GraphicsButtonItem *> nodes = GTUtilsPhyTree::getOrderedRectangularNodes(os);
    CHECK_SET_ERR(!nodes.isEmpty(), "Tree nodes are not found");
    GTUtilsPhyTree::clickNode(os, nodes.first());
    CHECK_SET_ERR(!GTUtilsPhyTree::getSelectedNodes(os).isEmpty(), "A clicked node wasn't selected");

//    3. Do the context menu command "Swap siblings".
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os,  QStringList() << "Swap Siblings"));
    GTMouseDriver::click(Qt::RightButton);

    GTUtilsTaskTreeView::waitTaskFinished(os);

//    Expected state: tree distances are not changed except two swapped branches.
    const QList<qreal> distancesNew = GTUtilsPhyTree::getOrderedRectangularBranchesDistances(os);
    CHECK_SET_ERR(!distancesNew.isEmpty(), "New distances array is empty");
    CHECK_SET_ERR(distances == distancesNew, "Tree has incorrect distances");
}

GUI_TEST_CLASS_DEFINITION(test_0028) {
    //    Swap siblings action.

    //    1. Open the file "data/samples/CLUSTALW/COI.aln"
    //    Expected state: a MSAEditor appears.
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, testDir + "_common_data/scenarios/sandbox/2298.nwk", 0, 0, true));
    GTWidget::click(os, GTAction::button(os, "Build Tree"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Select the parent node of "Bicolorana_bicolor_EF540830" and "Roeseliana_roeseli".
    QList<qreal> distances = GTUtilsPhyTree::getOrderedRectangularBranchesDistances(os);
    CHECK_SET_ERR(!distances.isEmpty(), "Distances array is empty");
    distances.swap(1, 2);

    GTWidget::click(os, GTUtilsPhyTree::getTreeViewerUi(os));
    QList<GraphicsButtonItem *> nodes = GTUtilsPhyTree::getOrderedRectangularNodes(os);
    CHECK_SET_ERR(!nodes.isEmpty(), "Tree nodes are not found");
    GTUtilsPhyTree::clickNode(os, nodes.first());
    CHECK_SET_ERR(!GTUtilsPhyTree::getSelectedNodes(os).isEmpty(), "A clicked node wasn't selected");

    //    3. Do the context menu command "Swap siblings".
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Swap Siblings"));
    GTMouseDriver::click(Qt::RightButton);

    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: tree distances are not changed except two swapped branches.
    const QList<qreal> distancesNew = GTUtilsPhyTree::getOrderedRectangularBranchesDistances(os);
    CHECK_SET_ERR(!distancesNew.isEmpty(), "New distances array is empty");
    CHECK_SET_ERR(distances == distancesNew, "Tree has incorrect distances");
}

GUI_TEST_CLASS_DEFINITION(test_0029){
    //    Reroot action.

    //    1. Open the file "data/samples/CLUSTALW/COI.aln"
    //    Expected state: a MSAEditor appears.
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, testDir + "_common_data/scenarios/sandbox/2298.nwk", 0, 0, true));
    QAbstractButton *tree = GTAction::button(os, "Build Tree");
    GTWidget::click(os, tree);
    GTGlobals::sleep();

    //    2. Select the parent node of "Bicolorana_bicolor_EF540830" and "Roeseliana_roeseli".
    GTWidget::click(os, GTUtilsPhyTree::getTreeViewerUi(os));
    QList<GraphicsButtonItem *> nodes = GTUtilsPhyTree::getOrderedRectangularNodes(os);
    CHECK_SET_ERR(!nodes.isEmpty(), "Tree nodes are not found");
    const qreal firstNodeDistance = GTUtilsPhyTree::getNodeDistance(os, nodes.first());
    GTUtilsPhyTree::clickNode(os, nodes.first());
    CHECK_SET_ERR(!GTUtilsPhyTree::getSelectedNodes(os).isEmpty(), "A clicked node wasn't selected");

    //    3. Do the context menu command "Reroot tree".
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Reroot tree"));
    GTMouseDriver::click(Qt::RightButton);

    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: the tree is rerooted. The selected node parent node becomes a new tree root.
    nodes = GTUtilsPhyTree::getOrderedRectangularNodes(os);
    CHECK_SET_ERR(!nodes.isEmpty(), "Tree nodes are not found");
    const qreal firstNodeDistanceNew = GTUtilsPhyTree::getNodeDistance(os, nodes.first());

    CHECK_SET_ERR(firstNodeDistance != firstNodeDistanceNew, "Distances are not changed. The tree was not rerooted?")
}

} // namespace GUITest_common_scenarios_tree_viewer
} // namespace U2
