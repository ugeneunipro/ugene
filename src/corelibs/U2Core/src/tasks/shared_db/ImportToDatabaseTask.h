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

#ifndef _U2_IMPORT_TO_DATABASE_TASK_H_
#define _U2_IMPORT_TO_DATABASE_TASK_H_

#include <U2Core/MultiTask.h>

namespace U2 {

class ImportDirToDatabaseTask;
class ImportDocumentToDatabaseTask;
class ImportFileToDatabaseTask;
class ImportObjectToDatabaseTask;

class U2CORE_EXPORT ImportToDatabaseTask : public MultiTask {
    Q_OBJECT
public:
    ImportToDatabaseTask(QList<Task*> tasks, int maxParallelSubtasks = 1);

    void run();
    QString generateReport() const;
    ReportResult report();

private:
    void sortSubtasks() const;
    QString createReport() const;

    QString sayAboutImportedFilesAndDirs() const;
    QString sayAboutImportedDirs() const;
    QString sayAboutImportedFiles() const;
    QString sayAboutImportedProjectItems() const;
    QString sayAboutImportedDocuments() const;
    QString sayAboutImportedObjects() const;

    QString sayAboutSkippedFilesAndDirs() const;
    QString sayAboutSkippedDirs() const;
    QString sayAboutSkippedFiles() const;
    QString sayAboutSkippedProjectItems() const;
    QString sayAboutSkippedDocuments() const;
    QString sayAboutSkippedObjects() const;

    QString reportString;
    qint64 startTime;

    mutable QList<ImportDirToDatabaseTask*> dirSubtasks;
    mutable QList<ImportDocumentToDatabaseTask*> documentSubtasks;
    mutable QList<ImportFileToDatabaseTask*> fileSubtasks;
    mutable QList<ImportObjectToDatabaseTask*> objectSubtasks;

    static const int MAX_SHOWN_ITEMS_IN_REPORT = 100;
};

}   // namespace U2

#endif // _U2_IMPORT_TO_DATABASE_TASK_H_
