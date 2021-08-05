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

#include "ExportTasks.h"

#include <QFileInfo>

#include <U2Core/AddDocumentTask.h>
#include <U2Core/AppContext.h>
#include <U2Core/Counter.h>
#include <U2Core/DNAChromatogramObject.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DNASequenceUtils.h>
#include <U2Core/DNATranslation.h>
#include <U2Core/DNATranslationImpl.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/GObjectRelationRoles.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/LoadDocumentTask.h>
#include <U2Core/MSAUtils.h>
#include <U2Core/MultipleSequenceAlignmentImporter.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2SequenceUtils.h>

#include <U2Formats/SCFFormat.h>

namespace U2 {

//////////////////////////////////////////////////////////////////////////
// DNAExportAlignmentTask
ExportAlignmentTask::ExportAlignmentTask(const MultipleSequenceAlignment &_ma, const QString &_url, const DocumentFormatId &_documentFormatId)
    : DocumentProviderTask(tr("Export alignment to %1").arg(_url), TaskFlag_None), ma(_ma->getCopy()), url(_url), documentFormatId(_documentFormatId) {
    GCOUNTER(cvar, "ExportAlignmentTask");
    documentDescription = QFileInfo(url).fileName();
    setVerboseLogMode(true);
    CHECK_EXT(!ma->isEmpty(), setError(tr("Nothing to export: multiple alignment is empty")), );
}

void ExportAlignmentTask::run() {
    DocumentFormat *format = AppContext::getDocumentFormatRegistry()->getFormatById(documentFormatId);
    SAFE_POINT(format != nullptr, L10N::nullPointerError("sequence document format"), );
    IOAdapterFactory *iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(url));
    SAFE_POINT(iof != nullptr, L10N::nullPointerError("I/O adapter factory"), );
    QScopedPointer<Document> exportedDocument(format->createNewLoadedDocument(iof, url, stateInfo));
    CHECK_OP(stateInfo, );

    MultipleSequenceAlignmentObject *obj = MultipleSequenceAlignmentImporter::createAlignment(exportedDocument->getDbiRef(), ma, stateInfo);
    CHECK_OP(stateInfo, );

    exportedDocument->addObject(obj);
    format->storeDocument(exportedDocument.get(), stateInfo);
    CHECK_OP(stateInfo, );
    exportedDocument.reset();    // Release resources.

    // Now reload the document.
    // Reason: document format may have some limits and change the original data: trim sequence names or replace spaces with underscores.
    resultDocument = format->loadDocument(iof, url, {}, stateInfo);
}

//////////////////////////////////////////////////////////////////////////
// export alignment  2 sequence format

ExportMSA2SequencesTask::ExportMSA2SequencesTask(const MultipleSequenceAlignment &_ma,
                                                 const QString &_url,
                                                 bool _trimLeadingAndTrailingGaps,
                                                 const DocumentFormatId &_documentFormatId)
    : DocumentProviderTask(tr("Export alignment as sequence to %1").arg(_url), TaskFlag_None), ma(_ma->getCopy()), url(_url),
      trimLeadingAndTrailingGaps(_trimLeadingAndTrailingGaps), documentFormatId(_documentFormatId) {
    documentDescription= QFileInfo(url).fileName();
    GCOUNTER(cvar, "ExportMSA2SequencesTask");
    setVerboseLogMode(true);
}

void ExportMSA2SequencesTask::run() {
    DocumentFormat *format = AppContext::getDocumentFormatRegistry()->getFormatById(documentFormatId);
    SAFE_POINT(format != nullptr, L10N::nullPointerError("sequence document format"), );
    IOAdapterFactory *iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(url));
    SAFE_POINT(iof != nullptr, L10N::nullPointerError("I/O adapter factory"), );
    QScopedPointer<Document> exportedDocument(format->createNewLoadedDocument(iof, url, stateInfo));
    CHECK_OP(stateInfo, );
    QList<DNASequence> lst = MSAUtils::ma2seq(ma, trimLeadingAndTrailingGaps);
    QSet<QString> usedNames;
    foreach (DNASequence s, lst) {
        QString name = s.getName();
        if (usedNames.contains(name)) {
            name = TextUtils::variate(name, " ", usedNames, false, 1);
            s.setName(name);
        }
        U2EntityRef seqRef = U2SequenceUtils::import(stateInfo, exportedDocument->getDbiRef(), s);
        CHECK_OP(stateInfo, );
        exportedDocument->addObject(new U2SequenceObject(name, seqRef));
        usedNames.insert(name);
    }
    format->storeDocument(exportedDocument.get(), stateInfo);
    CHECK_OP(stateInfo, );
    exportedDocument.reset();    // Release resources.

    // Now reload the document.
    // Reason: document format may have some limits and change the original data: trim sequence names or replace spaces with underscores.
    resultDocument = format->loadDocument(iof, url, {}, stateInfo);
}

//////////////////////////////////////////////////////////////////////////
// export nucleic alignment 2 amino alignment

ExportMSA2MSATask::ExportMSA2MSATask(const MultipleSequenceAlignment &_ma,
                                     int _offset,
                                     int _len,
                                     const QString &_url,
                                     const QList<DNATranslation *> &_aminoTranslations,
                                     const DocumentFormatId &_documentFormatId,
                                     bool _trimGaps,
                                     bool _convertUnknownToGap,
                                     bool _reverseComplement,
                                     int _translationFrame)
    : DocumentProviderTask(tr("Export alignment as alignment to %1").arg(_url), TaskFlag_None), ma(_ma->getCopy()), offset(_offset), len(_len), url(_url),
      documentFormatId(_documentFormatId), aminoTranslations(_aminoTranslations), trimLeadingAndTrailingGaps(_trimGaps),
      convertUnknownToGap(_convertUnknownToGap), reverseComplement(_reverseComplement), translationFrame(_translationFrame) {
    GCOUNTER(cvar, "ExportMSA2MSATask");
    documentDescription = QFileInfo(url).fileName();

    CHECK_EXT(!ma->isEmpty(), setError(tr("Nothing to export: multiple alignment is empty")), );
    SAFE_POINT_EXT(translationFrame >= 0 && translationFrame <= 2, setError(tr("Illegal translation frame offset: %1").arg(translationFrame)), );
    setVerboseLogMode(true);
}

void ExportMSA2MSATask::run() {
    DocumentFormat *format = AppContext::getDocumentFormatRegistry()->getFormatById(documentFormatId);
    SAFE_POINT(format != nullptr, L10N::nullPointerError("sequence document format"), );
    IOAdapterFactory *iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(url));
    SAFE_POINT(iof != nullptr, L10N::nullPointerError("I/O adapter factory"), );
    QScopedPointer<Document> exportedDocument(format->createNewLoadedDocument(iof, url, stateInfo));
    CHECK_OP(stateInfo, );

    QList<DNASequence> lst = MSAUtils::ma2seq(ma, trimLeadingAndTrailingGaps);
    QList<DNASequence> seqList;
    for (int i = offset; i < offset + len; i++) {
        DNASequence s = reverseComplement ? DNASequenceUtils::reverseComplement(lst[i]) : lst[i];
        s.seq = s.seq.right(s.seq.length() - translationFrame);
        QString name = s.getName();
        if (!aminoTranslations.isEmpty()) {
            DNATranslation *aminoTT = aminoTranslations.first();
            name += "(translated)";

            QByteArray seq = s.seq;
            int aminoSequenceLength = seq.length() / 3;
            QByteArray resseq(aminoSequenceLength, '\0');
            if (resseq.isNull() && aminoSequenceLength != 0) {
                setError(tr("Out of memory"));
                return;
            }
            assert(aminoTT->isThree2One());
            aminoTT->translate(seq.constData(), seq.length(), resseq.data(), resseq.length());

            if (!trimLeadingAndTrailingGaps && convertUnknownToGap) {
                resseq.replace("X", "-");
            }
            resseq.replace("*", "X");
            DNASequence rs(name, resseq, aminoTT->getDstAlphabet());
            seqList << rs;
        } else {
            seqList << s;
        }
    }
    MultipleSequenceAlignment aminoMa = MSAUtils::seq2ma(seqList, stateInfo);
    CHECK_OP(stateInfo, );

    MultipleSequenceAlignmentObject *obj = MultipleSequenceAlignmentImporter::createAlignment(exportedDocument->getDbiRef(), aminoMa, stateInfo);
    CHECK_OP(stateInfo, );

    exportedDocument->addObject(obj);
    format->storeDocument(exportedDocument.get(), stateInfo);
    CHECK_OP(stateInfo, );

    // Now reload the document.
    // Reason: document format may have some limits and change the original data: trim sequence names or replace spaces with underscores.
    resultDocument = format->loadDocument(iof, url, {}, stateInfo);
}

//////////////////////////////////////////////////////////////////////////
// export chromatogram to SCF

ExportDNAChromatogramTask::ExportDNAChromatogramTask(DNAChromatogramObject *_obj, const ExportChromatogramTaskSettings &_settings)
    : DocumentProviderTask(tr("Export chromatogram to SCF"), TaskFlags_NR_FOSCOE), chromaObject(_obj), settings(_settings), loadTask(nullptr) {
    GCOUNTER(cvar, "ExportDNAChromatogramTask");
    setVerboseLogMode(true);
}

void ExportDNAChromatogramTask::prepare() {
    Document *d = chromaObject->getDocument();
    SAFE_POINT_EXT(d != nullptr, setError(L10N::internalError("Chromatogram object has no associated document")), );

    QList<GObjectRelation> relatedObjs = chromaObject->findRelatedObjectsByRole(ObjectRole_Sequence);
    SAFE_POINT_EXT(relatedObjs.count() == 1, setError("Sequence related to chromatogram is not found!"), );

    QString seqObjName = relatedObjs.first().ref.objName;

    GObject *resObj = d->findGObjectByName(seqObjName);
    auto sObj = qobject_cast<U2SequenceObject *>(resObj);
    SAFE_POINT_EXT(sObj != nullptr, setError(L10N::nullPointerError("sequence object is null")), );

    DNAChromatogram cd = chromaObject->getChromatogram();
    QByteArray seq = sObj->getWholeSequenceData(stateInfo);
    CHECK_OP(stateInfo, );

    if (settings.reverse) {
        TextUtils::reverse(seq.data(), seq.length());
        reverseVector(cd.A);
        reverseVector(cd.C);
        reverseVector(cd.G);
        reverseVector(cd.T);
        int offset = 0;
        if (chromaObject->getDocument()->getDocumentFormatId() == BaseDocumentFormats::ABIF) {
            int baseNum = cd.baseCalls.count();
            int seqLen = cd.seqLength;
            // this is required for base <-> peak correspondence
            if (baseNum > seqLen) {
                cd.baseCalls.remove(baseNum - 1);
                cd.prob_A.remove(baseNum - 1);
                cd.prob_C.remove(baseNum - 1);
                cd.prob_G.remove(baseNum - 1);
                cd.prob_T.remove(baseNum - 1);
            }
        } else if (chromaObject->getDocument()->getDocumentFormatId() == BaseDocumentFormats::SCF) {
            // SCF format particularities
            offset = -1;
        }

        for (int i = 0; i < cd.seqLength; ++i) {
            cd.baseCalls[i] = cd.traceLength - cd.baseCalls[i] + offset;
        }
        reverseVector(cd.baseCalls);
        reverseVector(cd.prob_A);
        reverseVector(cd.prob_C);
        reverseVector(cd.prob_G);
        reverseVector(cd.prob_T);
    }

    if (settings.complement) {
        DNATranslation *tr = AppContext::getDNATranslationRegistry()->lookupTranslation(BaseDNATranslationIds::NUCL_DNA_DEFAULT_COMPLEMENT);
        tr->translate(seq.data(), seq.length());
        qSwap(cd.A, cd.T);
        qSwap(cd.C, cd.G);
        qSwap(cd.prob_A, cd.prob_T);
        qSwap(cd.prob_C, cd.prob_G);
    }

    SCFFormat::exportDocumentToSCF(settings.url, cd, seq, stateInfo);
    CHECK_OP(stateInfo, );

    if (settings.loadDocument) {
        IOAdapterFactory *iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(BaseIOAdapters::LOCAL_FILE);
        loadTask = new LoadDocumentTask(BaseDocumentFormats::SCF, settings.url, iof);
        addSubTask(loadTask);
    }
}
QList<Task *> ExportDNAChromatogramTask::onSubTaskFinished(Task *subTask) {
    if (subTask == loadTask) {
        resultDocument = loadTask->takeDocument();
    }
    return {};
}

}    // namespace U2
