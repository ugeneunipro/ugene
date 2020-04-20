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

#include "SnpEffSupport.h"
#include "SnpEffDatabaseListModel.h"
#include "SnpEffDatabaseListTask.h"
#include "java/JavaSupport.h"

#include <U2Core/AppContext.h>
#include <U2Core/AppResources.h>
#include <U2Core/AppSettings.h>
#include <U2Core/DataPathRegistry.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/ScriptingToolRegistry.h>
#include <U2Core/Settings.h>

#include <U2Formats/ConvertFileTask.h>

namespace U2 {

SnpEffDatabaseListModel* SnpEffSupport::databaseModel = new SnpEffDatabaseListModel();
const QString SnpEffSupport::ET_SNPEFF = "SnpEff";
const QString SnpEffSupport::ET_SNPEFF_ID = "USUPP_SNPEFF";

SnpEffSupport::SnpEffSupport(const QString& id, const QString& name, const QString& path) : ExternalTool(id, name, path)
{
    if (AppContext::getMainWindow()) {
        icon = QIcon(":external_tool_support/images/cmdline.png");
        grayIcon = QIcon(":external_tool_support/images/cmdline_gray.png");
        warnIcon = QIcon(":external_tool_support/images/cmdline_warn.png");
    }

    executableFileName="snpEff.jar";

    validMessage = "Usage: snpEff \\[command\\] \\[options\\] \\[files\\]";
    description = tr("<i>SnpEff</i>: Genetic variant annotation and effect prediction toolbox.");

    versionRegExp = QRegExp("version SnpEff (\\d+.\\d+[a-zA-Z]?)");
    validationArguments << "-h";
    toolKitName = "SnpEff";

    toolRunnerProgram = JavaSupport::ET_JAVA_ID;
    dependencies << JavaSupport::ET_JAVA_ID;

    connect(this, SIGNAL(si_toolValidationStatusChanged(bool)), SLOT(sl_validationStatusChanged(bool)));
}

QStringList SnpEffSupport::getToolRunnerAdditionalOptions() const {
    QStringList result;
    AppResourcePool* s = AppContext::getAppSettings()->getAppResourcePool();
    CHECK(s != NULL, result);
    //java VM can't allocate whole free memory, Xmx size should be lesser than free memory
    int memSize = s->getMaxMemorySizeInMB();
#if (defined(Q_OS_WIN) || defined(Q_OS_LINUX))
    ExternalToolRegistry* etRegistry = AppContext::getExternalToolRegistry();
    JavaSupport* java =  qobject_cast<JavaSupport*>(etRegistry->getById(JavaSupport::ET_JAVA_ID));
    CHECK(java != NULL, result);
    if (java->getArchitecture() == JavaSupport::x32) {
        memSize = memSize > 1212 ? 1212 : memSize;
    }
#endif // windows or linux
    result << "-Xmx" + QString::number(memSize > 150 ? memSize - 150 : memSize) + "M";
    return result;
}

void SnpEffSupport::sl_validationStatusChanged(bool isValid) {
    if (isValid) {
        SnpEffDatabaseListTask* task = new SnpEffDatabaseListTask();
        connect(task, SIGNAL(si_stateChanged()), SLOT(sl_databaseListIsReady()));
        AppContext::getTaskScheduler()->registerTopLevelTask(task);
    }
}

void SnpEffSupport::sl_databaseListIsReady() {
    SnpEffDatabaseListTask* task = dynamic_cast<SnpEffDatabaseListTask*>(sender());
    SAFE_POINT(task != NULL, "SnpEffDatabaseListTask is NULL: wrong sender",);
    if (task->isCanceled() || task->hasError() || !task->isFinished()) {
        return;
    }
    QString dbListFilePath = task->getDbListFilePath();
    SAFE_POINT(!dbListFilePath.isEmpty(), tr("Failed to get SnpEff database list"), );

    SnpEffSupport::databaseModel->getData(dbListFilePath);
}

}//namespace

