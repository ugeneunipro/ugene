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

#include <QApplication>
#include <QClipboard>
#include <QTreeWidgetItem>

#include "GTGlobals.h"

#include "GTTestsSequenceEditMode.h"

#include "GTUtilsAnnotationsTreeView.h"
#include "GTUtilsDocument.h"
#include "GTUtilsLog.h"
#include "GTUtilsMdi.h"
#include "GTUtilsMdi.h"
#include "GTUtilsNotifications.h"
#include "GTUtilsProject.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsSequenceView.h"
#include "GTUtilsTaskTreeView.h"

#include "system/GTClipboard.h"

#include "utils/GTKeyboardUtils.h"
#include "utils/GTUtilsApp.h"
#include "utils/GTUtilsToolTip.h"

#include <base_dialogs/GTFileDialog.h>

#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>

#include "primitives/GTMenu.h"
#include "primitives/PopupChooser.h"
#include <primitives/GTAction.h>
#include <primitives/GTTreeWidget.h>

#include "runnables/ugene/corelibs/U2Gui/EditSettingsDialogFiller.h"

#include <U2Core/U2IdTypes.h>

#include <U2View/DetView.h>


namespace U2{

namespace GUITest_common_scenarios_sequence_edit_mode {
using namespace HI;

GUI_TEST_CLASS_DEFINITION(without_anns_test_0001) {
    //1. Open human_T1.fa
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Uncheck "Wrap sequence" and "Show compliment strans" button
    QAction* wrapMode = GTAction::findActionByText(os, "Wrap sequence");
    CHECK_SET_ERR(wrapMode != NULL, "Cannot find Wrap sequence action");
    if (wrapMode->isChecked()) {
        GTWidget::click(os, GTAction::button(os, wrapMode));
    }

    QAction* compStrand = GTAction::findActionByText(os, "Show complementary strand");
    CHECK_SET_ERR(compStrand != NULL, "Cannot find Wrap sequence action");
    if (compStrand->isChecked()) {
        GTWidget::click(os, GTAction::button(os, compStrand));
    }

    //3. Check "Edit sequence" button
    GTUtilsSequenceView::enableEditingMode(os);

    //Expected state : Blinked cursor at the sequence  beggining
    const qint64 pos = GTUtilsSequenceView::getCursor(os);
    CHECK_SET_ERR(pos == 0, QString("Unexpected cursor pos, expected: 0, current %1").arg(pos));

    //4. Print "A, C, G, T, N, gap" symbols
    GTKeyboardDriver::keyClick('A');
    GTGlobals::sleep(100);
    GTKeyboardDriver::keyClick('C');
    GTGlobals::sleep(100);
    GTKeyboardDriver::keyClick('G');
    GTGlobals::sleep(100);
    GTKeyboardDriver::keyClick('T');
    GTGlobals::sleep(100);
    GTKeyboardDriver::keyClick('N');
    GTGlobals::sleep(100);
    GTKeyboardDriver::keyClick(Qt::Key_Space);
    GTGlobals::sleep(100);

    //Expected state: Sequence starts with "A,C, G,T,N,gap"
    QString string = GTUtilsSequenceView::getRegionAsString(os, U2Region(1, 6));
    CHECK_SET_ERR(string == "ACGTN-", QString("Unexpected string in the begginning of the sequence, expected: ACGTN-, current: %1").arg(string));

    //5. Put cursor in "199 939" position and press "A,C, G,T,N,gap"
    GTUtilsSequenceView::setCursor(os, 199939);

    GTKeyboardDriver::keyClick('A');
    GTGlobals::sleep(100);
    GTKeyboardDriver::keyClick('C');
    GTGlobals::sleep(100);
    GTKeyboardDriver::keyClick('G');
    GTGlobals::sleep(100);
    GTKeyboardDriver::keyClick('T');
    GTGlobals::sleep(100);
    GTKeyboardDriver::keyClick('N');
    GTGlobals::sleep(100);
    GTKeyboardDriver::keyClick(Qt::Key_Space);
    GTGlobals::sleep(100);

    //Expected state : Sequence ends with "A,C, G,T,N,gap"
    string = GTUtilsSequenceView::getRegionAsString(os, U2Region(199940, 6));
    CHECK_SET_ERR(string == "ACGTN-", QString("Unexpected string in the ending of the sequence, expected: ACGTN-, current: %1").arg(string));
}

GUI_TEST_CLASS_DEFINITION(without_anns_test_0002) {
    //1. Open human_T1.fa
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Uncheck "Wrap sequence" and "Show compliment strans" button
    QAction* wrapMode = GTAction::findActionByText(os, "Wrap sequence");
    CHECK_SET_ERR(wrapMode != NULL, "Cannot find Wrap sequence action");
    if (wrapMode->isChecked()) {
        GTWidget::click(os, GTAction::button(os, wrapMode));
    }

    QAction* compStrand = GTAction::findActionByText(os, "Show complementary strand");
    CHECK_SET_ERR(compStrand != NULL, "Cannot find Wrap sequence action");
    if (compStrand->isChecked()) {
        GTWidget::click(os, GTAction::button(os, compStrand));
    }

    //3. Check "Edit sequence" button
    GTUtilsSequenceView::enableEditingMode(os);

    //Expected state : Blinked cursor at the sequence  beggining
    const qint64 pos = GTUtilsSequenceView::getCursor(os);
    CHECK_SET_ERR(pos == 0, QString("Unexpected cursor pos, expected: 0, current %1").arg(pos));

    //4. Print "QWER" symbols
    GTKeyboardDriver::keyClick('Q');
    GTGlobals::sleep(100);
    GTKeyboardDriver::keyClick('W');
    GTGlobals::sleep(100);
    GTKeyboardDriver::keyClick('E');
    GTGlobals::sleep(100);
    GTKeyboardDriver::keyClick('R');
    GTGlobals::sleep(100);

    //Expected state: Sequence starts with "A,C, G,T,N,gap"
    QString string = GTUtilsSequenceView::getRegionAsString(os, U2Region(1, 4));
    bool hasUnexpectedSymbols = string.contains("Q") || string.contains("W") || string.contains("E") || string.contains("R");
    CHECK_SET_ERR(!hasUnexpectedSymbols, "Some unexpected symbols was inserted");

}

GUI_TEST_CLASS_DEFINITION(without_anns_test_0003) {
    //1. Open human_T1.fa
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Uncheck "Wrap sequence" and "Show compliment strans" button
    QAction* wrapMode = GTAction::findActionByText(os, "Wrap sequence");
    CHECK_SET_ERR(wrapMode != NULL, "Cannot find Wrap sequence action");
    if (wrapMode->isChecked()) {
        GTWidget::click(os, GTAction::button(os, wrapMode));
    }

    QAction* compStrand = GTAction::findActionByText(os, "Show complementary strand");
    CHECK_SET_ERR(compStrand != NULL, "Cannot find Wrap sequence action");
    if (compStrand->isChecked()) {
        GTWidget::click(os, GTAction::button(os, compStrand));
    }

    //3. Check "Edit sequence" button
    GTUtilsSequenceView::enableEditingMode(os);

    //Expected state : Blinked cursor at the sequence  beggining
    const qint64 pos = GTUtilsSequenceView::getCursor(os);
    CHECK_SET_ERR(pos == 0, QString("Unexpected cursor pos, expected: 0, current %1").arg(pos));

    //4. Print "-AAA" symbols
    GTKeyboardDriver::keyClick(Qt::Key_Space);
    GTGlobals::sleep(100);
    GTKeyboardDriver::keyClick('A');
    GTGlobals::sleep(100);
    GTKeyboardDriver::keyClick('A');
    GTGlobals::sleep(100);
    GTKeyboardDriver::keyClick('A');
    GTGlobals::sleep(100);

    //Expected state: Sequence starts with "-AAA"
    QString string = GTUtilsSequenceView::getRegionAsString(os, U2Region(1, 4));
    CHECK_SET_ERR(string == "-AAA", QString("Unexpected string, expected: -AAA, current: %1").arg(string));


    //5. Put cursor after "AAA" and push Bàckspañå 3 times
    GTUtilsSequenceView::clickOnDetView(os);
    GTUtilsSequenceView::setCursor(os, 1);
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTGlobals::sleep(100);
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTGlobals::sleep(100);
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTGlobals::sleep(100);

    //Expected state : Sequence ends with "-"
    string = GTUtilsSequenceView::getRegionAsString(os, U2Region(1, 4));
    CHECK_SET_ERR(string[0] == '-', QString("Unexpected symbol at the beginning, expected: -, current: %1").arg(string[0]));
    CHECK_SET_ERR(string == "-TTG", QString("Unexpected string, expected: -TTG, current: %1").arg(string));

    //6. Print "---" symbols at the beggining and put cursor before gap
    GTUtilsSequenceView::clickOnDetView(os);
    GTUtilsSequenceView::setCursor(os, 0);
    GTKeyboardDriver::keyClick(Qt::Key_Space);
    GTGlobals::sleep(100);
    GTKeyboardDriver::keyClick(Qt::Key_Space);
    GTGlobals::sleep(100);
    GTKeyboardDriver::keyClick(Qt::Key_Space);
    GTGlobals::sleep(100);
    GTUtilsSequenceView::setCursor(os, 0);

    //7. Push Bàckspañå 3 times
    GTKeyboardDriver::keyClick(Qt::Key_Backspace);
    GTGlobals::sleep(100);
    GTKeyboardDriver::keyClick(Qt::Key_Backspace);
    GTGlobals::sleep(100);
    GTKeyboardDriver::keyClick(Qt::Key_Backspace);
    GTGlobals::sleep(100);

    //Expected state : Nothing happens
    string = GTUtilsSequenceView::getRegionAsString(os, U2Region(1, 4));
    CHECK_SET_ERR(string == "----", QString("Unexpected string, expected: ----, current: %1").arg(string));

    //8. Push Detete 3 times
    GTUtilsSequenceView::setCursor(os, 0);
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTGlobals::sleep(100);
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTGlobals::sleep(100);
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTGlobals::sleep(100);

    //Expected state : 3 gaps are deleted
    string = GTUtilsSequenceView::getRegionAsString(os, U2Region(1, 4));
    CHECK_SET_ERR(string[0] == '-', QString("Unexpected symbol at the beginning, expected: -, current: %1").arg(string[0]));
    CHECK_SET_ERR(string == "-TTG", QString("Unexpected string, expected: -TTG, current: %1").arg(string));
}

GUI_TEST_CLASS_DEFINITION(without_anns_test_0004) {
    //1. Open human_T1.fa
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Uncheck "Wrap sequence" and "Show compliment strans" button
    QAction* wrapMode = GTAction::findActionByText(os, "Wrap sequence");
    CHECK_SET_ERR(wrapMode != NULL, "Cannot find Wrap sequence action");
    if (wrapMode->isChecked()) {
        GTWidget::click(os, GTAction::button(os, wrapMode));
    }

    QAction* compStrand = GTAction::findActionByText(os, "Show complementary strand");
    CHECK_SET_ERR(compStrand != NULL, "Cannot find Wrap sequence action");
    if (compStrand->isChecked()) {
        GTWidget::click(os, GTAction::button(os, compStrand));
    }

    //3. Check "Edit sequence" button
    GTUtilsSequenceView::enableEditingMode(os);

    //Expected state : Blinked cursor at the sequence  beggining
    const qint64 pos = GTUtilsSequenceView::getCursor(os);
    CHECK_SET_ERR(pos == 0, QString("Unexpected cursor pos, expected: 0, current %1").arg(pos));

    //4. Print "A, C, G, T, N, gap" symbols
    GTKeyboardDriver::keyClick('A');
    GTGlobals::sleep(100);
    GTKeyboardDriver::keyClick('C');
    GTGlobals::sleep(100);
    GTKeyboardDriver::keyClick('G');
    GTGlobals::sleep(100);
    GTKeyboardDriver::keyClick('T');
    GTGlobals::sleep(100);
    GTKeyboardDriver::keyClick('N');
    GTGlobals::sleep(100);
    GTKeyboardDriver::keyClick(Qt::Key_Space);
    GTGlobals::sleep(100);

    //Expected state: Sequence starts with "A,C, G,T,N,gap"
    QString string = GTUtilsSequenceView::getRegionAsString(os, U2Region(1, 6));
    CHECK_SET_ERR(string == "ACGTN-", QString("Unexpected string in the begginning of the sequence, expected: ACGTN-, current: %1").arg(string));

    //5. Select these 6 symbols and do ÑTRL+C
    GTUtilsSequenceView::selectSequenceRegion(os, 1, 6);
    GTKeyboardUtils::copy(os);

    //6. Put cursor in "199 939" position and do ÑTRL + V
    GTUtilsSequenceView::setCursor(os, 199939);
    GTKeyboardUtils::paste(os);
    GTGlobals::sleep();

    //Expected state : Sequence ends with "A,C, G,T,N,gap"
    string = GTUtilsSequenceView::getRegionAsString(os, U2Region(199940, 6));
    CHECK_SET_ERR(string == "ACGTN-", QString("Unexpected string in the ending of the sequence, expected: ACGTN-, current: %1").arg(string));
}

GUI_TEST_CLASS_DEFINITION(with_anns_test_0001) {
    //1. Open murine.gb
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);


    //2. Open "Edit->Annotation settings on sequence edditing" dialog.
    //   Be sure that "Expand or crop affected annotations" option is selected.
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Edit" << "Annotations settings on sequence editing..."));
    GTUtilsDialog::waitForDialog(os, new EditSettingsDialogFiller(os, EditSettingsDialogFiller::ExpandOrCropAffectedAnnotation, false));
    GTWidget::click(os, GTUtilsSequenceView::getDetViewByNumber(os), Qt::RightButton);

    //3. Push "Edit sequence" button, sequence in the edit mode
    GTUtilsSequenceView::enableEditingMode(os);

    //4. Select CDS with join (2970..3413,3412..3873) and do double click on it
    GTUtilsSequenceView::clickAnnotationPan(os, "CDS", 2970, 0, true);
    const QList<U2Region> regionsBeforeInsert = GTUtilsAnnotationsTreeView::getSelectedAnnotatedRegions(os);
    CHECK_SET_ERR(regionsBeforeInsert.size() == 2, QString("Unexpected annotation selection before insert, expected: 2, current %1")
                  .arg(regionsBeforeInsert.size()));

    //5. Vertical scroll sequence until position 3874
    GTUtilsSequenceView::goToPosition(os, 3874);

    //6. Do mouse click in position before last "A" symbol in sequence (in position 3873)
    GTUtilsSequenceView::setCursor(os, 3872);
    GTGlobals::sleep(1000);
    const U2Region visibleAreaBeforeInsert = GTUtilsSequenceView::getVisibleRange(os);
    GTKeyboardDriver::keyClick('A');
    GTGlobals::sleep(100);

    //Expected state : Visible area was not changed
    const U2Region visibleAreaAfterInsert = GTUtilsSequenceView::getVisibleRange(os);
    CHECK_SET_ERR(visibleAreaBeforeInsert == visibleAreaAfterInsert,
                  QString("Visible area was changed, area before insert - start: %1, length: %2, after insert - start: %3, length: %4")
                  .arg(visibleAreaBeforeInsert.startPos).arg(visibleAreaBeforeInsert.length).arg(visibleAreaAfterInsert.startPos)
                  .arg(visibleAreaAfterInsert.length));
    //Expected state : Annotation was expanded
    GTUtilsSequenceView::clickAnnotationPan(os, "CDS", 2970, 0, true);
    const QList<U2Region> regionsAfterInsert = GTUtilsAnnotationsTreeView::getSelectedAnnotatedRegions(os);
    CHECK_SET_ERR(regionsAfterInsert.size() == 2, QString("Unexpected annotation selection after insert, expected: 2, current %1")
                  .arg(regionsAfterInsert.size()));
    CHECK_SET_ERR(regionsBeforeInsert.first() == regionsAfterInsert.first(),
        QString("Unexpected changing os the first part of join annotation, before insert - start: %1, length: %2, after insert - start: %3, length %4")
        .arg(regionsBeforeInsert.first().startPos).arg(regionsBeforeInsert.first().length).arg(regionsAfterInsert.first().startPos)
        .arg(regionsAfterInsert.first().length));
    CHECK_SET_ERR(regionsBeforeInsert.last().startPos == regionsAfterInsert.last().startPos,
        QString("Start pos of the second part of join annotation unexpectedly was changed, before insert: %1, after insert: %2")
        .arg(regionsBeforeInsert.last().startPos).arg(regionsAfterInsert.last().startPos));
    CHECK_SET_ERR(regionsBeforeInsert.last().length + 1 == regionsAfterInsert.last().length,
        QString("Length of the second part of join annotation should have been expanded for symbol, before insert: %1, after insert: %2")
        .arg(regionsBeforeInsert.last().length).arg(regionsAfterInsert.last().length));
    }

GUI_TEST_CLASS_DEFINITION(with_anns_test_0002) {
    //1. Open murine.gb
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Open "Edit->Annotation settings on sequence edditing" dialog.
    //   Be sure that "Expand or crop affected annotations" option is selected.
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Edit" << "Annotations settings on sequence editing..."));
    GTUtilsDialog::waitForDialog(os, new EditSettingsDialogFiller(os, EditSettingsDialogFiller::ExpandOrCropAffectedAnnotation, false));
    GTWidget::click(os, GTUtilsSequenceView::getDetViewByNumber(os), Qt::RightButton);

    //3. Push "Edit sequence" button, sequence in the edit mode
    GTUtilsSequenceView::enableEditingMode(os);

    //4. Select CDS (1042, 2674)  and do double click on it
    GTUtilsSequenceView::clickAnnotationPan(os, "CDS", 1042, 0, true);

    //5. Put cursor in position before "A" in position 1043
    GTUtilsSequenceView::setCursor(os, 1042);

    //6. Type "ACGTN-"
    GTKeyboardDriver::keyClick('A');
    GTGlobals::sleep(100);
    GTKeyboardDriver::keyClick('C');
    GTGlobals::sleep(100);
    GTKeyboardDriver::keyClick('G');
    GTGlobals::sleep(100);
    GTKeyboardDriver::keyClick('T');
    GTGlobals::sleep(100);
    GTKeyboardDriver::keyClick('N');
    GTGlobals::sleep(100);
    GTKeyboardDriver::keyClick(Qt::Key_Space);
    GTGlobals::sleep(100);

    //Expected state : annotation starts with "AACGTN-"
    GTUtilsSequenceView::clickAnnotationPan(os, "CDS", 1042, 0, true);
    const QList<U2Region> selectedAnnotationRegions = GTUtilsAnnotationsTreeView::getSelectedAnnotatedRegions(os);
    CHECK_SET_ERR(selectedAnnotationRegions.size() == 1, QString("Unexpected annotation size, expected: 1, current: %1")
        .arg(selectedAnnotationRegions.size()));
    QString string = GTUtilsSequenceView::getRegionAsString(os, U2Region(selectedAnnotationRegions.first().startPos + 1, 7));
    CHECK_SET_ERR(string == "AACGTN-",
        QString("Unexpected string at the beginning of the first annotation, expected: AACGTN-, current: %1").arg(string));

    //7. Put cursor in position before "A" in position 1043
    GTUtilsSequenceView::setCursor(os, 1043);

    //8. Type some forbiden symbols "QWER"
    GTUtilsSequenceView::setCursor(os, 1042);
    GTKeyboardDriver::keyClick('Q');
    GTGlobals::sleep(100);
    GTKeyboardDriver::keyClick('W');
    GTGlobals::sleep(100);
    GTKeyboardDriver::keyClick('E');
    GTGlobals::sleep(100);
    GTKeyboardDriver::keyClick('R');
    GTGlobals::sleep(100);

    //Expected state: annotation starts with "AACGTN-"
    string = GTUtilsSequenceView::getRegionAsString(os, U2Region(selectedAnnotationRegions.first().startPos + 1, 7));
    CHECK_SET_ERR(string == "AACGTN-",
        QString("Unexpected string at the end of the first annotation, expected: AACGTN-, current: %1").arg(string));
}

GUI_TEST_CLASS_DEFINITION(with_anns_test_0003) {
    //1. Open murine.gb
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Open "Edit->Annotation settings on sequence edditing" dialog.
    //   Be sure that "Remove affected annotation" option is selected.
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Edit" << "Annotations settings on sequence editing..."));
    GTUtilsDialog::waitForDialog(os, new EditSettingsDialogFiller(os, EditSettingsDialogFiller::RemoveAffectedAnnotation, false));
    GTWidget::click(os, GTUtilsSequenceView::getDetViewByNumber(os), Qt::RightButton);

    //3. Push "Edit sequence" button, sequence in the edit mode
    GTUtilsSequenceView::enableEditingMode(os);

    //4. Select CDS (1042, 2674)  and do double click on it
    GTUtilsSequenceView::clickAnnotationPan(os, "CDS", 1042, 0, true);

    //5. Put cursor in position before "A" in position 1043 (annotation must be selected)
    GTUtilsSequenceView::setCursor(os, 1043, true);

    QList<U2Region> annotationRegions = GTUtilsAnnotationsTreeView::getAnnotatedRegions(os);
    CHECK_SET_ERR(annotationRegions.contains(U2Region(1041, 1617)), QString("Annotation start pos: 1041, length: 1617 was removed"));

    //6. Push "A" sympols
    GTKeyboardDriver::keyClick('A');
    GTGlobals::sleep(1000);

    //Expected state: Annotation CDS (1043, 2674) was removed
    annotationRegions = GTUtilsAnnotationsTreeView::getAnnotatedRegions(os);
    CHECK_SET_ERR(!annotationRegions.contains(U2Region(1041, 1617)), QString("Annotation start pos: 1041, length: 1617 was not removed"));
}

GUI_TEST_CLASS_DEFINITION(with_anns_test_0004) {
    //1. Open murine.gb
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Open "Edit->Annotation settings on sequence edditing" dialog.
    //   Be sure that "Remove affected annotation" option is selected.
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Edit" << "Annotations settings on sequence editing..."));
    GTUtilsDialog::waitForDialog(os, new EditSettingsDialogFiller(os, EditSettingsDialogFiller::RemoveAffectedAnnotation, false));
    GTWidget::click(os, GTUtilsSequenceView::getDetViewByNumber(os), Qt::RightButton);

    //3. Push "Edit sequence" button, sequence in the edit mode
    GTUtilsSequenceView::enableEditingMode(os);

    //4. Select CDS (1042, 2674)  and do double click on it
    GTUtilsSequenceView::clickAnnotationPan(os, "CDS", 1042, 0, true);

    //5. Select 5 symbols at the annotation begin: "ATGGG"
    GTUtilsSequenceView::selectSequenceRegion(os, 1042, 1047);

    QList<U2Region> annotationRegions = GTUtilsAnnotationsTreeView::getAnnotatedRegions(os);
    CHECK_SET_ERR(annotationRegions.contains(U2Region(1041, 1617)), QString("Annotation start pos: 1041, length: 1617 was removed"));

    //6. Push "gap" sympol
    GTUtilsSequenceView::setCursor(os, 1047);
    GTKeyboardDriver::keyClick(Qt::Key_Space);
    GTGlobals::sleep(1000);

    //Expected state : Annotation CDS(1043, 2674) was removed
    annotationRegions = GTUtilsAnnotationsTreeView::getAnnotatedRegions(os);
    CHECK_SET_ERR(!annotationRegions.contains(U2Region(1041, 1617)), QString("Annotation start pos: 1041, length: 1617 was not removed"));

    //Symbol gap "-" in position 1042, "-CAGA" is placed in segment 1042 : 1045
    const QString string =  GTUtilsSequenceView::getRegionAsString(os, U2Region(1048, 5));
    CHECK_SET_ERR(string.size() == 5,
        QString("Unexpected size of the selection, exprcted: 5, current: %1").arg(string.size()));
    CHECK_SET_ERR(string == "-CAGA",
        QString("Unexpected string, expected: -CAGA, current: %1").arg(string));
}

GUI_TEST_CLASS_DEFINITION(with_anns_test_0005) {
    //1. Open murine.gb
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Open "Edit->Annotation settings on sequence edditing" dialog.
    //   Be sure that "Split (join annitations parts)" option is selected.
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Edit" << "Annotations settings on sequence editing..."));
    GTUtilsDialog::waitForDialog(os, new EditSettingsDialogFiller(os, EditSettingsDialogFiller::SplitJoinAnnotationParts, false));
    GTWidget::click(os, GTUtilsSequenceView::getDetViewByNumber(os), Qt::RightButton);

    //3. Push "Edit sequence" button, sequence in the edit mode
    GTUtilsSequenceView::enableEditingMode(os);

    //4. Select CDS (1042, 2674)  and do double click on it
    GTUtilsSequenceView::clickAnnotationPan(os, "CDS", 1042, 0, true);

    //5. Select 1200 position
    GTUtilsSequenceView::setCursor(os, 1199);

    //6. Push "gap" sympol
    GTKeyboardDriver::keyClick(Qt::Key_Space);
    GTGlobals::sleep(1000);

    //Expected state : Annotation CDS(1042..1199, 1201..2661 appeared
    GTUtilsSequenceView::clickAnnotationPan(os, "CDS", 1042, 0, true);
    const QList<U2Region> annotatedRegions = GTUtilsAnnotationsTreeView::getSelectedAnnotatedRegions(os);
    CHECK_SET_ERR(annotatedRegions.size() == 2, QString("Unexpected annotation size, expected: 2, current: %1").arg(annotatedRegions.size()));
    CHECK_SET_ERR(annotatedRegions.first() == U2Region(1041, 158),
        QString("Unexpected first annotated region, expected - start: 1041, length: 158, current: start: %1, length: %2")
        .arg(annotatedRegions.first().startPos).arg(annotatedRegions.first().length));
    CHECK_SET_ERR(annotatedRegions.last() == U2Region(1200, 1459),
        QString("Unexpected first annotated region, expected - start: 1200, length: 1459, current: start: %1, length: %2")
        .arg(annotatedRegions.last().startPos).arg(annotatedRegions.last().length));

    //Symbol gap "-" in position 1200, "GA-CG" is placed in segment 1198:1202
    const QString string = GTUtilsSequenceView::getRegionAsString(os, U2Region(1198, 5));
    CHECK_SET_ERR(string == "GA-CG", QString("Unexpected string, expected: GA-CG, current %1").arg(string));
}

GUI_TEST_CLASS_DEFINITION(with_anns_test_0006) {
    //1. Open murine.gb
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Open "Edit->Annotation settings on sequence edditing" dialog.
    //   Be sure that "Split (join annitations parts)" option is selected.
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Edit" << "Annotations settings on sequence editing..."));
    GTUtilsDialog::waitForDialog(os, new EditSettingsDialogFiller(os, EditSettingsDialogFiller::SplitSeparateAnnotationParts, false));
    GTWidget::click(os, GTUtilsSequenceView::getDetViewByNumber(os), Qt::RightButton);

    //3. Push "Edit sequence" button, sequence in the edit mode
    GTUtilsSequenceView::enableEditingMode(os);

    //4. Select CDS (1042, 2674)  and do double click on it
    GTUtilsSequenceView::clickAnnotationPan(os, "CDS", 1042, 0, true);

    //5. Select 1200 position
    GTUtilsSequenceView::setCursor(os, 1199);

    //6. Push "gap" sympol
    GTKeyboardDriver::keyClick(Qt::Key_Space);
    GTGlobals::sleep(1000);

    //Expected state: Two annotation CDS (1042..1199, 1201..2661) appeared
    GTUtilsSequenceView::clickAnnotationPan(os, "CDS", 1042, 0, true);
    QList<U2Region> annotatedRegions = GTUtilsAnnotationsTreeView::getSelectedAnnotatedRegions(os);
    CHECK_SET_ERR(annotatedRegions.size() == 1, QString("Unexpected annotation size, expected: 1, current: %1").arg(annotatedRegions.size()));
    CHECK_SET_ERR(annotatedRegions.first() == U2Region(1041, 158),
        QString("Unexpected first annotated region, expected - start: 1041, length: 158, current: start: %1, length: %2")
        .arg(annotatedRegions.first().startPos).arg(annotatedRegions.first().length));

    GTUtilsSequenceView::clickAnnotationPan(os, "CDS", 1201, 0, true);
    annotatedRegions = GTUtilsAnnotationsTreeView::getSelectedAnnotatedRegions(os);
    CHECK_SET_ERR(annotatedRegions.size() == 1, QString("Unexpected annotation size, expected: 1, current: %1").arg(annotatedRegions.size()));
    CHECK_SET_ERR(annotatedRegions.first() == U2Region(1200, 1459),
        QString("Unexpected first annotated region, expected - start: 1200, length: 1459, current: start: %1, length: %2")
        .arg(annotatedRegions.first().startPos).arg(annotatedRegions.first().length));

    //Symbol gap "-" in position 1200, "GA-CG" is placed in segment 1198:1202
    const QString string = GTUtilsSequenceView::getRegionAsString(os, U2Region(1198, 5));
    CHECK_SET_ERR(string == "GA-CG", QString("Unexpected string, expected: GA-CG, current %1").arg(string));
}

GUI_TEST_CLASS_DEFINITION(with_anns_test_0007) {
    //1. Open murine.gb
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Open "Edit->Annotation settings on sequence edditing" dialog.
    //   Be sure that "Recalculate values of qualifiers" option is unchecked.
    //   Be sure that "Expand or crop affected annotations" option is unselected.
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Edit" << "Annotations settings on sequence editing..."));
    GTUtilsDialog::waitForDialog(os, new EditSettingsDialogFiller(os, EditSettingsDialogFiller::ExpandOrCropAffectedAnnotation, false));
    GTWidget::click(os, GTUtilsSequenceView::getDetViewByNumber(os), Qt::RightButton);

    //3. Select CDS (1042, 2674)  and do double click on it
    GTUtilsAnnotationsTreeView::clickItem(os, "CDS", 1, true);

    //4. Select Add->Qualifier from context menu
    //5. In "Add new qualifier" dialog add Òame "Test" and Value : "1500..2000"  and save
    GTUtilsAnnotationsTreeView::createQualifier(os, "Test", "1500..2000", "CDS");

    //   Be sure thar new qualifier "Test" appears
    GTUtilsAnnotationsTreeView::clickItem(os, "CDS", 1, true);
    QList<QTreeWidgetItem*> selectedItem = GTUtilsAnnotationsTreeView::getAllSelectedItems(os);
    CHECK_SET_ERR(selectedItem.size() == 1, QString("Unexpected selected items, expected: 1 item, current: %1 item").arg(selectedItem.size()));

    QString qualValue = GTUtilsAnnotationsTreeView::getQualifierValue(os, "Test", selectedItem.first());
    CHECK_SET_ERR(qualValue == "1500..2000", QString("Unexpected qualifire value, expected: 1500..2000, current: %1").arg(qualValue));

    //6. Push "Edit sequence" button, sequence in the edit mode
    GTUtilsSequenceView::enableEditingMode(os);

    //7. Select 1505 position
    GTUtilsSequenceView::setCursor(os, 1504);

    //8. Add 3 gaps
    GTKeyboardDriver::keyClick(Qt::Key_Space);
    GTGlobals::sleep(100);
    GTKeyboardDriver::keyClick(Qt::Key_Space);
    GTGlobals::sleep(100);
    GTKeyboardDriver::keyClick(Qt::Key_Space);
    GTGlobals::sleep(100);

    //Symbol gap "-" in position 1505 - 1507
    const QString string = GTUtilsSequenceView::getRegionAsString(os, U2Region(1505, 3));
    CHECK_SET_ERR(string == "---", QString("Unexpected selection, expected: ---, current: %1").arg(string));

    //Expected state : Values for qualifier "Test" is not changed, "1500..2000"
    GTUtilsAnnotationsTreeView::clickItem(os, "CDS", 1, true);
    selectedItem = GTUtilsAnnotationsTreeView::getAllSelectedItems(os);
    CHECK_SET_ERR(selectedItem.size() == 1, QString("Unexpected selected items, expected: 1 item, current: %1 item").arg(selectedItem.size()));

    qualValue = GTUtilsAnnotationsTreeView::getQualifierValue(os, "Test", selectedItem.first());
    CHECK_SET_ERR(qualValue == "1500..2000", QString("Unexpected qualifire value, expected: 1500..2000, current: %1").arg(qualValue));
}

GUI_TEST_CLASS_DEFINITION(with_anns_test_0008) {
    //1. Open murine.gb
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Open "Edit->Annotation settings on sequence edditing" dialog.
    //   Be sure that "Recalculate values of qualifiers" option is unchecked.
    //   Be sure that "Expand or crop affected annotations" option is selected.
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Edit" << "Annotations settings on sequence editing..."));
    GTUtilsDialog::waitForDialog(os, new EditSettingsDialogFiller(os, EditSettingsDialogFiller::ExpandOrCropAffectedAnnotation, true));
    GTWidget::click(os, GTUtilsSequenceView::getDetViewByNumber(os), Qt::RightButton);

    //3. Select CDS (1042, 2674)  and do double click on it
    GTUtilsAnnotationsTreeView::clickItem(os, "CDS", 1, true);

    //4. Select Add->Qualifier from context menu
    //5. In "Add new qualifier" dialog add Òame "Test" and Value : "1500..2000"  and save
    GTUtilsAnnotationsTreeView::createQualifier(os, "Test", "1500..2000", "CDS");

    //   Be sure thar new qualifier "Test" appears
    GTUtilsAnnotationsTreeView::clickItem(os, "CDS", 1, true);
    QList<QTreeWidgetItem*> selectedItem = GTUtilsAnnotationsTreeView::getAllSelectedItems(os);
    CHECK_SET_ERR(selectedItem.size() == 1, QString("Unexpected selected items, expected: 1 item, current: %1 item").arg(selectedItem.size()));

    QString qualValue = GTUtilsAnnotationsTreeView::getQualifierValue(os, "Test", selectedItem.first());
    CHECK_SET_ERR(qualValue == "1500..2000", QString("Unexpected qualifire value, expected: 1500..2000, current: %1").arg(qualValue));

    //6. Push "Edit sequence" button, sequence in the edit mode
    GTUtilsSequenceView::enableEditingMode(os);

    //7. Select 1505 position
    GTUtilsSequenceView::setCursor(os, 1504);

    //8. Add 3 gaps
    GTKeyboardDriver::keyClick(Qt::Key_Space);
    GTGlobals::sleep(100);
    GTKeyboardDriver::keyClick(Qt::Key_Space);
    GTGlobals::sleep(100);
    GTKeyboardDriver::keyClick(Qt::Key_Space);
    GTGlobals::sleep(100);

    //Symbol gap "-" in position 1505 - 1507
    const QString string = GTUtilsSequenceView::getRegionAsString(os, U2Region(1505, 3));
    CHECK_SET_ERR(string == "---", QString("Unexpected selection, expected: ---, current: %1").arg(string));

    //Expected state : Values for qualifier "Test" is changed, "1500..2003"
    GTUtilsAnnotationsTreeView::clickItem(os, "CDS", 1, true);
    selectedItem = GTUtilsAnnotationsTreeView::getAllSelectedItems(os);
    CHECK_SET_ERR(selectedItem.size() == 1, QString("Unexpected selected items, expected: 1 item, current: %1 item").arg(selectedItem.size()));

    qualValue = GTUtilsAnnotationsTreeView::getQualifierValue(os, "Test", selectedItem.first());
    CHECK_SET_ERR(qualValue == "1500..2003", QString("Unexpected qualifire value, expected: 1500..2003, current: %1").arg(qualValue));
}


} // namespace

} // namespace U2
