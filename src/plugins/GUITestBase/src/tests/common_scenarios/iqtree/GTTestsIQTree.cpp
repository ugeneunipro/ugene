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
#include <primitives/GTComboBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTPlainTextEdit.h>
#include <primitives/GTToolbar.h>
#include <primitives/GTWidget.h>

#include <QApplication>

#include "GTTestsIQTree.h"
#include "GTUtilsLog.h"
#include "GTUtilsMdi.h"
#include "GTUtilsMsaEditor.h"
#include "GTUtilsTaskTreeView.h"
#include "runnables/ugene/corelibs/U2View//ov_msa/BuildTreeDialogFiller.h"

namespace U2 {
namespace GUITest_common_scenarios_iqtree {

GUI_TEST_CLASS_DEFINITION(test_0001) {
    // Check that IQ-TREE tool runs and produces a valid tree for AMINO, DNA and RNA alignments.

    QStringList alignmentFileList = {testDir + "_common_data/phylip/int_amino.phy",  // Amino.
                                     testDir + "_common_data/phylip/int_DNA.phy",  // DNA.
                                     testDir + "_common_data/clustal/dafault_RNA.aln"};  // RNA.

    class RunIQTreeScenario : public CustomScenario {
    public:
        void run(GUITestOpStatus& os) override {
            auto dialog = GTWidget::getActiveModalWidget(os);
            GTComboBox::selectItemByText(os, "algorithmBox", dialog, "IQ-TREE");

            // Set output file name.
            GTLineEdit::setText(os, "fileNameEdit", sandBoxDir + "GUITest_common_scenarios_iqtree_test_0001.nwk", dialog);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    for (const QString& alignmentFile : qAsConst(alignmentFileList)) {
        GTUtilsMdi::closeAllWindows(os);
        GTFileDialog::openFile(os, alignmentFile);
        GTUtilsTaskTreeView::waitTaskFinished(os);

        GTLogTracer logTracer("Launching IQ-TREE tool");
        GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, new RunIQTreeScenario()));
        GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Build Tree");
        GTUtilsTaskTreeView::waitTaskFinished(os);

        // Check that tool is launched.
        GTUtilsLog::checkContainsMessage(os, logTracer, true);
        CHECK_SET_ERR(logTracer.getJoinedErrorString().isEmpty(), "Errors in the log: " + logTracer.getJoinedErrorString());

        // Check that tree view is opened.
        GTUtilsMsaEditor::getTreeView(os);
    }
}

GUI_TEST_CLASS_DEFINITION(test_0002) {
    // Check that IQ-TREE support passing of custom user options to the original tools.

    GTFileDialog::openFile(os, testDir + "_common_data/clustal/amino_from_wikipedia.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTLogTracer logTracer;

    class RunIQTreeScenario : public CustomScenario {
    public:
        void run(GUITestOpStatus& os) override {
            auto dialog = GTWidget::getActiveModalWidget(os);
            GTComboBox::selectItemByText(os, "algorithmBox", dialog, "IQ-TREE");

            auto paramsEditor = GTWidget::findPlainTextEdit(os, "extraParametersTextEdit");
            GTPlainTextEdit::setPlainText(os, paramsEditor, "-seed\n12345 -m\"ra\"te \"E,I,G,I+G\"");

            // Set output file name.
            GTLineEdit::setText(os, "fileNameEdit", sandBoxDir + "GUITest_common_scenarios_iqtree_test_0002.nwk", dialog);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, new RunIQTreeScenario()));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Build Tree");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Check that tool is launched.
    CHECK_SET_ERR(GTLogTracer::checkMessage("Launching IQ-TREE tool"), "No tool launch message found");
    CHECK_SET_ERR(GTLogTracer::checkMessage("-seed 12345"), "No custom argument message found/-seed");
    CHECK_SET_ERR(GTLogTracer::checkMessage("-mrate E,I,G,I+G"), "No custom argument message found/-mrate");
    CHECK_SET_ERR(logTracer.getJoinedErrorString().isEmpty(), "Errors in the log: " + logTracer.getJoinedErrorString());

    // Check that tree view is opened.
    GTUtilsMsaEditor::getTreeView(os);
}

}  // namespace GUITest_common_scenarios_iqtree
}  // namespace U2
