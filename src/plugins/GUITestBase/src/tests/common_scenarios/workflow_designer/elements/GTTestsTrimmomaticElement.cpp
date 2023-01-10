/**009
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

#include "GTTestsTrimmomaticElement.h"
#include <drivers/GTMouseDriver.h>
#include <primitives/GTTableView.h>
#include <primitives/GTWidget.h>

#include "../../workflow_designer/src/WorkflowViewItems.h"
#include "GTUtilsWorkflowDesigner.h"
#include "runnables/ugene/plugins/external_tools/TrimmomaticDialogFiller.h"

namespace U2 {

namespace GUITest_common_scenarios_trimmomatic_element {
using namespace HI;

using TrimmomaticAddSettings = QPair<TrimmomaticDialogFiller::TrimmomaticSteps, QMap<TrimmomaticDialogFiller::TrimmomaticValues, QVariant>>;
using TrimmomaticMoveSettings = QPair<QPair<TrimmomaticDialogFiller::TrimmomaticSteps, int>, TrimmomaticDialogFiller::TrimmomaticDirection>;

GUI_TEST_CLASS_DEFINITION(test_0001) {
    // 1. Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    // 2. Add "Improve Reads with Trimmomatic" to the scene
    const QString trimmomaticName = "Improve Reads with Trimmomatic";
    WorkflowProcessItem* trimmomaticElement = GTUtilsWorkflowDesigner::addElement(os, trimmomaticName);

    // 3. Click on the element, open the "Trimmomatic steps" dialog
    // 4. Add the "AVGQUAL" step and set "Quality Threshold" to 25
    // 5. Add the "CROP" step and set "Length" to 10
    // 6. Add the "HEADCROP" step and set "Length" to 15
    // 7. Add the "ILLUMINACLIP" step and set "AdapterSequence" to "adapters/illumina/TruSeq3-SE.fa",
    //    "Seed Mismatches" to 5, "Palindrome Clip Threshold" to 25, "Simple Clip Threshold" to 8,
    //    check "Provide Optional Settings", "Min Adapter Length" to 6 and "Keep Both Reads" to True
    // 8. Add the "LEADING" step and set "Strictness" to 0.60 and "Target Length" to 35.
    // 9. Add the "MAXINFO" step and set "Length" to 10
    // 10. Add the "MINLEN" step and set "Window Size" to 5 and "Quality Threshold" to 25
    // 11. Add the "TOPHRED33" step
    // 12. Add the "TOPHRED64" step
    // 13. Add the "TRAILING" step and set "Quality Threshold" to 25
    // 14. Accept the dialog

    QList<TrimmomaticAddSettings> steps;
    QMap<TrimmomaticDialogFiller::TrimmomaticValues, QVariant> avgqual = {{TrimmomaticDialogFiller::TrimmomaticValues::QualityThreshold, "25"}};
    steps.append(TrimmomaticAddSettings(TrimmomaticDialogFiller::TrimmomaticSteps::AVGQUAL, avgqual));
    QMap<TrimmomaticDialogFiller::TrimmomaticValues, QVariant> crop = {{TrimmomaticDialogFiller::TrimmomaticValues::Length, "10"}};
    steps.append(TrimmomaticAddSettings(TrimmomaticDialogFiller::TrimmomaticSteps::CROP, crop));
    QMap<TrimmomaticDialogFiller::TrimmomaticValues, QVariant> headcrop = {{TrimmomaticDialogFiller::TrimmomaticValues::Length, "15"}};
    steps.append(TrimmomaticAddSettings(TrimmomaticDialogFiller::TrimmomaticSteps::HEADCROP, headcrop));
    QMap<TrimmomaticDialogFiller::TrimmomaticValues, QVariant> illuminaclip = {{TrimmomaticDialogFiller::TrimmomaticValues::AdapterSequence, dataDir + "adapters/illumina/TruSeq3-SE.fa"},
                                                                               {TrimmomaticDialogFiller::TrimmomaticValues::SeedMismatches, "5"},
                                                                               {TrimmomaticDialogFiller::TrimmomaticValues::PalindromeClipThreshold, "25"},
                                                                               {TrimmomaticDialogFiller::TrimmomaticValues::SimpleClipThreshold, "8"},
                                                                               {TrimmomaticDialogFiller::TrimmomaticValues::ProvideOptionalSettings, true},
                                                                               {TrimmomaticDialogFiller::TrimmomaticValues::MinAdapterLength, "6"},
                                                                               {TrimmomaticDialogFiller::TrimmomaticValues::KeepBothReads, "True"}};
    steps.append(TrimmomaticAddSettings(TrimmomaticDialogFiller::TrimmomaticSteps::ILLUMINACLIP, illuminaclip));
    QMap<TrimmomaticDialogFiller::TrimmomaticValues, QVariant> leading = {{TrimmomaticDialogFiller::TrimmomaticValues::QualityThreshold, "15"}};
    steps.append(TrimmomaticAddSettings(TrimmomaticDialogFiller::TrimmomaticSteps::LEADING, leading));
    QMap<TrimmomaticDialogFiller::TrimmomaticValues, QVariant> maxinfo = {{TrimmomaticDialogFiller::TrimmomaticValues::TargetLength, "35"},
                                                                          {TrimmomaticDialogFiller::TrimmomaticValues::Strictness, "0.60"}};
    steps.append(TrimmomaticAddSettings(TrimmomaticDialogFiller::TrimmomaticSteps::MAXINFO, maxinfo));
    QMap<TrimmomaticDialogFiller::TrimmomaticValues, QVariant> minlen = {{TrimmomaticDialogFiller::TrimmomaticValues::Length, "10"}};
    steps.append(TrimmomaticAddSettings(TrimmomaticDialogFiller::TrimmomaticSteps::MINLEN, minlen));
    QMap<TrimmomaticDialogFiller::TrimmomaticValues, QVariant> slidingwindow = {{TrimmomaticDialogFiller::TrimmomaticValues::WindowSize, "5"},
                                                                                {TrimmomaticDialogFiller::TrimmomaticValues::QualityThreshold, "25"}};
    steps.append(TrimmomaticAddSettings(TrimmomaticDialogFiller::TrimmomaticSteps::SLIDINGWINDOW, slidingwindow));
    QMap<TrimmomaticDialogFiller::TrimmomaticValues, QVariant> tophred;
    steps.append(TrimmomaticAddSettings(TrimmomaticDialogFiller::TrimmomaticSteps::TOPHRED33, tophred));
    steps.append(TrimmomaticAddSettings(TrimmomaticDialogFiller::TrimmomaticSteps::TOPHRED64, tophred));
    QMap<TrimmomaticDialogFiller::TrimmomaticValues, QVariant> trailing = {{TrimmomaticDialogFiller::TrimmomaticValues::QualityThreshold, "25"}};
    steps.append(TrimmomaticAddSettings(TrimmomaticDialogFiller::TrimmomaticSteps::TRAILING, trailing));

    TrimmomaticDialogFiller* filler = new TrimmomaticDialogFiller(os, steps);

    GTUtilsDialog::waitForDialog(os, filler);
    TrimmomaticDialogFiller::openDialog(os, trimmomaticElement);
}

GUI_TEST_CLASS_DEFINITION(test_0002) {
    // 1. Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    // 2. Add "Improve Reads with Trimmomatic" to the scene
    const QString trimmomaticName = "Improve Reads with Trimmomatic";
    WorkflowProcessItem* trimmomaticElement = GTUtilsWorkflowDesigner::addElement(os, trimmomaticName);

    // 3. Click on the element, open the "Trimmomatic steps" dialog
    // 4. Add the "AVGQUAL" step and set "Quality Threshold" to 1
    // 5. Add the "AVGQUAL" step and set "Quality Threshold" to 2
    // 6. Add the "AVGQUAL" step and set "Quality Threshold" to 3
    // 7. Accept the dialog
    QList<TrimmomaticAddSettings> steps;
    QMap<TrimmomaticDialogFiller::TrimmomaticValues, QVariant> avgqual1 = {{TrimmomaticDialogFiller::TrimmomaticValues::QualityThreshold, "1"}};
    steps.append(TrimmomaticAddSettings(TrimmomaticDialogFiller::TrimmomaticSteps::AVGQUAL, avgqual1));
    QMap<TrimmomaticDialogFiller::TrimmomaticValues, QVariant> avgqual2 = {{TrimmomaticDialogFiller::TrimmomaticValues::QualityThreshold, "2"}};
    steps.append(TrimmomaticAddSettings(TrimmomaticDialogFiller::TrimmomaticSteps::AVGQUAL, avgqual2));
    QMap<TrimmomaticDialogFiller::TrimmomaticValues, QVariant> avgqual3 = {{TrimmomaticDialogFiller::TrimmomaticValues::QualityThreshold, "3"}};
    steps.append(TrimmomaticAddSettings(TrimmomaticDialogFiller::TrimmomaticSteps::AVGQUAL, avgqual3));
    TrimmomaticDialogFiller* addFiller = new TrimmomaticDialogFiller(os, steps);

    GTUtilsDialog::waitForDialog(os, addFiller);
    TrimmomaticDialogFiller::openDialog(os, trimmomaticElement);

    // 8. Click on the element, open the "Trimmomatic steps" dialog
    // 9. Click on the "AVGQUAL" step 1 and move it down
    // 10. Click on  the "AVGQUAL" step 2 and move it down
    // 11. Click on  the "AVGQUAL" step 2 and move it up
    // 12. Accept the dialog
    QList<TrimmomaticMoveSettings> moveSteps;
    TrimmomaticMoveSettings step1(QPair<TrimmomaticDialogFiller::TrimmomaticSteps, int>(TrimmomaticDialogFiller::TrimmomaticSteps::AVGQUAL, 0), TrimmomaticDialogFiller::TrimmomaticDirection::Down);
    moveSteps.append(step1);
    TrimmomaticMoveSettings step2(QPair<TrimmomaticDialogFiller::TrimmomaticSteps, int>(TrimmomaticDialogFiller::TrimmomaticSteps::AVGQUAL, 1), TrimmomaticDialogFiller::TrimmomaticDirection::Down);
    moveSteps.append(step2);
    TrimmomaticMoveSettings step3(QPair<TrimmomaticDialogFiller::TrimmomaticSteps, int>(TrimmomaticDialogFiller::TrimmomaticSteps::AVGQUAL, 1), TrimmomaticDialogFiller::TrimmomaticDirection::Up);
    moveSteps.append(step3);
    TrimmomaticDialogFiller* moveFiller = new TrimmomaticDialogFiller(os, moveSteps);

    GTUtilsDialog::waitForDialog(os, moveFiller);
    TrimmomaticDialogFiller::openDialog(os, trimmomaticElement);
}

GUI_TEST_CLASS_DEFINITION(test_0003) {
    // 1. Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    // 2. Add "Improve Reads with Trimmomatic" to the scene
    const QString trimmomaticName = "Improve Reads with Trimmomatic";
    WorkflowProcessItem* trimmomaticElement = GTUtilsWorkflowDesigner::addElement(os, trimmomaticName);

    // 3. Click on the element, open the "Trimmomatic steps" dialog
    // 4. Add the "AVGQUAL" step and set "Quality Threshold" to 1
    // 5. Add the "AVGQUAL" step and set "Quality Threshold" to 2
    // 6. Add the "AVGQUAL" step and set "Quality Threshold" to 3
    // 7. Accept the dialog
    QList<TrimmomaticAddSettings> steps;
    QMap<TrimmomaticDialogFiller::TrimmomaticValues, QVariant> avgqual1 = {{TrimmomaticDialogFiller::TrimmomaticValues::QualityThreshold, "1"}};
    steps.append(TrimmomaticAddSettings(TrimmomaticDialogFiller::TrimmomaticSteps::AVGQUAL, avgqual1));
    QMap<TrimmomaticDialogFiller::TrimmomaticValues, QVariant> avgqual2 = {{TrimmomaticDialogFiller::TrimmomaticValues::QualityThreshold, "2"}};
    steps.append(TrimmomaticAddSettings(TrimmomaticDialogFiller::TrimmomaticSteps::AVGQUAL, avgqual2));
    QMap<TrimmomaticDialogFiller::TrimmomaticValues, QVariant> avgqual3 = {{TrimmomaticDialogFiller::TrimmomaticValues::QualityThreshold, "3"}};
    steps.append(TrimmomaticAddSettings(TrimmomaticDialogFiller::TrimmomaticSteps::AVGQUAL, avgqual3));
    TrimmomaticDialogFiller* addFiller = new TrimmomaticDialogFiller(os, steps);

    GTUtilsDialog::waitForDialog(os, addFiller);
    TrimmomaticDialogFiller::openDialog(os, trimmomaticElement);
    // 8. Click on the element, open the "Trimmomatic steps" dialog
    // 9. Click on the "AVGQUAL" step 2 and remove it
    // 10. Click on  the "AVGQUAL" step 1 and remove it
    // 11. Accept the dialog
    QList<QPair<TrimmomaticDialogFiller::TrimmomaticSteps, int>> removeValues;
    QPair<TrimmomaticDialogFiller::TrimmomaticSteps, int> step1(TrimmomaticDialogFiller::TrimmomaticSteps::AVGQUAL, 2);
    removeValues.append(step1);
    QPair<TrimmomaticDialogFiller::TrimmomaticSteps, int> step2(TrimmomaticDialogFiller::TrimmomaticSteps::AVGQUAL, 0);
    removeValues.append(step2);
    TrimmomaticDialogFiller* removeFiller = new TrimmomaticDialogFiller(os, removeValues);

    GTUtilsDialog::waitForDialog(os, removeFiller);
    TrimmomaticDialogFiller::openDialog(os, trimmomaticElement);
}

}  // namespace GUITest_common_scenarios_trimmomatic_element

}  // namespace U2
