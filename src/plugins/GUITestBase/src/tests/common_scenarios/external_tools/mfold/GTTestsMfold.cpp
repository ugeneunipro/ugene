/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2026 UniPro <ugene@unipro.ru>
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
#include <random>

#include <QFileInfo>

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
#include "primitives/GTTabWidget.h"
#include "primitives/GTTextEdit.h"
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
            checkDoubleSpinbox("naDoubleSpinBox", 0, 1, 1.5);
            checkDoubleSpinbox("mgDoubleSpinBox", 0, 0, 1.5);
            checkSpinbox("pSpinBox", 1, 5, 100);
            checkSpinbox("maxSpinBox", 1, 50, 100);
            checkSpinbox("wSpinBox", -1, -1, 50);
            checkSpinbox("maxBpSpinBox", 0, 0, 6000);
            checkSpinbox("labFrSpinBox", -1, -1, 1000);

            // Check that angle can be -180 degrees and 180 degrees. Limits are not that important.
            auto w = GTWidget::findDoubleSpinBox("rotAngDoubleSpinBox", dialog);
            auto curValue = GTDoubleSpinbox::getValue(w);
            CHECK_SET_ERR(curValue == 0, QString("Expected angle default value is 0, got %1").arg(curValue));
            GTDoubleSpinbox::setValue(w, -180, GTGlobals::UseKeyBoard);
            GTDoubleSpinbox::setValue(w, 180, GTGlobals::UseKeyBoard);

            checkSpinbox("dpiSpinBox", 60, 96, 1800);

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
GUI_TEST_CLASS_DEFINITION(test_0005_large) {
    // Check large report doesn't contain any structures info.
    // Open linear sequence. Check its appearance.
    GTFileDialog::openFile(dataDir + "samples/Genbank/", "CVU55762.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    // Select region.
    GTUtilsSequenceView::selectSequenceRegion(1, 500);
    // Call dialog and run task.
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Mfold");
    GTUtilsDialog::add(new AnyDialogFiller("MfoldDialog", QDialogButtonBox::Ok));
    GTUtilsTaskTreeView::waitTaskFinished();

    // Check no "Found structures", no "Structure 1".
    GTUtilsNotifications::clickOnNotificationWidget();
    QWidget* reportWindow = GTUtilsMdi::checkWindowIsActive("Task report ");
    QString html = GTWidget::findTextEdit("reportTextEdit", reportWindow)->toHtml();

    doesHtmlContainRow(html, {{"Status", "Finished"}});
    QString unexpected = "Found structures";
    CHECK_SET_ERR(!html.contains(unexpected),
                  QString("Message `%1` was found in `%2`, but should not").arg(unexpected, html));
    unexpected = "Structure 1";
    CHECK_SET_ERR(!html.contains(unexpected),
                  QString("Message `%1` was found in `%2`, but should not").arg(unexpected, html));
}
GUI_TEST_CLASS_DEFINITION(test_0006_html_name) {
    // Check that mfold report for sequences with strange names looks good.
    // Open sequence. Check its appearance.
    GTFileDialog::openFile(testDir + "_common_data/et/mfold/bad_names", "&quote;&amp;&lt;.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    // Call dialog and run task.
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Mfold");
    GTUtilsDialog::add(new AnyDialogFiller("MfoldDialog", QDialogButtonBox::Ok));
    GTUtilsTaskTreeView::waitTaskFinished();

    // Check that name in report is the same as sequence name.
    GTUtilsNotifications::clickOnNotificationWidget();
    auto reportEdit = GTWidget::findTextEdit("reportTextEdit");
    GTTextEdit::containsString(reportEdit, "&quote;&amp;&lt;");
    GTTextEdit::containsString(reportEdit, "_common_data/et/mfold/bad_names/&quote;&amp;&lt;");
}

namespace {
struct WidgetStates final {
    int temperature = 37;
    double na = 1;
    double mg = 0;
    int percent = 5;
    int numFold = 50;
    int window = -1;
    int distance = 0;
    int frequency = -1;
    double angle = 0;
    QPair<int, int> region = {0, 0};
    QString out;
    int dpi = 96;
    friend bool operator==(const WidgetStates& lhs, const WidgetStates& rhs) {
        return std::make_tuple(lhs.temperature,
                               lhs.na,
                               lhs.mg,
                               lhs.percent,
                               lhs.numFold,
                               lhs.window,
                               lhs.distance,
                               lhs.frequency,
                               lhs.angle,
                               lhs.region,
                               QFileInfo(lhs.out),
                               lhs.dpi) == std::make_tuple(rhs.temperature,
                                                           rhs.na,
                                                           rhs.mg,
                                                           rhs.percent,
                                                           rhs.numFold,
                                                           rhs.window,
                                                           rhs.distance,
                                                           rhs.frequency,
                                                           rhs.angle,
                                                           rhs.region,
                                                           QFileInfo(rhs.out),
                                                           rhs.dpi);
    }
};
static void setState(const WidgetStates& state, QWidget* parent = nullptr) {
    GTSpinBox::setValue("tSpinBox", state.temperature, GTGlobals::UseKeyBoard, parent);
    GTDoubleSpinbox::setValue("naDoubleSpinBox", state.na, GTGlobals::UseKeyBoard, parent);
    GTDoubleSpinbox::setValue("mgDoubleSpinBox", state.mg, GTGlobals::UseKeyBoard, parent);
    GTSpinBox::setValue("pSpinBox", state.percent, GTGlobals::UseKeyBoard, parent);
    GTSpinBox::setValue("maxSpinBox", state.numFold, GTGlobals::UseKeyBoard, parent);
    GTSpinBox::setValue("wSpinBox", state.window, GTGlobals::UseKeyBoard, parent);
    GTSpinBox::setValue("maxBpSpinBox", state.distance, GTGlobals::UseKeyBoard, parent);
    GTSpinBox::setValue("labFrSpinBox", state.frequency, GTGlobals::UseKeyBoard, parent);
    GTDoubleSpinbox::setValue("rotAngDoubleSpinBox", state.angle, GTGlobals::UseKeyBoard, parent);

    if (state.region.first > 0 && state.region.second > 0) {
        auto regionSelector = GTWidget::findExactWidget<RegionSelector*>("range_selector", parent);
        GTRegionSelector::setRegion(regionSelector, {state.region.first, state.region.second});
    }

    GTTabWidget::clickTab("tabWidget", parent, "Output");
    if (!state.out.isEmpty()) {
        GTLineEdit::setText("outPathLineEdit", state.out, parent);
    }
    GTSpinBox::setValue("dpiSpinBox", state.dpi, parent);
}
static WidgetStates getState(QWidget* parent = nullptr) {
    WidgetStates state;
    state.temperature = GTSpinBox::getValue("tSpinBox", parent);
    state.na = GTWidget::findDoubleSpinBox("naDoubleSpinBox", parent)->value();
    state.mg = GTWidget::findDoubleSpinBox("mgDoubleSpinBox", parent)->value();
    state.percent = GTSpinBox::getValue("pSpinBox", parent);
    state.numFold = GTSpinBox::getValue("maxSpinBox", parent);
    state.window = GTSpinBox::getValue("wSpinBox", parent);
    state.distance = GTSpinBox::getValue("maxBpSpinBox", parent);
    state.frequency = GTSpinBox::getValue("labFrSpinBox", parent);
    state.angle = GTWidget::findDoubleSpinBox("rotAngDoubleSpinBox", parent)->value();
    state.region.first = GTLineEdit::getText("start_edit_line", parent).toInt();
    state.region.second = GTLineEdit::getText("end_edit_line", parent).toInt();
    state.out = GTLineEdit::getText("outPathLineEdit", parent);
    state.dpi = GTSpinBox::getValue("dpiSpinBox", parent);
    return state;
}
static WidgetStates genRandState() {
    static std::mt19937 gen {std::random_device {}()};
    WidgetStates randState;

    randState.temperature = std::uniform_int_distribution<int>(0, 100)(gen);
    std::uniform_int_distribution<int> concDist(0, 150);
    randState.na = static_cast<double>(concDist(gen)) / 100;
    randState.mg = static_cast<double>(concDist(gen)) / 100;
    randState.percent = std::uniform_int_distribution<int>(1, 100)(gen);
    randState.numFold = std::uniform_int_distribution<int>(1, 100)(gen);
    randState.window = std::uniform_int_distribution<int>(-1, 50)(gen);
    randState.distance = std::uniform_int_distribution<int>(0, 6000)(gen);
    randState.frequency = std::uniform_int_distribution<int>(-1, 1000)(gen);
    randState.angle = static_cast<double>(std::uniform_int_distribution<int>(-1800, 1800)(gen)) / 10;

    std::uniform_int_distribution<int> regionDist(1, 3000);
    int first = regionDist(gen);
    int second = regionDist(gen);
    randState.region = {std::min(first, second), std::max(first, second)};

    randState.out = "mfold_test_" +
                    QString::number(std::uniform_int_distribution<int>(0, std::numeric_limits<int>::max())(gen));
    randState.dpi = std::uniform_int_distribution<int>(60, 1800)(gen);
    return randState;
}
}  // namespace
GUI_TEST_CLASS_DEFINITION(test_0007_save_dialog) {
    // 1. Check that dialog state is preserved upon restart (except for region settings -- they should not be saved).
    // 2. Check that dialog state is reset when Restore Defaults button is pressed (except for region, path and dpi
    //      settings).

    // Open sequence. Check its appearance.
    GTFileDialog::openFile(dataDir + "samples/Genbank/", "CVU55762.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    // Generate some random settings. They are used for CVU55762 only.
    WidgetStates randSettings = genRandState();

    // Call dialog and change settings to random. Remember entered values.
    class ChangeSettings final : public CustomScenario {
        WidgetStates newState;

    public:
        explicit ChangeSettings(const WidgetStates& newState)
            : newState(newState) {
        }
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            setState(newState, dialog);
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };
    GTUtilsDialog::waitForDialog(new AnyDialogFiller("MfoldDialog", new ChangeSettings(randSettings)));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Mfold");

    // 1. Call dialog and compare settings with already entered. The region must be equal to selected one.
    // 2. Then reset settings and check that they're equal to default. Region and output settings shouldn't change!

    // Select region that differs from the previous one and from the entire sequence.
    GTUtilsSequenceView::selectSequenceRegion(2, 499);
    randSettings.region = {2, 499};

    // Call dialog.
    class CompareAndRestore final : public CustomScenario {
        WidgetStates expected;

    public:
        explicit CompareAndRestore(const WidgetStates& previousState)
            : expected(previousState) {
        }
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();

            // 1. Compare settings with the ones you remember.
            CHECK_SET_ERR(getState(dialog) == expected, "States aren't equal");

            // Click Restore Defaults.
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::RestoreDefaults);

            // 2. Check that settings are equal to default settings, except for output settings.
            WidgetStates defaultState;
            defaultState.region = expected.region;
            defaultState.out = expected.out;
            defaultState.dpi = expected.dpi;
            CHECK_SET_ERR(getState(dialog) == defaultState, "States aren't equal (second check)");

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };
    GTUtilsDialog::waitForDialog(new AnyDialogFiller("MfoldDialog", new CompareAndRestore(randSettings)));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Mfold");
}
GUI_TEST_CLASS_DEFINITION(test_0008_save_dialog) {
    // 1. Check that dialog states are different for different windows.
    // 2. Check that resetting saved state of one will not affect saved state of the other.

    // Open sequences. Check their appearance.
    GTFileDialog::openFile(dataDir + "samples/Genbank/", "murine.gb");
    GTFileDialog::openFile(dataDir + "samples/Genbank/", "CVU55762.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Set random settings.
    GTUtilsMdi::activateWindow("CVU55762");
    WidgetStates randStateCvu = genRandState();
    class ChangeSettings final : public CustomScenario {
        WidgetStates newState;

    public:
        explicit ChangeSettings(const WidgetStates& newState)
            : newState(newState) {
        }
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            setState(newState, dialog);
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };
    GTUtilsDialog::waitForDialog(new AnyDialogFiller("MfoldDialog", new ChangeSettings(randStateCvu)));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Mfold");
    GTUtilsSequenceView::selectSequenceRegion(randStateCvu.region.first, randStateCvu.region.second);

    // Settings for murine shouldn't change.
    GTUtilsMdi::activateWindow("murine");
    class CompareDefaultAndChangeMurine final : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();

            WidgetStates def;
            def.region = {1, 5833};
            def.out = dataDir + "samples/Genbank";
            CHECK_SET_ERR(getState(dialog) == def, "States aren't equal");

            WidgetStates newState = genRandState();
            setState(newState, dialog);
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };
    GTUtilsDialog::waitForDialog(new AnyDialogFiller("MfoldDialog", new CompareDefaultAndChangeMurine));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Mfold");

    // Reset murine settings.
    class RestoreMurine final : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::RestoreDefaults);
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };
    GTUtilsDialog::waitForDialog(new AnyDialogFiller("MfoldDialog", new RestoreMurine));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Mfold");

    // Check that CVU55762 settings are the same.
    GTUtilsMdi::activateWindow("CVU55762");
    class ComparePrevious final : public CustomScenario {
        WidgetStates expected;

    public:
        explicit ComparePrevious(const WidgetStates& previousState)
            : expected(previousState) {
        }
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            CHECK_SET_ERR(getState(dialog) == expected, "States aren't equal (second check)");
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };
    GTUtilsDialog::waitForDialog(new AnyDialogFiller("MfoldDialog", new ComparePrevious(randStateCvu)));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Mfold");
}
}  // namespace GUITest_common_scenarios_mfold
}  // namespace U2
