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

#include "ETSProjectViewItemsController.h"

#include <QMainWindow>
#include <QMessageBox>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DocumentSelection.h>
#include <U2Core/GObjectSelection.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/SelectionModel.h>
#include <U2Core/SelectionUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UserApplicationsSettings.h>

#include <U2Gui/ProjectView.h>

#include "ExternalToolSupportSettingsController.h"
#include "blast/BlastSupport.h"
#include "blast/MakeBlastDbDialog.h"
#include "blast/MakeBlastDbTask.h"

namespace U2 {

ETSProjectViewItemsController::ETSProjectViewItemsController(QObject* p)
    : QObject(p) {
    makeBlastDbOnSelectionAction = new ExternalToolSupportAction(tr("BLAST make DB..."), this, {BlastSupport::ET_MAKEBLASTDB_ID});
    connect(makeBlastDbOnSelectionAction, &QAction::triggered, this, &ETSProjectViewItemsController::sl_runMakeBlastDbOnSelection);

    ProjectView* pv = AppContext::getProjectView();
    SAFE_POINT(pv != nullptr, "No project view found", );
    connect(pv, SIGNAL(si_onDocTreePopupMenuRequested(QMenu&)), SLOT(sl_addToProjectViewMenu(QMenu&)));
}

void ETSProjectViewItemsController::sl_addToProjectViewMenu(QMenu& m) {
    ProjectView* pv = AppContext::getProjectView();
    SAFE_POINT(pv != nullptr, "No project view found", );

    MultiGSelection ms;  // ms.addSelection(pv->getGObjectSelection());
    ms.addSelection(pv->getDocumentSelection());
    QList<Document*> set = SelectionUtils::getSelectedDocs(ms);
    bool hasFastaDocs = false;
    foreach (Document* doc, set) {
        if (doc->getDocumentFormatId() == BaseDocumentFormats::FASTA) {
            hasFastaDocs = true;
            break;
        }
    }
    if (hasFastaDocs) {
        QMenu* subMenu = m.addMenu(tr("BLAST"));
        subMenu->menuAction()->setObjectName(ACTION_BLAST_SUBMENU);
        subMenu->setIcon(QIcon(":external_tool_support/images/ncbi.png"));
        subMenu->addAction(makeBlastDbOnSelectionAction);
    }
}

void ETSProjectViewItemsController::sl_runMakeBlastDbOnSelection() {
    ProjectView* projectView = AppContext::getProjectView();
    SAFE_POINT(projectView != nullptr, "ProjectView is null!", );

    MultiGSelection ms;
    ms.addSelection(projectView->getGObjectSelection());
    ms.addSelection(projectView->getDocumentSelection());
    MakeBlastDbSettings settings;
    foreach (Document* doc, projectView->getDocumentSelection()->getSelectedDocuments()) {
        if (doc->getDocumentFormatId() == BaseDocumentFormats::FASTA) {
            settings.inputFilesPath.append(doc->getURLString());

            const QList<GObject*>& objects = doc->getObjects();
            SAFE_POINT(!objects.isEmpty(), "FASTA document: sequence objects count error", );
            auto seqObj = dynamic_cast<U2SequenceObject*>(objects.first());
            if (seqObj != nullptr) {
                SAFE_POINT(seqObj->getAlphabet() != nullptr,
                           QString("Alphabet for '%1' is not set").arg(seqObj->getGObjectName()), );
                const DNAAlphabet* alphabet = seqObj->getAlphabet();
                settings.isInputAmino = alphabet->isAmino();
            }
        }
    }
    QObjectScopedPointer<MakeBlastDbDialog> makeBlastDbDialog = new MakeBlastDbDialog(AppContext::getMainWindow()->getQMainWindow(), settings);
    makeBlastDbDialog->exec();
    CHECK(!makeBlastDbDialog.isNull() && makeBlastDbDialog->result() == QDialog::Accepted, );

    CHECK(BlastSupport::checkBlastTool(BlastSupport::ET_MAKEBLASTDB_ID), );
    AppContext::getTaskScheduler()->registerTopLevelTask(new MakeBlastDbTask(makeBlastDbDialog->getTaskSettings()));
}

}  // namespace U2
