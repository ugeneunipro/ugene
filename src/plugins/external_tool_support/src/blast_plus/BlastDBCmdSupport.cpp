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

#include "BlastDBCmdDialog.h"
#include "BlastDBCmdSupport.h"
#include "BlastDBCmdSupportTask.h"
#include "ExternalToolSupportSettings.h"
#include "ExternalToolSupportSettingsController.h"

namespace U2 {

const QString BlastDbCmdSupport::ET_BLASTDBCMD = "BlastDBCmd";
const QString BlastDbCmdSupport::ET_BLASTDBCMD_ID = "USUPP_BLAST_DB_CMD";

BlastDbCmdSupport::BlastDbCmdSupport(const QString& path) : ExternalTool(ET_BLASTDBCMD_ID, ET_BLASTDBCMD, path)
{
    if (AppContext::getMainWindow() != NULL) {
        icon = QIcon(":external_tool_support/images/ncbi.png");
        grayIcon = QIcon(":external_tool_support/images/ncbi_gray.png");
        warnIcon = QIcon(":external_tool_support/images/ncbi_warn.png");
    }

#ifdef Q_OS_WIN
    executableFileName="blastdbcmd.exe";
#else
    #if defined(Q_OS_UNIX)
    executableFileName="blastdbcmd";
    #endif
#endif
    validationArguments<<"--help";
    validMessage="blastdbcmd";
    description=tr("The <i>BlastDBCmd</i> fetches protein or"
        " nucleotide sequences from BLAST+ database based on a query.");

    versionRegExp=QRegExp("BLAST database client, version (\\d+\\.\\d+\\.\\d+\\+?)");
    toolKitName="BLAST+";

}

void BlastDbCmdSupport::sl_runWithExtFileSpecify(){
    //Check that BlastDBCmd folder path defined
    if (path.isEmpty()){
        QObjectScopedPointer<QMessageBox> msgBox = new QMessageBox;
        msgBox->setWindowTitle("BLAST+ " +name);
        msgBox->setText(tr("Path for BLAST+ %1 tool not selected.").arg(ET_BLASTDBCMD));
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
    BlastDBCmdSupportTaskSettings settings;
    QObjectScopedPointer<BlastDBCmdDialog> blastDBCmdDialog = new BlastDBCmdDialog(settings, AppContext::getMainWindow()->getQMainWindow());
    blastDBCmdDialog->exec();
    CHECK(!blastDBCmdDialog.isNull(), );

    if(blastDBCmdDialog->result() != QDialog::Accepted){
        return;
    }

    BlastDBCmdSupportTask* blastDBCmdSupportTask =new BlastDBCmdSupportTask(settings);
    AppContext::getTaskScheduler()->registerTopLevelTask(blastDBCmdSupportTask);
}
}//namespace
