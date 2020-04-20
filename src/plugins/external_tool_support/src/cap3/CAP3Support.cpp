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
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UserApplicationsSettings.h>

#include <U2Gui/AppSettingsGUI.h>
#include <U2Gui/DialogUtils.h>
#include <U2Gui/GUIUtils.h>
#include <U2Gui/MainWindow.h>
#include <U2Core/QObjectScopedPointer.h>

#include <U2View/MSAEditor.h>
#include <U2View/MaEditorFactory.h>

#include "CAP3Support.h"
#include "CAP3SupportDialog.h"
#include "CAP3SupportTask.h"
#include "ExternalToolSupportSettings.h"
#include "ExternalToolSupportSettingsController.h"

namespace U2 {

const QString CAP3Support::ET_CAP3 = "CAP3";
const QString CAP3Support::ET_CAP3_ID = "USUPP_CAP3";
const QString CAP3Support::CAP3_TMP_DIR = "cap3";

CAP3Support::CAP3Support(const QString& id, const QString& name, const QString& path) : ExternalTool(id, name, path)
{
    if (AppContext::getMainWindow()) {
        viewCtx = NULL; //new CAP3SupportContext(this);
        icon = QIcon(":external_tool_support/images/cmdline.png");
        grayIcon = QIcon(":external_tool_support/images/cmdline_gray.png");
        warnIcon = QIcon(":external_tool_support/images/cmdline_warn.png");
    }
#ifdef Q_OS_WIN
    executableFileName="cap3.exe";
#else
    #if defined(Q_OS_UNIX)
    executableFileName="cap3";
    #endif
#endif
    validMessage="cap3 File_of_reads \\[options\\]";
    description=tr("<i>CAP3</i> is a contig assembly program. \
                   <br>It allows one to assembly long DNA reads (up to 1000 bp). \
                   <br>Binaries can be downloaded from http://seq.cs.iastate.edu/cap3.html");
    description+=tr("<br><br> Huang, X. and Madan, A.  (1999)");
    description+=tr("<br>CAP3: A DNA Sequence Assembly Program,");
    description+=tr("<br>Genome Research, 9: 868-877.");
    versionRegExp=QRegExp("VersionDate: (\\d+\\/\\d+\\/\\d+)");
    toolKitName="CAP3";
}

void CAP3Support::sl_runWithExtFileSpecify(){
    //Check that CAP3 and temporary folder path defined
    if (path.isEmpty()){
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
    CHECK_OP(os,);

    //Call select input file and setup settings dialog
    CAP3SupportTaskSettings settings;
    QObjectScopedPointer<CAP3SupportDialog> cap3Dialog = new CAP3SupportDialog(settings, QApplication::activeWindow());
    cap3Dialog->exec();
    CHECK(!cap3Dialog.isNull(), );

    if(cap3Dialog->result() != QDialog::Accepted){
        return;
    }

    assert(!settings.inputFiles.isEmpty());

    RunCap3AndOpenResultTask* task = new RunCap3AndOpenResultTask(settings);
    AppContext::getTaskScheduler()->registerTopLevelTask(task);

}



}//namespace
