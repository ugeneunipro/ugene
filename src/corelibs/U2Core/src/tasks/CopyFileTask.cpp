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

#include <QFile>

#include <U2Core/GUrlUtils.h>
#include <U2Core/U2SafePoints.h>

#include "CopyFileTask.h"

namespace U2 {

CopyFileTask::CopyFileTask(const QString &sourceFilePath, const QString &targetFilePath)
    : Task(tr("File '%1' copy task").arg(sourceFilePath), TaskFlag_None),
      sourceFilePath(sourceFilePath),
      targetFilePath(GUrlUtils::rollFileName(targetFilePath, "_"))
{

}

QString CopyFileTask::getSourceFilePath() const {
    return sourceFilePath;
}

QString CopyFileTask::getTargetFilePath() const {
    return targetFilePath;
}

void CopyFileTask::run() {
    QFile sourceFile(sourceFilePath);
    CHECK_EXT(sourceFile.exists(), setError(tr("File '%1' doesn't exist").arg(sourceFilePath)), );
    const bool succeeded = sourceFile.copy(targetFilePath);
    CHECK_EXT(succeeded, setError(tr("File copy from '%1' to '%2' failed").arg(sourceFilePath).arg(targetFilePath)), );
}

}   // namespace U2
