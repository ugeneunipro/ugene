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

#include <U2Core/U2IdTypes.h>
#include "system/GTClipboard.h"
#include "GTTestsSequenceEdit.h"
#include "GTGlobals.h"
#include <drivers/GTKeyboardDriver.h>
#include "utils/GTKeyboardUtils.h"
#include <drivers/GTMouseDriver.h>
#include "primitives/GTMenu.h"
#include <base_dialogs/GTFileDialog.h>
#include <primitives/GTTreeWidget.h>
#include "GTUtilsProject.h"
#include "GTUtilsDocument.h"
#include "GTUtilsLog.h"
#include "utils/GTUtilsApp.h"
#include "utils/GTUtilsToolTip.h"
#include "GTUtilsMdi.h"
#include "GTUtilsAnnotationsTreeView.h"
#include "GTUtilsNotifications.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsTaskTreeView.h"
#include "GTUtilsSequenceView.h"
#include "GTUtilsMdi.h"
#include "primitives/PopupChooser.h"
#include "runnables/ugene/corelibs/U2Gui/EditSequenceDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/RangeSelectionDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/RemovePartFromSequenceDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ReplaceSubsequenceDialogFiller.h"

#include <U2Core/DocumentModel.h>
#include <U2View/AnnotatedDNAViewFactory.h>
#include <U2View/MaEditorFactory.h>
#include <U2View/ADVConstants.h>

namespace U2{

namespace GUITest_common_scenarios_sequence_edit {
using namespace HI;
GUI_TEST_CLASS_DEFINITION(test_0001) {

    GTUtilsProject::openFiles(os, dataDir + "samples/FASTA/human_T1.fa");
    GTGlobals::sleep();

    GTUtilsDialog::waitForDialog(os, new SelectSequenceRegionDialogFiller(os, 1, 50));
    GTWidget::click(os, GTWidget::findWidget(os, "ADV_single_sequence_widget_0"));
    GTKeyboardUtils::selectAll(os);
    GTGlobals::sleep(1000);

    Runnable *removeDialog = new RemovePartFromSequenceDialogFiller(os,
        RemovePartFromSequenceDialogFiller::Remove,
        true,
        testDir+"_common_data/scenarios/sandbox/result.fa",
        RemovePartFromSequenceDialogFiller::FASTA
    );
    GTUtilsDialog::waitForDialog(os, removeDialog);

    GTMenu::clickMainMenuItem(os, QStringList() << "Actions" << "Edit" << "Remove subsequence...", GTGlobals::UseMouse);
    GTGlobals::sleep(1000);

    GTUtilsSequenceView::openSequenceView(os, "result.fa");

    int sequenceLength = GTUtilsSequenceView::getLengthOfSequence(os);
    CHECK_SET_ERR(sequenceLength == 199900, "Sequence length is " + QString::number(sequenceLength) + ", expected 199900");

    QString sequenceBegin = GTUtilsSequenceView::getBeginOfSequenceAsString(os, 7);
    CHECK_SET_ERR(sequenceBegin == "AGAGAGA", "Sequence starts with <" + sequenceBegin + ">, expected AGAGAGA");
}

GUI_TEST_CLASS_DEFINITION(test_0002) {
// Removing part from sequence
//
// Steps:
//
// 1. Use menu {File->Open}. Open file samples/FASTA/human_T1.fa
    GTUtilsProject::openFiles(os, dataDir + "samples/FASTA/human_T1.fa");

// 2. Click Ctrl+A.
// Expected state: Select range dialog appears
//
// 3. Fill the next field in dialog:
//     {Range:} 1..50
//
    GTUtilsDialog::waitForDialog(os, new SelectSequenceRegionDialogFiller(os, 1, 50));
    GTWidget::click(os, GTWidget::findWidget(os, "ADV_single_sequence_widget_0"));
    GTKeyboardUtils::selectAll(os);
    GTGlobals::sleep(1000);

// 4. Click OK. Right click on sequence area. Use context menu {Edit sequence->Remove selected sequence}.
// Expected state: Remove subsequence dialog appears
//
// 5. Fill the next field in dialog:
//     {Save resulted document to a new file} set checked
//     {Document format} Genbank
//     {Document location} _common_data/scenarios/sandbox/result.gb
// 6. Click Remove Button.
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_EDIT << ACTION_EDIT_REMOVE_SUBSEQUENCE, GTGlobals::UseMouse));
    Runnable *removeDialog = new RemovePartFromSequenceDialogFiller(os,
        RemovePartFromSequenceDialogFiller::Remove,
        true,
        testDir+"_common_data/scenarios/sandbox/result.gb",
        RemovePartFromSequenceDialogFiller::Genbank
    );
    GTUtilsDialog::waitForDialog(os, removeDialog);
    GTWidget::click(os, GTWidget::findWidget(os, "ADV_single_sequence_widget_0"), Qt::RightButton);

    GTUtilsTaskTreeView::waitTaskFinished(os);

// Expected state:
//     document with edited sequence must appear in project view,
//     sequence length in new document must be 199900
//     sequence must starts with "AGAGAGA"
    GTUtilsSequenceView::openSequenceView(os, "result.gb");
    int length = GTUtilsSequenceView::getLengthOfSequence(os);
    CHECK_SET_ERR(length == 199900, "Expected length differs");
    QString seqStart = GTUtilsSequenceView::getBeginOfSequenceAsString(os, 7);
    CHECK_SET_ERR(seqStart == "AGAGAGA", "Expected sequence beginning differs");
}

GUI_TEST_CLASS_DEFINITION(test_0003) {

    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();

    Runnable *filler = new InsertSequenceFiller(os,
        "AAAAAA", InsertSequenceFiller::Resize, 1,
        testDir + "_common_data/scenarios/sandbox/result.fa",
        InsertSequenceFiller::FASTA,
        true,
        false
    );
    GTUtilsDialog::waitForDialog(os, filler);
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions" << "Edit" << "Insert subsequence...", GTGlobals::UseKey);
    GTGlobals::sleep();

    GTUtilsDocument::checkDocument(os, "result.fa");
    GTGlobals::sleep();

    GTUtilsSequenceView::openSequenceView(os, "result.fa");

    int sequenceLength = GTUtilsSequenceView::getLengthOfSequence(os);
    CHECK_SET_ERR(sequenceLength == 199956, "Sequence length is " + QString::number(sequenceLength) + ", expected 199956");

    QString sequenceBegin = GTUtilsSequenceView::getBeginOfSequenceAsString(os, 6);
    CHECK_SET_ERR(sequenceBegin == "AAAAAA", "Sequence starts with <" + sequenceBegin + ">, expected AAAAAA");

}
GUI_TEST_CLASS_DEFINITION(test_0004) {

    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();

    Runnable *filler = new InsertSequenceFiller(os,
        "AAAAAA", InsertSequenceFiller::Resize, 1,
        testDir + "_common_data/scenarios/sandbox/result.gb",
        InsertSequenceFiller::Genbank,
        true,
        false
    );
    GTUtilsDialog::waitForDialog(os, filler);
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions" << "Edit" << "Insert subsequence...", GTGlobals::UseKey);
    GTGlobals::sleep();

    GTUtilsDocument::checkDocument(os, "result.gb");
    GTGlobals::sleep();

    GTUtilsSequenceView::openSequenceView(os, "result.gb");

    int sequenceLength = GTUtilsSequenceView::getLengthOfSequence(os);
    CHECK_SET_ERR(sequenceLength == 199956, "Sequence length is " + QString::number(sequenceLength) + ", expected 199956");

    QString sequenceBegin = GTUtilsSequenceView::getBeginOfSequenceAsString(os, 6);
    CHECK_SET_ERR(sequenceBegin == "AAAAAA", "Sequence starts with <" + sequenceBegin + ">, expected AAAAAA");

}
GUI_TEST_CLASS_DEFINITION(test_0005) {

    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsDialog::waitForDialog(os, new SelectSequenceRegionDialogFiller(os, 1, 50));
    GTKeyboardUtils::selectAll(os);
    GTGlobals::sleep(1000);
    Runnable *removeDialog = new RemovePartFromSequenceDialogFiller(os,
        RemovePartFromSequenceDialogFiller::Remove,
        true,
        testDir+"_common_data/scenarios/sandbox/result.fa",
        RemovePartFromSequenceDialogFiller::FASTA
    );
    GTUtilsDialog::waitForDialog(os, removeDialog);
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions" << "Edit" << "Remove subsequence...", GTGlobals::UseMouse);
    GTGlobals::sleep(1000);
    GTUtilsDocument::checkDocument(os, "result.fa");
    GTGlobals::sleep(1000);
    GTUtilsSequenceView::openSequenceView(os, "result.fa");
    GTGlobals::sleep(1000);

    int sequenceLength = GTUtilsSequenceView::getLengthOfSequence(os);
    CHECK_SET_ERR(sequenceLength == 199900, "Sequence length is " + QString::number(sequenceLength) + ", expected 199900");

    QString sequenceBegin = GTUtilsSequenceView::getBeginOfSequenceAsString(os, 7);
    CHECK_SET_ERR(sequenceBegin == "AGAGAGA", "Sequence starts with <" + sequenceBegin + ">, expected AGAGAGA");


}
GUI_TEST_CLASS_DEFINITION(test_0006) {
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/dp_view/", "NC_014267.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    QWidget *mdiWindow = GTUtilsMdi::activeWindow(os);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_EDIT << ACTION_EDIT_RESERVE_COMPLEMENT_SEQUENCE, GTGlobals::UseKey));
    GTMenu::showContextMenu(os, mdiWindow);
    GTGlobals::sleep(1000);

    QString expectedSequenceBegin = "ATCAGATT";
    QString sequenceBegin = GTUtilsSequenceView::getBeginOfSequenceAsString(os, 8);
    CHECK_SET_ERR(sequenceBegin == expectedSequenceBegin, "unecpected begin. Expected ATCAGATT, actual: " + sequenceBegin);
}

GUI_TEST_CLASS_DEFINITION(test_0007) {
    // 1. Open file "test/_common_data/edit_sequence/test.gb"
    GTUtilsProject::openFiles(os, testDir + "_common_data/edit_sequence/test.gb");

    // 2. Select "Remove subsequence" in the context menu.
    // 3. Insert region "2..2" into the "Region to remove" field.
    GTUtilsDialog::waitForDialog(os, new RemovePartFromSequenceDialogFiller(os, "2..2"));
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions" << "Edit" << "Remove subsequence...", GTGlobals::UseMouse);
    GTGlobals::sleep(1000);

    // Expected result: the sequence is started from "AAT", the sequence length is 29, DUMMY_1 annotation is [2..5].
    QString sequenceBegin = GTUtilsSequenceView::getBeginOfSequenceAsString(os, 3);
    CHECK_SET_ERR(sequenceBegin == "AAT", "Sequence starts with " + sequenceBegin + ", expected AAT");

    int length = GTUtilsSequenceView::getLengthOfSequence(os);
    CHECK_SET_ERR(length == 29, "Sequence length is " + QString::number(length) + ", expected 29");

    bool found = GTUtilsAnnotationsTreeView::findRegion(os, "DUMMY_1", U2Region(2, 5));
    CHECK_SET_ERR(found == true, "There is no {2..5} region in annotation");
}

GUI_TEST_CLASS_DEFINITION(test_0008) {

    GTFileDialog::openFile(os, testDir + "_common_data/edit_sequence/", "test.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTGlobals::sleep();
    QTreeWidgetItem *dummyTest = GTUtilsAnnotationsTreeView::findItem(os, "DUMMY_1");
    CHECK_SET_ERR(dummyTest != NULL, "There is no annotation DUMMY_1");

    GTUtilsDialog::waitForDialog(os, new SelectSequenceRegionDialogFiller(os, 2, 2));
    GTKeyboardUtils::selectAll(os);
    GTGlobals::sleep();


    Runnable *removeDialog = new RemovePartFromSequenceDialogFiller(os,
        RemovePartFromSequenceDialogFiller::Remove,
        false,
        testDir+"_common_data/scenarios/sandbox/result.fa",
        RemovePartFromSequenceDialogFiller::FASTA
        );

    GTUtilsDialog::waitForDialog(os, removeDialog);
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions" << "Edit" << "Remove subsequence...", GTGlobals::UseMouse);
    GTGlobals::sleep();

    int sequenceLength = GTUtilsSequenceView::getLengthOfSequence(os);
    CHECK_SET_ERR(sequenceLength == 29, "Sequence length is " + QString::number(sequenceLength) + ", expected 29");

    QString sequenceBegin = GTUtilsSequenceView::getBeginOfSequenceAsString(os, 3);
    CHECK_SET_ERR(sequenceBegin == "AAT", "Sequence starts with <" + sequenceBegin + ">, expected AAT");

    QTreeWidgetItem *dummy1 = GTUtilsAnnotationsTreeView::findItem(os, "DUMMY_1", GTGlobals::FindOptions(false));
    CHECK_SET_ERR(dummy1 == NULL, "There is annotation DUMMY_1, expected state there is no annotation DUMMY_1");
}

GUI_TEST_CLASS_DEFINITION(test_0009) {
    GTUtilsProject::openFiles(os, testDir + "_common_data/fasta/AMINO.fa");
    GTUtilsDialog::waitForDialog(os, new SelectSequenceRegionDialogFiller(os, 10, 13));

    GTWidget::click(os, GTWidget::findWidget(os, "ADV_single_sequence_widget_0"));
    GTGlobals::sleep();
    GTKeyboardUtils::selectAll(os);
    GTGlobals::sleep(1000);

    GTKeyboardDriver::keyClick( 'c', Qt::ControlModifier);
    GTGlobals::sleep(1000);

    QString sequence = GTClipboard::text(os);
    CHECK_SET_ERR("ACCC" == sequence, "Incorrect sequence is copied");

}

GUI_TEST_CLASS_DEFINITION(test_0010) {
    GTFileDialog::openFile(os, testDir + "_common_data/edit_sequence/", "test.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    QWidget *mdiWindow = GTUtilsMdi::activeWindow(os);

    GTUtilsDialog::waitForDialog(os, new SelectSequenceRegionDialogFiller(os, 1, 11));
    GTKeyboardUtils::selectAll(os);
    GTGlobals::sleep(1000);
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_COPY << ADV_COPY_TRANSLATION_ACTION, GTGlobals::UseKey));
    GTMenu::showContextMenu(os, mdiWindow);
    GTGlobals::sleep(1000);
    QString text = GTClipboard::text(os);

    CHECK_SET_ERR(text == "K*K", "Sequcence part translated to <" + text + ">, expected K*K");
}

GUI_TEST_CLASS_DEFINITION(test_0011) {

    GTFileDialog::openFile(os, testDir + "_common_data/edit_sequence/", "test.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep(1000);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "ADV_MENU_COPY" << "action_copy_annotation_sequence"));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "DUMMY_1"));
    GTGlobals::sleep(1000);
    GTMouseDriver::click(Qt::RightButton);
    GTGlobals::sleep(1000);

    const QString expectedSequence = "AATGA";

    QString realSequence = GTClipboard::text(os);
    CHECK_SET_ERR(expectedSequence == realSequence, "Sequence is not pasted");

}

GUI_TEST_CLASS_DEFINITION(test_0012) {
    GTUtilsProject::openFiles(os, testDir + "_common_data/edit_sequence/test.gb");

    GTUtilsDialog::waitForDialog(os, new RemovePartFromSequenceDialogFiller(os, "2..2"));
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions" << "Edit" << "Remove subsequence...", GTGlobals::UseMouse);
    GTGlobals::sleep(1000);

    QString sequenceBegin = GTUtilsSequenceView::getBeginOfSequenceAsString(os, 3);
    CHECK_SET_ERR(sequenceBegin == "AAT", "Sequence starts with " + sequenceBegin + ", expected AAT");

    int length = GTUtilsSequenceView::getLengthOfSequence(os);
    CHECK_SET_ERR(length == 29, "Sequence length is " + QString::number(length) + ", expected 29");

    bool found = GTUtilsAnnotationsTreeView::findRegion(os, "DUMMY_1", U2Region(2, 5));
    CHECK_SET_ERR(found == true, "There is no {2..5} region in annotation");
}

namespace {

QMap<QString, QString> getReferenceQualifiers() {
    static QMap<QString, QString> qualifiers;
    if (qualifiers.isEmpty()) {
        qualifiers["new_qualifier"] = "adsdas 50..60 asdk 70..80 ljsad";
        qualifiers["new_qualifier1"] = "sdfsdfsdf join(20..30,90..100) dfdfdsf";
        qualifiers["new_qualifier2"] = "asdas order(230..250,270..300) a dsd";
        qualifiers["new_qualifier3"] = "sdfsdfk complement(order(450..470, 490..500)) dfdsf";
        qualifiers["new_qualifier4"] = "sdfsdfk complement(join(370..400,420..440)) dfdsf";
        qualifiers["new_qualifier5"] = "sdfsdfk complement(320..350) df complement(355..365) dsf";
    }
    return qualifiers;
}

QString shiftQualifierRegions(const QString &value, int delta) {
    QString result = value;
    QRegExp digitMatcher("\\d+");
    int lastFoundPos = 0;
    int lastReplacementPos = 0;

    while ((lastFoundPos = digitMatcher.indexIn(value, lastFoundPos)) != -1) {
        const QString number = digitMatcher.cap();
        const QString newNumber = QString::number(number.toLongLong() + delta);
        const int replacementStart = result.indexOf(number, lastReplacementPos);
        result.replace(replacementStart, number.length(), newNumber);
        lastReplacementPos = replacementStart + newNumber.length();
        lastFoundPos += digitMatcher.matchedLength();
    }
    return result;
}

void checkQualifierValue(HI::GUITestOpStatus &os, const QString &qualName, int regionShift) {
    QTreeWidgetItem *qual = GTUtilsAnnotationsTreeView::findItem(os, qualName);
    const QString qualValue = qual->data(2, Qt::DisplayRole).toString();
    const QString expectedVal = shiftQualifierRegions(getReferenceQualifiers()[qualName], regionShift);
    CHECK_SET_ERR(qualValue == expectedVal, QString("Qualifier value has changed unexpectedly. Expected: '%1'. Actual: '%2'")
        .arg(expectedVal).arg(qualValue));
}

void checkQualifierRegionsShift(HI::GUITestOpStatus &os, int shift) {
    foreach(const QString &qualName, getReferenceQualifiers().keys()) {
        checkQualifierValue(os, qualName, shift);
    }
}

void doMagic(HI::GUITestOpStatus &os) {
    QTreeWidgetItem *annotationGroup = GTUtilsAnnotationsTreeView::findItem(os, "Misc. Feature  (0, 2)");
    GTTreeWidget::getItemCenter(os, annotationGroup);
    for (int i = 0; i < annotationGroup->childCount(); ++i) {
        GTTreeWidget::getItemCenter(os, annotationGroup->child(i));
    }
}

}

GUI_TEST_CLASS_DEFINITION(test_0013_1) {
    // Check that qualifiers are recalculated on a removal of a subsequence that is located to the left of a region mentioned in a qualifier

    GTFileDialog::openFile(os, testDir + "_common_data/genbank/qulifier_rebuilding.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    doMagic(os); // for some reason annotation qualifiers are not found without actions done by this function

    GTUtilsDialog::waitForDialog(os, new RemovePartFromSequenceDialogFiller(os, "1..10", false));
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions" << "Edit" << "Remove subsequence...", GTGlobals::UseMouse);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    checkQualifierRegionsShift(os, 0);

    GTUtilsDialog::waitForDialog(os, new RemovePartFromSequenceDialogFiller(os, "1..10", true));
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions" << "Edit" << "Remove subsequence...", GTGlobals::UseMouse);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    checkQualifierRegionsShift(os, -10);
}

GUI_TEST_CLASS_DEFINITION(test_0013_1_neg) {
    // Check that qualifiers aren't recalculated on a removal of a subsequence that is located to the right of a region mentioned in a qualifier

    GTFileDialog::openFile(os, testDir + "_common_data/genbank/qulifier_rebuilding.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    doMagic(os); // for some reason annotation qualifiers are not found without actions done by this function

    GTUtilsDialog::waitForDialog(os, new RemovePartFromSequenceDialogFiller(os, "1000..1100", true));
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions" << "Edit" << "Remove subsequence...", GTGlobals::UseMouse);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    checkQualifierRegionsShift(os, 0);
}

GUI_TEST_CLASS_DEFINITION(test_0013_2) {
    // Check that a translation qualifier is recalculated on a removal of a subsequence that is located inside an annotated region

    GTFileDialog::openFile(os, dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new RemovePartFromSequenceDialogFiller(os, "1040..1042", true));
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions" << "Edit" << "Remove subsequence...", GTGlobals::UseMouse);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QTreeWidgetItem *annotationGroup = GTUtilsAnnotationsTreeView::findItem(os, "CDS  (0, 4)");
    GTTreeWidget::getItemCenter(os, annotationGroup);
    GTTreeWidget::getItemCenter(os, annotationGroup->child(0));
    QTreeWidgetItem *qualItem = annotationGroup->child(0)->child(5);
    CHECK_SET_ERR("translation" == qualItem->text(0), "Unexpected qualifier found");
    CHECK_SET_ERR(qualItem->text(2).startsWith("WARLLPLP*V*P*"), "Unexpected 'translation' qualifier value");
}

GUI_TEST_CLASS_DEFINITION(test_0013_2_neg) {
    // Check that a translation qualifier isn't recalculated on a removal of a subsequence that is located outside an annotated region

    GTFileDialog::openFile(os, dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new RemovePartFromSequenceDialogFiller(os, "996..1000", true));
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions" << "Edit" << "Remove subsequence...", GTGlobals::UseMouse);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QTreeWidgetItem *annotationGroup = GTUtilsAnnotationsTreeView::findItem(os, "CDS  (0, 4)");
    GTTreeWidget::getItemCenter(os, annotationGroup);
    GTTreeWidget::getItemCenter(os, annotationGroup->child(0));
    QTreeWidgetItem *qualItem = annotationGroup->child(0)->child(5);
    CHECK_SET_ERR("translation" == qualItem->text(0), "Unexpected qualifier found");
    CHECK_SET_ERR(qualItem->text(2).startsWith("MGQTVTTPLSLTLDHWKD"), "Unexpected 'translation' qualifier value");
}

GUI_TEST_CLASS_DEFINITION(test_0014_1) {
    // Check that qualifiers are recalculated on an insertion of a subsequence that is located to the left of a region mentioned in a qualifier

    GTFileDialog::openFile(os, testDir + "_common_data/genbank/qulifier_rebuilding.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    doMagic(os); // for some reason annotation qualifiers are not found without actions done by this function

    GTUtilsSequenceView::selectSequenceRegion(os, 1, 1);

    GTUtilsDialog::waitForDialog(os, new InsertSequenceFiller(os, "AAAAAAAAAA", InsertSequenceFiller::Resize, 1, "",
        InsertSequenceFiller::FASTA, false, false, GTGlobals::UseKey, false, false));
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions" << "Edit" << "Insert subsequence...", GTGlobals::UseMouse);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    checkQualifierRegionsShift(os, 0);

    GTUtilsDialog::waitForDialog(os, new InsertSequenceFiller(os, "AAAAAAAAAA", InsertSequenceFiller::Resize, 1, "",
        InsertSequenceFiller::FASTA, false, false, GTGlobals::UseKey, false, true));
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions" << "Edit" << "Insert subsequence...", GTGlobals::UseMouse);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    checkQualifierRegionsShift(os, 10);
}

GUI_TEST_CLASS_DEFINITION(test_0014_1_neg) {
    // Check that qualifiers aren't recalculated on an insertion of a subsequence that is located to the right of a region mentioned in a qualifier

    GTFileDialog::openFile(os, testDir + "_common_data/genbank/qulifier_rebuilding.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    doMagic(os); // for some reason annotation qualifiers are not found without actions done by this function

    GTUtilsSequenceView::selectSequenceRegion(os, 100000, 100000);

    GTUtilsDialog::waitForDialog(os, new InsertSequenceFiller(os, "AAAAAAAAAA", InsertSequenceFiller::Resize, 100000, "",
        InsertSequenceFiller::FASTA, false, false, GTGlobals::UseKey, false, true));
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions" << "Edit" << "Insert subsequence...", GTGlobals::UseMouse);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    checkQualifierRegionsShift(os, 0);
}

GUI_TEST_CLASS_DEFINITION(test_0014_2) {
    // Check that a translation qualifier is recalculated on an insertion of a subsequence that is located inside an annotated region

    GTFileDialog::openFile(os, dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsSequenceView::selectSequenceRegion(os, 1050, 1050);

    GTUtilsDialog::waitForDialog(os, new InsertSequenceFiller(os, "A", InsertSequenceFiller::Resize, 1050, "",
        InsertSequenceFiller::FASTA, false, false, GTGlobals::UseKey, false, true));
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions" << "Edit" << "Insert subsequence...", GTGlobals::UseMouse);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QTreeWidgetItem *annotationGroup = GTUtilsAnnotationsTreeView::findItem(os, "CDS  (0, 4)");
    GTTreeWidget::getItemCenter(os, annotationGroup);
    GTTreeWidget::getItemCenter(os, annotationGroup->child(0));
    QTreeWidgetItem *qualItem = annotationGroup->child(0)->child(5);
    CHECK_SET_ERR("translation" == qualItem->text(0), "Unexpected qualifier found");
    CHECK_SET_ERR(qualItem->text(2).startsWith("MGQDCYHSLKFDLRSLER"), "Unexpected 'translation' qualifier value");
}

GUI_TEST_CLASS_DEFINITION(test_0014_2_neg) {
    // Check that a translation qualifier isn't recalculated on an insertion of a subsequence that is located outside an annotated region

    GTFileDialog::openFile(os, dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsSequenceView::selectSequenceRegion(os, 1, 1);

    GTUtilsDialog::waitForDialog(os, new InsertSequenceFiller(os, "A", InsertSequenceFiller::Resize, 1, "",
        InsertSequenceFiller::FASTA, false, false, GTGlobals::UseKey, false, true));
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions" << "Edit" << "Insert subsequence...", GTGlobals::UseMouse);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QTreeWidgetItem *annotationGroup = GTUtilsAnnotationsTreeView::findItem(os, "CDS  (0, 4)");
    GTTreeWidget::getItemCenter(os, annotationGroup);
    GTTreeWidget::getItemCenter(os, annotationGroup->child(0));
    QTreeWidgetItem *qualItem = annotationGroup->child(0)->child(5);
    CHECK_SET_ERR("translation" == qualItem->text(0), "Unexpected qualifier found");
    CHECK_SET_ERR(qualItem->text(2).startsWith("MGQTVTTPLSLTLDHWKD"), "Unexpected 'translation' qualifier value");
}

GUI_TEST_CLASS_DEFINITION(test_0015_1) {
    // Check that a qualifier is recalculated on a substitution of a subsequence that is located to the left of a region mentioned in the qualifier

    GTFileDialog::openFile(os, testDir + "_common_data/genbank/qulifier_rebuilding.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    doMagic(os); // for some reason annotation qualifiers are not found without actions done by this function

    GTUtilsSequenceView::selectSequenceRegion(os, 1, 10);

    GTUtilsDialog::waitForDialog(os, new ReplaceSubsequenceDialogFiller(os, "AAAAA", false));
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Edit" << "Replace subsequence..."));
    GTWidget::click(os, GTUtilsSequenceView::getSeqWidgetByNumber(os), Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    checkQualifierRegionsShift(os, 0);

    GTUtilsSequenceView::selectSequenceRegion(os, 1, 10);

    GTUtilsDialog::waitForDialog(os, new ReplaceSubsequenceDialogFiller(os, "AAAAA", true));
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Edit" << "Replace subsequence..."));
    GTWidget::click(os, GTUtilsSequenceView::getSeqWidgetByNumber(os), Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    checkQualifierRegionsShift(os, -5);
}

GUI_TEST_CLASS_DEFINITION(test_0015_1_neg) {
    // Check that a qualifier isn't recalculated on a substitution of a subsequence that is located to the right of a region mentioned in the qualifier

    GTFileDialog::openFile(os, testDir + "_common_data/genbank/qulifier_rebuilding.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    doMagic(os); // for some reason annotation qualifiers are not found without actions done by this function

    GTUtilsSequenceView::selectSequenceRegion(os, 1000, 1010);

    GTUtilsDialog::waitForDialog(os, new ReplaceSubsequenceDialogFiller(os, "AAAAA", true));
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Edit" << "Replace subsequence..."));
    GTWidget::click(os, GTUtilsSequenceView::getSeqWidgetByNumber(os), Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    checkQualifierRegionsShift(os, 0);
}

GUI_TEST_CLASS_DEFINITION(test_0015_2) {
    // Check that a translation qualifier is recalculated on a substitution of a subsequence that is located inside an annotated region

    GTFileDialog::openFile(os, dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsSequenceView::selectSequenceRegion(os, 1050, 1050);

    GTUtilsDialog::waitForDialog(os, new ReplaceSubsequenceDialogFiller(os, "AAA", true));
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Edit" << "Replace subsequence..."));
    GTWidget::click(os, GTUtilsSequenceView::getSeqWidgetByNumber(os), Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QTreeWidgetItem *annotationGroup = GTUtilsAnnotationsTreeView::findItem(os, "CDS  (0, 4)");
    GTTreeWidget::getItemCenter(os, annotationGroup);
    GTTreeWidget::getItemCenter(os, annotationGroup->child(0));
    QTreeWidgetItem *qualItem = annotationGroup->child(0)->child(5);
    CHECK_SET_ERR("translation" == qualItem->text(0), "Unexpected qualifier found");
    CHECK_SET_ERR(qualItem->text(2).startsWith("MGQKLLPLP*V*P*ITGKMS"), "Unexpected 'translation' qualifier value");
}

GUI_TEST_CLASS_DEFINITION(test_0015_2_neg) {
    // Check that a translation qualifier isn't recalculated on a substitution of a subsequence that is located outside an annotated region

    GTFileDialog::openFile(os, dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsSequenceView::selectSequenceRegion(os, 996, 1000);

    GTUtilsDialog::waitForDialog(os, new ReplaceSubsequenceDialogFiller(os, "AA", true));
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Edit" << "Replace subsequence...", GTGlobals::UseMouse));
    GTWidget::click(os, GTUtilsSequenceView::getSeqWidgetByNumber(os), Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QTreeWidgetItem *annotationGroup = GTUtilsAnnotationsTreeView::findItem(os, "CDS  (0, 4)");
    GTTreeWidget::getItemCenter(os, annotationGroup);
    GTTreeWidget::getItemCenter(os, annotationGroup->child(0));
    QTreeWidgetItem *qualItem = annotationGroup->child(0)->child(5);
    CHECK_SET_ERR("translation" == qualItem->text(0), "Unexpected qualifier found");
    CHECK_SET_ERR(qualItem->text(2).startsWith("MGQTVTTPLSLTLDHWKD"), "Unexpected 'translation' qualifier value");
}

GUI_TEST_CLASS_DEFINITION(test_0016_1) {
    // Check that a qualifier isn't recalculated on a removal of a subsequence that includes a region mentioned in the qualifier

    GTFileDialog::openFile(os, testDir + "_common_data/genbank/qulifier_rebuilding.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    doMagic(os); // for some reason annotation qualifiers are not found without actions done by this function

    GTUtilsDialog::waitForDialog(os, new RemovePartFromSequenceDialogFiller(os, "1..600", true));
    GTUtilsNotifications::waitForNotification(os, false);
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions" << "Edit" << "Remove subsequence...", GTGlobals::UseMouse);

    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsMdi::activateWindow(os, "qulifier_rebuilding [s] human_T1");

    checkQualifierRegionsShift(os, 0);
}

GUI_TEST_CLASS_DEFINITION(test_0016_2) {
    // Check that a qualifier isn't recalculated on a complete replacement of a subsequence that includes a region mentioned in the qualifier

    GTFileDialog::openFile(os, testDir + "_common_data/genbank/qulifier_rebuilding.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    doMagic(os); // for some reason annotation qualifiers are not found without actions done by this function

    GTUtilsSequenceView::selectSequenceRegion(os, 1, 600);

    GTUtilsDialog::waitForDialog(os, new ReplaceSubsequenceDialogFiller(os, "AAAAA", true));
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Edit" << "Replace subsequence..."));
    GTUtilsNotifications::waitForNotification(os, false);
    GTWidget::click(os, GTUtilsSequenceView::getSeqWidgetByNumber(os), Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsMdi::activateWindow(os, "qulifier_rebuilding [s] human_T1");

    checkQualifierRegionsShift(os, 0);
}

} // namespace

} // namespace U2
