/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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
#include <base_dialogs/ColorDialogFiller.h>
#include <base_dialogs/DefaultDialogFiller.h>
#include <base_dialogs/FontDialogFiller.h>
#include <base_dialogs/GTFileDialog.h>
#include <base_dialogs/MessageBoxFiller.h>
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <harness/UGUITestBase.h>
#include <primitives/GTAction.h>
#include <primitives/GTCheckBox.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTMenu.h>
#include <primitives/GTRadioButton.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTToolbar.h>
#include <primitives/PopupChooser.h>
#include <system/GTClipboard.h>
#include <system/GTFile.h>
#include <utils/GTKeyboardUtils.h>
#include <utils/GTThread.h>

#include <QApplication>

#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/TextUtils.h>

#include <U2View/ADVConstants.h>
#include <U2View/MSAEditor.h>
#include <U2View/MaEditorNameList.h>

#include "GTTestsMsaExcludeList.h"
#include "GTUtilsLog.h"
#include "GTUtilsMdi.h"
#include "GTUtilsMsaEditor.h"
#include "GTUtilsMsaEditorSequenceArea.h"
#include "GTUtilsNotifications.h"
#include "GTUtilsOptionPanelMSA.h"
#include "GTUtilsProject.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsTaskTreeView.h"
#include "api/GTSequenceReadingModeDialogUtils.h"
#include "runnables/ugene/corelibs/U2Gui/ExportDocumentDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ExportImageDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/PositionSelectorFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ProjectTreeItemSelectorDialogFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/BuildTreeDialogFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/DeleteGapsDialogFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/DistanceMatrixDialogFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/ExportHighlightedDialogFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/ExtractSelectedAsMSADialogFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/GenerateAlignmentProfileDialogFiller.h"
#include "runnables/ugene/plugins/dna_export/ExportMSA2MSADialogFiller.h"
#include "runnables/ugene/plugins/dna_export/ExportSelectedSequenceFromAlignmentDialogFiller.h"
#include "runnables/ugene/plugins/dna_export/ExportSequences2MSADialogFiller.h"
#include "runnables/ugene/plugins/weight_matrix/PwmBuildDialogFiller.h"
#include "runnables/ugene/plugins/workflow_designer/WizardFiller.h"
#include "runnables/ugene/plugins_3rdparty/kalign/KalignDialogFiller.h"
#include "runnables/ugene/plugins_3rdparty/umuscle/MuscleDialogFiller.h"

namespace U2 {

namespace GUITest_common_scenarios_msa_exclude_list {
using namespace HI;

GUI_TEST_CLASS_DEFINITION(test_0001) {
    // Check initial state of MSA editor & Exclude list with no selection.
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/ty3.aln.gz");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    auto msaEditorWindow = GTUtilsMsaEditor::getActiveMsaEditorWindow(os);
    auto excludeListWidget = GTWidget::findWidget(os, "msa_exclude_list", msaEditorWindow, false);
    CHECK_SET_ERR(excludeListWidget == nullptr, "Exclude list must not be opened by default");

    GTUtilsDialog::waitForDialog(os, new PopupChecker(os, {MSAE_MENU_COPY, "exclude_list_move_from_msa_action"}, PopupChecker::IsDisabled));
    GTMenu::showContextMenu(os, GTUtilsMSAEditorSequenceArea::getSequenceArea(os));

    auto toolbar = GTToolbar::getToolbar(os, MWTOOLBAR_ACTIVEMDI);
    auto toggleExcludeListButton = GTToolbar::getToolButtonByAction(os, toolbar, "exclude_list_toggle_action");
    CHECK_SET_ERR(!toggleExcludeListButton->isChecked(), "Toggle exclude list button must not be checked by default");

    // Open exclude list.
    GTWidget::click(os, toggleExcludeListButton);

    // Check the exclude list state.
    CHECK_SET_ERR(toggleExcludeListButton->isChecked(), "Toggle exclude list button must be checked");
    excludeListWidget = GTWidget::findWidget(os, "msa_exclude_list", msaEditorWindow);

    GTWidget::findLabelByText(os, "Exclude list is empty", excludeListWidget);

    // Check buttons.
    auto selectFileButton = GTWidget::findToolButton(os, "exclude_list_select_file_button", excludeListWidget);
    CHECK_SET_ERR(selectFileButton->text() == "ty3.exclude-list.fasta", "Invalid select file button text: " + selectFileButton->text());
    CHECK_SET_ERR(selectFileButton->isEnabled(), "selectFileButton must be enabled");

    auto saveAsButton = GTWidget::findToolButton(os, "exclude_list_save_as_button", excludeListWidget);
    CHECK_SET_ERR(saveAsButton->isEnabled(), "saveAsButton must be enabled");

    auto moveToMsaButton = GTWidget::findToolButton(os, "exclude_list_move_to_msa_button", excludeListWidget);
    CHECK_SET_ERR(!moveToMsaButton->isEnabled(), "moveToMsa button must not be enabled");

    auto moveToExcludeListButton = GTWidget::findToolButton(os, "exclude_list_move_to_exclude_list_button", excludeListWidget);
    CHECK_SET_ERR(!moveToExcludeListButton->isEnabled(), "moveToExcludeList button must not be enabled");

    auto nameListArea = GTWidget::findListWidget(os, "exclude_list_name_list_widget", excludeListWidget);
    CHECK_SET_ERR(nameListArea->isVisible(), "Name list must be visible");

    auto sequenceViewArea = GTWidget::findPlainTextEdit(os, "exclude_list_sequence_view", excludeListWidget);
    CHECK_SET_ERR(sequenceViewArea->isVisible(), "Sequence area must be visible");

    // Hide exclude list.
    GTWidget::click(os, toggleExcludeListButton);
    CHECK_SET_ERR(!toggleExcludeListButton->isChecked(), "Toggle exclude list button must not be checked");
    GTWidget::findWidget(os, "msa_exclude_list", msaEditorWindow, false);
}
}  // namespace GUITest_common_scenarios_msa_exclude_list
}  // namespace U2.
