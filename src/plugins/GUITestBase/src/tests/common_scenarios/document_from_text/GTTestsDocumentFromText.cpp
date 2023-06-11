/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
 * http://ugene.net
 *
 * This program is free software; you can redistribute it and/or * modify it under the terms of the GNU General Public License
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

#include "GTTestsDocumentFromText.h"
#include <base_dialogs/GTFileDialog.h>
#include <base_dialogs/MessageBoxFiller.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTPlainTextEdit.h>
#include <primitives/GTWidget.h>

#include <QApplication>

#include "GTUtilsDocument.h"
#include "GTUtilsProject.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsSequenceView.h"
#include "GTUtilsTaskTreeView.h"
#include "primitives/GTAction.h"
#include "primitives/GTMenu.h"
#include "primitives/PopupChooser.h"
#include "runnables/ugene/corelibs/U2Gui/CreateDocumentFromTextDialogFiller.h"
#include "runnables/ugene/ugeneui/SaveProjectDialogFiller.h"

namespace U2 {

namespace GUITest_common_scenarios_document_from_text {

GUI_TEST_CLASS_DEFINITION(test_0001) {
    auto filler = new CreateDocumentFiller(
        "ACAAGTCGGATTTATA",
        false,
        CreateDocumentFiller::StandardRNA,
        true,
        false,
        "",
        testDir + "_common_data/scenarios/sandbox/result",
        CreateDocumentFiller::FASTA,
        "result",
        true);
    GTUtilsDialog::waitForDialog(filler);
    GTMenu::clickMainMenuItem({"File", "New document from text..."}, GTGlobals::UseKey);

    GTUtilsDialog::checkNoActiveWaiters();

    GTUtilsDocument::checkDocument("result");
    GTUtilsSequenceView::checkSequence("ACAAGTCGGATTTATA");
}

GUI_TEST_CLASS_DEFINITION(test_0001_1) {
    auto filler = new CreateDocumentFiller(
        "ACAAGTCGGATTTATA",
        false,
        CreateDocumentFiller::StandardRNA,
        true,
        false,
        "",
        testDir + "_common_data/scenarios/sandbox/result",
        CreateDocumentFiller::FASTA,
        "result",
        false);

    GTUtilsDialog::waitForDialog(filler);

    GTMenu::clickMainMenuItem({"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDocument::checkDocument("result");

    GTUtilsSequenceView::checkSequence("ACAAGTCGGATTTATA");
}

GUI_TEST_CLASS_DEFINITION(test_0001_2) {
    auto filler = new CreateDocumentFiller(
        "ACAAGTCGGATTTATA",
        false,
        CreateDocumentFiller::StandardRNA,
        true,
        false,
        "",
        testDir + "_common_data/scenarios/sandbox/result",
        CreateDocumentFiller::Genbank,
        "result",
        true);

    GTUtilsDialog::waitForDialog(filler);
    GTMenu::clickMainMenuItem({"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDocument::checkDocument("result.gb");

    GTUtilsSequenceView::checkSequence("ACAAGTCGGATTTATA");
}

GUI_TEST_CLASS_DEFINITION(test_0002) {
    auto filler = new CreateDocumentFiller(
        "ACAAGTCGGATTTATA",
        false,
        CreateDocumentFiller::StandardRNA,
        true,
        false,
        "",
        testDir + "_common_data/scenarios/sandbox/result",
        CreateDocumentFiller::Genbank,
        "result",
        true);
    GTUtilsDialog::waitForDialog(filler);

    GTMenu::clickMainMenuItem({"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDocument::checkDocument("result.gb");

    GTUtilsSequenceView::checkSequence("ACAAGTCGGATTTATA");
}

GUI_TEST_CLASS_DEFINITION(test_0002_1) {
    auto filler = new CreateDocumentFiller(
        "ACAAGTCGGATTTATAACAAGTCGGATTTATA",
        false,
        CreateDocumentFiller::StandardRNA,
        true,
        false,
        "",
        testDir + "_common_data/scenarios/sandbox/result",
        CreateDocumentFiller::Genbank,
        "result",
        false);
    GTUtilsDialog::waitForDialog(filler);
    GTMenu::clickMainMenuItem({"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDocument::checkDocument("result.gb");

    GTUtilsSequenceView::checkSequence("ACAAGTCGGATTTATAACAAGTCGGATTTATA");
}

GUI_TEST_CLASS_DEFINITION(test_0002_2) {
    auto filler = new CreateDocumentFiller(
        "ACAAGTCGGATTTATA",
        false,
        CreateDocumentFiller::StandardDNA,
        true,
        false,
        "",
        testDir + "_common_data/scenarios/sandbox/result",
        CreateDocumentFiller::Genbank,
        "result",
        true);
    GTUtilsDialog::waitForDialog(filler);

    GTMenu::clickMainMenuItem({"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDocument::checkDocument("result.gb");

    GTUtilsSequenceView::checkSequence("ACAAGTCGGATTTATA");

    GTUtilsDocument::removeDocument("result.gb");

    Runnable* filler1 = new CreateDocumentFiller(
        "ACAA",
        false,
        CreateDocumentFiller::StandardRNA,
        true,
        false,
        "",
        testDir + "_common_data/scenarios/sandbox/result_new",
        CreateDocumentFiller::Genbank,
        "result_new",
        true);
    GTUtilsDialog::waitForDialog(filler1);

    GTMenu::clickMainMenuItem({"File", "New document from text..."}, GTGlobals::UseKey);

    GTUtilsDocument::checkDocument("result_new.gb");

    GTUtilsSequenceView::checkSequence("ACAA");
}

GUI_TEST_CLASS_DEFINITION(test_0003) {
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok));
    auto filler = new CreateDocumentFiller(
        "FKMDNYTRVEPPG,DD.JFUYBVYERHGK",
        true,
        CreateDocumentFiller::AllSymbols,
        true,
        false,
        "",
        testDir + "_common_data/scenarios/sandbox/result",
        CreateDocumentFiller::FASTA,
        "result",
        true);
    GTUtilsDialog::waitForDialog(filler);

    GTMenu::clickMainMenuItem({"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDocument::checkDocument("result");

    GTUtilsSequenceView::checkSequence("FKMDNYTRVEPPG,DD.JFUYBVYERHGK");
}

GUI_TEST_CLASS_DEFINITION(test_0003_1) {
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok));
    auto filler = new CreateDocumentFiller(
        "FKMDNYTRVEPPG,DD.JFUYBVYERHGK",
        true,
        CreateDocumentFiller::AllSymbols,
        true,
        false,
        "",
        testDir + "_common_data/scenarios/sandbox/result",
        CreateDocumentFiller::Genbank,
        "result",
        true);
    GTUtilsDialog::waitForDialog(filler);

    GTMenu::clickMainMenuItem({"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDocument::checkDocument("result.gb");

    GTUtilsSequenceView::checkSequence("FKMDNYTRVEPPG,DD.JFUYBVYERHGK");
}

GUI_TEST_CLASS_DEFINITION(test_0003_2) {
    auto filler = new CreateDocumentFiller(
        "ACGT",
        true,
        CreateDocumentFiller::StandardDNA,
        true,
        false,
        "",
        testDir + "_common_data/scenarios/sandbox/result",
        CreateDocumentFiller::FASTA,
        "result",
        false);
    GTUtilsDialog::waitForDialog(filler);

    GTMenu::clickMainMenuItem({"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDocument::checkDocument("result");

    GTUtilsSequenceView::checkSequence("ACGT");
}

GUI_TEST_CLASS_DEFINITION(test_0004) {
    auto filler = new CreateDocumentFiller(
        "RNACCGTTAAIOUAGCCDOOPMAGTZZ",
        true,
        CreateDocumentFiller::StandardDNA,
        true,
        false,
        "",
        testDir + "_common_data/scenarios/sandbox/result",
        CreateDocumentFiller::FASTA,
        "result",
        true);
    GTUtilsDialog::waitForDialog(filler);

    GTMenu::clickMainMenuItem({"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDocument::checkDocument("result");

    GTUtilsSequenceView::checkSequence("NACCGTTAAAGCCAGT");
}

GUI_TEST_CLASS_DEFINITION(test_0004_1) {
    auto filler = new CreateDocumentFiller(
        "RNACCGTTAAIOUAGCCDOOPMAGTZZ",
        true,
        CreateDocumentFiller::StandardDNA,
        true,
        false,
        "",
        testDir + "_common_data/scenarios/sandbox/result",
        CreateDocumentFiller::Genbank,
        "result",
        true);
    GTUtilsDialog::waitForDialog(filler);

    GTMenu::clickMainMenuItem({"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDocument::checkDocument("result.gb");

    GTUtilsSequenceView::checkSequence("NACCGTTAAAGCCAGT");
}

GUI_TEST_CLASS_DEFINITION(test_0004_2) {
    auto filler = new CreateDocumentFiller(
        "RNACCGTTAAIOUAGCCDOOPMAGTZZ",
        true,
        CreateDocumentFiller::StandardRNA,
        true,
        false,
        "",
        testDir + "_common_data/scenarios/sandbox/result",
        CreateDocumentFiller::FASTA,
        "result",
        true);
    GTUtilsDialog::waitForDialog(filler);

    GTMenu::clickMainMenuItem({"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDocument::checkDocument("result");

    GTUtilsSequenceView::checkSequence("NACCGAAUAGCCAG");
}

GUI_TEST_CLASS_DEFINITION(test_0005) {
    auto filler = new CreateDocumentFiller(
        "RNACCGTTAAIOUAGCCDOOPMAGTZZ",
        true,
        CreateDocumentFiller::StandardDNA,
        false,
        true,
        "-",
        testDir + "_common_data/scenarios/sandbox/result",
        CreateDocumentFiller::FASTA,
        "result",
        true);
    GTUtilsDialog::waitForDialog(filler);

    GTMenu::clickMainMenuItem({"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDocument::checkDocument("result");

    GTUtilsSequenceView::checkSequence("-NACCGTTAA---AGCC-----AGT--");
}

GUI_TEST_CLASS_DEFINITION(test_0005_1) {
    auto filler = new CreateDocumentFiller(
        "RNACCGTTAAIOUAGCCDOOPMAGTZZ",
        true,
        CreateDocumentFiller::StandardDNA,
        false,
        true,
        "-",
        testDir + "_common_data/scenarios/sandbox/result",
        CreateDocumentFiller::Genbank,
        "result",
        true);
    GTUtilsDialog::waitForDialog(filler);

    GTMenu::clickMainMenuItem({"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDocument::checkDocument("result.gb");

    GTUtilsSequenceView::checkSequence("-NACCGTTAA---AGCC-----AGT--");
}

GUI_TEST_CLASS_DEFINITION(test_0005_2) {
    auto filler = new CreateDocumentFiller(
        "RNACCGTTAAIOUAGCCDOOPMAGTZZ",
        true,
        CreateDocumentFiller::StandardRNA,
        false,
        true,
        "A",
        testDir + "_common_data/scenarios/sandbox/result",
        CreateDocumentFiller::FASTA,
        "result",
        true);
    GTUtilsDialog::waitForDialog(filler);

    GTMenu::clickMainMenuItem({"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDocument::checkDocument("result");

    GTUtilsSequenceView::checkSequence("ANACCGAAAAAAUAGCCAAAAAAGAAA");
}
GUI_TEST_CLASS_DEFINITION(test_0006) {
    auto filler = new CreateDocumentFiller(
        "RNACCGTTAAIOUAGCCDOOPMAGTZZ",
        true,
        CreateDocumentFiller::ExtendedDNA,
        true,
        false,
        "A",
        testDir + "_common_data/scenarios/sandbox/result",
        CreateDocumentFiller::FASTA,
        "result",
        true);
    GTUtilsDialog::waitForDialog(filler);

    GTMenu::clickMainMenuItem({"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDocument::checkDocument("result");

    GTUtilsSequenceView::checkSequence("RNACCGTTAAAGCCDMAGT");
}

GUI_TEST_CLASS_DEFINITION(test_0006_1) {
    auto filler = new CreateDocumentFiller(
        "RNACCGTTAAIOUAGCCDOOPMAGTZZ",
        true,
        CreateDocumentFiller::ExtendedDNA,
        true,
        false,
        "A",
        testDir + "_common_data/scenarios/sandbox/result",
        CreateDocumentFiller::Genbank,
        "result",
        true);
    GTUtilsDialog::waitForDialog(filler);

    GTMenu::clickMainMenuItem({"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDocument::checkDocument("result.gb");

    GTUtilsSequenceView::checkSequence("RNACCGTTAAAGCCDMAGT");
}

GUI_TEST_CLASS_DEFINITION(test_0006_2) {
    auto filler = new CreateDocumentFiller(
        "RNACCGTTAAIOUA---GCCDOOPMAGTZZ",
        true,
        CreateDocumentFiller::ExtendedDNA,
        true,
        false,
        "A",
        testDir + "_common_data/scenarios/sandbox/result",
        CreateDocumentFiller::FASTA,
        "result",
        true);
    GTUtilsDialog::waitForDialog(filler);

    GTMenu::clickMainMenuItem({"File", "New document from text..."}, GTGlobals::UseKey);

    GTUtilsDocument::checkDocument("result");

    GTUtilsSequenceView::checkSequence("RNACCGTTAAA---GCCDMAGT");
}

GUI_TEST_CLASS_DEFINITION(test_0007) {
    auto filler = new CreateDocumentFiller(
        "RNACCGTTAAIOUAGCCDOOPMAGTZZ",
        true,
        CreateDocumentFiller::StandardRNA,
        true,
        false,
        "A",
        testDir + "_common_data/scenarios/sandbox/result",
        CreateDocumentFiller::FASTA,
        "result",
        true);
    GTUtilsDialog::waitForDialog(filler);

    GTMenu::clickMainMenuItem({"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDocument::checkDocument("result");

    GTUtilsSequenceView::checkSequence("NACCGAAUAGCCAG");
}

GUI_TEST_CLASS_DEFINITION(test_0007_1) {
    auto filler = new CreateDocumentFiller(
        "RNACCGTTAAIOUAGCCDOOPMAGTZZ",
        true,
        CreateDocumentFiller::StandardRNA,
        true,
        false,
        "A",
        testDir + "_common_data/scenarios/sandbox/result",
        CreateDocumentFiller::Genbank,
        "result",
        true);
    GTUtilsDialog::waitForDialog(filler);

    GTMenu::clickMainMenuItem({"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDocument::checkDocument("result.gb");

    GTUtilsSequenceView::checkSequence("NACCGAAUAGCCAG");
}

GUI_TEST_CLASS_DEFINITION(test_0007_2) {
    auto filler = new CreateDocumentFiller(
        "RNACCGTTAAIOU---AGCCDOOPMAGTZZ",
        true,
        CreateDocumentFiller::StandardRNA,
        true,
        false,
        "A",
        testDir + "_common_data/scenarios/sandbox/result",
        CreateDocumentFiller::FASTA,
        "result",
        true);
    GTUtilsDialog::waitForDialog(filler);

    GTMenu::clickMainMenuItem({"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDocument::checkDocument("result");

    GTUtilsSequenceView::checkSequence("NACCGAAU---AGCCAG");
}

GUI_TEST_CLASS_DEFINITION(test_0008) {
    auto filler = new CreateDocumentFiller(
        "RNACCGTTAAIOUAGCCDOOPMAGTZZ",
        true,
        CreateDocumentFiller::ExtendedRNA,
        true,
        false,
        "A",
        testDir + "_common_data/scenarios/sandbox/result",
        CreateDocumentFiller::FASTA,
        "result",
        true);
    GTUtilsDialog::waitForDialog(filler);
    GTMenu::clickMainMenuItem({"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDocument::checkDocument("result");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    GTUtilsSequenceView::checkSequence("RNACCGAAUAGCCDMAG");
}

GUI_TEST_CLASS_DEFINITION(test_0008_1) {
    auto filler = new CreateDocumentFiller(
        "RNACCGTTAAIOUAGCCDOOPMAGTZZ",
        true,
        CreateDocumentFiller::ExtendedRNA,
        true,
        false,
        "A",
        testDir + "_common_data/scenarios/sandbox/result",
        CreateDocumentFiller::Genbank,
        "result",
        true);
    GTUtilsDialog::waitForDialog(filler);

    GTMenu::clickMainMenuItem({"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDocument::checkDocument("result.gb");

    GTUtilsSequenceView::checkSequence("RNACCGAAUAGCCDMAG");
}

GUI_TEST_CLASS_DEFINITION(test_0008_2) {
    auto filler = new CreateDocumentFiller(
        "RNACCGTTAAIOU---AGCCDOOPMAGTZZ",
        true,
        CreateDocumentFiller::ExtendedRNA,
        true,
        false,
        "A",
        testDir + "_common_data/scenarios/sandbox/result",
        CreateDocumentFiller::FASTA,
        "result",
        true);
    GTUtilsDialog::waitForDialog(filler);

    GTMenu::clickMainMenuItem({"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDocument::checkDocument("result");

    GTUtilsSequenceView::checkSequence("RNACCGAAU---AGCCDMAG");
}

GUI_TEST_CLASS_DEFINITION(test_0009) {
    auto filler = new CreateDocumentFiller(
        "RNACCGTTAAIOUAGCCDOOPMAGTZZ",
        true,
        CreateDocumentFiller::ExtendedDNA,
        false,
        true,
        "-",
        testDir + "_common_data/scenarios/sandbox/result",
        CreateDocumentFiller::FASTA,
        "result",
        true);
    GTUtilsDialog::waitForDialog(filler);

    GTMenu::clickMainMenuItem({"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDocument::checkDocument("result");

    GTUtilsSequenceView::checkSequence("RNACCGTTAA---AGCCD---MAGT--");
}

GUI_TEST_CLASS_DEFINITION(test_0009_1) {
    auto filler = new CreateDocumentFiller(
        "RNACCGTTAAIOUAGCCDOOPMAGTZZ",
        true,
        CreateDocumentFiller::ExtendedDNA,
        false,
        true,
        "-",
        testDir + "_common_data/scenarios/sandbox/result",
        CreateDocumentFiller::Genbank,
        "result",
        true);
    GTUtilsDialog::waitForDialog(filler);

    GTMenu::clickMainMenuItem({"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDocument::checkDocument("result.gb");

    GTUtilsSequenceView::checkSequence("RNACCGTTAA---AGCCD---MAGT--");
}

GUI_TEST_CLASS_DEFINITION(test_0009_2) {
    auto filler = new CreateDocumentFiller(
        "RNACCGT---TAAIOUAGCCDOOPMAGTZZ",
        true,
        CreateDocumentFiller::ExtendedDNA,
        false,
        true,
        "-",
        testDir + "_common_data/scenarios/sandbox/result",
        CreateDocumentFiller::FASTA,
        "result",
        true);
    GTUtilsDialog::waitForDialog(filler);

    GTMenu::clickMainMenuItem({"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDocument::checkDocument("result");

    GTUtilsSequenceView::checkSequence("RNACCGT---TAA---AGCCD---MAGT--");
}

GUI_TEST_CLASS_DEFINITION(test_0010) {
    //    Creating new document from text

    //  1. Open menu {File->New Document From Text}
    //  Expected result: Create document dialog has appear
    //  2. Fill the next fields in dialog:
    //    {Paste data here} RNACCGTTAAIOUAGCCDOOPMAGTZZ
    //    {Custom settings} set checked
    //    {Alphabet} Extended RNA alphabet
    //    {Replace unknown symbols} set checked {Text field} -
    //    {Document location} _common_data/scenarios/sandbox/result.fa
    //    {Document Format} FASTA
    //    {Save file immidiately} set checked
    auto filler = new CreateDocumentFiller(
        "RNACCGTTAAIOUAGCCDOOPMAGTZZ",
        true,
        CreateDocumentFiller::ExtendedRNA,
        false,
        true,
        "-",
        testDir + "_common_data/scenarios/sandbox/result",
        CreateDocumentFiller::FASTA,
        "result",
        true);
    GTUtilsDialog::waitForDialog(filler);

    GTMenu::clickMainMenuItem({"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished();

    //  3. Click Create button
    //  Expected result:
    //   - result.fa document added to project
    //   - sequence view opened
    //   - sequence are RNACCG--AA--UAGCCD---MAG---

    GTUtilsDocument::checkDocument("result");

    GTUtilsSequenceView::checkSequence("RNACCG--AA--UAGCCD---MAG---");
}

GUI_TEST_CLASS_DEFINITION(test_0010_1) {
    // CHANGES: using genbank format
    //     Creating new document from text

    //  1. Open menu {File->New Document From Text}
    //  Expected result: Create document dialog has appear
    //  2. Fill the next fields in dialog:
    //    {Paste data here} RNACCGTTAAIOUAGCCDOOPMAGTZZ
    //    {Custom settings} set checked
    //    {Alphabet} Extended RNA alphabet
    //    {Replace unknown symbols} set checked {Text field} -
    //    {Document location} _common_data/scenarios/sandbox/result.fa
    //    {Document Format} FASTA
    //    {Save file immidiately} set checked
    auto filler = new CreateDocumentFiller(
        "RNACCGTTAAIOUAGCCDOOPMAGTZZ",
        true,
        CreateDocumentFiller::ExtendedRNA,
        false,
        true,
        "-",
        testDir + "_common_data/scenarios/sandbox/result",
        CreateDocumentFiller::Genbank,
        "result.gb",
        true);
    GTUtilsDialog::waitForDialog(filler);

    GTMenu::clickMainMenuItem({"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished();

    //  3. Click Create button
    //  Expected result:
    //   - result.fa document added to project
    //   - sequence view opened
    //   - sequence are RNACCG--AA--UAGCCD---MAG---

    GTUtilsDocument::checkDocument("result.gb");

    GTUtilsSequenceView::checkSequence("RNACCG--AA--UAGCCD---MAG---");
}

GUI_TEST_CLASS_DEFINITION(test_0010_2) {
    // CHANGES: using keys instead mouse
    //     Creating new document from text

    //  1. Open menu {File->New Document From Text}
    //  Expected result: Create document dialog has appear
    //  2. Fill the next fields in dialog:
    //    {Paste data here} RNACCGTTAAIOUAGCCDOOPMAGTZZ
    //    {Custom settings} set checked
    //    {Alphabet} Extended RNA alphabet
    //    {Replace unknown symbols} set checked {Text field} -
    //    {Document location} _common_data/scenarios/sandbox/result.fa
    //    {Document Format} FASTA
    //    {Save file immidiately} set checked
    auto filler = new CreateDocumentFiller(
        "RNACCGTTAAIOUAGCCDOOPMAGTZZ",
        true,
        CreateDocumentFiller::ExtendedRNA,
        false,
        true,
        "-",
        testDir + "_common_data/scenarios/sandbox/result",
        CreateDocumentFiller::FASTA,
        "result",
        true,
        GTGlobals::UseMouse);
    GTUtilsDialog::waitForDialog(filler);

    GTMenu::clickMainMenuItem({"File", "New document from text..."}, GTGlobals::UseMouse);
    GTUtilsTaskTreeView::waitTaskFinished();

    //  3. Click Create button
    //  Expected result:
    //   - result.fa document added to project
    //   - sequence view opened
    //   - sequence are RNACCG--AA--UAGCCD---MAG---

    GTUtilsDocument::checkDocument("result");

    GTUtilsSequenceView::checkSequence("RNACCG--AA--UAGCCD---MAG---");
}

GUI_TEST_CLASS_DEFINITION(test_0011) {
    //    Creating new document from text

    //  1. Open menu {File->New Document From Text}
    //  Expected result: Create document dialog has appear
    //  2. Fill the next fields in dialog:
    //  {Paste data here} RNACCGTTAAIOUAGCCDOOPMAGTZZ
    //  {Custom settings} set checked
    //  {Alphabet} Standard RNA alphabet
    //  {Replace unknown symbols} set checked {Text field} -
    //  {Document location} _common_data/scenarios/sandbox/result.fa
    //  {Document Format} FASTA
    //  {Save file immidiately} set checked
    auto filler = new CreateDocumentFiller(
        "RNACCGTTAAIOUAGCCDOOPMAGTZZ",
        true,
        CreateDocumentFiller::StandardRNA,
        false,
        true,
        "-",
        testDir + "_common_data/scenarios/sandbox/result",
        CreateDocumentFiller::FASTA,
        "result",
        true);
    GTUtilsDialog::waitForDialog(filler);

    GTMenu::clickMainMenuItem({"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished();

    //  3. Click Create button
    //  Expected result:
    //  - result.fa document added to project
    //  - sequence view opened
    //  - sequence are -NACCG--AA--UAGCC-----AG---

    GTUtilsDocument::checkDocument("result");

    GTUtilsSequenceView::checkSequence("-NACCG--AA--UAGCC-----AG---");
}

GUI_TEST_CLASS_DEFINITION(test_0011_1) {
    // CHANGES: using genbank format
    //     Creating new document from text

    //  1. Open menu {File->New Document From Text}
    //  Expected result: Create document dialog has appear
    //  2. Fill the next fields in dialog:
    //  {Paste data here} RNACCGTTAAIOUAGCCDOOPMAGTZZ
    //  {Custom settings} set checked
    //  {Alphabet} Standard RNA alphabet
    //  {Replace unknown symbols} set checked {Text field} -
    //  {Document location} _common_data/scenarios/sandbox/result.fa
    //  {Document Format} FASTA
    //  {Save file immidiately} set checked
    auto filler = new CreateDocumentFiller(
        "RNACCGTTAAIOUAGCCDOOPMAGTZZ",
        true,
        CreateDocumentFiller::StandardRNA,
        false,
        true,
        "-",
        testDir + "_common_data/scenarios/sandbox/result",
        CreateDocumentFiller::Genbank,
        "result.gb",
        true);
    GTUtilsDialog::waitForDialog(filler);

    GTMenu::clickMainMenuItem({"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished();

    //  3. Click Create button
    //  Expected result:
    //  - result.fa document added to project
    //  - sequence view opened
    //  - sequence are -NACCG--AA--UAGCC-----AG---

    GTUtilsDocument::checkDocument("result.gb");

    GTUtilsSequenceView::checkSequence("-NACCG--AA--UAGCC-----AG---");
}

GUI_TEST_CLASS_DEFINITION(test_0011_2) {
    //    Creating new document from text

    //  1. Open menu {File->New Document From Text}
    //  Expected result: Create document dialog has appear
    //  2. Fill the next fields in dialog:
    //  {Paste data here} RNACCGTTAAIOUAGCCDOOPMAGTZZ
    //  {Custom settings} set checked
    //  {Alphabet} Standard RNA alphabet
    //  {Replace unknown symbols} set checked {Text field} -
    //  {Document location} _common_data/scenarios/sandbox/result.fa
    //  {Document Format} FASTA
    //  {Save file immediately} set checked
    auto filler = new CreateDocumentFiller(
        "RNACCGTTAAIOUAGCCDOOPMAGTZZ",
        true,
        CreateDocumentFiller::StandardRNA,
        false,
        true,
        "-",
        testDir + "_common_data/scenarios/sandbox/result",
        CreateDocumentFiller::FASTA,
        "result",
        true,
        GTGlobals::UseMouse);
    GTUtilsDialog::waitForDialog(filler);

    GTMenu::clickMainMenuItem({"File", "New document from text..."});
    GTUtilsTaskTreeView::waitTaskFinished();

    //  3. Click Create button
    //  Expected result:
    //  - result.fa document added to project
    //  - sequence view opened
    //  - sequence are -NACCG--AA--UAGCC-----AG---

    GTUtilsDocument::checkDocument("result");

    GTUtilsSequenceView::checkSequence("-NACCG--AA--UAGCC-----AG---");
}

GUI_TEST_CLASS_DEFINITION(test_0012) {
    auto filler = new CreateDocumentFiller(
        "ACAAGTCGGATTTATA",
        false,
        CreateDocumentFiller::ExtendedDNA,
        false,
        true,
        "-",
        testDir + "_common_data/scenarios/sandbox/result.fa",
        CreateDocumentFiller::FASTA,
        "result",
        true);
    GTUtilsDialog::waitForDialog(filler);

    GTMenu::clickMainMenuItem({"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDocument::checkDocument("result.fa");

    GTUtilsDialog::waitForDialog(new SaveProjectDialogFiller(QDialogButtonBox::No));
    GTMenu::clickMainMenuItem({"File", "Close project"});

    GTFileDialog::openFile(testDir + "_common_data/scenarios/sandbox/", "result.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_0012_1) {
    auto filler = new CreateDocumentFiller(
        "ACAAGTCGGATTTATA",
        false,
        CreateDocumentFiller::ExtendedDNA,
        false,
        true,
        "-",
        testDir + "_common_data/scenarios/sandbox/result.gb",
        CreateDocumentFiller::Genbank,
        "result",
        true);
    GTUtilsDialog::waitForDialog(filler);

    GTMenu::clickMainMenuItem({"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDocument::checkDocument("result.gb");

    GTUtilsDialog::waitForDialog(new SaveProjectDialogFiller(QDialogButtonBox::No));
    GTMenu::clickMainMenuItem({"File", "Close project"});

    GTFileDialog::openFile(testDir + "_common_data/scenarios/sandbox/", "result.gb");
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_0012_2) {
    auto filler = new CreateDocumentFiller(
        "ACAAGTC---GGATTTATA",
        false,
        CreateDocumentFiller::ExtendedDNA,
        false,
        true,
        "-",
        testDir + "_common_data/scenarios/sandbox/result.fa",
        CreateDocumentFiller::FASTA,
        "result",
        true);
    GTUtilsDialog::waitForDialog(filler);

    GTMenu::clickMainMenuItem({"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDocument::checkDocument("result.fa");

    GTUtilsDialog::waitForDialog(new SaveProjectDialogFiller(QDialogButtonBox::No));
    GTMenu::clickMainMenuItem({"File", "Close project"});

    GTFileDialog::openFile(testDir + "_common_data/scenarios/sandbox/", "result.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_0013) {
    auto filler = new CreateDocumentFiller(
        "ACAAGTCGGATTTATA",
        false,
        CreateDocumentFiller::ExtendedDNA,
        false,
        true,
        "-",
        testDir + "_common_data/scenarios/sandbox/result.fa",
        CreateDocumentFiller::FASTA,
        "result",
        true);

    GTUtilsDialog::waitForDialog(filler);

    GTMenu::clickMainMenuItem({"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsDocument::checkDocument("result.fa");

    GTUtilsProjectTreeView::rename("result", "result_new");

    GTUtilsProjectTreeView::findIndex("result_new");  // checks inside
}
GUI_TEST_CLASS_DEFINITION(test_0013_1) {
    auto filler = new CreateDocumentFiller(
        "ACAAGTCGGATTTATA",
        false,
        CreateDocumentFiller::ExtendedDNA,
        false,
        true,
        "-",
        testDir + "_common_data/scenarios/sandbox/result.gb",
        CreateDocumentFiller::Genbank,
        "result",
        true);

    GTUtilsDialog::waitForDialog(filler);

    GTMenu::clickMainMenuItem({"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsDocument::checkDocument("result.gb");

    GTUtilsProjectTreeView::rename("result", "result_new");

    GTUtilsProjectTreeView::findIndex("result_new");  // checks inside
}
GUI_TEST_CLASS_DEFINITION(test_0013_2) {
    auto filler = new CreateDocumentFiller(
        "ACAAGTCG---GATTTATA",
        false,
        CreateDocumentFiller::ExtendedDNA,
        false,
        true,
        "-",
        testDir + "_common_data/scenarios/sandbox/result.fa",
        CreateDocumentFiller::FASTA,
        "result",
        true);

    GTUtilsDialog::waitForDialog(filler);

    GTMenu::clickMainMenuItem({"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsDocument::checkDocument("result.fa");

    GTUtilsProjectTreeView::rename("result", "result_new");

    GTUtilsProjectTreeView::findIndex("result_new");  // checks inside
}

GUI_TEST_CLASS_DEFINITION(test_0014) {
    auto filler = new CancelCreateDocumentFiller(
        "",
        false,
        CancelCreateDocumentFiller::ExtendedDNA,
        false,
        true,
        "-",
        testDir + "",
        CancelCreateDocumentFiller::FASTA,
        "",
        false);

    GTUtilsDialog::waitForDialog(filler);

    GTMenu::clickMainMenuItem({"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsProject::checkProject(GTUtilsProject::NotExists);
}

GUI_TEST_CLASS_DEFINITION(test_0014_1) {
    auto filler = new CancelCreateDocumentFiller(
        "AAAA",
        false,
        CancelCreateDocumentFiller::ExtendedDNA,
        false,
        true,
        "-",
        testDir + "",
        CancelCreateDocumentFiller::Genbank,
        "",
        false);

    GTUtilsDialog::waitForDialog(filler);

    GTMenu::clickMainMenuItem({"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsProject::checkProject(GTUtilsProject::NotExists);
}

GUI_TEST_CLASS_DEFINITION(test_0014_2) {
    auto filler = new CreateDocumentFiller(
        "AAA",
        false,
        CreateDocumentFiller::ExtendedDNA,
        false,
        true,
        "-",
        testDir + "_common_data/scenarios/sandbox/result.fa",
        CreateDocumentFiller::FASTA,
        "result",
        false);

    GTUtilsDialog::waitForDialog(filler);

    GTMenu::clickMainMenuItem({"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsProject::checkProject(GTUtilsProject::Exists);
}

GUI_TEST_CLASS_DEFINITION(test_0015) {
    auto filler = new CreateDocumentFiller(
        "AAA\n",
        false,
        CreateDocumentFiller::ExtendedDNA,
        false,
        true,
        "-",
        testDir + "_common_data/scenarios/sandbox/result.fa",
        CreateDocumentFiller::FASTA,
        "result",
        false);

    GTUtilsDialog::waitForDialog(filler);

    GTMenu::clickMainMenuItem({"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished();

    QAbstractButton* complement = GTAction::button("complement_action");
    CHECK_SET_ERR(!complement->isEnabled(), "button is not disabled");

    auto toolbar = GTWidget::findWidget("views_tool_bar_result");
    GTWidget::click(GTWidget::findWidget("show_hide_details_view", toolbar));

    QAbstractButton* complement1 = GTAction::button("complement_action");
    CHECK_SET_ERR(complement1->isEnabled(), "button is not enabled");

    GTUtilsDialog::waitForDialog(new PopupChecker({"do_not_translate_radiobutton"}, PopupChecker::IsEnabled));
    GTWidget::click(GTWidget::findWidget("translationsMenuToolbarButton"));
}

GUI_TEST_CLASS_DEFINITION(test_0015_1) {
    auto filler = new CreateDocumentFiller(
        "AAA",
        false,
        CreateDocumentFiller::ExtendedDNA,
        false,
        true,
        "-",
        testDir + "_common_data/scenarios/sandbox/result.fa",
        CreateDocumentFiller::FASTA,
        "result",
        false);

    GTUtilsDialog::waitForDialog(filler);

    GTMenu::clickMainMenuItem({"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished();

    QAbstractButton* complement = GTAction::button("complement_action");
    CHECK_SET_ERR(!complement->isEnabled(), "button is not disabled");

    auto toolbar = GTWidget::findWidget("views_tool_bar_result");
    GTWidget::click(GTWidget::findWidget("show_hide_details_view", toolbar));

    QAbstractButton* complement1 = GTAction::button("complement_action");
    CHECK_SET_ERR(complement1->isEnabled(), "button is not enabled");

    GTUtilsDialog::waitForDialog(new PopupChecker({"do_not_translate_radiobutton"}, PopupChecker::IsEnabled));
    GTWidget::click(GTWidget::findWidget("translationsMenuToolbarButton"));
}

GUI_TEST_CLASS_DEFINITION(test_0015_2) {
    auto filler = new CreateDocumentFiller(
        "AAA\n",
        false,
        CreateDocumentFiller::ExtendedDNA,
        false,
        true,
        "-",
        testDir + "_common_data/scenarios/sandbox/result.fa",
        CreateDocumentFiller::FASTA,
        "result",
        false);

    GTUtilsDialog::waitForDialog(filler);

    GTMenu::clickMainMenuItem({"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished();

    QAbstractButton* complement = GTAction::button("complement_action");
    CHECK_SET_ERR(!complement->isEnabled(), "button is not disabled");

    auto toolbar = GTWidget::findWidget("views_tool_bar_result");
    GTWidget::click(GTWidget::findWidget("show_hide_details_view", toolbar));

    QAbstractButton* complement1 = GTAction::button("complement_action");
    CHECK_SET_ERR(complement1->isEnabled(), "button is not enabled");

    GTUtilsDialog::waitForDialog(new PopupChecker({"do_not_translate_radiobutton"}, PopupChecker::IsEnabled));
    GTWidget::click(GTWidget::findWidget("translationsMenuToolbarButton"));
}

GUI_TEST_CLASS_DEFINITION(test_0016) {
    //    Create a sequence from text in FASTA format (UGENE-1564): single sequence, data starts with sequence header

    //    1. Select {File -> New document from text} in the main menu.
    //    2. Input a nucleotide sequence in the dialog appeared in FASTA format:
    //    >seq_name
    //    ACGT

    GTUtilsDialog::waitForDialog(new CreateDocumentFiller(">seq_name\nACGT", false, CreateDocumentFiller::ExtendedDNA, false, true, "-", sandBoxDir + "test_0016.fa", CreateDocumentFiller::FASTA, "test_0016", false));

    //    3. Specify a created document location and press the "Create" button in the dialog.
    GTMenu::clickMainMenuItem({"File", "New document from text..."});

    GTUtilsTaskTreeView::waitTaskFinished();

    //    Expected state: the sequence has been added to the Project View, view is opened. The sequence name is the same as specified in the input text, sequence data are correct.
    GTUtilsProjectTreeView::findIndex("seq_name");
    const QString sequenceData = GTUtilsSequenceView::getSequenceAsString();
    const QString expectedSequenceData = "ACGT";
    CHECK_SET_ERR(sequenceData == expectedSequenceData, QString("Incorrect sequence data: expect '%1', got '%2'").arg(expectedSequenceData).arg(sequenceData));
}

GUI_TEST_CLASS_DEFINITION(test_0017) {
    //    Create a sequence from text in FASTA format (UGENE-1564): single sequence, data starts with a comment

    //    1. Select {File -> New document from text} in the main menu.
    //    2. Input a nucleotide sequence in the dialog appeared in FASTA format:
    //    ;just a comment
    //    >seq_name
    //    ACGT

    GTUtilsDialog::waitForDialog(new CreateDocumentFiller(";just a comment\n>seq_name\nACGT", false, CreateDocumentFiller::ExtendedDNA, false, true, "-", sandBoxDir + "test_0017.fa", CreateDocumentFiller::FASTA, "test_0017", false));

    //    3. Specify a created document location and press the "Create" button in the dialog.
    GTMenu::clickMainMenuItem({"File", "New document from text..."});

    GTUtilsTaskTreeView::waitTaskFinished();

    //    Expected state: the sequence has been added to the Project View, view is opened. The sequence name is the same as specified in the input text, sequence data are correct.
    GTUtilsProjectTreeView::findIndex("seq_name");
    const QString sequenceData = GTUtilsSequenceView::getSequenceAsString();
    const QString expectedSequenceData = "ACGT";
    CHECK_SET_ERR(sequenceData == expectedSequenceData, QString("Incorrect sequence data: expect '%1', got '%2'").arg(expectedSequenceData).arg(sequenceData));
}

GUI_TEST_CLASS_DEFINITION(test_0018) {
    //    Create a sequence from text in FASTA format (UGENE-1564): several sequences

    //    1. Select {File -> New document from text} in the main menu.
    //    2. Input nucleotide sequences in the dialog appeared in FASTA format:
    //    >seq_name1
    //    ACGT
    //    >seq_name2
    //    CCCC
    //    >seq_name3
    //    TTTT

    const QString data = ">seq_name1\nACGT\n>seq_name2\nCCCC\n>seq_name3\nTTTT";
    GTUtilsDialog::waitForDialog(new CreateDocumentFiller(data, false, CreateDocumentFiller::ExtendedDNA, false, true, "-", sandBoxDir + "test_0018.fa", CreateDocumentFiller::FASTA, "test_0018", false));

    //    3. Specify a created document location and press the "Create" button in the dialog.
    GTMenu::clickMainMenuItem({"File", "New document from text..."});

    GTUtilsTaskTreeView::waitTaskFinished();

    //    Expected state: sequences have been added to the Project View, view is opened. Sequence names are the same as specified in the input text, sequences data are correct.
    GTUtilsProjectTreeView::findIndex("seq_name1");
    GTUtilsProjectTreeView::findIndex("seq_name2");
    GTUtilsProjectTreeView::findIndex("seq_name3");

    const QString sequenceData1 = GTUtilsSequenceView::getSequenceAsString(0);
    const QString sequenceData2 = GTUtilsSequenceView::getSequenceAsString(1);
    const QString sequenceData3 = GTUtilsSequenceView::getSequenceAsString(2);

    const QString expectedSequenceData1 = "ACGT";
    const QString expectedSequenceData2 = "CCCC";
    const QString expectedSequenceData3 = "TTTT";

    CHECK_SET_ERR(sequenceData1 == expectedSequenceData1, QString("Incorrect first sequence data: expect '%1', got '%2'").arg(expectedSequenceData1).arg(sequenceData1));
    CHECK_SET_ERR(sequenceData2 == expectedSequenceData2, QString("Incorrect sequence sequence data: expect '%1', got '%2'").arg(expectedSequenceData2).arg(sequenceData2));
    CHECK_SET_ERR(sequenceData3 == expectedSequenceData3, QString("Incorrect third sequence data: expect '%1', got '%2'").arg(expectedSequenceData3).arg(sequenceData3));
}

GUI_TEST_CLASS_DEFINITION(test_0019) {
    //    Create a sequence from text in FASTA format (UGENE-1564): several sequences, one sequence is empty

    //    1. Select {File -> New document from text} in the main menu.

    class Scenario : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            //    2. Input nucleotide sequences in the dialog appeared in FASTA format:
            //    >seq_name1
            //    >seq_name2
            //    CCCC
            //    >seq_name3
            //    TTTT
            const QString data = ">seq_name1\n>seq_name2\nCCCC\n>seq_name3\nTTTT";
            GTPlainTextEdit::setText(GTWidget::findPlainTextEdit("sequenceEdit", dialog), data);

            GTLineEdit::setText(GTWidget::findLineEdit("filepathEdit", dialog), sandBoxDir + "test_0018.fa");

            //    3. Specify a created document location and press the "Create" button in the dialog.
            //    Expected state: a message box appears, dialog is not accepted.
            GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok, "Input sequence is empty"));
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(new CreateDocumentFiller(new Scenario()));
    GTMenu::clickMainMenuItem({"File", "New document from text..."});
    GTUtilsTaskTreeView::waitTaskFinished();
}

}  // namespace GUITest_common_scenarios_document_from_text
}  // namespace U2
