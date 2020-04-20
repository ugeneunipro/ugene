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

#include <U2Core/AppContext.h>
#include <U2Core/CreateAnnotationTask.h>
#include <U2Core/DeleteObjectsTask.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/GenbankFeatures.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/LoadDocumentTask.h>
#include <U2Core/SaveDocumentTask.h>
#include <U2Core/U1AnnotationUtils.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/VariantTrackObject.h>

#include <U2Formats/SnpeffInfoParser.h>

#include "ConvertSnpeffVariationsToAnnotationsTask.h"

namespace U2 {

const QString ConvertSnpeffVariationsToAnnotationsTask::CHROM_QUALIFIER_NAME = "chrom";
const QString ConvertSnpeffVariationsToAnnotationsTask::LOCATION_QUALIFIER_NAME = "Location";
const QString ConvertSnpeffVariationsToAnnotationsTask::REFERENCE_QUALIFIER_NAME = "Reference_bases";
const QString ConvertSnpeffVariationsToAnnotationsTask::ALTERNATE_QUALIFIER_NAME = "Alternate_bases";
const QString ConvertSnpeffVariationsToAnnotationsTask::ALLELE_QUALIFIER_NAME = "Allele";
const QString ConvertSnpeffVariationsToAnnotationsTask::ID_QUALIFIER_NAME = "ID";

ConvertSnpeffVariationsToAnnotationsTask::ConvertSnpeffVariationsToAnnotationsTask(const QList<VariantTrackObject *> &variantTrackObjects)
    : Task(tr("Convert SnpEff variations to annotations task"), TaskFlag_None),
      variantTrackObjects(variantTrackObjects)
{

}

const QMap<QString, QList<SharedAnnotationData> > & ConvertSnpeffVariationsToAnnotationsTask::getAnnotationsData() const {
    return annotationTablesData;
}

void ConvertSnpeffVariationsToAnnotationsTask::run() {
    foreach (VariantTrackObject *variantTrackObject, variantTrackObjects) {
        QList<SharedAnnotationData> annotationTableData;

        const U2VariantTrack variantTrack = variantTrackObject->getVariantTrack(stateInfo);
        CHECK_OP(stateInfo, );

        QScopedPointer<U2DbiIterator<U2Variant> > variantsIterator(variantTrackObject->getVariants(U2_REGION_MAX, stateInfo));
        CHECK_OP(stateInfo, );

        SharedAnnotationData tableAnnotationData(new AnnotationData);
        tableAnnotationData->qualifiers << U2Qualifier(CHROM_QUALIFIER_NAME, variantTrack.sequenceName);
        tableAnnotationData->type = U2FeatureTypes::Variation;

        SnpeffInfoParser infoParser;
        while (variantsIterator.data()->hasNext()) {
            const U2Variant variant = variantsIterator.data()->next();

            SharedAnnotationData entryAnnotationData = tableAnnotationData;
            entryAnnotationData->name = GBFeatureUtils::getKeyInfo(GBFeatureKey_variation).text;
            entryAnnotationData->location->regions << U2Region(variant.startPos, variant.endPos - variant.startPos + 1);
            entryAnnotationData->qualifiers << U2Qualifier(REFERENCE_QUALIFIER_NAME, variant.refData);
            entryAnnotationData->qualifiers << U2Qualifier(ALTERNATE_QUALIFIER_NAME, variant.obsData);
            entryAnnotationData->qualifiers << U2Qualifier(LOCATION_QUALIFIER_NAME,
                                                           U2Region(variant.startPos + 1, variant.endPos - variant.startPos + 1).toString(U2Region::FormatDots));
            if (!variant.publicId.isEmpty()) {
                entryAnnotationData->qualifiers << U2Qualifier(ID_QUALIFIER_NAME, variant.publicId);
            }

            U2OpStatusImpl os;
            const QList<QList<U2Qualifier> > qualifiersList = infoParser.parse(os, variant.additionalInfo[U2Variant::VCF4_INFO]);
            CHECK_OP(os, );
            CHECK_OP(stateInfo, );
            stateInfo.addWarnings(os.getWarnings());

            foreach (const QList<U2Qualifier> &qualifiers, qualifiersList) {
                if (qualifiers.isEmpty()) {
                    continue;
                }

                SharedAnnotationData parsedAnnotationData = entryAnnotationData;
                parsedAnnotationData->qualifiers << qualifiers.toVector();
                if (U1AnnotationUtils::containsQualifier(qualifiers, ALLELE_QUALIFIER_NAME)) {
                    U1AnnotationUtils::removeAllQualifier(parsedAnnotationData, ALTERNATE_QUALIFIER_NAME);
                }
                annotationTableData << parsedAnnotationData;
            }

            if (!os.hasWarnings() && qualifiersList.isEmpty()) {
                annotationTableData << entryAnnotationData;
            }
        }
        annotationTablesData.insert(variantTrack.sequenceName, annotationTableData);
    }
}

LoadConvertAndSaveSnpeffVariationsToAnnotationsTask::LoadConvertAndSaveSnpeffVariationsToAnnotationsTask(const QString &variationsUrl,
                                                                                                         const U2DbiRef &dstDbiRef,
                                                                                                         const QString &dstUrl,
                                                                                                         const QString &formatId)
    : Task(tr("Load file and convert SnpEff variations to annotations task"), TaskFlags_NR_FOSE_COSC | TaskFlag_CollectChildrenWarnings),
      variationsUrl(variationsUrl),
      dstDbiRef(dstDbiRef),
      dstUrl(dstUrl),
      formatId(formatId),
      loadTask(NULL),
      convertTask(NULL),
      saveTask(NULL),
      loadedVariationsDocument(NULL),
      annotationsDocument(NULL)
{
    SAFE_POINT_EXT(!variationsUrl.isEmpty(), setError("Source VCF file URL is empty"), );
    SAFE_POINT_EXT(dstDbiRef.isValid(), setError("Destination DBI reference is invalid"), );
    SAFE_POINT_EXT(!dstUrl.isEmpty(), setError("Destination file URL is empty"), );
    SAFE_POINT_EXT(!formatId.isEmpty(), setError("Destination file format is empty"), );
}

LoadConvertAndSaveSnpeffVariationsToAnnotationsTask::~LoadConvertAndSaveSnpeffVariationsToAnnotationsTask() {
    qDeleteAll(annotationTableObjects);
    delete loadedVariationsDocument;
    delete annotationsDocument;
}

const QString & LoadConvertAndSaveSnpeffVariationsToAnnotationsTask::getResultUrl() const {
    return dstUrl;
}

void LoadConvertAndSaveSnpeffVariationsToAnnotationsTask::prepare() {
    QVariantMap hints;
    hints[DocumentFormat::DBI_REF_HINT] = QVariant::fromValue<U2DbiRef>(dstDbiRef);
    loadTask = LoadDocumentTask::getDefaultLoadDocTask(variationsUrl, hints);
    addSubTask(loadTask);
}

QList<Task *> LoadConvertAndSaveSnpeffVariationsToAnnotationsTask::onSubTaskFinished(Task *subTask) {
    QList<Task *> newSubtasks;
    CHECK_OP(stateInfo, newSubtasks);

    if (loadTask == subTask) {
        loadedVariationsDocument = loadTask->takeDocument();
        CHECK_EXT(NULL != loadedVariationsDocument, setError(tr("'%1' load failed, the result document is NULL").arg(variationsUrl)), newSubtasks);
        loadedVariationsDocument->setDocumentOwnsDbiResources(false);

        QList<GObject *> objects = loadedVariationsDocument->findGObjectByType(GObjectTypes::VARIANT_TRACK);
        CHECK_EXT(!objects.isEmpty(), setError(tr("File '%1' doesn't contain variation tracks").arg(variationsUrl)), newSubtasks);

        QList<VariantTrackObject *> variantTrackObjects;
        foreach (GObject *object, objects) {
            VariantTrackObject *variantTrackObject = qobject_cast<VariantTrackObject *>(object);
            SAFE_POINT_EXT(NULL != variantTrackObject, setError("Can't cast GObject to VariantTrackObject"), newSubtasks);
            variantTrackObjects << variantTrackObject;
        }

        convertTask = new ConvertSnpeffVariationsToAnnotationsTask(variantTrackObjects);
        newSubtasks << convertTask;
    }

    if (convertTask == subTask) {
        QMap<QString, QList<SharedAnnotationData> > annotationsData = convertTask->getAnnotationsData();
        foreach (const QString &chromosome, annotationsData.keys()) {
            AnnotationTableObject *annotationTableObject = new AnnotationTableObject(chromosome, dstDbiRef);
            annotationTableObjects << annotationTableObject;

            createAnnotationsTasks << new CreateAnnotationsTask(annotationTableObject, annotationsData[chromosome], "Variations");
        }
        newSubtasks << createAnnotationsTasks;
    }

    if (createAnnotationsTasks.contains(subTask)) {
        createAnnotationsTasks.removeAll(subTask);
        if (createAnnotationsTasks.isEmpty()) {
            prepareSaveTask();
            CHECK_OP(stateInfo, newSubtasks);
            newSubtasks << saveTask;
            newSubtasks << new DeleteObjectsTask(loadedVariationsDocument->getObjects());
            delete loadedVariationsDocument;
            loadedVariationsDocument = NULL;
        }
    }

    if (saveTask == subTask) {
        newSubtasks << new DeleteObjectsTask(annotationsDocument->getObjects());
        delete annotationsDocument;
        annotationsDocument = NULL;
    }

    return newSubtasks;
}

Document * LoadConvertAndSaveSnpeffVariationsToAnnotationsTask::prepareDocument() {
    DocumentFormat *format = AppContext::getDocumentFormatRegistry()->getFormatById(formatId);
    SAFE_POINT_EXT(NULL != format, setError(QString("Document format '%1' not found in the registry").arg(formatId)), NULL);
    IOAdapterFactory *ioAdapterFactory = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(dstUrl));
    SAFE_POINT_EXT(NULL != ioAdapterFactory, setError(L10N::nullPointerError("ioAdapterFactory")), NULL);

    QVariantMap hints;
    hints[DocumentFormat::DBI_REF_HINT] = QVariant::fromValue<U2DbiRef>(dstDbiRef);

    Document *document = format->createNewLoadedDocument(ioAdapterFactory, dstUrl, stateInfo, hints);
    CHECK_OP(stateInfo, NULL);
    document->setDocumentOwnsDbiResources(false);

    foreach (AnnotationTableObject *annotationTableObject, annotationTableObjects) {
        document->addObject(annotationTableObject);
    }
    annotationTableObjects.clear();

    return document;
}

void LoadConvertAndSaveSnpeffVariationsToAnnotationsTask::prepareSaveTask() {
    annotationsDocument = prepareDocument();
    CHECK_OP(stateInfo, );
    saveTask = new SaveDocumentTask(annotationsDocument);
}

}   // namespace U2
