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

#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTLineEdit.h>
#include <utils/GTUtilsDialog.h>

#include <QApplication>
#include <QDir>

#include "GTDatabaseConfig.h"
#include "GTTestsPrimerLibrary.h"
#include "GTUtilsAnnotationsTreeView.h"
#include "GTUtilsLog.h"
#include "GTUtilsMdi.h"
#include "GTUtilsMeltingTemperature.h"
#include "GTUtilsOptionPanelSequenceView.h"
#include "GTUtilsPcr.h"
#include "GTUtilsPrimerLibrary.h"
#include "GTUtilsProject.h"
#include "GTUtilsSequenceView.h"
#include "GTUtilsTaskTreeView.h"
#include "runnables/ugene/corelibs/U2View/temperature/MeltingTemperatureSettingsDialogFiller.h"
#include "runnables/ugene/plugins/pcr/AddPrimerDialogFiller.h"
#include "runnables/ugene/plugins/pcr/ExportPrimersDialogFiller.h"
#include "runnables/ugene/plugins/pcr/ImportPrimersDialogFiller.h"
#include "runnables/ugene/plugins/pcr/PrimerLibrarySelectorFiller.h"

namespace U2 {
namespace GUITest_common_scenarios_primer_library {
using namespace HI;
GUI_TEST_CLASS_DEFINITION(test_0001) {
    GTUtilsMdi::click(GTGlobals::Close);
    // The library is the singleton MDI window

    // 1. Click the menu Tools -> Primer -> Primer Library.
    // Expected: the library MDI window is opened.
    QWidget* libraryMdi1 = GTUtilsPrimerLibrary::openLibrary();

    // 2. Click the menu again.
    QWidget* libraryMdi2 = GTUtilsPrimerLibrary::openLibrary();

    // Expected: the same MDI windows is opened (not the second one).
    CHECK_SET_ERR(libraryMdi1 == libraryMdi2, "Different MDI windows");

    // 3. Click the close button.
    GTUtilsPrimerLibrary::clickButton(GTUtilsPrimerLibrary::Close);
    // Expected: The window is closed.
    QWidget* libraryMdi3 = GTUtilsMdi::activeWindow({false});
    CHECK_SET_ERR(libraryMdi3 == nullptr, "Library MDI is not closed");
}

GUI_TEST_CLASS_DEFINITION(test_0002) {
    // Add new primer:
    //     Primer line edit ACGT content
    //     Availability of the OK button

    // 1. Click the menu Tools -> Primer -> Primer Library.
    GTUtilsPrimerLibrary::openLibrary();
    int librarySize = GTUtilsPrimerLibrary::librarySize();

    // 2. Click the new primer button.
    // Expected: the dialog appears. The OK button is disabled.
    class Scenario : public CustomScenario {
    public:
        void run() {
            // 3. Set the focus at the primer line edit and write "Q%1" (not ACGT).
            auto primerEdit = dynamic_cast<QLineEdit*>(GTWidget::findWidget("primerEdit"));
            GTLineEdit::setText(primerEdit, "Q%1", true);

            // Expected: the line edit is empty.
            CHECK_SET_ERR(primerEdit->text().isEmpty(), "Wrong input");

            // 4. Write "atcg".
            GTLineEdit::setText(primerEdit, "atcg", true);

            // Expected: the line edit content is "ATCG". The OK button is enabled.
            CHECK_SET_ERR(primerEdit->text() == "ATCG", "No upper-case");

            // 5. Remove the primer name.
            auto nameEdit = GTWidget::findLineEdit("nameEdit");
            GTLineEdit::setText(nameEdit, "");

            // Expected: The OK button is disabled.
            QWidget* dialog = GTWidget::getActiveModalWidget();
            QPushButton* okButton = GTUtilsDialog::buttonBox(dialog)->button(QDialogButtonBox::Ok);
            CHECK_SET_ERR(!okButton->isEnabled(), "The OK button is enabled");

            // 6. Set the name "Primer".
            GTLineEdit::setText(nameEdit, "Primer");

            // 7. Click the OK button.
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };
    AddPrimerDialogFiller::Parameters parameters;
    parameters.scenario = new Scenario();
    GTUtilsDialog::waitForDialog(new AddPrimerDialogFiller(parameters));
    GTUtilsPrimerLibrary::clickButton(GTUtilsPrimerLibrary::Add);

    // Expected: the new primer appears in the table.
    CHECK_SET_ERR(librarySize + 1 == GTUtilsPrimerLibrary::librarySize(), "Wrong primers count");
    CHECK_SET_ERR(GTUtilsPrimerLibrary::getPrimerSequence(librarySize) == "ATCG", "Wrong primer");
}

GUI_TEST_CLASS_DEFINITION(test_0003) {
    // Remove primers:
    //     Availability of the button

    // 1. Click the menu Tools -> Primer -> Primer Library.
    GTUtilsPrimerLibrary::openLibrary();
    int librarySize = GTUtilsPrimerLibrary::librarySize();

    // 2. Add a new primer if the library is empty.
    AddPrimerDialogFiller::Parameters parameters;
    parameters.primer = "AAAAAAAAAAAAAA";
    GTUtilsDialog::waitForDialog(new AddPrimerDialogFiller(parameters));
    GTUtilsPrimerLibrary::clickButton(GTUtilsPrimerLibrary::Add);

    // 3. Click the empty place of the table.
    QPoint emptyPoint = GTUtilsPrimerLibrary::getPrimerPoint(librarySize);
    emptyPoint.setY(emptyPoint.y() + 40);
    GTMouseDriver::moveTo(emptyPoint);
    GTMouseDriver::click();

    // Expected: The remove button is disabled.
    QAbstractButton* removeButton = GTUtilsPrimerLibrary::getButton(GTUtilsPrimerLibrary::Remove);
    CHECK_SET_ERR(!removeButton->isEnabled(), "The remove button is enabled");

    // 4. Select the primer.
    GTMouseDriver::moveTo(GTUtilsPrimerLibrary::getPrimerPoint(librarySize));
    GTMouseDriver::click();

    // Expected: The remove button is enabled.
    CHECK_SET_ERR(removeButton->isEnabled(), "The remove button is disabled");

    // 5. Click the button.
    GTUtilsPrimerLibrary::clickButton(GTUtilsPrimerLibrary::Remove);

    // Expected: the primer is disappeared from the table.
    CHECK_SET_ERR(librarySize == GTUtilsPrimerLibrary::librarySize(), "Wrong primers count");
}

GUI_TEST_CLASS_DEFINITION(test_0004) {
    // In silico PCR with the library data:
    //     Add a primer from the library
    //     Double click

    {  // Pre-test
        GTUtilsPrimerLibrary::openLibrary();
        AddPrimerDialogFiller::Parameters parameters;
        parameters.primer = "AAAAAAAAAAAAAA";
        GTUtilsDialog::waitForDialog(new AddPrimerDialogFiller(parameters));
        GTUtilsPrimerLibrary::clickButton(GTUtilsPrimerLibrary::Add);
        GTUtilsPrimerLibrary::clickButton(GTUtilsPrimerLibrary::Close);
    }

    // 1. Open "_common_data/fasta/pcr_test.fa".
    GTFileDialog::openFile(testDir + "_common_data/fasta", "pcr_test.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Open the PCR OP.
    GTWidget::click(GTWidget::findWidget("OP_IN_SILICO_PCR"));

    // 3. Click the browse library button for the forward primer.
    // Expected: the library dialog appears, the OK button is disabled.
    // 4. Click a primer in the table.
    // Expected: the OK button is enabled.
    // 5. Double click the primer.
    GTUtilsDialog::waitForDialog(new PrimerLibrarySelectorFiller(-1, true));
    GTWidget::click(GTUtilsPcr::browseButton(U2Strand::Direct));

    // Expected: the dialog is closed, the chosen primer sequence is in the forward primer line edit.
    auto primerEdit = GTWidget::findLineEdit("primerEdit", GTUtilsPcr::primerBox(U2Strand::Direct));
    CHECK_SET_ERR(primerEdit->text() == "AAAAAAAAAAAAAA", "Wrong primer");
}

GUI_TEST_CLASS_DEFINITION(test_0005) {
    // Edit primer:
    //     Availability of the button

    // 1. Click the menu Tools -> Primer -> Primer Library.
    GTUtilsPrimerLibrary::openLibrary();

    // 2. Add a new primer if the library is empty.
    for (int i = 0; i < 3; i++) {
        AddPrimerDialogFiller::Parameters parameters;
        parameters.primer = "AAAAAAAAAAAAAA";
        GTUtilsDialog::waitForDialog(new AddPrimerDialogFiller(parameters));
        GTUtilsPrimerLibrary::clickButton(GTUtilsPrimerLibrary::Add);
    }
    int lastPrimer = GTUtilsPrimerLibrary::librarySize() - 1;

    // 3. Click the empty place of the table.
    QPoint emptyPoint = GTUtilsPrimerLibrary::getPrimerPoint(lastPrimer);
    emptyPoint.setY(emptyPoint.y() + 40);
    GTMouseDriver::moveTo(emptyPoint);
    GTMouseDriver::click();

    // Expected: The edit button is disabled.
    QAbstractButton* editButton = GTUtilsPrimerLibrary::getButton(GTUtilsPrimerLibrary::Edit);
    CHECK_SET_ERR(!editButton->isEnabled(), "The remove button is enabled");

    // 4. Select several primers.
    GTMouseDriver::moveTo(GTUtilsPrimerLibrary::getPrimerPoint(lastPrimer));
    GTMouseDriver::click();
    GTMouseDriver::moveTo(GTUtilsPrimerLibrary::getPrimerPoint(lastPrimer - 2));
    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    GTMouseDriver::click();
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);

    // Expected: The edit button is disabled.
    CHECK_SET_ERR(!editButton->isEnabled(), "The remove button is enabled");

    // 5. Select the primer P.
    GTMouseDriver::moveTo(GTUtilsPrimerLibrary::getPrimerPoint(lastPrimer));
    GTMouseDriver::click();

    // Expected: The edit button is enabled.
    CHECK_SET_ERR(editButton->isEnabled(), "The remove button is disabled");

    // 6. Double click the primer P.
    // Expected: the dialog appears. The P's data is written.
    // 7. Edit primer and name and click OK.
    AddPrimerDialogFiller::Parameters parameters;
    parameters.primer = "CCCCCCCCCCCCCC";
    parameters.name = "test_0005";
    GTUtilsDialog::waitForDialog(new AddPrimerDialogFiller(parameters));
    GTMouseDriver::doubleClick();

    // Expected: the primer is changed in the table.
    CHECK_SET_ERR("CCCCCCCCCCCCCC" == GTUtilsPrimerLibrary::getPrimerSequence(lastPrimer), "The sequence is not changed");
}

GUI_TEST_CLASS_DEFINITION(test_0006) {
    //    Export whole library to the fasta file

    //    1. Open the library, clear it, add sequences "AAAA", "CCCC", "GGGG", "TTTT".
    GTUtilsPrimerLibrary::openLibrary();
    GTUtilsPrimerLibrary::clearLibrary();

    GTUtilsPrimerLibrary::addPrimer("primer1", "AAAA");
    GTUtilsPrimerLibrary::addPrimer("primer2", "CCCC");
    GTUtilsPrimerLibrary::addPrimer("primer3", "GGGG");
    GTUtilsPrimerLibrary::addPrimer("primer4", "TTTT");

    //    2. Select all sequences.
    GTUtilsPrimerLibrary::selectAll();

    //    3. Click "Export".
    //    4. Fill the dialog:
    //        Export to: "Local file";
    //        Format: "fasta";
    //        File path: any valid path;
    //    and accept the dialog.
    class ExportToFastaScenario : public CustomScenario {
        void run() override {
            ExportPrimersDialogFiller::setFormat("FASTA");
            ExportPrimersDialogFiller::setFilePath(sandBoxDir + "pcrlib/test_0006/primers.fa");
            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Ok);
        }
    };

    QDir().mkpath(sandBoxDir + "pcrlib/test_0006");
    GTUtilsDialog::waitForDialog(new ExportPrimersDialogFiller(new ExportToFastaScenario));
    GTUtilsPrimerLibrary::clickButton(GTUtilsPrimerLibrary::Export);

    //    5. Open the exported file.
    //    Expected state: there are the same sequences in the file as in the library.
    GTUtilsTaskTreeView::waitTaskFinished();
    const QStringList names = {"primer1", "primer2", "primer3", "primer4"};
    GTUtilsProject::openFileExpectSequences(sandBoxDir + "pcrlib/test_0006/", "primers.fa", names);

    const QString firstSeq = GTUtilsSequenceView::getSequenceAsString(0);
    CHECK_SET_ERR("AAAA" == firstSeq, QString("Incorrect sequence data: expect '%1', got '%2'").arg("AAAA").arg(firstSeq));

    const QString secondSeq = GTUtilsSequenceView::getSequenceAsString(1);
    CHECK_SET_ERR("CCCC" == secondSeq, QString("Incorrect sequence data: expect '%1', got '%2'").arg("CCCC").arg(secondSeq));

    const QString thirdSeq = GTUtilsSequenceView::getSequenceAsString(2);
    CHECK_SET_ERR("GGGG" == thirdSeq, QString("Incorrect sequence data: expect '%1', got '%2'").arg("GGGG").arg(thirdSeq));

    const QString fourthSeq = GTUtilsSequenceView::getSequenceAsString(3);
    CHECK_SET_ERR("TTTT" == fourthSeq, QString("Incorrect sequence data: expect '%1', got '%2'").arg("TTTT").arg(fourthSeq));
}

GUI_TEST_CLASS_DEFINITION(test_0007) {
    //    Export several primers from library to the genbank file

    //    1. Open the library, clear it, add sequences "AAAA", "CCCC", "GGGG", "TTTT".
    GTUtilsPrimerLibrary::openLibrary();
    GTUtilsPrimerLibrary::clearLibrary();
    QMap<GTUtilsMeltingTemperature::Parameter, QString> parameters = {
        {GTUtilsMeltingTemperature::Parameter::Algorithm, "Rough"}};
    GTUtilsDialog::waitForDialog(new MeltingTemperatureSettingsDialogFiller(parameters));
    GTUtilsPrimerLibrary::clickButton(GTUtilsPrimerLibrary::Button::Temperature);

    GTUtilsPrimerLibrary::addPrimer("primer1", "AAAA");
    GTUtilsPrimerLibrary::addPrimer("primer2", "CCCC");
    GTUtilsPrimerLibrary::addPrimer("primer3", "GGGG");
    GTUtilsPrimerLibrary::addPrimer("primer4", "TTTT");

    //    2. Select the second and the third sequences.
    GTUtilsPrimerLibrary::selectPrimers(QList<int>() << 0 << 2);

    //    3. Click "Export".
    //    4. Fill the dialog:
    //        Export to: "Local file";
    //        Format: "GenBank";
    //        File path: any valid path;
    //    and accept the dialog.
    class ExportToGenbankScenario : public CustomScenario {
        void run() override {
            ExportPrimersDialogFiller::setFormat("GenBank");
            ExportPrimersDialogFiller::setFilePath(sandBoxDir + "pcrlib/test_0007/primers.gb");
            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Ok);
        }
    };

    QDir().mkpath(sandBoxDir + "pcrlib/test_0007");
    GTUtilsDialog::waitForDialog(new ExportPrimersDialogFiller(new ExportToGenbankScenario));
    GTUtilsPrimerLibrary::clickButton(GTUtilsPrimerLibrary::Export);

    //    5. Open the exported file.
    //    Expected state: there are two sequences (primer1: AAAA, primer3: GGGG) with annotations, that contains qualifiers with primer's parameters.
    GTUtilsTaskTreeView::waitTaskFinished();
    const QStringList names = {"primer1", "primer3"};
    GTUtilsProject::openFileExpectSequences(sandBoxDir + "pcrlib/test_0007/", "primers.gb", names);

    const QString firstSeq = GTUtilsSequenceView::getSequenceAsString(0);
    CHECK_SET_ERR("AAAA" == firstSeq, QString("Incorrect sequence data: expect '%1', got '%2'").arg("AAAA").arg(firstSeq));

    const QString secondSeq = GTUtilsSequenceView::getSequenceAsString(1);
    CHECK_SET_ERR("GGGG" == secondSeq, QString("Incorrect sequence data: expect '%1', got '%2'").arg("GGGG").arg(secondSeq));

    const QList<QTreeWidgetItem*> items = GTUtilsAnnotationsTreeView::findItems("primer_bind");
    CHECK_SET_ERR(items.size() == 2, QString("Unexpected annotations count: epxect %1, got %2").arg(2).arg(items.size()));

    GTUtilsAnnotationsTreeView::selectItemsByName({"primer_bind"});
    const QString sequenceQualifier = GTUtilsAnnotationsTreeView::getQualifierValue("sequence", "primer_bind");
    const QString gcQualifier = GTUtilsAnnotationsTreeView::getQualifierValue("gc%", "primer_bind");
    const QString tmQualifier = GTUtilsAnnotationsTreeView::getQualifierValue("tm", "primer_bind");

    CHECK_SET_ERR("AAAA" == sequenceQualifier, QString("Incorrect value of sequence qualifier: '%1'").arg(sequenceQualifier));
    CHECK_SET_ERR("0" == gcQualifier, QString("Incorrect value of gc content qualifier: '%1'").arg(gcQualifier));
    CHECK_SET_ERR("8" == tmQualifier, QString("Incorrect value of tm qualifier: '%1'").arg(tmQualifier));
}

GUI_TEST_CLASS_DEFINITION(test_0009) {
    //    Import primers from multifasta

    //    1. Open the library, clear it.
    GTUtilsPrimerLibrary::openLibrary();
    GTUtilsPrimerLibrary::clearLibrary();

    //    2. Click "Import".
    //    3. Fill the dialog:
    //        Import from: "Local file(s)";
    //        Files: "_common_data/fasta/random_primers.fa"
    //    and accept the dialog.
    class ImportFromMultifasta : public CustomScenario {
        void run() override {
            ImportPrimersDialogFiller::addFile(testDir + "_common_data/fasta/random_primers.fa");
            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(new ImportPrimersDialogFiller(new ImportFromMultifasta));
    GTUtilsPrimerLibrary::clickButton(GTUtilsPrimerLibrary::Import);

    //    4. Check the library.
    //    Expected state: the library contains four primers.
    GTUtilsTaskTreeView::waitTaskFinished();

    const int librarySize = GTUtilsPrimerLibrary::librarySize();
    CHECK_SET_ERR(4 == librarySize, QString("An unexpected library size: expect %1, got %2").arg(4).arg(librarySize));

    const QString firstData = GTUtilsPrimerLibrary::getPrimerSequence("primer1");
    CHECK_SET_ERR("ACCCGTGCTAGC" == firstData, QString("An unexpected primer '%1' data: expect %2, got %3").arg("primer1").arg("ACCCGTGCTAGC").arg(firstData));

    const QString secondData = GTUtilsPrimerLibrary::getPrimerSequence("primer2");
    CHECK_SET_ERR("GGCATGATCATTCAACG" == secondData, QString("An unexpected primer '%1' data: expect %2, got %3").arg("primer2").arg("GGCATGATCATTCAACG").arg(secondData));

    const QString thirdData = GTUtilsPrimerLibrary::getPrimerSequence("primer3");
    CHECK_SET_ERR("GGAACTTCGACTAG" == thirdData, QString("An unexpected primer '%1' data: expect %2, got %3").arg("primer3").arg("GGAACTTCGACTAG").arg(thirdData));

    const QString fourthData = GTUtilsPrimerLibrary::getPrimerSequence("primer4");
    CHECK_SET_ERR("TTTAGGAGGAATCACACACCCACC" == fourthData, QString("An unexpected primer '%1' data: expect %2, got %3").arg("primer4").arg("TTTAGGAGGAATCACACACCCACC").arg(fourthData));
}

GUI_TEST_CLASS_DEFINITION(test_0010) {
    //    Import primers from several files

    //    1. Open the library, clear it.
    GTUtilsPrimerLibrary::openLibrary();
    GTUtilsPrimerLibrary::clearLibrary();

    //    2. Click "Import".
    //    3. Fill the dialog:
    //        Import from: "Local file(s)";
    //        Files: "_common_data/fasta/random_primers.fa",
    //               "_common_data/fasta/random_primers.fa2"
    //    and accept the dialog.
    class ImportFromSeveralFiles : public CustomScenario {
        void run() override {
            ImportPrimersDialogFiller::addFile(testDir + "_common_data/fasta/random_primers.fa");
            ImportPrimersDialogFiller::addFile(testDir + "_common_data/fasta/random_primers2.fa");
            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(new ImportPrimersDialogFiller(new ImportFromSeveralFiles));
    GTUtilsPrimerLibrary::clickButton(GTUtilsPrimerLibrary::Import);

    //    4. Check the library.
    //    Expected state: the library contains six primers.
    GTUtilsTaskTreeView::waitTaskFinished();

    const int librarySize = GTUtilsPrimerLibrary::librarySize();
    CHECK_SET_ERR(6 == librarySize, QString("An unexpected library size: expect %1, got %2").arg(6).arg(librarySize));

    const QString firstData = GTUtilsPrimerLibrary::getPrimerSequence("primer1");
    CHECK_SET_ERR("ACCCGTGCTAGC" == firstData, QString("An unexpected primer '%1' data: expect %2, got %3").arg("primer1").arg("ACCCGTGCTAGC").arg(firstData));

    const QString secondData = GTUtilsPrimerLibrary::getPrimerSequence("primer2");
    CHECK_SET_ERR("GGCATGATCATTCAACG" == secondData, QString("An unexpected primer '%1' data: expect %2, got %3").arg("primer2").arg("GGCATGATCATTCAACG").arg(secondData));

    const QString thirdData = GTUtilsPrimerLibrary::getPrimerSequence("primer3");
    CHECK_SET_ERR("GGAACTTCGACTAG" == thirdData, QString("An unexpected primer '%1' data: expect %2, got %3").arg("primer3").arg("GGAACTTCGACTAG").arg(thirdData));

    const QString fourthData = GTUtilsPrimerLibrary::getPrimerSequence("primer4");
    CHECK_SET_ERR("TTTAGGAGGAATCACACACCCACC" == fourthData, QString("An unexpected primer '%1' data: expect %2, got %3").arg("primer4").arg("TTTAGGAGGAATCACACACCCACC").arg(fourthData));

    const QString fifthData = GTUtilsPrimerLibrary::getPrimerSequence("primer5");
    CHECK_SET_ERR("GGTTCAGTACAGTCAG" == fifthData, QString("An unexpected primer '%1' data: expect %2, got %3").arg("primer5").arg("GGTTCAGTACAGTCAG").arg(fifthData));

    const QString sixthData = GTUtilsPrimerLibrary::getPrimerSequence("primer6");
    CHECK_SET_ERR("GGTATATTAATTATTATTA" == sixthData, QString("An unexpected primer '%1' data: expect %2, got %3").arg("primer6").arg("GGTATATTAATTATTATTA").arg(sixthData));
}

GUI_TEST_CLASS_DEFINITION(test_0014) {
    // Degenerated primers in the primer library
    // 1. Open primer library
    GTUtilsPrimerLibrary::openLibrary();
    GTUtilsPrimerLibrary::clearLibrary();

    // 2. Create the forward primer "TTNGGTGATGWCGGTGAAARCCTCTGACMCATGCAGCT"
    GTUtilsPrimerLibrary::addPrimer("test_0014_forward", "TTNGGTGATGWCGGTGAAARCCTCTGACMCATGCAGCT");

    // 3. Create the reverse primer "AAGCGCGCGAACAGAAGCGAGAAGCGAACT"
    GTUtilsPrimerLibrary::addPrimer("test_0014_reverse", "AAGCGCGCGAACAGAAGCGAGAAGCGAACT");

    // 4. Edit the reverse primer. New value: "AAGCGNNNNNNNNNNNNNNNNNNNNNR"
    GTUtilsPrimerLibrary::clickPrimer(1);

    AddPrimerDialogFiller::Parameters parameters;
    parameters.primer = "AAGCGNNNNNNNNNNNNNNNNNNNNNR";
    parameters.name = "test_0014_reverse_edit";
    GTUtilsDialog::waitForDialog(new AddPrimerDialogFiller(parameters));

    GTUtilsPrimerLibrary::clickButton(GTUtilsPrimerLibrary::Edit);
}

GUI_TEST_CLASS_DEFINITION(test_0015) {
    // Find the product with the degenerated primers from the library
    // 1. Open primer library
    GTUtilsPrimerLibrary::openLibrary();
    GTUtilsPrimerLibrary::clearLibrary();

    // 2. Create the forward primer "GGGCCAAACAGGATATCTGTGGTAAGCAGT"
    GTUtilsPrimerLibrary::addPrimer("test_0015_forward", "GGGCCAAACAGGATATCTGTGGTAAGCAGT");

    // 3. Create the reverse primer "AAGCGNNNNNNNNNNNNNNNNNNNNNR"
    GTUtilsPrimerLibrary::addPrimer("test_0015_reverse", "AAGCGNNNNNNNNNNNNNNNNNNNNNR");
    GTUtilsPrimerLibrary::clickButton(GTUtilsPrimerLibrary::Close);

    // 4. Open "_common_data/fasta/begin-end.fa"
    GTFileDialog::openFile(testDir + "_common_data/cmdline/pcr/begin-end.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 5. Set the primers from the library
    GTUtilsOptionPanelSequenceView::openTab(GTUtilsOptionPanelSequenceView::InSilicoPcr);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new PrimerLibrarySelectorFiller(0, true));
    GTWidget::click(GTUtilsPcr::browseButton(U2Strand::Direct));

    GTUtilsDialog::waitForDialog(new PrimerLibrarySelectorFiller(1, true));
    GTWidget::click(GTUtilsPcr::browseButton(U2Strand::Complementary));

    // 4. Find the product
    GTWidget::click(GTWidget::findWidget("findProductButton"));
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected: 2 results were found
    int productsCount = GTUtilsPcr::productsCount();
    CHECK_SET_ERR(productsCount == 2, "Wrong results count. Expected 2, got " + QString::number(productsCount));
}

GUI_TEST_CLASS_DEFINITION(test_0016) {
    //    Import primers with degenerated symbols

    //    1. Open the library, clear it.
    GTUtilsPrimerLibrary::openLibrary();
    GTUtilsPrimerLibrary::clearLibrary();

    //    2. Click "Import".
    //    3. Fill the dialog:
    //        Import from: "Local file(s)";
    //        Files: "_common_data/cmdline/primers/primer_degenerated_1.fasta",
    //               "_common_data/cmdline/primers/primer_degenerated_2.fasta"
    //    and accept the dialog.
    class ImportFromSeveralFiles : public CustomScenario {
        void run() override {
            ImportPrimersDialogFiller::addFile(testDir + "_common_data/cmdline/primers/primer_degenerated_1.fasta");
            ImportPrimersDialogFiller::addFile(testDir + "_common_data/cmdline/primers/primer_degenerated_2.fasta");
            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(new ImportPrimersDialogFiller(new ImportFromSeveralFiles));
    GTUtilsPrimerLibrary::clickButton(GTUtilsPrimerLibrary::Import);

    const int librarySize = GTUtilsPrimerLibrary::librarySize();
    CHECK_SET_ERR(2 == librarySize, QString("An unexpected library size: expect %1, got %2").arg(2).arg(librarySize));
}

GUI_TEST_CLASS_DEFINITION(test_0017) {
    // Open Primer Library
    // Check, that dialog works
    GTUtilsPrimerLibrary::openLibrary();

    QMap<GTUtilsMeltingTemperature::Parameter, QString> parameters = {
        {GTUtilsMeltingTemperature::Parameter::Algorithm, "Primer 3"},
        {GTUtilsMeltingTemperature::Parameter::DnaConc, "51.00"},
        {GTUtilsMeltingTemperature::Parameter::MonovalentConc, "51.00"},
        {GTUtilsMeltingTemperature::Parameter::DivalentConc, "0.50"},
        {GTUtilsMeltingTemperature::Parameter::DntpConc, "0"},
        {GTUtilsMeltingTemperature::Parameter::DmsoConc, "1"},
        {GTUtilsMeltingTemperature::Parameter::DmsoFactor, "1.6"},
        {GTUtilsMeltingTemperature::Parameter::FormamideConc, "1"},
        {GTUtilsMeltingTemperature::Parameter::ThermodynamicTable, "0"},
        {GTUtilsMeltingTemperature::Parameter::SaltCorrectionFormula, "2"},
        {GTUtilsMeltingTemperature::Parameter::MaxLen, "33"}};
    GTUtilsDialog::waitForDialog(new MeltingTemperatureSettingsDialogFiller(parameters));
    GTUtilsPrimerLibrary::clickButton(GTUtilsPrimerLibrary::Button::Temperature);
}

}  // namespace GUITest_common_scenarios_primer_library
}  // namespace U2
