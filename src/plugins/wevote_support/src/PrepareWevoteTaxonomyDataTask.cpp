/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2018 UniPro <ugene@unipro.ru>
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

#include <QTextStream>

#include <U2Core/AppContext.h>
#include <U2Core/AppFileStorage.h>
#include <U2Core/DataPathRegistry.h>
#include <U2Core/FileStorageUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/U2SafePoints.h>

#include "PrepareWevoteTaxonomyDataTask.h"
#include "../../ngs_reads_classification/src/NgsReadsClassificationPlugin.h"

namespace U2 {

const QString PrepareWevoteTaxonomyDataTask::WEVOTE_DIR = "wevote_taxonomy";
const QString PrepareWevoteTaxonomyDataTask::WEVOTE_NODES = "nodes_wevote.dmp";
const QString PrepareWevoteTaxonomyDataTask::WEVOTE_NAMES = "names_wevote.dmp";
const qint64 PrepareWevoteTaxonomyDataTask::BUFFER_SIZE = 4 * 1024 * 1024;
const QString PrepareWevoteTaxonomyDataTask::SCIENTIFIC_NAME = "scientific_name";

PrepareWevoteTaxonomyDataTask::PrepareWevoteTaxonomyDataTask(FileStorage::WorkflowProcess &_workflowProcess)
    : Task(tr("Prepare taxonomy data for WEVOTE"), TaskFlag_None),
      workflowProcess(_workflowProcess),
      removeDestinationFiles(false)
{
    U2DataPathRegistry *dataPathRegistry = AppContext::getDataPathRegistry();
    SAFE_POINT_EXT(NULL != dataPathRegistry, setError("U2DataPathRegistry is NULL"), );

    U2DataPath *taxonomyDataPath = dataPathRegistry->getDataPathByName(NgsReadsClassificationPlugin::TAXONOMY_DATA_ID);
    SAFE_POINT_EXT(NULL != taxonomyDataPath, setError("Taxonomy data path is not registered"), );
    CHECK_EXT(taxonomyDataPath->isValid(), setError(tr("Taxonomy data are missed")), );

    taxonomyNamesUrl = taxonomyDataPath->getPathByName(NgsReadsClassificationPlugin::TAXON_NAMES_ITEM_ID);
    taxonomyNodesUrl = taxonomyDataPath->getPathByName(NgsReadsClassificationPlugin::TAXON_NODES_ITEM_ID);

    CHECK_EXT(!taxonomyNamesUrl.isEmpty(),
              setError(tr("Taxonomy file '%1' is not found.").arg(NgsReadsClassificationPlugin::TAXON_NODES_ITEM_ID)), );
    CHECK_EXT(!taxonomyNodesUrl.isEmpty(),
              setError(tr("Taxonomy file '%1' is not found.").arg(NgsReadsClassificationPlugin::TAXON_NAMES_ITEM_ID)), );

    wevoteTaxonomyDir = AppContext::getAppFileStorage()->getStorageDir() + "/" + WEVOTE_DIR;
    QDir().mkpath(wevoteTaxonomyDir);
}

const QString &PrepareWevoteTaxonomyDataTask::getWevoteTaxonomyDir() const {
    return wevoteTaxonomyDir;
}

void PrepareWevoteTaxonomyDataTask::run() {
    if (!isActual()) {
        coreLog.details("WEVOTE taxonomy is out of date, rebuilding...");
        prepareNamesFile();
        CHECK_OP_EXT(stateInfo, removeDestinationFiles = true, );

        prepareNodesFile();
        CHECK_OP_EXT(stateInfo, removeDestinationFiles = true, );
    } else {
        coreLog.details("WEVOTE taxonomy is up to date, skip");
    }
}

Task::ReportResult PrepareWevoteTaxonomyDataTask::report() {
    if (removeDestinationFiles) {
        QFile(wevoteTaxonomyDir + "/" + WEVOTE_NODES).remove();
        QFile(wevoteTaxonomyDir + "/" + WEVOTE_NAMES).remove();
    }
    return ReportResult_Finished;
}

bool PrepareWevoteTaxonomyDataTask::isActual() const {
    const QString wevoteNamesUrl = FileStorageUtils::getFileToFileInfo(taxonomyNamesUrl, StorageRoles::CUSTOM_FILE_TO_FILE, workflowProcess);
    const QString wevoteNodesUrl = FileStorageUtils::getFileToFileInfo(taxonomyNodesUrl, StorageRoles::CUSTOM_FILE_TO_FILE, workflowProcess);
    return !wevoteNamesUrl.isEmpty() && !wevoteNodesUrl.isEmpty();
}

void PrepareWevoteTaxonomyDataTask::prepareNamesFile() {
    QFile sourceFile(taxonomyNamesUrl);
    const bool isSourceOpened = sourceFile.open(QIODevice::ReadOnly);
    CHECK_EXT(isSourceOpened, setError(L10N::errorOpeningFileRead(sourceFile.fileName())), );

    QFile destinationFile(wevoteTaxonomyDir + "/" + WEVOTE_NAMES);
    const bool isDestinationOpened = destinationFile.open(QIODevice::WriteOnly);
    CHECK_EXT(isDestinationOpened, setError(L10N::errorOpeningFileWrite(destinationFile.fileName())), );

    QTextStream readStream(&sourceFile);
    QTextStream writeStream(&destinationFile);

    while (!readStream.atEnd()) {
        QString buffer = readStream.readLine().replace(QChar(' '), QChar('_'));
        if (buffer.contains(SCIENTIFIC_NAME)) {
            writeStream << buffer << '\n';
        }
    }

    FileStorage::FileInfo fileInfo(sourceFile.fileName(), StorageRoles::CUSTOM_FILE_TO_FILE, destinationFile.fileName());
    FileStorageUtils::addFileToFileInfo(fileInfo, workflowProcess);
}

void PrepareWevoteTaxonomyDataTask::prepareNodesFile() {
    QFile sourceFile(taxonomyNodesUrl);
    const bool isSourceOpened = sourceFile.open(QIODevice::ReadOnly);
    CHECK_EXT(isSourceOpened, setError(L10N::errorOpeningFileRead(sourceFile.fileName())), );

    QFile destinationFile(wevoteTaxonomyDir + "/" + WEVOTE_NODES);
    const bool isDestinationOpened = destinationFile.open(QIODevice::WriteOnly);
    CHECK_EXT(isDestinationOpened, setError(L10N::errorOpeningFileWrite(destinationFile.fileName())), );

    QTextStream readStream(&sourceFile);
    QTextStream writeStream(&destinationFile);

    while (!readStream.atEnd()) {
        writeStream << readStream.read(BUFFER_SIZE).replace(QChar(' '), QChar('_'));
    }

    FileStorage::FileInfo fileInfo(sourceFile.fileName(), StorageRoles::CUSTOM_FILE_TO_FILE, destinationFile.fileName());
    FileStorageUtils::addFileToFileInfo(fileInfo, workflowProcess);
}

}   // namespace U2
