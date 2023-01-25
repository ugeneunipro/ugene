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

#include "MakeBlastDbAlignerSubtask.h"

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

#include "blast/BlastSupport.h"
#include "blast/MakeBlastDbTask.h"

namespace U2 {
namespace Workflow {

MakeBlastDbAlignerSubtask::MakeBlastDbAlignerSubtask(const QString& referenceUrl,
                                                     const SharedDbiDataHandler& referenceDbHandler,
                                                     DbiDataStorage* storage)
    : Task(tr("'makeblastdb' task wrapper"), TaskFlags_NR_FOSE_COSC),
      referenceUrl(referenceUrl),
      referenceDbHandler(referenceDbHandler),
      storage(storage) {
}

void MakeBlastDbAlignerSubtask::prepare() {
    MakeBlastDbSettings settings;
    settings.inputFilesPath << referenceUrl;

    QScopedPointer<U2SequenceObject> refObject(StorageUtils::getSequenceObject(storage, referenceDbHandler));
    CHECK_EXT(!refObject.isNull(), setError(L10N::nullPointerError("U2SequenceObject")), );

    CHECK_EXT(refObject->getAlphabet() != nullptr, setError(L10N::nullPointerError("DNAAlphabet")), );
    settings.isInputAmino = refObject->getAlphabet()->isAmino();
    settings.databaseTitle = refObject->getSequenceName();

    QString tempDirPath = getAcceptableTempDir();
    CHECK_EXT(!tempDirPath.isEmpty(), setError(tr("The task uses a temporary folder to process the data. It is required that the folder path doesn't have spaces. "
                                                  "Please set up an appropriate path for the \"Temporary files\" parameter on the \"Directories\" tab of the UGENE Application Settings.")), );

    QString workingDir = GUrlUtils::getSlashEndedPath(ExternalToolSupportUtils::createTmpDir(tempDirPath, "align_to_ref", stateInfo));
    settings.tempDirPath = workingDir;
    settings.outputPath = workingDir + QFileInfo(referenceUrl).completeBaseName();
    CHECK_OP(stateInfo, );

    addSubTask(new MakeBlastDbTask(settings));

    databaseNameAndPath = settings.outputPath;
}

const QString& MakeBlastDbAlignerSubtask::getResultPath() const {
    return databaseNameAndPath;
}

QString MakeBlastDbAlignerSubtask::getAcceptableTempDir() const {
    auto isTempDirAcceptable = [](const QString& tempDir) {
        CHECK(!tempDir.contains(QRegExp("\\s")), false);
        QTemporaryDir testSubDir(tempDir + "/XXXXXX");
        return testSubDir.isValid();
    };

    QString tempDirPath = AppContext::getAppSettings()->getUserAppsSettings()->getCurrentProcessTemporaryDirPath();
    if (isTempDirAcceptable(tempDirPath)) {
        return tempDirPath;
    }

    tempDirPath = QFileInfo(referenceUrl).absoluteDir().path();
    if (isTempDirAcceptable(tempDirPath)) {
        return tempDirPath;
    }

    tempDirPath = isOsWindows() ? "C:/ugene_tmp" : "/tmp/ugene_tmp";
    return QDir().mkpath(tempDirPath) && isTempDirAcceptable(tempDirPath) ? tempDirPath : "";
}

}  // namespace Workflow
}  // namespace U2
