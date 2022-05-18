/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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

#include "GTUtilsAnnotationsTreeView.h"
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
    Runnable* filler = new CreateDocumentFiller(os,
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
    GTUtilsDialog::waitForDialog(os, filler);
    GTMenu::clickMainMenuItem(os, {"File", "New document from text..."}, GTGlobals::UseKey);

    GTUtilsDialog::checkNoActiveWaiters(os);

    GTUtilsDocument::checkDocument(os, "result");
    GTUtilsSequenceView::checkSequence(os, "ACAAGTCGGATTTATA");
}

GUI_TEST_CLASS_DEFINITION(test_0001_1) {
    Runnable* filler = new CreateDocumentFiller(os,
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

    GTUtilsDialog::waitForDialog(os, filler);

    GTMenu::clickMainMenuItem(os, {"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDocument::checkDocument(os, "result");

    GTUtilsSequenceView::checkSequence(os, "ACAAGTCGGATTTATA");
}

GUI_TEST_CLASS_DEFINITION(test_0001_2) {
    Runnable* filler = new CreateDocumentFiller(os,
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

    GTUtilsDialog::waitForDialog(os, filler);

    GTMenu::clickMainMenuItem(os, {"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDocument::checkDocument(os, "result.gb");

    GTUtilsSequenceView::checkSequence(os, "ACAAGTCGGATTTATA");
}

GUI_TEST_CLASS_DEFINITION(test_0002) {
    Runnable* filler = new CreateDocumentFiller(os,
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
    GTUtilsDialog::waitForDialog(os, filler);

    GTMenu::clickMainMenuItem(os, {"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDocument::checkDocument(os, "result.gb");

    GTUtilsSequenceView::checkSequence(os, "ACAAGTCGGATTTATA");
}

GUI_TEST_CLASS_DEFINITION(test_0002_1) {
    Runnable* filler = new CreateDocumentFiller(os,
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
    GTUtilsDialog::waitForDialog(os, filler);

    GTMenu::clickMainMenuItem(os, {"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDocument::checkDocument(os, "result.gb");

    GTUtilsSequenceView::checkSequence(os, "ACAAGTCGGATTTATAACAAGTCGGATTTATA");
}

GUI_TEST_CLASS_DEFINITION(test_0002_2) {
    Runnable* filler = new CreateDocumentFiller(os,
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
    GTUtilsDialog::waitForDialog(os, filler);

    GTMenu::clickMainMenuItem(os, {"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDocument::checkDocument(os, "result.gb");

    GTUtilsSequenceView::checkSequence(os, "ACAAGTCGGATTTATA");

    GTUtilsDocument::removeDocument(os, "result.gb");

    Runnable* filler1 = new CreateDocumentFiller(os,
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
    GTUtilsDialog::waitForDialog(os, filler1);

    GTMenu::clickMainMenuItem(os, {"File", "New document from text..."}, GTGlobals::UseKey);

    GTUtilsDocument::checkDocument(os, "result_new.gb");

    GTUtilsSequenceView::checkSequence(os, "ACAA");
}

GUI_TEST_CLASS_DEFINITION(test_0003) {
    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok));
    Runnable* filler = new CreateDocumentFiller(os,
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
    GTUtilsDialog::waitForDialog(os, filler);

    GTMenu::clickMainMenuItem(os, {"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDocument::checkDocument(os, "result");

    GTUtilsSequenceView::checkSequence(os, "FKMDNYTRVEPPG,DD.JFUYBVYERHGK");
}

GUI_TEST_CLASS_DEFINITION(test_0003_1) {
    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok));
    Runnable* filler = new CreateDocumentFiller(os,
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
    GTUtilsDialog::waitForDialog(os, filler);

    GTMenu::clickMainMenuItem(os, {"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDocument::checkDocument(os, "result.gb");

    GTUtilsSequenceView::checkSequence(os, "FKMDNYTRVEPPG,DD.JFUYBVYERHGK");
}

GUI_TEST_CLASS_DEFINITION(test_0003_2) {
    Runnable* filler = new CreateDocumentFiller(os,
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
    GTUtilsDialog::waitForDialog(os, filler);

    GTMenu::clickMainMenuItem(os, {"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDocument::checkDocument(os, "result");

    GTUtilsSequenceView::checkSequence(os, "ACGT");
}

GUI_TEST_CLASS_DEFINITION(test_0004) {
    Runnable* filler = new CreateDocumentFiller(os,
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
    GTUtilsDialog::waitForDialog(os, filler);

    GTMenu::clickMainMenuItem(os, {"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDocument::checkDocument(os, "result");

    GTUtilsSequenceView::checkSequence(os, "NACCGTTAAAGCCAGT");
}

GUI_TEST_CLASS_DEFINITION(test_0004_1) {
    Runnable* filler = new CreateDocumentFiller(os,
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
    GTUtilsDialog::waitForDialog(os, filler);

    GTMenu::clickMainMenuItem(os, {"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDocument::checkDocument(os, "result.gb");

    GTUtilsSequenceView::checkSequence(os, "NACCGTTAAAGCCAGT");
}

GUI_TEST_CLASS_DEFINITION(test_0004_2) {
    Runnable* filler = new CreateDocumentFiller(os,
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
    GTUtilsDialog::waitForDialog(os, filler);

    GTMenu::clickMainMenuItem(os, {"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDocument::checkDocument(os, "result");

    GTUtilsSequenceView::checkSequence(os, "NACCGAAUAGCCAG");
}

GUI_TEST_CLASS_DEFINITION(test_0005) {
    Runnable* filler = new CreateDocumentFiller(os,
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
    GTUtilsDialog::waitForDialog(os, filler);

    GTMenu::clickMainMenuItem(os, {"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDocument::checkDocument(os, "result");

    GTUtilsSequenceView::checkSequence(os, "-NACCGTTAA---AGCC-----AGT--");
}

GUI_TEST_CLASS_DEFINITION(test_0005_1) {
    Runnable* filler = new CreateDocumentFiller(os,
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
    GTUtilsDialog::waitForDialog(os, filler);

    GTMenu::clickMainMenuItem(os, {"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDocument::checkDocument(os, "result.gb");

    GTUtilsSequenceView::checkSequence(os, "-NACCGTTAA---AGCC-----AGT--");
}

GUI_TEST_CLASS_DEFINITION(test_0005_2) {
    Runnable* filler = new CreateDocumentFiller(os,
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
    GTUtilsDialog::waitForDialog(os, filler);

    GTMenu::clickMainMenuItem(os, {"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDocument::checkDocument(os, "result");

    GTUtilsSequenceView::checkSequence(os, "ANACCGAAAAAAUAGCCAAAAAAGAAA");
}
GUI_TEST_CLASS_DEFINITION(test_0006) {
    Runnable* filler = new CreateDocumentFiller(os,
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
    GTUtilsDialog::waitForDialog(os, filler);

    GTMenu::clickMainMenuItem(os, {"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDocument::checkDocument(os, "result");

    GTUtilsSequenceView::checkSequence(os, "RNACCGTTAAAGCCDMAGT");
}

GUI_TEST_CLASS_DEFINITION(test_0006_1) {
    Runnable* filler = new CreateDocumentFiller(os,
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
    GTUtilsDialog::waitForDialog(os, filler);

    GTMenu::clickMainMenuItem(os, {"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDocument::checkDocument(os, "result.gb");

    GTUtilsSequenceView::checkSequence(os, "RNACCGTTAAAGCCDMAGT");
}

GUI_TEST_CLASS_DEFINITION(test_0006_2) {
    Runnable* filler = new CreateDocumentFiller(os,
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
    GTUtilsDialog::waitForDialog(os, filler);

    GTMenu::clickMainMenuItem(os, {"File", "New document from text..."}, GTGlobals::UseKey);

    GTUtilsDocument::checkDocument(os, "result");

    GTUtilsSequenceView::checkSequence(os, "RNACCGTTAAA---GCCDMAGT");
}

GUI_TEST_CLASS_DEFINITION(test_0007) {
    Runnable* filler = new CreateDocumentFiller(os,
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
    GTUtilsDialog::waitForDialog(os, filler);

    GTMenu::clickMainMenuItem(os, {"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDocument::checkDocument(os, "result");

    GTUtilsSequenceView::checkSequence(os, "NACCGAAUAGCCAG");
}

GUI_TEST_CLASS_DEFINITION(test_0007_1) {
    Runnable* filler = new CreateDocumentFiller(os,
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
    GTUtilsDialog::waitForDialog(os, filler);

    GTMenu::clickMainMenuItem(os, {"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDocument::checkDocument(os, "result.gb");

    GTUtilsSequenceView::checkSequence(os, "NACCGAAUAGCCAG");
}

GUI_TEST_CLASS_DEFINITION(test_0007_2) {
    Runnable* filler = new CreateDocumentFiller(os,
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
    GTUtilsDialog::waitForDialog(os, filler);

    GTMenu::clickMainMenuItem(os, {"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDocument::checkDocument(os, "result");

    GTUtilsSequenceView::checkSequence(os, "NACCGAAU---AGCCAG");
}

GUI_TEST_CLASS_DEFINITION(test_0008) {
    Runnable* filler = new CreateDocumentFiller(os,
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
    GTUtilsDialog::waitForDialog(os, filler);
    GTMenu::clickMainMenuItem(os, {"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDocument::checkDocument(os, "result");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    GTUtilsSequenceView::checkSequence(os, "RNACCGAAUAGCCDMAG");
}

GUI_TEST_CLASS_DEFINITION(test_0008_1) {
    Runnable* filler = new CreateDocumentFiller(os,
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
    GTUtilsDialog::waitForDialog(os, filler);

    GTMenu::clickMainMenuItem(os, {"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDocument::checkDocument(os, "result.gb");

    GTUtilsSequenceView::checkSequence(os, "RNACCGAAUAGCCDMAG");
}

GUI_TEST_CLASS_DEFINITION(test_0008_2) {
    Runnable* filler = new CreateDocumentFiller(os,
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
    GTUtilsDialog::waitForDialog(os, filler);

    GTMenu::clickMainMenuItem(os, {"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDocument::checkDocument(os, "result");

    GTUtilsSequenceView::checkSequence(os, "RNACCGAAU---AGCCDMAG");
}

GUI_TEST_CLASS_DEFINITION(test_0009) {
    Runnable* filler = new CreateDocumentFiller(os,
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
    GTUtilsDialog::waitForDialog(os, filler);

    GTMenu::clickMainMenuItem(os, {"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDocument::checkDocument(os, "result");

    GTUtilsSequenceView::checkSequence(os, "RNACCGTTAA---AGCCD---MAGT--");
}

GUI_TEST_CLASS_DEFINITION(test_0009_1) {
    Runnable* filler = new CreateDocumentFiller(os,
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
    GTUtilsDialog::waitForDialog(os, filler);

    GTMenu::clickMainMenuItem(os, {"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDocument::checkDocument(os, "result.gb");

    GTUtilsSequenceView::checkSequence(os, "RNACCGTTAA---AGCCD---MAGT--");
}

GUI_TEST_CLASS_DEFINITION(test_0009_2) {
    Runnable* filler = new CreateDocumentFiller(os,
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
    GTUtilsDialog::waitForDialog(os, filler);

    GTMenu::clickMainMenuItem(os, {"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDocument::checkDocument(os, "result");

    GTUtilsSequenceView::checkSequence(os, "RNACCGT---TAA---AGCCD---MAGT--");
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
    Runnable* filler = new CreateDocumentFiller(os,
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
    GTUtilsDialog::waitForDialog(os, filler);

    GTMenu::clickMainMenuItem(os, {"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //  3. Click Create button
    //  Expected result:
    //   - result.fa document added to project
    //   - sequence view opened
    //   - sequence are RNACCG--AA--UAGCCD---MAG---

    GTUtilsDocument::checkDocument(os, "result");

    GTUtilsSequenceView::checkSequence(os, "RNACCG--AA--UAGCCD---MAG---");
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
    Runnable* filler = new CreateDocumentFiller(os,
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
    GTUtilsDialog::waitForDialog(os, filler);

    GTMenu::clickMainMenuItem(os, {"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //  3. Click Create button
    //  Expected result:
    //   - result.fa document added to project
    //   - sequence view opened
    //   - sequence are RNACCG--AA--UAGCCD---MAG---

    GTUtilsDocument::checkDocument(os, "result.gb");

    GTUtilsSequenceView::checkSequence(os, "RNACCG--AA--UAGCCD---MAG---");
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
    Runnable* filler = new CreateDocumentFiller(os,
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
    GTUtilsDialog::waitForDialog(os, filler);

    GTMenu::clickMainMenuItem(os, {"File", "New document from text..."}, GTGlobals::UseMouse);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //  3. Click Create button
    //  Expected result:
    //   - result.fa document added to project
    //   - sequence view opened
    //   - sequence are RNACCG--AA--UAGCCD---MAG---

    GTUtilsDocument::checkDocument(os, "result");

    GTUtilsSequenceView::checkSequence(os, "RNACCG--AA--UAGCCD---MAG---");
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
    Runnable* filler = new CreateDocumentFiller(os,
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
    GTUtilsDialog::waitForDialog(os, filler);

    GTMenu::clickMainMenuItem(os, {"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //  3. Click Create button
    //  Expected result:
    //  - result.fa document added to project
    //  - sequence view opened
    //  - sequence are -NACCG--AA--UAGCC-----AG---

    GTUtilsDocument::checkDocument(os, "result");

    GTUtilsSequenceView::checkSequence(os, "-NACCG--AA--UAGCC-----AG---");
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
    Runnable* filler = new CreateDocumentFiller(os,
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
    GTUtilsDialog::waitForDialog(os, filler);

    GTMenu::clickMainMenuItem(os, {"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //  3. Click Create button
    //  Expected result:
    //  - result.fa document added to project
    //  - sequence view opened
    //  - sequence are -NACCG--AA--UAGCC-----AG---

    GTUtilsDocument::checkDocument(os, "result.gb");

    GTUtilsSequenceView::checkSequence(os, "-NACCG--AA--UAGCC-----AG---");
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
    //  {Save file immidiately} set checked
    Runnable* filler = new CreateDocumentFiller(os,
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
    GTUtilsDialog::waitForDialog(os, filler);

    GTMenu::clickMainMenuItem(os, {"File", "New document from text..."}, GTGlobals::UseMouse);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //  3. Click Create button
    //  Expected result:
    //  - result.fa document added to project
    //  - sequence view opened
    //  - sequence are -NACCG--AA--UAGCC-----AG---

    GTUtilsDocument::checkDocument(os, "result");

    GTUtilsSequenceView::checkSequence(os, "-NACCG--AA--UAGCC-----AG---");
}

GUI_TEST_CLASS_DEFINITION(test_0012) {
    Runnable* filler = new CreateDocumentFiller(os,
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
    GTUtilsDialog::waitForDialog(os, filler);

    GTMenu::clickMainMenuItem(os, {"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDocument::checkDocument(os, "result.fa");

    GTUtilsDialog::waitForDialog(os, new SaveProjectDialogFiller(os, QDialogButtonBox::No));
    GTMenu::clickMainMenuItem(os, {"File", "Close project"});

    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/sandbox/", "result.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
}

GUI_TEST_CLASS_DEFINITION(test_0012_1) {
    Runnable* filler = new CreateDocumentFiller(os,
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
    GTUtilsDialog::waitForDialog(os, filler);

    GTMenu::clickMainMenuItem(os, {"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDocument::checkDocument(os, "result.gb");

    GTUtilsDialog::waitForDialog(os, new SaveProjectDialogFiller(os, QDialogButtonBox::No));
    GTMenu::clickMainMenuItem(os, {"File", "Close project"});

    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/sandbox/", "result.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
}

GUI_TEST_CLASS_DEFINITION(test_0012_2) {
    Runnable* filler = new CreateDocumentFiller(os,
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
    GTUtilsDialog::waitForDialog(os, filler);

    GTMenu::clickMainMenuItem(os, {"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDocument::checkDocument(os, "result.fa");

    GTUtilsDialog::waitForDialog(os, new SaveProjectDialogFiller(os, QDialogButtonBox::No));
    GTMenu::clickMainMenuItem(os, {"File", "Close project"});

    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/sandbox/", "result.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
}

GUI_TEST_CLASS_DEFINITION(test_0013) {
    Runnable* filler = new CreateDocumentFiller(os,
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

    GTUtilsDialog::waitForDialog(os, filler);

    GTMenu::clickMainMenuItem(os, {"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsDocument::checkDocument(os, "result.fa");

    GTUtilsProjectTreeView::rename(os, "result", "result_new");

    GTUtilsProjectTreeView::findIndex(os, "result_new");  // checks inside
}
GUI_TEST_CLASS_DEFINITION(test_0013_1) {
    Runnable* filler = new CreateDocumentFiller(os,
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

    GTUtilsDialog::waitForDialog(os, filler);

    GTMenu::clickMainMenuItem(os, {"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsDocument::checkDocument(os, "result.gb");

    GTUtilsProjectTreeView::rename(os, "result", "result_new");

    GTUtilsProjectTreeView::findIndex(os, "result_new");  // checks inside
}
GUI_TEST_CLASS_DEFINITION(test_0013_2) {
    Runnable* filler = new CreateDocumentFiller(os,
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

    GTUtilsDialog::waitForDialog(os, filler);

    GTMenu::clickMainMenuItem(os, {"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsDocument::checkDocument(os, "result.fa");

    GTUtilsProjectTreeView::rename(os, "result", "result_new");

    GTUtilsProjectTreeView::findIndex(os, "result_new");  // checks inside
}

GUI_TEST_CLASS_DEFINITION(test_0014) {
    Runnable* filler = new CancelCreateDocumentFiller(os,
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

    GTUtilsDialog::waitForDialog(os, filler);

    GTMenu::clickMainMenuItem(os, {"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsProject::checkProject(os, GTUtilsProject::NotExists);
}

GUI_TEST_CLASS_DEFINITION(test_0014_1) {
    Runnable* filler = new CancelCreateDocumentFiller(os,
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

    GTUtilsDialog::waitForDialog(os, filler);

    GTMenu::clickMainMenuItem(os, {"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsProject::checkProject(os, GTUtilsProject::NotExists);
}

GUI_TEST_CLASS_DEFINITION(test_0014_2) {
    Runnable* filler = new CreateDocumentFiller(os,
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

    GTUtilsDialog::waitForDialog(os, filler);

    GTMenu::clickMainMenuItem(os, {"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsProject::checkProject(os, GTUtilsProject::Exists);
}

GUI_TEST_CLASS_DEFINITION(test_0015) {
    Runnable* filler = new CreateDocumentFiller(os,
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

    GTUtilsDialog::waitForDialog(os, filler);

    GTMenu::clickMainMenuItem(os, {"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QAbstractButton* complement = GTAction::button(os, "complement_action");
    CHECK_SET_ERR(!complement->isEnabled(), "button is not disabled");

    auto toolbar = GTWidget::findWidget(os, "views_tool_bar_result");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_details_view", toolbar));

    QAbstractButton* complement1 = GTAction::button(os, "complement_action");
    CHECK_SET_ERR(complement1->isEnabled(), "button is not enabled");

    GTUtilsDialog::waitForDialog(os, new PopupChecker(os, {"do_not_translate_radiobutton"}, PopupChecker::IsEnabled));
    GTWidget::click(os, GTWidget::findWidget(os, "translationsMenuToolbarButton"));
}

GUI_TEST_CLASS_DEFINITION(test_0015_1) {
    Runnable* filler = new CreateDocumentFiller(os,
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

    GTUtilsDialog::waitForDialog(os, filler);

    GTMenu::clickMainMenuItem(os, {"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QAbstractButton* complement = GTAction::button(os, "complement_action");
    CHECK_SET_ERR(!complement->isEnabled(), "button is not disabled");

    auto toolbar = GTWidget::findWidget(os, "views_tool_bar_result");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_details_view", toolbar));

    QAbstractButton* complement1 = GTAction::button(os, "complement_action");
    CHECK_SET_ERR(complement1->isEnabled(), "button is not enabled");

    GTUtilsDialog::waitForDialog(os, new PopupChecker(os, {"do_not_translate_radiobutton"}, PopupChecker::IsEnabled));
    GTWidget::click(os, GTWidget::findWidget(os, "translationsMenuToolbarButton"));
}

GUI_TEST_CLASS_DEFINITION(test_0015_2) {
    Runnable* filler = new CreateDocumentFiller(os,
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

    GTUtilsDialog::waitForDialog(os, filler);

    GTMenu::clickMainMenuItem(os, {"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QAbstractButton* complement = GTAction::button(os, "complement_action");
    CHECK_SET_ERR(!complement->isEnabled(), "button is not disabled");

    auto toolbar = GTWidget::findWidget(os, "views_tool_bar_result");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_details_view", toolbar));

    QAbstractButton* complement1 = GTAction::button(os, "complement_action");
    CHECK_SET_ERR(complement1->isEnabled(), "button is not enabled");

    GTUtilsDialog::waitForDialog(os, new PopupChecker(os, {"do_not_translate_radiobutton"}, PopupChecker::IsEnabled));
    GTWidget::click(os, GTWidget::findWidget(os, "translationsMenuToolbarButton"));
}

GUI_TEST_CLASS_DEFINITION(test_0016) {
    //    Create a sequence from text in FASTA format (UGENE-1564): single sequence, data starts with sequence header

    //    1. Select {File -> New document from text} in the main menu.
    //    2. Input a nucleotide sequence in the dialog appeared in FASTA format:
    //    >seq_name
    //    ACGT

    GTUtilsDialog::waitForDialog(os, new CreateDocumentFiller(os, ">seq_name\nACGT", false, CreateDocumentFiller::ExtendedDNA, false, true, "-", sandBoxDir + "test_0016.fa", CreateDocumentFiller::FASTA, "test_0016", false));

    //    3. Specify a created document location and press the "Create" button in the dialog.
    GTMenu::clickMainMenuItem(os, {"File", "New document from text..."});

    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: the sequence has been added to the Project View, view is opened. The sequence name is the same as specified in the input text, sequence data are correct.
    GTUtilsProjectTreeView::findIndex(os, "seq_name");
    const QString sequenceData = GTUtilsSequenceView::getSequenceAsString(os);
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

    GTUtilsDialog::waitForDialog(os, new CreateDocumentFiller(os, ";just a comment\n>seq_name\nACGT", false, CreateDocumentFiller::ExtendedDNA, false, true, "-", sandBoxDir + "test_0017.fa", CreateDocumentFiller::FASTA, "test_0017", false));

    //    3. Specify a created document location and press the "Create" button in the dialog.
    GTMenu::clickMainMenuItem(os, {"File", "New document from text..."});

    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: the sequence has been added to the Project View, view is opened. The sequence name is the same as specified in the input text, sequence data are correct.
    GTUtilsProjectTreeView::findIndex(os, "seq_name");
    const QString sequenceData = GTUtilsSequenceView::getSequenceAsString(os);
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
    GTUtilsDialog::waitForDialog(os, new CreateDocumentFiller(os, data, false, CreateDocumentFiller::ExtendedDNA, false, true, "-", sandBoxDir + "test_0018.fa", CreateDocumentFiller::FASTA, "test_0018", false));

    //    3. Specify a created document location and press the "Create" button in the dialog.
    GTMenu::clickMainMenuItem(os, {"File", "New document from text..."});

    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: sequences have been added to the Project View, view is opened. Sequence names are the same as specified in the input text, sequences data are correct.
    GTUtilsProjectTreeView::findIndex(os, "seq_name1");
    GTUtilsProjectTreeView::findIndex(os, "seq_name2");
    GTUtilsProjectTreeView::findIndex(os, "seq_name3");

    const QString sequenceData1 = GTUtilsSequenceView::getSequenceAsString(os, 0);
    const QString sequenceData2 = GTUtilsSequenceView::getSequenceAsString(os, 1);
    const QString sequenceData3 = GTUtilsSequenceView::getSequenceAsString(os, 2);

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
        void run(HI::GUITestOpStatus& os) override {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);
            //    2. Input nucleotide sequences in the dialog appeared in FASTA format:
            //    >seq_name1
            //    >seq_name2
            //    CCCC
            //    >seq_name3
            //    TTTT
            const QString data = ">seq_name1\n>seq_name2\nCCCC\n>seq_name3\nTTTT";
            GTPlainTextEdit::setPlainText(os, GTWidget::findPlainTextEdit(os, "sequenceEdit", dialog), data);

            GTLineEdit::setText(os, GTWidget::findLineEdit(os, "filepathEdit", dialog), sandBoxDir + "test_0018.fa");

            //    3. Specify a created document location and press the "Create" button in the dialog.
            //    Expected state: a message box appears, dialog is not accepted.
            GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok, "Input sequence is empty"));
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(os, new CreateDocumentFiller(os, new Scenario()));
    GTMenu::clickMainMenuItem(os, {"File", "New document from text..."});
    GTUtilsTaskTreeView::waitTaskFinished(os);
}

}  // namespace GUITest_common_scenarios_document_from_text
}  // namespace U2
