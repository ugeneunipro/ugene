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
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2SequenceUtils.h>

#include "ExportSelectedSeqRegionsTask.h"

namespace U2 {

namespace {

bool isSeqObjectValid(const QPointer<U2SequenceObject> &seqObject, U2OpStatus &os) {
    if (seqObject.isNull()) {
        os.setError(CreateExportItemsFromSeqRegionsTask::tr("Invalid sequence object detected"));
        return false;
    } else {
        return true;
    }
}

// removes all annotation regions that does not intersect the given region
// adjusts startpos to make in relative to the given region
void adjustAnnotationLocations(const U2Region &r, QList<SharedAnnotationData> &anns) {
    for (int i = anns.size(); --i >= 0;) {
        SharedAnnotationData &d = anns[i];
        for (int j = d->location->regions.size(); --j >= 0;) {
            U2Region &ar = d->location->regions[j];
            U2Region resr = ar.intersect(r);
            if (resr.isEmpty()) {
                d->location->regions.remove(j);
                continue;
            }
            resr.startPos -= r.startPos;
            d->location->regions[j] = resr;
        }
        if (d->location->regions.isEmpty()) {
            anns.removeAt(i);
        }
    }
}

}

//////////////////////////////////////////////////////////////////////////
/// CreateExportItemsFromSeqRegionsTask
//////////////////////////////////////////////////////////////////////////

CreateExportItemsFromSeqRegionsTask::CreateExportItemsFromSeqRegionsTask(const QPointer<U2SequenceObject> &seqObject,
    const QList<QPointer<AnnotationTableObject> > &connectedAts, const QVector<U2Region> &regions, const ExportSequenceTaskSettings &exportSettings,
    const DNATranslation *aminoTrans, const DNATranslation *backTranslation, const DNATranslation *complTrans)
    : Task(tr("Extract sequences from regions task"), TaskFlag_None), seqObject(seqObject), annotations(connectedAts), regions(regions),
    exportSettings(exportSettings), aminoTrans(aminoTrans), backTranslation(backTranslation), complTrans(complTrans)
{
    CHECK(isSeqObjectValid(seqObject, stateInfo), );

    foreach (const QPointer<AnnotationTableObject> &aobj, annotations) {
        if (aobj.isNull()) {
            stateInfo.setError(tr("Invalid annotation table detected"));
            return;
        }
    }
}

const ExportSequenceTaskSettings & CreateExportItemsFromSeqRegionsTask::getExportSettings() const {
    return exportSettings;
}

QList<SharedAnnotationData> CreateExportItemsFromSeqRegionsTask::findAnnotationsInRegion(const U2Region &region) {
    QList<SharedAnnotationData> result;
    foreach (const QPointer<AnnotationTableObject> &aobj, annotations) {
        if (aobj.isNull()) {
            stateInfo.setError(tr("Invalid annotation table detected"));
            return result;
        }
        foreach (Annotation *a, aobj->getAnnotationsByRegion(region)) {
            result.append(a->getData());
        }
    }
    return result;
}

void CreateExportItemsFromSeqRegionsTask::run() {
    const qint64 sequenceChunkMaxLength = 4194304; // 4 MiB chunk
    const U2DbiRef dbiRef = AppContext::getDbiRegistry()->getSessionTmpDbiRef(stateInfo);
    SAFE_POINT_OP(stateInfo, );

    CHECK(isSeqObjectValid(seqObject, stateInfo), );

    DbiOperationsBlock dbiBlock(dbiRef, stateInfo);
    Q_UNUSED(dbiBlock);

    ExportSequenceItem startItem;
    ExportSequenceItem endItem;
    int regionCount = 0;
    QSet<QString> usedNames;
    foreach (const U2Region &r, regions) {
        const QString prefix = QString("region [%1 %2]").arg(r.startPos + 1).arg(r.endPos());
        QString name = prefix;

        int sameNameCounter = 0;
        while (usedNames.contains(name)) {
            name = prefix + "|" + QString::number(++sameNameCounter);
        }

        usedNames.insert(name);

        const qint64 seqLength = seqObject->getSequenceLength();

        U2SequenceImporter seqImporter(QVariantMap(), true);
        bool isCircular = seqObject->isCircular() && (r.startPos == 0 && r.length == seqLength);
        seqImporter.startSequence(stateInfo, dbiRef, U2ObjectDbi::ROOT_FOLDER, name, isCircular);
        SAFE_POINT_OP(stateInfo, );
        for (qint64 pos = r.startPos; pos < r.endPos(); pos += sequenceChunkMaxLength) {
            const qint64 currentChunkSize = qMin(sequenceChunkMaxLength, r.endPos() - pos);
            const U2Region chunkRegion(pos, currentChunkSize);

            CHECK(isSeqObjectValid(seqObject, stateInfo), );
            const QByteArray chunkContent = seqObject->getSequenceData(chunkRegion);

            seqImporter.addBlock(chunkContent.constData(), chunkContent.length(), stateInfo);
            SAFE_POINT_OP(stateInfo, );
        }
        const U2Sequence importedRegionSeq = seqImporter.finalizeSequence(stateInfo);
        SAFE_POINT_OP(stateInfo, );

        ExportSequenceItem ei;
        ei.setOwnershipOverSeq(importedRegionSeq, dbiRef);
        ei.complTT = complTrans;
        ei.aminoTT = aminoTrans;
        ei.backTT = backTranslation;
        if (exportSettings.saveAnnotations) {
            ei.annotations = findAnnotationsInRegion(r);
            CHECK_OP(stateInfo, );
            adjustAnnotationLocations(r, ei.annotations);
        }
        exportSettings.items.append(ei);

        stateInfo.setProgress(100 * ++regionCount / regions.size());

        const qint64 endPos = r.endPos();
        CHECK_CONTINUE(!(r.startPos == 0 && endPos == seqLength));
        CHECK_OPERATIONS(r.startPos != 0, startItem = ei, continue);
        CHECK_OPERATIONS(endPos != seqLength, endItem = ei, continue);
    }

    if (!startItem.isEmpty() && !endItem.isEmpty() && seqObject->isCircular()) {
        const ExportSequenceItem circularItem = ExportSequenceTask::mergedCircularItem(endItem, startItem, stateInfo);
        CHECK_OP(stateInfo, );

        exportSettings.items.removeOne(startItem);
        exportSettings.items.removeOne(endItem);
        exportSettings.items.append(circularItem);
    }
}

//////////////////////////////////////////////////////////////////////////
/// ExportSelectedSeqRegionsTask
//////////////////////////////////////////////////////////////////////////

ExportSelectedSeqRegionsTask::ExportSelectedSeqRegionsTask(U2SequenceObject *seqObject, const QSet<AnnotationTableObject *> &connectedAts,
    const QVector<U2Region> &regions, const ExportSequenceTaskSettings &exportSettings, const DNATranslation *aminoTrans,
    const DNATranslation *backTrans, const DNATranslation *complTrans)
    : DocumentProviderTask(tr("Export selected regions from a sequence task"), TaskFlags_NR_FOSE_COSC), seqObject(seqObject),
    regions(regions), exportSettings(exportSettings), aminoTrans(aminoTrans), backTrans(backTrans), complTrans(complTrans)
{
    CHECK(isSeqObjectValid(seqObject, stateInfo), );

    foreach (AnnotationTableObject *aObj, connectedAts) {
        if (NULL == aObj) {
            stateInfo.setError(tr("Invalid annotation table detected"));
            return;
        } else {
            annotations.append(QPointer<AnnotationTableObject>(aObj));
        }
    }
}

void ExportSelectedSeqRegionsTask::prepare() {
    CreateExportItemsFromSeqRegionsTask *t = new CreateExportItemsFromSeqRegionsTask(seqObject, annotations, regions,
        exportSettings, aminoTrans, backTrans, complTrans);
    addSubTask(t);
}

QList<Task *> ExportSelectedSeqRegionsTask::onSubTaskFinished(Task *subTask) {
    QList<Task *> resultTasks;

    CHECK(!subTask->hasError() && !subTask->isCanceled(), resultTasks);

    CreateExportItemsFromSeqRegionsTask *createExportItemsTask = qobject_cast<CreateExportItemsFromSeqRegionsTask *>(subTask);
    if (NULL != createExportItemsTask) {
        resultTasks.append(new ExportSequenceTask(createExportItemsTask->getExportSettings()));
        return resultTasks;
    }

    ExportSequenceTask *exportSeqTask = qobject_cast<ExportSequenceTask *>(subTask);
    if (NULL != exportSeqTask) {
        resultDocument = exportSeqTask->takeDocument();
    }
    return resultTasks;
}

} // namespace U2
