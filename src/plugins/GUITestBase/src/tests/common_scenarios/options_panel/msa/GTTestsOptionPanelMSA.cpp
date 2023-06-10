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
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Open general option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::General);
    //    3. Use button to add Phaneroptera_falcata as referene
    GTUtilsOptionPanelMsa::addReference(seqName);
    //    Expected state:
    //    reference sequence line edit is empty
    auto sequenceLineEdit = GTWidget::findLineEdit("sequenceLineEdit");
    QString text = sequenceLineEdit->text();
    CHECK_SET_ERR(text == seqName, QString("sequenceLineEdit contains %1, expected: %2").arg(text).arg(seqName));
    CHECK_SET_ERR(GTUtilsMSAEditorSequenceArea::isSequenceHighlighted(seqName), "sequence not highlighted");
    //    Expected state: Phaneroptera_falcata highlighted as reference

    //    4. Use button to remove reference
    GTUtilsOptionPanelMsa::removeReference();
    //    Expected state:
    //    reference sequence line edit contains "select and add"
    //    Phaneroptera_falcata is not highlighted as reference
    text = sequenceLineEdit->text();
    CHECK_SET_ERR(text.isEmpty(), QString("sequenceLineEdit contains %1, no text expected").arg(text));
    CHECK_SET_ERR(!GTUtilsMSAEditorSequenceArea::isSequenceHighlighted(seqName), "sequence not highlighted");
}

GUI_TEST_CLASS_DEFINITION(general_test_0001_1) {
    // Difference: popup completer is used
    const QString seqName = "Phaneroptera_falcata";
    //    1. Open file data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Open general option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::General);
    //    3. Use button to add Phaneroptera_falcata as reference
    GTUtilsOptionPanelMsa::addReference(seqName, GTUtilsOptionPanelMsa::Completer);
    //    Expected state:
    //    reference sequence line edit contains Phaneroptera_falcata
    auto sequenceLineEdit = GTWidget::findLineEdit("sequenceLineEdit");
    QString text = sequenceLineEdit->text();
    CHECK_SET_ERR(text == seqName, QString("sequenceLineEdit contains %1, expected: %2").arg(text).arg(seqName));
    CHECK_SET_ERR(GTUtilsMSAEditorSequenceArea::isSequenceHighlighted(seqName), "sequence not highlighted");
    //    Phaneroptera_falcata highlighted as reference

    //    4. Use button to remove reference
    GTUtilsOptionPanelMsa::removeReference();
    //    Expected state:
    //    reference sequence line edit contains "select and add"
    //    Phaneroptera_falcata is not highlighted as reference
    text = sequenceLineEdit->text();
    CHECK_SET_ERR(text.isEmpty(), QString("sequenceLineEdit contains %1, no text expected").arg(text));
    CHECK_SET_ERR(!GTUtilsMSAEditorSequenceArea::isSequenceHighlighted(seqName), "sequence not highlighted");
}

GUI_TEST_CLASS_DEFINITION(general_test_0002) {
    const QString seqName = "Phaneroptera_falcata";
    // 1. Open file data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    // 2. Open general option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::General);
    // 3. Type "phan" in reference line edit
    auto sequenceLineEdit = GTWidget::findLineEdit("sequenceLineEdit");
    GTLineEdit::setText(sequenceLineEdit, "phan");
    QStringList names = GTBaseCompleter::getNames(sequenceLineEdit);
    // Expected state: popup helper contains Phaneroptera_falcata.(case insensitivity is checked)
    int num = names.count();
    CHECK_SET_ERR(num == 1, QString("wrong number of sequences in completer. Expected 1, found %1").arg(num));

    QString name = names[0];
    CHECK_SET_ERR(name == seqName, QString("wrong sequence name. Expected %1, found %2").arg(seqName).arg(name));
    GTWidget::click(GTUtilsMdi::activeWindow());
}

GUI_TEST_CLASS_DEFINITION(general_test_0003) {
    //    1. Open file data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Open general option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::General);
    //    3. Type some string which is not part some sequence's name
    auto sequenceLineEdit = GTWidget::findLineEdit("sequenceLineEdit");
    GTLineEdit::setText(sequenceLineEdit, "wrong name");
    //    Expected state: empty popup helper appeared
    bool empty = GTBaseCompleter::isEmpty(sequenceLineEdit);
    CHECK_SET_ERR(empty, "completer is not empty");
    GTWidget::click(sequenceLineEdit);  // needed to close completer
}

GUI_TEST_CLASS_DEFINITION(general_test_0004) {
    //    1. Open file data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Open general option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::General);
    //    3. Rename Isophya_altaica_EF540820 to Phaneroptera_falcata
    GTUtilsMSAEditorSequenceArea::renameSequence("Isophya_altaica_EF540820", "Phaneroptera_falcata");
    //    4. Type "phan" in reference line edit
    auto sequenceLineEdit = GTWidget::findLineEdit("sequenceLineEdit");
    GTLineEdit::setText(sequenceLineEdit, "Phan");
    QStringList completerList = GTBaseCompleter::getNames(sequenceLineEdit);
    //    Expected state: two sequence names "Phaneroptera_falcata" appeared in popup helper
    CHECK_SET_ERR(completerList.count() == 2, "wrong number of sequences in completer");
    QString first = completerList[0];
    QString second = completerList[1];
    CHECK_SET_ERR(first == "Phaneroptera_falcata", QString("first sequence in completer is wrong: %1").arg(first))
    CHECK_SET_ERR(second == "Phaneroptera_falcata", QString("second sequence in completer is wrong: %1").arg(second))
    GTWidget::click(sequenceLineEdit);  // needed to close completer
}

GUI_TEST_CLASS_DEFINITION(general_test_0005) {
    //    1. Open file data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Open general option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::General);

    //    3. Delete Hetrodes_pupus_EF540832
    GTUtilsMSAEditorSequenceArea::selectSequence("Hetrodes_pupus_EF540832");
    GTKeyboardDriver::keyClick(Qt::Key_Delete);

    //    Expected state: Sequence number is 17
    const int height = GTUtilsOptionPanelMsa::getHeight();
    CHECK_SET_ERR(height == 17, QString("wrong height. expected 17, found %1").arg(height));

    //    4. Select one column. Press delete
    GTUtilsMsaEditor::clickColumn(5);
    GTKeyboardDriver::keyClick(Qt::Key_Delete);

    //    Expected state: Length is 603
    const int length = GTUtilsOptionPanelMsa::getLength();
    CHECK_SET_ERR(length == 603, QString("wrong length. expected 603, found %1").arg(length));
}

GUI_TEST_CLASS_DEFINITION(highlighting_test_0001) {
    //    1. Open file test/_common_data/scenarios/msa/ma2_gapped.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Open highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Highlighting);
    //    3. Select "UGENE" color scheme
    auto colorScheme = GTWidget::findComboBox("colorScheme");
    GTComboBox::selectItemByText(colorScheme, "UGENE");
    QString a = GTUtilsMSAEditorSequenceArea::getColor(QPoint(0, 0));
    QString t = GTUtilsMSAEditorSequenceArea::getColor(QPoint(0, 2));
    QString g = GTUtilsMSAEditorSequenceArea::getColor(QPoint(2, 0));
    QString c = GTUtilsMSAEditorSequenceArea::getColor(QPoint(4, 0));
    QString gap = GTUtilsMSAEditorSequenceArea::getColor(QPoint(4, 2));
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
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Open highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Highlighting);
    //    3. Select "No colors" color scheme
    auto colorScheme = GTWidget::findComboBox("colorScheme");
    GTComboBox::selectItemByText(colorScheme, "No colors");
    QString a = GTUtilsMSAEditorSequenceArea::getColor(QPoint(0, 0));
    QString t = GTUtilsMSAEditorSequenceArea::getColor(QPoint(0, 2));
    QString g = GTUtilsMSAEditorSequenceArea::getColor(QPoint(2, 0));
    QString c = GTUtilsMSAEditorSequenceArea::getColor(QPoint(4, 0));
    QString gap = GTUtilsMSAEditorSequenceArea::getColor(QPoint(4, 2));
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
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Open highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Highlighting);
    //    3. Select "Jalview" color scheme
    auto colorScheme = GTWidget::findComboBox("colorScheme");
    GTComboBox::selectItemByText(colorScheme, "Jalview");
    QString a = GTUtilsMSAEditorSequenceArea::getColor(QPoint(0, 0));
    QString t = GTUtilsMSAEditorSequenceArea::getColor(QPoint(0, 2));
    QString g = GTUtilsMSAEditorSequenceArea::getColor(QPoint(2, 0));
    QString c = GTUtilsMSAEditorSequenceArea::getColor(QPoint(4, 0));
    QString gap = GTUtilsMSAEditorSequenceArea::getColor(QPoint(4, 2));
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
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Open highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Highlighting);
    //    3. Select "Percentage identity" color scheme
    auto colorScheme = GTWidget::findComboBox("colorScheme");
    GTComboBox::selectItemByText(colorScheme, "Percentage identity");
    QString a = GTUtilsMSAEditorSequenceArea::getColor(QPoint(0, 0));
    QString t = GTUtilsMSAEditorSequenceArea::getColor(QPoint(0, 2));
    QString g = GTUtilsMSAEditorSequenceArea::getColor(QPoint(2, 0));
    QString c = GTUtilsMSAEditorSequenceArea::getColor(QPoint(5, 0));
    QString gap = GTUtilsMSAEditorSequenceArea::getColor(QPoint(4, 2));
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
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Open highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Highlighting);
    //    3. Select "Percentage identity (gray)" color scheme
    auto colorScheme = GTWidget::findComboBox("colorScheme");
    GTComboBox::selectItemByText(colorScheme, "Percentage identity (gray)");
    QString a = GTUtilsMSAEditorSequenceArea::getColor(QPoint(0, 0));
    QString t = GTUtilsMSAEditorSequenceArea::getColor(QPoint(0, 2));
    QString g = GTUtilsMSAEditorSequenceArea::getColor(QPoint(2, 0));
    QString c = GTUtilsMSAEditorSequenceArea::getColor(QPoint(5, 0));
    QString gap = GTUtilsMSAEditorSequenceArea::getColor(QPoint(4, 2));
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
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    //    2. Create custom color scheme
    QString suffix = GTUtils::genUniqueString();
    QString schemeName = name + "Scheme" + suffix;
    GTUtilsMSAEditorSequenceArea::createColorScheme(schemeName, NewColorSchemeCreator::nucl);

    //    3. Open highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Highlighting);
    GTUtilsOptionPanelMsa::isTabOpened(GTUtilsOptionPanelMsa::Highlighting);

    //    Expected state: color scheme added to "Color" combobox
    auto colorScheme = GTWidget::findComboBox("colorScheme");
    GTComboBox::selectItemByText(colorScheme, "No colors");
    GTComboBox::selectItemByText(colorScheme, schemeName);

    //    4. Select custom scheme
    //    Expected state: scheme changed
    QString a = GTUtilsMSAEditorSequenceArea::getColor(QPoint(0, 0));
    QString t = GTUtilsMSAEditorSequenceArea::getColor(QPoint(0, 2));
    QString g = GTUtilsMSAEditorSequenceArea::getColor(QPoint(2, 0));
    QString c = GTUtilsMSAEditorSequenceArea::getColor(QPoint(4, 0));
    QString gap = GTUtilsMSAEditorSequenceArea::getColor(QPoint(4, 2));
    CHECK_SET_ERR(a == "#fcff92", QString("a has color %1").arg(a));
    CHECK_SET_ERR(t == "#ff99b1", QString("t has color %1").arg(t));
    CHECK_SET_ERR(g == "#4eade1", QString("g has color %1").arg(g));
    CHECK_SET_ERR(c == "#70f970", QString("c has color %1").arg(c));
    CHECK_SET_ERR(gap == "#ffffff", QString("gap has color %1").arg(gap));
}

GUI_TEST_CLASS_DEFINITION(highlighting_test_0002_1) {
    //    1. Open file test/_common_data/scenarios/msa/ma2_gapped.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Create 3 color schemes
    QString suffix = GTUtils::genUniqueString();
    QString scheme1 = name + "_scheme1" + suffix;
    QString scheme2 = name + "_scheme2" + suffix;
    QString scheme3 = name + "_scheme3" + suffix;
    GTUtilsMSAEditorSequenceArea::createColorScheme(scheme1, NewColorSchemeCreator::nucl);
    GTUtilsMSAEditorSequenceArea::createColorScheme(scheme2, NewColorSchemeCreator::nucl);
    GTUtilsMSAEditorSequenceArea::createColorScheme(scheme3, NewColorSchemeCreator::nucl);
    //    Expected state: color schemes added to "Color" combobox
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Highlighting);
    auto colorScheme = GTWidget::findComboBox("colorScheme");
    GTComboBox::selectItemByText(colorScheme, scheme1);
    GTComboBox::selectItemByText(colorScheme, scheme2);
    GTComboBox::selectItemByText(colorScheme, scheme3);
}

GUI_TEST_CLASS_DEFINITION(highlighting_test_0003) {
    //    1. Open file test/_common_data/scenarios/msa/ma2_gapped.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Create custom color scheme
    QString suffix = GTUtils::genUniqueString();
    const QString scheme = name + "_scheme111111111111111111111111111111111111111111111" + suffix;
    GTUtilsMSAEditorSequenceArea::createColorScheme(scheme, NewColorSchemeCreator::nucl);
    //    3. Open highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Highlighting);
    //    4. Select custom scheme
    auto colorScheme = GTWidget::findComboBox("colorScheme");
    GTComboBox::selectItemByText(colorScheme, scheme);
    //    5. Delete scheme which is selected
    GTUtilsMSAEditorSequenceArea::deleteColorScheme(scheme);

    //    UGENE not crashes
    //    default color sheme is selected
    const QString currentScheme = colorScheme->currentText();
    CHECK_SET_ERR(currentScheme == "UGENE", QString("wrong color scheme selected: expected '%1', got '%2'").arg("UGENE").arg(currentScheme));

    const QString a = GTUtilsMSAEditorSequenceArea::getColor(QPoint(0, 0));
    const QString t = GTUtilsMSAEditorSequenceArea::getColor(QPoint(0, 2));
    const QString g = GTUtilsMSAEditorSequenceArea::getColor(QPoint(2, 0));
    const QString c = GTUtilsMSAEditorSequenceArea::getColor(QPoint(4, 0));
    const QString gap = GTUtilsMSAEditorSequenceArea::getColor(QPoint(4, 2));
    CHECK_SET_ERR(a == "#fcff92", QString("a has color %1").arg(a));
    CHECK_SET_ERR(t == "#ff99b1", QString("t has color %1").arg(t));
    CHECK_SET_ERR(g == "#4eade1", QString("g has color %1").arg(g));
    CHECK_SET_ERR(c == "#70f970", QString("c has color %1").arg(c));
    CHECK_SET_ERR(gap == "#ffffff", QString("gap has color %1").arg(gap));
}

GUI_TEST_CLASS_DEFINITION(highlighting_test_0004) {
    //    1. Open file test/_common_data/alphabets/extended_amino.aln
    GTFileDialog::openFile(testDir + "_common_data/alphabets", "extended_amino.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Open highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Highlighting);
    //    3. Select "No color" color scheme
    auto colorScheme = GTWidget::findComboBox("colorScheme");
    GTComboBox::selectItemByText(colorScheme, "No colors");
    //    4. Check colors for all symbols
    for (int i = 0; i < 29; i++) {
        GTUtilsMSAEditorSequenceArea::checkColor(QPoint(i, 0), "#ffffff");
    }
}

GUI_TEST_CLASS_DEFINITION(highlighting_test_0004_1) {
    //    1. Open file test/_common_data/alphabets/extended_amino.aln
    GTFileDialog::openFile(testDir + "_common_data/alphabets", "extended_amino.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Open highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Highlighting);
    //    3. Select "Buried index" color scheme
    auto colorScheme = GTWidget::findComboBox("colorScheme");
    GTComboBox::selectItemByText(colorScheme, "Buried index");
    //    4. Check colors for all symbols
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(0, 0), "#00a35c");  // a
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(1, 0), "#00eb14");  // b
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(2, 0), "#0000ff");  // c
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(3, 0), "#00eb14");  // d
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(4, 0), "#00f10e");  // e
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(5, 0), "#008778");  // f
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(6, 0), "#009d62");  // g
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(7, 0), "#00d52a");  // h
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(8, 0), "#0054ab");  // i
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(28, 0), "#ffffff");  // j
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(9, 0), "#00ff00");  // k
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(10, 0), "#007b84");  // l
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(11, 0), "#009768");  // m
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(12, 0), "#00eb14");  // n
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(26, 0), "#ffffff");  // o
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(13, 0), "#00e01f");  // p
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(14, 0), "#00f10e");  // q
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(15, 0), "#00fc03");  // r
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(16, 0), "#00d52a");  // s
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(17, 0), "#00db24");  // t
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(27, 0), "#ffffff");  // u
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(18, 0), "#005fa0");  // v
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(19, 0), "#00a857");  // w
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(20, 0), "#00b649");  // x
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(21, 0), "#00e619");  // y
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(22, 0), "#00f10e");  // z
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(23, 0), "#ffffff");  //*
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(25, 0), "#ffffff");  // gap
}

GUI_TEST_CLASS_DEFINITION(highlighting_test_0004_2) {
    //    1. Open file test/_common_data/alphabets/extended_amino.aln
    GTFileDialog::openFile(testDir + "_common_data/alphabets", "extended_amino.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Open highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Highlighting);
    //    3. Select "Clustal X" color scheme
    auto colorScheme = GTWidget::findComboBox("colorScheme");
    GTComboBox::selectItemByText(colorScheme, "Clustal X");
    //    4. Check colors for all symbols
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(0, 0), "#80a0f0");  // a
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(1, 0), "#ffffff");  // b
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(2, 0), "#f08080");  // c
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(3, 0), "#c048c0");  // d
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(4, 0), "#c048c0");  // e
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(5, 0), "#80a0f0");  // f
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(6, 0), "#f09048");  // g
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(7, 0), "#15a4a4");  // h
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(8, 0), "#80a0f0");  // i
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(28, 0), "#ffffff");  // j
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(9, 0), "#f01505");  // k
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(10, 0), "#80a0f0");  // l
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(11, 0), "#80a0f0");  // m
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(12, 0), "#15c015");  // n
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(26, 0), "#ffffff");  // o
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(13, 0), "#c0c000");  // p
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(14, 0), "#15c015");  // q
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(15, 0), "#f01505");  // r
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(16, 0), "#15c015");  // s
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(17, 0), "#15c015");  // t
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(27, 0), "#ffffff");  // u
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(18, 0), "#80a0f0");  // v
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(19, 0), "#80a0f0");  // w
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(20, 0), "#ffffff");  // x
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(21, 0), "#15a4a4");  // y
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(22, 0), "#ffffff");  // z
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(23, 0), "#ffffff");  //*
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(25, 0), "#ffffff");  // gap
}

GUI_TEST_CLASS_DEFINITION(highlighting_test_0004_3) {
    //    1. Open file test/_common_data/alphabets/extended_amino.aln
    GTFileDialog::openFile(testDir + "_common_data/alphabets", "extended_amino.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Open highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Highlighting);
    //    3. Select "Helix propensity" color scheme
    auto colorScheme = GTWidget::findComboBox("colorScheme");
    GTComboBox::selectItemByText(colorScheme, "Helix propensity");
    //    4. Check colors for all symbols
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(0, 0), "#e718e7");  // a
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(1, 0), "#49b649");  // b
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(2, 0), "#23dc23");  // c
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(3, 0), "#778877");  // d
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(4, 0), "#ff00ff");  // e
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(5, 0), "#986798");  // f
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(6, 0), "#00ff00");  // g
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(7, 0), "#758a75");  // h
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(8, 0), "#8a758a");  // i
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(28, 0), "#ffffff");  // j
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(9, 0), "#a05fa0");  // k
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(10, 0), "#ae51ae");  // l
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(11, 0), "#ef10ef");  // m
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(12, 0), "#1be41b");  // n
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(26, 0), "#ffffff");  // o
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(13, 0), "#00ff00");  // p
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(14, 0), "#926d92");  // q
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(15, 0), "#6f906f");  // r
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(16, 0), "#36c936");  // s
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(17, 0), "#47b847");  // t
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(27, 0), "#ffffff");  // u
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(18, 0), "#857a85");  // v
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(19, 0), "#8a758a");  // w
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(20, 0), "#758a75");  // x
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(21, 0), "#21de21");  // y
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(22, 0), "#c936c9");  // z
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(23, 0), "#ffffff");  //*
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(25, 0), "#ffffff");  // gap
}

GUI_TEST_CLASS_DEFINITION(highlighting_test_0004_4) {
    //    1. Open file test/_common_data/alphabets/extended_amino.aln
    GTFileDialog::openFile(testDir + "_common_data/alphabets", "extended_amino.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Open highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Highlighting);
    //    3. Select "Hydrophobicity" color scheme
    auto colorScheme = GTWidget::findComboBox("colorScheme");
    GTComboBox::selectItemByText(colorScheme, "Hydrophobicity");
    //    4. Check colors for all symbols
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(0, 0), "#ad0052");  // a
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(1, 0), "#0c00f3");  // b
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(2, 0), "#c2003d");  // c
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(3, 0), "#0c00f3");  // d
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(4, 0), "#0c00f3");  // e
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(5, 0), "#cb0034");  // f
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(6, 0), "#6a0095");  // g
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(7, 0), "#1500ea");  // h
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(8, 0), "#ff0000");  // i
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(28, 0), "#ffffff");  // j
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(9, 0), "#0000ff");  // k
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(10, 0), "#ea0015");  // l
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(11, 0), "#b0004f");  // m
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(12, 0), "#0c00f3");  // n
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(26, 0), "#ffffff");  // o
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(13, 0), "#4600b9");  // p
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(14, 0), "#0c00f3");  // q
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(15, 0), "#0000ff");  // r
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(16, 0), "#5e00a1");  // s
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(17, 0), "#61009e");  // t
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(27, 0), "#ffffff");  // u
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(18, 0), "#f60009");  // v
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(19, 0), "#5b00a4");  // w
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(20, 0), "#680097");  // x
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(21, 0), "#4f00b0");  // y
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(22, 0), "#0c00f3");  // z
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(23, 0), "#ffffff");  //*
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(25, 0), "#ffffff");  // gap
}

GUI_TEST_CLASS_DEFINITION(highlighting_test_0004_5) {
    //    1. Open file test/_common_data/alphabets/extended_amino.aln
    GTFileDialog::openFile(testDir + "_common_data/alphabets", "extended_amino.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Open highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Highlighting);
    //    3. Select "Strand propensity" color scheme
    auto colorScheme = GTWidget::findComboBox("colorScheme");
    GTComboBox::selectItemByText(colorScheme, "Strand propensity");
    //    4. Check colors for all symbols
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(0, 0), "#5858a7");  // a
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(1, 0), "#4343bc");  // b
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(2, 0), "#9d9d62");  // c
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(3, 0), "#2121de");  // d
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(4, 0), "#0000ff");  // e
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(5, 0), "#c2c23d");  // f
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(6, 0), "#4949b6");  // g
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(7, 0), "#60609f");  // h
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(8, 0), "#ecec13");  // i
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(28, 0), "#ffffff");  // j
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(9, 0), "#4747b8");  // k
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(10, 0), "#b2b24d");  // l
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(11, 0), "#82827d");  // m
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(12, 0), "#64649b");  // n
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(26, 0), "#ffffff");  // o
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(13, 0), "#2323dc");  // p
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(14, 0), "#8c8c73");  // q
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(15, 0), "#6b6b94");  // r
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(16, 0), "#4949b6");  // s
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(17, 0), "#9d9d62");  // t
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(27, 0), "#ffffff");  // u
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(18, 0), "#ffff00");  // v
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(19, 0), "#c0c03f");  // w
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(20, 0), "#797986");  // x
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(21, 0), "#d3d32c");  // y
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(22, 0), "#4747b8");  // z
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(23, 0), "#ffffff");  //*
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(25, 0), "#ffffff");  // gap
}

GUI_TEST_CLASS_DEFINITION(highlighting_test_0004_6) {
    //    1. Open file test/_common_data/alphabets/extended_amino.aln
    GTFileDialog::openFile(testDir + "_common_data/alphabets", "extended_amino.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Open highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Highlighting);
    //    3. Select "Tailor" color scheme
    auto colorScheme = GTWidget::findComboBox("colorScheme");
    GTComboBox::selectItemByText(colorScheme, "Tailor");
    //    4. Check colors for all symbols
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(0, 0), "#ccff00");  // a
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(1, 0), "#ffffff");  // b
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(2, 0), "#ffff00");  // c
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(3, 0), "#ff0000");  // d
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(4, 0), "#ff0066");  // e
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(5, 0), "#00ff66");  // f
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(6, 0), "#ff9900");  // g
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(7, 0), "#0066ff");  // h
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(8, 0), "#66ff00");  // i
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(28, 0), "#ffffff");  // j
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(9, 0), "#6600ff");  // k
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(10, 0), "#33ff00");  // l
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(11, 0), "#00ff00");  // m
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(12, 0), "#cc00ff");  // n
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(26, 0), "#ffffff");  // o
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(13, 0), "#ffcc00");  // p
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(14, 0), "#ff00cc");  // q
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(15, 0), "#0000ff");  // r
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(16, 0), "#ff3300");  // s
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(17, 0), "#ff6600");  // t
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(27, 0), "#ffffff");  // u
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(18, 0), "#99ff00");  // v
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(19, 0), "#00ccff");  // w
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(20, 0), "#ffffff");  // x
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(21, 0), "#00ffcc");  // y
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(22, 0), "#ffffff");  // z
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(23, 0), "#ffffff");  //*
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(25, 0), "#ffffff");  // gap
}

GUI_TEST_CLASS_DEFINITION(highlighting_test_0004_7) {
    //    1. Open file test/_common_data/alphabets/extended_amino.aln
    GTFileDialog::openFile(testDir + "_common_data/alphabets", "extended_amino.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Open highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Highlighting);
    //    3. Select "Turn propensity" color scheme
    auto colorScheme = GTWidget::findComboBox("colorScheme");
    GTComboBox::selectItemByText(colorScheme, "Turn propensity");
    //    4. Check colors for all symbols
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(0, 0), "#2cd3d3");  // a
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(1, 0), "#f30c0c");  // b
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(2, 0), "#a85757");  // c
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(3, 0), "#e81717");  // d
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(4, 0), "#3fc0c0");  // e
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(5, 0), "#1ee1e1");  // f
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(6, 0), "#ff0000");  // g
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(7, 0), "#708f8f");  // h
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(8, 0), "#00ffff");  // i
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(28, 0), "#ffffff");  // j
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(9, 0), "#7e8181");  // k
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(10, 0), "#1ce3e3");  // l
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(11, 0), "#1ee1e1");  // m
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(12, 0), "#ff0000");  // n
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(26, 0), "#ffffff");  // o
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(13, 0), "#f60909");  // p
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(14, 0), "#778888");  // q
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(15, 0), "#708f8f");  // r
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(16, 0), "#e11e1e");  // s
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(17, 0), "#738c8c");  // t
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(27, 0), "#ffffff");  // u
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(18, 0), "#07f8f8");  // v
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(19, 0), "#738c8c");  // w
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(20, 0), "#7c8383");  // x
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(21, 0), "#9d6262");  // y
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(22, 0), "#5ba4a4");  // z
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(23, 0), "#ffffff");  //*
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(25, 0), "#ffffff");  // gap
}

GUI_TEST_CLASS_DEFINITION(highlighting_test_0004_8) {
    //    1. Open file test/_common_data/alphabets/extended_amino.aln
    GTFileDialog::openFile(testDir + "_common_data/alphabets", "extended_amino.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    //    2. Open highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Highlighting);
    //    3. Select "UGENE" color scheme
    auto colorScheme = GTWidget::findComboBox("colorScheme");
    GTComboBox::selectItemByText(colorScheme, "UGENE");
    //    4. Check colors for all symbols
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(0, 0), "#00ccff");  // a
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(1, 0), "#ccff99");  // b
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(2, 0), "#6600ff");  // c
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(3, 0), "#ffff00");  // d
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(4, 0), "#c0bdbb");  // e
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(5, 0), "#3df490");  // f
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(6, 0), "#ff5082");  // g
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(7, 0), "#fff233");  // h
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(8, 0), "#00abed");  // i
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(28, 0), "#6699ff");  // j
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(9, 0), "#ffee00");  // k
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(10, 0), "#008fc6");  // l
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(11, 0), "#1dc0ff");  // m
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(12, 0), "#33ff00");  // n
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(26, 0), "#ffff99");  // o
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(13, 0), "#d5426c");  // p
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(14, 0), "#3399ff");  // q
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(15, 0), "#d5c700");  // r
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(16, 0), "#ff83a7");  // s
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(17, 0), "#ffd0dd");  // t
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(27, 0), "#ff00cc");  // u
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(18, 0), "#ff6699");  // v
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(19, 0), "#33cc78");  // w
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(20, 0), "#fcfcfc");  // x
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(21, 0), "#65ffab");  // y
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(22, 0), "#ffffcc");  // z
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(23, 0), "#33ff00");  //*
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(25, 0), "#ffffff");  // gap
}

GUI_TEST_CLASS_DEFINITION(highlighting_test_0004_9) {
    //    1. Open file test/_common_data/alphabets/extended_amino.aln
    GTFileDialog::openFile(testDir + "_common_data/alphabets", "extended_amino.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Open highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Highlighting);
    //    3. Select "Zappo" color scheme
    auto colorScheme = GTWidget::findComboBox("colorScheme");
    GTComboBox::selectItemByText(colorScheme, "Zappo");
    //    4. Check colors for all symbols
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(0, 0), "#ffafaf");  // a
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(1, 0), "#ffffff");  // b
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(2, 0), "#ffff00");  // c
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(3, 0), "#ff0000");  // d
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(4, 0), "#ff0000");  // e
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(5, 0), "#ffc800");  // f
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(6, 0), "#ff00ff");  // g
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(7, 0), "#6464ff");  // h
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(8, 0), "#ffafaf");  // i
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(28, 0), "#ffffff");  // j
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(9, 0), "#6464ff");  // k
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(10, 0), "#ffafaf");  // l
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(11, 0), "#ffafaf");  // m
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(12, 0), "#00ff00");  // n
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(26, 0), "#ffffff");  // o
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(13, 0), "#ff00ff");  // p
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(14, 0), "#00ff00");  // q
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(15, 0), "#6464ff");  // r
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(16, 0), "#00ff00");  // s
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(17, 0), "#00ff00");  // t
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(27, 0), "#ffffff");  // u
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(18, 0), "#ffafaf");  // v
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(19, 0), "#ffc800");  // w
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(20, 0), "#ffffff");  // x
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(21, 0), "#ffc800");  // y
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(22, 0), "#ffffff");  // z
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(23, 0), "#ffffff");  //*
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(25, 0), "#ffffff");  // gap
}

GUI_TEST_CLASS_DEFINITION(highlighting_test_0005) {
    //    1. Open file test/_common_data/alphabets/extended_amino.aln
    GTFileDialog::openFile(testDir + "_common_data/alphabets", "extended_amino.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Create custom color scheme
    QString suffix = GTUtils::genUniqueString();
    const QString scheme = name + "_scheme" + suffix;
    GTUtilsMSAEditorSequenceArea::createColorScheme(scheme, NewColorSchemeCreator::amino);
    //    3. Open highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Highlighting);
    //    Expected state: color scheme added to "Color" combobox
    auto colorScheme = GTWidget::findComboBox("colorScheme");
    GTComboBox::selectItemByText(colorScheme, "No colors");
    GTComboBox::selectItemByText(colorScheme, scheme);
    //    4. Select custom scheme
    //    Expected state: scheme changed
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(0, 0), "#ad0052");  // a
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(1, 0), "#0c00f3");  // b
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(2, 0), "#c2003d");  // c
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(3, 0), "#0c00f3");  // d
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(4, 0), "#0c00f3");  // e
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(5, 0), "#cb0034");  // f
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(6, 0), "#6a0095");  // g
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(7, 0), "#1500ea");  // h
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(8, 0), "#ff0000");  // i
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(28, 0), "#ffffff");  // j
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(9, 0), "#0000ff");  // k
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(10, 0), "#ea0015");  // l
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(11, 0), "#b0004f");  // m
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(12, 0), "#0c00f3");  // n
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(26, 0), "#ffffff");  // o
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(13, 0), "#4600b9");  // p
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(14, 0), "#0c00f3");  // q
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(15, 0), "#0000ff");  // r
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(16, 0), "#5e00a1");  // s
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(17, 0), "#61009e");  // t
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(27, 0), "#ffffff");  // u
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(18, 0), "#f60009");  // v
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(19, 0), "#5b00a4");  // w
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(20, 0), "#680097");  // x
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(21, 0), "#4f00b0");  // y
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(22, 0), "#0c00f3");  // z
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(23, 0), "#ffffff");  //*
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(25, 0), "#ffffff");  // gap
}

GUI_TEST_CLASS_DEFINITION(highlighting_test_0005_1) {
    //    1. Open file test/_common_data/alphabets/extended_amino.aln
    GTFileDialog::openFile(testDir + "_common_data/alphabets", "extended_amino.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Create 3 color schemes
    QString suffix = GTUtils::genUniqueString();
    QString scheme1 = name + "_scheme1" + suffix;
    QString scheme2 = name + "_scheme2" + suffix;
    QString scheme3 = name + "_scheme3" + suffix;
    GTUtilsMSAEditorSequenceArea::createColorScheme(scheme1, NewColorSchemeCreator::amino);
    GTUtilsMSAEditorSequenceArea::createColorScheme(scheme2, NewColorSchemeCreator::amino);
    GTUtilsMSAEditorSequenceArea::createColorScheme(scheme3, NewColorSchemeCreator::amino);
    //    Expected state: color schemes added to "Color" combobox
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Highlighting);
    auto colorScheme = GTWidget::findComboBox("colorScheme");
    GTComboBox::selectItemByText(colorScheme, scheme1);
    GTComboBox::selectItemByText(colorScheme, scheme2);
    GTComboBox::selectItemByText(colorScheme, scheme3);
}

GUI_TEST_CLASS_DEFINITION(highlighting_test_0006) {
    //    1. Open file test/_common_data/alphabets/extended_amino.aln
    GTFileDialog::openFile(testDir + "_common_data/alphabets", "extended_amino.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    //    2. Create custom color scheme
    QString suffix = GTUtils::genUniqueString();
    const QString scheme = name + "_scheme" + suffix;
    GTUtilsMSAEditorSequenceArea::createColorScheme(scheme, NewColorSchemeCreator::amino);

    //    3. Open highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Highlighting);
    GTUtilsOptionPanelMsa::checkTabIsOpened(GTUtilsOptionPanelMsa::Highlighting);

    //    4. Select custom scheme
    auto colorScheme = GTWidget::findComboBox("colorScheme");
    GTComboBox::selectItemByText(colorScheme, scheme);

    //    5. Delete scheme which is selected
    GTUtilsMSAEditorSequenceArea::deleteColorScheme(scheme);

    //    UGENE doesn't crash
    QString currentScheme = GTUtilsOptionPanelMsa::getColorScheme();
    CHECK_SET_ERR(currentScheme == "UGENE", QString("An unexpected color scheme is set: expect '%1', got '%2'").arg("UGENE").arg(currentScheme));

    GTUtilsDialog::waitForDialog(new PopupCheckerByText({"Appearance", "Colors", "UGENE"}, PopupChecker::IsChecked));
    GTUtilsMSAEditorSequenceArea::callContextMenu();

    GTUtilsOptionPanelMsa::closeTab(GTUtilsOptionPanelMsa::Highlighting);
}

static void setHighlightingType(const QString& type) {
    auto highlightingScheme = GTWidget::findComboBox("highlightingScheme");
    GTComboBox::selectItemByText(highlightingScheme, type);
}

GUI_TEST_CLASS_DEFINITION(highlighting_test_0007) {
    //    1. Open file test/_common_data/scenarios/msa/ma2_gapped.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Open highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Highlighting);
    //    3. Select Phaneroptera_falcata as reference.
    GTUtilsOptionPanelMsa::addReference("Phaneroptera_falcata");
    //    4. Check no highlighting
    setHighlightingType("No highlighting");
    QString a = GTUtilsMSAEditorSequenceArea::getColor(QPoint(0, 0));
    QString t = GTUtilsMSAEditorSequenceArea::getColor(QPoint(0, 2));
    QString g = GTUtilsMSAEditorSequenceArea::getColor(QPoint(2, 0));
    QString c = GTUtilsMSAEditorSequenceArea::getColor(QPoint(4, 0));
    QString gap = GTUtilsMSAEditorSequenceArea::getColor(QPoint(4, 2));
    CHECK_SET_ERR(a == "#fdff6a", QString("a has color %1").arg(a));
    CHECK_SET_ERR(t == "#ff99b1", QString("t has color %1").arg(t));
    CHECK_SET_ERR(g == "#2aa1e1", QString("g has color %1").arg(g));
    CHECK_SET_ERR(c == "#49f949", QString("c has color %1").arg(c));
    CHECK_SET_ERR(gap == "#ffffff", QString("gap has color %1").arg(gap));
    /* GTUtilsMSAEditorSequenceArea::checkColor(QPoint(0,0), "#fcff92");//yellow
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(0,2), "#ff99b1");//red
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(2,0),"#4eade1"); //blue
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(4,0), "#70f970");//green
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(4,2), "#ffffff");//white
*/
}

GUI_TEST_CLASS_DEFINITION(highlighting_test_0007_1) {
    //    1. Open file test/_common_data/scenarios/msa/ty3.aln.gz
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "ty3.aln.gz");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Open highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Highlighting);
    //    3. Select Phaneroptera_falcata as reference.
    GTUtilsOptionPanelMsa::addReference("CfT-1_Cladosporium_fulvum");
    //    4. Check no highlighting
    setHighlightingType("No highlighting");
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(6, 1), "#c0bdbb");
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(7, 1), "#ffee00");
}

GUI_TEST_CLASS_DEFINITION(highlighting_test_0008) {
    //    1. Open file test/_common_data/scenarios/msa/ma2_gapped.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Open highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Highlighting);
    //    3. Select Phaneroptera_falcata as reference.
    GTUtilsOptionPanelMsa::addReference("Phaneroptera_falcata");
    //    4. Check Agreements highlighting type
    setHighlightingType("Agreements");

    QString a = GTUtilsMSAEditorSequenceArea::getColor(QPoint(0, 0));
    QString gap1 = GTUtilsMSAEditorSequenceArea::getColor(QPoint(0, 2));
    QString g = GTUtilsMSAEditorSequenceArea::getColor(QPoint(2, 0));
    QString gap2 = GTUtilsMSAEditorSequenceArea::getColor(QPoint(3, 1));
    QString gap3 = GTUtilsMSAEditorSequenceArea::getColor(QPoint(4, 2));
    CHECK_SET_ERR(a == "#fdff6a", QString("a has color %1 intead of %2").arg(a).arg("#fcff92"));
    CHECK_SET_ERR(gap1 == "#ffffff", QString("gap1 has color %1 intead of %2").arg(gap1).arg("#ffffff"));
    CHECK_SET_ERR(g == "#2aa1e1", QString("g has color %1 intead of %2").arg(g).arg("#4eade1"));
    CHECK_SET_ERR(gap2 == "#ffffff", QString("gap2 has color%1 intead of %2").arg(gap2).arg("#ffffff"));
    CHECK_SET_ERR(gap3 == "#ffffff", QString("gap3 has color %1 intead of %2").arg(gap3).arg("#ffffff"));
    /*
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(0,0), "#fcff92");
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(0,2), "#ffffff");
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(2,0), "#4eade1");
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(3,1), "#ffffff");
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(4,2), "#ffffff");
    */
}

GUI_TEST_CLASS_DEFINITION(highlighting_test_0008_1) {
    //    1. Open file test/_common_data/scenarios/msa/ty3.aln.gz
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "ty3.aln.gz");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Open highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Highlighting);
    //    3. Select Phaneroptera_falcata as reference.
    GTUtilsOptionPanelMsa::addReference("CfT-1_Cladosporium_fulvum");
    //    4. Check Agreements highlighting type
    setHighlightingType("Agreements");
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(6, 1), "#ffffff");
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(7, 1), "#ffee00");
}

GUI_TEST_CLASS_DEFINITION(highlighting_test_0009) {
    //    1. Open file test/_common_data/scenarios/msa/ma2_gapped.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Open highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Highlighting);
    //    3. Select Phaneroptera_falcata as reference.
    GTUtilsOptionPanelMsa::addReference("Phaneroptera_falcata");
    //    4. Check Disagreements highlighting type
    setHighlightingType("Disagreements");
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(0, 1), "#ffffff");
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(2, 1), "#ffffff");
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(5, 1), "#ffffff");
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(7, 1), "#ffffff");
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(4, 2), "#ffffff");
}

GUI_TEST_CLASS_DEFINITION(highlighting_test_0009_1) {
    //    1. Open file test/_common_data/scenarios/msa/ty3.aln.gz
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "ty3.aln.gz");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Open highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Highlighting);

    //    3. Select Phaneroptera_falcata as reference.
    GTUtilsOptionPanelMsa::addReference("CfT-1_Cladosporium_fulvum");

    //    4. Check Disagreements highlighting type
    setHighlightingType("Disagreements");
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(4, 1), "#ffffff");
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(7, 1), "#ffffff");
}

GUI_TEST_CLASS_DEFINITION(highlighting_test_0010) {
    //    1. Open file test/_common_data/scenarios/msa/ma2_gapped.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Open highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Highlighting);

    //    3. Select Phaneroptera_falcata as reference.
    GTUtilsOptionPanelMsa::addReference("Phaneroptera_falcata");

    //    4. Check Gaps highlighting type
    setHighlightingType("Gaps");
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(4, 2), "#c0c0c0");
}

GUI_TEST_CLASS_DEFINITION(highlighting_test_0010_1) {
    //    1. Open file test/_common_data/scenarios/msa/ty3.aln.gz
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "ty3.aln.gz");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Open highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Highlighting);
    //    3. Select Phaneroptera_falcata as reference.
    GTUtilsOptionPanelMsa::addReference("CfT-1_Cladosporium_fulvum");
    //    4. Check Gaps highlighting type
    setHighlightingType("Gaps");
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(0, 1), "#c0c0c0");
}

GUI_TEST_CLASS_DEFINITION(highlighting_test_0011) {
    //    1. Open file test/_common_data/scenarios/msa/ma2_gapped.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Open highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Highlighting);
    //    3. Select Phaneroptera_falcata as reference.
    GTUtilsOptionPanelMsa::addReference("Phaneroptera_falcata");
    //    4. Check Transitions highlighting type
    setHighlightingType("Transitions");
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(0, 1), "#ffffff");
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(2, 1), "#ffffff");
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(4, 1), "#ff99b1");
}

GUI_TEST_CLASS_DEFINITION(highlighting_test_0012) {
    //    1. Open file test/_common_data/scenarios/msa/ma2_gapped.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Open highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Highlighting);
    //    3. Select Phaneroptera_falcata as reference.
    GTUtilsOptionPanelMsa::addReference("Phaneroptera_falcata");
    //    4. Check Transversions highlighting type
    setHighlightingType("Transversions");
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(0, 1), "#ffffff");
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(3, 1), "#70f970");
    GTUtilsMSAEditorSequenceArea::checkColor(QPoint(6, 1), "#fcff92");
}

GUI_TEST_CLASS_DEFINITION(highlighting_test_0013) {
    // 1. Open file test/_common_data/scenarios/msa/ma2_gapped.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Open highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Highlighting);
    auto w = GTUtilsMSAEditorSequenceArea::getSequenceArea(0);
    const QImage initImg = GTWidget::getImage(w);

    // 3. Check "use dots" checkbox
    setHighlightingType("Agreements");
    auto useDots = GTWidget::findCheckBox("useDots");
    GTCheckBox::setChecked(useDots, true);

    // Expected state: no effect
    QImage img = GTWidget::getImage(w);
    CHECK_SET_ERR(img == initImg, "sequence area unexpectedly changed");

    // 4. Select Phaneroptera_falcata as reference.
    GTUtilsOptionPanelMsa::addReference("Phaneroptera_falcata");

    // Expected state: not highlighted changed to dots
    img = GTWidget::getImage(w);
    CHECK_SET_ERR(img != initImg, "image not changed");  // no way to check dots. Can only check that sequence area changed
}

GUI_TEST_CLASS_DEFINITION(pairwise_alignment_test_0001) {
    //    1. Open file test/_common_data/scenarios/msa/ma2_gapped.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Open Pairwise alignment option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::PairwiseAlignment);
    //    3. Add Phaneroptera_falcata sequence
    GTUtilsOptionPanelMsa::addFirstSeqToPA("Phaneroptera_falcata");
    //    Expected state: align button is disabled
    auto alignButton = GTWidget::findPushButton("alignButton");
    CHECK_SET_ERR(!alignButton->isEnabled(), "alignButton is unexpectidly enabled");
    //    4. Add Isophya_altaica_EF540820 sequence
    GTUtilsOptionPanelMsa::addSecondSeqToPA("Isophya_altaica_EF540820");
    //    Expected state: Similarity label appeared. Similarity is 43%. Align button enabled
    CHECK_SET_ERR(alignButton->isEnabled(), "align button is unexpectibly disabled");
    auto similarityValueLabel = GTWidget::findLabel("similarityValueLabel");
    QString percent = similarityValueLabel->text();
    CHECK_SET_ERR(percent == "43%", QString("unexpected percent: %1").arg(percent));
    //    (branches: amino, raw alphabets)
}

GUI_TEST_CLASS_DEFINITION(pairwise_alignment_test_0002) {
    //    1. Open file test/_common_data/scenarios/msa/ma2_gapped.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Open Pairwise alignment option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::PairwiseAlignment);
    //    3. Add Phaneroptera_falcata sequence two times
    GTUtilsOptionPanelMsa::addFirstSeqToPA("Phaneroptera_falcata");
    GTUtilsOptionPanelMsa::addSecondSeqToPA("Phaneroptera_falcata");
    //    Expected state: Similarity label appeared. Similarity is 100%. Align button disabled
    auto alignButton = GTWidget::findPushButton("alignButton");
    CHECK_SET_ERR(!alignButton->isEnabled(), "align button is unexpectibly disabled");

    auto similarityValueLabel = GTWidget::findLabel("similarityValueLabel");
    QString percent = similarityValueLabel->text();
    CHECK_SET_ERR(percent == "100%", QString("unexpected percent: %1").arg(percent));
    //    (branches: amino, raw alphabets)
}

GUI_TEST_CLASS_DEFINITION(pairwise_alignment_test_0003) {
    //    1. Open file test/_common_data/scenarios/msa/ma2_gapped.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Open Pairwise alignment option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::PairwiseAlignment);
    //    3. Add Phaneroptera_falcata sequence with popup helper
    //    Expected state: sequence added
    GTUtilsOptionPanelMsa::addFirstSeqToPA("Phaneroptera_falcata", GTUtilsOptionPanelMsa::Completer);
    QLineEdit* line1 = GTUtilsOptionPanelMsa::getSeqLineEdit(1);
    CHECK_SET_ERR(line1 != nullptr, "lineEdit 1 not found");
    CHECK_SET_ERR(line1->text() == "Phaneroptera_falcata", QString("unexpected lineEdit 1 text: %1").arg(line1->text()));
    //    4. Add Isophya_altaica_EF540820 sequence with popup helper
    //    Expected state: sequence added
    GTUtilsOptionPanelMsa::addSecondSeqToPA("Isophya_altaica_EF540820", GTUtilsOptionPanelMsa::Completer);
    QLineEdit* line2 = GTUtilsOptionPanelMsa::getSeqLineEdit(2);
    CHECK_SET_ERR(line2 != nullptr, "lineEdit 2 not found");
    CHECK_SET_ERR(line2->text() == "Isophya_altaica_EF540820", QString("unexpected lineEdit 2 text: %1").arg(line2->text()));
    //    5. Remove Phaneroptera_falcata with button
    GTWidget::click(GTUtilsOptionPanelMsa::getDeleteButton(1));
    CHECK_SET_ERR(line1->text().isEmpty(), QString("unexpected lineEdit 1 text: %1").arg(line1->text()));
    //    6. Remove Phaneroptera_falcata with button
    GTWidget::click(GTUtilsOptionPanelMsa::getDeleteButton(2));
    CHECK_SET_ERR(line2->text().isEmpty(), QString("unexpected lineEdit 2 text: %1").arg(line2->text()));
}

GUI_TEST_CLASS_DEFINITION(pairwise_alignment_test_0004) {
    //    1. Open file test/_common_data/scenarios/msa/ma2_gapped.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Open Pairwise alignment option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::PairwiseAlignment);
    //    3. Type wrong names in sequences' line edits.
    QLineEdit* line1 = GTUtilsOptionPanelMsa::getSeqLineEdit(1);
    CHECK_SET_ERR(line1 != nullptr, "lineEdit 1 not found");
    GTLineEdit::setText(line1, "wrong name");
    CHECK_SET_ERR(GTBaseCompleter::isEmpty(line1), "Completer is not empty");

    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    QLineEdit* line2 = GTUtilsOptionPanelMsa::getSeqLineEdit(2);
    CHECK_SET_ERR(line2 != nullptr, "lineEdit 2 not found");
    GTLineEdit::setText(line2, "wrong name");
    CHECK_SET_ERR(GTBaseCompleter::isEmpty(line2), "Completer is not empty");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
    GTUtilsOptionPanelMsa::toggleTab(GTUtilsOptionPanelMsa::PairwiseAlignment);
    //    Expected state: empty popup helper appeared
}

GUI_TEST_CLASS_DEFINITION(pairwise_alignment_test_0005) {
    //    1. Open file test/_common_data/scenarios/msa/ty3.aln.gz
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "ty3.aln.gz");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Open Pairwise alignment option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::PairwiseAlignment);
    //    3. Add two sequences to PA line edits
    GTUtilsOptionPanelMsa::addFirstSeqToPA("CfT-1_Cladosporium_fulvum");
    GTUtilsOptionPanelMsa::addSecondSeqToPA("AspOryTy3-2");
    //    Expected state: sequenseq added
    QLineEdit* line1 = GTUtilsOptionPanelMsa::getSeqLineEdit(1);
    QLineEdit* line2 = GTUtilsOptionPanelMsa::getSeqLineEdit(2);
    CHECK_SET_ERR(line1 != nullptr, "line edit1 not found");
    CHECK_SET_ERR(line2 != nullptr, "line edit2 not found");
    CHECK_SET_ERR(line1->text() == "CfT-1_Cladosporium_fulvum", QString("wrong text in line edit1: %1").arg(line1->text()));
    CHECK_SET_ERR(line2->text() == "AspOryTy3-2", QString("wrong text in line edit2: %1").arg(line2->text()));
    //    4. Remove sequenses
    GTWidget::click(GTUtilsOptionPanelMsa::getDeleteButton(1));
    GTWidget::click(GTUtilsOptionPanelMsa::getDeleteButton(2));
    //    Expected state: sequences removed
    CHECK_SET_ERR(line1->text().isEmpty(), QString("wrong text in line edit1: %1").arg(line1->text()));
    CHECK_SET_ERR(line2->text().isEmpty(), QString("wrong text in line edit2: %1").arg(line2->text()));
}

GUI_TEST_CLASS_DEFINITION(pairwise_alignment_test_0005_1) {
    //    1. Open file test/_common_data/alphabets/raw_alphabet.aln
    GTFileDialog::openFile(testDir + "_common_data/alphabets", "raw_alphabet.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Open Pairwise alignment option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::PairwiseAlignment);
    //    Expected state: wrong alphabet hint appeared
    auto lblMessage = GTWidget::findLabel("lblMessage");
    CHECK_SET_ERR(lblMessage->text() == "Pairwise alignment is not available for alignments with \"Raw\" alphabet.",
                  QString("wrong label text: %1").arg(lblMessage->text()));
    //    3. Add two sequences to PA line edits
    GTUtilsOptionPanelMsa::addFirstSeqToPA("seq7_1");
    GTUtilsOptionPanelMsa::addSecondSeqToPA("seq7");
    //    Expected state: sequenseq added
    QLineEdit* line1 = GTUtilsOptionPanelMsa::getSeqLineEdit(1);
    QLineEdit* line2 = GTUtilsOptionPanelMsa::getSeqLineEdit(2);
    CHECK_SET_ERR(line1 != nullptr, "line edit1 not found");
    CHECK_SET_ERR(line2 != nullptr, "line edit2 not found");
    CHECK_SET_ERR(line1->text() == "seq7_1", QString("wrong text in line edit1: %1").arg(line1->text()));
    CHECK_SET_ERR(line2->text() == "seq7", QString("wrong text in line edit2: %1").arg(line2->text()));
    //    4. Remove sequenses
    GTWidget::click(GTUtilsOptionPanelMsa::getDeleteButton(1));
    GTWidget::click(GTUtilsOptionPanelMsa::getDeleteButton(2));
    GTWidget::click(GTUtilsOptionPanelMsa::getDeleteButton(1));

    //    Expected state: sequences removed
    CHECK_SET_ERR(line1->text().isEmpty(), QString("wrong text in line edit1: %1").arg(line1->text()));
    CHECK_SET_ERR(line2->text().isEmpty(), QString("wrong text in line edit2: %1").arg(line2->text()));
}

namespace {

void expandSettings(const QString& widgetName, const QString& arrowName) {
    auto widget = GTWidget::findWidget(widgetName);
    CHECK_SET_ERR(widget != nullptr, QString("%1 not found").arg(widgetName));
    if (widget->isHidden()) {
        GTWidget::click(GTWidget::findWidget(arrowName));
    }
}

void expandAlgoSettings() {
    expandSettings("settingsContainerWidget", "ArrowHeader_Algorithm settings");
}

void expandOutputSettings() {
    expandSettings("outputContainerWidget", "ArrowHeader_Output settings");
}

}  // namespace
GUI_TEST_CLASS_DEFINITION(pairwise_alignment_test_0006) {
    //    1. Open file test/_common_data/scenarios/msa/ma2_gapped.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Open Pairwise alignment option panel tab. check spinboxes limits for KAilign
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::PairwiseAlignment);
    expandAlgoSettings();

    auto gapOpen = GTWidget::findDoubleSpinBox("gapOpen");
    auto gapExtd = GTWidget::findDoubleSpinBox("gapExtd");
    auto gapTerm = GTWidget::findDoubleSpinBox("gapTerm");
    auto bonusScore = GTWidget::findDoubleSpinBox("bonusScore");

    GTDoubleSpinbox::checkLimits(gapOpen, 0, 65535);
    GTDoubleSpinbox::checkLimits(gapExtd, 0, 65535);
    GTDoubleSpinbox::checkLimits(gapTerm, 0, 65535);
    GTDoubleSpinbox::checkLimits(bonusScore, 0, 65535);
    //    3. Add Phaneroptera_falcata and Isophya_altaica_EF540820 sequences to PA
    GTUtilsOptionPanelMsa::addFirstSeqToPA("Phaneroptera_falcata");
    GTUtilsOptionPanelMsa::addSecondSeqToPA("Isophya_altaica_EF540820");
    //    4. Align with KAlign
    GTWidget::click(GTWidget::findWidget("alignButton"));
    QString expected = "AAGACTTCTTTTAA\n"
                       "AAGCTTACT---AA";
    GTUtilsMSAEditorSequenceArea::checkSelection(QPoint(0, 0), QPoint(13, 1), expected);
}

namespace {

void setSpinValue(double value, const QString& spinName) {
    expandAlgoSettings();
    auto spinBox = GTWidget::findDoubleSpinBox(spinName);
    GTDoubleSpinbox::setValue(spinBox, value, GTGlobals::UseKeyBoard);
}

void setGapOpen(double value) {
    setSpinValue(value, "gapOpen");
}

void setGapExtd(double value) {
    setSpinValue(value, "gapExtd");
}

void setGapTerm(double value) {
    setSpinValue(value, "gapTerm");
}

void setBonusScore(double value) {
    setSpinValue(value, "bonusScore");
}

void inNewWindow(bool inNew) {
    expandOutputSettings();
    auto inNewWindowCheckBox = GTWidget::findCheckBox("inNewWindowCheckBox");
    GTCheckBox::setChecked(inNewWindowCheckBox, inNew);
}

void align() {
    GTWidget::click(GTWidget::findWidget("alignButton"));
}

}  // namespace
GUI_TEST_CLASS_DEFINITION(pairwise_alignment_test_0007) {
    //    1. Open file test/_common_data/scenarios/msa/ma2_gapped.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Open Pairwise alignment option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::PairwiseAlignment);
    //    3. Add Phaneroptera_falcata and Isophya_altaica_EF540820 sequences to PA
    GTUtilsOptionPanelMsa::addFirstSeqToPA("Phaneroptera_falcata");
    GTUtilsOptionPanelMsa::addSecondSeqToPA("Isophya_altaica_EF540820");
    //    4. Set gapOpen to 1. Press align button
    setGapOpen(1);
    inNewWindow(false);
    align();
    //    Expected state: Isophya_altaica_EF540820 is AAG-CTTA-CT-AA
    GTUtilsMSAEditorSequenceArea::checkSelection(QPoint(0, 1), QPoint(13, 1), "AAG-CTTA-CT-AA");
}

GUI_TEST_CLASS_DEFINITION(pairwise_alignment_test_0007_1) {
    //    1. Open file test/_common_data/scenarios/msa/ma2_gapped.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Open Pairwise alignment option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::PairwiseAlignment);
    //    3. Add Phaneroptera_falcata and Isophya_altaica_EF540820 sequences to PA
    GTUtilsOptionPanelMsa::addFirstSeqToPA("Phaneroptera_falcata");
    GTUtilsOptionPanelMsa::addSecondSeqToPA("Isophya_altaica_EF540820");
    //    4. Set gap extension penalty to 1000. Press align button
    setGapExtd(1000);
    inNewWindow(false);
    align();
    //    Expected state: Isophya_altaica_EF540820 is AAG-CT--TACTAA
    GTUtilsMSAEditorSequenceArea::checkSelection(QPoint(0, 1), QPoint(13, 1), "AAG-CT--TACTAA");
}

GUI_TEST_CLASS_DEFINITION(pairwise_alignment_test_0007_2) {
    //    1. Open file test/_common_data/scenarios/msa/ma2_gapped.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Open Pairwise alignment option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::PairwiseAlignment);
    //    3. Add Phaneroptera_falcata and Isophya_altaica_EF540820 sequences to PA
    GTUtilsOptionPanelMsa::addFirstSeqToPA("Phaneroptera_falcata");
    GTUtilsOptionPanelMsa::addSecondSeqToPA("Isophya_altaica_EF540820");
    //    4. Set terminate gap penalty to 1000. Press align button
    setGapTerm(1000);
    inNewWindow(false);
    align();
    //    Expected state: Isophya_altaica_EF540820 is AAGCTTACT---AA
    GTUtilsMSAEditorSequenceArea::checkSelection(QPoint(0, 1), QPoint(13, 1), "AAGCTTACT---AA");
}

GUI_TEST_CLASS_DEFINITION(pairwise_alignment_test_0007_3) {
    //    1. Open file test/_common_data/scenarios/msa/ma2_gapped.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Open Pairwise alignment option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::PairwiseAlignment);
    //    3. Add Phaneroptera_falcata and Isophya_altaica_EF540820 sequences to PA
    GTUtilsOptionPanelMsa::addFirstSeqToPA("Phaneroptera_falcata");
    GTUtilsOptionPanelMsa::addSecondSeqToPA("Isophya_altaica_EF540820");
    //    4. Set gap open to 10, gap ext to 1, bonus score to 1. Press align button
    setGapOpen(10);
    setGapExtd(1);
    setBonusScore(1);
    inNewWindow(false);
    align();
    //    Expected state: Isophya_altaica_EF540820 is AAG-CTTACT---AA
    GTUtilsMSAEditorSequenceArea::checkSelection(QPoint(0, 1), QPoint(14, 1), "AAG-CTTACT---AA");
}
namespace {
void setOutputPath(const QString& path, const QString& name, bool clickYes = true) {
    expandOutputSettings();
    auto outputFileSelectButton = GTWidget::findWidget("outputFileSelectButton");
    if (clickYes) {
        GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Yes));
    }
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(path, name, GTFileDialogUtils::Save));
    GTWidget::click(outputFileSelectButton);
}
}  // namespace

GUI_TEST_CLASS_DEFINITION(pairwise_alignment_test_0008) {
    const QString fileName = "pairwise_alignment_test_0008.aln";
    //    1. Open file test/_common_data/scenarios/msa/ma2_gapped.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Open Pairwise alignment option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::PairwiseAlignment);
    //    3. Add Phaneroptera_falcata sequence
    GTUtilsOptionPanelMsa::addFirstSeqToPA("Phaneroptera_falcata");
    GTUtilsOptionPanelMsa::addSecondSeqToPA("Isophya_altaica_EF540820");
    //    4. Add Isophya_altaica_EF540820 sequence
    //    5. Select some existing file as output
    QString s = sandBoxDir + fileName;
    QFile f(s);
    bool created = f.open(QFile::ReadWrite);
    CHECK_SET_ERR(created, "file not created");
    f.close();

    setOutputPath(sandBoxDir, fileName);
    align();
    //    Expected state: file is rewritten.
    qint64 size = GTFile::getSize(sandBoxDir + fileName);
    CHECK_SET_ERR(size == 185, QString("unexpected file size %1").arg(size));
    GTUtilsProjectTreeView::doubleClickItem(fileName);
}

GUI_TEST_CLASS_DEFINITION(pairwise_alignment_test_0009) {
    GTLogTracer lt;
    const QString fileName = "pairwise_alignment_test_0009.aln";
    const QString dirName = "pairwise_alignment_test_0009";
    //    1. Open file test/_common_data/scenarios/msa/ma2_gapped.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Open Pairwise alignment option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::PairwiseAlignment);
    //    3. Add Phaneroptera_falcata sequence
    GTUtilsOptionPanelMsa::addFirstSeqToPA("Phaneroptera_falcata");
    GTUtilsOptionPanelMsa::addSecondSeqToPA("Isophya_altaica_EF540820");
    //    4. Add Isophya_altaica_EF540820 sequence
    //    5. Select some existing read-only file as output
    QString dirPath = sandBoxDir + "pairwise_alignment_test_0009";

    QDir().mkpath(dirPath);

    const QString filePath = dirPath + "/" + fileName;
    QFile f(filePath);
    bool created = f.open(QFile::ReadWrite);
    CHECK_SET_ERR(created, "file not created");
    f.close();
    GTFile::setReadOnly(filePath);

    setOutputPath(sandBoxDir + dirName, fileName);
    align();
    //    Expected state: error in log: Task {Pairwise alignment task} finished with error: No permission to write to 'pairwise_alignment_test_0009.aln' file.
    QString error = lt.getJoinedErrorString();
    const QString expectedFilePath = QFileInfo(filePath).absoluteFilePath();
    const QString expected = QString("Task {Pairwise alignment task} finished with error: No permission to write to \'%1\' file.").arg(expectedFilePath);
    CHECK_SET_ERR(error.contains(expected), QString("enexpected error: %1").arg(error));

    GTFile::setReadWrite(filePath);
}

GUI_TEST_CLASS_DEFINITION(pairwise_alignment_test_0010) {
    GTLogTracer lt;
    const QString fileName = "pairwise_alignment_test_0010.aln";
    const QString dirName = "pairwise_alignment_test_0010";
    //    1. Open file test/_common_data/scenarios/msa/ma2_gapped.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Open Pairwise alignment option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::PairwiseAlignment);
    //    3. Add Phaneroptera_falcata sequence
    GTUtilsOptionPanelMsa::addFirstSeqToPA("Phaneroptera_falcata");
    GTUtilsOptionPanelMsa::addSecondSeqToPA("Isophya_altaica_EF540820");
    //    4. Add Isophya_altaica_EF540820 sequence
    //    5. Select some existing read-only file as output
    QString dirPath = sandBoxDir + dirName;
    bool ok = QDir().mkpath(dirPath);
    CHECK_SET_ERR(ok, "subfolder not created");

    GTFile::setReadOnly(dirPath);

    const QString filePath = dirPath + "/" + fileName;

    setOutputPath(dirPath, fileName, false);
    align();
    //    Expected state: error in log: Task {Pairwise alignment task} finished with error: No permission to write to 'COI_transl.aln' file.
    QString error = lt.getJoinedErrorString();
    const QString expectedFilePath = QFileInfo(filePath).absoluteFilePath();
    const QString expected = QString("Task {Pairwise alignment task} finished with error: No permission to write to \'%1\' file.").arg(expectedFilePath);
    CHECK_SET_ERR(error == expected, QString("enexpected error: %1").arg(error));

    GTFile::setReadWrite(dirPath);
}

GUI_TEST_CLASS_DEFINITION(tree_settings_test_0001) {
    //    1. Open data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Open tree settings option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::AddTree);
    //    3. Press "Open tree" button. Select data/samples/CLUSTALW/COI.nwk in file dialog
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(dataDir + "samples/Newick", "COI.nwk"));
    GTWidget::click(GTWidget::findWidget("openTreeButton"));
    //    Expected state: tree opened.
    GTWidget::findWidget("treeView");
}

GUI_TEST_CLASS_DEFINITION(tree_settings_test_0002) {
    //    1. Open data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Open tree settings option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::AddTree);
    //    3. Press "build tree" button.
    GTUtilsDialog::waitForDialog(new BuildTreeDialogFiller("default"));
    GTUtilsMsaEditor::clickBuildTreeButton();
    GTUtilsTaskTreeView::waitTaskFinished();

    //    4. Fill build tree dialog with defaulb values
    //    Expected state: tree built.
    GTWidget::findWidget("treeView");
}

GUI_TEST_CLASS_DEFINITION(tree_settings_test_0003) {
    //    1. Open data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Open tree settings option panel tab. build tree
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::AddTree);
    GTUtilsDialog::waitForDialog(new BuildTreeDialogFiller("default", 0, 0, true));
    GTUtilsMsaEditor::clickBuildTreeButton();
    GTUtilsTaskTreeView::waitTaskFinished();

    // Check/prepare tree widgets.
    auto treeView = GTWidget::findWidget("treeView");
    auto breadthScaleAdjustmentSlider = GTWidget::findWidget("breadthScaleAdjustmentSlider");
    auto layoutCombo = GTWidget::findComboBox("layoutCombo");

    QImage rectImage = GTWidget::getImage(treeView);

    //    3. Select circular layout
    GTComboBox::selectItemByText(layoutCombo, "Circular");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    Expected state: layout changed, breadth slider is disabled.
    QImage circularImage = GTWidget::getImage(treeView);
    CHECK_SET_ERR(rectImage != circularImage, "tree view not changed to circular");
    CHECK_SET_ERR(!breadthScaleAdjustmentSlider->isEnabled(), "breadthScaleAdjustmentSlider in enabled for circular layout");

    //    4. Select unrooted layout
    GTComboBox::selectItemByText(layoutCombo, "Unrooted");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    Expected state: layout changed, breadth slider is disabled.
    QImage unrootedImage = GTWidget::getImage(treeView);
    CHECK_SET_ERR(rectImage != unrootedImage, "tree view not changed to unrooted");
    CHECK_SET_ERR(!breadthScaleAdjustmentSlider->isEnabled(), "breadthScaleAdjustmentSlider in enabled for unrooted layout");

    //    5. Select rectangular layout
    GTComboBox::selectItemByText(layoutCombo, "Rectangular");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state: tree is similar to the beginning, breadth slider is enabled.
    QImage rectImage2 = GTWidget::getImage(treeView);
    CHECK_SET_ERR(rectImage == rectImage2, "final image is not equal to initial");
    CHECK_SET_ERR(breadthScaleAdjustmentSlider->isEnabled(), "breadthScaleAdjustmentSlider in disabled for rectangular layout");
}

GUI_TEST_CLASS_DEFINITION(tree_settings_test_0004) {
    // Compare that 'Default'/'Cladogram'/'Phylogram' are stable within the type and are different between different types.
    // To check that images are stable capture 2 versions of each image.

    // Open data/samples/CLUSTALW/COI.aln.
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsProjectTreeView::toggleView();  // Close project view to make all actions on toolbar available.

    // Open tree settings option panel tab. build tree.
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::AddTree);
    GTUtilsDialog::waitForDialog(new BuildTreeDialogFiller("default", 0, 0, true));
    GTUtilsMsaEditor::clickBuildTreeButton();
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsOptionPanelMsa::closeTab(GTUtilsOptionPanelMsa::TreeOptions);

    QAbstractButton* syncModeButton = GTAction::button("sync_msa_action");
    GTWidget::click(syncModeButton);
    CHECK_SET_ERR(!syncModeButton->isChecked(), "Sync mode must be OFF");

    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::TreeOptions);
    auto treeView = GTWidget::findWidget("treeView");
    auto treeViewCombo = GTWidget::findComboBox("treeViewCombo");

    // Capture 2 variants of  'Phylogram', 'Default', 'Cladogram' images.
    GTComboBox::selectItemByText(treeViewCombo, "Phylogram");
    GTUtilsTaskTreeView::waitTaskFinished();
    QImage phylogramImage1 = GTWidget::getImage(treeView);

    GTComboBox::selectItemByText(treeViewCombo, "Default");
    GTUtilsTaskTreeView::waitTaskFinished();
    QImage defaultImage1 = GTWidget::getImage(treeView);

    GTComboBox::selectItemByText(treeViewCombo, "Cladogram");
    GTUtilsTaskTreeView::waitTaskFinished();
    QImage cladogramImage1 = GTWidget::getImage(treeView);

    GTComboBox::selectItemByText(treeViewCombo, "Phylogram");
    GTUtilsTaskTreeView::waitTaskFinished();
    QImage phylogramImage2 = GTWidget::getImage(treeView);

    GTComboBox::selectItemByText(treeViewCombo, "Cladogram");
    GTUtilsTaskTreeView::waitTaskFinished();
    QImage cladogramImage2 = GTWidget::getImage(treeView);

    GTComboBox::selectItemByText(treeViewCombo, "Default");
    GTUtilsTaskTreeView::waitTaskFinished();
    QImage defaultImage2 = GTWidget::getImage(treeView);

    CHECK_SET_ERR(defaultImage1 == defaultImage2, "Default images are not equal");
    CHECK_SET_ERR(cladogramImage1 == cladogramImage2, "Cladogram images are not equal");
    CHECK_SET_ERR(phylogramImage1 == phylogramImage2, "Phylogram images are not equal");

    CHECK_SET_ERR(defaultImage1 != cladogramImage1, "Default image must not be equal to Cladogram");
    CHECK_SET_ERR(defaultImage1 != phylogramImage1, "Default image must not be equal to Phylogram");
    CHECK_SET_ERR(cladogramImage1 != phylogramImage1, "Cladogram image image must not be equal to Phylogram");
}

GUI_TEST_CLASS_DEFINITION(tree_settings_test_0005) {
    //    1. Open data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Open tree settings option panel tab. build tree
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::AddTree);
    GTUtilsDialog::waitForDialog(new BuildTreeDialogFiller("default", 0, 0, true));
    GTUtilsMsaEditor::clickBuildTreeButton();
    GTUtilsTaskTreeView::waitTaskFinished();

    auto showNamesCheck = GTWidget::findCheckBox("showNamesCheck");
    auto showDistancesCheck = GTWidget::findCheckBox("showDistancesCheck");
    auto alignLabelsCheck = GTWidget::findCheckBox("alignLabelsCheck");
    auto subWindow = GTWidget::findWidget("COI [COI.aln]_SubWindow");
    auto treeView = GTWidget::findGraphicsView("treeView", subWindow);

    QList<QGraphicsSimpleTextItem*> initNames = GTUtilsPhyTree::getVisibleLabels(treeView);
    QList<QGraphicsSimpleTextItem*> initDistances = GTUtilsPhyTree::getVisibleDistances(treeView);

    //    3. Uncheck "show names" checkbox.
    GTCheckBox::setChecked(showNamesCheck, false);

    //    Expected state: names are not shown, align labels checkbox is disabled
    QList<QGraphicsSimpleTextItem*> names = GTUtilsPhyTree::getVisibleLabels(treeView);
    CHECK_SET_ERR(names.isEmpty(), QString("unexpected number of names: %1").arg(names.count()));
    CHECK_SET_ERR(!alignLabelsCheck->isEnabled(), "align labels checkbox is unexpectedly enabled");

    //    4. Check "show names" checkbox.
    GTCheckBox::setChecked(showNamesCheck, true);

    //    Expected state: names are shown, align labels checkbox is enabled
    names = GTUtilsPhyTree::getVisibleLabels(treeView);
    CHECK_SET_ERR(names.count() == initNames.count(), QString("unexpected number of names: %1").arg(names.count()));
    CHECK_SET_ERR(alignLabelsCheck->isEnabled(), "align labels checkbox is unexpectedly disabled");

    //    5. Uncheck "show distances" checkbox.
    GTCheckBox::setChecked(showDistancesCheck, false);

    //    Expected state: distances are not shown
    QList<QGraphicsSimpleTextItem*> distances = GTUtilsPhyTree::getVisibleDistances(treeView);
    CHECK_SET_ERR(distances.isEmpty(), QString("unexpected number of distances: %1").arg(names.count()));

    //    6. Check "show distances" checkbox.
    GTCheckBox::setChecked(showDistancesCheck, true);

    //    Expected state: distances are shown
    distances = GTUtilsPhyTree::getVisibleDistances(treeView);
    CHECK_SET_ERR(distances.count() == initDistances.count(), QString("unexpected number of distances: %1").arg(names.count()));

    //    7. Check "align labels" checkbox.
    // Saving init image
    GTCheckBox::setChecked(alignLabelsCheck, false);
    QImage initImg = GTWidget::getImage(treeView);  // initial state

    GTCheckBox::setChecked(alignLabelsCheck, true);

    //    Expected state: labels are aligned
    QImage alignedImg = GTWidget::getImage(treeView);
    CHECK_SET_ERR(alignedImg != initImg, "labels are not aligned");

    //    8. Uncheck "align labels" checkbox.
    GTCheckBox::setChecked(alignLabelsCheck, false);

    //    Expected state: labels are not aligned
    QImage finalImg = GTWidget::getImage(treeView);
    CHECK_SET_ERR(finalImg == initImg, "tree is aligned");
}

static void setLabelsColor(int r, int g, int b) {
    GTUtilsDialog::waitForDialog(new ColorDialogFiller(r, g, b));
    auto labelsColorButton = GTWidget::findWidget("labelsColorButton");
    GTWidget::click(labelsColorButton);
}

static void checkLabelColor(const QString& expectedColorName) {
    auto graphicsView = GTWidget::findGraphicsView("treeView");
    QList<QGraphicsSimpleTextItem*> labels = GTUtilsPhyTree::getVisibleLabels(graphicsView);
    CHECK_SET_ERR(!labels.isEmpty(), "there are no visiable labels");

    QColor expectedColor(expectedColorName);
    QColor closestColor;  // Used in error message in case if the test fails.
    QImage img = GTWidget::getImage(AppContext::getMainWindow()->getQMainWindow());
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
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Open tree settings option panel tab. build tree.
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::AddTree);
    GTUtilsDialog::waitForDialog(new BuildTreeDialogFiller("default", 0, 0, true));
    GTUtilsMsaEditor::clickBuildTreeButton();

    GTUtilsTaskTreeView::waitTaskFinished();

    // Change labels font.
    auto fontComboBox = GTWidget::findComboBox("fontComboBox");
    auto fontNameEdit = fontComboBox->findChild<QLineEdit*>();
    CHECK_SET_ERR(fontNameEdit != nullptr, "font name edit is not found");
    GTLineEdit::setText(fontNameEdit, "Serif");
    GTKeyboardDriver::keyClick(Qt::Key_Enter);

    // Expected: font changed.
    QGraphicsSimpleTextItem* label = GTUtilsPhyTree::getVisibleLabels().at(0);
    QString family = label->font().family();
    CHECK_SET_ERR(family == "Serif", "unexpected style: " + family);

    // Change labels size.
    auto fontSizeSpinBox = GTWidget::findSpinBox("fontSizeSpinBox");
    auto fontSizeEdit = fontSizeSpinBox->findChild<QLineEdit*>();
    CHECK_SET_ERR(fontSizeEdit != nullptr, "font size edit is not found");
    GTLineEdit::setText(fontSizeEdit, "20");
    GTKeyboardDriver::keyClick(Qt::Key_Enter);

    // Expected: size changed.
    int pointSize = label->font().pointSize();
    CHECK_SET_ERR(pointSize == 20, QString("unexpected point size: %1").arg(pointSize));

    // Check font settings buttons
    auto boldAttrButton = GTWidget::findWidget("boldAttrButton");
    auto italicAttrButton = GTWidget::findWidget("italicAttrButton");
    auto underlineAttrButton = GTWidget::findWidget("underlineAttrButton");

    // Bold.
    GTWidget::click(boldAttrButton);
    CHECK_SET_ERR(label->font().bold(), "expected bold font");

    // Not bold.
    GTWidget::click(boldAttrButton);
    CHECK_SET_ERR(!label->font().bold(), "bold font not canceled");

    // Italic.
    GTWidget::click(italicAttrButton);
    CHECK_SET_ERR(label->font().italic(), "expected italic font");

    // Not italic.
    GTWidget::click(italicAttrButton);
    CHECK_SET_ERR(!label->font().italic(), "italic font not canceled");

    // Underline.
    GTWidget::click(underlineAttrButton);
    CHECK_SET_ERR(label->font().underline(), "expected underline font");

    // Not underline.
    GTWidget::click(underlineAttrButton);
    CHECK_SET_ERR(!label->font().underline(), "underline font not canceled");

    // Change labels color. Run this check last after label size was increased to avoid color-aliasing effects.
    if (!isOsMac()) {
        setLabelsColor(255, 0, 0);
        // Expected: color changed
        checkLabelColor("#ff0000");
    }
}

GUI_TEST_CLASS_DEFINITION(tree_settings_test_0007) {
    // Open data/samples/CLUSTALW/COI.aln.
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    GTUtilsProjectTreeView::toggleView();  // Close opened project tree view to make all icons on the toolbar visible with no overflow.

    // Open tree settings option panel tab. Build a tree.
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::AddTree);

    GTUtilsDialog::waitForDialog(new BuildTreeDialogFiller("default", 0, 0, true));
    GTUtilsMsaEditor::clickBuildTreeButton();
    GTUtilsTaskTreeView::waitTaskFinished();

    // Disable sync mode to allow resize of the view.
    GTUtilsOptionPanelMsa::closeTab(GTUtilsOptionPanelMsa::TreeOptions);

    QAbstractButton* syncModeButton = GTAction::button("sync_msa_action");
    CHECK_SET_ERR(syncModeButton->isChecked(), "Sync mode must be ON");

    GTWidget::click(syncModeButton);
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(!syncModeButton->isChecked(), "Sync mode must be OFF");

    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::TreeOptions);

    auto treeView = GTWidget::findGraphicsView("treeView");
    QGraphicsScene* scene = treeView->scene();

    // Decrease breadthScaleAdjustmentSlider value.
    double initialHeight = scene->height();
    auto breadthScaleAdjustmentSlider = GTWidget::findSlider("breadthScaleAdjustmentSlider");
    GTSlider::setValue(breadthScaleAdjustmentSlider, 50);

    // Expected state: the tree breadth (height) was reduced.
    double reducedHeight = scene->height();
    CHECK_SET_ERR(reducedHeight < initialHeight, QString("Scene height is not reduced! Initial: %1, final: %2").arg(initialHeight).arg(reducedHeight));

    // Increase breadthScaleAdjustmentSlider value.
    GTSlider::setValue(breadthScaleAdjustmentSlider, 200);

    // Expected state: the tree breadth (height) was increased.
    double increasedHeight = scene->height();
    CHECK_SET_ERR(increasedHeight > initialHeight, QString("Height is not increased! Initial: %1, final: %2").arg(initialHeight).arg(increasedHeight));
}

GUI_TEST_CLASS_DEFINITION(tree_settings_test_0008) {
    //    1. Open data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Open tree settings option panel tab. build tree
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::AddTree);
    GTUtilsDialog::waitForDialog(new BuildTreeDialogFiller("default", 0, 0, true));
    GTUtilsMsaEditor::clickBuildTreeButton();
    GTThread::waitForMainThread();

    GTUtilsPhyTree::setBranchColor(255, 0, 0);

    // Expected state: color changed
    QString colorName = "#ff0000";
    auto treeView = GTWidget::findGraphicsView("treeView");
    double initPercent = GTUtilsPhyTree::getColorPercent(treeView, colorName);
    CHECK_SET_ERR(initPercent > 0, "color not changed");

    // Change  line Weight
    auto lineWeightSpinBox = GTWidget::findSpinBox("lineWeightSpinBox");
    GTSpinBox::setValue(lineWeightSpinBox, 30, GTGlobals::UseKeyBoard);
    double finalPercent = GTUtilsPhyTree::getColorPercent(treeView, colorName);
    CHECK_SET_ERR(finalPercent > initPercent * 10, "branches width changed not enough");
}

namespace {
void setConsensusOutputPath(const QString& path) {
    auto pathLe = GTWidget::findLineEdit("pathLe");
    GTLineEdit::setText(pathLe, path);
}
}  // namespace

GUI_TEST_CLASS_DEFINITION(export_consensus_test_0001) {
    const QString fileName = "export_consensus_test_0001.txt";
    //    1. Open data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Open export consensus option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::ExportConsensus);
    //    3. Select some existing file as output
    QString s = sandBoxDir + fileName;
    QFile f(s);
    bool created = f.open(QFile::ReadWrite);
    CHECK_SET_ERR(created, "file not created");
    f.close();

    setConsensusOutputPath(sandBoxDir + fileName);
    //    4. Press export button
    GTWidget::click(GTWidget::findWidget("exportBtn"));
    GTUtilsTaskTreeView::waitTaskFinished();
    //    Expected state: file rewrited
    qint64 size = GTFile::getSize(sandBoxDir + fileName);
    CHECK_SET_ERR(size == 604, QString("unexpected file size %1").arg(size));
    GTUtilsProjectTreeView::doubleClickItem(fileName);
}

GUI_TEST_CLASS_DEFINITION(export_consensus_test_0002) {
    GTLogTracer lt;
    const QString fileName = "export_consensus_test_0002.aln";
    //    1. Open data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Open export consensus option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::ExportConsensus);
    //    3. Select existing read-only file "export_consensus_test_0002.aln" as output

    const QString dirPath = sandBoxDir + "export_consensus_test_0002";
    QDir().mkpath(dirPath);

    const QString filePath = dirPath + "/" + fileName;
    QFile f(filePath);
    bool created = f.open(QFile::ReadWrite);
    CHECK_SET_ERR(created, "file not created");
    f.close();

    GTFile::setReadOnly(filePath);

    setConsensusOutputPath(filePath);
    //    4. Press export button
    GTWidget::click(GTWidget::findWidget("exportBtn"));
    //    Expected state: error in log: Task {Save document} finished with error: No permission to write to 'COI_transl.aln' file.
    QString error = lt.getJoinedErrorString();
    const QString expectedFilePath = QFileInfo(filePath).absoluteFilePath();
    QString expected = QString("Task {Export consensus} finished with error: Subtask {Save document} is failed: No permission to write to \'%1\' file.").arg(expectedFilePath);
    CHECK_SET_ERR(error.contains(expected), QString("Unexpected error: %1").arg(error));

    GTFile::setReadWrite(filePath);
}

GUI_TEST_CLASS_DEFINITION(export_consensus_test_0003) {
    GTLogTracer lt;
    const QString fileName = "export_consensus_test_0003.aln";
    const QString dirName = "export_consensus_test_0003";
    //    1. Open data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    //    2. Open export consensus option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::ExportConsensus);

    //    3. Select some existing file in read-only directory as output
    QString dirPath = sandBoxDir + dirName;
    bool ok = QDir().mkpath(dirPath);
    CHECK_SET_ERR(ok, "subfolder not created");
    GTFile::setReadOnly(dirPath);

    QString filePath = dirPath + '/' + fileName;
    setConsensusOutputPath(filePath);

    //    4. Press export button
    GTWidget::click(GTWidget::findWidget("exportBtn"));
    GTThread::waitForMainThread();

    //    Expected state: notification is shown that folder is read-only.
    QString error = lt.getJoinedErrorString();
    QString expected = QString("Task {Export consensus} finished with error: Folder is read-only: %1").arg(QFileInfo(filePath).absolutePath());
    CHECK_SET_ERR(error == expected, QString("Unexpected error: '%1', expected: '%2'").arg(error).arg(expected));
}

GUI_TEST_CLASS_DEFINITION(export_consensus_test_0004) {
    // 0. Change Documents folder to sandbox
    class Custom : public CustomScenario {
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            AppSettingsDialogFiller::setDocumentsDirPath(sandBoxDir);
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(new AppSettingsDialogFiller(new Custom()));
    GTMenu::clickMainMenuItem({"Settings", "Preferences..."}, GTGlobals::UseMouse);
    GTUtilsTaskTreeView::waitTaskFinished();

    //    1. Open data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Open export consensus option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::ExportConsensus);
    //    3. Set empty path
    setConsensusOutputPath("");

    class exportConsensusTest0004Filler : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };
    GTWidget::click(GTWidget::findWidget("exportBtn"));
    GTUtilsTaskTreeView::waitTaskFinished();

    auto pathLe = GTWidget::findLineEdit("pathLe");
    QString pathLeText = pathLe->text();
    CHECK_SET_ERR(!pathLeText.isEmpty() && pathLeText.contains("COI_consensus_1.txt"), "wrong lineEdit text: " + pathLeText);
}

GUI_TEST_CLASS_DEFINITION(statistics_test_0001) {
    //    1. Open data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Open export statistics option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Statistics);
    //    3. check showDistancesColumn checkbox
    auto showDistancesColumnCheck = GTWidget::findCheckBox("showDistancesColumnCheck");
    GTCheckBox::setChecked(showDistancesColumnCheck, true);
    //    4. Check reference hint
    auto refSeqWarning = GTWidget::findLabel("refSeqWarning");
    CHECK_SET_ERR(refSeqWarning->text() == "Hint: select a reference above", QString("Unexpected hint: %1").arg(refSeqWarning->text()));
    //    5. Add Phaneroptera_falcata as reference
    GTUtilsOptionPanelMsa::addReference("Phaneroptera_falcata");
    //    Expected state: similarity column appaered
    QString s0 = GTUtilsMSAEditorSequenceArea::getSimilarityValue(0);
    CHECK_SET_ERR(s0 == "0%", QString("Unexpected similarity at line 1: %1").arg(s0));
    QString s1 = GTUtilsMSAEditorSequenceArea::getSimilarityValue(1);
    CHECK_SET_ERR(s1 == "18%", QString("Unexpected similarity at line 2: %1").arg(s1));

    //    6. Check counts mode
    auto countsButton = GTWidget::findRadioButton("countsButton");
    GTRadioButton::click(countsButton);

    s0 = GTUtilsMSAEditorSequenceArea::getSimilarityValue(0);
    CHECK_SET_ERR(s0 == "0", QString("Unexpected similarity at line 1: %1").arg(s0));
    s1 = GTUtilsMSAEditorSequenceArea::getSimilarityValue(1);
    CHECK_SET_ERR(s1 == "106", QString("Unexpected similarity at line 2: %1").arg(s1));
    //    7. Check exclude gabs mode
    auto excludeGapsCheckBox = GTWidget::findCheckBox("excludeGapsCheckBox");
    GTCheckBox::setChecked(excludeGapsCheckBox, true);

    s0 = GTUtilsMSAEditorSequenceArea::getSimilarityValue(7);
    CHECK_SET_ERR(s0 == "110", QString("Unexpected similarity at line 8: %1").arg(s0));
    s1 = GTUtilsMSAEditorSequenceArea::getSimilarityValue(8);
    CHECK_SET_ERR(s1 == "100", QString("Unexpected similarity at line 9: %1").arg(s1));
}

GUI_TEST_CLASS_DEFINITION(statistics_test_0002) {
    //    1. Open data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Open export statistics option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Statistics);
    //    3. check showDistancesColumn checkbox
    auto showDistancesColumnCheck = GTWidget::findCheckBox("showDistancesColumnCheck");
    GTCheckBox::setChecked(showDistancesColumnCheck, true);
    //    4. Add Phaneroptera_falcata as reference
    GTUtilsOptionPanelMsa::addReference("Phaneroptera_falcata");
    //    5. Check identity algorithm
    auto algoComboBox = GTWidget::findComboBox("algoComboBox");
    GTComboBox::selectItemByText(algoComboBox, "Similarity");
    /*
    QString s0 = GTUtilsMSAEditorSequenceArea::getSimilarityValue(0);
    CHECK_SET_ERR(s0 == "100%", QString("Unexpected similarity at line 1: %1").arg(s0));
    QString s1 = GTUtilsMSAEditorSequenceArea::getSimilarityValue(1);
    CHECK_SET_ERR(s1 == "82%", QString("Unexpected similarity at line 2: %1").arg(s1));
//    6. Check counts mode
    QRadioButton* countsButton = GTWidget::findRadioButton("countsButton");
    GTRadioButton::click(countsButton);

    s0 = GTUtilsMSAEditorSequenceArea::getSimilarityValue(0);
    CHECK_SET_ERR(s0 == "604", QString("Unexpected similarity at line 1: %1").arg(s0));
    s1 = GTUtilsMSAEditorSequenceArea::getSimilarityValue(1);
    CHECK_SET_ERR(s1 == "498", QString("Unexpected similarity at line 2: %1").arg(s1));
    */
}
GUI_TEST_CLASS_DEFINITION(statistics_test_0003) {
    // 1. Open data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    // 2. Open export statistics option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Statistics);
    // 3. check showDistancesColumn checkbox
    auto showDistancesColumnCheck = GTWidget::findCheckBox("showDistancesColumnCheck");
    GTCheckBox::setChecked(showDistancesColumnCheck, true);
    // 4. Add Phaneroptera_falcata as reference
    GTUtilsOptionPanelMsa::addReference("Phaneroptera_falcata");
    // 5. delete symbol at point (0,0)
    GTWidget::click(GTUtilsMdi::activeWindow());
    GTUtilsMSAEditorSequenceArea::click(QPoint(0, 0));
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    // Expected state: similarity changed, updateButton is disabled.
    QString s0 = GTUtilsMSAEditorSequenceArea::getSimilarityValue(0);
    CHECK_SET_ERR(s0 == "0%", QString("(1)Unexpected similarity at line 1: %1").arg(s0));
    QString s1 = GTUtilsMSAEditorSequenceArea::getSimilarityValue(1);
    CHECK_SET_ERR(s1 == "69%", QString("(1)Unexpected similarity at line 2: %1").arg(s1));

    auto updateButton = GTWidget::findWidget("updateButton");
    CHECK_SET_ERR(!updateButton->isEnabled(), "updateButton is unexpectedly enabled");
    // 6. Undo. Uncheck automatic update checkbox
    GTWidget::click(GTAction::button("msa_action_undo"));
    auto autoUpdateCheck = GTWidget::findCheckBox("autoUpdateCheck");
    GTCheckBox::setChecked(autoUpdateCheck, false);
    // 5. Delete symbol at point (0,0)
    GTWidget::click(GTUtilsMdi::activeWindow());
    GTUtilsMSAEditorSequenceArea::click(QPoint(0, 0));
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    // Expected state: similarity not changed.
    s0 = GTUtilsMSAEditorSequenceArea::getSimilarityValue(0);
    CHECK_SET_ERR(s0 == "0%", QString("(2)Unexpected similarity at line 1: %1").arg(s0));
    s1 = GTUtilsMSAEditorSequenceArea::getSimilarityValue(1);
    CHECK_SET_ERR(s1 == "18%", QString("(2)Unexpected similarity at line 2: %1").arg(s1));
    // 6. Press autoUpdate button.
    GTWidget::click(updateButton);
    // Expected state: similarity is updated.
    s0 = GTUtilsMSAEditorSequenceArea::getSimilarityValue(0);
    CHECK_SET_ERR(s0 == "0%", QString("(3)Unexpected similarity at line 1: %1").arg(s0));
    s1 = GTUtilsMSAEditorSequenceArea::getSimilarityValue(1);
    CHECK_SET_ERR(s1 == "69%", QString("(3)Unexpected similarity at line 2: %1").arg(s1));
}

GUI_TEST_CLASS_DEFINITION(save_parameters_test_0001) {
    //    1. Open data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Open export general option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::General);
    //   Check saving parameters
    // find widgets
    auto consensusType = GTWidget::findComboBox("consensusType");
    auto thresholdSpinBox = GTWidget::findSpinBox("thresholdSpinBox");

    // set some parameters
    GTUtilsOptionPanelMsa::addReference("Phaneroptera_falcata");
    GTComboBox::selectItemByText(consensusType, "Strict");
    GTSpinBox::setValue(thresholdSpinBox, 50, GTGlobals::UseKeyBoard);

    // close and open option panel
    GTWidget::click(GTWidget::findWidget("OP_MSA_GENERAL"));
    GTWidget::click(GTWidget::findWidget("OP_MSA_GENERAL"));

    // checks
    auto sequenceLineEdit = GTWidget::findLineEdit("sequenceLineEdit");
    consensusType = GTWidget::findComboBox("consensusType");
    thresholdSpinBox = GTWidget::findSpinBox("thresholdSpinBox");

    CHECK_SET_ERR(sequenceLineEdit->text() == "Phaneroptera_falcata", QString("unexpected reference: %1").arg(sequenceLineEdit->text()));
    CHECK_SET_ERR(consensusType->currentText() == "Strict", QString("unexpected consensus: %1").arg(consensusType->currentText()));
    CHECK_SET_ERR(thresholdSpinBox->value() == 50, QString("unexpected threshold value: %1").arg(thresholdSpinBox->value()));
}

GUI_TEST_CLASS_DEFINITION(save_parameters_test_0002) {
    //    1. Open data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Open export highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Highlighting);

    //   Check saving parameters
    // find widgets
    auto colorScheme = GTWidget::findComboBox("colorScheme");
    auto highlightingScheme = GTWidget::findComboBox("highlightingScheme");
    auto useDots = GTWidget::findCheckBox("useDots");

    // set some parameters
    GTUtilsOptionPanelMsa::addReference("Phaneroptera_falcata");
    GTComboBox::selectItemByText(colorScheme, "Jalview");
    GTComboBox::selectItemByText(highlightingScheme, "Agreements");
    GTCheckBox::setChecked(useDots, true);

    // close and open option panel
    GTWidget::click(GTWidget::findWidget("OP_MSA_HIGHLIGHTING"));
    GTWidget::click(GTWidget::findWidget("OP_MSA_HIGHLIGHTING"));

    // checks
    auto sequenceLineEdit = GTWidget::findLineEdit("sequenceLineEdit");
    colorScheme = GTWidget::findComboBox("colorScheme");
    highlightingScheme = GTWidget::findComboBox("highlightingScheme");
    useDots = GTWidget::findCheckBox("useDots");

    CHECK_SET_ERR(sequenceLineEdit->text() == "Phaneroptera_falcata", QString("unexpected reference: %1").arg(sequenceLineEdit->text()));
    CHECK_SET_ERR(colorScheme->currentText() == "Jalview", QString("uexpected color scheme: %1").arg(colorScheme->currentText()));
    CHECK_SET_ERR(highlightingScheme->currentText() == "Agreements", QString("uexpected highlighting: %1").arg(highlightingScheme->currentText()));
    CHECK_SET_ERR(useDots->isChecked(), "Use dots checkbox is not checked");
}

GUI_TEST_CLASS_DEFINITION(save_parameters_test_0003) {
    // KAlign settings
    //     1. Open data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Open export highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::PairwiseAlignment);

    //   Check saving parameters
    GTUtilsOptionPanelMsa::addFirstSeqToPA("Phaneroptera_falcata");
    GTUtilsOptionPanelMsa::addSecondSeqToPA("Isophya_altaica_EF540820");

    // expand settings
    GTWidget::click(GTWidget::findWidget("ArrowHeader_Algorithm settings"));
    GTWidget::click(GTWidget::findWidget("ArrowHeader_Output settings"));

    // find widgets
    auto gapOpen = GTWidget::findDoubleSpinBox("gapOpen");
    auto gapExtd = GTWidget::findDoubleSpinBox("gapExtd");
    auto gapTerm = GTWidget::findDoubleSpinBox("gapTerm");
    auto bonusScore = GTWidget::findDoubleSpinBox("bonusScore");
    auto inNewWindowCheckBox = GTWidget::findCheckBox("inNewWindowCheckBox");

    // set values
    GTDoubleSpinbox::setValue(gapOpen, 100, GTGlobals::UseKeyBoard);
    GTDoubleSpinbox::setValue(gapExtd, 100, GTGlobals::UseKeyBoard);
    GTDoubleSpinbox::setValue(gapTerm, 100, GTGlobals::UseKeyBoard);
    GTDoubleSpinbox::setValue(bonusScore, 100, GTGlobals::UseKeyBoard);
    GTCheckBox::setChecked(inNewWindowCheckBox, false);

    // close and open option panel
    GTWidget::click(GTWidget::findWidget("OP_PAIRALIGN"));
    GTWidget::click(GTWidget::findWidget("OP_PAIRALIGN"));

    // checks
    QLineEdit* l1 = GTUtilsOptionPanelMsa::getSeqLineEdit(1);
    QLineEdit* l2 = GTUtilsOptionPanelMsa::getSeqLineEdit(2);
    gapOpen = GTWidget::findDoubleSpinBox("gapOpen");
    gapExtd = GTWidget::findDoubleSpinBox("gapExtd");
    gapTerm = GTWidget::findDoubleSpinBox("gapTerm");
    bonusScore = GTWidget::findDoubleSpinBox("bonusScore");
    inNewWindowCheckBox = GTWidget::findCheckBox("inNewWindowCheckBox");

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
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Open export highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::PairwiseAlignment);

    //   Check saving parameters
    GTUtilsOptionPanelMsa::addFirstSeqToPA("Phaneroptera_falcata");
    GTUtilsOptionPanelMsa::addSecondSeqToPA("Isophya_altaica_EF540820");

    // expand settings
    GTWidget::click(GTWidget::findWidget("ArrowHeader_Algorithm settings"));

    // find widgets
    auto algorithmListComboBox = GTWidget::findComboBox("algorithmListComboBox");
    GTComboBox::selectItemByText(algorithmListComboBox, "Smith-Waterman");

    auto algorithmVersion = GTWidget::findComboBox("algorithmVersion");
    auto scoringMatrix = GTWidget::findComboBox("scoringMatrix");
    auto gapOpen = GTWidget::findSpinBox("gapOpen");
    auto gapExtd = GTWidget::findSpinBox("gapExtd");

    // setValues
    GTComboBox::selectItemByText(algorithmVersion, "SW_classic");
    GTComboBox::selectItemByText(scoringMatrix, "dna");
    GTSpinBox::setValue(gapOpen, 5);
    GTSpinBox::setValue(gapExtd, 5);

    // close and open option panel
    GTWidget::click(GTWidget::findWidget("OP_PAIRALIGN"));
    GTWidget::click(GTWidget::findWidget("OP_PAIRALIGN"));

    // checks
    QLineEdit* l1 = GTUtilsOptionPanelMsa::getSeqLineEdit(1);
    QLineEdit* l2 = GTUtilsOptionPanelMsa::getSeqLineEdit(2);
    algorithmListComboBox = GTWidget::findComboBox("algorithmListComboBox");
    algorithmVersion = GTWidget::findComboBox("algorithmVersion");
    scoringMatrix = GTWidget::findComboBox("scoringMatrix");
    gapOpen = GTWidget::findSpinBox("gapOpen");
    gapExtd = GTWidget::findSpinBox("gapExtd");

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
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Open tree settings option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::AddTree);
    //    3. Press "build tree" button.
    GTUtilsDialog::waitForDialog(new BuildTreeDialogFiller("default", 0, 0, true));
    GTUtilsMsaEditor::clickBuildTreeButton();
    GTUtilsTaskTreeView::waitTaskFinished();

    // set some values
    auto layoutCombo = GTWidget::findComboBox("layoutCombo");
    GTComboBox::selectItemByText(layoutCombo, "Circular");

    auto treeViewCombo = GTWidget::findComboBox("treeViewCombo");
    GTComboBox::selectItemByText(treeViewCombo, "Cladogram");
    setLabelsColor(255, 255, 255);
    QString initialColor = GTWidget::getColor(GTWidget::findWidget("labelsColorButton"), QPoint(10, 10)).name();
    auto fontComboBox = GTWidget::findComboBox("fontComboBox");
    auto l = fontComboBox->findChild<QLineEdit*>();
    QString fontName = isOsLinux() ? "Serif" : "Tahoma";
    GTLineEdit::setText(l, fontName);
    GTKeyboardDriver::keyClick(Qt::Key_Enter);

    auto fontSizeSpinBox = GTWidget::findSpinBox("fontSizeSpinBox");
    GTSpinBox::setValue(fontSizeSpinBox, 14);

    GTWidget::click(GTWidget::findWidget("boldAttrButton"));
    GTWidget::click(GTWidget::findWidget("italicAttrButton"));
    GTWidget::click(GTWidget::findWidget("underlineAttrButton"));

    // close and open option panel
    GTWidget::click(GTWidget::findWidget("OP_MSA_TREES_WIDGET"));
    GTWidget::click(GTWidget::findWidget("OP_MSA_TREES_WIDGET"));

    // check settings
    layoutCombo = GTWidget::findComboBox("layoutCombo");
    treeViewCombo = GTWidget::findComboBox("treeViewCombo");
    auto labelsColorButton = GTWidget::findWidget("labelsColorButton");
    fontComboBox = GTWidget::findComboBox("fontComboBox");
    fontSizeSpinBox = GTWidget::findSpinBox("fontSizeSpinBox");
    auto boldAttrButton = GTWidget::findToolButton("boldAttrButton");
    auto italicAttrButton = GTWidget::findToolButton("italicAttrButton");
    auto underlineAttrButton = GTWidget::findToolButton("underlineAttrButton");

    CHECK_SET_ERR(layoutCombo->currentText() == "Circular", QString("unexpected layout: %1").arg(layoutCombo->currentText()));
    CHECK_SET_ERR(treeViewCombo->currentText() == "Cladogram", QString("unexpected tree view: %1").arg(treeViewCombo->currentText()));
    CHECK_SET_ERR(GTWidget::hasPixelWithColor(labelsColorButton, initialColor), QString("Initial color is not found: %1").arg(initialColor));
    CHECK_SET_ERR(fontComboBox->currentText().contains(fontName), QString("unexpected font: %1").arg(fontComboBox->currentText()));
    CHECK_SET_ERR(fontSizeSpinBox->value() == 14, QString("unexpected font size: %1").arg(fontSizeSpinBox->value()));
    CHECK_SET_ERR(boldAttrButton->isChecked(), "boldAttrButton is not checked");
    CHECK_SET_ERR(italicAttrButton->isChecked(), "italicAttrButton is not checked");
    CHECK_SET_ERR(underlineAttrButton->isChecked(), "underlineAttrButton is not checked");
}

GUI_TEST_CLASS_DEFINITION(save_parameters_test_0004_1) {
    //    1. Open data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Open tree settings option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::AddTree);
    //    3. Press "build tree" button.
    GTUtilsDialog::waitForDialog(new BuildTreeDialogFiller("default", 0, 0, true));
    GTUtilsMsaEditor::clickBuildTreeButton();
    GTUtilsTaskTreeView::waitTaskFinished();

    // find widgets
    auto showNamesCheck = GTWidget::findCheckBox("showNamesCheck");
    auto showDistancesCheck = GTWidget::findCheckBox("showDistancesCheck");
    auto breadthScaleAdjustmentSlider = GTWidget::findSlider("breadthScaleAdjustmentSlider");

    auto lineWeightSpinBox = GTWidget::findSpinBox("lineWeightSpinBox");

    // set some values
    GTCheckBox::setChecked(showNamesCheck, false);
    GTCheckBox::setChecked(showDistancesCheck, false);
    GTSlider::setValue(breadthScaleAdjustmentSlider, 50);
    GTUtilsPhyTree::setBranchColor(255, 255, 255);
    QString initialColor = GTWidget::getColor(GTWidget::findWidget("branchesColorButton"), QPoint(10, 10)).name();
    GTSpinBox::setValue(lineWeightSpinBox, 2);

    // close and open option panel
    GTWidget::click(GTWidget::findWidget("OP_MSA_TREES_WIDGET"));
    GTWidget::click(GTWidget::findWidget("OP_MSA_TREES_WIDGET"));

    // checks
    showNamesCheck = GTWidget::findCheckBox("showNamesCheck");
    showDistancesCheck = GTWidget::findCheckBox("showDistancesCheck");
    breadthScaleAdjustmentSlider = GTWidget::findSlider("breadthScaleAdjustmentSlider");
    lineWeightSpinBox = GTWidget::findSpinBox("lineWeightSpinBox");
    auto branchesColorButton = GTWidget::findWidget("branchesColorButton");

    CHECK_SET_ERR(!showNamesCheck->isChecked(), "show names checkbox is unexpectedly checked");
    CHECK_SET_ERR(!showDistancesCheck->isChecked(), "show distances checkbox is unexpectedly checked");
    CHECK_SET_ERR(breadthScaleAdjustmentSlider->value() == 50, QString("unexpected breadthScaleAdjustmentSlider value: %1").arg(breadthScaleAdjustmentSlider->value()));
    CHECK_SET_ERR(lineWeightSpinBox->value() == 2, QString("unexpected line width: %1").arg(lineWeightSpinBox->value()));
    CHECK_SET_ERR(GTWidget::hasPixelWithColor(branchesColorButton, initialColor), QString("Initial color is not found: %1").arg(initialColor));
}

GUI_TEST_CLASS_DEFINITION(save_parameters_test_0005) {
    //    1. Open data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2.  Set strict consensus
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::General);
    auto consensusType = GTWidget::findComboBox("consensusType");
    GTComboBox::selectItemByText(consensusType, "Strict");
    //    3. Open export consensus tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::ExportConsensus);

    // find widgets
    auto pathLe = GTWidget::findLineEdit("pathLe");
    auto formatCb = GTWidget::findComboBox("formatCb");
    auto keepGapsChb = GTWidget::findCheckBox("keepGapsChb");

    // set some values
    GTLineEdit::setText(pathLe, "some_path");
    GTComboBox::selectItemByText(formatCb, "GenBank");
    GTCheckBox::setChecked(keepGapsChb, true);

    // close and open option panel
    GTWidget::click(GTWidget::findWidget("OP_EXPORT_CONSENSUS"));
    GTWidget::click(GTWidget::findWidget("OP_EXPORT_CONSENSUS"));

    // checks
    pathLe = GTWidget::findLineEdit("pathLe");
    formatCb = GTWidget::findComboBox("formatCb");
    keepGapsChb = GTWidget::findCheckBox("keepGapsChb");

    CHECK_SET_ERR(pathLe->text().contains("some_path"), QString("unexpected path: %1").arg(pathLe->text()));
    CHECK_SET_ERR(formatCb->currentText() == "GenBank", QString("unexpected format: %1").arg(formatCb->currentText()));
    CHECK_SET_ERR(keepGapsChb->isChecked(), "keep gaps checkBox is unexpectedly unchecked");
}

GUI_TEST_CLASS_DEFINITION(save_parameters_test_0006) {
    //    1. Open data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2.  Set strict consensus
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Statistics);
    GTUtilsOptionPanelMsa::addReference("Phaneroptera_falcata");

    // find widgets
    auto showDistancesColumnCheck = GTWidget::findCheckBox("showDistancesColumnCheck");
    auto algoComboBox = GTWidget::findComboBox("algoComboBox");
    auto countsButton = GTWidget::findRadioButton("countsButton");
    auto excludeGapsCheckBox = GTWidget::findCheckBox("excludeGapsCheckBox");
    auto autoUpdateCheck = GTWidget::findCheckBox("autoUpdateCheck");

    // set some parameters
    GTCheckBox::setChecked(showDistancesColumnCheck, true);
    GTComboBox::selectItemByText(algoComboBox, "Similarity");
    GTRadioButton::click(countsButton);
    GTCheckBox::setChecked(excludeGapsCheckBox, true);
    GTCheckBox::setChecked(autoUpdateCheck, false);

    // close and open option panel
    GTWidget::click(GTWidget::findWidget("OP_SEQ_STATISTICS_WIDGET"));
    GTWidget::click(GTWidget::findWidget("OP_SEQ_STATISTICS_WIDGET"));

    // checks
    showDistancesColumnCheck = GTWidget::findCheckBox("showDistancesColumnCheck");
    algoComboBox = GTWidget::findComboBox("algoComboBox");
    countsButton = GTWidget::findRadioButton("countsButton");
    excludeGapsCheckBox = GTWidget::findCheckBox("excludeGapsCheckBox");
    autoUpdateCheck = GTWidget::findCheckBox("autoUpdateCheck");

    CHECK_SET_ERR(showDistancesColumnCheck->isChecked(), "show distances is unexpectedly unchecked");
    CHECK_SET_ERR(algoComboBox->currentText() == "Similarity", QString("unexpected algorithm: %1").arg(algoComboBox->currentText()));
    CHECK_SET_ERR(countsButton->isChecked(), "counts radio is not checked");
    CHECK_SET_ERR(excludeGapsCheckBox->isChecked(), "exclude gaps not checked");
    CHECK_SET_ERR(!autoUpdateCheck->isChecked(), "auto update is unexpectedly checked");
}
}  // namespace GUITest_common_scenarios_options_panel_MSA
}  // namespace U2
