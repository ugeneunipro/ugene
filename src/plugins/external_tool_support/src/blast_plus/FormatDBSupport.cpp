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
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UserApplicationsSettings.h>

#include <U2Gui/DialogUtils.h>
#include <U2Gui/GUIUtils.h>
#include <U2Gui/MainWindow.h>
#include <U2Core/QObjectScopedPointer.h>

#include <U2View/MSAEditor.h>
#include <U2View/MaEditorFactory.h>

#include "ExternalToolSupportSettings.h"
#include "ExternalToolSupportSettingsController.h"
#include "FormatDBSupport.h"
#include "FormatDBSupportRunDialog.h"
#include "FormatDBSupportTask.h"

namespace U2 {

const QString FormatDBSupport::ET_MAKEBLASTDB = "MakeBLASTDB";
const QString FormatDBSupport::ET_MAKEBLASTDB_ID = "USUPP_MAKE_BLAST_DB";
const QString FormatDBSupport::ET_GPU_MAKEBLASTDB = "GPU-MakeBLASTDB";
const QString FormatDBSupport::ET_GPU_MAKEBLASTDB_ID = "UGENE_GPU_MAKE_BLAST_DB";
const QString FormatDBSupport::FORMATDB_TMP_DIR = "FormatDB";

FormatDBSupport::FormatDBSupport(const QString& id, const QString& name, const QString& path) : ExternalTool(id, name, path)
{
    if (AppContext::getMainWindow() != NULL) {
        icon = QIcon(":external_tool_support/images/ncbi.png");
        grayIcon = QIcon(":external_tool_support/images/ncbi_gray.png");
        warnIcon = QIcon(":external_tool_support/images/ncbi_warn.png");
    }
    assert((id == ET_MAKEBLASTDB_ID)||(id == ET_GPU_MAKEBLASTDB_ID));
    if(id == ET_MAKEBLASTDB_ID) {
#ifdef Q_OS_WIN
    executableFileName="makeblastdb.exe";
#else
    #if defined(Q_OS_UNIX)
    executableFileName="makeblastdb";
    #endif
#endif
    validationArguments<<"-help";
    validMessage="makeblastdb";
    description=tr("The <i>makeblastdb</i> formats protein or"
                   " nucleotide source databases before these databases"
                   " can be searched by other BLAST+ tools.");
    versionRegExp=QRegExp("Application to create BLAST databases, version (\\d+\\.\\d+\\.\\d+\\+?)");
    toolKitName="BLAST+";
    } else if(id == ET_GPU_MAKEBLASTDB_ID) {
#ifdef Q_OS_WIN
    executableFileName="makeblastdb.exe";
#else
    #ifdef Q_OS_UNIX
    executableFileName="makeblastdb";
    #endif
#endif
    validationArguments<<"-help";
    validMessage="-sort_volumes";
    description=tr("The <i>makeblastdb</i> formats protein or"
                   " nucleotide source databases before these databases"
                   " can be searched by other BLAST+ tools.");
    versionRegExp=QRegExp("Application to create BLAST databases, version (\\d+\\.\\d+\\.\\d+\\+?)");
    toolKitName="GPU-BLAST+";
    }
}

void FormatDBSupport::sl_runWithExtFileSpecify(){
    //Check that makeblastdb and temporary folder path defined
    if (path.isEmpty()){
        QObjectScopedPointer<QMessageBox> msgBox = new QMessageBox;
        msgBox->setWindowTitle("BLAST+ "+name);
        msgBox->setText(tr("Path for BLAST+ %1 tool not selected.").arg(name));
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
    if (path.isEmpty()){
        return;
    }
    U2OpStatus2Log os(LogLevel_DETAILS);
    ExternalToolSupportSettings::checkTemporaryDir(os);
    CHECK_OP(os, );

    //Call select input file and setup settings dialog
    FormatDBSupportTaskSettings settings;
    QObjectScopedPointer<FormatDBSupportRunDialog> formatDBRunDialog = new FormatDBSupportRunDialog(name, settings, AppContext::getMainWindow()->getQMainWindow());
    formatDBRunDialog->exec();
    CHECK(!formatDBRunDialog.isNull(), );

    if (formatDBRunDialog->result() != QDialog::Accepted){
        return;
    }
    FormatDBSupportTask* formatDBSupportTask = new FormatDBSupportTask(id, settings);
    AppContext::getTaskScheduler()->registerTopLevelTask(formatDBSupportTask);
}
}//namespace
