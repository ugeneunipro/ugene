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

#include <GTGlobals.h>
#include <base_dialogs/GTFileDialog.h>
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTAction.h>
#include <primitives/GTCheckBox.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTMenu.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTToolbar.h>
#include <primitives/GTTreeWidget.h>
#include <primitives/GTWidget.h>
#include <primitives/PopupChooser.h>
#include <system/GTClipboard.h>
#include <utils/GTKeyboardUtils.h>
#include <utils/GTUtilsDialog.h>

#include <QApplication>

#include <U2Core/AppContext.h>
#include <U2Core/ExternalToolRegistry.h>

#include <U2View/MSAEditor.h>
#include <U2View/MSAEditorSequenceArea.h>

#include "GTTestsAlignSequenceToMsa.h"
#include "GTUtilsExternalTools.h"
#include "GTUtilsLog.h"
#include "GTUtilsMdi.h"
#include "GTUtilsMsaEditor.h"
#include "GTUtilsMsaEditorSequenceArea.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsTaskTreeView.h"
#include "runnables/ugene/corelibs/U2Gui/PositionSelectorFiller.h"
#include "runnables/ugene/corelibs/U2Gui/util/RenameSequenceFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/DeleteGapsDialogFiller.h"

namespace U2 {
namespace GUITest_common_scenarios_align_sequences_to_msa{
using namespace HI;

void checkAlignedRegion(HI::GUITestOpStatus& os, const QRect &selectionRect, const QString& expectedContent) {
    GTUtilsDialog::waitForDialog(os, new GoToDialogFiller(os, selectionRect.center().x()));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << MSAE_MENU_NAVIGATION <<
                                                      "action_go_to_position"));
    GTMenu::showContextMenu(os, GTUtilsMdi::activeWindow(os));

    GTUtilsMSAEditorSequenceArea::selectArea(os, selectionRect.topLeft(), selectionRect.bottomRight());
    GTKeyboardUtils::copy(os);
    GTGlobals::sleep(500);

    const QString clipboardText = GTClipboard::text(os);
    CHECK_SET_ERR(clipboardText == expectedContent, QString("Incorrect alignment of the region\n Expected: \n%1 \nResult: \n%2").arg(expectedContent).arg(clipboardText));
}

GUI_TEST_CLASS_DEFINITION(test_0001){
    //Try to delete the MSA object during aligning
    //Expected state: the sequences are locked and and can not be deleted
    GTLogTracer logTracer;
    GTFileDialog::openFile(os, testDir + "_common_data/clustal/", "3000_sequences.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTFileDialog::openFile(os, testDir + "_common_data/alignment/align_sequence_to_an_alignment/", "tub1.txt");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsProjectTreeView::click(os, "tub");
    GTUtilsMdi::activateWindow(os, "3000_sequences [m] 3000_sequences");

    QAbstractButton *align = GTAction::button( os, "Align sequence(s) to this alignment" );
    CHECK_SET_ERR(align != NULL, "MSA \"Align sequence(s) to this alignment\" action not found");
    GTWidget::click( os, align);

    GTUtilsProjectTreeView::click(os, "tub1.txt");
    GTKeyboardDriver::keyClick( Qt::Key_Delete);
    GTGlobals::sleep();

    const bool hasMessage = logTracer.checkMessage("Cannot remove document tub1.txt");
    CHECK_SET_ERR(hasMessage, "The expected message is not found in the log");

    GTUtilsTaskTreeView::waitTaskFinished(os);

    CHECK_SET_ERR(GTUtilsMsaEditor::getSequencesCount(os) == 3086, "Incorrect sequences count");
}

GUI_TEST_CLASS_DEFINITION(test_0002){
    //Try to delete the MSA object during aligning
    //Expected state: the MSA object is locked and and can not be deleted
    GTLogTracer logTracer;
    GTFileDialog::openFile(os, testDir + "_common_data/clustal/", "3000_sequences.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTFileDialog::openFile(os, testDir + "_common_data/alignment/align_sequence_to_an_alignment/", "tub1.txt");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsProjectTreeView::click(os, "tub");
    GTUtilsMdi::activateWindow(os, "3000_sequences [m] 3000_sequences");

    QAbstractButton *align = GTAction::button( os, "Align sequence(s) to this alignment" );
    CHECK_SET_ERR(align != NULL, "MSA \"Align sequence(s) to this alignment\" action not found");
    GTWidget::click( os, align);

    GTUtilsProjectTreeView::click(os, "3000_sequences.aln");
    GTKeyboardDriver::keyClick( Qt::Key_Delete);
    GTGlobals::sleep();

    const bool hasMessage = logTracer.checkMessage("Cannot remove document 3000_sequences.aln");
    CHECK_SET_ERR(hasMessage, "The expected message is not found in the log");

    GTUtilsTaskTreeView::waitTaskFinished(os);

    CHECK_SET_ERR(GTUtilsMsaEditor::getSequencesCount(os) == 3086, "Incorrect sequences count");
}

GUI_TEST_CLASS_DEFINITION(test_0003) {
    //Align short sequences with default settings(on platforms with MAFFT)
    //Expected state: MAFFT alignment started and finished successfully with using option --addfragments
    ExternalTool *mafftTool = AppContext::getExternalToolRegistry()->getById("USUPP_MAFFT");
    CHECK_SET_ERR(NULL != mafftTool, "Can't find MAFFT tool in the registry");
    CHECK_SET_ERR(mafftTool->isValid(), "MAFFT tool is not valid");

    GTLogTracer logTracer;

    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTFileDialogUtils *ob = new GTFileDialogUtils(os, testDir + "_common_data/cmdline/primers/", "primers.fa");
    GTUtilsDialog::waitForDialog(os, ob);

    QAbstractButton *align = GTAction::button( os, "Align sequence(s) to this alignment" );
    CHECK_SET_ERR(align != NULL, "MSA \"Align sequence(s) to this alignment\" action not found");
    GTWidget::click( os, align);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    CHECK_SET_ERR(GTUtilsMsaEditor::getSequencesCount(os) == 152, "Incorrect sequences count");

    const bool hasMessage = logTracer.checkMessage("--addfragments");
    CHECK_SET_ERR(hasMessage, "The expected message is not found in the log");

    checkAlignedRegion(os, QRect(QPoint(86, 17), QPoint(114, 23)),
        QString("CATGCCTTTGTAATAATCTTCTTTATAGT\n"
                "-----------------------------\n"
                "-----------------------------\n"
                "CTATCCTTCGCAAGACCCTTC--------\n"
                "-----------------------------\n"
                "-----------------------------\n"
                "---------ATAATACCGCGCCACATAGC"));
}


GUI_TEST_CLASS_DEFINITION(test_0004){
    //Remove MAFFT from external tools, then align short sequences
    //Expected state: UGENE alignment started and finished successfully
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsExternalTools::removeTool(os, "MAFFT");

    GTFileDialogUtils *ob = new GTFileDialogUtils(os, testDir + "_common_data/cmdline/primers/", "primers.fa");
    GTUtilsDialog::waitForDialog(os, ob);

    QAbstractButton *align = GTAction::button( os, "Align sequence(s) to this alignment" );
    CHECK_SET_ERR(align != NULL, "MSA \"Align sequence(s) to this alignment\" action not found");
    GTWidget::click( os, align);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    CHECK_SET_ERR(GTUtilsMsaEditor::getSequencesCount(os) == 152, "Incorrect sequences count");

    checkAlignedRegion(os, QRect(QPoint(51, 17), QPoint(71, 19)),
        QString("GTGATAGTCAAATCTATAATG\n"
                "---------------------\n"
                "GACTGGTTCCAATTGACAAGC"));
}

GUI_TEST_CLASS_DEFINITION(test_0005){
    ExternalTool *mafftTool = AppContext::getExternalToolRegistry()->getById("USUPP_MAFFT");
    CHECK_SET_ERR(NULL != mafftTool, "Can't find MAFFT tool in the registry");
    CHECK_SET_ERR(mafftTool->isValid(), "MAFFT tool is not valid");

    GTFileDialog::openFile(os, testDir + "_common_data/alignment/align_sequence_to_an_alignment/", "TUB.msf");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QStringList fileList;
    fileList << "tub1.txt" << "tub3.txt";
    GTFileDialogUtils_list *ob = new GTFileDialogUtils_list(os, testDir + "_common_data/alignment/align_sequence_to_an_alignment/", fileList);
    GTUtilsDialog::waitForDialog(os, ob);

    QAbstractButton *align = GTAction::button( os, "Align sequence(s) to this alignment" );
    CHECK_SET_ERR(align != NULL, "MSA \"Align sequence(s) to this alignment\" action not found");
    GTWidget::click( os, align);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    CHECK_SET_ERR(GTUtilsMsaEditor::getSequencesCount(os) == 17, "Incorrect sequences count");

    checkAlignedRegion(os, QRect(QPoint(970, 7), QPoint(985, 15)),
        QString("TTCCCAGGTCAGCTCA\n"
                "----------------\n"
                "----------------\n"
                "----------------\n"
                "----------------\n"
                "----------------\n"
                "----------------\n"
                "----------------\n"
                "TTCCCAGGTCAGCTCA"));

    checkAlignedRegion(os, QRect(QPoint(875, 7), QPoint(889, 16)),
        QString("TCTGCTTCCGTACAC\n"
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

GUI_TEST_CLASS_DEFINITION(test_0006){
    GTFileDialog::openFile(os, testDir + "_common_data/alignment/align_sequence_to_an_alignment/", "TUB.msf");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsExternalTools::removeTool(os, "MAFFT");

    QStringList fileList;
    fileList << "tub1.txt" << "tub3.txt";
    GTFileDialogUtils_list *ob = new GTFileDialogUtils_list(os, testDir + "_common_data/alignment/align_sequence_to_an_alignment/", fileList);
    GTUtilsDialog::waitForDialog(os, ob);

    QAbstractButton *align = GTAction::button( os, "Align sequence(s) to this alignment" );
    CHECK_SET_ERR(align != NULL, "MSA \"Align sequence(s) to this alignment\" action not found");
    GTWidget::click( os, align);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    CHECK_SET_ERR(GTUtilsMsaEditor::getSequencesCount(os) == 17, "Incorrect sequences count");

    checkAlignedRegion(os, QRect(QPoint(970, 7), QPoint(985, 15)),
        QString("TTCCCAGGTCAGCTCA\n"
        "----------------\n"
        "----------------\n"
        "----------------\n"
        "----------------\n"
        "----------------\n"
        "----------------\n"
        "----------------\n"
        "TTCCCAGGTCAGCTCA"));

    checkAlignedRegion(os, QRect(QPoint(875, 7), QPoint(889, 16)),
        QString("TCTGCTTCCGTACAC\n"
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

GUI_TEST_CLASS_DEFINITION(test_0007){
    //Do not select anything in the project. Click the button. Add a sequence in GenBank format.
    //Expected state: The sequence was added to the alignment and aligned.
    GTLogTracer logTracer;
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTFileDialogUtils *ob = new GTFileDialogUtils(os, dataDir + "samples/Genbank/", "CVU55762.gb");
    GTUtilsDialog::waitForDialog(os, ob);

    QAbstractButton *align = GTAction::button( os, "Align sequence(s) to this alignment" );
    CHECK_SET_ERR(align != NULL, "MSA \"Align sequence(s) to this alignment\" action not found");
    GTWidget::click( os, align);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    CHECK_SET_ERR(GTUtilsMsaEditor::getSequencesCount(os) == 19, "Incorrect sequences count");
}

GUI_TEST_CLASS_DEFINITION(test_0008){
    //Do not select anything in the project. Click the button. Add several ABI files.
    //Expected state: The sequences were added to the alignment and aligned
    GTLogTracer logTracer;
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QStringList fileList;
    fileList << "39_034.ab1" << "19_022.ab1" << "25_032.ab1";
    GTFileDialogUtils_list *ob = new GTFileDialogUtils_list(os, testDir + "_common_data/abif/", fileList);
    GTUtilsDialog::waitForDialog(os, ob);

    QAbstractButton *align = GTAction::button( os, "Align sequence(s) to this alignment" );
    CHECK_SET_ERR(align != NULL, "MSA \"Align sequence(s) to this alignment\" action not found");
    GTWidget::click( os, align);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    CHECK_SET_ERR(GTUtilsMsaEditor::getSequencesCount(os) == 21, "Incorrect sequences count");
}

GUI_TEST_CLASS_DEFINITION(test_0009){
    //Do not select anything in the project. Click the button. Add sequences in ClustalW format. Uncheck several sequences in the appeared dialog.
    //Expected state: Only checked sequences were added to the alignment.
    GTLogTracer logTracer;
    GTFileDialog::openFile(os, testDir + "_common_data/alignment/align_sequence_to_an_alignment/", "TUB.msf");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTFileDialogUtils *ob = new GTFileDialogUtils(os, testDir + "_common_data/clustal/", "COI na.aln");
    GTUtilsDialog::waitForDialog(os, ob);

    QAbstractButton *align = GTAction::button( os, "Align sequence(s) to this alignment" );
    CHECK_SET_ERR(align != NULL, "MSA \"Align sequence(s) to this alignment\" action not found");
    GTWidget::click( os, align);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    CHECK_SET_ERR(GTUtilsMsaEditor::getSequencesCount(os) == 33, "Incorrect sequences count");
}

GUI_TEST_CLASS_DEFINITION(test_0010) {
    //1. Open "_common_data/fasta/empty.fa" as msa.
    //2. Ensure that MAFFT tool is set.
    GTFileDialog::openFile(os, testDir + "_common_data/fasta/empty.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //3. Click "Align sequence(s) to this alignment" button on the toolbar.
    //4. Select "data/samples/FASTQ/eas.fastq".
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, dataDir + "samples/FASTQ/eas.fastq"));
    GTWidget::click(os, GTAction::button(os, "Align sequence(s) to this alignment"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //Expected state: three sequences are added to the msa.
    CHECK_SET_ERR(GTUtilsMsaEditor::getSequencesCount(os) == 3, "Incorrect sequences count");
}

GUI_TEST_CLASS_DEFINITION(test_0011) {
//    Adding and aligning with MAFFT a sequence, which is longer than an alignment.

//    1. Open "_common_data/scenarios/msa/ma.aln".
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/ma.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    2. Ensure that MAFFT tool is set.
    ExternalTool *mafftTool = AppContext::getExternalToolRegistry()->getById("USUPP_MAFFT");
    CHECK_SET_ERR(NULL != mafftTool, "Can't find MAFFT tool in the registry");
    CHECK_SET_ERR(mafftTool->isValid(), "MAFFT tool is not valid");

//    3. Click "Align sequence(s) to this alignment" button on the toolbar.
//    4. Select "_common_data/scenarios/add_and_align/add_and_align_1.fa" in the dialog.
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, testDir + "_common_data/scenarios/add_and_align/add_and_align_1.fa"));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Align sequence(s) to this alignment");

//    Expected state: an additional row appeared in the alignment, all old rows were shifted to be aligned with the new row.
    const QStringList expectedMsaData = QStringList() << "----TAAGACTTCTAA------------"
                                                      << "----TAAGCTTACTAA------------"
                                                      << "----TTAGTTTATTAA------------"
                                                      << "----TCAGTCTATTAA------------"
                                                      << "----TCAGTTTATTAA------------"
                                                      << "----TTAGTCTACTAA------------"
                                                      << "----TCAGATTATTAA------------"
                                                      << "----TTAGATTGCTAA------------"
                                                      << "----TTAGATTATTAA------------"
                                                      << "----TAAGTCTATTAA------------"
                                                      << "----TTAGCTTATTAA------------"
                                                      << "----TTAGCTTATTAA------------"
                                                      << "----TTAGCTTATTAA------------"
                                                      << "----TAAGTCTTTTAA------------"
                                                      << "----TAAGTCTTTTAA------------"
                                                      << "----TAAGTCTTTTAA------------"
                                                      << "----TAAGAATAATTA------------"
                                                      << "----TAAGCCTTTTAA------------"
                                                      << "GCGCTAAGCCTTTTAAGCGCGCGCGCGC";
    GTUtilsTaskTreeView::waitTaskFinished(os);
    const QStringList msaData = GTUtilsMsaEditor::getWholeData(os);
    CHECK_SET_ERR(expectedMsaData == msaData, "Unexpected MSA data");
}

GUI_TEST_CLASS_DEFINITION(test_0012) {
//    Adding and aligning with MAFFT a sequence, which can be aligned with an alignment shifting

//    1. Open "_common_data/scenarios/msa/ma.aln".
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/ma.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    2. Ensure that MAFFT tool is set.
    ExternalTool *mafftTool = AppContext::getExternalToolRegistry()->getById("USUPP_MAFFT");
    CHECK_SET_ERR(NULL != mafftTool, "Can't find MAFFT tool in the registry");
    CHECK_SET_ERR(mafftTool->isValid(), "MAFFT tool is not valid");

//    3. Click "Align sequence(s) to this alignment" button on the toolbar.
//    4. Select "_common_data/scenarios/add_and_align/add_and_align_2.fa" in the dialog.
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, testDir + "_common_data/scenarios/add_and_align/add_and_align_2.fa"));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Align sequence(s) to this alignment");

//    Expected state: an additional row appeared in the alignment, all old rows were shifted to be aligned with the new row.
    const QStringList expectedMsaData = QStringList() << "------TAAGACTTCTAA"
                                                      << "------TAAGCTTACTAA"
                                                      << "------TTAGTTTATTAA"
                                                      << "------TCAGTCTATTAA"
                                                      << "------TCAGTTTATTAA"
                                                      << "------TTAGTCTACTAA"
                                                      << "------TCAGATTATTAA"
                                                      << "------TTAGATTGCTAA"
                                                      << "------TTAGATTATTAA"
                                                      << "------TAAGTCTATTAA"
                                                      << "------TTAGCTTATTAA"
                                                      << "------TTAGCTTATTAA"
                                                      << "------TTAGCTTATTAA"
                                                      << "------TAAGTCTTTTAA"
                                                      << "------TAAGTCTTTTAA"
                                                      << "------TAAGTCTTTTAA"
                                                      << "------TAAGAATAATTA"
                                                      << "------TAAGCCTTTTAA"
                                                      << "GCGCGCTAAGCC------";
    GTUtilsTaskTreeView::waitTaskFinished(os);
    const QStringList msaData = GTUtilsMsaEditor::getWholeData(os);
    CHECK_SET_ERR(expectedMsaData == msaData, "Unexpected MSA data");
}

GUI_TEST_CLASS_DEFINITION(test_0013) {
//    Adding and aligning with MAFFT a sequence to an alignment with columns of gaps

//    1. Open "_common_data/scenarios/msa/ma2_gap_8_col.aln".
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/ma2_gap_8_col.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    2. Ensure that MAFFT tool is set.
    ExternalTool *mafftTool = AppContext::getExternalToolRegistry()->getById("USUPP_MAFFT");
    CHECK_SET_ERR(NULL != mafftTool, "Can't find MAFFT tool in the registry");
    CHECK_SET_ERR(mafftTool->isValid(), "MAFFT tool is not valid");

//    3. Click "Align sequence(s) to this alignment" button on the toolbar.
//    4. Select "_common_data/scenarios/add_and_align/add_and_align_1.fa" in the dialog.
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, testDir + "_common_data/scenarios/add_and_align/add_and_align_1.fa"));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Align sequence(s) to this alignment");

//    Expected state: an additional row appeared in the alignment, all old rows were shifted to be aligned with the new row, columns with gaps were removed
    const QStringList expectedMsaData = QStringList() << "-----AAGCTTCTTTTAA----------"
                                                      << "-----AAGTTACTAA-------------"
                                                      << "-----TAG---TTATTAA----------"
                                                      << "-----AAGC---TATTAA----------"
                                                      << "-----TAGTTATTAA-------------"
                                                      << "-----TAGTTATTAA-------------"
                                                      << "-----TAGTTATTAA-------------"
                                                      << "-----AAGCTTT---TAA----------"
                                                      << "-----A--AGAATAATTA----------"
                                                      << "-----AAGCTTTTAA-------------"
                                                      << "GCGCTAAGCCTTTTAAGCGCGCGCGCGC";
    GTUtilsTaskTreeView::waitTaskFinished(os);
    const QStringList msaData = GTUtilsMsaEditor::getWholeData(os);
    CHECK_SET_ERR(expectedMsaData == msaData, "Unexpected MSA data");
}

GUI_TEST_CLASS_DEFINITION(test_0014) {
//    Adding and aligning with MAFFT should remove all columns of gaps from the source msa before the aligning, also it should be trimmed after the aligning.

//    1. Open "_common_data/scenarios/msa/ma2_gap_8_col.aln".
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/ma2_gap_8_col.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    2. Ensure that MAFFT tool is set.
    ExternalTool *mafftTool = AppContext::getExternalToolRegistry()->getById("USUPP_MAFFT");
    CHECK_SET_ERR(NULL != mafftTool, "Can't find MAFFT tool in the registry");
    CHECK_SET_ERR(mafftTool->isValid(), "MAFFT tool is not valid");

//    3. Click "Align sequence(s) to this alignment" button on the toolbar.
//    4. Select "_common_data/scenarios/add_and_align/add_and_align_3.fa" in the dialog.
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, testDir + "_common_data/scenarios/add_and_align/add_and_align_3.fa"));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Align sequence(s) to this alignment");

//    Expected state: an additional row appeared in the alignment, the forth column doesn't consist only of gaps, there are no columns of gaps even in the end of the alignment.
    const QStringList expectedMsaData = QStringList() << "AAGCTTCTTTTAA"
                                                      << "AAGTTACTAA---"
                                                      << "TAG---TTATTAA"
                                                      << "AAGC---TATTAA"
                                                      << "TAGTTATTAA---"
                                                      << "TAGTTATTAA---"
                                                      << "TAGTTATTAA---"
                                                      << "AAGCTTT---TAA"
                                                      << "A--AGAATAATTA"
                                                      << "AAGCTTTTAA---"
                                                      << "AAGAATA------";
    GTUtilsTaskTreeView::waitTaskFinished(os);
    const QStringList msaData = GTUtilsMsaEditor::getWholeData(os);
    CHECK_SET_ERR(expectedMsaData == msaData, "Unexpected MSA data");
}

GUI_TEST_CLASS_DEFINITION(test_0015) {
//    Adding and aligning without MAFFT should remove all columns of gaps from the source msa before the aligning, also it should be trimmed after the aligning.

//    2. Ensure that MAFFT tool is not set. Remove it, if it is set.
    GTUtilsExternalTools::removeTool(os, "MAFFT");

//    1. Open "_common_data/scenarios/msa/ma2_gap_8_col.aln".
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/ma2_gap_8_col.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    3. Click "Align sequence(s) to this alignment" button on the toolbar.
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, testDir + "_common_data/scenarios/add_and_align/add_and_align_3.fa"));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Align sequence(s) to this alignment");

//    4. Select "_common_data/scenarios/add_and_align/add_and_align_3.fa" in the dialog.
//    Expected state: an additional row appeared in the alignment, the forth column doesn't consist only of gaps, there are no columns of gaps even in the end of the alignment.
    const QStringList expectedMsaData = QStringList() << "AAGCTTCTTTTAA"
                                                      << "AAGTTACTAA---"
                                                      << "TAG---TTATTAA"
                                                      << "AAGC---TATTAA"
                                                      << "TAGTTATTAA---"
                                                      << "TAGTTATTAA---"
                                                      << "TAGTTATTAA---"
                                                      << "AAGCTTT---TAA"
                                                      << "A--AGAATAATTA"
                                                      << "AAGCTTTTAA---"
                                                      << "AAGAATA------";
    GTUtilsTaskTreeView::waitTaskFinished(os);
    const QStringList msaData = GTUtilsMsaEditor::getWholeData(os);
    CHECK_SET_ERR(expectedMsaData == msaData, "Unexpected MSA data");
}

GUI_TEST_CLASS_DEFINITION(test_0016_1) {
//    Sequences with length less or equal than 50 should be aligned without gaps, even the result alignment is worse in this case.

    ExternalTool *mafftTool = AppContext::getExternalToolRegistry()->getById("USUPP_MAFFT");
    CHECK_SET_ERR(NULL != mafftTool, "Can't find MAFFT tool in the registry");
    CHECK_SET_ERR(mafftTool->isValid(), "MAFFT tool is not valid");

//    1. Open "_common_data/clustal/COI na.aln".
    GTFileDialog::openFile(os, testDir + "_common_data/clustal/COI na.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    2. Click "Align sequence(s) to this alignment" button on the toolbar.
//    3. Select "_common_data/scenarios/add_and_align/seq1.fa" as sequence to align.
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, testDir + "_common_data/scenarios/add_and_align/seq1.fa"));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Align sequence(s) to this alignment");

    GTUtilsTaskTreeView::waitTaskFinished(os);

//    Expected state: the new sequence doesn't have gaps within the sequence data.
    const QString expectedRowData = "---------TAATTCGTTCAGAACTAAGACAACCCGGTGTACTTTTATTGGTGATAGTC-----------";
    const QString actualRowData = GTUtilsMSAEditorSequenceArea::getSequenceData(os, 18).left(expectedRowData.length());
    CHECK_SET_ERR(expectedRowData == actualRowData, QString("Unexpected row data: expected '%1', got '%2'").arg(expectedRowData).arg(actualRowData));
}

GUI_TEST_CLASS_DEFINITION(test_0016_2) {
//    Sequences with length greater than 50 should be aligned with gaps

    ExternalTool *mafftTool = AppContext::getExternalToolRegistry()->getById("USUPP_MAFFT");
    CHECK_SET_ERR(NULL != mafftTool, "Can't find MAFFT tool in the registry");
    CHECK_SET_ERR(mafftTool->isValid(), "MAFFT tool is not valid");

//    1. Open "_common_data/clustal/COI na.aln".
    GTFileDialog::openFile(os, testDir + "_common_data/clustal/COI na.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    2. Click "Align sequence(s) to this alignment" button on the toolbar.
//    3. Select "_common_data/scenarios/add_and_align/seq2.fa" as sequence to align.
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, testDir + "_common_data/scenarios/add_and_align/seq2.fa"));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Align sequence(s) to this alignment");

    GTUtilsTaskTreeView::waitTaskFinished(os);

//    Expected state: the new sequence has a gap within the sequence data.
    const QString expectedRowData = "---------TAATTCGTTCAGAACTAAGACAACCCGG-TGTACTTTTATTGGTGATAGTCA---------";
    const QString actualRowData = GTUtilsMSAEditorSequenceArea::getSequenceData(os, 18).left(expectedRowData.length());
    CHECK_SET_ERR(expectedRowData == actualRowData, QString("Unexpected row data: expected '%1', got '%2'").arg(expectedRowData).arg(actualRowData));
}

GUI_TEST_CLASS_DEFINITION(test_0016_3) {
//    Sequences with length greater than 50 should be aligned with gaps
//    Sequences with length less or equal than 50 should be aligned without gaps, even the result alignment is worse in this case.
//    This behaviour should be applied, even if input data is alignment

    ExternalTool *mafftTool = AppContext::getExternalToolRegistry()->getById("USUPP_MAFFT");
    CHECK_SET_ERR(NULL != mafftTool, "Can't find MAFFT tool in the registry");
    CHECK_SET_ERR(mafftTool->isValid(), "MAFFT tool is not valid");

//    1. Open "_common_data/clustal/COI na.aln".
    GTFileDialog::openFile(os, testDir + "_common_data/clustal/COI na.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    2. Click "Align sequence(s) to this alignment" button on the toolbar.
//    3. Select "_common_data/scenarios/add_and_align/two_seqs.aln" as input data.
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, testDir + "_common_data/scenarios/add_and_align/two_seqs.aln"));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Align sequence(s) to this alignment");

    GTUtilsTaskTreeView::waitTaskFinished(os);

//    Expected state: sequence 'seq1' doesn't have gaps within the sequence data, sequence 'seq2' has a gap within the sequence data.
    const QString expectedSeq1Data = "---------TAATTCGTTCAGAACTAAGACAACCCGGTGTACTTTTATTGGTGATAGTC-----------";
    const QString actualSeq1Data = GTUtilsMSAEditorSequenceArea::getSequenceData(os, 18).left(expectedSeq1Data.length());
    CHECK_SET_ERR(expectedSeq1Data == actualSeq1Data, QString("Unexpected 'seq1' data: expected '%1', got '%2'").arg(expectedSeq1Data).arg(actualSeq1Data));

    const QString expectedSeq2Data = "---------TAATTCGTTCAGAACTAAGACAACCCGG-TGTACTTTTATTGGTGATAGTCA---------";
    const QString actualSeq2Data = GTUtilsMSAEditorSequenceArea::getSequenceData(os, 19).left(expectedSeq2Data.length());
    CHECK_SET_ERR(expectedSeq2Data == actualSeq2Data, QString("Unexpected 'seq2' data: expected '%1', got '%2'").arg(expectedSeq2Data).arg(actualSeq2Data));
}

} // namespace
} // namespace U2
