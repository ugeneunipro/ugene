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

#include <base_dialogs/GTFileDialog.h>
#include <base_dialogs/MessageBoxFiller.h>
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTCheckBox.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTLabel.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTMenu.h>
#include <primitives/GTPlainTextEdit.h>
#include <primitives/GTToolbar.h>
#include <primitives/GTWidget.h>
#include <system/GTFile.h>
#include <utils/GTKeyboardUtils.h>
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
#include "GTUtilsNotifications.h"
#include "GTUtilsMdi.h"
#include "GTUtilsOptionPanelMSA.h"
#include "GTUtilsOptionPanelSequenceView.h"
#include "GTUtilsProject.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsSequenceView.h"
#include "GTUtilsTaskTreeView.h"
#include "GTUtilsWizard.h"
#include "GTUtilsWorkflowDesigner.h"
#include "runnables/ugene/corelibs/U2Gui/AppSettingsDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/CreateAnnotationWidgetFiller.h"
#include "runnables/ugene/corelibs/U2Gui/CreateDocumentFromTextDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/EditSequenceDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/RangeSelectionDialogFiller.h"
#include "runnables/ugene/plugins/dna_export/DNASequenceGeneratorDialogFiller.h"
#include "runnables/ugene/plugins/dna_export/ExportSequencesDialogFiller.h"
#include "runnables/ugene/plugins/enzymes/FindEnzymesDialogFiller.h"
#include "runnables/ugene/plugins/external_tools/AlignToReferenceBlastDialogFiller.h"
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

}  // namespace GUITest_regression_scenarios

}  // namespace U2
