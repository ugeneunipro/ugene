/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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
#include <primitives/GTComboBox.h>
#include <primitives/GTMenu.h>
#include <primitives/GTTreeWidget.h>
#include <primitives/PopupChooser.h>
#include <system/GTClipboard.h>
#include <utils/GTKeyboardUtils.h>
#include <utils/GTUtilsDialog.h>

#include <QApplication>

#include <U2Core/AppContext.h>
#include <U2Core/ExternalToolRegistry.h>

#include <U2View/MSAEditor.h>

#include "GTTestsAlignSequenceToMsa.h"
#include "GTUtilsExternalTools.h"
#include "GTUtilsLog.h"
#include "GTUtilsMdi.h"
#include "GTUtilsMsaEditor.h"
#include "GTUtilsMsaEditorSequenceArea.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsTaskTreeView.h"
#include "runnables/ugene/corelibs/U2Gui/PositionSelectorFiller.h"

namespace U2 {
namespace GUITest_common_scenarios_align_sequences_to_msa {
using namespace HI;

static void checkAlignedRegion(HI::GUITestOpStatus& os, const QRect& selectionRect, const QString& expectedContent) {
    GTUtilsDialog::waitForDialog(os, new GoToDialogFiller(os, selectionRect.center().x()));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {MSAE_MENU_NAVIGATION, "action_go_to_position"}));
    GTMenu::showContextMenu(os, GTUtilsMdi::activeWindow(os));

    GTUtilsMSAEditorSequenceArea::selectArea(os, selectionRect.topLeft(), selectionRect.bottomRight());
    GTKeyboardUtils::copy();

    const QString clipboardText = GTClipboard::text(os);
    CHECK_SET_ERR(clipboardText == expectedContent, QString("Incorrect alignment of the region\n Expected: \n%1 \nResult: \n%2").arg(expectedContent).arg(clipboardText));
}

GUI_TEST_CLASS_DEFINITION(test_0001) {
    // Try to delete the MSA object during aligning
    // Expected state: the sequences are locked and and can not be deleted
    GTLogTracer logTracer;
    GTFileDialog::openFile(os, testDir + "_common_data/clustal/", "3000_sequences.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTFileDialog::openFile(os, testDir + "_common_data/alignment/align_sequence_to_an_alignment/", "tub1.txt");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsProjectTreeView::click(os, "tub");
    GTUtilsMdi::activateWindow(os, "3000_sequences [3000_sequences.aln]");

    GTUtilsMsaEditor::activateAlignSequencesToAlignmentMenu(os, "MAFFT");

    GTUtilsProjectTreeView::click(os, "tub1.txt");
    GTKeyboardDriver::keyClick(Qt::Key_Delete);

    bool hasMessage = GTLogTracer::checkMessage("Cannot remove document tub1.txt");
    CHECK_SET_ERR(hasMessage, "The expected message is not found in the log");

    GTUtilsTaskTreeView::waitTaskFinished(os);

    CHECK_SET_ERR(GTUtilsMsaEditor::getSequencesCount(os) == 3086, "Incorrect sequences count");
}

GUI_TEST_CLASS_DEFINITION(test_0002) {
    // Try to delete the MSA object during aligning
    // Expected state: the MSA object is locked and and can not be deleted
    GTLogTracer logTracer;
    GTFileDialog::openFile(os, testDir + "_common_data/clustal/", "3000_sequences.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTFileDialog::openFile(os, testDir + "_common_data/alignment/align_sequence_to_an_alignment/", "tub1.txt");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsProjectTreeView::click(os, "tub");
    GTUtilsMdi::activateWindow(os, "3000_sequences [3000_sequences.aln]");

    GTUtilsMsaEditor::activateAlignSequencesToAlignmentMenu(os, "MAFFT");

    GTUtilsProjectTreeView::click(os, "3000_sequences.aln");
    GTKeyboardDriver::keyClick(Qt::Key_Delete);

    bool hasMessage = GTLogTracer::checkMessage("Cannot remove document 3000_sequences.aln");
    CHECK_SET_ERR(hasMessage, "The expected message is not found in the log");

    GTUtilsTaskTreeView::waitTaskFinished(os);

    CHECK_SET_ERR(GTUtilsMsaEditor::getSequencesCount(os) == 3086, "Incorrect sequences count");
}

GUI_TEST_CLASS_DEFINITION(test_0003) {
    // Align short sequences with default settings(on platforms with MAFFT)
    // Expected state: MAFFT alignment started and finished successfully with using option --addfragments
    GTLogTracer logTracer;

    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, testDir + "_common_data/cmdline/primers/", "primers.fa"));

    GTUtilsMsaEditor::activateAlignSequencesToAlignmentMenu(os, "MAFFT");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    CHECK_SET_ERR(GTUtilsMsaEditor::getSequencesCount(os) == 152, "Incorrect sequences count");

    bool hasMessage = GTLogTracer::checkMessage("--addfragments");
    CHECK_SET_ERR(hasMessage, "The expected message is not found in the log");

    checkAlignedRegion(os, QRect(QPoint(86, 17), QPoint(114, 23)), QString("CATGCCTTTGTAATAATCTTCTTTATAGT\n"
                                                                           "-----------------------------\n"
                                                                           "-----------------------------\n"
                                                                           "CTATCCTTCGCAAGACCCTTC--------\n"
                                                                           "-----------------------------\n"
                                                                           "-----------------------------\n"
                                                                           "---------ATAATACCGCGCCACATAGC"));
}

GUI_TEST_CLASS_DEFINITION(test_0004) {
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, testDir + "_common_data/cmdline/primers/", "primers.fa"));

    GTUtilsMsaEditor::activateAlignSequencesToAlignmentMenu(os, "UGENE");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    CHECK_SET_ERR(GTUtilsMsaEditor::getSequencesCount(os) == 152, "Incorrect sequences count");

    checkAlignedRegion(os, QRect(QPoint(51, 17), QPoint(71, 19)), QString("GTGATAGTCAAATCTATAATG\n"
                                                                          "---------------------\n"
                                                                          "GACTGGTTCCAATTGACAAGC"));
}

GUI_TEST_CLASS_DEFINITION(test_0005) {
    GTFileDialog::openFile(os, testDir + "_common_data/alignment/align_sequence_to_an_alignment/", "TUB.msf");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QStringList fileList = {"tub1.txt", "tub3.txt"};
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils_list(os, testDir + "_common_data/alignment/align_sequence_to_an_alignment/", fileList));

    GTUtilsMsaEditor::activateAlignSequencesToAlignmentMenu(os, "MAFFT");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    CHECK_SET_ERR(GTUtilsMsaEditor::getSequencesCount(os) == 17, "Incorrect sequences count");

    checkAlignedRegion(os, QRect(QPoint(970, 7), QPoint(985, 15)), QString("TTCCCAGGTCAGCTCA\n"
                                                                           "----------------\n"
                                                                           "----------------\n"
                                                                           "----------------\n"
                                                                           "----------------\n"
                                                                           "----------------\n"
                                                                           "----------------\n"
                                                                           "----------------\n"
                                                                           "TTCCCAGGTCAGCTCA"));

    checkAlignedRegion(os, QRect(QPoint(875, 7), QPoint(889, 16)), QString("TCTGCTTCCGTACAC\n"
                                                                           "---------------\n"
                                                                           "---------------\n"
                                                                           "--------CGTACAC\n"
                                                                           "---------------\n"
                                                                           "---------------\n"
                                                                           "---------------\n"
                                                                           "---------------\n"
                                                                           "---------------\n"
                                                                           "TCTGCTTCCGTACAC"));
}

GUI_TEST_CLASS_DEFINITION(test_0006) {
    GTFileDialog::openFile(os, testDir + "_common_data/alignment/align_sequence_to_an_alignment/", "TUB.msf");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QStringList fileList = {"tub1.txt", "tub3.txt"};
    GTFileDialogUtils_list* ob = new GTFileDialogUtils_list(os, testDir + "_common_data/alignment/align_sequence_to_an_alignment/", fileList);
    GTUtilsDialog::waitForDialog(os, ob);

    GTUtilsMsaEditor::activateAlignSequencesToAlignmentMenu(os, "UGENE");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    CHECK_SET_ERR(GTUtilsMsaEditor::getSequencesCount(os) == 17, "Incorrect sequences count");

    checkAlignedRegion(os, QRect(QPoint(970, 7), QPoint(985, 15)), QString("TTCCCAGGTCAGCTCA\n"
                                                                           "----------------\n"
                                                                           "----------------\n"
                                                                           "----------------\n"
                                                                           "----------------\n"
                                                                           "----------------\n"
                                                                           "----------------\n"
                                                                           "----------------\n"
                                                                           "TTCCCAGGTCAGCTCA"));

    checkAlignedRegion(os, QRect(QPoint(875, 7), QPoint(889, 16)), QString("TCTGCTTCCGTACAC\n"
                                                                           "---------------\n"
                                                                           "---------------\n"
                                                                           "--------CGTACAC\n"
                                                                           "---------------\n"
                                                                           "---------------\n"
                                                                           "---------------\n"
                                                                           "---------------\n"
                                                                           "---------------\n"
                                                                           "TCTGCTTCCGTACAC"));
}

GUI_TEST_CLASS_DEFINITION(test_0007) {
    // Do not select anything in the project. Click the button. Add a sequence in GenBank format.
    // Expected state: The sequence was added to the alignment and aligned.
    GTLogTracer logTracer;
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, dataDir + "samples/Genbank/", "CVU55762.gb"));

    GTUtilsMsaEditor::activateAlignSequencesToAlignmentMenu(os, "MAFFT");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    CHECK_SET_ERR(GTUtilsMsaEditor::getSequencesCount(os) == 19, "Incorrect sequences count");
}

GUI_TEST_CLASS_DEFINITION(test_0008) {
    // Do not select anything in the project. Click the button. Add several ABI files.
    // Expected state: The sequences were added to the alignment and aligned
    GTLogTracer logTracer;
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QStringList fileList = {"39_034.ab1", "19_022.ab1", "25_032.ab1"};
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils_list(os, testDir + "_common_data/abif/", fileList));

    GTUtilsMsaEditor::activateAlignSequencesToAlignmentMenu(os, "MAFFT");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    CHECK_SET_ERR(GTUtilsMsaEditor::getSequencesCount(os) == 21, "Incorrect sequences count");
}

GUI_TEST_CLASS_DEFINITION(test_0009) {
    // Do not select anything in the project. Click the button. Add sequences in ClustalW format. Uncheck several sequences in the appeared dialog.
    // Expected state: Only checked sequences were added to the alignment.
    GTLogTracer logTracer;
    GTFileDialog::openFile(os, testDir + "_common_data/alignment/align_sequence_to_an_alignment/", "TUB.msf");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, testDir + "_common_data/clustal/", "COI na.aln"));

    GTUtilsMsaEditor::activateAlignSequencesToAlignmentMenu(os, "MAFFT");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    CHECK_SET_ERR(GTUtilsMsaEditor::getSequencesCount(os) == 33, "Incorrect sequences count");
}

GUI_TEST_CLASS_DEFINITION(test_0010) {
    // 1. Open "_common_data/fasta/empty.fa" as msa.
    // 2. Ensure that MAFFT tool is set.
    GTFileDialog::openFile(os, testDir + "_common_data/fasta/empty.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    int sequenceCount = GTUtilsMsaEditor::getSequencesCount(os);
    CHECK_SET_ERR(sequenceCount == 2, "1. Incorrect sequences count: " + QString::number(sequenceCount));

    // 3. Click "align_new_sequences_to_alignment_action" button on the toolbar.
    // 4. Select "data/samples/FASTQ/eas.fastq".
    GTUtilsMsaEditor::checkAlignSequencesToAlignmentMenu(os, "MAFFT", PopupChecker::IsDisabled);
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, dataDir + "samples/FASTQ/eas.fastq"));
    GTUtilsMsaEditor::activateAlignSequencesToAlignmentMenu(os, "UGENE");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: three sequences are added to the msa.
    sequenceCount = GTUtilsMsaEditor::getSequencesCount(os);
    CHECK_SET_ERR(sequenceCount == 5, "2. Incorrect sequences count: " + QString::number(sequenceCount));
}

GUI_TEST_CLASS_DEFINITION(test_0011) {
    // Adding and aligning with MAFFT a sequence, which is longer than an alignment.

    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/ma.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //  Click "align_new_sequences_to_alignment_action" button on the toolbar.
    //  Select "_common_data/scenarios/add_and_align/add_and_align_1.fa" in the dialog.
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, testDir + "_common_data/scenarios/add_and_align/add_and_align_1.fa"));
    GTUtilsMsaEditor::activateAlignSequencesToAlignmentMenu(os, "MAFFT");

    //    Expected state: an additional row appeared in the alignment, all old rows were shifted to be aligned with the new row.
    QStringList expectedMsaData = {"----TAAGACTTCTAA------------",
                                   "----TAAGCTTACTAA------------",
                                   "----TTAGTTTATTAA------------",
                                   "----TCAGTCTATTAA------------",
                                   "----TCAGTTTATTAA------------",
                                   "----TTAGTCTACTAA------------",
                                   "----TCAGATTATTAA------------",
                                   "----TTAGATTGCTAA------------",
                                   "----TTAGATTATTAA------------",
                                   "----TAAGTCTATTAA------------",
                                   "----TTAGCTTATTAA------------",
                                   "----TTAGCTTATTAA------------",
                                   "----TTAGCTTATTAA------------",
                                   "----TAAGTCTTTTAA------------",
                                   "----TAAGTCTTTTAA------------",
                                   "----TAAGTCTTTTAA------------",
                                   "----TAAGAATAATTA------------",
                                   "----TAAGCCTTTTAA------------",
                                   "GCGCTAAGCCTTTTAAGCGCGCGCGCGC"};
    GTUtilsTaskTreeView::waitTaskFinished(os);
    QStringList msaData = GTUtilsMsaEditor::getWholeData(os);
    CHECK_SET_ERR(expectedMsaData == msaData, "Expected:\n" + expectedMsaData.join("\n") + "\nFound:\n" + msaData.join("\n"));
}

GUI_TEST_CLASS_DEFINITION(test_0012) {
    // Adding and aligning with MAFFT a sequence, which can be aligned with an alignment shifting

    // Open "_common_data/scenarios/msa/ma.aln".
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/ma.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Click "align_new_sequences_to_alignment_action" button on the toolbar.
    // Select "_common_data/scenarios/add_and_align/add_and_align_2.fa" in the dialog.
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, testDir + "_common_data/scenarios/add_and_align/add_and_align_2.fa"));
    GTUtilsMsaEditor::activateAlignSequencesToAlignmentMenu(os, "MAFFT");

    //    Expected state: an additional row appeared in the alignment, all old rows were shifted to be aligned with the new row.
    QStringList expectedMsaData = {"------TAAGACTTCTAA",
                                   "------TAAGCTTACTAA",
                                   "------TTAGTTTATTAA",
                                   "------TCAGTCTATTAA",
                                   "------TCAGTTTATTAA",
                                   "------TTAGTCTACTAA",
                                   "------TCAGATTATTAA",
                                   "------TTAGATTGCTAA",
                                   "------TTAGATTATTAA",
                                   "------TAAGTCTATTAA",
                                   "------TTAGCTTATTAA",
                                   "------TTAGCTTATTAA",
                                   "------TTAGCTTATTAA",
                                   "------TAAGTCTTTTAA",
                                   "------TAAGTCTTTTAA",
                                   "------TAAGTCTTTTAA",
                                   "------TAAGAATAATTA",
                                   "------TAAGCCTTTTAA",
                                   "GCGCGCTAAGCC------"};
    GTUtilsTaskTreeView::waitTaskFinished(os);
    QStringList msaData = GTUtilsMsaEditor::getWholeData(os);
    CHECK_SET_ERR(expectedMsaData == msaData, "Unexpected MSA data");
}

GUI_TEST_CLASS_DEFINITION(test_0013) {
    // Adding and aligning with MAFFT a sequence to an alignment with columns of gaps

    // Open "_common_data/scenarios/msa/ma2_gap_8_col.aln".
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/ma2_gap_8_col.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Click "align_new_sequences_to_alignment_action" button on the toolbar.
    // Select "_common_data/scenarios/add_and_align/add_and_align_1.fa" in the dialog.
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, testDir + "_common_data/scenarios/add_and_align/add_and_align_1.fa"));
    GTUtilsMsaEditor::activateAlignSequencesToAlignmentMenu(os, "MAFFT");

    // Expected state: an additional row appeared in the alignment, all old rows were shifted to be aligned with the new row, columns with gaps were removed
    const QStringList expectedMsaData = {"-----AAGCTTCTTTTAA----------",
                                         "-----AAGTTACTAA-------------",
                                         "-----TAG---TTATTAA----------",
                                         "-----AAGC---TATTAA----------",
                                         "-----TAGTTATTAA-------------",
                                         "-----TAGTTATTAA-------------",
                                         "-----TAGTTATTAA-------------",
                                         "-----AAGCTTT---TAA----------",
                                         "-----A--AGAATAATTA----------",
                                         "-----AAGCTTTTAA-------------",
                                         "GCGCTAAGCCTTTTAAGCGCGCGCGCGC"};
    GTUtilsTaskTreeView::waitTaskFinished(os);
    QStringList msaData = GTUtilsMsaEditor::getWholeData(os);
    CHECK_SET_ERR(expectedMsaData == msaData, "Unexpected MSA data");
}

GUI_TEST_CLASS_DEFINITION(test_0014) {
    // Adding and aligning with MAFFT should remove all columns of gaps from the source msa before the aligning, also it should be trimmed after the aligning.

    // Open "_common_data/scenarios/msa/ma2_gap_8_col.aln".
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/ma2_gap_8_col.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Click "align_new_sequences_to_alignment_action" button on the toolbar.
    // Select "_common_data/scenarios/add_and_align/add_and_align_3.fa" in the dialog.
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, testDir + "_common_data/scenarios/add_and_align/add_and_align_3.fa"));
    GTUtilsMsaEditor::activateAlignSequencesToAlignmentMenu(os, "MAFFT");

    //    Expected state: an additional row appeared in the alignment, the forth column doesn't consist only of gaps, there are no columns of gaps even in the end of the alignment.
    QStringList expectedMsaData = {"AAGCTTCTTTTAA",
                                   "AAGTTACTAA---",
                                   "TAG---TTATTAA",
                                   "AAGC---TATTAA",
                                   "TAGTTATTAA---",
                                   "TAGTTATTAA---",
                                   "TAGTTATTAA---",
                                   "AAGCTTT---TAA",
                                   "A--AGAATAATTA",
                                   "AAGCTTTTAA---",
                                   "AAGAATA------"};
    GTUtilsTaskTreeView::waitTaskFinished(os);
    QStringList msaData = GTUtilsMsaEditor::getWholeData(os);
    CHECK_SET_ERR(expectedMsaData == msaData, "Unexpected MSA data");
}

GUI_TEST_CLASS_DEFINITION(test_0015) {
    //    Adding and aligning without MAFFT should remove all columns of gaps from the source msa before the aligning, also it should be trimmed after the aligning.

    //    2. Ensure that MAFFT tool is not set. Remove it, if it is set.
    GTUtilsExternalTools::removeTool(os, "MAFFT");

    //    1. Open "_common_data/scenarios/msa/ma2_gap_8_col.aln".
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/ma2_gap_8_col.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    3. Click "align_new_sequences_to_alignment_action" button on the toolbar.
    GTUtilsMsaEditor::checkAlignSequencesToAlignmentMenu(os, "MAFFT", PopupChecker::NotExists);
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, testDir + "_common_data/scenarios/add_and_align/add_and_align_3.fa"));
    GTUtilsMsaEditor::activateAlignSequencesToAlignmentMenu(os, "UGENE");

    //    4. Select "_common_data/scenarios/add_and_align/add_and_align_3.fa" in the dialog.
    //    Expected state: an additional row appeared in the alignment, the forth column doesn't consist only of gaps, there are no columns of gaps even in the end of the alignment.
    QStringList expectedMsaData = {"AAGCTTCTTTTAA",
                                   "AAGTTACTAA---",
                                   "TAG---TTATTAA",
                                   "AAGC---TATTAA",
                                   "TAGTTATTAA---",
                                   "TAGTTATTAA---",
                                   "TAGTTATTAA---",
                                   "AAGCTTT---TAA",
                                   "A--AGAATAATTA",
                                   "AAGCTTTTAA---",
                                   "AAGAATA------"};
    GTUtilsTaskTreeView::waitTaskFinished(os);
    QStringList msaData = GTUtilsMsaEditor::getWholeData(os);
    CHECK_SET_ERR(expectedMsaData == msaData, "Unexpected MSA data");
}

GUI_TEST_CLASS_DEFINITION(test_0016_1) {
    // Sequences with length less or equal than 50 should be aligned without gaps, even the result alignment is worse in this case.

    //    1. Open "_common_data/clustal/COI na.aln".
    GTFileDialog::openFile(os, testDir + "_common_data/clustal/COI na.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Click "align_new_sequences_to_alignment_action" button on the toolbar.
    //    3. Select "_common_data/scenarios/add_and_align/seq1.fa" as sequence to align.
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, testDir + "_common_data/scenarios/add_and_align/seq1.fa"));
    GTUtilsMsaEditor::activateAlignSequencesToAlignmentMenu(os, "MAFFT");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: the new sequence doesn't have gaps within the sequence data.
    QString expectedRowData = "---------TAATTCGTTCAGAACTAAGACAACCCGGTGTACTTTTATTGGTGATAGTC-----------";
    QString actualRowData = GTUtilsMSAEditorSequenceArea::getSequenceData(os, 18).left(expectedRowData.length());
    CHECK_SET_ERR(expectedRowData == actualRowData, QString("Unexpected row data: expected '%1', got '%2'").arg(expectedRowData).arg(actualRowData));
}

GUI_TEST_CLASS_DEFINITION(test_0016_2) {
    // Sequences with length greater than 50 should be aligned with gaps

    // Open "_common_data/clustal/COI na.aln".
    GTFileDialog::openFile(os, testDir + "_common_data/clustal/COI na.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Click "align_new_sequences_to_alignment_action" button on the toolbar.
    // Select "_common_data/scenarios/add_and_align/seq2.fa" as sequence to align.
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, testDir + "_common_data/scenarios/add_and_align/seq2.fa"));
    GTUtilsMsaEditor::activateAlignSequencesToAlignmentMenu(os, "MAFFT");

    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: the new sequence has a gap within the sequence data.
    QString expectedRowData = "---------TAATTCGTTCAGAACTAAGACAACCCGG-TGTACTTTTATTGGTGATAGTCA---------";
    QString actualRowData = GTUtilsMSAEditorSequenceArea::getSequenceData(os, 18).left(expectedRowData.length());
    CHECK_SET_ERR(expectedRowData == actualRowData, QString("Unexpected row data: expected '%1', got '%2'").arg(expectedRowData).arg(actualRowData));
}

GUI_TEST_CLASS_DEFINITION(test_0016_3) {
    // Sequences with length greater than 50 should be aligned with gaps
    // Sequences with length less or equal than 50 should be aligned without gaps, even the result alignment is worse in this case.
    // This behaviour should be applied, even if input data is alignment

    // Open "_common_data/clustal/COI na.aln".
    GTFileDialog::openFile(os, testDir + "_common_data/clustal/COI na.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Click "align_new_sequences_to_alignment_action" button on the toolbar.
    // Select "_common_data/scenarios/add_and_align/two_seqs.aln" as input data.
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, testDir + "_common_data/scenarios/add_and_align/two_seqs.aln"));
    GTUtilsMsaEditor::activateAlignSequencesToAlignmentMenu(os, "MAFFT");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: sequence 'seq1' doesn't have gaps within the sequence data, sequence 'seq2' has a gap within the sequence data.
    QString expectedSeq1Data = "---------TAATTCGTTCAGAACTAAGACAACCCGGTGTACTTTTATTGGTGATAGTC-----------";
    QString actualSeq1Data = GTUtilsMSAEditorSequenceArea::getSequenceData(os, 18).left(expectedSeq1Data.length());
    CHECK_SET_ERR(expectedSeq1Data == actualSeq1Data, QString("Unexpected 'seq1' data: expected '%1', got '%2'").arg(expectedSeq1Data).arg(actualSeq1Data));

    QString expectedSeq2Data = "---------TAATTCGTTCAGAACTAAGACAACCCGG-TGTACTTTTATTGGTGATAGTCA---------";
    QString actualSeq2Data = GTUtilsMSAEditorSequenceArea::getSequenceData(os, 19).left(expectedSeq2Data.length());
    CHECK_SET_ERR(expectedSeq2Data == actualSeq2Data, QString("Unexpected 'seq2' data: expected '%1', got '%2'").arg(expectedSeq2Data).arg(actualSeq2Data));
}

GUI_TEST_CLASS_DEFINITION(test_0017) {
    // The test checks that ClustalO "Align alignment to alignment" works correctly.

    GTFileDialog::openFile(os, testDir + "_common_data/muscul4/protein.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    QStringList alignmentBefore = GTUtilsMsaEditor::getWholeData(os);

    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, testDir + "_common_data/alignment/align_sequence_to_an_alignment/chicken-part.fa"));
    GTUtilsMsaEditor::activateAlignSequencesToAlignmentMenu(os, "ClustalO");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QStringList alignmentAfter = GTUtilsMsaEditor::getWholeData(os);
    CHECK_SET_ERR(alignmentAfter.size() == alignmentBefore.size() + 1, "Unexpected alignment size: " + QString::number(alignmentAfter.size()));

    alignmentAfter.removeLast();
    CHECK_SET_ERR(alignmentBefore == alignmentAfter, "Original alignment was changed");

    QString alignedSequence = GTUtilsMSAEditorSequenceArea::getSequenceData(os, "Chicken_Part");
    QString expectedSequence = "MANHSQLGFQDASSPIMEELVEFHDHALMVALAICSLVLYLLTLMLMEKLS-SNTVDAQEVELIWTILPAIVLVLLALPSL--------------------------------KDLSFDSYMTPTTDLPLGHFRLLEVDHRIVIPMESPIRVIITADDVLHSWAVPALGVKTDAIPGRLNQTSFITTRPGVFYGQCSEICGANHSYMPIVVESTPLKHFEAWSSLLSS------";
    CHECK_SET_ERR(alignedSequence == expectedSequence, "Wrong aligned sequence: " + alignedSequence);
}

}  // namespace GUITest_common_scenarios_align_sequences_to_msa
}  // namespace U2
