/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2020 UniPro <ugene@unipro.ru>
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

#include <QApplication>
#include <QAbstractItemModel>
#include <QDir>
#include <QListWidget>
#include <QTreeView>

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
#include <utils/GTUtilsDialog.h>

#include "GTTestsCreateAnnotationWidget.h"
#include "GTUtilsAnnotationsTreeView.h"
#include "GTUtilsMdi.h"
#include "GTUtilsOptionPanelSequenceView.h"
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

void openFileAndCallCreateAnnotationDialog(HI::GUITestOpStatus &os, const QString &filePath) {
    GTFileDialog::openFile(os, filePath);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "New annotation");
}

void openFileAndCallSmithWatermanDialog(HI::GUITestOpStatus &os, const QString &filePath) {
    GTFileDialog::openFile(os, filePath);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Find pattern [Smith-Waterman]");
}

void setSmithWatermanPatternAndOpenLastTab(HI::GUITestOpStatus &os, QWidget *dialog, const QString &pattern = "AGGAAAAAATGCTAAGGGCAGC") {
    GTTextEdit::setText(os, GTWidget::findExactWidget<QTextEdit *>(os, "teditPattern", dialog), pattern);
    GTTabWidget::setCurrentIndex(os, GTWidget::findExactWidget<QTabWidget *>(os, "tabWidget", dialog), 1);
}

void openFileOpenSearchTabAndSetPattern(HI::GUITestOpStatus &os, const QString &filePath, const QString &pattern = "GCCCATCAGACTAACAG") {
    GTFileDialog::openFile(os, filePath);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::Search);
    GTTextEdit::setText(os, GTWidget::findExactWidget<QTextEdit *>(os, "textPattern"), pattern);
    GTUtilsTaskTreeView::waitTaskFinished(os);
}

QString getTypeFromFullWidget(HI::GUITestOpStatus &os, QWidget *dialog) {
    QListWidget *lwAnnotationType = GTWidget::findExactWidget<QListWidget *>(os, "lwAnnotationType", dialog);
    CHECK_SET_ERR_RESULT(NULL != lwAnnotationType, "lwAnnotationType is NULL", "");
    return lwAnnotationType->currentItem()->text();
}

void setTypeInFullWidget(HI::GUITestOpStatus &os, const QString &type, QWidget *dialog) {
    GTListWidget::click(os, GTWidget::findExactWidget<QListWidget *>(os, "lwAnnotationType", dialog), type);
}

bool checkTypePresenceInFullWidget(HI::GUITestOpStatus &os, const QString &type, QWidget *dialog) {
    return GTListWidget::getItems(os, GTWidget::findExactWidget<QListWidget *>(os, "lwAnnotationType", dialog)).contains(type);
}

QString getTypeFromNormalWidget(HI::GUITestOpStatus &os, QWidget *dialog) {
    QComboBox *cbAnnotationType = GTWidget::findExactWidget<QComboBox *>(os, "cbAnnotationType", dialog);
    CHECK_SET_ERR_RESULT(NULL != cbAnnotationType, "cbAnnotationType is NULL", "");
    return cbAnnotationType->currentText();
}

void setTypeInNormalWidget(HI::GUITestOpStatus &os, const QString &type, QWidget *dialog) {
    GTComboBox::setIndexWithText(os, GTWidget::findExactWidget<QComboBox *>(os, "cbAnnotationType", dialog), type, true, GTGlobals::UseMouse);
}

bool checkTypePresenceInNormalWidget(HI::GUITestOpStatus &os, const QString &type, QWidget *dialog) {
    QComboBox *cbAnnotationType = GTWidget::findExactWidget<QComboBox *>(os, "cbAnnotationType", dialog);
    CHECK_SET_ERR_RESULT(NULL != cbAnnotationType, "cbAnnotationType is NULL", "");
    for (int i = 0; i < cbAnnotationType->count(); i++) {
        if (type == cbAnnotationType->itemText(i)) {
            return true;
        }
    }
    return false;
}

QString getTypeFromOptionsPanelWidget(HI::GUITestOpStatus &os) {
    GTUtilsOptionPanelSequenceView::openAnnotationParametersShowHideWidget(os);
    QComboBox *cbAnnotationType = GTWidget::findExactWidget<QComboBox *>(os, "cbAnnotationType");
    CHECK_SET_ERR_RESULT(NULL != cbAnnotationType, "cbAnnotationType is NULL", "");
    return cbAnnotationType->currentText();
}

void setTypeInOptionsPanelWidget(HI::GUITestOpStatus &os, const QString &type) {
    GTUtilsOptionPanelSequenceView::openAnnotationParametersShowHideWidget(os);
    GTComboBox::setIndexWithText(os, GTWidget::findExactWidget<QComboBox *>(os, "cbAnnotationType"), type, true, GTGlobals::UseMouse);
}

bool checkTypePresenceInOptionsPanelWidget(HI::GUITestOpStatus &os, const QString &type) {
    GTUtilsOptionPanelSequenceView::openAnnotationParametersShowHideWidget(os);
    QComboBox *cbAnnotationType = GTWidget::findExactWidget<QComboBox *>(os, "cbAnnotationType");
    CHECK_SET_ERR_RESULT(NULL != cbAnnotationType, "cbAnnotationType is NULL", "");
    for (int i = 0; i < cbAnnotationType->count(); i++) {
        if (type == cbAnnotationType->itemText(i)) {
            return true;
        }
    }
    return false;
}

void setGroupName(HI::GUITestOpStatus &os, const QString &name, QWidget *dialog = NULL) {
    GTLineEdit::setText(os, GTWidget::findExactWidget<QLineEdit *>(os, "leGroupName", dialog), name);
}

void checkGroupName(HI::GUITestOpStatus &os, const QString &name, QWidget *dialog = NULL) {
    GTLineEdit::checkText(os, GTWidget::findExactWidget<QLineEdit *>(os, "leGroupName", dialog), name);
}

void clickSelectGroupButton(HI::GUITestOpStatus &os, QWidget *dialog = NULL) {
    GTWidget::click(os, GTWidget::findWidget(os, "tbSelectGroupName", dialog));
}

void setAnnotationName(HI::GUITestOpStatus &os, const QString &name, QWidget *dialog = NULL) {
    GTLineEdit::setText(os, GTWidget::findExactWidget<QLineEdit *>(os, "leAnnotationName", dialog), name);
}

void setSimpleLocation(HI::GUITestOpStatus &os, int startPos, int endPos, bool complement, QWidget *dialog) {
    GTRadioButton::click(os, GTWidget::findExactWidget<QRadioButton *>(os, "rbSimpleFormat", dialog));
    GTLineEdit::setText(os, GTWidget::findExactWidget<QLineEdit *>(os, "leRegionStart", dialog), QString::number(startPos));
    GTLineEdit::setText(os, GTWidget::findExactWidget<QLineEdit *>(os, "leRegionEnd", dialog), QString::number(endPos));
    GTCheckBox::setChecked(os, GTWidget::findExactWidget<QCheckBox *>(os, "chbComplement", dialog), complement);
}

void setGenbankLocation(HI::GUITestOpStatus &os, const QString &locationString, QWidget *dialog) {
    GTRadioButton::click(os, GTWidget::findExactWidget<QRadioButton *>(os, "rbGenbankFormat", dialog));
    GTLineEdit::setText(os, GTWidget::findExactWidget<QLineEdit *>(os, "leLocation", dialog), locationString);
}

void setExistingTable(HI::GUITestOpStatus &os, QWidget *dialog = NULL, const QString &tableName = "") {
    GTRadioButton::click(os, GTWidget::findExactWidget<QRadioButton *>(os, "rbExistingTable", dialog));
    if (!tableName.isEmpty()) {
        GTComboBox::setIndexWithText(os, GTWidget::findExactWidget<QComboBox *>(os, "cbExistingTable", dialog), tableName);
    }
}

void setNewTable(HI::GUITestOpStatus &os, QWidget *dialog = NULL, const QString &tablePath = "") {
    GTRadioButton::click(os, GTWidget::findExactWidget<QRadioButton *>(os, "rbCreateNewTable", dialog));
    if (!tablePath.isEmpty()) {
        GTLineEdit::setText(os, GTWidget::findExactWidget<QLineEdit *>(os, "leNewTablePath", dialog), tablePath);
    }
}

class GroupMenuChecker : public CustomScenario {
public:
    GroupMenuChecker(const QStringList &groupNames, const QString &groupToSelect = "") :
        groupNames(groupNames),
        groupToSelect(groupToSelect)
    {

    }

    void run(HI::GUITestOpStatus &os) {
        GTGlobals::sleep(1000);
        GTMouseDriver::release();
        QMenu* activePopupMenu = qobject_cast<QMenu *>(QApplication::activePopupWidget());
        CHECK_SET_ERR(NULL != activePopupMenu, "Active popup menu is NULL");

        QStringList actualGroupNames;
        foreach (QAction *action, activePopupMenu->actions()) {
            actualGroupNames << action->text();
        }

        CHECK_SET_ERR(groupNames == actualGroupNames, QString("Group names list doesn't match: expect '%1', got '%2'")
                      .arg(groupNames.join(", ")).arg(actualGroupNames.join(", ")));

        if (groupToSelect.isEmpty()) {
            GTKeyboardDriver::keyClick( Qt::Key_Escape);
        } else {
            GTMenu::clickMenuItemByText(os, activePopupMenu, QStringList() << groupToSelect);
        }
    }

private:
    const QStringList groupNames;
    const QString groupToSelect;
};

}

GUI_TEST_CLASS_DEFINITION(test_0001) {
//    Test annotation type for a full widget and nucleotide sequence.

//    1. Open "data/samples/FASTA/human_T1.fa".
//    2. Call "Create new annotation" dialog.

    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

//    3. Ensure that the default type is "Misc. Feature".
            const QString type = getTypeFromFullWidget(os, dialog);
            CHECK_SET_ERR("Misc. Feature" == type,
                          QString("An unexpected default type: expect '%1', got '%2'")
                          .arg("Misc. Feature").arg(type));

//    4. Ensure that there is no type "Kinase".
            const bool kinasePresent = checkTypePresenceInFullWidget(os, "Kinase", dialog);
            CHECK_SET_ERR(!kinasePresent,
                          QString("Amino type is unexpectedly present for nucleotide sequence: '%1'")
                          .arg("Kinase"));

//    5. Ensure that there is a type "Transit Peptide".
            const bool transitPeptidePresent = checkTypePresenceInFullWidget(os, "Transit Peptide", dialog);
            CHECK_SET_ERR(transitPeptidePresent,
                          QString("Universal type is unexpectedly missed for nucleotide sequence: '%1'")
                          .arg("Transit Peptide"));

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
    CHECK_SET_ERR("bHLH Domain" == type, QString("An unexpected annotation type: expect '%1', got '%2'")
                  .arg("bHLH Domain").arg(type));
}

GUI_TEST_CLASS_DEFINITION(test_0002) {
//    Test annotation type for an embedded widget and nucleotide sequence.

//    1. Open "data/samples/FASTA/human_T1.fa".
//    2. Call "Smith-Waterman Search" dialog.

    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

//    3. Enter any pattern that can be found. Open "Input and output" tab.
            setSmithWatermanPatternAndOpenLastTab(os, dialog);

//    4. Ensure that the default type is "Misc. Feature".
            const QString type = getTypeFromNormalWidget(os, dialog);
            CHECK_SET_ERR("Misc. Feature" == type,
                          QString("An unexpected default type: expect '%1', got '%2'")
                          .arg("Misc. Feature").arg(type));

//    5. Ensure that there is no type "Kinase".
            const bool kinasePresent = checkTypePresenceInNormalWidget(os, "Kinase", dialog);
            CHECK_SET_ERR(!kinasePresent,
                          QString("Amino type is unexpectedly present for nucleotide sequence: '%1'")
                          .arg("Kinase"));

//    6. Ensure that there is a type "Transit Peptide".
            const bool transitPeptidePresent = checkTypePresenceInNormalWidget(os, "Transit Peptide", dialog);
            CHECK_SET_ERR(transitPeptidePresent,
                          QString("Universal type is unexpectedly missed for nucleotide sequence: '%1'")
                          .arg("Transit Peptide"));

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
    CHECK_SET_ERR("bHLH Domain" == type, QString("An unexpected annotation type: expect '%1', got '%2'")
                  .arg("bHLH Domain").arg(type));
}

GUI_TEST_CLASS_DEFINITION(test_0003) {
//    Test annotation type for an options panel widget and nucleotide sequence.

//    1. Open "data/samples/FASTA/human_T1.fa".
//    2. Open "Search in Sequence" options panel tab.
//    3. Enter any pattern that can be found. Open "Annotation parameters" group.
    openFileOpenSearchTabAndSetPattern(os, dataDir + "samples/FASTA/human_T1.fa");

//    4. Ensure that the default type is "Misc. Feature".
    const QString defaultType = getTypeFromOptionsPanelWidget(os);
    CHECK_SET_ERR("Misc. Feature" == defaultType,
                  QString("An unexpected default type: expect '%1', got '%2'")
                  .arg("Misc. Feature").arg(defaultType));

//    5. Ensure that there is no type "Kinase".
    const bool kinasePresent = checkTypePresenceInOptionsPanelWidget(os, "Kinase");
    CHECK_SET_ERR(!kinasePresent,
                  QString("Amino type is unexpectedly present for nucleotide sequence: '%1'")
                  .arg("Kinase"));

//    6. Ensure that there is a type "Transit Peptide".
    const bool transitPeptidePresent = checkTypePresenceInOptionsPanelWidget(os, "Transit Peptide");
    CHECK_SET_ERR(transitPeptidePresent,
                  QString("Universal type is unexpectedly missed for nucleotide sequence: '%1'")
                  .arg("Transit Peptide"));

//    7. Select type "bHLH Domain". Create annotations from the search results.
    setTypeInOptionsPanelWidget(os, "bHLH Domain");
    setAnnotationName(os, "test_0003");
    GTUtilsOptionPanelSequenceView::clickGetAnnotation(os);

//    Expected state: there is an annotation with type "bHLH Domain".
    const QString type = GTUtilsAnnotationsTreeView::getAnnotationType(os, "test_0003");
    CHECK_SET_ERR("bHLH Domain" == type, QString("An unexpected annotation type: expect '%1', got '%2'")
                  .arg("bHLH Domain").arg(type));
}

GUI_TEST_CLASS_DEFINITION(test_0004) {
//    Test annotation type for a full widget and amino acid sequence.

//    1. Open "_common_data/fasta/AMINO.fa".
//    2. Call "Create new annotation" dialog.

    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

//    3. Ensure that the default type is "Misc. Feature".
            const QString type = getTypeFromFullWidget(os, dialog);
            CHECK_SET_ERR("Misc. Feature" == type,
                          QString("An unexpected default type: expect '%1', got '%2'")
                          .arg("Misc. Feature").arg(type));

//    4. Ensure that there is no type "bHLH Domain".
            const bool bhlhPresent = checkTypePresenceInFullWidget(os, "bHLH Domain", dialog);
            CHECK_SET_ERR(!bhlhPresent,
                          QString("Nucleotide type is unexpectedly present for amino sequence: '%1'")
                          .arg("bHLH Domain"));

//    5. Ensure that there is a type "Transit Peptide".
            const bool transitPeptidePresent = checkTypePresenceInFullWidget(os, "Transit Peptide", dialog);
            CHECK_SET_ERR(transitPeptidePresent,
                          QString("Universal type is unexpectedly missed for amino sequence: '%1'")
                          .arg("Transit Peptide"));

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
    CHECK_SET_ERR("Kinase" == type, QString("An unexpected annotation type: expect '%1', got '%2'")
                  .arg("Kinase").arg(type));
}

GUI_TEST_CLASS_DEFINITION(test_0005) {
//    Test annotation type for an embedded widget and amino acid sequence.

//    1. Open "_common_data/fasta/AMINO.fa".
//    2. Call "Smith-Waterman Search" dialog.

    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

//    3. Enter any pattern that can be found. Open "Input and output" tab.
            setSmithWatermanPatternAndOpenLastTab(os, dialog, "AAAAAACCCCCCC");

//    4. Ensure that the default type is "Misc. Feature".
            const QString type = getTypeFromNormalWidget(os, dialog);
            CHECK_SET_ERR("Misc. Feature" == type,
                          QString("An unexpected default type: expect '%1', got '%2'")
                          .arg("Misc. Feature").arg(type));

//    5. Ensure that there is no type "bHLH Domain".
            const bool bhlhPresent = checkTypePresenceInNormalWidget(os, "bHLH Domain", dialog);
            CHECK_SET_ERR(!bhlhPresent,
                          QString("Nucleotide type is unexpectedly present for amino sequence: '%1'")
                          .arg("bHLH Domain"));

//    6. Ensure that there is a type "Transit Peptide".
            const bool transitPeptidePresent = checkTypePresenceInNormalWidget(os, "Transit Peptide", dialog);
            CHECK_SET_ERR(transitPeptidePresent,
                          QString("Universal type is unexpectedly missed for amino sequence: '%1'")
                          .arg("Transit Peptide"));

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
    CHECK_SET_ERR("Kinase" == type, QString("An unexpected annotation type: expect '%1', got '%2'")
                  .arg("Kinase").arg(type));
}

GUI_TEST_CLASS_DEFINITION(test_0006) {
//    Test annotation type for an options panel widget and amino acid sequence.

//    1. Open "_common_data/fasta/AMINO.fa".
//    2. Open "Search in Sequence" options panel tab.
//    3. Enter any pattern that can be found. Open "Annotation parameters" group.
        openFileOpenSearchTabAndSetPattern(os, testDir + "_common_data/fasta/AMINO.fa", "DDDEEEEEEE");

//    4. Ensure that the default type is "Misc. Feature".
        const QString defaultType = getTypeFromOptionsPanelWidget(os);
        CHECK_SET_ERR("Misc. Feature" == defaultType,
                      QString("An unexpected default type: expect '%1', got '%2'")
                      .arg("Misc. Feature").arg(defaultType));

//    5. Ensure that there is no type "bHLH Domain".
        const bool bhlhPresent = checkTypePresenceInOptionsPanelWidget(os, "bHLH Domain");
        CHECK_SET_ERR(!bhlhPresent,
                      QString("Nucleotide type is unexpectedly present for amino sequence: '%1'")
                      .arg("bHLH Domain"));

//    6. Ensure that there is a type "Transit Peptide".
        const bool transitPeptidePresent = checkTypePresenceInOptionsPanelWidget(os, "Transit Peptide");
        CHECK_SET_ERR(transitPeptidePresent,
                      QString("Universal type is unexpectedly missed for amino sequence: '%1'")
                      .arg("Transit Peptide"));

//    7. Select type "Kinase". Create annotations from the search results.
        setTypeInOptionsPanelWidget(os, "Kinase");
        setAnnotationName(os, "test_0006");
        GTUtilsOptionPanelSequenceView::clickGetAnnotation(os);

//    Expected state: there is an annotation with type "Kinase".
        const QString type = GTUtilsAnnotationsTreeView::getAnnotationType(os, "test_0006");
        CHECK_SET_ERR("Kinase" == type, QString("An unexpected annotation type: expect '%1', got '%2'")
                      .arg("Kinase").arg(type));
}

GUI_TEST_CLASS_DEFINITION(test_0007) {
//    Test annotation selection by the keyboard

//    1. Open "data/samples/FASTA/human_T1.fa".
//    2. Call "Create new annotation" dialog.

    class CreateAnnotationDialogScenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

//    3. Enter "tel".
            GTKeyboardDriver::keySequence("tel");
            GTGlobals::sleep(500);

//    Expected state: "Telomere" type is selected. Cancel the dialog.
            const QString type = getTypeFromFullWidget(os, dialog);
            CHECK_SET_ERR("Telomere" == type,
                          QString("1: An unexpected feature type: expect '%1', got '%2'")
                          .arg("Telomere").arg(type));

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, new CreateAnnotationDialogScenario));
    openFileAndCallCreateAnnotationDialog(os, dataDir + "samples/FASTA/human_T1.fa");

//    4. Call "Smith-Waterman Search" dialog. Open "Input and output" tab.

    class SmithWatermanDialogScenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

            GTTabWidget::setCurrentIndex(os, GTWidget::findExactWidget<QTabWidget *>(os, "tabWidget", dialog), 1);

//    5. Click to the annotation type combobox. Enter "tel". Click "Enter".
            GTWidget::click(os, GTWidget::findExactWidget<QComboBox *>(os, "cbAnnotationType", dialog));
            GTKeyboardDriver::keySequence("tel");
            GTGlobals::sleep();
            GTKeyboardDriver::keyClick( Qt::Key_Enter);
            GTGlobals::sleep();

//    Expected state: "Telomere" type is selected. Cancel the dialog.
            const QString type = getTypeFromNormalWidget(os, dialog);
            CHECK_SET_ERR("Telomere" == type,
                          QString("2: An unexpected feature type: expect '%1', got '%2'")
                          .arg("Telomere").arg(type));

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(os, new SmithWatermanDialogFiller(os, new SmithWatermanDialogScenario));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Find pattern [Smith-Waterman]");

//    6. Open "Search in Sequence" options panel tab. Open "Annotation parameters" group.
    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::Search);
    GTUtilsOptionPanelSequenceView::openAnnotationParametersShowHideWidget(os);

//    7. Click to the annotation type combobox. Enter "tel". Click "Enter".
//    Expected state: "Telomere" type is selected. Cancel the dialog.
    GTWidget::click(os, GTWidget::findExactWidget<QComboBox *>(os, "cbAnnotationType"));
    GTKeyboardDriver::keySequence("tel");
    GTGlobals::sleep();
    GTKeyboardDriver::keyClick( Qt::Key_Enter);
    GTGlobals::sleep();

    const QString type = getTypeFromOptionsPanelWidget(os);
    CHECK_SET_ERR("Telomere" == type,
                  QString("3: An unexpected feature type: expect '%1', got '%2'")
                  .arg("Telomere").arg(type));
}

GUI_TEST_CLASS_DEFINITION(test_0008) {
//    Test default group name on sequence without annotation table in a full widget.

//    1. Open "data/samples/FASTA/human_T1.fa".
//    2. Call "Create new annotation" dialog.

    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

//    3. Ensure that the group name is "<auto>".
            checkGroupName(os, "<auto>", dialog);

//    4. Click "Predefined group names" button.
//    Expected state: nothing happens, there is no popup menus.
            GTUtilsDialog::waitForDialogWhichMustNotBeRun(os, new EscapeClicker(os));
            clickSelectGroupButton(os, dialog);

//    5. Set the annotation name. Accept the dialog.
            setAnnotationName(os, "test_0008", dialog);
            setGenbankLocation(os, "100..200", dialog);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, new Scenario));
    openFileAndCallCreateAnnotationDialog(os, dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    Expected state: the group has the same name as the annotation.
    const QStringList expectedGroupNames = QStringList() << "test_0008  (0, 1)";
    const QStringList groupNames = GTUtilsAnnotationsTreeView::getGroupNames(os);
    CHECK_SET_ERR(expectedGroupNames == groupNames, QString("Unexpected group names: expect '%1', got '%2'")
                  .arg(expectedGroupNames.join(", ").arg(groupNames.join(", "))));

    const QStringList expectedAnnotationNames = QStringList() << "test_0008";
    const QStringList annotationNames = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "test_0008  (0, 1)");
    CHECK_SET_ERR(expectedAnnotationNames == annotationNames, QString("Unexpected annotation names: expect '%1', got '%2'")
                  .arg(expectedAnnotationNames.join(", ")).arg(annotationNames.join(", ")));
}

GUI_TEST_CLASS_DEFINITION(test_0009) {
//    Test default group name on sequence without annotation table in an embedded widget.

//    1. Open "data/samples/FASTA/human_T1.fa".
//    2. Call "Smith-Waterman Search" dialog. Set any pattern. Open "Input and output" tab.

    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

            setSmithWatermanPatternAndOpenLastTab(os, dialog);

//    3. Ensure that the group name is "<auto>".
            checkGroupName(os, "<auto>", dialog);

//    4. Click "Predefined group names" button.
//    Expected state: nothing happens, there is no popup menus.
            GTUtilsDialog::waitForDialogWhichMustNotBeRun(os, new EscapeClicker(os));
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
    const QStringList expectedGroupNames = QStringList() << "test_0009  (0, 1)";
    const QStringList groupNames = GTUtilsAnnotationsTreeView::getGroupNames(os);
    CHECK_SET_ERR(expectedGroupNames == groupNames, QString("Unexpected group names: expect '%1', got '%2'")
                  .arg(expectedGroupNames.join(", ").arg(groupNames.join(", "))));

    const QStringList expectedAnnotationNames = QStringList() << "test_0009";
    const QStringList annotationNames = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "test_0009  (0, 1)");
    CHECK_SET_ERR(expectedAnnotationNames == annotationNames, QString("Unexpected annotation names: expect '%1', got '%2'")
                  .arg(expectedAnnotationNames.join(", ")).arg(annotationNames.join(", ")));
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
//    Expected state: nothing happens, there is no popup menus.
    GTUtilsDialog::waitForDialogWhichMustNotBeRun(os, new EscapeClicker(os));
    clickSelectGroupButton(os);

//    5. Set the annotation name. Create annotations.
    setAnnotationName(os, "test_0010");
    GTUtilsOptionPanelSequenceView::clickGetAnnotation(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    Expected state: the group has the same name as the annotation.
    const QStringList expectedGroupNames = QStringList() << "test_0010  (0, 1)";
    const QStringList groupNames = GTUtilsAnnotationsTreeView::getGroupNames(os);
    CHECK_SET_ERR(expectedGroupNames == groupNames, QString("Unexpected group names: expect '%1', got '%2'")
                  .arg(expectedGroupNames.join(", ").arg(groupNames.join(", "))));

    const QStringList expectedAnnotationNames = QStringList() << "test_0010";
    const QStringList annotationNames = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "test_0010  (0, 1)");
    CHECK_SET_ERR(expectedAnnotationNames == annotationNames, QString("Unexpected annotation names: expect '%1', got '%2'")
                  .arg(expectedAnnotationNames.join(", ")).arg(annotationNames.join(", ")));
}

GUI_TEST_CLASS_DEFINITION(test_0011) {
//    Test group name on sequence without annotation table in a full widget.

//    1. Open "data/samples/FASTA/human_T1.fa".
//    2. Call "Create new annotation" dialog.

    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

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
    const QStringList expectedGroupNames = QStringList() << "test_0011  (0, 1)";
    const QStringList groupNames = GTUtilsAnnotationsTreeView::getGroupNames(os);
    CHECK_SET_ERR(expectedGroupNames == groupNames, QString("Unexpected group names: expect '%1', got '%2'")
                  .arg(expectedGroupNames.join(", ").arg(groupNames.join(", "))));

    const QStringList annotationNames = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "test_0011  (0, 1)");
    CHECK_SET_ERR(1 == annotationNames.size(), QString("Unexpected annotations count: expect '%1', got '%2'")
                  .arg(1).arg(annotationNames.size()));
}

GUI_TEST_CLASS_DEFINITION(test_0012) {
//    Test group name on sequence without annotation table in an embedded widget.

//    1. Open "data/samples/FASTA/human_T1.fa".
//    2. Call "Smith-Waterman Search" dialog. Set any pattern. Open "Input and output" tab.

    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

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
    const QStringList expectedGroupNames = QStringList() << "test_0012  (0, 1)";
    const QStringList groupNames = GTUtilsAnnotationsTreeView::getGroupNames(os);
    CHECK_SET_ERR(expectedGroupNames == groupNames, QString("Unexpected group names: expect '%1', got '%2'")
                  .arg(expectedGroupNames.join(", ").arg(groupNames.join(", "))));

    const QStringList annotationNames = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "test_0012  (0, 1)");
    CHECK_SET_ERR(1 == annotationNames.size(), QString("Unexpected annotations count: expect '%1', got '%2'")
                  .arg(1).arg(annotationNames.size()));
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
    const QStringList expectedGroupNames = QStringList() << "test_0013  (0, 1)";
    const QStringList groupNames = GTUtilsAnnotationsTreeView::getGroupNames(os);
    CHECK_SET_ERR(expectedGroupNames == groupNames, QString("Unexpected group names: expect '%1', got '%2'")
                  .arg(expectedGroupNames.join(", ").arg(groupNames.join(", "))));

    const QStringList annotationNames = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "test_0013  (0, 1)");
    CHECK_SET_ERR(1 == annotationNames.size(), QString("Unexpected annotations count: expect '%1', got '%2'")
                  .arg(1).arg(annotationNames.size()));
}

GUI_TEST_CLASS_DEFINITION(test_0014) {
//    Test default group name on sequence with an annotation table in a full widget.

//    1. Open "data/samples/Genbank/murine.gb".
//    2. Call "Create new annotation" dialog.

    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

//    3. Click "Predefined group names" button.
//    Expected state: a popup menu is shown, it contains all groups from the existing annotation table.
            const QStringList expectedGroups = QStringList() << "<auto>" << "CDS" << "comment" << "misc_feature" << "source";
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
    const QStringList expectedGroupNames = QStringList() << "CDS  (0, 4)"
                                                         << "comment  (0, 1)"
                                                         << "misc_feature  (0, 2)"
                                                         << "source  (0, 1)"
                                                         << "Insertion  (0, 1)";
    const QStringList groupNames = GTUtilsAnnotationsTreeView::getGroupNames(os);
    CHECK_SET_ERR(expectedGroupNames.toSet() == groupNames.toSet(), QString("Unexpected group names: expect '%1', got '%2'")
                  .arg(expectedGroupNames.join(", ").arg(groupNames.join(", "))));

    const QStringList annotationNames = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "Insertion  (0, 1)");
    CHECK_SET_ERR(1 == annotationNames.size(), QString("Unexpected annotations count: expect '%1', got '%2'")
                  .arg(1).arg(annotationNames.size()));
}

GUI_TEST_CLASS_DEFINITION(test_0015) {
//    Test default group name on sequence with an annotation table in an embedded widget.

//    1. Open "data/samples/Genbank/murine.gb".
//    2. Call "Smith-Waterman Search" dialog. Set any pattern. Open "Input and output" tab.

    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

            setSmithWatermanPatternAndOpenLastTab(os, dialog, "GATTTTATTTAGTCTCCAG");

//    3. Click "Predefined group names" button.
//    Expected state: a popup menu is shown, it contains all groups from the existing annotation table.
            const QStringList expectedGroups = QStringList() << "<auto>" << "CDS" << "comment" << "misc_feature" << "source";
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
    const QStringList expectedGroupNames = QStringList() << "CDS  (0, 4)"
                                                         << "comment  (0, 1)"
                                                         << "misc_feature  (0, 2)"
                                                         << "source  (0, 1)"
                                                         << "Insertion  (0, 1)";
    const QStringList groupNames = GTUtilsAnnotationsTreeView::getGroupNames(os);
    CHECK_SET_ERR(expectedGroupNames.toSet() == groupNames.toSet(), QString("Unexpected group names: expect '%1', got '%2'")
                  .arg(expectedGroupNames.join(", ").arg(groupNames.join(", "))));

    const QStringList annotationNames = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "Insertion  (0, 1)");
    CHECK_SET_ERR(1 == annotationNames.size(), QString("Unexpected annotations count: expect '%1', got '%2'")
                  .arg(1).arg(annotationNames.size()));
}

GUI_TEST_CLASS_DEFINITION(test_0016) {
//    Test default group name on sequence with an annotation table in an options panel widget.

//    1. Open "data/samples/Genbank/murine.gb".
//    2. Open "Search in Sequence" options panel tab. Set any pattern. Open "Annotation Parameters" group.
    openFileOpenSearchTabAndSetPattern(os, dataDir + "samples/Genbank/murine.gb", "GATTTTATTTAGTCTCCAG");
    GTUtilsOptionPanelSequenceView::openAnnotationParametersShowHideWidget(os);

//    3. Click "Predefined group names" button.
//    Expected state: a popup menu is shown, it contains all groups from the existing annotation table.
    const QStringList expectedGroups = QStringList() << "<auto>" << "CDS" << "comment" << "misc_feature" << "source";
    GTUtilsDialog::waitForDialog(os, new PopupChecker(os, new GroupMenuChecker(expectedGroups)));
    clickSelectGroupButton(os);

//    4. Close the menu. Set "Insertion" type. Create annotations.
    setTypeInOptionsPanelWidget(os, "Insertion");

    GTUtilsOptionPanelSequenceView::clickGetAnnotation(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    Expected state: there is an additional "Insertion" group with an annotation inside.
    const QStringList expectedGroupNames = QStringList() << "CDS  (0, 4)"
                                                         << "comment  (0, 1)"
                                                         << "misc_feature  (0, 2)"
                                                         << "source  (0, 1)"
                                                         << "Insertion  (0, 1)";
    const QStringList groupNames = GTUtilsAnnotationsTreeView::getGroupNames(os);
    CHECK_SET_ERR(expectedGroupNames.toSet() == groupNames.toSet(), QString("Unexpected group names: expect '%1', got '%2'")
                  .arg(expectedGroupNames.join(", ").arg(groupNames.join(", "))));

    const QStringList annotationNames = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "Insertion  (0, 1)");
    CHECK_SET_ERR(1 == annotationNames.size(), QString("Unexpected annotations count: expect '%1', got '%2'")
                  .arg(1).arg(annotationNames.size()));
}

GUI_TEST_CLASS_DEFINITION(test_0017) {
//    Test non-default group name on sequence with an annotation table in a full widget.

//    1. Open "data/samples/Genbank/murine.gb".
//    2. Call "Create new annotation" dialog.

    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

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
    const QStringList expectedGroupNames = QStringList() << "CDS  (0, 4)"
                                                         << "comment  (0, 1)"
                                                         << "misc_feature  (0, 2)"
                                                         << "source  (0, 1)"
                                                         << "test_0017  (0, 1)";
    const QStringList groupNames = GTUtilsAnnotationsTreeView::getGroupNames(os);
    CHECK_SET_ERR(expectedGroupNames.toSet() == groupNames.toSet(), QString("Unexpected group names: expect '%1', got '%2'")
                  .arg(expectedGroupNames.join(", ")).arg(groupNames.join(", ")));

    const QStringList annotationNames = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "test_0017  (0, 1)");
    CHECK_SET_ERR(1 == annotationNames.size(), QString("Unexpected annotations count: expect '%1', got '%2'")
                  .arg(1).arg(annotationNames.size()));
}

GUI_TEST_CLASS_DEFINITION(test_0018) {
//    Test non-default group name on sequence with an annotation table in an embedded widget.

//    1. Open "data/samples/Genbank/murine.gb".
//    2. Call "Smith-Waterman Search" dialog. Set any pattern. Open "Input and output" tab.

    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

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
    const QStringList expectedGroupNames = QStringList() << "CDS  (0, 4)"
                                                         << "comment  (0, 1)"
                                                         << "misc_feature  (0, 2)"
                                                         << "source  (0, 1)"
                                                         << "test_0018  (0, 1)";
    const QStringList groupNames = GTUtilsAnnotationsTreeView::getGroupNames(os);
    CHECK_SET_ERR(expectedGroupNames.toSet() == groupNames.toSet(), QString("Unexpected group names: expect '%1', got '%2'")
                  .arg(expectedGroupNames.join(", ")).arg(groupNames.join(", ")));

    const QStringList annotationNames = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "test_0018  (0, 1)");
    CHECK_SET_ERR(1 == annotationNames.size(), QString("Unexpected annotations count: expect '%1', got '%2'")
                  .arg(1).arg(annotationNames.size()));
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
    const QStringList expectedGroupNames = QStringList() << "CDS  (0, 4)"
                                                         << "comment  (0, 1)"
                                                         << "misc_feature  (0, 2)"
                                                         << "source  (0, 1)"
                                                         << "test_0019  (0, 1)";
    const QStringList groupNames = GTUtilsAnnotationsTreeView::getGroupNames(os);
    CHECK_SET_ERR(expectedGroupNames.toSet() == groupNames.toSet(), QString("Unexpected group names: expect '%1', got '%2'")
                  .arg(expectedGroupNames.join(", ")).arg(groupNames.join(", ")));

    const QStringList annotationNames = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "test_0019  (0, 1)");
    CHECK_SET_ERR(1 == annotationNames.size(), QString("Unexpected annotations count: expect '%1', got '%2'")
                  .arg(1).arg(annotationNames.size()));
}

GUI_TEST_CLASS_DEFINITION(test_0020) {
//    Test predefined group name on sequence with an annotation table in a full widget.

//    1. Open "data/samples/Genbank/murine.gb".
//    2. Call "Create new annotation" dialog.

    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

//    3. Click the "Predefined group names" button.
//    Expected state: a popup menu with possible group names is shown.
//    4. Select "CDS" group. Accept the dialog.
            GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "CDS"));
            clickSelectGroupButton(os, dialog);
            setGenbankLocation(os, "100..200", dialog);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, new Scenario));
    openFileAndCallCreateAnnotationDialog(os, dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    Expected state: there is an additional annotation in the "CDS" group.
    const QStringList expectedGroupNames = QStringList() << "CDS  (0, 5)"
                                                         << "comment  (0, 1)"
                                                         << "misc_feature  (0, 2)"
                                                         << "source  (0, 1)";
    const QStringList groupNames = GTUtilsAnnotationsTreeView::getGroupNames(os);
    CHECK_SET_ERR(expectedGroupNames.toSet() == groupNames.toSet(), QString("Unexpected group names: expect '%1', got '%2'")
                  .arg(expectedGroupNames.join(", ")).arg(groupNames.join(", ")));

    const QStringList annotationNames = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "CDS  (0, 5)");
    CHECK_SET_ERR(5 == annotationNames.size(), QString("Unexpected annotations count: expect '%1', got '%2'")
                  .arg(5).arg(annotationNames.size()));
}

GUI_TEST_CLASS_DEFINITION(test_0021) {
//    Test predefined group name on sequence with an annotation table in an embedded widget.

//    1. Open "data/samples/Genbank/murine.gb".
//    2. Call "Smith-Waterman Search" dialog. Set any pattern. Open "Input and output" tab.

    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

            setSmithWatermanPatternAndOpenLastTab(os, dialog, "GATTTTATTTAGTCTCCAG");

//    3. Click the "Predefined group names" button.
//    Expected state: a popup menu with possible group names is shown.
//    4. Select "CDS" group. Accept the dialog.
            GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "CDS"));
            clickSelectGroupButton(os, dialog);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new SmithWatermanDialogFiller(os, new Scenario));
    openFileAndCallSmithWatermanDialog(os, dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    Expected state: there is an additional annotation in the "CDS" group.
    const QStringList expectedGroupNames = QStringList() << "CDS  (0, 5)"
                                                         << "comment  (0, 1)"
                                                         << "misc_feature  (0, 2)"
                                                         << "source  (0, 1)";
    const QStringList groupNames = GTUtilsAnnotationsTreeView::getGroupNames(os);
    CHECK_SET_ERR(expectedGroupNames.toSet() == groupNames.toSet(), QString("Unexpected group names: expect '%1', got '%2'")
                  .arg(expectedGroupNames.join(", ")).arg(groupNames.join(", ")));

    const QStringList annotationNames = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "CDS  (0, 5)");
    CHECK_SET_ERR(5 == annotationNames.size(), QString("Unexpected annotations count: expect '%1', got '%2'")
                  .arg(5).arg(annotationNames.size()));
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
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "CDS"));
    clickSelectGroupButton(os);

    GTUtilsOptionPanelSequenceView::clickGetAnnotation(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    Expected state: there is an additional annotation in the "CDS" group.
    const QStringList expectedGroupNames = QStringList() << "CDS  (0, 5)"
                                                         << "comment  (0, 1)"
                                                         << "misc_feature  (0, 2)"
                                                         << "source  (0, 1)";
    const QStringList groupNames = GTUtilsAnnotationsTreeView::getGroupNames(os);
    CHECK_SET_ERR(expectedGroupNames.toSet() == groupNames.toSet(), QString("Unexpected group names: expect '%1', got '%2'")
                  .arg(expectedGroupNames.join(", ")).arg(groupNames.join(", ")));

    const QStringList annotationNames = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "CDS  (0, 5)");
    CHECK_SET_ERR(5 == annotationNames.size(), QString("Unexpected annotations count: expect '%1', got '%2'")
                  .arg(5).arg(annotationNames.size()));
}

GUI_TEST_CLASS_DEFINITION(test_0023) {
//    Test annotation name in a full widget.

//    1. Open "data/samples/FASTA/human_T1.fa".
//    2. Call "Create new annotation" dialog.

    class Scenario1 : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

//    3. Use default type, group and name. Accept the dialog.
            setGenbankLocation(os, "100..200", dialog);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, new Scenario1));
    openFileAndCallCreateAnnotationDialog(os, dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    Expected state: there is an annotation named "Misc. Feature" of "Misc. Feature" type in a group named "Misc. Feature".
    QStringList expectedGroupNames = QStringList() << "Misc. Feature  (0, 1)";
    QStringList groupNames = GTUtilsAnnotationsTreeView::getGroupNames(os);
    CHECK_SET_ERR(expectedGroupNames.toSet() == groupNames.toSet(), QString("Unexpected group names: expect '%1', got '%2'")
                  .arg(expectedGroupNames.join(", ")).arg(groupNames.join(", ")));

    QStringList expectedAnnotationNames = QStringList() << "Misc. Feature";
    QStringList annotationNames = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "Misc. Feature  (0, 1)");
    CHECK_SET_ERR(expectedAnnotationNames == annotationNames, QString("Unexpected annotation names: expect '%1', got '%2'")
                  .arg(expectedAnnotationNames.join(", ")).arg(annotationNames.join(", ")));

    QString expectedAnnotationType = "Misc. Feature";
    QString annotationType = GTUtilsAnnotationsTreeView::getAnnotationType(os, "Misc. Feature");
    CHECK_SET_ERR(expectedAnnotationType == annotationType, QString("Unexpected annotation type: expect '%1', got '%2'")
                  .arg(expectedAnnotationType).arg(annotationType));

    GTUtilsAnnotationsTreeView::deleteItem(os, "Misc. Feature  (0, 1)");

//    4. Call "Create new annotation" dialog.

    class Scenario2 : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

//    5. Set any name, use default group and type. Accept the dialog.
            setAnnotationName(os, "test_0023", dialog);
            setGenbankLocation(os, "200..300", dialog);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, new Scenario2));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "New annotation");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    Expected state: there is a correctly named annotation of "Misc. Feature" type in a group named the same as the annotation.
    expectedGroupNames = QStringList() << "test_0023  (0, 1)";
    groupNames = GTUtilsAnnotationsTreeView::getGroupNames(os);
    CHECK_SET_ERR(expectedGroupNames.toSet() == groupNames.toSet(), QString("Unexpected group names: expect '%1', got '%2'")
                  .arg(expectedGroupNames.join(", ")).arg(groupNames.join(", ")));

    expectedAnnotationNames = QStringList() << "test_0023";
    annotationNames = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "test_0023  (0, 1)");
    CHECK_SET_ERR(expectedAnnotationNames == annotationNames, QString("Unexpected annotation names: expect '%1', got '%2'")
                  .arg(expectedAnnotationNames.join(", ")).arg(annotationNames.join(", ")));

    expectedAnnotationType = "Misc. Feature";
    annotationType = GTUtilsAnnotationsTreeView::getAnnotationType(os, "test_0023");
    CHECK_SET_ERR(expectedAnnotationType == annotationType, QString("Unexpected annotation type: expect '%1', got '%2'")
                  .arg(expectedAnnotationType).arg(annotationType));

    GTUtilsAnnotationsTreeView::deleteItem(os, "test_0023  (0, 1)");

//    6. Call "Create new annotation" dialog.

    class Scenario3 : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

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
    CHECK_SET_ERR(expectedGroupNames.toSet() == groupNames.toSet(), QString("Unexpected group names: expect '%1', got '%2'")
                  .arg(expectedGroupNames.join(", ")).arg(groupNames.join(", ")));

    expectedAnnotationNames = QStringList() << "Loci";
    annotationNames = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "Loci  (0, 1)");
    CHECK_SET_ERR(expectedAnnotationNames == annotationNames, QString("Unexpected annotation names: expect '%1', got '%2'")
                  .arg(expectedAnnotationNames.join(", ")).arg(annotationNames.join(", ")));

    expectedAnnotationType = "Loci";
    annotationType = GTUtilsAnnotationsTreeView::getAnnotationType(os, "Loci");
    CHECK_SET_ERR(expectedAnnotationType == annotationType, QString("Unexpected annotation type: expect '%1', got '%2'")
                  .arg(expectedAnnotationType).arg(annotationType));

    GTUtilsAnnotationsTreeView::deleteItem(os, "Loci  (0, 1)");

//    8. Call "Create new annotation" dialog.

    class Scenario4 : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

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
    CHECK_SET_ERR(expectedGroupNames.toSet() == groupNames.toSet(), QString("Unexpected group names: expect '%1', got '%2'")
                  .arg(expectedGroupNames.join(", ")).arg(groupNames.join(", ")));

    expectedAnnotationNames = QStringList() << "test_0023 again";
    annotationNames = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "test_0023 again  (0, 1)");
    CHECK_SET_ERR(expectedAnnotationNames == annotationNames, QString("Unexpected annotation names: expect '%1', got '%2'")
                  .arg(expectedAnnotationNames.join(", ")).arg(annotationNames.join(", ")));

    expectedAnnotationType = "Attenuator";
    annotationType = GTUtilsAnnotationsTreeView::getAnnotationType(os, "test_0023 again");
    CHECK_SET_ERR(expectedAnnotationType == annotationType, QString("Unexpected annotation type: expect '%1', got '%2'")
                  .arg(expectedAnnotationType).arg(annotationType));
}

GUI_TEST_CLASS_DEFINITION(test_0024) {
//    Test annotation name in an embedded widget.

//    1. Open "data/samples/FASTA/human_T1.fa".
//    2. Call "Smith-Waterman Search" dialog. Set any pattern. Open "Input and output" tab.

    class Scenario1 : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

            setSmithWatermanPatternAndOpenLastTab(os, dialog);

//    3. Use default type, group and name. Accept the dialog.
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new SmithWatermanDialogFiller(os, new Scenario1));
    openFileAndCallSmithWatermanDialog(os, dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    Expected state: there is an annotation named "Misc. Feature" of "Misc. Feature" type in a group named "Misc. Feature".
    QStringList expectedGroupNames = QStringList() << "Misc. Feature  (0, 1)";
    QStringList groupNames = GTUtilsAnnotationsTreeView::getGroupNames(os);
    CHECK_SET_ERR(expectedGroupNames.toSet() == groupNames.toSet(), QString("Unexpected group names: expect '%1', got '%2'")
                  .arg(expectedGroupNames.join(", ")).arg(groupNames.join(", ")));

    QStringList expectedAnnotationNames = QStringList() << "Misc. Feature";
    QStringList annotationNames = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "Misc. Feature  (0, 1)");
    CHECK_SET_ERR(expectedAnnotationNames == annotationNames, QString("Unexpected annotation names: expect '%1', got '%2'")
                  .arg(expectedAnnotationNames.join(", ")).arg(annotationNames.join(", ")));

    QString expectedAnnotationType = "Misc. Feature";
    QString annotationType = GTUtilsAnnotationsTreeView::getAnnotationType(os, "Misc. Feature");
    CHECK_SET_ERR(expectedAnnotationType == annotationType, QString("Unexpected annotation type: expect '%1', got '%2'")
                  .arg(expectedAnnotationType).arg(annotationType));

    GTUtilsAnnotationsTreeView::deleteItem(os, "Misc. Feature  (0, 1)");

//    4. Call "Smith-Waterman Search" dialog. Set any pattern. Open "Input and output" tab.

    class Scenario2 : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

            setSmithWatermanPatternAndOpenLastTab(os, dialog);

//    5. Set any name, use default group and type. Accept the dialog.
            setAnnotationName(os, "test_0024", dialog);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new SmithWatermanDialogFiller(os, new Scenario2));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Find pattern [Smith-Waterman]");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    Expected state: there is a correctly named annotation of "Misc. Feature" type in a group named the same as the annotation.
    expectedGroupNames = QStringList() << "test_0024  (0, 1)";
    groupNames = GTUtilsAnnotationsTreeView::getGroupNames(os);
    CHECK_SET_ERR(expectedGroupNames.toSet() == groupNames.toSet(), QString("Unexpected group names: expect '%1', got '%2'")
                  .arg(expectedGroupNames.join(", ")).arg(groupNames.join(", ")));

    expectedAnnotationNames = QStringList() << "test_0024";
    annotationNames = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "test_0024  (0, 1)");
    CHECK_SET_ERR(expectedAnnotationNames == annotationNames, QString("Unexpected annotation names: expect '%1', got '%2'")
                  .arg(expectedAnnotationNames.join(", ")).arg(annotationNames.join(", ")));

    expectedAnnotationType = "Misc. Feature";
    annotationType = GTUtilsAnnotationsTreeView::getAnnotationType(os, "test_0024");
    CHECK_SET_ERR(expectedAnnotationType == annotationType, QString("Unexpected annotation type: expect '%1', got '%2'")
                  .arg(expectedAnnotationType).arg(annotationType));

    GTUtilsAnnotationsTreeView::deleteItem(os, "test_0024  (0, 1)");

//    6. Call "Smith-Waterman Search" dialog. Set any pattern. Open "Input and output" tab.

    class Scenario3 : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

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
    CHECK_SET_ERR(expectedGroupNames.toSet() == groupNames.toSet(), QString("Unexpected group names: expect '%1', got '%2'")
                  .arg(expectedGroupNames.join(", ")).arg(groupNames.join(", ")));

    expectedAnnotationNames = QStringList() << "Loci";
    annotationNames = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "Loci  (0, 1)");
    CHECK_SET_ERR(expectedAnnotationNames == annotationNames, QString("Unexpected annotation names: expect '%1', got '%2'")
                  .arg(expectedAnnotationNames.join(", ")).arg(annotationNames.join(", ")));

    expectedAnnotationType = "Loci";
    annotationType = GTUtilsAnnotationsTreeView::getAnnotationType(os, "Loci");
    CHECK_SET_ERR(expectedAnnotationType == annotationType, QString("Unexpected annotation type: expect '%1', got '%2'")
                  .arg(expectedAnnotationType).arg(annotationType));

    GTUtilsAnnotationsTreeView::deleteItem(os, "Loci  (0, 1)");

//    8. Call "Smith-Waterman Search" dialog. Set any pattern. Open "Input and output" tab.

    class Scenario4 : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

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
    CHECK_SET_ERR(expectedGroupNames.toSet() == groupNames.toSet(), QString("Unexpected group names: expect '%1', got '%2'")
                  .arg(expectedGroupNames.join(", ")).arg(groupNames.join(", ")));

    expectedAnnotationNames = QStringList() << "test_0024 again";
    annotationNames = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "test_0024 again  (0, 1)");
    CHECK_SET_ERR(expectedAnnotationNames == annotationNames, QString("Unexpected annotation names: expect '%1', got '%2'")
                  .arg(expectedAnnotationNames.join(", ")).arg(annotationNames.join(", ")));

    expectedAnnotationType = "Attenuator";
    annotationType = GTUtilsAnnotationsTreeView::getAnnotationType(os, "test_0024 again");
    CHECK_SET_ERR(expectedAnnotationType == annotationType, QString("Unexpected annotation type: expect '%1', got '%2'")
                  .arg(expectedAnnotationType).arg(annotationType));
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

//    Expected state: there is an annotation named "Misc. Feature" of "Misc. Feature" type in a group named "Misc. Feature".
    QStringList expectedGroupNames = QStringList() << "Misc. Feature  (0, 1)";
    QStringList groupNames = GTUtilsAnnotationsTreeView::getGroupNames(os);
    CHECK_SET_ERR(expectedGroupNames.toSet() == groupNames.toSet(), QString("Unexpected group names: expect '%1', got '%2'")
                  .arg(expectedGroupNames.join(", ")).arg(groupNames.join(", ")));

    QStringList expectedAnnotationNames = QStringList() << "Misc. Feature";
    QStringList annotationNames = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "Misc. Feature  (0, 1)");
    CHECK_SET_ERR(expectedAnnotationNames == annotationNames, QString("Unexpected annotation names: expect '%1', got '%2'")
                  .arg(expectedAnnotationNames.join(", ")).arg(annotationNames.join(", ")));

    QString expectedAnnotationType = "Misc. Feature";
    QString annotationType = GTUtilsAnnotationsTreeView::getAnnotationType(os, "Misc. Feature");
    CHECK_SET_ERR(expectedAnnotationType == annotationType, QString("Unexpected annotation type: expect '%1', got '%2'")
                  .arg(expectedAnnotationType).arg(annotationType));

    GTUtilsAnnotationsTreeView::deleteItem(os, "Misc. Feature  (0, 1)");
    setGroupName(os, "");
    setAnnotationName(os, "");

//    4. Set any name, use default group and type. Click "Create annotations" button.
    setAnnotationName(os, "test_0025");

    GTUtilsOptionPanelSequenceView::clickGetAnnotation(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    Expected state: there is a correctly named annotation of "Misc. Feature" type in a group named the same as the annotation.
    expectedGroupNames = QStringList() << "test_0025  (0, 1)";
    groupNames = GTUtilsAnnotationsTreeView::getGroupNames(os);
    CHECK_SET_ERR(expectedGroupNames.toSet() == groupNames.toSet(), QString("Unexpected group names: expect '%1', got '%2'")
                  .arg(expectedGroupNames.join(", ")).arg(groupNames.join(", ")));

    expectedAnnotationNames = QStringList() << "test_0025";
    annotationNames = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "test_0025  (0, 1)");
    CHECK_SET_ERR(expectedAnnotationNames == annotationNames, QString("Unexpected annotation names: expect '%1', got '%2'")
                  .arg(expectedAnnotationNames.join(", ")).arg(annotationNames.join(", ")));

    expectedAnnotationType = "Misc. Feature";
    annotationType = GTUtilsAnnotationsTreeView::getAnnotationType(os, "test_0025");
    CHECK_SET_ERR(expectedAnnotationType == annotationType, QString("Unexpected annotation type: expect '%1', got '%2'")
                  .arg(expectedAnnotationType).arg(annotationType));

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
    CHECK_SET_ERR(expectedGroupNames.toSet() == groupNames.toSet(), QString("Unexpected group names: expect '%1', got '%2'")
                  .arg(expectedGroupNames.join(", ")).arg(groupNames.join(", ")));

    expectedAnnotationNames = QStringList() << "Loci";
    annotationNames = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "Loci  (0, 1)");
    CHECK_SET_ERR(expectedAnnotationNames == annotationNames, QString("Unexpected annotation names: expect '%1', got '%2'")
                  .arg(expectedAnnotationNames.join(", ")).arg(annotationNames.join(", ")));

    expectedAnnotationType = "Loci";
    annotationType = GTUtilsAnnotationsTreeView::getAnnotationType(os, "Loci");
    CHECK_SET_ERR(expectedAnnotationType == annotationType, QString("Unexpected annotation type: expect '%1', got '%2'")
                  .arg(expectedAnnotationType).arg(annotationType));

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
    CHECK_SET_ERR(expectedGroupNames.toSet() == groupNames.toSet(), QString("Unexpected group names: expect '%1', got '%2'")
                  .arg(expectedGroupNames.join(", ")).arg(groupNames.join(", ")));

    expectedAnnotationNames = QStringList() << "test_0025 again";
    annotationNames = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "test_0025 again  (0, 1)");
    CHECK_SET_ERR(expectedAnnotationNames == annotationNames, QString("Unexpected annotation names: expect '%1', got '%2'")
                  .arg(expectedAnnotationNames.join(", ")).arg(annotationNames.join(", ")));

    expectedAnnotationType = "Attenuator";
    annotationType = GTUtilsAnnotationsTreeView::getAnnotationType(os, "test_0025 again");
    CHECK_SET_ERR(expectedAnnotationType == annotationType, QString("Unexpected annotation type: expect '%1', got '%2'")
                  .arg(expectedAnnotationType).arg(annotationType));
}

GUI_TEST_CLASS_DEFINITION(test_0026) {
//    Test GenBank location style in a full widget.

//    1. Open "data/samples/FASTA/human_T1.fa".
//    2. Call "Create new annotation" dialog.

    class Scenario1 : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

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
    QString location = GTUtilsAnnotationsTreeView::getAnnotationRegionString(os, "Misc. Feature");
    CHECK_SET_ERR(expectedLocation == location, QString("Unexpected location: expect '%1', got '%2")
                  .arg(expectedLocation).arg(location));

    GTUtilsAnnotationsTreeView::deleteItem(os, "Misc. Feature  (0, 1)");

//    4. Call "Create new annotation" dialog.

    class Scenario2 : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

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
    location = GTUtilsAnnotationsTreeView::getAnnotationRegionString(os, "Misc. Feature");
    CHECK_SET_ERR(expectedLocation == location, QString("Unexpected location: expect '%1', got '%2")
                  .arg(expectedLocation).arg(location));

    GTUtilsAnnotationsTreeView::deleteItem(os, "Misc. Feature  (0, 1)");

//    6. Call "Create new annotation" dialog.

    class Scenario3 : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

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
    location = GTUtilsAnnotationsTreeView::getAnnotationRegionString(os, "Misc. Feature");
    CHECK_SET_ERR(expectedLocation == location, QString("Unexpected location: expect '%1', got '%2")
                  .arg(expectedLocation).arg(location));

    GTUtilsAnnotationsTreeView::deleteItem(os, "Misc. Feature  (0, 1)");

//    8. Call "Create new annotation" dialog.

    class Scenario4 : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

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
    location = GTUtilsAnnotationsTreeView::getAnnotationRegionString(os, "Misc. Feature");
    CHECK_SET_ERR(expectedLocation == location, QString("Unexpected location: expect '%1', got '%2")
                  .arg(expectedLocation).arg(location));
}

GUI_TEST_CLASS_DEFINITION(test_0027) {
//    Test simple location style in a full widget.

//    1. Open "data/samples/FASTA/human_T1.fa".
//    2. Call "Create new annotation" dialog.

    class Scenario1 : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

//    3. Set simple location style. Set region "100..200". Accept the dialog.
            setSimpleLocation(os, 100, 200, false, dialog);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, new Scenario1));
    openFileAndCallCreateAnnotationDialog(os, dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    Expected state: there is an annotation with region "100..200".
    QString expectedLocation = "100..200";
    QString location = GTUtilsAnnotationsTreeView::getAnnotationRegionString(os, "Misc. Feature");
    CHECK_SET_ERR(expectedLocation == location, QString("Unexpected location: expect '%1', got '%2")
                  .arg(expectedLocation).arg(location));

    GTUtilsAnnotationsTreeView::deleteItem(os, "Misc. Feature  (0, 1)");

//    4. Call "Create new annotation" dialog.

    class Scenario2 : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

//    5. Set simple location style. Set region "100..200". Check the "Complement" checkbox. Accept the dialog.
            setSimpleLocation(os, 100, 200, true, dialog);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, new Scenario2));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "New annotation");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    Expected state: there is an annotation with region "complement(100..200)".
    expectedLocation = "complement(100..200)";
    location = GTUtilsAnnotationsTreeView::getAnnotationRegionString(os, "Misc. Feature");
    CHECK_SET_ERR(expectedLocation == location, QString("Unexpected location: expect '%1', got '%2")
                  .arg(expectedLocation).arg(location));

    GTUtilsAnnotationsTreeView::deleteItem(os, "Misc. Feature  (0, 1)");

//    5. Call "Create new annotation" dialog.

    class Scenario3 : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

//    6. Set simple location style. Set region "200..100". Accept the dialog.
            setSimpleLocation(os, 200, 100, false, dialog);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, new Scenario3));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "New annotation");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    Expected state: there is an annotation with region "join(1..100,200..199950)".
    expectedLocation = "join(1..100,200..199950)";
    location = GTUtilsAnnotationsTreeView::getAnnotationRegionString(os, "Misc. Feature");
    CHECK_SET_ERR(expectedLocation == location, QString("Unexpected location: expect '%1', got '%2")
                  .arg(expectedLocation).arg(location));

    GTUtilsAnnotationsTreeView::deleteItem(os, "Misc. Feature  (0, 1)");

//    7. Call "Create new annotation" dialog.

    class Scenario4 : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

//    8. Set simple location style. Set region "200..100". Check the "Complement" checkbox. Accept the dialog.
            setSimpleLocation(os, 200, 100, true, dialog);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, new Scenario4));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "New annotation");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    Expected state: there is an annotation with region "complement(join(1..100,200..199950))".
    expectedLocation = "complement(join(1..100,200..199950))";
    location = GTUtilsAnnotationsTreeView::getAnnotationRegionString(os, "Misc. Feature");
    CHECK_SET_ERR(expectedLocation == location, QString("Unexpected location: expect '%1', got '%2")
                  .arg(expectedLocation).arg(location));
}

GUI_TEST_CLASS_DEFINITION(test_0028) {
//    Test creation annotation on sequence with existing annotation table in a full widget.

//    1. Open "data/samples/Genbank/murine.gb".
//    2. Call "Create new annotation" dialog.

    class Scenario1 : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

//    3. Select "Existing table" option. Accept the dialog.
            setExistingTable(os, dialog);
            setGenbankLocation(os, "100..200", dialog);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, new Scenario1));
    openFileAndCallCreateAnnotationDialog(os, dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    Expected state: there is an additional annotation in the existing annotation table.
    QStringList expectedGroups = QStringList() << "CDS  (0, 4)"
                                               << "comment  (0, 1)"
                                               << "misc_feature  (0, 2)"
                                               << "source  (0, 1)"
                                               << "Misc. Feature  (0, 1)";
    QStringList groups = GTUtilsAnnotationsTreeView::getGroupNames(os, "NC_001363 features [murine.gb] *");
    CHECK_SET_ERR(expectedGroups.toSet() == groups.toSet(), QString("Unexpected group names: expect '%1', got '%2'")
                  .arg(expectedGroups.join(", ")).arg(groups.join(", ")));

    QStringList expectedAnnotations = QStringList() << "Misc. Feature";
    QStringList annotations = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "Misc. Feature  (0, 1)");
    CHECK_SET_ERR(expectedAnnotations.toSet() == annotations.toSet(), QString("Unexpected annotation names: expect '%1', got '%2'")
                  .arg(expectedAnnotations.join(", ")).arg(annotations.join(", ")));

//    4. Call "Create new annotation" dialog.
    class Scenario2 : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

//    5. Select "Create new table" option. Set any valid table path. Accept the dialog.
            QDir().mkpath(sandBoxDir + "test_0028");
            setNewTable(os, dialog, sandBoxDir + "test_0028/test_0028.gb");
            setGenbankLocation(os, "100..200", dialog);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, new Scenario2));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "New annotation");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    Expected state: there is a new annotation table with an annotation within.
    expectedGroups = QStringList() << "Misc. Feature  (0, 1)";
    groups = GTUtilsAnnotationsTreeView::getGroupNames(os, "Annotations [test_0028.gb] *");
    CHECK_SET_ERR(expectedGroups.toSet() == groups.toSet(), QString("Unexpected group names: expect '%1', got '%2'")
                  .arg(expectedGroups.join(", ")).arg(groups.join(", ")));

    expectedAnnotations = QStringList() << "Misc. Feature";
    annotations = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "Misc. Feature  (0, 1)");
    CHECK_SET_ERR(expectedAnnotations.toSet() == annotations.toSet(), QString("Unexpected annotation names: expect '%1', got '%2'")
                  .arg(expectedAnnotations.join(", ")).arg(annotations.join(", ")));
}

GUI_TEST_CLASS_DEFINITION(test_0029) {
//    Test creation annotation on sequence with existing annotation table in an embedded widget.

//    1. Open "data/samples/Genbank/murine.gb".
//    2. Call "Smith-Waterman Search" dialog. Set any pattern. Open "Input and output" tab.

    class Scenario1 : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

            setSmithWatermanPatternAndOpenLastTab(os, dialog, "GATTTTATTTAGTCTCCAG");

//    3. Select "Existing table" option. Accept the dialog.
            setExistingTable(os, dialog);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new SmithWatermanDialogFiller(os, new Scenario1));
    openFileAndCallSmithWatermanDialog(os, dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    Expected state: there is an additional annotation in the existing annotation table.
    QStringList expectedGroups = QStringList() << "CDS  (0, 4)"
                                               << "comment  (0, 1)"
                                               << "misc_feature  (0, 2)"
                                               << "source  (0, 1)"
                                               << "Misc. Feature  (0, 1)";
    QStringList groups = GTUtilsAnnotationsTreeView::getGroupNames(os, "NC_001363 features [murine.gb] *");
    CHECK_SET_ERR(expectedGroups.toSet() == groups.toSet(), QString("Unexpected group names: expect '%1', got '%2'")
                  .arg(expectedGroups.join(", ")).arg(groups.join(", ")));

    QStringList expectedAnnotations = QStringList() << "Misc. Feature";
    QStringList annotations = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "Misc. Feature  (0, 1)");
    CHECK_SET_ERR(expectedAnnotations.toSet() == annotations.toSet(), QString("Unexpected annotation names: expect '%1', got '%2'")
                  .arg(expectedAnnotations.join(", ")).arg(annotations.join(", ")));

//    4. Call "Smith-Waterman Search" dialog. Set any pattern. Open "Input and output" tab.

    class Scenario2 : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

            setSmithWatermanPatternAndOpenLastTab(os, dialog, "GATTTTATTTAGTCTCCAG");

//    5. Select "Create new table" option. Set any valid table path. Accept the dialog.
            QDir().mkpath(sandBoxDir + "test_0029");
            setNewTable(os, dialog, sandBoxDir + "test_0029/test_0029.gb");

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new SmithWatermanDialogFiller(os, new Scenario2));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Find pattern [Smith-Waterman]");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    Expected state: there is a new annotation table with an annotation within.
    expectedGroups = QStringList() << "Misc. Feature  (0, 1)";
    groups = GTUtilsAnnotationsTreeView::getGroupNames(os, "Annotations [test_0029.gb] *");
    CHECK_SET_ERR(expectedGroups.toSet() == groups.toSet(), QString("Unexpected group names: expect '%1', got '%2'")
                  .arg(expectedGroups.join(", ")).arg(groups.join(", ")));

    expectedAnnotations = QStringList() << "Misc. Feature";
    annotations = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "Misc. Feature  (0, 1)");
    CHECK_SET_ERR(expectedAnnotations.toSet() == annotations.toSet(), QString("Unexpected annotation names: expect '%1', got '%2'")
                  .arg(expectedAnnotations.join(", ")).arg(annotations.join(", ")));
}

GUI_TEST_CLASS_DEFINITION(test_0030) {
//    Test creation annotation on sequence with existing annotation table in an options panel widget.

//    1. Open "data/samples/Genbank/murine.gb".
//    2. Open "Search in Sequence" options panel tab. Set any pattern. Open "Save annotation(s) to" group.
    openFileOpenSearchTabAndSetPattern(os, dataDir + "samples/Genbank/murine.gb", "GATTTTATTTAGTCTCCAG");
    GTUtilsOptionPanelSequenceView::openSaveAnnotationToShowHideWidget(os);

//    3. Select "Existing table" option. Click "Create annotations" button.
    setExistingTable(os);

    GTUtilsOptionPanelSequenceView::clickGetAnnotation(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    Expected state: there is an additional annotation in the existing annotation table.
    QStringList expectedGroups = QStringList() << "CDS  (0, 4)"
                                               << "comment  (0, 1)"
                                               << "misc_feature  (0, 2)"
                                               << "source  (0, 1)"
                                               << "Misc. Feature  (0, 1)";
    QStringList groups = GTUtilsAnnotationsTreeView::getGroupNames(os, "NC_001363 features [murine.gb] *");
    CHECK_SET_ERR(expectedGroups.toSet() == groups.toSet(), QString("Unexpected group names: expect '%1', got '%2'")
                  .arg(expectedGroups.join(", ")).arg(groups.join(", ")));

    QStringList expectedAnnotations = QStringList() << "Misc. Feature";
    QStringList annotations = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "Misc. Feature  (0, 1)");
    CHECK_SET_ERR(expectedAnnotations.toSet() == annotations.toSet(), QString("Unexpected annotation names: expect '%1', got '%2'")
                  .arg(expectedAnnotations.join(", ")).arg(annotations.join(", ")));

//    4. Select "Create new table" option. Set any valid table path. Click "Create annotations" button.
    QDir().mkpath(sandBoxDir + "test_0030");
    setNewTable(os, NULL, sandBoxDir + "test_0030/test_0030.gb");

    GTUtilsOptionPanelSequenceView::clickGetAnnotation(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    Expected state: there is a new annotation table with an annotation within.
    expectedGroups = QStringList() << "Misc. Feature  (0, 1)";
    groups = GTUtilsAnnotationsTreeView::getGroupNames(os, "Annotations [test_0030.gb] *");
    CHECK_SET_ERR(expectedGroups.toSet() == groups.toSet(), QString("Unexpected group names: expect '%1', got '%2'")
                  .arg(expectedGroups.join(", ")).arg(groups.join(", ")));

    expectedAnnotations = QStringList() << "Misc. Feature";
    annotations = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "Misc. Feature  (0, 1)");
    CHECK_SET_ERR(expectedAnnotations.toSet() == annotations.toSet(), QString("Unexpected annotation names: expect '%1', got '%2'")
                  .arg(expectedAnnotations.join(", ")).arg(annotations.join(", ")));
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
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

//    4. Select "Existing table" option. Click "Browse existing tables" button.
            setExistingTable(os, dialog);

            class ProjectTreeItemsChecker : public CustomScenario {
            public:
                void run(HI::GUITestOpStatus &os) {
                    QWidget *dialog = QApplication::activeModalWidget();
                    CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

                    QTreeView* treeView = dialog->findChild<QTreeView*>();
                    CHECK_SET_ERR(treeView != NULL, "treeWidget is NULL");

//    Expected state: there are two possible tables to save annotation to.
                    int visibleItemCount = 0;
                    for (int i = 0; i < treeView->model()->rowCount(); ++i) {
                        if (Qt::NoItemFlags != treeView->model()->flags(treeView->model()->index(i, 0))) {
                            ++visibleItemCount;
                        }
                    }
                    CHECK_SET_ERR(1 == visibleItemCount, QString("Unexpected documents count: expect %1, got %2")
                                  .arg(1).arg(visibleItemCount));

                    const QModelIndex documentIndex = GTUtilsProjectTreeView::findIndex(os, treeView, "2annot_1seq.gb");
                    visibleItemCount = 0;
                    for (int i = 0, n = treeView->model()->rowCount(documentIndex); i < n; ++i) {
                        if (Qt::NoItemFlags != treeView->model()->flags(treeView->model()->index(i, 0, documentIndex))) {
                            ++visibleItemCount;
                        }
                    }
                    CHECK_SET_ERR(2 == visibleItemCount, QString("Unexpected objects count: expect %1, got %2")
                                  .arg(2).arg(visibleItemCount));

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
            QStringList expectedGroups = QStringList() << "<auto>"
                                                       << "CDS"
                                                       << "misc_feature"
                                                       << "source";
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
    QStringList expectedGroups = QStringList() << "group  (0, 1)"
                                               << "just an annotation  (0, 1)"
                                               << "test_0031  (0, 1)";
    QStringList groups = GTUtilsAnnotationsTreeView::getGroupNames(os, "NC_001363 annotations 2 [2annot_1seq.gb] *");
    CHECK_SET_ERR(expectedGroups.toSet() == groups.toSet(), QString("Unexpected group names: expect '%1', got '%2'")
                  .arg(expectedGroups.join(", ")).arg(groups.join(", ")));

    QStringList expectedAnnotations = QStringList() << "Misc. Feature";
    QStringList annotations = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "test_0031  (0, 1)");
    CHECK_SET_ERR(expectedAnnotations.toSet() == annotations.toSet(), QString("Unexpected annotation names: expect '%1', got '%2'")
                  .arg(expectedAnnotations.join(", ")).arg(annotations.join(", ")));
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
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

            setSmithWatermanPatternAndOpenLastTab(os, dialog, "GATTTTATTTAGTCTCCAG");

//    4. Select "Existing table" option. Click "Browse existing tables" button.
            setExistingTable(os, dialog);

            class ProjectTreeItemsChecker : public CustomScenario {
            public:
                void run(HI::GUITestOpStatus &os) {
                    QWidget *dialog = QApplication::activeModalWidget();
                    CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

                    QTreeView* treeView = dialog->findChild<QTreeView*>();
                    CHECK_SET_ERR(treeView != NULL, "treeWidget is NULL");

//    Expected state: there are two possible tables to save annotation to.
                    int visibleItemCount = 0;
                    for (int i = 0; i < treeView->model()->rowCount(); ++i) {
                        if (Qt::NoItemFlags != treeView->model()->flags(treeView->model()->index(i, 0))) {
                            ++visibleItemCount;
                        }
                    }
                    CHECK_SET_ERR(1 == visibleItemCount, QString("Unexpected documents count: expect %1, got %2")
                                  .arg(1).arg(visibleItemCount));

                    const QModelIndex documentIndex = GTUtilsProjectTreeView::findIndex(os, treeView, "2annot_1seq.gb");
                    visibleItemCount = 0;
                    for (int i = 0, n = treeView->model()->rowCount(documentIndex); i < n; ++i) {
                        if (Qt::NoItemFlags != treeView->model()->flags(treeView->model()->index(i, 0, documentIndex))) {
                            ++visibleItemCount;
                        }
                    }
                    CHECK_SET_ERR(2 == visibleItemCount, QString("Unexpected objects count: expect %1, got %2")
                                  .arg(2).arg(visibleItemCount));

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
            QStringList expectedGroups = QStringList() << "<auto>"
                                                       << "CDS"
                                                       << "misc_feature"
                                                       << "source";
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
    QStringList expectedGroups = QStringList() << "group  (0, 1)"
                                               << "just an annotation  (0, 1)"
                                               << "test_0032  (0, 1)";
    QStringList groups = GTUtilsAnnotationsTreeView::getGroupNames(os, "NC_001363 annotations 2 [2annot_1seq.gb] *");
    CHECK_SET_ERR(expectedGroups.toSet() == groups.toSet(), QString("Unexpected group names: expect '%1', got '%2'")
                  .arg(expectedGroups.join(", ")).arg(groups.join(", ")));

    QStringList expectedAnnotations = QStringList() << "Misc. Feature";
    QStringList annotations = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "test_0032  (0, 1)");
    CHECK_SET_ERR(expectedAnnotations.toSet() == annotations.toSet(), QString("Unexpected annotation names: expect '%1', got '%2'")
                  .arg(expectedAnnotations.join(", ")).arg(annotations.join(", ")));
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
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

            QTreeView* treeView = dialog->findChild<QTreeView*>();
            CHECK_SET_ERR(treeView != NULL, "treeWidget is NULL");

//    Expected state: there are two possible tables to save annotation to.
            int visibleItemCount = 0;
            for (int i = 0; i < treeView->model()->rowCount(); ++i) {
                if (Qt::NoItemFlags != treeView->model()->flags(treeView->model()->index(i, 0))) {
                    ++visibleItemCount;
                }
            }
            CHECK_SET_ERR(1 == visibleItemCount, QString("Unexpected documents count: expect %1, got %2")
                          .arg(1).arg(visibleItemCount));

            const QModelIndex documentIndex = GTUtilsProjectTreeView::findIndex(os, treeView, "2annot_1seq.gb");
            visibleItemCount = 0;
            for (int i = 0, n = treeView->model()->rowCount(documentIndex); i < n; ++i) {
                if (Qt::NoItemFlags != treeView->model()->flags(treeView->model()->index(i, 0, documentIndex))) {
                    ++visibleItemCount;
                }
            }
            CHECK_SET_ERR(2 == visibleItemCount, QString("Unexpected objects count: expect %1, got %2")
                          .arg(2).arg(visibleItemCount));

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
    QStringList expectedGroups = QStringList() << "<auto>"
                                               << "CDS"
                                               << "misc_feature"
                                               << "source";
    GTUtilsDialog::waitForDialog(os, new PopupChecker(os, new GroupMenuChecker(expectedGroups)));
    clickSelectGroupButton(os);

//    7. Close the popup menu. Set "NC_001363 annotations 2" in existing tables combobox.
    GTUtilsOptionPanelSequenceView::openSaveAnnotationToShowHideWidget(os);
    setExistingTable(os, NULL, "2annot_1seq.gb [NC_001363 annotations 2]");
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
    CHECK_SET_ERR(expectedGroups.toSet() == groups.toSet(), QString("Unexpected group names: expect '%1', got '%2'")
                  .arg(expectedGroups.join(", ")).arg(groups.join(", ")));

    QStringList expectedAnnotations = QStringList() << "Misc. Feature";
    QStringList annotations = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "test_0033  (0, 1)");
    CHECK_SET_ERR(expectedAnnotations.toSet() == annotations.toSet(), QString("Unexpected annotation names: expect '%1', got '%2'")
                  .arg(expectedAnnotations.join(", ")).arg(annotations.join(", ")));
}

GUI_TEST_CLASS_DEFINITION(test_0034) {
//    Test creation annotation to a new table in a full widget.

//    1. Open "data/samples/FASTA/human_T1.fa".
//    2. Call "Create new annotation" dialog.

    class Scenario1 : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

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
    QStringList expectedGroups = QStringList() << "Misc. Feature  (0, 1)";
    QStringList groups = GTUtilsAnnotationsTreeView::getGroupNames(os, "Annotations [test_0034_1.gb] *");
    CHECK_SET_ERR(expectedGroups.toSet() == groups.toSet(), QString("Unexpected group names: expect '%1', got '%2'")
                  .arg(expectedGroups.join(", ")).arg(groups.join(", ")));

    QStringList expectedAnnotations = QStringList() << "Misc. Feature";
    QStringList annotations = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "Misc. Feature  (0, 1)");
    CHECK_SET_ERR(expectedAnnotations.toSet() == annotations.toSet(), QString("Unexpected annotation names: expect '%1', got '%2'")
                  .arg(expectedAnnotations.join(", ")).arg(annotations.join(", ")));

//    4. Call "Create new annotation" dialog.

    class Scenario2 : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

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
    expectedGroups = QStringList() << "Misc. Feature  (0, 1)";
    groups = GTUtilsAnnotationsTreeView::getGroupNames(os, "Annotations [test_0034_2.gb] *");
    CHECK_SET_ERR(expectedGroups.toSet() == groups.toSet(), QString("Unexpected group names: expect '%1', got '%2'")
                  .arg(expectedGroups.join(", ")).arg(groups.join(", ")));

    expectedAnnotations = QStringList() << "Misc. Feature";
    annotations = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "Misc. Feature  (0, 1)");
    CHECK_SET_ERR(expectedAnnotations.toSet() == annotations.toSet(), QString("Unexpected annotation names: expect '%1', got '%2'")
                  .arg(expectedAnnotations.join(", ")).arg(annotations.join(", ")));
}

GUI_TEST_CLASS_DEFINITION(test_0035) {
//    Test creation annotation to a new table in an embedded widget.

//    1. Open "data/samples/FASTA/human_T1.fa".
//    2. Call "Smith-Waterman" dialog. Set any pattern. Open "Input and output" tab.

    class Scenario1 : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

            setSmithWatermanPatternAndOpenLastTab(os, dialog);

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
    QStringList expectedGroups = QStringList() << "Misc. Feature  (0, 1)";
    QStringList groups = GTUtilsAnnotationsTreeView::getGroupNames(os, "Annotations [test_0035_1.gb] *");
    CHECK_SET_ERR(expectedGroups.toSet() == groups.toSet(), QString("Unexpected group names: expect '%1', got '%2'")
                  .arg(expectedGroups.join(", ")).arg(groups.join(", ")));

    QStringList expectedAnnotations = QStringList() << "Misc. Feature";
    QStringList annotations = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "Misc. Feature  (0, 1)");
    CHECK_SET_ERR(expectedAnnotations.toSet() == annotations.toSet(), QString("Unexpected annotation names: expect '%1', got '%2'")
                  .arg(expectedAnnotations.join(", ")).arg(annotations.join(", ")));

//    4. Call "Smith-Waterman" dialog. Set any pattern. Open "Input and output" tab.

    class Scenario2 : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

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
    expectedGroups = QStringList() << "Misc. Feature  (0, 1)";
    groups = GTUtilsAnnotationsTreeView::getGroupNames(os, "Annotations [test_0035_2.gb] *");
    CHECK_SET_ERR(expectedGroups.toSet() == groups.toSet(), QString("Unexpected group names: expect '%1', got '%2'")
                  .arg(expectedGroups.join(", ")).arg(groups.join(", ")));

    expectedAnnotations = QStringList() << "Misc. Feature";
    annotations = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "Misc. Feature  (0, 1)");
    CHECK_SET_ERR(expectedAnnotations.toSet() == annotations.toSet(), QString("Unexpected annotation names: expect '%1', got '%2'")
                  .arg(expectedAnnotations.join(", ")).arg(annotations.join(", ")));
}

GUI_TEST_CLASS_DEFINITION(test_0036) {
//    Test creation annotation to a new table in an options panel widget.

//    1. Open "data/samples/FASTA/human_T1.fa".
//    2. Open "Search in Sequence" options panel tab. Set any pattern. Open "Save annotation(s) to" group.
    openFileOpenSearchTabAndSetPattern(os, dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsOptionPanelSequenceView::openSaveAnnotationToShowHideWidget(os);

//    3. Select "Create new table" option. Click "Browse new file" button. Select any file. Accept the dialog.
    setNewTable(os);
    QDir().mkpath(sandBoxDir + "test_0036");
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, sandBoxDir + "test_0036/test_0036_1.gb", GTGlobals::UseMouse, GTFileDialogUtils::Save));
    GTWidget::click(os, GTWidget::findWidget(os, "tbBrowseNewTable"));

    GTUtilsOptionPanelSequenceView::clickGetAnnotation(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    Expected state: there is a new annotation in a new annotation table.
    QStringList expectedGroups = QStringList() << "Misc. Feature  (0, 1)";
    QStringList groups = GTUtilsAnnotationsTreeView::getGroupNames(os, "Annotations [test_0036_1.gb] *");
    CHECK_SET_ERR(expectedGroups.toSet() == groups.toSet(), QString("Unexpected group names: expect '%1', got '%2'")
                  .arg(expectedGroups.join(", ")).arg(groups.join(", ")));

    QStringList expectedAnnotations = QStringList() << "Misc. Feature";
    QStringList annotations = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "Misc. Feature  (0, 1)");
    CHECK_SET_ERR(expectedAnnotations.toSet() == annotations.toSet(), QString("Unexpected annotation names: expect '%1', got '%2'")
                  .arg(expectedAnnotations.join(", ")).arg(annotations.join(", ")));

//    4. Select "Create new table" option. Set any valid file path in the lineedit. Accept the dialog.
    setNewTable(os, NULL, sandBoxDir + "test_0036/test_0036_2.gb");

    GTUtilsOptionPanelSequenceView::clickGetAnnotation(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    Expected state: there is a new annotation in a new annotation table.
    expectedGroups = QStringList() << "Misc. Feature  (0, 1)";
    groups = GTUtilsAnnotationsTreeView::getGroupNames(os, "Annotations [test_0036_2.gb] *");
    CHECK_SET_ERR(expectedGroups.toSet() == groups.toSet(), QString("Unexpected group names: expect '%1', got '%2'")
                  .arg(expectedGroups.join(", ")).arg(groups.join(", ")));

    expectedAnnotations = QStringList() << "Misc. Feature";
    annotations = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "Misc. Feature  (0, 1)");
    CHECK_SET_ERR(expectedAnnotations.toSet() == annotations.toSet(), QString("Unexpected annotation names: expect '%1', got '%2'")
                  .arg(expectedAnnotations.join(", ")).arg(annotations.join(", ")));
}

GUI_TEST_CLASS_DEFINITION(test_0037) {
//    Test creation annotation to an auto table in an embedded widget.

//    1. Open "data/samples/FASTA/human_T1.fa".
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    2. Call "ORF Marker" dialog. Open "Output" tab.

    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

            GTTabWidget::setCurrentIndex(os, GTWidget::findExactWidget<QTabWidget *>(os, "tabWidget", dialog), 1);

//    3. Select "Use auto-annotations table" option. Accept the dialog.
            GTRadioButton::click(os, GTWidget::findExactWidget<QRadioButton *>(os, "rbUseAutoTable", dialog));

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
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

//    3. Check "Use pattern names" checkbox state.
//    Expected state: it is invisible.
            QCheckBox *usePatternNames = GTWidget::findExactWidget<QCheckBox *>(os, "chbUsePatternNames", dialog);
            CHECK_SET_ERR(NULL != usePatternNames, "usePatternNames is NULL");
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
            QRadioButton *rbSimpleFormat = GTWidget::findExactWidget<QRadioButton *>(os, "rbSimpleFormat", dialog);
            CHECK_SET_ERR(NULL != rbSimpleFormat, "rbSimpleFormat is NULL");
            CHECK_SET_ERR(rbSimpleFormat->isEnabled(), "rbSimpleFormat is unexpectedly disabled");

            QLineEdit *leRegionStart = GTWidget::findExactWidget<QLineEdit *>(os, "leRegionStart", dialog);
            CHECK_SET_ERR(NULL != leRegionStart, "leRegionStart is NULL");
            CHECK_SET_ERR(leRegionStart->isEnabled(), "leRegionStart is unexpectedly disabled");

            QLineEdit *leRegionEnd = GTWidget::findExactWidget<QLineEdit *>(os, "leRegionEnd", dialog);
            CHECK_SET_ERR(NULL != leRegionEnd, "leRegionEnd is NULL");
            CHECK_SET_ERR(leRegionEnd->isEnabled(), "leRegionEnd is unexpectedly disabled");

            QCheckBox *chbComplement = GTWidget::findExactWidget<QCheckBox *>(os, "chbComplement", dialog);
            CHECK_SET_ERR(NULL != chbComplement, "chbComplement is NULL");
            CHECK_SET_ERR(chbComplement->isEnabled(), "chbComplement is unexpectedly disabled");

            QRadioButton *rbGenbankFormat = GTWidget::findExactWidget<QRadioButton *>(os, "rbGenbankFormat", dialog);
            CHECK_SET_ERR(NULL != rbGenbankFormat, "rbGenbankFormat is NULL");
            CHECK_SET_ERR(rbGenbankFormat->isEnabled(), "rbGenbankFormat is unexpectedly disabled");

            QLineEdit *leLocation = GTWidget::findExactWidget<QLineEdit *>(os, "leLocation", dialog);
            CHECK_SET_ERR(NULL != leLocation, "leLocation is NULL");
            CHECK_SET_ERR(!leLocation->isEnabled(), "leLocation is unexpectedly enabled");

            QToolButton *tbDoComplement = GTWidget::findExactWidget<QToolButton *>(os, "tbDoComplement", dialog);
            CHECK_SET_ERR(NULL != tbDoComplement, "tbDoComplement is NULL");
            CHECK_SET_ERR(!tbDoComplement->isEnabled(), "tbDoComplement is unexpectedly enabled");

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
            CHECK_SET_ERR(leRegionStart->text().isEmpty(), QString("An unexpected text in leRegionStart: '%1'").arg(leRegionStart->text()));

            GTLineEdit::setText(os, leRegionStart, "qwerty", true);
            CHECK_SET_ERR(leRegionStart->text().isEmpty(), QString("An unexpected text in leRegionStart: '%1'").arg(leRegionStart->text()));

            GTLineEdit::setText(os, leRegionEnd, "-1", true);
            CHECK_SET_ERR(leRegionEnd->text().isEmpty(), QString("An unexpected text in leRegionEnd: '%1'").arg(leRegionEnd->text()));

            GTLineEdit::setText(os, leRegionEnd, "qwerty", true);
            CHECK_SET_ERR(leRegionEnd->text().isEmpty(), QString("An unexpected text in leRegionEnd: '%1'").arg(leRegionEnd->text()));

//    6. Enter region "(100..200)" to simple location widgets.
            setSimpleLocation(os, 100, 200, false, dialog);

//    Expected state: GenBank location string contains "100..200" region.
            QString expectedGenbankLocation = "100..200";
            QString genbankLocation = leLocation->text();
            CHECK_SET_ERR(expectedGenbankLocation == genbankLocation, QString("Unexpected GenBank location string: expect '%1', got '%2'")
                          .arg(expectedGenbankLocation).arg(genbankLocation));

//    7. Check "Complement" checkbox.
            GTCheckBox::setChecked(os, chbComplement);

//    Expected state: GenBank location string contains "complement(100..200)" region.
            expectedGenbankLocation = "complement(100..200)";
            genbankLocation = leLocation->text();
            CHECK_SET_ERR(expectedGenbankLocation == genbankLocation, QString("Unexpected GenBank location string: expect '%1', got '%2'")
                          .arg(expectedGenbankLocation).arg(genbankLocation));

//    8. Uncheck "Complement" checkbox.
            GTCheckBox::setChecked(os, chbComplement, false);

//    Expected state: GenBank location string contains "100..200" region.
            expectedGenbankLocation = "100..200";
            genbankLocation = leLocation->text();
            CHECK_SET_ERR(expectedGenbankLocation == genbankLocation, QString("Unexpected GenBank location string: expect '%1', got '%2'")
                          .arg(expectedGenbankLocation).arg(genbankLocation));

//    9. Enter region "(200..100)" to simple location widgets.
            setSimpleLocation(os, 200, 100, false, dialog);

//    Expected state: GenBank location string contains "join(1..100,200..199950)" region.
            expectedGenbankLocation = "join(1..100,200..199950)";
            genbankLocation = leLocation->text();
            CHECK_SET_ERR(expectedGenbankLocation == genbankLocation, QString("Unexpected GenBank location string: expect '%1', got '%2'")
                          .arg(expectedGenbankLocation).arg(genbankLocation));

//    10. Check "Complement" checkbox.
            GTCheckBox::setChecked(os, chbComplement);

//    Expected state: GenBank location string contains "complement(join(1..100,200..199950))" region.
            expectedGenbankLocation = "complement(join(1..100,200..199950))";
            genbankLocation = leLocation->text();
            CHECK_SET_ERR(expectedGenbankLocation == genbankLocation, QString("Unexpected GenBank location string: expect '%1', got '%2'")
                          .arg(expectedGenbankLocation).arg(genbankLocation));

//    11. Uncheck "Complement" checkbox.
            GTCheckBox::setChecked(os, chbComplement, false);

//    Expected state: GenBank location string contains "join(1..100,200..199950)" region.
            expectedGenbankLocation = "join(1..100,200..199950)";
            genbankLocation = leLocation->text();
            CHECK_SET_ERR(expectedGenbankLocation == genbankLocation, QString("Unexpected GenBank location string: expect '%1', got '%2'")
                          .arg(expectedGenbankLocation).arg(genbankLocation));

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
            CHECK_SET_ERR(rbSimpleFormat->isEnabled(), "rbSimpleFormat is unexpectedly disabled");
            CHECK_SET_ERR(!leRegionStart->isEnabled(), "leRegionStart is unexpectedly enabled");
            CHECK_SET_ERR(!leRegionEnd->isEnabled(), "leRegionEnd is unexpectedly enabled");
            CHECK_SET_ERR(!chbComplement->isEnabled(), "chbComplement is unexpectedly enabled");
            CHECK_SET_ERR(rbGenbankFormat->isEnabled(), "rbGenbankFormat is unexpectedly disabled");
            CHECK_SET_ERR(leLocation->isEnabled(), "leLocation is unexpectedly disabled");
            CHECK_SET_ERR(tbDoComplement->isEnabled(), "tbDoComplement is unexpectedly disabled");

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
            CHECK_SET_ERR(rbSimpleFormat->isEnabled(), "rbSimpleFormat is unexpectedly disabled");
            CHECK_SET_ERR(leRegionStart->isEnabled(), "leRegionStart is unexpectedly disabled");
            CHECK_SET_ERR(leRegionEnd->isEnabled(), "leRegionEnd is unexpectedly disabled");
            CHECK_SET_ERR(chbComplement->isEnabled(), "chbComplement is unexpectedly disabled");
            CHECK_SET_ERR(rbGenbankFormat->isEnabled(), "rbGenbankFormat is unexpectedly disabled");
            CHECK_SET_ERR(!leLocation->isEnabled(), "leLocation is unexpectedly enabled");
            CHECK_SET_ERR(!tbDoComplement->isEnabled(), "tbDoComplement is unexpectedly enabled");

//    14. Select "GenBank/EMBL format" location style. Set location "300..400".
            setGenbankLocation(os, "300..400", dialog);
            GTKeyboardDriver::keyClick( Qt::Key_Tab);

//    Expected state: simple location widgets contains non-complemented region "(300..400)".
            CHECK_SET_ERR("300" == leRegionStart->text(), QString("Unexpected simple location region start: expect %1, got %2")
                          .arg("300").arg(leRegionStart->text()));
            CHECK_SET_ERR("400" == leRegionEnd->text(), QString("Unexpected simple location region end: expect %1, got %2")
                          .arg("400").arg(leRegionEnd->text()));
            CHECK_SET_ERR(!chbComplement->isChecked(), "Simple location complement checkbox is unexpectedly checked");

//    15. Set location "join(500..600,700..800)".
            setGenbankLocation(os, "join(500..600,700..800)", dialog);
            GTKeyboardDriver::keyClick( Qt::Key_Tab);

//    Expected state: simple location widgets contains non-complemented region "(500..600)".
            CHECK_SET_ERR("500" == leRegionStart->text(), QString("Unexpected simple location region start: expect %1, got %2")
                          .arg("500").arg(leRegionStart->text()));
            CHECK_SET_ERR("600" == leRegionEnd->text(), QString("Unexpected simple location region end: expect %1, got %2")
                          .arg("600").arg(leRegionEnd->text()));
            CHECK_SET_ERR(!chbComplement->isChecked(), "Simple location complement checkbox is unexpectedly checked");

//    16. Set location "join(1..100,200..199950)".
            setGenbankLocation(os, "join(1..100,200..199950)", dialog);
            GTKeyboardDriver::keyClick( Qt::Key_Tab);

//    Expected state: simple location widgets contains non-complemented region "(1..100)".
            CHECK_SET_ERR("1" == leRegionStart->text(), QString("Unexpected simple location region start: expect %1, got %2")
                          .arg("1").arg(leRegionStart->text()));
            CHECK_SET_ERR("100" == leRegionEnd->text(), QString("Unexpected simple location region end: expect %1, got %2")
                          .arg("100").arg(leRegionEnd->text()));
            CHECK_SET_ERR(!chbComplement->isChecked(), "Simple location complement checkbox is unexpectedly checked");

//    17. Set location "complement(200..300)".
            setGenbankLocation(os, "complement(200..300)", dialog);
            GTKeyboardDriver::keyClick( Qt::Key_Tab);

//    Expected state: simple location widgets contains complemented region "(200..300)".
            CHECK_SET_ERR("200" == leRegionStart->text(), QString("Unexpected simple location region start: expect %1, got %2")
                          .arg("200").arg(leRegionStart->text()));
            CHECK_SET_ERR("300" == leRegionEnd->text(), QString("Unexpected simple location region end: expect %1, got %2")
                          .arg("300").arg(leRegionEnd->text()));
            CHECK_SET_ERR(chbComplement->isChecked(), "Simple location complement checkbox is unexpectedly unchecked");

//    18. Click "Do complement" button.
            GTWidget::click(os, tbDoComplement);

//    Expected state: simple location widgets contains non-complemented region "(200..300)", GenBank location string contains "200..300".
            CHECK_SET_ERR("200" == leRegionStart->text(), QString("Unexpected simple location region start: expect %1, got %2")
                          .arg("200").arg(leRegionStart->text()));
            CHECK_SET_ERR("300" == leRegionEnd->text(), QString("Unexpected simple location region end: expect %1, got %2")
                          .arg("300").arg(leRegionEnd->text()));
            CHECK_SET_ERR(!chbComplement->isChecked(), "Simple location complement checkbox is unexpectedly checked");
            CHECK_SET_ERR("200..300" == leLocation->text(), QString("Unexpected GenBank location string: expect '%1', got '%2'")
                          .arg("200..300").arg(leLocation->text()));

//    19. Set location "400..500qwerty".
            setGenbankLocation(os, "400..500qwerty", dialog);
            GTKeyboardDriver::keyClick( Qt::Key_Tab);

//    Expected state: simple location widgets are empty, GenBank location string is empty.
            CHECK_SET_ERR(leRegionStart->text().isEmpty(), QString("Unexpected simple location region start: expect an empty string, got %1")
                          .arg(leRegionStart->text()));
            CHECK_SET_ERR(leRegionEnd->text().isEmpty(), QString("Unexpected simple location region end: expect an empty string, got %1")
                          .arg(leRegionEnd->text()));
            CHECK_SET_ERR(!chbComplement->isChecked(), "Simple location complement checkbox is unexpectedly checked");
            CHECK_SET_ERR(leLocation->text().isEmpty(), QString("Unexpected GenBank location string: expect an empty string, got, '%1'")
                          .arg(leLocation->text()));

//    20. Check if destination table widgets are enabled or disabled.
//    Expected state:
//        Existing table radio button - disabled
//        Existing table combobox - disabled
//        Existing table browse button - disabled
//        New table radio button - enabled
//        New table lineedit - enabled
//        New table browse button - enabled
//        Auto table radio button - not visible
            QRadioButton *rbExistingTable = GTWidget::findExactWidget<QRadioButton *>(os, "rbExistingTable", dialog);
            CHECK_SET_ERR(NULL != rbExistingTable, "rbExistingTable is NULL");
            CHECK_SET_ERR(!rbExistingTable->isEnabled(), "rbExistingTable is unexpectedly enabled");

            QComboBox *cbExistingTable = GTWidget::findExactWidget<QComboBox *>(os, "cbExistingTable", dialog);
            CHECK_SET_ERR(NULL != cbExistingTable, "cbExistingTable is NULL");
            CHECK_SET_ERR(!cbExistingTable->isEnabled(), "cbExistingTable is unexpectedly enabled");

            QToolButton *tbBrowseExistingTable = GTWidget::findExactWidget<QToolButton *>(os, "tbBrowseExistingTable", dialog);
            CHECK_SET_ERR(NULL != tbBrowseExistingTable, "tbBrowseExistingTable is NULL");
            CHECK_SET_ERR(!tbBrowseExistingTable->isEnabled(), "tbBrowseExistingTable is unexpectedly enabled");

            QRadioButton *rbCreateNewTable = GTWidget::findExactWidget<QRadioButton *>(os, "rbCreateNewTable", dialog);
            CHECK_SET_ERR(NULL != rbCreateNewTable, "rbCreateNewTable is NULL");
            CHECK_SET_ERR(rbCreateNewTable->isEnabled(), "rbCreateNewTable is unexpectedly disabled");

            QLineEdit *leNewTablePath = GTWidget::findExactWidget<QLineEdit *>(os, "leNewTablePath", dialog);
            CHECK_SET_ERR(NULL != leNewTablePath, "leNewTablePath is NULL");
            CHECK_SET_ERR(leNewTablePath->isEnabled(), "leNewTablePath is unexpectedly disabled");

            QToolButton *tbBrowseNewTable = GTWidget::findExactWidget<QToolButton *>(os, "tbBrowseNewTable", dialog);
            CHECK_SET_ERR(NULL != tbBrowseNewTable, "tbBrowseNewTable is NULL");
            CHECK_SET_ERR(tbBrowseNewTable->isEnabled(), "tbBrowseNewTable is unexpectedly disabled");

            QRadioButton *rbUseAutoTable = GTWidget::findExactWidget<QRadioButton *>(os, "rbUseAutoTable", dialog);
            CHECK_SET_ERR(NULL != rbUseAutoTable, "rbUseAutoTable is NULL");
            CHECK_SET_ERR(!rbUseAutoTable->isVisible(), "rbUseAutoTable is unexpectedly visible");

//    21. Cancel the dialog.
            // Dialog is applied to check boundaries.
            setSimpleLocation(os, 199950, 1, false, dialog);
            GTUtilsDialog::waitForDialogWhichMustNotBeRun(os, new MessageBoxDialogFiller(os, QMessageBox::Ok, "Invalid location! Location must be in GenBank format."));
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, new Scenario1));
    openFileAndCallCreateAnnotationDialog(os, dataDir + "samples/FASTA/human_T1.fa");

//    22. Open "data/damples/Genbank/murine.gb". Mark the sequence object as circular. Call "Create new annotation" dialog.

    class Scenario2 : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

//    23. Check if destination table widgets are enabled or disabled.
//    Expected state:
//        Existing table radio button - enabled
//        Existing table combobox - enabled
//        Existing table browse button - enabled
//        New table radio button - enabled
//        New table lineedit - disabled
//        New table browse button - disabled
//        Auto table radio button - not visible
            QRadioButton *rbExistingTable = GTWidget::findExactWidget<QRadioButton *>(os, "rbExistingTable", dialog);
            CHECK_SET_ERR(NULL != rbExistingTable, "rbExistingTable is NULL");
            CHECK_SET_ERR(rbExistingTable->isEnabled(), "rbExistingTable is unexpectedly disabled");

            QComboBox *cbExistingTable = GTWidget::findExactWidget<QComboBox *>(os, "cbExistingTable", dialog);
            CHECK_SET_ERR(NULL != cbExistingTable, "cbExistingTable is NULL");
            CHECK_SET_ERR(cbExistingTable->isEnabled(), "cbExistingTable is unexpectedly disabled");

            QToolButton *tbBrowseExistingTable = GTWidget::findExactWidget<QToolButton *>(os, "tbBrowseExistingTable", dialog);
            CHECK_SET_ERR(NULL != tbBrowseExistingTable, "tbBrowseExistingTable is NULL");
            CHECK_SET_ERR(tbBrowseExistingTable->isEnabled(), "tbBrowseExistingTable is unexpectedly disabled");

            QRadioButton *rbCreateNewTable = GTWidget::findExactWidget<QRadioButton *>(os, "rbCreateNewTable", dialog);
            CHECK_SET_ERR(NULL != rbCreateNewTable, "rbCreateNewTable is NULL");
            CHECK_SET_ERR(rbCreateNewTable->isEnabled(), "rbCreateNewTable is unexpectedly disabled");

            QLineEdit *leNewTablePath = GTWidget::findExactWidget<QLineEdit *>(os, "leNewTablePath", dialog);
            CHECK_SET_ERR(NULL != leNewTablePath, "leNewTablePath is NULL");
            CHECK_SET_ERR(!leNewTablePath->isEnabled(), "leNewTablePath is unexpectedly enabled");

            QToolButton *tbBrowseNewTable = GTWidget::findExactWidget<QToolButton *>(os, "tbBrowseNewTable", dialog);
            CHECK_SET_ERR(NULL != tbBrowseNewTable, "tbBrowseNewTable is NULL");
            CHECK_SET_ERR(!tbBrowseNewTable->isEnabled(), "tbBrowseNewTable is unexpectedly enabled");

            QRadioButton *rbUseAutoTable = GTWidget::findExactWidget<QRadioButton *>(os, "rbUseAutoTable", dialog);
            CHECK_SET_ERR(NULL != rbUseAutoTable, "rbUseAutoTable is NULL");
            CHECK_SET_ERR(!rbUseAutoTable->isVisible(), "rbUseAutoTable is unexpectedly visible");

//    24. Select "Create new table" option. Check if destination table widgets are enabled or disabled.
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

//    25. Select "Existing table" option. Check if destination table widgets are enabled or disabled.
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

//    26. Select "GenBank/EMBL format" location style. Set location "join(1..100,200..199950)".
            setGenbankLocation(os, "join(1..100,200..199950)", dialog);

            QLineEdit *leRegionStart = GTWidget::findExactWidget<QLineEdit *>(os, "leRegionStart", dialog);
            CHECK_SET_ERR(NULL != leRegionStart, "leRegionStart is NULL");
            QLineEdit *leRegionEnd = GTWidget::findExactWidget<QLineEdit *>(os, "leRegionEnd", dialog);
            CHECK_SET_ERR(NULL != leRegionEnd, "leRegionEnd is NULL");
            QCheckBox *chbComplement = GTWidget::findExactWidget<QCheckBox *>(os, "chbComplement", dialog);
            CHECK_SET_ERR(NULL != chbComplement, "chbComplement is NULL");

//    Expected state: simple location widgets contains non-complemented region "(200..100)".
            CHECK_SET_ERR("200" == leRegionStart->text(), QString("Unexpected simple location region start: expect %1, got %2")
                          .arg("200").arg(leRegionStart->text()));
            CHECK_SET_ERR("100" == leRegionEnd->text(), QString("Unexpected simple location region end: expect %1, got %2")
                          .arg("100").arg(leRegionEnd->text()));
            CHECK_SET_ERR(!chbComplement->isChecked(), "Simple location complement checkbox is unexpectedly checked");

//    27. Set location "complement(join(1..100,200..199950))".
            setGenbankLocation(os, "complement(join(1..100,200..199950))", dialog);

//    Expected state: simple location widgets contains complemented region "(200..100)".
            CHECK_SET_ERR("200" == leRegionStart->text(), QString("Unexpected simple location region start: expect %1, got %2")
                          .arg("200").arg(leRegionStart->text()));
            CHECK_SET_ERR("100" == leRegionEnd->text(), QString("Unexpected simple location region end: expect %1, got %2")
                          .arg("100").arg(leRegionEnd->text()));
            CHECK_SET_ERR(chbComplement->isChecked(), "Simple location complement checkbox is unexpectedly unchecked");

//    28. Click "Do complement" button.
            GTWidget::click(os, GTWidget::findExactWidget<QToolButton *>(os, "tbDoComplement", dialog));

//    Expected state: simple location widgets contains non-complemented region "(200..100)".
            CHECK_SET_ERR("200" == leRegionStart->text(), QString("Unexpected simple location region start: expect %1, got %2")
                          .arg("200").arg(leRegionStart->text()));
            CHECK_SET_ERR("100" == leRegionEnd->text(), QString("Unexpected simple location region end: expect %1, got %2")
                          .arg("100").arg(leRegionEnd->text()));
            CHECK_SET_ERR(!chbComplement->isChecked(), "Simple location complement checkbox is unexpectedly checked");

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, new Scenario2));
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsProjectTreeView::markSequenceAsCircular(os, "murine.gb");
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "New annotation");
}

GUI_TEST_CLASS_DEFINITION(test_0039) {
//    Test common embedded widget behavior.

//    1. Open "data/samples/FASTA/human_T1.fa".
//    2. Call "Smith-Waterman Search" dialog. Open "Input and output" tab.

    class Scenario1 : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

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
            QRadioButton *rbExistingTable = GTWidget::findExactWidget<QRadioButton *>(os, "rbExistingTable", dialog);
            CHECK_SET_ERR(NULL != rbExistingTable, "rbExistingTable is NULL");
            CHECK_SET_ERR(!rbExistingTable->isEnabled(), "rbExistingTable is unexpectedly enabled");

            QComboBox *cbExistingTable = GTWidget::findExactWidget<QComboBox *>(os, "cbExistingTable", dialog);
            CHECK_SET_ERR(NULL != cbExistingTable, "cbExistingTable is NULL");
            CHECK_SET_ERR(!cbExistingTable->isEnabled(), "cbExistingTable is unexpectedly enabled");

            QToolButton *tbBrowseExistingTable = GTWidget::findExactWidget<QToolButton *>(os, "tbBrowseExistingTable", dialog);
            CHECK_SET_ERR(NULL != tbBrowseExistingTable, "tbBrowseExistingTable is NULL");
            CHECK_SET_ERR(!tbBrowseExistingTable->isEnabled(), "tbBrowseExistingTable is unexpectedly enabled");

            QRadioButton *rbCreateNewTable = GTWidget::findExactWidget<QRadioButton *>(os, "rbCreateNewTable", dialog);
            CHECK_SET_ERR(NULL != rbCreateNewTable, "rbCreateNewTable is NULL");
            CHECK_SET_ERR(rbCreateNewTable->isEnabled(), "rbCreateNewTable is unexpectedly disabled");

            QLineEdit *leNewTablePath = GTWidget::findExactWidget<QLineEdit *>(os, "leNewTablePath", dialog);
            CHECK_SET_ERR(NULL != leNewTablePath, "leNewTablePath is NULL");
            CHECK_SET_ERR(leNewTablePath->isEnabled(), "leNewTablePath is unexpectedly disabled");

            QToolButton *tbBrowseNewTable = GTWidget::findExactWidget<QToolButton *>(os, "tbBrowseNewTable", dialog);
            CHECK_SET_ERR(NULL != tbBrowseNewTable, "tbBrowseNewTable is NULL");
            CHECK_SET_ERR(tbBrowseNewTable->isEnabled(), "tbBrowseNewTable is unexpectedly disabled");

            QRadioButton *rbUseAutoTable = GTWidget::findExactWidget<QRadioButton *>(os, "rbUseAutoTable", dialog);
            CHECK_SET_ERR(NULL != rbUseAutoTable, "rbUseAutoTable is NULL");
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
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

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
            QRadioButton *rbExistingTable = GTWidget::findExactWidget<QRadioButton *>(os, "rbExistingTable", dialog);
            CHECK_SET_ERR(NULL != rbExistingTable, "rbExistingTable is NULL");
            CHECK_SET_ERR(rbExistingTable->isEnabled(), "rbExistingTable is unexpectedly disabled");

            QComboBox *cbExistingTable = GTWidget::findExactWidget<QComboBox *>(os, "cbExistingTable", dialog);
            CHECK_SET_ERR(NULL != cbExistingTable, "cbExistingTable is NULL");
            CHECK_SET_ERR(cbExistingTable->isEnabled(), "cbExistingTable is unexpectedly disabled");

            QToolButton *tbBrowseExistingTable = GTWidget::findExactWidget<QToolButton *>(os, "tbBrowseExistingTable", dialog);
            CHECK_SET_ERR(NULL != tbBrowseExistingTable, "tbBrowseExistingTable is NULL");
            CHECK_SET_ERR(tbBrowseExistingTable->isEnabled(), "tbBrowseExistingTable is unexpectedly disabled");

            QRadioButton *rbCreateNewTable = GTWidget::findExactWidget<QRadioButton *>(os, "rbCreateNewTable", dialog);
            CHECK_SET_ERR(NULL != rbCreateNewTable, "rbCreateNewTable is NULL");
            CHECK_SET_ERR(rbCreateNewTable->isEnabled(), "rbCreateNewTable is unexpectedly disabled");

            QLineEdit *leNewTablePath = GTWidget::findExactWidget<QLineEdit *>(os, "leNewTablePath", dialog);
            CHECK_SET_ERR(NULL != leNewTablePath, "leNewTablePath is NULL");
            CHECK_SET_ERR(!leNewTablePath->isEnabled(), "leNewTablePath is unexpectedly enabled");

            QToolButton *tbBrowseNewTable = GTWidget::findExactWidget<QToolButton *>(os, "tbBrowseNewTable", dialog);
            CHECK_SET_ERR(NULL != tbBrowseNewTable, "tbBrowseNewTable is NULL");
            CHECK_SET_ERR(!tbBrowseNewTable->isEnabled(), "tbBrowseNewTable is unexpectedly enabled");

            QRadioButton *rbUseAutoTable = GTWidget::findExactWidget<QRadioButton *>(os, "rbUseAutoTable", dialog);
            CHECK_SET_ERR(NULL != rbUseAutoTable, "rbUseAutoTable is NULL");
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
            QCheckBox *chbUsePatternNames = GTWidget::findExactWidget<QCheckBox *>(os, "chbUsePatternNames", dialog);
            CHECK_SET_ERR(NULL != chbUsePatternNames, "chbUsePatternNames is NULL");
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
    QRadioButton *rbExistingTable = GTWidget::findExactWidget<QRadioButton *>(os, "rbExistingTable");
    CHECK_SET_ERR(NULL != rbExistingTable, "rbExistingTable is NULL");
    CHECK_SET_ERR(!rbExistingTable->isEnabled(), "rbExistingTable is unexpectedly enabled for human_T1");

    QComboBox *cbExistingTable = GTWidget::findExactWidget<QComboBox *>(os, "cbExistingTable");
    CHECK_SET_ERR(NULL != cbExistingTable, "cbExistingTable is NULL");
    CHECK_SET_ERR(!cbExistingTable->isEnabled(), "cbExistingTable is unexpectedly enabled for human_T1");

    QToolButton *tbBrowseExistingTable = GTWidget::findExactWidget<QToolButton *>(os, "tbBrowseExistingTable");
    CHECK_SET_ERR(NULL != tbBrowseExistingTable, "tbBrowseExistingTable is NULL");
    CHECK_SET_ERR(!tbBrowseExistingTable->isEnabled(), "tbBrowseExistingTable is unexpectedly enabled for human_T1");

    QRadioButton *rbCreateNewTable = GTWidget::findExactWidget<QRadioButton *>(os, "rbCreateNewTable");
    CHECK_SET_ERR(NULL != rbCreateNewTable, "rbCreateNewTable is NULL");
    CHECK_SET_ERR(rbCreateNewTable->isEnabled(), "rbCreateNewTable is unexpectedly disabled for human_T1");

    QLineEdit *leNewTablePath = GTWidget::findExactWidget<QLineEdit *>(os, "leNewTablePath");
    CHECK_SET_ERR(NULL != leNewTablePath, "leNewTablePath is NULL");
    CHECK_SET_ERR(leNewTablePath->isEnabled(), "leNewTablePath is unexpectedly disabled for human_T1");

    QToolButton *tbBrowseNewTable = GTWidget::findExactWidget<QToolButton *>(os, "tbBrowseNewTable");
    CHECK_SET_ERR(NULL != tbBrowseNewTable, "tbBrowseNewTable is NULL");
    CHECK_SET_ERR(tbBrowseNewTable->isEnabled(), "tbBrowseNewTable is unexpectedly disabled for human_T1");

    QRadioButton *rbUseAutoTable = GTWidget::findExactWidget<QRadioButton *>(os, "rbUseAutoTable");
    CHECK_SET_ERR(NULL != rbUseAutoTable, "rbUseAutoTable is NULL");
    CHECK_SET_ERR(!rbUseAutoTable->isVisible(), "rbUseAutoTable is unexpectedly visible for human_T1");

//    4. Open "Annotation parameters" group. Check "Use pattern name" checkbox state.
    GTUtilsOptionPanelSequenceView::openAnnotationParametersShowHideWidget(os);

//    Expected state: it is visible and enabled.
    QCheckBox *chbUsePatternNames = GTWidget::findExactWidget<QCheckBox *>(os, "chbUsePatternNames");
    CHECK_SET_ERR(NULL != chbUsePatternNames, "chbUsePatternNames is NULL");
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
    rbExistingTable = GTWidget::findExactWidget<QRadioButton *>(os, "rbExistingTable");
    CHECK_SET_ERR(NULL != rbExistingTable, "rbExistingTable is NULL");
    CHECK_SET_ERR(rbExistingTable->isEnabled(), "rbExistingTable is unexpectedly disabled for murine_1");

    cbExistingTable = GTWidget::findExactWidget<QComboBox *>(os, "cbExistingTable");
    CHECK_SET_ERR(NULL != cbExistingTable, "cbExistingTable is NULL");
    CHECK_SET_ERR(cbExistingTable->isEnabled(), "cbExistingTable is unexpectedly disabled for murine_1");

    tbBrowseExistingTable = GTWidget::findExactWidget<QToolButton *>(os, "tbBrowseExistingTable");
    CHECK_SET_ERR(NULL != tbBrowseExistingTable, "tbBrowseExistingTable is NULL");
    CHECK_SET_ERR(tbBrowseExistingTable->isEnabled(), "tbBrowseExistingTable is unexpectedly disabled for murine_1");

    rbCreateNewTable = GTWidget::findExactWidget<QRadioButton *>(os, "rbCreateNewTable");
    CHECK_SET_ERR(NULL != rbCreateNewTable, "rbCreateNewTable is NULL");
    CHECK_SET_ERR(rbCreateNewTable->isEnabled(), "rbCreateNewTable is unexpectedly disabled for murine_1");

    leNewTablePath = GTWidget::findExactWidget<QLineEdit *>(os, "leNewTablePath");
    CHECK_SET_ERR(NULL != leNewTablePath, "leNewTablePath is NULL");
    CHECK_SET_ERR(!leNewTablePath->isEnabled(), "leNewTablePath is unexpectedly enabled for murine_1");

    tbBrowseNewTable = GTWidget::findExactWidget<QToolButton *>(os, "tbBrowseNewTable");
    CHECK_SET_ERR(NULL != tbBrowseNewTable, "tbBrowseNewTable is NULL");
    CHECK_SET_ERR(!tbBrowseNewTable->isEnabled(), "tbBrowseNewTable is unexpectedly enabled for murine_1");

    rbUseAutoTable = GTWidget::findExactWidget<QRadioButton *>(os, "rbUseAutoTable");
    CHECK_SET_ERR(NULL != rbUseAutoTable, "rbUseAutoTable is NULL");
    CHECK_SET_ERR(!rbUseAutoTable->isVisible(), "rbUseAutoTable is unexpectedly visible  for murine_1");

//    8. Select "Create new table" option. Check if destination table widgets are enabled or disabled.
    //GTWidget::click(os, GTWidget::findWidget(os, "rbCreateNewTable"));
    GTRadioButton::click(os, GTWidget::findExactWidget<QRadioButton*>(os, "rbCreateNewTable"));
    GTGlobals::sleep();

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
    GTRadioButton::click(os, GTWidget::findExactWidget<QRadioButton*>(os, "rbExistingTable"));
    GTGlobals::sleep();

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
    GTGlobals::sleep();

//    Expected state: it is visible and enabled.
    chbUsePatternNames = GTWidget::findExactWidget<QCheckBox *>(os, "chbUsePatternNames");
    CHECK_SET_ERR(NULL != chbUsePatternNames, "chbUsePatternNames is NULL");
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
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

            setAnnotationName(os, "test_0041_1", dialog);
            setGenbankLocation(os, "1..100", dialog);
            GTLineEdit::setText(os, GTWidget::findExactWidget<QLineEdit *>(os, "leDescription", dialog), "");

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, new Scenario1));
    openFileAndCallCreateAnnotationDialog(os, dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    Expected state: a new annotation appears, it hasn't qualifier "note".
    GTUtilsAnnotationsTreeView::selectItems(os, QStringList() << "test_0041_1");
    QTreeWidgetItem *descriptionItem = GTUtilsAnnotationsTreeView::findItem(os, "note", GTGlobals::FindOptions(false));
    CHECK_SET_ERR(NULL == descriptionItem, "There is an unexpected note qualifier");

//    4. Call "Create new annotation" dialog.
//    5. Create an annotation with some description.
    class Scenario2 : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

            setAnnotationName(os, "test_0041_2", dialog);
            setGenbankLocation(os, "100..200", dialog);
            GTLineEdit::setText(os, GTWidget::findExactWidget<QLineEdit *>(os, "leDescription", dialog), "test_0041_2 description");

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, new Scenario2));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "New annotation");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    Expected state: a new annotation appears, it has a qualifier "note" with description.
    const QString description = GTUtilsAnnotationsTreeView::getQualifierValue(os, "note", "test_0041_2");
    CHECK_SET_ERR("test_0041_2 description" == description,
                  QString("An unexpected annotation description: expect '%1', got '%2'")
                  .arg("test_0041_2 description").arg(description));
}

GUI_TEST_CLASS_DEFINITION(test_0042) {
//    Test annotation description field in an embedded widget

//    1. Open "data/samples/FASTA/human_T1.fa".
//    2. Call "Smith-Waterman" dialog. Set any pattern. Open "Input and output" tab.

    class Scenario1 : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

            setSmithWatermanPatternAndOpenLastTab(os, dialog);

//    3. Ensure that description field is empty. Accept the dialog.
            setAnnotationName(os, "test_0042_1", dialog);
            GTLineEdit::setText(os, GTWidget::findExactWidget<QLineEdit *>(os, "leDescription", dialog), "");

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new SmithWatermanDialogFiller(os, new Scenario1));
    openFileAndCallSmithWatermanDialog(os, dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    Expected state: a new annotation appears, it hasn't qualifier "note".
    GTUtilsAnnotationsTreeView::selectItems(os, QStringList() << "test_0042_1");
    QTreeWidgetItem *descriptionItem = GTUtilsAnnotationsTreeView::findItem(os, "note", GTGlobals::FindOptions(false));
    CHECK_SET_ERR(NULL == descriptionItem, "There is an unexpected note qualifier");

//    4. Call "Smith-Waterman" dialog. Set any pattern. Open "Input and output" tab.

    class Scenario2 : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

            setSmithWatermanPatternAndOpenLastTab(os, dialog);

//    5. Enter any description. Accept the dialog.
            setAnnotationName(os, "test_0042_2", dialog);
            GTLineEdit::setText(os, GTWidget::findExactWidget<QLineEdit *>(os, "leDescription", dialog), "test_0042_2 description");

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new SmithWatermanDialogFiller(os, new Scenario2));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Find pattern [Smith-Waterman]");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    Expected state: a new annotation appears, it has a qualifier "note" with description.
    const QString description = GTUtilsAnnotationsTreeView::getQualifierValue(os, "note", "test_0042_2");
    CHECK_SET_ERR("test_0042_2 description" == description,
                  QString("An unexpected annotation description: expect '%1', got '%2'")
                  .arg("test_0042_2 description").arg(description));
}

GUI_TEST_CLASS_DEFINITION(test_0043) {
//    Test annotation description field in options panel widget

//    1. Open "data/samples/FASTA/human_T1.fa".
    openFileOpenSearchTabAndSetPattern(os, dataDir + "samples/FASTA/human_T1.fa");

//    2. Open "Search in Sequence" options panel tab. Set any pattern. Open "Annotation parameters" group.
    GTUtilsOptionPanelSequenceView::openAnnotationParametersShowHideWidget(os);

//    3. Ensure that description field is empty. Click "Create annotations" button.
    setAnnotationName(os, "test_0043_1");
    GTLineEdit::setText(os, GTWidget::findExactWidget<QLineEdit *>(os, "leDescription"), "");

    GTUtilsOptionPanelSequenceView::clickGetAnnotation(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    Expected state: a new annotation appears, it hasn't qualifier "note".
    GTUtilsAnnotationsTreeView::selectItems(os, QStringList() << "test_0043_1");
    QTreeWidgetItem *descriptionItem = GTUtilsAnnotationsTreeView::findItem(os, "note", GTGlobals::FindOptions(false));
    CHECK_SET_ERR(NULL == descriptionItem, "There is an unexpected note qualifier");

//    4. Set any description. Click "Create annotations" button.
    setAnnotationName(os, "test_0043_2");
    GTLineEdit::setText(os, GTWidget::findExactWidget<QLineEdit *>(os, "leDescription"), "test_0043_2 description");

    GTUtilsOptionPanelSequenceView::clickGetAnnotation(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    Expected state: a new annotation appears, it has a qualifier "note" with description.
    const QString description = GTUtilsAnnotationsTreeView::getQualifierValue(os, "note", "test_0043_2");
    CHECK_SET_ERR("test_0043_2 description" == description,
                  QString("An unexpected annotation description: expect '%1', got '%2'")
                  .arg("test_0043_2 description").arg(description));
}

GUI_TEST_CLASS_DEFINITION(test_0044) {
/* 1. Open "_common_data/genbank/1seq.gen".
 * 2. Try to add annotation to opened file
 * Expected state: option is avaliable and no errors showed
*/
    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

            setAnnotationName(os, "test_0044", dialog);
            setGenbankLocation(os, "10..20", dialog);
            QRadioButton *rbExistingTable = GTWidget::findExactWidget<QRadioButton *>(os, "rbExistingTable");
            CHECK_SET_ERR(NULL != rbExistingTable, "rbExistingTable is NULL");
            CHECK_SET_ERR(rbExistingTable->isEnabled(), "rbExistingTable is unexpectedly disabled");

            QComboBox *cbExistingTable = GTWidget::findExactWidget<QComboBox *>(os, "cbExistingTable");
            CHECK_SET_ERR(NULL != cbExistingTable, "cbExistingTable is NULL");
            CHECK_SET_ERR(cbExistingTable->isEnabled(), "cbExistingTable is unexpectedly disnabled");

            QToolButton *tbBrowseExistingTable = GTWidget::findExactWidget<QToolButton *>(os, "tbBrowseExistingTable");
            CHECK_SET_ERR(NULL != tbBrowseExistingTable, "tbBrowseExistingTable is NULL");
            CHECK_SET_ERR(tbBrowseExistingTable->isEnabled(), "tbBrowseExistingTable is unexpectedly disnabled");

            CHECK_SET_ERR(rbExistingTable->isChecked(), "rbExistingTable is unexpectedly unchecked");
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, new Scenario));
    openFileAndCallCreateAnnotationDialog(os, testDir + "_common_data/genbank/1seq.gen");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    Expected state: there is an annotation with type "Misc. Feature".
    const QString type = GTUtilsAnnotationsTreeView::getAnnotationType(os, "test_0044");
    CHECK_SET_ERR("Misc. Feature" == type, QString("An unexpected annotation type: expect '%1', got '%2'")
                  .arg("Misc. Feature").arg(type));

}

GUI_TEST_CLASS_DEFINITION(test_0045) {
/* 1. Open "_common_data/genbank/70Bp2.gen".
 * 2. Try to add annotation to both sequences
 * Expected state: option is avaliable and no errors showed
*/
    class Scenario : public CustomScenario {
        QString annotationName;
    public:
        Scenario(QString annotationName) : annotationName(annotationName) {}
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

            setAnnotationName(os, annotationName, dialog);
            setGenbankLocation(os, "10..20", dialog);
            QRadioButton *rbExistingTable = GTWidget::findExactWidget<QRadioButton *>(os, "rbExistingTable");
            CHECK_SET_ERR(NULL != rbExistingTable, "rbExistingTable is NULL");
            CHECK_SET_ERR(rbExistingTable->isEnabled(), "rbExistingTable is unexpectedly disabled");

            QComboBox *cbExistingTable = GTWidget::findExactWidget<QComboBox *>(os, "cbExistingTable");
            CHECK_SET_ERR(NULL != cbExistingTable, "cbExistingTable is NULL");
            CHECK_SET_ERR(cbExistingTable->isEnabled(), "cbExistingTable is unexpectedly disnabled");

            QToolButton *tbBrowseExistingTable = GTWidget::findExactWidget<QToolButton *>(os, "tbBrowseExistingTable");
            CHECK_SET_ERR(NULL != tbBrowseExistingTable, "tbBrowseExistingTable is NULL");
            CHECK_SET_ERR(tbBrowseExistingTable->isEnabled(), "tbBrowseExistingTable is unexpectedly disnabled");

            CHECK_SET_ERR(rbExistingTable->isChecked(), "rbExistingTable is unexpectedly unchecked");
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os));
    GTFileDialog::openFile(os, testDir + "_common_data/genbank/70Bp2.gen");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTWidget::click(os, GTWidget::findWidget(os, "ADV_single_sequence_widget_0"));
    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, new Scenario("test_0045_1")));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "New annotation");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTWidget::click(os, GTWidget::findWidget(os, "ADV_single_sequence_widget_1"));
    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, new Scenario("test_0045_2")));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "New annotation");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //Expected state: there is an annotation with type "Misc. Feature".
    QString type = GTUtilsAnnotationsTreeView::getAnnotationType(os, "test_0045_1");
    CHECK_SET_ERR("Misc. Feature" == type, QString("An unexpected annotation type: expect '%1', got '%2'")
                  .arg("Misc. Feature").arg(type));

    //Expected state: there is an annotation with type "Misc. Feature".
    type = GTUtilsAnnotationsTreeView::getAnnotationType(os, "test_0045_2");
        CHECK_SET_ERR("Misc. Feature" == type, QString("An unexpected annotation type: expect '%1', got '%2'")
                      .arg("Misc. Feature").arg(type));

}

}   // namespace GUITest_common_scenarios_create_annotation_widget

}   // namespace U2
