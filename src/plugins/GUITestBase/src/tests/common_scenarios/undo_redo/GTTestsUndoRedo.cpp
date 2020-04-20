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
#include <primitives/GTMenu.h>
#include <primitives/GTTreeWidget.h>
#include <primitives/GTTreeWidget.h>
#include <primitives/GTWidget.h>
#include <primitives/PopupChooser.h>
#include <system/GTClipboard.h>
#include <utils/GTUtilsDialog.h>

#include <U2View/MSAEditor.h>

#include "GTTestsUndoRedo.h"
#include "GTUtilsMdi.h"
#include "GTUtilsMsaEditor.h"
#include "GTUtilsMsaEditorSequenceArea.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsTaskTreeView.h"
#include "runnables/ugene/corelibs/U2Gui/util/RenameSequenceFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/DeleteGapsDialogFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/ExtractSelectedAsMSADialogFiller.h"
#include "runnables/ugene/plugins_3rdparty/clustalw/ClustalWDialogFiller.h"
#include "runnables/ugene/plugins_3rdparty/kalign/KalignDialogFiller.h"
#include "runnables/ugene/plugins_3rdparty/umuscle/MuscleDialogFiller.h"

namespace U2{

namespace GUITest_common_scenarios_undo_redo{
using namespace HI;

GUI_TEST_CLASS_DEFINITION(test_0001){//DIFFERENCE: lock document is checked
//Check Undo/Redo functional
//1. Open document COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
//2. Insert seversl spaces somewhere
    GTUtilsMSAEditorSequenceArea::click(os, QPoint(0,0));
    for(int i=0; i<6; i++){
        GTKeyboardDriver::keyClick( Qt::Key_Space);
        GTGlobals::sleep(200);
    }

    QAbstractButton *undo= GTAction::button(os,"msa_action_undo");
    QAbstractButton *redo= GTAction::button(os,"msa_action_redo");
//3. Undo this
    for (int i=0; i<3; i++){
        GTWidget::click(os,undo);
        GTGlobals::sleep(200);
    }
//4. lock document
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os,"COI.aln"));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os,QStringList() << ACTION_DOCUMENT__LOCK));
    GTMouseDriver::click(Qt::RightButton);

//Expected state: Undo and redo buttons are disabled
    CHECK_SET_ERR(!undo->isEnabled(),"Undo button is enebled after locking document");
    CHECK_SET_ERR(!redo->isEnabled(),"Redo button is enebled after locking document");

//5. Unlock document
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os,"COI.aln"));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os,QStringList() << ACTION_DOCUMENT__UNLOCK));
    GTMouseDriver::click(Qt::RightButton);

//Expected state: undo and redo buttons are enebled and work properly
    CHECK_SET_ERR(undo->isEnabled(),"Undo button is disabled after unlocking document");
    CHECK_SET_ERR(redo->isEnabled(),"Redo button is disabled after unlocking document");

    //check undo
    GTWidget::click(os,GTUtilsMdi::activeWindow(os));
    GTWidget::click(os, undo);
    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(0,0), QPoint(9,0));
    GTKeyboardDriver::keyClick('c',Qt::ControlModifier);
    GTGlobals::sleep(500);
    QString clipboardText = GTClipboard::text(os);
    CHECK_SET_ERR(clipboardText=="--TAAGACTT","Undo works wrong. Found text is: " + clipboardText);

    //check redo
    GTWidget::click(os,GTUtilsMdi::activeWindow(os));
    GTWidget::click(os, redo);
    GTGlobals::sleep(200);
    GTWidget::click(os, redo);
    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(0,0), QPoint(9,0));
    GTKeyboardDriver::keyClick('c',Qt::ControlModifier);
    GTGlobals::sleep(500);
    clipboardText = GTClipboard::text(os);
    CHECK_SET_ERR(clipboardText=="----TAAGAC", "Redo works wrong. Found text is: " + clipboardText);
}

GUI_TEST_CLASS_DEFINITION(test_0002){//DIFFERENCE: delete sequence is checked
//Check Undo/Redo functional
//1. Open document COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//2. Delete 4-th sequence
    GTUtilsMSAEditorSequenceArea::moveTo(os, QPoint(-10,3));
    GTUtilsDialog::waitForDialog(os, new RenameSequenceFiller(os, "Roeseliana_roeseli", "Roeseliana_roeseli"));
    GTMouseDriver::doubleClick();
    GTGlobals::sleep(500);
    GTKeyboardDriver::keyClick( Qt::Key_Delete);

// Expected state: sequence deleted
    GTUtilsMSAEditorSequenceArea::moveTo(os, QPoint(-10,3));
    GTUtilsDialog::waitForDialog(os, new RenameSequenceFiller(os, "Montana_montana", "Montana_montana"));
    GTMouseDriver::doubleClick();
    GTGlobals::sleep(500);

//3. undo deletion
    QAbstractButton *undo = GTAction::button(os,"msa_action_undo");
    QAbstractButton *redo = GTAction::button(os,"msa_action_redo");

    GTWidget::click(os, undo);

//Expected state: deletion undone
    GTUtilsMSAEditorSequenceArea::moveTo(os, QPoint(-10,3));
    GTUtilsDialog::waitForDialog(os, new RenameSequenceFiller(os, "Roeseliana_roeseli", "Roeseliana_roeseli"));
    GTMouseDriver::doubleClick();
    GTGlobals::sleep(500);

//4. Redo delition
    GTWidget::click(os, redo);

//Expected state: delition is redone
    GTUtilsMSAEditorSequenceArea::moveTo(os, QPoint(-10,3));
    GTUtilsDialog::waitForDialog(os, new RenameSequenceFiller(os, "Montana_montana", "Montana_montana"));
    GTMouseDriver::doubleClick();
    GTGlobals::sleep(500);
}

GUI_TEST_CLASS_DEFINITION(test_0003){//DIFFERENCE: add sequence is checked
    //Check Undo/Redo functional
//1. Open document COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
//2. add sequence to alignment
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList()<<MSAE_MENU_LOAD<<"Sequence from file"));
    GTFileDialogUtils *ob = new GTFileDialogUtils(os, dataDir + "/samples/Raw/", "raw.seq");
    GTUtilsDialog::waitForDialog(os, ob);
    GTMenu::showContextMenu(os, GTUtilsMdi::activeWindow(os));

// Expected state: raw sequence appeared in alignment
    CHECK_SET_ERR(GTUtilsMSAEditorSequenceArea::getNameList(os).contains("raw"), "raw is not added");
    GTUtilsMSAEditorSequenceArea::moveTo(os, QPoint(-10,18));
    GTUtilsDialog::waitForDialog(os, new RenameSequenceFiller(os, "raw", "raw"));
    GTMouseDriver::doubleClick();

//3. undo adding
    QAbstractButton *undo= GTAction::button(os,"msa_action_undo");
    QAbstractButton *redo= GTAction::button(os,"msa_action_redo");

    GTWidget::click(os, undo);

//Expected state: raw doesn't present in namelist
    QStringList nameList = GTUtilsMSAEditorSequenceArea::getNameList(os);
    CHECK_SET_ERR(!nameList.contains("raw"), "adding raw is not undone");

//4. Redo delition
    GTWidget::click(os, redo);

//Expected state: delition is redone
    CHECK_SET_ERR(GTUtilsMSAEditorSequenceArea::getNameList(os).contains("raw"), "Adding raw is not redone");
    GTUtilsMSAEditorSequenceArea::moveTo(os, QPoint(-10,18));
    GTUtilsDialog::waitForDialog(os, new RenameSequenceFiller(os, "raw", "raw"));
    GTMouseDriver::doubleClick();
}

GUI_TEST_CLASS_DEFINITION(test_0004){//DIFFERENCE: add sequence is checked
//Check Undo/Redo functional
//1. Open document COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
//2. insert gap->undo->insert gap->undo->redo
    QAbstractButton *undo= GTAction::button(os,"msa_action_undo");
    QAbstractButton *redo= GTAction::button(os,"msa_action_redo");

    GTUtilsMSAEditorSequenceArea::click(os, QPoint(0,0));
    GTKeyboardDriver::keyClick( Qt::Key_Space);
    GTWidget::click(os, undo);

    GTUtilsMSAEditorSequenceArea::click(os, QPoint(0,0));
    GTKeyboardDriver::keyClick( Qt::Key_Space);
    GTWidget::click(os, undo);

    GTWidget::click(os, redo);

// Expected state: redo button is disabled
    CHECK_SET_ERR(!redo->isEnabled(), "Redo button is enebled");
}

GUI_TEST_CLASS_DEFINITION(test_0005){//undo remove selection
    //open file
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //remove selection
    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(0,0), QPoint(3,1));
    GTKeyboardDriver::keyClick( Qt::Key_Delete);

    //Expected state: selection removed
    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(0,0), QPoint(3,1));
    GTKeyboardDriver::keyClick( 'c', Qt::ControlModifier);
    GTGlobals::sleep(500);
    QString clipdoardText = GTClipboard::text(os);
    CHECK_SET_ERR(clipdoardText=="ACTT\nCTTA", QString("Expected ACTT\nCTTA, found: %1").arg(clipdoardText));

    //undo
    QAbstractButton *undo= GTAction::button(os,"msa_action_undo");
    GTWidget::click(os, undo);

    //Expected state: delition undone
    GTWidget::click(os,GTUtilsMdi::activeWindow(os));
    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(0,0), QPoint(3,1));
    GTKeyboardDriver::keyClick( 'c', Qt::ControlModifier);
    GTGlobals::sleep(500);
    clipdoardText = GTClipboard::text(os);
    CHECK_SET_ERR(clipdoardText=="TAAG\nTAAG", QString("Expected TAAG\nTAAG, found: %1").arg(clipdoardText));

    //redo
    QAbstractButton *redo= GTAction::button(os,"msa_action_redo");
    GTWidget::click(os, redo);

    //Expected state: delition redone
    GTWidget::click(os,GTUtilsMdi::activeWindow(os));
    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(0,0), QPoint(3,1));
    GTKeyboardDriver::keyClick( 'c', Qt::ControlModifier);
    GTGlobals::sleep(500);
    clipdoardText = GTClipboard::text(os);
    CHECK_SET_ERR(clipdoardText=="ACTT\nCTTA", QString("Expected ACTT\nCTTA, found: %1").arg(clipdoardText));
}

GUI_TEST_CLASS_DEFINITION(test_0006){//undo replace_selected_rows_with_reverse-complement
// In-place reverse complement replace in MSA Editor (0002425)

// 1. Open file _common_data\scenarios\msa\translations_nucl.aln
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/", "translations_nucl.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

// 2. Select first sequence and do context menu {Edit->Replace selected rows with reverce complement}
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << MSAE_MENU_EDIT << "replace_selected_rows_with_reverse-complement"));
    GTUtilsMSAEditorSequenceArea::selectArea( os, QPoint( 0, 0 ), QPoint( -1, 2 ) );
    GTMouseDriver::click(Qt::RightButton);

// Expected state: sequence changed from TTG -> CAA
    GTGlobals::sleep(500);
    GTKeyboardDriver::keyClick( 'c', Qt::ControlModifier);

    GTGlobals::sleep(500);
    QString clipboardText = GTClipboard::text(os);
    CHECK_SET_ERR(clipboardText == "CAA\nTGA\nATC",
        "Clipboard string and expected MSA string differs");

//  sequence name changed from L -> L|revcompl
    QStringList nameList = GTUtilsMSAEditorSequenceArea::getNameList(os);
    CHECK_SET_ERR( nameList.size( ) >= 6, "nameList doesn't contain enough strings" );
    CHECK_SET_ERR( ( nameList[0] == "L|revcompl" )
        && ( nameList[1] == "S|revcompl" )
        && ( nameList[2] == "D|revcompl" ), "Unexpected sequence names" );

// 3. Undo
    QAbstractButton *undo= GTAction::button(os,"msa_action_undo");
    GTWidget::click(os, undo);

// Expected state: sequence changed from CAA -> TTG
    GTGlobals::sleep(500);
    GTKeyboardDriver::keyClick( 'c', Qt::ControlModifier);

    GTGlobals::sleep(500);
    clipboardText = GTClipboard::text(os);
    CHECK_SET_ERR( clipboardText == "TTG\nTCA\nGAT",
        "Clipboard string and expected MSA string differs" );

//  sequence name changed from L|revcompl ->
    nameList = GTUtilsMSAEditorSequenceArea::getNameList(os);
    CHECK_SET_ERR( nameList.size( ) >= 3, "nameList doesn't contain enough strings" );
    CHECK_SET_ERR( ( nameList[0] == "L" ) && ( nameList[1] == "S" ) && ( nameList[2] == "D" ),
        "There are unexpected names in nameList" );

    GTGlobals::sleep(500);

// 4. Redo
    QAbstractButton *redo= GTAction::button(os,"msa_action_redo");
    GTWidget::click(os, redo);

// Expected state: sequence changed from TTG -> CAA
    GTGlobals::sleep(500);
    GTKeyboardDriver::keyClick( 'c', Qt::ControlModifier);

    GTGlobals::sleep(500);
    clipboardText = GTClipboard::text(os);
    CHECK_SET_ERR(clipboardText == "CAA\nTGA\nATC",
        "Clipboard string and expected MSA string differs");

//  sequence name changed from L -> L|revcompl
    nameList = GTUtilsMSAEditorSequenceArea::getNameList(os);
    CHECK_SET_ERR( nameList.size( ) >= 6, "nameList doesn't contain enough strings" );
    CHECK_SET_ERR( ( nameList[0] == "L|revcompl" )
        && ( nameList[1] == "S|revcompl" )
        && ( nameList[2] == "D|revcompl" ), "Unexpected sequence names" );
}

GUI_TEST_CLASS_DEFINITION(test_0006_1){//undo replace_selected_rows_with_reverse
// In-place reverse complement replace in MSA Editor (0002425)

// 1. Open file _common_data\scenarios\msa\translations_nucl.aln
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/", "translations_nucl.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

// 2. Select first sequence and do context menu {Edit->Replace selected rows with reverce complement}
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << MSAE_MENU_EDIT << "replace_selected_rows_with_reverse"));
    GTUtilsMSAEditorSequenceArea::selectArea( os, QPoint( 0, 0 ), QPoint( -1, 2 ) );
    GTMouseDriver::click(Qt::RightButton);

// Expected state: sequence changed from TTG -> GTT
    GTGlobals::sleep(500);
    GTKeyboardDriver::keyClick( 'c', Qt::ControlModifier);

    GTGlobals::sleep(500);
    QString clipboardText = GTClipboard::text(os);
    CHECK_SET_ERR( clipboardText == "GTT\nACT\nTAG",
        "Clipboard string and expected MSA string differs");

// sequence name  changed from L -> L|revcompl
    QStringList nameList = GTUtilsMSAEditorSequenceArea::getNameList(os);
    CHECK_SET_ERR( nameList.size( ) >= 6, "nameList doesn't contain enough strings" );
    CHECK_SET_ERR( ( nameList[0] == "L|rev" )
        && ( nameList[1] == "S|rev" )
        && ( nameList[2] == "D|rev" ), "Unexpected sequence names" );

// 3. Undo
    QAbstractButton *undo= GTAction::button(os,"msa_action_undo");
    GTWidget::click(os, undo);

// Expected state: sequence changed from GTT -> TTG
    GTGlobals::sleep(500);
    GTKeyboardDriver::keyClick( 'c', Qt::ControlModifier);

    GTGlobals::sleep(500);
    clipboardText = GTClipboard::text(os);
    CHECK_SET_ERR( clipboardText == "TTG\nTCA\nGAT",
        "Clipboard string and expected MSA string differs" );

//  sequence name changed from L|rev ->
    nameList = GTUtilsMSAEditorSequenceArea::getNameList(os);
    CHECK_SET_ERR( nameList.size( ) >= 3, "nameList doesn't contain enough strings" );
    CHECK_SET_ERR( ( nameList[0] == "L" ) && ( nameList[1] == "S" ) && ( nameList[2] == "D" ),
        "There are unexpected names in nameList" );

    GTGlobals::sleep(500);

// 4. Redo
    QAbstractButton *redo= GTAction::button(os,"msa_action_redo");
    GTWidget::click(os, redo);

// Expected state: sequence changed from TTG -> GTT
    GTGlobals::sleep(500);
    GTKeyboardDriver::keyClick( 'c', Qt::ControlModifier);

    GTGlobals::sleep(500);
    clipboardText = GTClipboard::text(os);
    CHECK_SET_ERR( clipboardText == "GTT\nACT\nTAG",
        "Clipboard string and expected MSA string differs");

//  sequence name changed from L -> L|revcompl
    nameList = GTUtilsMSAEditorSequenceArea::getNameList(os);
    CHECK_SET_ERR( nameList.size( ) >= 6, "nameList doesn't contain enough strings" );
    CHECK_SET_ERR( ( nameList[0] == "L|rev" )
        && ( nameList[1] == "S|rev" )
        && ( nameList[2] == "D|rev" ), "Unexpected sequence names" );
}

GUI_TEST_CLASS_DEFINITION( test_0006_2 )
{
//undo replace_selected_rows_with_complement
// In-place reverse complement replace in MSA Editor (0002425)

// 1. Open file _common_data\scenarios\msa\translations_nucl.aln
    GTFileDialog::openFile( os, testDir + "_common_data/scenarios/msa/", "translations_nucl.aln" );
    GTUtilsTaskTreeView::waitTaskFinished(os);

// 2. Select first sequence and do context menu {Edit->Replace selected rows with reverce complement}
    GTUtilsDialog::waitForDialog( os, new PopupChooser( os, QStringList( ) << MSAE_MENU_EDIT
        << "replace_selected_rows_with_complement" ) );
    GTUtilsMSAEditorSequenceArea::selectArea( os, QPoint( 0, 0 ), QPoint( -1, 2 ) );
    GTMouseDriver::click(Qt::RightButton );

// Expected state: sequence changed from TTG -> AAC
    GTGlobals::sleep( 500 );
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier );

    GTGlobals::sleep( 500 );
    QString clipboardText = GTClipboard::text( os );
    CHECK_SET_ERR( clipboardText == "AAC\nAGT\nCTA",
        "Clipboard string and expected MSA string differs. Clipboard string is: "
        + clipboardText );

//  sequence name  changed from L -> L|compl
    QStringList nameList = GTUtilsMSAEditorSequenceArea::getNameList( os );
    CHECK_SET_ERR( nameList.size( ) >= 6, "nameList doesn't contain enough strings" );
    CHECK_SET_ERR( ( nameList[0] == "L|compl" )
        && ( nameList[1] == "S|compl" )
        && ( nameList[2] == "D|compl" ), "Unexpected sequence names" );

// 3. Undo
    QAbstractButton *undo = GTAction::button( os, "msa_action_undo" );
    GTWidget::click( os, undo );

// Expected state: sequence changed from AAC -> TTG
    GTGlobals::sleep( 500 );
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier );

    GTGlobals::sleep( 500 );
    clipboardText = GTClipboard::text( os );
    CHECK_SET_ERR( clipboardText == "TTG\nTCA\nGAT",
        "Clipboard string and expected MSA string differs" );

//  sequence name changed from L|rev ->
    nameList = GTUtilsMSAEditorSequenceArea::getNameList( os );
    CHECK_SET_ERR( nameList.size( ) >= 3, "nameList doesn't contain enough strings" );
    CHECK_SET_ERR( ( nameList[0] == "L" ) && ( nameList[1] == "S" ) && ( nameList[2] == "D" ),
        "There are unexpected names in nameList" );

    GTGlobals::sleep( 500 );

// 4. Redo
    QAbstractButton *redo = GTAction::button( os, "msa_action_redo" );
    GTWidget::click( os, redo );

// Expected state: sequence changed from TTG -> AAC
    GTGlobals::sleep( 500 );
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier );

    GTGlobals::sleep( 500 );
    clipboardText = GTClipboard::text( os );
    CHECK_SET_ERR( clipboardText == "AAC\nAGT\nCTA",
        "Clipboard string and expected MSA string differs" );

//  sequence name  changed from L -> L|revcompl
    nameList = GTUtilsMSAEditorSequenceArea::getNameList( os );
    CHECK_SET_ERR( nameList.size( ) >= 6, "nameList doesn't contain enough strings" );
    CHECK_SET_ERR( ( nameList[0] == "L|compl" )
        && ( nameList[1] == "S|compl" )
        && ( nameList[2] == "D|compl" ),
       "There are unexpected names in nameList" );
}

GUI_TEST_CLASS_DEFINITION(test_0007) {
    // remove columns with 3 or more gaps
    //Open file
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/ma2_gap_col.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //save initial state
    const QStringList originalMsa = GTUtilsMsaEditor::getWholeData(os);
    const QStringList expectedChangedMsa = QStringList() << "AAGCTTCTTT"
                                                         << "AAGTTACTAA"
                                                         << "TAG---TTAT"
                                                         << "AAGC---TAT"
                                                         << "TAGTTATTAA"
                                                         << "TAGTTATTAA"
                                                         << "TAGTTATTAA"
                                                         << "AAGCTTT---"
                                                         << "A--AGAATAA"
                                                         << "AAGCTTTTAA";

    //fill remove columns of gaps dialog
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << MSAE_MENU_EDIT << "remove_columns_of_gaps", GTGlobals::UseMouse));
    GTUtilsDialog::waitForDialog(os, new RemoveGapColsDialogFiller(os, RemoveGapColsDialogFiller::Number, 3));
    GTMenu::showContextMenu(os, GTUtilsMdi::activeWindow(os));

    const QStringList changedMsa = GTUtilsMsaEditor::getWholeData(os);
    CHECK_SET_ERR(changedMsa == expectedChangedMsa, "remove gaps option works wrong");

    //undo
    GTUtilsMsaEditor::undo(os);

    const QStringList undoneMsa = GTUtilsMsaEditor::getWholeData(os);
    CHECK_SET_ERR(undoneMsa == originalMsa, "undo works wrong");

    //redo
    GTUtilsMsaEditor::redo(os);

    const QStringList redoneMsa = GTUtilsMsaEditor::getWholeData(os);
    CHECK_SET_ERR(redoneMsa == expectedChangedMsa, "redo works wrong");
}

GUI_TEST_CLASS_DEFINITION(test_0007_1) {
    // remove columns with 15 percents of gaps
    //Open file
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/ma2_gap_col.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //save initial state
    const QStringList originalMsa = GTUtilsMsaEditor::getWholeData(os);
    const QStringList expectedChangedMsa = QStringList() << "AAGCCTTT"
                                                         << "AAGTCTAA"
                                                         << "TAG-TTAT"
                                                         << "AAGC-TAT"
                                                         << "TAGTTTAA"
                                                         << "TAGTTTAA"
                                                         << "TAGTTTAA"
                                                         << "AAGCT---"
                                                         << "A--AATAA"
                                                         << "AAGCTTAA";

    //fill remove columns of gaps dialog
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << MSAE_MENU_EDIT << "remove_columns_of_gaps", GTGlobals::UseMouse));
    GTUtilsDialog::waitForDialog(os, new RemoveGapColsDialogFiller(os, RemoveGapColsDialogFiller::Percent, 15));
    GTMenu::showContextMenu(os, GTUtilsMdi::activeWindow(os));

    const QStringList changedMsa = GTUtilsMsaEditor::getWholeData(os);
    CHECK_SET_ERR(changedMsa == expectedChangedMsa, "remove gaps option works wrong:\nChenged MSA:\n" + changedMsa.join("\n") + "\nOriginal MSA:\n" + expectedChangedMsa.join("\n"));

    //undo
    GTUtilsMsaEditor::undo(os);

    const QStringList undoneMsa = GTUtilsMsaEditor::getWholeData(os);
    CHECK_SET_ERR(undoneMsa == originalMsa, "undo works wrong");

    //redo
    GTUtilsMsaEditor::redo(os);

    const QStringList redoneMsa = GTUtilsMsaEditor::getWholeData(os);
    CHECK_SET_ERR(redoneMsa == expectedChangedMsa, "redo works wrong");
}

GUI_TEST_CLASS_DEFINITION(test_0007_2) {
    // remove columns of gaps is tested
    //Open file
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/ma2_gap_col.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //save initial state
    const QStringList originalMsa = GTUtilsMsaEditor::getWholeData(os);
    const QStringList expectedChangedMsa = QStringList() << "AAGCTTCTTTTAA"
                                                         << "AAGTTACTAA---"
                                                         << "TAG---TTATTAA"
                                                         << "AAGC---TATTAA"
                                                         << "TAGTTATTAA---"
                                                         << "TAGTTATTAA---"
                                                         << "TAGTTATTAA---"
                                                         << "AAGCTTT---TAA"
                                                         << "A--AGAATAATTA"
                                                         << "AAGCTTTTAA---";

    //fill remove columns of gaps dialog
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << MSAE_MENU_EDIT << "remove_columns_of_gaps", GTGlobals::UseMouse));
    GTUtilsDialog::waitForDialog(os, new RemoveGapColsDialogFiller(os, RemoveGapColsDialogFiller::Column));
    GTMenu::showContextMenu(os, GTUtilsMdi::activeWindow(os));

    const QStringList changedMsa = GTUtilsMsaEditor::getWholeData(os);
    CHECK_SET_ERR(changedMsa == expectedChangedMsa, "remove gaps option works wrong:\nChenged MSA:\n" + changedMsa.join("\n") + "\nOriginal MSA:\n" + expectedChangedMsa.join("\n"));

    //undo
    GTUtilsMsaEditor::undo(os);

    const QStringList undoneMsa = GTUtilsMsaEditor::getWholeData(os);
    CHECK_SET_ERR(undoneMsa == originalMsa, "undo works wrong");

    //redo
    GTUtilsMsaEditor::redo(os);

    const QStringList redoneMsa = GTUtilsMsaEditor::getWholeData(os);
    CHECK_SET_ERR(redoneMsa == expectedChangedMsa, "redo works wrong");
}

GUI_TEST_CLASS_DEFINITION(test_0008) {
    // remove all gaps is tested
    //Open file
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/ma2_gap_col.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //save initial state
    const QStringList originalMsa = GTUtilsMsaEditor::getWholeData(os);
    const QStringList expectedChangedMsa = QStringList() << "AAGCTTCTTTTAA"
                                                         << "AAGTTACTAA---"
                                                         << "TAGTTATTAA---"
                                                         << "AAGCTATTAA---"
                                                         << "TAGTTATTAA---"
                                                         << "TAGTTATTAA---"
                                                         << "TAGTTATTAA---"
                                                         << "AAGCTTTTAA---"
                                                         << "AAGAATAATTA--"
                                                         << "AAGCTTTTAA---";

    //fill remove columns of gaps dialog
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << MSAE_MENU_EDIT << "Remove all gaps", GTGlobals::UseMouse));
    GTMenu::showContextMenu(os, GTUtilsMdi::activeWindow(os));

    const QStringList changedMsa = GTUtilsMsaEditor::getWholeData(os);
    CHECK_SET_ERR(changedMsa == expectedChangedMsa, "remove gaps option works wrong:\nChenged MSA:\n" + changedMsa.join("\n") + "\nOriginal MSA:\n" + expectedChangedMsa.join("\n"));

    //undo
    GTUtilsMsaEditor::undo(os);

    const QStringList undoneMsa = GTUtilsMsaEditor::getWholeData(os);
    CHECK_SET_ERR(undoneMsa == originalMsa, "undo works wrong");

    //redo
    GTUtilsMsaEditor::redo(os);

    const QStringList redoneMsa = GTUtilsMsaEditor::getWholeData(os);
    CHECK_SET_ERR(redoneMsa == expectedChangedMsa, "redo works wrong");
}

GUI_TEST_CLASS_DEFINITION(test_0009) {
    // rename msa is tested
    //Open file
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/ma2_gap_col.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //rename msa
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "Rename"));
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "ma2_gap_col"));
    GTMouseDriver::click(Qt::RightButton);
    GTKeyboardDriver::keySequence("some_name");
    GTKeyboardDriver::keyClick(Qt::Key_Enter);

    //Expected state: msa renamed
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "some_name"));

    //undo
    GTUtilsMsaEditor::undo(os);

    //Expected state: rename undone
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "ma2_gap_col"));

    //redo
    GTUtilsMsaEditor::redo(os);

    //Expected state: rename redone
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "some_name"));
}

GUI_TEST_CLASS_DEFINITION(test_0010) {
    // MUSCLE aligner undo test
    //Open file
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    const QStringList originalMsa = GTUtilsMsaEditor::getWholeData(os);
    const QStringList expectedChangedMsa = QStringList() << "AAG---AATAATTA"
                                                         << "AAG---TCTATTAA"
                                                         << "AAGACTTCTTTTAA"
                                                         << "AAG---TCTTTTAA"
                                                         << "AAG---CCTTTTAA"
                                                         << "AAG---CTTACTAA"
                                                         << "TAG---TTTATTAA"
                                                         << "TAG---CTTATTAA"
                                                         << "TAG---CTTATTAA"
                                                         << "TAG---CTTATTAA";

    //Use context {Edit->Align with MUSCLE}
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << MSAE_MENU_ALIGN << "Align with muscle", GTGlobals::UseMouse));
    GTUtilsDialog::waitForDialog(os, new MuscleDialogFiller(os, MuscleDialogFiller::Default, false));
    GTMenu::showContextMenu(os, GTUtilsMdi::activeWindow(os));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    const QStringList changedMsa = GTUtilsMsaEditor::getWholeData(os);
    CHECK_SET_ERR(changedMsa == expectedChangedMsa, "Unexpected alignment:\n" + changedMsa.join("\n"));

    //undo
    GTUtilsMsaEditor::undo(os);

    const QStringList undoneMsa = GTUtilsMsaEditor::getWholeData(os);
    CHECK_SET_ERR(undoneMsa == originalMsa, "Undo works wrong:\n" + undoneMsa.join("\n"));

    //redo
    GTUtilsMsaEditor::redo(os);

    const QStringList redoneMsa = GTUtilsMsaEditor::getWholeData(os);
    CHECK_SET_ERR(redoneMsa == expectedChangedMsa, "Redo works wrong:\n" + redoneMsa.join("\n"));
}

GUI_TEST_CLASS_DEFINITION(test_0011) {
    // Kalign undo test
    //Open file
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    const QStringList originalMsa = GTUtilsMsaEditor::getWholeData(os);
    const QStringList expectedChangedMsa = QStringList() << "AAGACTTCTTTTAA"
                                                         << "AAGCTTACT---AA"
                                                         << "TAGTTTATT---AA"
                                                         << "AAGTCTATT---AA"
                                                         << "TAGCTTATT---AA"
                                                         << "TAGCTTATT---AA"
                                                         << "TAGCTTATT---AA"
                                                         << "AAGTCTTTT---AA"
                                                         << "AAGAATAAT---TA"
                                                         << "AAGCCTTTT---AA";

    //Use context {Edit->Align with Kalign}
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << MSAE_MENU_ALIGN << "align_with_kalign", GTGlobals::UseKey));
    GTUtilsDialog::waitForDialog(os, new KalignDialogFiller(os));
    GTMenu::showContextMenu(os, GTUtilsMdi::activeWindow(os));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    const QStringList changedMsa = GTUtilsMsaEditor::getWholeData(os);
    CHECK_SET_ERR(changedMsa == expectedChangedMsa, "Unexpected alignment:\n" + changedMsa.join("\n"));

    //undo
    GTUtilsMsaEditor::undo(os);

    const QStringList undoneMsa = GTUtilsMsaEditor::getWholeData(os);
    CHECK_SET_ERR(undoneMsa == originalMsa, "Undo works wrong:\n" + undoneMsa.join("\n"));

    //redo
    GTUtilsMsaEditor::redo(os);

    const QStringList redoneMsa = GTUtilsMsaEditor::getWholeData(os);
    CHECK_SET_ERR(redoneMsa == expectedChangedMsa, "Redo works wrong:\n" + redoneMsa.join("\n"));
}

GUI_TEST_CLASS_DEFINITION(test_0011_1) {
    //Kalign undo test
    //Open file
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    const QStringList originalMsa = GTUtilsMsaEditor::getWholeData(os);
    const QStringList expectedChangedMsa = QStringList() << "AAGACTTCTTTTAA"
                                                         << "AAG-CTTACT--AA"
                                                         << "TAG-TTTATT--AA"
                                                         << "AAG-TCTATT--AA"
                                                         << "TAG-CTTATT--AA"
                                                         << "TAG-CTTATT--AA"
                                                         << "TAG-CTTATT--AA"
                                                         << "AAG-TCTTTT--AA"
                                                         << "AAG-AATAAT--TA"
                                                         << "AAG-CCTTTT--AA";

    //Use context {Edit->Align with Kalign}
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << MSAE_MENU_ALIGN << "align_with_kalign", GTGlobals::UseMouse));
    GTUtilsDialog::waitForDialog(os, new KalignDialogFiller(os, 100));
    GTMenu::showContextMenu(os, GTUtilsMdi::activeWindow(os));

    GTUtilsTaskTreeView::waitTaskFinished(os);

    const QStringList changedMsa = GTUtilsMsaEditor::getWholeData(os);
    CHECK_SET_ERR(changedMsa == expectedChangedMsa, "Unexpected alignment:\n" + changedMsa.join("\n"));

    //undo
    GTUtilsMsaEditor::undo(os);

    const QStringList undoneMsa = GTUtilsMsaEditor::getWholeData(os);
    CHECK_SET_ERR(undoneMsa == originalMsa, "Undo works wrong:\n" + undoneMsa.join("\n"));

    //redo
    GTUtilsMsaEditor::redo(os);

    const QStringList redoneMsa = GTUtilsMsaEditor::getWholeData(os);
    CHECK_SET_ERR(redoneMsa == expectedChangedMsa, "Redo works wrong:\n" + redoneMsa.join("\n"));
}

GUI_TEST_CLASS_DEFINITION(test_0012) {
    // ClustalW aligner undo test
    //Open file
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    const QStringList originalMsa = GTUtilsMsaEditor::getWholeData(os);
    const QStringList expectedChangedMsa = QStringList() << "---AAGACTTCTTTTAA"
                                                         << "---AAGCTT---ACTAA"
                                                         << "---TAGT---TTATTAA"
                                                         << "---AAGTC---TATTAA"
                                                         << "---TAGCTT---ATTAA"
                                                         << "---TAGCTT---ATTAA"
                                                         << "---TAGCTT---ATTAA"
                                                         << "---AAGTCTTT---TAA"
                                                         << "A---AGAAT--AATTA-"
                                                         << "---AAGCCT---TTTAA";

    //Use context {Edit->Align with Kalign}
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << MSAE_MENU_ALIGN << "Align with ClustalW", GTGlobals::UseMouse));
    GTUtilsDialog::waitForDialog(os, new ClustalWDialogFiller(os));
    GTMenu::showContextMenu(os, GTUtilsMdi::activeWindow(os));

    const QStringList changedMsa = GTUtilsMsaEditor::getWholeData(os);
    CHECK_SET_ERR(changedMsa == expectedChangedMsa, "Unexpected alignment:\n" + changedMsa.join("\n"));

    //undo
    GTUtilsMsaEditor::undo(os);

    const QStringList undoneMsa = GTUtilsMsaEditor::getWholeData(os);
    CHECK_SET_ERR(undoneMsa == originalMsa, "Undo works wrong:\n" + undoneMsa.join("\n"));

    //redo
    GTUtilsMsaEditor::redo(os);

    const QStringList redoneMsa = GTUtilsMsaEditor::getWholeData(os);
    CHECK_SET_ERR(redoneMsa == expectedChangedMsa, "Redo works wrong:\n" + redoneMsa.join("\n"));
}

}   // namespace GUITest_common_scenarios_undo_redo
}   // namespace U2
