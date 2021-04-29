/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2021 UniPro <ugene@unipro.ru>
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
#include <primitives/GTAction.h>
#include <primitives/GTCheckBox.h>
#include <primitives/GTMenu.h>
#include <primitives/GTRadioButton.h>
#include <primitives/GTWidget.h>
#include <primitives/PopupChooser.h>
#include <system/GTClipboard.h>
#include <system/GTFile.h>
#include <utils/GTUtilsDialog.h>

#include <QApplication>
#include <QFileInfo>
#include <QRadioButton>

#include "GTTestsRegressionScenarios_7001_8000.h"
#include "GTUtilsMdi.h"
#include "GTUtilsMsaEditor.h"
#include "GTUtilsMsaEditorSequenceArea.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsSequenceView.h"
#include "GTUtilsTaskTreeView.h"
#include "api/GTMSAEditorStatusWidget.h"
#include "runnables/ugene/corelibs/U2Gui/AppSettingsDialogFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/ExtractSelectedAsMSADialogFiller.h"
#include "runnables/ugene/plugins/dna_export/ExportSequencesDialogFiller.h"
#include "runnables/ugene/ugeneui/SequenceReadingModeSelectorDialogFiller.h"

namespace U2 {

namespace GUITest_regression_scenarios {
using namespace HI;

GUI_TEST_CLASS_DEFINITION(test_7003) {
    // 1. Ensure that 'UGENE_EXTERNAL_TOOLS_VALIDATION_BY_PATH_ONLY' is not set to "1"
    // 2. Open "UGENE Application Settings", select "External Tools" tab
    // 3. Add the 'dumb.sh' or 'dumb.cmd' as a Python executable
    // 4. Check that validation fails

    qputenv("UGENE_EXTERNAL_TOOLS_VALIDATION_BY_PATH_ONLY", "0");

    class CheckPythonInvalidation : public CustomScenario {
        void run(GUITestOpStatus &os) override {
            AppSettingsDialogFiller::openTab(os, AppSettingsDialogFiller::ExternalTools);

            QString toolPath = testDir + "_common_data/regression/7003/dumb.";
            toolPath += isOsWindows() ? "cmd" : "sh";

            AppSettingsDialogFiller::setExternalToolPath(os, "python", QFileInfo(toolPath).absoluteFilePath());
            CHECK_SET_ERR(!AppSettingsDialogFiller::isExternalToolValid(os, "python"),
                          "Python module is expected to be invalid, but in fact it is valid")

            GTUtilsDialog::clickButtonBox(os, GTWidget::getActiveModalWidget(os), QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(os, new AppSettingsDialogFiller(os, new CheckPythonInvalidation()));
    GTMenu::clickMainMenuItem(os, QStringList() << "Settings"
                                                << "Preferences...",
                              GTGlobals::UseMouse);
}

GUI_TEST_CLASS_DEFINITION(test_7014) {
    // The test checks 'Save subalignment' in the collapse (virtual groups) mode.
    GTFileDialog::openFile(os, testDir + "_common_data/nexus", "DQB1_exon4.nexus");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    // Enable collapsing.
    GTUtilsMsaEditor::toggleCollapsingMode(os);

    // Expand collapsed group.
    GTUtilsMsaEditor::toggleCollapsingGroup(os, "LR882519 exotic DQB1");

    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(1, 1), QPoint(5, 4));

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << MSAE_MENU_EXPORT << "Save subalignment", GTGlobals::UseMouse));
    auto saveSubalignmentDialogFiller = new ExtractSelectedAsMSADialogFiller(os, sandBoxDir + "test_7014.aln");
    saveSubalignmentDialogFiller->setUseDefaultSequenceSelection(true);
    GTUtilsDialog::waitForDialog(os, saveSubalignmentDialogFiller);
    GTMenu::showContextMenu(os, GTUtilsMsaEditor::getSequenceArea(os));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsMdi::closeWindow(os, "DQB1_exon4 [DQB1_exon4.nexus]");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    // Expected state: the saved sub-alignment is opened. Check the content.
    QStringList nameList = GTUtilsMSAEditorSequenceArea::getNameList(os);
    QStringList expectedNameList = QStringList() << "LR882519 exotic DQB1"
                                                 << "LR882531 local DQB1"
                                                 << "LR882507 local DQB1"
                                                 << "LR882509 local DQB1";
    CHECK_SET_ERR(nameList == expectedNameList, "Unexpected name list in the exported alignment: " + nameList.join(","));

    int msaLength = GTUtilsMSAEditorSequenceArea::getLength(os);
    CHECK_SET_ERR(msaLength == 5, "Unexpected exported alignment length: " + QString::number(msaLength));
}

GUI_TEST_CLASS_DEFINITION(test_7022) {
    // 1. Open _common_data/scenarios/_regression/7022/test_7022.gb
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/_regression/7022/test_7022.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    // 2. Turn on "Wrap mode" and click on the firts annotation in DetView
    QAction *wrapMode = GTAction::findActionByText(os, "Wrap sequence");
    CHECK_SET_ERR(wrapMode != NULL, "Cannot find Wrap sequence action");
    if (!wrapMode->isChecked()) {
        GTWidget::click(os, GTAction::button(os, wrapMode));
    }
    GTUtilsSequenceView::clickAnnotationDet(os, "Misc. Feature", 2);

    // 3. copy selected annotation
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Copy/Paste"
                                                                              << "Copy annotation sequence"));
    GTMenu::showContextMenu(os, GTUtilsSequenceView::getPanOrDetView(os));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected: TGTCAGATTCACCAAAGTTGAAATGAAGGAAAAAATGCTAAGGGCAGCCAGAGAGAGGTCAGGTTACCCACAAAGGGAAGCCCATCAGAC
    QString expected = "TGTCAGATTCACCAAAGTTGAAATGAAGGAAAAAATGCTAAGGGCAGCCAGAGAGAGGTCAGGTTACCCACAAAGGGAAGCCCATCAGAC";
    QString text = GTClipboard::text(os);
    CHECK_SET_ERR(text == expected, QString("Unexpected annotation, expected: %1, current: %2").arg(expected).arg(text));
}

GUI_TEST_CLASS_DEFINITION(test_7043) {
    // 1. Open data/samples/PDB/1CF7.pdb
    // 2. Check that you see 3D struct or black screen with text "Failed to initialize OpenGL"
    GTFileDialog::openFile(os, dataDir + "samples/PDB/1CF7.PDB");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    auto biostructWidget = GTWidget::findWidget(os, "1-1CF7");
    const QImage image1 = GTWidget::getImage(os, biostructWidget, true);
    QSet<QRgb> colors;
    for (int i = 0; i < image1.width(); i++) {
        for (int j = 0; j < image1.height(); j++) {
            colors << image1.pixel(i, j);
        }
    }
    bool isPicture = colors.size() > 100;    // Usually 875 colors are drawn for 1CF7.pdb

    auto errorLbl = GTWidget::findLabelByText(os, "Failed to initialize OpenGL", nullptr, GTGlobals::FindOptions(false));
    bool isError = errorLbl.size() > 0;

    // There must be one thing: either a picture or an error
    CHECK_SET_ERR(isPicture != isError, "Biostruct was not drawn or error label wasn't displayed");
}

GUI_TEST_CLASS_DEFINITION(test_7044) {
    // The test checks 'Save subalignment' in the collapse (virtual groups) mode after reordering.
    GTFileDialog::openFile(os, testDir + "_common_data/nexus", "DQB1_exon4.nexus");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    // Enable collapsing.
    GTUtilsMsaEditor::toggleCollapsingMode(os);

    // Rename the last two sequences in 'seqA' and 'seqB'.
    GTUtilsMSAEditorSequenceArea::renameSequence(os, "LR882509 local DQB1", "seqA");
    GTUtilsMSAEditorSequenceArea::renameSequence(os, "LR882503 local DQB1", "seqB");

    // Copy seqA.
    GTUtilsMSAEditorSequenceArea::selectSequence(os, "seqA");
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);

    // Select first collapsed mode and 'Paste before'.
    GTUtilsMSAEditorSequenceArea::selectSequence(os, "LR882520 exotic DQB1");
    GTKeyboardDriver::keyPress(Qt::Key_Control);
    GTKeyboardDriver::keyClick('v', Qt::AltModifier);
    GTKeyboardDriver::keyRelease(Qt::Key_Control);

    // Cut seqB.
    GTUtilsMSAEditorSequenceArea::selectSequence(os, "seqB");
    GTKeyboardDriver::keyClick('x', Qt::ControlModifier);

    // Select the first sequence and 'Paste before'
    GTUtilsMSAEditorSequenceArea::selectSequence(os, "seqA_1");
    GTKeyboardDriver::keyPress(Qt::Key_Control);
    GTKeyboardDriver::keyClick('v', Qt::AltModifier);
    GTKeyboardDriver::keyRelease(Qt::Key_Control);

    // Select seqB and seqA_1 (a group of seqA_1 and seqA).
    GTUtilsMSAEditorSequenceArea::selectSequence(os, "seqA_1");
    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    GTUtilsMSAEditorSequenceArea::selectSequence(os, "seqB");
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);

    // Export -> Save subalignment.
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << MSAE_MENU_EXPORT << "Save subalignment", GTGlobals::UseMouse));
    auto saveSubalignmentDialogFiller = new ExtractSelectedAsMSADialogFiller(os, sandBoxDir + "test_7044.aln");
    saveSubalignmentDialogFiller->setUseDefaultSequenceSelection(true);
    GTUtilsDialog::waitForDialog(os, saveSubalignmentDialogFiller);
    GTMenu::showContextMenu(os, GTUtilsMsaEditor::getSequenceArea(os));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: the saved sub-alignment is opened. Check the content.
    QStringList nameList = GTUtilsMSAEditorSequenceArea::getNameList(os);
    QStringList expectedNameList = QStringList() << "seqB"
                                                 << "seqA_1"
                                                 << "seqA";
    CHECK_SET_ERR(nameList == expectedNameList, "Unexpected name list in the exported alignment: " + nameList.join(","));
}

GUI_TEST_CLASS_DEFINITION(test_7045) {
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa", "COI_subalign.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    // Select 's1'.
    GTUtilsMSAEditorSequenceArea::selectSequence(os, "s1");

    // Copy (CTRL C) and Paste (CTRL V) -> new 's1_1' sequence appears.
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);

    // Switch collapsing mode on -> 2 collapsed groups: 's1' and' Mecopoda_elongata_Ishigaki_J' are on the screen.
    GTUtilsMsaEditor::toggleCollapsingMode(os);

    // Select 's1'.
    GTUtilsMSAEditorSequenceArea::selectSequence(os, "s1");

    // Call Export -> Save subalignment context menu.
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << MSAE_MENU_EXPORT << "Save subalignment", GTGlobals::UseMouse));
    auto saveSubalignmentDialogFiller = new ExtractSelectedAsMSADialogFiller(os, sandBoxDir + "test_7044.aln");
    saveSubalignmentDialogFiller->setUseDefaultSequenceSelection(true);
    GTUtilsDialog::waitForDialog(os, saveSubalignmentDialogFiller);
    GTMenu::showContextMenu(os, GTUtilsMsaEditor::getSequenceArea(os));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state : new alignment where s1, s1_1 and s2 are present.
    QStringList nameList = GTUtilsMSAEditorSequenceArea::getNameList(os);
    QStringList expectedNameList = QStringList() << "s1"
                                                 << "s1_1"
                                                 << "s2";
    CHECK_SET_ERR(nameList == expectedNameList, "Unexpected name list in the exported alignment: " + nameList.join(","));
}

GUI_TEST_CLASS_DEFINITION(test_7091) {
    // The test compares images of UGENE's main window before and after "Preferences" dialog is closed.
    QWidget *mainWindow = QApplication::activeWindow();
    QImage initialImage = GTWidget::getImage(os, mainWindow);

    // The scenario does nothing and only closes the dialog.
    class NoOpScenario : public CustomScenario {
    public:
        void run(GUITestOpStatus &os) override {
            QWidget *dialog = GTWidget::getActiveModalWidget(os);
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };
    GTUtilsDialog::waitForDialog(os, new AppSettingsDialogFiller(os, new NoOpScenario()));
    GTMenu::clickMainMenuItem(os, {"Settings", "Preferences..."});

    QImage currentImage = GTWidget::getImage(os, mainWindow);
    CHECK_SET_ERR(initialImage == currentImage, "Visual appearance of the dialog should not change.");
}

GUI_TEST_CLASS_DEFINITION(test_7106) {
    // Check that in Tree-Sync mode Drag & Drop of sequences in the MSA name list is disabled.
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    GTUtilsMsaEditor::buildPhylogeneticTree(os, sandBoxDir + "test_7106");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QStringList sequenceList1 = GTUtilsMSAEditorSequenceArea::getVisibleNames(os);

    QRect firstRowRect = GTUtilsMsaEditor::getSequenceNameRect(os, 0);
    QRect secondRowRect = GTUtilsMsaEditor::getSequenceNameRect(os, 1);

    GTMouseDriver::click(firstRowRect.center());
    GTMouseDriver::dragAndDrop(firstRowRect.center(), secondRowRect.center());

    QStringList sequenceList2 = GTUtilsMSAEditorSequenceArea::getVisibleNames(os);
    CHECK_SET_ERR(sequenceList2 == sequenceList1, "Sequence order must not change");
}

GUI_TEST_CLASS_DEFINITION(test_7127) {
    // Make an alignment ordered by tree and check that the row order shown in the status bar is correct.
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    GTUtilsMsaEditor::buildPhylogeneticTree(os, sandBoxDir + "test_7127");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    for (int i = 0; i < 18; i++) {
        GTUtilsMSAEditorSequenceArea::clickToPosition(os, QPoint(0, i));
        QString rowNumber = GTMSAEditorStatusWidget::getRowNumberString(os);
        QString expectedRowNumber = QString::number(i + 1);
        CHECK_SET_ERR(rowNumber == expectedRowNumber, "Unexpected row number! Expected:  " + expectedRowNumber + ", got: " + rowNumber);
    }
}

GUI_TEST_CLASS_DEFINITION(test_7152) {
    // Check that corner characters of an alignment has valid info shown in the status bar.
    GTFileDialog::openFile(os, testDir + "_common_data/clustal/region.full-gap.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    GTUtilsMSAEditorSequenceArea::clickToPosition(os, QPoint(0, 0));
    QString topLeft = GTMSAEditorStatusWidget::getRowNumberString(os) + "/" +
                      GTMSAEditorStatusWidget::getColumnNumberString(os) + "/" +
                      GTMSAEditorStatusWidget::getSequenceUngappedPositionString(os);
    GTMSAEditorStatusWidget::getColumnNumberString(os);
    CHECK_SET_ERR(topLeft == "1/1/1", "Top left position is wrong: " + topLeft);

    GTUtilsMSAEditorSequenceArea::clickToPosition(os, QPoint(39, 10));
    QString bottomRight = GTMSAEditorStatusWidget::getRowNumberString(os) + "/" +
                          GTMSAEditorStatusWidget::getColumnNumberString(os) + "/" +
                          GTMSAEditorStatusWidget::getSequenceUngappedPositionString(os);
    GTMSAEditorStatusWidget::getColumnNumberString(os);
    CHECK_SET_ERR(bottomRight == "11/40/35", "Bottom right position is wrong: " + bottomRight);
}

GUI_TEST_CLASS_DEFINITION(test_7183) {
    class ExportSequencesScenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(dialog, "activeModalWidget is NULL");

            GTRadioButton::click(os, GTWidget::findExactWidget<QRadioButton *>(os, "bothStrandsButton", dialog));
            GTCheckBox::setChecked(os, GTWidget::findExactWidget<QCheckBox *>(os, "translateButton", dialog), true);
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };
    //1. Open file _common_data/fasta/reads.fa as separate sequences.
    QString filePath = testDir + "_common_data/fasta/reads.fa";
    QString fileName = "reads.fa";
    GTFile::copy(os, filePath, sandBoxDir + "/" + fileName);
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os));
    GTFileDialog::openFile(os, sandBoxDir + "/" + fileName);
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);
    
    for (int i = 0; i < 8; i++) {
        GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION << ACTION_EXPORT_SEQUENCE));
        GTUtilsDialog::waitForDialog(os, new ExportSelectedRegionFiller(os, new ExportSequencesScenario()));
        GTUtilsProjectTreeView::click(os, "reads.fa", Qt::RightButton);
        GTUtilsTaskTreeView::waitTaskFinished(os);
    }
    //2. Open context menu on reads.fa file in project view. Select "Export/Import -> Export sequences..."
    //3. Check the "Save both strands" radiobutton
    //4. Check the "Translate to amino" checkbox
    //5. Push Export button in the dialog.
    //6. Repeat steps 2-5 8 times
    //Expected state: UGENE is not crash
}

}    // namespace GUITest_regression_scenarios

}    // namespace U2
