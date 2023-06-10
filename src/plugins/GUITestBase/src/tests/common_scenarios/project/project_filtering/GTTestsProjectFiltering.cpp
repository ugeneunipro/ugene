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

#include <base_dialogs/DefaultDialogFiller.h>
#include <base_dialogs/GTFileDialog.h>
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTWidget.h>

#include <QTreeView>

#include <U2Core/U2IdTypes.h>

#include "GTTestsProjectFiltering.h"
#include "GTUtilsMdi.h"
#include "GTUtilsProject.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsTaskTreeView.h"
#include "primitives/PopupChooser.h"
#include "runnables/qt/EscapeClicker.h"
#include "runnables/ugene/ugeneui/DocumentFormatSelectorDialogFiller.h"

namespace U2 {

namespace GUITest_common_scenarios_project_filtering {
using namespace HI;

GUI_TEST_CLASS_DEFINITION(test_0014) {
    // 1. Open "data/samples/Genbank/murine.gb"
    GTFileDialog::openFile(dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Open "data/samples/CLUSTALW/COI.aln"
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state: Project filter clear button is invisible
    auto clearButton = GTWidget::findWidget("project filter clear button");
    CHECK_SET_ERR(!clearButton->isVisible(), "Project filter clear button is unexpectedly visible");

    // 3. Type to the project filter field "polyprotein"
    GTUtilsProjectTreeView::openView();
    auto nameFilterEdit = GTWidget::findLineEdit("nameFilterEdit");
    GTLineEdit::setText(nameFilterEdit, "polyprotein");

    // Expected state: Project filter clear button is visible
    CHECK_SET_ERR(clearButton->isVisible(), "Project filter clear button is unexpectedly invisible");
    GTGlobals::sleep(3000);
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(clearButton->isVisible(), "Project filter clear button is unexpectedly invisible");

    // 4. Click the clear button
    GTWidget::click(clearButton);

    // Expected state: project filter field is empty, clear button is invisible
    CHECK_SET_ERR(nameFilterEdit->text().isEmpty(), "Project filter clear button hasn't wiped filter");
    CHECK_SET_ERR(!clearButton->isVisible(), "Project filter clear button is unexpectedly visible");

    // 5. Type to the project filter field "AAA"
    GTLineEdit::setText(nameFilterEdit, "AAA");

    // Expected state: Project filter clear button is visible
    CHECK_SET_ERR(clearButton->isVisible(), "Project filter clear button is unexpectedly invisible");

    // 6. Remove typed word using a backspace
    for (int i = 0; i < 3; ++i) {
        GTKeyboardDriver::keyPress(Qt::Key_Backspace);
        GTGlobals::sleep(100);
    }
    GTGlobals::sleep(3000);

    // Expected state: Project filter clear button is invisible
    CHECK_SET_ERR(!clearButton->isVisible(), "Project filter clear button is unexpectedly visible");
}

}  // namespace GUITest_common_scenarios_project_filtering

}  // namespace U2
