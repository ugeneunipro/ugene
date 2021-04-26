/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2021 UniPro <ugene@unipro.ru>
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

#include "GTTestsNGS_WD.h"

#include <GTUtilsTaskTreeView.h>
#include <base_dialogs/DefaultDialogFiller.h>
#include <base_dialogs/MessageBoxFiller.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTMenu.h>
#include <primitives/GTWidget.h>

#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QTableWidget>
#include <QTreeWidget>

#include <U2Core/AppContext.h>
#include <U2Core/AppResources.h>
#include <U2Core/AppSettings.h>

#include <harness/UGUITest.h>

#include "utils/GTUtilsDialog.h"
#include "GTUtilsWorkflowDesigner.h"
#include "runnables/ugene/corelibs/U2Gui/AppSettingsDialogFiller.h"
#include "runnables/ugene/plugins/workflow_designer/ConfigurationWizardFiller.h"
#include "runnables/ugene/plugins/workflow_designer/WizardFiller.h"

#include <drivers/GTKeyboardDriver.h>
#include "GTUtilsWizard.h"

namespace U2 {

namespace GUITest_common_scenarios_ngs_workflow_desingner {
using namespace HI;

GUI_TEST_CLASS_DEFINITION(test_0001) {
    class TrimmomaticScenario : public CustomScenario {
         void run(HI::GUITestOpStatus &os) {
             QWidget *dialog = QApplication::activeModalWidget();
             CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

             GTWidget::click(os, GTWidget::findWidget(os, "buttonAdd"));
             QMenu *menu = qobject_cast<QMenu *>(GTWidget::findWidget(os, "stepsMenu"));
             GTMenu::clickMenuItemByName(os, menu, QStringList() << "ILLUMINACLIP");
             GTKeyboardDriver::keyClick(Qt::Key_Escape);

             GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
         }
     };

    class custom : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(dialog, "activeModalWidget is NULL");

            QLineEdit *lineEdit = GTWidget::findWidgetByType<QLineEdit *>(os, dialog, "lineedit not found");
            GTLineEdit::setText(os, lineEdit, QFileInfo(dataDir + "samples/FASTQ/eas.fastq").absoluteFilePath());

            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);

            GTUtilsDialog::waitForDialog(os, new DefaultDialogFiller(os, "TrimmomaticPropertyDialog", QDialogButtonBox::Ok, new TrimmomaticScenario()));

            GTWidget::click(os, GTWidget::findWidget(os, "trimmomaticPropertyToolButton"));

            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Run);
        }
    };


    //1. Click Tools -> NGS data analysis -> Metagenomics classification.... Choose Parallel reads classification and Single-end
    //2. Set "samples/FASTQ/eas.fastq" as input
    //3. Add "ILLUMINACLIP" step
    //4. Click "Next" several times and "Run"
    //5. Wait for workflow finished
    //Expected state: no errors
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsDialog::waitForDialog(os, new ConfigurationWizardFiller(os,
                                                                    "Configure NGS Reads Classification Workflow",
                                                                    QStringList() << "Parallel reads classification" << "Single-end"));
    GTUtilsDialog::waitForDialog(os, new WizardFiller(os, "SE Reads Parallel Classification Wizard", new custom()));

    GTMenu::clickMainMenuItem(os, QStringList() << "Tools"
                                                << "NGS data analysis"
                                                << "Metagenomics classification...");
    GTUtilsTaskTreeView::waitTaskFinished(os, 500000);
    QStringList errors = GTUtilsWorkflowDesigner::getErrors(os);
    CHECK_SET_ERR(errors.size() == 0, "Unexpected errors");
}

GUI_TEST_CLASS_DEFINITION(test_0002) {

    class TrimmomaticScenario : public CustomScenario {
         void run(HI::GUITestOpStatus &os) {
             QWidget *dialog = QApplication::activeModalWidget();
             CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

             GTWidget::click(os, GTWidget::findWidget(os, "buttonAdd"));
             QMenu *menu = qobject_cast<QMenu *>(GTWidget::findWidget(os, "stepsMenu"));
             GTMenu::clickMenuItemByName(os, menu, QStringList() << "ILLUMINACLIP");
             GTKeyboardDriver::keyClick(Qt::Key_Escape);

             GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
         }
     };

    class custom : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(dialog, "activeModalWidget is NULL");

            QLineEdit *lineEdit1 = qobject_cast<QLineEdit *>(GTWidget::findWidget(os, "FASTQ file(s) 1 widget"));
            QLineEdit *lineEdit2 = qobject_cast<QLineEdit *>(GTWidget::findWidget(os, "FASTQ file(s) 2 widget"));
            GTLineEdit::setText(os, lineEdit1, QFileInfo(testDir + "_common_data/metagenomics/workflow_designer/4R-WGA_S31_L001_R1_001.fastq").absoluteFilePath());
            GTLineEdit::setText(os, lineEdit2, QFileInfo(testDir + "_common_data/metagenomics/workflow_designer/4R-WGA_S31_L001_R2_001.fastq").absoluteFilePath());

            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);

            GTUtilsDialog::waitForDialog(os, new DefaultDialogFiller(os, "TrimmomaticPropertyDialog", QDialogButtonBox::Ok, new TrimmomaticScenario()));

            GTWidget::click(os, GTWidget::findWidget(os, "trimmomaticPropertyToolButton"));

            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Run);
        }
    };


    //1. Click Tools -> NGS data analysis -> Metagenomics classification.... Choose Parallel reads classification and Paired-end
    //2. Set "_common_data/metagenomics/workflow_designer/4R-WGA_S31_L001_R1_001.fastq" and _common_data/metagenomics/workflow_designer/4R-WGA_S31_L001_R2_001.fastq as input
    //3. Add "ILLUMINACLIP" step
    //4. Click "Next" several times and "Run"
    //5. Wait for workflow finished
    //Expected state: no errors
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsDialog::waitForDialog(os, new ConfigurationWizardFiller(os,
                                                                    "Configure NGS Reads Classification Workflow",
                                                                    QStringList() << "Parallel reads classification" << "Paired-end"));
    GTUtilsDialog::waitForDialog(os, new WizardFiller(os, "PE Reads Parallel Classification Wizard", new custom()));

    GTMenu::clickMainMenuItem(os, QStringList() << "Tools"
                                                << "NGS data analysis"
                                                << "Metagenomics classification...");
    GTUtilsTaskTreeView::waitTaskFinished(os, 500000);
    QStringList errors = GTUtilsWorkflowDesigner::getErrors(os);
    CHECK_SET_ERR(errors.size() == 0, "Unexpected errors");
}

GUI_TEST_CLASS_DEFINITION(test_0003) {
    class TrimmomaticScenario : public CustomScenario {
         void run(HI::GUITestOpStatus &os) {
             QWidget *dialog = QApplication::activeModalWidget();
             CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

             GTWidget::click(os, GTWidget::findWidget(os, "buttonAdd"));
             QMenu *menu = qobject_cast<QMenu *>(GTWidget::findWidget(os, "stepsMenu"));
             GTMenu::clickMenuItemByName(os, menu, QStringList() << "ILLUMINACLIP");
             GTKeyboardDriver::keyClick(Qt::Key_Escape);

             GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
         }
     };

    class custom : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(dialog, "activeModalWidget is NULL");


            QLineEdit *lineEdit = GTWidget::findWidgetByType<QLineEdit *>(os, dialog, "lineedit not found");
            GTLineEdit::setText(os, lineEdit, QFileInfo(dataDir + "samples/FASTQ/eas.fastq").absoluteFilePath());

            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);

            GTUtilsDialog::waitForDialog(os, new DefaultDialogFiller(os, "TrimmomaticPropertyDialog", QDialogButtonBox::Ok, new TrimmomaticScenario()));

            GTWidget::click(os, GTWidget::findWidget(os, "trimmomaticPropertyToolButton"));

            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Run);
        }
    };


    //1. Click Tools -> NGS data analysis -> Metagenomics classification.... Choose Serial reads classification and Single-end
    //2. Set "samples/FASTQ/eas.fastq" as input
    //3. Add "ILLUMINACLIP" step
    //4. Click "Next" several times and "Run"
    //5. Wait for workflow finished
    //Expected state: no errors
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsDialog::waitForDialog(os, new ConfigurationWizardFiller(os,
                                                                    "Configure NGS Reads Classification Workflow",
                                                                    QStringList() << "Serial reads classification" << "Single-end"));
    GTUtilsDialog::waitForDialog(os, new WizardFiller(os, "SE Reads Serial Classification Wizard", new custom()));

    GTMenu::clickMainMenuItem(os, QStringList() << "Tools"
                                                << "NGS data analysis"
                                                << "Metagenomics classification...");
    GTUtilsTaskTreeView::waitTaskFinished(os, 500000);
    QStringList errors = GTUtilsWorkflowDesigner::getErrors(os);
    CHECK_SET_ERR(errors.size() == 0, "Unexpected errors");
}

GUI_TEST_CLASS_DEFINITION(test_0004) {
    class TrimmomaticScenario : public CustomScenario {
         void run(HI::GUITestOpStatus &os) {
             QWidget *dialog = QApplication::activeModalWidget();
             CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

             GTWidget::click(os, GTWidget::findWidget(os, "buttonAdd"));
             QMenu *menu = qobject_cast<QMenu *>(GTWidget::findWidget(os, "stepsMenu"));
             GTMenu::clickMenuItemByName(os, menu, QStringList() << "ILLUMINACLIP");
             GTKeyboardDriver::keyClick(Qt::Key_Escape);

             GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
         }
     };

    class custom : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(dialog, "activeModalWidget is NULL");

            QLineEdit *lineEdit1 = qobject_cast<QLineEdit *>(GTWidget::findWidget(os, "FASTQ file(s) 1 widget"));
            QLineEdit *lineEdit2 = qobject_cast<QLineEdit *>(GTWidget::findWidget(os, "FASTQ file(s) 2 widget"));
            GTLineEdit::setText(os, lineEdit1, QFileInfo(testDir + "_common_data/metagenomics/workflow_designer/4R-WGA_S31_L001_R1_001.fastq").absoluteFilePath());
            GTLineEdit::setText(os, lineEdit2, QFileInfo(testDir + "_common_data/metagenomics/workflow_designer/4R-WGA_S31_L001_R2_001.fastq").absoluteFilePath());

            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);

            GTUtilsDialog::waitForDialog(os, new DefaultDialogFiller(os, "TrimmomaticPropertyDialog", QDialogButtonBox::Ok, new TrimmomaticScenario()));

            GTWidget::click(os, GTWidget::findWidget(os, "trimmomaticPropertyToolButton"));

            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Run);
        }
    };


    //1. Click Tools -> NGS data analysis -> Metagenomics classification.... Choose Serial reads classification and Paired-end
    //2. Set "_common_data/metagenomics/workflow_designer/4R-WGA_S31_L001_R1_001.fastq" and _common_data/metagenomics/workflow_designer/4R-WGA_S31_L001_R2_001.fastq as input
    //3. Add "ILLUMINACLIP" step
    //4. Click "Next" several times and "Run"
    //5. Wait for workflow finished
    //Expected state: no errors
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsDialog::waitForDialog(os, new ConfigurationWizardFiller(os,
                                                                    "Configure NGS Reads Classification Workflow",
                                                                    QStringList() << "Serial reads classification"  << "Paired-end"));
    GTUtilsDialog::waitForDialog(os, new WizardFiller(os, "PE Reads Serial Classification Wizard", new custom()));

    GTMenu::clickMainMenuItem(os, QStringList() << "Tools"
                                                << "NGS data analysis"
                                                << "Metagenomics classification...");
    GTUtilsTaskTreeView::waitTaskFinished(os, 500000);
    QStringList errors = GTUtilsWorkflowDesigner::getErrors(os);
    CHECK_SET_ERR(errors.size() == 0, "Unexpected errors");
}

GUI_TEST_CLASS_DEFINITION(test_0005) {
    class TrimmomaticScenario : public CustomScenario {
         void run(HI::GUITestOpStatus &os) {
             QWidget *dialog = QApplication::activeModalWidget();
             CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

             GTWidget::click(os, GTWidget::findWidget(os, "buttonAdd"));
             QMenu *menu = qobject_cast<QMenu *>(GTWidget::findWidget(os, "stepsMenu"));
             GTMenu::clickMenuItemByName(os, menu, QStringList() << "ILLUMINACLIP");
             GTKeyboardDriver::keyClick(Qt::Key_Escape);

             GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
         }
     };

    class custom : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(dialog, "activeModalWidget is NULL");


            QLineEdit *lineEdit = GTWidget::findWidgetByType<QLineEdit *>(os, dialog, "lineedit not found");
            GTLineEdit::setText(os, lineEdit, QFileInfo(testDir + "_common_data/metagenomics/workflow_designer/4R-WGA_S31_L001_R2_001.fastq").absoluteFilePath());

            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);

            GTUtilsDialog::waitForDialog(os, new DefaultDialogFiller(os, "TrimmomaticPropertyDialog", QDialogButtonBox::Ok, new TrimmomaticScenario()));

            GTWidget::click(os, GTWidget::findWidget(os, "trimmomaticPropertyToolButton"));

            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Run);
        }
    };


    //1. Click Tools -> NGS data analysis -> Metagenomics classification.... Choose Serial reads classification and Single-end
    //2. Set "samples/FASTQ/eas.fastq" as input
    //3. Add "ILLUMINACLIP" step
    //4. Click "Next" several times and "Run"
    //5. Wait for workflow finished
    //Expected state: no errors
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsDialog::waitForDialog(os, new ConfigurationWizardFiller(os,
                                                                    "Configure NGS Reads Classification Workflow",
                                                                    QStringList() << "Reads de novo assembly and contigs classification" << "Single-end"));
    GTUtilsDialog::waitForDialog(os, new WizardFiller(os, "SE Reads Assembly and Classification Wizard", new custom()));

    GTMenu::clickMainMenuItem(os, QStringList() << "Tools"
                                                << "NGS data analysis"
                                                << "Metagenomics classification...");
    GTUtilsTaskTreeView::waitTaskFinished(os, 500000);
    QStringList errors = GTUtilsWorkflowDesigner::getErrors(os);
    CHECK_SET_ERR(errors.size() == 0, "Unexpected errors");
}

GUI_TEST_CLASS_DEFINITION(test_0006) {
    class TrimmomaticScenario : public CustomScenario {
         void run(HI::GUITestOpStatus &os) {
             QWidget *dialog = QApplication::activeModalWidget();
             CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

             GTWidget::click(os, GTWidget::findWidget(os, "buttonAdd"));
             QMenu *menu = qobject_cast<QMenu *>(GTWidget::findWidget(os, "stepsMenu"));
             GTMenu::clickMenuItemByName(os, menu, QStringList() << "ILLUMINACLIP");
             GTKeyboardDriver::keyClick(Qt::Key_Escape);

             GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
         }
     };

    class custom : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(dialog, "activeModalWidget is NULL");

            QLineEdit *lineEdit1 = qobject_cast<QLineEdit *>(GTWidget::findWidget(os, "FASTQ file(s) 1 widget"));
            QLineEdit *lineEdit2 = qobject_cast<QLineEdit *>(GTWidget::findWidget(os, "FASTQ file(s) 2 widget"));
            GTLineEdit::setText(os, lineEdit1, QFileInfo(testDir + "_common_data/metagenomics/workflow_designer/4R-WGA_S31_L001_R1_001.fastq").absoluteFilePath());
            GTLineEdit::setText(os, lineEdit2, QFileInfo(testDir + "_common_data/metagenomics/workflow_designer/4R-WGA_S31_L001_R2_001.fastq").absoluteFilePath());

            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);

            GTUtilsDialog::waitForDialog(os, new DefaultDialogFiller(os, "TrimmomaticPropertyDialog", QDialogButtonBox::Ok, new TrimmomaticScenario()));

            GTWidget::click(os, GTWidget::findWidget(os, "trimmomaticPropertyToolButton"));

            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Run);
        }
    };


    //1. Click Tools -> NGS data analysis -> Metagenomics classification.... Choose Serial reads classification and Single-end
    //2. Set "samples/FASTQ/eas.fastq" as input
    //3. Add "ILLUMINACLIP" step
    //4. Click "Next" several times and "Run"
    //5. Wait for workflow finished
    //Expected state: no errors
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsDialog::waitForDialog(os, new ConfigurationWizardFiller(os,
                                                                    "Configure NGS Reads Classification Workflow",
                                                                    QStringList() << "Reads de novo assembly and contigs classification" << "Paired-end"));
    GTUtilsDialog::waitForDialog(os, new WizardFiller(os, "PE Reads Assembly and Classification Wizard", new custom()));

    GTMenu::clickMainMenuItem(os, QStringList() << "Tools"
                                                << "NGS data analysis"
                                                << "Metagenomics classification...");
    GTUtilsTaskTreeView::waitTaskFinished(os, 500000);
    QStringList errors = GTUtilsWorkflowDesigner::getErrors(os);
    CHECK_SET_ERR(errors.size() == 0, "Unexpected errors");
}

}    // namespace GUITest_common_scenarios_ngs_workflow_desingner

}
