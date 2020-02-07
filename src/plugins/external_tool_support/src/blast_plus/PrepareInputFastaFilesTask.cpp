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

#include <QFileInfo>

#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/CopyFileTask.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Formats/ConvertFileTask.h>

#include "PrepareInputFastaFilesTask.h"

namespace U2 {

PrepareInputFastaFilesTask::PrepareInputFastaFilesTask(const QStringList &inputFiles, const QString &tempDir)
    : Task(tr("Prepare input FASTA files"), TaskFlags_NR_FOSE_COSC),
      inputFiles(inputFiles),
      tempDir(tempDir)
{

}

QStringList PrepareInputFastaFilesTask::getFastaFiles() const {
    return fastaFiles;
}

QStringList PrepareInputFastaFilesTask::getTempFiles() const {
    return tempFiles;
}

void PrepareInputFastaFilesTask::prepare() {
    foreach (const QString &filePath, inputFiles) {
        const QString formatId = getBestFormatId(filePath);
        CHECK_CONTINUE(!formatId.isEmpty());

        if (formatId != BaseDocumentFormats::FASTA) {
            const QString targetFilePath = tempDir + "/" + getFixedFileName(filePath);
            DefaultConvertFileTask *convertTask = new DefaultConvertFileTask(filePath, formatId, targetFilePath, BaseDocumentFormats::FASTA, tempDir);
            addSubTask(convertTask);
        } else if (!isFilePathAcceptable(filePath)) {
            CopyFileTask *copyTask = new CopyFileTask(filePath, tempDir + "/" + getFixedFileName(filePath));
            addSubTask(copyTask);
        } else {
            fastaFiles << filePath;
        }
    }
}

QList<Task *> PrepareInputFastaFilesTask::onSubTaskFinished(Task *subTask) {
    QList<Task *> newSubTasks;
    CHECK_OP(stateInfo, newSubTasks);

    DefaultConvertFileTask *convertTask = qobject_cast<DefaultConvertFileTask *>(subTask);
    if (NULL != convertTask) {
        fastaFiles << convertTask->getResult();
        tempFiles << convertTask->getResult();
    }

    CopyFileTask *copyTask = qobject_cast<CopyFileTask *>(subTask);
    if (NULL != copyTask) {
        fastaFiles << copyTask->getTargetFilePath();
        tempFiles << copyTask->getTargetFilePath();
    }

    return newSubTasks;
}

QString PrepareInputFastaFilesTask::getBestFormatId(const QString &filePath) {
    QList<FormatDetectionResult> formats = DocumentUtils::detectFormat(filePath);
    if (formats.isEmpty()) {
        stateInfo.addWarning(tr("File '%1' was skipped. Cannot detect the file format.").arg(filePath));
        return "";
    }
    SAFE_POINT_EXT(NULL != formats.first().format, setError("An incorrect format found. An importer?"), "");
    return formats.first().format->getFormatId();
}

bool PrepareInputFastaFilesTask::isFilePathAcceptable(const QString &filePath) const {
    return !filePath.contains(" ");
}

QString PrepareInputFastaFilesTask::getFixedFileName(const QString &filePath) const {
    QFileInfo fileInfo(filePath);
    return fileInfo.fileName().replace(" ", "_");
}

}   // namespace U2
