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

#include <base_dialogs/GTFileDialog.h>

#include <QApplication>
#include <QColor>

#include "GTTestsEntropyCalculation.h"

#include "GTUtilsOptionPanelSequenceView.h"
#include "GTUtilsTaskTreeView.h"
#include "GTUtilsSequenceView.h"

#include <primitives/GTLineEdit.h>
#include <primitives/GTWidget.h>
#include <primitives/PopupChooser.h>
#include <primitives/GTMenu.h>

#include "runnables/ugene/corelibs/U2Gui/DownloadRemoteFileDialogFiller.h"

#include <utils/GTUtilsDialog.h>

#include <system/GTFile.h>

namespace U2 {
namespace GUITest_common_scenarios_entropy {

GUI_TEST_CLASS_DEFINITION(test_0001) {
    // Open "_common_data/entropy/4aa8.pdb"
    GTFileDialog::openFile(os, testDir + "_common_data/entropy/4aa8.pdb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //Set coloring scheme to "Alignment Entropy"
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Coloring Scheme", "Alignment Entropy"}));
    QWidget* sequenceViewWindow = GTUtilsSequenceView::getActiveSequenceViewWindow(os);
    auto pdb2Widget = GTWidget::findWidget(os, "1-4AA8", sequenceViewWindow);
    GTMenu::showContextMenu(os, pdb2Widget);
    QImage pdb2ImageBefore = GTWidget::getImage(os, pdb2Widget);

    //Run alignment entropy calculation
    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::Tabs::EntropyCalculation);
    GTLineEdit::setText(os, "alignmentLineEdit", testDir + "_common_data/entropy/chymosin.fas");
    GTLineEdit::setText(os, "saveToLineEdit", sandBoxDir + "entropy_0001.pdb");
    GTWidget::click(os, GTWidget::findPushButton(os, "runButton"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Coloring Scheme", "Alignment Entropy"}));

    sequenceViewWindow = GTUtilsSequenceView::getActiveSequenceViewWindow(os);
    pdb2Widget = GTWidget::findWidget(os, "2-4AA8", sequenceViewWindow);
    
    GTMenu::showContextMenu(os, pdb2Widget);

    QImage pdb2ImageAfter = GTWidget::getImage(os, pdb2Widget);
    CHECK_SET_ERR(pdb2ImageBefore != pdb2ImageAfter, "PDB2 3D image is not changed");
    CHECK_SET_ERR(GTFile::equals(os, sandBoxDir + "entropy_0001.pdb", 
        testDir + "_common_data/entropy/results/entropy_0001.pdb"), 
        "PDB file is not equal to _common_data/entropy/results/entropy_0001.pdb");
}

GUI_TEST_CLASS_DEFINITION(test_0002) {
    // Open "_common_data/entropy/2DD8.pdb"
    GTFileDialog::openFile(os, testDir + "_common_data/entropy/2DD8.pdb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Coloring Scheme", "Alignment Entropy"}));
    QWidget* sequenceViewWindow = GTUtilsSequenceView::getActiveSequenceViewWindow(os);
    auto pdb2Widget = GTWidget::findWidget(os, "1-2DD8", sequenceViewWindow);
    GTMenu::showContextMenu(os, pdb2Widget);
    QImage pdb2ImageBefore = GTWidget::getImage(os, pdb2Widget);

    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::Tabs::EntropyCalculation);
    GTLineEdit::setText(os, "alignmentLineEdit", testDir + "_common_data/entropy/RBD SARS-Cov-2.fas");
    GTLineEdit::setText(os, "saveToLineEdit", sandBoxDir + "entropy_0002.pdb");
    GTWidget::click(os, GTWidget::findPushButton(os, "runButton"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Coloring Scheme", "Alignment Entropy"}));

    sequenceViewWindow = GTUtilsSequenceView::getActiveSequenceViewWindow(os);
    pdb2Widget = GTWidget::findWidget(os, "2-2DD8", sequenceViewWindow);

    GTMenu::showContextMenu(os, pdb2Widget);

    QImage pdb2ImageAfter = GTWidget::getImage(os, pdb2Widget);
    CHECK_SET_ERR(pdb2ImageBefore != pdb2ImageAfter, "PDB2 3D image is not changed");
    CHECK_SET_ERR(GTFile::equals(os, sandBoxDir + "entropy_0002.pdb", 
        testDir + "_common_data/entropy/results/entropy_0002.pdb"),
        "PDB file is not equal to _common_data/entropy/results/entropy_0002.pdb");
}

GUI_TEST_CLASS_DEFINITION(test_0003) {
    // Open "_common_data/entropy/6QS9.pdb"
    GTFileDialog::openFile(os, testDir + "_common_data/entropy/6QS9.pdb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Coloring Scheme", "Alignment Entropy"}));
    QWidget* sequenceViewWindow = GTUtilsSequenceView::getActiveSequenceViewWindow(os);
    auto pdb2Widget = GTWidget::findWidget(os, "1-6QS9", sequenceViewWindow);
    GTMenu::showContextMenu(os, pdb2Widget);
    QImage pdb2ImageBefore = GTWidget::getImage(os, pdb2Widget);

    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::Tabs::EntropyCalculation);
    GTLineEdit::setText(os, "alignmentLineEdit", testDir + "_common_data/entropy/albumin.fas");
    GTLineEdit::setText(os, "saveToLineEdit", sandBoxDir + "entropy_0003.pdb");
    GTWidget::click(os, GTWidget::findPushButton(os, "runButton"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Coloring Scheme", "Alignment Entropy"}));

    sequenceViewWindow = GTUtilsSequenceView::getActiveSequenceViewWindow(os);
    pdb2Widget = GTWidget::findWidget(os, "2-6QS9", sequenceViewWindow);

    GTMenu::showContextMenu(os, pdb2Widget);

    QImage pdb2ImageAfter = GTWidget::getImage(os, pdb2Widget);
    CHECK_SET_ERR(pdb2ImageBefore != pdb2ImageAfter, "PDB2 3D image is not changed");
    CHECK_SET_ERR(GTFile::equals(os, sandBoxDir + "entropy_0003.pdb", testDir + 
        "_common_data/entropy/results/entropy_0003.pdb"),
        "PDB file is not equal to _common_data/entropy/results/entropy_0003.pdb");
}

GUI_TEST_CLASS_DEFINITION(test_0004) {
    QList<DownloadRemoteFileDialogFiller::Action> actions;

    actions << DownloadRemoteFileDialogFiller::Action(DownloadRemoteFileDialogFiller::SetResourceIds, {"A2BC19"});
    actions << DownloadRemoteFileDialogFiller::Action(DownloadRemoteFileDialogFiller::SetDatabase, "AlphaFold Protein Structure Database");
    actions << DownloadRemoteFileDialogFiller::Action(DownloadRemoteFileDialogFiller::EnterSaveToDirectoryPath, sandBoxDir + "test_0002");
    actions << DownloadRemoteFileDialogFiller::Action(DownloadRemoteFileDialogFiller::ClickOk, "");

    GTUtilsDialog::waitForDialog(os, new DownloadRemoteFileDialogFiller(os, actions));
    GTMenu::clickMainMenuItem(os, {"File", "Access remote database..."});
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);
}

}  // namespace GUITest_common_scenarios_entropy
}  // namespace U2
