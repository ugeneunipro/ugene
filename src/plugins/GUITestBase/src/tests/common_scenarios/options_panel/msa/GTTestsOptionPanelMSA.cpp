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
#include <base_dialogs/ColorDialogFiller.h>
#include <base_dialogs/GTFileDialog.h>
#include <base_dialogs/MessageBoxFiller.h>
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTAction.h>
#include <primitives/GTCheckBox.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTDoubleSpinBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTMenu.h>
#include <primitives/GTRadioButton.h>
#include <primitives/GTSlider.h>
#include <primitives/GTWidget.h>
#include <primitives/PopupChooser.h>
#include <system/GTFile.h>
#include <utils/GTThread.h>

#include <QApplication>
#include <QDir>
#include <QGraphicsItem>
#include <QMainWindow>

#include <U2Core/AppContext.h>

#include "GTTestsOptionPanelMSA.h"
#include "GTUtilsLog.h"
#include "GTUtilsMdi.h"
#include "GTUtilsMsaEditor.h"
#include "GTUtilsMsaEditorSequenceArea.h"
#include "GTUtilsOptionPanelMSA.h"
#include "GTUtilsPhyTree.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsTaskTreeView.h"
#include "api/GTBaseCompleter.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/BuildTreeDialogFiller.h"

namespace U2 {

namespace GUITest_common_scenarios_options_panel_MSA {
using namespace HI;
GUI_TEST_CLASS_DEFINITION(general_test_0001) {
    const QString seqName = "Phaneroptera_falcata";
    //    1. Open file data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Open general option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::General);
    //    3. Use button to add Phaneroptera_falcata as referene
    GTUtilsOptionPanelMsa::addReference(os, seqName);
    //    Expected state:
    //    reference sequence line edit is empty
    auto sequenceLineEdit = GTWidget::findLineEdit(os, "sequenceLineEdit");
    QString text = sequenceLineEdit->text();
    CHECK_SET_ERR(text == seqName, QString("sequenceLineEdit contains %1, expected: %2").arg(text).arg(seqName));
    CHECK_SET_ERR(GTUtilsMSAEditorSequenceArea::isSequenceHighlighted(os, seqName), "sequence not highlighted");
    //    Expected state: Phaneroptera_falcata highlighted as reference

    //    4. Use button to remove reference
    GTUtilsOptionPanelMsa::removeReference(os);
    //    Expected state:
    //    reference sequence line edit contains "select and add"
    //    Phaneroptera_falcata is not highlighted as reference
    text = sequenceLineEdit->text();
    CHECK_SET_ERR(text.isEmpty(), QString("sequenceLineEdit contains %1, no text expected").arg(text));
    CHECK_SET_ERR(!GTUtilsMSAEditorSequenceArea::isSequenceHighlighted(os, seqName), "sequence not highlighted");
}

GUI_TEST_CLASS_DEFINITION(general_test_0001_1) {
    // Difference: popup completer is used
    const QString seqName = "Phaneroptera_falcata";
    //    1. Open file data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Open general option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::General);
    //    3. Use button to add Phaneroptera_falcata as reference
    GTUtilsOptionPanelMsa::addReference(os, seqName, GTUtilsOptionPanelMsa::Completer);
    //    Expected state:
    //    reference sequence line edit contains Phaneroptera_falcata
    auto sequenceLineEdit = GTWidget::findLineEdit(os, "sequenceLineEdit");
    QString text = sequenceLineEdit->text();
    CHECK_SET_ERR(text == seqName, QString("sequenceLineEdit contains %1, expected: %2").arg(text).arg(seqName));
    CHECK_SET_ERR(GTUtilsMSAEditorSequenceArea::isSequenceHighlighted(os, seqName), "sequence not highlighted");
    //    Phaneroptera_falcata highlighted as reference

    //    4. Use button to remove reference
    GTUtilsOptionPanelMsa::removeReference(os);
    //    Expected state:
    //    reference sequence line edit contains "select and add"
    //    Phaneroptera_falcata is not highlighted as reference
    text = sequenceLineEdit->text();
    CHECK_SET_ERR(text.isEmpty(), QString("sequenceLineEdit contains %1, no text expected").arg(text));
    CHECK_SET_ERR(!GTUtilsMSAEditorSequenceArea::isSequenceHighlighted(os, seqName), "sequence not highlighted");
}

GUI_TEST_CLASS_DEFINITION(general_test_0002) {
    const QString seqName = "Phaneroptera_falcata";
    // 1. Open file data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    // 2. Open general option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::General);
    // 3. Type "phan" in reference line edit
    auto sequenceLineEdit = GTWidget::findLineEdit(os, "sequenceLineEdit");
    GTLineEdit::setText(os, sequenceLineEdit, "phan");
    QStringList names = GTBaseCompleter::getNames(os, sequenceLineEdit);
    // Expected state: popup helper contains Phaneroptera_falcata.(case insensitivity is checked)
    int num = names.count();
    CHECK_SET_ERR(num == 1, QString("wrong number of sequences in completer. Expected 1, found %1").arg(num));

    QString name = names[0];
    CHECK_SET_ERR(name == seqName, QString("wrong sequence name. Expected %1, found %2").arg(seqName).arg(name));
    GTWidget::click(os, GTUtilsMdi::activeWindow(os));
}

GUI_TEST_CLASS_DEFINITION(general_test_0003) {
    //    1. Open file data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Open general option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::General);
    //    3. Type some string which is not part some sequence's name
    auto sequenceLineEdit = GTWidget::findLineEdit(os, "sequenceLineEdit");
    GTLineEdit::setText(os, sequenceLineEdit, "wrong name");
    //    Expected state: empty popup helper appeared
    bool empty = GTBaseCompleter::isEmpty(os, sequenceLineEdit);
    CHECK_SET_ERR(empty, "completer is not empty");
    GTWidget::click(os, sequenceLineEdit);  // needed to close completer
}

GUI_TEST_CLASS_DEFINITION(general_test_0004) {
    //    1. Open file data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Open general option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::General);
    //    3. Rename Isophya_altaica_EF540820 to Phaneroptera_falcata
    GTUtilsMSAEditorSequenceArea::renameSequence(os, "Isophya_altaica_EF540820", "Phaneroptera_falcata");
    //    4. Type "phan" in reference line edit
    auto sequenceLineEdit = GTWidget::findLineEdit(os, "sequenceLineEdit");
    GTLineEdit::setText(os, sequenceLineEdit, "Phan");
    QStringList completerList = GTBaseCompleter::getNames(os, sequenceLineEdit);
    //    Expected state: two sequence names "Phaneroptera_falcata" appeared in popup helper
    CHECK_SET_ERR(completerList.count() == 2, "wrong number of sequences in completer");
    QString first = completerList[0];
    QString second = completerList[1];
    CHECK_SET_ERR(first == "Phaneroptera_falcata", QString("first sequence in completer is wrong: %1").arg(first))
    CHECK_SET_ERR(second == "Phaneroptera_falcata", QString("second sequence in completer is wrong: %1").arg(second))
    GTWidget::click(os, sequenceLineEdit);  // needed to close completer
}

GUI_TEST_CLASS_DEFINITION(general_test_0005) {
    //    1. Open file data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Open general option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::General);

    //    3. Delete Hetrodes_pupus_EF540832
    GTUtilsMSAEditorSequenceArea::selectSequence(os, "Hetrodes_pupus_EF540832");
    GTKeyboardDriver::keyClick(Qt::Key_Delete);

    //    Expected state: Sequence number is 17
    const int height = GTUtilsOptionPanelMsa::getHeight(os);
    CHECK_SET_ERR(height == 17, QString("wrong height. expected 17, found %1").arg(height));

    //    4. Select one column. Press delete
    GTUtilsMsaEditor::clickColumn(os, 5);
    GTKeyboardDriver::keyClick(Qt::Key_Delete);

    //    Expected state: Length is 603
    const int length = GTUtilsOptionPanelMsa::getLength(os);
    CHECK_SET_ERR(length == 603, QString("wrong length. expected 603, found %1").arg(length));
}

GUI_TEST_CLASS_DEFINITION(highlighting_test_0001) {
    //    1. Open file test/_common_data/scenarios/msa/ma2_gapped.aln
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Open highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::Highlighting);
    //    3. Select "UGENE" color scheme
    auto colorScheme = GTWidget::findComboBox(os, "colorScheme");
    GTComboBox::selectItemByText(os, colorScheme, "UGENE");
    QString a = GTUtilsMSAEditorSequenceArea::getColor(os, QPoint(0, 0));
    QString t = GTUtilsMSAEditorSequenceArea::getColor(os, QPoint(0, 2));
    QString g = GTUtilsMSAEditorSequenceArea::getColor(os, QPoint(2, 0));
    QString c = GTUtilsMSAEditorSequenceArea::getColor(os, QPoint(4, 0));
    QString gap = GTUtilsMSAEditorSequenceArea::getColor(os, QPoint(4, 2));
    CHECK_SET_ERR(a == "#fcff92", QString("a has color %1").arg(a));
    CHECK_SET_ERR(t == "#ff99b1", QString("t has color %1").arg(t));
    CHECK_SET_ERR(g == "#4eade1", QString("g has color %1").arg(g));
    CHECK_SET_ERR(c == "#70f970", QString("c has color %1").arg(c));
    CHECK_SET_ERR(gap == "#ffffff", QString("gap has color %1").arg(gap));
    //    4. Check colors for all symbols
    //    (branches: check Jalview, Percentage identity, Percentage identity(gray), UGENE color schemes)
}

GUI_TEST_CLASS_DEFINITION(highlighting_test_0001_1) {
    //    1. Open file test/_common_data/scenarios/msa/ma2_gapped.aln
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Open highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::Highlighting);
    //    3. Select "No colors" color scheme
    auto colorScheme = GTWidget::findComboBox(os, "colorScheme");
    GTComboBox::selectItemByText(os, colorScheme, "No colors");
    QString a = GTUtilsMSAEditorSequenceArea::getColor(os, QPoint(0, 0));
    QString t = GTUtilsMSAEditorSequenceArea::getColor(os, QPoint(0, 2));
    QString g = GTUtilsMSAEditorSequenceArea::getColor(os, QPoint(2, 0));
    QString c = GTUtilsMSAEditorSequenceArea::getColor(os, QPoint(4, 0));
    QString gap = GTUtilsMSAEditorSequenceArea::getColor(os, QPoint(4, 2));
    CHECK_SET_ERR(a == "#ffffff", QString("a has color %1").arg(a));
    CHECK_SET_ERR(t == "#ffffff", QString("t has color %1").arg(t));
    CHECK_SET_ERR(g == "#ffffff", QString("g has color %1").arg(g));
    CHECK_SET_ERR(c == "#ffffff", QString("c has color %1").arg(c));
    CHECK_SET_ERR(gap == "#ffffff", QString("gap has color %1").arg(gap));
    //    4. Check colors for all symbols
    //    (branches: check Jalview, Percentage identity, Percentage identity(gray), UGENE color schemes)
}

GUI_TEST_CLASS_DEFINITION(highlighting_test_0001_2) {
    //    1. Open file test/_common_data/scenarios/msa/ma2_gapped.aln
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Open highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::Highlighting);
    //    3. Select "Jalview" color scheme
    auto colorScheme = GTWidget::findComboBox(os, "colorScheme");
    GTComboBox::selectItemByText(os, colorScheme, "Jalview");
    QString a = GTUtilsMSAEditorSequenceArea::getColor(os, QPoint(0, 0));
    QString t = GTUtilsMSAEditorSequenceArea::getColor(os, QPoint(0, 2));
    QString g = GTUtilsMSAEditorSequenceArea::getColor(os, QPoint(2, 0));
    QString c = GTUtilsMSAEditorSequenceArea::getColor(os, QPoint(4, 0));
    QString gap = GTUtilsMSAEditorSequenceArea::getColor(os, QPoint(4, 2));
    CHECK_SET_ERR(a == "#64f73f", QString("a has color %1").arg(a));
    CHECK_SET_ERR(t == "#3c88ee", QString("t has color %1").arg(t));
    CHECK_SET_ERR(g == "#eb413c", QString("g has color %1").arg(g));
    CHECK_SET_ERR(c == "#ffb340", QString("c has color %1").arg(c));
    CHECK_SET_ERR(gap == "#ffffff", QString("gap has color %1").arg(gap));
    //    4. Check colors for all symbols
    //    (branches: check Jalview, Percentage identity, Percentage identity(gray), UGENE color schemes)
}

GUI_TEST_CLASS_DEFINITION(highlighting_test_0001_3) {
    //    1. Open file test/_common_data/scenarios/msa/ma2_gapped.aln
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Open highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::Highlighting);
    //    3. Select "Percentage identity" color scheme
    auto colorScheme = GTWidget::findComboBox(os, "colorScheme");
    GTComboBox::selectItemByText(os, colorScheme, "Percentage identity");
    QString a = GTUtilsMSAEditorSequenceArea::getColor(os, QPoint(0, 0));
    QString t = GTUtilsMSAEditorSequenceArea::getColor(os, QPoint(0, 2));
    QString g = GTUtilsMSAEditorSequenceArea::getColor(os, QPoint(2, 0));
    QString c = GTUtilsMSAEditorSequenceArea::getColor(os, QPoint(5, 0));
    QString gap = GTUtilsMSAEditorSequenceArea::getColor(os, QPoint(4, 2));
    CHECK_SET_ERR(a == "#ccccff", QString("a has color %1").arg(a));
    CHECK_SET_ERR(t == "#ffffff", QString("t has color %1").arg(t));
    CHECK_SET_ERR(g == "#6464ff", QString("g has color %1").arg(g));
    CHECK_SET_ERR(c == "#9999ff", QString("c has color %1").arg(c));
    CHECK_SET_ERR(gap == "#ffffff", QString("gap has color %1").arg(gap));
    //    4. Check colors for all symbols
    //    (branches: check Jalview, Percentage identity, Percentage identity(gray), UGENE color schemes)
}

GUI_TEST_CLASS_DEFINITION(highlighting_test_0001_4) {
    //    1. Open file test/_common_data/scenarios/msa/ma2_gapped.aln
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Open highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::Highlighting);
    //    3. Select "Percentage identity (gray)" color scheme
    auto colorScheme = GTWidget::findComboBox(os, "colorScheme");
    GTComboBox::selectItemByText(os, colorScheme, "Percentage identity (gray)");
    QString a = GTUtilsMSAEditorSequenceArea::getColor(os, QPoint(0, 0));
    QString t = GTUtilsMSAEditorSequenceArea::getColor(os, QPoint(0, 2));
    QString g = GTUtilsMSAEditorSequenceArea::getColor(os, QPoint(2, 0));
    QString c = GTUtilsMSAEditorSequenceArea::getColor(os, QPoint(5, 0));
    QString gap = GTUtilsMSAEditorSequenceArea::getColor(os, QPoint(4, 2));
    CHECK_SET_ERR(a == "#cccccc", QString("a has color %1").arg(a));
    CHECK_SET_ERR(t == "#ffffff", QString("t has color %1").arg(t));
    CHECK_SET_ERR(g == "#646464", QString("g has color %1").arg(g));
    CHECK_SET_ERR(c == "#999999", QString("c has color %1").arg(c));
    CHECK_SET_ERR(gap == "#ffffff", QString("gap has color %1").arg(gap));
    //    4. Check colors for all symbols
    //    (branches: check Jalview, Percentage identity, Percentage identity(gray), UGENE color schemes)
}

GUI_TEST_CLASS_DEFINITION(highlighting_test_0002) {
    //    1. Open file test/_common_data/scenarios/msa/ma2_gapped.aln
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    //    2. Create custom color scheme
    QString suffix = GTUtils::genUniqueString();
    QString schemeName = name + "Scheme" + suffix;
    GTUtilsMSAEditorSequenceArea::createColorScheme(os, schemeName, NewColorSchemeCreator::nucl);

    //    3. Open highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::Highlighting);
    GTUtilsOptionPanelMsa::isTabOpened(os, GTUtilsOptionPanelMsa::Highlighting);

    //    Expected state: color scheme added to "Color" combobox
    auto colorScheme = GTWidget::findComboBox(os, "colorScheme");
    GTComboBox::selectItemByText(os, colorScheme, "No colors");
    GTComboBox::selectItemByText(os, colorScheme, schemeName);

    //    4. Select custom scheme
    //    Expected state: scheme changed
    QString a = GTUtilsMSAEditorSequenceArea::getColor(os, QPoint(0, 0));
    QString t = GTUtilsMSAEditorSequenceArea::getColor(os, QPoint(0, 2));
    QString g = GTUtilsMSAEditorSequenceArea::getColor(os, QPoint(2, 0));
    QString c = GTUtilsMSAEditorSequenceArea::getColor(os, QPoint(4, 0));
    QString gap = GTUtilsMSAEditorSequenceArea::getColor(os, QPoint(4, 2));
    CHECK_SET_ERR(a == "#fcff92", QString("a has color %1").arg(a));
    CHECK_SET_ERR(t == "#ff99b1", QString("t has color %1").arg(t));
    CHECK_SET_ERR(g == "#4eade1", QString("g has color %1").arg(g));
    CHECK_SET_ERR(c == "#70f970", QString("c has color %1").arg(c));
    CHECK_SET_ERR(gap == "#ffffff", QString("gap has color %1").arg(gap));
}

GUI_TEST_CLASS_DEFINITION(highlighting_test_0002_1) {
    //    1. Open file test/_common_data/scenarios/msa/ma2_gapped.aln
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Create 3 color schemes
    QString suffix = GTUtils::genUniqueString();
    QString scheme1 = name + "_scheme1" + suffix;
    QString scheme2 = name + "_scheme2" + suffix;
    QString scheme3 = name + "_scheme3" + suffix;
    GTUtilsMSAEditorSequenceArea::createColorScheme(os, scheme1, NewColorSchemeCreator::nucl);
    GTUtilsMSAEditorSequenceArea::createColorScheme(os, scheme2, NewColorSchemeCreator::nucl);
    GTUtilsMSAEditorSequenceArea::createColorScheme(os, scheme3, NewColorSchemeCreator::nucl);
    //    Expected state: color schemes added to "Color" combobox
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::Highlighting);
    auto colorScheme = GTWidget::findComboBox(os, "colorScheme");
    GTComboBox::selectItemByText(os, colorScheme, scheme1);
    GTComboBox::selectItemByText(os, colorScheme, scheme2);
    GTComboBox::selectItemByText(os, colorScheme, scheme3);
}

GUI_TEST_CLASS_DEFINITION(highlighting_test_0003) {
    //    1. Open file test/_common_data/scenarios/msa/ma2_gapped.aln
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Create custom color scheme
    QString suffix = GTUtils::genUniqueString();
    const QString scheme = name + "_scheme111111111111111111111111111111111111111111111" + suffix;
    GTUtilsMSAEditorSequenceArea::createColorScheme(os, scheme, NewColorSchemeCreator::nucl);
    //    3. Open highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::Highlighting);
    //    4. Select custom scheme
    auto colorScheme = GTWidget::findComboBox(os, "colorScheme");
    GTComboBox::selectItemByText(os, colorScheme, scheme);
    //    5. Delete scheme which is selected
    GTUtilsMSAEditorSequenceArea::deleteColorScheme(os, scheme);

    //    UGENE not crashes
    //    default color sheme is selected
    const QString currentScheme = colorScheme->currentText();
    CHECK_SET_ERR(currentScheme == "UGENE", QString("wrong color scheme selected: expected '%1', got '%2'").arg("UGENE").arg(currentScheme));

    const QString a = GTUtilsMSAEditorSequenceArea::getColor(os, QPoint(0, 0));
    const QString t = GTUtilsMSAEditorSequenceArea::getColor(os, QPoint(0, 2));
    const QString g = GTUtilsMSAEditorSequenceArea::getColor(os, QPoint(2, 0));
    const QString c = GTUtilsMSAEditorSequenceArea::getColor(os, QPoint(4, 0));
    const QString gap = GTUtilsMSAEditorSequenceArea::getColor(os, QPoint(4, 2));
    CHECK_SET_ERR(a == "#fcff92", QString("a has color %1").arg(a));
    CHECK_SET_ERR(t == "#ff99b1", QString("t has color %1").arg(t));
    CHECK_SET_ERR(g == "#4eade1", QString("g has color %1").arg(g));
    CHECK_SET_ERR(c == "#70f970", QString("c has color %1").arg(c));
    CHECK_SET_ERR(gap == "#ffffff", QString("gap has color %1").arg(gap));
}

GUI_TEST_CLASS_DEFINITION(highlighting_test_0004) {
    //    1. Open file test/_common_data/alphabets/extended_amino.aln
    GTFileDialog::openFile(os, testDir + "_common_data/alphabets", "extended_amino.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Open highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::Highlighting);
    //    3. Select "No color" color scheme
    auto colorScheme = GTWidget::findComboBox(os, "colorScheme");
    GTComboBox::selectItemByText(os, colorScheme, "No colors");
    //    4. Check colors for all symbols
    for (int i = 0; i < 29; i++) {
        GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(i, 0), "#ffffff");
    }
}

GUI_TEST_CLASS_DEFINITION(highlighting_test_0004_1) {
    //    1. Open file test/_common_data/alphabets/extended_amino.aln
    GTFileDialog::openFile(os, testDir + "_common_data/alphabets", "extended_amino.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Open highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::Highlighting);
    //    3. Select "Buried index" color scheme
    auto colorScheme = GTWidget::findComboBox(os, "colorScheme");
    GTComboBox::selectItemByText(os, colorScheme, "Buried index");
    //    4. Check colors for all symbols
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(0, 0), "#00a35c");  // a
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(1, 0), "#00eb14");  // b
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(2, 0), "#0000ff");  // c
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(3, 0), "#00eb14");  // d
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(4, 0), "#00f10e");  // e
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(5, 0), "#008778");  // f
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(6, 0), "#009d62");  // g
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(7, 0), "#00d52a");  // h
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(8, 0), "#0054ab");  // i
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(28, 0), "#ffffff");  // j
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(9, 0), "#00ff00");  // k
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(10, 0), "#007b84");  // l
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(11, 0), "#009768");  // m
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(12, 0), "#00eb14");  // n
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(26, 0), "#ffffff");  // o
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(13, 0), "#00e01f");  // p
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(14, 0), "#00f10e");  // q
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(15, 0), "#00fc03");  // r
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(16, 0), "#00d52a");  // s
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(17, 0), "#00db24");  // t
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(27, 0), "#ffffff");  // u
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(18, 0), "#005fa0");  // v
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(19, 0), "#00a857");  // w
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(20, 0), "#00b649");  // x
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(21, 0), "#00e619");  // y
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(22, 0), "#00f10e");  // z
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(23, 0), "#ffffff");  //*
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(25, 0), "#ffffff");  // gap
}

GUI_TEST_CLASS_DEFINITION(highlighting_test_0004_2) {
    //    1. Open file test/_common_data/alphabets/extended_amino.aln
    GTFileDialog::openFile(os, testDir + "_common_data/alphabets", "extended_amino.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Open highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::Highlighting);
    //    3. Select "Clustal X" color scheme
    auto colorScheme = GTWidget::findComboBox(os, "colorScheme");
    GTComboBox::selectItemByText(os, colorScheme, "Clustal X");
    //    4. Check colors for all symbols
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(0, 0), "#80a0f0");  // a
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(1, 0), "#ffffff");  // b
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(2, 0), "#f08080");  // c
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(3, 0), "#c048c0");  // d
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(4, 0), "#c048c0");  // e
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(5, 0), "#80a0f0");  // f
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(6, 0), "#f09048");  // g
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(7, 0), "#15a4a4");  // h
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(8, 0), "#80a0f0");  // i
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(28, 0), "#ffffff");  // j
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(9, 0), "#f01505");  // k
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(10, 0), "#80a0f0");  // l
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(11, 0), "#80a0f0");  // m
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(12, 0), "#15c015");  // n
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(26, 0), "#ffffff");  // o
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(13, 0), "#c0c000");  // p
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(14, 0), "#15c015");  // q
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(15, 0), "#f01505");  // r
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(16, 0), "#15c015");  // s
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(17, 0), "#15c015");  // t
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(27, 0), "#ffffff");  // u
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(18, 0), "#80a0f0");  // v
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(19, 0), "#80a0f0");  // w
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(20, 0), "#ffffff");  // x
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(21, 0), "#15a4a4");  // y
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(22, 0), "#ffffff");  // z
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(23, 0), "#ffffff");  //*
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(25, 0), "#ffffff");  // gap
}

GUI_TEST_CLASS_DEFINITION(highlighting_test_0004_3) {
    //    1. Open file test/_common_data/alphabets/extended_amino.aln
    GTFileDialog::openFile(os, testDir + "_common_data/alphabets", "extended_amino.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Open highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::Highlighting);
    //    3. Select "Helix propensity" color scheme
    auto colorScheme = GTWidget::findComboBox(os, "colorScheme");
    GTComboBox::selectItemByText(os, colorScheme, "Helix propensity");
    //    4. Check colors for all symbols
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(0, 0), "#e718e7");  // a
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(1, 0), "#49b649");  // b
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(2, 0), "#23dc23");  // c
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(3, 0), "#778877");  // d
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(4, 0), "#ff00ff");  // e
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(5, 0), "#986798");  // f
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(6, 0), "#00ff00");  // g
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(7, 0), "#758a75");  // h
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(8, 0), "#8a758a");  // i
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(28, 0), "#ffffff");  // j
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(9, 0), "#a05fa0");  // k
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(10, 0), "#ae51ae");  // l
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(11, 0), "#ef10ef");  // m
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(12, 0), "#1be41b");  // n
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(26, 0), "#ffffff");  // o
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(13, 0), "#00ff00");  // p
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(14, 0), "#926d92");  // q
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(15, 0), "#6f906f");  // r
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(16, 0), "#36c936");  // s
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(17, 0), "#47b847");  // t
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(27, 0), "#ffffff");  // u
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(18, 0), "#857a85");  // v
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(19, 0), "#8a758a");  // w
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(20, 0), "#758a75");  // x
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(21, 0), "#21de21");  // y
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(22, 0), "#c936c9");  // z
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(23, 0), "#ffffff");  //*
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(25, 0), "#ffffff");  // gap
}

GUI_TEST_CLASS_DEFINITION(highlighting_test_0004_4) {
    //    1. Open file test/_common_data/alphabets/extended_amino.aln
    GTFileDialog::openFile(os, testDir + "_common_data/alphabets", "extended_amino.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Open highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::Highlighting);
    //    3. Select "Hydrophobicity" color scheme
    auto colorScheme = GTWidget::findComboBox(os, "colorScheme");
    GTComboBox::selectItemByText(os, colorScheme, "Hydrophobicity");
    //    4. Check colors for all symbols
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(0, 0), "#ad0052");  // a
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(1, 0), "#0c00f3");  // b
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(2, 0), "#c2003d");  // c
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(3, 0), "#0c00f3");  // d
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(4, 0), "#0c00f3");  // e
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(5, 0), "#cb0034");  // f
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(6, 0), "#6a0095");  // g
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(7, 0), "#1500ea");  // h
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(8, 0), "#ff0000");  // i
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(28, 0), "#ffffff");  // j
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(9, 0), "#0000ff");  // k
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(10, 0), "#ea0015");  // l
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(11, 0), "#b0004f");  // m
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(12, 0), "#0c00f3");  // n
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(26, 0), "#ffffff");  // o
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(13, 0), "#4600b9");  // p
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(14, 0), "#0c00f3");  // q
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(15, 0), "#0000ff");  // r
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(16, 0), "#5e00a1");  // s
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(17, 0), "#61009e");  // t
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(27, 0), "#ffffff");  // u
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(18, 0), "#f60009");  // v
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(19, 0), "#5b00a4");  // w
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(20, 0), "#680097");  // x
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(21, 0), "#4f00b0");  // y
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(22, 0), "#0c00f3");  // z
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(23, 0), "#ffffff");  //*
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(25, 0), "#ffffff");  // gap
}

GUI_TEST_CLASS_DEFINITION(highlighting_test_0004_5) {
    //    1. Open file test/_common_data/alphabets/extended_amino.aln
    GTFileDialog::openFile(os, testDir + "_common_data/alphabets", "extended_amino.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Open highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::Highlighting);
    //    3. Select "Strand propensity" color scheme
    auto colorScheme = GTWidget::findComboBox(os, "colorScheme");
    GTComboBox::selectItemByText(os, colorScheme, "Strand propensity");
    //    4. Check colors for all symbols
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(0, 0), "#5858a7");  // a
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(1, 0), "#4343bc");  // b
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(2, 0), "#9d9d62");  // c
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(3, 0), "#2121de");  // d
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(4, 0), "#0000ff");  // e
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(5, 0), "#c2c23d");  // f
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(6, 0), "#4949b6");  // g
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(7, 0), "#60609f");  // h
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(8, 0), "#ecec13");  // i
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(28, 0), "#ffffff");  // j
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(9, 0), "#4747b8");  // k
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(10, 0), "#b2b24d");  // l
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(11, 0), "#82827d");  // m
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(12, 0), "#64649b");  // n
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(26, 0), "#ffffff");  // o
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(13, 0), "#2323dc");  // p
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(14, 0), "#8c8c73");  // q
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(15, 0), "#6b6b94");  // r
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(16, 0), "#4949b6");  // s
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(17, 0), "#9d9d62");  // t
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(27, 0), "#ffffff");  // u
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(18, 0), "#ffff00");  // v
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(19, 0), "#c0c03f");  // w
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(20, 0), "#797986");  // x
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(21, 0), "#d3d32c");  // y
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(22, 0), "#4747b8");  // z
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(23, 0), "#ffffff");  //*
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(25, 0), "#ffffff");  // gap
}

GUI_TEST_CLASS_DEFINITION(highlighting_test_0004_6) {
    //    1. Open file test/_common_data/alphabets/extended_amino.aln
    GTFileDialog::openFile(os, testDir + "_common_data/alphabets", "extended_amino.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Open highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::Highlighting);
    //    3. Select "Tailor" color scheme
    auto colorScheme = GTWidget::findComboBox(os, "colorScheme");
    GTComboBox::selectItemByText(os, colorScheme, "Tailor");
    //    4. Check colors for all symbols
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(0, 0), "#ccff00");  // a
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(1, 0), "#ffffff");  // b
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(2, 0), "#ffff00");  // c
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(3, 0), "#ff0000");  // d
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(4, 0), "#ff0066");  // e
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(5, 0), "#00ff66");  // f
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(6, 0), "#ff9900");  // g
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(7, 0), "#0066ff");  // h
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(8, 0), "#66ff00");  // i
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(28, 0), "#ffffff");  // j
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(9, 0), "#6600ff");  // k
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(10, 0), "#33ff00");  // l
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(11, 0), "#00ff00");  // m
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(12, 0), "#cc00ff");  // n
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(26, 0), "#ffffff");  // o
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(13, 0), "#ffcc00");  // p
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(14, 0), "#ff00cc");  // q
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(15, 0), "#0000ff");  // r
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(16, 0), "#ff3300");  // s
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(17, 0), "#ff6600");  // t
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(27, 0), "#ffffff");  // u
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(18, 0), "#99ff00");  // v
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(19, 0), "#00ccff");  // w
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(20, 0), "#ffffff");  // x
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(21, 0), "#00ffcc");  // y
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(22, 0), "#ffffff");  // z
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(23, 0), "#ffffff");  //*
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(25, 0), "#ffffff");  // gap
}

GUI_TEST_CLASS_DEFINITION(highlighting_test_0004_7) {
    //    1. Open file test/_common_data/alphabets/extended_amino.aln
    GTFileDialog::openFile(os, testDir + "_common_data/alphabets", "extended_amino.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Open highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::Highlighting);
    //    3. Select "Turn propensity" color scheme
    auto colorScheme = GTWidget::findComboBox(os, "colorScheme");
    GTComboBox::selectItemByText(os, colorScheme, "Turn propensity");
    //    4. Check colors for all symbols
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(0, 0), "#2cd3d3");  // a
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(1, 0), "#f30c0c");  // b
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(2, 0), "#a85757");  // c
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(3, 0), "#e81717");  // d
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(4, 0), "#3fc0c0");  // e
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(5, 0), "#1ee1e1");  // f
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(6, 0), "#ff0000");  // g
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(7, 0), "#708f8f");  // h
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(8, 0), "#00ffff");  // i
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(28, 0), "#ffffff");  // j
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(9, 0), "#7e8181");  // k
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(10, 0), "#1ce3e3");  // l
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(11, 0), "#1ee1e1");  // m
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(12, 0), "#ff0000");  // n
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(26, 0), "#ffffff");  // o
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(13, 0), "#f60909");  // p
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(14, 0), "#778888");  // q
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(15, 0), "#708f8f");  // r
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(16, 0), "#e11e1e");  // s
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(17, 0), "#738c8c");  // t
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(27, 0), "#ffffff");  // u
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(18, 0), "#07f8f8");  // v
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(19, 0), "#738c8c");  // w
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(20, 0), "#7c8383");  // x
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(21, 0), "#9d6262");  // y
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(22, 0), "#5ba4a4");  // z
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(23, 0), "#ffffff");  //*
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(25, 0), "#ffffff");  // gap
}

GUI_TEST_CLASS_DEFINITION(highlighting_test_0004_8) {
    //    1. Open file test/_common_data/alphabets/extended_amino.aln
    GTFileDialog::openFile(os, testDir + "_common_data/alphabets", "extended_amino.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    //    2. Open highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::Highlighting);
    //    3. Select "UGENE" color scheme
    auto colorScheme = GTWidget::findComboBox(os, "colorScheme");
    GTComboBox::selectItemByText(os, colorScheme, "UGENE");
    //    4. Check colors for all symbols
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(0, 0), "#00ccff");  // a
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(1, 0), "#ccff99");  // b
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(2, 0), "#6600ff");  // c
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(3, 0), "#ffff00");  // d
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(4, 0), "#c0bdbb");  // e
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(5, 0), "#3df490");  // f
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(6, 0), "#ff5082");  // g
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(7, 0), "#fff233");  // h
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(8, 0), "#00abed");  // i
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(28, 0), "#6699ff");  // j
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(9, 0), "#ffee00");  // k
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(10, 0), "#008fc6");  // l
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(11, 0), "#1dc0ff");  // m
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(12, 0), "#33ff00");  // n
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(26, 0), "#ffff99");  // o
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(13, 0), "#d5426c");  // p
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(14, 0), "#3399ff");  // q
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(15, 0), "#d5c700");  // r
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(16, 0), "#ff83a7");  // s
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(17, 0), "#ffd0dd");  // t
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(27, 0), "#ff00cc");  // u
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(18, 0), "#ff6699");  // v
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(19, 0), "#33cc78");  // w
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(20, 0), "#fcfcfc");  // x
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(21, 0), "#65ffab");  // y
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(22, 0), "#ffffcc");  // z
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(23, 0), "#33ff00");  //*
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(25, 0), "#ffffff");  // gap
}

GUI_TEST_CLASS_DEFINITION(highlighting_test_0004_9) {
    //    1. Open file test/_common_data/alphabets/extended_amino.aln
    GTFileDialog::openFile(os, testDir + "_common_data/alphabets", "extended_amino.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Open highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::Highlighting);
    //    3. Select "Zappo" color scheme
    auto colorScheme = GTWidget::findComboBox(os, "colorScheme");
    GTComboBox::selectItemByText(os, colorScheme, "Zappo");
    //    4. Check colors for all symbols
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(0, 0), "#ffafaf");  // a
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(1, 0), "#ffffff");  // b
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(2, 0), "#ffff00");  // c
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(3, 0), "#ff0000");  // d
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(4, 0), "#ff0000");  // e
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(5, 0), "#ffc800");  // f
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(6, 0), "#ff00ff");  // g
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(7, 0), "#6464ff");  // h
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(8, 0), "#ffafaf");  // i
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(28, 0), "#ffffff");  // j
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(9, 0), "#6464ff");  // k
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(10, 0), "#ffafaf");  // l
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(11, 0), "#ffafaf");  // m
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(12, 0), "#00ff00");  // n
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(26, 0), "#ffffff");  // o
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(13, 0), "#ff00ff");  // p
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(14, 0), "#00ff00");  // q
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(15, 0), "#6464ff");  // r
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(16, 0), "#00ff00");  // s
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(17, 0), "#00ff00");  // t
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(27, 0), "#ffffff");  // u
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(18, 0), "#ffafaf");  // v
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(19, 0), "#ffc800");  // w
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(20, 0), "#ffffff");  // x
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(21, 0), "#ffc800");  // y
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(22, 0), "#ffffff");  // z
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(23, 0), "#ffffff");  //*
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(25, 0), "#ffffff");  // gap
}

GUI_TEST_CLASS_DEFINITION(highlighting_test_0005) {
    //    1. Open file test/_common_data/alphabets/extended_amino.aln
    GTFileDialog::openFile(os, testDir + "_common_data/alphabets", "extended_amino.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Create custom color scheme
    QString suffix = GTUtils::genUniqueString();
    const QString scheme = name + "_scheme" + suffix;
    GTUtilsMSAEditorSequenceArea::createColorScheme(os, scheme, NewColorSchemeCreator::amino);
    //    3. Open highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::Highlighting);
    //    Expected state: color scheme added to "Color" combobox
    auto colorScheme = GTWidget::findComboBox(os, "colorScheme");
    GTComboBox::selectItemByText(os, colorScheme, "No colors");
    GTComboBox::selectItemByText(os, colorScheme, scheme);
    //    4. Select custom scheme
    //    Expected state: scheme changed
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(0, 0), "#ad0052");  // a
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(1, 0), "#0c00f3");  // b
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(2, 0), "#c2003d");  // c
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(3, 0), "#0c00f3");  // d
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(4, 0), "#0c00f3");  // e
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(5, 0), "#cb0034");  // f
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(6, 0), "#6a0095");  // g
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(7, 0), "#1500ea");  // h
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(8, 0), "#ff0000");  // i
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(28, 0), "#ffffff");  // j
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(9, 0), "#0000ff");  // k
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(10, 0), "#ea0015");  // l
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(11, 0), "#b0004f");  // m
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(12, 0), "#0c00f3");  // n
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(26, 0), "#ffffff");  // o
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(13, 0), "#4600b9");  // p
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(14, 0), "#0c00f3");  // q
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(15, 0), "#0000ff");  // r
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(16, 0), "#5e00a1");  // s
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(17, 0), "#61009e");  // t
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(27, 0), "#ffffff");  // u
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(18, 0), "#f60009");  // v
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(19, 0), "#5b00a4");  // w
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(20, 0), "#680097");  // x
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(21, 0), "#4f00b0");  // y
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(22, 0), "#0c00f3");  // z
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(23, 0), "#ffffff");  //*
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(25, 0), "#ffffff");  // gap
}

GUI_TEST_CLASS_DEFINITION(highlighting_test_0005_1) {
    //    1. Open file test/_common_data/alphabets/extended_amino.aln
    GTFileDialog::openFile(os, testDir + "_common_data/alphabets", "extended_amino.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Create 3 color schemes
    QString suffix = GTUtils::genUniqueString();
    QString scheme1 = name + "_scheme1" + suffix;
    QString scheme2 = name + "_scheme2" + suffix;
    QString scheme3 = name + "_scheme3" + suffix;
    GTUtilsMSAEditorSequenceArea::createColorScheme(os, scheme1, NewColorSchemeCreator::amino);
    GTUtilsMSAEditorSequenceArea::createColorScheme(os, scheme2, NewColorSchemeCreator::amino);
    GTUtilsMSAEditorSequenceArea::createColorScheme(os, scheme3, NewColorSchemeCreator::amino);
    //    Expected state: color schemes added to "Color" combobox
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::Highlighting);
    auto colorScheme = GTWidget::findComboBox(os, "colorScheme");
    GTComboBox::selectItemByText(os, colorScheme, scheme1);
    GTComboBox::selectItemByText(os, colorScheme, scheme2);
    GTComboBox::selectItemByText(os, colorScheme, scheme3);
}

GUI_TEST_CLASS_DEFINITION(highlighting_test_0006) {
    //    1. Open file test/_common_data/alphabets/extended_amino.aln
    GTFileDialog::openFile(os, testDir + "_common_data/alphabets", "extended_amino.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    //    2. Create custom color scheme
    QString suffix = GTUtils::genUniqueString();
    const QString scheme = name + "_scheme" + suffix;
    GTUtilsMSAEditorSequenceArea::createColorScheme(os, scheme, NewColorSchemeCreator::amino);

    //    3. Open highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::Highlighting);
    GTUtilsOptionPanelMsa::checkTabIsOpened(os, GTUtilsOptionPanelMsa::Highlighting);

    //    4. Select custom scheme
    auto colorScheme = GTWidget::findComboBox(os, "colorScheme");
    GTComboBox::selectItemByText(os, colorScheme, scheme);

    //    5. Delete scheme which is selected
    GTUtilsMSAEditorSequenceArea::deleteColorScheme(os, scheme);

    //    UGENE doesn't crash
    QString currentScheme = GTUtilsOptionPanelMsa::getColorScheme(os);
    CHECK_SET_ERR(currentScheme == "UGENE", QString("An unexpected color scheme is set: expect '%1', got '%2'").arg("UGENE").arg(currentScheme));

    GTUtilsDialog::waitForDialog(os, new PopupCheckerByText(os, {"Appearance", "Colors", "UGENE"}, PopupChecker::IsChecked));
    GTUtilsMSAEditorSequenceArea::callContextMenu(os);

    GTUtilsOptionPanelMsa::closeTab(os, GTUtilsOptionPanelMsa::Highlighting);
}

static void setHighlightingType(HI::GUITestOpStatus& os, const QString& type) {
    auto highlightingScheme = GTWidget::findComboBox(os, "highlightingScheme");
    GTComboBox::selectItemByText(os, highlightingScheme, type);
}

GUI_TEST_CLASS_DEFINITION(highlighting_test_0007) {
    //    1. Open file test/_common_data/scenarios/msa/ma2_gapped.aln
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Open highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::Highlighting);
    //    3. Select Phaneroptera_falcata as reference.
    GTUtilsOptionPanelMsa::addReference(os, "Phaneroptera_falcata");
    //    4. Check no highlighting
    setHighlightingType(os, "No highlighting");
    QString a = GTUtilsMSAEditorSequenceArea::getColor(os, QPoint(0, 0));
    QString t = GTUtilsMSAEditorSequenceArea::getColor(os, QPoint(0, 2));
    QString g = GTUtilsMSAEditorSequenceArea::getColor(os, QPoint(2, 0));
    QString c = GTUtilsMSAEditorSequenceArea::getColor(os, QPoint(4, 0));
    QString gap = GTUtilsMSAEditorSequenceArea::getColor(os, QPoint(4, 2));
    CHECK_SET_ERR(a == "#fdff6a", QString("a has color %1").arg(a));
    CHECK_SET_ERR(t == "#ff99b1", QString("t has color %1").arg(t));
    CHECK_SET_ERR(g == "#2aa1e1", QString("g has color %1").arg(g));
    CHECK_SET_ERR(c == "#49f949", QString("c has color %1").arg(c));
    CHECK_SET_ERR(gap == "#ffffff", QString("gap has color %1").arg(gap));
    /* GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(0,0), "#fcff92");//yellow
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(0,2), "#ff99b1");//red
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(2,0),"#4eade1"); //blue
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(4,0), "#70f970");//green
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(4,2), "#ffffff");//white
*/
}

GUI_TEST_CLASS_DEFINITION(highlighting_test_0007_1) {
    //    1. Open file test/_common_data/scenarios/msa/ty3.aln.gz
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "ty3.aln.gz");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Open highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::Highlighting);
    //    3. Select Phaneroptera_falcata as reference.
    GTUtilsOptionPanelMsa::addReference(os, "CfT-1_Cladosporium_fulvum");
    //    4. Check no highlighting
    setHighlightingType(os, "No highlighting");
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(6, 1), "#c0bdbb");
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(7, 1), "#ffee00");
}

GUI_TEST_CLASS_DEFINITION(highlighting_test_0008) {
    //    1. Open file test/_common_data/scenarios/msa/ma2_gapped.aln
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Open highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::Highlighting);
    //    3. Select Phaneroptera_falcata as reference.
    GTUtilsOptionPanelMsa::addReference(os, "Phaneroptera_falcata");
    //    4. Check Agreements highlighting type
    setHighlightingType(os, "Agreements");

    QString a = GTUtilsMSAEditorSequenceArea::getColor(os, QPoint(0, 0));
    QString gap1 = GTUtilsMSAEditorSequenceArea::getColor(os, QPoint(0, 2));
    QString g = GTUtilsMSAEditorSequenceArea::getColor(os, QPoint(2, 0));
    QString gap2 = GTUtilsMSAEditorSequenceArea::getColor(os, QPoint(3, 1));
    QString gap3 = GTUtilsMSAEditorSequenceArea::getColor(os, QPoint(4, 2));
    CHECK_SET_ERR(a == "#fdff6a", QString("a has color %1 intead of %2").arg(a).arg("#fcff92"));
    CHECK_SET_ERR(gap1 == "#ffffff", QString("gap1 has color %1 intead of %2").arg(gap1).arg("#ffffff"));
    CHECK_SET_ERR(g == "#2aa1e1", QString("g has color %1 intead of %2").arg(g).arg("#4eade1"));
    CHECK_SET_ERR(gap2 == "#ffffff", QString("gap2 has color%1 intead of %2").arg(gap2).arg("#ffffff"));
    CHECK_SET_ERR(gap3 == "#ffffff", QString("gap3 has color %1 intead of %2").arg(gap3).arg("#ffffff"));
    /*
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(0,0), "#fcff92");
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(0,2), "#ffffff");
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(2,0), "#4eade1");
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(3,1), "#ffffff");
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(4,2), "#ffffff");
    */
}

GUI_TEST_CLASS_DEFINITION(highlighting_test_0008_1) {
    //    1. Open file test/_common_data/scenarios/msa/ty3.aln.gz
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "ty3.aln.gz");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Open highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::Highlighting);
    //    3. Select Phaneroptera_falcata as reference.
    GTUtilsOptionPanelMsa::addReference(os, "CfT-1_Cladosporium_fulvum");
    //    4. Check Agreements highlighting type
    setHighlightingType(os, "Agreements");
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(6, 1), "#ffffff");
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(7, 1), "#ffee00");
}

GUI_TEST_CLASS_DEFINITION(highlighting_test_0009) {
    //    1. Open file test/_common_data/scenarios/msa/ma2_gapped.aln
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Open highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::Highlighting);
    //    3. Select Phaneroptera_falcata as reference.
    GTUtilsOptionPanelMsa::addReference(os, "Phaneroptera_falcata");
    //    4. Check Disagreements highlighting type
    setHighlightingType(os, "Disagreements");
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(0, 1), "#ffffff");
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(2, 1), "#ffffff");
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(5, 1), "#ffffff");
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(7, 1), "#ffffff");
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(4, 2), "#ffffff");
}

GUI_TEST_CLASS_DEFINITION(highlighting_test_0009_1) {
    //    1. Open file test/_common_data/scenarios/msa/ty3.aln.gz
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "ty3.aln.gz");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Open highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::Highlighting);

    //    3. Select Phaneroptera_falcata as reference.
    GTUtilsOptionPanelMsa::addReference(os, "CfT-1_Cladosporium_fulvum");

    //    4. Check Disagreements highlighting type
    setHighlightingType(os, "Disagreements");
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(4, 1), "#ffffff");
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(7, 1), "#ffffff");
}

GUI_TEST_CLASS_DEFINITION(highlighting_test_0010) {
    //    1. Open file test/_common_data/scenarios/msa/ma2_gapped.aln
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Open highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::Highlighting);

    //    3. Select Phaneroptera_falcata as reference.
    GTUtilsOptionPanelMsa::addReference(os, "Phaneroptera_falcata");

    //    4. Check Gaps highlighting type
    setHighlightingType(os, "Gaps");
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(4, 2), "#c0c0c0");
}

GUI_TEST_CLASS_DEFINITION(highlighting_test_0010_1) {
    //    1. Open file test/_common_data/scenarios/msa/ty3.aln.gz
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "ty3.aln.gz");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Open highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::Highlighting);
    //    3. Select Phaneroptera_falcata as reference.
    GTUtilsOptionPanelMsa::addReference(os, "CfT-1_Cladosporium_fulvum");
    //    4. Check Gaps highlighting type
    setHighlightingType(os, "Gaps");
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(0, 1), "#c0c0c0");
}

GUI_TEST_CLASS_DEFINITION(highlighting_test_0011) {
    //    1. Open file test/_common_data/scenarios/msa/ma2_gapped.aln
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Open highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::Highlighting);
    //    3. Select Phaneroptera_falcata as reference.
    GTUtilsOptionPanelMsa::addReference(os, "Phaneroptera_falcata");
    //    4. Check Transitions highlighting type
    setHighlightingType(os, "Transitions");
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(0, 1), "#ffffff");
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(2, 1), "#ffffff");
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(4, 1), "#ff99b1");
}

GUI_TEST_CLASS_DEFINITION(highlighting_test_0012) {
    //    1. Open file test/_common_data/scenarios/msa/ma2_gapped.aln
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Open highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::Highlighting);
    //    3. Select Phaneroptera_falcata as reference.
    GTUtilsOptionPanelMsa::addReference(os, "Phaneroptera_falcata");
    //    4. Check Transversions highlighting type
    setHighlightingType(os, "Transversions");
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(0, 1), "#ffffff");
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(3, 1), "#70f970");
    GTUtilsMSAEditorSequenceArea::checkColor(os, QPoint(6, 1), "#fcff92");
}

GUI_TEST_CLASS_DEFINITION(highlighting_test_0013) {
    // 1. Open file test/_common_data/scenarios/msa/ma2_gapped.aln
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Open highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::Highlighting);
    auto w = GTUtilsMSAEditorSequenceArea::getSequenceArea(os, 0);
    const QImage initImg = GTWidget::getImage(os, w);

    // 3. Check "use dots" checkbox
    setHighlightingType(os, "Agreements");
    auto useDots = GTWidget::findCheckBox(os, "useDots");
    GTCheckBox::setChecked(os, useDots, true);

    // Expected state: no effect
    QImage img = GTWidget::getImage(os, w);
    CHECK_SET_ERR(img == initImg, "sequence area unexpectedly changed");

    // 4. Select Phaneroptera_falcata as reference.
    GTUtilsOptionPanelMsa::addReference(os, "Phaneroptera_falcata");

    // Expected state: not highlighted changed to dots
    img = GTWidget::getImage(os, w);
    CHECK_SET_ERR(img != initImg, "image not changed");  // no way to check dots. Can only check that sequence area changed
}

GUI_TEST_CLASS_DEFINITION(pairwise_alignment_test_0001) {
    //    1. Open file test/_common_data/scenarios/msa/ma2_gapped.aln
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Open Pairwise alignment option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::PairwiseAlignment);
    //    3. Add Phaneroptera_falcata sequence
    GTUtilsOptionPanelMsa::addFirstSeqToPA(os, "Phaneroptera_falcata");
    //    Expected state: align button is disabled
    auto alignButton = GTWidget::findPushButton(os, "alignButton");
    CHECK_SET_ERR(!alignButton->isEnabled(), "alignButton is unexpectidly enabled");
    //    4. Add Isophya_altaica_EF540820 sequence
    GTUtilsOptionPanelMsa::addSecondSeqToPA(os, "Isophya_altaica_EF540820");
    //    Expected state: Similarity label appeared. Similarity is 43%. Align button enabled
    CHECK_SET_ERR(alignButton->isEnabled(), "align button is unexpectibly disabled");
    auto similarityValueLabel = GTWidget::findLabel(os, "similarityValueLabel");
    QString percent = similarityValueLabel->text();
    CHECK_SET_ERR(percent == "43%", QString("unexpected percent: %1").arg(percent));
    //    (branches: amino, raw alphabets)
}

GUI_TEST_CLASS_DEFINITION(pairwise_alignment_test_0002) {
    //    1. Open file test/_common_data/scenarios/msa/ma2_gapped.aln
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Open Pairwise alignment option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::PairwiseAlignment);
    //    3. Add Phaneroptera_falcata sequence two times
    GTUtilsOptionPanelMsa::addFirstSeqToPA(os, "Phaneroptera_falcata");
    GTUtilsOptionPanelMsa::addSecondSeqToPA(os, "Phaneroptera_falcata");
    //    Expected state: Similarity label appeared. Similarity is 100%. Align button disabled
    auto alignButton = GTWidget::findPushButton(os, "alignButton");
    CHECK_SET_ERR(!alignButton->isEnabled(), "align button is unexpectibly disabled");

    auto similarityValueLabel = GTWidget::findLabel(os, "similarityValueLabel");
    QString percent = similarityValueLabel->text();
    CHECK_SET_ERR(percent == "100%", QString("unexpected percent: %1").arg(percent));
    //    (branches: amino, raw alphabets)
}

GUI_TEST_CLASS_DEFINITION(pairwise_alignment_test_0003) {
    //    1. Open file test/_common_data/scenarios/msa/ma2_gapped.aln
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Open Pairwise alignment option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::PairwiseAlignment);
    //    3. Add Phaneroptera_falcata sequence with popup helper
    //    Expected state: sequence added
    GTUtilsOptionPanelMsa::addFirstSeqToPA(os, "Phaneroptera_falcata", GTUtilsOptionPanelMsa::Completer);
    QLineEdit* line1 = GTUtilsOptionPanelMsa::getSeqLineEdit(os, 1);
    CHECK_SET_ERR(line1 != nullptr, "lineEdit 1 not found");
    CHECK_SET_ERR(line1->text() == "Phaneroptera_falcata", QString("unexpected lineEdit 1 text: %1").arg(line1->text()));
    //    4. Add Isophya_altaica_EF540820 sequence with popup helper
    //    Expected state: sequence added
    GTUtilsOptionPanelMsa::addSecondSeqToPA(os, "Isophya_altaica_EF540820", GTUtilsOptionPanelMsa::Completer);
    QLineEdit* line2 = GTUtilsOptionPanelMsa::getSeqLineEdit(os, 2);
    CHECK_SET_ERR(line2 != nullptr, "lineEdit 2 not found");
    CHECK_SET_ERR(line2->text() == "Isophya_altaica_EF540820", QString("unexpected lineEdit 2 text: %1").arg(line2->text()));
    //    5. Remove Phaneroptera_falcata with button
    GTWidget::click(os, GTUtilsOptionPanelMsa::getDeleteButton(os, 1));
    CHECK_SET_ERR(line1->text().isEmpty(), QString("unexpected lineEdit 1 text: %1").arg(line1->text()));
    //    6. Remove Phaneroptera_falcata with button
    GTWidget::click(os, GTUtilsOptionPanelMsa::getDeleteButton(os, 2));
    CHECK_SET_ERR(line2->text().isEmpty(), QString("unexpected lineEdit 2 text: %1").arg(line2->text()));
}

GUI_TEST_CLASS_DEFINITION(pairwise_alignment_test_0004) {
    //    1. Open file test/_common_data/scenarios/msa/ma2_gapped.aln
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Open Pairwise alignment option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::PairwiseAlignment);
    //    3. Type wrong names in sequences' line edits.
    QLineEdit* line1 = GTUtilsOptionPanelMsa::getSeqLineEdit(os, 1);
    CHECK_SET_ERR(line1 != nullptr, "lineEdit 1 not found");
    GTLineEdit::setText(os, line1, "wrong name");
    CHECK_SET_ERR(GTBaseCompleter::isEmpty(os, line1), "Completer is not empty");

    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    QLineEdit* line2 = GTUtilsOptionPanelMsa::getSeqLineEdit(os, 2);
    CHECK_SET_ERR(line2 != nullptr, "lineEdit 2 not found");
    GTLineEdit::setText(os, line2, "wrong name");
    CHECK_SET_ERR(GTBaseCompleter::isEmpty(os, line2), "Completer is not empty");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
    GTUtilsOptionPanelMsa::toggleTab(os, GTUtilsOptionPanelMsa::PairwiseAlignment);
    //    Expected state: empty popup helper appeared
}

GUI_TEST_CLASS_DEFINITION(pairwise_alignment_test_0005) {
    //    1. Open file test/_common_data/scenarios/msa/ty3.aln.gz
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "ty3.aln.gz");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Open Pairwise alignment option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::PairwiseAlignment);
    //    3. Add two sequences to PA line edits
    GTUtilsOptionPanelMsa::addFirstSeqToPA(os, "CfT-1_Cladosporium_fulvum");
    GTUtilsOptionPanelMsa::addSecondSeqToPA(os, "AspOryTy3-2");
    //    Expected state: sequenseq added
    QLineEdit* line1 = GTUtilsOptionPanelMsa::getSeqLineEdit(os, 1);
    QLineEdit* line2 = GTUtilsOptionPanelMsa::getSeqLineEdit(os, 2);
    CHECK_SET_ERR(line1 != nullptr, "line edit1 not found");
    CHECK_SET_ERR(line2 != nullptr, "line edit2 not found");
    CHECK_SET_ERR(line1->text() == "CfT-1_Cladosporium_fulvum", QString("wrong text in line edit1: %1").arg(line1->text()));
    CHECK_SET_ERR(line2->text() == "AspOryTy3-2", QString("wrong text in line edit2: %1").arg(line2->text()));
    //    4. Remove sequenses
    GTWidget::click(os, GTUtilsOptionPanelMsa::getDeleteButton(os, 1));
    GTWidget::click(os, GTUtilsOptionPanelMsa::getDeleteButton(os, 2));
    //    Expected state: sequences removed
    CHECK_SET_ERR(line1->text().isEmpty(), QString("wrong text in line edit1: %1").arg(line1->text()));
    CHECK_SET_ERR(line2->text().isEmpty(), QString("wrong text in line edit2: %1").arg(line2->text()));
}

GUI_TEST_CLASS_DEFINITION(pairwise_alignment_test_0005_1) {
    //    1. Open file test/_common_data/alphabets/raw_alphabet.aln
    GTFileDialog::openFile(os, testDir + "_common_data/alphabets", "raw_alphabet.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Open Pairwise alignment option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::PairwiseAlignment);
    //    Expected state: wrong alphabet hint appeared
    auto lblMessage = GTWidget::findLabel(os, "lblMessage");
    CHECK_SET_ERR(lblMessage->text() == "Pairwise alignment is not available for alignments with \"Raw\" alphabet.",
                  QString("wrong label text: %1").arg(lblMessage->text()));
    //    3. Add two sequences to PA line edits
    GTUtilsOptionPanelMsa::addFirstSeqToPA(os, "seq7_1");
    GTUtilsOptionPanelMsa::addSecondSeqToPA(os, "seq7");
    //    Expected state: sequenseq added
    QLineEdit* line1 = GTUtilsOptionPanelMsa::getSeqLineEdit(os, 1);
    QLineEdit* line2 = GTUtilsOptionPanelMsa::getSeqLineEdit(os, 2);
    CHECK_SET_ERR(line1 != nullptr, "line edit1 not found");
    CHECK_SET_ERR(line2 != nullptr, "line edit2 not found");
    CHECK_SET_ERR(line1->text() == "seq7_1", QString("wrong text in line edit1: %1").arg(line1->text()));
    CHECK_SET_ERR(line2->text() == "seq7", QString("wrong text in line edit2: %1").arg(line2->text()));
    //    4. Remove sequenses
    GTWidget::click(os, GTUtilsOptionPanelMsa::getDeleteButton(os, 1));
    GTWidget::click(os, GTUtilsOptionPanelMsa::getDeleteButton(os, 2));
    GTWidget::click(os, GTUtilsOptionPanelMsa::getDeleteButton(os, 1));

    //    Expected state: sequences removed
    CHECK_SET_ERR(line1->text().isEmpty(), QString("wrong text in line edit1: %1").arg(line1->text()));
    CHECK_SET_ERR(line2->text().isEmpty(), QString("wrong text in line edit2: %1").arg(line2->text()));
}

namespace {

void expandSettings(HI::GUITestOpStatus& os, const QString& widgetName, const QString& arrowName) {
    auto widget = GTWidget::findWidget(os, widgetName);
    CHECK_SET_ERR(widget != nullptr, QString("%1 not found").arg(widgetName));
    if (widget->isHidden()) {
        GTWidget::click(os, GTWidget::findWidget(os, arrowName));
    }
}

void expandAlgoSettings(HI::GUITestOpStatus& os) {
    expandSettings(os, "settingsContainerWidget", "ArrowHeader_Algorithm settings");
}

void expandOutputSettings(HI::GUITestOpStatus& os) {
    expandSettings(os, "outputContainerWidget", "ArrowHeader_Output settings");
}

}  // namespace
GUI_TEST_CLASS_DEFINITION(pairwise_alignment_test_0006) {
    //    1. Open file test/_common_data/scenarios/msa/ma2_gapped.aln
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Open Pairwise alignment option panel tab. check spinboxes limits for KAilign
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::PairwiseAlignment);
    expandAlgoSettings(os);

    auto gapOpen = GTWidget::findDoubleSpinBox(os, "gapOpen");
    auto gapExtd = GTWidget::findDoubleSpinBox(os, "gapExtd");
    auto gapTerm = GTWidget::findDoubleSpinBox(os, "gapTerm");
    auto bonusScore = GTWidget::findDoubleSpinBox(os, "bonusScore");

    GTDoubleSpinbox::checkLimits(os, gapOpen, 0, 65535);
    GTDoubleSpinbox::checkLimits(os, gapExtd, 0, 65535);
    GTDoubleSpinbox::checkLimits(os, gapTerm, 0, 65535);
    GTDoubleSpinbox::checkLimits(os, bonusScore, 0, 65535);
    //    3. Add Phaneroptera_falcata and Isophya_altaica_EF540820 sequences to PA
    GTUtilsOptionPanelMsa::addFirstSeqToPA(os, "Phaneroptera_falcata");
    GTUtilsOptionPanelMsa::addSecondSeqToPA(os, "Isophya_altaica_EF540820");
    //    4. Align with KAlign
    GTWidget::click(os, GTWidget::findWidget(os, "alignButton"));
    QString expected = "AAGACTTCTTTTAA\n"
                       "AAGCTTACT---AA";
    GTUtilsMSAEditorSequenceArea::checkSelection(os, QPoint(0, 0), QPoint(13, 1), expected);
}

namespace {

void setSpinValue(HI::GUITestOpStatus& os, double value, const QString& spinName) {
    expandAlgoSettings(os);
    auto spinBox = GTWidget::findDoubleSpinBox(os, spinName);
    GTDoubleSpinbox::setValue(os, spinBox, value, GTGlobals::UseKeyBoard);
}

void setGapOpen(HI::GUITestOpStatus& os, double value) {
    setSpinValue(os, value, "gapOpen");
}

void setGapExtd(HI::GUITestOpStatus& os, double value) {
    setSpinValue(os, value, "gapExtd");
}

void setGapTerm(HI::GUITestOpStatus& os, double value) {
    setSpinValue(os, value, "gapTerm");
}

void setBonusScore(HI::GUITestOpStatus& os, double value) {
    setSpinValue(os, value, "bonusScore");
}

void inNewWindow(HI::GUITestOpStatus& os, bool inNew) {
    expandOutputSettings(os);
    auto inNewWindowCheckBox = GTWidget::findCheckBox(os, "inNewWindowCheckBox");
    GTCheckBox::setChecked(os, inNewWindowCheckBox, inNew);
}

void align(HI::GUITestOpStatus& os) {
    GTWidget::click(os, GTWidget::findWidget(os, "alignButton"));
}

}  // namespace
GUI_TEST_CLASS_DEFINITION(pairwise_alignment_test_0007) {
    //    1. Open file test/_common_data/scenarios/msa/ma2_gapped.aln
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Open Pairwise alignment option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::PairwiseAlignment);
    //    3. Add Phaneroptera_falcata and Isophya_altaica_EF540820 sequences to PA
    GTUtilsOptionPanelMsa::addFirstSeqToPA(os, "Phaneroptera_falcata");
    GTUtilsOptionPanelMsa::addSecondSeqToPA(os, "Isophya_altaica_EF540820");
    //    4. Set gapOpen to 1. Press align button
    setGapOpen(os, 1);
    inNewWindow(os, false);
    align(os);
    //    Expected state: Isophya_altaica_EF540820 is AAG-CTTA-CT-AA
    GTUtilsMSAEditorSequenceArea::checkSelection(os, QPoint(0, 1), QPoint(13, 1), "AAG-CTTA-CT-AA");
}

GUI_TEST_CLASS_DEFINITION(pairwise_alignment_test_0007_1) {
    //    1. Open file test/_common_data/scenarios/msa/ma2_gapped.aln
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Open Pairwise alignment option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::PairwiseAlignment);
    //    3. Add Phaneroptera_falcata and Isophya_altaica_EF540820 sequences to PA
    GTUtilsOptionPanelMsa::addFirstSeqToPA(os, "Phaneroptera_falcata");
    GTUtilsOptionPanelMsa::addSecondSeqToPA(os, "Isophya_altaica_EF540820");
    //    4. Set gap extension penalty to 1000. Press align button
    setGapExtd(os, 1000);
    inNewWindow(os, false);
    align(os);
    //    Expected state: Isophya_altaica_EF540820 is AAG-CT--TACTAA
    GTUtilsMSAEditorSequenceArea::checkSelection(os, QPoint(0, 1), QPoint(13, 1), "AAG-CT--TACTAA");
}

GUI_TEST_CLASS_DEFINITION(pairwise_alignment_test_0007_2) {
    //    1. Open file test/_common_data/scenarios/msa/ma2_gapped.aln
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Open Pairwise alignment option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::PairwiseAlignment);
    //    3. Add Phaneroptera_falcata and Isophya_altaica_EF540820 sequences to PA
    GTUtilsOptionPanelMsa::addFirstSeqToPA(os, "Phaneroptera_falcata");
    GTUtilsOptionPanelMsa::addSecondSeqToPA(os, "Isophya_altaica_EF540820");
    //    4. Set terminate gap penalty to 1000. Press align button
    setGapTerm(os, 1000);
    inNewWindow(os, false);
    align(os);
    //    Expected state: Isophya_altaica_EF540820 is AAGCTTACT---AA
    GTUtilsMSAEditorSequenceArea::checkSelection(os, QPoint(0, 1), QPoint(13, 1), "AAGCTTACT---AA");
}

GUI_TEST_CLASS_DEFINITION(pairwise_alignment_test_0007_3) {
    //    1. Open file test/_common_data/scenarios/msa/ma2_gapped.aln
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Open Pairwise alignment option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::PairwiseAlignment);
    //    3. Add Phaneroptera_falcata and Isophya_altaica_EF540820 sequences to PA
    GTUtilsOptionPanelMsa::addFirstSeqToPA(os, "Phaneroptera_falcata");
    GTUtilsOptionPanelMsa::addSecondSeqToPA(os, "Isophya_altaica_EF540820");
    //    4. Set gap open to 10, gap ext to 1, bonus score to 1. Press align button
    setGapOpen(os, 10);
    setGapExtd(os, 1);
    setBonusScore(os, 1);
    inNewWindow(os, false);
    align(os);
    //    Expected state: Isophya_altaica_EF540820 is AAG-CTTACT---AA
    GTUtilsMSAEditorSequenceArea::checkSelection(os, QPoint(0, 1), QPoint(14, 1), "AAG-CTTACT---AA");
}
namespace {
void setOutputPath(HI::GUITestOpStatus& os, const QString& path, const QString& name, bool clickYes = true) {
    expandOutputSettings(os);
    auto outputFileSelectButton = GTWidget::findWidget(os, "outputFileSelectButton");
    if (clickYes) {
        GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Yes));
    }
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, path, name, GTFileDialogUtils::Save));
    GTWidget::click(os, outputFileSelectButton);
}
}  // namespace

GUI_TEST_CLASS_DEFINITION(pairwise_alignment_test_0008) {
    const QString fileName = "pairwise_alignment_test_0008.aln";
    //    1. Open file test/_common_data/scenarios/msa/ma2_gapped.aln
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Open Pairwise alignment option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::PairwiseAlignment);
    //    3. Add Phaneroptera_falcata sequence
    GTUtilsOptionPanelMsa::addFirstSeqToPA(os, "Phaneroptera_falcata");
    GTUtilsOptionPanelMsa::addSecondSeqToPA(os, "Isophya_altaica_EF540820");
    //    4. Add Isophya_altaica_EF540820 sequence
    //    5. Select some existing file as output
    QString s = sandBoxDir + fileName;
    QFile f(s);
    bool created = f.open(QFile::ReadWrite);
    CHECK_SET_ERR(created, "file not created");
    f.close();

    setOutputPath(os, sandBoxDir, fileName);
    align(os);
    //    Expected state: file is rewritten.
    qint64 size = GTFile::getSize(os, sandBoxDir + fileName);
    CHECK_SET_ERR(size == 185, QString("unexpected file size %1").arg(size));
    GTUtilsProjectTreeView::doubleClickItem(os, fileName);
}

GUI_TEST_CLASS_DEFINITION(pairwise_alignment_test_0009) {
    GTLogTracer l;
    const QString fileName = "pairwise_alignment_test_0009.aln";
    const QString dirName = "pairwise_alignment_test_0009";
    //    1. Open file test/_common_data/scenarios/msa/ma2_gapped.aln
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Open Pairwise alignment option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::PairwiseAlignment);
    //    3. Add Phaneroptera_falcata sequence
    GTUtilsOptionPanelMsa::addFirstSeqToPA(os, "Phaneroptera_falcata");
    GTUtilsOptionPanelMsa::addSecondSeqToPA(os, "Isophya_altaica_EF540820");
    //    4. Add Isophya_altaica_EF540820 sequence
    //    5. Select some existing read-only file as output
    QString dirPath = sandBoxDir + "pairwise_alignment_test_0009";

    QDir().mkpath(dirPath);

    const QString filePath = dirPath + "/" + fileName;
    QFile f(filePath);
    bool created = f.open(QFile::ReadWrite);
    CHECK_SET_ERR(created, "file not created");
    f.close();
    GTFile::setReadOnly(os, filePath);

    setOutputPath(os, sandBoxDir + dirName, fileName);
    align(os);
    //    Expected state: error in log: Task {Pairwise alignment task} finished with error: No permission to write to 'pairwise_alignment_test_0009.aln' file.
    QString error = l.getJoinedErrorString();
    const QString expectedFilePath = QFileInfo(filePath).absoluteFilePath();
    const QString expected = QString("Task {Pairwise alignment task} finished with error: No permission to write to \'%1\' file.").arg(expectedFilePath);
    CHECK_SET_ERR(error.contains(expected), QString("enexpected error: %1").arg(error));

    GTFile::setReadWrite(os, filePath);
}

GUI_TEST_CLASS_DEFINITION(pairwise_alignment_test_0010) {
    GTLogTracer l;
    const QString fileName = "pairwise_alignment_test_0010.aln";
    const QString dirName = "pairwise_alignment_test_0010";
    //    1. Open file test/_common_data/scenarios/msa/ma2_gapped.aln
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Open Pairwise alignment option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::PairwiseAlignment);
    //    3. Add Phaneroptera_falcata sequence
    GTUtilsOptionPanelMsa::addFirstSeqToPA(os, "Phaneroptera_falcata");
    GTUtilsOptionPanelMsa::addSecondSeqToPA(os, "Isophya_altaica_EF540820");
    //    4. Add Isophya_altaica_EF540820 sequence
    //    5. Select some existing read-only file as output
    QString dirPath = sandBoxDir + dirName;
    bool ok = QDir().mkpath(dirPath);
    CHECK_SET_ERR(ok, "subfolder not created");

    GTFile::setReadOnly(os, dirPath);

    const QString filePath = dirPath + "/" + fileName;

    setOutputPath(os, dirPath, fileName, false);
    align(os);
    //    Expected state: error in log: Task {Pairwise alignment task} finished with error: No permission to write to 'COI_transl.aln' file.
    QString error = l.getJoinedErrorString();
    const QString expectedFilePath = QFileInfo(filePath).absoluteFilePath();
    const QString expected = QString("Task {Pairwise alignment task} finished with error: No permission to write to \'%1\' file.").arg(expectedFilePath);
    CHECK_SET_ERR(error == expected, QString("enexpected error: %1").arg(error));

    GTFile::setReadWrite(os, dirPath);
}

GUI_TEST_CLASS_DEFINITION(tree_settings_test_0001) {
    //    1. Open data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Open tree settings option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::TreeSettings);
    //    3. Press "Open tree" button. Select data/samples/CLUSTALW/COI.nwk in file dialog
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, dataDir + "samples/Newick", "COI.nwk"));
    GTWidget::click(os, GTWidget::findWidget(os, "openTreeButton"));
    //    Expected state: tree opened.
    GTWidget::findWidget(os, "treeView");
}

GUI_TEST_CLASS_DEFINITION(tree_settings_test_0002) {
    //    1. Open data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Open tree settings option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::TreeSettings);
    //    3. Press "build tree" button.
    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, "default"));
    GTUtilsMsaEditor::clickBuildTreeButton(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    4. Fill build tree dialog with defaulb values
    //    Expected state: tree built.
    GTWidget::findWidget(os, "treeView");
}

GUI_TEST_CLASS_DEFINITION(tree_settings_test_0003) {
    //    1. Open data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Open tree settings option panel tab. build tree
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::TreeSettings);
    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, "default", 0, 0, true));
    GTUtilsMsaEditor::clickBuildTreeButton(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Check/prepare tree widgets.
    auto treeView = GTWidget::findWidget(os, "treeView");
    auto breadthScaleAdjustmentSlider = GTWidget::findWidget(os, "breadthScaleAdjustmentSlider");
    auto layoutCombo = GTWidget::findComboBox(os, "layoutCombo");

    QImage rectImage = GTWidget::getImage(os, treeView);

    //    3. Select circular layout
    GTComboBox::selectItemByText(os, layoutCombo, "Circular");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: layout changed, height slider is disabled
    QImage circularImage = GTWidget::getImage(os, treeView);
    CHECK_SET_ERR(rectImage != circularImage, "tree view not changed to circular");
    CHECK_SET_ERR(!breadthScaleAdjustmentSlider->isEnabled(), "breadthScaleAdjustmentSlider in enabled for circular layout");

    //    4. Select unrooted layout
    GTComboBox::selectItemByText(os, layoutCombo, "Unrooted");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: layout changed, height slider is disabled
    QImage unrootedImage = GTWidget::getImage(os, treeView);
    CHECK_SET_ERR(rectImage != unrootedImage, "tree view not changed to unrooted");
    CHECK_SET_ERR(!breadthScaleAdjustmentSlider->isEnabled(), "breadthScaleAdjustmentSlider in enabled for unrooted layout");

    //    5. Select rectangular layout
    GTComboBox::selectItemByText(os, layoutCombo, "Rectangular");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: tree is similar to the beginning, height slider is enabled
    QImage rectImage2 = GTWidget::getImage(os, treeView);
    CHECK_SET_ERR(rectImage == rectImage2, "final image is not equal to initial");
    CHECK_SET_ERR(breadthScaleAdjustmentSlider->isEnabled(), "breadthScaleAdjustmentSlider in disabled for rectangular layout");
}

GUI_TEST_CLASS_DEFINITION(tree_settings_test_0004) {
    // Compare that 'Default'/'Cladogram'/'Phylogram' are stable within the type and are different between different types.
    // To check that images are stable capture 2 versions of each image.

    // Open data/samples/CLUSTALW/COI.aln.
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsProjectTreeView::toggleView(os);  // Close project view to make all actions on toolbar available.

    // Open tree settings option panel tab. build tree.
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::TreeSettings);
    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, "default", 0, 0, true));
    GTUtilsMsaEditor::clickBuildTreeButton(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    auto treeView = GTWidget::findWidget(os, "treeView");
    auto treeViewCombo = GTWidget::findComboBox(os, "treeViewCombo");

    QAbstractButton* syncModeButton = GTAction::button(os, "sync_msa_action");
    GTWidget::click(os, syncModeButton);
    CHECK_SET_ERR(!syncModeButton->isChecked(), "Sync mode must be OFF");

    // Capture 2 variants of  'Phylogram', 'Default', 'Cladogram' images.
    GTComboBox::selectItemByText(os, treeViewCombo, "Phylogram");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    QImage phylogramImage1 = GTWidget::getImage(os, treeView);

    GTComboBox::selectItemByText(os, treeViewCombo, "Default");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    QImage defaultImage1 = GTWidget::getImage(os, treeView);

    GTComboBox::selectItemByText(os, treeViewCombo, "Cladogram");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    QImage cladogramImage1 = GTWidget::getImage(os, treeView);

    GTComboBox::selectItemByText(os, treeViewCombo, "Phylogram");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    QImage phylogramImage2 = GTWidget::getImage(os, treeView);

    GTComboBox::selectItemByText(os, treeViewCombo, "Cladogram");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    QImage cladogramImage2 = GTWidget::getImage(os, treeView);

    GTComboBox::selectItemByText(os, treeViewCombo, "Default");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    QImage defaultImage2 = GTWidget::getImage(os, treeView);

    CHECK_SET_ERR(defaultImage1 == defaultImage2, "Default images are not equal");
    CHECK_SET_ERR(cladogramImage1 == cladogramImage2, "Cladogram images are not equal");
    CHECK_SET_ERR(phylogramImage1 == phylogramImage2, "Phylogram images are not equal");

    CHECK_SET_ERR(defaultImage1 != cladogramImage1, "Default image must not be equal to Cladogram");
    CHECK_SET_ERR(defaultImage1 != phylogramImage1, "Default image must not be equal to Phylogram");
    CHECK_SET_ERR(cladogramImage1 != phylogramImage1, "Cladogram image image must not be equal to Phylogram");
}

GUI_TEST_CLASS_DEFINITION(tree_settings_test_0005) {
    //    1. Open data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Open tree settings option panel tab. build tree
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::TreeSettings);
    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, "default", 0, 0, true));
    GTUtilsMsaEditor::clickBuildTreeButton(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    auto showNamesCheck = GTWidget::findCheckBox(os, "showNamesCheck");
    auto showDistancesCheck = GTWidget::findCheckBox(os, "showDistancesCheck");
    auto alignLabelsCheck = GTWidget::findCheckBox(os, "alignLabelsCheck");
    auto subWindow = GTWidget::findWidget(os, "COI [COI.aln]_SubWindow");
    auto treeView = GTWidget::findGraphicsView(os, "treeView", subWindow);

    QList<QGraphicsSimpleTextItem*> initNames = GTUtilsPhyTree::getVisibleLabels(os, treeView);
    QList<QGraphicsSimpleTextItem*> initDistances = GTUtilsPhyTree::getVisibleDistances(os, treeView);

    //    3. Uncheck "show names" checkbox.
    GTCheckBox::setChecked(os, showNamesCheck, false);

    //    Expected state: names are not shown, align labels checkbox is disabled
    QList<QGraphicsSimpleTextItem*> names = GTUtilsPhyTree::getVisibleLabels(os, treeView);
    CHECK_SET_ERR(names.isEmpty(), QString("unexpected number of names: %1").arg(names.count()));
    CHECK_SET_ERR(!alignLabelsCheck->isEnabled(), "align labels checkbox is unexpectedly enabled");

    //    4. Check "show names" checkbox.
    GTCheckBox::setChecked(os, showNamesCheck, true);

    //    Expected state: names are shown, align labels checkbox is enabled
    names = GTUtilsPhyTree::getVisibleLabels(os, treeView);
    CHECK_SET_ERR(names.count() == initNames.count(), QString("unexpected number of names: %1").arg(names.count()));
    CHECK_SET_ERR(alignLabelsCheck->isEnabled(), "align labels checkbox is unexpectedly disabled");

    //    5. Uncheck "show distances" checkbox.
    GTCheckBox::setChecked(os, showDistancesCheck, false);

    //    Expected state: distances are not shown
    QList<QGraphicsSimpleTextItem*> distances = GTUtilsPhyTree::getVisibleDistances(os, treeView);
    CHECK_SET_ERR(distances.isEmpty(), QString("unexpected number of distances: %1").arg(names.count()));

    //    6. Check "show distances" checkbox.
    GTCheckBox::setChecked(os, showDistancesCheck, true);

    //    Expected state: distances are shown
    distances = GTUtilsPhyTree::getVisibleDistances(os, treeView);
    CHECK_SET_ERR(distances.count() == initDistances.count(), QString("unexpected number of distances: %1").arg(names.count()));

    //    7. Check "align labels" checkbox.
    // Saving init image
    GTCheckBox::setChecked(os, alignLabelsCheck, false);
    QImage initImg = GTWidget::getImage(os, treeView);  // initial state

    GTCheckBox::setChecked(os, alignLabelsCheck, true);

    //    Expected state: labels are aligned
    QImage alignedImg = GTWidget::getImage(os, treeView);
    CHECK_SET_ERR(alignedImg != initImg, "labels are not aligned");

    //    8. Uncheck "align labels" checkbox.
    GTCheckBox::setChecked(os, alignLabelsCheck, false);

    //    Expected state: labels are not aligned
    QImage finalImg = GTWidget::getImage(os, treeView);
    CHECK_SET_ERR(finalImg == initImg, "tree is aligned");
}

static void setLabelsColor(HI::GUITestOpStatus& os, int r, int g, int b) {
    GTUtilsDialog::waitForDialog(os, new ColorDialogFiller(os, r, g, b));
    auto labelsColorButton = GTWidget::findWidget(os, "labelsColorButton");
    GTWidget::click(os, labelsColorButton);
}

static void checkLabelColor(HI::GUITestOpStatus& os, const QString& expectedColorName) {
    auto graphicsView = GTWidget::findGraphicsView(os, "treeView");
    QList<QGraphicsSimpleTextItem*> labels = GTUtilsPhyTree::getVisibleLabels(os, graphicsView);
    CHECK_SET_ERR(!labels.isEmpty(), "there are no visiable labels");

    QColor expectedColor(expectedColorName);
    QColor closestColor;  // Used in error message in case if the test fails.
    QImage img = GTWidget::getImage(os, AppContext::getMainWindow()->getQMainWindow());
    for (int time = 0; time < 5000; time += GT_OP_CHECK_MILLIS) {
        GTGlobals::sleep(time > 0 ? GT_OP_CHECK_MILLIS : 0);
        for (QGraphicsSimpleTextItem* label : qAsConst(labels)) {
            QRectF rect = label->boundingRect();
            graphicsView->ensureVisible(label);
            for (int i = 0; i < rect.right(); i++) {
                for (int j = 0; j < rect.bottom(); j++) {
                    QPoint p(i, j);
                    QPoint global = graphicsView->viewport()->mapToGlobal(graphicsView->mapFromScene(label->mapToScene(p)));

                    QRgb rgb = img.pixel(global);
                    QColor pointColor = QColor(rgb);
                    QString name = pointColor.name();
                    if (name == expectedColorName) {
                        return;
                    }
                    auto getColorDistance = [](const QColor& c1, const QColor& c2) {
                        return qAbs(c1.green() - c2.green()) + qAbs(c1.blue() - c2.blue()) + qAbs(c1.red() - c2.red());
                    };
                    if (getColorDistance(pointColor, expectedColor) < getColorDistance(closestColor, expectedColor)) {
                        closestColor = pointColor;
                    }
                }
            }
        }
    }
    GT_FAIL("Color is not found. Closest: " + closestColor.name(), );
}

GUI_TEST_CLASS_DEFINITION(tree_settings_test_0006) {
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Open tree settings option panel tab. build tree.
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::TreeSettings);
    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, "default", 0, 0, true));
    GTUtilsMsaEditor::clickBuildTreeButton(os);

    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Change labels font.
    auto fontComboBox = GTWidget::findComboBox(os, "fontComboBox");
    auto fontNameEdit = fontComboBox->findChild<QLineEdit*>();
    CHECK_SET_ERR(fontNameEdit != nullptr, "font name edit is not found");
    GTLineEdit::setText(os, fontNameEdit, "Serif");
    GTKeyboardDriver::keyClick(Qt::Key_Enter);

    // Expected: font changed.
    QGraphicsSimpleTextItem* label = GTUtilsPhyTree::getVisibleLabels(os).at(0);
    QString family = label->font().family();
    CHECK_SET_ERR(family == "Serif", "unexpected style: " + family);

    // Change labels size.
    auto fontSizeSpinBox = GTWidget::findSpinBox(os, "fontSizeSpinBox");
    auto fontSizeEdit = fontSizeSpinBox->findChild<QLineEdit*>();
    CHECK_SET_ERR(fontSizeEdit != nullptr, "font size edit is not found");
    GTLineEdit::setText(os, fontSizeEdit, "20");
    GTKeyboardDriver::keyClick(Qt::Key_Enter);

    // Expected: size changed.
    int pointSize = label->font().pointSize();
    CHECK_SET_ERR(pointSize == 20, QString("unexpected point size: %1").arg(pointSize));

    // Check font settings buttons
    auto boldAttrButton = GTWidget::findWidget(os, "boldAttrButton");
    auto italicAttrButton = GTWidget::findWidget(os, "italicAttrButton");
    auto underlineAttrButton = GTWidget::findWidget(os, "underlineAttrButton");

    // Bold.
    GTWidget::click(os, boldAttrButton);
    CHECK_SET_ERR(label->font().bold(), "expected bold font");

    // Not bold.
    GTWidget::click(os, boldAttrButton);
    CHECK_SET_ERR(!label->font().bold(), "bold font not canceled");

    // Italic.
    GTWidget::click(os, italicAttrButton);
    CHECK_SET_ERR(label->font().italic(), "expected italic font");

    // Not italic.
    GTWidget::click(os, italicAttrButton);
    CHECK_SET_ERR(!label->font().italic(), "italic font not canceled");

    // Underline.
    GTWidget::click(os, underlineAttrButton);
    CHECK_SET_ERR(label->font().underline(), "expected underline font");

    // Not underline.
    GTWidget::click(os, underlineAttrButton);
    CHECK_SET_ERR(!label->font().underline(), "underline font not canceled");

    // Change labels color. Run this check last after label size was increased to avoid color-aliasing effects.
    if (!isOsMac()) {
        setLabelsColor(os, 255, 0, 0);
        // Expected: color changed
        checkLabelColor(os, "#ff0000");
    }
}

GUI_TEST_CLASS_DEFINITION(tree_settings_test_0007) {
    // Open data/samples/CLUSTALW/COI.aln.
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    // Open tree settings option panel tab. Build a tree.
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::TreeSettings);

    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, "default", 0, 0, true));
    GTUtilsMsaEditor::clickBuildTreeButton(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Disable sync mode to allow resize of the view.

    GTUtilsProjectTreeView::toggleView(os);  // Close opened project tree view to make all icons on the toolbar visible with no overflow.
    QAbstractButton* syncModeButton = GTAction::button(os, "sync_msa_action");
    CHECK_SET_ERR(syncModeButton->isChecked(), "Sync mode must be ON");

    GTWidget::click(os, syncModeButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    CHECK_SET_ERR(!syncModeButton->isChecked(), "Sync mode must be OFF");

    auto treeView = GTWidget::findGraphicsView(os, "treeView");
    QGraphicsScene* scene = treeView->scene();

    // Decrease breadthScaleAdjustmentSlider value.
    double initialHeight = scene->height();
    auto breadthScaleAdjustmentSlider = GTWidget::findSlider(os, "breadthScaleAdjustmentSlider");
    GTSlider::setValue(os, breadthScaleAdjustmentSlider, 50);

    // Expected state: the tree breadth (height) was reduced.
    double reducedHeight = scene->height();
    CHECK_SET_ERR(reducedHeight < initialHeight, QString("Scene height is not reduced! Initial: %1, final: %2").arg(initialHeight).arg(reducedHeight));

    // Increase breadthScaleAdjustmentSlider value.
    GTSlider::setValue(os, breadthScaleAdjustmentSlider, 200);

    // Expected state: the tree breadth (height) was increased.
    double increasedHeight = scene->height();
    CHECK_SET_ERR(increasedHeight > initialHeight, QString("Height is not increased! Initial: %1, final: %2").arg(initialHeight).arg(increasedHeight));
}

void setBranchColor(HI::GUITestOpStatus& os, int r, int g, int b) {
    GTUtilsDialog::waitForDialog(os, new ColorDialogFiller(os, r, g, b));
    auto branchesColorButton = GTWidget::findWidget(os, "branchesColorButton");
    GTWidget::click(os, branchesColorButton);
}

static double colorPercent(HI::GUITestOpStatus& os, QWidget* widget, const QString& colorName) {
    int total = 0;
    int found = 0;
    const QImage img = GTWidget::getImage(os, widget);
    QRect r = widget->rect();
    int wid = r.width();
    int heig = r.height();
    for (int i = 0; i < wid; i++) {
        for (int j = 0; j < heig; j++) {
            total++;
            QPoint p(i, j);
            QRgb rgb = img.pixel(p);
            QColor color = QColor(rgb);
            QString name = color.name();
            if (name == colorName) {
                found++;
            }
        }
    }
    double result = static_cast<double>(found) / total;
    return result;
}

GUI_TEST_CLASS_DEFINITION(tree_settings_test_0008) {
    //    1. Open data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Open tree settings option panel tab. build tree
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::TreeSettings);
    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, "default", 0, 0, true));
    GTUtilsMsaEditor::clickBuildTreeButton(os);
    GTThread::waitForMainThread();

    // Click to empty space near the node to reset selection
    auto treeView = GTWidget::findGraphicsView(os, "treeView");
    TvNodeItem* node = GTUtilsPhyTree::getNodeByBranchText(os, "0.006", "0.104");
    QPointF sceneCoord = node->mapToScene(node->boundingRect().topLeft());
    QPoint viewCord = treeView->mapFromScene(sceneCoord);
    QPoint globalCoord = treeView->mapToGlobal(viewCord);
    globalCoord += QPoint(node->boundingRect().width() / 2 + 8, node->boundingRect().height() / 2 + 8);
    GTMouseDriver::moveTo(globalCoord);
    GTMouseDriver::click();
    setBranchColor(os, 255, 0, 0);

    // Expected state: color changed
    CHECK_SET_ERR(treeView != nullptr, "tree view not found");
    QString colorName = "#ff0000";
    double initPercent = colorPercent(os, treeView, colorName);
    CHECK_SET_ERR(initPercent != 0, "color not changed");

    // Change  line Weight
    auto lineWeightSpinBox = GTWidget::findSpinBox(os, "lineWeightSpinBox");
    GTSpinBox::setValue(os, lineWeightSpinBox, 30, GTGlobals::UseKeyBoard);
    double finalPercent = colorPercent(os, treeView, colorName);
    CHECK_SET_ERR(finalPercent > initPercent * 10, "branches width changed not enough");
}

namespace {
void setConsensusOutputPath(HI::GUITestOpStatus& os, const QString& path) {
    auto pathLe = GTWidget::findLineEdit(os, "pathLe");
    GTLineEdit::setText(os, pathLe, path);
}
}  // namespace

GUI_TEST_CLASS_DEFINITION(export_consensus_test_0001) {
    const QString fileName = "export_consensus_test_0001.txt";
    //    1. Open data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Open export consensus option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::ExportConsensus);
    //    3. Select some existing file as output
    QString s = sandBoxDir + fileName;
    QFile f(s);
    bool created = f.open(QFile::ReadWrite);
    CHECK_SET_ERR(created, "file not created");
    f.close();

    setConsensusOutputPath(os, sandBoxDir + fileName);
    //    4. Press export button
    GTWidget::click(os, GTWidget::findWidget(os, "exportBtn"));
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    Expected state: file rewrited
    qint64 size = GTFile::getSize(os, sandBoxDir + fileName);
    CHECK_SET_ERR(size == 604, QString("unexpected file size %1").arg(size));
    GTUtilsProjectTreeView::doubleClickItem(os, fileName);
}

GUI_TEST_CLASS_DEFINITION(export_consensus_test_0002) {
    GTLogTracer l;
    const QString fileName = "export_consensus_test_0002.aln";
    //    1. Open data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Open export consensus option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::ExportConsensus);
    //    3. Select existing read-only file "export_consensus_test_0002.aln" as output

    const QString dirPath = sandBoxDir + "export_consensus_test_0002";
    QDir().mkpath(dirPath);

    const QString filePath = dirPath + "/" + fileName;
    QFile f(filePath);
    bool created = f.open(QFile::ReadWrite);
    CHECK_SET_ERR(created, "file not created");
    f.close();

    GTFile::setReadOnly(os, filePath);

    setConsensusOutputPath(os, filePath);
    //    4. Press export button
    GTWidget::click(os, GTWidget::findWidget(os, "exportBtn"));
    //    Expected state: error in log: Task {Save document} finished with error: No permission to write to 'COI_transl.aln' file.
    QString error = l.getJoinedErrorString();
    const QString expectedFilePath = QFileInfo(filePath).absoluteFilePath();
    QString expected = QString("Task {Export consensus} finished with error: Subtask {Save document} is failed: No permission to write to \'%1\' file.").arg(expectedFilePath);
    CHECK_SET_ERR(error.contains(expected), QString("Unexpected error: %1").arg(error));

    GTFile::setReadWrite(os, filePath);
}

GUI_TEST_CLASS_DEFINITION(export_consensus_test_0003) {
    GTLogTracer l;
    const QString fileName = "export_consensus_test_0003.aln";
    const QString dirName = "export_consensus_test_0003";
    //    1. Open data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    //    2. Open export consensus option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::ExportConsensus);

    //    3. Select some existing file in read-only directory as output
    QString dirPath = sandBoxDir + dirName;
    bool ok = QDir().mkpath(dirPath);
    CHECK_SET_ERR(ok, "subfolder not created");
    GTFile::setReadOnly(os, dirPath);

    QString filePath = dirPath + '/' + fileName;
    setConsensusOutputPath(os, filePath);

    //    4. Press export button
    GTWidget::click(os, GTWidget::findWidget(os, "exportBtn"));
    GTThread::waitForMainThread();

    //    Expected state: notification is shown that folder is read-only.
    QString error = l.getJoinedErrorString();
    QString expected = QString("Task {Export consensus} finished with error: Folder is read-only: %1").arg(QFileInfo(filePath).absolutePath());
    CHECK_SET_ERR(error == expected, QString("Unexpected error: '%1', expected: '%2'").arg(error).arg(expected));
}

GUI_TEST_CLASS_DEFINITION(export_consensus_test_0004) {
    // 0. Change Documents folder to sandbox
    class Custom : public CustomScenario {
        void run(HI::GUITestOpStatus& os) override {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);
            AppSettingsDialogFiller::setDocumentsDirPath(os, sandBoxDir);
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new AppSettingsDialogFiller(os, new Custom()));
    GTMenu::clickMainMenuItem(os, {"Settings", "Preferences..."}, GTGlobals::UseMouse);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    1. Open data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Open export consensus option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::ExportConsensus);
    //    3. Set empty path
    setConsensusOutputPath(os, "");

    class exportConsensusTest0004Filler : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) override {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
        }
    };
    GTWidget::click(os, GTWidget::findWidget(os, "exportBtn"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    auto pathLe = GTWidget::findLineEdit(os, "pathLe");
    QString pathLeText = pathLe->text();
    CHECK_SET_ERR(!pathLeText.isEmpty() && pathLeText.contains("COI_consensus_1.txt"), "wrong lineEdit text: " + pathLeText);
}

GUI_TEST_CLASS_DEFINITION(statistics_test_0001) {
    //    1. Open data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Open export statistics option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::Statistics);
    //    3. check showDistancesColumn checkbox
    auto showDistancesColumnCheck = GTWidget::findCheckBox(os, "showDistancesColumnCheck");
    GTCheckBox::setChecked(os, showDistancesColumnCheck, true);
    //    4. Check reference hint
    auto refSeqWarning = GTWidget::findLabel(os, "refSeqWarning");
    CHECK_SET_ERR(refSeqWarning->text() == "Hint: select a reference above", QString("Unexpected hint: %1").arg(refSeqWarning->text()));
    //    5. Add Phaneroptera_falcata as reference
    GTUtilsOptionPanelMsa::addReference(os, "Phaneroptera_falcata");
    //    Expected state: similarity column appaered
    QString s0 = GTUtilsMSAEditorSequenceArea::getSimilarityValue(os, 0);
    CHECK_SET_ERR(s0 == "0%", QString("Unexpected similarity at line 1: %1").arg(s0));
    QString s1 = GTUtilsMSAEditorSequenceArea::getSimilarityValue(os, 1);
    CHECK_SET_ERR(s1 == "18%", QString("Unexpected similarity at line 2: %1").arg(s1));

    //    6. Check counts mode
    auto countsButton = GTWidget::findRadioButton(os, "countsButton");
    GTRadioButton::click(os, countsButton);

    s0 = GTUtilsMSAEditorSequenceArea::getSimilarityValue(os, 0);
    CHECK_SET_ERR(s0 == "0", QString("Unexpected similarity at line 1: %1").arg(s0));
    s1 = GTUtilsMSAEditorSequenceArea::getSimilarityValue(os, 1);
    CHECK_SET_ERR(s1 == "106", QString("Unexpected similarity at line 2: %1").arg(s1));
    //    7. Check exclude gabs mode
    auto excludeGapsCheckBox = GTWidget::findCheckBox(os, "excludeGapsCheckBox");
    GTCheckBox::setChecked(os, excludeGapsCheckBox, true);

    s0 = GTUtilsMSAEditorSequenceArea::getSimilarityValue(os, 7);
    CHECK_SET_ERR(s0 == "110", QString("Unexpected similarity at line 8: %1").arg(s0));
    s1 = GTUtilsMSAEditorSequenceArea::getSimilarityValue(os, 8);
    CHECK_SET_ERR(s1 == "100", QString("Unexpected similarity at line 9: %1").arg(s1));
}

GUI_TEST_CLASS_DEFINITION(statistics_test_0002) {
    //    1. Open data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Open export statistics option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::Statistics);
    //    3. check showDistancesColumn checkbox
    auto showDistancesColumnCheck = GTWidget::findCheckBox(os, "showDistancesColumnCheck");
    GTCheckBox::setChecked(os, showDistancesColumnCheck, true);
    //    4. Add Phaneroptera_falcata as reference
    GTUtilsOptionPanelMsa::addReference(os, "Phaneroptera_falcata");
    //    5. Check identity algorithm
    auto algoComboBox = GTWidget::findComboBox(os, "algoComboBox");
    GTComboBox::selectItemByText(os, algoComboBox, "Similarity");
    /*
    QString s0 = GTUtilsMSAEditorSequenceArea::getSimilarityValue(os, 0);
    CHECK_SET_ERR(s0 == "100%", QString("Unexpected similarity at line 1: %1").arg(s0));
    QString s1 = GTUtilsMSAEditorSequenceArea::getSimilarityValue(os, 1);
    CHECK_SET_ERR(s1 == "82%", QString("Unexpected similarity at line 2: %1").arg(s1));
//    6. Check counts mode
    QRadioButton* countsButton = GTWidget::findRadioButton(os, "countsButton");
    GTRadioButton::click(os, countsButton);

    s0 = GTUtilsMSAEditorSequenceArea::getSimilarityValue(os, 0);
    CHECK_SET_ERR(s0 == "604", QString("Unexpected similarity at line 1: %1").arg(s0));
    s1 = GTUtilsMSAEditorSequenceArea::getSimilarityValue(os, 1);
    CHECK_SET_ERR(s1 == "498", QString("Unexpected similarity at line 2: %1").arg(s1));
    */
}
GUI_TEST_CLASS_DEFINITION(statistics_test_0003) {
    // 1. Open data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    // 2. Open export statistics option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::Statistics);
    // 3. check showDistancesColumn checkbox
    auto showDistancesColumnCheck = GTWidget::findCheckBox(os, "showDistancesColumnCheck");
    GTCheckBox::setChecked(os, showDistancesColumnCheck, true);
    // 4. Add Phaneroptera_falcata as reference
    GTUtilsOptionPanelMsa::addReference(os, "Phaneroptera_falcata");
    // 5. delete symbol at point (0,0)
    GTWidget::click(os, GTUtilsMdi::activeWindow(os));
    GTUtilsMSAEditorSequenceArea::click(os, QPoint(0, 0));
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    // Expected state: similarity changed, updateButton is disabled.
    QString s0 = GTUtilsMSAEditorSequenceArea::getSimilarityValue(os, 0);
    CHECK_SET_ERR(s0 == "0%", QString("(1)Unexpected similarity at line 1: %1").arg(s0));
    QString s1 = GTUtilsMSAEditorSequenceArea::getSimilarityValue(os, 1);
    CHECK_SET_ERR(s1 == "69%", QString("(1)Unexpected similarity at line 2: %1").arg(s1));

    auto updateButton = GTWidget::findWidget(os, "updateButton");
    CHECK_SET_ERR(!updateButton->isEnabled(), "updateButton is unexpectedly enabled");
    // 6. Undo. Uncheck automatic update checkbox
    GTWidget::click(os, GTAction::button(os, "msa_action_undo"));
    auto autoUpdateCheck = GTWidget::findCheckBox(os, "autoUpdateCheck");
    GTCheckBox::setChecked(os, autoUpdateCheck, false);
    // 5. Delete symbol at point (0,0)
    GTWidget::click(os, GTUtilsMdi::activeWindow(os));
    GTUtilsMSAEditorSequenceArea::click(os, QPoint(0, 0));
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    // Expected state: similarity not changed.
    s0 = GTUtilsMSAEditorSequenceArea::getSimilarityValue(os, 0);
    CHECK_SET_ERR(s0 == "0%", QString("(2)Unexpected similarity at line 1: %1").arg(s0));
    s1 = GTUtilsMSAEditorSequenceArea::getSimilarityValue(os, 1);
    CHECK_SET_ERR(s1 == "18%", QString("(2)Unexpected similarity at line 2: %1").arg(s1));
    // 6. Press autoUpdate button.
    GTWidget::click(os, updateButton);
    // Expected state: similarity is updated.
    s0 = GTUtilsMSAEditorSequenceArea::getSimilarityValue(os, 0);
    CHECK_SET_ERR(s0 == "0%", QString("(3)Unexpected similarity at line 1: %1").arg(s0));
    s1 = GTUtilsMSAEditorSequenceArea::getSimilarityValue(os, 1);
    CHECK_SET_ERR(s1 == "69%", QString("(3)Unexpected similarity at line 2: %1").arg(s1));
}

GUI_TEST_CLASS_DEFINITION(save_parameters_test_0001) {
    //    1. Open data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Open export general option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::General);
    //   Check saving parameters
    // find widgets
    auto consensusType = GTWidget::findComboBox(os, "consensusType");
    auto thresholdSpinBox = GTWidget::findSpinBox(os, "thresholdSpinBox");

    // set some parameters
    GTUtilsOptionPanelMsa::addReference(os, "Phaneroptera_falcata");
    GTComboBox::selectItemByText(os, consensusType, "Strict");
    GTSpinBox::setValue(os, thresholdSpinBox, 50, GTGlobals::UseKeyBoard);

    // close and open option panel
    GTWidget::click(os, GTWidget::findWidget(os, "OP_MSA_GENERAL"));
    GTWidget::click(os, GTWidget::findWidget(os, "OP_MSA_GENERAL"));

    // checks
    auto sequenceLineEdit = GTWidget::findLineEdit(os, "sequenceLineEdit");
    consensusType = GTWidget::findComboBox(os, "consensusType");
    thresholdSpinBox = GTWidget::findSpinBox(os, "thresholdSpinBox");

    CHECK_SET_ERR(sequenceLineEdit->text() == "Phaneroptera_falcata", QString("unexpected reference: %1").arg(sequenceLineEdit->text()));
    CHECK_SET_ERR(consensusType->currentText() == "Strict", QString("unexpected consensus: %1").arg(consensusType->currentText()));
    CHECK_SET_ERR(thresholdSpinBox->value() == 50, QString("unexpected threshold value: %1").arg(thresholdSpinBox->value()));
}

GUI_TEST_CLASS_DEFINITION(save_parameters_test_0002) {
    //    1. Open data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Open export highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::Highlighting);

    //   Check saving parameters
    // find widgets
    auto colorScheme = GTWidget::findComboBox(os, "colorScheme");
    auto highlightingScheme = GTWidget::findComboBox(os, "highlightingScheme");
    auto useDots = GTWidget::findCheckBox(os, "useDots");

    // set some parameters
    GTUtilsOptionPanelMsa::addReference(os, "Phaneroptera_falcata");
    GTComboBox::selectItemByText(os, colorScheme, "Jalview");
    GTComboBox::selectItemByText(os, highlightingScheme, "Agreements");
    GTCheckBox::setChecked(os, useDots, true);

    // close and open option panel
    GTWidget::click(os, GTWidget::findWidget(os, "OP_MSA_HIGHLIGHTING"));
    GTWidget::click(os, GTWidget::findWidget(os, "OP_MSA_HIGHLIGHTING"));

    // checks
    auto sequenceLineEdit = GTWidget::findLineEdit(os, "sequenceLineEdit");
    colorScheme = GTWidget::findComboBox(os, "colorScheme");
    highlightingScheme = GTWidget::findComboBox(os, "highlightingScheme");
    useDots = GTWidget::findCheckBox(os, "useDots");

    CHECK_SET_ERR(sequenceLineEdit->text() == "Phaneroptera_falcata", QString("unexpected reference: %1").arg(sequenceLineEdit->text()));
    CHECK_SET_ERR(colorScheme->currentText() == "Jalview", QString("uexpected color scheme: %1").arg(colorScheme->currentText()));
    CHECK_SET_ERR(highlightingScheme->currentText() == "Agreements", QString("uexpected highlighting: %1").arg(highlightingScheme->currentText()));
    CHECK_SET_ERR(useDots->isChecked(), "Use dots checkbox is not checked");
}

GUI_TEST_CLASS_DEFINITION(save_parameters_test_0003) {
    // KAlign settings
    //     1. Open data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Open export highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::PairwiseAlignment);

    //   Check saving parameters
    GTUtilsOptionPanelMsa::addFirstSeqToPA(os, "Phaneroptera_falcata");
    GTUtilsOptionPanelMsa::addSecondSeqToPA(os, "Isophya_altaica_EF540820");

    // expand settings
    GTWidget::click(os, GTWidget::findWidget(os, "ArrowHeader_Algorithm settings"));
    GTWidget::click(os, GTWidget::findWidget(os, "ArrowHeader_Output settings"));

    // find widgets
    auto gapOpen = GTWidget::findDoubleSpinBox(os, "gapOpen");
    auto gapExtd = GTWidget::findDoubleSpinBox(os, "gapExtd");
    auto gapTerm = GTWidget::findDoubleSpinBox(os, "gapTerm");
    auto bonusScore = GTWidget::findDoubleSpinBox(os, "bonusScore");
    auto inNewWindowCheckBox = GTWidget::findCheckBox(os, "inNewWindowCheckBox");

    // set values
    GTDoubleSpinbox::setValue(os, gapOpen, 100, GTGlobals::UseKeyBoard);
    GTDoubleSpinbox::setValue(os, gapExtd, 100, GTGlobals::UseKeyBoard);
    GTDoubleSpinbox::setValue(os, gapTerm, 100, GTGlobals::UseKeyBoard);
    GTDoubleSpinbox::setValue(os, bonusScore, 100, GTGlobals::UseKeyBoard);
    GTCheckBox::setChecked(os, inNewWindowCheckBox, false);

    // close and open option panel
    GTWidget::click(os, GTWidget::findWidget(os, "OP_PAIRALIGN"));
    GTWidget::click(os, GTWidget::findWidget(os, "OP_PAIRALIGN"));

    // checks
    QLineEdit* l1 = GTUtilsOptionPanelMsa::getSeqLineEdit(os, 1);
    QLineEdit* l2 = GTUtilsOptionPanelMsa::getSeqLineEdit(os, 2);
    gapOpen = GTWidget::findDoubleSpinBox(os, "gapOpen");
    gapExtd = GTWidget::findDoubleSpinBox(os, "gapExtd");
    gapTerm = GTWidget::findDoubleSpinBox(os, "gapTerm");
    bonusScore = GTWidget::findDoubleSpinBox(os, "bonusScore");
    inNewWindowCheckBox = GTWidget::findCheckBox(os, "inNewWindowCheckBox");

    CHECK_SET_ERR(l1->text() == "Phaneroptera_falcata", QString("unexpected seq1: %1").arg(l1->text()));
    CHECK_SET_ERR(l2->text() == "Isophya_altaica_EF540820", QString("unexpected seq2: %1").arg(l2->text()));
    CHECK_SET_ERR(gapOpen->value() == 100, QString("unexpected gapOpen value: %1").arg(gapOpen->value()));
    CHECK_SET_ERR(gapExtd->value() == 100, QString("unexpected gapExtd value: %1").arg(gapExtd->value()));
    CHECK_SET_ERR(gapTerm->value() == 100, QString("unexpected gapTerm value: %1").arg(gapTerm->value()));
    CHECK_SET_ERR(bonusScore->value() == 100, QString("unexpected bonusScore value: %1").arg(bonusScore->value()));
    CHECK_SET_ERR(!inNewWindowCheckBox->isChecked(), "inNewWindowCheckBox is unexpectidly checked");
}

GUI_TEST_CLASS_DEFINITION(save_parameters_test_0003_1) {  //
    // smith-waterman settings
    //     1. Open data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Open export highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::PairwiseAlignment);

    //   Check saving parameters
    GTUtilsOptionPanelMsa::addFirstSeqToPA(os, "Phaneroptera_falcata");
    GTUtilsOptionPanelMsa::addSecondSeqToPA(os, "Isophya_altaica_EF540820");

    // expand settings
    GTWidget::click(os, GTWidget::findWidget(os, "ArrowHeader_Algorithm settings"));

    // find widgets
    auto algorithmListComboBox = GTWidget::findComboBox(os, "algorithmListComboBox");
    GTComboBox::selectItemByText(os, algorithmListComboBox, "Smith-Waterman");

    auto algorithmVersion = GTWidget::findComboBox(os, "algorithmVersion");
    auto scoringMatrix = GTWidget::findComboBox(os, "scoringMatrix");
    auto gapOpen = GTWidget::findSpinBox(os, "gapOpen");
    auto gapExtd = GTWidget::findSpinBox(os, "gapExtd");

    // setValues
    GTComboBox::selectItemByText(os, algorithmVersion, "SW_classic");
    GTComboBox::selectItemByText(os, scoringMatrix, "dna");
    GTSpinBox::setValue(os, gapOpen, 5);
    GTSpinBox::setValue(os, gapExtd, 5);

    // close and open option panel
    GTWidget::click(os, GTWidget::findWidget(os, "OP_PAIRALIGN"));
    GTWidget::click(os, GTWidget::findWidget(os, "OP_PAIRALIGN"));

    // checks
    QLineEdit* l1 = GTUtilsOptionPanelMsa::getSeqLineEdit(os, 1);
    QLineEdit* l2 = GTUtilsOptionPanelMsa::getSeqLineEdit(os, 2);
    algorithmListComboBox = GTWidget::findComboBox(os, "algorithmListComboBox");
    algorithmVersion = GTWidget::findComboBox(os, "algorithmVersion");
    scoringMatrix = GTWidget::findComboBox(os, "scoringMatrix");
    gapOpen = GTWidget::findSpinBox(os, "gapOpen");
    gapExtd = GTWidget::findSpinBox(os, "gapExtd");

    CHECK_SET_ERR(l1->text() == "Phaneroptera_falcata", QString("unexpected seq1: %1").arg(l1->text()));
    CHECK_SET_ERR(l2->text() == "Isophya_altaica_EF540820", QString("unexpected seq2: %1").arg(l2->text()));
    CHECK_SET_ERR(algorithmListComboBox->currentText() == "Smith-Waterman", QString("unexpected current text").arg(algorithmListComboBox->currentText()));
    CHECK_SET_ERR(algorithmVersion->currentText() == "SW_classic", QString("unexpected algorithm: %1").arg(algorithmVersion->currentText()));
    CHECK_SET_ERR(scoringMatrix->currentText().contains("dna"), QString("unexpected scoring matrix").arg(scoringMatrix->currentText()));
    CHECK_SET_ERR(gapOpen->value() == 5, QString("unexpected gap open value: %1").arg(gapOpen->value()));
    CHECK_SET_ERR(gapExtd->value() == 5, QString("unexpected gap ext value: %1").arg(gapExtd->value()));
}

GUI_TEST_CLASS_DEFINITION(save_parameters_test_0004) {
    //    1. Open data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Open tree settings option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::TreeSettings);
    //    3. Press "build tree" button.
    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, "default", 0, 0, true));
    GTUtilsMsaEditor::clickBuildTreeButton(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // set some values
    auto layoutCombo = GTWidget::findComboBox(os, "layoutCombo");
    GTComboBox::selectItemByText(os, layoutCombo, "Circular");

    auto treeViewCombo = GTWidget::findComboBox(os, "treeViewCombo");
    GTComboBox::selectItemByText(os, treeViewCombo, "Cladogram");
    setLabelsColor(os, 255, 255, 255);
    QString initialColor = GTWidget::getColor(os, GTWidget::findWidget(os, "labelsColorButton"), QPoint(10, 10)).name();
    auto fontComboBox = GTWidget::findComboBox(os, "fontComboBox");
    auto l = fontComboBox->findChild<QLineEdit*>();
    QString fontName = isOsLinux() ? "Serif" : "Tahoma";
    GTLineEdit::setText(os, l, fontName);
    GTKeyboardDriver::keyClick(Qt::Key_Enter);

    auto fontSizeSpinBox = GTWidget::findSpinBox(os, "fontSizeSpinBox");
    GTSpinBox::setValue(os, fontSizeSpinBox, 14);

    GTWidget::click(os, GTWidget::findWidget(os, "boldAttrButton"));
    GTWidget::click(os, GTWidget::findWidget(os, "italicAttrButton"));
    GTWidget::click(os, GTWidget::findWidget(os, "underlineAttrButton"));

    // close and open option panel
    GTWidget::click(os, GTWidget::findWidget(os, "OP_MSA_TREES_WIDGET"));
    GTWidget::click(os, GTWidget::findWidget(os, "OP_MSA_TREES_WIDGET"));

    // check settings
    layoutCombo = GTWidget::findComboBox(os, "layoutCombo");
    treeViewCombo = GTWidget::findComboBox(os, "treeViewCombo");
    auto labelsColorButton = GTWidget::findWidget(os, "labelsColorButton");
    fontComboBox = GTWidget::findComboBox(os, "fontComboBox");
    fontSizeSpinBox = GTWidget::findSpinBox(os, "fontSizeSpinBox");
    auto boldAttrButton = GTWidget::findToolButton(os, "boldAttrButton");
    auto italicAttrButton = GTWidget::findToolButton(os, "italicAttrButton");
    auto underlineAttrButton = GTWidget::findToolButton(os, "underlineAttrButton");

    CHECK_SET_ERR(layoutCombo->currentText() == "Circular", QString("unexpected layout: %1").arg(layoutCombo->currentText()));
    CHECK_SET_ERR(treeViewCombo->currentText() == "Cladogram", QString("unexpected tree view: %1").arg(treeViewCombo->currentText()));
    CHECK_SET_ERR(GTWidget::hasPixelWithColor(os, labelsColorButton, initialColor), QString("Initial color is not found: %1").arg(initialColor));
    CHECK_SET_ERR(fontComboBox->currentText().contains(fontName), QString("unexpected font: %1").arg(fontComboBox->currentText()));
    CHECK_SET_ERR(fontSizeSpinBox->value() == 14, QString("unexpected font size: %1").arg(fontSizeSpinBox->value()));
    CHECK_SET_ERR(boldAttrButton->isChecked(), "boldAttrButton is not checked");
    CHECK_SET_ERR(italicAttrButton->isChecked(), "italicAttrButton is not checked");
    CHECK_SET_ERR(underlineAttrButton->isChecked(), "underlineAttrButton is not checked");
}

GUI_TEST_CLASS_DEFINITION(save_parameters_test_0004_1) {
    //    1. Open data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Open tree settings option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::TreeSettings);
    //    3. Press "build tree" button.
    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, "default", 0, 0, true));
    GTUtilsMsaEditor::clickBuildTreeButton(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // find widgets
    auto showNamesCheck = GTWidget::findCheckBox(os, "showNamesCheck");
    auto showDistancesCheck = GTWidget::findCheckBox(os, "showDistancesCheck");
    auto breadthScaleAdjustmentSlider = GTWidget::findSlider(os, "breadthScaleAdjustmentSlider");

    auto lineWeightSpinBox = GTWidget::findSpinBox(os, "lineWeightSpinBox");

    // set some values
    GTCheckBox::setChecked(os, showNamesCheck, false);
    GTCheckBox::setChecked(os, showDistancesCheck, false);
    GTSlider::setValue(os, breadthScaleAdjustmentSlider, 50);
    setBranchColor(os, 255, 255, 255);
    QString initialColor = GTWidget::getColor(os, GTWidget::findWidget(os, "branchesColorButton"), QPoint(10, 10)).name();
    GTSpinBox::setValue(os, lineWeightSpinBox, 2);

    // close and open option panel
    GTWidget::click(os, GTWidget::findWidget(os, "OP_MSA_TREES_WIDGET"));
    GTWidget::click(os, GTWidget::findWidget(os, "OP_MSA_TREES_WIDGET"));

    // checks
    showNamesCheck = GTWidget::findCheckBox(os, "showNamesCheck");
    showDistancesCheck = GTWidget::findCheckBox(os, "showDistancesCheck");
    breadthScaleAdjustmentSlider = GTWidget::findSlider(os, "breadthScaleAdjustmentSlider");
    lineWeightSpinBox = GTWidget::findSpinBox(os, "lineWeightSpinBox");
    auto branchesColorButton = GTWidget::findWidget(os, "branchesColorButton");

    CHECK_SET_ERR(!showNamesCheck->isChecked(), "show names checkbox is unexpectedly checked");
    CHECK_SET_ERR(!showDistancesCheck->isChecked(), "show distances checkbox is unexpectedly checked");
    CHECK_SET_ERR(breadthScaleAdjustmentSlider->value() == 50, QString("unexpected breadthScaleAdjustmentSlider value: %1").arg(breadthScaleAdjustmentSlider->value()));
    CHECK_SET_ERR(lineWeightSpinBox->value() == 2, QString("unexpected line width: %1").arg(lineWeightSpinBox->value()));
    CHECK_SET_ERR(GTWidget::hasPixelWithColor(os, branchesColorButton, initialColor), QString("Initial color is not found: %1").arg(initialColor));
}

GUI_TEST_CLASS_DEFINITION(save_parameters_test_0005) {
    //    1. Open data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2.  Set strict consensus
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::General);
    auto consensusType = GTWidget::findComboBox(os, "consensusType");
    GTComboBox::selectItemByText(os, consensusType, "Strict");
    //    3. Open export consensus tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::ExportConsensus);

    // find widgets
    auto pathLe = GTWidget::findLineEdit(os, "pathLe");
    auto formatCb = GTWidget::findComboBox(os, "formatCb");
    auto keepGapsChb = GTWidget::findCheckBox(os, "keepGapsChb");

    // set some values
    GTLineEdit::setText(os, pathLe, "some_path");
    GTComboBox::selectItemByText(os, formatCb, "GenBank");
    GTCheckBox::setChecked(os, keepGapsChb, true);

    // close and open option panel
    GTWidget::click(os, GTWidget::findWidget(os, "OP_EXPORT_CONSENSUS"));
    GTWidget::click(os, GTWidget::findWidget(os, "OP_EXPORT_CONSENSUS"));

    // checks
    pathLe = GTWidget::findLineEdit(os, "pathLe");
    formatCb = GTWidget::findComboBox(os, "formatCb");
    keepGapsChb = GTWidget::findCheckBox(os, "keepGapsChb");

    CHECK_SET_ERR(pathLe->text().contains("some_path"), QString("unexpected path: %1").arg(pathLe->text()));
    CHECK_SET_ERR(formatCb->currentText() == "GenBank", QString("unexpected format: %1").arg(formatCb->currentText()));
    CHECK_SET_ERR(keepGapsChb->isChecked(), "keep gaps checkBox is unexpectedly unchecked");
}

GUI_TEST_CLASS_DEFINITION(save_parameters_test_0006) {
    //    1. Open data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2.  Set strict consensus
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::Statistics);
    GTUtilsOptionPanelMsa::addReference(os, "Phaneroptera_falcata");

    // find widgets
    auto showDistancesColumnCheck = GTWidget::findCheckBox(os, "showDistancesColumnCheck");
    auto algoComboBox = GTWidget::findComboBox(os, "algoComboBox");
    auto countsButton = GTWidget::findRadioButton(os, "countsButton");
    auto excludeGapsCheckBox = GTWidget::findCheckBox(os, "excludeGapsCheckBox");
    auto autoUpdateCheck = GTWidget::findCheckBox(os, "autoUpdateCheck");

    // set some parameters
    GTCheckBox::setChecked(os, showDistancesColumnCheck, true);
    GTComboBox::selectItemByText(os, algoComboBox, "Similarity");
    GTRadioButton::click(os, countsButton);
    GTCheckBox::setChecked(os, excludeGapsCheckBox, true);
    GTCheckBox::setChecked(os, autoUpdateCheck, false);

    // close and open option panel
    GTWidget::click(os, GTWidget::findWidget(os, "OP_SEQ_STATISTICS_WIDGET"));
    GTWidget::click(os, GTWidget::findWidget(os, "OP_SEQ_STATISTICS_WIDGET"));

    // checks
    showDistancesColumnCheck = GTWidget::findCheckBox(os, "showDistancesColumnCheck");
    algoComboBox = GTWidget::findComboBox(os, "algoComboBox");
    countsButton = GTWidget::findRadioButton(os, "countsButton");
    excludeGapsCheckBox = GTWidget::findCheckBox(os, "excludeGapsCheckBox");
    autoUpdateCheck = GTWidget::findCheckBox(os, "autoUpdateCheck");

    CHECK_SET_ERR(showDistancesColumnCheck->isChecked(), "show distances is unexpectedly unchecked");
    CHECK_SET_ERR(algoComboBox->currentText() == "Similarity", QString("unexpected algorithm: %1").arg(algoComboBox->currentText()));
    CHECK_SET_ERR(countsButton->isChecked(), "counts radio is not checked");
    CHECK_SET_ERR(excludeGapsCheckBox->isChecked(), "exclude gaps not checked");
    CHECK_SET_ERR(!autoUpdateCheck->isChecked(), "auto update is unexpectedly checked");
}
}  // namespace GUITest_common_scenarios_options_panel_MSA
}  // namespace U2
