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
#include <base_dialogs/MessageBoxFiller.h>
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTCheckBox.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTListWidget.h>
#include <primitives/GTMenu.h>
#include <primitives/GTRadioButton.h>
#include <primitives/GTTabWidget.h>
#include <primitives/GTTextEdit.h>
#include <primitives/GTToolbar.h>
#include <primitives/GTWidget.h>
#include <primitives/PopupChooser.h>
#include <utils/GTThread.h>
#include <utils/GTUtilsDialog.h>

#include <QAbstractItemModel>
#include <QApplication>
#include <QDir>
#include <QListWidget>
#include <QTreeView>

#include <U2Core/UserApplicationsSettings.h>

#include "GTTestsCreateAnnotationWidget.h"
#include "GTUtilsAnnotationsTreeView.h"
#include "GTUtilsMdi.h"
#include "GTUtilsOptionPanelSequenceView.h"
#include "GTUtilsProject.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsTaskTreeView.h"
#include "runnables/qt/EscapeClicker.h"
#include "runnables/ugene/corelibs/U2Gui/CreateAnnotationWidgetFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ProjectTreeItemSelectorDialogFiller.h"
#include "runnables/ugene/corelibs/U2View/utils_smith_waterman/SmithWatermanDialogBaseFiller.h"
#include "runnables/ugene/plugins/orf_marker/OrfDialogFiller.h"
#include "runnables/ugene/ugeneui/SequenceReadingModeSelectorDialogFiller.h"

namespace U2 {

namespace GUITest_common_scenarios_create_annotation_widget {
using namespace HI;

namespace {

void openFileAndCallCreateAnnotationDialog(HI::GUITestOpStatus& os, const QString& filePath) {
    GTFileDialog::openFile(os, filePath);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "New annotation");
}

void openFileAndCallSmithWatermanDialog(HI::GUITestOpStatus& os, const QString& filePath) {
    GTFileDialog::openFile(os, filePath);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Find pattern [Smith-Waterman]");
}

void setSmithWatermanPatternAndOpenLastTab(HI::GUITestOpStatus& os, QWidget* dialog, const QString& pattern = "AGGAAAAAATGCTAAGGGCAGC") {
    GTTextEdit::setText(os, GTWidget::findTextEdit(os, "teditPattern", dialog), pattern);
    GTTabWidget::setCurrentIndex(os, GTWidget::findTabWidget(os, "tabWidget", dialog), 1);
}

void openFileOpenSearchTabAndSetPattern(HI::GUITestOpStatus& os, const QString& filePath, const QString& pattern = "GCCCATCAGACTAACAG") {
    GTFileDialog::openFile(os, filePath);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::Search);
    GTTextEdit::setText(os, GTWidget::findTextEdit(os, "textPattern"), pattern);
    GTUtilsTaskTreeView::waitTaskFinished(os);
}

QString getTypeFromFullWidget(HI::GUITestOpStatus& os, QWidget* dialog) {
    auto lwAnnotationType = GTWidget::findListWidget(os, "lwAnnotationType", dialog);
    return lwAnnotationType->currentItem()->text();
}

void setTypeInFullWidget(HI::GUITestOpStatus& os, const QString& type, QWidget* dialog) {
    GTListWidget::click(os, GTWidget::findListWidget(os, "lwAnnotationType", dialog), type);
}

bool checkTypePresenceInFullWidget(HI::GUITestOpStatus& os, const QString& type, QWidget* dialog) {
    return GTListWidget::getItems(os, GTWidget::findListWidget(os, "lwAnnotationType", dialog)).contains(type);
}

QString getTypeFromNormalWidget(HI::GUITestOpStatus& os, QWidget* dialog) {
    auto cbAnnotationType = GTWidget::findComboBox(os, "cbAnnotationType", dialog);
    return cbAnnotationType->currentText();
}

void setTypeInNormalWidget(HI::GUITestOpStatus& os, const QString& type, QWidget* dialog) {
    GTComboBox::selectItemByText(os, GTWidget::findComboBox(os, "cbAnnotationType", dialog), type, GTGlobals::UseMouse);
}

bool checkTypePresenceInNormalWidget(HI::GUITestOpStatus& os, const QString& type, QWidget* dialog) {
    auto cbAnnotationType = GTWidget::findComboBox(os, "cbAnnotationType", dialog);
    for (int i = 0; i < cbAnnotationType->count(); i++) {
        if (type == cbAnnotationType->itemText(i)) {
            return true;
        }
    }
    return false;
}

QString getTypeFromOptionsPanelWidget(HI::GUITestOpStatus& os) {
    GTUtilsOptionPanelSequenceView::openAnnotationParametersShowHideWidget(os);
    auto cbAnnotationType = GTWidget::findComboBox(os, "cbAnnotationType");
    return cbAnnotationType->currentText();
}

void setTypeInOptionsPanelWidget(HI::GUITestOpStatus& os, const QString& type) {
    GTUtilsOptionPanelSequenceView::openAnnotationParametersShowHideWidget(os);
    GTComboBox::selectItemByText(os, GTWidget::findComboBox(os, "cbAnnotationType"), type, GTGlobals::UseMouse);
}

bool checkTypePresenceInOptionsPanelWidget(HI::GUITestOpStatus& os, const QString& type) {
    GTUtilsOptionPanelSequenceView::openAnnotationParametersShowHideWidget(os);
    auto cbAnnotationType = GTWidget::findComboBox(os, "cbAnnotationType");
    for (int i = 0; i < cbAnnotationType->count(); i++) {
        if (type == cbAnnotationType->itemText(i)) {
            return true;
        }
    }
    return false;
}

void setGroupName(HI::GUITestOpStatus& os, const QString& name, QWidget* dialog = nullptr) {
    GTLineEdit::setText(os, "leGroupName", name, dialog);
}

void checkGroupName(HI::GUITestOpStatus& os, const QString& name, QWidget* dialog = nullptr) {
    GTLineEdit::checkText(os, "leGroupName", dialog, name);
}

void clickSelectGroupButton(HI::GUITestOpStatus& os, QWidget* dialog = nullptr) {
    GTWidget::click(os, GTWidget::findWidget(os, "tbSelectGroupName", dialog));
}

void setAnnotationName(HI::GUITestOpStatus& os, const QString& name, QWidget* dialog = nullptr) {
    GTLineEdit::setText(os, "leAnnotationName", name, dialog);
}

void setSimpleLocation(HI::GUITestOpStatus& os, int startPos, int endPos, bool complement, QWidget* dialog) {
    GTRadioButton::click(os, GTWidget::findRadioButton(os, "rbSimpleFormat", dialog));
    GTLineEdit::setText(os, "leRegionStart", QString::number(startPos), dialog);
    GTLineEdit::setText(os, "leRegionEnd", QString::number(endPos), dialog);
    GTCheckBox::setChecked(os, GTWidget::findCheckBox(os, "chbComplement", dialog), complement);
    GTThread::waitForMainThread();
}

void setGenbankLocation(HI::GUITestOpStatus& os, const QString& locationString, QWidget* dialog) {
    GTRadioButton::click(os, GTWidget::findRadioButton(os, "rbGenbankFormat", dialog));
    GTLineEdit::setText(os, "leLocation", locationString, dialog);
}

void setExistingTable(HI::GUITestOpStatus& os, QWidget* dialog = nullptr, const QString& tableName = "") {
    GTRadioButton::click(os, GTWidget::findRadioButton(os, "rbExistingTable", dialog));
    if (!tableName.isEmpty()) {
        GTComboBox::selectItemByText(os, GTWidget::findComboBox(os, "cbExistingTable", dialog), tableName);
    }
}

void setNewTable(HI::GUITestOpStatus& os, QWidget* dialog = nullptr, const QString& tablePath = "") {
    GTRadioButton::click(os, GTWidget::findRadioButton(os, "rbCreateNewTable", dialog));
    if (!tablePath.isEmpty()) {
        GTLineEdit::setText(os, "leNewTablePath", tablePath, dialog);
    }
}

class GroupMenuChecker : public CustomScenario {
public:
    GroupMenuChecker(const QStringList& groupNames, const QString& groupToSelect = "")
        : groupNames(groupNames),
          groupToSelect(groupToSelect) {
    }

    void run(HI::GUITestOpStatus& os) {
        GTMouseDriver::release();
        QMenu* activePopupMenu = qobject_cast<QMenu*>(QApplication::activePopupWidget());
        CHECK_SET_ERR(nullptr != activePopupMenu, "Active popup menu is NULL");

        QStringList actualGroupNames;
        foreach (QAction* action, activePopupMenu->actions()) {
            actualGroupNames << action->text();
        }

        CHECK_SET_ERR(groupNames == actualGroupNames, QString("Group names list doesn't match: expect '%1', got '%2'").arg(groupNames.join(", ")).arg(actualGroupNames.join(", ")));

        if (groupToSelect.isEmpty()) {
            GTKeyboardDriver::keyClick(Qt::Key_Escape);
        } else {
            GTMenu::clickMenuItemByText(os, activePopupMenu, {groupToSelect});
        }
    }

private:
    const QStringList groupNames;
    const QString groupToSelect;
};

}  // namespace

GUI_TEST_CLASS_DEFINITION(test_0001) {
    //    Test annotation type for a full widget and nucleotide sequence.

    //    1. Open "data/samples/FASTA/human_T1.fa".
    //    2. Call "Create new annotation" dialog.

    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);

            //    3. Ensure that the default type is "misc_feature".
            const QString type = getTypeFromFullWidget(os, dialog);
            CHECK_SET_ERR("misc_feature" == type,
                          QString("An unexpected default type: expect '%1', got '%2'")
                              .arg("misc_feature")
                              .arg(type));

            //    4. Ensure that there is no type "Kinase".
            const bool kinasePresent = checkTypePresenceInFullWidget(os, "Kinase", dialog);
            CHECK_SET_ERR(!kinasePresent,
                          QString("Amino type is unexpectedly present for nucleotide sequence: '%1'")
                              .arg("Kinase"));

            //    5. Ensure that there is a type "transit_peptide".
            const bool transitPeptidePresent = checkTypePresenceInFullWidget(os, "transit_peptide", dialog);
            CHECK_SET_ERR(transitPeptidePresent,
                          QString("Universal type is unexpectedly missed for nucleotide sequence: '%1'")
                              .arg("transit_peptide"));

            //    6. Select type "bHLH Domain", fill other fields. Accept the dialog.
            setTypeInFullWidget(os, "bHLH Domain", dialog);
            setAnnotationName(os, "test_0001", dialog);
            setGenbankLocation(os, "100..200", dialog);
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, new Scenario));
    openFileAndCallCreateAnnotationDialog(os, dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: there is an annotation with type "bHLH Domain".
    const QString type = GTUtilsAnnotationsTreeView::getAnnotationType(os, "test_0001");
    CHECK_SET_ERR("bHLH Domain" == type, QString("An unexpected annotation type: expect '%1', got '%2'").arg("bHLH Domain").arg(type));
}

GUI_TEST_CLASS_DEFINITION(test_0002) {
    //    Test annotation type for an embedded widget and nucleotide sequence.

    //    1. Open "data/samples/FASTA/human_T1.fa".
    //    2. Call "Smith-Waterman Search" dialog.

    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);

            //    3. Enter any pattern that can be found. Open "Input and output" tab.
            setSmithWatermanPatternAndOpenLastTab(os, dialog);

            //    4. Ensure that the default type is "misc_feature".
            const QString type = getTypeFromNormalWidget(os, dialog);
            CHECK_SET_ERR("misc_feature" == type,
                          QString("An unexpected default type: expect '%1', got '%2'")
                              .arg("misc_feature")
                              .arg(type));

            //    5. Ensure that there is no type "Kinase".
            const bool kinasePresent = checkTypePresenceInNormalWidget(os, "Kinase", dialog);
            CHECK_SET_ERR(!kinasePresent,
                          QString("Amino type is unexpectedly present for nucleotide sequence: '%1'")
                              .arg("Kinase"));

            //    6. Ensure that there is a type "transit_peptide".
            const bool transitPeptidePresent = checkTypePresenceInNormalWidget(os, "transit_peptide", dialog);
            CHECK_SET_ERR(transitPeptidePresent,
                          QString("Universal type is unexpectedly missed for nucleotide sequence: '%1'")
                              .arg("transit_peptide"));

            //    7. Select type "bHLH Domain", fill other fields. Accept the dialog.
            setTypeInNormalWidget(os, "bHLH Domain", dialog);
            setAnnotationName(os, "test_0002", dialog);
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new SmithWatermanDialogFiller(os, new Scenario));
    openFileAndCallSmithWatermanDialog(os, dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: there is an annotation with type "bHLH Domain".
    const QString type = GTUtilsAnnotationsTreeView::getAnnotationType(os, "test_0002");
    CHECK_SET_ERR("bHLH Domain" == type, QString("An unexpected annotation type: expect '%1', got '%2'").arg("bHLH Domain").arg(type));
}

GUI_TEST_CLASS_DEFINITION(test_0003) {
    //    Test annotation type for an options panel widget and nucleotide sequence.

    //    1. Open "data/samples/FASTA/human_T1.fa".
    //    2. Open "Search in Sequence" options panel tab.
    //    3. Enter any pattern that can be found. Open "Annotation parameters" group.
    openFileOpenSearchTabAndSetPattern(os, dataDir + "samples/FASTA/human_T1.fa");

    //    4. Ensure that the default type is "misc_feature".
    const QString defaultType = getTypeFromOptionsPanelWidget(os);
    CHECK_SET_ERR("misc_feature" == defaultType,
                  QString("An unexpected default type: expect '%1', got '%2'")
                      .arg("misc_feature")
                      .arg(defaultType));

    //    5. Ensure that there is no type "Kinase".
    const bool kinasePresent = checkTypePresenceInOptionsPanelWidget(os, "Kinase");
    CHECK_SET_ERR(!kinasePresent,
                  QString("Amino type is unexpectedly present for nucleotide sequence: '%1'")
                      .arg("Kinase"));

    //    6. Ensure that there is a type "transit_peptide".
    const bool transitPeptidePresent = checkTypePresenceInOptionsPanelWidget(os, "transit_peptide");
    CHECK_SET_ERR(transitPeptidePresent,
                  QString("Universal type is unexpectedly missed for nucleotide sequence: '%1'")
                      .arg("transit_peptide"));

    //    7. Select type "bHLH Domain". Create annotations from the search results.
    setTypeInOptionsPanelWidget(os, "bHLH Domain");
    setAnnotationName(os, "test_0003");
    GTUtilsOptionPanelSequenceView::clickGetAnnotation(os);

    //    Expected state: there is an annotation with type "bHLH Domain".
    const QString type = GTUtilsAnnotationsTreeView::getAnnotationType(os, "test_0003");
    CHECK_SET_ERR("bHLH Domain" == type, QString("An unexpected annotation type: expect '%1', got '%2'").arg("bHLH Domain").arg(type));
}

GUI_TEST_CLASS_DEFINITION(test_0004) {
    //    Test annotation type for a full widget and amino acid sequence.

    //    1. Open "_common_data/fasta/AMINO.fa".
    //    2. Call "Create new annotation" dialog.

    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);

            //    3. Ensure that the default type is "misc_feature".
            const QString type = getTypeFromFullWidget(os, dialog);
            CHECK_SET_ERR("misc_feature" == type,
                          QString("An unexpected default type: expect '%1', got '%2'")
                              .arg("misc_feature")
                              .arg(type));

            //    4. Ensure that there is no type "bHLH Domain".
            const bool bhlhPresent = checkTypePresenceInFullWidget(os, "bHLH Domain", dialog);
            CHECK_SET_ERR(!bhlhPresent,
                          QString("Nucleotide type is unexpectedly present for amino sequence: '%1'")
                              .arg("bHLH Domain"));

            //    5. Ensure that there is a type "transit_peptide".
            const bool transitPeptidePresent = checkTypePresenceInFullWidget(os, "transit_peptide", dialog);
            CHECK_SET_ERR(transitPeptidePresent,
                          QString("Universal type is unexpectedly missed for amino sequence: '%1'")
                              .arg("transit_peptide"));

            //    6. Select type "Kinase", fill other fields. Accept the dialog.
            setTypeInFullWidget(os, "Kinase", dialog);
            setAnnotationName(os, "test_0004", dialog);
            setGenbankLocation(os, "10..20", dialog);
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, new Scenario));
    openFileAndCallCreateAnnotationDialog(os, testDir + "_common_data/fasta/AMINO.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: there is an annotation with type "Kinase".
    const QString type = GTUtilsAnnotationsTreeView::getAnnotationType(os, "test_0004");
    CHECK_SET_ERR("Kinase" == type, QString("An unexpected annotation type: expect '%1', got '%2'").arg("Kinase").arg(type));
}

GUI_TEST_CLASS_DEFINITION(test_0005) {
    //    Test annotation type for an embedded widget and amino acid sequence.

    //    1. Open "_common_data/fasta/AMINO.fa".
    //    2. Call "Smith-Waterman Search" dialog.

    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);

            //    3. Enter any pattern that can be found. Open "Input and output" tab.
            setSmithWatermanPatternAndOpenLastTab(os, dialog, "AAAAAACCCCCCC");

            //    4. Ensure that the default type is "misc_feature".
            const QString type = getTypeFromNormalWidget(os, dialog);
            CHECK_SET_ERR("misc_feature" == type,
                          QString("An unexpected default type: expect '%1', got '%2'")
                              .arg("misc_feature")
                              .arg(type));

            //    5. Ensure that there is no type "bHLH Domain".
            const bool bhlhPresent = checkTypePresenceInNormalWidget(os, "bHLH Domain", dialog);
            CHECK_SET_ERR(!bhlhPresent,
                          QString("Nucleotide type is unexpectedly present for amino sequence: '%1'")
                              .arg("bHLH Domain"));

            //    6. Ensure that there is a type "transit_peptide".
            const bool transitPeptidePresent = checkTypePresenceInNormalWidget(os, "transit_peptide", dialog);
            CHECK_SET_ERR(transitPeptidePresent,
                          QString("Universal type is unexpectedly missed for amino sequence: '%1'")
                              .arg("transit_peptide"));

            //    7. Select type "Kinase", fill other fields. Accept the dialog.
            setTypeInNormalWidget(os, "Kinase", dialog);
            setAnnotationName(os, "test_0005", dialog);
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new SmithWatermanDialogFiller(os, new Scenario));
    openFileAndCallSmithWatermanDialog(os, testDir + "_common_data/fasta/AMINO.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: there is an annotation with type "Kinase".
    const QString type = GTUtilsAnnotationsTreeView::getAnnotationType(os, "test_0005");
    CHECK_SET_ERR("Kinase" == type, QString("An unexpected annotation type: expect '%1', got '%2'").arg("Kinase").arg(type));
}

GUI_TEST_CLASS_DEFINITION(test_0006) {
    //    Test annotation type in options panel widget and amino acid sequence.

    //    1. Open "_common_data/fasta/AMINO.fa".
    //    2. Open "Search in Sequence" options panel tab.
    //    3. Enter any pattern that can be found. Open "Annotation parameters" group.
    openFileOpenSearchTabAndSetPattern(os, testDir + "_common_data/fasta/AMINO.fa", "DDDEEEEEEE");

    //    4. Ensure that the default type is "misc_feature".
    QString defaultType = getTypeFromOptionsPanelWidget(os);
    CHECK_SET_ERR(defaultType == "misc_feature",
                  QString("An unexpected default type: expect '%1', got '%2'")
                      .arg("misc_feature")
                      .arg(defaultType));

    //    5. Ensure that there is no type "bHLH Domain".
    bool bhlhPresent = checkTypePresenceInOptionsPanelWidget(os, "bHLH Domain");
    CHECK_SET_ERR(!bhlhPresent,
                  QString("Nucleotide type is unexpectedly present for amino sequence: '%1'")
                      .arg("bHLH Domain"));

    //    6. Ensure that there is a type "transit_peptide".
    bool transitPeptidePresent = checkTypePresenceInOptionsPanelWidget(os, "transit_peptide");
    CHECK_SET_ERR(transitPeptidePresent,
                  QString("Universal type is unexpectedly missed for amino sequence: '%1'")
                      .arg("transit_peptide"));

    //    7. Select type "Kinase". Create annotations from the search results.
    setTypeInOptionsPanelWidget(os, "Kinase");
    setAnnotationName(os, "test_0006");
    GTUtilsOptionPanelSequenceView::clickGetAnnotation(os);

    //    Expected state: there is an annotation with type "Kinase".
    QString type = GTUtilsAnnotationsTreeView::getAnnotationType(os, "test_0006");
    CHECK_SET_ERR(type == "Kinase", QString("An unexpected annotation type: expect '%1', got '%2'").arg("Kinase").arg(type));
}

GUI_TEST_CLASS_DEFINITION(test_0007) {
    //    Test annotation selection by the keyboard

    //    1. Open "data/samples/FASTA/human_T1.fa".
    //    2. Call "Create new annotation" dialog.

    class CreateAnnotationDialogScenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);

            //    3. Enter "tel".
            GTKeyboardDriver::keySequence("tel");

            //    Expected state: "telomere" type is selected. Cancel the dialog.
            const QString type = getTypeFromFullWidget(os, dialog);
            CHECK_SET_ERR("telomere" == type,
                          QString("1: An unexpected feature type: expect '%1', got '%2'")
                              .arg("telomere")
                              .arg(type));

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, new CreateAnnotationDialogScenario));
    openFileAndCallCreateAnnotationDialog(os, dataDir + "samples/FASTA/human_T1.fa");

    //    4. Call "Smith-Waterman Search" dialog. Open "Input and output" tab.

    class SmithWatermanDialogScenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);

            GTTabWidget::setCurrentIndex(os, GTWidget::findTabWidget(os, "tabWidget", dialog), 1);

            //    5. Click to the annotation type combobox. Enter "tel". Click "Enter".
            GTWidget::click(os, GTWidget::findComboBox(os, "cbAnnotationType", dialog));
            GTKeyboardDriver::keySequence("tel");
            GTKeyboardDriver::keyClick(Qt::Key_Enter);

            //    Expected state: "telomere" type is selected. Cancel the dialog.
            const QString type = getTypeFromNormalWidget(os, dialog);
            CHECK_SET_ERR("telomere" == type,
                          QString("2: An unexpected feature type: expect '%1', got '%2'")
                              .arg("telomere")
                              .arg(type));

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(os, new SmithWatermanDialogFiller(os, new SmithWatermanDialogScenario));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Find pattern [Smith-Waterman]");

    //    6. Open "Search in Sequence" options panel tab. Open "Annotation parameters" group.
    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::Search);
    GTUtilsOptionPanelSequenceView::openAnnotationParametersShowHideWidget(os);

    //    7. Click to the annotation type combobox. Enter "tel". Click "Enter".
    //    Expected state: "telomere" type is selected. Cancel the dialog.
    GTWidget::click(os, GTWidget::findComboBox(os, "cbAnnotationType"));
    GTKeyboardDriver::keySequence("tel");
    GTKeyboardDriver::keyClick(Qt::Key_Enter);

    const QString type = getTypeFromOptionsPanelWidget(os);
    CHECK_SET_ERR("telomere" == type,
                  QString("3: An unexpected feature type: expect '%1', got '%2'")
                      .arg("telomere")
                      .arg(type));
}

GUI_TEST_CLASS_DEFINITION(test_0008) {
    //    Test default group name on sequence without annotation table in a full widget.

    //    1. Open "data/samples/FASTA/human_T1.fa".
    //    2. Call "Create new annotation" dialog.

    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) override {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);

            //    3. Ensure that the group name is "<auto>".
            checkGroupName(os, "<auto>", dialog);

            //    4. Click "Predefined group names" button.
            clickSelectGroupButton(os, dialog);

            //    5. Set the annotation name. Accept the dialog.
            setAnnotationName(os, "test_0008", dialog);
            setGenbankLocation(os, "100..200", dialog);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, new Scenario()));
    openFileAndCallCreateAnnotationDialog(os, dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: the group has the same name as the annotation.
    QStringList expectedGroupNames = {"test_0008  (0, 1)"};
    QStringList groupNames = GTUtilsAnnotationsTreeView::getGroupNames(os);
    CHECK_SET_ERR(expectedGroupNames == groupNames,
                  QString("Unexpected group names: expect '%1', got '%2'").arg(expectedGroupNames.join(", ")).arg(groupNames.join(", ")));

    QStringList expectedAnnotationNames = {"test_0008"};
    QStringList annotationNames = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "test_0008  (0, 1)");
    CHECK_SET_ERR(expectedAnnotationNames == annotationNames,
                  QString("Unexpected annotation names: expect '%1', got '%2'").arg(expectedAnnotationNames.join(", ")).arg(annotationNames.join(", ")));
}

GUI_TEST_CLASS_DEFINITION(test_0009) {
    //    Test default group name on sequence without annotation table in an embedded widget.

    //    1. Open "data/samples/FASTA/human_T1.fa".
    //    2. Call "Smith-Waterman Search" dialog. Set any pattern. Open "Input and output" tab.

    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);

            setSmithWatermanPatternAndOpenLastTab(os, dialog);

            //    3. Ensure that the group name is "<auto>".
            checkGroupName(os, "<auto>", dialog);

            //    4. Click "Predefined group names" button.
            clickSelectGroupButton(os, dialog);

            //    5. Set the annotation name. Accept the dialog.
            setAnnotationName(os, "test_0009", dialog);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new SmithWatermanDialogFiller(os, new Scenario));
    openFileAndCallSmithWatermanDialog(os, dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: the group has the same name as the annotation.
    const QStringList expectedGroupNames = {"test_0009  (0, 1)"};
    const QStringList groupNames = GTUtilsAnnotationsTreeView::getGroupNames(os);
    CHECK_SET_ERR(expectedGroupNames == groupNames, QString("Unexpected group names: expect '%1', got '%2'").arg(expectedGroupNames.join(", ").arg(groupNames.join(", "))));

    const QStringList expectedAnnotationNames = {"test_0009"};
    const QStringList annotationNames = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "test_0009  (0, 1)");
    CHECK_SET_ERR(expectedAnnotationNames == annotationNames, QString("Unexpected annotation names: expect '%1', got '%2'").arg(expectedAnnotationNames.join(", ")).arg(annotationNames.join(", ")));
}

GUI_TEST_CLASS_DEFINITION(test_0010) {
    //    Test default group name on sequence without annotation table in an options panel widget.

    //    1. Open "data/samples/FASTA/human_T1.fa".
    //    2. Open "Search in Sequence" options panel tab. Set any pattern. Open "Annotation paramaters" group.
    openFileOpenSearchTabAndSetPattern(os, dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsOptionPanelSequenceView::openAnnotationParametersShowHideWidget(os);

    //    3. Ensure that the group name is "<auto>".
    checkGroupName(os, "<auto>");

    //    4. Click "Predefined group names" button.
    clickSelectGroupButton(os);

    //    5. Set the annotation name. Create annotations.
    setAnnotationName(os, "test_0010");
    GTUtilsOptionPanelSequenceView::clickGetAnnotation(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: the group has the same name as the annotation.
    const QStringList expectedGroupNames = {"test_0010  (0, 1)"};
    const QStringList groupNames = GTUtilsAnnotationsTreeView::getGroupNames(os);
    CHECK_SET_ERR(expectedGroupNames == groupNames, QString("Unexpected group names: expect '%1', got '%2'").arg(expectedGroupNames.join(", ").arg(groupNames.join(", "))));

    const QStringList expectedAnnotationNames = {"test_0010"};
    const QStringList annotationNames = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "test_0010  (0, 1)");
    CHECK_SET_ERR(expectedAnnotationNames == annotationNames, QString("Unexpected annotation names: expect '%1', got '%2'").arg(expectedAnnotationNames.join(", ")).arg(annotationNames.join(", ")));
}

GUI_TEST_CLASS_DEFINITION(test_0011) {
    //    Test group name on sequence without annotation table in a full widget.

    //    1. Open "data/samples/FASTA/human_T1.fa".
    //    2. Call "Create new annotation" dialog.

    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);

            //    3. Set the group name. Accept the dialog.
            setGroupName(os, "test_0011", dialog);
            setGenbankLocation(os, "100..200", dialog);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, new Scenario));
    openFileAndCallCreateAnnotationDialog(os, dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: there is a correctly named group with an annotation inside.
    const QStringList expectedGroupNames = {"test_0011  (0, 1)"};
    const QStringList groupNames = GTUtilsAnnotationsTreeView::getGroupNames(os);
    CHECK_SET_ERR(expectedGroupNames == groupNames, QString("Unexpected group names: expect '%1', got '%2'").arg(expectedGroupNames.join(", ").arg(groupNames.join(", "))));

    const QStringList annotationNames = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "test_0011  (0, 1)");
    CHECK_SET_ERR(1 == annotationNames.size(), QString("Unexpected annotations count: expect '%1', got '%2'").arg(1).arg(annotationNames.size()));
}

GUI_TEST_CLASS_DEFINITION(test_0012) {
    //    Test group name on sequence without annotation table in an embedded widget.

    //    1. Open "data/samples/FASTA/human_T1.fa".
    //    2. Call "Smith-Waterman Search" dialog. Set any pattern. Open "Input and output" tab.

    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);

            setSmithWatermanPatternAndOpenLastTab(os, dialog);

            //    3. Set the group name. Accept the dialog.
            setGroupName(os, "test_0012", dialog);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new SmithWatermanDialogFiller(os, new Scenario));
    openFileAndCallSmithWatermanDialog(os, dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: there is a correctly named group with an annotation inside.
    const QStringList expectedGroupNames = {"test_0012  (0, 1)"};
    const QStringList groupNames = GTUtilsAnnotationsTreeView::getGroupNames(os);
    CHECK_SET_ERR(expectedGroupNames == groupNames, QString("Unexpected group names: expect '%1', got '%2'").arg(expectedGroupNames.join(", ").arg(groupNames.join(", "))));

    const QStringList annotationNames = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "test_0012  (0, 1)");
    CHECK_SET_ERR(1 == annotationNames.size(), QString("Unexpected annotations count: expect '%1', got '%2'").arg(1).arg(annotationNames.size()));
}

GUI_TEST_CLASS_DEFINITION(test_0013) {
    //    Test group name on sequence without annotation table in an options panel widget.

    //    1. Open "data/samples/FASTA/human_T1.fa".
    //    2. Open "Search in Sequence" options panel tab. Set any pattern. Open "Annotation Parameters" group.
    openFileOpenSearchTabAndSetPattern(os, dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsOptionPanelSequenceView::openAnnotationParametersShowHideWidget(os);

    //    3. Set the group name. Create annotations.
    setGroupName(os, "test_0013");

    GTUtilsOptionPanelSequenceView::clickGetAnnotation(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: there is a correctly named group with an annotation inside.
    const QStringList expectedGroupNames = {"test_0013  (0, 1)"};
    const QStringList groupNames = GTUtilsAnnotationsTreeView::getGroupNames(os);
    CHECK_SET_ERR(expectedGroupNames == groupNames, QString("Unexpected group names: expect '%1', got '%2'").arg(expectedGroupNames.join(", ").arg(groupNames.join(", "))));

    const QStringList annotationNames = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "test_0013  (0, 1)");
    CHECK_SET_ERR(1 == annotationNames.size(), QString("Unexpected annotations count: expect '%1', got '%2'").arg(1).arg(annotationNames.size()));
}

GUI_TEST_CLASS_DEFINITION(test_0014) {
    //    Test default group name on sequence with an annotation table in a full widget.

    //    1. Open "data/samples/Genbank/murine.gb".
    //    2. Call "Create new annotation" dialog.

    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);

            //    3. Click "Predefined group names" button.
            //    Expected state: a popup menu is shown, it contains all groups from the existing annotation table.
            const QStringList expectedGroups = {"<auto>", "CDS", "comment", "misc_feature", "source"};
            GTUtilsDialog::waitForDialog(os, new PopupChecker(os, new GroupMenuChecker(expectedGroups)));
            clickSelectGroupButton(os, dialog);

            //    4. Close the menu. Set "Insertion" type. Accept the dialog.
            setTypeInFullWidget(os, "Insertion", dialog);
            setGenbankLocation(os, "100..200", dialog);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, new Scenario));
    openFileAndCallCreateAnnotationDialog(os, dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: there is an additional "Insertion" group with an annotation inside.
    const QStringList expectedGroupNames = {"CDS  (0, 4)", "comment  (0, 1)", "misc_feature  (0, 2)", "source  (0, 1)", "Insertion  (0, 1)"};
    const QStringList groupNames = GTUtilsAnnotationsTreeView::getGroupNames(os);
    CHECK_SET_ERR(expectedGroupNames.toSet() == groupNames.toSet(), QString("Unexpected group names: expect '%1', got '%2'").arg(expectedGroupNames.join(", ").arg(groupNames.join(", "))));

    const QStringList annotationNames = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "Insertion  (0, 1)");
    CHECK_SET_ERR(1 == annotationNames.size(), QString("Unexpected annotations count: expect '%1', got '%2'").arg(1).arg(annotationNames.size()));
}

GUI_TEST_CLASS_DEFINITION(test_0015) {
    //    Test default group name on sequence with an annotation table in an embedded widget.

    //    1. Open "data/samples/Genbank/murine.gb".
    //    2. Call "Smith-Waterman Search" dialog. Set any pattern. Open "Input and output" tab.

    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);

            setSmithWatermanPatternAndOpenLastTab(os, dialog, "GATTTTATTTAGTCTCCAG");

            //    3. Click "Predefined group names" button.
            //    Expected state: a popup menu is shown, it contains all groups from the existing annotation table.
            const QStringList expectedGroups = {"<auto>", "CDS", "comment", "misc_feature", "source"};
            GTUtilsDialog::waitForDialog(os, new PopupChecker(os, new GroupMenuChecker(expectedGroups)));
            clickSelectGroupButton(os, dialog);

            //    4. Close the menu. Set "Insertion" type. Accept the dialog.
            setTypeInNormalWidget(os, "Insertion", dialog);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new SmithWatermanDialogFiller(os, new Scenario));
    openFileAndCallSmithWatermanDialog(os, dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: there is an additional "Insertion" group with an annotation inside.
    const QStringList expectedGroupNames = {"CDS  (0, 4)", "comment  (0, 1)", "misc_feature  (0, 2)", "source  (0, 1)", "Insertion  (0, 1)"};
    const QStringList groupNames = GTUtilsAnnotationsTreeView::getGroupNames(os);
    CHECK_SET_ERR(expectedGroupNames.toSet() == groupNames.toSet(), QString("Unexpected group names: expect '%1', got '%2'").arg(expectedGroupNames.join(", ").arg(groupNames.join(", "))));

    const QStringList annotationNames = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "Insertion  (0, 1)");
    CHECK_SET_ERR(1 == annotationNames.size(), QString("Unexpected annotations count: expect '%1', got '%2'").arg(1).arg(annotationNames.size()));
}

GUI_TEST_CLASS_DEFINITION(test_0016) {
    //    Test default group name on sequence with an annotation table in an options panel widget.

    //    1. Open "data/samples/Genbank/murine.gb".
    //    2. Open "Search in Sequence" options panel tab. Set any pattern. Open "Annotation Parameters" group.
    openFileOpenSearchTabAndSetPattern(os, dataDir + "samples/Genbank/murine.gb", "GATTTTATTTAGTCTCCAG");
    GTUtilsOptionPanelSequenceView::openAnnotationParametersShowHideWidget(os);

    //    3. Click "Predefined group names" button.
    //    Expected state: a popup menu is shown, it contains all groups from the existing annotation table.
    const QStringList expectedGroups = {"<auto>", "CDS", "comment", "misc_feature", "source"};
    GTUtilsDialog::waitForDialog(os, new PopupChecker(os, new GroupMenuChecker(expectedGroups)));
    clickSelectGroupButton(os);

    //    4. Close the menu. Set "Insertion" type. Create annotations.
    setTypeInOptionsPanelWidget(os, "Insertion");

    GTUtilsOptionPanelSequenceView::clickGetAnnotation(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: there is an additional "Insertion" group with an annotation inside.
    const QStringList expectedGroupNames = {"CDS  (0, 4)", "comment  (0, 1)", "misc_feature  (0, 2)", "source  (0, 1)", "Insertion  (0, 1)"};
    const QStringList groupNames = GTUtilsAnnotationsTreeView::getGroupNames(os);
    CHECK_SET_ERR(expectedGroupNames.toSet() == groupNames.toSet(), QString("Unexpected group names: expect '%1', got '%2'").arg(expectedGroupNames.join(", ").arg(groupNames.join(", "))));

    const QStringList annotationNames = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "Insertion  (0, 1)");
    CHECK_SET_ERR(1 == annotationNames.size(), QString("Unexpected annotations count: expect '%1', got '%2'").arg(1).arg(annotationNames.size()));
}

GUI_TEST_CLASS_DEFINITION(test_0017) {
    //    Test non-default group name on sequence with an annotation table in a full widget.

    //    1. Open "data/samples/Genbank/murine.gb".
    //    2. Call "Create new annotation" dialog.

    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);

            //    3. Set the group name. Accept the dialog.
            setGroupName(os, "test_0017", dialog);
            setGenbankLocation(os, "100..200", dialog);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, new Scenario));
    openFileAndCallCreateAnnotationDialog(os, dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: there is an additional correctly named group with an annotation inside.
    const QStringList expectedGroupNames = {"CDS  (0, 4)", "comment  (0, 1)", "misc_feature  (0, 2)", "source  (0, 1)", "test_0017  (0, 1)"};
    const QStringList groupNames = GTUtilsAnnotationsTreeView::getGroupNames(os);
    CHECK_SET_ERR(expectedGroupNames.toSet() == groupNames.toSet(), QString("Unexpected group names: expect '%1', got '%2'").arg(expectedGroupNames.join(", ")).arg(groupNames.join(", ")));

    const QStringList annotationNames = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "test_0017  (0, 1)");
    CHECK_SET_ERR(1 == annotationNames.size(), QString("Unexpected annotations count: expect '%1', got '%2'").arg(1).arg(annotationNames.size()));
}

GUI_TEST_CLASS_DEFINITION(test_0018) {
    //    Test non-default group name on sequence with an annotation table in an embedded widget.

    //    1. Open "data/samples/Genbank/murine.gb".
    //    2. Call "Smith-Waterman Search" dialog. Set any pattern. Open "Input and output" tab.

    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);

            setSmithWatermanPatternAndOpenLastTab(os, dialog, "GATTTTATTTAGTCTCCAG");

            //    3. Set the group name. Accept the dialog.
            setGroupName(os, "test_0018", dialog);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new SmithWatermanDialogFiller(os, new Scenario));
    openFileAndCallSmithWatermanDialog(os, dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: there is an additional correctly named group with an annotation inside.
    const QStringList expectedGroupNames = {"CDS  (0, 4)", "comment  (0, 1)", "misc_feature  (0, 2)", "source  (0, 1)", "test_0018  (0, 1)"};
    const QStringList groupNames = GTUtilsAnnotationsTreeView::getGroupNames(os);
    CHECK_SET_ERR(expectedGroupNames.toSet() == groupNames.toSet(), QString("Unexpected group names: expect '%1', got '%2'").arg(expectedGroupNames.join(", ")).arg(groupNames.join(", ")));

    const QStringList annotationNames = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "test_0018  (0, 1)");
    CHECK_SET_ERR(1 == annotationNames.size(), QString("Unexpected annotations count: expect '%1', got '%2'").arg(1).arg(annotationNames.size()));
}

GUI_TEST_CLASS_DEFINITION(test_0019) {
    //    Test non-default group name on sequence with an annotation table in an options panel widget.

    //    1. Open "data/samples/Genbank/murine.gb".
    //    2. Open "Search in Sequence" options panel tab. Set any pattern. Open "Annotation Parameters" group.
    openFileOpenSearchTabAndSetPattern(os, dataDir + "samples/Genbank/murine.gb", "GATTTTATTTAGTCTCCAG");
    GTUtilsOptionPanelSequenceView::openAnnotationParametersShowHideWidget(os);

    //    3. Set the group name. Create annotations.
    setGroupName(os, "test_0019");
    GTUtilsOptionPanelSequenceView::clickGetAnnotation(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: there is an additional correctly named group with an annotation inside.
    const QStringList expectedGroupNames = {"CDS  (0, 4)", "comment  (0, 1)", "misc_feature  (0, 2)", "source  (0, 1)", "test_0019  (0, 1)"};
    const QStringList groupNames = GTUtilsAnnotationsTreeView::getGroupNames(os);
    CHECK_SET_ERR(expectedGroupNames.toSet() == groupNames.toSet(), QString("Unexpected group names: expect '%1', got '%2'").arg(expectedGroupNames.join(", ")).arg(groupNames.join(", ")));

    const QStringList annotationNames = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "test_0019  (0, 1)");
    CHECK_SET_ERR(1 == annotationNames.size(), QString("Unexpected annotations count: expect '%1', got '%2'").arg(1).arg(annotationNames.size()));
}

GUI_TEST_CLASS_DEFINITION(test_0020) {
    //    Test predefined group name on sequence with an annotation table in a full widget.

    //    1. Open "data/samples/Genbank/murine.gb".
    //    2. Call "Create new annotation" dialog.

    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);

            //    3. Click the "Predefined group names" button.
            //    Expected state: a popup menu with possible group names is shown.
            //    4. Select "CDS" group. Accept the dialog.
            GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, {"CDS"}));
            clickSelectGroupButton(os, dialog);
            setGenbankLocation(os, "100..200", dialog);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, new Scenario));
    openFileAndCallCreateAnnotationDialog(os, dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: there is an additional annotation in the "CDS" group.
    const QStringList expectedGroupNames = {"CDS  (0, 5)", "comment  (0, 1)", "misc_feature  (0, 2)", "source  (0, 1)"};
    const QStringList groupNames = GTUtilsAnnotationsTreeView::getGroupNames(os);
    CHECK_SET_ERR(expectedGroupNames.toSet() == groupNames.toSet(), QString("Unexpected group names: expect '%1', got '%2'").arg(expectedGroupNames.join(", ")).arg(groupNames.join(", ")));

    const QStringList annotationNames = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "CDS  (0, 5)");
    CHECK_SET_ERR(5 == annotationNames.size(), QString("Unexpected annotations count: expect '%1', got '%2'").arg(5).arg(annotationNames.size()));
}

GUI_TEST_CLASS_DEFINITION(test_0021) {
    //    Test predefined group name on sequence with an annotation table in an embedded widget.

    //    1. Open "data/samples/Genbank/murine.gb".
    //    2. Call "Smith-Waterman Search" dialog. Set any pattern. Open "Input and output" tab.

    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);

            setSmithWatermanPatternAndOpenLastTab(os, dialog, "GATTTTATTTAGTCTCCAG");

            //    3. Click the "Predefined group names" button.
            //    Expected state: a popup menu with possible group names is shown.
            //    4. Select "CDS" group. Accept the dialog.
            GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, {"CDS"}));
            clickSelectGroupButton(os, dialog);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new SmithWatermanDialogFiller(os, new Scenario));
    openFileAndCallSmithWatermanDialog(os, dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: there is an additional annotation in the "CDS" group.
    const QStringList expectedGroupNames = {"CDS  (0, 5)", "comment  (0, 1)", "misc_feature  (0, 2)", "source  (0, 1)"};
    const QStringList groupNames = GTUtilsAnnotationsTreeView::getGroupNames(os);
    CHECK_SET_ERR(expectedGroupNames.toSet() == groupNames.toSet(), QString("Unexpected group names: expect '%1', got '%2'").arg(expectedGroupNames.join(", ")).arg(groupNames.join(", ")));

    const QStringList annotationNames = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "CDS  (0, 5)");
    CHECK_SET_ERR(5 == annotationNames.size(), QString("Unexpected annotations count: expect '%1', got '%2'").arg(5).arg(annotationNames.size()));
}

GUI_TEST_CLASS_DEFINITION(test_0022) {
    //    Test predefined group name on sequence with an annotation table in an options panel widget.

    //    1. Open "data/samples/Genbank/murine.gb".
    //    2. Open "Search in Sequence" options panel tab. Set any pattern. Open "Annotation Parameters" group.
    openFileOpenSearchTabAndSetPattern(os, dataDir + "samples/Genbank/murine.gb", "GATTTTATTTAGTCTCCAG");
    GTUtilsOptionPanelSequenceView::openAnnotationParametersShowHideWidget(os);

    //    3. Click the "Predefined group names" button.
    //    Expected state: a popup menu with possible group names is shown.
    //    4. Select "CDS" group. Create annotations.
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, {"CDS"}));
    clickSelectGroupButton(os);

    GTUtilsOptionPanelSequenceView::clickGetAnnotation(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: there is an additional annotation in the "CDS" group.
    const QStringList expectedGroupNames = {"CDS  (0, 5)", "comment  (0, 1)", "misc_feature  (0, 2)", "source  (0, 1)"};
    const QStringList groupNames = GTUtilsAnnotationsTreeView::getGroupNames(os);
    CHECK_SET_ERR(expectedGroupNames.toSet() == groupNames.toSet(), QString("Unexpected group names: expect '%1', got '%2'").arg(expectedGroupNames.join(", ")).arg(groupNames.join(", ")));

    const QStringList annotationNames = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "CDS  (0, 5)");
    CHECK_SET_ERR(5 == annotationNames.size(), QString("Unexpected annotations count: expect '%1', got '%2'").arg(5).arg(annotationNames.size()));
}

GUI_TEST_CLASS_DEFINITION(test_0023) {
    //    Test annotation name in a full widget.

    //    1. Open "data/samples/FASTA/human_T1.fa".
    //    2. Call "Create new annotation" dialog.

    class Scenario1 : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);

            //    3. Use default type, group and name. Accept the dialog.
            setGenbankLocation(os, "100..200", dialog);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, new Scenario1));
    openFileAndCallCreateAnnotationDialog(os, dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: there is an annotation named "misc_feature" of "misc_feature" type in a group named "misc_feature".
    QStringList expectedGroupNames = {"misc_feature  (0, 1)"};
    QStringList groupNames = GTUtilsAnnotationsTreeView::getGroupNames(os);
    CHECK_SET_ERR(expectedGroupNames.toSet() == groupNames.toSet(), QString("Unexpected group names: expect '%1', got '%2'").arg(expectedGroupNames.join(", ")).arg(groupNames.join(", ")));

    QStringList expectedAnnotationNames = {"misc_feature"};
    QStringList annotationNames = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "misc_feature  (0, 1)");
    CHECK_SET_ERR(expectedAnnotationNames == annotationNames, QString("Unexpected annotation names: expect '%1', got '%2'").arg(expectedAnnotationNames.join(", ")).arg(annotationNames.join(", ")));

    QString expectedAnnotationType = "misc_feature";
    QString annotationType = GTUtilsAnnotationsTreeView::getAnnotationType(os, "misc_feature");
    CHECK_SET_ERR(expectedAnnotationType == annotationType, QString("Unexpected annotation type: expect '%1', got '%2'").arg(expectedAnnotationType).arg(annotationType));

    GTUtilsAnnotationsTreeView::deleteItem(os, "misc_feature  (0, 1)");

    //    4. Call "Create new annotation" dialog.

    class Scenario2 : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);

            //    5. Set any name, use default group and type. Accept the dialog.
            setAnnotationName(os, "test_0023", dialog);
            setGenbankLocation(os, "200..300", dialog);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, new Scenario2));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "New annotation");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: there is a correctly named annotation of "misc_feature" type in a group named the same as the annotation.
    expectedGroupNames = QStringList() << "test_0023  (0, 1)";
    groupNames = GTUtilsAnnotationsTreeView::getGroupNames(os);
    CHECK_SET_ERR(expectedGroupNames.toSet() == groupNames.toSet(), QString("Unexpected group names: expect '%1', got '%2'").arg(expectedGroupNames.join(", ")).arg(groupNames.join(", ")));

    expectedAnnotationNames = QStringList() << "test_0023";
    annotationNames = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "test_0023  (0, 1)");
    CHECK_SET_ERR(expectedAnnotationNames == annotationNames, QString("Unexpected annotation names: expect '%1', got '%2'").arg(expectedAnnotationNames.join(", ")).arg(annotationNames.join(", ")));

    expectedAnnotationType = "misc_feature";
    annotationType = GTUtilsAnnotationsTreeView::getAnnotationType(os, "test_0023");
    CHECK_SET_ERR(expectedAnnotationType == annotationType, QString("Unexpected annotation type: expect '%1', got '%2'").arg(expectedAnnotationType).arg(annotationType));

    GTUtilsAnnotationsTreeView::deleteItem(os, "test_0023  (0, 1)");

    //    6. Call "Create new annotation" dialog.

    class Scenario3 : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);

            //    7. Set "Loci" type, use default group and name. Accept the dialog.
            setTypeInFullWidget(os, "Loci", dialog);
            setGenbankLocation(os, "300..400", dialog);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, new Scenario3));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "New annotation");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: there is an annotation named "Loci" of "Loci" type in a group named "Loci".
    expectedGroupNames = QStringList() << "Loci  (0, 1)";
    groupNames = GTUtilsAnnotationsTreeView::getGroupNames(os);
    CHECK_SET_ERR(expectedGroupNames.toSet() == groupNames.toSet(), QString("Unexpected group names: expect '%1', got '%2'").arg(expectedGroupNames.join(", ")).arg(groupNames.join(", ")));

    expectedAnnotationNames = QStringList() << "Loci";
    annotationNames = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "Loci  (0, 1)");
    CHECK_SET_ERR(expectedAnnotationNames == annotationNames, QString("Unexpected annotation names: expect '%1', got '%2'").arg(expectedAnnotationNames.join(", ")).arg(annotationNames.join(", ")));

    expectedAnnotationType = "Loci";
    annotationType = GTUtilsAnnotationsTreeView::getAnnotationType(os, "Loci");
    CHECK_SET_ERR(expectedAnnotationType == annotationType, QString("Unexpected annotation type: expect '%1', got '%2'").arg(expectedAnnotationType).arg(annotationType));

    GTUtilsAnnotationsTreeView::deleteItem(os, "Loci  (0, 1)");

    //    8. Call "Create new annotation" dialog.

    class Scenario4 : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);

            //    9. Set "Attenuator" type and any name, use default group. Accept the dialog.
            setTypeInFullWidget(os, "Attenuator", dialog);
            setAnnotationName(os, "test_0023 again");
            setGenbankLocation(os, "400..500", dialog);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, new Scenario4));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "New annotation");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: there is a correctly named annotation of "Attenuator" type in a group named the same as the annotation.
    expectedGroupNames = QStringList() << "test_0023 again  (0, 1)";
    groupNames = GTUtilsAnnotationsTreeView::getGroupNames(os);
    CHECK_SET_ERR(expectedGroupNames.toSet() == groupNames.toSet(), QString("Unexpected group names: expect '%1', got '%2'").arg(expectedGroupNames.join(", ")).arg(groupNames.join(", ")));

    expectedAnnotationNames = QStringList() << "test_0023 again";
    annotationNames = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "test_0023 again  (0, 1)");
    CHECK_SET_ERR(expectedAnnotationNames == annotationNames, QString("Unexpected annotation names: expect '%1', got '%2'").arg(expectedAnnotationNames.join(", ")).arg(annotationNames.join(", ")));

    expectedAnnotationType = "Attenuator";
    annotationType = GTUtilsAnnotationsTreeView::getAnnotationType(os, "test_0023 again");
    CHECK_SET_ERR(expectedAnnotationType == annotationType, QString("Unexpected annotation type: expect '%1', got '%2'").arg(expectedAnnotationType).arg(annotationType));
}

GUI_TEST_CLASS_DEFINITION(test_0024) {
    //    Test annotation name in an embedded widget.

    //    1. Open "data/samples/FASTA/human_T1.fa".
    //    2. Call "Smith-Waterman Search" dialog. Set any pattern. Open "Input and output" tab.

    class Scenario1 : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);

            setSmithWatermanPatternAndOpenLastTab(os, dialog);

            //    3. Use default type, group and name. Accept the dialog.
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new SmithWatermanDialogFiller(os, new Scenario1));
    openFileAndCallSmithWatermanDialog(os, dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: there is an annotation named "misc_feature" of "misc_feature" type in a group named "misc_feature".
    QStringList expectedGroupNames = {"misc_feature  (0, 1)"};
    QStringList groupNames = GTUtilsAnnotationsTreeView::getGroupNames(os);
    CHECK_SET_ERR(expectedGroupNames.toSet() == groupNames.toSet(), QString("Unexpected group names: expect '%1', got '%2'").arg(expectedGroupNames.join(", ")).arg(groupNames.join(", ")));

    QStringList expectedAnnotationNames = {"misc_feature"};
    QStringList annotationNames = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "misc_feature  (0, 1)");
    CHECK_SET_ERR(expectedAnnotationNames == annotationNames, QString("Unexpected annotation names: expect '%1', got '%2'").arg(expectedAnnotationNames.join(", ")).arg(annotationNames.join(", ")));

    QString expectedAnnotationType = "misc_feature";
    QString annotationType = GTUtilsAnnotationsTreeView::getAnnotationType(os, "misc_feature");
    CHECK_SET_ERR(expectedAnnotationType == annotationType, QString("Unexpected annotation type: expect '%1', got '%2'").arg(expectedAnnotationType).arg(annotationType));

    GTUtilsAnnotationsTreeView::deleteItem(os, "misc_feature  (0, 1)");

    //    4. Call "Smith-Waterman Search" dialog. Set any pattern. Open "Input and output" tab.

    class Scenario2 : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);

            setSmithWatermanPatternAndOpenLastTab(os, dialog);

            //    5. Set any name, use default group and type. Accept the dialog.
            setAnnotationName(os, "test_0024", dialog);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new SmithWatermanDialogFiller(os, new Scenario2));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Find pattern [Smith-Waterman]");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: there is a correctly named annotation of "misc_feature" type in a group named the same as the annotation.
    expectedGroupNames = QStringList() << "test_0024  (0, 1)";
    groupNames = GTUtilsAnnotationsTreeView::getGroupNames(os);
    CHECK_SET_ERR(expectedGroupNames.toSet() == groupNames.toSet(), QString("Unexpected group names: expect '%1', got '%2'").arg(expectedGroupNames.join(", ")).arg(groupNames.join(", ")));

    expectedAnnotationNames = QStringList() << "test_0024";
    annotationNames = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "test_0024  (0, 1)");
    CHECK_SET_ERR(expectedAnnotationNames == annotationNames, QString("Unexpected annotation names: expect '%1', got '%2'").arg(expectedAnnotationNames.join(", ")).arg(annotationNames.join(", ")));

    expectedAnnotationType = "misc_feature";
    annotationType = GTUtilsAnnotationsTreeView::getAnnotationType(os, "test_0024");
    CHECK_SET_ERR(expectedAnnotationType == annotationType, QString("Unexpected annotation type: expect '%1', got '%2'").arg(expectedAnnotationType).arg(annotationType));

    GTUtilsAnnotationsTreeView::deleteItem(os, "test_0024  (0, 1)");

    //    6. Call "Smith-Waterman Search" dialog. Set any pattern. Open "Input and output" tab.

    class Scenario3 : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);

            setSmithWatermanPatternAndOpenLastTab(os, dialog);

            //    7. Set "Loci" type, use default group and name. Accept the dialog.
            setTypeInNormalWidget(os, "Loci", dialog);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new SmithWatermanDialogFiller(os, new Scenario3));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Find pattern [Smith-Waterman]");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: there is an annotation named "Loci" of "Loci" type in a group named "Loci".
    expectedGroupNames = QStringList() << "Loci  (0, 1)";
    groupNames = GTUtilsAnnotationsTreeView::getGroupNames(os);
    CHECK_SET_ERR(expectedGroupNames.toSet() == groupNames.toSet(), QString("Unexpected group names: expect '%1', got '%2'").arg(expectedGroupNames.join(", ")).arg(groupNames.join(", ")));

    expectedAnnotationNames = QStringList() << "Loci";
    annotationNames = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "Loci  (0, 1)");
    CHECK_SET_ERR(expectedAnnotationNames == annotationNames, QString("Unexpected annotation names: expect '%1', got '%2'").arg(expectedAnnotationNames.join(", ")).arg(annotationNames.join(", ")));

    expectedAnnotationType = "Loci";
    annotationType = GTUtilsAnnotationsTreeView::getAnnotationType(os, "Loci");
    CHECK_SET_ERR(expectedAnnotationType == annotationType, QString("Unexpected annotation type: expect '%1', got '%2'").arg(expectedAnnotationType).arg(annotationType));

    GTUtilsAnnotationsTreeView::deleteItem(os, "Loci  (0, 1)");

    //    8. Call "Smith-Waterman Search" dialog. Set any pattern. Open "Input and output" tab.

    class Scenario4 : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);

            setSmithWatermanPatternAndOpenLastTab(os, dialog);

            //    9. Set "Attenuator" type and any name, use default group. Accept the dialog.
            setTypeInNormalWidget(os, "Attenuator", dialog);
            setAnnotationName(os, "test_0024 again", dialog);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new SmithWatermanDialogFiller(os, new Scenario4));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Find pattern [Smith-Waterman]");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: there is a correctly named annotation of "Attenuator" type in a group named the same as the annotation.
    expectedGroupNames = QStringList() << "test_0024 again  (0, 1)";
    groupNames = GTUtilsAnnotationsTreeView::getGroupNames(os);
    CHECK_SET_ERR(expectedGroupNames.toSet() == groupNames.toSet(), QString("Unexpected group names: expect '%1', got '%2'").arg(expectedGroupNames.join(", ")).arg(groupNames.join(", ")));

    expectedAnnotationNames = QStringList() << "test_0024 again";
    annotationNames = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "test_0024 again  (0, 1)");
    CHECK_SET_ERR(expectedAnnotationNames == annotationNames, QString("Unexpected annotation names: expect '%1', got '%2'").arg(expectedAnnotationNames.join(", ")).arg(annotationNames.join(", ")));

    expectedAnnotationType = "Attenuator";
    annotationType = GTUtilsAnnotationsTreeView::getAnnotationType(os, "test_0024 again");
    CHECK_SET_ERR(expectedAnnotationType == annotationType, QString("Unexpected annotation type: expect '%1', got '%2'").arg(expectedAnnotationType).arg(annotationType));
}

GUI_TEST_CLASS_DEFINITION(test_0025) {
    //    Test annotation name in an options panel widget.

    //    1. Open "data/samples/FASTA/human_T1.fa".
    //    2. Open "Search in Sequence" options panel tab. Set any pattern. Open "Annotation Parameters" group.
    openFileOpenSearchTabAndSetPattern(os, dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsOptionPanelSequenceView::openAnnotationParametersShowHideWidget(os);

    //    3. Use default type, group and name. Click "Create annotations" button.
    GTUtilsOptionPanelSequenceView::clickGetAnnotation(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: there is an annotation named "misc_feature" of "misc_feature" type in a group named "misc_feature".
    QStringList expectedGroupNames = {"misc_feature  (0, 1)"};
    QStringList groupNames = GTUtilsAnnotationsTreeView::getGroupNames(os);
    CHECK_SET_ERR(expectedGroupNames.toSet() == groupNames.toSet(), QString("Unexpected group names: expect '%1', got '%2'").arg(expectedGroupNames.join(", ")).arg(groupNames.join(", ")));

    QStringList expectedAnnotationNames = {"misc_feature"};
    QStringList annotationNames = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "misc_feature  (0, 1)");
    CHECK_SET_ERR(expectedAnnotationNames == annotationNames, QString("Unexpected annotation names: expect '%1', got '%2'").arg(expectedAnnotationNames.join(", ")).arg(annotationNames.join(", ")));

    QString expectedAnnotationType = "misc_feature";
    QString annotationType = GTUtilsAnnotationsTreeView::getAnnotationType(os, "misc_feature");
    CHECK_SET_ERR(expectedAnnotationType == annotationType, QString("Unexpected annotation type: expect '%1', got '%2'").arg(expectedAnnotationType).arg(annotationType));

    GTUtilsAnnotationsTreeView::deleteItem(os, "misc_feature  (0, 1)");
    setGroupName(os, "");
    setAnnotationName(os, "");

    //    4. Set any name, use default group and type. Click "Create annotations" button.
    setAnnotationName(os, "test_0025");

    GTUtilsOptionPanelSequenceView::clickGetAnnotation(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: there is a correctly named annotation of "misc_feature" type in a group named the same as the annotation.
    expectedGroupNames = QStringList() << "test_0025  (0, 1)";
    groupNames = GTUtilsAnnotationsTreeView::getGroupNames(os);
    CHECK_SET_ERR(expectedGroupNames.toSet() == groupNames.toSet(), QString("Unexpected group names: expect '%1', got '%2'").arg(expectedGroupNames.join(", ")).arg(groupNames.join(", ")));

    expectedAnnotationNames = QStringList() << "test_0025";
    annotationNames = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "test_0025  (0, 1)");
    CHECK_SET_ERR(expectedAnnotationNames == annotationNames, QString("Unexpected annotation names: expect '%1', got '%2'").arg(expectedAnnotationNames.join(", ")).arg(annotationNames.join(", ")));

    expectedAnnotationType = "misc_feature";
    annotationType = GTUtilsAnnotationsTreeView::getAnnotationType(os, "test_0025");
    CHECK_SET_ERR(expectedAnnotationType == annotationType, QString("Unexpected annotation type: expect '%1', got '%2'").arg(expectedAnnotationType).arg(annotationType));

    GTUtilsAnnotationsTreeView::deleteItem(os, "test_0025  (0, 1)");
    setGroupName(os, "");
    setAnnotationName(os, "");

    //    5. Set "Loci" type, use default group and name. Click "Create annotations" button.
    setTypeInOptionsPanelWidget(os, "Loci");

    GTUtilsOptionPanelSequenceView::clickGetAnnotation(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: there is an annotation named "Loci" of "Loci" type in a group named "Loci".
    expectedGroupNames = QStringList() << "Loci  (0, 1)";
    groupNames = GTUtilsAnnotationsTreeView::getGroupNames(os);
    CHECK_SET_ERR(expectedGroupNames.toSet() == groupNames.toSet(), QString("Unexpected group names: expect '%1', got '%2'").arg(expectedGroupNames.join(", ")).arg(groupNames.join(", ")));

    expectedAnnotationNames = QStringList() << "Loci";
    annotationNames = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "Loci  (0, 1)");
    CHECK_SET_ERR(expectedAnnotationNames == annotationNames, QString("Unexpected annotation names: expect '%1', got '%2'").arg(expectedAnnotationNames.join(", ")).arg(annotationNames.join(", ")));

    expectedAnnotationType = "Loci";
    annotationType = GTUtilsAnnotationsTreeView::getAnnotationType(os, "Loci");
    CHECK_SET_ERR(expectedAnnotationType == annotationType, QString("Unexpected annotation type: expect '%1', got '%2'").arg(expectedAnnotationType).arg(annotationType));

    GTUtilsAnnotationsTreeView::deleteItem(os, "Loci  (0, 1)");
    setGroupName(os, "");
    setAnnotationName(os, "");

    //    6. Set "Attenuator" type and any name, use default group. Click "Create annotations" button.
    setTypeInOptionsPanelWidget(os, "Attenuator");
    setAnnotationName(os, "test_0025 again");

    GTUtilsOptionPanelSequenceView::clickGetAnnotation(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: there is a correctly named annotation of "Attenuator" type in a group named the same as the annotation.
    expectedGroupNames = QStringList() << "test_0025 again  (0, 1)";
    groupNames = GTUtilsAnnotationsTreeView::getGroupNames(os);
    CHECK_SET_ERR(expectedGroupNames.toSet() == groupNames.toSet(), QString("Unexpected group names: expect '%1', got '%2'").arg(expectedGroupNames.join(", ")).arg(groupNames.join(", ")));

    expectedAnnotationNames = QStringList() << "test_0025 again";
    annotationNames = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "test_0025 again  (0, 1)");
    CHECK_SET_ERR(expectedAnnotationNames == annotationNames, QString("Unexpected annotation names: expect '%1', got '%2'").arg(expectedAnnotationNames.join(", ")).arg(annotationNames.join(", ")));

    expectedAnnotationType = "Attenuator";
    annotationType = GTUtilsAnnotationsTreeView::getAnnotationType(os, "test_0025 again");
    CHECK_SET_ERR(expectedAnnotationType == annotationType, QString("Unexpected annotation type: expect '%1', got '%2'").arg(expectedAnnotationType).arg(annotationType));
}

GUI_TEST_CLASS_DEFINITION(test_0026) {
    //    Test GenBank location style in a full widget.

    //    1. Open "data/samples/FASTA/human_T1.fa".
    //    2. Call "Create new annotation" dialog.

    class Scenario1 : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);

            //    3. Set GenBank location style. Set region "100..200". Accept the dialog.
            setGenbankLocation(os, "100..200", dialog);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, new Scenario1));
    openFileAndCallCreateAnnotationDialog(os, dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: there is an annotation with region "100..200".
    QString expectedLocation = "100..200";
    QString location = GTUtilsAnnotationsTreeView::getAnnotationRegionString(os, "misc_feature");
    CHECK_SET_ERR(expectedLocation == location, QString("Unexpected location: expected '%1', got '%2").arg(expectedLocation).arg(location));

    GTUtilsAnnotationsTreeView::deleteItem(os, "misc_feature  (0, 1)");

    //    4. Call "Create new annotation" dialog.

    class Scenario2 : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);

            //    5. Set GenBank location style. Set region "100..200,300..400". Accept the dialog.
            setGenbankLocation(os, "100..200,300..400", dialog);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, new Scenario2));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "New annotation");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: there is an annotation with region "join(100..200,300..400)".
    expectedLocation = "join(100..200,300..400)";
    location = GTUtilsAnnotationsTreeView::getAnnotationRegionString(os, "misc_feature");
    CHECK_SET_ERR(expectedLocation == location, QString("Unexpected location: expected '%1', got '%2").arg(expectedLocation).arg(location));

    GTUtilsAnnotationsTreeView::deleteItem(os, "misc_feature  (0, 1)");

    //    6. Call "Create new annotation" dialog.

    class Scenario3 : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);

            //    7. Set GenBank location style. Set region "complement(100..200)". Accept the dialog.
            setGenbankLocation(os, "complement(100..200)", dialog);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, new Scenario3));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "New annotation");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: there is an annotation with region "complement(100..200)".
    expectedLocation = "complement(100..200)";
    location = GTUtilsAnnotationsTreeView::getAnnotationRegionString(os, "misc_feature");
    CHECK_SET_ERR(expectedLocation == location, QString("Unexpected location: expected '%1', got '%2").arg(expectedLocation).arg(location));

    GTUtilsAnnotationsTreeView::deleteItem(os, "misc_feature  (0, 1)");

    //    8. Call "Create new annotation" dialog.

    class Scenario4 : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);

            //    9. Set GenBank location style. Set region "complement(join(100..200,300..400))". Accept the dialog.
            setGenbankLocation(os, "complement(join(100..200,300..400))", dialog);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, new Scenario4));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "New annotation");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: there is an annotation with region "complement(join(100..200,300..400))".
    expectedLocation = "complement(join(100..200,300..400))";
    location = GTUtilsAnnotationsTreeView::getAnnotationRegionString(os, "misc_feature");
    CHECK_SET_ERR(expectedLocation == location, QString("Unexpected location: expected '%1', got '%2").arg(expectedLocation).arg(location));
}

GUI_TEST_CLASS_DEFINITION(test_0027) {
    //    Test simple location style in a full widget.

    //    1. Open "data/samples/FASTA/human_T1.fa".
    //    2. Call "Create new annotation" dialog.

    class Scenario1 : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);
            //    3. Set simple location style. Set region "100..200". Accept the dialog.
            setSimpleLocation(os, 100, 200, false, dialog);
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, new Scenario1));
    openFileAndCallCreateAnnotationDialog(os, dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsDialog::checkNoActiveWaiters(os);

    //    Expected state: there is an annotation with region "100..200".
    QString expectedLocation = "100..200";
    QString location = GTUtilsAnnotationsTreeView::getAnnotationRegionString(os, "misc_feature");
    CHECK_SET_ERR(expectedLocation == location, QString("Unexpected location: expected '%1', got '%2").arg(expectedLocation).arg(location));

    GTUtilsAnnotationsTreeView::deleteItem(os, "misc_feature  (0, 1)");

    //    4. Call "Create new annotation" dialog.

    class Scenario2 : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);

            //    5. Set simple location style. Set region "100..200". Check the "Complement" checkbox. Accept the dialog.
            setSimpleLocation(os, 100, 200, true, dialog);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, new Scenario2));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "New annotation");
    GTUtilsDialog::checkNoActiveWaiters(os);

    //    Expected state: there is an annotation with region "complement(100..200)".
    expectedLocation = "complement(100..200)";
    location = GTUtilsAnnotationsTreeView::getAnnotationRegionString(os, "misc_feature");
    CHECK_SET_ERR(expectedLocation == location, QString("Unexpected location: expected '%1', got '%2").arg(expectedLocation).arg(location));

    GTUtilsAnnotationsTreeView::deleteItem(os, "misc_feature  (0, 1)");

    //    6. Call "Create new annotation" dialog.

    class Scenario3 : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);
            //    7. Set simple location style. Set region "200..100". Accept the dialog.
            setSimpleLocation(os, 200, 100, false, dialog);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, new Scenario3));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "New annotation");
    GTUtilsDialog::checkNoActiveWaiters(os);

    //    Expected state: there is an annotation with region "join(200..199950,1..100)".
    expectedLocation = "join(200..199950,1..100)";
    location = GTUtilsAnnotationsTreeView::getAnnotationRegionString(os, "misc_feature");
    CHECK_SET_ERR(expectedLocation == location, QString("Unexpected location: expected '%1', got '%2").arg(expectedLocation).arg(location));

    GTUtilsAnnotationsTreeView::deleteItem(os, "misc_feature  (0, 1)");

    //    8. Call "Create new annotation" dialog.

    class Scenario4 : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);
            //    9. Set simple location style. Set region "200..100". Check the "Complement" checkbox. Accept the dialog.
            setSimpleLocation(os, 200, 100, true, dialog);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, new Scenario4));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "New annotation");
    GTUtilsDialog::checkNoActiveWaiters(os);

    //    Expected state: there is an annotation with region "complement(join(200..199950,1..100))".
    expectedLocation = "complement(join(200..199950,1..100))";
    location = GTUtilsAnnotationsTreeView::getAnnotationRegionString(os, "misc_feature");
    CHECK_SET_ERR(expectedLocation == location, QString("Unexpected location: expect '%1', got '%2").arg(expectedLocation).arg(location));
}

GUI_TEST_CLASS_DEFINITION(test_0028) {
    // Test creation annotation on sequence with existing annotation table in a full widget.

    // Add a new annotation to the existing document.
    class CreateAnnotationInExistingDocumentScenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) override {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);
            setExistingTable(os, dialog);
            setGenbankLocation(os, "100..200", dialog);
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, new CreateAnnotationInExistingDocumentScenario()));
    openFileAndCallCreateAnnotationDialog(os, dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: there is an additional annotation in the existing annotation table.
    QStringList expectedGroups = {"CDS  (0, 4)", "comment  (0, 1)", "misc_feature  (0, 3)", "source  (0, 1)"};
    QStringList groups = GTUtilsAnnotationsTreeView::getGroupNames(os, "NC_001363 features [murine.gb] *");
    CHECK_SET_ERR(expectedGroups.toSet() == groups.toSet(), QString("Unexpected group names: expect '%1', got '%2'").arg(expectedGroups.join(", ")).arg(groups.join(", ")));

    QStringList expectedAnnotations = {"misc_feature"};
    QStringList annotations = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "misc_feature  (0, 3)");
    CHECK_SET_ERR(expectedAnnotations.toSet() == annotations.toSet(), QString("Unexpected annotation names: expect '%1', got '%2'").arg(expectedAnnotations.join(", ")).arg(annotations.join(", ")));

    // Add a new annotation into a new document.
    class CreateAnnotationInNewDocumentScenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) override {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);

            //    5. Select "Create new table" option. Set any valid table path. Accept the dialog.
            QDir().mkpath(sandBoxDir + "test_0028");
            setNewTable(os, dialog, sandBoxDir + "test_0028/test_0028.gb");
            setGenbankLocation(os, "100..200", dialog);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, new CreateAnnotationInNewDocumentScenario()));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "New annotation");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //  Expected state: there is a new annotation table with an annotation within.
    expectedGroups = QStringList("misc_feature  (0, 1)");
    groups = GTUtilsAnnotationsTreeView::getGroupNames(os, "Annotations [test_0028.gb] *");
    CHECK_SET_ERR(expectedGroups.toSet() == groups.toSet(), QString("Unexpected group names: expect '%1', got '%2'").arg(expectedGroups.join(", ")).arg(groups.join(", ")));

    expectedAnnotations = QStringList("misc_feature");
    annotations = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "misc_feature  (0, 1)");
    CHECK_SET_ERR(expectedAnnotations.toSet() == annotations.toSet(), QString("Unexpected annotation names: expect '%1', got '%2'").arg(expectedAnnotations.join(", ")).arg(annotations.join(", ")));
}

GUI_TEST_CLASS_DEFINITION(test_0029) {
    // Test creation annotation on sequence with existing annotation table in an embedded widget.

    // Add a new annotation to the existing document.
    class CreateAnnotationInExistingDocumentScenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) override {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);
            setSmithWatermanPatternAndOpenLastTab(os, dialog, "GATTTTATTTAGTCTCCAG");
            setExistingTable(os, dialog);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new SmithWatermanDialogFiller(os, new CreateAnnotationInExistingDocumentScenario()));
    openFileAndCallSmithWatermanDialog(os, dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: there is an additional annotation in the existing annotation table.
    QStringList expectedGroups = {"CDS  (0, 4)", "comment  (0, 1)", "misc_feature  (0, 3)", "source  (0, 1)"};
    QStringList groups = GTUtilsAnnotationsTreeView::getGroupNames(os, "NC_001363 features [murine.gb] *");
    CHECK_SET_ERR(expectedGroups.toSet() == groups.toSet(), QString("Unexpected group names: expect '%1', got '%2'").arg(expectedGroups.join(", ")).arg(groups.join(", ")));

    QStringList expectedAnnotations = {"misc_feature"};
    QStringList annotations = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "misc_feature  (0, 3)");
    CHECK_SET_ERR(expectedAnnotations.toSet() == annotations.toSet(), QString("Unexpected annotation names: expect '%1', got '%2'").arg(expectedAnnotations.join(", ")).arg(annotations.join(", ")));

    // Add a new annotation to a new document.
    class CreateAnnotationInNewDocumentScenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) override {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);
            setSmithWatermanPatternAndOpenLastTab(os, dialog, "GATTTTATTTAGTCTCCAG");
            QDir().mkpath(sandBoxDir + "test_0029");
            setNewTable(os, dialog, sandBoxDir + "test_0029/test_0029.gb");
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new SmithWatermanDialogFiller(os, new CreateAnnotationInNewDocumentScenario()));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Find pattern [Smith-Waterman]");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: there is a new annotation table with an annotation within.
    expectedGroups = QStringList("misc_feature  (0, 1)");
    groups = GTUtilsAnnotationsTreeView::getGroupNames(os, "Annotations [test_0029.gb] *");
    CHECK_SET_ERR(expectedGroups.toSet() == groups.toSet(), QString("Unexpected group names: expect '%1', got '%2'").arg(expectedGroups.join(", ")).arg(groups.join(", ")));

    expectedAnnotations = QStringList("misc_feature");
    annotations = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "misc_feature  (0, 1)");
    CHECK_SET_ERR(expectedAnnotations.toSet() == annotations.toSet(), QString("Unexpected annotation names: expect '%1', got '%2'").arg(expectedAnnotations.join(", ")).arg(annotations.join(", ")));
}

GUI_TEST_CLASS_DEFINITION(test_0030) {
    // Test annotation creation from the Options Panel.

    openFileOpenSearchTabAndSetPattern(os, dataDir + "samples/Genbank/murine.gb", "GATTTTATTTAGTCTCCAG");
    GTUtilsOptionPanelSequenceView::openSaveAnnotationToShowHideWidget(os);

    // Add a new annotation to the existing document.
    setExistingTable(os);

    GTUtilsOptionPanelSequenceView::clickGetAnnotation(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: there is an additional annotation in the existing annotation table.
    QStringList expectedGroups = {"CDS  (0, 4)", "comment  (0, 1)", "misc_feature  (0, 3)", "source  (0, 1)"};
    QStringList groups = GTUtilsAnnotationsTreeView::getGroupNames(os, "NC_001363 features [murine.gb] *");
    CHECK_SET_ERR(expectedGroups.toSet() == groups.toSet(), QString("Unexpected group names: expect '%1', got '%2'").arg(expectedGroups.join(", ")).arg(groups.join(", ")));

    QStringList expectedAnnotations = QStringList("misc_feature");
    QStringList annotations = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "misc_feature  (0, 3)");
    CHECK_SET_ERR(expectedAnnotations.toSet() == annotations.toSet(), QString("Unexpected annotation names: expect '%1', got '%2'").arg(expectedAnnotations.join(", ")).arg(annotations.join(", ")));

    //  Add a new annotation to a new document.
    QDir().mkpath(sandBoxDir + "test_0030");
    setNewTable(os, nullptr, sandBoxDir + "test_0030/test_0030.gb");

    GTUtilsOptionPanelSequenceView::clickGetAnnotation(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: there is a new annotation table with an annotation within.
    expectedGroups = QStringList("misc_feature  (0, 1)");
    groups = GTUtilsAnnotationsTreeView::getGroupNames(os, "Annotations [test_0030.gb] *");
    CHECK_SET_ERR(expectedGroups.toSet() == groups.toSet(), QString("Unexpected group names: expect '%1', got '%2'").arg(expectedGroups.join(", ")).arg(groups.join(", ")));

    expectedAnnotations = QStringList("misc_feature");
    annotations = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "misc_feature  (0, 1)");
    CHECK_SET_ERR(expectedAnnotations.toSet() == annotations.toSet(), QString("Unexpected annotation names: expect '%1', got '%2'").arg(expectedAnnotations.join(", ")).arg(annotations.join(", ")));
}

GUI_TEST_CLASS_DEFINITION(test_0031) {
    //    Test creation annotation on sequence with existing annotation table in a full widget.

    //    1. Open "_common_data/genbank/2nanot_1seq.gb".
    GTFileDialog::openFile(os, testDir + "_common_data/genbank/2annot_1seq.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Drag'n'drop "NC_001363 annotations 2" object to the sequence view, confirm the relation creation.
    GTUtilsAnnotationsTreeView::addAnnotationsTableFromProject(os, "NC_001363 annotations 2");

    //    3. Call "Create new annotation" dialog.

    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);

            //    4. Select "Existing table" option. Click "Browse existing tables" button.
            setExistingTable(os, dialog);

            class ProjectTreeItemsChecker : public CustomScenario {
            public:
                void run(HI::GUITestOpStatus& os) {
                    QWidget* dialog = GTWidget::getActiveModalWidget(os);

                    QTreeView* treeView = dialog->findChild<QTreeView*>();
                    CHECK_SET_ERR(treeView != nullptr, "treeWidget is NULL");

                    //    Expected state: there are two possible tables to save annotation to.
                    int visibleItemCount = 0;
                    for (int i = 0; i < treeView->model()->rowCount(); ++i) {
                        if (Qt::NoItemFlags != treeView->model()->flags(treeView->model()->index(i, 0))) {
                            ++visibleItemCount;
                        }
                    }
                    CHECK_SET_ERR(1 == visibleItemCount, QString("Unexpected documents count: expect %1, got %2").arg(1).arg(visibleItemCount));

                    const QModelIndex documentIndex = GTUtilsProjectTreeView::findIndex(os, treeView, "2annot_1seq.gb");
                    visibleItemCount = 0;
                    for (int i = 0, n = treeView->model()->rowCount(documentIndex); i < n; ++i) {
                        if (Qt::NoItemFlags != treeView->model()->flags(treeView->model()->index(i, 0, documentIndex))) {
                            ++visibleItemCount;
                        }
                    }
                    CHECK_SET_ERR(2 == visibleItemCount, QString("Unexpected objects count: expect %1, got %2").arg(2).arg(visibleItemCount));

                    GTUtilsProjectTreeView::checkObjectTypes(os, treeView, QSet<GObjectType>() << GObjectTypes::ANNOTATION_TABLE, documentIndex);

                    //    5. Select "NC_001363 annotations" object.
                    const QModelIndex objectIndex = GTUtilsProjectTreeView::findIndex(os, treeView, "NC_001363 annotations", documentIndex);
                    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, treeView, objectIndex));
                    GTMouseDriver::click();

                    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
                }
            };

            GTUtilsDialog::waitForDialog(os, new ProjectTreeItemSelectorDialogFiller(os, new ProjectTreeItemsChecker));
            GTWidget::click(os, GTWidget::findWidget(os, "tbBrowseExistingTable", dialog));

            //    6. Click "Predefined group names" button.
            //    Expected state: a popup menu contains all groups from the "NC_001363 annotations" table.
            QStringList expectedGroups = {"<auto>", "CDS", "misc_feature", "source"};
            GTUtilsDialog::waitForDialog(os, new PopupChecker(os, new GroupMenuChecker(expectedGroups)));
            clickSelectGroupButton(os, dialog);

            //    7. Close the popup menu. Set "NC_001363 annotations 2" in existing tables combobox.
            setExistingTable(os, dialog, "2annot_1seq.gb [NC_001363 annotations 2]");

            //    8. Click "Predefined group names" button.
            //    Expected state: a popup menu contains all groups from the "NC_001363 annotations 2" table.
            expectedGroups = QStringList() << "<auto>"
                                           << "group"
                                           << "just an annotation";
            GTUtilsDialog::waitForDialog(os, new PopupChecker(os, new GroupMenuChecker(expectedGroups)));
            clickSelectGroupButton(os, dialog);

            //    9. Close the popup menu. Set any group name. Accept the dialog.
            setGroupName(os, "test_0031", dialog);
            setGenbankLocation(os, "100..200", dialog);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, new Scenario));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "New annotation");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: there is a new annotation in the new correctly named group in the "NC_001363 annotations 2" table.
    QStringList expectedGroups = {"group  (0, 1)", "just an annotation  (0, 1)", "test_0031  (0, 1)"};
    QStringList groups = GTUtilsAnnotationsTreeView::getGroupNames(os, "NC_001363 annotations 2 [2annot_1seq.gb] *");
    CHECK_SET_ERR(expectedGroups.toSet() == groups.toSet(), QString("Unexpected group names: expect '%1', got '%2'").arg(expectedGroups.join(", ")).arg(groups.join(", ")));

    QStringList expectedAnnotations = {"misc_feature"};
    QStringList annotations = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "test_0031  (0, 1)");
    CHECK_SET_ERR(expectedAnnotations.toSet() == annotations.toSet(), QString("Unexpected annotation names: expect '%1', got '%2'").arg(expectedAnnotations.join(", ")).arg(annotations.join(", ")));
}

GUI_TEST_CLASS_DEFINITION(test_0032) {
    //    Test creation annotation on sequence with existing annotation table in an embedded widget.

    //    1. Open "_common_data/genbank/2anot_1seq.gb".
    GTFileDialog::openFile(os, testDir + "_common_data/genbank/2annot_1seq.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Drag'n'drop "NC_001363 annotations 2" object to the sequence view, confirm the relation creation.
    GTUtilsAnnotationsTreeView::addAnnotationsTableFromProject(os, "NC_001363 annotations 2");

    //    3. Call "Smith-Waterman Search" dialog. Set any pattern. Open "Input and output" tab.

    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);

            setSmithWatermanPatternAndOpenLastTab(os, dialog, "GATTTTATTTAGTCTCCAG");

            //    4. Select "Existing table" option. Click "Browse existing tables" button.
            setExistingTable(os, dialog);

            class ProjectTreeItemsChecker : public CustomScenario {
            public:
                void run(HI::GUITestOpStatus& os) {
                    QWidget* dialog = GTWidget::getActiveModalWidget(os);

                    QTreeView* treeView = dialog->findChild<QTreeView*>();
                    CHECK_SET_ERR(treeView != nullptr, "treeWidget is NULL");

                    //    Expected state: there are two possible tables to save annotation to.
                    int visibleItemCount = 0;
                    for (int i = 0; i < treeView->model()->rowCount(); ++i) {
                        if (Qt::NoItemFlags != treeView->model()->flags(treeView->model()->index(i, 0))) {
                            ++visibleItemCount;
                        }
                    }
                    CHECK_SET_ERR(1 == visibleItemCount, QString("Unexpected documents count: expect %1, got %2").arg(1).arg(visibleItemCount));

                    const QModelIndex documentIndex = GTUtilsProjectTreeView::findIndex(os, treeView, "2annot_1seq.gb");
                    visibleItemCount = 0;
                    for (int i = 0, n = treeView->model()->rowCount(documentIndex); i < n; ++i) {
                        if (Qt::NoItemFlags != treeView->model()->flags(treeView->model()->index(i, 0, documentIndex))) {
                            ++visibleItemCount;
                        }
                    }
                    CHECK_SET_ERR(2 == visibleItemCount, QString("Unexpected objects count: expect %1, got %2").arg(2).arg(visibleItemCount));

                    GTUtilsProjectTreeView::checkObjectTypes(os, treeView, QSet<GObjectType>() << GObjectTypes::ANNOTATION_TABLE, documentIndex);

                    //    5. Select "NC_001363 annotations" object.
                    const QModelIndex objectIndex = GTUtilsProjectTreeView::findIndex(os, treeView, "NC_001363 annotations", documentIndex);
                    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, treeView, objectIndex));
                    GTMouseDriver::click();

                    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
                }
            };

            GTUtilsDialog::waitForDialog(os, new ProjectTreeItemSelectorDialogFiller(os, new ProjectTreeItemsChecker));
            GTWidget::click(os, GTWidget::findWidget(os, "tbBrowseExistingTable", dialog));

            //    6. Click "Predefined group names" button.
            //    Expected state: a popup menu contains all groups from the "NC_001363 annotations" table.
            QStringList expectedGroups = {"<auto>", "CDS", "misc_feature", "source"};
            GTUtilsDialog::waitForDialog(os, new PopupChecker(os, new GroupMenuChecker(expectedGroups)));
            clickSelectGroupButton(os, dialog);

            //    7. Close the popup menu. Set "NC_001363 annotations 2" in existing tables combobox.
            setExistingTable(os, dialog, "2annot_1seq.gb [NC_001363 annotations 2]");

            //    8. Click "Predefined group names" button.
            //    Expected state: a popup menu contains all groups from the "NC_001363 annotations 2" table.
            expectedGroups = QStringList() << "<auto>"
                                           << "group"
                                           << "just an annotation";
            GTUtilsDialog::waitForDialog(os, new PopupChecker(os, new GroupMenuChecker(expectedGroups)));
            clickSelectGroupButton(os, dialog);

            //    9. Close the popup menu. Set any group name. Accept the dialog.
            setGroupName(os, "test_0032", dialog);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new SmithWatermanDialogFiller(os, new Scenario));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Find pattern [Smith-Waterman]");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: there is a new annotation in the new correctly named group in the "NC_001363 annotations 2" table.
    QStringList expectedGroups = {"group  (0, 1)", "just an annotation  (0, 1)", "test_0032  (0, 1)"};
    QStringList groups = GTUtilsAnnotationsTreeView::getGroupNames(os, "NC_001363 annotations 2 [2annot_1seq.gb] *");
    CHECK_SET_ERR(expectedGroups.toSet() == groups.toSet(), QString("Unexpected group names: expect '%1', got '%2'").arg(expectedGroups.join(", ")).arg(groups.join(", ")));

    QStringList expectedAnnotations = {"misc_feature"};
    QStringList annotations = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "test_0032  (0, 1)");
    CHECK_SET_ERR(expectedAnnotations.toSet() == annotations.toSet(), QString("Unexpected annotation names: expect '%1', got '%2'").arg(expectedAnnotations.join(", ")).arg(annotations.join(", ")));
}

GUI_TEST_CLASS_DEFINITION(test_0033) {
    //    Test creation annotation on sequence with existing annotation table in an options panel widget.

    //    1. Open "_common_data/genbank/2anot_1seq.gb".
    GTFileDialog::openFile(os, testDir + "_common_data/genbank/2annot_1seq.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Drag'n'drop "NC_001363 annotations 2" object to the sequence view, confirm the relation creation.
    GTUtilsAnnotationsTreeView::addAnnotationsTableFromProject(os, "NC_001363 annotations 2");

    //    3. Open "Search in Sequence" options panel tab. Set any pattern. Open "Save annotation(s) to" and "Annotation parameters" groups.
    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::Search);
    GTUtilsOptionPanelSequenceView::enterPattern(os, "GATTTTATTTAGTCTCCAG", true);
    GTUtilsOptionPanelSequenceView::openSaveAnnotationToShowHideWidget(os);
    GTUtilsOptionPanelSequenceView::openAnnotationParametersShowHideWidget(os);

    //    4. Select "Existing table" option. Click "Browse existing tables" button.
    setExistingTable(os);

    class ProjectTreeItemsChecker : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);

            QTreeView* treeView = dialog->findChild<QTreeView*>();
            CHECK_SET_ERR(treeView != nullptr, "treeWidget is NULL");

            //    Expected state: there are two possible tables to save annotation to.
            int visibleItemCount = 0;
            for (int i = 0; i < treeView->model()->rowCount(); ++i) {
                if (Qt::NoItemFlags != treeView->model()->flags(treeView->model()->index(i, 0))) {
                    ++visibleItemCount;
                }
            }
            CHECK_SET_ERR(1 == visibleItemCount, QString("Unexpected documents count: expect %1, got %2").arg(1).arg(visibleItemCount));

            const QModelIndex documentIndex = GTUtilsProjectTreeView::findIndex(os, treeView, "2annot_1seq.gb");
            visibleItemCount = 0;
            for (int i = 0, n = treeView->model()->rowCount(documentIndex); i < n; ++i) {
                if (Qt::NoItemFlags != treeView->model()->flags(treeView->model()->index(i, 0, documentIndex))) {
                    ++visibleItemCount;
                }
            }
            CHECK_SET_ERR(2 == visibleItemCount, QString("Unexpected objects count: expect %1, got %2").arg(2).arg(visibleItemCount));

            GTUtilsProjectTreeView::checkObjectTypes(os, treeView, QSet<GObjectType>() << GObjectTypes::ANNOTATION_TABLE, documentIndex);

            //    5. Select "NC_001363 annotations" object.
            const QModelIndex objectIndex = GTUtilsProjectTreeView::findIndex(os, treeView, "NC_001363 annotations", documentIndex);
            GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, treeView, objectIndex));
            GTMouseDriver::click();

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new ProjectTreeItemSelectorDialogFiller(os, new ProjectTreeItemsChecker));
    GTWidget::click(os, GTWidget::findWidget(os, "tbBrowseExistingTable"));
    GTUtilsOptionPanelSequenceView::openSaveAnnotationToShowHideWidget(os, false);

    //    6. Click "Predefined group names" button.
    //    Expected state: a popup menu contains all groups from the "NC_001363 annotations" table.
    QStringList expectedGroups = {"<auto>", "CDS", "misc_feature", "source"};
    GTUtilsDialog::waitForDialog(os, new PopupChecker(os, new GroupMenuChecker(expectedGroups)));
    clickSelectGroupButton(os);

    //    7. Close the popup menu. Set "NC_001363 annotations 2" in existing tables combobox.
    GTUtilsOptionPanelSequenceView::openSaveAnnotationToShowHideWidget(os);
    setExistingTable(os, nullptr, "2annot_1seq.gb [NC_001363 annotations 2]");
    GTUtilsOptionPanelSequenceView::openSaveAnnotationToShowHideWidget(os, false);

    //    8. Click "Predefined group names" button.
    //    Expected state: a popup menu contains all groups from the "NC_001363 annotations 2" table.
    expectedGroups = QStringList() << "<auto>"
                                   << "group"
                                   << "just an annotation";
    GTUtilsDialog::waitForDialog(os, new PopupChecker(os, new GroupMenuChecker(expectedGroups)));
    clickSelectGroupButton(os);

    //    9. Close the popup menu. Set any group name. Create annotations.
    setGroupName(os, "test_0033");
    GTUtilsOptionPanelSequenceView::openAnnotationParametersShowHideWidget(os, false);

    GTUtilsOptionPanelSequenceView::clickGetAnnotation(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: there is a new annotation in the new correctly named group in the "NC_001363 annotations 2" table.
    expectedGroups = QStringList() << "group  (0, 1)"
                                   << "just an annotation  (0, 1)"
                                   << "test_0033  (0, 1)";
    QStringList groups = GTUtilsAnnotationsTreeView::getGroupNames(os, "NC_001363 annotations 2 [2annot_1seq.gb] *");
    CHECK_SET_ERR(expectedGroups.toSet() == groups.toSet(), QString("Unexpected group names: expect '%1', got '%2'").arg(expectedGroups.join(", ")).arg(groups.join(", ")));

    QStringList expectedAnnotations = {"misc_feature"};
    QStringList annotations = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "test_0033  (0, 1)");
    CHECK_SET_ERR(expectedAnnotations.toSet() == annotations.toSet(), QString("Unexpected annotation names: expect '%1', got '%2'").arg(expectedAnnotations.join(", ")).arg(annotations.join(", ")));
}

GUI_TEST_CLASS_DEFINITION(test_0034) {
    //    Test creation annotation to a new table in a full widget.

    //    1. Open "data/samples/FASTA/human_T1.fa".
    //    2. Call "Create new annotation" dialog.

    class Scenario1 : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);

            //    Expected state: "New document" field contais "~/Documents/UGENE_Data/MyDocument.gb"
            const QString expectedPath = UserAppsSettings().getDefaultDataDirPath() + "/MyDocument.gb";
            const QString actualPath = GTLineEdit::getText(os, "leNewTablePath", dialog);
            CHECK_SET_ERR(QFileInfo(expectedPath).absoluteFilePath() == QFileInfo(actualPath).absoluteFilePath(),
                          QString("New document path: expect \"%1\", got \"%2\"").arg(expectedPath, actualPath))

            //    3. Select "Create new table" option. Click "Browse new file" button. Select any file. Accept the dialog.
            QDir().mkpath(sandBoxDir + "test_0034");
            setNewTable(os, dialog);
            GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, sandBoxDir + "test_0034/test_0034_1.gb", GTGlobals::UseMouse, GTFileDialogUtils::Save));
            GTWidget::click(os, GTWidget::findWidget(os, "tbBrowseNewTable", dialog));

            setGenbankLocation(os, "100..200", dialog);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, new Scenario1));
    openFileAndCallCreateAnnotationDialog(os, dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: there is a new annotation in a new annotation table.
    QStringList expectedGroups = {"misc_feature  (0, 1)"};
    QStringList groups = GTUtilsAnnotationsTreeView::getGroupNames(os, "Annotations [test_0034_1.gb] *");
    CHECK_SET_ERR(expectedGroups.toSet() == groups.toSet(), QString("Unexpected group names: expect '%1', got '%2'").arg(expectedGroups.join(", ")).arg(groups.join(", ")));

    QStringList expectedAnnotations = {"misc_feature"};
    QStringList annotations = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "misc_feature  (0, 1)");
    CHECK_SET_ERR(expectedAnnotations.toSet() == annotations.toSet(), QString("Unexpected annotation names: expect '%1', got '%2'").arg(expectedAnnotations.join(", ")).arg(annotations.join(", ")));

    //    4. Call "Create new annotation" dialog.

    class Scenario2 : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);

            //    5. Select "Create new table" option. Set any valid file path in the lineedit. Accept the dialog.
            setNewTable(os, dialog, sandBoxDir + "test_0034/test_0034_2.gb");
            setGenbankLocation(os, "100..200", dialog);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, new Scenario2));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "New annotation");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: there is a new annotation in a new annotation table.
    expectedGroups = QStringList() << "misc_feature  (0, 1)";
    groups = GTUtilsAnnotationsTreeView::getGroupNames(os, "Annotations [test_0034_2.gb] *");
    CHECK_SET_ERR(expectedGroups.toSet() == groups.toSet(), QString("Unexpected group names: expect '%1', got '%2'").arg(expectedGroups.join(", ")).arg(groups.join(", ")));

    expectedAnnotations = QStringList() << "misc_feature";
    annotations = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "misc_feature  (0, 1)");
    CHECK_SET_ERR(expectedAnnotations.toSet() == annotations.toSet(), QString("Unexpected annotation names: expect '%1', got '%2'").arg(expectedAnnotations.join(", ")).arg(annotations.join(", ")));
}

GUI_TEST_CLASS_DEFINITION(test_0035) {
    //    Test creation annotation to a new table in an embedded widget.

    //    1. Open "data/samples/FASTA/human_T1.fa".
    //    2. Call "Smith-Waterman" dialog. Set any pattern. Open "Input and output" tab.

    class Scenario1 : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);

            setSmithWatermanPatternAndOpenLastTab(os, dialog);

            //    Expected state: "New document" field contais "~/Documents/UGENE_Data/MyDocument.gb"
            const QString expectedPath = UserAppsSettings().getDefaultDataDirPath() + "/MyDocument.gb";
            const QString actualPath = GTLineEdit::getText(os, "leNewTablePath", dialog);
            CHECK_SET_ERR(QFileInfo(expectedPath).absoluteFilePath() == QFileInfo(actualPath).absoluteFilePath(),
                          QString("New document path: expect \"%1\", got \"%2\"").arg(expectedPath, actualPath))

            //    3. Select "Create new table" option. Click "Browse new file" button. Select any file. Accept the dialog.
            QDir().mkpath(sandBoxDir + "test_0035");
            setNewTable(os, dialog);
            GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, sandBoxDir + "test_0035/test_0035_1.gb", GTGlobals::UseMouse, GTFileDialogUtils::Save));
            GTWidget::click(os, GTWidget::findWidget(os, "tbBrowseNewTable", dialog));

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new SmithWatermanDialogFiller(os, new Scenario1));
    openFileAndCallSmithWatermanDialog(os, dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: there is a new annotation in a new annotation table.
    QStringList expectedGroups = {"misc_feature  (0, 1)"};
    QStringList groups = GTUtilsAnnotationsTreeView::getGroupNames(os, "Annotations [test_0035_1.gb] *");
    CHECK_SET_ERR(expectedGroups.toSet() == groups.toSet(), QString("Unexpected group names: expect '%1', got '%2'").arg(expectedGroups.join(", ")).arg(groups.join(", ")));

    QStringList expectedAnnotations = {"misc_feature"};
    QStringList annotations = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "misc_feature  (0, 1)");
    CHECK_SET_ERR(expectedAnnotations.toSet() == annotations.toSet(), QString("Unexpected annotation names: expect '%1', got '%2'").arg(expectedAnnotations.join(", ")).arg(annotations.join(", ")));

    //    4. Call "Smith-Waterman" dialog. Set any pattern. Open "Input and output" tab.

    class Scenario2 : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);

            setSmithWatermanPatternAndOpenLastTab(os, dialog);

            //    5. Select "Create new table" option. Set any valid file path in the lineedit. Accept the dialog.
            setNewTable(os, dialog, sandBoxDir + "test_0035/test_0035_2.gb");

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new SmithWatermanDialogFiller(os, new Scenario2));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Find pattern [Smith-Waterman]");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: there is a new annotation in a new annotation table.
    expectedGroups = QStringList() << "misc_feature  (0, 1)";
    groups = GTUtilsAnnotationsTreeView::getGroupNames(os, "Annotations [test_0035_2.gb] *");
    CHECK_SET_ERR(expectedGroups.toSet() == groups.toSet(), QString("Unexpected group names: expect '%1', got '%2'").arg(expectedGroups.join(", ")).arg(groups.join(", ")));

    expectedAnnotations = QStringList() << "misc_feature";
    annotations = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "misc_feature  (0, 1)");
    CHECK_SET_ERR(expectedAnnotations.toSet() == annotations.toSet(), QString("Unexpected annotation names: expect '%1', got '%2'").arg(expectedAnnotations.join(", ")).arg(annotations.join(", ")));
}

GUI_TEST_CLASS_DEFINITION(test_0036) {
    //    Test creation annotation to a new table in an options panel widget.

    //    1. Open "data/samples/FASTA/human_T1.fa".
    //    2. Open "Search in Sequence" options panel tab. Set any pattern. Open "Save annotation(s) to" group.
    openFileOpenSearchTabAndSetPattern(os, dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsOptionPanelSequenceView::openSaveAnnotationToShowHideWidget(os);

    //    Expected state: "New document" field contais "~/Documents/UGENE_Data/MyDocument.gb"
    const QString expectedPath = UserAppsSettings().getDefaultDataDirPath() + "/MyDocument.gb";
    const QString actualPath = GTLineEdit::getText(os, "leNewTablePath");
    CHECK_SET_ERR(QFileInfo(expectedPath).absoluteFilePath() == QFileInfo(actualPath).absoluteFilePath(),
                  QString("New document path: expect \"%1\", got \"%2\"").arg(expectedPath, actualPath))

    //    3. Select "Create new table" option. Click "Browse new file" button. Select any file. Accept the dialog.
    setNewTable(os);
    QDir().mkpath(sandBoxDir + "test_0036");
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, sandBoxDir + "test_0036/test_0036_1.gb", GTGlobals::UseMouse, GTFileDialogUtils::Save));
    GTWidget::click(os, GTWidget::findWidget(os, "tbBrowseNewTable"));

    GTUtilsOptionPanelSequenceView::clickGetAnnotation(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: there is a new annotation in a new annotation table.
    QStringList expectedGroups = {"misc_feature  (0, 1)"};
    QStringList groups = GTUtilsAnnotationsTreeView::getGroupNames(os, "Annotations [test_0036_1.gb] *");
    CHECK_SET_ERR(expectedGroups.toSet() == groups.toSet(), QString("Unexpected group names: expect '%1', got '%2'").arg(expectedGroups.join(", ")).arg(groups.join(", ")));

    QStringList expectedAnnotations = {"misc_feature"};
    QStringList annotations = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "misc_feature  (0, 1)");
    CHECK_SET_ERR(expectedAnnotations.toSet() == annotations.toSet(), QString("Unexpected annotation names: expect '%1', got '%2'").arg(expectedAnnotations.join(", ")).arg(annotations.join(", ")));

    //    4. Select "Create new table" option. Set any valid file path in the lineedit. Accept the dialog.
    setNewTable(os, nullptr, sandBoxDir + "test_0036/test_0036_2.gb");

    GTUtilsOptionPanelSequenceView::clickGetAnnotation(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: there is a new annotation in a new annotation table.
    expectedGroups = QStringList() << "misc_feature  (0, 1)";
    groups = GTUtilsAnnotationsTreeView::getGroupNames(os, "Annotations [test_0036_2.gb] *");
    CHECK_SET_ERR(expectedGroups.toSet() == groups.toSet(), QString("Unexpected group names: expect '%1', got '%2'").arg(expectedGroups.join(", ")).arg(groups.join(", ")));

    expectedAnnotations = QStringList() << "misc_feature";
    annotations = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "misc_feature  (0, 1)");
    CHECK_SET_ERR(expectedAnnotations.toSet() == annotations.toSet(), QString("Unexpected annotation names: expect '%1', got '%2'").arg(expectedAnnotations.join(", ")).arg(annotations.join(", ")));
}

GUI_TEST_CLASS_DEFINITION(test_0037) {
    //    Test creation annotation to an auto table in an embedded widget.

    //    1. Open "data/samples/FASTA/human_T1.fa".
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Call "ORF Marker" dialog. Open "Output" tab.

    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);

            GTTabWidget::setCurrentIndex(os, GTWidget::findTabWidget(os, "tabWidget", dialog), 1);

            //    3. Select "Use auto-annotations table" option. Accept the dialog.
            GTRadioButton::click(os, GTWidget::findRadioButton(os, "rbUseAutoTable", dialog));

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new OrfDialogFiller(os, new Scenario));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Find ORFs");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: there are new annotations in the auto-annotation table.
    const QStringList groups = GTUtilsAnnotationsTreeView::getGroupNames(os, "Auto-annotations [human_T1.fa | human_T1 (UCSC April 2002 chr7:115977709-117855134)]");
    CHECK_SET_ERR(1 == groups.size() && groups.first().contains("orf  "), "ORFs auto-annotation group is not found");

    const QStringList annotations = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, groups.first());
    CHECK_SET_ERR(!annotations.isEmpty(), "Auto-annotations list is unexpectedly empty");
}

GUI_TEST_CLASS_DEFINITION(test_0038) {
    //    Test common full widget behavior.

    //    1. Open "data/samples/FASTA/human_T1.fa".
    //    2. Call "Create new annotation" dialog.

    class Scenario1 : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);

            //    3. Check "Use pattern names" checkbox state.
            //    Expected state: it is invisible.
            auto usePatternNames = GTWidget::findCheckBox(os, "chbUsePatternNames", dialog);
            CHECK_SET_ERR(!usePatternNames->isVisible(), "usePatternNames is unexpectedly visible");

            //    4. Check if location widgets are enabled or disabled.
            //    Expected state:
            //        simple radio button - enabled
            //        simple start - enabled
            //        simple end - enabled
            //        simple complementary - enabled
            //        genbank radio button - enabled
            //        genbank lineedit - disabled
            //        genbank complementary button - disabled
            auto rbSimpleFormat = GTWidget::findRadioButton(os, "rbSimpleFormat", dialog);
            CHECK_SET_ERR(rbSimpleFormat->isEnabled(), "1. rbSimpleFormat is unexpectedly disabled");

            auto leRegionStart = GTWidget::findLineEdit(os, "leRegionStart", dialog);
            CHECK_SET_ERR(leRegionStart->isEnabled(), "2. leRegionStart is unexpectedly disabled");

            auto leRegionEnd = GTWidget::findLineEdit(os, "leRegionEnd", dialog);
            CHECK_SET_ERR(leRegionEnd->isEnabled(), "3. leRegionEnd is unexpectedly disabled");

            auto chbComplement = GTWidget::findCheckBox(os, "chbComplement", dialog);
            CHECK_SET_ERR(chbComplement->isEnabled(), "4. chbComplement is unexpectedly disabled");

            auto rbGenbankFormat = GTWidget::findRadioButton(os, "rbGenbankFormat", dialog);
            CHECK_SET_ERR(rbGenbankFormat->isEnabled(), "5. rbGenbankFormat is unexpectedly disabled");

            auto leLocation = GTWidget::findLineEdit(os, "leLocation", dialog);
            CHECK_SET_ERR(!leLocation->isEnabled(), "6. leLocation is unexpectedly enabled");

            auto tbDoComplement = GTWidget::findToolButton(os, "tbDoComplement", dialog);
            CHECK_SET_ERR(!tbDoComplement->isEnabled(), "7. tbDoComplement is unexpectedly enabled");

            //    5. Check boundaries for simple location widgets.
            //    Expected state: values belong to region [1..seq_len], the start position can be greater than the end position. If the position is incorrect (e.g. equal 0) a messagebox appears on dialog acception, the dialog is not closed.
            setSimpleLocation(os, 0, 199950, false, dialog);
            GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok, "Invalid location! Location must be in GenBank format."));
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);

            setSimpleLocation(os, 199951, 199950, false, dialog);
            GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok, "Invalid location! Location must be in GenBank format."));
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);

            setSimpleLocation(os, 1, 0, false, dialog);
            GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok, "Invalid location! Location must be in GenBank format."));
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);

            setSimpleLocation(os, 1, 199951, false, dialog);
            GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok, "Invalid location! Location must be in GenBank format."));
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);

            GTLineEdit::setText(os, leRegionStart, "-1", true);
            CHECK_SET_ERR(leRegionStart->text() == "1", QString("12. An unexpected text in leRegionStart: '%1'").arg(leRegionStart->text()));

            GTLineEdit::setText(os, leRegionStart, "qwerty", true);
            CHECK_SET_ERR(leRegionStart->text().isEmpty(), QString("13. An unexpected text in leRegionStart: '%1'").arg(leRegionStart->text()));

            GTLineEdit::setText(os, leRegionEnd, "-1", true);
            CHECK_SET_ERR(leRegionEnd->text() == "1", QString("14. An unexpected text in leRegionEnd: '%1'").arg(leRegionEnd->text()));

            GTLineEdit::setText(os, leRegionEnd, "qwerty", true);
            CHECK_SET_ERR(leRegionEnd->text().isEmpty(), QString("15. An unexpected text in leRegionEnd: '%1'").arg(leRegionEnd->text()));

            //    6. Enter region "(100..200)" to simple location widgets.
            setSimpleLocation(os, 100, 200, false, dialog);

            //    Expected state: GenBank location string contains "100..200" region.
            QString expectedGenbankLocation = "100..200";
            QString genbankLocation = leLocation->text();
            CHECK_SET_ERR(expectedGenbankLocation == genbankLocation, QString("16. Unexpected GenBank location string: expect '%1', got '%2'").arg(expectedGenbankLocation).arg(genbankLocation));

            //    7. Check "Complement" checkbox.
            GTCheckBox::setChecked(os, chbComplement);

            //    Expected state: GenBank location string contains "complement(100..200)" region.
            expectedGenbankLocation = "complement(100..200)";
            genbankLocation = leLocation->text();
            CHECK_SET_ERR(expectedGenbankLocation == genbankLocation, QString("17. Unexpected GenBank location string: expect '%1', got '%2'").arg(expectedGenbankLocation).arg(genbankLocation));

            //    8. Uncheck "Complement" checkbox.
            GTCheckBox::setChecked(os, chbComplement, false);

            //    Expected state: GenBank location string contains "100..200" region.
            expectedGenbankLocation = "100..200";
            genbankLocation = leLocation->text();
            CHECK_SET_ERR(expectedGenbankLocation == genbankLocation, QString("18. Unexpected GenBank location string: expect '%1', got '%2'").arg(expectedGenbankLocation).arg(genbankLocation));

            //    9. Enter region "(200..100)" to simple location widgets.
            setSimpleLocation(os, 200, 100, false, dialog);

            //    Expected state: GenBank location string contains "join(200..199950,1..100)" region.
            expectedGenbankLocation = "join(200..199950,1..100)";
            genbankLocation = leLocation->text();
            CHECK_SET_ERR(expectedGenbankLocation == genbankLocation, QString("19. Unexpected GenBank location string: expect '%1', got '%2'").arg(expectedGenbankLocation).arg(genbankLocation));

            //    10. Check "Complement" checkbox.
            GTCheckBox::setChecked(os, chbComplement);

            //    Expected state: GenBank location string contains "complement(200..199950,1..100)" region.
            expectedGenbankLocation = "complement(200..199950,1..100)";
            genbankLocation = leLocation->text();
            CHECK_SET_ERR(expectedGenbankLocation == genbankLocation, QString("20. Unexpected GenBank location string: expect '%1', got '%2'").arg(expectedGenbankLocation).arg(genbankLocation));

            //    11. Uncheck "Complement" checkbox.
            GTCheckBox::setChecked(os, chbComplement, false);

            //    Expected state: GenBank location string contains "200..199950,1..100" region.
            expectedGenbankLocation = "200..199950,1..100";
            genbankLocation = leLocation->text();
            CHECK_SET_ERR(expectedGenbankLocation == genbankLocation, QString("21. Unexpected GenBank location string: expect '%1', got '%2'").arg(expectedGenbankLocation).arg(genbankLocation));

            //    12. Select "GenBank/EMBL format" location style. Check if location widgets are enabled or disabled.
            GTRadioButton::click(os, rbGenbankFormat);

            //    Expected state:
            //        simple radio button - enabled
            //        simple start - disabled
            //        simple end - disabled
            //        simple complementary - disabled
            //        genbank radio button - enabled
            //        genbank lineedit - enabled
            //        genbank complementary button - enabled
            CHECK_SET_ERR(rbSimpleFormat->isEnabled(), "22. rbSimpleFormat is unexpectedly disabled");
            CHECK_SET_ERR(!leRegionStart->isEnabled(), "23. leRegionStart is unexpectedly enabled");
            CHECK_SET_ERR(!leRegionEnd->isEnabled(), "leRegionEnd is unexpectedly enabled");
            CHECK_SET_ERR(!chbComplement->isEnabled(), "24. chbComplement is unexpectedly enabled");
            CHECK_SET_ERR(rbGenbankFormat->isEnabled(), "25. rbGenbankFormat is unexpectedly disabled");
            CHECK_SET_ERR(leLocation->isEnabled(), "26. leLocation is unexpectedly disabled");
            CHECK_SET_ERR(tbDoComplement->isEnabled(), "27. tbDoComplement is unexpectedly disabled");

            //    13. Select "Simple format" location style. Check if location widgets are enabled or disabled.
            GTRadioButton::click(os, rbSimpleFormat);

            //    Expected state:
            //        simple radio button - enabled
            //        simple start - enabled
            //        simple end - enabled
            //        simple complementary - enabled
            //        genbank radio button - enabled
            //        genbank lineedit - disabled
            //        genbank complementary button - disabled
            CHECK_SET_ERR(rbSimpleFormat->isEnabled(), "28. rbSimpleFormat is unexpectedly disabled");
            CHECK_SET_ERR(leRegionStart->isEnabled(), "29. leRegionStart is unexpectedly disabled");
            CHECK_SET_ERR(leRegionEnd->isEnabled(), "30. leRegionEnd is unexpectedly disabled");
            CHECK_SET_ERR(chbComplement->isEnabled(), "31. chbComplement is unexpectedly disabled");
            CHECK_SET_ERR(rbGenbankFormat->isEnabled(), "32. rbGenbankFormat is unexpectedly disabled");
            CHECK_SET_ERR(!leLocation->isEnabled(), "33. leLocation is unexpectedly enabled");
            CHECK_SET_ERR(!tbDoComplement->isEnabled(), "34. tbDoComplement is unexpectedly enabled");

            //   30. Enter region "(1'000'000..50)" to simple location widgets.
            setSimpleLocation(os, 1000000, 50, false, dialog);

            //    Expected state: GenBank location string contains "1..50" region.
            expectedGenbankLocation = "1..50";
            genbankLocation = leLocation->text();
            CHECK_SET_ERR(expectedGenbankLocation == genbankLocation, QString("77. Unexpected GenBank location string: expect '%1', got '%2'").arg(expectedGenbankLocation).arg(genbankLocation));

            //    14. Select "GenBank/EMBL format" location style. Set location "300..400".
            setGenbankLocation(os, "300..400", dialog);
            GTKeyboardDriver::keyClick(Qt::Key_Tab);

            //    Expected state: simple location widgets contains non-complemented region "(300..400)".
            CHECK_SET_ERR("300" == leRegionStart->text(), QString("35. Unexpected simple location region start: expect %1, got %2").arg("300").arg(leRegionStart->text()));
            CHECK_SET_ERR("400" == leRegionEnd->text(), QString("36. Unexpected simple location region end: expect %1, got %2").arg("400").arg(leRegionEnd->text()));
            CHECK_SET_ERR(!chbComplement->isChecked(), "37. Simple location complement checkbox is unexpectedly checked");

            //    15. Set location "join(500..600,700..800)".
            setGenbankLocation(os, "join(500..600,700..800)", dialog);
            GTKeyboardDriver::keyClick(Qt::Key_Tab);

            //    Expected state: simple location widgets contains non-complemented region "(500..600)".
            CHECK_SET_ERR(leRegionStart->text() == "500", QString("38. Unexpected simple location region start: expect %1, got %2").arg("500").arg(leRegionStart->text()));
            CHECK_SET_ERR(leRegionEnd->text() == "600", QString("39. Unexpected simple location region end: expect %1, got %2").arg("600").arg(leRegionEnd->text()));
            CHECK_SET_ERR(!chbComplement->isChecked(), "40. Simple location complement checkbox is unexpectedly checked");

            //    16. Set location "join(1..100,200..199950)".
            setGenbankLocation(os, "join(1..100,200..199950)", dialog);
            GTKeyboardDriver::keyClick(Qt::Key_Tab);

            //    Expected state: simple location widgets contains non-complemented region "(1..100)".
            CHECK_SET_ERR("1" == leRegionStart->text(), QString("41. Unexpected simple location region start: expect %1, got %2").arg("1").arg(leRegionStart->text()));
            CHECK_SET_ERR("100" == leRegionEnd->text(), QString("42. Unexpected simple location region end: expect %1, got %2").arg("100").arg(leRegionEnd->text()));
            CHECK_SET_ERR(!chbComplement->isChecked(), "43. Simple location complement checkbox is unexpectedly checked");

            //    17. Set location "join(200..199950,1..100)".
            setGenbankLocation(os, "join(200..199950,1..100)", dialog);
            GTKeyboardDriver::keyClick(Qt::Key_Tab);

            //    Expected state: simple location widgets contains non-complemented region "(200..100)".
            CHECK_SET_ERR("200" == leRegionStart->text(), QString("44. Unexpected simple location region start: expect %1, got %2").arg("200").arg(leRegionStart->text()));
            CHECK_SET_ERR("100" == leRegionEnd->text(), QString("45. Unexpected simple location region end: expect %1, got %2").arg("100").arg(leRegionEnd->text()));
            CHECK_SET_ERR(!chbComplement->isChecked(), "46. Simple location complement checkbox is unexpectedly checked");

            //    18. Click "Do complement" button.
            GTWidget::click(os, tbDoComplement);

            //    Expected state: simple location widgets contains complemented region "(200..100)", GenBank location string contains "complement(200..199950,1..100)".
            expectedGenbankLocation = "complement(200..199950,1..100)";
            genbankLocation = leLocation->text();
            CHECK_SET_ERR(leRegionStart->text() == "200", QString("47. Unexpected simple location region start: expect %1, got %2").arg("200").arg(leRegionStart->text()));
            CHECK_SET_ERR(leRegionEnd->text() == "100", QString("48. Unexpected simple location region end: expect %1, got %2").arg("100").arg(leRegionEnd->text()));
            CHECK_SET_ERR(chbComplement->isChecked(), "49. Simple location complement checkbox is unexpectedly unchecked");
            CHECK_SET_ERR(genbankLocation == expectedGenbankLocation, QString("50. Unexpected GenBank location string: expect '%1', got '%2'").arg(expectedGenbankLocation).arg(leLocation->text()));

            //    19. Set location "complement(200..300)".
            setGenbankLocation(os, "complement(200..300)", dialog);
            GTKeyboardDriver::keyClick(Qt::Key_Tab);

            //    Expected state: simple location widgets contains complemented region "(200..300)".
            CHECK_SET_ERR(leRegionStart->text() == "200", QString("51. Unexpected simple location region start: expect %1, got %2").arg("200").arg(leRegionStart->text()));
            CHECK_SET_ERR(leRegionEnd->text() == "300", QString("52. Unexpected simple location region end: expect %1, got %2").arg("300").arg(leRegionEnd->text()));
            CHECK_SET_ERR(chbComplement->isChecked(), "53. Simple location complement checkbox is unexpectedly unchecked");

            //    20. Click "Do complement" button.
            GTWidget::click(os, tbDoComplement);

            //    Expected state: simple location widgets contains non-complemented region "(200..300)", GenBank location string contains "200..300".
            CHECK_SET_ERR(leRegionStart->text() == "200", QString("54. Unexpected simple location region start: expect %1, got %2").arg("200").arg(leRegionStart->text()));
            CHECK_SET_ERR(leRegionEnd->text() == "300", QString("55. Unexpected simple location region end: expect %1, got %2").arg("300").arg(leRegionEnd->text()));
            CHECK_SET_ERR(!chbComplement->isChecked(), "56. Simple location complement checkbox is unexpectedly checked");
            CHECK_SET_ERR(leLocation->text() == "200..300", QString("57. Unexpected GenBank location string: expect '%1', got '%2'").arg("200..300").arg(leLocation->text()));

            //    21. Set location "complement(join(200..199950,1..100))".
            setGenbankLocation(os, "complement(join(200..199950,1..100))", dialog);
            GTKeyboardDriver::keyClick(Qt::Key_Tab);

            //    Expected state: simple location widgets contains complemented region "(200..100)".
            CHECK_SET_ERR("200" == leRegionStart->text(), QString("58. Unexpected simple location region start: expect %1, got %2").arg("200").arg(leRegionStart->text()));
            CHECK_SET_ERR("100" == leRegionEnd->text(), QString("59. Unexpected simple location region end: expect %1, got %2").arg("100").arg(leRegionEnd->text()));
            CHECK_SET_ERR(chbComplement->isChecked(), "60. Simple location complement checkbox is unexpectedly unchecked");

            //    22. Click "Do complement" button.
            GTWidget::click(os, tbDoComplement);

            //    Expected state: simple location widgets contains non-complemented region "(200..100)", GenBank location string contains "200..199950,1..100".
            expectedGenbankLocation = "200..199950,1..100";
            genbankLocation = leLocation->text();
            CHECK_SET_ERR(leRegionStart->text() == "200", QString("61. Unexpected simple location region start: expect %1, got %2").arg("200").arg(leRegionStart->text()));
            CHECK_SET_ERR(leRegionEnd->text() == "100", QString("62. Unexpected simple location region end: expect %1, got %2").arg("100").arg(leRegionEnd->text()));
            CHECK_SET_ERR(!chbComplement->isChecked(), "63. Simple location complement checkbox is unexpectedly checked");
            CHECK_SET_ERR(genbankLocation == expectedGenbankLocation, QString("64. Unexpected GenBank location string: expect '%1', got '%2'").arg(expectedGenbankLocation).arg(leLocation->text()));

            //    23. Set location "400..500qwerty".
            setGenbankLocation(os, "400..500qwerty", dialog);
            GTKeyboardDriver::keyClick(Qt::Key_Tab);

            //    Expected state: simple location widgets are empty, GenBank location string is empty.
            CHECK_SET_ERR(leRegionStart->text().isEmpty(), QString("65. Unexpected simple location region start: expect an empty string, got %1").arg(leRegionStart->text()));
            CHECK_SET_ERR(leRegionEnd->text().isEmpty(), QString("66. Unexpected simple location region end: expect an empty string, got %1").arg(leRegionEnd->text()));
            CHECK_SET_ERR(!chbComplement->isChecked(), "67. Simple location complement checkbox is unexpectedly checked");
            CHECK_SET_ERR(leLocation->text().isEmpty(), QString("68. Unexpected GenBank location string: expect an empty string, got, '%1'").arg(leLocation->text()));

            //    30. Set Location "1..2,3..4".
            setGenbankLocation(os, "1..2,3..4", dialog);
            //    31. Select "Simple format" location style. Check "Complement" checkbox.
            GTRadioButton::click(os, rbSimpleFormat);
            GTCheckBox::setChecked(os, chbComplement, true);
            //    Expected state: Simple format checked, Genbank format unchecked.
            CHECK_SET_ERR(rbSimpleFormat->isChecked() && !rbGenbankFormat->isChecked(), "76. Unexpected switch between formats");

            //    24. Check if destination table widgets are enabled or disabled.
            //    Expected state:
            //        Existing table radio button - disabled
            //        Existing table combobox - disabled
            //        Existing table browse button - disabled
            //        New table radio button - enabled
            //        New table lineedit - enabled
            //        New table browse button - enabled
            //        Auto table radio button - not visible
            auto rbExistingTable = GTWidget::findRadioButton(os, "rbExistingTable", dialog);
            CHECK_SET_ERR(!rbExistingTable->isEnabled(), "69. rbExistingTable is unexpectedly enabled");

            auto cbExistingTable = GTWidget::findComboBox(os, "cbExistingTable", dialog);
            CHECK_SET_ERR(!cbExistingTable->isEnabled(), "70. cbExistingTable is unexpectedly enabled");

            auto tbBrowseExistingTable = GTWidget::findToolButton(os, "tbBrowseExistingTable", dialog);
            CHECK_SET_ERR(!tbBrowseExistingTable->isEnabled(), "71. tbBrowseExistingTable is unexpectedly enabled");

            auto rbCreateNewTable = GTWidget::findRadioButton(os, "rbCreateNewTable", dialog);
            CHECK_SET_ERR(rbCreateNewTable->isEnabled(), "72. rbCreateNewTable is unexpectedly disabled");

            auto leNewTablePath = GTWidget::findLineEdit(os, "leNewTablePath", dialog);
            CHECK_SET_ERR(leNewTablePath->isEnabled(), "73. leNewTablePath is unexpectedly disabled");

            auto tbBrowseNewTable = GTWidget::findToolButton(os, "tbBrowseNewTable", dialog);
            CHECK_SET_ERR(tbBrowseNewTable->isEnabled(), "74. tbBrowseNewTable is unexpectedly disabled");

            auto rbUseAutoTable = GTWidget::findRadioButton(os, "rbUseAutoTable", dialog);
            CHECK_SET_ERR(!rbUseAutoTable->isVisible(), "75. rbUseAutoTable is unexpectedly visible");

            //    25. Cancel the dialog.
            // Dialog is applied to check boundaries.
            setSimpleLocation(os, 199950, 1, false, dialog);
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, new Scenario1));
    openFileAndCallCreateAnnotationDialog(os, dataDir + "samples/FASTA/human_T1.fa");

    //    26. Open "data/damples/Genbank/murine.gb". Mark the sequence object as circular. Call "Create new annotation" dialog.

    class Scenario2 : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);

            //    27. Check if destination table widgets are enabled or disabled.
            //    Expected state:
            //        Existing table radio button - enabled
            //        Existing table combobox - enabled
            //        Existing table browse button - enabled
            //        New table radio button - enabled
            //        New table lineedit - disabled
            //        New table browse button - disabled
            //        Auto table radio button - not visible
            auto rbExistingTable = GTWidget::findRadioButton(os, "rbExistingTable", dialog);
            CHECK_SET_ERR(rbExistingTable->isEnabled(), "rbExistingTable is unexpectedly disabled");

            auto cbExistingTable = GTWidget::findComboBox(os, "cbExistingTable", dialog);
            CHECK_SET_ERR(cbExistingTable->isEnabled(), "cbExistingTable is unexpectedly disabled");

            auto tbBrowseExistingTable = GTWidget::findToolButton(os, "tbBrowseExistingTable", dialog);
            CHECK_SET_ERR(tbBrowseExistingTable->isEnabled(), "tbBrowseExistingTable is unexpectedly disabled");

            auto rbCreateNewTable = GTWidget::findRadioButton(os, "rbCreateNewTable", dialog);
            CHECK_SET_ERR(rbCreateNewTable->isEnabled(), "rbCreateNewTable is unexpectedly disabled");

            auto leNewTablePath = GTWidget::findLineEdit(os, "leNewTablePath", dialog);
            CHECK_SET_ERR(!leNewTablePath->isEnabled(), "leNewTablePath is unexpectedly enabled");

            auto tbBrowseNewTable = GTWidget::findToolButton(os, "tbBrowseNewTable", dialog);
            CHECK_SET_ERR(!tbBrowseNewTable->isEnabled(), "tbBrowseNewTable is unexpectedly enabled");

            auto rbUseAutoTable = GTWidget::findRadioButton(os, "rbUseAutoTable", dialog);
            CHECK_SET_ERR(!rbUseAutoTable->isVisible(), "rbUseAutoTable is unexpectedly visible");

            //    28. Select "Create new table" option. Check if destination table widgets are enabled or disabled.
            GTWidget::click(os, rbCreateNewTable);

            //    Expected state:
            //        Existing table radio button - enabled
            //        Existing table combobox - disabled
            //        Existing table browse button - disabled
            //        New table radio button - enabled
            //        New table lineedit - enabled
            //        New table browse button - enabled
            //        Auto table radio button - not visible
            CHECK_SET_ERR(rbExistingTable->isEnabled(), "rbExistingTable is unexpectedly disabled");
            CHECK_SET_ERR(!cbExistingTable->isEnabled(), "cbExistingTable is unexpectedly enabled");
            CHECK_SET_ERR(!tbBrowseExistingTable->isEnabled(), "tbBrowseExistingTable is unexpectedly enabled");
            CHECK_SET_ERR(rbCreateNewTable->isEnabled(), "rbCreateNewTable is unexpectedly disabled");
            CHECK_SET_ERR(leNewTablePath->isEnabled(), "leNewTablePath is unexpectedly disabled");
            CHECK_SET_ERR(tbBrowseNewTable->isEnabled(), "tbBrowseNewTable is unexpectedly disabled");
            CHECK_SET_ERR(!rbUseAutoTable->isVisible(), "rbUseAutoTable is unexpectedly visible");

            //    29. Select "Existing table" option. Check if destination table widgets are enabled or disabled.
            GTWidget::click(os, rbExistingTable);

            //    Expected state:
            //        Existing table radio button - enabled
            //        Existing table combobox - enabled
            //        Existing table browse button - enabled
            //        New table radio button - enabled
            //        New table lineedit - disabled
            //        New table browse button - disabled
            //        Auto table radio button - not visible
            CHECK_SET_ERR(rbExistingTable->isEnabled(), "rbExistingTable is unexpectedly disabled");
            CHECK_SET_ERR(cbExistingTable->isEnabled(), "cbExistingTable is unexpectedly disabled");
            CHECK_SET_ERR(tbBrowseExistingTable->isEnabled(), "tbBrowseExistingTable is unexpectedly disabled");
            CHECK_SET_ERR(rbCreateNewTable->isEnabled(), "rbCreateNewTable is unexpectedly disabled");
            CHECK_SET_ERR(!leNewTablePath->isEnabled(), "leNewTablePath is unexpectedly enabled");
            CHECK_SET_ERR(!tbBrowseNewTable->isEnabled(), "tbBrowseNewTable is unexpectedly enabled");
            CHECK_SET_ERR(!rbUseAutoTable->isVisible(), "rbUseAutoTable is unexpectedly visible");

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, new Scenario2));
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsProjectTreeView::markSequenceAsCircular(os, "NC_001363");
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "New annotation");
}

GUI_TEST_CLASS_DEFINITION(test_0039) {
    //    Test common embedded widget behavior.

    //    1. Open "data/samples/FASTA/human_T1.fa".
    //    2. Call "Smith-Waterman Search" dialog. Open "Input and output" tab.

    class Scenario1 : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);

            setSmithWatermanPatternAndOpenLastTab(os, dialog);

            //    3. Check if destination table widgets are enabled or disabled.
            //    Expected state:
            //        Existing table radio button - disabled
            //        Existing table combobox - disabled
            //        Existing table browse button - disabled
            //        New table radio button - enabled
            //        New table lineedit - enabled
            //        New table browse button - enabled
            //        Auto table radio button - disabled
            auto rbExistingTable = GTWidget::findRadioButton(os, "rbExistingTable", dialog);
            CHECK_SET_ERR(!rbExistingTable->isEnabled(), "rbExistingTable is unexpectedly enabled");

            auto cbExistingTable = GTWidget::findComboBox(os, "cbExistingTable", dialog);
            CHECK_SET_ERR(!cbExistingTable->isEnabled(), "cbExistingTable is unexpectedly enabled");

            auto tbBrowseExistingTable = GTWidget::findToolButton(os, "tbBrowseExistingTable", dialog);
            CHECK_SET_ERR(!tbBrowseExistingTable->isEnabled(), "tbBrowseExistingTable is unexpectedly enabled");

            auto rbCreateNewTable = GTWidget::findRadioButton(os, "rbCreateNewTable", dialog);
            CHECK_SET_ERR(rbCreateNewTable->isEnabled(), "rbCreateNewTable is unexpectedly disabled");

            auto leNewTablePath = GTWidget::findLineEdit(os, "leNewTablePath", dialog);
            CHECK_SET_ERR(leNewTablePath->isEnabled(), "leNewTablePath is unexpectedly disabled");

            auto tbBrowseNewTable = GTWidget::findToolButton(os, "tbBrowseNewTable", dialog);
            CHECK_SET_ERR(tbBrowseNewTable->isEnabled(), "tbBrowseNewTable is unexpectedly disabled");

            auto rbUseAutoTable = GTWidget::findRadioButton(os, "rbUseAutoTable", dialog);
            CHECK_SET_ERR(!rbUseAutoTable->isVisible(), "rbUseAutoTable is unexpectedly visible");

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(os, new SmithWatermanDialogFiller(os, new Scenario1));
    openFileAndCallSmithWatermanDialog(os, dataDir + "samples/FASTA/human_T1.fa");

    //    4. Open "data/samples/Genbank/murine.gb".
    //    5. Call "Smith-Waterman Search" dialog. Open "Input and output" tab.

    class Scenario2 : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);

            setSmithWatermanPatternAndOpenLastTab(os, dialog);

            //    6. Check if destination table widgets are enabled or disabled.
            //    Expected state:
            //        Existing table radio button - enabled
            //        Existing table combobox - enabled
            //        Existing table browse button - enabled
            //        New table radio button - enabled
            //        New table lineedit - disabled
            //        New table browse button - disabled
            //        Auto table radio button - disabled
            auto rbExistingTable = GTWidget::findRadioButton(os, "rbExistingTable", dialog);
            CHECK_SET_ERR(rbExistingTable->isEnabled(), "rbExistingTable is unexpectedly disabled");

            auto cbExistingTable = GTWidget::findComboBox(os, "cbExistingTable", dialog);
            CHECK_SET_ERR(cbExistingTable->isEnabled(), "cbExistingTable is unexpectedly disabled");

            auto tbBrowseExistingTable = GTWidget::findToolButton(os, "tbBrowseExistingTable", dialog);
            CHECK_SET_ERR(tbBrowseExistingTable->isEnabled(), "tbBrowseExistingTable is unexpectedly disabled");

            auto rbCreateNewTable = GTWidget::findRadioButton(os, "rbCreateNewTable", dialog);
            CHECK_SET_ERR(rbCreateNewTable->isEnabled(), "rbCreateNewTable is unexpectedly disabled");

            auto leNewTablePath = GTWidget::findLineEdit(os, "leNewTablePath", dialog);
            CHECK_SET_ERR(!leNewTablePath->isEnabled(), "leNewTablePath is unexpectedly enabled");

            auto tbBrowseNewTable = GTWidget::findToolButton(os, "tbBrowseNewTable", dialog);
            CHECK_SET_ERR(!tbBrowseNewTable->isEnabled(), "tbBrowseNewTable is unexpectedly enabled");

            auto rbUseAutoTable = GTWidget::findRadioButton(os, "rbUseAutoTable", dialog);
            CHECK_SET_ERR(!rbUseAutoTable->isVisible(), "rbUseAutoTable is unexpectedly visible");

            //    7. Select "Create new table" option. Check if destination table widgets are enabled or disabled.
            GTWidget::click(os, GTWidget::findWidget(os, "rbCreateNewTable", dialog));

            //    Expected state:
            //        Existing table radio button - enabled
            //        Existing table combobox - disabled
            //        Existing table browse button - disabled
            //        New table radio button - enabled
            //        New table lineedit - enabled
            //        New table browse button - enabled
            //        Auto table radio button - disabled
            CHECK_SET_ERR(rbExistingTable->isEnabled(), "rbExistingTable is unexpectedly disabled");
            CHECK_SET_ERR(!cbExistingTable->isEnabled(), "cbExistingTable is unexpectedly enabled");
            CHECK_SET_ERR(!tbBrowseExistingTable->isEnabled(), "tbBrowseExistingTable is unexpectedly enabled");
            CHECK_SET_ERR(rbCreateNewTable->isEnabled(), "rbCreateNewTable is unexpectedly disabled");
            CHECK_SET_ERR(leNewTablePath->isEnabled(), "leNewTablePath is unexpectedly disabled");
            CHECK_SET_ERR(tbBrowseNewTable->isEnabled(), "tbBrowseNewTable is unexpectedly disabled");
            CHECK_SET_ERR(!rbUseAutoTable->isVisible(), "rbUseAutoTable is unexpectedly visible");

            //    8. Select "Existing table" option. Check if destination table widgets are enabled or disabled.
            GTWidget::click(os, GTWidget::findWidget(os, "rbExistingTable", dialog));

            //    Expected state:
            //        Existing table radio button - enabled
            //        Existing table combobox - enabled
            //        Existing table browse button - enabled
            //        New table radio button - enabled
            //        New table lineedit - disabled
            //        New table browse button - disabled
            //        Auto table radio button - disabled
            CHECK_SET_ERR(rbExistingTable->isEnabled(), "rbExistingTable is unexpectedly disabled");
            CHECK_SET_ERR(cbExistingTable->isEnabled(), "cbExistingTable is unexpectedly disabled");
            CHECK_SET_ERR(tbBrowseExistingTable->isEnabled(), "tbBrowseExistingTable is unexpectedly disabled");
            CHECK_SET_ERR(rbCreateNewTable->isEnabled(), "rbCreateNewTable is unexpectedly disabled");
            CHECK_SET_ERR(!leNewTablePath->isEnabled(), "leNewTablePath is unexpectedly enabled");
            CHECK_SET_ERR(!tbBrowseNewTable->isEnabled(), "tbBrowseNewTable is unexpectedly enabled");
            CHECK_SET_ERR(!rbUseAutoTable->isVisible(), "rbUseAutoTable is unexpectedly visible");

            //    9. Check "Use pattern name" checkbox state.
            //    Expected state: it is invisible.
            auto chbUsePatternNames = GTWidget::findCheckBox(os, "chbUsePatternNames", dialog);
            CHECK_SET_ERR(!chbUsePatternNames->isVisible(), "chbUsePatternNames is unexpectedly visible");

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(os, new SmithWatermanDialogFiller(os, new Scenario2));
    openFileAndCallSmithWatermanDialog(os, dataDir + "samples/Genbank/murine.gb");
}

GUI_TEST_CLASS_DEFINITION(test_0040) {
    //    Test common options panel widget behavior.

    //    1. Open "data/samples/FASTA/human_T1.fa".
    openFileOpenSearchTabAndSetPattern(os, dataDir + "samples/FASTA/human_T1.fa", "");

    //    2. Open "Search in Sequence" options panel tab. Open "Save annotation(s) to" group.
    GTUtilsOptionPanelSequenceView::openSaveAnnotationToShowHideWidget(os);

    //    3. Check if destination table widgets are enabled or disabled.
    //    Expected state:
    //        Existing table radio button - disabled
    //        Existing table combobox - disabled
    //        Existing table browse button - disabled
    //        New table radio button - enabled
    //        New table lineedit - enabled
    //        New table browse button - enabled
    //        Auto table radio button - invisible
    auto rbExistingTable = GTWidget::findRadioButton(os, "rbExistingTable");
    CHECK_SET_ERR(!rbExistingTable->isEnabled(), "rbExistingTable is unexpectedly enabled for human_T1");

    auto cbExistingTable = GTWidget::findComboBox(os, "cbExistingTable");
    CHECK_SET_ERR(!cbExistingTable->isEnabled(), "cbExistingTable is unexpectedly enabled for human_T1");

    auto tbBrowseExistingTable = GTWidget::findToolButton(os, "tbBrowseExistingTable");
    CHECK_SET_ERR(!tbBrowseExistingTable->isEnabled(), "tbBrowseExistingTable is unexpectedly enabled for human_T1");

    auto rbCreateNewTable = GTWidget::findRadioButton(os, "rbCreateNewTable");
    CHECK_SET_ERR(rbCreateNewTable->isEnabled(), "rbCreateNewTable is unexpectedly disabled for human_T1");

    auto leNewTablePath = GTWidget::findLineEdit(os, "leNewTablePath");
    CHECK_SET_ERR(leNewTablePath->isEnabled(), "leNewTablePath is unexpectedly disabled for human_T1");

    auto tbBrowseNewTable = GTWidget::findToolButton(os, "tbBrowseNewTable");
    CHECK_SET_ERR(tbBrowseNewTable->isEnabled(), "tbBrowseNewTable is unexpectedly disabled for human_T1");

    auto rbUseAutoTable = GTWidget::findRadioButton(os, "rbUseAutoTable");
    CHECK_SET_ERR(!rbUseAutoTable->isVisible(), "rbUseAutoTable is unexpectedly visible for human_T1");

    //    4. Open "Annotation parameters" group. Check "Use pattern name" checkbox state.
    GTUtilsOptionPanelSequenceView::openAnnotationParametersShowHideWidget(os);

    //    Expected state: it is visible and enabled.
    auto chbUsePatternNames = GTWidget::findCheckBox(os, "chbUsePatternNames");
    CHECK_SET_ERR(chbUsePatternNames->isVisible(), "chbUsePatternNames is unexpectedly invisible for human_T1");
    CHECK_SET_ERR(chbUsePatternNames->isEnabled(), "chbUsePatternNames is unexpectedly disabled for human_T1");

    GTUtilsMdi::click(os, GTGlobals::Close);

    //    5. Open "data/samples/Genbank/murine.gb".
    openFileOpenSearchTabAndSetPattern(os, dataDir + "samples/Genbank/murine.gb", "");

    //    6. Open "Search in Sequence" options panel tab. Open "Save annotation(s) to" group.
    GTUtilsOptionPanelSequenceView::openSaveAnnotationToShowHideWidget(os);

    //    7. Check if destination table widgets are enabled or disabled.
    //    Expected state:
    //        Existing table radio button - enabled
    //        Existing table combobox - enabled
    //        Existing table browse button - enabled
    //        New table radio button - enabled
    //        New table lineedit - disabled
    //        New table browse button - disabled
    //        Auto table radio button - invisible
    rbExistingTable = GTWidget::findRadioButton(os, "rbExistingTable");
    CHECK_SET_ERR(nullptr != rbExistingTable, "rbExistingTable is NULL");
    CHECK_SET_ERR(rbExistingTable->isEnabled(), "rbExistingTable is unexpectedly disabled for murine_1");

    cbExistingTable = GTWidget::findComboBox(os, "cbExistingTable");
    CHECK_SET_ERR(nullptr != cbExistingTable, "cbExistingTable is NULL");
    CHECK_SET_ERR(cbExistingTable->isEnabled(), "cbExistingTable is unexpectedly disabled for murine_1");

    tbBrowseExistingTable = GTWidget::findToolButton(os, "tbBrowseExistingTable");
    CHECK_SET_ERR(nullptr != tbBrowseExistingTable, "tbBrowseExistingTable is NULL");
    CHECK_SET_ERR(tbBrowseExistingTable->isEnabled(), "tbBrowseExistingTable is unexpectedly disabled for murine_1");

    rbCreateNewTable = GTWidget::findRadioButton(os, "rbCreateNewTable");
    CHECK_SET_ERR(nullptr != rbCreateNewTable, "rbCreateNewTable is NULL");
    CHECK_SET_ERR(rbCreateNewTable->isEnabled(), "rbCreateNewTable is unexpectedly disabled for murine_1");

    leNewTablePath = GTWidget::findLineEdit(os, "leNewTablePath");
    CHECK_SET_ERR(nullptr != leNewTablePath, "leNewTablePath is NULL");
    CHECK_SET_ERR(!leNewTablePath->isEnabled(), "leNewTablePath is unexpectedly enabled for murine_1");

    tbBrowseNewTable = GTWidget::findToolButton(os, "tbBrowseNewTable");
    CHECK_SET_ERR(nullptr != tbBrowseNewTable, "tbBrowseNewTable is NULL");
    CHECK_SET_ERR(!tbBrowseNewTable->isEnabled(), "tbBrowseNewTable is unexpectedly enabled for murine_1");

    rbUseAutoTable = GTWidget::findRadioButton(os, "rbUseAutoTable");
    CHECK_SET_ERR(nullptr != rbUseAutoTable, "rbUseAutoTable is NULL");
    CHECK_SET_ERR(!rbUseAutoTable->isVisible(), "rbUseAutoTable is unexpectedly visible  for murine_1");

    //    8. Select "Create new table" option. Check if destination table widgets are enabled or disabled.
    GTRadioButton::click(os, GTWidget::findRadioButton(os, "rbCreateNewTable"));

    //    Expected state:
    //        Existing table radio button - enabled
    //        Existing table combobox - disabled
    //        Existing table browse button - disabled
    //        New table radio button - enabled
    //        New table lineedit - enabled
    //        New table browse button - enabled
    //        Auto table radio button - invisible
    CHECK_SET_ERR(rbExistingTable->isEnabled(), "rbExistingTable is unexpectedly disabled for murine_2");
    CHECK_SET_ERR(!cbExistingTable->isEnabled(), "cbExistingTable is unexpectedly enabled for murine_2");
    CHECK_SET_ERR(!tbBrowseExistingTable->isEnabled(), "tbBrowseExistingTable is unexpectedly enabled for murine_2");
    CHECK_SET_ERR(rbCreateNewTable->isEnabled(), "rbCreateNewTable is unexpectedly disabled for murine_2");
    CHECK_SET_ERR(leNewTablePath->isEnabled(), "leNewTablePath is unexpectedly disabled  for murine_2");
    CHECK_SET_ERR(tbBrowseNewTable->isEnabled(), "tbBrowseNewTable is unexpectedly disabled  for murine_2");
    CHECK_SET_ERR(!rbUseAutoTable->isVisible(), "rbUseAutoTable is unexpectedly visible for murine_2");

    //    9. Select "Existing table" option. Check if destination table widgets are enabled or disabled.
    GTRadioButton::click(os, GTWidget::findRadioButton(os, "rbExistingTable"));

    //    Expected state:
    //        Existing table radio button - enabled
    //        Existing table combobox - enabled
    //        Existing table browse button - enabled
    //        New table radio button - enabled
    //        New table lineedit - disabled
    //        New table browse button - disabled
    //        Auto table radio button - invisible
    CHECK_SET_ERR(rbExistingTable->isEnabled(), "rbExistingTable is unexpectedly disabled for murine_3");
    CHECK_SET_ERR(cbExistingTable->isEnabled(), "cbExistingTable is unexpectedly disabled for murine_3");
    CHECK_SET_ERR(tbBrowseExistingTable->isEnabled(), "tbBrowseExistingTable is unexpectedly disabled for murine_3");
    CHECK_SET_ERR(rbCreateNewTable->isEnabled(), "rbCreateNewTable is unexpectedly disabled for murine_3");
    CHECK_SET_ERR(!leNewTablePath->isEnabled(), "leNewTablePath is unexpectedly enabled for murine_3");
    CHECK_SET_ERR(!tbBrowseNewTable->isEnabled(), "tbBrowseNewTable is unexpectedly enabled for murine_3");
    CHECK_SET_ERR(!rbUseAutoTable->isVisible(), "rbUseAutoTable is unexpectedly visible for murine_3");

    //    10. Open "Annotation parameters" group. Check "Use pattern name" checkbox state.
    GTUtilsOptionPanelSequenceView::openAnnotationParametersShowHideWidget(os);

    //    Expected state: it is visible and enabled.
    chbUsePatternNames = GTWidget::findCheckBox(os, "chbUsePatternNames");
    CHECK_SET_ERR(nullptr != chbUsePatternNames, "chbUsePatternNames is NULL");
    CHECK_SET_ERR(chbUsePatternNames->isVisible(), "chbUsePatternNames is unexpectedly invisible for murine");
    CHECK_SET_ERR(chbUsePatternNames->isEnabled(), "chbUsePatternNames is unexpectedly disabled for murine");
}

GUI_TEST_CLASS_DEFINITION(test_0041) {
    //    Test annotation description field on full widget

    //    1. Open "data/samples/FASTA/human_T1.fa".
    //    2. Call "Create new annotation" dialog.
    //    3. Create an annotation without a description.

    class Scenario1 : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);

            setAnnotationName(os, "test_0041_1", dialog);
            setGenbankLocation(os, "1..100", dialog);
            GTLineEdit::setText(os, "leDescription", "", dialog);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, new Scenario1));
    openFileAndCallCreateAnnotationDialog(os, dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsDialog::checkNoActiveWaiters(os);

    //    Expected state: a new annotation appears, it hasn't qualifier "note".
    GTUtilsAnnotationsTreeView::selectItemsByName(os, {"test_0041_1"});
    QTreeWidgetItem* descriptionItem = GTUtilsAnnotationsTreeView::findItem(os, "note", nullptr, {false});
    CHECK_SET_ERR(descriptionItem == nullptr, "There is an unexpected note qualifier");

    //    4. Call "Create new annotation" dialog.
    //    5. Create an annotation with some description.
    class Scenario2 : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);
            setAnnotationName(os, "test_0041_2", dialog);
            setGenbankLocation(os, "100..200", dialog);
            GTLineEdit::setText(os, "leDescription", "test_0041_2 description", dialog);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, new Scenario2));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "New annotation");
    GTUtilsDialog::checkNoActiveWaiters(os);

    GTUtilsAnnotationsTreeView::selectItemsByName(os, {"test_0041_2"});

    //    Expected state: a new annotation appears, it has a qualifier "note" with description.
    const QString description = GTUtilsAnnotationsTreeView::getQualifierValue(os, "note", "test_0041_2");
    CHECK_SET_ERR("test_0041_2 description" == description,
                  QString("An unexpected annotation description: expect '%1', got '%2'")
                      .arg("test_0041_2 description")
                      .arg(description));
}

GUI_TEST_CLASS_DEFINITION(test_0042) {
    //    Test annotation description field in an embedded widget

    //    1. Open "data/samples/FASTA/human_T1.fa".
    //    2. Call "Smith-Waterman" dialog. Set any pattern. Open "Input and output" tab.

    class Scenario1 : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);

            setSmithWatermanPatternAndOpenLastTab(os, dialog);

            //    3. Ensure that description field is empty. Accept the dialog.
            setAnnotationName(os, "test_0042_1", dialog);
            GTLineEdit::setText(os, "leDescription", "", dialog);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new SmithWatermanDialogFiller(os, new Scenario1));
    openFileAndCallSmithWatermanDialog(os, dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsDialog::checkNoActiveWaiters(os);

    //    Expected state: a new annotation appears, it hasn't qualifier "note".
    GTUtilsAnnotationsTreeView::selectItemsByName(os, {"test_0042_1"});
    QTreeWidgetItem* descriptionItem = GTUtilsAnnotationsTreeView::findItem(os, "note", nullptr, {false});
    CHECK_SET_ERR(descriptionItem == nullptr, "There is an unexpected note qualifier");

    //    4. Call "Smith-Waterman" dialog. Set any pattern. Open "Input and output" tab.

    class Scenario2 : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);

            setSmithWatermanPatternAndOpenLastTab(os, dialog);

            //    5. Enter any description. Accept the dialog.
            setAnnotationName(os, "test_0042_2", dialog);
            GTLineEdit::setText(os, "leDescription", "test_0042_2 description", dialog);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new SmithWatermanDialogFiller(os, new Scenario2));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Find pattern [Smith-Waterman]");
    GTUtilsDialog::checkNoActiveWaiters(os);

    //    Expected state: a new annotation appears, it has a qualifier "note" with description.
    GTUtilsAnnotationsTreeView::selectItemsByName(os, {"test_0042_2"});
    QString description = GTUtilsAnnotationsTreeView::getQualifierValue(os, "note", "test_0042_2");
    CHECK_SET_ERR("test_0042_2 description" == description,
                  QString("An unexpected annotation description: expect '%1', got '%2'")
                      .arg("test_0042_2 description")
                      .arg(description));
}

GUI_TEST_CLASS_DEFINITION(test_0043) {
    //    Test annotation description field in options panel widget

    //    1. Open "data/samples/FASTA/human_T1.fa".
    openFileOpenSearchTabAndSetPattern(os, dataDir + "samples/FASTA/human_T1.fa");

    //    2. Open "Search in Sequence" options panel tab. Set any pattern. Open "Annotation parameters" group.
    GTUtilsOptionPanelSequenceView::openAnnotationParametersShowHideWidget(os);

    //    3. Ensure that description field is empty. Click "Create annotations" button.
    setAnnotationName(os, "test_0043_1");
    GTLineEdit::setText(os, "leDescription", "");

    GTUtilsOptionPanelSequenceView::clickGetAnnotation(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: a new annotation appears, it hasn't qualifier "note".
    GTUtilsAnnotationsTreeView::selectItemsByName(os, {"test_0043_1"});
    QTreeWidgetItem* descriptionItem = GTUtilsAnnotationsTreeView::findItem(os, "note", nullptr, {false});
    CHECK_SET_ERR(descriptionItem == nullptr, "There is an unexpected note qualifier");

    //    4. Set any description. Click "Create annotations" button.
    setAnnotationName(os, "test_0043_2");
    GTLineEdit::setText(os, "leDescription", "test_0043_2 description");

    GTUtilsOptionPanelSequenceView::clickGetAnnotation(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: a new annotation appears, it has a qualifier "note" with description.
    GTUtilsAnnotationsTreeView::selectItemsByName(os, {"test_0043_2"});
    QString description = GTUtilsAnnotationsTreeView::getQualifierValue(os, "note", "test_0043_2");
    CHECK_SET_ERR(description == "test_0043_2 description",
                  QString("An unexpected annotation description: expect '%1', got '%2'")
                      .arg("test_0043_2 description")
                      .arg(description));
}

GUI_TEST_CLASS_DEFINITION(test_0044) {
    /* 1. Open "_common_data/genbank/1seq.gen".
     * 2. Try to add annotation to opened file
     * Expected state: option is avaliable and no errors showed
     */
    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);

            setAnnotationName(os, "test_0044", dialog);
            setGenbankLocation(os, "10..20", dialog);
            auto rbExistingTable = GTWidget::findRadioButton(os, "rbExistingTable");
            CHECK_SET_ERR(rbExistingTable->isEnabled(), "rbExistingTable is unexpectedly disabled");

            auto cbExistingTable = GTWidget::findComboBox(os, "cbExistingTable");
            CHECK_SET_ERR(cbExistingTable->isEnabled(), "cbExistingTable is unexpectedly disnabled");

            auto tbBrowseExistingTable = GTWidget::findToolButton(os, "tbBrowseExistingTable");
            CHECK_SET_ERR(tbBrowseExistingTable->isEnabled(), "tbBrowseExistingTable is unexpectedly disnabled");

            CHECK_SET_ERR(rbExistingTable->isChecked(), "rbExistingTable is unexpectedly unchecked");
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, new Scenario));
    openFileAndCallCreateAnnotationDialog(os, testDir + "_common_data/genbank/1seq.gen");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: there is an annotation with type "misc_feature".
    const QString type = GTUtilsAnnotationsTreeView::getAnnotationType(os, "test_0044");
    CHECK_SET_ERR("misc_feature" == type, QString("An unexpected annotation type: expect '%1', got '%2'").arg("misc_feature").arg(type));
}

GUI_TEST_CLASS_DEFINITION(test_0045) {
    /* 1. Open "_common_data/genbank/70Bp2.gen".
     * 2. Try to add annotation to both sequences
     * Expected state: option is avaliable and no errors showed
     */
    class Scenario : public CustomScenario {
        QString annotationName;

    public:
        Scenario(QString annotationName)
            : annotationName(annotationName) {
        }
        void run(HI::GUITestOpStatus& os) {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);

            setAnnotationName(os, annotationName, dialog);
            setGenbankLocation(os, "10..20", dialog);
            auto rbExistingTable = GTWidget::findRadioButton(os, "rbExistingTable");
            CHECK_SET_ERR(rbExistingTable->isEnabled(), "rbExistingTable is unexpectedly disabled");

            auto cbExistingTable = GTWidget::findComboBox(os, "cbExistingTable");
            CHECK_SET_ERR(cbExistingTable->isEnabled(), "cbExistingTable is unexpectedly disnabled");

            auto tbBrowseExistingTable = GTWidget::findToolButton(os, "tbBrowseExistingTable");
            CHECK_SET_ERR(tbBrowseExistingTable->isEnabled(), "tbBrowseExistingTable is unexpectedly disnabled");

            CHECK_SET_ERR(rbExistingTable->isChecked(), "rbExistingTable is unexpectedly unchecked");
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os));
    GTUtilsProject::openFile(os, testDir + "_common_data/genbank/70Bp2.gen");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTWidget::click(os, GTWidget::findWidget(os, "ADV_single_sequence_widget_0"));
    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, new Scenario("test_0045_1")));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "New annotation");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTWidget::click(os, GTWidget::findWidget(os, "ADV_single_sequence_widget_1"));
    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, new Scenario("test_0045_2")));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "New annotation");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: there is an annotation with type "misc_feature".
    QString type = GTUtilsAnnotationsTreeView::getAnnotationType(os, "test_0045_1");
    CHECK_SET_ERR("misc_feature" == type, QString("An unexpected annotation type: expect '%1', got '%2'").arg("misc_feature").arg(type));

    // Expected state: there is an annotation with type "misc_feature".
    type = GTUtilsAnnotationsTreeView::getAnnotationType(os, "test_0045_2");
    CHECK_SET_ERR("misc_feature" == type, QString("An unexpected annotation type: expect '%1', got '%2'").arg("misc_feature").arg(type));
}

}  // namespace GUITest_common_scenarios_create_annotation_widget

}  // namespace U2
