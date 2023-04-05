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
    GTUtilsMdi::click(os, GTGlobals::Close);
    // The library is the singleton MDI window

    // 1. Click the menu Tools -> Primer -> Primer Library.
    // Expected: the library MDI window is opened.
    QWidget* libraryMdi1 = GTUtilsPrimerLibrary::openLibrary(os);

    // 2. Click the menu again.
    QWidget* libraryMdi2 = GTUtilsPrimerLibrary::openLibrary(os);

    // Expected: the same MDI windows is opened (not the second one).
    CHECK_SET_ERR(libraryMdi1 == libraryMdi2, "Different MDI windows");

    // 3. Click the close button.
    GTUtilsPrimerLibrary::clickButton(os, GTUtilsPrimerLibrary::Close);
    // Expected: The window is closed.
    QWidget* libraryMdi3 = GTUtilsMdi::activeWindow(os, {false});
    CHECK_SET_ERR(libraryMdi3 == nullptr, "Library MDI is not closed");
}

GUI_TEST_CLASS_DEFINITION(test_0002) {
    // Add new primer:
    //     Primer line edit ACGT content
    //     Availability of the OK button

    // 1. Click the menu Tools -> Primer -> Primer Library.
    GTUtilsPrimerLibrary::openLibrary(os);
    int librarySize = GTUtilsPrimerLibrary::librarySize(os);

    // 2. Click the new primer button.
    // Expected: the dialog appears. The OK button is disabled.
    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) {
            // 3. Set the focus at the primer line edit and write "Q%1" (not ACGT).
            auto primerEdit = dynamic_cast<QLineEdit*>(GTWidget::findWidget(os, "primerEdit"));
            GTLineEdit::setText(os, primerEdit, "Q%1", true);

            // Expected: the line edit is empty.
            CHECK_SET_ERR(primerEdit->text().isEmpty(), "Wrong input");

            // 4. Write "atcg".
            GTLineEdit::setText(os, primerEdit, "atcg", true);

            // Expected: the line edit content is "ATCG". The OK button is enabled.
            CHECK_SET_ERR(primerEdit->text() == "ATCG", "No upper-case");

            // 5. Remove the primer name.
            auto nameEdit = GTWidget::findLineEdit(os, "nameEdit");
            GTLineEdit::setText(os, nameEdit, "");

            // Expected: The OK button is disabled.
            QWidget* dialog = GTWidget::getActiveModalWidget(os);
            QPushButton* okButton = GTUtilsDialog::buttonBox(os, dialog)->button(QDialogButtonBox::Ok);
            CHECK_SET_ERR(!okButton->isEnabled(), "The OK button is enabled");

            // 6. Set the name "Primer".
            GTLineEdit::setText(os, nameEdit, "Primer");

            // 7. Click the OK button.
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };
    AddPrimerDialogFiller::Parameters parameters;
    parameters.scenario = new Scenario();
    GTUtilsDialog::waitForDialog(os, new AddPrimerDialogFiller(os, parameters));
    GTUtilsPrimerLibrary::clickButton(os, GTUtilsPrimerLibrary::Add);

    // Expected: the new primer appears in the table.
    CHECK_SET_ERR(librarySize + 1 == GTUtilsPrimerLibrary::librarySize(os), "Wrong primers count");
    CHECK_SET_ERR(GTUtilsPrimerLibrary::getPrimerSequence(os, librarySize) == "ATCG", "Wrong primer");
}

GUI_TEST_CLASS_DEFINITION(test_0003) {
    // Remove primers:
    //     Availability of the button

    // 1. Click the menu Tools -> Primer -> Primer Library.
    GTUtilsPrimerLibrary::openLibrary(os);
    int librarySize = GTUtilsPrimerLibrary::librarySize(os);

    // 2. Add a new primer if the library is empty.
    AddPrimerDialogFiller::Parameters parameters;
    parameters.primer = "AAAAAAAAAAAAAA";
    GTUtilsDialog::waitForDialog(os, new AddPrimerDialogFiller(os, parameters));
    GTUtilsPrimerLibrary::clickButton(os, GTUtilsPrimerLibrary::Add);

    // 3. Click the empty place of the table.
    QPoint emptyPoint = GTUtilsPrimerLibrary::getPrimerPoint(os, librarySize);
    emptyPoint.setY(emptyPoint.y() + 40);
    GTMouseDriver::moveTo(emptyPoint);
    GTMouseDriver::click();

    // Expected: The remove button is disabled.
    QAbstractButton* removeButton = GTUtilsPrimerLibrary::getButton(os, GTUtilsPrimerLibrary::Remove);
    CHECK_SET_ERR(!removeButton->isEnabled(), "The remove button is enabled");

    // 4. Select the primer.
    GTMouseDriver::moveTo(GTUtilsPrimerLibrary::getPrimerPoint(os, librarySize));
    GTMouseDriver::click();

    // Expected: The remove button is enabled.
    CHECK_SET_ERR(removeButton->isEnabled(), "The remove button is disabled");

    // 5. Click the button.
    GTUtilsPrimerLibrary::clickButton(os, GTUtilsPrimerLibrary::Remove);

    // Expected: the primer is disappeared from the table.
    CHECK_SET_ERR(librarySize == GTUtilsPrimerLibrary::librarySize(os), "Wrong primers count");
}

GUI_TEST_CLASS_DEFINITION(test_0004) {
    // In silico PCR with the library data:
    //     Add a primer from the library
    //     Double click

    {  // Pre-test
        GTUtilsPrimerLibrary::openLibrary(os);
        AddPrimerDialogFiller::Parameters parameters;
        parameters.primer = "AAAAAAAAAAAAAA";
        GTUtilsDialog::waitForDialog(os, new AddPrimerDialogFiller(os, parameters));
        GTUtilsPrimerLibrary::clickButton(os, GTUtilsPrimerLibrary::Add);
        GTUtilsPrimerLibrary::clickButton(os, GTUtilsPrimerLibrary::Close);
    }

    // 1. Open "_common_data/fasta/pcr_test.fa".
    GTFileDialog::openFile(os, testDir + "_common_data/fasta", "pcr_test.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Open the PCR OP.
    GTWidget::click(os, GTWidget::findWidget(os, "OP_IN_SILICO_PCR"));

    // 3. Click the browse library button for the forward primer.
    // Expected: the library dialog appears, the OK button is disabled.
    // 4. Click a primer in the table.
    // Expected: the OK button is enabled.
    // 5. Double click the primer.
    GTUtilsDialog::waitForDialog(os, new PrimerLibrarySelectorFiller(os, -1, true));
    GTWidget::click(os, GTUtilsPcr::browseButton(os, U2Strand::Direct));

    // Expected: the dialog is closed, the chosen primer sequence is in the forward primer line edit.
    auto primerEdit = GTWidget::findLineEdit(os, "primerEdit", GTUtilsPcr::primerBox(os, U2Strand::Direct));
    CHECK_SET_ERR(primerEdit->text() == "AAAAAAAAAAAAAA", "Wrong primer");
}

GUI_TEST_CLASS_DEFINITION(test_0005) {
    // Edit primer:
    //     Availability of the button

    // 1. Click the menu Tools -> Primer -> Primer Library.
    GTUtilsPrimerLibrary::openLibrary(os);

    // 2. Add a new primer if the library is empty.
    for (int i = 0; i < 3; i++) {
        AddPrimerDialogFiller::Parameters parameters;
        parameters.primer = "AAAAAAAAAAAAAA";
        GTUtilsDialog::waitForDialog(os, new AddPrimerDialogFiller(os, parameters));
        GTUtilsPrimerLibrary::clickButton(os, GTUtilsPrimerLibrary::Add);
    }
    int lastPrimer = GTUtilsPrimerLibrary::librarySize(os) - 1;

    // 3. Click the empty place of the table.
    QPoint emptyPoint = GTUtilsPrimerLibrary::getPrimerPoint(os, lastPrimer);
    emptyPoint.setY(emptyPoint.y() + 40);
    GTMouseDriver::moveTo(emptyPoint);
    GTMouseDriver::click();

    // Expected: The edit button is disabled.
    QAbstractButton* editButton = GTUtilsPrimerLibrary::getButton(os, GTUtilsPrimerLibrary::Edit);
    CHECK_SET_ERR(!editButton->isEnabled(), "The remove button is enabled");

    // 4. Select several primers.
    GTMouseDriver::moveTo(GTUtilsPrimerLibrary::getPrimerPoint(os, lastPrimer));
    GTMouseDriver::click();
    GTMouseDriver::moveTo(GTUtilsPrimerLibrary::getPrimerPoint(os, lastPrimer - 2));
    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    GTMouseDriver::click();
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);

    // Expected: The edit button is disabled.
    CHECK_SET_ERR(!editButton->isEnabled(), "The remove button is enabled");

    // 5. Select the primer P.
    GTMouseDriver::moveTo(GTUtilsPrimerLibrary::getPrimerPoint(os, lastPrimer));
    GTMouseDriver::click();

    // Expected: The edit button is enabled.
    CHECK_SET_ERR(editButton->isEnabled(), "The remove button is disabled");

    // 6. Double click the primer P.
    // Expected: the dialog appears. The P's data is written.
    // 7. Edit primer and name and click OK.
    AddPrimerDialogFiller::Parameters parameters;
    parameters.primer = "CCCCCCCCCCCCCC";
    parameters.name = "test_0005";
    GTUtilsDialog::waitForDialog(os, new AddPrimerDialogFiller(os, parameters));
    GTMouseDriver::doubleClick();

    // Expected: the primer is changed in the table.
    CHECK_SET_ERR("CCCCCCCCCCCCCC" == GTUtilsPrimerLibrary::getPrimerSequence(os, lastPrimer), "The sequence is not changed");
}

GUI_TEST_CLASS_DEFINITION(test_0006) {
    //    Export whole library to the fasta file

    //    1. Open the library, clear it, add sequences "AAAA", "CCCC", "GGGG", "TTTT".
    GTUtilsPrimerLibrary::openLibrary(os);
    GTUtilsPrimerLibrary::clearLibrary(os);

    GTUtilsPrimerLibrary::addPrimer(os, "primer1", "AAAA");
    GTUtilsPrimerLibrary::addPrimer(os, "primer2", "CCCC");
    GTUtilsPrimerLibrary::addPrimer(os, "primer3", "GGGG");
    GTUtilsPrimerLibrary::addPrimer(os, "primer4", "TTTT");

    //    2. Select all sequences.
    GTUtilsPrimerLibrary::selectAll(os);

    //    3. Click "Export".
    //    4. Fill the dialog:
    //        Export to: "Local file";
    //        Format: "fasta";
    //        File path: any valid path;
    //    and accept the dialog.
    class ExportToFastaScenario : public CustomScenario {
        void run(HI::GUITestOpStatus& os) override {
            ExportPrimersDialogFiller::setFormat(os, "FASTA");
            ExportPrimersDialogFiller::setFilePath(os, sandBoxDir + "pcrlib/test_0006/primers.fa");
            GTUtilsDialog::clickButtonBox(os, QDialogButtonBox::Ok);
        }
    };

    QDir().mkpath(sandBoxDir + "pcrlib/test_0006");
    GTUtilsDialog::waitForDialog(os, new ExportPrimersDialogFiller(os, new ExportToFastaScenario));
    GTUtilsPrimerLibrary::clickButton(os, GTUtilsPrimerLibrary::Export);

    //    5. Open the exported file.
    //    Expected state: there are the same sequences in the file as in the library.
    GTUtilsTaskTreeView::waitTaskFinished(os);
    const QStringList names = {"primer1", "primer2", "primer3", "primer4"};
    GTUtilsProject::openFileExpectSequences(os, sandBoxDir + "pcrlib/test_0006/", "primers.fa", names);

    const QString firstSeq = GTUtilsSequenceView::getSequenceAsString(os, 0);
    CHECK_SET_ERR("AAAA" == firstSeq, QString("Incorrect sequence data: expect '%1', got '%2'").arg("AAAA").arg(firstSeq));

    const QString secondSeq = GTUtilsSequenceView::getSequenceAsString(os, 1);
    CHECK_SET_ERR("CCCC" == secondSeq, QString("Incorrect sequence data: expect '%1', got '%2'").arg("CCCC").arg(secondSeq));

    const QString thirdSeq = GTUtilsSequenceView::getSequenceAsString(os, 2);
    CHECK_SET_ERR("GGGG" == thirdSeq, QString("Incorrect sequence data: expect '%1', got '%2'").arg("GGGG").arg(thirdSeq));

    const QString fourthSeq = GTUtilsSequenceView::getSequenceAsString(os, 3);
    CHECK_SET_ERR("TTTT" == fourthSeq, QString("Incorrect sequence data: expect '%1', got '%2'").arg("TTTT").arg(fourthSeq));
}

GUI_TEST_CLASS_DEFINITION(test_0007) {
    //    Export several primers from library to the genbank file

    //    1. Open the library, clear it, add sequences "AAAA", "CCCC", "GGGG", "TTTT".
    GTUtilsPrimerLibrary::openLibrary(os);
    GTUtilsPrimerLibrary::clearLibrary(os);
    QMap<GTUtilsMeltingTemperature::Parameter, QString> parameters = {
    {GTUtilsMeltingTemperature::Parameter::Algorithm, "Rough" } };
    GTUtilsDialog::waitForDialog(os, new MeltingTemperatureSettingsDialogFiller(os, parameters));
    GTUtilsPrimerLibrary::clickButton(os, GTUtilsPrimerLibrary::Button::Temperature);


    GTUtilsPrimerLibrary::addPrimer(os, "primer1", "AAAA");
    GTUtilsPrimerLibrary::addPrimer(os, "primer2", "CCCC");
    GTUtilsPrimerLibrary::addPrimer(os, "primer3", "GGGG");
    GTUtilsPrimerLibrary::addPrimer(os, "primer4", "TTTT");

    //    2. Select the second and the third sequences.
    GTUtilsPrimerLibrary::selectPrimers(os, QList<int>() << 0 << 2);

    //    3. Click "Export".
    //    4. Fill the dialog:
    //        Export to: "Local file";
    //        Format: "GenBank";
    //        File path: any valid path;
    //    and accept the dialog.
    class ExportToGenbankScenario : public CustomScenario {
        void run(HI::GUITestOpStatus& os) override {
            ExportPrimersDialogFiller::setFormat(os, "GenBank");
            ExportPrimersDialogFiller::setFilePath(os, sandBoxDir + "pcrlib/test_0007/primers.gb");
            GTUtilsDialog::clickButtonBox(os, QDialogButtonBox::Ok);
        }
    };

    QDir().mkpath(sandBoxDir + "pcrlib/test_0007");
    GTUtilsDialog::waitForDialog(os, new ExportPrimersDialogFiller(os, new ExportToGenbankScenario));
    GTUtilsPrimerLibrary::clickButton(os, GTUtilsPrimerLibrary::Export);

    //    5. Open the exported file.
    //    Expected state: there are two sequences (primer1: AAAA, primer3: GGGG) with annotations, that contains qualifiers with primer's parameters.
    GTUtilsTaskTreeView::waitTaskFinished(os);
    const QStringList names = {"primer1", "primer3"};
    GTUtilsProject::openFileExpectSequences(os, sandBoxDir + "pcrlib/test_0007/", "primers.gb", names);

    const QString firstSeq = GTUtilsSequenceView::getSequenceAsString(os, 0);
    CHECK_SET_ERR("AAAA" == firstSeq, QString("Incorrect sequence data: expect '%1', got '%2'").arg("AAAA").arg(firstSeq));

    const QString secondSeq = GTUtilsSequenceView::getSequenceAsString(os, 1);
    CHECK_SET_ERR("GGGG" == secondSeq, QString("Incorrect sequence data: expect '%1', got '%2'").arg("GGGG").arg(secondSeq));

    const QList<QTreeWidgetItem*> items = GTUtilsAnnotationsTreeView::findItems(os, "primer_bind");
    CHECK_SET_ERR(items.size() == 2, QString("Unexpected annotations count: epxect %1, got %2").arg(2).arg(items.size()));

    GTUtilsAnnotationsTreeView::selectItemsByName(os, {"primer_bind"});
    const QString sequenceQualifier = GTUtilsAnnotationsTreeView::getQualifierValue(os, "sequence", "primer_bind");
    const QString gcQualifier = GTUtilsAnnotationsTreeView::getQualifierValue(os, "gc%", "primer_bind");
    const QString tmQualifier = GTUtilsAnnotationsTreeView::getQualifierValue(os, "tm", "primer_bind");

    CHECK_SET_ERR("AAAA" == sequenceQualifier, QString("Incorrect value of sequence qualifier: '%1'").arg(sequenceQualifier));
    CHECK_SET_ERR("0" == gcQualifier, QString("Incorrect value of gc content qualifier: '%1'").arg(gcQualifier));
    CHECK_SET_ERR("8" == tmQualifier, QString("Incorrect value of tm qualifier: '%1'").arg(tmQualifier));
}

GUI_TEST_CLASS_DEFINITION(test_0009) {
    //    Import primers from multifasta

    //    1. Open the library, clear it.
    GTUtilsPrimerLibrary::openLibrary(os);
    GTUtilsPrimerLibrary::clearLibrary(os);

    //    2. Click "Import".
    //    3. Fill the dialog:
    //        Import from: "Local file(s)";
    //        Files: "_common_data/fasta/random_primers.fa"
    //    and accept the dialog.
    class ImportFromMultifasta : public CustomScenario {
        void run(HI::GUITestOpStatus& os) override {
            ImportPrimersDialogFiller::addFile(os, testDir + "_common_data/fasta/random_primers.fa");
            GTUtilsDialog::clickButtonBox(os, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new ImportPrimersDialogFiller(os, new ImportFromMultifasta));
    GTUtilsPrimerLibrary::clickButton(os, GTUtilsPrimerLibrary::Import);

    //    4. Check the library.
    //    Expected state: the library contains four primers.
    GTUtilsTaskTreeView::waitTaskFinished(os);

    const int librarySize = GTUtilsPrimerLibrary::librarySize(os);
    CHECK_SET_ERR(4 == librarySize, QString("An unexpected library size: expect %1, got %2").arg(4).arg(librarySize));

    const QString firstData = GTUtilsPrimerLibrary::getPrimerSequence(os, "primer1");
    CHECK_SET_ERR("ACCCGTGCTAGC" == firstData, QString("An unexpected primer '%1' data: expect %2, got %3").arg("primer1").arg("ACCCGTGCTAGC").arg(firstData));

    const QString secondData = GTUtilsPrimerLibrary::getPrimerSequence(os, "primer2");
    CHECK_SET_ERR("GGCATGATCATTCAACG" == secondData, QString("An unexpected primer '%1' data: expect %2, got %3").arg("primer2").arg("GGCATGATCATTCAACG").arg(secondData));

    const QString thirdData = GTUtilsPrimerLibrary::getPrimerSequence(os, "primer3");
    CHECK_SET_ERR("GGAACTTCGACTAG" == thirdData, QString("An unexpected primer '%1' data: expect %2, got %3").arg("primer3").arg("GGAACTTCGACTAG").arg(thirdData));

    const QString fourthData = GTUtilsPrimerLibrary::getPrimerSequence(os, "primer4");
    CHECK_SET_ERR("TTTAGGAGGAATCACACACCCACC" == fourthData, QString("An unexpected primer '%1' data: expect %2, got %3").arg("primer4").arg("TTTAGGAGGAATCACACACCCACC").arg(fourthData));
}

GUI_TEST_CLASS_DEFINITION(test_0010) {
    //    Import primers from several files

    //    1. Open the library, clear it.
    GTUtilsPrimerLibrary::openLibrary(os);
    GTUtilsPrimerLibrary::clearLibrary(os);

    //    2. Click "Import".
    //    3. Fill the dialog:
    //        Import from: "Local file(s)";
    //        Files: "_common_data/fasta/random_primers.fa",
    //               "_common_data/fasta/random_primers.fa2"
    //    and accept the dialog.
    class ImportFromSeveralFiles : public CustomScenario {
        void run(HI::GUITestOpStatus& os) override {
            ImportPrimersDialogFiller::addFile(os, testDir + "_common_data/fasta/random_primers.fa");
            ImportPrimersDialogFiller::addFile(os, testDir + "_common_data/fasta/random_primers2.fa");
            GTUtilsDialog::clickButtonBox(os, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new ImportPrimersDialogFiller(os, new ImportFromSeveralFiles));
    GTUtilsPrimerLibrary::clickButton(os, GTUtilsPrimerLibrary::Import);

    //    4. Check the library.
    //    Expected state: the library contains six primers.
    GTUtilsTaskTreeView::waitTaskFinished(os);

    const int librarySize = GTUtilsPrimerLibrary::librarySize(os);
    CHECK_SET_ERR(6 == librarySize, QString("An unexpected library size: expect %1, got %2").arg(6).arg(librarySize));

    const QString firstData = GTUtilsPrimerLibrary::getPrimerSequence(os, "primer1");
    CHECK_SET_ERR("ACCCGTGCTAGC" == firstData, QString("An unexpected primer '%1' data: expect %2, got %3").arg("primer1").arg("ACCCGTGCTAGC").arg(firstData));

    const QString secondData = GTUtilsPrimerLibrary::getPrimerSequence(os, "primer2");
    CHECK_SET_ERR("GGCATGATCATTCAACG" == secondData, QString("An unexpected primer '%1' data: expect %2, got %3").arg("primer2").arg("GGCATGATCATTCAACG").arg(secondData));

    const QString thirdData = GTUtilsPrimerLibrary::getPrimerSequence(os, "primer3");
    CHECK_SET_ERR("GGAACTTCGACTAG" == thirdData, QString("An unexpected primer '%1' data: expect %2, got %3").arg("primer3").arg("GGAACTTCGACTAG").arg(thirdData));

    const QString fourthData = GTUtilsPrimerLibrary::getPrimerSequence(os, "primer4");
    CHECK_SET_ERR("TTTAGGAGGAATCACACACCCACC" == fourthData, QString("An unexpected primer '%1' data: expect %2, got %3").arg("primer4").arg("TTTAGGAGGAATCACACACCCACC").arg(fourthData));

    const QString fifthData = GTUtilsPrimerLibrary::getPrimerSequence(os, "primer5");
    CHECK_SET_ERR("GGTTCAGTACAGTCAG" == fifthData, QString("An unexpected primer '%1' data: expect %2, got %3").arg("primer5").arg("GGTTCAGTACAGTCAG").arg(fifthData));

    const QString sixthData = GTUtilsPrimerLibrary::getPrimerSequence(os, "primer6");
    CHECK_SET_ERR("GGTATATTAATTATTATTA" == sixthData, QString("An unexpected primer '%1' data: expect %2, got %3").arg("primer6").arg("GGTATATTAATTATTATTA").arg(sixthData));
}

GUI_TEST_CLASS_DEFINITION(test_0014) {
    // Degenerated primers in the primer library
    // 1. Open primer library
    GTUtilsPrimerLibrary::openLibrary(os);
    GTUtilsPrimerLibrary::clearLibrary(os);

    // 2. Create the forward primer "TTNGGTGATGWCGGTGAAARCCTCTGACMCATGCAGCT"
    GTUtilsPrimerLibrary::addPrimer(os, "test_0014_forward", "TTNGGTGATGWCGGTGAAARCCTCTGACMCATGCAGCT");

    // 3. Create the reverse primer "AAGCGCGCGAACAGAAGCGAGAAGCGAACT"
    GTUtilsPrimerLibrary::addPrimer(os, "test_0014_reverse", "AAGCGCGCGAACAGAAGCGAGAAGCGAACT");

    // 4. Edit the reverse primer. New value: "AAGCGNNNNNNNNNNNNNNNNNNNNNR"
    GTUtilsPrimerLibrary::clickPrimer(os, 1);

    AddPrimerDialogFiller::Parameters parameters;
    parameters.primer = "AAGCGNNNNNNNNNNNNNNNNNNNNNR";
    parameters.name = "test_0014_reverse_edit";
    GTUtilsDialog::waitForDialog(os, new AddPrimerDialogFiller(os, parameters));

    GTUtilsPrimerLibrary::clickButton(os, GTUtilsPrimerLibrary::Edit);
}

GUI_TEST_CLASS_DEFINITION(test_0015) {
    // Find the product with the degenerated primers from the library
    // 1. Open primer library
    GTUtilsPrimerLibrary::openLibrary(os);
    GTUtilsPrimerLibrary::clearLibrary(os);

    // 2. Create the forward primer "GGGCCAAACAGGATATCTGTGGTAAGCAGT"
    GTUtilsPrimerLibrary::addPrimer(os, "test_0015_forward", "GGGCCAAACAGGATATCTGTGGTAAGCAGT");

    // 3. Create the reverse primer "AAGCGNNNNNNNNNNNNNNNNNNNNNR"
    GTUtilsPrimerLibrary::addPrimer(os, "test_0015_reverse", "AAGCGNNNNNNNNNNNNNNNNNNNNNR");
    GTUtilsPrimerLibrary::clickButton(os, GTUtilsPrimerLibrary::Close);

    // 4. Open "_common_data/fasta/begin-end.fa"
    GTFileDialog::openFile(os, testDir + "_common_data/cmdline/pcr/begin-end.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 5. Set the primers from the library
    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::InSilicoPcr);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new PrimerLibrarySelectorFiller(os, 0, true));
    GTWidget::click(os, GTUtilsPcr::browseButton(os, U2Strand::Direct));

    GTUtilsDialog::waitForDialog(os, new PrimerLibrarySelectorFiller(os, 1, true));
    GTWidget::click(os, GTUtilsPcr::browseButton(os, U2Strand::Complementary));

    // 4. Find the product
    GTWidget::click(os, GTWidget::findWidget(os, "findProductButton"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected: 2 results were found
    int productsCount = GTUtilsPcr::productsCount(os);
    CHECK_SET_ERR(productsCount == 2, "Wrong results count. Expected 2, got " + QString::number(productsCount));
}

GUI_TEST_CLASS_DEFINITION(test_0016) {
    //    Import primers with degenerated symbols

    //    1. Open the library, clear it.
    GTUtilsPrimerLibrary::openLibrary(os);
    GTUtilsPrimerLibrary::clearLibrary(os);

    //    2. Click "Import".
    //    3. Fill the dialog:
    //        Import from: "Local file(s)";
    //        Files: "_common_data/cmdline/primers/primer_degenerated_1.fasta",
    //               "_common_data/cmdline/primers/primer_degenerated_2.fasta"
    //    and accept the dialog.
    class ImportFromSeveralFiles : public CustomScenario {
        void run(HI::GUITestOpStatus& os) override {
            ImportPrimersDialogFiller::addFile(os, testDir + "_common_data/cmdline/primers/primer_degenerated_1.fasta");
            ImportPrimersDialogFiller::addFile(os, testDir + "_common_data/cmdline/primers/primer_degenerated_2.fasta");
            GTUtilsDialog::clickButtonBox(os, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new ImportPrimersDialogFiller(os, new ImportFromSeveralFiles));
    GTUtilsPrimerLibrary::clickButton(os, GTUtilsPrimerLibrary::Import);

    const int librarySize = GTUtilsPrimerLibrary::librarySize(os);
    CHECK_SET_ERR(2 == librarySize, QString("An unexpected library size: expect %1, got %2").arg(2).arg(librarySize));
}

GUI_TEST_CLASS_DEFINITION(test_0017) {
    // Open Primer Library
    // Check, that dialog works
    GTUtilsPrimerLibrary::openLibrary(os);

    QMap<GTUtilsMeltingTemperature::Parameter, QString> parameters = {
        {GTUtilsMeltingTemperature::Parameter::Algorithm, "Primer 3" },
        { GTUtilsMeltingTemperature::Parameter::DnaConc, "51.00" },
        { GTUtilsMeltingTemperature::Parameter::MonovalentConc, "51.00" },
        { GTUtilsMeltingTemperature::Parameter::DivalentConc, "0.50" },
        { GTUtilsMeltingTemperature::Parameter::DntpConc, "0" },
        { GTUtilsMeltingTemperature::Parameter::DmsoConc, "1" },
        { GTUtilsMeltingTemperature::Parameter::DmsoFactor, "1.6" },
        { GTUtilsMeltingTemperature::Parameter::FormamideConc, "1" },
        { GTUtilsMeltingTemperature::Parameter::ThermodynamicTable, "0" },
        { GTUtilsMeltingTemperature::Parameter::SaltCorrectionFormula, "2" },
        { GTUtilsMeltingTemperature::Parameter::MaxLen, "33"} };
    GTUtilsDialog::waitForDialog(os, new MeltingTemperatureSettingsDialogFiller(os, parameters));
    GTUtilsPrimerLibrary::clickButton(os, GTUtilsPrimerLibrary::Button::Temperature);
}


}  // namespace GUITest_common_scenarios_primer_library
}  // namespace U2
