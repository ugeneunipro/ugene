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

#include <base_dialogs/GTFileDialog.h>
#include <primitives/GTAction.h>
//#include <primitives/GTComboBox.h>
//#include <primitives/GTLineEdit.h>
//#include <primitives/GTTabWidget.h>
#include <primitives/GTToolbar.h>
#include <primitives/GTWidget.h>
#include <primitives/PopupChooser.h>

//#include <QApplication>

#include "runnables/ugene/plugins_3rdparty/primer3/Primer3DialogFiller.h"
#include "GTTestsCommonScenariosPrimer3.h"
//#include "GTUtilsLog.h"
#include "GTUtilsTaskTreeView.h"
#include "GTUtilsSequenceView.h"

namespace U2 {
namespace GUITest_common_scenarios_primer3 {

GUI_TEST_CLASS_DEFINITION(test_tab_main_all) {
    GTFileDialog::openFile(os, testDir + "_common_data/primer3", "boundary.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    Primer3DialogFiller::Primer3Settings settings;
    settings.filePath = testDir + "_common_data/primer3/all_parameters_are_changed.txt";

    GTUtilsDialog::add(os, new PopupChooser(os, { "ADV_MENU_ANALYSE", "primer3_action" }));
    GTUtilsDialog::add(os, new Primer3DialogFiller(os, settings));
    GTWidget::click(os, GTUtilsSequenceView::getSeqWidgetByNumber(os), Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);


}

GUI_TEST_CLASS_DEFINITION(test_boundary_0001) {
    GTFileDialog::openFile(os, testDir + "_common_data/primer3", "boundary.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    Primer3DialogFiller::Primer3Settings settings;
    settings.filePath = testDir + "_common_data/primer3/input/boundary/test_00.txt";

    //GTUtilsDialog::add(os, new PopupChooser(os, { "ADV_MENU_ANALYSE", "primer3_action" }));
    GTUtilsDialog::add(os, new Primer3DialogFiller(os, settings));
    //GTWidget::click(os, GTUtilsSequenceView::getSeqWidgetByNumber(os), Qt::RightButton);
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Primer3");

    GTUtilsTaskTreeView::waitTaskFinished(os);
}

}  // namespace GUITest_common_scenarios_phyml
}  // namespace U2
