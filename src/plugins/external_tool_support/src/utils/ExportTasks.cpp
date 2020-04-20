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

#include <U2Core/AddDocumentTask.h>
#include <U2Core/AppContext.h>
#include <U2Core/CloneObjectTask.h>
#include <U2Core/Counter.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNAChromatogramObject.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DNATranslation.h>
#include <U2Core/DNATranslationImpl.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/GHints.h>
#include <U2Core/GObjectRelationRoles.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/MSAUtils.h>
#include <U2Core/MultipleSequenceAlignmentImporter.h>
#include <U2Core/SaveDocumentTask.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U2DbiRegistry.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2SequenceUtils.h>

#include <U2Formats/SCFFormat.h>

#include "ExportTasks.h"

namespace U2 {

//////////////////////////////////////////////////////////////////////////
// DNAExportAlignmentTask
SaveAlignmentTask::SaveAlignmentTask(const MultipleSequenceAlignment& _ma, const QString& _fileName, DocumentFormatId _f, const QVariantMap& _hints)
: Task("", TaskFlag_None),
  ma(_ma->getCopy()),
  fileName(_fileName),
  hints(_hints),
  format(_f)
{
    GCOUNTER( cvar, tvar, "ExportAlignmentTask" );
    setTaskName(tr("Export alignment to '%1'").arg(QFileInfo(fileName).fileName()));
    setVerboseLogMode(true);

    if (ma->isEmpty()) {
        setError(tr("An alignment is empty"));
    }
}

void SaveAlignmentTask::run() {
    DocumentFormatRegistry* r = AppContext::getDocumentFormatRegistry();
    DocumentFormat* f = r->getFormatById(format);
    IOAdapterFactory* iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(fileName));
    doc.reset(f->createNewLoadedDocument(iof, fileName, stateInfo));

    MultipleSequenceAlignmentObject* obj = MultipleSequenceAlignmentImporter::createAlignment(doc->getDbiRef(), ma, stateInfo);
    CHECK_OP(stateInfo, );

    GHints* docHints = doc->getGHints();
    foreach(const QString& key, hints.keys()){
        docHints->set(key, hints[key]);
    }

    doc->addObject(obj);
    f->storeDocument(doc.data(), stateInfo);
}

Document * SaveAlignmentTask::getDocument() const {
    return doc.data();
}
const QString & SaveAlignmentTask::getUrl() const {
    return fileName;
}

const MultipleSequenceAlignment & SaveAlignmentTask::getMAlignment() const {
    return ma;
}

//////////////////////////////////////////////////////////////////////////
// export alignment  2 sequence format

SaveMSA2SequencesTask::SaveMSA2SequencesTask(const MultipleSequenceAlignment& _ma, const QString& _url, bool _trimAli, DocumentFormatId _format)
: Task(tr("Export alignment to sequence: %1").arg(_url), TaskFlag_None),
ma(_ma->getCopy()), url(_url), trimAli(_trimAli), format(_format)
{
    GCOUNTER( cvar, tvar, "ExportMSA2SequencesTask" );
    setVerboseLogMode(true);
    stateInfo.setProgress(0);
}

void SaveMSA2SequencesTask::run() {
    DocumentFormatRegistry* r = AppContext::getDocumentFormatRegistry();
    DocumentFormat* f = r->getFormatById(format);
    IOAdapterFactory* iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(url));
    doc.reset(f->createNewLoadedDocument(iof, url, stateInfo));
    CHECK_OP(stateInfo, );

    QList<DNASequence> lst = MSAUtils::ma2seq(ma, trimAli);
    QSet<QString> usedNames;
    foreach(const DNASequence& s, lst) {
        QString name = s.getName();
        if (usedNames.contains(name)) {
            name = TextUtils::variate(name, " ", usedNames, false, 1);
        }
        U2EntityRef seqRef = U2SequenceUtils::import(stateInfo, doc->getDbiRef(), s);
        CHECK_OP(stateInfo, );
        doc->addObject(new U2SequenceObject(name, seqRef));
        usedNames.insert(name);
    }
    f->storeDocument(doc.data(), stateInfo);
}

SaveSequenceTask::SaveSequenceTask(const QPointer<U2SequenceObject> &sequence, const QString &url, const DocumentFormatId &formatId):
    Task(tr("Save sequence"), TaskFlags_NR_FOSE_COSC),
    sequence(sequence),
    url(url),
    formatId(formatId),
    locker(NULL),
    cloneTask(NULL)
{
    SAFE_POINT_EXT(NULL != sequence, setError("Sequence is NULL"), );
    SAFE_POINT_EXT(!url.isEmpty(), setError("URL is empty"), );
}

void SaveSequenceTask::prepare() {
    locker = new StateLocker(sequence);
    cloneTask = new CloneObjectTask(sequence, AppContext::getDbiRegistry()->getSessionTmpDbiRef(stateInfo), U2ObjectDbi::ROOT_FOLDER);
    CHECK_OP(stateInfo, );
    cloneTask->setSubtaskProgressWeight(50);
    addSubTask(cloneTask);
}

QList<Task *> SaveSequenceTask::onSubTaskFinished(Task *subTask) {
    QList<Task *> result;

    if (subTask == cloneTask) {
        delete locker;
        locker = NULL;
    }

    CHECK_OP(stateInfo, result);

    if (subTask == cloneTask) {
        DocumentFormat *format = AppContext::getDocumentFormatRegistry()->getFormatById(formatId);
        SAFE_POINT_EXT(NULL != format, setError(tr("'%' format is not registered").arg(formatId)), result);

        Document *document = format->createNewLoadedDocument(IOAdapterUtils::get(BaseIOAdapters::LOCAL_FILE), url, stateInfo);
        CHECK_OP(stateInfo, result);
        document->setDocumentOwnsDbiResources(true);
        document->addObject(cloneTask->takeResult());

        SaveDocumentTask *saveTask = new SaveDocumentTask(document, NULL, GUrl(), SaveDocFlags(SaveDoc_Overwrite) | SaveDoc_DestroyAfter);
        saveTask->setSubtaskProgressWeight(50);
        result << saveTask;
    }

    return result;
}

}   // namespace U2
