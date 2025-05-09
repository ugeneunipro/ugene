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

#include <base_dialogs/GTFileDialog.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTTreeWidget.h>

#include <QTreeWidget>

#include <U2View/ADVConstants.h>

#include "../../../GTUtilsMsaEditorSequenceArea.h"
#include "../../../runnables/ugene/corelibs/U2View/utils_smith_waterman/SmithWatermanDialogBaseFiller.h"
#include "GTTestsSWDialog.h"
#include "GTUtilsAnnotationsTreeView.h"
#include "GTUtilsMdi.h"
#include "GTUtilsProject.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsSequenceView.h"
#include "GTUtilsTaskTreeView.h"
#include "primitives/GTMenu.h"
#include "primitives/PopupChooser.h"

namespace U2 {

namespace GUITest_common_scenarios_sw_dialog {
using namespace HI;
GUI_TEST_CLASS_DEFINITION(test_0001) {
    // Performing Smith-Waterman algorithm with multiple alignment as result
    //
    //  Steps:
    //
    //  1. Use menu {File->Open}. Open file _common_data/smith_waterman2/multi/06/query.txt. Get pattern sequence
    GTFileDialog::openFile(testDir + "_common_data/smith_waterman2/multi/06/", "query.txt");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsDocument::checkDocument("query.txt");
    QString patternSequence;
    GTUtilsSequenceView::getSequenceAsString(patternSequence);
    CHECK_SET_ERR(!patternSequence.isEmpty(), "Pattern sequence is empty");

    // 2. Use menu {File->Open}. Open file _common_data/smith_waterman2/multi/06/search.txt
    GTFileDialog::openFile(testDir + "_common_data/smith_waterman2/multi/06/", "search.txt");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsDocument::checkDocument("search.txt");

    // 3. Run Smith-Waterman Search by SW dialog
    Runnable* swDialog = new SmithWatermanDialogFiller(SmithWatermanDialogFiller::CLASSIC, SmithWatermanSettings::MULTIPLE_ALIGNMENT, testDir + "_common_data/scenarios/sandbox/", patternSequence);
    GTUtilsDialog::waitForDialog(swDialog);

    GTMenu::clickMainMenuItem({"Actions", "Analyze", "Find pattern [Smith-Waterman]..."}, GTGlobals::UseMouse);

    // 4. Check names of alignment files and names of found subsequences
    const QString seqNameMismatchErrorMessage = "sequences name list mismatch detected in file ";
    const QString seqNumberMismatchErrorMessage = "count of sequences mismatch detected in file ";
    const QString alignmentFileExtension = ".aln";

    for (int i = 2; i > 0; i--) {
        const QString expectedFileName = "P1_S_" + QString::number(i) + "_test]" + alignmentFileExtension;
        GTUtilsDocument::checkDocument(expectedFileName);

        GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(expectedFileName.left(expectedFileName.length() - alignmentFileExtension.length())));
        GTMouseDriver::doubleClick();

        const QStringList sequencesNameList = GTUtilsMSAEditorSequenceArea::getNameList();
        CHECK_SET_ERR(2 == sequencesNameList.length(), seqNumberMismatchErrorMessage + expectedFileName);
        switch (i) {
            case 1:
                CHECK_SET_ERR(sequencesNameList[0] == "S_2_15_test]" && sequencesNameList[1] == "P1_4_16",
                              seqNameMismatchErrorMessage + expectedFileName);
                break;

            case 2:
                CHECK_SET_ERR(sequencesNameList[0] == "S_34_42_test]" && sequencesNameList[1] == "P1_5_13",
                              seqNameMismatchErrorMessage + expectedFileName);
                break;

            default:
                assert(0);
        }
        GTUtilsMdi::click(GTGlobals::Close);
        GTMouseDriver::click();
    }
}

GUI_TEST_CLASS_DEFINITION(test_0002) {
    // Performing Smith-Waterman algorithm with annotations as result
    //
    //  Steps:
    //
    //  1. Use menu {File->Open}. Open file _common_data/smith_waterman2/multi/06/query.txt. Get pattern sequence
    GTFileDialog::openFile(testDir + "_common_data/smith_waterman2/multi/06/", "query.txt");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsDocument::checkDocument("query.txt");
    QString patternSequence;
    GTUtilsSequenceView::getSequenceAsString(patternSequence);
    CHECK_SET_ERR(!patternSequence.isEmpty(), "Pattern sequence is empty");

    // 2. Use menu {File->Open}. Open file _common_data/smith_waterman2/multi/06/search.txt
    GTFileDialog::openFile(testDir + "_common_data/smith_waterman2/multi/06/", "search.txt");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsDocument::checkDocument("search.txt");

    // 3. Run Smith-Waterman Search by SW dialog
    Runnable* swDialog = new SmithWatermanDialogFiller(SmithWatermanDialogFiller::CLASSIC, SmithWatermanSettings::ANNOTATIONS, testDir + "_common_data/scenarios/sandbox/", patternSequence);
    GTUtilsDialog::waitForDialog(swDialog);

    GTMenu::clickMainMenuItem({"Actions", "Analyze", "Find pattern [Smith-Waterman]..."}, GTGlobals::UseMouse);

    // 4. Close sequence view, then reopen it
    GTUtilsMdi::click(GTGlobals::Close);
    GTMouseDriver::click();

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("S"));
    GTMouseDriver::doubleClick();

    // 5. Check names and count of annotations
    QTreeWidget* treeWidget = GTUtilsAnnotationsTreeView::getTreeWidget();
    QList<QTreeWidgetItem*> treeItems = GTTreeWidget::getItems(treeWidget->invisibleRootItem());
    int annotationsCounter = 0;
    foreach (QTreeWidgetItem* item, treeItems) {
        QString treeItemName = GTUtilsAnnotationsTreeView::getAVItemName((AVItem*)item);
        if ("test" == treeItemName) {
            annotationsCounter++;
        }
    }

    CHECK_SET_ERR(3 == annotationsCounter, QString("Result count mismatch Expected= %1 Actual= %2").arg(3).arg(annotationsCounter));
}

}  // namespace GUITest_common_scenarios_sw_dialog

}  // namespace U2
