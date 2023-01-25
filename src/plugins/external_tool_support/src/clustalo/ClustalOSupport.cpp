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

#include "ClustalOSupport.h"

#include <QMainWindow>
#include <QMessageBox>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/FileFilters.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UserApplicationsSettings.h>

#include <U2Gui/GUIUtils.h>
#include <U2Gui/LastUsedDirHelper.h>

#include <U2View/MSAEditor.h>
#include <U2View/MaEditorFactory.h>

#include "ClustalOSupportRunDialog.h"
#include "ClustalOSupportTask.h"
#include "ExternalToolSupportSettings.h"
#include "ExternalToolSupportSettingsController.h"

namespace U2 {

const QString ClustalOSupport::ET_CLUSTALO_ID = "USUPP_CLUSTALO";
const QString ClustalOSupport::CLUSTALO_TMP_DIR = "clustalo";

ClustalOSupport::ClustalOSupport()
    : ExternalTool(ClustalOSupport::ET_CLUSTALO_ID, "clustalo", "ClustalO") {
    if (AppContext::getMainWindow() != nullptr) {
        viewCtx = new ClustalOSupportContext(this);
        icon = QIcon(":external_tool_support/images/clustalo.png");
        grayIcon = QIcon(":external_tool_support/images/clustalo_gray.png");
        warnIcon = QIcon(":external_tool_support/images/clustalo_warn.png");
    }
    executableFileName = isOsWindows() ? "ClustalO.exe" : "clustalo";
    validationArguments << "--help";
    validMessage = "Clustal Omega";
    description = tr("<i>Clustal Omega</i> is a free sequence alignment software for proteins.");
    versionRegExp = QRegExp("Clustal Omega - (\\d+\\.\\d+\\.\\d+).*");
    toolKitName = "ClustalO";
}

void ClustalOSupport::sl_runWithExtFileSpecify() {
    // Check that Clustal and temporary folder path defined
    if (path.isEmpty()) {
        QObjectScopedPointer<QMessageBox> msgBox = new QMessageBox;
        msgBox->setWindowTitle(name);
        msgBox->setText(tr("Path for %1 tool not selected.").arg(name));
        msgBox->setInformativeText(tr("Do you want to select it now?"));
        msgBox->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox->setDefaultButton(QMessageBox::Yes);
        const int ret = msgBox->exec();
        CHECK(!msgBox.isNull(), );

        switch (ret) {
            case QMessageBox::Yes:
                AppContext::getAppSettingsGUI()->showSettingsDialog(ExternalToolSupportSettingsPageId);
                break;
            case QMessageBox::No:
                return;
            default:
                assert(false);
        }
    }
    if (path.isEmpty()) {
        return;
    }
    U2OpStatus2Log os(LogLevel_DETAILS);
    ExternalToolSupportSettings::checkTemporaryDir(os);
    CHECK_OP(os, );

    // Call select input file and setup settings dialog
    ClustalOSupportTaskSettings settings;
    QObjectScopedPointer<ClustalOWithExtFileSpecifySupportRunDialog> clustalORunDialog = new ClustalOWithExtFileSpecifySupportRunDialog(settings, AppContext::getMainWindow()->getQMainWindow());
    clustalORunDialog->exec();
    CHECK(!clustalORunDialog.isNull(), );

    if (clustalORunDialog->result() != QDialog::Accepted) {
        return;
    }
    assert(!settings.inputFilePath.isEmpty());

    ClustalOWithExtFileSpecifySupportTask* ClustalOSupportTask = new ClustalOWithExtFileSpecifySupportTask(settings);
    AppContext::getTaskScheduler()->registerTopLevelTask(ClustalOSupportTask);
}

////////////////////////////////////////
// ExternalToolSupportMSAContext
ClustalOSupportContext::ClustalOSupportContext(QObject* p)
    : GObjectViewWindowContext(p, MsaEditorFactory::ID) {
}

void ClustalOSupportContext::initViewContext(GObjectView* view) {
    MSAEditor* msaEditor = qobject_cast<MSAEditor*>(view);
    SAFE_POINT(msaEditor != nullptr, "Invalid GObjectView", );

    msaEditor->registerActionProvider(this);

    auto alignAction = new AlignMsaAction(this, ClustalOSupport::ET_CLUSTALO_ID, msaEditor, tr("Align with ClustalO..."), 2000);
    alignAction->setObjectName("Align with ClustalO");
    alignAction->setMenuTypes({MsaEditorMenuType::ALIGN});
    connect(alignAction, SIGNAL(triggered()), SLOT(sl_align()));
    addViewAction(alignAction);

    auto addAlignmentToAlignmentAction = new AlignMsaAction(this, ClustalOSupport::ET_CLUSTALO_ID, msaEditor, tr("Align alignment to alignment with ClustalO…"), 2001);
    addAlignmentToAlignmentAction->setObjectName("align-alignment-to-alignment-clustalo");
    addAlignmentToAlignmentAction->setMenuTypes({MsaEditorMenuType::ALIGN_NEW_ALIGNMENT_TO_ALIGNMENT});
    connect(addAlignmentToAlignmentAction, SIGNAL(triggered()), SLOT(sl_addAlignmentToAlignment()));
    addViewAction(addAlignmentToAlignmentAction);
}

void ClustalOSupportContext::sl_align() {
    U2OpStatus2Log os(LogLevel_DETAILS);
    checkClustalOSetup(os);
    CHECK_OP(os, );

    // Call run ClustalO align dialog
    auto action = qobject_cast<AlignMsaAction*>(sender());
    SAFE_POINT(action != nullptr, "Sender is not 'AlignMsaAction'", );
    MSAEditor* msaEditor = action->getMsaEditor();
    MultipleSequenceAlignmentObject* obj = msaEditor->getMaObject();
    if (obj == nullptr || obj->isStateLocked()) {
        return;
    }

    ClustalOSupportTaskSettings settings;
    QObjectScopedPointer<ClustalOSupportRunDialog> clustalORunDialog = new ClustalOSupportRunDialog(obj->getMultipleAlignment(),
                                                                                                    settings,
                                                                                                    AppContext::getMainWindow()->getQMainWindow());
    clustalORunDialog->exec();
    CHECK(!clustalORunDialog.isNull(), );

    if (clustalORunDialog->result() != QDialog::Accepted) {
        return;
    }

    auto clustalOSupportTask = new ClustalOSupportTask(obj->getMultipleAlignment(), GObjectReference(obj), settings);
    connect(obj, SIGNAL(destroyed()), clustalOSupportTask, SLOT(cancel()));
    AppContext::getTaskScheduler()->registerTopLevelTask(clustalOSupportTask);

    // Turn off rows collapsing mode.
    msaEditor->resetCollapseModel();
}

void ClustalOSupportContext::sl_addAlignmentToAlignment() {
    U2OpStatus2Log os(LogLevel_DETAILS);
    checkClustalOSetup(os);
    CHECK_OP(os, );

    // Call run ClustalO align dialog
    auto action = qobject_cast<AlignMsaAction*>(sender());
    SAFE_POINT(action != nullptr, "Sender is not 'AlignMsaAction'", );
    MSAEditor* msaEditor = action->getMsaEditor();
    MultipleSequenceAlignmentObject* msaObject = msaEditor->getMaObject();

    DocumentFormatConstraints c;
    QString filter = FileFilters::createFileFilterByObjectTypes({GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT, GObjectTypes::SEQUENCE});

    LastUsedDirHelper lod;
    lod.url = U2FileDialog::getOpenFileName(nullptr, tr("Select file with another alignment"), lod, filter);
    CHECK(!lod.url.isEmpty(), );

    ClustalOSupportTaskSettings settings;
    auto clustalOSupportTask = new ClustalOSupportTask(msaObject->getMultipleAlignment(), GObjectReference(msaObject), lod.url, settings);
    connect(msaObject, SIGNAL(destroyed()), clustalOSupportTask, SLOT(cancel()));
    AppContext::getTaskScheduler()->registerTopLevelTask(clustalOSupportTask);

    // Turn off rows collapsing mode.
    msaEditor->resetCollapseModel();
}

void ClustalOSupportContext::checkClustalOSetup(U2OpStatus& os) {
    QString pathToClustalO = AppContext::getExternalToolRegistry()->getById(ClustalOSupport::ET_CLUSTALO_ID)->getPath();
    if (pathToClustalO.isEmpty()) {
        QObjectScopedPointer<QMessageBox> msgBox = new QMessageBox();
        msgBox->setWindowTitle("ClustalO");
        msgBox->setText(tr("Path for ClustalO tool is not selected."));
        msgBox->setInformativeText(tr("Do you want to select it now?"));
        msgBox->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox->setDefaultButton(QMessageBox::Yes);
        int ret = msgBox->exec();
        CHECK_EXT(!msgBox.isNull(), os.setError(tr("Can't validate ClustalO external tool")), );

        if (ret == QMessageBox::Yes) {
            AppContext::getAppSettingsGUI()->showSettingsDialog(ExternalToolSupportSettingsPageId);
            pathToClustalO = AppContext::getExternalToolRegistry()->getById(ClustalOSupport::ET_CLUSTALO_ID)->getPath();
        }
    }
    CHECK_EXT(!pathToClustalO.isEmpty(), os.setError(tr("ClustalO external tool is not set up")), );
    ExternalToolSupportSettings::checkTemporaryDir(os);
}

}  // namespace U2
