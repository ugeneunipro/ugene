/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include "GTTestsDarkMode.h"

#include <base_dialogs/GTFileDialog.h>
#include <base_dialogs/MessageBoxFiller.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTPlainTextEdit.h>
#include <primitives/GTWidget.h>

#include <QApplication>

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

namespace GUITest_common_scenarios_dark_mode {

GUI_TEST_CLASS_DEFINITION(test_0001_win) {
    // Open "Settings -> Preferences"
    // Set WindowsVista style
    // Expected: "WindowsVisty style is not compatible" error
    // Set Fusion style
    // Expected: Error dissapeared
    // Set "Dark" color mode
    // Expected: No WindowsVisata style
    // Click OK
    // Expected: start page color is dark
}

GUI_TEST_CLASS_DEFINITION(test_0001_not_win) {
    // Open "Settings -> Preferences"
    // Set "Dark" color mode
    // Click OK
    // Expected: start page color is dark
}

GUI_TEST_CLASS_DEFINITION(test_0002) {
    // Открыть murine.gb
    //

}

GUI_TEST_CLASS_DEFINITION(test_0003) {
    // Выравнивание

}

GUI_TEST_CLASS_DEFINITION(test_0004) {
    // Сборка

}

GUI_TEST_CLASS_DEFINITION(test_0005) {
    // Сэнгер

}

GUI_TEST_CLASS_DEFINITION(test_0006) {
    // WD

}

GUI_TEST_CLASS_DEFINITION(test_0007) {
    // QD
}

}  // namespace GUITest_common_scenarios_dark_mode
}  // namespace U2
