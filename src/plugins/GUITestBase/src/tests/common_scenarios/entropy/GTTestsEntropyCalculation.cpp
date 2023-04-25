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
//#include <primitives/GTCheckBox.h>
//#include <primitives/GTComboBox.h>
//#include <primitives/GTLineEdit.h>
//#include <primitives/GTPlainTextEdit.h>
//#include <primitives/GTToolbar.h>
//#include <primitives/GTWidget.h>

#include <QApplication>
#include <QColor>

#include "GTTestsEntropyCalculation.h"
//#include "GTUtilsLog.h"
//#include "GTUtilsMdi.h"
#include "GTUtilsOptionPanelSequenceView.h"
#include "GTUtilsTaskTreeView.h"
#include "GTUtilsSequenceView.h"

#include <primitives/GTLineEdit.h>
#include <primitives/GTWidget.h>
#include <primitives/PopupChooser.h>
#include <primitives/GTMenu.h>

#include <utils/GTUtilsDialog.h>

#include <system/GTFile.h>
#include "runnables/ugene/corelibs/U2Gui/DownloadRemoteFileDialogFiller.h"



//#include "runnables/ugene/corelibs/U2View//ov_msa/BuildTreeDialogFiller.h"

namespace U2 {
namespace GUITest_common_scenarios_entropy {

GUI_TEST_CLASS_DEFINITION(test_0001) {
    // Open "_common_data/entropy/4aa8.pdb"
    GTFileDialog::openFile(os, testDir + "_common_data/entropy/4aa8.pdb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Coloring Scheme", "Alignment Entropy"}));
    QWidget* sequenceViewWindow = GTUtilsSequenceView::getActiveSequenceViewWindow(os);
    auto pdb2Widget = GTWidget::findWidget(os, "1-4AA8", sequenceViewWindow);
    GTMenu::showContextMenu(os, pdb2Widget);
    QImage pdb2ImageBefore = GTWidget::getImage(os, pdb2Widget);

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
    GTFile::equals(os, sandBoxDir + "entropy_0001.pdb", testDir + "_common_data/entropy/results/entropy_0001.pdb");
    //pdb2ImageAfter.save("C:/Users/alice/source/repos/test/general/_common_data/entropy/img.jpg", nullptr, 100);
    
}

GUI_TEST_CLASS_DEFINITION(test_0002) {
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

}  // namespace GUITest_common_scenarios_fasttree
}  // namespace U2
