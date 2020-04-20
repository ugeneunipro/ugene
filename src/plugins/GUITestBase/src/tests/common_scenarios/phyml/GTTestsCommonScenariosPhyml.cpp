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

#include <base_dialogs/GTFileDialog.h>
#include <primitives/GTCheckBox.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTTabWidget.h>
#include <primitives/GTToolbar.h>
#include <primitives/GTWidget.h>

#include <QApplication>

#include "GTTestsCommonScenariosPhyml.h"
#include "GTUtilsTaskTreeView.h"
#include "GTUtilsLog.h"
#include "runnables/ugene/corelibs/U2View//ov_msa/BuildTreeDialogFiller.h"

namespace U2 {
namespace GUITest_common_scenarios_phyml {

GUI_TEST_CLASS_DEFINITION(test_0001) {
//# Test "Optimise" options: no options

//1. Open "_common_data/scenarios/msa/ma2_gapped.aln".
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//2. Click the "Build Tree" button on the toolbar.

    GTLogTracer logTracerTool("Launching PhyML Maximum Likelihood tool");
    GTLogTracer logTracerParameter("-o ");

    class Scenario : public CustomScenario {
    public:
        void run(GUITestOpStatus &os) {
            QWidget * const dialog = GTWidget::getActiveModalWidget(os);

//3. Select the "PhyML Maximum Likelihood" algorithm.
            GTComboBox::setIndexWithText(os, "algorithmBox", dialog, "PhyML Maximum Likelihood");

//4. Open the "Tree Searching" tab.
            GTTabWidget::clickTab(os, "twSettings", dialog, "Tree Searching");

//5. Ensure that all optimise options are unchecked.
            GTCheckBox::checkState(os, "optTopologyCheckbox", false, dialog);
            GTCheckBox::checkState(os, "optBranchCheckbox", false, dialog);
            GTCheckBox::checkState(os, "optimiseSubstitutionRateCheckbox", false, dialog);

//6. Set other necessary options and accept the dialog.
            GTLineEdit::setText(os, "fileNameEdit", sandBoxDir + "GUITest_common_scenarios_phyml_test_0001.nwk", dialog);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, new Scenario));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Build Tree");

    GTUtilsTaskTreeView::waitTaskFinished(os);

//Expected state: the tool is launched, there is no '-o' option in the launch parameters (or there can be '-o n' option, which means "none").
    GTUtilsLog::checkContainsMessage(os, logTracerTool, true);
    GTUtilsLog::checkContainsMessage(os, logTracerParameter, false);
}

GUI_TEST_CLASS_DEFINITION(test_0002) {
//# Test "Optimise" options: check "Optimise tree topology" option.

//1. Open "_common_data/scenarios/msa/ma2_gapped.aln".
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//2. Click the "Build Tree" button on the toolbar.

    GTLogTracer logTracerTool("Launching PhyML Maximum Likelihood tool");
    GTLogTracer logTracerParameter("-o tl");

    class Scenario : public CustomScenario {
    public:
        void run(GUITestOpStatus &os) {
            QWidget * const dialog = GTWidget::getActiveModalWidget(os);

//3. Select the "PhyML Maximum Likelihood" algorithm.
            GTComboBox::setIndexWithText(os, "algorithmBox", dialog, "PhyML Maximum Likelihood");

//4. Open the "Tree Searching" tab.
            GTTabWidget::clickTab(os, "twSettings", dialog, "Tree Searching");

//5. Ensure that all optimize options are unchecked.
            GTCheckBox::checkState(os, "optTopologyCheckbox", false, dialog);
            GTCheckBox::checkState(os, "optBranchCheckbox", false, dialog);
            GTCheckBox::checkState(os, "optimiseSubstitutionRateCheckbox", false, dialog);

//6. Check "Optimise tree topology" option.
            GTCheckBox::setChecked(os, "optTopologyCheckbox", true, dialog);

//Expected state: "Optimise tree topology" and "Optimise branch lengths" options are checked, "Optimise branch lengths" checkbox is disabled.
            GTCheckBox::checkState(os, "optTopologyCheckbox", true, dialog);
            GTCheckBox::checkState(os, "optBranchCheckbox", true, dialog);
            GTCheckBox::checkState(os, "optimiseSubstitutionRateCheckbox", false, dialog);
            GTWidget::checkEnabled(os, "optBranchCheckbox", false, dialog);

//7. Uncheck "Optimise tree topology" option.
            GTCheckBox::setChecked(os, "optTopologyCheckbox", false, dialog);

//Expected state: all options are unchecked, "Optimise branch lengths" checkbox is enabled.
            GTCheckBox::checkState(os, "optTopologyCheckbox", false, dialog);
            GTCheckBox::checkState(os, "optBranchCheckbox", false, dialog);
            GTCheckBox::checkState(os, "optimiseSubstitutionRateCheckbox", false, dialog);
            GTWidget::checkEnabled(os, "optBranchCheckbox", true, dialog);

//8. Check "Optimise branch lengths" option.
            GTCheckBox::setChecked(os, "optBranchCheckbox", true, dialog);

//Expected state: "Optimise branch length" is checked and enabled.
            GTCheckBox::checkState(os, "optTopologyCheckbox", false, dialog);
            GTCheckBox::checkState(os, "optBranchCheckbox", true, dialog);
            GTCheckBox::checkState(os, "optimiseSubstitutionRateCheckbox", false, dialog);
            GTWidget::checkEnabled(os, "optBranchCheckbox", true, dialog);

//9. Check "Optimise tree topology" option.
            GTCheckBox::setChecked(os, "optTopologyCheckbox", true, dialog);

//Expected state: "Optimise tree topology" and "Optimise branch lengths" options are checked, "Optimise branch lengths" checkbox is disabled.
            GTCheckBox::checkState(os, "optTopologyCheckbox", true, dialog);
            GTCheckBox::checkState(os, "optBranchCheckbox", true, dialog);
            GTCheckBox::checkState(os, "optimiseSubstitutionRateCheckbox", false, dialog);
            GTWidget::checkEnabled(os, "optBranchCheckbox", false, dialog);

//10. Uncheck "Optimise tree topology" option.
            GTCheckBox::setChecked(os, "optTopologyCheckbox", false, dialog);

//Expected state: "Optimise branch length" is checked and enabled.
            GTCheckBox::checkState(os, "optTopologyCheckbox", false, dialog);
            GTCheckBox::checkState(os, "optBranchCheckbox", true, dialog);
            GTCheckBox::checkState(os, "optimiseSubstitutionRateCheckbox", false, dialog);
            GTWidget::checkEnabled(os, "optBranchCheckbox", true, dialog);

//11. Check "Optimise tree topology" option.
            GTCheckBox::setChecked(os, "optTopologyCheckbox", true, dialog);

//12. Set other necessary options and accept the dialog.
            GTLineEdit::setText(os, "fileNameEdit", sandBoxDir + "GUITest_common_scenarios_phyml_test_0002.nwk", dialog);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, new Scenario));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Build Tree");

    GTUtilsTaskTreeView::waitTaskFinished(os);

//Expected state: the tool is launched, there is '-o tl' option in the launch parameters.
    GTUtilsLog::checkContainsMessage(os, logTracerTool, true);
    GTUtilsLog::checkContainsMessage(os, logTracerParameter, true);
}

GUI_TEST_CLASS_DEFINITION(test_0003) {
//# Test "Optimise" options: "Optimise branch lengths" option.

//1. Open "_common_data/scenarios/msa/ma2_gapped.aln".
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//2. Click the "Build Tree" button on the toolbar.

    GTLogTracer logTracerTool("Launching PhyML Maximum Likelihood tool");
    GTLogTracer logTracerParameter("-o l");

    class Scenario : public CustomScenario {
    public:
        void run(GUITestOpStatus &os) {
            QWidget * const dialog = GTWidget::getActiveModalWidget(os);

//3. Select the "PhyML Maximum Likelihood" algorithm.
            GTComboBox::setIndexWithText(os, "algorithmBox", dialog, "PhyML Maximum Likelihood");

//4. Open the "Tree Searching" tab.
            GTTabWidget::clickTab(os, "twSettings", dialog, "Tree Searching");

//5. Check "Optimise branch lengths" option.
            GTCheckBox::setChecked(os, "optBranchCheckbox", true, dialog);

//6. Set other necessary options and accept the dialog.
            GTLineEdit::setText(os, "fileNameEdit", sandBoxDir + "GUITest_common_scenarios_phyml_test_0003.nwk", dialog);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, new Scenario));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Build Tree");

    GTUtilsTaskTreeView::waitTaskFinished(os);

//Expected state: the tool is launched, there is '-o l' option in the launch parameters.
    GTUtilsLog::checkContainsMessage(os, logTracerTool, true);
    GTUtilsLog::checkContainsMessage(os, logTracerParameter, true);
}

GUI_TEST_CLASS_DEFINITION(test_0004) {
//# Test "Optimise" options: "Optimise substitution rate" option.

//1. Open "_common_data/scenarios/msa/ma2_gapped.aln".
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//2. Click the "Build Tree" button on the toolbar.

    GTLogTracer logTracerTool("Launching PhyML Maximum Likelihood tool");
    GTLogTracer logTracerParameter("-o r");

    class Scenario : public CustomScenario {
    public:
        void run(GUITestOpStatus &os) {
            QWidget * const dialog = GTWidget::getActiveModalWidget(os);

//3. Select the "PhyML Maximum Likelihood" algorithm.
            GTComboBox::setIndexWithText(os, "algorithmBox", dialog, "PhyML Maximum Likelihood");

//4. Open the "Tree Searching" tab.
            GTTabWidget::clickTab(os, "twSettings", dialog, "Tree Searching");

//5. Check "Optimise substitution rate" option.
            GTCheckBox::setChecked(os, "optimiseSubstitutionRateCheckbox", true, dialog);

//6. Set other necessary options and accept the dialog.
            GTLineEdit::setText(os, "fileNameEdit", sandBoxDir + "GUITest_common_scenarios_phyml_test_0004.nwk", dialog);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, new Scenario));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Build Tree");

    GTUtilsTaskTreeView::waitTaskFinished(os);

//Expected state: the tool is launched, there is '-o r' option in the launch parameters.
    GTUtilsLog::checkContainsMessage(os, logTracerTool, true);
    GTUtilsLog::checkContainsMessage(os, logTracerParameter, true);
}

GUI_TEST_CLASS_DEFINITION(test_0005) {
//# Test "Optimise" options: "Optimise branch lengths" and "Optimise substitution rate" options.

//1. Open "_common_data/scenarios/msa/ma2_gapped.aln".
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//2. Click the "Build Tree" button on the toolbar.

    GTLogTracer logTracerTool("Launching PhyML Maximum Likelihood tool");
    GTLogTracer logTracerParameter("-o lr");

    class Scenario : public CustomScenario {
    public:
        void run(GUITestOpStatus &os) {
            QWidget * const dialog = GTWidget::getActiveModalWidget(os);

//3. Select the "PhyML Maximum Likelihood" algorithm.
            GTComboBox::setIndexWithText(os, "algorithmBox", dialog, "PhyML Maximum Likelihood");

//4. Open the "Tree Searching" tab.
            GTTabWidget::clickTab(os, "twSettings", dialog, "Tree Searching");

//5. Check "Optimise branch lengths" and "Optimise substitution rate" options.
            GTCheckBox::setChecked(os, "optBranchCheckbox", true, dialog);
            GTCheckBox::setChecked(os, "optimiseSubstitutionRateCheckbox", true, dialog);

//6. Set other necessary options and accept the dialog.
            GTLineEdit::setText(os, "fileNameEdit", sandBoxDir + "GUITest_common_scenarios_phyml_test_0005.nwk", dialog);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, new Scenario));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Build Tree");

    GTUtilsTaskTreeView::waitTaskFinished(os);

//Expected state: the tool is launched, there is '-o lr' option in the launch parameters.
    GTUtilsLog::checkContainsMessage(os, logTracerTool, true);
    GTUtilsLog::checkContainsMessage(os, logTracerParameter, true);
}

GUI_TEST_CLASS_DEFINITION(test_0006) {
//# Test "Optimise" options: "Optimise tree topology" and "Optimise substitution rate" options.

//1. Open "_common_data/scenarios/msa/ma2_gapped.aln".
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//2. Click the "Build Tree" button on the toolbar.

    GTLogTracer logTracerTool("Launching PhyML Maximum Likelihood tool");
    GTLogTracer logTracerParameter("-o tlr");

    class Scenario : public CustomScenario {
    public:
        void run(GUITestOpStatus &os) {
            QWidget * const dialog = GTWidget::getActiveModalWidget(os);

//3. Select the "PhyML Maximum Likelihood" algorithm.
            GTComboBox::setIndexWithText(os, "algorithmBox", dialog, "PhyML Maximum Likelihood");

//4. Open the "Tree Searching" tab.
            GTTabWidget::clickTab(os, "twSettings", dialog, "Tree Searching");

//5. Check "Optimise tree topology" and "Optimise substitution rate" options.
            GTCheckBox::setChecked(os, "optTopologyCheckbox", true, dialog);
            GTCheckBox::setChecked(os, "optimiseSubstitutionRateCheckbox", true, dialog);

//6. Set other necessary options and accept the dialog.
            GTLineEdit::setText(os, "fileNameEdit", sandBoxDir + "GUITest_common_scenarios_phyml_test_0006.nwk", dialog);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, new Scenario));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Build Tree");

    GTUtilsTaskTreeView::waitTaskFinished(os);

//Expected state: the tool is launched, there is '-o tlr' option in the launch parameters.
    GTUtilsLog::checkContainsMessage(os, logTracerTool, true);
    GTUtilsLog::checkContainsMessage(os, logTracerParameter, true);
}

}   // namespace GUITest_common_scenarios_phyml
}   // namespace U2
