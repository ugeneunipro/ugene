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

#include <QMainWindow>
#include <QMessageBox>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DocumentSelection.h>
#include <U2Core/GObjectSelection.h>
#include <U2Core/GObjectUtils.h>
#include <U2Core/SelectionModel.h>
#include <U2Core/SelectionUtils.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UserApplicationsSettings.h>

#include <U2Gui/MainWindow.h>
#include <U2Gui/ProjectView.h>
#include <U2Core/QObjectScopedPointer.h>

#include "ETSProjectViewItemsContoller.h"
#include "ExternalToolSupportSettings.h"
#include "ExternalToolSupportSettingsController.h"
#include "blast_plus/FormatDBSupport.h"
#include "blast_plus/FormatDBSupportRunDialog.h"
#include "blast_plus/FormatDBSupportTask.h"

namespace U2 {

ETSProjectViewItemsContoller::ETSProjectViewItemsContoller(QObject* p) : QObject(p) {
    makeBLASTDBOnSelectionAction = new ExternalToolSupportAction(tr("BLAST+ make DB..."), this, QStringList(FormatDBSupport::ET_MAKEBLASTDB_ID));
    connect(makeBLASTDBOnSelectionAction,SIGNAL(triggered()), SLOT(sl_runMakeBlastDbOnSelection()));

    ProjectView* pv = AppContext::getProjectView();
    assert(pv!=NULL);
    connect(pv, SIGNAL(si_onDocTreePopupMenuRequested(QMenu&)), SLOT(sl_addToProjectViewMenu(QMenu&)));
}

void ETSProjectViewItemsContoller::sl_addToProjectViewMenu(QMenu& m) {

    ProjectView* pv = AppContext::getProjectView();
    assert(pv!=NULL);

    MultiGSelection ms; //ms.addSelection(pv->getGObjectSelection());
    ms.addSelection(pv->getDocumentSelection());
    QList<Document*> set = SelectionUtils::getSelectedDocs(ms);
    bool hasFastaDocs=false;
    foreach(Document* doc,set){
        if(doc->getDocumentFormatId() == BaseDocumentFormats::FASTA){
            hasFastaDocs=true;
            break;
        }
    }
    if (hasFastaDocs) {
        QMenu* subMenu = m.addMenu(tr("BLAST"));
        subMenu->menuAction()->setObjectName(ACTION_BLAST_SUBMENU);
        subMenu->setIcon(QIcon(":external_tool_support/images/ncbi.png"));
        subMenu->addAction(makeBLASTDBOnSelectionAction);
    }
}

void ETSProjectViewItemsContoller::sl_runMakeBlastDbOnSelection(){
    ExternalToolSupportAction* s = qobject_cast<ExternalToolSupportAction*>(sender());
    assert(s != NULL);
    //Check that formatDB and temporary folder path defined
    if (AppContext::getExternalToolRegistry()->getById(s->getToolIds().at(0))->getPath().isEmpty()){
        QObjectScopedPointer<QMessageBox> msgBox = new QMessageBox;
        QString toolName = AppContext::getExternalToolRegistry()->getById(s->getToolIds().at(0))->getName();
        msgBox->setWindowTitle("BLAST+ "+s->getToolIds().at(0));
        msgBox->setText(tr("Path for BLAST+ %1 tool not selected.").arg(toolName));
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
               break;
           default:
               assert(false);
               break;
         }
    }
    if (AppContext::getExternalToolRegistry()->getById(s->getToolIds().at(0))->getPath().isEmpty()){
        return;
    }
    U2OpStatus2Log os(LogLevel_DETAILS);
    ExternalToolSupportSettings::checkTemporaryDir(os);
    CHECK_OP(os, );

    ProjectView* pv = AppContext::getProjectView();
    assert(pv!=NULL);

    MultiGSelection ms;
    ms.addSelection(pv->getGObjectSelection());
    ms.addSelection(pv->getDocumentSelection());
    FormatDBSupportTaskSettings settings;
    foreach(Document* doc,pv->getDocumentSelection()->getSelectedDocuments()){
        if(doc->getDocumentFormatId() == BaseDocumentFormats::FASTA){
            settings.inputFilesPath.append(doc->getURLString());

            const QList<GObject*>& objects = doc->getObjects();
            SAFE_POINT(!objects.isEmpty( ), "FASTA document: sequence objects count error", );
            U2SequenceObject *seqObj = dynamic_cast<U2SequenceObject*>(objects.first());
            if ( NULL != seqObj ) {
                SAFE_POINT(seqObj->getAlphabet() != NULL,
                           QString("Alphabet for '%1' is not set").arg(seqObj->getGObjectName()), );
                const DNAAlphabet* alphabet = seqObj->getAlphabet();
                settings.isInputAmino = alphabet->isAmino();
            }
        }
    }
    QString toolId = s->getToolIds().at(0);
    QString toolName = AppContext::getExternalToolRegistry()->getById(toolId)->getName();
    QObjectScopedPointer<FormatDBSupportRunDialog> formatDBRunDialog = new FormatDBSupportRunDialog(toolName, settings, AppContext::getMainWindow()->getQMainWindow());
    formatDBRunDialog->exec();
    CHECK(!formatDBRunDialog.isNull(), );

    if (formatDBRunDialog->result() != QDialog::Accepted){
        return;
    }
    FormatDBSupportTask* formatDBSupportTask = new FormatDBSupportTask(toolId, settings);
    AppContext::getTaskScheduler()->registerTopLevelTask(formatDBSupportTask);
}

}//namespace
