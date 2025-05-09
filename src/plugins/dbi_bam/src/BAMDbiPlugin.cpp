/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include "BAMDbiPlugin.h"

#include <QAction>
#include <QDir>
#include <QFile>
#include <QMainWindow>
#include <QMenu>
#include <QMessageBox>
#include <QTemporaryFile>

#include <U2Core/AddDocumentTask.h>
#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/AssemblyObject.h>
#include <U2Core/CloneObjectTask.h>
#include <U2Core/CollectionUtils.h>
#include <U2Core/DbiDocumentFormat.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/FileAndDirectoryUtils.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/LoadDocumentTask.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/TaskSignalMapper.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U2DbiRegistry.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UserApplicationsSettings.h>

#include <U2Formats/SAMFormat.h>

#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/MainWindow.h>
#include <U2Gui/OpenViewTask.h>

#include "BAMFormat.h"
#include "ConvertToSQLiteDialog.h"
#include "ConvertToSQLiteTask.h"
#include "Dbi.h"
#include "Exception.h"
#include "LoadBamInfoTask.h"
#include "SamtoolsBasedDbi.h"

namespace U2 {
namespace BAM {

extern "C" Q_DECL_EXPORT Plugin* U2_PLUGIN_INIT_FUNC() {
    auto plug = new BAMDbiPlugin();
    return plug;
}

BAMDbiPlugin::BAMDbiPlugin()
    : Plugin(tr("BAM format support"), tr("Interface for indexed read-only access to BAM files")) {
    DocumentFormat* bamDbi = new BAMFormat();
    AppContext::getDocumentFormatRegistry()->registerFormat(bamDbi);
    AppContext::getDbiRegistry()->registerDbiFactory(new SamtoolsBasedDbiFactory());

    AppContext::getDocumentFormatRegistry()->getImportSupport()->addDocumentImporter(new BAMImporter());
}

//////////////////////////////////////////////////////////////////////////
// BAM importer
BAMImporter::BAMImporter()
    : DocumentImporter("bam-importer", tr("BAM/SAM file import")) {
    // prepare sorted extensions list
    QSet<QString> extsSet;
    BAMFormatUtils bam;
    SAMFormat sam;
    extsSet.unite(toSet(bam.getSupportedDocumentFileExtensions()).unite(toSet(sam.getSupportedDocumentFileExtensions())));
    QStringList exts = toList(extsSet);
    std::sort(exts.begin(), exts.end());

    formatIds << BaseDocumentFormats::BAM << BaseDocumentFormats::SAM;
    extensions << exts;
    importerDescription = tr("BAM files importer is used to convert conventional BAM and SAM files into UGENE database format. Having BAM or SAM file converted into UGENE DB format you get an fast and efficient interface to your data with an option to change the content");
    supportedObjectTypes << GObjectTypes::ASSEMBLY;
}

#define SAM_HINT "bam-importer-sam-hint"

FormatCheckResult BAMImporter::checkRawData(const QByteArray& rawData, const GUrl& url) {
    BAMFormatUtils bamFormatUtils;
    FormatCheckResult bamScore = bamFormatUtils.checkRawData(rawData, url);

    SAMFormat samFormat;
    FormatCheckResult samScore = samFormat.checkRawData(rawData, url);

    if (bamScore.score > samScore.score) {
        return bamScore;
    }
    samScore.properties[SAM_HINT] = true;
    return samScore;
}

DocumentProviderTask* BAMImporter::createImportTask(const FormatDetectionResult& res, bool showGui, const QVariantMap& hints) {
    bool sam = res.rawDataCheckResult.properties[SAM_HINT].toBool();
    QVariantMap fullHints(hints);
    fullHints[SAM_HINT] = sam;
    return new BAMImporterTask(res.url, showGui, fullHints);
}

BAMImporterTask::BAMImporterTask(const GUrl& url, bool _useGui, const QVariantMap& hints)
    : DocumentProviderTask(tr("BAM/SAM file import: %1").arg(url.fileName()), TaskFlags_NR_FOSCOE),
      loadInfoTask(nullptr),
      loadBamInfoTask(nullptr),
      prepareToImportTask(nullptr),
      convertTask(nullptr),
      loadDocTask(nullptr),
      isSqliteDbTransit(false),
      useGui(_useGui),
      sam(hints.value(SAM_HINT, false).toBool()),
      hints(hints),
      hintedDbiRef(hints.value(DocumentFormat::DBI_REF_HINT).value<U2DbiRef>()),
      startTime(0) {
    documentDescription = url.fileName();
    loadInfoTask = new LoadInfoTask(url, sam);
    addSubTask(loadInfoTask);
}

void BAMImporterTask::prepare() {
    startTime = time(0);
}

namespace {
QString getDirUrl(const GUrl& fileUrl) {
    return QFileInfo(fileUrl.getURLString()).dir().absolutePath();
}
}  // namespace

QList<Task*> BAMImporterTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> res;

    if (subTask->hasError()) {
        propagateSubtaskError();
        return res;
    }

    if (loadInfoTask == subTask) {
        initPrepareToImportTask();
        CHECK(prepareToImportTask != nullptr, res);
        res << prepareToImportTask;
    }

    else if (prepareToImportTask == subTask && prepareToImportTask->isNewURL()) {
        // This happens if the original file wasn't sorted
        // and PrepareToImportTask created a new sorted file
        initLoadBamInfoTask();
        CHECK(loadBamInfoTask != nullptr, res);
        res << loadBamInfoTask;
    }

    else if (loadBamInfoTask == subTask || prepareToImportTask == subTask) {
        initConvertToSqliteTask();
        CHECK(convertTask != nullptr, res);
        res << convertTask;
    }

    else if (isSqliteDbTransit && convertTask == subTask) {
        initCloneObjectTasks();
        CHECK(!cloneTasks.isEmpty(), res);
        res << cloneTasks;
    }

    else if (!isSqliteDbTransit && convertTask == subTask) {
        initLoadDocumentTask();
        CHECK(loadDocTask != nullptr, res);
        res << loadDocTask;
    }

    else if ((isSqliteDbTransit && cloneTasks.contains(subTask))) {
        cloneTasks.removeOne(subTask);
        auto cloneTask = qobject_cast<CloneObjectTask*>(subTask);
        SAFE_POINT_EXT(cloneTask != nullptr, setError("Unexpected task type: CloneObjectTask expected"), res);
        delete cloneTask->getSourceObject();

        if (cloneTasks.isEmpty()) {
            initLoadDocumentTask();
            CHECK(loadDocTask != nullptr, res);
            res << loadDocTask;
        }
    }

    else if (subTask == loadDocTask) {
        resultDocument = loadDocTask->takeDocument();
    }

    return res;
}

Task::ReportResult BAMImporterTask::report() {
    time_t totalTime = time(0) - startTime;
    taskLog.info(QString("BAMImporter task total time is %1 sec").arg(totalTime));
    return ReportResult_Finished;
}

void BAMImporterTask::initPrepareToImportTask() {
    GUrl srcUrl = loadInfoTask->getSourceUrl();

    isSqliteDbTransit = hintedDbiRef.isValid() && SQLITE_DBI_ID != hintedDbiRef.dbiFactoryId;
    if (!isSqliteDbTransit) {
        localDbiRef = U2DbiRef(SQLITE_DBI_ID, srcUrl.dirPath() + QDir::separator() + srcUrl.fileName() + ".ugenedb");
    } else {
        const QString tmpDir = AppContext::getAppSettings()->getUserAppsSettings()->getCurrentProcessTemporaryDirPath("assembly_conversion") + QDir::separator();
        QDir().mkpath(tmpDir);

        const QString pattern = tmpDir + "XXXXXX.ugenedb";
        auto tempLocalDb = new QTemporaryFile(pattern, this);

        tempLocalDb->open();
        const QString filePath = tempLocalDb->fileName();
        tempLocalDb->close();

        SAFE_POINT_EXT(QFile::exists(filePath), setError("Can't create a temporary database"), );

        localDbiRef = U2DbiRef(SQLITE_DBI_ID, filePath);
    }

    QString refUrl;
    bool convert = true;
    if (useGui) {
        QObjectScopedPointer<ConvertToSQLiteDialog> convertDialog = new ConvertToSQLiteDialog(loadInfoTask->getSourceUrl(), loadInfoTask->getInfo(), loadInfoTask->isSam());
        convertDialog->hideAddToProjectOption();
        const int rc = convertDialog->exec();
        CHECK_EXT(!convertDialog.isNull(), setError("NULL dialog"), );

        if (rc == QDialog::Accepted) {
            localDbiRef = U2DbiRef(SQLITE_DBI_ID, convertDialog->getDestinationUrl().getURLString());
            refUrl = convertDialog->getReferenceUrl();

        } else {
            convert = false;
            stateInfo.setCanceled(true);
        }
        if (!QFile::exists(srcUrl.getURLString())) {
            setError(tr("File %1 does not exists.").arg(srcUrl.getURLString()));
            QMessageBox::critical(AppContext::getMainWindow()->getQMainWindow(), L10N::errorTitle(), stateInfo.getError());
            return;
        }
    } else if (loadInfoTask->isSam() && loadInfoTask->getInfo().getHeader().getReferences().isEmpty()) {
        convert = false;
        setError(tr("SAM cannot be converted to BAM: neither reference nor header in SAM file is present"));
    }

    if (convert) {
        QString dirUrl = getDirUrl(loadInfoTask->getSourceUrl());
        if (!FileAndDirectoryUtils::isDirectoryWritable(dirUrl)) {
            const GUrl url(U2DbiUtils::ref2Url(localDbiRef));
            if (url.isLocalFile()) {
                dirUrl = getDirUrl(url);
            } else {
                dirUrl = getDirUrl(AppContext::getAppSettings()->getUserAppsSettings()->getUserTemporaryDirPath());
            }
        }
        prepareToImportTask = new PrepareToImportTask(loadInfoTask->getSourceUrl(), loadInfoTask->isSam(), refUrl, dirUrl);
    }
}

void BAMImporterTask::initLoadBamInfoTask() {
    bool samFormat = false;
    loadBamInfoTask = new LoadInfoTask(prepareToImportTask->getSourceUrl(), samFormat);
}

void BAMImporterTask::initConvertToSqliteTask() {
    bool samFormat = false;
    GUrl sourceURL;
    BAMInfo bamInfo;
    if (prepareToImportTask->isNewURL()) {
        sourceURL = loadBamInfoTask->getSourceUrl();
        bamInfo = loadBamInfoTask->getInfo();
        if (useGui) {
            // Using @ConvertToSQLiteDialog we could set which scaffolds we should import
            // and should we import unmapped reads as a separate scaffold or not.
            // This info is stored to @BAMInfo of @LoadInfoTask loadInfoTask.
            // If the original file is not sorted, it will be sorted and saved nearby (to "ORIGINAL_NAME_sorted.bam").
            // If the new sorted file was created, it will be processed by a new one @LoadInfoTask loadBamInfoTask.
            // As far as this task will create its own new @BAMInfo, we have to store inforation about what scaffolds we should improt.
            auto& bamInfoWithDialogSettings = loadInfoTask->getInfo();
            bamInfo.setUnmappedSelected(bamInfoWithDialogSettings.isUnmappedSelected());
            auto& currentBamSelected = bamInfo.getSelected();
            const auto& selectedWithDialog = bamInfoWithDialogSettings.getSelected();
            if (!selectedWithDialog.isEmpty()) {
                // Empty list of selected scaffolds in the original file means,
                // that this file doesn't have header (it is possible, if the original file has SAM format)
                // Overwise, number of scaffolds in orginal and in sorted files should be equal
                SAFE_POINT_EXT(currentBamSelected.size() == selectedWithDialog.size(),
                               setError("Original and sorted files have different number of scaffolds"), );

                for (int i = 0; i < currentBamSelected.size(); i++) {
                    currentBamSelected[i] = selectedWithDialog[i];
                }
            }
        }
    } else {
        sourceURL = prepareToImportTask->getSourceUrl();
        bamInfo = loadInfoTask->getInfo();
    }
    convertTask = new ConvertToSQLiteTask(sourceURL, localDbiRef, bamInfo, samFormat);
}

void BAMImporterTask::initCloneObjectTasks() {
    QList<U2Assembly> assemblies = convertTask->getAssemblies();
    foreach (const U2Assembly& assembly, assemblies) {
        auto object = new AssemblyObject(assembly.visualName, U2EntityRef(localDbiRef, assembly.id));
        cloneTasks << new CloneObjectTask(object, hintedDbiRef, hints.value(DocumentFormat::DBI_FOLDER_HINT, U2ObjectDbi::ROOT_FOLDER).toString());
    }
}

void BAMImporterTask::initLoadDocumentTask() {
    if (hints.value(BAMImporter::LOAD_RESULT_DOCUMENT, true).toBool()) {
        loadDocTask = LoadDocumentTask::getDefaultLoadDocTask(convertTask->getDestinationUrl());
        if (loadDocTask == nullptr) {
            setError(tr("Failed to get load task for : %1").arg(convertTask->getDestinationUrl().getURLString()));
        }
    }
}

}  // namespace BAM
}  // namespace U2
