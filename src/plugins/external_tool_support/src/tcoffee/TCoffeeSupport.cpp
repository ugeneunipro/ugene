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

#include "TCoffeeSupport.h"

#include <QMainWindow>
#include <QMessageBox>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UserApplicationsSettings.h>

#include <U2View/MSAEditor.h>
#include <U2View/MaEditorFactory.h>

#include "ExternalToolSupportSettings.h"
#include "ExternalToolSupportSettingsController.h"
#include "TCoffeeSupportRunDialog.h"
#include "TCoffeeSupportTask.h"
#include "utils/AlignMsaAction.h"

namespace U2 {

const QString TCoffeeSupport::ET_TCOFFEE_ID = "USUPP_T_COFFEE";
const QString TCoffeeSupport::TCOFFEE_TMP_DIR = "tcoffee";

TCoffeeSupport::TCoffeeSupport()
    : ExternalTool(TCoffeeSupport::ET_TCOFFEE_ID, "tcoffee", "T-Coffee") {
    if (AppContext::getMainWindow() != nullptr) {
        viewCtx = new TCoffeeSupportContext(this);
        icon = QIcon(":external_tool_support/images/tcoffee.png");
        grayIcon = QIcon(":external_tool_support/images/tcoffee_gray.png");
        warnIcon = QIcon(":external_tool_support/images/tcoffee_warn.png");
    }
#ifdef Q_OS_WIN
    executableFileName = "t_coffee.bat";
#else
#    if defined(Q_OS_UNIX)
    executableFileName = "t_coffee";
#    endif
#endif
    validationArguments << "-help";
    validMessage = "PROGRAM: T-COFFEE";
    description = tr("<i>T-Coffee</i> is a multiple sequence alignment package.");
    versionRegExp = QRegExp("PROGRAM: T-COFFEE Version_(\\d+\\.\\d+)");
    toolKitName = "T-Coffee";
    pathChecks << ExternalTool::PathChecks::SpacesTemporaryDirPath;
    if (isOsLinux()) {
        pathChecks << ExternalTool::PathChecks::NonLatinTemporaryDirPath
                   << ExternalTool::PathChecks::SpacesTemporaryDirPath
                   << ExternalTool::PathChecks::SpacesToolPath
                   << ExternalTool::PathChecks::SpacesArguments;
    }
    if (isOsMac()) {
        //Fill me after UGENE-7831 fix
    }
}

void TCoffeeSupport::sl_runWithExtFileSpecify() {
    // Check that T-Coffee and temporary folder path defined
    if (path.isEmpty()) {
        QObjectScopedPointer<QMessageBox> msgBox = new QMessageBox;
        msgBox->setWindowTitle(name);
        msgBox->setText(tr("Path for %1 tool not selected.").arg(name));
        msgBox->setInformativeText(tr("Do you want to select it now?"));
        msgBox->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox->setDefaultButton(QMessageBox::Yes);
        int rc = msgBox->exec();
        CHECK(rc == QMessageBox::Yes || !msgBox.isNull(), );
        AppContext::getAppSettingsGUI()->showSettingsDialog(ExternalToolSupportSettingsPageId);
    }
    CHECK(!path.isEmpty(), );

    U2OpStatus2Log os(LogLevel_DETAILS);
    ExternalToolSupportSettings::checkTemporaryDir(os);
    CHECK_OP(os, );

    // Call select input file and setup settings dialog
    TCoffeeSupportTaskSettings settings;
    QObjectScopedPointer<TCoffeeWithExtFileSpecifySupportRunDialog> tCoffeeRunDialog = new TCoffeeWithExtFileSpecifySupportRunDialog(settings, AppContext::getMainWindow()->getQMainWindow());
    tCoffeeRunDialog->exec();
    CHECK(!tCoffeeRunDialog.isNull() && tCoffeeRunDialog->result() == QDialog::Accepted, );
    SAFE_POINT(!settings.inputFilePath.isEmpty(), "inputFilePath is empty", );

    AppContext::getTaskScheduler()->registerTopLevelTask(new TCoffeeWithExtFileSpecifySupportTask(settings));
}

////////////////////////////////////////
// TCoffeeSupportContext
TCoffeeSupportContext::TCoffeeSupportContext(QObject* p)
    : GObjectViewWindowContext(p, MsaEditorFactory::ID) {
}

void TCoffeeSupportContext::initViewContext(GObjectViewController* view) {
    auto msaEditor = qobject_cast<MSAEditor*>(view);
    SAFE_POINT(msaEditor != nullptr, "Invalid GObjectView", );
    msaEditor->registerActionProvider(this);

    auto alignAction = new AlignMsaAction(this, TCoffeeSupport::ET_TCOFFEE_ID, msaEditor, tr("Align with T-Coffee..."), 6000);
    alignAction->setObjectName("Align with T-Coffee");
    alignAction->setMenuTypes({MsaEditorMenuType::ALIGN});
    connect(alignAction, SIGNAL(triggered()), SLOT(sl_align_with_TCoffee()));
    addViewAction(alignAction);
}

void TCoffeeSupportContext::sl_align_with_TCoffee() {
    // Check that T-Coffee and temporary folder path defined
    if (AppContext::getExternalToolRegistry()->getById(TCoffeeSupport::ET_TCOFFEE_ID)->getPath().isEmpty()) {
        QObjectScopedPointer<QMessageBox> msgBox = new QMessageBox;
        msgBox->setWindowTitle("T-Coffee");
        msgBox->setText(tr("Path for T-Coffee tool is not selected."));
        msgBox->setInformativeText(tr("Do you want to select it now?"));
        msgBox->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox->setDefaultButton(QMessageBox::Yes);
        int rc = msgBox->exec();
        CHECK(!msgBox.isNull() && rc == QMessageBox::Yes, );
        AppContext::getAppSettingsGUI()->showSettingsDialog(ExternalToolSupportSettingsPageId);
    }
    if (AppContext::getExternalToolRegistry()->getById(TCoffeeSupport::ET_TCOFFEE_ID)->getPath().isEmpty()) {
        return;
    }
    U2OpStatus2Log os(LogLevel_DETAILS);
    ExternalToolSupportSettings::checkTemporaryDir(os);
    CHECK_OP(os, );

    // Call run T-Coffee align dialog
    auto action = qobject_cast<AlignMsaAction*>(sender());
    CHECK(action != nullptr, );

    MSAEditor* msaEditor = action->getMsaEditor();
    MultipleSequenceAlignmentObject* obj = msaEditor->getMaObject();
    CHECK(obj != nullptr && !obj->isStateLocked(), )

    TCoffeeSupportTaskSettings settings;
    QObjectScopedPointer<TCoffeeSupportRunDialog> tCoffeeRunDialog = new TCoffeeSupportRunDialog(settings, AppContext::getMainWindow()->getQMainWindow());
    tCoffeeRunDialog->exec();
    CHECK(!tCoffeeRunDialog.isNull() && tCoffeeRunDialog->result() == QDialog::Accepted, );

    TCoffeeSupportTask* tCoffeeSupportTask = new TCoffeeSupportTask(obj->getMultipleAlignment(), GObjectReference(obj), settings);
    connect(obj, SIGNAL(destroyed()), tCoffeeSupportTask, SLOT(cancel()));
    AppContext::getTaskScheduler()->registerTopLevelTask(tCoffeeSupportTask);

    // Turn off rows collapsing mode.
    msaEditor->resetCollapseModel();
}

}  // namespace U2
