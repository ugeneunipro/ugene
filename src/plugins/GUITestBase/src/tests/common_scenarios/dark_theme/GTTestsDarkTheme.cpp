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

#include "GTTestsDarkTheme.h"

#include "base_dialogs/GTFileDialog.h"
#include "base_dialogs/MessageBoxFiller.h"
#include "drivers/GTKeyboardDriver.h"
#include "primitives/GTComboBox.h"
#include "primitives/GTLabel.h"
#include "primitives/GTLineEdit.h"
#include "primitives/GTPlainTextEdit.h"
#include "primitives/GTTreeWidget.h"
#include "primitives/GTSpinBox.h"
#include "primitives/GTWidget.h"
#include "system/GTFile.h"

#include <QApplication>

#include "GTUtilsAnnotationsHighlightingTreeView.h"
#include "GTUtilsDocument.h"
#include "GTUtilsMsaEditor.h"
#include "GTUtilsMsaEditorSequenceArea.h"
#include "GTUtilsOptionsPanelPhyTree.h"
#include "GTUtilsProject.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsSequenceView.h"
#include "GTUtilsTaskTreeView.h"
#include "primitives/GTAction.h"
#include "primitives/GTMenu.h"
#include "primitives/GTToolbar.h"
#include "primitives/PopupChooser.h"
#include "runnables/ugene/corelibs/U2Gui/AppSettingsDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/CreateDocumentFromTextDialogFiller.h"
#include "runnables/ugene/ugeneui/SaveProjectDialogFiller.h"
#include "runnables/ugene/plugins/enzymes/FindEnzymesDialogFiller.h"

namespace U2 {

namespace GUITest_common_scenarios_dark_theme {



#ifdef Q_OS_DARWIN
static void testPreferences(QWidget* dialog) {
    GTComboBox::checkValuesPresence(GTWidget::findComboBox("styleCombo", dialog), {"Fusion", "Windows", "Macintosh"}, true);
    GTComboBox::checkValuesPresence(GTWidget::findComboBox("colorThemeCombo", dialog), {"Light", "Dark", "Auto"}, true);
    CHECK_SET_ERR(GTLabel::getText("errorLabel", dialog).isEmpty(), "Error should be empty");
}
#elif defined(Q_OS_UNIX)
static void testPreferences(QWidget* dialog) {
    GTComboBox::checkValuesPresence(GTWidget::findComboBox("styleCombo", dialog), {"Fusion", "Windows"}, true);
    GTComboBox::checkValuesPresence(GTWidget::findComboBox("colorThemeCombo", dialog), {"Light", "Dark"}, true);
    CHECK_SET_ERR(GTLabel::getText("errorLabel", dialog).isEmpty(), "Error should be empty");
}
#else
static void testPreferences(QWidget* dialog) {
    auto styleCombo = GTWidget::findComboBox("styleCombo", dialog);
    GTComboBox::selectItemByText(styleCombo , "WindowsVista");
    auto currentText = GTLabel::getText("errorLabel", dialog);
    static const QString expectedText = "Note: WindowsVista style is incompatible with Dark color theme. We suggest using Fusion";
    CHECK_SET_ERR(currentText == expectedText, QString("Expected error: %1, actual error: %2").arg(expectedText).arg(currentText));

    auto colorThemeCombo = GTWidget::findComboBox("colorThemeCombo", dialog);
    GTComboBox::checkValuesPresence(colorThemeCombo, {"Light"}, true);
    GTComboBox::selectItemByText(styleCombo , "Fusion");
    currentText = GTLabel::getText("errorLabel", dialog);
    CHECK_SET_ERR(currentText.isEmpty(), QString("Error is found, but should not be: %1").arg(currentText));

    GTComboBox::checkValuesPresence(colorThemeCombo, {"Light", "Dark"});
    GTComboBox::selectItemByText(colorThemeCombo, "Dark");
    GTComboBox::checkValuesPresence(styleCombo, {"Fusion", "Windows"}, true);
}
#endif

GUI_TEST_CLASS_DEFINITION(test_0001) {
    // Open "Settings -> Preferences"
    // macOS:
    // Expected 3 styles and 3 color themess
    // Linux:
    // Expected 2 styles and 2 color themes
    // Windows:
    // Set WindowsVista style
    // Expected: "WindowsVisty style is not compatible" error and no Auto and Dark color theme
    // Set Fusion style
    // Expected: Error dissapeared
    // Set "Dark" color theme
    // Expected: No WindowsVisata style
    // Click OK
    // Expected: start page color is dark
    class Scenario : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();

            testPreferences(dialog);

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };
    GTUtilsDialog::waitForDialog(new AppSettingsDialogFiller(new Scenario));
    GTMenu::clickMainMenuItem({"Settings", "Preferences..."});
}

void checkColor(QPalette::ColorGroup colorGroup, QPalette::ColorRole colorRole, const QString& expectedColor) {
    QPalette palette;
    auto color = palette.color(colorGroup, colorRole).name();
    CHECK_SET_ERR(color == expectedColor, QString("Incorrect color for group: %1, role: %2. Expected: %3, currecnt: %4")
        .arg(colorGroup).arg(colorRole).arg(expectedColor).arg(color));
};

#ifdef Q_OS_DARWIN
static void checkDark() {
    checkColor(QPalette::Active, QPalette::Window, "#323232");
    checkColor(QPalette::Active, QPalette::WindowText, "#ffffff");
    checkColor(QPalette::Disabled, QPalette::WindowText, "#ffffff");
    checkColor(QPalette::Active, QPalette::Base, "#1e1e1e");
    checkColor(QPalette::Active, QPalette::AlternateBase, "#989898");
    checkColor(QPalette::Active, QPalette::ToolTipBase, "#ffffff");
    checkColor(QPalette::Active, QPalette::ToolTipText, "#000000");
    checkColor(QPalette::Disabled, QPalette::Text, "#ffffff");
    checkColor(QPalette::Active, QPalette::Midlight, "#343434");
    checkColor(QPalette::Active, QPalette::Mid, "#242424");
    checkColor(QPalette::Active, QPalette::Shadow, "#000000");
    checkColor(QPalette::Active, QPalette::Button, "#323232");
    checkColor(QPalette::Active, QPalette::ButtonText, "#000000");
    checkColor(QPalette::Disabled, QPalette::ButtonText, "#1f1f1f");
    checkColor(QPalette::Active, QPalette::BrightText, "#373737");
    checkColor(QPalette::Active, QPalette::Link, "#419cff");
    checkColor(QPalette::Active, QPalette::LinkVisited, "#ff00ff");
    checkColor(QPalette::Active, QPalette::Highlight, "#3f638b");
    checkColor(QPalette::Disabled, QPalette::Highlight, "#464646");
    checkColor(QPalette::Active, QPalette::HighlightedText, "#ffffff");
    checkColor(QPalette::Disabled, QPalette::HighlightedText, "#ffffff");
}

static void checkLight() {
    checkColor(QPalette::Active, QPalette::Window, "#ececec");
    checkColor(QPalette::Active, QPalette::WindowText, "#000000");
    checkColor(QPalette::Disabled, QPalette::WindowText, "#000000");
    checkColor(QPalette::Active, QPalette::Base, "#ffffff");
    checkColor(QPalette::Active, QPalette::AlternateBase, "#f5f5f5");
    checkColor(QPalette::Active, QPalette::ToolTipBase, "#ffffff");
    checkColor(QPalette::Active, QPalette::ToolTipText, "#000000");
    checkColor(QPalette::Disabled, QPalette::Text, "#000000");
    checkColor(QPalette::Active, QPalette::Midlight, "#f5f5f5");
    checkColor(QPalette::Active, QPalette::Mid, "#a9a9a9");
    checkColor(QPalette::Active, QPalette::Shadow, "#000000");
    checkColor(QPalette::Active, QPalette::Button, "#ececec");
    checkColor(QPalette::Active, QPalette::ButtonText, "#000000");
    checkColor(QPalette::Disabled, QPalette::ButtonText, "#939393");
    checkColor(QPalette::Active, QPalette::BrightText, "#ffffff");
    checkColor(QPalette::Active, QPalette::Link, "#0068da");
    checkColor(QPalette::Active, QPalette::LinkVisited, "#ff00ff");
    checkColor(QPalette::Active, QPalette::Highlight, "#b3d7ff");
    checkColor(QPalette::Disabled, QPalette::Highlight, "#dcdcdc");
    checkColor(QPalette::Active, QPalette::HighlightedText, "#000000");
    checkColor(QPalette::Disabled, QPalette::HighlightedText, "#000000");
}

#else
static void checkDark() {
    checkColor(QPalette::Active, QPalette::Window, "#353535");
    checkColor(QPalette::Active, QPalette::WindowText, "#ffffff");
    checkColor(QPalette::Disabled, QPalette::WindowText, "#7f7f7f");
    checkColor(QPalette::Active, QPalette::Base, "#303030");
    checkColor(QPalette::Active, QPalette::AlternateBase, "#424242");
    checkColor(QPalette::Active, QPalette::ToolTipBase, "#ffffff");
    checkColor(QPalette::Active, QPalette::ToolTipText, "#353535");
    checkColor(QPalette::Disabled, QPalette::Text, "#7f7f7f");
    checkColor(QPalette::Active, QPalette::Midlight, "#1c1c1c");
    checkColor(QPalette::Active, QPalette::Mid, "#5f5f5f");
    checkColor(QPalette::Active, QPalette::Shadow, "#d2d2d2");
    checkColor(QPalette::Active, QPalette::Button, "#353535");
    checkColor(QPalette::Active, QPalette::ButtonText, "#ffffff");
    checkColor(QPalette::Disabled, QPalette::ButtonText, "#7f7f7f");
    checkColor(QPalette::Active, QPalette::BrightText, "#ff0000");
    checkColor(QPalette::Active, QPalette::Link, "#add8e6");
    checkColor(QPalette::Active, QPalette::LinkVisited, "#9d8bd6");
    checkColor(QPalette::Active, QPalette::Highlight, "#2a82da");
    checkColor(QPalette::Disabled, QPalette::Highlight, "#505050");
    checkColor(QPalette::Active, QPalette::HighlightedText, "#ffffff");
    checkColor(QPalette::Disabled, QPalette::HighlightedText, "#7f7f7f");
}
#if defined(Q_OS_UNIX)
static void checkLight() {
    checkColor(QPalette::Active, QPalette::Window, "#efefef");
    checkColor(QPalette::Active, QPalette::WindowText, "#000000");
    checkColor(QPalette::Disabled, QPalette::WindowText, "#bebebe");
    checkColor(QPalette::Active, QPalette::Base, "#ffffff");
    checkColor(QPalette::Active, QPalette::AlternateBase, "#f7f7f7");
    checkColor(QPalette::Active, QPalette::ToolTipBase, "#ffffdc");
    checkColor(QPalette::Active, QPalette::ToolTipText, "#000000");
    checkColor(QPalette::Disabled, QPalette::Text, "#bebebe");
    checkColor(QPalette::Active, QPalette::Midlight, "#cacaca");
    checkColor(QPalette::Active, QPalette::Mid, "#b8b8b8");
    checkColor(QPalette::Active, QPalette::Shadow, "#767676");
    checkColor(QPalette::Active, QPalette::Button, "#efefef");
    checkColor(QPalette::Active, QPalette::ButtonText, "#000000");
    checkColor(QPalette::Disabled, QPalette::ButtonText, "#bebebe");
    checkColor(QPalette::Active, QPalette::BrightText, "#ffffff");
    checkColor(QPalette::Active, QPalette::Link, "#0000ff");
    checkColor(QPalette::Active, QPalette::LinkVisited, "#ff00ff");
    checkColor(QPalette::Active, QPalette::Highlight, "#308cc6");
    checkColor(QPalette::Disabled, QPalette::Highlight, "#919191");
    checkColor(QPalette::Active, QPalette::HighlightedText, "#ffffff");
    checkColor(QPalette::Disabled, QPalette::HighlightedText, "#ffffff");
}
#else
static void checkLight() {
    checkColor(QPalette::Active, QPalette::Window, "#f0f0f0");
    checkColor(QPalette::Active, QPalette::WindowText, "#000000");
    checkColor(QPalette::Disabled, QPalette::WindowText, "#787878");
    checkColor(QPalette::Active, QPalette::Base, "#ffffff");
    checkColor(QPalette::Active, QPalette::AlternateBase, "#f5f5f5");
    checkColor(QPalette::Active, QPalette::ToolTipBase, "#ffffdc");
    checkColor(QPalette::Active, QPalette::ToolTipText, "#000000");
    checkColor(QPalette::Disabled, QPalette::Text, "#787878");
    checkColor(QPalette::Active, QPalette::Midlight, "#e3e3e3");
    checkColor(QPalette::Active, QPalette::Mid, "#a0a0a0");
    checkColor(QPalette::Active, QPalette::Shadow, "#696969");
    checkColor(QPalette::Active, QPalette::Button, "#f0f0f0");
    checkColor(QPalette::Active, QPalette::ButtonText, "#000000");
    checkColor(QPalette::Disabled, QPalette::ButtonText, "#787878");
    checkColor(QPalette::Active, QPalette::BrightText, "#ffffff");
    checkColor(QPalette::Active, QPalette::Link, "#0000ff");
    checkColor(QPalette::Active, QPalette::LinkVisited, "#ff00ff");
    checkColor(QPalette::Active, QPalette::Highlight, "#0078d7");
    checkColor(QPalette::Disabled, QPalette::Highlight, "#0078d7");
    checkColor(QPalette::Active, QPalette::HighlightedText, "#ffffff");
    checkColor(QPalette::Disabled, QPalette::HighlightedText, "#ffffff");
}
#endif
#endif

GUI_TEST_CLASS_DEFINITION(test_0002) {
    // Check color from palette
    // Expected: white theme
    // Set Dark theme
    // Check color from palette
    // Expected: dark theme

    checkLight();

    GTMenu::clickMainMenuItem({"Help", "Switch color theme"});

    checkDark();
}

GUI_TEST_CLASS_DEFINITION(test_0003) {
    // Set Dark theme
    // Open murine.gb
    // Expected: (48, 48, 48) background color
    // Expected: misc_feature annotatation color #7f7f4c
    // Expected: CDS annotatation color #4d7f7f
    // Open GC content Graph
    // Expected: graph color is white, background is (48, 48, 48)
    // Open dotplot dialog
    // Expected: repeats have white color
    // Open restriction sites dialog and select any enzyme
    // Check html enzyme representation

    GTMenu::clickMainMenuItem({"Help", "Switch color theme"});
    GTFileDialog::openFile(dataDir + "samples/Genbank", "murine.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();
    GTWidget::click(GTWidget::findWidget("OP_ANNOT_HIGHLIGHT"));
    auto miscFeatureColor = GTUtilsAnnotHighlightingTreeView::getItemColor("misc_feature").name();
    CHECK_SET_ERR(miscFeatureColor == "#7f7f4c", QString("misc_feature color expected: #7f7f4c, current: %1").arg(miscFeatureColor));

    auto cdsColor = GTUtilsAnnotHighlightingTreeView::getItemColor("CDS").name();
    CHECK_SET_ERR(cdsColor == "#4d7f7f", QString("CDS color expected: #4d7f7f, current: %1").arg(cdsColor));

    GTUtilsDialog::waitForDialog(new PopupChooser({"GC Content (%)"}));
    GTWidget::click(GTWidget::findWidget("GraphMenuAction"));
    auto graphColor = GTUtilsSequenceView::getGraphColor(GTUtilsSequenceView::getGraphView()).name();
    CHECK_SET_ERR(graphColor == "#ffffff", QString("Graph color expected: color #ffffff, current: %1").arg(graphColor));

    class Scenario : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();

            auto enzymesTree = GTWidget::findTreeWidget("tree", dialog);
            QTreeWidgetItem* item = GTTreeWidget::findItem(enzymesTree, "AanI");
            GTTreeWidget::click(item);
            auto tooltip = item->data(3, Qt::ToolTipRole).toString();
            auto toltipFromFile = GTFile::readAll(testDir + "_common_data/enzymes/dark_theme_0003.html");
            CHECK_SET_ERR(toltipFromFile == tooltip, "Incorrect enzyme");

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(new FindEnzymesDialogFiller({""}, new Scenario));
    GTWidget::click(GTToolbar::getWidgetForActionObjectName(GTToolbar::getToolbar(MWTOOLBAR_ACTIVEMDI), "Find restriction sites"));
}

GUI_TEST_CLASS_DEFINITION(test_0004) {
    // Set Dark theme
    // Open COI.aln
    // Check color scheme color
    // Check, that following color schemes exists: UGENE Dark, UGENE Sanger Dark
    // Switch to the UGENE sanger Dark
    // Check color scheme color

    GTMenu::clickMainMenuItem({"Help", "Switch color theme"});
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    auto checkColor = [](const QPoint& coord, const QString& expectedColor) {
        auto color = GTUtilsMSAEditorSequenceArea::getColor(coord);
        CHECK_SET_ERR(color == expectedColor, QString("Incorrect color at (%1, %2), currecnt: %3, expected: %4")
            .arg(coord.x()).arg(coord.y()).arg(expectedColor).arg(color));
    };
    checkColor(QPoint(0, 0), "#ff4271");
    checkColor(QPoint(1, 0), "#c8cc00");
    checkColor(QPoint(3, 0), "#4eade1");
    checkColor(QPoint(5, 0), "#00bf00");

    GTWidget::click(GTWidget::findWidget("OP_MSA_HIGHLIGHTING"));
    auto colorScheme = GTWidget::findComboBox("colorScheme");
    GTComboBox::checkValues(colorScheme, {"UGENE Dark", "UGENE Sanger Dark"});
    GTComboBox::selectItemByText(colorScheme, "UGENE Sanger Dark");
    checkColor(QPoint(0, 0), "#e2666a");
    checkColor(QPoint(1, 0), "#30ba80");
    checkColor(QPoint(3, 0), "#595959");
    checkColor(QPoint(5, 0), "#348ab5");
}

GUI_TEST_CLASS_DEFINITION(test_0005) {
    // Set Dark theme
    // Open COI.nwk
    // Expected: line color is white on the tree settings page

    GTMenu::clickMainMenuItem({"Help", "Switch color theme"});
    GTFileDialog::openFile(dataDir + "samples/Newick/COI.nwk");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsOptionPanelPhyTree::openTab();

    class Scenario : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();

            QList<QSpinBox*> spinList = dialog->findChildren<QSpinBox*>();
            auto redSb = spinList.at(3);
            CHECK_SET_ERR(GTSpinBox::getValue(redSb) == 255, QString("Incorrect red, expected: 255, current: %1").arg(GTSpinBox::getValue(redSb)));
            auto greenSb = spinList.at(4);
            CHECK_SET_ERR(GTSpinBox::getValue(greenSb) == 255, QString("Incorrect green, expected: 255, current: %1").arg(GTSpinBox::getValue(greenSb)));
            auto blueSb = spinList.at(5);
            CHECK_SET_ERR(GTSpinBox::getValue(blueSb) == 255, QString("Incorrect blue, expected: 255, current: %1").arg(GTSpinBox::getValue(blueSb)));

            GTKeyboardDriver::keyClick(Qt::Key_Escape);
        }
    };

    GTUtilsDialog::waitForDialog(new Filler("",  new Scenario));
    auto branchesColorButton = GTWidget::findWidget("branchesColorButton");
    GTWidget::click(branchesColorButton);
}

}  // namespace GUITest_common_scenarios_dark_theme
}  // namespace U2
