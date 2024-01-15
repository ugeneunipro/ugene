/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2024 UniPro <ugene@unipro.ru>
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
#include <base_dialogs/GTFileDialog.h>
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTAction.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTMenu.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTWidget.h>
#include <primitives/PopupChooser.h>
#include <system/GTClipboard.h>
#include <utils/GTKeyboardUtils.h>
#include <utils/GTUtilsDialog.h>

#include <QApplication>
#include <QCheckBox>

#include <U2View/MsaEditor.h>

#include "GTTestsMSAEditorEdit.h"
#include "GTUtilsMdi.h"
#include "GTUtilsMsaEditor.h"
#include "GTUtilsMsaEditorSequenceArea.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsTaskTreeView.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/DeleteGapsDialogFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/ExtractSelectedAsMSADialogFiller.h"

namespace U2 {
using namespace HI;

static void test_1(int i, QString expectedSec, int j = 0) {
    GTUtilsMSAEditorSequenceArea::click(QPoint(j, i));
    GTKeyboardDriver::keyClick(Qt::Key_Space);

    GTUtilsMSAEditorSequenceArea::click(QPoint(0, 1));
    if (j == 0) {
        GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, i), QPoint(14, i));
    } else {
        GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, i), QPoint(13, i));
    }

    GTKeyboardUtils::copy();
    QString clipboardTest = GTClipboard::text();

    CHECK_SET_ERR(clipboardTest == expectedSec, clipboardTest);
}

namespace GUITest_common_scenarios_msa_editor_edit {

GUI_TEST_CLASS_DEFINITION(test_0001) {
    // Check insert gaps
    // 1. Open document _common_data\scenarios\msa\ma2_gapped.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    test_1(0, "-AAGACTTCTTTTAA");
    // 2. Select first symbol for Phaneroptera_falcata sequence. Press "space".
    // Expected state: Phaneroptera_falcata -AAGACTTCTTTTAA, sequence length 15, right offset 15
}
GUI_TEST_CLASS_DEFINITION(test_0001_1) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    test_1(3, "-AAGTC---TATTAA");
    // DIFFERENCE:Select first symbol for Tettigonia_viridissima sequence. Press "space".
    // Expected state: Tettigonia_viridissima --AAGTC---TATTAA, sequence length 15, right offset 15
}
GUI_TEST_CLASS_DEFINITION(test_0001_2) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    test_1(6, "TAGCT-TATTAA--", 5);
    // DIFFERENCE:Select fifth symbol for Conocephalus_percaudata sequence. Press "space".
    // Expected state: Conocephalus_percaudata TAGCT-TATTAA--, sequence length 14, right offset 14
}

void test_3(int i = 0, QString expectedSec = "") {
    // 2. Select 6th symbol for Phaneroptera_falcata sequence. Use context menu {Edit->Insert gap}.
    GTUtilsMSAEditorSequenceArea::click(QPoint(13, i));

    auto seq = GTUtilsMSAEditorSequenceArea::getSequenceArea(0);
    GTUtilsDialog::waitForDialog(new PopupChooser({"MSAE_MENU_EDIT", "insert_gaps"}));
    GTMenu::showContextMenu(seq);

    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, i), QPoint(14, i));
    GTKeyboardUtils::copy();

    QString clipboardTest = GTClipboard::text();

    CHECK_SET_ERR(clipboardTest == expectedSec, clipboardTest);
    // Expected state: Phaneroptera_falcata AAGAC-TTCTTTTAA, sequence length 15, right offset 14
}

GUI_TEST_CLASS_DEFINITION(test_0003) {
    // 1. Open document _common_data\scenarios\msa\ma2_gapped.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    test_3(0, "AAGACTTCTTTTA-A");
    // Expected state: Phaneroptera_falcata AAGAC-TTCTTTTAA, sequence length 15, right offset 14
}

GUI_TEST_CLASS_DEFINITION(test_0003_1) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    test_3(3, "AAGTC---TATTA-A");
    // Expected state: DIFFERENCE: Tettigonia_viridissima AAGTC---TATTA-A, sequence length 15, right offset 14
}

GUI_TEST_CLASS_DEFINITION(test_0003_2) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    test_3(7, "AAGTCTTT---TA-A");
    // Expected state: DIFFERENCE: Mecopoda_elongata__Ishigaki__J AAGTCTTT---TA-A, sequence length 15, right offset 14
}

void test_4(int startPos, int endPos, const QString& expectedSeq, int i = 0, int context = 0) {
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(startPos, i), QPoint(endPos, i));

    if (0 != context) {
        auto seq = GTUtilsMSAEditorSequenceArea::getSequenceArea(0);
        GTUtilsDialog::waitForDialog(new PopupChooser({"MSAE_MENU_EDIT", "Remove selection"}));
        GTMenu::showContextMenu(seq);
    } else {
        GTKeyboardDriver::keyClick(Qt::Key_Delete);
    }

    const QString rowData = GTUtilsMSAEditorSequenceArea::getSequenceData(i);
    CHECK_SET_ERR(rowData == expectedSeq, "Incorrect row:" + rowData);
}

GUI_TEST_CLASS_DEFINITION(test_0004) {
    //    1. Open document _common_data\scenarios\msa\ma2_gapped.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Select 1..5 region for Phaneroptera_falcata sequence. Press "delete".
    //    Expected state: Phaneroptera_falcata TTCTTTTAA-----, sequence length 14, right offset 9
    test_4(0, 4, "TTCTTTTAA-----");
}

GUI_TEST_CLASS_DEFINITION(test_0004_1) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    Expected state: DIFFERENCE: Tettigonia_viridissima ---TATTAA-----, sequence length 14, right offset 9
    test_4(0, 4, "---TATTAA-----", 3);
}

GUI_TEST_CLASS_DEFINITION(test_0004_2) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    Expected state: DIFFERENCE: Mecopoda_elongata__Ishigaki__J TTT---TAA-----, sequence length 14, right offset 9
    test_4(0, 4, "TTT---TAA-----", 7);
}

GUI_TEST_CLASS_DEFINITION(test_0005) {
    //    1. Open document _common_data\scenarios\msa\ma2_gapped.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Select 6..9 symbol for Phaneroptera_falcata sequence. Use context menu {Edit->Delete selection}.
    //    Expected state: Phaneroptera_falcata AAGACTTTAA----, sequence length 14, right offsets 10
    test_4(5, 8, "AAGACTTTAA----");
}

GUI_TEST_CLASS_DEFINITION(test_0005_1) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    Expected state: DIFFERENCE: Tettigonia_viridissima AAGTCATTAA----, sequence length 14, right offset 10
    test_4(5, 8, "AAGTCATTAA----", 3);
}

GUI_TEST_CLASS_DEFINITION(test_0005_2) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    Expected state: DIFFERENCE: Mecopoda_elongata__Ishigaki__J AAGTC--TAA----, sequence length 14, right offset 10
    test_4(5, 8, "AAGTC--TAA----", 7);
}

GUI_TEST_CLASS_DEFINITION(test_0006) {
    //    1. Open document _common_data\scenarios\msa\ma2_gapped.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Select 13..14 symbol for Phaneroptera_falcata sequence. Use context menu {Edit->Delete selection}.
    //     Expected state: Phaneroptera_falcata AAGACTTCTTTT--, sequence length 14, right offsets 12
    test_4(12, 13, "AAGACTTCTTTT--", 0, 1);
}

GUI_TEST_CLASS_DEFINITION(test_0006_1) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    Expected state: DIFFERENCE: Tettigonia_viridissima AAGTC---TATT--, sequence length 14, right offsets 12
    test_4(12, 13, "AAGTC---TATT--", 3, 1);
}

GUI_TEST_CLASS_DEFINITION(test_0006_2) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    Expected state: DIFFERENCE: Mecopoda_elongata__Ishigaki__J AAGTCTTT---T--, sequence length 14, right offsets 12
    test_4(12, 13, "AAGTCTTT---T--", 7, 1);
}

GUI_TEST_CLASS_DEFINITION(test_0007) {
    //    Check remove all gaps
    //    1. Open document _common_data\scenarios\msa\ma2_gapped.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Use msa editor context menu {Edit->Remove all gaps}.
    auto seq = GTUtilsMSAEditorSequenceArea::getSequenceArea(0);
    GTUtilsDialog::waitForDialog(new PopupChooser({"MSAE_MENU_EDIT", "Remove all gaps"}));
    GTMenu::showContextMenu(seq);

    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 0), QPoint(13, 9));
    GTKeyboardUtils::copy();

    QString clipboardTest = GTClipboard::text();
    QString expectedSeq = QString("AAGACTTCTTTTAA\n"
                                  "AAGCTTACTAA---\n"
                                  "TAGTTTATTAA---\n"
                                  "AAGTCTATTAA---\n"
                                  "TAGCTTATTAA---\n"
                                  "TAGCTTATTAA---\n"
                                  "TAGCTTATTAA---\n"
                                  "AAGTCTTTTAA---\n"
                                  "AAGAATAATTA---\n"
                                  "AAGCCTTTTAA---");

    CHECK_SET_ERR(clipboardTest == expectedSeq, "\n Expected: \n" + expectedSeq + "\nFound:\n" + clipboardTest);
    /*    Expected state: length 14, right offsets 14
     *    Phaneroptera_falcata               AAGACTTCTTTTAA
     *    Isophya_altaica_EF540820           AAGCTTACTAA---
     *    Bicolorana_bicolor_EF540830        TAGTTTATTAA---
     *    Tettigonia_viridissima             AAGTCTATTAA---
     *    Conocephalus_discolor              TAGCTTATTAA---
     *    Conocephalus_sp.                   TAGCTTATTAA---
     *    Conocephalus_percaudata            TAGCTTATTAA---
     *    Mecopoda_elongata__Ishigaki__J     AAGTCTTTTAA---
     *    Podisma_sapporensis                AAGAATAATTA---
     *    Hetrodes_pupus_EF540832            AAGCCTTTTAA--- */
}

GUI_TEST_CLASS_DEFINITION(test_0007_1) {
    //    Check remove all gaps
    // DIFFERENSE:    1. Open document _common_data\scenarios\msa\ma2_gap_col.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gap_col.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Use msa editor context menu {Edit->Remove all gaps}.
    auto seq = GTUtilsMSAEditorSequenceArea::getSequenceArea(0);
    GTUtilsDialog::waitForDialog(new PopupChooser({"MSAE_MENU_EDIT", "Remove all gaps"}));
    GTMenu::showContextMenu(seq);

    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 0), QPoint(12, 9));
    GTKeyboardUtils::copy();

    QString clipboardTest = GTClipboard::text();
    QString expectedSeq = QString("AAGCTTCTTTTAA\n"
                                  "AAGTTACTAA---\n"
                                  "TAGTTATTAA---\n"
                                  "AAGCTATTAA---\n"
                                  "TAGTTATTAA---\n"
                                  "TAGTTATTAA---\n"
                                  "TAGTTATTAA---\n"
                                  "AAGCTTTTAA---\n"
                                  "AAGAATAATTA--\n"
                                  "AAGCTTTTAA---");

    CHECK_SET_ERR(clipboardTest == expectedSeq, "\n Expected: \n" + expectedSeq + "\nFound:\n" + clipboardTest);
    /*    Expected state: length 14, right offsets 14
     *    Phaneroptera_falcata               AAGCTTCTTTTAA
     *    Isophya_altaica_EF540820           AAGTTACTAA---
     *    Bicolorana_bicolor_EF540830        TAGTTATTAA---
     *    Tettigonia_viridissima             AAGCTATTAA---
     *    Conocephalus_discolor              TAGTTATTAA---
     *    Conocephalus_sp.                   TAGTTATTAA---
     *    Conocephalus_percaudata            TAGTTATTAA---
     *    Mecopoda_elongata__Ishigaki__J     AAGCTTTTAA---
     *    Podisma_sapporensis                AAGAATAATTA--
     *    Hetrodes_pupus_EF540832            AAGCTTTTAA--- */
}
GUI_TEST_CLASS_DEFINITION(test_0008) {
    // Check remove columns with gaps
    // 1. Open document _common_data\scenarios\msa\ma2_gap_col.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gap_col.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    // 2. Place cursor on 4th column of alignment. Use msa editor context menu (at the column with gaps) {Edit->Remove column of gaps}.
    GTUtilsMSAEditorSequenceArea::click(QPoint(3, 3));

    auto seq = GTUtilsMSAEditorSequenceArea::getSequenceArea(0);
    GTUtilsDialog::add(new PopupChooser({"MSAE_MENU_EDIT", "remove_columns_of_gaps"}));
    GTUtilsDialog::add(new DeleteGapsDialogFiller(1));
    GTMenu::showContextMenu(seq);

    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 0), QPoint(12, 9));
    GTKeyboardUtils::copy();

    QString clipboardTest = GTClipboard::text();
    QString expectedSeq = QString("AAGCTTCTTTTAA\n"
                                  "AAGTTACTAA---\n"
                                  "TAG---TTATTAA\n"
                                  "AAGC---TATTAA\n"
                                  "TAGTTATTAA---\n"
                                  "TAGTTATTAA---\n"
                                  "TAGTTATTAA---\n"
                                  "AAGCTTT---TAA\n"
                                  "A--AGAATAATTA\n"
                                  "AAGCTTTTAA---");

    CHECK_SET_ERR(clipboardTest == expectedSeq, "\n Expected: \n" + expectedSeq + "\nFound:\n" + clipboardTest);
    // Expected state: "Remove column of gaps" dialog appears

    // 3. Choose option "Delete all columns of gaps" and click "Delete" button
    /*Expected state: length 13, right offsets 13
Phaneroptera_falcata               AAGCTTCTTTTAA
Isophya_altaica_EF540820           AAGTTACTAA---
Bicolorana_bicolor_EF540830        TAG---TTATTAA
Tettigonia_viridissima             AAGC---TATTAA
Conocephalus_discolor              TAGTTATTAA---
Conocephalus_sp.                   TAGTTATTAA---
Conocephalus_percaudata            TAGTTATTAA---
Mecopoda_elongata__Ishigaki__J     AAGCTTT---TAA
Podisma_sapporensis                A--AGAATAATTA
Hetrodes_pupus_EF540832            AAGCTTTTAA---
*/
}

GUI_TEST_CLASS_DEFINITION(test_0008_1) {
    // Check remove columns with gaps
    // 1. Open document _common_data\scenarios\msa\ma2_gap_col.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gap_col.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    // 2. Place cursor on 4th column of alignment. Use msa editor context menu (at the column with gaps) {Edit->Remove column of gaps}.
    GTUtilsMSAEditorSequenceArea::click(QPoint(3, 3));

    // TODO: shift+delete doesn't work under windows

    //     QWidget* seq=GTUtilsMSAEditorSequenceArea::getSequenceArea(0);
    //     GTUtilsDialog::waitForDialog(new DeleteGapsDialogFiller(1));
    //     GTWidget::click(seq);
    //    GTKeyboardDriver::keyClick(Qt::Key_Delete,Qt::Key_Shift);
    auto seq = GTUtilsMSAEditorSequenceArea::getSequenceArea(0);
    GTUtilsDialog::add(new PopupChooser({"MSAE_MENU_EDIT", "remove_columns_of_gaps"}));
    GTUtilsDialog::add(new DeleteGapsDialogFiller(1));
    GTMenu::showContextMenu(seq);

    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 0), QPoint(12, 9));
    GTKeyboardUtils::copy();

    QString clipboardTest = GTClipboard::text();
    QString expectedSeq = QString("AAGCTTCTTTTAA\n"
                                  "AAGTTACTAA---\n"
                                  "TAG---TTATTAA\n"
                                  "AAGC---TATTAA\n"
                                  "TAGTTATTAA---\n"
                                  "TAGTTATTAA---\n"
                                  "TAGTTATTAA---\n"
                                  "AAGCTTT---TAA\n"
                                  "A--AGAATAATTA\n"
                                  "AAGCTTTTAA---");

    CHECK_SET_ERR(clipboardTest == expectedSeq, "\n Expected: \n" + expectedSeq + "\nFound:\n" + clipboardTest);
    // Expected state: "Remove column of gaps" dialog appears

    // 3. Choose option "Delete all columns of gaps" and click "Delete" button
    /*Expected state: length 13, right offsets 13
Phaneroptera_falcata               AAGCTTCTTTTAA
Isophya_altaica_EF540820           AAGTTACTAA---
Bicolorana_bicolor_EF540830        TAG---TTATTAA
Tettigonia_viridissima             AAGC---TATTAA
Conocephalus_discolor              TAGTTATTAA---
Conocephalus_sp.                   TAGTTATTAA---
Conocephalus_percaudata            TAGTTATTAA---
Mecopoda_elongata__Ishigaki__J     AAGCTTT---TAA
Podisma_sapporensis                A--AGAATAATTA
Hetrodes_pupus_EF540832            AAGCTTTTAA---
*/
}

void test_9(int i = 0) {
    auto seq = GTUtilsMSAEditorSequenceArea::getSequenceArea(0);
    QString gaps;

    if (0 != i) {
        GTUtilsMSAEditorSequenceArea::selectArea(QPoint(i, 0), QPoint(i, 9));
    } else {
        GTUtilsMSAEditorSequenceArea::click(QPoint(13, 0));
    }

    GTKeyboardDriver::keyClick(Qt::Key_Space);
    GTKeyboardDriver::keyClick(Qt::Key_Space);
    GTWidget::click(seq);

    // Expected state: two columns with gaps added to the end of sequence.
    if (0 != i) {
        GTUtilsMSAEditorSequenceArea::selectArea(QPoint(i, 0), QPoint(i + 1, 9));
        GTKeyboardUtils::copy();
        gaps = QString("--\n--\n--\n--\n--\n--\n--\n--\n--\n--");
    } else {
        GTUtilsMSAEditorSequenceArea::selectArea(QPoint(14, 0), QPoint(15, 9));
        GTKeyboardUtils::copy();
        gaps = QString("-A\n--\n--\n--\n--\n--\n--\n--\n--\n--");
    }

    const QString clipboardText = GTClipboard::text();
    CHECK_SET_ERR(clipboardText == gaps, "Expected:\n" + gaps + "\nFound:\n" + clipboardText);

    // 3. Move cursor at 15th symbol in first sequence. Use msa editor context menu {Edit->Delete column of gaps}.
    GTUtilsDialog::add(new PopupChooser({"MSAE_MENU_EDIT", "remove_columns_of_gaps"}));
    GTUtilsDialog::add(new DeleteGapsDialogFiller(1));
    GTMenu::showContextMenu(seq);

    //    Expected state:
    //    Phaneroptera_falcata               AAGACTTCTTTTA-A
    //    Isophya_altaica_EF540820           AAGCTTACTAA----
    //    Bicolorana_bicolor_EF540830        TAGT---TTATTAA-
    //    Tettigonia_viridissima             AAGTC---TATTAA-
    //    Conocephalus_discolor              TAGCTTATTAA----
    //    Conocephalus_sp.                   TAGCTTATTAA----
    //    Conocephalus_percaudata            TAGCTTATTAA----
    //    Mecopoda_elongata__Ishigaki__J     AAGTCTTT---TAA-
    //    Podisma_sapporensis                A---AGAATAATTA-
    //    Hetrodes_pupus_EF540832            AAGCCTTTTAA----
    const QStringList msa = GTUtilsMsaEditor::getWholeData();
    const QStringList expectedMsa = (0 != i ? QStringList() << "AAGACTTCTTTTAA"
                                                            << "AAGCTTACTAA---"
                                                            << "TAGT---TTATTAA"
                                                            << "AAGTC---TATTAA"
                                                            << "TAGCTTATTAA---"
                                                            << "TAGCTTATTAA---"
                                                            << "TAGCTTATTAA---"
                                                            << "AAGTCTTT---TAA"
                                                            << "A---AGAATAATTA"
                                                            << "AAGCCTTTTAA---"
                                            : QStringList() << "AAGACTTCTTTTA-A"
                                                            << "AAGCTTACTAA----"
                                                            << "TAGT---TTATTAA-"
                                                            << "AAGTC---TATTAA-"
                                                            << "TAGCTTATTAA----"
                                                            << "TAGCTTATTAA----"
                                                            << "TAGCTTATTAA----"
                                                            << "AAGTCTTT---TAA-"
                                                            << "A---AGAATAATTA-"
                                                            << "AAGCCTTTTAA----");
    CHECK_SET_ERR(msa == expectedMsa, "Expected:\n" + expectedMsa.join("\n") + "\nFound:\n" + msa.join("\n"));
}

GUI_TEST_CLASS_DEFINITION(test_0009) {
    // Check remove columns with gaps
    // 1. Open document _common_data\scenarios\msa\ma2_gapped.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Put cursor at last symbol in first sequence, click "Space" two times.
    test_9();
}

GUI_TEST_CLASS_DEFINITION(test_0009_1) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // DIFFERENCE: 2. Select column 3, click "Space" two times.
    test_9(2);
}

GUI_TEST_CLASS_DEFINITION(test_0009_2) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // DIFFERENCE: 2. Select column 9, click "Space" two times.
    test_9(8);
}

GUI_TEST_CLASS_DEFINITION(test_0010) {
    // UGENE crashes when edit multiply alignment (0001744)
    // 1. Open file samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    // 2. Open at least 2 views for multiple alignment
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gap_col.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 3. Try to edit MSA(insert spaces or something
    GTUtilsMSAEditorSequenceArea::click();
    for (int i = 0; i < 7; i++) {
        GTKeyboardDriver::keyClick(Qt::Key_Space);
    }
    // Expected state: UGENE not crash
}

GUI_TEST_CLASS_DEFINITION(test_0011) {
    // Check Undo/Redo functional
    // 1. Open document _common_data\scenarios\msa\ma.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    // 2. Select region 4..11 from Zychia_baranovi sequence. Press "Delete" button.
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(3, 8), QPoint(10, 8));
    GTKeyboardDriver::keyClick(Qt::Key_Delete);

    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 8), QPoint(11, 8));
    GTKeyboardUtils::copy();
    QString clipboardText = GTClipboard::text();
    GTWidget::click(GTUtilsMSAEditorSequenceArea::getSequenceArea());
    CHECK_SET_ERR(clipboardText == "TTAA--------", "\nExpected: TTAA--------\nFound:\n" + clipboardText);
    // Expected state: Zychia_baranovi TTAA

    // 3. Click Undo button on toolbar panel.
    QAbstractButton* undo = GTAction::button("msa_action_undo");
    GTWidget::click(undo);
    // Expected state: Zychia_baranovi TTAGATTATTAA
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 8), QPoint(11, 8));
    GTKeyboardUtils::copy();
    clipboardText = GTClipboard::text();
    GTWidget::click(GTUtilsMSAEditorSequenceArea::getSequenceArea());
    CHECK_SET_ERR(clipboardText == "TTAGATTATTAA", "\nExpected: TTAGATTATTAA\nFound:\n" + clipboardText);

    // 4. Click Redo button on toolbar panel.msa_action_redo
    QAbstractButton* redo = GTAction::button("msa_action_redo");
    GTWidget::click(redo);
    // Expected state: Zychia_baranovi TTAA
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 8), QPoint(11, 8));
    GTKeyboardUtils::copy();
    clipboardText = GTClipboard::text();
    CHECK_SET_ERR(clipboardText == "TTAA--------", "\nExpected: TTAA--------\nFound:\n" + clipboardText);
}
GUI_TEST_CLASS_DEFINITION(test_0011_1) {
    // Check Undo/Redo functional
    // 1. Open document _common_data\scenarios\msa\ma.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    // 2. Select region 4..11 from Zychia_baranovi sequence. Press "Delete" button.
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(3, 8), QPoint(10, 8));
    GTKeyboardDriver::keyClick(Qt::Key_Delete);

    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 8), QPoint(11, 8));
    GTKeyboardUtils::copy();
    QString clipboardText = GTClipboard::text();
    GTWidget::click(GTUtilsMSAEditorSequenceArea::getSequenceArea());
    CHECK_SET_ERR(clipboardText == "TTAA--------", "\nExpected: TTAA--------\nFound:\n" + clipboardText);
    // Expected state: Zychia_baranovi TTAA

    // DIFFERENCE: 3. Click ctrl+z
    GTKeyboardDriver::keyClick('z', Qt::ControlModifier);
    // Expected state: Zychia_baranovi TTAGATTATTAA
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 8), QPoint(11, 8));
    GTKeyboardUtils::copy();
    clipboardText = GTClipboard::text();
    GTWidget::click(GTUtilsMSAEditorSequenceArea::getSequenceArea());
    CHECK_SET_ERR(clipboardText == "TTAGATTATTAA", "\nExpected: TTAGATTATTAA\nFound:\n" + clipboardText);

    // 4.DIFFERENCE: 3. Click ctrl+y or ctrl+shift+z
    if (isOsWindows()) {
        GTKeyboardDriver::keyClick('y', Qt::ControlModifier);
    } else {
        GTKeyboardDriver::keyPress(Qt::Key_Shift);
        GTKeyboardDriver::keyClick('z', Qt::ControlModifier);
        GTKeyboardDriver::keyRelease(Qt::Key_Shift);
    }
    // Expected state: Zychia_baranovi TTAA
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 8), QPoint(11, 8));
    GTKeyboardUtils::copy();
    clipboardText = GTClipboard::text();
    CHECK_SET_ERR(clipboardText == "TTAA--------", "\nExpected: TTAA--------\nFound:\n" + clipboardText);
}

GUI_TEST_CLASS_DEFINITION(test_0011_2) {
    // Check Undo/Redo functional
    // 1. Open document _common_data\scenarios\msa\ma.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Use msa editor context menu {Edit->Remove all gaps}.
    auto seq = GTUtilsMSAEditorSequenceArea::getSequenceArea(0);
    GTUtilsDialog::waitForDialog(new PopupChooser({"MSAE_MENU_EDIT", "Remove all gaps"}));
    GTMenu::showContextMenu(seq);

    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 0), QPoint(13, 9));
    GTKeyboardUtils::copy();

    QString clipboardTest = GTClipboard::text();
    QString expectedSeq = QString("AAGACTTCTTTTAA\n"
                                  "AAGCTTACTAA---\n"
                                  "TAGTTTATTAA---\n"
                                  "AAGTCTATTAA---\n"
                                  "TAGCTTATTAA---\n"
                                  "TAGCTTATTAA---\n"
                                  "TAGCTTATTAA---\n"
                                  "AAGTCTTTTAA---\n"
                                  "AAGAATAATTA---\n"
                                  "AAGCCTTTTAA---");

    CHECK_SET_ERR(clipboardTest == expectedSeq, "\n Expected: \n" + expectedSeq + "\nFound:\n" + clipboardTest);
    // 3. Click Undo button on toolbar panel.
    QAbstractButton* undo = GTAction::button("msa_action_undo");
    GTWidget::click(undo);
    GTWidget::click(seq);

    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 0), QPoint(13, 9));
    GTKeyboardUtils::copy();

    clipboardTest = GTClipboard::text();
    QString modyfiedSeq = QString("AAGACTTCTTTTAA\n"
                                  "AAGCTTACTAA---\n"
                                  "TAGT---TTATTAA\n"
                                  "AAGTC---TATTAA\n"
                                  "TAGCTTATTAA---\n"
                                  "TAGCTTATTAA---\n"
                                  "TAGCTTATTAA---\n"
                                  "AAGTCTTT---TAA\n"
                                  "A---AGAATAATTA\n"
                                  "AAGCCTTTTAA---");

    CHECK_SET_ERR(clipboardTest == modyfiedSeq, "\n Expected: \n" + expectedSeq + "\nFound:\n" + clipboardTest);
    // 4. Click Redo button on toolbar panel.
    QAbstractButton* redo = GTAction::button("msa_action_redo");
    GTWidget::click(redo);
    GTWidget::click(seq);

    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 0), QPoint(13, 9));
    GTKeyboardUtils::copy();

    clipboardTest = GTClipboard::text();

    CHECK_SET_ERR(clipboardTest == expectedSeq, "\n Expected: \n" + expectedSeq + "\nFound:\n" + clipboardTest);
}

GUI_TEST_CLASS_DEFINITION(test_0011_3) {
    // Check Undo/Redo functional
    // 1. Open document _common_data\scenarios\msa\ma.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    // 2. Insert seversl spaces somewhere
    GTUtilsMSAEditorSequenceArea::click(QPoint(0, 0));
    for (int i = 0; i < 6; i++) {
        GTKeyboardDriver::keyClick(Qt::Key_Space);
    }
    // 3. Undo this
    for (int i = 0; i < 6; i++) {
        GTKeyboardDriver::keyClick('z', Qt::ControlModifier);
    }
    // Expected state: First sequwnce is AAGACTTCTTTTAA
    GTWidget::click(GTUtilsMSAEditorSequenceArea::getSequenceArea());

    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 0), QPoint(13, 0));
    GTKeyboardUtils::copy();

    QString clipboardTest = GTClipboard::text();
    QString expectedSeq = QString("AAGACTTCTTTTAA");
    CHECK_SET_ERR(clipboardTest == expectedSeq, "\n Expected: \n" + expectedSeq + "\nFound:\n" + clipboardTest);
}

GUI_TEST_CLASS_DEFINITION(test_0012) {
    // Check copy
    // 1. Open document _common_data\scenarios\msa\ma2_gapped.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    // 2. Select region 3..8 for Conocephalus_discolor sequence. Press Ctrl+C.
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(2, 4), QPoint(7, 4));
    GTKeyboardUtils::copy();
    // Expected state: GCTTAT has copied to clipboard
    QString clipboardTest = GTClipboard::text();
    CHECK_SET_ERR(clipboardTest == "GCTTAT", "\n Expected: \nGCTTAT\nFound:\n" + clipboardTest);
    // 3. Select region 6..12 for Conocephalus_discolor sequence. Use context menu {Copy->Copy selection}.
    GTWidget::click(GTUtilsMSAEditorSequenceArea::getSequenceArea());
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(5, 4), QPoint(11, 4));
    GTUtilsMSAEditorSequenceArea::copySelectionByContextMenu();

    // Expected state: TATTAA- has copied to clipboard
    clipboardTest = GTClipboard::text();
    CHECK_SET_ERR(clipboardTest == "TATTAA-", "\n Expected: \nTATTAA-\nFound:\n" + clipboardTest);
}

static void test_13(int comboVal, int spinVal, const QString& expectedCons) {
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();
    auto seq = GTUtilsMSAEditorSequenceArea::getSequenceArea(0);

    GTUtilsDialog::waitForDialog(new PopupChooser({"Consensus mode"}, GTGlobals::UseMouse));
    GTMenu::showContextMenu(seq);

    auto consensusCombo = GTWidget::findComboBox("consensusType");
    GTComboBox::selectItemByIndex(consensusCombo, comboVal);

    auto thresholdSpinBox = GTWidget::findSpinBox("thresholdSpinBox");
    GTSpinBox::setValue(thresholdSpinBox, spinVal, GTGlobals::UseKeyBoard);

    GTUtilsDialog::waitForDialog(new PopupChooser({"MSAE_MENU_COPY", "Copy consensus"}, GTGlobals::UseMouse));
    GTMenu::showContextMenu(seq);
    GTUtilsDialog::checkNoActiveWaiters();
    GTUtilsTaskTreeView::waitTaskFinished();

    QString clipboardText = GTClipboard::text();
    CHECK_SET_ERR(clipboardText == expectedCons, "\n Expected: \n" + expectedCons + "\nFound:\n" + clipboardText);
}

GUI_TEST_CLASS_DEFINITION(test_0013) {
    // 1. Open document _common_data\scenarios\msa\ma2_gapped.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    test_13(1, 100, "aagc+tattaataa");
}

GUI_TEST_CLASS_DEFINITION(test_0013_1) {
    // 1. Open document _common_data\scenarios\msa\ma2_gapped.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    test_13(1, 1, "AAGC+TATTAATAA");
}

GUI_TEST_CLASS_DEFINITION(test_0013_2) {
    // 1. Open document _common_data\scenarios\msa\ma2_gapped.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    test_13(2, 75, "WAGYYKWYTAW");
}

GUI_TEST_CLASS_DEFINITION(test_0014) {
    // Create subaligniment
    // 1. Open document samples\CLUSTALW\COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    // 2. Right click on aligniment view. Open context menu {Export->save subalignment}
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 5), QPoint(10, 5));
    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_EXPORT, "Save subalignment"}));
    GTUtilsDialog::add(new ExtractSelectedAsMSADialogFiller(testDir + "_common_data/scenarios/sandbox/result.aln", {"Zychia_baranovi", "Montana_montana"}, 6, 16));
    GTMenu::showContextMenu(GTUtilsMSAEditorSequenceArea::getSequenceArea(0));
    // Expected state: Create subaligniment dialog has appeared

    // 3. Fill the next fields in dialog:
    //     {Subaligniment start:} 6
    //     {End:} 16
    //     {Selected sequences:} set checked: Montana_montana, Zychia_baranovi
    //     {Path:} _common_data/sandbox/result.aln
    //     {Add to project:} set checked
    // 4. Click OK
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("result.aln"));
    GTMouseDriver::doubleClick();
    // Expected result: subaligniment created and added to project

    // 5. Open result.aln in aligniment viewer
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 0), QPoint(10, 1));
    GTKeyboardUtils::copy();

    QString clipboardText = GTClipboard::text();
    CHECK_SET_ERR(clipboardText == "TTATTAATTCG\nTTATTAATCCG", "clipboardText is:\n" + clipboardText);
    // Expected result: aligniment with 2 sequences appear in editor:
    // Montana_montana TTATTAATTCG
    // Zychia_baranovi TTATTAATCCG
}

GUI_TEST_CLASS_DEFINITION(test_0015) {
    // Create subaligniment
    // 1. Open document "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Right click on aligniment view. Open context menu {Export->save subalignment}

    class Scenario : public CustomScenario {
        void run() {
            // Expected state: Create subaligniment dialog has appeared, all sequences are checked
            QWidget* dialog = GTWidget::getActiveModalWidget();
            auto table = GTWidget::findTableWidget("sequencesTableWidget", dialog);

            for (int i = 0; i < table->rowCount(); i++) {
                auto box = qobject_cast<QCheckBox*>(table->cellWidget(i, 0));
                CHECK_SET_ERR(box->isChecked(), QString("box '%1' is not checked").arg(box->text()));
            }

            // 3. Click button Clean selection
            GTWidget::click(GTWidget::findWidget("noneButton"));

            // Expected result: all sequences unchecked
            for (int i = 0; i < table->rowCount(); i++) {
                auto box = qobject_cast<QCheckBox*>(table->cellWidget(i, 0));
                CHECK_SET_ERR(!box->isChecked(), QString("box '%1' is checked").arg(box->text()));
            }

            // 4. Check sequences Montana_montana, Zychia_baranovi
            const QPoint p = dialog->mapToGlobal(table->geometry().topRight() + QPoint(-2, 2));
            GTMouseDriver::moveTo(p);
            GTMouseDriver::doubleClick();

            const QStringList list = {"Zychia_baranovi", "Montana_montana"};
            for (int i = 0; i < table->rowCount(); i++) {
                GTKeyboardDriver::keyClick(Qt::Key_Down);
                auto box = qobject_cast<QCheckBox*>(table->cellWidget(i, 0));
                if (list.contains(box->text())) {
                    GTKeyboardDriver::keyClick(Qt::Key_Space);
                }
            }

            // 5. Click button Invert selection
            GTWidget::click(GTWidget::findWidget("invertButton", dialog));

            // Expected result: all sequences except Montana_montana, Zychia_baranovi are checked
            for (int i = 1; i < table->rowCount(); i++) {
                auto box = qobject_cast<QCheckBox*>(table->cellWidget(i, 0));
                if (list.contains(box->text())) {
                    CHECK_SET_ERR(!box->isChecked(), QString("box '%1' is checked. It must be unchecked").arg(box->text()));
                } else {
                    CHECK_SET_ERR(box->isChecked(), QString("box '%1' is unchecked. It must be cheked").arg(box->text()));
                }
            }

            // 6. Click button Select all
            GTWidget::click(GTWidget::findWidget("allButton"));

            // Expected result: all sequences are checked
            for (int i = 0; i < table->rowCount(); i++) {
                auto box = qobject_cast<QCheckBox*>(table->cellWidget(i, 0));
                CHECK_SET_ERR(box->isChecked(), QString("box '%1' is not checked").arg(box->text()));
            }

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_EXPORT, "Save subalignment"}));
    GTUtilsDialog::add(new ExtractSelectedAsMSADialogFiller(new Scenario));
    GTUtilsMSAEditorSequenceArea::callContextMenu();
}

GUI_TEST_CLASS_DEFINITION(test_0016) {
    // 1. Open document "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Select area from (0, 0) to (10, 10)
    GTUtilsMSAEditorSequenceArea::selectArea({0, 0}, {10, 10});

    // 3. Click "Replace with gaps" with popup menu
    GTUtilsDialog::waitForDialog(new PopupChooser({"MSAE_MENU_EDIT", "replace_with_gaps"}));
    GTMenu::showContextMenu(GTUtilsMSAEditorSequenceArea::getSequenceArea(0));

    // Expected: selection hasn't been changed
    auto check = [&]() {
        auto selectedRect = GTUtilsMSAEditorSequenceArea::getSelectedRect();
        auto tl = selectedRect.topLeft();
        CHECK_SET_ERR(tl.x() == 11 && tl.y() == 0, QString("Expected top-left selection: 11, 0; current: %1, %2").arg(tl.x()).arg(tl.y()));

        auto br = selectedRect.bottomRight();
        CHECK_SET_ERR(br.x() == 21 && br.y() == 10, QString("Expected bottom-right selection: 21, 0; current: %1, %2").arg(br.x()).arg(br.y()));

        const QStringList GAPPED_DATA = {
            "-----------ATTCGAGCCGA",
            "-----------ATCCGGGCCGA",
            "-----------ATTCGAGCTGA",
            "-----------ATTCGAGCAGA",
            "-----------ATTCGAGCTGA",
            "-----------ATTCGAGCTGA",
            "-----------ATTCGAGCTGA",
            "-----------ATTCGAGCCGA",
            "-----------ATCCGGGCTGA",
            "-----------ATTCGAGCTGA",
            "-----------ATTCGTGCTGA"};

        GTUtilsMSAEditorSequenceArea::selectArea({0, 0}, {22, 10});
        GTKeyboardUtils::copy();
        auto result = GTClipboard::text().split("\n");
        for (int i = 0; i < 11; i++) {
            CHECK_SET_ERR(result[i].startsWith(GAPPED_DATA[i]), QString("Expected sequence beginning: %1; current: %2").arg(GAPPED_DATA[i]).arg(result[i].left(20)));
        }
    };
    check();

    // 4. Click undo
    GTWidget::click(GTAction::button("msa_action_undo"));
    GTWidget::click(GTAction::button("msa_action_undo"));

    // 5. Select area from (0, 0) to (10, 10)
    GTUtilsMSAEditorSequenceArea::selectArea({0, 0}, {10, 10});

    // 6. Click "Replace with gaps" with shortcut
    GTKeyboardDriver::keyClick(Qt::Key_Space, Qt::ShiftModifier);

    // Expected: selection hasn't been changed
    check();
}

}  // namespace GUITest_common_scenarios_msa_editor_edit
}  // namespace U2
