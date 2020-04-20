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

#include <QCoreApplication>
#include <QDir>

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/Counter.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DNASequenceUtils.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/GenbankFeatures.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/L10n.h>
#include <U2Core/SaveDocumentTask.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2SequenceDbi.h>
#include <U2Core/U2SequenceUtils.h>
#include <U2Core/UserApplicationsSettings.h>

#include "ExtractProductTask.h"

namespace U2 {

ExtractProductSettings::ExtractProductSettings()
: annotationsExtraction(Inner)
{

}

QString ExtractProductTask::getProductName(const QString &sequenceName, qint64 sequenceLength, const U2Region &region, bool fileName) {
    qint64 endPos = region.endPos();
    if (endPos > sequenceLength) {
        endPos = endPos % sequenceLength;
    }

    return QString("%1%2%3-%4")
        .arg(sequenceName)
        .arg(fileName ? "_" : ":")
        .arg(region.startPos + 1)
        .arg(endPos);
}

ExtractProductTask::ExtractProductTask(const InSilicoPcrProduct &product, const ExtractProductSettings &settings)
: Task(tr("Extract PCR product"), TaskFlags_FOSE_COSC), product(product), settings(settings), wholeSequenceLength(0), result(NULL)
{
    GCOUNTER(cvar, tvar, "ExtractProductTask");
}

ExtractProductTask::~ExtractProductTask() {
    delete result;
}

DNASequence ExtractProductTask::getProductSequence() {
    DNASequence sequence = extractTargetSequence();
    CHECK_OP(stateInfo, sequence);
    sequence.seq = toProductSequence(sequence.seq);
    return sequence;
}

DNASequence ExtractProductTask::extractTargetSequence() {
    DNASequence result("", "");
    DbiConnection connection(settings.sequenceRef.dbiRef, stateInfo);
    CHECK_OP(stateInfo, result);
    SAFE_POINT_EXT(NULL != connection.dbi, setError(L10N::nullPointerError("DBI")), result);
    U2SequenceDbi *sequenceDbi = connection.dbi->getSequenceDbi();
    SAFE_POINT_EXT(NULL != sequenceDbi, setError(L10N::nullPointerError("Sequence DBI")), result);

    U2Sequence sequence = sequenceDbi->getSequenceObject(settings.sequenceRef.entityId, stateInfo);
    CHECK_OP(stateInfo, result);
    wholeSequenceLength = sequence.length;

    result.seq = sequenceDbi->getSequenceData(settings.sequenceRef.entityId, product.region, stateInfo);
    CHECK_OP(stateInfo, result);
    if (product.region.endPos() > sequence.length) {
        U2Region tail(0, product.region.endPos() % sequence.length);
        const int ledgeSize = product.forwardPrimerLedge.size() + product.reversePrimerLedge.size();
        if (tail.length == ledgeSize) {
            result.seq = result.seq.insert(0, product.forwardPrimerLedge);
            result.seq = result.seq.insert(result.seq.size(), DNASequenceUtils::reverseComplement(product.reversePrimerLedge));
            sequence.length += ledgeSize;
            wholeSequenceLength = sequence.length;
        } else {
            result.seq += sequenceDbi->getSequenceData(settings.sequenceRef.entityId, tail, stateInfo);
            CHECK_OP(stateInfo, result);
        }
    }

    result.setName(getProductName(sequence.visualName, sequence.length, product.region));
    return result;
}

QByteArray ExtractProductTask::toProductSequence(const QByteArray &targetSequence) const {
    int startPos = product.forwardPrimerMatchLength;
    int length = targetSequence.length() - product.forwardPrimerMatchLength - product.reversePrimerMatchLength;
    return product.forwardPrimer + targetSequence.mid(startPos, length) + DNASequenceUtils::reverseComplement(product.reversePrimer);
}

namespace {
    bool prepareCircularRegions(const U2Region &begin, const U2Region &end, qint64 sequenceLength, QVector<U2Region> &regions) {
        bool cropped = false;
        QVector<U2Region> result;
        for (int i=0; i<regions.size(); i++) {
            U2Region region = regions[i];
            if (region.intersects(begin) && region.intersects(end)) {
                U2Region endIntersection = region.intersect(end);
                endIntersection.startPos += sequenceLength;
                result << region.intersect(begin);
                result << endIntersection;
                cropped = true;
            } else if (region.intersects(end)) {
                region.startPos += sequenceLength;
                result << region;
            } else {
                result << region;
            }
        }
        regions = result;
        return cropped;
    }

    bool crop(const U2Region &within, QVector<U2Region> &regions) {
        bool cropped = false;
        foreach (const U2Region &region, regions) {
            if (!within.intersects(region)) {
                int idx = regions.indexOf(region);
                if (-1 != idx) {
                    regions.remove(idx);
                }
                cropped = true;
            }
        }

        for (int i=0; i<regions.size(); i++) {
            U2Region &region = regions[i];
            const U2Region intersection = within.intersect(region);
            if (intersection != region) {
                region = intersection;
                cropped = true;
            }
        }
        return cropped;
    }
}

void ExtractProductTask::addProductAnnotations(AnnotationTableObject *targetObject, const U2EntityRef &annsRef) const {
    QScopedPointer<AnnotationTableObject> annsObject(new AnnotationTableObject("features", annsRef));
    const bool contain = (ExtractProductSettings::Inner == settings.annotationsExtraction);
    QSet<Annotation*> anns = annsObject->getAnnotationsByRegion(product.region, contain).toSet();

    U2Region begin = product.region;
    U2Region end(0, 0);
    if (product.region.endPos() > wholeSequenceLength) { // circular
        begin.length = wholeSequenceLength - product.region.startPos;
        end.length = product.region.endPos() % wholeSequenceLength;
        anns.unite(annsObject->getAnnotationsByRegion(end, contain).toSet());
    }

    foreach (Annotation *ann, anns) {
        SharedAnnotationData data = ann->getData();
        for (int i = 0; i < data->location->regions.size(); i++) {
            data->location->regions[i].startPos += product.forwardPrimerLedge.size();
        }
        bool cropped = prepareCircularRegions(begin, end, wholeSequenceLength, data->location->regions);
        cropped |= crop(product.region, data->location->regions);
        U2Region::shift(-product.region.startPos, data->location->regions);
        if (cropped) {
            U2Qualifier note;
            note.name = "warning";
            note.value = "This is only a part of the original annotation. The annotation was truncated by PCR product extraction.";
            data->qualifiers.prepend(note);
        }

        if (!data->location->regions.isEmpty()) {
            QList<SharedAnnotationData> dataList;
            dataList << data;
            targetObject->addAnnotations(dataList, ann->getGroup()->getGroupPath());
        }
    }
}

SharedAnnotationData ExtractProductTask::getPrimerAnnotation(int matchLengh, U2Strand::Direction strand, int sequenceLength) {
    SharedAnnotationData result(new AnnotationData);
    U2Region region;
    if (U2Strand::Direct == strand) {
        region = U2Region(0, matchLengh);
    } else {
        region = U2Region(sequenceLength - matchLengh, matchLengh);
    }
    result->location->regions << region;
    result->location->strand = U2Strand(strand);

    result->name = GBFeatureUtils::getKeyInfo(GBFeatureKey_misc_feature).text;
    result->qualifiers << U2Qualifier(GBFeatureUtils::QUALIFIER_NOTE, "primer");
    return result;
}

void ExtractProductTask::run() {
    IOAdapterFactory *iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(BaseIOAdapters::LOCAL_FILE);
    SAFE_POINT_EXT(NULL != iof, setError(L10N::nullPointerError("IOAdapterFactory")), );

    DocumentFormat *format = AppContext::getDocumentFormatRegistry()->getFormatById(BaseDocumentFormats::PLAIN_GENBANK);
    SAFE_POINT_EXT(NULL != format, setError(L10N::nullPointerError("Genbank Format")), );
    QVariantMap hints;
    if (settings.targetDbiRef.isValid()) {
        hints[DocumentFormat::DBI_REF_HINT] = qVariantFromValue(settings.targetDbiRef);
    }
    QScopedPointer<Document> doc(format->createNewLoadedDocument(iof, settings.outputFile, stateInfo, hints));
    CHECK_OP(stateInfo, );

    U2DbiRef dbiRef = AppContext::getDbiRegistry()->getSessionTmpDbiRef(stateInfo);
    CHECK_OP(stateInfo, );

    DNASequence productSequence = getProductSequence();
    CHECK_OP(stateInfo, );
    U2EntityRef productRef = U2SequenceUtils::import(stateInfo, dbiRef, productSequence);
    CHECK_OP(stateInfo, );

    U2SequenceObject *sequenceObject = new U2SequenceObject(productSequence.getName(), productRef);
    doc->addObject(sequenceObject);
    AnnotationTableObject *annotations = new AnnotationTableObject(productSequence.getName() + " features", dbiRef);
    annotations->addAnnotations(QList<SharedAnnotationData>() << getPrimerAnnotation(product.forwardPrimerMatchLength, U2Strand::Direct, productSequence.length()));
    annotations->addAnnotations(QList<SharedAnnotationData>() << getPrimerAnnotation(product.reversePrimerMatchLength, U2Strand::Complementary, productSequence.length()));
    annotations->addObjectRelation(GObjectRelation(GObjectReference(sequenceObject), ObjectRole_Sequence));
    doc->addObject(annotations);

    if (ExtractProductSettings::None != settings.annotationsExtraction) {
        foreach (const U2EntityRef &annsRef, settings.annotationRefs) {
            addProductAnnotations(annotations, annsRef);
        }
    }

    result = doc.take();
}

Document * ExtractProductTask::takeResult() {
    CHECK(NULL != result, NULL);
    if (result->thread() != QCoreApplication::instance()->thread()) {
        result->moveToThread(QCoreApplication::instance()->thread());
    }
    Document *returnValue = result;
    result = NULL;
    return returnValue;
}

const InSilicoPcrProduct & ExtractProductTask::getProduct() const {
    return product;
}

/************************************************************************/
/* ExtractProductWrapperTask */
/************************************************************************/
ExtractProductWrapperTask::ExtractProductWrapperTask(const InSilicoPcrProduct &product, const QString &sequenceName, qint64 sequenceLength, const ExtractProductSettings &settings)
: Task(tr("Extract PCR product and open document"), TaskFlags_NR_FOSE_COSC), extractTask(NULL), settings(settings)
{
    prepareUrl(product, sequenceName, sequenceLength);
    CHECK_OP(stateInfo, );
    extractTask = new ExtractProductTask(product, this->settings);
}

void ExtractProductWrapperTask::prepare() {
    addSubTask(extractTask);
}

QList<Task*> ExtractProductWrapperTask::onSubTaskFinished(Task *subTask) {
    QList<Task*> result;
    CHECK(extractTask == subTask, result);
    SaveDocFlags flags;
    flags |= SaveDoc_OpenAfter;
    flags |= SaveDoc_DestroyAfter;
    flags |= SaveDoc_Overwrite;
    QFile::remove(settings.outputFile);
    result << new SaveDocumentTask(extractTask->takeResult(), flags);
    return result;
}

Task::ReportResult ExtractProductWrapperTask::report() {
    if (extractTask->isCanceled()) {
        QFile::remove(settings.outputFile);
    }
    return ReportResult_Finished;
}

void ExtractProductWrapperTask::prepareUrl(const InSilicoPcrProduct &product, const QString &sequenceName, qint64 sequenceLength) {
    // generate file name
    QString fileName = ExtractProductTask::getProductName(sequenceName, sequenceLength, product.region) + ".gb";
    QRegExp regExp("[^A-z0-9_\\-\\s\\.\\(\\)]");
    fileName.replace(regExp, "_");

    QString outputDir = AppContext::getAppSettings()->getUserAppsSettings()->getDefaultDataDirPath() + QDir::separator() + "pcr";
    QString url = GUrlUtils::prepareDirLocation(outputDir, stateInfo) + QDir::separator() + fileName;
    CHECK_OP(stateInfo, );

    settings.outputFile = GUrlUtils::rollFileName(url, "_", QSet<QString>());

    // reserve file
    QFile file(settings.outputFile);
    file.open(QIODevice::WriteOnly);
    file.close();
}

} // U2
