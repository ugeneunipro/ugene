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

#ifndef _U2_ACE_IMPORTER_H_
#define _U2_ACE_IMPORTER_H_

#include <U2Core/DocumentImport.h>
#include <U2Core/DocumentProviderTask.h>

namespace U2 {

class ConvertAceToSqliteTask;
class CloneObjectTask;
class LoadDocumentTask;

class AceImporterTask : public DocumentProviderTask {
    Q_OBJECT
public:
    AceImporterTask(const GUrl& url, const QVariantMap& settings);

    void prepare() override;
    QList<Task*> onSubTaskFinished(Task* subTask) override;
    ReportResult report() override;

private:
    void initCloneObjectTasks();
    void initLoadDocumentTask();

    ConvertAceToSqliteTask* convertTask;
    QList<Task*> cloneTasks;
    LoadDocumentTask* loadDocTask;

    bool isSqliteDbTransit;

    QVariantMap settings;

    U2DbiRef localDbiRef;
    U2DbiRef dstDbiRef;
    GUrl srcUrl;
    qint64 startTime;
};

///////////////////////////////////
//// AceImporter
///////////////////////////////////

class U2FORMATS_EXPORT AceImporter : public DocumentImporter {
    Q_OBJECT
public:
    AceImporter();

    FormatCheckResult checkRawData(const QByteArray& rawData, const GUrl& url) override;
    DocumentProviderTask* createImportTask(const FormatDetectionResult& res, bool showGui, const QVariantMap& hints) override;

    static const QString ID;
    static const QString SRC_URL;
};

}  // namespace U2

#endif  // _U2_ACE_IMPORTER_H_
