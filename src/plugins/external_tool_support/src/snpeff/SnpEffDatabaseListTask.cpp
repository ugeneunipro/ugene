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

#include "SnpEffDatabaseListTask.h"

#include <QDir>
#include <QFileInfo>

#include <U2Core/AppContext.h>
#include <U2Core/Counter.h>
#include <U2Core/Settings.h>

#include "SnpEffSupport.h"
#include "SnpEffTask.h"

namespace U2 {

#define SNPEFF_DATABASE_LIST_SETTINGS +"snpEffDatabaseList_"

SnpEffDatabaseListTask::SnpEffDatabaseListTask()
    : ExternalToolSupportTask(tr("SnpEff Database List task"), TaskFlags_FOSE_COSC) {
    snpEffVersion = AppContext::getExternalToolRegistry()->getById(SnpEffSupport::ET_SNPEFF_ID)->getVersion();
    dbListFilePath = qgetenv("UGENE_SNPEFF_DB_LIST");
    if (dbListFilePath.isEmpty()) {
        dbListFilePath = AppContext::getSettings()->getValue(SNPEFF_DATABASE_LIST_SETTINGS + snpEffVersion, QVariant(""), true).toString();
    }
    GCOUNTER(cvar, "ExternalTool_SnpEff");
}

void SnpEffDatabaseListTask::prepare() {
    if (!dbListFilePath.isEmpty() && QFileInfo(dbListFilePath).size() != 0) {
        return;
    }
    dbListFilePath = qgetenv("UGENE_SNPEFF_DB_LIST");
    if (!dbListFilePath.isEmpty()) {
        QDir dbListDir = QFileInfo(dbListFilePath).dir();
        if (!dbListDir.exists()) {
            bool isCreated = dbListDir.mkpath(dbListDir.absolutePath());
            if (!isCreated) {
                dbListFilePath = "";
            }
        }
    }
    if (dbListFilePath.isEmpty()) {
        QString iniFile = AppContext::getSettings()->fileName();
        dbListFilePath = QFileInfo(iniFile).absoluteDir().absolutePath();
        dbListFilePath += QString(QDir::separator()) + "SnpEff_DB_" + snpEffVersion + ".list";
    }

    QStringList args("databases");
    ExternalToolRunTask* etTask = new ExternalToolRunTask(SnpEffSupport::ET_SNPEFF_ID, args, new SnpEffParser(), "", QStringList(), true);
    setListenerForTask(etTask);
    etTask->setStandartOutputFile(dbListFilePath);
    addSubTask(etTask);
}

void SnpEffDatabaseListTask::run() {
    if (stateInfo.isCoR()) {
        return;
    }
    AppContext::getSettings()->setValue(SNPEFF_DATABASE_LIST_SETTINGS + snpEffVersion, dbListFilePath, true);
}

}  // namespace U2
