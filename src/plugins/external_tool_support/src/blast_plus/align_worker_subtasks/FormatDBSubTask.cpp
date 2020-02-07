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

#include <QDir>
#include <QTemporaryDir>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/ExternalToolRunTask.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UserApplicationsSettings.h>

#include <U2Lang/DbiDataStorage.h>

#include "FormatDBSubTask.h"
#include "blast_plus/FormatDBSupport.h"
#include "blast_plus/FormatDBSupportTask.h"

namespace U2 {
namespace Workflow {

FormatDBSubTask::FormatDBSubTask(const QString &referenceUrl,
                                 const SharedDbiDataHandler &referenceDbHandler,
                                 DbiDataStorage *storage)
    : Task(tr("Format DB task wrapper"), TaskFlags_NR_FOSE_COSC),
      referenceUrl(referenceUrl),
      referenceDbHandler(referenceDbHandler),
      storage(storage)
{

}

void FormatDBSubTask::prepare() {
    FormatDBSupportTaskSettings settings;
    settings.inputFilesPath << referenceUrl;

    QScopedPointer<U2SequenceObject> refObject(StorageUtils::getSequenceObject(storage, referenceDbHandler));
    CHECK_EXT(!refObject.isNull(), setError(L10N::nullPointerError("U2SequenceObject")), );

    CHECK_EXT(refObject->getAlphabet() != NULL, setError(L10N::nullPointerError("DNAAlphabet")), );
    settings.isInputAmino = refObject->getAlphabet()->isAmino();
    settings.databaseTitle = refObject->getSequenceName();

    const QString tempDirPath = getAcceptableTempDir();
    CHECK_EXT(!tempDirPath.isEmpty(), setError(tr("The task uses a temporary folder to process the data. It is required that the folder path doesn't have spaces. "
                                                  "Please set up an appropriate path for the \"Temporary files\" parameter on the \"Directories\" tab of the UGENE Application Settings.")), );

    const QString workingDir = GUrlUtils::getSlashEndedPath(ExternalToolSupportUtils::createTmpDir(tempDirPath, "align_to_ref", stateInfo));
    settings.tempDirPath = workingDir;
    settings.outputPath = workingDir + QFileInfo(referenceUrl).completeBaseName();
    CHECK_OP(stateInfo, );

    FormatDBSupportTask* formatTask = new FormatDBSupportTask(FormatDBSupport::ET_MAKEBLASTDB_ID, settings);
    addSubTask(formatTask);

    databaseNameAndPath = settings.outputPath;
}

const QString& FormatDBSubTask::getResultPath() const {
    return databaseNameAndPath;
}

namespace {

bool isTempDirAcceptable(const QString &tempDir) {
    CHECK(!tempDir.contains(QRegExp("\\s")), false);
    QTemporaryDir testSubDir(tempDir + "/XXXXXX");
    CHECK(testSubDir.isValid(), false);
    return true;
}

}

QString FormatDBSubTask::getAcceptableTempDir() const {
    QString tempDirPath = AppContext::getAppSettings()->getUserAppsSettings()->getCurrentProcessTemporaryDirPath();
    if (isTempDirAcceptable(tempDirPath)) {
        return tempDirPath;
    }

    tempDirPath = QFileInfo(referenceUrl).absoluteDir().path();
    if (isTempDirAcceptable(tempDirPath)) {
        return tempDirPath;
    }

#if defined (Q_OS_WIN)
    tempDirPath = "C:/ugene_tmp";
#elif defined (Q_OS_UNIX)
    tempDirPath = "/tmp/ugene_tmp";
#else
    return QString();
#endif

    const bool created = QDir().mkpath(tempDirPath);
    if (created && isTempDirAcceptable(tempDirPath)) {
        return tempDirPath;
    }

    return QString();
}

} // namespace Workflow
} // namespace U2
