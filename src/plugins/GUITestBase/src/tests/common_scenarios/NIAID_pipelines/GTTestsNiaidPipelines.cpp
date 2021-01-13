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

#include "GTTestsNiaidPipelines.h"
#include <base_dialogs/GTFileDialog.h>
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTTreeWidget.h>
#include <primitives/GTWidget.h>

#include <QApplication>
#include <QFileInfo>
#include <QGraphicsItem>
#include <QLineEdit>
#include <QProcess>
#include <QWizard>

#include <U2Core/AppContext.h>

#include <U2Gui/ToolsMenu.h>

#include "GTGlobals.h"
#include "GTUtilsTaskTreeView.h"
#include "GTUtilsWizard.h"
#include "GTUtilsWorkflowDesigner.h"
#include "primitives/GTAction.h"
#include "primitives/GTLineEdit.h"
#include "primitives/GTMenu.h"
#include "primitives/PopupChooser.h"
#include "runnables/ugene/plugins/workflow_designer/ConfigurationWizardFiller.h"
#include "runnables/ugene/plugins/workflow_designer/StartupDialogFiller.h"
#include "runnables/ugene/plugins/workflow_designer/WizardFiller.h"
#include "runnables/ugene/ugeneui/SequenceReadingModeSelectorDialogFiller.h"
#include "system/GTFile.h"
#include "utils/GTKeyboardUtils.h"
#include "utils/GTUtilsApp.h"

namespace U2 {

namespace GUITest_common_scenarios_NIAID_pipelines {

GUI_TEST_CLASS_DEFINITION(test_0001) {
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    GTUtilsWorkflowDesigner::addSample(os, "call variants");
    //GTUtilsDialog::waitForDialog(os, new WizardFiller0001(os,"BED or position list file"));
    QAbstractButton *wiz = GTAction::button(os, "Show wizard");
    GTWidget::click(os, wiz);

    TaskScheduler *scheduller = AppContext::getTaskScheduler();

    GTGlobals::sleep(5000);
    while (!scheduller->getTopLevelTasks().isEmpty()) {
        GTGlobals::sleep();
    }
    GTUtilsWizard::clickButton(os, GTUtilsWizard::Cancel);
}
#define GT_CLASS_NAME "GTUtilsDialog::WizardFiller0002"
#define GT_METHOD_NAME "run"
class WizardFiller0002 : public WizardFiller {
public:
    WizardFiller0002(HI::GUITestOpStatus &_os)
        : WizardFiller(_os, "Tuxedo Wizard") {
    }
    void run() {
        QWidget *dialog = QApplication::activeModalWidget();
        GT_CHECK(dialog, "activeModalWidget is NULL");

        QList<QWidget *> list = dialog->findChildren<QWidget *>();

        QList<QWidget *> datasetList;
        foreach (QWidget *act, list) {
            if (act->objectName() == "DatasetWidget")
                datasetList.append(act);
        }
        QWidget *dataset = datasetList.takeLast();

        QPushButton *cancel = qobject_cast<QPushButton *>(GTWidget::findButtonByText(os, "Cancel", dialog));

        GT_CHECK(dataset, "dataset widget not found");
        GT_CHECK(cancel, "cancel button not found");

        QPoint i = dataset->mapToGlobal(dataset->rect().bottomLeft());
        QPoint j = cancel->mapToGlobal(cancel->rect().topLeft());

        CHECK_SET_ERR(qAbs(i.y() - j.y()) < 100, QString("%1   %2").arg(i.y()).arg(j.y()));
        GTUtilsWizard::clickButton(os, GTUtilsWizard::Cancel);
    }
};
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

GUI_TEST_CLASS_DEFINITION(test_0002) {
    //    1. Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    //    2. Open tuxedo pipeline from samples
    GTUtilsDialog::waitForDialog(os, new WizardFiller0002(os));
    GTUtilsDialog::waitForDialog(os, new ConfigurationWizardFiller(os, "Configure Tuxedo Workflow", QStringList() << "Full"
                                                                                                                  << "Single-end"));
    GTUtilsWorkflowDesigner::addSample(os, "Tuxedo tools");
    GTGlobals::sleep();
    //    3. Open wizard

    //    Expected state: dataset widget fits full height
}

GUI_TEST_CLASS_DEFINITION(test_0003) {
    class custom : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(dialog, "activeModalWidget is NULL");

            QLineEdit *lineEdit = GTWidget::findWidgetByType<QLineEdit *>(os, dialog, "lineEdit not found");
            GTLineEdit::setText(os, lineEdit, QFileInfo(dataDir + "cistrome_input/macs_input_chr4/chr4.bed").absoluteFilePath());

            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Run);
        }
    };


    //1. Click Tools -> NGS data analysis -> ChIP-Seq data analysis.... Choose Only treatment tags
    //2. Set "cistrome_input/macs_input_chr4/chr4.bed" as input
    //3. Click "Next" several times and "Run"
    //4. Wait for workflow finished
    //Expected state: no errors
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsDialog::waitForDialog(os, new ConfigurationWizardFiller(os,
                                                                    "Configure Cistrome Workflow",
                                                                    QStringList() << "Only treatment tags"));
    GTUtilsDialog::waitForDialog(os, new WizardFiller(os, "ChIP-seq Analysis Wizard", new custom()));

    GTMenu::clickMainMenuItem(os, QStringList() << "Tools"
                                                << "NGS data analysis"
                                                << "ChIP-Seq data analysis...");
    GTUtilsTaskTreeView::waitTaskFinished(os, 60 * 1000 * 20);
    QStringList errors = GTUtilsWorkflowDesigner::getErrors(os);
    CHECK_SET_ERR(errors.size() == 0, "Unexpected errors");
}

GUI_TEST_CLASS_DEFINITION(test_0004) {
    class custom : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(dialog, "activeModalWidget is NULL");

            QLineEdit *lineEdit1 = qobject_cast<QLineEdit *>(GTWidget::findWidget(os, "Treatment FASTQ widget"));
            QLineEdit *lineEdit2 = qobject_cast<QLineEdit *>(GTWidget::findWidget(os, "Control FASTQ widget"));
            GTLineEdit::setText(os, lineEdit1, QFileInfo(dataDir + "cistrome_input/macs_input_chr4/chr4.bed").absoluteFilePath());
            GTLineEdit::setText(os, lineEdit2, QFileInfo(dataDir + "cistrome_input/macs_input_chr4/control_tags/chr4.bed").absoluteFilePath());

            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Run);
        }
    };


    //1. Click Tools -> NGS data analysis -> ChIP-Seq data analysis.... Choose Treatment and control
    //2. Set "cistrome_input/macs_input_chr4/chr4.bed" as "Treatment" and "cistrome_input/macs_input_chr4/control_tags/chr4.bed" as "Control"
    //3. Click "Next" several times and "Run"
    //4. Wait for workflow finished
    //Expected state: no errors
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsDialog::waitForDialog(os, new ConfigurationWizardFiller(os,
                                                                    "Configure Cistrome Workflow",
                                                                    QStringList() << "Treatment and control"));
    GTUtilsDialog::waitForDialog(os, new WizardFiller(os, "ChIP-Seq Analysis Wizard", new custom()));

    GTMenu::clickMainMenuItem(os, QStringList() << "Tools"
                                                << "NGS data analysis"
                                                << "ChIP-Seq data analysis...");
    GTUtilsTaskTreeView::waitTaskFinished(os, 60 * 1000 * 20);
    QStringList errors = GTUtilsWorkflowDesigner::getErrors(os);
    CHECK_SET_ERR(errors.size() == 0, "Unexpected errors");
}

}    // namespace GUITest_common_scenarios_NIAID_pipelines
}    // namespace U2
