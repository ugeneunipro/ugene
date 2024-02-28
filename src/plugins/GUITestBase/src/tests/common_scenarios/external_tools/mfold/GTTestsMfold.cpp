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

#include "GTUtilsLog.h"
#include "GTUtilsMdi.h"
#include "GTUtilsNotifications.h"
#include "GTUtilsSequenceView.h"
#include "GTUtilsTaskTreeView.h"
#include "api/GTRegionSelector.h"
#include "base_dialogs/GTFileDialog.h"
#include "primitives/GTDoubleSpinBox.h"
#include "primitives/GTLineEdit.h"
#include "primitives/GTMenu.h"
#include "primitives/GTSpinBox.h"
#include "primitives/GTToolbar.h"
#include "primitives/GTWidget.h"
#include "runnables/ugene/plugins/dna_export/DNASequenceGeneratorDialogFiller.h"
#include "runnables/ugene/ugeneui/AnyDialogFiller.h"
namespace U2 {
namespace GUITest_common_scenarios_mfold {
using namespace HI;
static QString clickNotificationAndGetHtml() {
    GTUtilsNotifications::clickOnNotificationWidget();
    QWidget* reportWindow = GTUtilsMdi::checkWindowIsActive("Task report ");
    auto reportEdit = GTWidget::findTextEdit("reportTextEdit", reportWindow);
    return reportEdit->toHtml();
}
// Checks that HTML report contains row with two consecutive cells: left cell contains leftVal,
// right cell contains rightVal.
static void doesHtmlContainRow(const QString& html, const QVector<QPair<QString, QString>>& rowValues) {
    for (auto p : qAsConst(rowValues)) {
        QRegularExpression re(QString(p.first) + ".*?<\\/td>\\s*<td.*?>.*" + p.second,
                              QRegularExpression::PatternOption::DotMatchesEverythingOption);
        CHECK_SET_ERR(html.contains(re), QString("Expected row `%1: %2` not found").arg(p.first, p.second));
    }
}
GUI_TEST_CLASS_DEFINITION(test_0001_success) {
    // Simple run.
    // Open sequence. Check its appearance.
    GTFileDialog::openFile(dataDir + "samples/Genbank/", "CVU55762.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    // Call dialog. Set region 1..100 and run.
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

    // Check report ok.
    QString html = clickNotificationAndGetHtml();
    doesHtmlContainRow(html,
                       {{"Status", "Finished"},
                        {"Sequence name", "CVU55762"},
                        {"Region", "1..100"},
                        {"Sequence type", "Circular DNA"},
                        {"Temperature", "37"},
                        {"Percent suboptimality", "5%"},
                        {"", "Na=1.00 M"},
                        {"", "Mg=0.00 M"},
                        {"Window", "default"},
                        {"Maximum distance between paired bases", "default"}});
}
GUI_TEST_CLASS_DEFINITION(test_0002_fail) {
    // Create sequence that contains no possible complement pairs.
    GTMenu::clickMainMenuItem({"Tools", "Random sequence generator..."});
    DNASequenceGeneratorDialogFillerModel model(sandBoxDir + "mfold2.fa");
    model.formatId = "fasta";
    model.length = 100;
    model.percentA = 50;
    model.percentC = 50;
    model.percentG = 0;
    model.percentT = 0;
    model.window = 100;
    GTUtilsDialog::waitForDialog(new DNASequenceGeneratorDialogFiller(model));
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    // Run mfold.
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Mfold");
    GTUtilsDialog::add(new AnyDialogFiller("MfoldDialog", QDialogButtonBox::Ok));
    GTUtilsTaskTreeView::waitTaskFinished();
    QString html = clickNotificationAndGetHtml();
    doesHtmlContainRow(html, {{"Status", "Failed"}});
    QString expected = "No foldings.";
    CHECK_SET_ERR(html.contains(expected), QString("Expected message `%1` not found in `%2`").arg(expected, html));
}
GUI_TEST_CLASS_DEFINITION(test_0003_limits) {
    // Check that each spinbox has limits.
    // Open sequence. Check its appearance.
    GTFileDialog::openFile(dataDir + "samples/Genbank/", "CVU55762.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    // Call dialog and check. Don't run task.
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Mfold");
    class SpinboxChecker final : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            auto checkSpinbox = [dialog](const QString& widgetName, int min, int defaultValue, int max) {
                auto w = GTWidget::findSpinBox(widgetName, dialog);
                GTSpinBox::checkLimits(w, min, max);
                auto curValue = GTSpinBox::getValue(w);
                CHECK_SET_ERR(curValue == defaultValue,
                              QString("Expected default `%1` value is %2, got %3")
                                  .arg(widgetName)
                                  .arg(defaultValue)
                                  .arg(curValue));
            };
            auto checkDoubleSpinbox = [dialog](const QString& widgetName, double min, double defaultValue, double max) {
                auto w = GTWidget::findDoubleSpinBox(widgetName, dialog);

                class GetDoubleSbMinMax final : public CustomScenario {
                    double& min_;
                    double& max_;
                    QDoubleSpinBox* w = nullptr;

                public:
                    explicit GetDoubleSbMinMax(double& min, double& max, QDoubleSpinBox* w)
                        : min_(min), max_(max), w(w) {
                    }
                    void run() override {
                        min_ = w->minimum();
                        max_ = w->maximum();
                    }
                };
                double minVal = 0;
                double maxVal = 0;
                GTThread::runInMainThread(new GetDoubleSbMinMax(minVal, maxVal, w));
                GTThread::waitForMainThread();

                CHECK_SET_ERR(minVal == min,
                              QString("Expected minimum `%1` value is %2, got %3")
                                  .arg(widgetName)
                                  .arg(min)
                                  .arg(minVal));
                CHECK_SET_ERR(maxVal == max,
                              QString("Expected maximum `%1` value is %2, got %3")
                                  .arg(widgetName)
                                  .arg(max)
                                  .arg(maxVal));

                auto curValue = GTDoubleSpinbox::getValue(w);
                CHECK_SET_ERR(curValue == defaultValue,
                              QString("Expected default `%1` value is %2, got %3")
                                  .arg(widgetName)
                                  .arg(defaultValue)
                                  .arg(curValue));
            };
            checkSpinbox("tSpinBox", 0, 37, 100);
            checkDoubleSpinbox("naDoubleSpinBox", 0, 1, 99.99);
            checkDoubleSpinbox("mgDoubleSpinBox", 0, 0, 99.99);
            checkSpinbox("pSpinBox", 0, 5, 100);
            checkSpinbox("maxSpinBox", 1, 100, 100);
            checkSpinbox("wSpinBox", -1, -1, 50000);
            checkSpinbox("maxBpSpinBox", 0, 0, 6000);
            checkSpinbox("labFrSpinBox", -1, -1, 1000);

            // Check that angle can be -180 degrees and 180 degrees. Limits are not that important.
            auto w = GTWidget::findDoubleSpinBox("rotAngDoubleSpinBox", dialog);
            auto curValue = GTDoubleSpinbox::getValue(w);
            CHECK_SET_ERR(curValue == 0, QString("Expected angle default value is 0, got %1").arg(curValue));
            GTDoubleSpinbox::setValue(w, -180, GTGlobals::UseKeyBoard);
            GTDoubleSpinbox::setValue(w, 180, GTGlobals::UseKeyBoard);

            checkSpinbox("dpiSpinBox", 60, 72, 4000);

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };
    GTUtilsDialog::add(new AnyDialogFiller("MfoldDialog", new SpinboxChecker()));
}
GUI_TEST_CLASS_DEFINITION(test_0004_region) {
    // Check error messages about wrong region.
    // Open linear sequence. Check its appearance.
    GTFileDialog::openFile(dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    // Select region.
    GTUtilsSequenceView::selectSequenceRegion(50, 100);
    // Call dialog and check that dialog sees selected region. Don't run task.
    class StartGreaterThanEndScenario final : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            auto regionSelector = GTWidget::findExactWidget<RegionSelector*>("range_selector", dialog);
            GTRegionSelector::setRegion(regionSelector, GTRegionSelector::RegionSelectorSettings(50, 10));
            GTWidget::findLabelByText("Start position cannot be greater than end position", dialog);
            GTRegionSelector::setRegion(regionSelector, GTRegionSelector::RegionSelectorSettings(50, 5000));
            GTWidget::findLabelByText("Region cannot be larger than 3000 nucleotides", dialog);
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };
    GTUtilsDialog::waitForDialog(new AnyDialogFiller("MfoldDialog", new StartGreaterThanEndScenario()));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Mfold");

    // Open circular sequence. Check its appearance.
    GTFileDialog::openFile(dataDir + "samples/Genbank/", "CVU55762.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    // Select region.
    GTUtilsSequenceView::selectSeveralRegionsByDialog("4730..16");

    // Call dialog and check that dialog sees selected region.
    // Run task and check that region is completely parsed without errors.
    class RegionChecker final : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            auto regionSelector = GTWidget::findExactWidget<RegionSelector*>("range_selector", dialog);
            auto startLe = GTWidget::findLineEdit("start_edit_line", regionSelector);
            auto endLe = GTWidget::findLineEdit("end_edit_line", regionSelector);

            GTLineEdit::checkText(startLe, "4730");
            GTLineEdit::checkText(endLe, "16");

            GTLineEdit::clear(startLe);
            GTWidget::findLabelByText("Start position not specified", dialog);
            GTLineEdit::setText(startLe, "4730");
            GTLineEdit::clear(endLe);
            GTWidget::findLabelByText("End position not specified", dialog);

            GTLineEdit::setText(endLe, "4729");
            GTWidget::findLabelByText("Region cannot be larger than 3000 nucleotides", dialog);

            GTLineEdit::setText(endLe, "16");
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };
    GTLogTracer lt;
    GTUtilsDialog::waitForDialog(new AnyDialogFiller("MfoldDialog", new RegionChecker()));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Mfold");
    GTUtilsLog::checkMessageWithWait(lt, "Sequence length is 20");
}
}  // namespace GUITest_common_scenarios_mfold
}  // namespace U2
