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

#include <U2Gui/GUIUtils.h>

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
}

void TCoffeeSupport::sl_runWithExtFileSpecify() {
    //Check that T-Coffee and temporary folder path defined
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

    //Call select input file and setup settings dialog
    TCoffeeSupportTaskSettings settings;
    QObjectScopedPointer<TCoffeeWithExtFileSpecifySupportRunDialog> tCoffeeRunDialog = new TCoffeeWithExtFileSpecifySupportRunDialog(settings, AppContext::getMainWindow()->getQMainWindow());
    tCoffeeRunDialog->exec();
    CHECK(!tCoffeeRunDialog.isNull() && tCoffeeRunDialog->result() == QDialog::Accepted, );
    SAFE_POINT(!settings.inputFilePath.isEmpty(), "inputFilePath is empty", );

    AppContext::getTaskScheduler()->registerTopLevelTask(new TCoffeeWithExtFileSpecifySupportTask(settings));
}

////////////////////////////////////////
//TCoffeeSupportContext
TCoffeeSupportContext::TCoffeeSupportContext(QObject *p)
    : GObjectViewWindowContext(p, MsaEditorFactory::ID) {
}

void TCoffeeSupportContext::initViewContext(GObjectView *view) {
    MSAEditor *msaEditor = qobject_cast<MSAEditor *>(view);
    SAFE_POINT(msaEditor != nullptr, "Invalid GObjectView", );
    CHECK(msaEditor->getMaObject() != nullptr, );

    bool objLocked = msaEditor->getMaObject()->isStateLocked();
    bool isMsaEmpty = msaEditor->isAlignmentEmpty();

    AlignMsaAction *alignAction = new AlignMsaAction(this, TCoffeeSupport::ET_TCOFFEE_ID, view, tr("Align with T-Coffee..."), 2000);
    alignAction->setObjectName("Align with T-Coffee");

    addViewAction(alignAction);
    alignAction->setEnabled(!objLocked && !isMsaEmpty);

    connect(msaEditor->getMaObject(), SIGNAL(si_lockedStateChanged()), alignAction, SLOT(sl_updateState()));
    connect(msaEditor->getMaObject(), SIGNAL(si_alignmentBecomesEmpty(bool)), alignAction, SLOT(sl_updateState()));
    connect(alignAction, SIGNAL(triggered()), SLOT(sl_align_with_TCoffee()));
}

void TCoffeeSupportContext::buildMenu(GObjectView *view, QMenu *m) {
    QList<GObjectViewAction *> actions = getViewActions(view);
    QMenu *alignMenu = GUIUtils::findSubMenu(m, MSAE_MENU_ALIGN);
    SAFE_POINT(alignMenu != nullptr, "alignMenu", );
    for (GObjectViewAction *a : qAsConst(actions)) {
        a->addToMenuWithOrder(alignMenu);
    }
}

void TCoffeeSupportContext::sl_align_with_TCoffee() {
    //Check that T-Coffee and temporary folder path defined
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

    //Call run T-Coffee align dialog
    AlignMsaAction *action = qobject_cast<AlignMsaAction *>(sender());
    CHECK(action != nullptr, );

    MSAEditor *msaEditor = action->getMsaEditor();
    MultipleSequenceAlignmentObject *obj = msaEditor->getMaObject();
    CHECK(obj != nullptr && !obj->isStateLocked(), )

    TCoffeeSupportTaskSettings settings;
    QObjectScopedPointer<TCoffeeSupportRunDialog> tCoffeeRunDialog = new TCoffeeSupportRunDialog(settings, AppContext::getMainWindow()->getQMainWindow());
    tCoffeeRunDialog->exec();
    CHECK(!tCoffeeRunDialog.isNull() && tCoffeeRunDialog->result() == QDialog::Accepted, );

    TCoffeeSupportTask *tCoffeeSupportTask = new TCoffeeSupportTask(obj->getMultipleAlignment(), GObjectReference(obj), settings);
    connect(obj, SIGNAL(destroyed()), tCoffeeSupportTask, SLOT(cancel()));
    AppContext::getTaskScheduler()->registerTopLevelTask(tCoffeeSupportTask);

    // Turn off rows collapsing
    msaEditor->resetCollapsibleModel();
}

}    // namespace U2
