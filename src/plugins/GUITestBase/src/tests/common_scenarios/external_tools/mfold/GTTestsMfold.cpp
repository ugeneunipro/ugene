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
#include "GTTestsMfold.h"
#include <base_dialogs/GTFileDialog.h>
#include <primitives/GTToolbar.h>
#include <primitives/GTWidget.h>
#include <runnables/ugene/ugeneui/AnyDialogFiller.h>

#include "GTUtilsMdi.h"
#include "GTUtilsNotifications.h"
#include "GTUtilsSequenceView.h"
#include "GTUtilsTaskTreeView.h"
#include "api/GTRegionSelector.h"
namespace U2 {
namespace GUITest_common_scenarios_mfold {
using namespace HI;
GUI_TEST_CLASS_DEFINITION(test_0001_success) {
    // Simple run
    GTFileDialog::openFile(dataDir + "samples/Genbank/", "CVU55762.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Mfold");
    class SimpleScenario : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            auto regionSelector = GTWidget::findExactWidget<RegionSelector*>("range_selector", dialog);
            GTRegionSelector::setRegion(regionSelector, GTRegionSelector::RegionSelectorSettings(1, 100));
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };
    GTUtilsDialog::add(new AnyDialogFiller("MfoldDialog", new SimpleScenario()));
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsNotifications::clickOnNotificationWidget();

    // Get report.
    QWidget* reportWindow = GTUtilsMdi::checkWindowIsActive("Task report ");
    auto reportEdit = GTWidget::findTextEdit("reportTextEdit", reportWindow);
    QString html = reportEdit->toHtml();

    // Checks that HTML report contains row with two consecutive cells: left cell contains leftVal,
    // right cell contains rightVal.
    auto doesReportContainRow = [&html](const std::pair<const char*, const char*>& p) {
        QRegularExpression re(QString(p.first) + ".*?<\\/td>\\s*?<td.*?>.*?" + p.second,
                              QRegularExpression::PatternOption::DotMatchesEverythingOption);
        return html.contains(re);
    };
    for (auto p : std::initializer_list<std::pair<const char*, const char*>> {{"Sequence name", "CVU55762"},
                                                                              {"Region", "1..100"},
                                                                              {"Sequence type", "Circular DNA"},
                                                                              {"Temperature", "37"},
                                                                              {"Percent suboptimality", "5%"},
                                                                              {"", "Na=1.00 M"},
                                                                              {"", "Mg=0.00 M"},
                                                                              {"Window", "default"},
                                                                              {"Maximum distance between paired bases", "default"}}) {
        CHECK_SET_ERR(doesReportContainRow(p), QString("Expected row `%1: %2` not found").arg(p.first, p.second));
    }
}
}  // namespace GUITest_common_scenarios_mfold
}  // namespace U2
