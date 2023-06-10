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

#include <GTGlobals.h>
#include <base_dialogs/GTFileDialog.h>
#include <drivers/GTKeyboardDriver.h>
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

static void checkAlignedRegion(const QRect& selectionRect, const QString& expectedContent) {
    int onScreenSelectionStartX = selectionRect.x() + 1;
    GTUtilsMsaEditor::gotoWithKeyboardShortcut(onScreenSelectionStartX);

    GTUtilsMSAEditorSequenceArea::selectArea(selectionRect.topLeft(), selectionRect.bottomRight());
    GTKeyboardUtils::copy();

    QString clipboardText = GTClipboard::text();
    CHECK_SET_ERR(clipboardText == expectedContent, QString("Incorrect alignment of the region\n Expected: \n%1 \nResult: \n%2").arg(expectedContent).arg(clipboardText));
}

GUI_TEST_CLASS_DEFINITION(test_0001) {
    // Try to delete the MSA object during aligning
    // Expected state: the sequences are locked and can not be deleted
    GTLogTracer lt;
    GTFileDialog::openFile(testDir + "_common_data/clustal/", "3000_sequences.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTFileDialog::openFile(testDir + "_common_data/alignment/align_sequence_to_an_alignment/", "tub1.txt");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsProjectTreeView::click("tub");
    GTUtilsMdi::activateWindow("3000_sequences [3000_sequences.aln]");

    GTUtilsMsaEditor::activateAlignSequencesToAlignmentMenu("MAFFT");

    GTUtilsProjectTreeView::click("tub1.txt");
    GTKeyboardDriver::keyClick(Qt::Key_Delete);

    CHECK_SET_ERR(lt.hasMessage("Cannot remove document tub1.txt"), "The expected message is not found in the log");
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(GTUtilsMsaEditor::getSequencesCount() == 3086, "Incorrect sequences count");
}

GUI_TEST_CLASS_DEFINITION(test_0002) {
    // Try to delete the MSA object during aligning
    // Expected state: the MSA object is locked and can not be deleted
    GTLogTracer lt;
    GTFileDialog::openFile(testDir + "_common_data/clustal/", "3000_sequences.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTFileDialog::openFile(testDir + "_common_data/alignment/align_sequence_to_an_alignment/", "tub1.txt");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsProjectTreeView::click("tub");
    GTUtilsMdi::activateWindow("3000_sequences [3000_sequences.aln]");

    GTUtilsMsaEditor::activateAlignSequencesToAlignmentMenu("MAFFT");

    GTUtilsProjectTreeView::click("3000_sequences.aln");
    GTKeyboardDriver::keyClick(Qt::Key_Delete);

    CHECK_SET_ERR(lt.hasMessage("Cannot remove document 3000_sequences.aln"), "The expected message is not found in the log");
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(GTUtilsMsaEditor::getSequencesCount() == 3086, "Incorrect sequences count");
}

GUI_TEST_CLASS_DEFINITION(test_0003) {
    // Align short sequences with default settings(on platforms with MAFFT)
    // Expected state: MAFFT alignment started and finished successfully with using option --addfragments
    GTLogTracer lt;

    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/cmdline/primers/", "primers.fa"));

    GTUtilsMsaEditor::activateAlignSequencesToAlignmentMenu("MAFFT");
    GTUtilsTaskTreeView::waitTaskFinished();

    CHECK_SET_ERR(GTUtilsMsaEditor::getSequencesCount() == 152, "Incorrect sequences count");

    CHECK_SET_ERR(lt.hasMessage("--addfragments"), "The expected message is not found in the log");

    checkAlignedRegion(QRect(QPoint(86, 17), QPoint(114, 23)), QString("CATGCCTTTGTAATAATCTTCTTTATAGT\n"
                                                                       "-----------------------------\n"
                                                                       "-----------------------------\n"
                                                                       "CTATCCTTCGCAAGACCCTTC--------\n"
                                                                       "-----------------------------\n"
                                                                       "-----------------------------\n"
                                                                       "---------ATAATACCGCGCCACATAGC"));
}

GUI_TEST_CLASS_DEFINITION(test_0004) {
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/cmdline/primers/", "primers.fa"));

    GTUtilsMsaEditor::activateAlignSequencesToAlignmentMenu("UGENE");
    GTUtilsTaskTreeView::waitTaskFinished();

    CHECK_SET_ERR(GTUtilsMsaEditor::getSequencesCount() == 152, "Incorrect sequences count");

    checkAlignedRegion(QRect(QPoint(51, 17), QPoint(71, 19)), QString("GTGATAGTCAAATCTATAATG\n"
                                                                      "---------------------\n"
                                                                      "GACTGGTTCCAATTGACAAGC"));
}

GUI_TEST_CLASS_DEFINITION(test_0005) {
    GTFileDialog::openFile(testDir + "_common_data/alignment/align_sequence_to_an_alignment/", "TUB.msf");
    GTUtilsTaskTreeView::waitTaskFinished();

    QStringList fileList = {"tub1.txt", "tub3.txt"};
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils_list(testDir + "_common_data/alignment/align_sequence_to_an_alignment/", fileList));

    GTUtilsMsaEditor::activateAlignSequencesToAlignmentMenu("MAFFT");
    GTUtilsTaskTreeView::waitTaskFinished();

    CHECK_SET_ERR(GTUtilsMsaEditor::getSequencesCount() == 17, "Incorrect sequences count");

    checkAlignedRegion(QRect(QPoint(970, 7), QPoint(985, 15)), QString("TTCCCAGGTCAGCTCA\n"
                                                                       "----------------\n"
                                                                       "----------------\n"
                                                                       "----------------\n"
                                                                       "----------------\n"
                                                                       "----------------\n"
                                                                       "----------------\n"
                                                                       "----------------\n"
                                                                       "TTCCCAGGTCAGCTCA"));

    checkAlignedRegion(QRect(QPoint(875, 7), QPoint(889, 16)), QString("TCTGCTTCCGTACAC\n"
                                                                       "---------------\n"
                                                                       "---------------\n"
                                                                       "--------CGTACAC\n"
                                                                       "---------------\n"
                                                                       "---------------\n"
                                                                       "---------------\n"
                                                                       "---------------\n"
                                                                       "---------------\n"
                                                                       "TCTGCTTCCGTACAC"));

    checkAlignedRegion(QRect(QPoint(0, 7), QPoint(4, 16)), QString("-----\n"
                                                                   "-----\n"
                                                                   "-----\n"
                                                                   "-----\n"
                                                                   "-----\n"
                                                                   "-----\n"
                                                                   "-----\n"
                                                                   "-----\n"
                                                                   "-----\n"
                                                                   "-----"));
}

GUI_TEST_CLASS_DEFINITION(test_0006) {
    GTFileDialog::openFile(testDir + "_common_data/alignment/align_sequence_to_an_alignment/", "TUB.msf");
    GTUtilsTaskTreeView::waitTaskFinished();

    QStringList fileList = {"tub1.txt", "tub3.txt"};
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils_list(testDir + "_common_data/alignment/align_sequence_to_an_alignment/", fileList));

    GTUtilsMsaEditor::activateAlignSequencesToAlignmentMenu("UGENE");
    GTUtilsTaskTreeView::waitTaskFinished();

    CHECK_SET_ERR(GTUtilsMsaEditor::getSequencesCount() == 17, "Incorrect sequences count");

    checkAlignedRegion(QRect(QPoint(970, 7), QPoint(985, 15)), QString("TTCCCAGGTCAGCTCA\n"
                                                                       "----------------\n"
                                                                       "----------------\n"
                                                                       "----------------\n"
                                                                       "----------------\n"
                                                                       "----------------\n"
                                                                       "----------------\n"
                                                                       "----------------\n"
                                                                       "TTCCCAGGTCAGCTCA"));

    checkAlignedRegion(QRect(QPoint(875, 7), QPoint(889, 16)), QString("TCTGCTTCCGTACAC\n"
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
    GTLogTracer lt;
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(dataDir + "samples/Genbank/", "CVU55762.gb"));

    GTUtilsMsaEditor::activateAlignSequencesToAlignmentMenu("MAFFT");
    GTUtilsTaskTreeView::waitTaskFinished();

    CHECK_SET_ERR(GTUtilsMsaEditor::getSequencesCount() == 19, "Incorrect sequences count");
}

GUI_TEST_CLASS_DEFINITION(test_0008) {
    // Do not select anything in the project. Click the button. Add several ABI files.
    // Expected state: The sequences were added to the alignment and aligned
    GTLogTracer lt;
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    QStringList fileList = {"39_034.ab1", "19_022.ab1", "25_032.ab1"};
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils_list(testDir + "_common_data/abif/", fileList));

    GTUtilsMsaEditor::activateAlignSequencesToAlignmentMenu("MAFFT");
    GTUtilsTaskTreeView::waitTaskFinished();

    CHECK_SET_ERR(GTUtilsMsaEditor::getSequencesCount() == 21, "Incorrect sequences count");
}

GUI_TEST_CLASS_DEFINITION(test_0009) {
    // Do not select anything in the project. Click the button. Add sequences in ClustalW format. Uncheck several sequences in the appeared dialog.
    // Expected state: Only checked sequences were added to the alignment.
    GTLogTracer lt;
    GTFileDialog::openFile(testDir + "_common_data/alignment/align_sequence_to_an_alignment/", "TUB.msf");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/clustal/", "COI na.aln"));

    GTUtilsMsaEditor::activateAlignSequencesToAlignmentMenu("MAFFT");
    GTUtilsTaskTreeView::waitTaskFinished();

    CHECK_SET_ERR(GTUtilsMsaEditor::getSequencesCount() == 33, "Incorrect sequences count");
}

GUI_TEST_CLASS_DEFINITION(test_0010) {
    // 1. Open "_common_data/fasta/empty.fa" as msa.
    // 2. Ensure that MAFFT tool is set.
    GTFileDialog::openFile(testDir + "_common_data/fasta/empty.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    int sequenceCount = GTUtilsMsaEditor::getSequencesCount();
    CHECK_SET_ERR(sequenceCount == 2, "1. Incorrect sequences count: " + QString::number(sequenceCount));

    // 3. Click "align_new_sequences_to_alignment_action" button on the toolbar.
    // 4. Select "data/samples/FASTQ/eas.fastq".
    GTUtilsMsaEditor::checkAlignSequencesToAlignmentMenu("MAFFT", PopupChecker::IsDisabled);
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(dataDir + "samples/FASTQ/eas.fastq"));
    GTUtilsMsaEditor::activateAlignSequencesToAlignmentMenu("UGENE");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state: three sequences are added to the msa.
    sequenceCount = GTUtilsMsaEditor::getSequencesCount();
    CHECK_SET_ERR(sequenceCount == 5, "2. Incorrect sequences count: " + QString::number(sequenceCount));
}

GUI_TEST_CLASS_DEFINITION(test_0011) {
    // Adding and aligning with MAFFT a sequence, which is longer than an alignment.

    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/ma.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    //  Click "align_new_sequences_to_alignment_action" button on the toolbar.
    //  Select "_common_data/scenarios/add_and_align/add_and_align_1.fa" in the dialog.
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/scenarios/add_and_align/add_and_align_1.fa"));
    GTUtilsMsaEditor::activateAlignSequencesToAlignmentMenu("MAFFT");

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
    GTUtilsTaskTreeView::waitTaskFinished();
    QStringList msaData = GTUtilsMsaEditor::getWholeData();
    CHECK_SET_ERR(expectedMsaData == msaData, "Expected:\n" + expectedMsaData.join("\n") + "\nFound:\n" + msaData.join("\n"));
}

GUI_TEST_CLASS_DEFINITION(test_0012) {
    // Adding and aligning with MAFFT a sequence, which can be aligned with an alignment shifting

    // Open "_common_data/scenarios/msa/ma.aln".
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/ma.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Click "align_new_sequences_to_alignment_action" button on the toolbar.
    // Select "_common_data/scenarios/add_and_align/add_and_align_2.fa" in the dialog.
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/scenarios/add_and_align/add_and_align_2.fa"));
    GTUtilsMsaEditor::activateAlignSequencesToAlignmentMenu("MAFFT");

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
    GTUtilsTaskTreeView::waitTaskFinished();
    QStringList msaData = GTUtilsMsaEditor::getWholeData();
    CHECK_SET_ERR(expectedMsaData == msaData, "Unexpected MSA data");
}

GUI_TEST_CLASS_DEFINITION(test_0013) {
    // Adding and aligning with MAFFT a sequence to an alignment with columns of gaps

    // Open "_common_data/scenarios/msa/ma2_gap_8_col.aln".
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/ma2_gap_8_col.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Click "align_new_sequences_to_alignment_action" button on the toolbar.
    // Select "_common_data/scenarios/add_and_align/add_and_align_1.fa" in the dialog.
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/scenarios/add_and_align/add_and_align_1.fa"));
    GTUtilsMsaEditor::activateAlignSequencesToAlignmentMenu("MAFFT");

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
    GTUtilsTaskTreeView::waitTaskFinished();
    QStringList msaData = GTUtilsMsaEditor::getWholeData();
    CHECK_SET_ERR(expectedMsaData == msaData, "Unexpected MSA data");
}

GUI_TEST_CLASS_DEFINITION(test_0014) {
    // Adding and aligning with MAFFT should remove all columns of gaps from the source msa before the aligning, also it should be trimmed after the aligning.

    // Open "_common_data/scenarios/msa/ma2_gap_8_col.aln".
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/ma2_gap_8_col.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Click "align_new_sequences_to_alignment_action" button on the toolbar.
    // Select "_common_data/scenarios/add_and_align/add_and_align_3.fa" in the dialog.
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/scenarios/add_and_align/add_and_align_3.fa"));
    GTUtilsMsaEditor::activateAlignSequencesToAlignmentMenu("MAFFT");

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
    GTUtilsTaskTreeView::waitTaskFinished();
    QStringList msaData = GTUtilsMsaEditor::getWholeData();
    CHECK_SET_ERR(expectedMsaData == msaData, "Unexpected MSA data");
}

GUI_TEST_CLASS_DEFINITION(test_0015) {
    //    Adding and aligning without MAFFT should remove all columns of gaps from the source msa before the aligning, also it should be trimmed after the aligning.

    //    2. Ensure that MAFFT tool is not set. Remove it, if it is set.
    GTUtilsExternalTools::removeTool("MAFFT");

    //    1. Open "_common_data/scenarios/msa/ma2_gap_8_col.aln".
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/ma2_gap_8_col.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    3. Click "align_new_sequences_to_alignment_action" button on the toolbar.
    GTUtilsMsaEditor::checkAlignSequencesToAlignmentMenu("MAFFT", PopupChecker::NotExists);
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/scenarios/add_and_align/add_and_align_3.fa"));
    GTUtilsMsaEditor::activateAlignSequencesToAlignmentMenu("UGENE");

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
    GTUtilsTaskTreeView::waitTaskFinished();
    QStringList msaData = GTUtilsMsaEditor::getWholeData();
    CHECK_SET_ERR(expectedMsaData == msaData, "Unexpected MSA data");
}

GUI_TEST_CLASS_DEFINITION(test_0016_1) {
    // Sequences with length less or equal than 50 should be aligned without gaps, even the result alignment is worse in this case.

    //    1. Open "_common_data/clustal/COI na.aln".
    GTFileDialog::openFile(testDir + "_common_data/clustal/COI na.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Click "align_new_sequences_to_alignment_action" button on the toolbar.
    //    3. Select "_common_data/scenarios/add_and_align/seq1.fa" as sequence to align.
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/scenarios/add_and_align/seq1.fa"));
    GTUtilsMsaEditor::activateAlignSequencesToAlignmentMenu("MAFFT");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    Expected state: the new sequence doesn't have gaps within the sequence data.
    QString expectedRowData = "---------TAATTCGTTCAGAACTAAGACAACCCGGTGTACTTTTATTGGTGATAGTC-----------";
    QString actualRowData = GTUtilsMSAEditorSequenceArea::getSequenceData(18).left(expectedRowData.length());
    CHECK_SET_ERR(expectedRowData == actualRowData, QString("Unexpected row data: expected '%1', got '%2'").arg(expectedRowData).arg(actualRowData));
}

GUI_TEST_CLASS_DEFINITION(test_0016_2) {
    // Sequences with length greater than 50 should be aligned with gaps

    // Open "_common_data/clustal/COI na.aln".
    GTFileDialog::openFile(testDir + "_common_data/clustal/COI na.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Click "align_new_sequences_to_alignment_action" button on the toolbar.
    // Select "_common_data/scenarios/add_and_align/seq2.fa" as sequence to align.
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/scenarios/add_and_align/seq2.fa"));
    GTUtilsMsaEditor::activateAlignSequencesToAlignmentMenu("MAFFT");

    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state: the new sequence has a gap within the sequence data.
    QString expectedRowData = "---------TAATTCGTTCAGAACTAAGACAACCCGG-TGTACTTTTATTGGTGATAGTCA---------";
    QString actualRowData = GTUtilsMSAEditorSequenceArea::getSequenceData(18).left(expectedRowData.length());
    CHECK_SET_ERR(expectedRowData == actualRowData, QString("Unexpected row data: expected '%1', got '%2'").arg(expectedRowData).arg(actualRowData));
}

GUI_TEST_CLASS_DEFINITION(test_0016_3) {
    // Sequences with length greater than 50 should be aligned with gaps
    // Sequences with length less or equal than 50 should be aligned without gaps, even the result alignment is worse in this case.
    // This behaviour should be applied, even if input data is alignment

    // Open "_common_data/clustal/COI na.aln".
    GTFileDialog::openFile(testDir + "_common_data/clustal/COI na.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Click "align_new_sequences_to_alignment_action" button on the toolbar.
    // Select "_common_data/scenarios/add_and_align/two_seqs.aln" as input data.
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/scenarios/add_and_align/two_seqs.aln"));
    GTUtilsMsaEditor::activateAlignSequencesToAlignmentMenu("MAFFT");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    Expected state: sequence 'seq1' doesn't have gaps within the sequence data, sequence 'seq2' has a gap within the sequence data.
    QString expectedSeq1Data = "---------TAATTCGTTCAGAACTAAGACAACCCGGTGTACTTTTATTGGTGATAGTC-----------";
    QString actualSeq1Data = GTUtilsMSAEditorSequenceArea::getSequenceData(18).left(expectedSeq1Data.length());
    CHECK_SET_ERR(expectedSeq1Data == actualSeq1Data, QString("Unexpected 'seq1' data: expected '%1', got '%2'").arg(expectedSeq1Data).arg(actualSeq1Data));

    QString expectedSeq2Data = "---------TAATTCGTTCAGAACTAAGACAACCCGG-TGTACTTTTATTGGTGATAGTCA---------";
    QString actualSeq2Data = GTUtilsMSAEditorSequenceArea::getSequenceData(19).left(expectedSeq2Data.length());
    CHECK_SET_ERR(expectedSeq2Data == actualSeq2Data, QString("Unexpected 'seq2' data: expected '%1', got '%2'").arg(expectedSeq2Data).arg(actualSeq2Data));
}

GUI_TEST_CLASS_DEFINITION(test_0017) {
    // The test checks that ClustalO "Align alignment to alignment" works correctly.

    GTFileDialog::openFile(testDir + "_common_data/muscul4/protein.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    QStringList alignmentBefore = GTUtilsMsaEditor::getWholeData();

    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/alignment/align_sequence_to_an_alignment/chicken-part.fa"));
    GTUtilsMsaEditor::activateAlignSequencesToAlignmentMenu("ClustalO");
    GTUtilsTaskTreeView::waitTaskFinished();

    QStringList alignmentAfter = GTUtilsMsaEditor::getWholeData();
    CHECK_SET_ERR(alignmentAfter.size() == alignmentBefore.size() + 1, "Unexpected alignment size: " + QString::number(alignmentAfter.size()));

    alignmentAfter.removeLast();
    CHECK_SET_ERR(alignmentBefore == alignmentAfter, "Original alignment was changed");

    QString alignedSequence = GTUtilsMSAEditorSequenceArea::getSequenceData("Chicken_Part");
    QString expectedSequence = "MANHSQLGFQDASSPIMEELVEFHDHALMVALAICSLVLYLLTLMLMEKLS-SNTVDAQEVELIWTILPAIVLVLLALPSL--------------------------------KDLSFDSYMTPTTDLPLGHFRLLEVDHRIVIPMESPIRVIITADDVLHSWAVPALGVKTDAIPGRLNQTSFITTRPGVFYGQCSEICGANHSYMPIVVESTPLKHFEAWSSLLSS------";
    CHECK_SET_ERR(alignedSequence == expectedSequence, "Wrong aligned sequence: " + alignedSequence);
}

}  // namespace GUITest_common_scenarios_align_sequences_to_msa
}  // namespace U2
