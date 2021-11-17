/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2021 UniPro <ugene@unipro.ru>
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

#include "ExtractPrimerTask.h"
#include "PCRPrimerDesignForDNAAssemblyTask.h"

#include <QApplication>
#include <QDir>
#include <QFile>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/AnnotationTableObject.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/L10n.h>
#include <U2Core/LoadDocumentTask.h>
#include <U2Core/SaveDocumentTask.h>
#include <U2Core/U2DbiRegistry.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2SequenceDbi.h>
#include <U2Core/U2SequenceUtils.h>
#include <U2Core/UserApplicationsSettings.h>

namespace U2 {

const QString ExtractPrimerTask::BACKBONE_ANNOTATION_NAME = "Backbone";
const QString ExtractPrimerTask::RESULT_ANNOTATION_GROUP_NAME = "misc_feature";

 ExtractPrimerTask::ExtractPrimerTask(const ExtractPrimerTaskSettings &_settings)
    : Task(tr("Extract Primer Task"), TaskFlags_FOSE_COSC), settings(_settings) {
}

ExtractPrimerTask::~ExtractPrimerTask() {
    delete result;
}

void ExtractPrimerTask::run() {
    IOAdapterFactory *iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(BaseIOAdapters::LOCAL_FILE);
    SAFE_POINT_EXT(nullptr != iof, setError(L10N::nullPointerError("IOAdapterFactory")), );

    DocumentFormat *format = AppContext::getDocumentFormatRegistry()->getFormatById(BaseDocumentFormats::PLAIN_GENBANK);
    SAFE_POINT_EXT(nullptr != format, setError(L10N::nullPointerError("Genbank Format")), );
    QVariantMap hints;
    QScopedPointer<Document> doc(format->createNewLoadedDocument(iof, settings.outputFileUrl, stateInfo, hints));
    CHECK_OP(stateInfo, );

    U2DbiRef dbiRef = AppContext::getDbiRegistry()->getSessionTmpDbiRef(stateInfo);
    CHECK_OP(stateInfo, );

    DNASequence productSequence = getProductSequence();
    CHECK_OP(stateInfo, );
    U2EntityRef productRef = U2SequenceUtils::import(stateInfo, dbiRef, productSequence);
    CHECK_OP(stateInfo, );

    QList<SharedAnnotationData> annotations;
    int resultIndex = PCRPrimerDesignForDNAAssemblyTask::FRAGMENT_INDEX_TO_NAME.indexOf(settings.fragmentName);
    bool isForward = resultIndex % 2 == 0;

    U2Region fragmentRegion(0, productSequence.length());
    if (!settings.backboneSequence.isEmpty()) {
        SharedAnnotationData backboneAnnotationData(new AnnotationData());
        backboneAnnotationData->name = BACKBONE_ANNOTATION_NAME;
        U2Strand backboneStrand = U2Strand(U2Strand::Direct);
        if (isForward) {
            if (settings.direction == PCRPrimerDesignForDNAAssemblyTaskSettings::BackboneBearings::Backbone5) {
                backboneStrand = U2Strand(U2Strand::Complementary);
            }
            productSequence.seq.prepend(settings.backboneSequence);
            fragmentRegion.startPos =+ settings.backboneSequence.length();
            backboneAnnotationData->location->regions.append(U2Region(0, settings.backboneSequence.length()));
        } else {
            if (settings.direction == PCRPrimerDesignForDNAAssemblyTaskSettings::BackboneBearings::Backbone3) {
                backboneStrand = U2Strand(U2Strand::Complementary);
            }
            backboneAnnotationData->location->regions.append(U2Region(productSequence.length(), settings.backboneSequence.length()));
            productSequence.seq.append(settings.backboneSequence);
        }
        backboneAnnotationData->setStrand(backboneStrand);
        annotations.append(backboneAnnotationData);
    }
    SharedAnnotationData fragmentAnnotationData(new AnnotationData());
    fragmentAnnotationData->setStrand(isForward ? U2Strand(U2Strand::Direct) : U2Strand(U2Strand::Complementary));
    fragmentAnnotationData->name = settings.fragmentName;
    fragmentAnnotationData->location->regions.append(fragmentRegion);
    annotations.append(fragmentAnnotationData);
    auto sequenceObject = new U2SequenceObject(productSequence.getName(), productRef);
    sequenceObject->setWholeSequence(productSequence);
    doc->addObject(sequenceObject);
    auto annotationsTableObject = new AnnotationTableObject(productSequence.getName() + " annotations", dbiRef);
    doc->addObject(annotationsTableObject);
    annotationsTableObject->addObjectRelation(sequenceObject, ObjectRole_Sequence);
    annotationsTableObject->addAnnotations(annotations, RESULT_ANNOTATION_GROUP_NAME);
    result = doc.take();
}

Document *ExtractPrimerTask::takeResult() {
    CHECK(result != nullptr, nullptr);
    if (result->thread() != QCoreApplication::instance()->thread()) {
        result->moveToThread(QCoreApplication::instance()->thread());
    }
    Document *returnValue = result;
    result = nullptr;
    return returnValue;
}

DNASequence ExtractPrimerTask::getProductSequence() {
    DNASequence primerSequence("", "");
    DbiConnection connection(settings.sequenceRef.dbiRef, stateInfo);
    CHECK_OP(stateInfo, primerSequence);
    SAFE_POINT_EXT(connection.dbi != nullptr, setError(L10N::nullPointerError("DBI")), primerSequence);
    U2SequenceDbi *sequenceDbi = connection.dbi->getSequenceDbi();
    SAFE_POINT_EXT(sequenceDbi != nullptr, setError(L10N::nullPointerError("Sequence DBI")), primerSequence);

    U2Sequence sequence = sequenceDbi->getSequenceObject(settings.sequenceRef.entityId, stateInfo);
    CHECK_OP(stateInfo, primerSequence);

    primerSequence.seq = sequenceDbi->getSequenceData(settings.sequenceRef.entityId, settings.fragmentLocation, stateInfo);
    CHECK_OP(stateInfo, primerSequence);
    primerSequence.setName(settings.fragmentName + " annotations");
    return primerSequence;
}

ExtractPrimerAndOpenDocumentTask::ExtractPrimerAndOpenDocumentTask(const ExtractPrimerTaskSettings &_settings)
    : Task(tr("Extract Primer And Open View Task"), TaskFlags_NR_FOSE_COSC), settings(_settings) {
    prepareUrl();
    CHECK_OP(stateInfo, );
    extractTask = new ExtractPrimerTask(settings);
 }

void ExtractPrimerAndOpenDocumentTask::prepare() {
    addSubTask(extractTask);
}

QList<Task *> ExtractPrimerAndOpenDocumentTask::onSubTaskFinished(Task *subTask) {
    QList<Task *> result;
    CHECK(extractTask == subTask, result);
    SaveDocFlags flags;
    flags |= SaveDoc_OpenAfter;
    flags |= SaveDoc_DestroyAfter;
    flags |= SaveDoc_Overwrite;
    QFile::remove(settings.outputFileUrl);
    result << new SaveDocumentTask(extractTask->takeResult(), flags);
    return result;
}

U2::Task::ReportResult ExtractPrimerAndOpenDocumentTask::report() {
    if (extractTask->isCanceled()) {
        QFile::remove(settings.outputFileUrl);
    }
    return ReportResult_Finished;
}

void ExtractPrimerAndOpenDocumentTask::prepareUrl() {
    // generate file name
    const QString fragmentNameInLowerCase = settings.fragmentName.toLower().replace(" ", "_");
    QString fileName = settings.originalSequenceFileName + "_" + fragmentNameInLowerCase;

    QString outputDir = AppContext::getAppSettings()->getUserAppsSettings()->getDefaultDataDirPath() + QDir::separator() + "pcr";
    QString url = GUrlUtils::prepareDirLocation(outputDir, stateInfo) + QDir::separator() + fileName + ".gb";
    CHECK_OP(stateInfo, );

    settings.outputFileUrl = GUrlUtils::rollFileName(url, "_", QSet<QString>());

    // reserve file
    QFile file(settings.outputFileUrl);
    if (!file.open(QIODevice::WriteOnly)) {
        stateInfo.setError(L10N::errorOpeningFileWrite(GUrl(settings.outputFileUrl)));
        return;
    }
    file.close();
}

}
