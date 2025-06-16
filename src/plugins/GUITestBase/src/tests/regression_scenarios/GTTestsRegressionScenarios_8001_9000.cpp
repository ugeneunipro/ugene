/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include <api/GTSequenceReadingModeDialog.h>
#include <api/GTSequenceReadingModeDialogUtils.h>
#include <base_dialogs/DefaultDialogFiller.h>
#include <base_dialogs/GTFileDialog.h>
#include <base_dialogs/MessageBoxFiller.h>
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTAction.h>
#include <primitives/GTCheckBox.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTLabel.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTMenu.h>
#include <primitives/GTPlainTextEdit.h>
#include <primitives/GTTableView.h>
#include <primitives/GTTabWidget.h>
#include <primitives/GTToolbar.h>
#include <primitives/GTTreeWidget.h>
#include <primitives/GTRadioButton.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTWidget.h>
#include <system/GTFile.h>
#include <utils/GTKeyboardUtils.h>
#include <utils/GTUtilsDialog.h>

#include <QClipboard>
#include <QDir>

#include <U2Core/AppContext.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/ProjectModel.h>

#include <U2View/McaEditorReferenceArea.h>
#include <U2View/TvTextItem.h>

#include "GTTestsRegressionScenarios_8001_9000.h"
#include "GTUtilsAssemblyBrowser.h"
#include "GTUtilsAnnotationsTreeView.h"
#include "GTUtilsBookmarksTreeView.h"
#include "GTUtilsCircularView.h"
#include "GTUtilsDashboard.h"
#include "GTUtilsLog.h"
#include "GTUtilsNotifications.h"
#include "GTUtilsMcaEditorSequenceArea.h"
#include "GTUtilsMdi.h"
#include "GTUtilsMcaEditor.h"
#include "GTUtilsMsaEditor.h"
#include "GTUtilsOptionPanelMca.h"
#include "GTUtilsOptionPanelMSA.h"
#include "GTUtilsOptionPanelSequenceView.h"
#include "GTUtilsProject.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsQueryDesigner.h"
#include "GTUtilsSequenceView.h"
#include "GTUtilsStartPage.h"
#include "GTUtilsTask.h"
#include "GTUtilsTaskTreeView.h"
#include "GTUtilsWizard.h"
#include "GTUtilsWorkflowDesigner.h"
#include "runnables/ugene/corelibs/U2Gui/AppSettingsDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/CreateAnnotationWidgetFiller.h"
#include "runnables/ugene/corelibs/U2Gui/CreateDocumentFromTextDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/EditSequenceDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ImportBAMFileDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/PositionSelectorFiller.h"
#include "runnables/ugene/corelibs/U2Gui/RangeSelectionDialogFiller.h"
#include "runnables/ugene/corelibs/U2View/utils_smith_waterman/SmithWatermanDialogBaseFiller.h"
#include "runnables/ugene/plugins/dna_export/DNASequenceGeneratorDialogFiller.h"
#include "runnables/ugene/plugins/dna_export/ExportSequencesDialogFiller.h"
#include "runnables/ugene/plugins/enzymes/ConstructMoleculeDialogFiller.h"
#include "runnables/ugene/plugins/enzymes/FindEnzymesDialogFiller.h"
#include "runnables/ugene/plugins/external_tools/AlignToReferenceBlastDialogFiller.h"
#include "runnables/ugene/plugins/workflow_designer/DefaultWizardFiller.h"
#include "runnables/ugene/plugins/workflow_designer/WizardFiller.h"
#include "runnables/ugene/plugins_3rdparty/primer3/Primer3DialogFiller.h"
#include "runnables/ugene/ugeneui/AnyDialogFiller.h"
#include "runnables/ugene/ugeneui/CreateNewProjectWidgetFiller.h"
#include "runnables/ugene/ugeneui/DocumentFormatSelectorDialogFiller.h"
#include "runnables/ugene/ugeneui/SaveProjectDialogFiller.h"

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
        void run() override {
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
    // Double-click on the first annotation
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

GUI_TEST_CLASS_DEFINITION(test_8004) {
    /*
     * 1. Open human_T1.fa
     * 2. Remove sequence object from document
     * 3. Open context menu on document
     * Expected state: submenu 'BLAST' not present in context menu
     */
    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    class BLASTMenuItemChecker : public CustomScenario {
        void run() override {
            auto activePopupMenu = qobject_cast<QMenu*>(QApplication::activePopupWidget());
            CHECK_SET_ERR(activePopupMenu != nullptr, "Active popup menu is NULL");

            QAction* showCircular = GTMenu::getMenuItem(activePopupMenu, "BLAST", false);
            CHECK_SET_ERR(showCircular == nullptr, "'BLAST' menu item should be NULL");

            GTKeyboardDriver::keyClick(Qt::Key_Escape);
        }
    };

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("human_T1 (UCSC April 2002 chr7:115977709-117855134)"));
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick(Qt::Key_Delete);

    GTUtilsDialog::waitForDialog(new PopupChecker(new BLASTMenuItemChecker));
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("human_T1.fa"));
    GTMouseDriver::click(Qt::RightButton);
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

GUI_TEST_CLASS_DEFINITION(test_8017) {
    /*
     * 1. Open samples/Genbank/NC_014267.1.gb and sars.gb
     * 2. Close view for sars.gb
     * 3. Right click on sequence object in sars.gb and add it to opened view
     * 4. Open "Lock scales" menu
     * Expected state: only "Lock scales: visible range start" item is active
     * 5. Select any region for first sequence. Open "Lock scales" menu
     * Expected state: "Lock scales: visible range start" and "Lock scales: selected sequence" items are active
     * 6. Select annotation for first sequence. Open "Lock scales" menu
     * Expected state: "Lock scales: visible range start" and "Lock scales: selected annotation" items are active
     */

    class MenuCheckerItemsEnabled : public CustomScenario {
    public:
        MenuCheckerItemsEnabled(const QStringList& enabledMmenuItems_)
            : enabledMmenuItems(enabledMmenuItems_) {};
        void run() override {
            QMenu* activePopupMenu = GTWidget::getActivePopupMenu();
            QList<QAction*> menuActions = activePopupMenu->actions();
            for (QAction* action : qAsConst(menuActions)) {
                if (action->isEnabled()) {
                    const QString actionText = action->text();
                    CHECK_SET_ERR(enabledMmenuItems.contains(actionText), QString("Item %1 enabled but shouldn't!").arg(actionText));
                    enabledMmenuItems.removeAll(actionText);
                }
            }
            CHECK_SET_ERR(enabledMmenuItems.isEmpty(), QString("Items '%1' are not enabled!").arg(enabledMmenuItems.join(", ")));
            GTKeyboardDriver::keyClick(Qt::Key_Escape);
        }

        QStringList enabledMmenuItems;
    };

    GTSequenceReadingModeDialog::mode = GTSequenceReadingModeDialog::Separate;
    GTUtilsDialog::waitForDialog(new GTSequenceReadingModeDialogUtils());
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils_list(dataDir + "samples/Genbank/", {"NC_014267.1.gb", "sars.gb"}));
    GTMenu::clickMainMenuItem({"File", "Open..."});

    GTUtilsMdi::closeWindow("NC_004718 [sars.gb]");
    GTUtilsMdi::closeWindow("Start Page");

    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Add to view", "Add to view: NC_014267 [NC_014267.1.gb]"}));
    GTUtilsProjectTreeView::click("NC_004718", Qt::RightButton);

    GTUtilsCv::commonCvBtn::click();

    QAbstractButton* lockScalesButton = qobject_cast<QAbstractButton*>(GTWidget::findWidget("Lock scales"));
    QPoint menuActivationPoint = QPoint(lockScalesButton->size().width() - 6, lockScalesButton->size().height() / 2);

    GTUtilsDialog::waitForDialog(new PopupChecker(new MenuCheckerItemsEnabled({"Lock scales: visible range start"})));
    GTWidget::click(lockScalesButton, Qt::LeftButton, menuActivationPoint);

    GTUtilsSequenceView::selectSequenceRegion(2, 222);
    GTUtilsDialog::waitForDialog(new PopupChecker(new MenuCheckerItemsEnabled({"Lock scales: visible range start", "Lock scales: selected sequence"})));
    GTWidget::click(lockScalesButton, Qt::LeftButton, menuActivationPoint);

    GTUtilsAnnotationsTreeView::selectItems({GTUtilsAnnotationsTreeView::findItems("rRNA").first()});

    GTUtilsDialog::waitForDialog(new PopupChecker(new MenuCheckerItemsEnabled({"Lock scales: visible range start", "Lock scales: selected annotation"})));
    GTWidget::click(lockScalesButton, Qt::LeftButton, menuActivationPoint);
}

GUI_TEST_CLASS_DEFINITION(test_8018) {
    class Custom : public CustomScenario {
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            AppSettingsDialogFiller::openTab(AppSettingsDialogFiller::ExternalTools);

            // 2. Open a Python 3 tab
            AppSettingsDialogFiller::isExternalToolValid("Python 3");

            // Expected:: Cutadapt module is valid
            bool isToolValid = AppSettingsDialogFiller::isExternalToolValid("Cutadapt");

            if (!isToolValid) {
                GT_FAIL("Bio is not valid", );
            }

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };

    // 1. Open "UGENE Application Settings", select "External Tools" tab.
    GTUtilsDialog::waitForDialog(new AppSettingsDialogFiller(new Custom()));
    GTMenu::clickMainMenuItem({"Settings", "Preferences..."}, GTGlobals::UseMouse);
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
            CHECK_SET_ERR(checked.isEmpty(), QString("Unexpected checked size, expected: 0, current: %1").arg(checked.size()));

            GTCheckBox::setChecked("promotersBox", dialog);
            GTCheckBox::setChecked("regulatoryBox", dialog);
            GTCheckBox::setChecked("geneBox", dialog);

            GTWidget::click(GTWidget::findPushButton("pbInvert"));
            checked = GTWidget::findChildren<QCheckBox>(dialog, [](QCheckBox* checkBox) { return checkBox->isChecked(); });
            CHECK_SET_ERR(checked.size() == 9, QString("Unexpected checked size, expected: 9, current: %1").arg(checked.size()));

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(new AnyDialogFiller("CustomAutoAnnotationDialog", new Scenario()));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Annotate plasmid");
}

GUI_TEST_CLASS_DEFINITION(test_8040) {
    GTLogTracer lt;
    GTFileDialog::openFile(testDir + "_common_data/regression/8040/8040.ugenedb");
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(lt.getJoinedErrorString().contains("The file was created with a newer version of UGENE"), "Expected message is not found");
    GTUtilsProjectTreeView::checkItem("8040.ugenedb");
    CHECK_SET_ERR(!GTUtilsDocument::isDocumentLoaded("8040.ugenedb"), "Document must be unloaded");
}

GUI_TEST_CLASS_DEFINITION(test_8049) {
    // Create a new Genbank document from text and check that its locus line contains
    // a correct alphabet and molecular topology.
    QString fileName = testDir + "_common_data/scenarios/sandbox/8049.gb";
    auto filler = new CreateDocumentFiller(
        "ACGT",
        false,
        CreateDocumentFiller::StandardRNA,
        true,
        false,
        "",
        fileName,
        CreateDocumentFiller::Genbank,
        "8049_name");
    GTUtilsDialog::waitForDialog(filler);
    GTMenu::clickMainMenuItem({"File", "New document from text..."}, GTGlobals::UseKey);

    QString fileContentLinear = GTFile::readAll(fileName);
    QList<QString> linesLinear = fileContentLinear.split("\n");
    CHECK_SET_ERR(linesLinear[0].startsWith("LOCUS       8049_name                  4 bp    DNA     linear       "), "1. Unexpected LOCUS line: " + linesLinear[0]);

    GTUtilsProjectTreeView::markSequenceAsCircular("8049_name");
    GTUtilsDialog::waitForDialog(new SaveProjectDialogFiller(QDialogButtonBox::No));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_MAIN, "Save all");

    QString fileContentCircular = GTFile::readAll(fileName);
    QList<QString> linesCircular = fileContentCircular.split("\n");
    CHECK_SET_ERR(linesCircular[0].startsWith("LOCUS       8049_name                  4 bp    DNA     circular     "), "2. Unexpected LOCUS line: " + linesCircular[0]);
}

GUI_TEST_CLASS_DEFINITION(test_8052) {
    // UGENE_GUI_TEST=1 env variable is required for this test
    // Open _common_data/fasta/5mbf.fa.gz
    // Select all
    // Create annotation
    // Copy annotated sequence to clipboard
    // Expected: Notification "Block size is too big and can't be copied into the clipboard" appeared
    GTFileDialog::openFile(testDir + "_common_data/fasta/5mbf.fa.gz");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    GTUtilsDialog::waitForDialog(new SelectSequenceRegionDialogFiller());
    GTKeyboardUtils::selectAll();
    GTUtilsDialog::checkNoActiveWaiters();

    class Scenario : public CustomScenario {
        void run() override {
            GTWidget::getActiveModalWidget();
            GTKeyboardDriver::keyClick(Qt::Key_Enter);
        }
    };

    GTUtilsDialog::waitForDialog(new CreateAnnotationWidgetFiller(new Scenario));
    GTKeyboardDriver::keyClick('n', Qt::ControlModifier);
    GTUtilsNotifications::waitForNotification(true, "Block size is too big and can't be copied into the clipboard");
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Copy/Paste", "Copy annotation sequence"}));
    GTMenu::showContextMenu(GTUtilsSequenceView::getPanOrDetView());
}

GUI_TEST_CLASS_DEFINITION(test_8064) {
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsWorkflowDesigner::toggleDebugMode();

    GTUtilsWorkflowDesigner::addSample("Align sequences with MUSCLE");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);  // Close wizard.

    GTUtilsWorkflowDesigner::click("Read alignment");
    GTUtilsWorkflowDesigner::addInputFile("Read alignment", dataDir + "samples/CLUSTALW/COI.aln");

    GTUtilsWorkflowDesigner::setBreakpoint("Write alignment");
    GTUtilsWorkflowDesigner::removeItem("Write alignment");

    GTUtilsWorkflowDesigner::addElement("Write Alignment");
    GTUtilsWorkflowDesigner::connect(GTUtilsWorkflowDesigner::getWorker("Align with MUSCLE"), GTUtilsWorkflowDesigner::getWorker("Write Alignment"));


    GTUtilsWorkflowDesigner::runWorkflow();
}

GUI_TEST_CLASS_DEFINITION(test_8069) {
    /*
    * 1. Open WD.
    * 2. Samples->Scenarios->In silico PCR.
    * 3. Set human_T1 (or any other valid input) as input and chrM.sorted.bam (or any other invalid input) as Primers URL.
    * 4. Run the workflow.
    * Expected: a task error about inappropriate primer file.
    */
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    class SetBinaryFileAsPrimerFile : public CustomScenario {
    public:
        void run() override {
            QWidget* wizard = GTWidget::getActiveModalWidget();
            GTWidget::click(wizard);
            GTUtilsWizard::setInputFiles({{dataDir + "samples/Assembly/chrM.fa"}});
            GTUtilsWizard::clickButton(GTUtilsWizard::Next);
            GTUtilsWizard::setParameter("Primers URL", dataDir + "samples/Assembly/chrM.sorted.bam");
            GTUtilsWizard::clickButton(GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(GTUtilsWizard::Run);
        }
    };

    GTLogTracer lt;
    GTUtilsDialog::waitForDialog(new WizardFiller("In Silico PCR", new SetBinaryFileAsPrimerFile()));
    GTUtilsWorkflowDesigner::addSample("In silico PCR");
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(lt.hasMessage("Can not read the primers file"), "Expected message 'Can not read the primers file' not found!");
    CHECK_SET_ERR(lt.hasMessage("Nothing to write"), "Expected message 'Nothing to write' not found!");
}

static void checkWorkflowPaused() {
    GTUtilsWorkflowDesigner::checkWorkflowDesignerWindowIsActive();
    GTWidget::checkEnabled(GTToolbar::getWidgetForActionTooltip(GTToolbar::getToolbar(MWTOOLBAR_ACTIVEMDI),
                                                                "Next step"));
}
static void waitForBreakpoint() {
    class GetVisibilityScenario final : public CustomScenario {
        const QWidget* w;
        bool& visible;

    public:
        GetVisibilityScenario(const QWidget* w, bool& visible)
            : w(w), visible(visible) {
        }
        void run() override {
            visible = w->isVisible();
        }
    };

    const auto w = GTToolbar::getWidgetForActionTooltip(GTToolbar::getToolbar("mwtoolbar_activemdi"), "Save workflow");
    bool visible = false;
    GTThread::runInMainThread(new GetVisibilityScenario(w, visible));
    GTThread::waitForMainThread();
    for (int time = 0; time < GT_OP_WAIT_MILLIS && !visible; time += GT_OP_CHECK_MILLIS) {
        GTThread::runInMainThread(new GetVisibilityScenario(w, visible));
        GTThread::waitForMainThread();
    }
}
GUI_TEST_CLASS_DEFINITION(test_8074) {
    // Set a break on element.
    // Run workflow.
    // Reopen from dashboard.
    // Run again.
    // ->No crash.
    // Set a break.
    // ->Check it in the break manager; no crash if rerun; the breakpoint triggers successfully.
    GTUtilsWorkflowDesigner::toggleDebugMode();
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    const auto elemName = QStringLiteral("Read Alignment");
    GTUtilsWorkflowDesigner::addElement(elemName);
    GTUtilsWorkflowDesigner::addInputFile(elemName, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsWorkflowDesigner::setBreakpoint(elemName);

    GTUtilsWorkflowDesigner::runWorkflow();
    checkWorkflowPaused();
    GTUtilsWorkflowDesigner::runWorkflow();  // To end execution

    GTUtilsTaskTreeView::waitTaskFinished();
    GTWidget::click(GTUtilsWorkflowDesigner::getGotoDashboardButton());
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Discard));
    GTWidget::click(GTUtilsDashboard::findLoadSchemaButton());
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsWorkflowDesigner::toggleBreakpointManager();
    auto breakNum = GTUtilsWorkflowDesigner::getBreakpointList().size();
    CHECK_SET_ERR(breakNum == 0,
                  QString("Expected no breakpoints, but there is %1 breakpoints in the breakpoint manager")
                      .arg(breakNum));
    GTUtilsWorkflowDesigner::runWorkflow();
    // No crash.
    GTUtilsTaskTreeView::waitTaskFinished();

    GTWidget::click(GTUtilsWorkflowDesigner::getGotoWorkflowButton());
    GTUtilsWorkflowDesigner::setBreakpoint(elemName);
    breakNum = GTUtilsWorkflowDesigner::getBreakpointList().size();
    CHECK_SET_ERR(breakNum == 1,
                  QString("Expected 1 breakpoint, but there is %1 breakpoints in the breakpoint manager")
                      .arg(breakNum));

    GTUtilsWorkflowDesigner::runWorkflow();
    checkWorkflowPaused();
    GTUtilsWorkflowDesigner::runWorkflow();  // To end execution
}

GUI_TEST_CLASS_DEFINITION(test_8077_1) {
    GTUtilsWorkflowDesigner::toggleDebugMode();

    GTUtilsDialog::waitForDialog(new WizardFiller("Align Sequences with MUSCLE Wizard",
                                                  {dataDir + "samples/CLUSTALW/COI.aln"}));
    GTFileDialog::openFile(dataDir + "workflow_samples/Alignment", "basic_align.uwl");
    GTUtilsWorkflowDesigner::checkWorkflowDesignerWindowIsActive();
    GTUtilsProject::checkProject(GTUtilsProject::ExistsAndEmpty);

    GTUtilsWorkflowDesigner::setBreakpoint("Write alignment");
    GTUtilsWorkflowDesigner::runWorkflow();
    // No crash.
    // Finish the workflow task:
    waitForBreakpoint();
    GTUtilsWorkflowDesigner::runWorkflow();
}
GUI_TEST_CLASS_DEFINITION(test_8077_2) {
    GTUtilsWorkflowDesigner::toggleDebugMode();
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsDialog::waitForDialog(new WizardFiller("Align Sequences with MUSCLE Wizard",
                                                  {dataDir + "samples/CLUSTALW/COI.aln"}));
    GTUtilsWorkflowDesigner::addSample("Align sequences with MUSCLE");

    GTUtilsWorkflowDesigner::checkWorkflowDesignerWindowIsActive();
    GTUtilsWorkflowDesigner::setBreakpoint("Write alignment");
    GTUtilsProject::checkProject(GTUtilsProject::NotExists);

    GTUtilsWorkflowDesigner::runWorkflow();
    waitForBreakpoint();

    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();
    GTUtilsProject::checkProject();
    // No crash.
    // Finish the workflow task:
    GTUtilsMdi::activateWindow("Workflow Designer");
    GTUtilsWorkflowDesigner::runWorkflow();
}

// Check that there is only one breakpoint "Read Alignment" in the breakpoint manager.
static void checkReadAlignmentBreakpoint() {
    GTThread::waitForMainThread();
    const auto items = GTTreeWidget::getItems(GTWidget::findTreeWidget("breakpoints list"));
    const auto breakNames = std::accumulate(items.cbegin(),
                                            items.cend(),
                                            QStringList(),
                                            [](QStringList res, const QTreeWidgetItem* item) {
                                                return res += item->text(1).toLower();
                                            });
    CHECK_SET_ERR(breakNames == QStringList {"read alignment"},
                  QString("One `read alignment` breakpoint was expected, but the following breakpoints exist: `%1`")
                      .arg(breakNames.join(",")));
}
// Opens Align with MUSCLE sample, set breakpoint on the first element "Read alignment" using dialog, check that the
// correct breakpoint is set.
static void openSampleSetBreakCheckBreak() {
    GTUtilsDialog::waitForDialog(new DefaultWizardFiller("Align Sequences with MUSCLE Wizard"));
    GTUtilsWorkflowDesigner::addSample("Align sequences with MUSCLE");

    CHECK_SET_ERR(GTUtilsWorkflowDesigner::getBreakpointList().empty(),
                  "Unexpected breakpoint in the breakpoint manager");

    GTUtilsDialog::waitForDialog(new DefaultDialogFiller("NewBreakpointDialog"));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Break at element");
    checkReadAlignmentBreakpoint();
}
// Starts the current "Align Sequences with MUSCLE Wizard" workflow. Checks that there is no crash and the workflow has
// paused. Then, brings it to the end.
static void runNoCrash() {
    GTUtilsWorkflowDesigner::addInputFile("Read alignment", UGUITest::dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsWorkflowDesigner::runWorkflow();
    // No crash.

    checkWorkflowPaused();
    // Finish the workflow task:
    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsWorkflowDesigner::runWorkflow();
}
GUI_TEST_CLASS_DEFINITION(test_8079_1) {
    GTUtilsWorkflowDesigner::toggleDebugMode();
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsWorkflowDesigner::checkWorkflowDesignerWindowIsActive();
    GTUtilsWorkflowDesigner::toggleBreakpointManager();

    openSampleSetBreakCheckBreak();
    openSampleSetBreakCheckBreak();
    runNoCrash();
}
GUI_TEST_CLASS_DEFINITION(test_8079_2) {
    GTUtilsWorkflowDesigner::toggleDebugMode();
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsWorkflowDesigner::checkWorkflowDesignerWindowIsActive();
    GTUtilsWorkflowDesigner::toggleBreakpointManager();

    // Add the element, then open the sample.
    const QString elemName = "Read Alignment";
    GTUtilsWorkflowDesigner::addElement(elemName);
    GTUtilsWorkflowDesigner::setBreakpoint(elemName);

    openSampleSetBreakCheckBreak();
    runNoCrash();
}
GUI_TEST_CLASS_DEFINITION(test_8079_3) {
    GTUtilsWorkflowDesigner::toggleDebugMode();
    // Open the workflow file, then the sample.
    GTUtilsDialog::waitForDialog(new DefaultWizardFiller("Align Sequences with MUSCLE Wizard"));
    GTFileDialog::openFile(dataDir + "workflow_samples/Alignment", "basic_align.uwl");
    GTUtilsWorkflowDesigner::checkWorkflowDesignerWindowIsActive();
    GTUtilsWorkflowDesigner::toggleBreakpointManager();

    GTUtilsWorkflowDesigner::setBreakpoint("Read alignment");

    openSampleSetBreakCheckBreak();
    runNoCrash();
}

GUI_TEST_CLASS_DEFINITION(test_8083_1) {
    const QString elemName = "Read Alignment";
    GTUtilsWorkflowDesigner::toggleDebugMode();
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsWorkflowDesigner::addElement(elemName);
    GTUtilsWorkflowDesigner::addInputFile(elemName, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsWorkflowDesigner::setBreakpoint(elemName);
    GTMenu::clickMainMenuItem({"Actions", "Copy"});
    GTMenu::clickMainMenuItem({"Actions", "Paste"});

    GTUtilsWorkflowDesigner::runWorkflow();
    checkWorkflowPaused();
    // No crash.
    // Finish the workflow task:
    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();

    GTMenu::clickMainMenuItem({"Actions", "Select all elements"});
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Break at element");
    // ->The first element doesn't have breakpoints, but the second does.
    GTMenu::clickMainMenuItem({"Actions", "Copy"});
    GTMenu::clickMainMenuItem({"Actions", "Paste"});
    // ->4 elements, 1 breakpoint.

    GTUtilsWorkflowDesigner::runWorkflow();
    checkWorkflowPaused();
    // No crash.
    // Finish the workflow task:
    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsWorkflowDesigner::runWorkflow();
}
GUI_TEST_CLASS_DEFINITION(test_8083_2) {
    // Add breakpoint for one element.
    // Copy paste the element.
    // Open the breakpoint manager.
    // ->It has one breakpoint.
    // Select it and click "Highlight selected item".
    // ->The correct element should be highlighted.

    // Selects the first breakpoint in the manager and presses the "Highlight" button.
    const auto higlightFirstBreakpoint = []() {
        auto treeItems = GTTreeWidget::getItems(GTWidget::findTreeWidget("breakpoints list"));
        GTTreeWidget::click(treeItems[0]);
        GTWidget::click(GTWidget::findButtonByText("Highlight selected item"));
    };

    const QString elemName = "Read Alignment";
    GTUtilsWorkflowDesigner::toggleDebugMode();
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsWorkflowDesigner::addElement(elemName);
    GTMenu::clickMainMenuItem({"Actions", "Copy"});
    GTUtilsWorkflowDesigner::setBreakpoint(elemName);
    GTMenu::clickMainMenuItem({"Actions", "Paste"});

    GTUtilsWorkflowDesigner::toggleBreakpointManager();
    checkReadAlignmentBreakpoint();
    higlightFirstBreakpoint();
    // No crash.

    GTMenu::clickMainMenuItem({"Actions", "Select all elements"});
    GTMenu::clickMainMenuItem({"Actions", "Copy"});
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Break at element");
    // ->The first element doesn't have breakpoints, but the second does.
    GTMenu::clickMainMenuItem({"Actions", "Paste"});
    // ->4 elements, 1 breakpoint.

    checkReadAlignmentBreakpoint();
    higlightFirstBreakpoint();
    // No crash.
}

GUI_TEST_CLASS_DEFINITION(test_8090) {
    /*
     * 1. Open external tools in setting->preferences
     * 2. Check next tools don't have "unknown" version:
     * vcf-consensus,Trimmomatic, Spidey (not on Mac), SnpEff, Kalign, BLAST
     */
    class CheckNotUnknownVersion : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            AppSettingsDialogFiller::openTab(AppSettingsDialogFiller::ExternalTools);

            if (!isOsMac() && AppSettingsDialogFiller::isToolDescriptionContainsString("Spidey", "unknown")) {
                GT_FAIL("Unknown Spidey version!", );
            }

            for (const QString& toolName : {"vcf-consensus", "Trimmomatic", "SnpEff", "Kalign", "BlastN"}) {
                if (AppSettingsDialogFiller::isToolDescriptionContainsString(toolName, "unknown")) {
                    GT_FAIL("Unknown " + toolName + " version!", );
                }
            }

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(new AppSettingsDialogFiller(new CheckNotUnknownVersion));
    GTMenu::clickMainMenuItem({"Settings", "Preferences..."}, GTGlobals::UseMouse);
}

GUI_TEST_CLASS_DEFINITION(test_8092) {
    // Description: check "Alternative mutations" using slider to set threshold
    // Copy test file to sandbox
    // Open sandBoxDir + "test_8069.ugenedb"
    // Check symbol (2141, 8)
    // Expected: T
    // Enable altrnative mutations
    // // Check symbol (2141, 8) again
    // Expected: still T
    QString file = sandBoxDir + "test_8069.ugenedb";
    GTFile::copy(dataDir + "samples/Sanger/alignment.ugenedb", file);
    GTFileDialog::openFile(file);

    GTUtilsMcaEditorSequenceArea::clickToPosition(QPoint(2141, 8));
    char ch = GTUtilsMcaEditorSequenceArea::getSelectedReadChar();
    CHECK_SET_ERR(ch == 'T', QString("Incorrect chararcter 1, expected: T, current: %1").arg(ch));

    GTUtilsOptionPanelMca::showAlternativeMutations(true);
    ch = GTUtilsMcaEditorSequenceArea::getSelectedReadChar();
    CHECK_SET_ERR(ch == 'T', QString("Incorrect chararcter 2, expected: T, current: %1").arg(ch));
}

GUI_TEST_CLASS_DEFINITION(test_8093) {
    // Open any sequence file
    // Open "Find enzymes" dialog
    // Expected: there is an information about enzymes hidden
    // Enable all suppliers
    // Expected: no hidden enzymes and no info about it
    GTFileDialog::openFile(dataDir + "/samples/FASTA", "human_T1.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    class custom : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();

            auto text = GTLabel::getText("statusLabel", dialog);
            CHECK_SET_ERR(text.contains("Some enzymes are hidden due to \"Enzyme table filter\" settings."), QString("Unexpected text: %1").arg(text));

            GTWidget::click(GTWidget::findPushButton("pbSelectAll", dialog));
            text = GTLabel::getText("statusLabel", dialog);
            CHECK_SET_ERR(!text.contains("Some enzymes are hidden due to \"Enzyme table filter\" settings."), QString("Unexpected text: %1").arg(text));
            CHECK_SET_ERR(text.contains("hidden: 0"), QString("Unexpected text: %1").arg(text));

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(new FindEnzymesDialogFiller(QStringList{}, new custom()));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Find restriction sites");
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_8097) {
    class EnzymeList : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            GTWidget::click(GTWidget::findWidget("selectNoneButton", dialog));
            GTComboBox::selectItemByText("filterComboBox", dialog, "name");
            QStringList enzymesToShow = {"BamHI", "BglII", "ClaI", "DraI"};
            GTLineEdit::setText("enzymesFilterEdit", enzymesToShow.join(","), dialog);
            auto enzymesTree = GTWidget::findTreeWidget("tree", GTWidget::findWidget("enzymesSelectorWidget"));
            for (const QString& enzyme : qAsConst(enzymesToShow)) {
                GTTreeWidget::findItem(enzymesTree, enzyme);
            }
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };

    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    GTUtilsDialog::add(new PopupChooser({"ADV_MENU_ANALYSE", "Find restriction sites"}));
    GTUtilsDialog::add(new FindEnzymesDialogFiller(QStringList {}, new EnzymeList()));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();
}

GUI_TEST_CLASS_DEFINITION(test_8096_1) {
    // Open _common_data/scenarios/_regression/8096/test_8096.gb
    // Expected: sequence already has two fragments
    // Open Cloning->Create molecule dialog
    // Click Add All button
    // Select the first fragment and click Adjust 3'end button
    // Remove this fragment
    // Expected: Click Adjust 3' end button is disabled
    GTFileDialog::openFile(testDir + "/_common_data/scenarios/_regression/8096", "test_8096.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    QList<ConstructMoleculeDialogFiller::Action> actions;
    actions << ConstructMoleculeDialogFiller::Action(ConstructMoleculeDialogFiller::AddAllFragments, "");
    actions << ConstructMoleculeDialogFiller::Action(ConstructMoleculeDialogFiller::SelectAddedFragment, "Fragment (1-20)");
    actions << ConstructMoleculeDialogFiller::Action(ConstructMoleculeDialogFiller::ClickAdjustRight, "");
    actions << ConstructMoleculeDialogFiller::Action(ConstructMoleculeDialogFiller::ClickRemove, "");
    actions << ConstructMoleculeDialogFiller::Action(ConstructMoleculeDialogFiller::CheckAdjustRightEnabled, false);
    actions << ConstructMoleculeDialogFiller::Action(ConstructMoleculeDialogFiller::ClickCancel, "");
    GTUtilsDialog::waitForDialog(new ConstructMoleculeDialogFiller(actions));
    GTUtilsDialog::waitForDialog(new PopupChooser({"Cloning", "CLONING_CONSTRUCT"}));
    GTMenu::showContextMenu(GTUtilsMdi::activeWindow());
}

GUI_TEST_CLASS_DEFINITION(test_8096_2) {
    // Open _common_data/scenarios/_regression/8096/test_8096.gb
    // Expected: sequence already has two fragments
    // Open Cloning->Create molecule dialog
    // Click Add All button
    // Select the last fragment and click Adjust 5'end button
    // Remove this fragment
    // Expected: Click Adjust 5' end button is disabled
    GTFileDialog::openFile(testDir + "/_common_data/scenarios/_regression/8096", "test_8096.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    QList<ConstructMoleculeDialogFiller::Action> actions;
    actions << ConstructMoleculeDialogFiller::Action(ConstructMoleculeDialogFiller::AddAllFragments, "");
    actions << ConstructMoleculeDialogFiller::Action(ConstructMoleculeDialogFiller::SelectAddedFragment, "Fragment (50-60)");
    actions << ConstructMoleculeDialogFiller::Action(ConstructMoleculeDialogFiller::ClickAdjustLeft, "");
    actions << ConstructMoleculeDialogFiller::Action(ConstructMoleculeDialogFiller::ClickRemove, "");
    actions << ConstructMoleculeDialogFiller::Action(ConstructMoleculeDialogFiller::CheckAdjustLeftEnabled, false);
    actions << ConstructMoleculeDialogFiller::Action(ConstructMoleculeDialogFiller::ClickCancel, "");
    GTUtilsDialog::waitForDialog(new ConstructMoleculeDialogFiller(actions));
    GTUtilsDialog::waitForDialog(new PopupChooser({"Cloning", "CLONING_CONSTRUCT"}));
    GTMenu::showContextMenu(GTUtilsMdi::activeWindow());
}

GUI_TEST_CLASS_DEFINITION(test_8096_3) {
    // Open _common_data/scenarios/_regression/8096/test_8096.gb
    // Expected: sequence already has two fragments
    // Open Cloning->Create molecule dialog
    // Click Add All button
    // Select the first
    // Click Down
    // Expected: Adjust 5' is enabled, adjust and 3' is disabled
    // Click Up
    // Expected: Adjust 5' is disabled, adjust and 3' is enabled
    // Check "Make circular"
    // Expected: Click Adjust 5' and 3' ends buttons are enabled
    // Check "Make fors blunt overhangs"
    // Expected: Click Adjust 5' and 3' ends buttons are disabled


    GTFileDialog::openFile(testDir + "/_common_data/scenarios/_regression/8096", "test_8096.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    QList<ConstructMoleculeDialogFiller::Action> actions;
    actions << ConstructMoleculeDialogFiller::Action(ConstructMoleculeDialogFiller::AddAllFragments, "");
    actions << ConstructMoleculeDialogFiller::Action(ConstructMoleculeDialogFiller::SelectAddedFragment, "Fragment (1-20)");
    actions << ConstructMoleculeDialogFiller::Action(ConstructMoleculeDialogFiller::ClickDown, "");
    actions << ConstructMoleculeDialogFiller::Action(ConstructMoleculeDialogFiller::CheckAdjustLeftEnabled, true);
    actions << ConstructMoleculeDialogFiller::Action(ConstructMoleculeDialogFiller::CheckAdjustRightEnabled, false);
    actions << ConstructMoleculeDialogFiller::Action(ConstructMoleculeDialogFiller::ClickUp, "");
    actions << ConstructMoleculeDialogFiller::Action(ConstructMoleculeDialogFiller::CheckAdjustLeftEnabled, false);
    actions << ConstructMoleculeDialogFiller::Action(ConstructMoleculeDialogFiller::CheckAdjustRightEnabled, true);
    actions << ConstructMoleculeDialogFiller::Action(ConstructMoleculeDialogFiller::CheckMakeCircular, true);
    actions << ConstructMoleculeDialogFiller::Action(ConstructMoleculeDialogFiller::SelectAddedFragment, "Fragment (1-20)");
    actions << ConstructMoleculeDialogFiller::Action(ConstructMoleculeDialogFiller::CheckAdjustLeftEnabled, true);
    actions << ConstructMoleculeDialogFiller::Action(ConstructMoleculeDialogFiller::CheckAdjustRightEnabled, true);
    actions << ConstructMoleculeDialogFiller::Action(ConstructMoleculeDialogFiller::CheckForceBlunt, true);
    actions << ConstructMoleculeDialogFiller::Action(ConstructMoleculeDialogFiller::SelectAddedFragment, "Fragment (1-20)");
    actions << ConstructMoleculeDialogFiller::Action(ConstructMoleculeDialogFiller::CheckAdjustLeftEnabled, false);
    actions << ConstructMoleculeDialogFiller::Action(ConstructMoleculeDialogFiller::CheckAdjustRightEnabled, false);
    actions << ConstructMoleculeDialogFiller::Action(ConstructMoleculeDialogFiller::ClickCancel, "");
    GTUtilsDialog::waitForDialog(new ConstructMoleculeDialogFiller(actions));
    GTUtilsDialog::waitForDialog(new PopupChooser({"Cloning", "CLONING_CONSTRUCT"}));
    GTMenu::showContextMenu(GTUtilsMdi::activeWindow());
}

GUI_TEST_CLASS_DEFINITION(test_8100) {
    // Open a file on which primer3 will run for a long time.
    // Open the Task View.
    // Run the primer3 task twice.
    // Cancel the "Pick primers task" from the second task.
    // ->No crash.
    GTFileDialog::openFile(testDir + "/_common_data/primer3/primer3_xml/primer1_th", "AGAG.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    auto taskView = GTUtilsTaskTreeView::openView();

    class SetSettings final : public CustomScenario {
    public:
        void run() override {
            auto w = GTWidget::findTabWidget("tabWidget");
            GTTabWidget::clickTab(w, "General Settings");
            GTCheckBox::setChecked("checkbox_PRIMER_THERMODYNAMIC_OLIGO_ALIGNMENT", w);
            GTWidget::click(GTWidget::findButtonByText("Pick primers"));
        }
    };
    GTUtilsDialog::add(new AnyDialogFiller("Primer3Dialog", new SetSettings));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Primer3");
    GTUtilsDialog::add(new Primer3DialogFiller);
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Primer3");

    GTThread::waitForMainThread();
    auto secondTaskWidgetItem = taskView->topLevelItem(1);
    GTTreeWidget::expand(secondTaskWidgetItem);
    GTMouseDriver::moveTo(GTTreeWidget::getItemCenter(secondTaskWidgetItem->child(0)));
    GTUtilsDialog::waitForDialog(new PopupChooser({"Cancel task"}, GTGlobals::UseMouse));
    GTMouseDriver::click(Qt::RightButton);

    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_8101) {
    /*
     * 1. Open samples/FASTA/human_T1.fa
     * 2. Open Find enzymes dialog, set following enzymes to search:
     * "BamHI", "BglII", "ClaI", "DraI", "EcoRI", "EcoRV", "HindIII", "PstI", "SalI", "SmaI", "XmaI"
     * 3. Run search
     * Expected state: 636 annotations found
     * 4. Open Find enzymes dialog, set excluded region "10000 - 12000"
     * 5. Run search
     * Expected state: 191 annotations found
     * 6. Open Find enzymes dialog
     * 7. Set search location "1000..8000,90000..108000,14000..34000"
     * 8. Set excluded location set excluded location "9000..12000,50000..55000"
     * 9. Run search
     * Expected state: 25 annotations found
     */

    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    FindEnzymesDialogFillerSettings settings({"BamHI", "BglII", "ClaI", "DraI", "EcoRI", "EcoRV", "HindIII", "PstI", "SalI", "SmaI", "XmaI"});

    GTUtilsDialog::add(new PopupChooser({"ADV_MENU_ANALYSE", "Find restriction sites"}));
    GTUtilsDialog::add(new FindEnzymesDialogFiller(settings));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::getAnnotatedRegions().size() == 636, "Annoatated region counter doesn't match.");

    settings.excludeRegionStart = 10000;
    settings.excludeRegionEnd = 12000;
    GTUtilsDialog::add(new PopupChooser({"ADV_MENU_ANALYSE", "Find restriction sites"}));
    GTUtilsDialog::add(new FindEnzymesDialogFiller(settings));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::getAnnotatedRegions().size() == 191, "Annoatated region counter doesn't match.");

    settings.searchRegions = {U2Region(1000, 7000), U2Region(90000, 18000), U2Region(14000, 20000)};
    settings.excludeRegions = {U2Region(9000, 3000), U2Region(50000, 5000)};
    settings.excludeRegionStart = -1;
    settings.excludeRegionEnd = -1;
    GTUtilsDialog::add(new PopupChooser({"ADV_MENU_ANALYSE", "Find restriction sites"}));
    GTUtilsDialog::add(new FindEnzymesDialogFiller(settings));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::getAnnotatedRegions().size() == 25, "Annoatated region counter doesn't match.");
}

GUI_TEST_CLASS_DEFINITION(test_8104) {
    /*
    * 1. Open human_T1.fa
    * 2. Open "Find restriction sites" dialog
    * 3. Set filter to "name" and search for "AS"
    * Expected state: enzymes with AS in name were found
    * 4. Change filter to "sequence" and search for "ACCT"
    * Expected state: enzymes with ACCT in sequence were found
    */
    class CheckFilter : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            auto enzymesSelectorWidget = GTWidget::findWidget("enzymesSelectorWidget");
            auto enzymesTree = GTWidget::findTreeWidget("tree", enzymesSelectorWidget);

            GTComboBox::selectItemByText("filterComboBox", dialog, "name");
            GTLineEdit::setText("enzymesFilterEdit", "AS", dialog);
            int treeItemsCount = GTTreeWidget::countVisibleItems(enzymesTree);
            CHECK_SET_ERR(treeItemsCount == 180, "Unexpected number of visible items");

            GTComboBox::selectItemByText("filterComboBox", dialog, "sequence");
            GTLineEdit::setText("enzymesFilterEdit", "ACCT", dialog);
            enzymesSelectorWidget = GTWidget::findWidget("enzymesSelectorWidget");
            enzymesTree = GTWidget::findTreeWidget("tree", enzymesSelectorWidget);
            treeItemsCount = GTTreeWidget::countVisibleItems(enzymesTree);
            CHECK_SET_ERR(treeItemsCount == 351, "Unexpected number of visible items");

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };

    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    GTUtilsDialog::add(new PopupChooser({"ADV_MENU_ANALYSE", "Find restriction sites"}));
    GTUtilsDialog::add(new FindEnzymesDialogFiller(QStringList{}, new CheckFilter));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();
}

GUI_TEST_CLASS_DEFINITION(test_8111) {
    /*
     * 1. Open "data/samples/FASTA/human_T1.fa".
     * 2. Rename to sequence with longer name
     * Expected state: sequence name label fits well with new name
     */

    GTFileDialog::openFile(dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    QWidget *nameLabel = GTWidget::findWidget("nameLabel");
    const QSize initialSize = nameLabel->size();
    GTUtilsProjectTreeView::rename("human_T1 (UCSC April 2002 chr7:115977709-117855134)", "human_T1 xxxxxxxxx(UCSC April 2002 chr7:115977709-117855134)xxxxxxxxx");
    CHECK_SET_ERR(initialSize != nameLabel->size(), "Sequence name label size should change!");
}

GUI_TEST_CLASS_DEFINITION(test_8118) {
    /*
    * 1. Open Mca alignment
    * 2. Press Ctrl+g and set valid position to go
    * Expected state: visible position changed to desired
    */
    GTFileDialog::openFile(testDir + "_common_data/sanger/alignment_short.ugenedb");
    GTUtilsMcaEditor::checkMcaEditorWindowIsActive();

    GTUtilsDialog::waitForDialog(new GoToDialogFiller(599));
    GTKeyboardDriver::keyClick('g', Qt::ControlModifier);

    CHECK_SET_ERR(GTUtilsMcaEditor::getReferenceArea()->getVisibleRange().endPos() == 599, QString("Unexpected text: slider position doesn't change after 'Go to'"));

    GTUtilsDialog::waitForDialog(new GoToDialogFiller(2081));
    GTKeyboardDriver::keyClick('g', Qt::ControlModifier);
    // gapped length returned so it differs with 'go to' value
    CHECK_SET_ERR(GTUtilsMcaEditor::getReferenceArea()->getVisibleRange().endPos() == 2082, QString("Unexpected text: slider position doesn't change after 'Go to'"));

    GTUtilsDialog::waitForDialog(new GoToDialogFiller(2082));
    GTKeyboardDriver::keyClick('g', Qt::ControlModifier);
    CHECK_SET_ERR(GTUtilsMcaEditor::getReferenceArea()->getVisibleRange().endPos() == 2084, QString("Unexpected text: slider position doesn't change after 'Go to'"));

    GTUtilsDialog::waitForDialog(new GoToDialogFiller(5666));
    GTKeyboardDriver::keyClick('g', Qt::ControlModifier);
    CHECK_SET_ERR(GTUtilsMcaEditor::getReferenceArea()->getVisibleRange().endPos() == 5724, QString("Unexpected text: slider position doesn't change after 'Go to'"));
}

GUI_TEST_CLASS_DEFINITION(test_8120_1) {
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();
    GTUtilsTaskTreeView::waitTaskFinished();
    // Switch to any other window
    // ->No "Render overview" task.
    GTLogTracer lt;
    GTUtilsStartPage::openStartPage();
    GTUtilsTaskTreeView::checkTaskIsPresent("Render overview", false);
    CHECK_SET_ERR(!lt.hasMessage("Render overview"), "Unexpected message in the log");
}

GUI_TEST_CLASS_DEFINITION(test_8120_2) {
    // Change MSA when its window inactive->no overview task.
    // Return back to MSA window->there is an overview task.
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsStartPage::openStartPage();
    GTLogTracer lt;

    const auto& objs = GTUtilsDocument::getDocument("COI.aln")->getObjects();
    CHECK_SET_ERR(objs.size() == 1, QString("Unexpected number of gobjects (%1) in document COI.aln").arg(objs.size()));
    auto msa = qobject_cast<MsaObject*>(objs[0]);
    CHECK_SET_ERR(msa, "Error casting to msa");
    GTThread::runInMainThread([msa]() { msa->removeRow(0); });
    GTUtilsTaskTreeView::checkTaskIsPresent("Render overview", false);
    CHECK_SET_ERR(!lt.hasMessage("Render overview"), "Unexpected message in the log");

    GTUtilsMdi::activateWindow("COI");
    CHECK_SET_ERR(lt.hasMessage("Render overview"), "No expected message in the log");
}

GUI_TEST_CLASS_DEFINITION(test_8136) {
    /*
    * 1. Open _common_data/scenarios/_regression/8136/8136.seq
    * 2. Open "Restriction enzymes"dialog
    * 3. Choose only one "AasI" enzyme.
    * 4. Check "Uncut area" and set it from 29 to 100.
    * 5. Click OK.
    * Expected state: no enzymes were found.
    * Expected state: there is a info in log about skipped enzyme
    * 6. Open "Restriction enzymes"dialog
    * 7. Check "Uncut area" and set it from 29 to 33.
    * 8. Set "Search area" from 30 to 31.
    * 5. Click OK.
    * Expected state: Message box with "Nothing to search" message appeared.
    */
    GTFileDialog::openFile(testDir + "_common_data/scenarios/_regression/8136/8136.seq");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    FindEnzymesDialogFillerSettings settings({"AasI"});
    settings.excludeRegionStart = 29;
    settings.excludeRegionEnd = 100;
    GTUtilsDialog::add(new PopupChooser({"ADV_MENU_ANALYSE", "Find restriction sites"}));
    GTUtilsDialog::add(new FindEnzymesDialogFiller(settings));
    GTLogTracer lt;
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::getAnnotatedRegions().size() == 0, "Annoatated region counter doesn't match.");
    lt.checkMessage("The following enzymes were found, but skipped because they were found inside of the \"Uncut area\":");

    class CheckErrorMessageBox : public CustomScenario {
    public:
        void run() override {
            GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok, "'Uncut' region/location fully contains 'Search in' inside it!"));
            filler->commonScenario();
            GTUtilsDialog::clickButtonBox(GTWidget::getActiveModalWidget(), QDialogButtonBox::Cancel);
        }
        FindEnzymesDialogFiller *filler;
    };

    settings.searchRegionStart = 30;
    settings.searchRegionEnd = 31;
    settings.excludeRegionStart = 29;
    settings.excludeRegionEnd = 33;

    GTUtilsDialog::add(new PopupChooser({"ADV_MENU_ANALYSE", "Find restriction sites"}));
    CheckErrorMessageBox* scenario = new CheckErrorMessageBox();
    FindEnzymesDialogFiller* filler = new FindEnzymesDialogFiller(settings, scenario);
    scenario->filler = filler;
    GTUtilsDialog::add(filler);
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();
}

GUI_TEST_CLASS_DEFINITION(test_8141) {
    /*
    * 1. Open _common_data/regression/8141/wd_uql.uwl
    * 2. Set input file samples/Genbank/NC_014267.1.gb
    * 3. Set uql schema _common_data/regression/8141/repeats_with_nc.uql
    * 4. Run schema
    * Expected state: result file contains annotations "Results", summary 1186 annotations
    */
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    GTUtilsWorkflowDesigner::loadWorkflow(testDir + "_common_data/regression/8141/wd_uql.uwl");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsWorkflowDesigner::click("Read Sequence");
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "samples/Genbank/NC_014267.1.gb");

    GTUtilsWorkflowDesigner::click("Annotate with UQL");
    GTUtilsWorkflowDesigner::setParameter("Schema", testDir + "_common_data/regression/8141/repeats_with_nc.uql", GTUtilsWorkflowDesigner::textValue);

    GTUtilsWorkflowDesigner::click("Write Sequence");
    GTUtilsWorkflowDesigner::setParameter("Output file", sandBoxDir + "8141_result.gb", GTUtilsWorkflowDesigner::valueType::lineEditWithFileSelector);

    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();

    GTFileDialog::openFile(sandBoxDir + "8141_result.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::getAnnotatedRegions().size() == 1186, "Annoatated region counter doesn't match.");
}

GUI_TEST_CLASS_DEFINITION(test_8151) {
    /*
    * 1. Open human_T1.fa
    * 2. Open SW dialog, activate "translation" radio button
    * 3. CLick Cancel
    * 4. Open AMINO.fa
    * 5. Open SW dialog
    * Expected state: no crash
    */
    class ActivateTranslationSWScenario : public CustomScenario {
    public:
        ActivateTranslationSWScenario(bool clickTranslationRadio_)
            : clickTranslationRadio(clickTranslationRadio_) {
        };

        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            if (clickTranslationRadio) {
                GTRadioButton::click("radioTranslation", dialog);
            }
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }

    private:
        bool clickTranslationRadio;
    };

    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();
    GTUtilsDialog::waitForDialog(new SmithWatermanDialogFiller(new ActivateTranslationSWScenario(true)));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Find pattern [Smith-Waterman]");

    GTFileDialog::openFile(testDir + "_common_data/fasta/AMINO.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();
    GTUtilsDialog::waitForDialog(new SmithWatermanDialogFiller(new ActivateTranslationSWScenario(false)));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Find pattern [Smith-Waterman]");
}

GUI_TEST_CLASS_DEFINITION(test_8159) {
    // Generate sequence 600000000 bases long
    // Open overview and enable density graph
    // Create an annotation
    // Expected: no crash
    class Scenario1 : public CustomScenario {
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();

            GTSpinBox::setValue("lengthSpin", 600000000, dialog);
            GTRadioButton::click("baseContentRadioButton", dialog);
            GTLineEdit::setText("outputEdit", sandBoxDir + "test_8159.ugenedb", dialog, true);

            GTWidget::click(GTWidget::findButtonByText("Generate", dialog));
        }
    };
    GTUtilsDialog::waitForDialog(new Filler("DNASequenceGeneratorDialog", new Scenario1));
    GTMenu::clickMainMenuItem({"Tools", "Random sequence generator..."});
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    if (!GTUtilsSequenceView::getOverviewByNumber()) {
        GTWidget::click(GTAction::button(GTAction::findAction("show_hide_overview")));
    }

    QAction* destGraph = GTAction::findAction("density_graph_action");
    if (!destGraph->isChecked()) {
        GTWidget::click(GTAction::button(destGraph));
    }
    GTUtilsSequenceView::getLengthOfSequence();

    class Scenario2 : public CustomScenario {
        void run() override {
            GTWidget::getActiveModalWidget();
            GTKeyboardDriver::keyClick(Qt::Key_Enter);
        }
    };

    GTUtilsDialog::waitForDialog(new CreateAnnotationWidgetFiller(new Scenario2));
    GTKeyboardDriver::keyClick('n', Qt::ControlModifier);
}

GUI_TEST_CLASS_DEFINITION(test_8153) {
    // Open QD
    // Add "Restriction Sites" element
    // CLick ont the "Enzymes" parameter and open the "Find restriction enzymes" dialog
    // Expected: Restriction enzymes are avaliable in the tree

    GTUtilsQueryDesigner::openQueryDesigner();
    GTUtilsQueryDesigner::addAlgorithm("Restriction Sites");
    GTWidget::moveToAndClick(GTUtilsQueryDesigner::getItemCenter("Restriction Sites"));
    GTUtilsQueryDesigner::clickParameter("Enzymes");
    auto table = GTWidget::findTableView("table");
    GTWidget::click(GTWidget::findWidget("tbOpenDialog", table));

    class custom : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();

            auto enzymesTree = GTWidget::findTreeWidget("tree", dialog);
            auto items = GTTreeWidget::getItems(enzymesTree);
            CHECK_SET_ERR(items.size() != 0, QString("Should be more than 0 enzymes"));

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(new FindEnzymesDialogFiller(QStringList {}, new custom()));
}

GUI_TEST_CLASS_DEFINITION(test_8160) {
    /*
    * 1. Open "samples/Assembly/chrM.sam"
    * 2. Click the first position coverage (6950).
    * 3. Click right mouse button on the assembly area -> Export -> Assebmly region.
    * 4. Click Export.
    * Expected: new assembly length 110
    * Repeat steps with BAM and SAM formats
    */
    class Scenario : public CustomScenario {
    public:
        Scenario(const QString& format)
            : formatToSave(format) {
        };
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            GTLineEdit::setText(GTWidget::findLineEdit("start_edit_line", dialog), "6901");
            GTLineEdit::setText(GTWidget::findLineEdit("end_edit_line", dialog), "7010");
            GTLineEdit::setText(GTWidget::findLineEdit("filepathLineEdit"), sandBoxDir + "8160/file");
            GTComboBox::selectItemByText(GTWidget::findComboBox("documentFormatComboBox", dialog), formatToSave);
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    private:
        QString formatToSave;
    };

    QDir(sandBoxDir).mkdir("8160");
    const QStringList formats = { "UGENE Database", "SAM", "BAM" };
    for (const QString& formatToSave : qAsConst(formats)) {
        GTFileDialog::openFile(testDir + "_common_data/ugenedb/chrM.sorted.bam.ugenedb");
        GTUtilsTaskTreeView::waitTaskFinished();
        GTUtilsAssemblyBrowser::zoomToReads();
        GTUtilsDialog::add(new PopupChooserByText({ "Export", "Assembly region" }));
        GTUtilsDialog::add(new AnyDialogFiller("ExtractAssemblyRegionDialog", new Scenario(formatToSave)));
        if (formatToSave != formats.first()) {
            GTUtilsDialog::add(new ImportBAMFileFiller(sandBoxDir +
                "8160/chrM.sorted." + formatToSave + ".ugenedb"));
        }
        GTUtilsAssemblyBrowser::callContextMenu(GTUtilsAssemblyBrowser::Reads);
        GTUtilsTaskTreeView::waitTaskFinished();

        CHECK_SET_ERR(GTUtilsAssemblyBrowser::getLength() == 110, "Expected assembly length not 110");
        if (formatToSave != formats.last()) {  // skip last for speed up
            GTUtilsDialog::waitForDialog(new SaveProjectDialogFiller(QDialogButtonBox::No));
            GTMenu::clickMainMenuItem({ "File", "Close project" });
            GTUtilsTaskTreeView::waitTaskFinished();
        }
    }
}

GUI_TEST_CLASS_DEFINITION(test_8161) {
    /*
    Load COI.aln
    Select Wrap mode on
    Select Highlighting tab
    Check up "Use dots" checkboõ
    Select "Statistics" tab
    Check up "Show distance column" checkboõ
    Select Distance algorithm=Similarity
    Uncheck Show distance column" checkboõ
    Select Wrap mode off
    Select  Highlighting tab
    Unexpectedly "Use dots" checkboõ is unchecked, check it again
    Select  "Statistics" tab
    */

    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    QAction* wrapMode = GTAction::findActionByText("Wrap mode");
    GTWidget::click(GTAction::button(wrapMode));
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Highlighting);
    GTCheckBox::setChecked(GTWidget::findCheckBox("useDots"), true);
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Statistics);
    GTCheckBox::setChecked(GTWidget::findCheckBox("showDistancesColumnCheck"), true);
    GTComboBox::selectItemByText(GTWidget::findComboBox("algoComboBox"), "Similarity");
    GTCheckBox::setChecked(GTWidget::findCheckBox("showDistancesColumnCheck"), false);
    GTWidget::click(GTAction::button(wrapMode));
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Highlighting);
    GTCheckBox::setChecked(GTWidget::findCheckBox("useDots"), true);
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Statistics);
}

GUI_TEST_CLASS_DEFINITION(test_8170) {
    // Open any sequence (e.g. murine.gb)
    // Enable Restriction Sites auto annotations
    // Expected: Restriction Sites auto annotations are enabled
    // Open the "Find restriction enzymes" dialog
    // Click "Select none"
    // Click "OK"
    // Expected: the messagebox "No enzymes are selected! Do you want to turn off enzymes annotations highlighting?" appeared
    // Click No
    // Expected: the messagebox dissapeared, but the "Find restriction enzymes" dialog is still opened
    // Click Yes
    // Expected: the "Find restriction enzymes" dialog is closed, Restriction Sites auto annotations are disabled
    GTFileDialog::openFile(dataDir + "samples/Genbank", "murine.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    auto parent = GTWidget::findWidget("ADV_single_sequence_widget_0");
    GTUtilsDialog::waitForDialog(new PopupChooser({"Restriction Sites"}));
    GTWidget::click(GTWidget::findWidget("AutoAnnotationUpdateAction", parent));
    GTUtilsTaskTreeView::waitTaskFinished();

    class Scenario : public CustomScenario {
        void run() override {
            auto dialog = GTWidget::getActiveModalWidget();
            GTWidget::click(GTWidget::findWidget("selectNoneButton", dialog));
            GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::No, "No enzymes are selected! Do you want to turn off <br>enzymes annotations highlighting?"));
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
            GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Yes, "No enzymes are selected! Do you want to turn off <br>enzymes annotations highlighting?"));
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(new FindEnzymesDialogFiller(QStringList{} , new Scenario));
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Analyze", "Find restriction sites..."}));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new PopupChecker({"Restriction Sites"}, PopupChecker::CheckOptions(PopupChecker::CheckOption::IsUnchecked)));
    GTWidget::click(GTWidget::findWidget("AutoAnnotationUpdateAction", parent));

}

}  // namespace GUITest_regression_scenarios

}  // namespace U2
