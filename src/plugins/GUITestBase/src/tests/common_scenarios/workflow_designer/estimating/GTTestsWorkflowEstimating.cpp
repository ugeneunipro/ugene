/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include <base_dialogs/MessageBoxFiller.h>
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTWidget.h>

#include "GTTestsWorkflowEstimating.h"
#include "GTUtilsLog.h"
#include "GTUtilsWorkflowDesigner.h"
#include "primitives/GTAction.h"
#include "primitives/GTMenu.h"
#include "utils/GTUtilsDialog.h"

namespace U2 {

namespace GUITest_common_scenarios_workflow_estimating {
using namespace HI;

GUI_TEST_CLASS_DEFINITION(test_0001) {
    // 1. Open WD.
    // 2. Open any NGS pipeline, e.g. Call Variants.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsWorkflowDesigner::addSample("Call variants with SAMtools");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    // 3. Click an "Estimate scheme" button on the toolbar.
    // Expected state: a warning message box appears: user should fix all errors.
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok, "Please fix issues listed in the error list (located under workflow)."));
    GTWidget::click(GTAction::button("Estimate workflow"));
}

GUI_TEST_CLASS_DEFINITION(test_0002) {
    // 1. Open WD.
    // 2. Open any NGS pipeline, e.g. Call Variants.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsWorkflowDesigner::addSample("Call variants with SAMtools");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    //    3. Set valid input data.
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Read Assembly (BAM/SAM)"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setDatasetInputFile(testDir + "_common_data/bam/accepted_hits_with_gaps.bam");

    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Read Sequence"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setDatasetInputFile(testDir + "_common_data/fasta/reference_ACGT_rand_1000.fa");
    //    4. Click an "Estimate scheme" button on the toolbar.
    //    Expected state: an info message box appears: there is a time estimation for the set input data and two buttons: close and run.
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Close, "Approximate estimation time of the workflow run is"));
    GTWidget::click(GTAction::button("Estimate workflow"));
}

}  // namespace GUITest_common_scenarios_workflow_estimating

}  // namespace U2
