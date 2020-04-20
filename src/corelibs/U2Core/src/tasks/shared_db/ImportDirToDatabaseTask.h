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

#ifndef _U2_IMPORT_DIR_TO_DATABASE_TASK_H_
#define _U2_IMPORT_DIR_TO_DATABASE_TASK_H_

#include <U2Core/ImportToDatabaseOptions.h>
#include <U2Core/StrPackUtils.h>
#include <U2Core/Task.h>
#include <U2Core/U2Type.h>

class QFileInfo;

namespace U2 {

class ImportFileToDatabaseTask;

class U2CORE_EXPORT ImportDirToDatabaseTask : public Task {
    Q_OBJECT
public:
    ImportDirToDatabaseTask(const QString &srcUrl, const U2DbiRef& dstDbiRef, const QString &dstFolder, const ImportToDatabaseOptions& options);

    void prepare();

    QStringList getImportedFiles() const;
    StrStrMap getSkippedFiles() const;

private:
    QString srcUrl;
    U2DbiRef dstDbiRef;
    QString dstFolder;
    ImportToDatabaseOptions options;

    QList<ImportDirToDatabaseTask*> importSubdirsTasks;
    QList<ImportFileToDatabaseTask*> importSubfilesTasks;
};

}   // namespace U2

#endif // _U2_IMPORT_DIR_TO_DATABASE_TASK_H_
