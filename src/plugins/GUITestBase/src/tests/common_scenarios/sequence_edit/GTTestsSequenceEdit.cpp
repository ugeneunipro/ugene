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

#include "GTTestsSequenceEdit.h"
#include <base_dialogs/GTFileDialog.h>
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTTreeWidget.h>

#include <QApplication>
#include <QClipboard>
#include <QTreeWidgetItem>

#include <U2Core/DocumentModel.h>
#include <U2Core/U2IdTypes.h>

#include <U2View/ADVConstants.h>
#include <U2View/AnnotatedDNAViewFactory.h>

#include "GTGlobals.h"
#include "GTUtilsAnnotationsTreeView.h"
#include "GTUtilsDocument.h"
#include "GTUtilsMdi.h"
#include "GTUtilsNotifications.h"
#include "GTUtilsProject.h"
#include "GTUtilsSequenceView.h"
#include "GTUtilsTaskTreeView.h"
#include "primitives/GTMenu.h"
#include "primitives/PopupChooser.h"
#include "runnables/ugene/corelibs/U2Gui/EditSequenceDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/RangeSelectionDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/RemovePartFromSequenceDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ReplaceSubsequenceDialogFiller.h"
#include "system/GTClipboard.h"
#include "utils/GTKeyboardUtils.h"

namespace U2 {

namespace GUITest_common_scenarios_sequence_edit {
using namespace HI;
GUI_TEST_CLASS_DEFINITION(test_0001) {
    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    GTUtilsDialog::add(new SelectSequenceRegionDialogFiller(1, 50));
    GTWidget::click(GTWidget::findWidget("ADV_single_sequence_widget_0"));
    GTKeyboardUtils::selectAll();

    GTUtilsDialog::add(
        new RemovePartFromSequenceDialogFiller(
            RemovePartFromSequenceDialogFiller::Remove,
            true,
            testDir + "_common_data/scenarios/sandbox/result.fa",
            RemovePartFromSequenceDialogFiller::FASTA));
    GTMenu::clickMainMenuItem({"Actions", "Edit", "Remove subsequence..."}, GTGlobals::UseMouse);

    GTUtilsSequenceView::openSequenceView("result.fa");

    int sequenceLength = GTUtilsSequenceView::getLengthOfSequence();
    CHECK_SET_ERR(sequenceLength == 199900, "Sequence length is " + QString::number(sequenceLength) + ", expected 199900");

    QString sequenceBegin = GTUtilsSequenceView::getBeginOfSequenceAsString(7);
    CHECK_SET_ERR(sequenceBegin == "AGAGAGA", "Sequence starts with <" + sequenceBegin + ">, expected AGAGAGA");
}

GUI_TEST_CLASS_DEFINITION(test_0002) {
    // Removing part from sequence
    //
    // Steps:
    //
    // 1. Use menu {File->Open}. Open file samples/FASTA/human_T1.fa
    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();
    // 2. Click Ctrl+A.
    // Expected state: Select range dialog appears
    //
    // 3. Fill the next field in dialog:
    //     {Range:} 1..50
    //
    GTUtilsDialog::waitForDialog(new SelectSequenceRegionDialogFiller(1, 50));
    GTUtilsSequenceView::clickMouseOnTheSafeSequenceViewArea();
    GTKeyboardUtils::selectAll();

    // 4. Click OK. Right click on sequence area. Use context menu {Edit sequence->Remove selected sequence}.
    // Expected state: Remove subsequence dialog appears
    //
    // 5. Fill the next field in dialog:
    //     {Save resulted document to a new file} set checked
    //     {Document format} Genbank
    //     {Document location} _common_data/scenarios/sandbox/result.gb
    // 6. Click Remove Button.
    GTUtilsDialog::add(new PopupChooser({ADV_MENU_EDIT, ACTION_EDIT_REMOVE_SUBSEQUENCE}, GTGlobals::UseMouse));
    GTUtilsDialog::add(
        new RemovePartFromSequenceDialogFiller(
            RemovePartFromSequenceDialogFiller::Remove,
            true,
            testDir + "_common_data/scenarios/sandbox/result.gb",
            RemovePartFromSequenceDialogFiller::Genbank));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();

    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state:
    //     document with edited sequence must appear in project view,
    //     sequence length in new document must be 199900
    //     sequence must starts with "AGAGAGA"
    GTUtilsSequenceView::openSequenceView("result.gb");

    int length = GTUtilsSequenceView::getLengthOfSequence();
    CHECK_SET_ERR(length == 199900, "Expected length differs");
    QString seqStart = GTUtilsSequenceView::getBeginOfSequenceAsString(7);
    CHECK_SET_ERR(seqStart == "AGAGAGA", "Expected sequence beginning differs");
}

GUI_TEST_CLASS_DEFINITION(test_0003) {
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    GTUtilsDialog::waitForDialog(
        new InsertSequenceFiller(
            "AAAAAA",
            InsertSequenceFiller::Resize,
            1,
            testDir + "_common_data/scenarios/sandbox/result.fa",
            InsertSequenceFiller::FASTA,
            true,
            false));
    GTMenu::clickMainMenuItem({"Actions", "Edit", "Insert subsequence..."}, GTGlobals::UseKey);
    GTUtilsDocument::checkDocument("result.fa");

    GTUtilsSequenceView::openSequenceView("result.fa");

    int sequenceLength = GTUtilsSequenceView::getLengthOfSequence();
    CHECK_SET_ERR(sequenceLength == 199956, "Sequence length is " + QString::number(sequenceLength) + ", expected 199956");

    QString sequenceBegin = GTUtilsSequenceView::getBeginOfSequenceAsString(6);
    CHECK_SET_ERR(sequenceBegin == "AAAAAA", "Sequence starts with <" + sequenceBegin + ">, expected AAAAAA");
}
GUI_TEST_CLASS_DEFINITION(test_0004) {
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    GTUtilsDialog::waitForDialog(
        new InsertSequenceFiller(
            "AAAAAA",
            InsertSequenceFiller::Resize,
            1,
            testDir + "_common_data/scenarios/sandbox/result.gb",
            InsertSequenceFiller::Genbank,
            true,
            false));
    GTMenu::clickMainMenuItem({"Actions", "Edit", "Insert subsequence..."}, GTGlobals::UseKey);

    GTUtilsDocument::checkDocument("result.gb");

    GTUtilsSequenceView::openSequenceView("result.gb");

    int sequenceLength = GTUtilsSequenceView::getLengthOfSequence();
    CHECK_SET_ERR(sequenceLength == 199956, "Sequence length is " + QString::number(sequenceLength) + ", expected 199956");

    QString sequenceBegin = GTUtilsSequenceView::getBeginOfSequenceAsString(6);
    CHECK_SET_ERR(sequenceBegin == "AAAAAA", "Sequence starts with <" + sequenceBegin + ">, expected AAAAAA");
}
GUI_TEST_CLASS_DEFINITION(test_0005) {
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    GTUtilsDialog::add(new SelectSequenceRegionDialogFiller(1, 50));
    GTKeyboardUtils::selectAll();
    GTUtilsDialog::add(
        new RemovePartFromSequenceDialogFiller(
            RemovePartFromSequenceDialogFiller::Remove,
            true,
            testDir + "_common_data/scenarios/sandbox/result.fa",
            RemovePartFromSequenceDialogFiller::FASTA));
    GTMenu::clickMainMenuItem({"Actions", "Edit", "Remove subsequence..."}, GTGlobals::UseMouse);
    GTUtilsDocument::checkDocument("result.fa");
    GTUtilsSequenceView::openSequenceView("result.fa");

    int sequenceLength = GTUtilsSequenceView::getLengthOfSequence();
    CHECK_SET_ERR(sequenceLength == 199900, "Sequence length is " + QString::number(sequenceLength) + ", expected 199900");

    QString sequenceBegin = GTUtilsSequenceView::getBeginOfSequenceAsString(7);
    CHECK_SET_ERR(sequenceBegin == "AGAGAGA", "Sequence starts with <" + sequenceBegin + ">, expected AGAGAGA");
}

GUI_TEST_CLASS_DEFINITION(test_0006) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/dp_view/", "NC_014267.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    GTUtilsDialog::waitForDialog(new PopupChooser({ADV_MENU_EDIT, ADV_MENU_REPLACE_WHOLE_SEQUENCE, ACTION_EDIT_RESERVE_COMPLEMENT_SEQUENCE}, GTGlobals::UseKey));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();

    QString expectedSequenceBegin = "ATCAGATT";
    QString sequenceBegin = GTUtilsSequenceView::getBeginOfSequenceAsString(8);
    CHECK_SET_ERR(sequenceBegin == expectedSequenceBegin, "unexpected begin. Expected ATCAGATT, actual: " + sequenceBegin);
}

GUI_TEST_CLASS_DEFINITION(test_0007) {
    // 1. Open file "test/_common_data/edit_sequence/test.gb"
    GTFileDialog::openFile(testDir + "_common_data/edit_sequence/test.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    // 2. Select "Remove subsequence" in the context menu.
    // 3. Insert region "2..2" into the "Region to remove" field.
    GTUtilsDialog::waitForDialog(new RemovePartFromSequenceDialogFiller("2..2"));
    GTMenu::clickMainMenuItem({"Actions", "Edit", "Remove subsequence..."}, GTGlobals::UseMouse);

    // Expected result: the sequence is started from "AAT", the sequence length is 29, DUMMY_1 annotation is [2..5].
    QString sequenceBegin = GTUtilsSequenceView::getBeginOfSequenceAsString(3);
    CHECK_SET_ERR(sequenceBegin == "AAT", "Sequence starts with " + sequenceBegin + ", expected AAT");

    int length = GTUtilsSequenceView::getLengthOfSequence();
    CHECK_SET_ERR(length == 29, "Sequence length is " + QString::number(length) + ", expected 29");

    bool found = GTUtilsAnnotationsTreeView::findRegion("DUMMY_1", U2Region(2, 5));
    CHECK_SET_ERR(found == true, "There is no {2..5} region in annotation");
}

GUI_TEST_CLASS_DEFINITION(test_0008) {
    GTFileDialog::openFile(testDir + "_common_data/edit_sequence/", "test.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    GTUtilsAnnotationsTreeView::findItem("DUMMY_1");

    GTUtilsDialog::add(new SelectSequenceRegionDialogFiller(2, 2));
    GTKeyboardUtils::selectAll();

    GTUtilsDialog::add(
        new RemovePartFromSequenceDialogFiller(
            RemovePartFromSequenceDialogFiller::Remove,
            false,
            testDir + "_common_data/scenarios/sandbox/result.fa",
            RemovePartFromSequenceDialogFiller::FASTA));
    GTMenu::clickMainMenuItem({"Actions", "Edit", "Remove subsequence..."}, GTGlobals::UseMouse);

    int sequenceLength = GTUtilsSequenceView::getLengthOfSequence();
    CHECK_SET_ERR(sequenceLength == 29, "Sequence length is " + QString::number(sequenceLength) + ", expected 29");

    QString sequenceBegin = GTUtilsSequenceView::getBeginOfSequenceAsString(3);
    CHECK_SET_ERR(sequenceBegin == "AAT", "Sequence starts with <" + sequenceBegin + ">, expected AAT");

    QTreeWidgetItem* dummy1 = GTUtilsAnnotationsTreeView::findItem("DUMMY_1", nullptr, {false});
    CHECK_SET_ERR(dummy1 == nullptr, "There is annotation DUMMY_1, expected state there is no annotation DUMMY_1");
}

GUI_TEST_CLASS_DEFINITION(test_0009) {
    GTFileDialog::openFile(testDir + "_common_data/fasta/AMINO.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    GTUtilsDialog::waitForDialog(new SelectSequenceRegionDialogFiller(10, 13));
    GTWidget::click(GTWidget::findWidget("ADV_single_sequence_widget_0"));
    GTKeyboardUtils::selectAll();

    GTKeyboardUtils::copy();

    QString sequence = GTClipboard::text();
    CHECK_SET_ERR(sequence == "ACCC", "Incorrect sequence is copied");
}

GUI_TEST_CLASS_DEFINITION(test_0010) {
    GTFileDialog::openFile(testDir + "_common_data/edit_sequence/", "test.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    QWidget* mdiWindow = GTUtilsMdi::activeWindow();
    GTUtilsDialog::add(new SelectSequenceRegionDialogFiller(1, 11));
    GTKeyboardUtils::selectAll();
    GTUtilsDialog::add(new PopupChooser({ADV_MENU_COPY, ADV_COPY_TRANSLATION_ACTION}, GTGlobals::UseKey));
    GTMenu::showContextMenu(mdiWindow);
    QString text = GTClipboard::text();

    CHECK_SET_ERR(text == "K*K", "Sequcence part translated to <" + text + ">, expected K*K");
}

GUI_TEST_CLASS_DEFINITION(test_0011) {
    GTFileDialog::openFile(testDir + "_common_data/edit_sequence/", "test.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new PopupChooser({"ADV_MENU_COPY", "action_copy_annotation_sequence"}));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter("DUMMY_1"));
    GTMouseDriver::click(Qt::RightButton);

    QString expectedSequence = "AATGA";

    QString realSequence = GTClipboard::text();
    CHECK_SET_ERR(expectedSequence == realSequence, "Sequence is not pasted");
}

GUI_TEST_CLASS_DEFINITION(test_0012) {
    GTFileDialog::openFile(testDir + "_common_data/edit_sequence/test.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    GTUtilsDialog::waitForDialog(new RemovePartFromSequenceDialogFiller("2..2"));
    GTMenu::clickMainMenuItem({"Actions", "Edit", "Remove subsequence..."}, GTGlobals::UseMouse);

    QString sequenceBegin = GTUtilsSequenceView::getBeginOfSequenceAsString(3);
    CHECK_SET_ERR(sequenceBegin == "AAT", "Sequence starts with " + sequenceBegin + ", expected AAT");

    int length = GTUtilsSequenceView::getLengthOfSequence();
    CHECK_SET_ERR(length == 29, "Sequence length is " + QString::number(length) + ", expected 29");

    bool found = GTUtilsAnnotationsTreeView::findRegion("DUMMY_1", U2Region(2, 5));
    CHECK_SET_ERR(found == true, "There is no {2..5} region in annotation");
}

static QMap<QString, QString> getReferenceQualifiers() {
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

static QString shiftQualifierRegions(const QString& value, int delta) {
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

static void checkQualifierValue(const QString& qualName, int regionShift) {
    QTreeWidgetItem* qual = GTUtilsAnnotationsTreeView::findItem(qualName);
    QString qualValue = qual->data(2, Qt::DisplayRole).toString();
    QString expectedVal = shiftQualifierRegions(getReferenceQualifiers()[qualName], regionShift);
    CHECK_SET_ERR(qualValue == expectedVal, QString("Qualifier value has changed unexpectedly. Expected: '%1'. Actual: '%2'").arg(expectedVal).arg(qualValue));
}

static void checkQualifierRegionsShift(int shift) {
    foreach (const QString& qualName, getReferenceQualifiers().keys()) {
        checkQualifierValue(qualName, shift);
    }
}

/** Expands all annotation in "Misc. Feature" group. This action lazily creates qualifier tree items. */
static void expandAllAnnotationsInGroup() {
    QTreeWidgetItem* groupItem = GTUtilsAnnotationsTreeView::findItem("Misc. Feature  (0, 2)");
    GTTreeWidget::expand(groupItem);
    for (int i = 0; i < groupItem->childCount(); ++i) {
        QTreeWidgetItem* annotationItem = groupItem->child(i);
        GTTreeWidget::expand(annotationItem);
    }
}

GUI_TEST_CLASS_DEFINITION(test_0013_1) {
    // Check that qualifiers are recalculated on a removal of a subsequence that is located to the left of a region mentioned in a qualifier

    GTFileDialog::openFile(testDir + "_common_data/genbank/qulifier_rebuilding.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    expandAllAnnotationsInGroup();

    GTUtilsDialog::waitForDialog(new RemovePartFromSequenceDialogFiller("1..10", false));
    GTMenu::clickMainMenuItem({"Actions", "Edit", "Remove subsequence..."}, GTGlobals::UseMouse);
    GTUtilsTaskTreeView::waitTaskFinished();

    checkQualifierRegionsShift(0);

    GTUtilsDialog::waitForDialog(new RemovePartFromSequenceDialogFiller("1..10", true));
    GTMenu::clickMainMenuItem({"Actions", "Edit", "Remove subsequence..."}, GTGlobals::UseMouse);
    GTUtilsTaskTreeView::waitTaskFinished();

    checkQualifierRegionsShift(-10);
}

GUI_TEST_CLASS_DEFINITION(test_0013_1_neg) {
    // Check that qualifiers aren't recalculated on a removal of a subsequence that is located to the right of a region mentioned in a qualifier

    GTFileDialog::openFile(testDir + "_common_data/genbank/qulifier_rebuilding.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    expandAllAnnotationsInGroup();

    GTUtilsDialog::waitForDialog(new RemovePartFromSequenceDialogFiller("1000..1100", true));
    GTMenu::clickMainMenuItem({"Actions", "Edit", "Remove subsequence..."}, GTGlobals::UseMouse);
    GTUtilsTaskTreeView::waitTaskFinished();

    checkQualifierRegionsShift(0);
}

GUI_TEST_CLASS_DEFINITION(test_0013_2) {
    // Check that a translation qualifier is recalculated on a removal of a subsequence that is located inside an annotated region

    GTFileDialog::openFile(dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new RemovePartFromSequenceDialogFiller("1040..1042", true));
    GTMenu::clickMainMenuItem({"Actions", "Edit", "Remove subsequence..."}, GTGlobals::UseMouse);
    GTUtilsTaskTreeView::waitTaskFinished();

    QTreeWidgetItem* annotationGroup = GTUtilsAnnotationsTreeView::findItem("CDS  (0, 4)");
    GTTreeWidget::expand(annotationGroup);
    GTTreeWidget::expand(annotationGroup->child(0));
    QTreeWidgetItem* qualItem = annotationGroup->child(0)->child(5);
    CHECK_SET_ERR(qualItem->text(0) == "translation", "Unexpected qualifier found");
    CHECK_SET_ERR(qualItem->text(2).startsWith("WARLLPLP*V*P*"), "Unexpected 'translation' qualifier value");
}

GUI_TEST_CLASS_DEFINITION(test_0013_2_neg) {
    // Check that a translation qualifier isn't recalculated on a removal of a subsequence that is located outside an annotated region

    GTFileDialog::openFile(dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new RemovePartFromSequenceDialogFiller("996..1000", true));
    GTMenu::clickMainMenuItem({"Actions", "Edit", "Remove subsequence..."}, GTGlobals::UseMouse);
    GTUtilsTaskTreeView::waitTaskFinished();

    QTreeWidgetItem* annotationGroup = GTUtilsAnnotationsTreeView::findItem("CDS  (0, 4)");
    GTTreeWidget::expand(annotationGroup);
    GTTreeWidget::expand(annotationGroup->child(0));
    QTreeWidgetItem* qualItem = annotationGroup->child(0)->child(5);
    CHECK_SET_ERR("translation" == qualItem->text(0), "Unexpected qualifier found");
    CHECK_SET_ERR(qualItem->text(2).startsWith("MGQTVTTPLSLTLDHWKD"), "Unexpected 'translation' qualifier value");
}

GUI_TEST_CLASS_DEFINITION(test_0014_1) {
    // Check that qualifiers are recalculated on an insertion of a subsequence that is located to the left of a region mentioned in a qualifier

    GTFileDialog::openFile(testDir + "_common_data/genbank/qulifier_rebuilding.gb");
    GTUtilsTaskTreeView::waitTaskFinished();
    expandAllAnnotationsInGroup();

    GTUtilsSequenceView::selectSequenceRegion(1, 1);

    GTUtilsDialog::waitForDialog(new InsertSequenceFiller("AAAAAAAAAA", InsertSequenceFiller::Resize, 1, "", InsertSequenceFiller::FASTA, false, false, GTGlobals::UseKey, false, false));
    GTMenu::clickMainMenuItem({"Actions", "Edit", "Insert subsequence..."}, GTGlobals::UseMouse);
    GTUtilsTaskTreeView::waitTaskFinished();

    checkQualifierRegionsShift(0);

    GTUtilsDialog::waitForDialog(new InsertSequenceFiller("AAAAAAAAAA", InsertSequenceFiller::Resize, 1, "", InsertSequenceFiller::FASTA, false, false, GTGlobals::UseKey, false, true));
    GTMenu::clickMainMenuItem({"Actions", "Edit", "Insert subsequence..."}, GTGlobals::UseMouse);
    GTUtilsTaskTreeView::waitTaskFinished();

    checkQualifierRegionsShift(10);
}

GUI_TEST_CLASS_DEFINITION(test_0014_1_neg) {
    // Check that qualifiers aren't recalculated on an insertion of a subsequence that is located to the right of a region mentioned in a qualifier

    GTFileDialog::openFile(testDir + "_common_data/genbank/qulifier_rebuilding.gb");
    GTUtilsTaskTreeView::waitTaskFinished();
    expandAllAnnotationsInGroup();

    GTUtilsSequenceView::selectSequenceRegion(100000, 100000);

    GTUtilsDialog::waitForDialog(new InsertSequenceFiller("AAAAAAAAAA", InsertSequenceFiller::Resize, 100000, "", InsertSequenceFiller::FASTA, false, false, GTGlobals::UseKey, false, true));
    GTMenu::clickMainMenuItem({"Actions", "Edit", "Insert subsequence..."}, GTGlobals::UseMouse);
    GTUtilsTaskTreeView::waitTaskFinished();

    checkQualifierRegionsShift(0);
}

GUI_TEST_CLASS_DEFINITION(test_0014_2) {
    // Check that a translation qualifier is recalculated on an insertion of a subsequence that is located inside an annotated region

    GTFileDialog::openFile(dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsSequenceView::selectSequenceRegion(1050, 1050);

    GTUtilsDialog::waitForDialog(new InsertSequenceFiller("A", InsertSequenceFiller::Resize, 1050, "", InsertSequenceFiller::FASTA, false, false, GTGlobals::UseKey, false, true));
    GTMenu::clickMainMenuItem({"Actions", "Edit", "Insert subsequence..."}, GTGlobals::UseMouse);
    GTUtilsTaskTreeView::waitTaskFinished();

    QTreeWidgetItem* annotationGroup = GTUtilsAnnotationsTreeView::findItem("CDS  (0, 4)");
    GTTreeWidget::expand(annotationGroup);
    GTTreeWidget::expand(annotationGroup->child(0));
    QTreeWidgetItem* qualItem = annotationGroup->child(0)->child(5);
    CHECK_SET_ERR(qualItem->text(0) == "translation", "Unexpected qualifier found");
    CHECK_SET_ERR(qualItem->text(2).startsWith("MGQDCYHSLKFDLRSLER"), "Unexpected 'translation' qualifier value");
}

GUI_TEST_CLASS_DEFINITION(test_0014_2_neg) {
    // Check that a translation qualifier isn't recalculated on an insertion of a subsequence that is located outside an annotated region

    GTFileDialog::openFile(dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsSequenceView::selectSequenceRegion(1, 1);

    GTUtilsDialog::waitForDialog(new InsertSequenceFiller("A", InsertSequenceFiller::Resize, 1, "", InsertSequenceFiller::FASTA, false, false, GTGlobals::UseKey, false, true));
    GTMenu::clickMainMenuItem({"Actions", "Edit", "Insert subsequence..."}, GTGlobals::UseMouse);
    GTUtilsTaskTreeView::waitTaskFinished();

    QTreeWidgetItem* annotationGroup = GTUtilsAnnotationsTreeView::findItem("CDS  (0, 4)");
    GTTreeWidget::expand(annotationGroup);
    GTTreeWidget::expand(annotationGroup->child(0));
    QTreeWidgetItem* qualItem = annotationGroup->child(0)->child(5);
    CHECK_SET_ERR(qualItem->text(0) == "translation", "Unexpected qualifier found");
    CHECK_SET_ERR(qualItem->text(2).startsWith("MGQTVTTPLSLTLDHWKD"), "Unexpected 'translation' qualifier value");
}

GUI_TEST_CLASS_DEFINITION(test_0015_1) {
    // Check that a qualifier is recalculated on a substitution of a subsequence that is located to the left of a region mentioned in the qualifier

    GTFileDialog::openFile(testDir + "_common_data/genbank/qulifier_rebuilding.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    expandAllAnnotationsInGroup();

    GTUtilsSequenceView::selectSequenceRegion(1, 10);

    GTUtilsDialog::waitForDialog(new ReplaceSubsequenceDialogFiller("AAAAA", false));
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Edit", "Replace subsequence..."}));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();
    GTUtilsTaskTreeView::waitTaskFinished();

    checkQualifierRegionsShift(0);

    GTUtilsSequenceView::selectSequenceRegion(1, 10);

    GTUtilsDialog::waitForDialog(new ReplaceSubsequenceDialogFiller("AAAAA", true));
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Edit", "Replace subsequence..."}));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();
    GTUtilsTaskTreeView::waitTaskFinished();

    checkQualifierRegionsShift(-5);
}

GUI_TEST_CLASS_DEFINITION(test_0015_1_neg) {
    // Check that a qualifier isn't recalculated on a substitution of a subsequence that is located to the right of a region mentioned in the qualifier

    GTFileDialog::openFile(testDir + "_common_data/genbank/qulifier_rebuilding.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    expandAllAnnotationsInGroup();

    GTUtilsSequenceView::selectSequenceRegion(1000, 1010);

    GTUtilsDialog::waitForDialog(new ReplaceSubsequenceDialogFiller("AAAAA", true));
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Edit", "Replace subsequence..."}));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();
    GTUtilsTaskTreeView::waitTaskFinished();

    checkQualifierRegionsShift(0);
}

GUI_TEST_CLASS_DEFINITION(test_0015_2) {
    // Check that a translation qualifier is recalculated on a substitution of a subsequence that is located inside an annotated region

    GTFileDialog::openFile(dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsSequenceView::selectSequenceRegion(1050, 1050);

    GTUtilsDialog::waitForDialog(new ReplaceSubsequenceDialogFiller("AAA", true));
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Edit", "Replace subsequence..."}));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();
    GTUtilsTaskTreeView::waitTaskFinished();

    QTreeWidgetItem* annotationGroup = GTUtilsAnnotationsTreeView::findItem("CDS  (0, 4)");
    GTTreeWidget::expand(annotationGroup);
    GTTreeWidget::expand(annotationGroup->child(0));
    QTreeWidgetItem* qualItem = annotationGroup->child(0)->child(5);
    CHECK_SET_ERR(qualItem->text(0) == "translation", "Unexpected qualifier found");
    CHECK_SET_ERR(qualItem->text(2).startsWith("MGQKLLPLP*V*P*ITGKMS"), "Unexpected 'translation' qualifier value");
}

GUI_TEST_CLASS_DEFINITION(test_0015_2_neg) {
    // Check that a translation qualifier isn't recalculated on a substitution of a subsequence that is located outside an annotated region

    GTFileDialog::openFile(dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsSequenceView::selectSequenceRegion(996, 1000);

    GTUtilsDialog::waitForDialog(new ReplaceSubsequenceDialogFiller("AA", true));
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Edit", "Replace subsequence..."}, GTGlobals::UseMouse));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();
    GTUtilsTaskTreeView::waitTaskFinished();

    QTreeWidgetItem* annotationGroup = GTUtilsAnnotationsTreeView::findItem("CDS  (0, 4)");
    GTTreeWidget::expand(annotationGroup);
    GTTreeWidget::expand(annotationGroup->child(0));
    QTreeWidgetItem* qualItem = annotationGroup->child(0)->child(5);
    CHECK_SET_ERR(qualItem->text(0) == "translation", "Unexpected qualifier found");
    CHECK_SET_ERR(qualItem->text(2).startsWith("MGQTVTTPLSLTLDHWKD"), "Unexpected 'translation' qualifier value");
}

GUI_TEST_CLASS_DEFINITION(test_0016_1) {
    // Check that a qualifier isn't recalculated on a removal of a subsequence that includes a region mentioned in the qualifier

    GTFileDialog::openFile(testDir + "_common_data/genbank/qulifier_rebuilding.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    expandAllAnnotationsInGroup();

    GTUtilsDialog::waitForDialog(new RemovePartFromSequenceDialogFiller("1..600", true));
    GTUtilsNotifications::waitForNotification(false);
    GTMenu::clickMainMenuItem({"Actions", "Edit", "Remove subsequence..."}, GTGlobals::UseMouse);

    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsMdi::activateWindow("human_T1 [qulifier_rebuilding.gb]");

    checkQualifierRegionsShift(0);
}

GUI_TEST_CLASS_DEFINITION(test_0016_2) {
    // Check that a qualifier isn't recalculated on a complete replacement of a subsequence that includes a region mentioned in the qualifier

    GTFileDialog::openFile(testDir + "_common_data/genbank/qulifier_rebuilding.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    expandAllAnnotationsInGroup();

    GTUtilsSequenceView::selectSequenceRegion(1, 600);

    GTUtilsDialog::waitForDialog(new ReplaceSubsequenceDialogFiller("AAAAA", true));
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Edit", "Replace subsequence..."}));
    GTUtilsNotifications::waitForNotification(false);
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsMdi::activateWindow("human_T1 [qulifier_rebuilding.gb]");

    checkQualifierRegionsShift(0);
}

}  // namespace GUITest_common_scenarios_sequence_edit

}  // namespace U2
