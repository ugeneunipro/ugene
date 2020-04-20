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

#include <QFileInfo>

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/AppContext.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DNATranslation.h>
#include <U2Core/DNATranslationImpl.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/GObjectRelationRoles.h>
#include <U2Core/L10n.h>
#include <U2Core/SequenceUtils.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U2AlphabetUtils.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SequenceUtils.h>
#include <U2Core/U2SafePoints.h>

#include "ExportUtils.h"
#include "ExportSequenceTask.h"

static const qint64 MAX_CHUNK_LENGTH = 4194305; // (4 MiB + 1) chunk. Remainder of the division by 3 is 2.

static QMutex seqRefGuard(QMutex::NonRecursive);

namespace U2 {

QMap<U2EntityRef, int> ExportSequenceItem::sequencesRefCounts = QMap<U2EntityRef, int>();

ExportSequenceItem::ExportSequenceItem()
    : circular(false), alphabet(NULL), length(0), complTT(NULL), aminoTT(NULL), backTT(NULL)
{

}

ExportSequenceItem::ExportSequenceItem(const ExportSequenceItem &other)
    : seqRef(other.seqRef), name(other.name), circular(other.circular), alphabet(other.alphabet), length(other.length),
    annotations(other.annotations), complTT(other.complTT), aminoTT(other.aminoTT), backTT(other.backTT)
{
    if (ownsSeq()) {
        incrementSeqRefCount();
    }
}

ExportSequenceItem & ExportSequenceItem::operator =(const ExportSequenceItem &other) {
    seqRef = other.seqRef;
    name = other.name;
    circular = other.circular;
    alphabet = other.alphabet;
    length = other.length;
    annotations = other.annotations;
    complTT = other.complTT;
    aminoTT = other.aminoTT;
    backTT = other.backTT;

    if (ownsSeq()) {
        incrementSeqRefCount();
    }
    return *this;
}

bool ExportSequenceItem::operator==(const ExportSequenceItem& other) const {
    return seqRef == other.seqRef &&
           name == other.name &&
           circular == other.circular &&
           alphabet == other.alphabet &&
           length == other.length &&
           annotations == other.annotations &&
           complTT == other.complTT &&
           aminoTT == other.aminoTT &&
           backTT == other.backTT;
}

ExportSequenceItem::~ExportSequenceItem() {
    releaseOwnedSeq();
}

void ExportSequenceItem::releaseOwnedSeq() {
    if (ownsSeq() && 0 == decrementSeqRefCount()) {
        stopSeqOwnership();
        U2OpStatus2Log os;
        DbiConnection con(seqRef.dbiRef, os);
        CHECK_OP(os, );
        con.dbi->getObjectDbi()->removeObject(seqRef.entityId, os);
        CHECK_OP(os, );
    }
}

void ExportSequenceItem::startSeqOwnership() {
    SAFE_POINT(seqRef.isValid(), "Invalid sequence DBI reference", );

    QMutexLocker locker(&seqRefGuard);
    SAFE_POINT(!sequencesRefCounts.contains(seqRef), "Sequence is unexpectedly tracked", );
    sequencesRefCounts.insert(seqRef, 1);
}

int ExportSequenceItem::incrementSeqRefCount() const {
    QMutexLocker locker(&seqRefGuard);
    SAFE_POINT(sequencesRefCounts.contains(seqRef) && sequencesRefCounts[seqRef] > 0, "Sequence is unexpectedly not tracked", -1);
    return ++sequencesRefCounts[seqRef];
}

int ExportSequenceItem::decrementSeqRefCount() const {
    QMutexLocker locker(&seqRefGuard);
    SAFE_POINT(sequencesRefCounts.contains(seqRef) && sequencesRefCounts[seqRef] > 0, "Sequence is unexpectedly not tracked", -1);
    return --sequencesRefCounts[seqRef];
}

void ExportSequenceItem::stopSeqOwnership() {
    QMutexLocker locker(&seqRefGuard);
    SAFE_POINT(sequencesRefCounts.contains(seqRef), "Sequence is unexpectedly not tracked", );
    sequencesRefCounts.remove(seqRef);
}

void ExportSequenceItem::setOwnershipOverSeq(const U2Sequence &seq, const U2DbiRef &dbiRef) {
    SAFE_POINT(seq.hasValidId() && dbiRef.isValid(), "Invalid DBI reference", );

    releaseOwnedSeq();

    seqRef = U2EntityRef(dbiRef, seq.id);
    name = seq.visualName;
    circular = seq.circular;
    alphabet = U2AlphabetUtils::getById(seq.alphabet);
    length = seq.length;

    startSeqOwnership();
}

U2SequenceObject * ExportSequenceItem::takeOwnedSeq() {
    stopSeqOwnership();
    return new U2SequenceObject(name, seqRef);
}

bool ExportSequenceItem::ownsSeq() const {
    QMutexLocker locker(&seqRefGuard);
    return sequencesRefCounts.contains(seqRef);
}

bool ExportSequenceItem::isEmpty() const {
    static const ExportSequenceItem empty;
    return *this == empty;
}

void ExportSequenceItem::setSequenceInfo(U2SequenceObject *seqObj) {
    SAFE_POINT(NULL != seqObj, L10N::nullPointerError("sequence object"), );

    seqRef = seqObj->getEntityRef();
    name = seqObj->getGObjectName();
    circular = seqObj->isCircular();
    alphabet = seqObj->getAlphabet();
    length = seqObj->getSequenceLength();
}

ExportSequenceTaskSettings::ExportSequenceTaskSettings()
                          : merge(false),
                            mergeGap(0),
                            strand(TriState_Yes),
                            allAminoFrames(false),
                            mostProbable(true),
                            saveAnnotations(false),
                            formatId(BaseDocumentFormats::FASTA),
                            sequenceLength(0) {}

//////////////////////////////////////////////////////////////////////////
//ExportSequenceTask

ExportSequenceTask::ExportSequenceTask(const ExportSequenceTaskSettings &s)
    : DocumentProviderTask("", TaskFlag_None), config(s)
{
    setTaskName(tr("Export sequence to '%1'").arg(QFileInfo(s.fileName).fileName()));
    setVerboseLogMode(true);
}

namespace {

bool checkFrame(const QVector<U2Region> &regions, int frame) {
    foreach (const U2Region &r, regions) {
        if (r.startPos % 3 != frame) {
            return false;
        }
    }
    return true;
}

ExportSequenceItem toRevComplement(ExportSequenceItem &ei, const U2DbiRef &resultDbiRef, U2OpStatus &os) {
    ExportSequenceItem complEi = ei;
    CHECK_EXT(NULL != ei.complTT, os.setError(ExportSequenceTask::tr("Complement translation not found")), complEi);

    U2SequenceImporter importer(QVariantMap(), true);
    importer.startSequence(os, resultDbiRef, U2ObjectDbi::ROOT_FOLDER, ei.name +"|rev-compl", ei.circular);
    CHECK_OP(os, complEi);

    // translate
    U2SequenceObject seqObject(ei.name, ei.seqRef);
    for (qint64 pos = 0; pos < ei.length; pos += MAX_CHUNK_LENGTH) {
        const qint64 currentChunkSize = qMin(MAX_CHUNK_LENGTH, ei.length - pos);
        const U2Region chunkRegion(ei.length - pos - currentChunkSize, currentChunkSize);
        QByteArray chunkContent = seqObject.getSequenceData(chunkRegion, os);
        CHECK_OP(os, complEi);

        char *rawChunkContent = chunkContent.data();
        ei.complTT->translate(rawChunkContent, currentChunkSize);
        TextUtils::reverse(rawChunkContent, currentChunkSize);

        importer.addBlock(rawChunkContent, currentChunkSize, os);
        CHECK_OP(os, complEi);
    }

    const U2Sequence rComplSeq = importer.finalizeSequence(os);
    CHECK_OP(os, complEi);

    complEi.setOwnershipOverSeq(rComplSeq, resultDbiRef);

    // fix annotation locations
    for (int a = 0; a < complEi.annotations.size(); a++) {
        SharedAnnotationData &ad = complEi.annotations[a];
        ad->setStrand(ad->getStrand() == U2Strand::Direct ? U2Strand::Complementary : U2Strand::Direct);
        U2Region::mirror(ei.length, ad->location->regions);
        U2Region::reverse(ad->location->regions);
    }

    return complEi;
}

QList<ExportSequenceItem> toAmino(ExportSequenceItem &ei, bool allFrames, const U2DbiRef &resultDbiRef, U2OpStatus &os) {
    QList<ExportSequenceItem> res;

    CHECK_EXT(NULL != ei.aminoTT, os.setError(ExportSequenceTask::tr("Amino translation not found")), res);
    SAFE_POINT(ei.aminoTT->isThree2One(), "Invalid amino translation", res);

    const int nFrames = allFrames ? 3 : 1;

    QVector<QByteArray> chunkRemainders(nFrames);
    QVector<U2SequenceImporter> frameImporters;

    for (int frameNumber = 0; frameNumber < nFrames; ++frameNumber) {
        const QString transName = ei.name + "|transl" + (nFrames == 1 ? "" : " " + QString::number(frameNumber));

        frameImporters.append(U2SequenceImporter(QVariantMap(), true));
        frameImporters[frameNumber].startSequence(os, resultDbiRef, U2ObjectDbi::ROOT_FOLDER, transName, false);
        CHECK_OP(os, res);
    }

    U2SequenceObject seqObject(ei.name, ei.seqRef);
    for (qint64 pos = 0; pos < ei.length; pos += MAX_CHUNK_LENGTH) {
        const qint64 currentChunkSize = qMin(MAX_CHUNK_LENGTH, ei.length - pos);
        const U2Region chunkRegion(pos, currentChunkSize);
        const QByteArray chunkContent = seqObject.getSequenceData(chunkRegion, os);
        CHECK_OP(os, res);

        for (int frameNumber = 0; frameNumber < nFrames; ++frameNumber) {
            QByteArray frameChunkContent = chunkRemainders[frameNumber] + chunkContent;
            if (0 == pos) {
                frameChunkContent = frameChunkContent.mid(frameNumber);
            }
            chunkRemainders[frameNumber] = frameChunkContent.right(frameChunkContent.size() % 3);

            const DNAAlphabet *dstAlphabet = ei.aminoTT->getDstAlphabet();
            QByteArray transContent(frameChunkContent.size() / 3, dstAlphabet->getDefaultSymbol());

            ei.aminoTT->translate(frameChunkContent.constData(), frameChunkContent.length(), transContent.data(), transContent.length());

            frameImporters[frameNumber].addBlock(transContent.constData(), transContent.length(), os);
            CHECK_OP(os, res);
        }
    }

    for (int frameNumber = 0; frameNumber < nFrames; ++frameNumber) {
        const U2Sequence frameSeq = frameImporters[frameNumber].finalizeSequence(os);
        CHECK_OP(os, res);

        if (0 == frameSeq.length) { // no translation was produced
            coreLog.info(ExportSequenceTask::tr("The \"%1\" translation is empty due to small source sequence length").arg(frameSeq.visualName));
            continue;
        }

        ExportSequenceItem transEi = ei;
        transEi.setOwnershipOverSeq(frameSeq, resultDbiRef);

        // fix annotation locations
        transEi.annotations.clear();
        foreach (const SharedAnnotationData &ad, ei.annotations) {
            if (checkFrame(ad->getRegions(), frameNumber)) {
                SharedAnnotationData r = ad;
                U2Region::divide(3, r->location->regions);
                transEi.annotations.append(r);
            }
        }
        res.append(transEi);
    }

    return res;
}

ExportSequenceItem backToNucleic(ExportSequenceItem &ei, bool mostProbable, const U2DbiRef &resultDbiRef, U2OpStatus &os) {
    ExportSequenceItem backEi = ei;
    CHECK_EXT(NULL != ei.backTT, os.setError(ExportSequenceTask::tr("Back-translation not found")), backEi);
    SAFE_POINT(ei.backTT->isOne2Three(), "Invalid reverse translation", backEi);

    U2SequenceImporter importer(QVariantMap(), true);
    importer.startSequence(os, resultDbiRef, U2ObjectDbi::ROOT_FOLDER, ei.name + "|revtransl", false);
    CHECK_OP(os, backEi);

    // translate
    const DNATranslation1to3Impl *trans = dynamic_cast<const DNATranslation1to3Impl *>(ei.backTT);
    SAFE_POINT(NULL != trans, L10N::nullPointerError("DNA translation"), backEi);
    const BackTranslationMode translationMode = mostProbable ? USE_MOST_PROBABLE_CODONS : USE_FREQUENCE_DISTRIBUTION;

    U2SequenceObject seqObject(ei.name, ei.seqRef);
    for (qint64 pos = 0; pos < ei.length; pos += MAX_CHUNK_LENGTH) {
        const qint64 currentChunkSize = qMin(MAX_CHUNK_LENGTH, ei.length - pos);
        const U2Region chunkRegion(pos, currentChunkSize);
        const QByteArray chunkContent = seqObject.getSequenceData(chunkRegion, os);
        CHECK_OP(os, backEi);

        QByteArray translationChunk = QByteArray(currentChunkSize * 3, trans->getDstAlphabet()->getDefaultSymbol());
        trans->translate(chunkContent.constData(), currentChunkSize, translationChunk.data(), translationChunk.length(), translationMode);

        importer.addBlock(translationChunk.constData(), translationChunk.length(), os);
        CHECK_OP(os, backEi);
    }

    const U2Sequence rTransSeq = importer.finalizeSequence(os);
    CHECK_OP(os, backEi);

    backEi.setOwnershipOverSeq(rTransSeq, resultDbiRef);

    // fix annotation locations
    for (int a = 0; a < backEi.annotations.size(); a++) {
        SharedAnnotationData &ad = backEi.annotations[a];
        U2Region::multiply(3, ad->location->regions);
    }
    return backEi;
}

ExportSequenceItem mergeExportItems(QList<ExportSequenceItem> &items, int mergeGap, U2OpStatus &os) {
    ExportSequenceItem mergedEi = items.takeFirst();
    QByteArray gapSequence(mergeGap, mergedEi.alphabet->getDefaultSymbol());

    U2SequenceObject mergedSeqObject(mergedEi.name, mergedEi.seqRef);
    for (int i = 0, n = items.size(); i < n; ++i) {
        ExportSequenceItem &ei2 = items[i];

        mergedSeqObject.replaceRegion(U2Region(mergedEi.length, 0), DNASequence(gapSequence, mergedEi.alphabet), os);
        CHECK_OP(os, mergedEi);
        mergedEi.length = mergedSeqObject.getSequenceLength();

        U2SequenceObject currentSeqObject(ei2.name, ei2.seqRef);
        for (qint64 pos = 0; pos < ei2.length; pos += MAX_CHUNK_LENGTH) {
            const qint64 currentChunkSize = qMin(MAX_CHUNK_LENGTH, ei2.length - pos);
            const U2Region chunkRegion(pos, currentChunkSize);
            const QByteArray chunkContent = currentSeqObject.getSequenceData(chunkRegion, os);
            CHECK_OP(os, mergedEi);

            mergedSeqObject.replaceRegion(U2Region(mergedEi.length, 0), DNASequence(chunkContent, mergedEi.alphabet), os);
            CHECK_OP(os, mergedEi);
        }

        // fix annotation locations
        foreach (const SharedAnnotationData &ad, ei2.annotations) {
            SharedAnnotationData ma = ad;
            U2Region::shift(mergedEi.length, ma->location->regions);
            mergedEi.annotations.append(ma);
        }
        mergedEi.length = mergedSeqObject.getSequenceLength();
    }
    return mergedEi;
}

void saveExportItems2Doc(const QList<ExportSequenceItem> &items, const QString &seqName, Document *doc, U2OpStatus &os) {
    QVariantMap hints;
    hints[DocumentFormat::DBI_FOLDER_HINT] = U2ObjectDbi::ROOT_FOLDER;
    QSet<QString> usedNames;
    int itemCount = 0;
    foreach (ExportSequenceItem ri, items) {
        U2SequenceObject *seqObj = ri.ownsSeq() ? ri.takeOwnedSeq()
            : qobject_cast<U2SequenceObject *>(U2SequenceObject(ri.name, ri.seqRef).clone(doc->getDbiRef(), os, hints));
        CHECK_OP(os, );
        seqObj->moveToThread(doc->thread());

        QString name = seqName.isEmpty() ? ri.name : seqName;
        if (name.isEmpty()) {
            name = "sequence";
        }
        name = ExportUtils::genUniqueName(usedNames, name);
        usedNames.insert(name);
        seqObj->setGObjectName(name);

        doc->addObject(seqObj);
        Document::Constraints c;
        c.objectTypeToAdd.append(GObjectTypes::ANNOTATION_TABLE);
        bool annotationsSupported = doc->checkConstraints(c);
        if (annotationsSupported && !ri.annotations.isEmpty()) {
            const QString aName = ExportUtils::genUniqueName(usedNames, name + " annotations");
            AnnotationTableObject *annObj = new AnnotationTableObject(aName, doc->getDbiRef());
            usedNames.insert(aName);
            annObj->addAnnotations(ri.annotations);
            annObj->addObjectRelation(seqObj, ObjectRole_Sequence);
            annObj->setModified(false);
            doc->addObject(annObj);
        }
        os.setProgress(100 * ++itemCount / items.length());
    }
}

QList<ExportSequenceItem> getTranslatedItems(QList<ExportSequenceItem> &items, bool allAminoFrames, bool mostProbableTranslation,
    const U2DbiRef &resDbiRef, U2OpStatus &os)
{
    // translate to amino or back-translate if needed
    QList<ExportSequenceItem> result;
    for (int i = 0, n = items.size(); i < n; ++i) {
        ExportSequenceItem &ei = items[i];
        if (ei.aminoTT != NULL) {
            result.append(toAmino(ei, allAminoFrames, resDbiRef, os));
        } else if (ei.backTT != NULL) {
            result.append(backToNucleic(ei, mostProbableTranslation, resDbiRef, os));
        } else {
            result.append(ei);
        }
        CHECK(!os.hasError(), result);
    }
    return result;
}

}

void ExportSequenceTask::run() {
    DocumentFormatRegistry *r = AppContext::getDocumentFormatRegistry();
    DocumentFormat *f = r->getFormatById(config.formatId);
    SAFE_POINT(NULL != f, L10N::nullPointerError("sequence document format"), );
    IOAdapterFactory *iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(config.fileName));
    SAFE_POINT(NULL != iof, L10N::nullPointerError("I/O adapter factory"), );
    resultDocument = f->createNewLoadedDocument(iof, config.fileName, stateInfo);
    CHECK_OP(stateInfo, );

    const U2DbiRef dbiRef = resultDocument->getDbiRef();

    DbiOperationsBlock dbiBlock(dbiRef, stateInfo);
    Q_UNUSED(dbiBlock);

    QList<ExportSequenceItem> notMergedItems;
    for (int i = 0, n = config.items.size(); i < n; ++i) {
        ExportSequenceItem &ei0 = config.items[i];

        QList<ExportSequenceItem> r1Items;
        if (config.strand == TriState_Yes || config.strand == TriState_Unknown) {
            r1Items.append(ei0);
        }
        if (config.strand == TriState_No || config.strand == TriState_Unknown) {
            r1Items.append(toRevComplement(ei0, dbiRef, stateInfo));
            CHECK_OP(stateInfo, );
        }
        CHECK_OP(stateInfo, );

        const QList<ExportSequenceItem> r2Items = getTranslatedItems(r1Items, config.allAminoFrames, config.mostProbable, dbiRef, stateInfo);
        CHECK_OP(stateInfo, );
        notMergedItems.append(r2Items);

        //TODO: if we do not need to merge items, here we can use streaming & save the doc!
    }
    CHECK_EXT(!notMergedItems.isEmpty(), stateInfo.setError(tr("No sequences have been produced.")), );

    QList<ExportSequenceItem> resultItems;
    if (config.merge && notMergedItems.size() > 1) {
        const ExportSequenceItem mergedEi = mergeExportItems(notMergedItems, config.mergeGap, stateInfo);
        CHECK_OP(stateInfo, );
        resultItems.append(mergedEi);
    } else {
        resultItems = notMergedItems;
    }

    saveExportItems2Doc(resultItems, config.sequenceName, resultDocument, stateInfo);
    CHECK_OP(stateInfo, );
    //store the document
    f->storeDocument(resultDocument, stateInfo);
}

ExportSequenceItem ExportSequenceTask::mergedCircularItem(const ExportSequenceItem &first, const ExportSequenceItem &second, U2OpStatus &os) {
    return mergeExportItems(QList<ExportSequenceItem>() << first << second, 0, os);
}

//////////////////////////////////////////////////////////////////////////
// Export sequence under annotations

ExportSequenceAItem::ExportSequenceAItem()
    : aminoTT(NULL), complTT(NULL)
{

}

ExportAnnotationSequenceTask::ExportAnnotationSequenceTask(const ExportAnnotationSequenceTaskSettings &s)
    : DocumentProviderTask(tr("Export annotations"), TaskFlags_NR_FOSE_COSC), config(s), exportSubTask(NULL)
{
    extractSubTask = new ExportAnnotationSequenceSubTask(config);
    addSubTask(extractSubTask);
}

QList<Task*> ExportAnnotationSequenceTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> res;
    CHECK(!isCanceled(), res);

    if (subTask == extractSubTask && !extractSubTask->hasError()) {
        exportSubTask = new ExportSequenceTask(config.exportSequenceSettings);
        res.append(exportSubTask);
    }
    if (subTask == exportSubTask && !exportSubTask->hasError()) {
        resultDocument = exportSubTask->takeDocument();
    }
    return res;
}

ExportAnnotationSequenceSubTask::ExportAnnotationSequenceSubTask(ExportAnnotationSequenceTaskSettings &s)
    : Task(tr("Extract annotation regions"), TaskFlag_None), config(s)
{

}

namespace {

int totalAnnotationCount(const ExportAnnotationSequenceTaskSettings &config) {
    int result = 0;
    foreach (const ExportSequenceAItem &item, config.items) {
        result += item.annotations.length();
    }
    return result;
}

}

U2Sequence ExportAnnotationSequenceSubTask::importAnnotatedSeq2Dbi(const SharedAnnotationData &ad, const ExportSequenceAItem &ei, const U2DbiRef &resultDbiRef,
    QVector<U2Region> &resultRegions, U2OpStatus &os)
{
    U2SequenceImporter importer(QVariantMap(), true);
    importer.startSequence(os, resultDbiRef, U2ObjectDbi::ROOT_FOLDER, ad->name, false);
    CHECK_OP(os, U2Sequence());

    foreach(const U2Region &annotatedRegion, ad->location->regions) {
        qint64 currentRegionLength = 0;
        for (qint64 pos = annotatedRegion.startPos; pos < annotatedRegion.endPos(); pos += MAX_CHUNK_LENGTH) {
            const qint64 currentChunkSize = qMin(MAX_CHUNK_LENGTH, annotatedRegion.endPos() - pos);
            const U2Region chunkRegion(pos, currentChunkSize);
            QByteArray chunkContent = ei.sequence->getSequenceData(chunkRegion, os);
            CHECK_OP(os, U2Sequence());
            if (chunkContent.isEmpty()) {
                continue;
            }

            if (ad->getStrand().isCompementary() && NULL != ei.complTT) {
                TextUtils::reverse(chunkContent.data(), currentChunkSize);
                ei.complTT->translate(chunkContent.data(), currentChunkSize);
            }

            if (NULL != ei.aminoTT) {
                int translationSize = currentChunkSize / 3;
                QByteArray translation(translationSize, ei.aminoTT->getDstAlphabet()->getDefaultSymbol());
                ei.aminoTT->translate(chunkContent.constData(), currentChunkSize, translation.data(), translationSize);
                chunkContent = translation;
            }

            importer.addBlock(chunkContent.constData(), chunkContent.length(), os);
            CHECK_OP(os, U2Sequence());
            currentRegionLength += chunkContent.length();
        }
        CHECK_EXT(currentRegionLength == annotatedRegion.length,
                  os.setError(tr("Sequences of the selected annotations can't be exported. At least one of the annotations is out of boundaries")),
                  U2Sequence());
        resultRegions.append(U2Region(resultRegions.isEmpty() ? 0 : resultRegions.last().endPos(), currentRegionLength));
    }
    return importer.finalizeSequence(os);
}

void ExportAnnotationSequenceSubTask::run() {
    const U2DbiRef dbiRef = AppContext::getDbiRegistry()->getSessionTmpDbiRef(stateInfo);

    DbiOperationsBlock dbiBlock(dbiRef, stateInfo);
    Q_UNUSED(dbiBlock);

    const int allAnnotationSize = totalAnnotationCount(config);
    int annotationCount = 0;
    // extract sequences for every annotation & form ExportSequenceTaskSettings
    foreach (const ExportSequenceAItem &ei, config.items) {
        if (ei.sequence.isNull()) {
            coreLog.info(tr("Exported sequence has been deleted unexpectedly"));
            continue;
        }
        foreach (const SharedAnnotationData &ad, ei.annotations) {
            QVector<U2Region> resultRegions;
            const U2Sequence annSeqDbiObject = importAnnotatedSeq2Dbi(ad, ei, dbiRef, resultRegions, stateInfo);
            CHECK_OP(stateInfo, );

            SharedAnnotationData newAnn = ad;
            newAnn->location->strand = U2Strand::Direct;
            newAnn->location->regions = resultRegions;

            ExportSequenceItem esi;
            esi.setOwnershipOverSeq(annSeqDbiObject, dbiRef);
            esi.annotations.append(newAnn);
            config.exportSequenceSettings.items.append(esi);

            stateInfo.setProgress(100 * ++annotationCount / allAnnotationSize);
        }
    }
}

}//namespace

