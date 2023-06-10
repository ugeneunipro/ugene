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
#include <primitives/GTCheckBox.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTPlainTextEdit.h>
#include <primitives/GTToolbar.h>
#include <primitives/GTWidget.h>

#include <QApplication>

#include "GTTestsFastTree.h"
#include "GTUtilsLog.h"
#include "GTUtilsMdi.h"
#include "GTUtilsMsaEditor.h"
#include "GTUtilsTaskTreeView.h"
#include "runnables/ugene/corelibs/U2View//ov_msa/BuildTreeDialogFiller.h"

namespace U2 {
namespace GUITest_common_scenarios_fasttree {

GUI_TEST_CLASS_DEFINITION(test_0001) {
    // Check that FastTree tool runs and produces a valid tree for AMINO, DNA and RNA alignments.

    QStringList alignmentFileList = {testDir + "_common_data/phylip/int_amino.phy",  // Amino.
                                     testDir + "_common_data/phylip/int_DNA.phy",  // DNA.
                                     testDir + "_common_data/clustal/dafault_RNA.aln"};  // RNA.

    class RunFastTreeScenario : public CustomScenario {
    public:
        void run() override {
            auto dialog = GTWidget::getActiveModalWidget();
            GTComboBox::selectItemByText("algorithmBox", dialog, "FastTree");

            // Set output file name.
            GTLineEdit::setText("fileNameEdit", sandBoxDir + "GUITest_common_scenarios_fasttree_test_0001.nwk", dialog);

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };

    for (const QString& alignmentFile : qAsConst(alignmentFileList)) {
        GTUtilsMdi::closeAllWindows();
        GTFileDialog::openFile(alignmentFile);
        GTUtilsTaskTreeView::waitTaskFinished();

        GTLogTracer lt;
        GTUtilsDialog::waitForDialog(new BuildTreeDialogFiller(new RunFastTreeScenario()));
        GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Build Tree");
        GTUtilsTaskTreeView::waitTaskFinished();

        // Check that tool is launched.
        CHECK_SET_ERR(lt.hasMessage("Launching FastTree tool"), "Expected message not found");
        CHECK_SET_ERR(lt.getJoinedErrorString().isEmpty(), "Errors in the log: " + lt.getJoinedErrorString());

        // Check that tree view is opened.
        GTUtilsMsaEditor::getTreeView();
    }
}

GUI_TEST_CLASS_DEFINITION(test_0002) {
    // Check that FastTree support passing of custom user options to the original tools.

    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTLogTracer lt;

    class RunFastTreeScenario : public CustomScenario {
    public:
        void run() override {
            auto dialog = GTWidget::getActiveModalWidget();
            GTComboBox::selectItemByText("algorithmBox", dialog, "FastTree");

            auto paramsEditor = GTWidget::findPlainTextEdit("extraParametersTextEdit");
            GTPlainTextEdit::setText(paramsEditor, "-gtr");

            GTCheckBox::setChecked("useFastestCheckBox", dialog);
            GTCheckBox::setChecked("usePseudoCountsCheckBox", dialog);

            GTLineEdit::setText("fileNameEdit", sandBoxDir + "GUITest_common_scenarios_fasttree_test_0002.nwk", dialog);
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(new BuildTreeDialogFiller(new RunFastTreeScenario()));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Build Tree");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Check that tool is launched.
    CHECK_SET_ERR(lt.hasMessage("Launching FastTree tool"), "No tool launch message found");
    CHECK_SET_ERR(lt.hasMessage("-gtr"), "No custom argument message found/--gtr");
    CHECK_SET_ERR(lt.hasMessage("-pseudo"), "No custom argument message found/-pseudo");
    CHECK_SET_ERR(lt.getJoinedErrorString().isEmpty(), "Errors in the log: " + lt.getJoinedErrorString());

    // Check that tree view is opened.
    GTUtilsMsaEditor::getTreeView();
}

}  // namespace GUITest_common_scenarios_fasttree
}  // namespace U2
