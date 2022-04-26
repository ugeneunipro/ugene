/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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

#include "ModifySequenceObjectTask.h"

#include <U2Core/AddDocumentTask.h>
#include <U2Core/AnnotationTableObject.h>
#include <U2Core/AppContext.h>
#include <U2Core/Counter.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DNATranslation.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/GObject.h>
#include <U2Core/GObjectRelationRoles.h>
#include <U2Core/GObjectUtils.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/Log.h>
#include <U2Core/MultiTask.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/SaveDocumentTask.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2SequenceUtils.h>

namespace U2 {

typedef QPair<QString, QString> QStrStrPair;

ModifySequenceContentTask::ModifySequenceContentTask(const DocumentFormatId& dfId, U2SequenceObject* seqObj, const U2Region& regionTodelete, const DNASequence& seq2Insert, bool recalculateQualifiers, U1AnnotationUtils::AnnotationStrategyForResize str, const GUrl& url, bool mergeAnnotations)
    : Task(tr("Modify sequence task"), TaskFlags(TaskFlag_NoRun) | TaskFlag_ReportingIsSupported), resultFormatId(dfId),
      mergeAnnotations(mergeAnnotations), recalculateQualifiers(recalculateQualifiers), curDoc(seqObj->getDocument()), newDoc(nullptr), url(url), strat(str),
      seqObj(seqObj), regionToReplace(regionTodelete), sequence2Insert(seq2Insert) {
    GCOUNTER(cvar, "Modify sequence task");
    inplaceMod = url == curDoc->getURL() || url.isEmpty();
}

Task::ReportResult ModifySequenceContentTask::report() {
    CHECK(!(regionToReplace.isEmpty() && sequence2Insert.seq.isEmpty()), ReportResult_Finished);
    CHECK_EXT(!curDoc->isStateLocked(), setError(tr("Document is locked")), ReportResult_Finished);

    U2Region seqRegion(0, seqObj->getSequenceLength());
    if (!seqRegion.contains(regionToReplace)) {
        algoLog.error(tr("Region to delete is larger than the whole sequence"));
        return ReportResult_Finished;
    }

    Project* project = AppContext::getProject();
    if (project != nullptr) {
        CHECK(!project->isStateLocked(), ReportResult_CallMeAgain);
        docs = project->getDocuments();
    }

    if (!docs.contains(curDoc)) {
        docs.append(curDoc);
    }

    if (!inplaceMod) {
        cloneSequenceAndAnnotations();
    }
    seqObj->replaceRegion(regionToReplace, sequence2Insert, stateInfo);
    CHECK_OP(stateInfo, ReportResult_Finished);

    annotationForReport = FixAnnotationsUtils::fixAnnotations(&stateInfo, seqObj, regionToReplace, sequence2Insert, docs, recalculateQualifiers, strat);
    if (!annotationForReport.isEmpty()) {
        setReportingEnabled(true);
    }

    if (!inplaceMod) {
        QList<Task*> tasks;
        IOAdapterFactory* iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(url));
        tasks.append(new SaveDocumentTask(seqObj->getDocument(), iof, url.getURLString()));
        if (project != nullptr) {
            tasks.append(new AddDocumentTask(newDoc));
        }
        AppContext::getTaskScheduler()->registerTopLevelTask(new MultiTask("Save document and add it to project (optional)", tasks));
    }
    return ReportResult_Finished;
}

namespace {

QString formatPairList(const QList<QStrStrPair>& pairList, bool useFirst) {
    QString result;
    const QString lineSeparator = "<br>";
    foreach (const QStrStrPair& pair, pairList) {
        result += useFirst ? pair.first : pair.second;
        result += lineSeparator;
    }
    result.chop(lineSeparator.length());
    return result;
}

}  // namespace

QString ModifySequenceContentTask::generateReport() const {
    CHECK(!annotationForReport.isEmpty(), QString());

    QString report = tr("Some annotations have qualifiers referring a sequence region that has been removed during the sequence editing. "
                        "You might want to change the qualifiers manually. Find them in the table below");
    report += "<br><table border=\"1\" cellpadding=\"1\">";
    report += "<tr><th>";
    report += tr("Annotation Name");
    report += "</th><th>";
    report += tr("Annotation Location");
    report += "</th><th>";
    report += tr("Qualifier Name");
    report += "</th><th>";
    report += tr("Referenced Region");
    report += "</th></tr>";

    foreach (Annotation* an, annotationForReport.keys()) {
        if (annotationForReport[an].isEmpty()) {
            coreLog.error(tr("Unexpected qualifiers count"));
            assert(false);
            continue;
        }

        report += QString("<tr><td>%1</td><td>%2</td>").arg(an->getName()).arg(U1AnnotationUtils::buildLocationString(*an->getLocation()));

        report += QString("<td>%1</td>").arg(formatPairList(annotationForReport[an], true));
        report += QString("<td>%1</td>").arg(formatPairList(annotationForReport[an], false));

        report += "</tr>";
    }
    report += "</table>";
    return report;
}

qint64 ModifySequenceContentTask::getSequenceLengthDelta() const {
    return sequence2Insert.length() + regionToReplace.length;
}

void ModifySequenceContentTask::cloneSequenceAndAnnotations() {
    IOAdapterRegistry* ioReg = AppContext::getIOAdapterRegistry();
    IOAdapterFactory* iof = ioReg->getIOAdapterFactoryById(IOAdapterUtils::url2io(url));
    CHECK(iof != nullptr, );
    DocumentFormatRegistry* dfReg = AppContext::getDocumentFormatRegistry();
    DocumentFormat* df = dfReg->getFormatById(resultFormatId);
    SAFE_POINT(df != nullptr, "Invalid document format!", );

    U2SequenceObject* oldSeqObj = seqObj;
    newDoc = df->createNewLoadedDocument(iof, url, stateInfo, curDoc->getGHintsMap());
    CHECK_OP(stateInfo, );

    SAFE_POINT_EXT(df->isObjectOpSupported(newDoc, DocumentFormat::DocObjectOp_Add, GObjectTypes::SEQUENCE),
                   stateInfo.setError(tr("Failed to add sequence object to document!")), );

    U2Sequence clonedSeq = U2SequenceUtils::copySequence(oldSeqObj->getSequenceRef(), newDoc->getDbiRef(), U2ObjectDbi::ROOT_FOLDER, stateInfo);
    CHECK_OP(stateInfo, );

    seqObj = new U2SequenceObject(oldSeqObj->getGObjectName(), U2EntityRef(newDoc->getDbiRef(), clonedSeq.id), oldSeqObj->getGHintsMap());
    newDoc->addObject(seqObj);

    if (df->isObjectOpSupported(newDoc, DocumentFormat::DocObjectOp_Add, GObjectTypes::ANNOTATION_TABLE)) {
        if (mergeAnnotations) {
            AnnotationTableObject* newDocAto = new AnnotationTableObject("Annotations", newDoc->getDbiRef());
            newDocAto->addObjectRelation(seqObj, ObjectRole_Sequence);

            for (Document* d : qAsConst(docs)) {
                QList<GObject*> annotationTablesList = d->findGObjectByType(GObjectTypes::ANNOTATION_TABLE);
                for (GObject* table : qAsConst(annotationTablesList)) {
                    auto ato = qobject_cast<AnnotationTableObject*>(table);
                    if (ato != nullptr && ato->hasObjectRelation(oldSeqObj, ObjectRole_Sequence)) {
                        foreach (Annotation* ann, ato->getAnnotations()) {
                            newDocAto->addAnnotations(QList<SharedAnnotationData>() << ann->getData(), ann->getGroup()->getName());
                        }
                    }
                }
            }
            newDoc->addObject(newDocAto);
        } else {
            // use only sequence-doc annotations
            foreach (GObject* o, curDoc->getObjects()) {
                if (auto aObj = qobject_cast<AnnotationTableObject*>(o)) {
                    GObject* cl = aObj->clone(newDoc->getDbiRef(), stateInfo);
                    CHECK_OP(stateInfo, );
                    newDoc->addObject(cl);
                    GObjectUtils::updateRelationsURL(cl, curDoc->getURL(), newDoc->getURL());
                }
            }
        }
    }
    docs.append(newDoc);
}

}  // namespace U2
