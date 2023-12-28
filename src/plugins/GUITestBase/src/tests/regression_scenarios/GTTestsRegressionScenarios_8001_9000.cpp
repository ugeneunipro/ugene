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

#include <base_dialogs/GTFileDialog.h>
#include <base_dialogs/MessageBoxFiller.h>
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTCheckBox.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTMainWindow.h>
#include <primitives/GTMenu.h>
#include <primitives/GTPlainTextEdit.h>
#include <primitives/GTToolbar.h>
#include <primitives/GTWidget.h>
#include <utils/GTUtilsDialog.h>

#include <QClipboard>

#include <U2Core/AppContext.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/ProjectModel.h>

#include <U2View/TvTextItem.h>

#include "GTTestsRegressionScenarios_8001_9000.h"
#include "GTUtilsAnnotationsTreeView.h"
#include "GTUtilsBookmarksTreeView.h"
#include "GTUtilsLog.h"
#include "GTUtilsMdi.h"
#include "GTUtilsOptionPanelMSA.h"
#include "GTUtilsOptionPanelSequenceView.h"
#include "GTUtilsProject.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsSequenceView.h"
#include "GTUtilsTaskTreeView.h"
#include "GTUtilsOptionPanelSequenceView.h"

#include "runnables/ugene/corelibs/U2Gui/CreateDocumentFromTextDialogFiller.h"
#include "runnables/ugene/plugins/dna_export/ExportSequencesDialogFiller.h"
#include "runnables/ugene/plugins/external_tools/AlignToReferenceBlastDialogFiller.h"
#include "runnables/ugene/plugins_3rdparty/primer3/Primer3DialogFiller.h"
#include "runnables/ugene/ugeneui/AnyDialogFiller.h"
#include "runnables/ugene/ugeneui/CreateNewProjectWidgetFiller.h"
#include "runnables/ugene/ugeneui/DocumentFormatSelectorDialogFiller.h"


namespace U2 {

namespace GUITest_regression_scenarios {
using namespace HI;

GUI_TEST_CLASS_DEFINITION(test_8001) {
    // 1. Click "File -> New document from text"
    // 2. Type "ACGT" as sequence and sandboxDir/test_8001.fa as path
    // 3. Click "Create"
    // Expected: sequence opened
    // 4. Click "File -> New document from text" again
    // 5. Type "ACGT" as sequence and sandboxDir/test_8001.fa as path again
    // Expected: Question "Do you want to remove it from the project and replace with the current sequence?" appeared
    // 6. Click Yes
    // Expected: "Do you want to reload document?" dialog appeared
    // 7. Click "Yes"
    // Expected: no errors in the log

    GTUtilsDialog::waitForDialog(new CreateDocumentFiller("ACGT", false, CreateDocumentFiller::StandardDNA, false, true, "-", sandBoxDir + "test_8001.fa", CreateDocumentFiller::FASTA, "test_8001"));
    GTMenu::clickMainMenuItem({"File", "New document from text..."});
    GTUtilsTaskTreeView::waitTaskFinished();

    class Scenario : public CustomScenario {
    public:
        void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            GTPlainTextEdit::setText(GTWidget::findPlainTextEdit("sequenceEdit", dialog), "ACGT");
            GTLineEdit::setText(GTWidget::findLineEdit("filepathEdit", dialog), sandBoxDir + "test_8001.fa");

            GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Yes, "Do you want to remove it from the project and replace with the current sequence?"));

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };

    GTLogTracer lt;
    GTUtilsDialog::waitForDialog(new CreateDocumentFiller(new Scenario));
    GTMenu::clickMainMenuItem({"File", "New document from text..."});
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Yes, "Do you want to reload it?"));
    CHECK_SET_ERR(!lt.hasErrors(), "Expected no errors");
}

GUI_TEST_CLASS_DEFINITION(test_8002) {
    // Open murine.gb
    // Open the "Statistics" tab
    // Hide "Common statistics"
    // Double click on the first annotation
    // Show "Common statistics"
    // Expected: Length = 589

    GTFileDialog::openFile(dataDir + "samples/Genbank", "murine.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();
    GTUtilsOptionPanelSequenceView::openTab(GTUtilsOptionPanelSequenceView::Statistics);
    auto arrowheaderCommonStatistics = GTWidget::findWidget("ArrowHeader_Common Statistics");
    GTWidget::click(arrowheaderCommonStatistics);
    GTUtilsSequenceView::clickAnnotationPan("misc_feature", 2, 0, true);
    GTWidget::click(arrowheaderCommonStatistics);
    auto commonStatistics = GTWidget::findLabel("Common Statistics");
    CHECK_SET_ERR(commonStatistics->text().contains("589 nt"), "Expected text not found");
}

GUI_TEST_CLASS_DEFINITION(test_8009) {
    /*
     * 1. Open Tools->Sanger data analysis-> Map reads to reference
     * 2. Set wrong format reference file from sample: sample/ACE/k26.ace
     * 3. Add reads: _common_data/sanger/sanger_03.ab1
     * 4. Click Map button
     * Expected: Error log message 'wrong reference format'
     * 5. Open Tools->Sanger data analysis-> Map reads to reference
     * 5. Set wrong format reference file from sample: path which not exists
     * 7. Add reads: _common_data/sanger/sanger_03.ab1
     * 8. Click Map button
     * Expected: Error log message 'reference file doesn't exist'
     */
    class SetRefAndAlign : public CustomScenario {
    public:
        void run() override {
            GTLineEdit::setText(GTWidget::findLineEdit("referenceLineEdit"), refUrl);

            GTUtilsDialog::waitForDialog(new GTFileDialogUtils_list({testDir + "_common_data/sanger/sanger_03.ab1"}));
            GTWidget::click(GTWidget::findPushButton("addReadButton"));
            GTUtilsTaskTreeView::waitTaskFinished();

            // Push "Align" button.
            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Ok);
        }

        QString refUrl;
    };

    GTLogTracer lt;
    auto setRefAndAlignScenario = new SetRefAndAlign();
    setRefAndAlignScenario->refUrl = dataDir + "samples/ACE/K26.ace";
    GTUtilsDialog::waitForDialog(new AlignToReferenceBlastDialogFiller(setRefAndAlignScenario));
    GTMenu::clickMainMenuItem({"Tools", "Sanger data analysis", "Map reads to reference..."});
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(lt.hasMessage("wrong reference format"), "Expected message 'wrong reference format' not found!");

    lt.clear();
    setRefAndAlignScenario = new SetRefAndAlign();
    setRefAndAlignScenario->refUrl = dataDir + "not_existing_path";
    GTUtilsDialog::waitForDialog(new AlignToReferenceBlastDialogFiller(setRefAndAlignScenario));
    GTMenu::clickMainMenuItem({"Tools", "Sanger data analysis", "Map reads to reference..."});
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(lt.hasMessage("reference file doesn't exist"), "Expected message 'reference file doesn't exist' not found!");
}

GUI_TEST_CLASS_DEFINITION(test_8010) {
    // Open murine.gb
    // Open "Primer 3"
    // Click OK with default settings.
    // Export sequence with annotations to another GenBank file
    // Expected : primers have pair group parents
    GTFileDialog::openFile(dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    GTUtilsDialog::waitForDialog(new Primer3DialogFiller(Primer3DialogFiller::Primer3Settings()));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Primer3");
    GTUtilsTaskTreeView::waitTaskFinished();

    class Scenario : public CustomScenario {
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            GTLineEdit::setText("fileNameEdit", sandBoxDir + "test_8010.gb", dialog, true);
            GTComboBox::selectItemByText("formatCombo", dialog, "GenBank");
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::add(new PopupChooser({ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, ACTION_EXPORT_SEQUENCE}));
    GTUtilsDialog::add(new ExportSelectedRegionFiller(new Scenario()));
    GTUtilsProjectTreeView::click("NC_001363", Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsAnnotationsTreeView::clickItem("pair 1  (0, 2)", 1, false);
    GTUtilsAnnotationsTreeView::clickItem("pair 2  (0, 2)", 1, false);
    GTUtilsAnnotationsTreeView::clickItem("pair 3  (0, 2)", 1, false);
    GTUtilsAnnotationsTreeView::clickItem("pair 4  (0, 2)", 1, false);
    GTUtilsAnnotationsTreeView::clickItem("pair 5  (0, 2)", 1, false);
}

GUI_TEST_CLASS_DEFINITION(test_8015) {
    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    GTUtilsOptionPanelSequenceView::openTab(GTUtilsOptionPanelSequenceView::Search);
    GTUtilsOptionPanelSequenceView::setAlgorithm("Substitute");
    GTUtilsOptionPanelMsa::enterPattern("ZZZ");
    GTUtilsOptionPanelSequenceView::setSearchWithAmbiguousBases();

    auto resultLabel = GTWidget::findLabel("resultLabel");
    CHECK_SET_ERR(resultLabel->text() == "Results: -/0", "Unexpected find algorithm results");
}

GUI_TEST_CLASS_DEFINITION(test_8028) {
    GTUtilsDialog::waitForDialog(new SaveProjectAsDialogFiller("proj_test_8028", sandBoxDir + "proj_test_8028"));
    GTMenu::clickMainMenuItem({"File", "New project..."}, GTGlobals::UseMouse);

    GTUtilsDialog::waitForDialog(new DocumentFormatSelectorDialogFiller("Plain text"));
    GTUtilsProject::openFile(testDir + "_common_data/text/text.txt");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsBookmarksTreeView::addBookmark(GTUtilsMdi::activeWindow()->windowTitle(), "test_8028");

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("text"));
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
}

GUI_TEST_CLASS_DEFINITION(test_8037) {
    // Open human_T1.fa
    // Open the "Annotate plasmid" dialog
    // Check "All"
    // Expected: 12 checkboxes checked
    // Check "None"
    // Expected: 0 checkboxes checked
    // Check "Promoter", "Regulatory sequence" and "Gene"
    // Check "Invert"
    // Expected: 9 checkboxes checked

    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    class Scenario : public CustomScenario {
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            GTWidget::click(GTWidget::findPushButton("pbAll"));
            auto checked = GTWidget::findChildren<QCheckBox>(dialog, [](QCheckBox* checkBox) { return checkBox->isChecked(); });
            CHECK_SET_ERR(checked.size() == 12, QString("Unexpected checked size, expected: 12, current: %1").arg(checked.size()));

            GTWidget::click(GTWidget::findPushButton("pbNone"));
            checked = GTWidget::findChildren<QCheckBox>(dialog, [](QCheckBox* checkBox) { return checkBox->isChecked(); });
            CHECK_SET_ERR(checked.size() == 0, QString("Unexpected checked size, expected: 0, current: %1").arg(checked.size()));

            GTCheckBox::setChecked("promotersBox", dialog);
            GTCheckBox::setChecked("regulatoryBox", dialog);
            GTCheckBox::setChecked("geneBox", dialog);

            GTWidget::click(GTWidget::findPushButton("pbInvert"));
            checked = GTWidget::findChildren<QCheckBox>(dialog, [](QCheckBox* checkBox) { return checkBox->isChecked(); });
            CHECK_SET_ERR(checked.size() == 9, QString("Unexpected checked size, expected: 9, current: %1").arg(checked.size()));

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(new AnyDialogFiller("CustomAutoAnnotationDialog",  new Scenario()));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Annotate plasmid");
}

}  // namespace GUITest_regression_scenarios

}  // namespace U2
