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

#include <U2Core/DocumentModel.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/AppContext.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DNATranslation.h>
#include <U2Core/DNATranslationImpl.h>
#include <U2Core/Counter.h>
#include <U2Core/DNAChromatogramObject.h>
#include <U2Core/GObjectRelationRoles.h>
#include <U2Core/LoadDocumentTask.h>
#include <U2Core/AddDocumentTask.h>
#include <U2Core/MSAUtils.h>
#include <U2Core/TextUtils.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/MultipleSequenceAlignmentImporter.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2SequenceUtils.h>

#include <U2Formats/SCFFormat.h>

#include <U2Gui/OpenViewTask.h>

#include "ExportTasks.h"

namespace U2 {


//////////////////////////////////////////////////////////////////////////
// AddDocumentAndOpenViewTask

AddExportedDocumentAndOpenViewTask::AddExportedDocumentAndOpenViewTask(DocumentProviderTask* t)
: Task("Export sequence to document", TaskFlags_NR_FOSCOE),
  loadTask(NULL)
{
    exportTask = t;
    addSubTask(exportTask);
}


QList<Task*> AddExportedDocumentAndOpenViewTask::onSubTaskFinished( Task* subTask ) {
    QList<Task*> subTasks;
    if (subTask == exportTask && !subTask->hasError() && !subTask->isCanceled()) {
        Document* doc = exportTask->getDocument();
        const GUrl& fullPath = doc->getURL();
        Project* prj = AppContext::getProject();
        if (prj) {
            Document* sameURLdoc = prj->findDocumentByURL(fullPath);
            if (sameURLdoc) {
                taskLog.trace(tr("Document is already added to the project %1").arg(doc->getURL().getURLString()));
                subTasks << new LoadUnloadedDocumentAndOpenViewTask(sameURLdoc);
                return subTasks;
            }
        }
        loadTask = LoadDocumentTask::getDefaultLoadDocTask(doc->getURL());
        CHECK_EXT(NULL != loadTask, setError(tr("Can't create load task")), subTasks);
        subTasks << loadTask;
    }
    if (subTask == loadTask) {
        subTasks << new AddDocumentAndOpenViewTask(loadTask->takeDocument());
    }
    //TODO: provide a report if subtask fails
    return subTasks;
}

//////////////////////////////////////////////////////////////////////////
// DNAExportAlignmentTask
ExportAlignmentTask::ExportAlignmentTask(const MultipleSequenceAlignment& _ma, const QString& _fileName, DocumentFormatId _f)
: DocumentProviderTask("", TaskFlag_None), ma(_ma->getCopy()), fileName(_fileName), format(_f)
{
    GCOUNTER( cvar, tvar, "ExportAlignmentTask" );
    setTaskName(tr("Export alignment to '%1'").arg(QFileInfo(fileName).fileName()));
    setVerboseLogMode(true);

    assert(!ma->isEmpty());
}

void ExportAlignmentTask::run() {
    DocumentFormatRegistry* r = AppContext::getDocumentFormatRegistry();
    DocumentFormat* f = r->getFormatById(format);
    IOAdapterFactory* iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(fileName));
    resultDocument = f->createNewLoadedDocument(iof, fileName, stateInfo);
    CHECK_OP(stateInfo, );

    MultipleSequenceAlignmentObject* obj = MultipleSequenceAlignmentImporter::createAlignment(resultDocument->getDbiRef(), ma, stateInfo);
    CHECK_OP(stateInfo, );

    resultDocument->addObject(obj);
    f->storeDocument(resultDocument, stateInfo);
}


//////////////////////////////////////////////////////////////////////////
// export alignment  2 sequence format

ExportMSA2SequencesTask::ExportMSA2SequencesTask(const MultipleSequenceAlignment& _ma, const QString& _url, bool _trimAli, DocumentFormatId _format)
: DocumentProviderTask(tr("Export alignment to sequence: %1").arg(_url), TaskFlag_None),
ma(_ma->getCopy()), url(_url), trimAli(_trimAli), format(_format)
{
    GCOUNTER( cvar, tvar, "ExportMSA2SequencesTask");
    setVerboseLogMode(true);
}

void ExportMSA2SequencesTask::run() {
    DocumentFormatRegistry* r = AppContext::getDocumentFormatRegistry();
    DocumentFormat* f = r->getFormatById(format);
    IOAdapterFactory* iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(url));
    resultDocument = f->createNewLoadedDocument(iof, url, stateInfo);
    CHECK_OP(stateInfo, );
    QList<DNASequence> lst = MSAUtils::ma2seq(ma, trimAli);
    QSet<QString> usedNames;
    foreach(DNASequence s, lst) {
        QString name = s.getName();
        if (usedNames.contains(name)) {
            name = TextUtils::variate(name, " ", usedNames, false, 1);
            s.setName(name);
        }
        U2EntityRef seqRef = U2SequenceUtils::import(stateInfo, resultDocument->getDbiRef(), s);
        CHECK_OP(stateInfo, );
        resultDocument->addObject(new U2SequenceObject(name, seqRef));
        usedNames.insert(name);
    }
    f->storeDocument(resultDocument, stateInfo);
}

//////////////////////////////////////////////////////////////////////////
// export nucleic alignment 2 amino alignment

ExportMSA2MSATask::ExportMSA2MSATask(const MultipleSequenceAlignment& _ma, int _offset, int _len, const QString& _url,
    const QList<DNATranslation*>& _aminoTranslations, DocumentFormatId _format)
: DocumentProviderTask(tr("Export alignment to alignment: %1").arg(_url), TaskFlag_None),
ma(_ma->getCopy()), offset(_offset), len(_len), url(_url), format(_format), aminoTranslations(_aminoTranslations)
{
    GCOUNTER( cvar, tvar, "ExportMSA2MSATask" );
    CHECK_EXT( !ma->isEmpty(), setError(tr("Nothing to export: multiple alignment is empty")), );
    setVerboseLogMode(true);
}

void ExportMSA2MSATask::run() {
    DocumentFormatRegistry* r = AppContext::getDocumentFormatRegistry();
    DocumentFormat* f = r->getFormatById(format);
    IOAdapterFactory* iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(url));
    resultDocument = f->createNewLoadedDocument(iof, url, stateInfo);
    CHECK_OP(stateInfo, );

    QList<DNASequence> lst = MSAUtils::ma2seq(ma, true);
    QList<DNASequence> seqList;
    for (int i = offset; i < offset + len; i++) {
        DNASequence& s = lst[i];
        QString name = s.getName();
        if (!aminoTranslations.isEmpty()) {
            DNATranslation* aminoTT = aminoTranslations.first();
            name += "(translated)";

            QByteArray seq = s.seq;
            int len = seq.length() / 3;
            QByteArray resseq(len, '\0');
            if (resseq.isNull() && len != 0) {
                stateInfo.setError( tr("Out of memory") );
                return;
            }
            assert(aminoTT->isThree2One());
            aminoTT->translate(seq.constData(), seq.length(), resseq.data(), resseq.length());

            resseq.replace("*","X");
            DNASequence rs(name, resseq, aminoTT->getDstAlphabet());
            seqList << rs;
        } else {
            seqList << s;
        }
    }
    MultipleSequenceAlignment ma = MSAUtils::seq2ma(seqList, stateInfo);
    CHECK_OP(stateInfo, );

    MultipleSequenceAlignmentObject* obj = MultipleSequenceAlignmentImporter::createAlignment(resultDocument->getDbiRef(), ma, stateInfo);
    CHECK_OP(stateInfo, );

    resultDocument->addObject(obj);
    f->storeDocument(resultDocument, stateInfo);
}

//////////////////////////////////////////////////////////////////////////
// export chromatogram to SCF

ExportDNAChromatogramTask::ExportDNAChromatogramTask( DNAChromatogramObject* _obj, const ExportChromatogramTaskSettings& _settings)
 : DocumentProviderTask(tr("Export chromatogram to SCF"), TaskFlags_NR_FOSCOE),
   cObj(_obj), settings(_settings), loadTask(NULL)
{
    GCOUNTER( cvar, tvar, "ExportDNAChromatogramTask" );
    setVerboseLogMode(true);
}

void ExportDNAChromatogramTask::prepare() {
    Document* d = cObj->getDocument();
    assert(d != NULL);
    if (d == NULL ) {
        stateInfo.setError("Chromatogram object document is not found!");
        return;
    }

    QList<GObjectRelation> relatedObjs = cObj->findRelatedObjectsByRole(ObjectRole_Sequence);
    assert(relatedObjs.count() == 1);
    if (relatedObjs.count() != 1) {
        stateInfo.setError("Sequence related to chromatogram is not found!");
    }
    QString seqObjName = relatedObjs.first().ref.objName;

    GObject* resObj = d->findGObjectByName(seqObjName);
    U2SequenceObject * sObj = qobject_cast<U2SequenceObject*>(resObj);
    assert(sObj != NULL);

    DNAChromatogram cd = cObj->getChromatogram();
    QByteArray seq = sObj->getWholeSequenceData(stateInfo);
    CHECK_OP(stateInfo, );

    if (settings.reverse) {
        TextUtils::reverse(seq.data(), seq.length());
        reverseVector(cd.A);
        reverseVector(cd.C);
        reverseVector(cd.G);
        reverseVector(cd.T);
        int offset = 0;
        if (cObj->getDocument()->getDocumentFormatId() == BaseDocumentFormats::ABIF) {
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
        } else if (cObj->getDocument()->getDocumentFormatId() == BaseDocumentFormats::SCF) {
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
        DNATranslation* tr = AppContext::getDNATranslationRegistry()->lookupTranslation(BaseDNATranslationIds::NUCL_DNA_DEFAULT_COMPLEMENT);
        tr->translate(seq.data(), seq.length());
        qSwap(cd.A,cd.T);
        qSwap(cd.C, cd.G);
        qSwap(cd.prob_A, cd.prob_T);
        qSwap(cd.prob_C, cd.prob_G);

    }

    SCFFormat::exportDocumentToSCF(settings.url, cd, seq, stateInfo);
    CHECK_OP(stateInfo, );

    if (settings.loadDocument) {
        IOAdapterFactory* iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(BaseIOAdapters::LOCAL_FILE);
        loadTask = new LoadDocumentTask(BaseDocumentFormats::SCF, settings.url, iof );
        addSubTask( loadTask );
    }
}
QList<Task*> ExportDNAChromatogramTask::onSubTaskFinished(Task* subTask) {
    if (subTask == loadTask) {
        resultDocument = loadTask->takeDocument();
    }
    return QList<Task*>();
}

}//namespace

