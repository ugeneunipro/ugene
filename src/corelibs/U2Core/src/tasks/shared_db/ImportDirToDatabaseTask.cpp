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
#include <QFileInfo>

#include <U2Core/Counter.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2SafePoints.h>

#include "ImportDirToDatabaseTask.h"
#include "ImportFileToDatabaseTask.h"

namespace U2 {

ImportDirToDatabaseTask::ImportDirToDatabaseTask(const QString &srcUrl, const U2DbiRef &dstDbiRef, const QString &dstFolder, const ImportToDatabaseOptions &options) :
    Task(tr("Import folder %1 to the database").arg(QFileInfo(srcUrl).fileName()), TaskFlag_NoRun),
    srcUrl(srcUrl),
    dstDbiRef(dstDbiRef),
    dstFolder(dstFolder),
    options(options)
{
    GCOUNTER(cvar, tvar, "ImportDirToDatabaseTask");
    CHECK_EXT(QFileInfo(srcUrl).isDir(), setError(tr("It is not a folder: ") + srcUrl), );
    CHECK_EXT(dstDbiRef.isValid(), setError(tr("Invalid database reference")), );

    setMaxParallelSubtasks(1);
}

void ImportDirToDatabaseTask::prepare() {
    const QFileInfoList subentriesInfo = QDir(srcUrl).entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
    foreach (QFileInfo subentryInfo, subentriesInfo) {
        if (options.processFoldersRecursively && subentryInfo.isDir()) {
            const QString dstDirFolder = dstFolder + (options.keepFoldersStructure ? U2ObjectDbi::PATH_SEP + subentryInfo.fileName() : "");
            ImportDirToDatabaseTask* importSubdirTask = new ImportDirToDatabaseTask(subentryInfo.filePath(), dstDbiRef, dstDirFolder, options);
            importSubdirsTasks << importSubdirTask;
            addSubTask(importSubdirTask);
        } else if (subentryInfo.isFile()) {
            ImportFileToDatabaseTask* importSubfileTask = new ImportFileToDatabaseTask(subentryInfo.filePath(), dstDbiRef, dstFolder, options);
            importSubfilesTasks << importSubfileTask;
            addSubTask(importSubfileTask);
        }
    }
}

QStringList ImportDirToDatabaseTask::getImportedFiles() const {
    QStringList importedFiles;
    CHECK(isFinished(), importedFiles);

    foreach (ImportDirToDatabaseTask* importSubdirTask, importSubdirsTasks) {
        importedFiles << importSubdirTask->getImportedFiles();
    }

    foreach (ImportFileToDatabaseTask* importSubfileTask, importSubfilesTasks) {
        if (!importSubfileTask->hasError() && !importSubfileTask->isCanceled()) {
            importedFiles << importSubfileTask->getFilePath();
        }
    }

    return importedFiles;
}

StrStrMap ImportDirToDatabaseTask::getSkippedFiles() const {
    StrStrMap skippedFiles;
    CHECK(isFinished(), skippedFiles);

    foreach (ImportDirToDatabaseTask* importSubdirTask, importSubdirsTasks) {
        skippedFiles.unite(importSubdirTask->getSkippedFiles());
    }

    foreach (ImportFileToDatabaseTask* importSubfileTask, importSubfilesTasks) {
        if (importSubfileTask->isCanceled()) {
            skippedFiles.insert(importSubfileTask->getFilePath(), tr("Import was cancelled"));
        } else if (importSubfileTask->hasError()) {
            skippedFiles.insert(importSubfileTask->getFilePath(), importSubfileTask->getError());
        }
    }

    return skippedFiles;
}

}
