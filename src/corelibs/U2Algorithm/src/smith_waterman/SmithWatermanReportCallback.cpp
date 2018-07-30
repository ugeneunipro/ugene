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

#include <U2Algorithm/SWMulAlignResultNamesTagsRegistry.h>

#include <U2Core/AppContext.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/MultipleSequenceAlignment.h>
#include <U2Core/MultipleSequenceAlignmentExporter.h>
#include <U2Core/MultipleSequenceAlignmentImporter.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/MSAUtils.h>
#include <U2Core/MsaDbiUtils.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/SaveDocumentTask.h>
#include <U2Core/U1AnnotationUtils.h>
#include <U2Core/U2AlphabetUtils.h>
#include <U2Core/DNATranslation.h>
#include <U2Core/U2Dbi.h>
#include <U2Core/U2DbiRegistry.h>
#include <U2Core/U2Msa.h>
#include <U2Core/U2MsaDbi.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2SequenceDbi.h>

#include "SmithWatermanReportCallback.h"

namespace U2 {

SmithWatermanReportCallback::~SmithWatermanReportCallback() {

}

// SmithWatermanReportCallbackAnnotImpl realization //////////////////////////////////////////////////////////////////////////

SmithWatermanReportCallbackAnnotImpl::SmithWatermanReportCallbackAnnotImpl(AnnotationTableObject *_aobj, U2FeatureType annotationType,
    const QString &_annotationName, const QString &_annotationGroup, const QString &annDescription, bool _addPatternSubseqToQual, QObject *pOwn)
    : QObject(pOwn), annotationType(annotationType), annotationName(_annotationName), annotationGroup(_annotationGroup),
    annDescription(annDescription), aObj(_aobj), autoReport(_aobj != NULL), addPatternSubseqToQual(_addPatternSubseqToQual)
{

}

QString SmithWatermanReportCallbackAnnotImpl::report(const QList<SmithWatermanResult> &results) {
    if (autoReport && aObj.isNull()) {
        return tr("Annotation object not found.");
    }

    if (autoReport && aObj->isStateLocked()) {
        return tr("Annotation table is read-only");
    }

    foreach (const SmithWatermanResult &res, results) {
        SharedAnnotationData annotation = res.toAnnotation(annotationName);
        annotation->type = annotationType;
        if (addPatternSubseqToQual && 0 != res.ptrnSubseq.length) {
            annotation->qualifiers.append(U2Qualifier("pattern_match_start", QString::number(res.ptrnSubseq.startPos)));
            annotation->qualifiers.append(U2Qualifier("pattern_match_len", QString::number(res.ptrnSubseq.length)));
        }
        U1AnnotationUtils::addDescriptionQualifier(annotation, annDescription);
        anns.append(annotation);
    }

    if (autoReport) {
        aObj->addAnnotations(anns, annotationGroup);
    }

    return QString();
}

const QList<SharedAnnotationData> & SmithWatermanReportCallbackAnnotImpl::getAnotations() const {
    return anns;
}

// SmithWatermanReportCallbackMAImpl realization //////////////////////////////////////////////////////////////////////////

SmithWatermanReportCallbackMAImpl::TagExpansionPossibleData::TagExpansionPossibleData()
    : curProcessingSubseq(NULL)
{

}

SmithWatermanReportCallbackMAImpl::TagExpansionPossibleData::TagExpansionPossibleData(
    const QString &_refSequenceName, const QString &_patternName)
    : refSequenceName(_refSequenceName), patternName(_patternName), curProcessingSubseq(NULL)
{

}

const quint8 SmithWatermanReportCallbackMAImpl::countOfSimultLoadedMADocs = 5;

SmithWatermanReportCallbackMAImpl::SmithWatermanReportCallbackMAImpl(
    const QString &_resultFolderName, const QString &_mobjectNamesTemplate,
    const QString &_refSubseqTemplate, const QString &_ptrnSubseqTemplate,
    const QByteArray &_refSequence, const QByteArray &_pattern, const QString &_refSeqName,
    const QString &_patternName, const DNAAlphabet *_alphabet, const DNATranslation* _aminoTT, WhatDoYouWantFromMe _plan)
    : plan(_plan), resultDirPath(_resultFolderName),
    mobjectNamesTemplate(_mobjectNamesTemplate), refSubseqTemplate(_refSubseqTemplate),
    ptrnSubseqTemplate(_ptrnSubseqTemplate), refSequenceData(_refSequence),
    ptrnSequenceData(_pattern), alphabet(_alphabet), aminoTT(_aminoTT), expansionInfo(_refSeqName, _patternName)
{

}

SmithWatermanReportCallbackMAImpl::SmithWatermanReportCallbackMAImpl(
    const QString &_resultDirPath, const QString &_mobjectName,
    const U2EntityRef &_firstSequenceRef, const U2EntityRef &_secondSequenceRef,
    const U2EntityRef &_sourceMsaRef, WhatDoYouWantFromMe _plan)
    : plan(_plan), resultDirPath(_resultDirPath), firstSequenceRef(_firstSequenceRef),
    secondSequenceRef(_secondSequenceRef), sourceMsaRef(_sourceMsaRef), mobjectName(_mobjectName)
{
    U2OpStatus2Log os;

    sourceMsaConnection.open(sourceMsaRef.dbiRef, os);
    CHECK_OP(os,);

    U2MsaDbi *msaDbi = sourceMsaConnection.dbi->getMsaDbi();
    SAFE_POINT(NULL != msaDbi, "Invalid MSA DBI detected!",);
    const U2AlphabetId alphabetId = msaDbi->getMsaObject(sourceMsaRef.entityId, os).alphabet;
    CHECK_OP(os,);
    alphabet = U2AlphabetUtils::getById(alphabetId);
    aminoTT = NULL;
    SAFE_POINT(NULL != alphabet, "Invalid alphabet detected!",);
}

SmithWatermanReportCallbackMAImpl::SmithWatermanReportCallbackMAImpl(
    const U2EntityRef &_firstSequenceRef, const U2EntityRef &_secondSequenceRef,
    const U2EntityRef &_sourceMsaRef, WhatDoYouWantFromMe _plan)
    : plan(_plan), firstSequenceRef(_firstSequenceRef),
    secondSequenceRef(_secondSequenceRef), sourceMsaRef(_sourceMsaRef)
{
    U2OpStatus2Log os;

    sourceMsaConnection.open(sourceMsaRef.dbiRef, os);
    if (os.isCoR()) {
        return;
    }

    U2MsaDbi *msaDbi = sourceMsaConnection.dbi->getMsaDbi();
    SAFE_POINT(NULL != msaDbi, "Invalid MSA DBI detected!",);
    const U2AlphabetId alphabetId = msaDbi->getMsaObject(sourceMsaRef.entityId, os).alphabet;
    if (os.isCoR()) {
        return;
    }
    alphabet = U2AlphabetUtils::getById(alphabetId);
    SAFE_POINT(NULL != alphabet, "Invalid alphabet detected!",);
    aminoTT = NULL;
}

QString SmithWatermanReportCallbackMAImpl::report(const QList<SmithWatermanResult> &_results) {
    switch (plan) {
    case SequenceView_Search:
        return planFor_SequenceView_Search(_results);
    case MSA_Alignment_InNewWindow:
        return planFor_MSA_Alignment_InNewWindow(_results);
    case MSA_Alignment_InCurrentWindow:
        return planFor_MSA_Alignment_InCurrentWindow(_results);
    default:
        FAIL("Unexpected algorithm mode!", QString::null);
    }
}

QString SmithWatermanReportCallbackMAImpl::planFor_SequenceView_Search(const QList<SmithWatermanResult> &results) {
    quint8 countOfLoadedDocs = 0;
    TaskStateInfo stateInfo;
    SWMulAlignResultNamesTagsRegistry *tagsRegistry = AppContext::getSWMulAlignResultNamesTagsRegistry();
    Project *currentProject = AppContext::getProject();
    TaskScheduler *taskScheduler = AppContext::getTaskScheduler();

    tagsRegistry->resetCounters();

    foreach (SmithWatermanResult pairAlignSeqs, results) {
        SAFE_POINT(!(0 == pairAlignSeqs.ptrnSubseq.startPos && 0 == pairAlignSeqs.ptrnSubseq.length) && !(0 == pairAlignSeqs.refSubseq.startPos
            && 0 == pairAlignSeqs.refSubseq.length), "Invalid SW algorithm result detected!", QString::null);

        DocumentFormat *format = AppContext::getDocumentFormatRegistry()
            ->getFormatById(BaseDocumentFormats::CLUSTAL_ALN);
        Document *alignmentDoc = NULL;

        const QString newFileName = tagsRegistry->parseStringWithTags(mobjectNamesTemplate, expansionInfo);
        QString newFileUrl = resultDirPath + newFileName + '.' + format->getSupportedDocumentFileExtensions().first();
        changeGivenUrlIfDocumentExists(newFileUrl, currentProject);

        alignmentDoc = format->createNewLoadedDocument(
            IOAdapterUtils::get(BaseIOAdapters::LOCAL_FILE), GUrl(newFileUrl), stateInfo);
        CHECK_OP(stateInfo, tr("SmithWatermanReportCallback failed to create new MA document"));

        QByteArray curResultRefSubseq = refSequenceData.mid(pairAlignSeqs.refSubseq.startPos, pairAlignSeqs.refSubseq.length);
        const DNAAlphabet* msaAlphabet = alphabet;
        if (aminoTT != NULL) {
            int buflen = curResultRefSubseq.length() / 3;
            QByteArray buf(buflen, '\0');
            aminoTT->translate(curResultRefSubseq.constData(), curResultRefSubseq.length(), buf.data(), buflen);
            curResultRefSubseq = buf;
            msaAlphabet = aminoTT->getDstAlphabet();
        }
        QByteArray curResultPtrnSubseq = ptrnSequenceData.mid(pairAlignSeqs.ptrnSubseq.startPos, pairAlignSeqs.ptrnSubseq.length);
        alignSequences(curResultRefSubseq, curResultPtrnSubseq, pairAlignSeqs.pairAlignment);

        MultipleSequenceAlignment msa(newFileName, msaAlphabet);

        expansionInfo.curProcessingSubseq = &pairAlignSeqs.refSubseq;
        msa->addRow(tagsRegistry->parseStringWithTags(refSubseqTemplate, expansionInfo), curResultRefSubseq);
        CHECK_OP(stateInfo, tr("Failed to add a reference subsequence row."));

        expansionInfo.curProcessingSubseq = &pairAlignSeqs.ptrnSubseq;
        msa->addRow(tagsRegistry->parseStringWithTags(ptrnSubseqTemplate, expansionInfo), curResultPtrnSubseq);
        CHECK_OP(stateInfo, tr("Failed to add a pattern subsequence row."));

        MultipleSequenceAlignmentObject *docObject = MultipleSequenceAlignmentImporter::createAlignment(alignmentDoc->getDbiRef(), msa, stateInfo);
        CHECK_OP(stateInfo, tr("Failed to create an alignment."));
        alignmentDoc->addObject(docObject);
        currentProject->addDocument(alignmentDoc);

        SaveDocFlags flags = SaveDoc_Overwrite;
        Task *saveMADocument = NULL;

        if (countOfLoadedDocs < SmithWatermanReportCallbackMAImpl::countOfSimultLoadedMADocs) {
            ++countOfLoadedDocs;
        } else {
            flags |= SaveDoc_UnloadAfter;
        }

        saveMADocument = new SaveDocumentTask(alignmentDoc, flags);

        taskScheduler->registerTopLevelTask(saveMADocument);
    }
    return QString();
}

QString SmithWatermanReportCallbackMAImpl::planFor_MSA_Alignment_InNewWindow(
    const QList<SmithWatermanResult> &_results)
{
    //There can be only one result: the first maximum. Other results will be ignored.
    quint8 countOfLoadedDocs = 0;
    TaskStateInfo stateInfo;
    SWMulAlignResultNamesTagsRegistry *tagsRegistry = AppContext::getSWMulAlignResultNamesTagsRegistry();
    Project *currentProject = AppContext::getProject();
    TaskScheduler *taskScheduler = AppContext::getTaskScheduler();
    CHECK(!_results.isEmpty(), tr("Smith-Waterman pairwise alignment: no results"));
    SmithWatermanResult pairAlignSeqs = _results.first();
    U2OpStatus2Log os;

    tagsRegistry->resetCounters();

    SAFE_POINT((firstSequenceRef.dbiRef == sourceMsaRef.dbiRef) && (secondSequenceRef.dbiRef ==sourceMsaRef.dbiRef),
        tr("Sequences aren't stored in the source msa dbi"), QString::null);

    SAFE_POINT(!(0 == pairAlignSeqs.ptrnSubseq.startPos && 0 == pairAlignSeqs.ptrnSubseq.length) &&
        !(0 == pairAlignSeqs.refSubseq.startPos && 0 == pairAlignSeqs.refSubseq.length),
        "Invalid SW algorithm results detected!", QString::null);

    DocumentFormat *format = AppContext::getDocumentFormatRegistry()
        ->getFormatById(BaseDocumentFormats::CLUSTAL_ALN);
    Document *alignmentDoc = NULL;

    QString newFileUrl = resultDirPath + mobjectName + '.' + format->getSupportedDocumentFileExtensions().first();
    changeGivenUrlIfDocumentExists(newFileUrl, currentProject);

    alignmentDoc = format->createNewLoadedDocument(IOAdapterUtils::get(BaseIOAdapters::LOCAL_FILE), GUrl(newFileUrl), stateInfo);
    CHECK_OP(stateInfo, tr("SmithWatermanReportCallback failed to create new MA document."));

    U2SequenceDbi *seqDbi = sourceMsaConnection.dbi->getSequenceDbi();
    SAFE_POINT(NULL != seqDbi, "Invalid sequence DBI detected!", QString::null);

    U2Sequence firstSequence = seqDbi->getSequenceObject(firstSequenceRef.entityId, os);
    CHECK_OP(os, tr("Failed to get the sequence object."));
    U2Sequence secondSequence = seqDbi->getSequenceObject(secondSequenceRef.entityId, os);
    CHECK_OP(os, tr("Failed to get the sequence object."));

    U2Sequence *refSequence;
    U2Sequence *ptrnSequence;

    if (firstSequence.length < secondSequence.length) {
        refSequence = &secondSequence;
        ptrnSequence = &firstSequence;
    } else {
        refSequence = &firstSequence;
        ptrnSequence = &secondSequence;
    }

    refSequenceData = seqDbi->getSequenceData(refSequence->id, U2Region(0, refSequence->length), os);
    CHECK_OP(os, tr("Failed to get the sequence data."));
    ptrnSequenceData = seqDbi->getSequenceData(ptrnSequence->id, U2Region(0, ptrnSequence->length), os);
    CHECK_OP(os, tr("Failed to get the sequence data."));

    SAFE_POINT(refSequenceData.length() > 0 && ptrnSequenceData.length() > 0, "Invalid sequence length detected!", QString::null);
    alignSequences(refSequenceData, ptrnSequenceData, pairAlignSeqs.pairAlignment);

    MultipleSequenceAlignment msa(refSequence->visualName + " vs. " + ptrnSequence->visualName, alphabet);
    msa->addRow(refSequence->visualName, refSequenceData);
    msa->addRow(ptrnSequence->visualName, ptrnSequenceData);

    MultipleSequenceAlignmentObject *docObject = MultipleSequenceAlignmentImporter::createAlignment(alignmentDoc->getDbiRef(), msa, stateInfo);
    CHECK_OP(stateInfo, tr("Failed to create an alignment."));
    alignmentDoc->addObject(docObject);

    SaveDocFlags flags = SaveDoc_Overwrite;
    flags |= SaveDoc_OpenAfter;
    Task *saveMADocument = NULL;

    if (countOfLoadedDocs < SmithWatermanReportCallbackMAImpl::countOfSimultLoadedMADocs) {
        ++countOfLoadedDocs;
    } else {
        flags |= SaveDoc_UnloadAfter;
    }

    saveMADocument = new SaveDocumentTask(alignmentDoc, flags);
    taskScheduler->registerTopLevelTask(saveMADocument);
    return QString();
}

QString SmithWatermanReportCallbackMAImpl::planFor_MSA_Alignment_InCurrentWindow(const QList<SmithWatermanResult> &_results) {
    //There can be only one result: the first maximum. Other results will be ignored.
    CHECK(!_results.isEmpty(), tr("Smith-Waterman pairwise alignment: no results"));
    SmithWatermanResult pairAlignSeqs = _results.first();
    U2OpStatus2Log os;

    SAFE_POINT((firstSequenceRef.dbiRef == sourceMsaRef.dbiRef)
        && (secondSequenceRef.dbiRef ==sourceMsaRef.dbiRef),
        tr("Sequences aren't stored in the source msa dbi"), 0);

    SAFE_POINT(!(0 == pairAlignSeqs.ptrnSubseq.startPos
        && 0 == pairAlignSeqs.ptrnSubseq.length) &&!(0 == pairAlignSeqs.refSubseq.startPos
        && pairAlignSeqs.refSubseq.length == 0), "Invalid SW algorithm result detected!", QString::null);

    U2SequenceDbi *seqDbi = sourceMsaConnection.dbi->getSequenceDbi();
    SAFE_POINT(NULL != seqDbi, "Invalid sequence DBI detected!", QString::null);

    U2Sequence firstSequence = seqDbi->getSequenceObject(firstSequenceRef.entityId, os);
    CHECK_OP(os, tr("Failed to get the sequence object."));
    U2Sequence secondSequence = seqDbi->getSequenceObject(secondSequenceRef.entityId, os);
    CHECK_OP(os, tr("Failed to get the sequence object."));

    U2Sequence *refSequence;
    U2Sequence *ptrnSequence;

    if (firstSequence.length < secondSequence.length) {
        refSequence = &secondSequence;
        ptrnSequence = &firstSequence;
    } else {
        refSequence = &firstSequence;
        ptrnSequence = &secondSequence;
    }

    refSequenceData = seqDbi->getSequenceData(refSequence->id, U2Region(0, refSequence->length), os);
    CHECK_OP(os, tr("Failed to get the sequence data."));
    ptrnSequenceData = seqDbi->getSequenceData(ptrnSequence->id,
        U2Region(0, ptrnSequence->length), os);
    CHECK_OP(os, tr("Failed to get the sequence data."));

    SAFE_POINT(refSequenceData.length() > 0 && ptrnSequenceData.length() > 0, "Invalid sequence length", QString::null);
    alignSequences(refSequenceData, ptrnSequenceData, pairAlignSeqs.pairAlignment);

    U2MsaDbi *msaDbi = sourceMsaConnection.dbi->getMsaDbi();
    SAFE_POINT(NULL != msaDbi, "Invalid MSA DBI detected!", QString::null);

    QList<U2MsaRow> rows = msaDbi->getRows(sourceMsaRef.entityId, os);
    CHECK_OP(os, tr("Failed to get msa from dbi"));

    U2MsaRow *refRow = NULL;
    U2MsaRow *ptrnRow = NULL;

    for (int  i = 0; i < rows.length(); ++i) {
        if (rows[i].sequenceId == refSequence->id) {
            refRow = &rows[i];
        }
        if (rows[i].sequenceId == ptrnSequence->id) {
            ptrnRow = &rows[i];
        }
    }

    QByteArray notUsedOutputParam;
    refRow->gaps.clear();
    ptrnRow->gaps.clear();
    MaDbiUtils::splitBytesToCharsAndGaps(refSequenceData, notUsedOutputParam, refRow->gaps);
    MaDbiUtils::splitBytesToCharsAndGaps(ptrnSequenceData, notUsedOutputParam, ptrnRow->gaps);

    U2UseCommonUserModStep userModStep(sourceMsaRef, os);
    Q_UNUSED(userModStep);
    SAFE_POINT_OP(os, QString());
    msaDbi->updateGapModel(sourceMsaRef.entityId, refRow->rowId, refRow->gaps, os);
    CHECK_OP(os, tr("Failed to update row gap model"));
    msaDbi->updateGapModel(sourceMsaRef.entityId, ptrnRow->rowId, ptrnRow->gaps, os);
    CHECK_OP(os, tr("Failed to update row gap model"));

    return QString();
}

void SmithWatermanReportCallbackMAImpl::alignSequences(QByteArray &refSequence, QByteArray &ptrnSequence, const QByteArray &pairwiseAlignment) {
    quint32 refSeqCurrentPosition = refSequence.length();
    quint32 ptrnSeqCurrentPosition = ptrnSequence.length();

    for (qint32 i = 0; i < pairwiseAlignment.length(); ++i) {
        switch (pairwiseAlignment[i]) {
            case SmithWatermanResult::DIAG:
                --refSeqCurrentPosition;
                --ptrnSeqCurrentPosition;
                continue;
                break;
            case SmithWatermanResult::UP:
                ptrnSequence.insert(ptrnSeqCurrentPosition, U2Msa::GAP_CHAR);
                --refSeqCurrentPosition;
                break;
            case SmithWatermanResult::LEFT:
                refSequence.insert(refSeqCurrentPosition, U2Msa::GAP_CHAR);
                --ptrnSeqCurrentPosition;
                break;
            default:
                FAIL("Unexpected pairwise alignment direction!", );
        }
    }
}

void SmithWatermanReportCallbackMAImpl::alignSequences(QList<U2MsaGap> &refSequenceGapModel, QList<U2MsaGap> &ptrnSequenceGapModel,
    const QByteArray& pairwiseAlignment)
{
    bool lastSymbolIsGapRef = false;
    bool lastSymbolIsGapPtrn = false;
    quint32 intervalStart = 0;
    quint32 intervalEnd = 0;
    for (quint32 i = 0; i < static_cast<quint32>(pairwiseAlignment.length()); ++i) {
        switch (pairwiseAlignment[i]) {
        case SmithWatermanResult::DIAG:
            if (lastSymbolIsGapRef) {
                intervalStart = i;
                refSequenceGapModel.prepend(U2MsaGap(intervalStart, intervalEnd));
                lastSymbolIsGapRef = false;
            }
            if (lastSymbolIsGapPtrn) {
                intervalStart = i;
                ptrnSequenceGapModel.prepend(U2MsaGap(intervalStart, intervalEnd));
                lastSymbolIsGapPtrn = false;
            }
            break;
        case SmithWatermanResult::UP:
            if (lastSymbolIsGapRef) {
                refSequenceGapModel.prepend(U2MsaGap(intervalStart, intervalEnd));
                lastSymbolIsGapRef = false;
            }
            if (!lastSymbolIsGapPtrn) {
                intervalEnd = i;
            }
            lastSymbolIsGapPtrn = true;
            break;
        case SmithWatermanResult::LEFT:
            if (lastSymbolIsGapPtrn) {
                ptrnSequenceGapModel.prepend(U2MsaGap(intervalStart, intervalEnd));
                lastSymbolIsGapPtrn = false;
            }
            if (!lastSymbolIsGapRef) {
                intervalEnd = i;
            }
            lastSymbolIsGapRef = true;
            break;
        default:
            FAIL ("Unexpected pairwise alignment direction!", );
        }
    }
}

void SmithWatermanReportCallbackMAImpl::changeGivenUrlIfDocumentExists(QString &givenUrl, const Project *curProject) {
    if (NULL != curProject->findDocumentByURL(GUrl(givenUrl))) {
        for (size_t i = 1; ; i++) {
            QString tmpUrl = givenUrl;
            QRegExp dotWithExtensionRegExp("\\.{1,1}[^\\.]*$|^[^\\.]*$");
            dotWithExtensionRegExp.lastIndexIn(tmpUrl);
            tmpUrl.replace(dotWithExtensionRegExp.capturedTexts().last(),
                "(" + QString::number(i) + ")"
                + dotWithExtensionRegExp.capturedTexts().last());
            if (NULL == curProject->findDocumentByURL(GUrl(tmpUrl))) {
                givenUrl = tmpUrl;
                break;
            }
        }
    }
}

} // namespace U2
