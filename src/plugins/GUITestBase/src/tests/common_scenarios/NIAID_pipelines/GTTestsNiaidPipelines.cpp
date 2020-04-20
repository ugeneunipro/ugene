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

#include <drivers/GTMouseDriver.h>
#include <drivers/GTKeyboardDriver.h>
#include "utils/GTKeyboardUtils.h"
#include <primitives/GTWidget.h>
#include <base_dialogs/GTFileDialog.h>
#include "primitives/GTMenu.h"
#include "GTGlobals.h"
#include <primitives/GTTreeWidget.h>
#include "primitives/GTAction.h"
#include "system/GTFile.h"
#include "primitives/PopupChooser.h"
#include "runnables/ugene/plugins/workflow_designer/ConfigurationWizardFiller.h"
#include "runnables/ugene/plugins/workflow_designer/WizardFiller.h"
#include "runnables/ugene/plugins/workflow_designer/StartupDialogFiller.h"
#include "runnables/ugene/ugeneui/SequenceReadingModeSelectorDialogFiller.h"
#include "GTUtilsWorkflowDesigner.h"
#include "utils/GTUtilsApp.h"
#include "GTUtilsWizard.h"

#include <U2Core/AppContext.h>
#include <U2Gui/ToolsMenu.h>

#include <QApplication>
#include <QGraphicsItem>
#include <QWizard>
#include <QLineEdit>

#include <QProcess>

namespace U2 {

namespace GUITest_common_scenarios_NIAID_pipelines {

GUI_TEST_CLASS_DEFINITION(test_0001){
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    GTUtilsWorkflowDesigner::addSample(os,"call variants");
    //GTUtilsDialog::waitForDialog(os, new WizardFiller0001(os,"BED or position list file"));
    QAbstractButton* wiz = GTAction::button(os, "Show wizard");
    GTWidget::click(os,wiz);

    TaskScheduler* scheduller = AppContext::getTaskScheduler();

    GTGlobals::sleep(5000);
    while(!scheduller->getTopLevelTasks().isEmpty()){
        GTGlobals::sleep();
    }
    GTUtilsWizard::clickButton(os, GTUtilsWizard::Cancel);
}
#define GT_CLASS_NAME "GTUtilsDialog::WizardFiller0002"
#define GT_METHOD_NAME "run"
class WizardFiller0002:public WizardFiller{
public:
    WizardFiller0002(HI::GUITestOpStatus &_os):WizardFiller(_os,"Tuxedo Wizard"){}
    void run(){
        QWidget* dialog = QApplication::activeModalWidget();
        GT_CHECK(dialog, "activeModalWidget is NULL");

        QList<QWidget*> list= dialog->findChildren<QWidget*>();

        QList<QWidget*> datasetList;
        foreach(QWidget* act, list){
            if(act->objectName()=="DatasetWidget")
            datasetList.append(act);
        }
        QWidget* dataset = datasetList.takeLast();

        QPushButton* cancel = qobject_cast<QPushButton*>(GTWidget::findButtonByText(os, "Cancel", dialog));

        GT_CHECK(dataset,"dataset widget not found");
        GT_CHECK(cancel, "cancel button not found");

        QPoint i = dataset->mapToGlobal(dataset->rect().bottomLeft());
        QPoint j = cancel->mapToGlobal(cancel->rect().topLeft());

        CHECK_SET_ERR(qAbs(i.y()-j.y())<100, QString("%1   %2").arg(i.y()).arg(j.y()));
        GTUtilsWizard::clickButton(os, GTUtilsWizard::Cancel);
    }
};
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

GUI_TEST_CLASS_DEFINITION(test_0002){
//    1. Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

//    2. Open tuxedo pipeline from samples
    GTUtilsDialog::waitForDialog(os, new WizardFiller0002(os));
    GTUtilsDialog::waitForDialog(os, new ConfigurationWizardFiller(os, "Configure Tuxedo Workflow", QStringList()<<
                                                                   "Full"<<"Single-end"));
    GTUtilsWorkflowDesigner::addSample(os,"Tuxedo tools");
    GTGlobals::sleep();
//    3. Open wizard


//    Expected state: dataset widget fits full height
}

} // namespace GUITest_common_scenarios_annotations_edit
} // namespace U2
