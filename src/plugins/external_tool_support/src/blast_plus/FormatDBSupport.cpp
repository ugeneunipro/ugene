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

#include "FormatDBSupport.h"

#include <QMainWindow>
#include <QMessageBox>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UserApplicationsSettings.h>

#include <U2View/MSAEditor.h>

#include "ExternalToolSupportSettings.h"
#include "ExternalToolSupportSettingsController.h"
#include "FormatDBSupportRunDialog.h"
#include "FormatDBSupportTask.h"

namespace U2 {

const QString FormatDBSupport::ET_MAKEBLASTDB_ID = "USUPP_MAKE_BLAST_DB";
const QString FormatDBSupport::FORMATDB_TMP_DIR = "FormatDB";

FormatDBSupport::FormatDBSupport(const QString &path)
    : ExternalTool(ET_MAKEBLASTDB_ID, "blast", "MakeBLASTDB", path) {
    if (AppContext::getMainWindow() != nullptr) {
        icon = QIcon(":external_tool_support/images/ncbi.png");
        grayIcon = QIcon(":external_tool_support/images/ncbi_gray.png");
        warnIcon = QIcon(":external_tool_support/images/ncbi_warn.png");
    }
#ifdef Q_OS_WIN
    executableFileName = "makeblastdb.exe";
#else
#    if defined(Q_OS_UNIX)
    executableFileName = "makeblastdb";
#    endif
#endif
    validationArguments << "-help";
    validMessage = "makeblastdb";
    description = tr("The <i>makeblastdb</i> formats protein or"
                     " nucleotide source databases before these databases"
                     " can be searched by other BLAST+ tools.");
    versionRegExp = QRegExp("Application to create BLAST databases, version (\\d+\\.\\d+\\.\\d+\\+?)");
    toolKitName = "BLAST+";
}

void FormatDBSupport::sl_runWithExtFileSpecify() {
    //Check that makeblastdb and temporary folder path defined
    if (path.isEmpty()) {
        QObjectScopedPointer<QMessageBox> msgBox = new QMessageBox;
        msgBox->setWindowTitle("BLAST+ " + name);
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

    //Call select input file and setup settings dialog
    FormatDBSupportTaskSettings settings;
    QObjectScopedPointer<FormatDBSupportRunDialog> formatDBRunDialog = new FormatDBSupportRunDialog(name, settings, AppContext::getMainWindow()->getQMainWindow());
    formatDBRunDialog->exec();
    CHECK(!formatDBRunDialog.isNull(), );

    if (formatDBRunDialog->result() != QDialog::Accepted) {
        return;
    }
    AppContext::getTaskScheduler()->registerTopLevelTask(new FormatDBSupportTask(settings));
}
}    // namespace U2
