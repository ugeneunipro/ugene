/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include "KalignSupport.h"

#include <QMainWindow>
#include <QMessageBox>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/MsaObject.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UserApplicationsSettings.h>

#include <U2View/MaEditorFactory.h>
#include <U2View/MsaEditor.h>

#include "ExternalToolSupportSettings.h"
#include "ExternalToolSupportSettingsController.h"
#include "KalignSupportRunDialog.h"
#include "KalignSupportTask.h"
#include "kalign/pairwise/KalignPairwiseAlignmentGUIExtensionFactory.h"
#include "kalign/pairwise/KalignPairwiseAlignmentTask.h"
#include "utils/AlignMsaAction.h"

namespace U2 {

const QString Kalign3Support::ET_KALIGN_ID = "USUPP_KALIGN";
const QString Kalign3Support::KALIGN_TMP_DIR = "kalign";

Kalign3Support::Kalign3Support()
    : ExternalTool(Kalign3Support::ET_KALIGN_ID, "kalign", "Kalign") {
    if (AppContext::getMainWindow() != nullptr) {
        viewCtx = new Kalign3SupportContext(this);
        icon = QIcon(":external_tool_support/images/kalign.png");
    }
    executableFileName = isOsWindows() ? "kalign.exe" : "kalign";
    validationMessageRegExp = "kalign \\d+\\.\\d+\\.\\d+";
    validationArguments << "--version";
    description = tr("<i>Kalign</i> is a fast multiple sequence alignment program for biological sequences.");
    versionRegExp = QRegExp("kalign (\\d+\\.\\d+\\.\\d+)");
    toolKitName = "Kalign";

    AppContext::getAlignmentAlgorithmsRegistry()->registerAlgorithm(new Kalign3PairwiseAlignmentAlgorithm());
}

GObjectViewWindowContext* Kalign3Support::getViewContext() const {
    return viewCtx;
}

void Kalign3Support::sl_runWithExternalFile() {
    // Check that the tool path and temporary folder path defined
    if (path.isEmpty()) {
        QObjectScopedPointer<QMessageBox> msgBox = new QMessageBox();
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
    Kalign3Settings settings;
    QObjectScopedPointer<Kalign3DialogWithFileInput> dialog(new Kalign3DialogWithFileInput(AppContext::getMainWindow()->getQMainWindow(), settings));
    dialog->exec();
    CHECK(!dialog.isNull() && dialog->result() == QDialog::Accepted, );
    AppContext::getTaskScheduler()->registerTopLevelTask(new Kalign3WithExternalFileSupportTask(settings));
}

////////////////////////////////////////
// Kalign3SupportContext
Kalign3SupportContext::Kalign3SupportContext(QObject* p)
    : GObjectViewWindowContext(p, MsaEditorFactory::ID) {
}

void Kalign3SupportContext::initViewContext(GObjectViewController* view) {
    auto msaEditor = qobject_cast<MsaEditor*>(view);
    SAFE_POINT(msaEditor != nullptr, "Invalid GObjectView: not MSAEditor", );
    msaEditor->registerActionProvider(this);

    auto alignAction = new AlignMsaAction(this, Kalign3Support::ET_KALIGN_ID, msaEditor, tr("Align with Kalign..."), 6000);
    alignAction->setObjectName("alignWithKalignAction");
    alignAction->setMenuTypes({MsaEditorMenuType::ALIGN});
    connect(alignAction, &QAction::triggered, this, &Kalign3SupportContext::sl_align);
    addViewAction(alignAction);
}

void Kalign3SupportContext::sl_align() {
    // Check that KAlign and temporary folder path defined
    if (AppContext::getExternalToolRegistry()->getById(Kalign3Support::ET_KALIGN_ID)->getPath().isEmpty()) {
        QObjectScopedPointer<QMessageBox> msgBox = new QMessageBox();
        msgBox->setWindowTitle("Kalign");
        msgBox->setText(tr("Path for Kalign tool is not selected."));
        msgBox->setInformativeText(tr("Do you want to select it now?"));
        msgBox->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox->setDefaultButton(QMessageBox::Yes);
        int rc = msgBox->exec();
        CHECK(!msgBox.isNull() && rc == QMessageBox::Yes, );
        AppContext::getAppSettingsGUI()->showSettingsDialog(ExternalToolSupportSettingsPageId);
    }
    if (AppContext::getExternalToolRegistry()->getById(Kalign3Support::ET_KALIGN_ID)->getPath().isEmpty()) {
        return;
    }
    U2OpStatus2Log os(LogLevel_DETAILS);
    ExternalToolSupportSettings::checkTemporaryDir(os);
    CHECK_OP(os, );

    // Show Kalign settings align dialog.
    auto action = qobject_cast<AlignMsaAction*>(sender());
    CHECK(action != nullptr, );

    MsaEditor* msaEditor = action->getMsaEditor();
    MsaObject* obj = msaEditor->getMaObject();
    CHECK(obj != nullptr && !obj->isStateLocked(), )

    const DNAAlphabet* alphabet = obj->getAlphabet();
    if (!Kalign3SupportTask::isAlphabetSupported(alphabet->getId())) {
        QMessageBox::information(msaEditor->getWidget(),
                                 tr("Unable to align with Kalign"),
                                 tr("Unable to align this Multiple alignment with Kalign.\r\nPlease, convert alignment from %1 alphabet to DNA, RNA or Amino and try again.")
                                     .arg(alphabet->getName()));
        return;
    }

    Kalign3Settings settings;
    QObjectScopedPointer<Kalign3DialogWithMsaInput> dialog(new Kalign3DialogWithMsaInput(AppContext::getMainWindow()->getQMainWindow(), obj->getAlignment(), settings));
    dialog->exec();
    CHECK(!dialog.isNull() && dialog->result() == QDialog::Accepted, );

    auto kalignTask = new Kalign3SupportTask(obj->getAlignment(), GObjectReference(obj), settings);
    connect(obj, &QObject::destroyed, kalignTask, &Task::cancel);
    AppContext::getTaskScheduler()->registerTopLevelTask(kalignTask);

    // Turn off rows collapsing mode.
    msaEditor->resetCollapseModel();
}

}  // namespace U2
