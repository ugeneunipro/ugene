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

#pragma once

#include <time.h>

#include <U2Core/DocumentImport.h>
#include <U2Core/DocumentProviderTask.h>
#include <U2Core/Log.h>
#include <U2Core/PluginModel.h>
#include <U2Core/U2Dbi.h>

#include "PrepareToImportTask.h"

namespace U2 {

class CloneObjectTask;
class LoadDocumentTask;

namespace BAM {

class BAMDbiPlugin : public Plugin {
    Q_OBJECT
public:
    BAMDbiPlugin();
};

class BAMImporter : public DocumentImporter {
    Q_OBJECT
public:
    BAMImporter();

    FormatCheckResult checkRawData(const QByteArray& rawData, const GUrl& url) override;

    DocumentProviderTask* createImportTask(const FormatDetectionResult& res, bool showGui, const QVariantMap& hints) override;
};

class LoadInfoTask;
class ConvertToSQLiteTask;

class BAMImporterTask : public DocumentProviderTask {
    Q_OBJECT
public:
    BAMImporterTask(const GUrl& url, bool useGui, const QVariantMap& hints);

    void prepare() override;
    QList<Task*> onSubTaskFinished(Task* subTask) override;
    ReportResult report() override;

private:
    void initPrepareToImportTask();
    void initLoadBamInfoTask();
    void initConvertToSqliteTask();
    void initCloneObjectTasks();
    void initLoadDocumentTask();

    LoadInfoTask* loadInfoTask;
    LoadInfoTask* loadBamInfoTask;
    PrepareToImportTask* prepareToImportTask;
    ConvertToSQLiteTask* convertTask;
    QList<Task*> cloneTasks;
    LoadDocumentTask* loadDocTask;

    bool isSqliteDbTransit;
    bool useGui;
    bool sam;

    QVariantMap hints;
    U2DbiRef hintedDbiRef;
    U2DbiRef localDbiRef;
    time_t startTime;
};

}  // namespace BAM
}  // namespace U2
