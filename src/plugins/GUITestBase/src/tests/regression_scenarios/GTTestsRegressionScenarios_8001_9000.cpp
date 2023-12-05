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

#include <api/GTUtils.h>
#include <base_dialogs/GTFileDialog.h>
#include <cmath>
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTAction.h>
#include <primitives/GTCheckBox.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTDoubleSpinBox.h>
#include <primitives/GTGroupBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTListWidget.h>
#include <primitives/GTMainWindow.h>
#include <primitives/GTMenu.h>
#include <primitives/GTPlainTextEdit.h>
#include <primitives/GTRadioButton.h>
#include <primitives/GTSlider.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTTabWidget.h>
#include <primitives/GTTableView.h>
#include <primitives/GTToolbar.h>
#include <primitives/GTTreeWidget.h>
#include <primitives/GTWidget.h>
#include <primitives/PopupChooser.h>
#include <system/GTClipboard.h>
#include <system/GTFile.h>
#include <utils/GTKeyboardUtils.h>
#include <utils/GTUtilsDialog.h>
#include <utils/GTUtilsText.h>
#include <utils/GTUtilsToolTip.h>

#include <QApplication>
#include <QClipboard>
#include <QDir>
#include <QFileInfo>
#include <QListWidget>
#include <QRadioButton>

#include <U2Core/AppContext.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/ProjectModel.h>

#include <U2Gui/Notification.h>

#include <U2View/ADVConstants.h>
#include <U2View/TvBranchItem.h>
#include <U2View/TvNodeItem.h>
#include <U2View/TvTextItem.h>

#include "GTTestsRegressionScenarios_8001_9000.h"
#include "GTUtilsBookmarksTreeView.h"
#include "GTUtilsLog.h"
#include "GTUtilsMdi.h"
#include "GTUtilsOptionPanelMSA.h"
#include "GTUtilsOptionPanelSequenceView.h"
#include "GTUtilsProject.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsSequenceView.h"
#include "GTUtilsTaskTreeView.h"
#include "runnables/ugene/plugins/external_tools/AlignToReferenceBlastDialogFiller.h"
#include "runnables/ugene/ugeneui/CreateNewProjectWidgetFiller.h"
#include "runnables/ugene/ugeneui/DocumentFormatSelectorDialogFiller.h"

namespace U2 {

namespace GUITest_regression_scenarios {
using namespace HI;

GUI_TEST_CLASS_DEFINITION(test_8009) {
    /*
     * 1. Open Tools->Sanger data ahalysis-> Map reads to reference
     * 2. Set wrong format reference file from sample: sample/ACE/k26.ace
     * 3. Add reads: _common_data/sanger/sanger_03.ab1
     * 4. Click Map button
     * Expected: Error log message 'wrong reference format'
     * 5. Open Tools->Sanger data ahalysis-> Map reads to reference
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
    SetRefAndAlign* setRefAndAlignScenario = new SetRefAndAlign();
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
    GTUtilsDialog::waitForDialog(new SaveProjectAsDialogFiller("proj_test_8028", sandBoxDir + "/proj_test_8028"));
    GTMenu::clickMainMenuItem({"File", "New project..."}, GTGlobals::UseMouse);

    GTUtilsDialog::waitForDialog(new DocumentFormatSelectorDialogFiller("Plain text"));
    GTUtilsProject::openFile(testDir + "_common_data/text/text.txt");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsBookmarksTreeView::addBookmark(GTUtilsMdi::activeWindow()->windowTitle(), "test_8028");

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("text"));
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
}

}  // namespace GUITest_regression_scenarios

}  // namespace U2
